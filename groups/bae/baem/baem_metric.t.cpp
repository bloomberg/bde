// baem_metric.t.cpp  -*-C++-*-
#include <baem_metric.h>

#include <baem_metricregistry.h>
#include <baem_metricsample.h>
#include <baem_publisher.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bcema_testallocator.h>
#include <bcemt_barrier.h>
#include <bcep_fixedthreadpool.h>
#include <bdef_bind.h>

#include <bsls_stopwatch.h>
#include <bsl_ostream.h>
#include <bsl_cstring.h>
#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;

using bsl::cout;
using bsl::endl;
using bsl::flush;

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The 'baem_Metric' class and the macros defined in this component primarily
// provide user-friendly access to a 'baem_Collector' object.  The classes and
// macros supply simplified constructors (that lookup the appropriate
// 'baem_Collector' instance) and manipulator methods that respect
// whether the category of the collector is enabled.  Most of the tests can be
// performed by creating an "oracle" 'baem_Collector' object and verifying that
// operations performed on a 'baem_Metric' (or with a macro) have the same
// effect as the same operation performed directly on the "oracle"
// 'baem_Collector'.
//-----------------------------------------------------------------------------
// CLASS METHODS
// [ 9] static baem_Collector *lookupCollector(const bdeut_StringRef&  ,
//                                             const bdeut_StringRef&  ,
//                                             baem_MetricsManager    *);
// [ 9] static baem_Collector *lookupCollector(const baem_MetricId&  ,
//                                             baem_MetricsManager  *);
// CREATORS
// [ 2] baem_Metric(const bdeut_StringRef&  ,
//                  const bdeut_StringRef&  ,
//                  baem_MetricsManager    *);
// [ 2] explicit baem_Metric(const baem_MetricId&  , baem_MetricsManager  *);
// [ 2] explicit baem_Metric(baem_Collector *collector);
// [ 5] baem_Metric(const baem_Metric& original);
// [ 2] ~baem_Metric();
// MANIPULATORS
// [ 6] void increment();
// [ 7] void update(double value);
// [ 8] void accumulateCountTotalMinMax(int, double, double, double);
// [ 2] baem_Collector *collector();
// ACCESSORS
// [ 2] const baem_Collector *collector() const;
// [ 2] baem_MetricId metricId() const;
// [ 3] bool isActive() const;
// FREE OPERATORS
// [ 4]  bool operator==(const baem_Metric&,
//                       const baem_Metric&);
// [ 4]  bool operator!=(const baem_Metric&,
//                       const baem_Metric&);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [10] CONCURRENCY TEST: 'baem_Metric'
// [11] USAGE EXAMPLE

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\n"; \
                aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t"  \
                          << #J << ": " << J << "\n"; \
                aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" \
                         << #J << ": " << J << "\t" \
                         << #K << ": " << K << "\n";\
               aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) bsl::cout << #X " = " << (X) << bsl::endl;
                                              // Print identifier and value.
#define Q(X) bsl::cout << "<| " #X " |>" << bsl::endl;
                                              // Quote identifier literally.
#define P_(X) bsl::cout << #X " = " << (X) << ", " << bsl::flush;
                                              // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define NL "\n"
#define T_() cout << '\t' << flush;

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
typedef baem_DefaultMetricsManager DefaultManager;
typedef baem_MetricRegistry        Registry;
typedef baem_CollectorRepository   Repository;
typedef baem_Collector             Collector;
typedef baem_MetricId              Id;
typedef baem_Category              Category;
typedef baem_Metric                Obj;
typedef baem_PublicationType       Type;

typedef bcema_SharedPtr<baem_Collector>         ColSPtr;
typedef bcema_SharedPtr<baem_IntegerCollector>  IColSPtr;
typedef bsl::vector<ColSPtr>                    ColSPtrVector;
typedef bsl::vector<IColSPtr>                   IColSPtrVector;

//=============================================================================
//                    CLASSES FOR AND FUNCTIONS TESTING
//-----------------------------------------------------------------------------

inline
baem_MetricRecord recordVal(const baem_Collector *collector)
    // Return the current record value of the specified 'collector'.
{
    baem_MetricRecord record;
    collector->load(&record);
    return record;
}

// --------------------- case 14: MetricConcurrencyTest -----------------------

class MetricConcurrencyTest {
    // Invoke a set of operations operations synchronously.

    // DATA
    bcep_FixedThreadPool   d_pool;
    baem_Metric           *d_metric;
    baem_MetricRegistry   *d_registry;
    bcemt_Barrier          d_barrier;
    bslma_Allocator       *d_allocator_p;

    // PRIVATE MANIPULATORS
    void execute();
        // Execute a single test.

  public:

    // CREATORS
    MetricConcurrencyTest(int                  numThreads,
                          baem_Metric         *metric,
                          baem_MetricRegistry *registry,
                          bslma_Allocator     *basicAllocator)
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
    // the manipulators for the 'baem_Metric' under test for a series of
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

    baem_MetricRecord recordValue; collector->load(&recordValue);
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
    bdef_Function<void(*)()> job = bdef_BindUtil::bindA(
                                            d_allocator_p,
                                            &MetricConcurrencyTest::execute,
                                            this);

    for (int i = 0; i < d_pool.numThreads(); ++i) {
        d_pool.enqueueJob(job);
    }
    d_pool.drain();
}


//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

//..
///Example 2 - Metric collection with 'baem_Metric'
///- - - - - - - - - - - - - - - - - - - - - - - -
// Alternatively we can use a 'baem_Metric' to record metric values.  In this
// third example we implement a hypothetical event manager object, similar in
// purpose to the 'processEvent' function of example 2.  We use 'baem_Metric'
// objects to record metrics for the size of the request, the elapsed
// processing time, and the number of failures.
//..
    class EventManager {

        // DATA
        baem_Metric d_messageSize;
        baem_Metric d_elapsedTime;
        baem_Metric d_failedRequests;

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
           bsls_Stopwatch stopwatch;
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

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;;

    bslma_TestAllocator testAlloc; bslma_TestAllocator *Z = &testAlloc;
    bslma_TestAllocator defaultAllocator;
    bslma_DefaultAllocatorGuard guard(&defaultAllocator);

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

///Usage
///-----
// The following examples demonstrate how to configure, collect, and publish
// metrics.
//
///Example 1 - Create and access the default 'baem_MetricsManager' instance
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates how to create the default 'baem_MetricManager'
// instance and perform a trivial configuration.
//
// Create a 'baem_DefaultMetricsManagerScopedGuard' that manages the lifetime
// of the default metrics manager instance, and provide it a stream ('stdout')
// that we want to publish metrics to.  Note that the default metrics
// manager is intended to be created and released by the *owner* of  'main'.
// The instance should be created during the initialization of an
// application (while the task has a single thread) and released just prior to
// termination (when there is similarly a single thread).
//..
//  int main(int argc, char *argv[])
    {

    // ...

        baem_DefaultMetricsManagerScopedGuard managerGuard(bsl::cout);
//..
// Once the default instance has been created, it can be accessed using the
// 'instance' operation
//..
        baem_MetricsManager *manager  = baem_DefaultMetricsManager::instance();
                        ASSERT(0       != manager);
//..
// Note that the default metrics manager will be released when the
// 'managerGuard' exits this scoped and is destroyed.  Clients that choose to
// explicitly call 'baem_DefaultMetricsManager::create' must also explicitly
// call 'baem_DefaultMetricsManager::release()'.

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

    } break;
      case 10: {
        // --------------------------------------------------------------------
        // CONCURRENCY TEST: 'baem_Metric'
        //
        // Testing:
        //     Thread-safety of 'baem_Metric' methods.
        //
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TEST CONCURRENCY: 'baem_Metric'" << endl
                                  << "===============================" << endl;

        bcema_TestAllocator defaultAllocator;

        baem_MetricsManager mgr(Z);
        Registry&   registry   = mgr.metricRegistry();
        Repository& repository = mgr.collectorRepository();
        baem_Metric metric(repository.getDefaultCollector("Test", "Test"));

        bcema_TestAllocator testAllocator;
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
        //   static baem_Collector *lookupCollector(
        //                                const bdeut_StringRef&  ,
        //                                const bdeut_StringRef&  ,
        //                                baem_MetricsManager    *);
        //   static baem_Collector *lookupCollector(const baem_MetricId&  ,
        //                                          baem_MetricsManager  *);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING: 'lookupCollector'\n"
                          << "==========================\n";

        const char *IDS[] = {"A", "B", "AB", "TEST"};
        const int NUM_IDS = sizeof IDS / sizeof *IDS;

        if (veryVerbose)
            cout << "\tverify with no default metrics manager\n";
        {
            baem_MetricRegistry registry(Z);
            for (int i = 0; i < NUM_IDS; ++i) {
                baem_MetricId id = registry.getId(IDS[i], IDS[i]);
                ASSERT(0 == Obj::lookupCollector(IDS[i], IDS[i], 0));
                ASSERT(0 == Obj::lookupCollector(IDS[i], IDS[i]));
                ASSERT(0 == Obj::lookupCollector(id, 0));
                ASSERT(0 == Obj::lookupCollector(id));
            }
        }

        if (veryVerbose)
            cout << "\tverify with explicit metrics manager\n";
        {
            baem_MetricsManager mgr;
            Registry&   registry   = mgr.metricRegistry();
            Repository& repository = mgr.collectorRepository();

            for (int i = 0; i < NUM_IDS; ++i) {
                baem_MetricId id = registry.getId(IDS[i], IDS[i]);
                baem_Collector *col = repository.getDefaultCollector(id);
                ASSERT(col == Obj::lookupCollector(IDS[i], IDS[i], &mgr));
                ASSERT(col == Obj::lookupCollector(id, &mgr));
            }
        }

        if (veryVerbose)
            cout << "\tverify with default metrics manager\n";
        {
            baem_DefaultMetricsManagerScopedGuard guard(Z);
            baem_MetricsManager& mgr = *DefaultManager::instance();
            Registry&   registry   = mgr.metricRegistry();
            Repository& repository = mgr.collectorRepository();

            for (int i = 0; i < NUM_IDS; ++i) {
                baem_MetricId id = registry.getId(IDS[i], IDS[i]);
                baem_Collector *col = repository.getDefaultCollector(id);
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
            baem_MetricsManager mgr(Z);
            Registry&   registry   = mgr.metricRegistry();
            Repository& repository = mgr.collectorRepository();

            for (int i = 0; i < NUM_IDS; ++i) {
                baem_Collector *col =
                               repository.getDefaultCollector(IDS[i], IDS[i]);
                baem_Collector  expValue(col->metricId());

                baem_Metric  mX(col); const baem_Metric& MX = mX;
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
                baem_MetricsManager mgr(Z);
                Registry&   registry   = mgr.metricRegistry();
                Repository& repository = mgr.collectorRepository();

                baem_Collector *col =
                               repository.getDefaultCollector(IDS[i], IDS[i]);
                baem_Collector  expValue(col->metricId());
                const Category *CATEGORY = col->metricId().category();

                baem_Metric  mX(col); const baem_Metric& MX = mX;
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
            baem_MetricsManager mgr(Z);
            Registry&   registry   = mgr.metricRegistry();
            Repository& repository = mgr.collectorRepository();

            for (int i = 0; i < NUM_IDS; ++i) {
                baem_Collector *col =
                               repository.getDefaultCollector(IDS[i], IDS[i]);
                baem_Collector  expValue(col->metricId());

                baem_Metric  mX(col); const baem_Metric& MX = mX;
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
                baem_MetricsManager mgr(Z);
                Registry&   registry   = mgr.metricRegistry();
                Repository& repository = mgr.collectorRepository();

                baem_Collector *col =
                             repository.getDefaultCollector(IDS[i], IDS[i]);
                baem_Collector  expValue(col->metricId());
                const Category *CATEGORY = col->metricId().category();

                baem_Metric  mX(col); const baem_Metric& MX = mX;
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
            baem_MetricsManager mgr(Z);
            Registry&   registry   = mgr.metricRegistry();
            Repository& repository = mgr.collectorRepository();
            for (int i = 0; i < NUM_IDS; ++i) {
                baem_Collector *col =
                               repository.getDefaultCollector(IDS[i], IDS[i]);
                baem_Collector  expValue(col->metricId());

                baem_Metric  mX(col); const baem_Metric& MX = mX;
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
                baem_MetricsManager mgr(Z);
                Registry&   registry   = mgr.metricRegistry();
                Repository& repository = mgr.collectorRepository();

                baem_Collector *col =
                              repository.getDefaultCollector(IDS[i], IDS[i]);
                baem_Collector  expValue(col->metricId());
                const Category *CATEGORY = col->metricId().category();

                baem_Metric  mX(col); const baem_Metric& MX = mX;
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
        //   baem_MetricRecord(const baem_MetricRecord& );
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING COPY CONSTRUCTOR\n"
                          << "=========================\n";

        const char *VALUES[] = {"A", "B", "AB", "TEST"};
        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

        baem_MetricsManager mgr(Z);
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
        //   bool operator==(const baem_Metric&, const baem_Metric&);
        //   bool operator!=(const baem_Metric&, const baem_Metric&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTPING EQUALITY OPERATOR\n"
                          << "==========================\n";

        const char *VALUES[] = {"A", "B", "AB", "TEST"};
        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

        baem_MetricsManager mgr(Z);
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
        //   'baem_Category' sets the metric's 'isActive' flag to 'false', and
        //   that enabling the category sets the metric's 'isActive' flag to
        //   'true'.  Also verify that a 'baem_Metric' with a null collector
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

        baem_MetricsManager mgr(Z);
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
        //   Create a 'baem_Metric' object for each member of set S without a
        //   metrics manager or default metrics manager and verify their
        //   collector value is 0.
        //
        //   Create a 'baem_Metric' object for each member of set S and
        //   explicitly specify the metrics manager.  Verify the collector
        //   value is the correct collector from the metrics manager supplied
        //   at construction.
        //
        //   Create a default metrics manager.  Then create a 'baem_Metric'
        //   object for each member of set S without (explicitly) specifying a
        //   metrics manager.  Verify the collector value is the correct
        //   collector from the default metrics manager.
        //
        // Testing:
        //    baem_Metric(const bdeut_StringRef&  ,
        //                const bdeut_StringRef&  ,
        //                baem_MetricsManager    *);
        //    explicit baem_Metric(const baem_MetricId&  ,
        //                         baem_MetricsManager  *);
        //    explicit baem_Metric(baem_Collector *);
        //    ~baem_Metric();
        //    baem_Collector *collector();
        //    const baem_Collector *collector() const;
        //    baem_MetricId metricId() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING PRIMARY ACCESSORS\n"
                          << "=========================\n";

        const char *IDS[] = {"A", "B", "AB", "TEST"};
        const int NUM_IDS = sizeof IDS / sizeof *IDS;

        baem_MetricsManager mgr(Z);
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
            baem_DefaultMetricsManagerScopedGuard guard(Z);
            baem_MetricsManager& mgr = *DefaultManager::instance();

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
            baem_MetricsManager       manager(Z);
            baem_MetricRegistry&      registry = manager.metricRegistry();
            baem_CollectorRepository& repository =
                                                manager.collectorRepository();

            baem_Collector *A_COL = repository.getDefaultCollector("A", "A");
            baem_Collector *B_COL = repository.getDefaultCollector("B", "B");

            const Id A_ID = A_COL->metricId();
            const Id B_ID = B_COL->metricId();

            if (veryVerbose) cout << "\tTesting 'bcemt_Metric'" << endl;

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

            ASSERT(baem_MetricRecord(A_ID) == recordVal(A_COL));
            ASSERT(baem_MetricRecord(B_ID) == recordVal(B_COL));

            a1.increment();
            a2.increment();
            a3.increment();
            b1.increment();
            b2.increment();
            b3.increment();

            ASSERT(baem_MetricRecord(A_ID, 3, 3, 1, 1) == recordVal(A_COL));
            ASSERT(baem_MetricRecord(B_ID, 3, 3, 1, 1) == recordVal(B_COL));

            a1.update(2);
            a2.update(2);
            a3.update(2);
            b1.update(2);
            b2.update(2);
            b3.update(2);

            ASSERT(baem_MetricRecord(A_ID, 6, 9, 1, 2) == recordVal(A_COL));
            ASSERT(baem_MetricRecord(B_ID, 6, 9, 1, 2) == recordVal(B_COL));

            a1.accumulateCountTotalMinMax(2, 3, -1, -1);
            a2.accumulateCountTotalMinMax(2, 3, -1, -1);
            a3.accumulateCountTotalMinMax(2, 3, -1, -1);
            b1.accumulateCountTotalMinMax(2, 3, -1, -1);
            b2.accumulateCountTotalMinMax(2, 3, -1, -1);
            b3.accumulateCountTotalMinMax(2, 3, -1, -1);

            ASSERT(baem_MetricRecord(A_ID, 12, 18, -1, 2) == recordVal(A_COL));
            ASSERT(baem_MetricRecord(B_ID, 12, 18, -1, 2) == recordVal(B_COL));

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
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
