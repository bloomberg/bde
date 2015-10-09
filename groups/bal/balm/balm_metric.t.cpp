// balm_metric.t.cpp                                                  -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balm_metric.h>

#include <balm_metricregistry.h>
#include <balm_metricsample.h>
#include <balm_publisher.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bslma_testallocator.h>
#include <bslmt_barrier.h>
#include <bdlmt_fixedthreadpool.h>
#include <bdlf_bind.h>

#include <bsls_stopwatch.h>

#include <bsl_functional.h>
#include <bsl_ostream.h>
#include <bsl_cstring.h>
#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

#include <bslim_testutil.h>

using namespace BloombergLP;

using bsl::cout;
using bsl::endl;
using bsl::flush;

// ============================================================================
//                                  TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The 'balm::Metric' class and the macros defined in this component primarily
// provide user-friendly access to a 'balm::Collector' object.  The classes and
// macros supply simplified constructors (that lookup the appropriate
// 'balm::Collector' instance) and manipulator methods that respect whether the
// category of the collector is enabled.  Most of the tests can be performed by
// creating an "oracle" 'balm::Collector' object and verifying that operations
// performed on a 'balm::Metric' (or with a macro) have the same effect as the
// same operation performed directly on the "oracle" 'balm::Collector'.
// ----------------------------------------------------------------------------
// CLASS METHODS
// [ 9] static balm::Collector *lookupCollector(const bslstl::StringRef&  ,
//                                             const bslstl::StringRef&  ,
//                                             balm::MetricsManager    *);
// [ 9] static balm::Collector *lookupCollector(const balm::MetricId&  ,
//                                             balm::MetricsManager  *);
// CREATORS
// [ 2] balm::Metric(const bslstl::StringRef&  ,
//                   const bslstl::StringRef&  ,
//                   balm::MetricsManager    *);
// [ 2] balm::Metric(const balm::MetricId&, balm::MetricsManager *);
// [ 2] balm::Metric(balm::Collector *collector);
// [ 5] balm::Metric(const balm::Metric& original);
// [ 2] ~balm::Metric();
// MANIPULATORS
// [ 6] void increment();
// [ 7] void update(double value);
// [ 8] void accumulateCountTotalMinMax(int, double, double, double);
// [ 2] balm::Collector *collector();
// ACCESSORS
// [ 2] const balm::Collector *collector() const;
// [ 2] balm::MetricId metricId() const;
// [ 3] bool isActive() const;
// FREE OPERATORS
// [ 4]  bool operator==(const balm::Metric&,
//                       const balm::Metric&);
// [ 4]  bool operator!=(const balm::Metric&,
//                       const balm::Metric&);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [10] CONCURRENCY TEST: 'balm::Metric'
// [11] USAGE EXAMPLE

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

// ============================================================================
//                      STANDARD BDE TEST DRIVER MACROS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define Q   BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P   BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_  BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------
typedef balm::DefaultMetricsManager DefaultManager;
typedef balm::MetricRegistry        Registry;
typedef balm::CollectorRepository   Repository;
typedef balm::Collector             Collector;
typedef balm::MetricId              Id;
typedef balm::Category              Category;
typedef balm::Metric                Obj;
typedef balm::PublicationType       Type;

typedef bsl::shared_ptr<balm::Collector>         ColSPtr;
typedef bsl::shared_ptr<balm::IntegerCollector>  IColSPtr;
typedef bsl::vector<ColSPtr>                     ColSPtrVector;
typedef bsl::vector<IColSPtr>                    IColSPtrVector;

// ============================================================================
//                     CLASSES FOR AND FUNCTIONS TESTING
// ----------------------------------------------------------------------------

inline
balm::MetricRecord recordVal(const balm::Collector *collector)
    // Return the current record value of the specified 'collector'.
{
    balm::MetricRecord record;
    collector->load(&record);
    return record;
}

// --------------------- case 14: MetricConcurrencyTest -----------------------

class MetricConcurrencyTest {
    // Invoke a set of operations operations synchronously.

    // DATA
    bdlmt::FixedThreadPool   d_pool;
    balm::Metric           *d_metric;
    balm::MetricRegistry   *d_registry;
    bslmt::Barrier          d_barrier;
    bslma::Allocator      *d_allocator_p;

    // PRIVATE MANIPULATORS
    void execute();
        // Execute a single test.

  public:

    // CREATORS
    MetricConcurrencyTest(int                  numThreads,
                          balm::Metric         *metric,
                          balm::MetricRegistry *registry,
                          bslma::Allocator    *basicAllocator)
    : d_pool(numThreads, 1000, basicAllocator)
    , d_metric(metric)
    , d_registry(registry)
    , d_barrier(numThreads)
    , d_allocator_p(basicAllocator)
    {
        d_pool.start();
    }

    ~MetricConcurrencyTest() {}

    //  MANIPULATORS
    void runTest();
        // Run the test.
};

void MetricConcurrencyTest::execute()
{
    // The test is performed in two parts.  The first part of the test invokes
    // the manipulators for the 'balm::Metric' under test for a series of
    // values.  The expected value for the metric can be computed exactly, and
    // the actual value is compared against this expected value.  The second
    // part of the test performs a similar series of operations while enabling
    // and disabling the category of the metric.  The expected value for the
    // metric cannot be computed exactly, but the actual value can be tested
    // to verify that it falls within an expected range of values.

    Obj& mX = *d_metric; const Obj& MX = mX;
    Collector       *collector = mX.collector();
    const Category  *CATEGORY  = collector->metricId().category();
    collector->reset();

    const int NUM_THREADS = d_pool.numThreads();
    const int MAX_VALUE   = 20;
    const int RANGE       = 2 * MAX_VALUE - 1;

    d_barrier.wait();

    for (int i = -MAX_VALUE + 1; i < MAX_VALUE; ++i) {
        mX.increment();
        mX.update(i);
        mX.accumulateCountTotalMinMax(2, 2 * i, i * 2, i * 2);

        ASSERT(MX.isActive());
        ASSERT(collector             == mX.collector());
        ASSERT(collector             == MX.collector());
        ASSERT(collector->metricId() == MX.metricId());
    }

    d_barrier.wait();

    balm::MetricRecord recordValue; collector->load(&recordValue);
    ASSERT(4 * RANGE * NUM_THREADS == recordValue.count());
    ASSERT(RANGE * NUM_THREADS     == recordValue.total());
    ASSERT(-2 * (MAX_VALUE - 1)    == recordValue.min());
    ASSERT(2 * (MAX_VALUE - 1)     == recordValue.max());

    d_barrier.wait();

    collector->reset();

    d_barrier.wait();

    // Perform a test while modifying the whether the category is
    for (int i = -MAX_VALUE + 1; i < MAX_VALUE; ++i) {
        d_registry->setCategoryEnabled(CATEGORY, true);

        mX.increment();
        mX.update(i);
        mX.accumulateCountTotalMinMax(1, i, i, i);

        d_registry->setCategoryEnabled(CATEGORY, false);

        mX.increment();
        mX.update(i);
        mX.accumulateCountTotalMinMax(1, i, i, i);

        MX.isActive();
        ASSERT(collector             == mX.collector());
        ASSERT(collector             == MX.collector());
        ASSERT(collector->metricId() == MX.metricId());
    }

    d_barrier.wait();

    collector->load(&recordValue);
    ASSERT(3 <= recordValue.count() &&
           6 * RANGE * NUM_THREADS >= recordValue.count());
    ASSERT(NUM_THREADS * 6 * -MAX_VALUE <= recordValue.total() &&
           NUM_THREADS * 6 * MAX_VALUE  >= recordValue.total());
    ASSERT(-MAX_VALUE + 1 == recordValue.min());
    ASSERT(MAX_VALUE  - 1 >= recordValue.max() &&
           1              <= recordValue.max());
    ASSERT(!MX.isActive());
    d_barrier.wait();

}

void MetricConcurrencyTest::runTest()
{
    bsl::function<void()> job = bdlf::BindUtil::bind(
                                               &MetricConcurrencyTest::execute,
                                                this);

    for (int i = 0; i < d_pool.numThreads(); ++i) {
        d_pool.enqueueJob(job);
    }
    d_pool.drain();
}


// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Usage
///-----
// The following examples demonstrate how to configure, collect, and publish
// metrics.
//
///Example 1 - Metric collection with 'balm::Metric'
///- - - - - - - - - - - - - - - - - - - - - - - - -
// We can use 'balm::Metric' objects to record metric values.  In this
// example we implement a hypothetical event manager object.  We use
// 'balm::Metric' objects to record metrics for the size of the request, the
// elapsed processing time, and the number of failures.
//..
    class EventManager {

        // DATA
        balm::Metric d_messageSize;
        balm::Metric d_elapsedTime;
        balm::Metric d_failedRequests;

      public:

        // CREATORS
        EventManager()
        : d_messageSize("MyCategory", "EventManager/size")
        , d_elapsedTime("MyCategory", "EventManager/elapsedTime")
        , d_failedRequests("MyCategory", "EventManager/failedRequests")
        {}

        // MANIPULATORS
        int handleEvent(int eventId, const bsl::string& eventMessage)
            // Process the event described by the specified 'eventId' and
            // 'eventMessage' .  Return 0 on success, and a non-zero value
            // if there was an error handling the event.
        {
           int returnCode = 0;

           d_messageSize.update(eventMessage.size());
           bsls::Stopwatch stopwatch;
           stopwatch.start();

           // Process 'data' ('returnCode' may change).

           if (0 != returnCode) {
               d_failedRequests.increment();
           }

           d_elapsedTime.update(stopwatch.elapsedTime());
           return returnCode;
        }

    // ...
    };
//..

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;;

    bslma::TestAllocator testAlloc; bslma::TestAllocator *Z = &testAlloc;
    bslma::TestAllocator defaultAllocator;
    bslma::DefaultAllocatorGuard guard(&defaultAllocator);

    switch (test) { case 0:  // Zero is always the leading case.
      case 11: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage Example"
                          << "\n=====================" << endl;

///Example 2 - Create and access the default 'balm::MetricsManager' instance
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates how to create the default 'balm::MetricManager'
// instance and perform a trivial configuration.
//
// First we create a 'balm::DefaultMetricsManagerScopedGuard', which manages
// the lifetime of the default metrics manager instance.  At construction, we
// provide the scoped guard an output stream ('stdout') that it will publish
// metrics to.  Note that the default metrics manager is intended to be created
// and destroyed by the *owner* of 'main'.  An instance of the manager should
// be created during the initialization of an application (while the task has a
// single thread) and destroyed just prior to termination (when there is
// similarly a single thread).
//..
//  int main(int argc, char *argv[])
    {
        // ...

        balm::DefaultMetricsManagerScopedGuard managerGuard(bsl::cout);
//..
// Once the default instance has been created, it can be accessed using the
// 'instance' operation.
//..
        balm::MetricsManager *manager =
                                       balm::DefaultMetricsManager::instance();
        ASSERT(0 != manager);
//..
// Note that the default metrics manager will be released when 'managerGuard'
// exits this scoped and is destroyed.  Clients that choose to explicitly call
// 'balm::DefaultMetricsManager::create' must also explicitly call
// 'balm::DefaultMetricsManager::release()'.
//
// Now that we have created a 'balm::MetricsManager' instance, we can use the
// instance to publish metrics collected using the event manager described in
// Example 1:
//..
        EventManager eventManager;

        eventManager.handleEvent(0, "ab");
        eventManager.handleEvent(0, "abc");
        eventManager.handleEvent(0, "abc");
        eventManager.handleEvent(0, "abdef");

        manager->publishAll();

        eventManager.handleEvent(0, "ab");
        eventManager.handleEvent(0, "abc");
        eventManager.handleEvent(0, "abc");
        eventManager.handleEvent(0, "abdef");

        eventManager.handleEvent(0, "a");
        eventManager.handleEvent(0, "abc");
        eventManager.handleEvent(0, "abc");
        eventManager.handleEvent(0, "abdefg");

        manager->publishAll();
    }
//..
    } break;
      case 10: {
        // --------------------------------------------------------------------
        // CONCURRENCY TEST: 'balm::Metric'
        //
        // Testing:
        //     Thread-safety of 'balm::Metric' methods.
        //
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TEST CONCURRENCY: 'balm::Metric'" << endl
                                  << "===============================" << endl;

        bslma::TestAllocator defaultAllocator;

        balm::MetricsManager mgr(Z);
        Registry&   registry   = mgr.metricRegistry();
        Repository& repository = mgr.collectorRepository();
        balm::Metric metric(repository.getDefaultCollector("Test", "Test"));

        bslma::TestAllocator testAllocator;
        {
            MetricConcurrencyTest tester(10, &metric,
                                         &registry,  &testAllocator);
            tester.runTest();
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING: 'lookupCollector'
        //
        // Concerns:
        //   That 'lookupCollector' properly looks up a collector from the
        //   supplied metrics manager, or from the default metrics manager, if
        //   none is supplied.
        //
        // Plan:
        //
        //
        // Testing:
        //   static balm::Collector *lookupCollector(
        //                                const bslstl::StringRef&  ,
        //                                const bslstl::StringRef&  ,
        //                                balm::MetricsManager    *);
        //   static balm::Collector *lookupCollector(const balm::MetricId&  ,
        //                                          balm::MetricsManager  *);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING: 'lookupCollector'\n"
                          << "==========================\n";

        const char *IDS[] = {"A", "B", "AB", "TEST"};
        const int NUM_IDS = sizeof IDS / sizeof *IDS;

        if (veryVerbose)
            cout << "\tverify with no default metrics manager\n";
        {
            balm::MetricRegistry registry(Z);
            for (int i = 0; i < NUM_IDS; ++i) {
                balm::MetricId id = registry.getId(IDS[i], IDS[i]);
                ASSERT(0 == Obj::lookupCollector(IDS[i], IDS[i], 0));
                ASSERT(0 == Obj::lookupCollector(IDS[i], IDS[i]));
                ASSERT(0 == Obj::lookupCollector(id, 0));
                ASSERT(0 == Obj::lookupCollector(id));
            }
        }

        if (veryVerbose)
            cout << "\tverify with explicit metrics manager\n";
        {
            balm::MetricsManager mgr;
            Registry&   registry   = mgr.metricRegistry();
            Repository& repository = mgr.collectorRepository();

            for (int i = 0; i < NUM_IDS; ++i) {
                balm::MetricId id = registry.getId(IDS[i], IDS[i]);
                balm::Collector *col = repository.getDefaultCollector(id);
                ASSERT(col == Obj::lookupCollector(IDS[i], IDS[i], &mgr));
                ASSERT(col == Obj::lookupCollector(id, &mgr));
            }
        }

        if (veryVerbose)
            cout << "\tverify with default metrics manager\n";
        {
            balm::DefaultMetricsManagerScopedGuard guard(Z);
            balm::MetricsManager& mgr = *DefaultManager::instance();
            Registry&   registry   = mgr.metricRegistry();
            Repository& repository = mgr.collectorRepository();

            for (int i = 0; i < NUM_IDS; ++i) {
                balm::MetricId id = registry.getId(IDS[i], IDS[i]);
                balm::Collector *col = repository.getDefaultCollector(id);
                ASSERT(col == Obj::lookupCollector(IDS[i], IDS[i]));
                ASSERT(col == Obj::lookupCollector(IDS[i], IDS[i], 0));
                ASSERT(col == Obj::lookupCollector(id));
                ASSERT(col == Obj::lookupCollector(id, 0));
            }
        }

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING: 'accumulateCountTotalMinMax'
        //
        // Concerns:
        //   That 'accumulateCountTotalMinMax' properly invokes the metric's
        //   collector's 'accumulateCountTotalMinMax' method.
        //
        // Plan:
        //   Specify a set S of unique object values having various minor or
        //   subtle differences.  For each value create an "oracle" collector,
        //   invoke 'accumulateCountTotalMinMax' on the metric and the
        //   "oracle", and verify the collector underlying the metric has the
        //   same value as the "oracle" collector.
        //
        //   Perform the same test again, but enabled or disable the metric's
        //   category on each iteration.
        //
        // Testing:
        //   void accumulateCountTotalMinMax(int, double, double, double);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING: 'accumulateCountTotalMinMax'\n"
                          << "=====================================\n";

        const char *IDS[] = {"A", "B", "AB", "TEST"};
        const int NUM_IDS = sizeof IDS / sizeof *IDS;

        struct {
            int    d_count;
            double d_total;
            double d_min;
            double d_max;
        } VALUES [] = {
            {       0,         0.0,         0.0,          0.0 },
            {       1,         1.0,         1.0,          1.0 },
            {      1210,   INT_MAX,     INT_MIN,      INT_MIN },
            {       1,         2.0,         3.0,          4.0 },
            {      10,        -2.0,        -3.0,         -4.0 },
            {      123110, INT_MIN,     INT_MIN,      INT_MAX },
            {     -12311, 1125.413,    -123.123,     41.00001 },
        };
        const int NUM_VALUES = sizeof(VALUES)/sizeof(*VALUES);

        if (veryVerbose)
            cout << "\tverify 'accumulateCountTotalMinMax' is applied "
                 << "correctly.\n";
        {
            balm::MetricsManager mgr(Z);
            Registry&   registry   = mgr.metricRegistry();
            Repository& repository = mgr.collectorRepository();

            for (int i = 0; i < NUM_IDS; ++i) {
                balm::Collector *col =
                               repository.getDefaultCollector(IDS[i], IDS[i]);
                balm::Collector  expValue(col->metricId());

                balm::Metric  mX(col); const balm::Metric& MX = mX;
                for (int j = 0; j < NUM_VALUES; ++j) {
                    ASSERT(recordVal(&expValue) == recordVal(col));
                    mX.accumulateCountTotalMinMax(VALUES[j].d_count,
                                                  VALUES[j].d_total,
                                                  VALUES[j].d_min,
                                                  VALUES[j].d_max);
                    expValue.accumulateCountTotalMinMax(VALUES[j].d_count,
                                                        VALUES[j].d_total,
                                                        VALUES[j].d_min,
                                                        VALUES[j].d_max);

                    ASSERT(recordVal(&expValue) == recordVal(col));
                }
            }
        }
        if (veryVerbose)
            cout << "\tverify 'accumulateCountTotalMinMax' respects "
                 << "'isActive' flag.\n";

        {
            for (int i = 0; i < NUM_IDS; ++i) {
                balm::MetricsManager mgr(Z);
                Registry&   registry   = mgr.metricRegistry();
                Repository& repository = mgr.collectorRepository();

                balm::Collector *col =
                               repository.getDefaultCollector(IDS[i], IDS[i]);
                balm::Collector  expValue(col->metricId());
                const Category *CATEGORY = col->metricId().category();

                balm::Metric  mX(col); const balm::Metric& MX = mX;
                for (int j = 0; j < NUM_VALUES; ++j) {
                    ASSERT(recordVal(&expValue) == recordVal(col));

                    bool enabled = 0 == j % 2;
                    registry.setCategoryEnabled(CATEGORY, enabled);
                    mX.accumulateCountTotalMinMax(VALUES[j].d_count,
                                                  VALUES[j].d_total,
                                                  VALUES[j].d_min,
                                                  VALUES[j].d_max);
                    if (enabled) {
                        expValue.accumulateCountTotalMinMax(VALUES[j].d_count,
                                                            VALUES[j].d_total,
                                                            VALUES[j].d_min,
                                                            VALUES[j].d_max);
                    }
                    ASSERT(enabled == MX.isActive());
                    ASSERT(recordVal(&expValue) == recordVal(col));
                }
            }
        }

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING: 'update'
        //
        // Concerns:
        //   That 'update' properly invokes the metric's collector's
        //   'update' method.
        //
        // Plan:
        //   Specify a set S of unique object values having various minor or
        //   subtle differences.  For each value create an "oracle" collector,
        //   update the metric and the "oracle", and verify the collector's
        //   underlying the metric has the same value as the "oracle"
        //   collector.
        //
        //   Perform the same test again, but enabled or disable the metric's
        //   category on each iteration.
        //
        // Testing:
        //    void update(double );
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING: 'update'\n"
                          << "=================\n";

        const char *IDS[] = {"A", "B", "AB", "TEST"};
        const int NUM_IDS = sizeof IDS / sizeof *IDS;

        double UPDATES[] = { 0.0, 12.0, -1321123, 2131241, 1321.5,
                             43145.1, .0001, -1.00001, -.002342};
        const int NUM_UPDATES = sizeof(UPDATES)/sizeof(*UPDATES);

        if (veryVerbose)
            cout << "\tverify 'update'  applied correctly.\n";

        {
            balm::MetricsManager mgr(Z);
            Registry&   registry   = mgr.metricRegistry();
            Repository& repository = mgr.collectorRepository();

            for (int i = 0; i < NUM_IDS; ++i) {
                balm::Collector *col =
                               repository.getDefaultCollector(IDS[i], IDS[i]);
                balm::Collector  expValue(col->metricId());

                balm::Metric  mX(col); const balm::Metric& MX = mX;
                for (int j = 0; j < NUM_UPDATES; ++j) {
                    ASSERT(recordVal(&expValue) == recordVal(col));
                    mX.update(UPDATES[j]);
                    expValue.update(UPDATES[j]);
                    ASSERT(recordVal(&expValue) == recordVal(col));
                }
            }
        }

        if (veryVerbose)
            cout << "\tverify 'update' respects 'isActive' flag.\n";

        {
            for (int i = 0; i < NUM_IDS; ++i) {
                balm::MetricsManager mgr(Z);
                Registry&   registry   = mgr.metricRegistry();
                Repository& repository = mgr.collectorRepository();

                balm::Collector *col =
                             repository.getDefaultCollector(IDS[i], IDS[i]);
                balm::Collector  expValue(col->metricId());
                const Category *CATEGORY = col->metricId().category();

                balm::Metric  mX(col); const balm::Metric& MX = mX;
                for (int j = 0; j < NUM_UPDATES; ++j) {
                    ASSERT(recordVal(&expValue) == recordVal(col));

                    bool enabled = 0 == j % 2;
                    registry.setCategoryEnabled(CATEGORY, enabled);
                    mX.update(UPDATES[j]);
                    if (enabled) {
                        expValue.update(UPDATES[j]);
                    }
                    ASSERT(enabled == MX.isActive());
                    ASSERT(recordVal(&expValue) == recordVal(col));
                }
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING: 'increment'
        //
        // Concerns:
        //   That 'increment' properly invokes the metric's collector's
        //   'increment' method.  That 'increment' properly observers the
        //   categories 'enabled' status.
        //
        // Plan:
        //   Specify a set S of unique object values having various minor or
        //   subtle differences.  For each value create an "oracle" collector,
        //   increment the metric and the "oracle", and verify the collector's
        //   underlying the metric has the same value as the "oracle"
        //   collector.
        //
        //   Perform the same test again, but enabled or disable the metric's
        //   category on each iteration.
        //
        // Testing:
        //    void increment();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING: 'increment'\n"
                          << "====================\n";

        const char *IDS[] = {"A", "B", "AB", "TEST"};
        const int NUM_IDS = sizeof IDS / sizeof *IDS;

        if (veryVerbose)
            cout << "\tverify 'increment' is applied correctly.\n";

        {
            balm::MetricsManager mgr(Z);
            Registry&   registry   = mgr.metricRegistry();
            Repository& repository = mgr.collectorRepository();
            for (int i = 0; i < NUM_IDS; ++i) {
                balm::Collector *col =
                               repository.getDefaultCollector(IDS[i], IDS[i]);
                balm::Collector  expValue(col->metricId());

                balm::Metric  mX(col); const balm::Metric& MX = mX;
                for (int j = 0; j < 10; ++j) {
                    ASSERT(recordVal(&expValue) == recordVal(col));
                    mX.increment();
                    expValue.update(1);
                    ASSERT(recordVal(&expValue) == recordVal(col));
                }
            }
        }

        if (veryVerbose)
            cout << "\tverify 'increment' respects 'isActive' flag.\n";

        {
            for (int i = 0; i < NUM_IDS; ++i) {
                balm::MetricsManager mgr(Z);
                Registry&   registry   = mgr.metricRegistry();
                Repository& repository = mgr.collectorRepository();

                balm::Collector *col =
                              repository.getDefaultCollector(IDS[i], IDS[i]);
                balm::Collector  expValue(col->metricId());
                const Category *CATEGORY = col->metricId().category();

                balm::Metric  mX(col); const balm::Metric& MX = mX;
                for (int j = 0; j < 10; ++j) {
                    ASSERT(recordVal(&expValue) == recordVal(col));

                    bool enabled = 0 == j % 2;
                    registry.setCategoryEnabled(CATEGORY, enabled);
                    mX.increment();
                    if (enabled) {
                        expValue.update(1);
                    }
                    ASSERT(enabled == MX.isActive());
                    ASSERT(recordVal(&expValue) == recordVal(col));
                }
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR:
        //
        // Concerns:
        //   Any value must be able to be copy constructed without affecting
        //   its argument.
        //
        // Plan:
        //   Specify a set S whose elements have substantial and varied
        //   differences in value.  For each element in S, construct and
        //   initialize identically valued objects w and x using tested
        //   methods.  Then copy construct an object y from x, and use the
        //   equality operator to assert that both x and y have the same value
        //   as w.
        //
        // Testing:
        //   balm::MetricRecord(const balm::MetricRecord& );
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING COPY CONSTRUCTOR\n"
                          << "=========================\n";

        const char *VALUES[] = {"A", "B", "AB", "TEST"};
        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

        balm::MetricsManager mgr(Z);
        Registry&   registry   = mgr.metricRegistry();
        Repository& repository = mgr.collectorRepository();

        for (int i = 0; i < NUM_VALUES; ++i) {

            Id   id = registry.getId(VALUES[i], VALUES[i]);
            Collector *collector = repository.getDefaultCollector(id);

            Obj x(collector); const Obj& X = x;
            Obj w(collector); const Obj& W = w;

            Obj y(X); const Obj& Y = y;

            ASSERT(W == Y);
            ASSERT(X == Y);

            // Test that the enabled flag is passed correctly.
            ASSERT(W.isActive());
            ASSERT(X.isActive());
            ASSERT(Y.isActive());

            registry.setCategoryEnabled(id.category(), false);
            ASSERT(!W.isActive());
            ASSERT(!X.isActive());
            ASSERT(!Y.isActive());

            registry.setCategoryEnabled(id.category(), true);
            ASSERT(W.isActive());
            ASSERT(X.isActive());
            ASSERT(Y.isActive());
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATOR
        //
        // Concerns:
        //   Any subtle variation in value must be detected by the equality
        //   operators.  The test data have variations in each input parameter,
        //   even though tested methods convert the input before the underlying
        //   equality operators are invoked.
        //
        // Plan:
        //   Specify a set S of unique object values having various minor or
        //   subtle differences.  Verify the correctness of 'operator==' and
        //   'operator!=' using all elements (u, v) of the cross product
        //    S X S.
        //
        // Testing:
        //   bool operator==(const balm::Metric&, const balm::Metric&);
        //   bool operator!=(const balm::Metric&, const balm::Metric&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTPING EQUALITY OPERATOR\n"
                          << "==========================\n";

        const char *VALUES[] = {"A", "B", "AB", "TEST"};
        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

        balm::MetricsManager mgr(Z);
        Registry&   registry   = mgr.metricRegistry();
        Repository& repository = mgr.collectorRepository();

        for (int i = 0; i < NUM_VALUES; ++i) {
            Obj u(repository.getDefaultCollector(VALUES[i], VALUES[i]));
            const Obj& U = u;

            for (int j = 0; j < NUM_VALUES; ++j) {
                Obj v(repository.getDefaultCollector(VALUES[j], VALUES[j]));
                const Obj& V = v;

                bool isEqual = i == j;
                LOOP2_ASSERT(i, j, isEqual  == (U == V));
                LOOP2_ASSERT(i, j, !isEqual == (U != V))
            }
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING: 'isActive'
        //
        // Concerns:
        //   That 'isActive' returns 'true' if the collector's category is
        //   enabled and 'false' if the collector's category is disabled, and
        //   that 'isActive' always returns 'false' if the metric's collector
        //   it is 0.
        //
        // Plan:
        //   Specify a set S of unique object values having various minor or
        //   subtle differences.  For each value, verify that disabling the
        //   'balm::Category' sets the metric's 'isActive' flag to 'false', and
        //   that enabling the category sets the metric's 'isActive' flag to
        //   'true'.  Also verify that a 'balm::Metric' with a null collector
        //   always returns 'false' for 'isActive'.
        //
        // Testing:
        //   bool isActive() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING: 'isActive'\n"
                          << "===================\n";

        const char *IDS[] = {"A", "B", "AB", "TEST"};
        const int NUM_IDS = sizeof IDS / sizeof *IDS;

        balm::MetricsManager mgr(Z);
        Registry&   registry   = mgr.metricRegistry();
        Repository& repository = mgr.collectorRepository();

        for (int i = 0; i < NUM_IDS; ++i) {
            const Category *CATEGORY = registry.getCategory(IDS[i]);

            Obj mX(IDS[i], IDS[i], &mgr); const Obj& MX = mX;
            Obj mY(IDS[i], IDS[i]); const Obj& MY = mY;

            ASSERT( MX.isActive());
            ASSERT(!MY.isActive());

            registry.setCategoryEnabled(CATEGORY, false);

            ASSERT(!MX.isActive());
            ASSERT(!MY.isActive());

            registry.setCategoryEnabled(CATEGORY, true);

            ASSERT(MX.isActive());
            ASSERT(!MY.isActive());

            registry.setCategoryEnabled(CATEGORY, false);

            ASSERT(!MX.isActive());
            ASSERT(!MY.isActive());
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING BASIC CONSTRUCTORS & ACCESSORS (BOOTSTRAP):
        //
        //
        // Concerns:
        //   The primary fields must be set by the constructors and correctly
        //   accessible.
        //
        // Plan:
        //   Specify a set S of unique object values having various minor or
        //   subtle differences.
        //
        //   Create a 'balm::Metric' object for each member of set S without a
        //   metrics manager or default metrics manager and verify their
        //   collector value is 0.
        //
        //   Create a 'balm::Metric' object for each member of set S and
        //   explicitly specify the metrics manager.  Verify the collector
        //   value is the correct collector from the metrics manager supplied
        //   at construction.
        //
        //   Create a default metrics manager.  Then create a 'balm::Metric'
        //   object for each member of set S without (explicitly) specifying a
        //   metrics manager.  Verify the collector value is the correct
        //   collector from the default metrics manager.
        //
        // Testing:
        //   balm::Metric(const bslstl::StringRef&  ,
        //                const bslstl::StringRef&  ,
        //                balm::MetricsManager    *);
        //   balm::Metric(const balm::MetricId&, balm::MetricsManager *);
        //   balm::Metric(balm::Collector *collector);
        //   ~balm::Metric();
        //   balm::Collector *collector();
        //   const balm::Collector *collector() const;
        //   balm::MetricId metricId() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING PRIMARY ACCESSORS\n"
                          << "=========================\n";

        const char *IDS[] = {"A", "B", "AB", "TEST"};
        const int NUM_IDS = sizeof IDS / sizeof *IDS;

        balm::MetricsManager mgr(Z);
        Registry&   registry   = mgr.metricRegistry();
        Repository& repository = mgr.collectorRepository();

        if (veryVerbose)
            cout << "\tTest construction with no default metrics manager\n";

        for (int i = 0; i < NUM_IDS; ++i) {
            Obj mX(IDS[i], IDS[i]); const Obj& MX = mX;
            Obj mY(registry.getId(IDS[i], IDS[i])); const Obj& MY = mY;

            ASSERT(0 == mX.collector());
            ASSERT(0 == mY.collector());
            ASSERT(0 == MX.collector());
            ASSERT(0 == MY.collector());
        }

        if (veryVerbose)
            cout << "\ttest with an explicit metrics manager\n";

        for (int i = 0; i < NUM_IDS; ++i) {
            const Id   ID = registry.getId(IDS[i], IDS[i]);
            Collector *COL = repository.getDefaultCollector(ID);
            Obj mX(IDS[i], IDS[i], &mgr); const Obj& MX = mX;
            Obj mY(ID, &mgr); const Obj& MY = mY;
            Obj mZ(COL); const Obj& MZ = mZ;

            ASSERT(COL == mX.collector());
            ASSERT(COL == mY.collector());
            ASSERT(COL == mZ.collector());
            ASSERT(COL == MX.collector());
            ASSERT(COL == MY.collector());
            ASSERT(COL == MZ.collector());

            ASSERT(ID == mX.metricId());
            ASSERT(ID == mY.metricId());
            ASSERT(ID == mZ.metricId());
            ASSERT(ID == MX.metricId());
            ASSERT(ID == MY.metricId());
            ASSERT(ID == MZ.metricId());

            ASSERT(0 == defaultAllocator.numBytesInUse());
        }

        if (veryVerbose)
            cout << "\ttest with the default metrics manager\n";
        {
            balm::DefaultMetricsManagerScopedGuard guard(Z);
            balm::MetricsManager& mgr = *DefaultManager::instance();

            Registry&   registry   = mgr.metricRegistry();
            Repository& repository = mgr.collectorRepository();

            for (int i = 0; i <NUM_IDS; ++i) {
                const Id   ID  = registry.getId(IDS[i], IDS[i]);
                Collector *COL = repository.getDefaultCollector(ID);
                Obj mX(IDS[i], IDS[i]); const Obj& MX = mX;
                Obj mY(ID); const Obj& MY = mY;

                ASSERT(COL == mX.collector());
                ASSERT(COL == mY.collector());
                ASSERT(COL == MX.collector());
                ASSERT(COL == MY.collector());
                ASSERT(ID == mX.metricId());
                ASSERT(ID == mY.metricId());
                ASSERT(ID == MX.metricId());
                ASSERT(ID == MY.metricId());

                ASSERT(0 == defaultAllocator.numBytesInUse());
            }
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Developers' Sandbox.
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   This "test" *exercises* basic functionality, but *tests* nothing.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        {
            balm::MetricsManager       manager(Z);
            balm::MetricRegistry&      registry = manager.metricRegistry();
            balm::CollectorRepository& repository =
                                                manager.collectorRepository();

            balm::Collector *A_COL = repository.getDefaultCollector("A", "A");
            balm::Collector *B_COL = repository.getDefaultCollector("B", "B");

            const Id A_ID = A_COL->metricId();
            const Id B_ID = B_COL->metricId();

            if (veryVerbose) cout << "\tTesting 'bcemt::Metric'" << endl;

            Obj a1("A", "A", &manager);                 const Obj& A1 = a1;
            Obj a2(registry.getId("A", "A"), &manager); const Obj& A2 = a2;
            Obj a3(repository.getDefaultCollector("A", "A"));
            const Obj& A3 = a3;

            Obj b1("B", "B", &manager);                 const Obj& B1 = b1;
            Obj b2(registry.getId("B", "B"), &manager); const Obj& B2 = b2;
            Obj b3(repository.getDefaultCollector("B", "B"));
            const Obj& B3 = b3;

            Obj invalid1("A", "A");
            const Obj& INVALID1 = invalid1;
            Obj invalid2(registry.getId("A", "A"));
            const Obj& INVALID2 = invalid2;

            ASSERT(A1 == A2 && A2 == A3);
            ASSERT(B1 == B2 && B2 == B3);
            ASSERT(!(A1 == B1));
            ASSERT(!(A2 == B2));
            ASSERT(!(A3 == B3));

            ASSERT(A_COL == a1.collector());
            ASSERT(A_COL == a2.collector());
            ASSERT(A_COL == a3.collector());
            ASSERT(A_COL == A1.collector());
            ASSERT(A_COL == A2.collector());
            ASSERT(A_COL == A3.collector());

            ASSERT(B_COL == b1.collector());
            ASSERT(B_COL == b2.collector());
            ASSERT(B_COL == b3.collector());
            ASSERT(B_COL == B1.collector());
            ASSERT(B_COL == B2.collector());
            ASSERT(B_COL == B3.collector());

            ASSERT(0     == invalid1.collector());
            ASSERT(0     == invalid2.collector());
            ASSERT(0     == INVALID1.collector());
            ASSERT(0     == INVALID2.collector());

            ASSERT(A1.isActive());
            ASSERT(A2.isActive());
            ASSERT(A3.isActive());
            ASSERT(B1.isActive());
            ASSERT(B2.isActive());
            ASSERT(B3.isActive());
            ASSERT(!INVALID1.isActive());
            ASSERT(!INVALID2.isActive());

            ASSERT(A_ID == A1.metricId());
            ASSERT(A_ID == A2.metricId());
            ASSERT(A_ID == A3.metricId());
            ASSERT(B_ID == B1.metricId());
            ASSERT(B_ID == B2.metricId());
            ASSERT(B_ID == B3.metricId());

            ASSERT(balm::MetricRecord(A_ID) == recordVal(A_COL));
            ASSERT(balm::MetricRecord(B_ID) == recordVal(B_COL));

            a1.increment();
            a2.increment();
            a3.increment();
            b1.increment();
            b2.increment();
            b3.increment();

            ASSERT(balm::MetricRecord(A_ID, 3, 3, 1, 1) == recordVal(A_COL));
            ASSERT(balm::MetricRecord(B_ID, 3, 3, 1, 1) == recordVal(B_COL));

            a1.update(2);
            a2.update(2);
            a3.update(2);
            b1.update(2);
            b2.update(2);
            b3.update(2);

            ASSERT(balm::MetricRecord(A_ID, 6, 9, 1, 2) == recordVal(A_COL));
            ASSERT(balm::MetricRecord(B_ID, 6, 9, 1, 2) == recordVal(B_COL));

            a1.accumulateCountTotalMinMax(2, 3, -1, -1);
            a2.accumulateCountTotalMinMax(2, 3, -1, -1);
            a3.accumulateCountTotalMinMax(2, 3, -1, -1);
            b1.accumulateCountTotalMinMax(2, 3, -1, -1);
            b2.accumulateCountTotalMinMax(2, 3, -1, -1);
            b3.accumulateCountTotalMinMax(2, 3, -1, -1);

            ASSERT(
                  balm::MetricRecord(A_ID, 12, 18, -1, 2) == recordVal(A_COL));
            ASSERT(
                  balm::MetricRecord(B_ID, 12, 18, -1, 2) == recordVal(B_COL));

            ASSERT(0 == defaultAllocator.numBytesInUse());
        }
      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
                  << bsl::endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
