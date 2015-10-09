// balm_metrics.t.cpp                                                 -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balm_metrics.h>

#include <balm_metricregistry.h>
#include <balm_metricsample.h>
#include <balm_publisher.h>

#include <bslma_testallocator.h>
#include <bslmt_barrier.h>
#include <bdlmt_fixedthreadpool.h>

#include <bdlf_bind.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bsls_assert.h>

#include <bsls_stopwatch.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_functional.h>
#include <bsl_iostream.h>
#include <bsl_ostream.h>
#include <bsl_sstream.h>

#include <ball_observer.h>
#include <ball_defaultobserver.h>
#include <ball_log.h>
#include <ball_loggermanager.h>
#include <ball_severity.h>

#include <bslim_testutil.h>

// It is very important not to say 'using BloombergLP;' in this test driver, in
// order to test that the macros will work when 'BloombergLP' is not in scope.

namespace Corp = BloombergLP;
namespace BALM = BloombergLP::balm;

using bsl::cout;
using bsl::endl;
using bsl::flush;

// ============================================================================
//                                  TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The macros defined in this component primarily provide user-friendly access
// to a 'balm::Collector' object.  Most of the tests can be performed by
// creating an "oracle" 'balm::Collector' object and verifying that operations
// performed on with a macro have the same effect as the same operation
// performed directly on the "oracle" 'balm::Collector'.  Note that, the
// "STANDARD" macro variants (i.e., 'BALM_METRICS_UPDATE') and the
// "THREAD_LOCAL" macro variants (i.e., 'BALM_METRICS_THREAD_LOCAL_UPDATE')
// have more complex behavior with respect to their statically cached (or
// thread-local statically cached) 'balm::Collector' object.
// ----------------------------------------------------------------------------
// MACROS
// [ 2] BALM_METRICS_UPDATE(CATEGORY, NAME, VALUE)
// [ 2] BALM_METRICS_INCREMENT(CATEGORY, NAME)
// [ 2] BALM_METRICS_TYPED_UPDATE(CATEGORY, NAME, VALUE, PREFERRED_TYPE)
// [ 2] BALM_METRICS_TYPED_INCREMENT(CATEGORY, NAME, PREFERRED_TYPE)
// [ 3] BALM_METRICS_DYNAMIC_UPDATE(CATEGORY, NAME, VALUE)
// [ 3] BALM_METRICS_DYNAMIC_INCREMENT(CATEGORY, NAME)
// [ 4] BALM_METRICS_THREAD_LOCAL_UPDATE(CATEGORY, NAME, VALUE)
// [ 4] BALM_METRICS_THREAD_LOCAL_INCREMENT(CATEGORY, NAME)
// [ 5] BALM_METRICS_INT_UPDATE(CATEGORY, NAME, VALUE)
// [ 5] BALM_METRICS_INT_INCREMENT(CATEGORY, NAME)
// [ 5] BALM_METRICS_TYPED_INT_UPDATE(CATEGORY, NAME, VALUE)
// [ 5] BALM_METRICS_TYPED_INT_INCREMENT(CATEGORY, NAME)
// [ 6] BALM_METRICS_DYNAMIC_INT_UPDATE(CATEGORY, NAME, VALUE)
// [ 6] BALM_METRICS_DYNAMIC_INT_INCREMENT(CATEGORY, NAME)
// [ 7] BALM_METRICS_THREAD_LOCAL_INT_UPDATE(CATEGORY, NAME, VALUE)
// [ 7] BALM_METRICS_THREAD_LOCAL_INT_INCREMENT(CATEGORY, NAME)
// [ 8] BEAM_METRICS_TIME_BLOCK(CATEGORY, METRIC, TIME_UNITS)
// [ 8] BALM_METRICS_DYNAMIC_TIME_BLOCK(CATEGORY, METRIC, TIME_UNITS)
// [ 9] BEAM_METRICS_TIME_BLOCK_SECONDS(CATEGORY, METRIC)
// [ 9] BEAM_METRICS_TIME_BLOCK_MILLISECONDS(CATEGORY, METRIC)
// [ 9] BEAM_METRICS_TIME_BLOCK_MICROSECONDS(CATEGORY, METRIC)
// [ 9] BEAM_METRICS_TIME_BLOCK_NANOSECONDS(CATEGORY, METRIC)
// [ 9] BALM_METRICS_DYNAMIC_TIME_BLOCK_SECONDS(CATEGORY, METRIC)
// [ 9] BALM_METRICS_DYNAMIC_TIME_BLOCK_MILLISECONDS(CATEGORY, METRIC)
// [ 9] BALM_METRICS_DYNAMIC_TIME_BLOCK_MICROSECONDS(CATEGORY, METRIC)
// [ 9] BALM_METRICS_DYNAMIC_TIME_BLOCK_NANOSECONDS(CATEGORY, METRIC)
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [11] CONCURRENCY TEST: STANDARD MACROS
// [12] CONCURRENCY TEST: DYNAMIC MACROS
// [13] CONCURRENCY TEST: THREAD_LOCAL MACROS
// [14] CONCURRENCY TEST: STANDARD INT MACROS
// [15] CONCURRENCY TEST: DYNAMIC INT MACROS
// [16] CONCURRENCY TEST: THREAD_LOCAL INT MACROS
// [17] void balm::Metrics_Helper::logEmptyName(NameType    type,
//                                             const char *name,
//                                             const char *file,
//                                             int         line);
// [18] WARNING LOG TEST: ALL MACROS
// [19] USAGE EXAMPLE

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
typedef BALM::DefaultMetricsManager DefaultManager;
typedef BALM::MetricRegistry        Registry;
typedef BALM::CollectorRepository   Repository;
typedef BALM::Collector             Collector;
typedef BALM::IntegerCollector      IntCollector;
typedef BALM::MetricId              Id;
typedef BALM::Category              Category;
typedef BALM::PublicationType       Type;
typedef BALM::MetricsManager        MetricsManager;
typedef BALM::StopwatchScopedGuard  SWGuard;

typedef bsl::shared_ptr<BALM::Collector>         ColSPtr;
typedef bsl::shared_ptr<BALM::IntegerCollector>  IColSPtr;
typedef bsl::vector<ColSPtr>                     ColSPtrVector;
typedef bsl::vector<IColSPtr>                    IColSPtrVector;

// ============================================================================
//                     CLASSES FOR AND FUNCTIONS TESTING
// ----------------------------------------------------------------------------

inline
BALM::MetricRecord recordVal(const BALM::Collector *collector)
    // Return the current record value of the specified 'collector'.
{
    BALM::MetricRecord record;
    collector->load(&record);
    return record;
}

inline
BALM::MetricRecord recordVal(const BALM::IntegerCollector *collector)
    // Return the current record value of the specified 'collector'.
{
    BALM::MetricRecord record;
    collector->load(&record);
    return record;
}

bool within(double         value,
            SWGuard::Units scale,
            double         expectedS,
            double         windowMs)
    // Return 'true' if the specified 'value' in the specified 'scale' is
    // within the specified 'windowMs' (milliseconds) of the specified
    // 'expectedS' (seconds).
{
    double expected = expectedS * scale;
    double window   = windowMs *1000 * scale;

    return ((expected - window) < value)
        && ((expected + window) > value);
}

bool withinDouble(double value, double min, double max)
{
    return (min <= value) && (max >= value);
}

bool withinInt(int value, int min, int max)
{
    return (min <= value) && (max >= value);
}

void staticTimeEmptyFunction(const char     *category,
                             const char     *metric,
                             SWGuard::Units  units = SWGuard::k_SECONDS)
    // Record, using the static 'BALM_TIME_BLOCK' macro, an empty function to
    // a metric identified by the specified 'category' and 'name'.  Optionally
    // specify the 'units' to report elapsed time values.  If no 'units' value
    // is supplied, time is reported in seconds.  Note that the identity of
    // the metric is defined on the *first* invocation of this macro.
{
    BALM_METRICS_TIME_BLOCK(category, metric, units);
}

void dynamicTimeEmptyFunction(const char *category, const char *metric)
    // Record, using the 'e_DYNAMIC_TIME_BLOCK_SECONDS' macro, an empty
    // function to a metric identified by the specified 'category' and 'name'.
{
    BALM_METRICS_DYNAMIC_TIME_BLOCK_SECONDS(category, metric);
}

bool breathingTestIfEnabledA()
{
    BALM_METRICS_IF_CATEGORY_ENABLED("A") {
        return true;                                                  // RETURN
    }
    return false;
}

bool balmMetricsIfCategoryEnabledTestA()
{
    BALM_METRICS_IF_CATEGORY_ENABLED("A") {
        return true;                                                  // RETURN
    }
    return false;
}

bool balmMetricsIfCategoryEnabledTest(const char *category)
{
    BALM_METRICS_IF_CATEGORY_ENABLED(category) {
        return true;                                                  // RETURN
    }
    return false;
}

class MessageObserver : public Corp::ball::Observer {
    // Custom observer to verify logged message.

  public:
    MessageObserver() : d_message() {}

    virtual ~MessageObserver() {}

    virtual void publish(const Corp::ball::Record& record,
                         const Corp::ball::Context&);
        // This function is called when a message is logged.  It will store the
        // message so that it can be retrieved and verified.  Any new messages
        // are concatenated to the message stored in the class.

    const bsl::string& getLastMessage() { return d_message; }
        // Retrieve the messages that was logged.

    void clearMessage() {d_message = "";}
        // Clear the message stored in the class.

  private:
    bsl::string d_message;
};

void MessageObserver::publish(const Corp::ball::Record& record,
                              const Corp::ball::Context&)
{
    const Corp::ball::RecordAttributes& fixedFields = record.fixedFields();
    d_message += fixedFields.message();
}

// ------------------- case 11 StandardMacroConcurrencyTest -----------------

class StandardMacroConcurrencyTest {
    // Invoke a set of operations operations synchronously.

    // DATA
    Corp::bdlmt::FixedThreadPool  d_pool;
    Corp::bslmt::Barrier          d_barrier;
    Corp::bslma::Allocator       *d_allocator_p;

    // PRIVATE MANIPULATORS
    void execute();
        // Execute a single test.

  public:

    // CREATORS
    StandardMacroConcurrencyTest(int                     numThreads,
                                 Corp::bslma::Allocator *basicAllocator)
    : d_pool(numThreads, 1000, basicAllocator)
    , d_barrier(numThreads)
    , d_allocator_p(basicAllocator)
    {
        d_pool.start();
    }

    ~StandardMacroConcurrencyTest() {}

    // MANIPULATORS
    void runTest();
        // Run the test.
};

void StandardMacroConcurrencyTest::execute()
{
    // Update 2 metrics in two categories ("A" and "B")  while enabling and
    // disabling the category "B".  Verify collectors being used by the macros
    // holds a value that matches the expected value (or expected range of
    // values) for the metric.

    const int   COUNT            = 30;

    BALM::MetricsManager &mgr = *DefaultManager::instance();
    Repository&                  repository = mgr.collectorRepository();

    d_barrier.wait();
    for (int i = 0; i < COUNT; ++i) {
        BALM_METRICS_UPDATE("A",
                            "A", 1.0);
        BALM_METRICS_UPDATE2("A",
                             "A", 2.0, "B", 2.0);
        BALM_METRICS_UPDATE3("A",
                             "A", 3.0, "B", 3.0, "C", 3.0);
        BALM_METRICS_UPDATE4("A",
                             "A", 4.0, "B", 4.0, "C", 4.0, "D", 4.0);
        BALM_METRICS_UPDATE5("A",
                             "A", 5.0, "B", 5.0, "C", 5.0, "D", 5.0,
                             "E", 5.0);
        BALM_METRICS_UPDATE6("A",
                             "A", 6.0, "B", 6.0, "C", 6.0, "D", 6.0,
                             "E", 6.0, "F", 6.0);
        BALM_METRICS_TYPED_UPDATE("A", "A", 7.0, Type::e_TOTAL);

        bool enabled = 0 == i % 2;

        mgr.setCategoryEnabled("B", enabled);

        BALM_METRICS_UPDATE("B",
                            "A", 1.0);
        BALM_METRICS_UPDATE2("B",
                             "A", 2.0, "B", 2.0);
        BALM_METRICS_UPDATE3("B",
                             "A", 3.0, "B", 3.0, "C", 3.0);
        BALM_METRICS_UPDATE4("B",
                             "A", 4.0, "B", 4.0, "C", 4.0, "D", 4.0);
        BALM_METRICS_UPDATE5("B",
                             "A", 5.0, "B", 5.0, "C", 5.0, "D", 5.0,
                             "E", 5.0);
        BALM_METRICS_UPDATE6("B",
                             "A", 6.0, "B", 6.0, "C", 6.0, "D", 6.0,
                             "E", 6.0, "F", 6.0);
        BALM_METRICS_TYPED_UPDATE("B", "A", 7.0, Type::e_TOTAL);
    }
    d_barrier.wait();

    const int REPS = COUNT * d_pool.numThreads();
    {
        // Verify basic macro behavior for category A

        BALM::Collector *A = repository.getDefaultCollector("A", "A");
        BALM::Collector *B = repository.getDefaultCollector("A", "B");
        BALM::Collector *C = repository.getDefaultCollector("A", "C");
        BALM::Collector *D = repository.getDefaultCollector("A", "D");
        BALM::Collector *E = repository.getDefaultCollector("A", "E");
        BALM::Collector *F = repository.getDefaultCollector("A", "F");

        ASSERT(BALM::MetricRecord(
                    A->metricId(), 7 * REPS, 28 * REPS, 1, 7) == recordVal(A));
        ASSERT(BALM::MetricRecord(
                    B->metricId(), 5 * REPS, 20 * REPS, 2, 6) == recordVal(B));
        ASSERT(BALM::MetricRecord(
                    C->metricId(), 4 * REPS, 18 * REPS, 3, 6) == recordVal(C));
        ASSERT(BALM::MetricRecord(
                    D->metricId(), 3 * REPS, 15 * REPS, 4, 6) == recordVal(D));
        ASSERT(BALM::MetricRecord(
                    E->metricId(), 2 * REPS, 11 * REPS, 5, 6) == recordVal(E));
        ASSERT(BALM::MetricRecord(
                    F->metricId(), 1 * REPS,  6 * REPS, 6, 6) == recordVal(F));
    }
    {
        // Verify macros respect enabled status by examining
        // 'ENABLED_CATEGORY' metric values.
        BALM::Collector *A = repository.getDefaultCollector("B", "A");
        BALM::Collector *B = repository.getDefaultCollector("B", "B");
        BALM::Collector *C = repository.getDefaultCollector("B", "C");
        BALM::Collector *D = repository.getDefaultCollector("B", "D");
        BALM::Collector *E = repository.getDefaultCollector("B", "E");
        BALM::Collector *F = repository.getDefaultCollector("B", "F");

        ASSERT(withinDouble(recordVal(A).total(), 0, 28 * REPS));
        ASSERT(withinDouble(recordVal(B).total(), 0, 20 * REPS));
        ASSERT(withinDouble(recordVal(C).total(), 0, 18 * REPS));
        ASSERT(withinDouble(recordVal(D).total(), 0, 15 * REPS));
        ASSERT(withinDouble(recordVal(E).total(), 0, 11 * REPS));
        ASSERT(withinDouble(recordVal(F).total(), 0,  6 * REPS));

        ASSERT(withinDouble(recordVal(A).count(), 0, 7 * REPS));
        ASSERT(withinDouble(recordVal(B).count(), 0, 5 * REPS));
        ASSERT(withinDouble(recordVal(C).count(), 0, 4 * REPS));
        ASSERT(withinDouble(recordVal(D).count(), 0, 3 * REPS));
        ASSERT(withinDouble(recordVal(E).count(), 0, 2 * REPS));
        ASSERT(withinDouble(recordVal(F).count(), 0, 1 * REPS));
    }
}

void StandardMacroConcurrencyTest::runTest()
{
    bsl::function<void()> job = Corp::bdlf::BindUtil::bind(
                                        &StandardMacroConcurrencyTest::execute,
                                         this);

    for (int i = 0; i < d_pool.numThreads(); ++i) {
        d_pool.enqueueJob(job);
    }
    d_pool.drain();
}

// ------------------- case 12: DynamicMacroConcurrencyTest ------------------

class DynamicMacroConcurrencyTest {
    // Invoke a set of operations operations synchronously.

    // DATA
    Corp::bdlmt::FixedThreadPool  d_pool;
    Corp::bslmt::Barrier          d_barrier;
    Corp::bslma::Allocator       *d_allocator_p;

    // PRIVATE MANIPULATORS
    void execute();
        // Execute a single test.

  public:
    // CREATORS
    DynamicMacroConcurrencyTest(int                     numThreads,
                                Corp::bslma::Allocator *basicAllocator)
    : d_pool(numThreads, 1000, basicAllocator)
    , d_barrier(numThreads)
    , d_allocator_p(basicAllocator)
    {
        d_pool.start();
    }

    ~DynamicMacroConcurrencyTest() {}

    // MANIPULATORS
    void runTest();
        // Run the test.
};

void DynamicMacroConcurrencyTest::execute()
{
    // Update 2 metrics 'TEST_CATEGORY.UPDATE' and 'ENABLED_CATEGORY.UPDATE'
    // while enabling and disabling the category 'ENABLED_CATEGORY'.  Verify
    // collectors being used by the macros holds a value that matches the
    // expected value (or expected range of values) for the metric.

    const int   COUNT            = 30;
    const char *TEST_CATEGORY    = "TestCategory";
    const char *ENABLED_CATEGORY = "EnabledCategory";
    const char *UPDATE           = "Update";

    BALM::MetricsManager &mgr = *DefaultManager::instance();
    Repository&           repository = mgr.collectorRepository();
    const Category *C2 = mgr.metricRegistry().getCategory(ENABLED_CATEGORY);

    d_barrier.wait();
    for (int i = 0; i < COUNT; ++i) {
        BALM_METRICS_DYNAMIC_UPDATE(TEST_CATEGORY, UPDATE, i);

        bool enabled = 0 == i % 2;

        mgr.setCategoryEnabled(C2, enabled);

        BALM_METRICS_DYNAMIC_UPDATE(ENABLED_CATEGORY, UPDATE, i);
    }
    d_barrier.wait();
    const int NUM_THREADS = d_pool.numThreads();
    const int TOTAL = (COUNT * (COUNT - 1)) / 2;
    {
        // Verify basic macro behavior for 'TEST_CATEGORY'.

        BALM::Collector *upCol  =
                         repository.getDefaultCollector(TEST_CATEGORY, UPDATE);

        ASSERT(COUNT * NUM_THREADS == recordVal(upCol).count());
        ASSERT(TOTAL * NUM_THREADS == recordVal(upCol).total());

        ASSERT(0 == recordVal(upCol).min());

        ASSERT(COUNT - 1 == recordVal(upCol).max());
    }
    {
        // Verify macros respect enabled status by examining
        // 'ENABLED_CATEGORY' metric values.

        BALM::Collector *upCol  =
                      repository.getDefaultCollector(ENABLED_CATEGORY, UPDATE);

        ASSERT(0 < recordVal(upCol).count() &&
               COUNT * NUM_THREADS > recordVal(upCol).count());

        ASSERT(0 < recordVal(upCol).total() &&
               TOTAL * NUM_THREADS > recordVal(upCol).total());

        ASSERT(0         == recordVal(upCol).min());
        ASSERT(COUNT - 1 >= recordVal(upCol).max());
    }
}

void DynamicMacroConcurrencyTest::runTest()
{
    bsl::function<void()> job = Corp::bdlf::BindUtil::bind(
                                         &DynamicMacroConcurrencyTest::execute,
                                         this);
    for (int i = 0; i < d_pool.numThreads(); ++i) {
        d_pool.enqueueJob(job);
    }
    d_pool.drain();
}

// ------------------- case 13: TlsMacroConcurrencyTest ------------------

#if 0
// The thread local macro has been removed until (hopefully) thread local
// variables are supported on all platforms.

class TlsMacroConcurrencyTest {
    // Invoke a set of operations operations synchronously.

    // DATA
    Corp::bdlmt::FixedThreadPool  d_pool;
    Corp::bslmt::Barrier          d_barrier;
    Corp::bslma::Allocator       *d_allocator_p;
    Corp::bslmt::Mutex            d_mutex;
    // PRIVATE MANIPULATORS
    void execute();
        // Execute a single test.

  public:

    // CREATORS
    TlsMacroConcurrencyTest(int               numThreads,
                            bslma::Allocator *basicAllocator)
    : d_pool(numThreads, 1000, basicAllocator)
    , d_barrier(numThreads)
    , d_allocator_p(basicAllocator)
    {
        d_pool.start();
    }

    ~TlsMacroConcurrencyTest() {}

    //  MANIPULATORS
    void runTest();
        // Run the test.
};

void TlsMacroConcurrencyTest::execute()
{
    // Using 4 metrics 'TEST_CATEGORY.UPDATE', 'TEST_CATEGORY.INCREMENT',
    // 'ENABLED_CATEGORY.UPDATE' and 'ENABLED_CATEGORY.INCREMENT': use the
    // increment macro on the 'INCREMENT' metrics and the update macro on the
    // 'UPDATE' metrics, while enabling and disabling the category
    // 'ENABLED_CATEGORY'.  Verify collectors being used by the macros holds a
    // value that matches the expected value (or expected range of values) for
    // the metric.  Note that this test uses
    // 'BALM::CollectorRepository::collectAndReset', which modifies the
    // metric values as it collects them, so verification can only be performed
    // on a single thread.

    const int   COUNT            = 30;
    const char *TEST_CATEGORY    = "TestCategory";
    const char *ENABLED_CATEGORY = "EnabledCategory";
    const char *INCREMENT        = "Increment";
    const char *UPDATE           = "Update";

    BALM::MetricsManager &mgr = *DefaultManager::instance();
    Repository&           repository = mgr.collectorRepository();
    Registry&             registry   = mgr.metricRegistry();
    bslma::Allocator     *Z = d_allocator_p;
    const Category       *C2 = mgr.metricRegistry().getCategory(
                                                             ENABLED_CATEGORY);

    d_barrier.wait();
    for (int i = 0; i < COUNT; ++i) {
        BALM_METRICS_THREAD_LOCAL_UPDATE(TEST_CATEGORY, UPDATE, i);
        BALM_METRICS_THREAD_LOCAL_INCREMENT(TEST_CATEGORY, INCREMENT);

        bool enabled = 0 == i % 2;

        mgr.setCategoryEnabled(C2, enabled);

        BALM_METRICS_THREAD_LOCAL_UPDATE(ENABLED_CATEGORY, UPDATE, i);
        BALM_METRICS_THREAD_LOCAL_INCREMENT(ENABLED_CATEGORY, INCREMENT);
    }
    d_barrier.wait();

    // Verification must be performed on a single thread because
    // 'BALM::CollectorRepository::collectAndReset' modifies the object
    // state when it collects the metric values.

    static bool executedTest; executedTest = false;
    d_barrier.wait();
    if (0 != d_mutex.tryLock()) {
        d_barrier.wait();
        ASSERT(executedTest);
        return;
    }

    const int NUM_THREADS = d_pool.numThreads();
    const int TOTAL = (COUNT * (COUNT - 1)) / 2;
    {
        // Verify basic macro behavior for 'TEST_CATEGORY'.
        BALM::MetricId upId  = registry.getId(TEST_CATEGORY,
                                             UPDATE);
        BALM::MetricId incId = registry.getId(TEST_CATEGORY,
                                             INCREMENT);

        bsl::vector<BALM::Collector *> cols(Z);
        bsl::vector<BALM::IntegerCollector *> intCols(Z);
        ASSERT(NUM_THREADS == repository.getAddedCollectors(&cols,
                                                            &intCols,
                                                            upId));
        ASSERT(NUM_THREADS == cols.size());
        ASSERT(0           == intCols.size());
        cols.clear(); intCols.clear();

        ASSERT(NUM_THREADS == repository.getAddedCollectors(&cols,
                                                            &intCols,
                                                            incId));
        ASSERT(NUM_THREADS == cols.size());
        ASSERT(0           == intCols.size());

        bsl::vector<BALM::MetricRecord> records(Z);
        repository.collectAndReset(&records, upId.category());
        ASSERT(2 == records.size());
        BALM::MetricRecord upRecord, incRecord;
        if (records[0].metricId() == upId) {
            upRecord = records[0]; incRecord = records[1];
        }
        else {
            BSLS_ASSERT(records[0].metricId() == incId);
            upRecord = records[1]; incRecord = records[0];
        }

        ASSERT(COUNT * NUM_THREADS == upRecord.count());
        ASSERT(COUNT * NUM_THREADS == incRecord.count());

        ASSERT(TOTAL * NUM_THREADS == upRecord.total());
        ASSERT(COUNT * NUM_THREADS == incRecord.total());

        ASSERT(0 == upRecord.min());
        ASSERT(1 == incRecord.min());

        ASSERT(COUNT - 1 == upRecord.max());
        ASSERT(1         == incRecord.max());
    }
    {
        // Verify macros respect enabled status by examining
        // 'ENABLED_CATEGORY' metric values.

        BALM::MetricId upId  = registry.getId(ENABLED_CATEGORY, UPDATE);
        BALM::MetricId incId = registry.getId(ENABLED_CATEGORY, INCREMENT);

        bsl::vector<BALM::Collector *> cols(Z);
        bsl::vector<BALM::IntegerCollector *> intCols(Z);
        ASSERT(NUM_THREADS == repository.getAddedCollectors(&cols,
                                                            &intCols,
                                                            upId));
        ASSERT(NUM_THREADS == cols.size());
        ASSERT(0           == intCols.size());
        cols.clear(); intCols.clear();

        ASSERT(NUM_THREADS == repository.getAddedCollectors(&cols,
                                                            &intCols,
                                                            incId));
        ASSERT(NUM_THREADS == cols.size());
        ASSERT(0           == intCols.size());

        bsl::vector<BALM::MetricRecord> records(Z);
        repository.collectAndReset(&records, upId.category());
        ASSERT(2 == records.size());
        BALM::MetricRecord upRecord, incRecord;
        if (records[0].metricId() == upId) {
            upRecord = records[0]; incRecord = records[1];
        }
        else {
            BSLS_ASSERT(records[0].metricId() == incId);
            upRecord = records[1]; incRecord = records[0];
        }

        ASSERT(0 < upRecord.count() &&
               COUNT * NUM_THREADS > upRecord.count());
        ASSERT(0 < incRecord.count() &&
               COUNT * NUM_THREADS > incRecord.count());

        ASSERT(0 < upRecord.total() &&
               TOTAL * NUM_THREADS > upRecord.total());
        ASSERT(0 < incRecord.total() &&
               COUNT * NUM_THREADS > incRecord.total());

        ASSERT(0 == upRecord.min());
        ASSERT(1 == incRecord.min());

        ASSERT(COUNT - 1 >= upRecord.max());
        ASSERT(1         == incRecord.max());
    }

    executedTest = true;
    d_barrier.wait();
    d_mutex.unlock();
}

void TlsMacroConcurrencyTest::runTest()
{
    bsl::function<void()> job = Corp::bdlf::BindUtil::bind(
                                             &TlsMacroConcurrencyTest::execute,
                                             this);
    for (int i = 0; i < d_pool.numThreads(); ++i) {
        d_pool.enqueueJob(job);
    }
    d_pool.drain();
}
#endif

// ------------------- case 14: StandardIntMacroConcurrencyTest ---------------

class StandardIntMacroConcurrencyTest {
    // Invoke a set of operations operations synchronously.

    // DATA
    Corp::bdlmt::FixedThreadPool   d_pool;
    Corp::bslmt::Barrier           d_barrier;
    Corp::bslma::Allocator        *d_allocator_p;

    // PRIVATE MANIPULATORS
    void execute();
        // Execute a single test.

  public:

    // CREATORS
    StandardIntMacroConcurrencyTest(int                     numThreads,
                                    Corp::bslma::Allocator *basicAllocator)
    : d_pool(numThreads, 1000, basicAllocator)
    , d_barrier(numThreads)
    , d_allocator_p(basicAllocator)
    {
        d_pool.start();
    }

    ~StandardIntMacroConcurrencyTest() {}

    // MANIPULATORS
    void runTest();
        // Run the test.
};

void StandardIntMacroConcurrencyTest::execute()
{
    // Update 2 metrics in two categories ("A" and "B")  while enabling and
    // disabling the category "B".  Verify collectors being used by the macros
    // holds a value that matches the expected value (or expected range of
    // values) for the metric.

    const int   COUNT            = 30;
    BALM::MetricsManager &mgr = *DefaultManager::instance();
    Repository&   repository = mgr.collectorRepository();
    d_barrier.wait();
    for (int i = 0; i < COUNT; ++i) {
        BALM_METRICS_INCREMENT("A", "A");
        BALM_METRICS_INT_UPDATE("A",
                                "A", 1);
        BALM_METRICS_INT_UPDATE2("A",
                                 "A", 2, "B", 2);
        BALM_METRICS_INT_UPDATE3("A",
                                 "A", 3, "B", 3, "C", 3);
        BALM_METRICS_INT_UPDATE4("A",
                                 "A", 4, "B", 4, "C", 4, "D", 4);
        BALM_METRICS_INT_UPDATE5("A",
                                 "A", 5, "B", 5, "C", 5, "D", 5,
                                 "E", 5);
        BALM_METRICS_INT_UPDATE6("A",
                                 "A", 6, "B", 6, "C", 6, "D", 6,
                                 "E", 6, "F", 6);
        BALM_METRICS_TYPED_INT_UPDATE("A", "A", 7, Type::e_TOTAL);
        BALM_METRICS_TYPED_INCREMENT("A", "A", Type::e_TOTAL);

        bool enabled = 0 == i % 2;

        mgr.setCategoryEnabled("B", enabled);

        BALM_METRICS_INCREMENT("B", "A");
        BALM_METRICS_INT_UPDATE("B",
                                "A", 1);
        BALM_METRICS_INT_UPDATE2("B",
                                 "A", 2, "B", 2);
        BALM_METRICS_INT_UPDATE3("B",
                                 "A", 3, "B", 3, "C", 3);
        BALM_METRICS_INT_UPDATE4("B",
                                 "A", 4, "B", 4, "C", 4, "D", 4);
        BALM_METRICS_INT_UPDATE5("B",
                                 "A", 5, "B", 5, "C", 5, "D", 5,
                                 "E", 5);
        BALM_METRICS_INT_UPDATE6("B",
                                 "A", 6, "B", 6, "C", 6, "D", 6,
                                 "E", 6, "F", 6);
        BALM_METRICS_TYPED_INT_UPDATE("B", "A", 7, Type::e_TOTAL);
        BALM_METRICS_TYPED_INCREMENT("B", "A", Type::e_TOTAL);
    }
    d_barrier.wait();

    const int REPS = COUNT * d_pool.numThreads();
    {
        // Verify basic macro behavior for category A

        IntCollector *A = repository.getDefaultIntegerCollector("A", "A");
        IntCollector *B = repository.getDefaultIntegerCollector("A", "B");
        IntCollector *C = repository.getDefaultIntegerCollector("A", "C");
        IntCollector *D = repository.getDefaultIntegerCollector("A", "D");
        IntCollector *E = repository.getDefaultIntegerCollector("A", "E");
        IntCollector *F = repository.getDefaultIntegerCollector("A", "F");

        ASSERT(BALM::MetricRecord(
                    A->metricId(), 9 * REPS, 30 * REPS, 1, 7) == recordVal(A));
        ASSERT(BALM::MetricRecord(
                    B->metricId(), 5 * REPS, 20 * REPS, 2, 6) == recordVal(B));
        ASSERT(BALM::MetricRecord(
                    C->metricId(), 4 * REPS, 18 * REPS, 3, 6) == recordVal(C));
        ASSERT(BALM::MetricRecord(
                    D->metricId(), 3 * REPS, 15 * REPS, 4, 6) == recordVal(D));
        ASSERT(BALM::MetricRecord(
                    E->metricId(), 2 * REPS, 11 * REPS, 5, 6) == recordVal(E));
        ASSERT(BALM::MetricRecord(
                    F->metricId(), 1 * REPS,  6 * REPS, 6, 6) == recordVal(F));
    }
    {
        // Verify macros respect enabled status by examining
        // 'ENABLED_CATEGORY' metric values.
        IntCollector *A = repository.getDefaultIntegerCollector("B", "A");
        IntCollector *B = repository.getDefaultIntegerCollector("B", "B");
        IntCollector *C = repository.getDefaultIntegerCollector("B", "C");
        IntCollector *D = repository.getDefaultIntegerCollector("B", "D");
        IntCollector *E = repository.getDefaultIntegerCollector("B", "E");
        IntCollector *F = repository.getDefaultIntegerCollector("B", "F");

        ASSERT(withinInt(recordVal(A).total(), 0, 30 * REPS));
        ASSERT(withinInt(recordVal(B).total(), 0, 20 * REPS));
        ASSERT(withinInt(recordVal(C).total(), 0, 18 * REPS));
        ASSERT(withinInt(recordVal(D).total(), 0, 15 * REPS));
        ASSERT(withinInt(recordVal(E).total(), 0, 11 * REPS));
        ASSERT(withinInt(recordVal(F).total(), 0,  6 * REPS));

        ASSERT(withinInt(recordVal(A).count(), 0, 9 * REPS));
        ASSERT(withinInt(recordVal(B).count(), 0, 5 * REPS));
        ASSERT(withinInt(recordVal(C).count(), 0, 4 * REPS));
        ASSERT(withinInt(recordVal(D).count(), 0, 3 * REPS));
        ASSERT(withinInt(recordVal(E).count(), 0, 2 * REPS));
        ASSERT(withinInt(recordVal(F).count(), 0, 1 * REPS));
    }
}

void StandardIntMacroConcurrencyTest::runTest()
{
    bsl::function<void()> job = Corp::bdlf::BindUtil::bind(
                                     &StandardIntMacroConcurrencyTest::execute,
                                     this);
    for (int i = 0; i < d_pool.numThreads(); ++i) {
        d_pool.enqueueJob(job);
    }
    d_pool.drain();
}

// ------------------- case 15: DynamicIntMacroConcurrencyTest ----------------

class DynamicIntMacroConcurrencyTest {
    // Invoke a set of operations operations synchronously.

    // DATA
    Corp::bdlmt::FixedThreadPool  d_pool;
    Corp::bslmt::Barrier          d_barrier;
    Corp::bslma::Allocator       *d_allocator_p;

    // PRIVATE MANIPULATORS
    void execute();
        // Execute a single test.

  public:
    // CREATORS
    DynamicIntMacroConcurrencyTest(int                     numThreads,
                                   Corp::bslma::Allocator *basicAllocator)
    : d_pool(numThreads, 1000, basicAllocator)
    , d_barrier(numThreads)
    , d_allocator_p(basicAllocator)
    {
        d_pool.start();
    }

    ~DynamicIntMacroConcurrencyTest() {}

    // MANIPULATORS
    void runTest();
        // Run the test.
};

void DynamicIntMacroConcurrencyTest::execute()
{
    // Using 4 metrics 'TEST_CATEGORY.UPDATE', 'TEST_CATEGORY.INCREMENT',
    // 'ENABLED_CATEGORY.UPDATE' and 'ENABLED_CATEGORY.INCREMENT': use the
    // increment macro on the 'INCREMENT' metrics and the update macro on the
    // 'UPDATE' metrics, while enabling and disabling the category
    // 'ENABLED_CATEGORY'.  Verify collectors being used by the macros holds a
    // value that matches the expected value (or expected range of values) for
    // the metric.

    const int   COUNT            = 30;
    const char *TEST_CATEGORY    = "TestCategory";
    const char *ENABLED_CATEGORY = "EnabledCategory";
    const char *INCREMENT        = "Increment";
    const char *UPDATE           = "Update";

    BALM::MetricsManager &mgr = *DefaultManager::instance();
    Repository&   repository = mgr.collectorRepository();
    const Category *C2 = mgr.metricRegistry().getCategory(ENABLED_CATEGORY);

    d_barrier.wait();
    for (int i = 0; i < COUNT; ++i) {
        BALM_METRICS_DYNAMIC_INT_UPDATE(TEST_CATEGORY, UPDATE, i);
        BALM_METRICS_DYNAMIC_INCREMENT(TEST_CATEGORY, INCREMENT);

        bool enabled = 0 == i % 2;

        mgr.setCategoryEnabled(C2, enabled);

        BALM_METRICS_DYNAMIC_INT_UPDATE(ENABLED_CATEGORY, UPDATE, i);
        BALM_METRICS_DYNAMIC_INCREMENT(ENABLED_CATEGORY, INCREMENT);
    }
    d_barrier.wait();
    const int NUM_THREADS = d_pool.numThreads();
    const int TOTAL = (COUNT * (COUNT - 1)) / 2;
    {
        // Verify basic macro behavior for 'TEST_CATEGORY'.

        IntCollector *upCol  = repository.getDefaultIntegerCollector(
                                                        TEST_CATEGORY, UPDATE);
        IntCollector *incCol  = repository.getDefaultIntegerCollector(
                                                     TEST_CATEGORY, INCREMENT);

        ASSERT(COUNT * NUM_THREADS == recordVal(upCol).count());
        ASSERT(COUNT * NUM_THREADS == recordVal(incCol).count());

        ASSERT(TOTAL * NUM_THREADS == recordVal(upCol).total());
        ASSERT(COUNT * NUM_THREADS == recordVal(incCol).total());

        ASSERT(0 == recordVal(upCol).min());
        ASSERT(1 == recordVal(incCol).min());

        ASSERT(COUNT - 1 == recordVal(upCol).max());
        ASSERT(1         == recordVal(incCol).max());
    }
    {
        // Verify macros respect enabled status by examining
        // 'ENABLED_CATEGORY' metric values.

        IntCollector *upCol  = repository.getDefaultIntegerCollector(
                                                     ENABLED_CATEGORY, UPDATE);
        IntCollector *incCol  = repository.getDefaultIntegerCollector(
                                                  ENABLED_CATEGORY, INCREMENT);

        ASSERT(0 < recordVal(upCol).count() &&
               COUNT * NUM_THREADS > recordVal(upCol).count());
        ASSERT(0 < recordVal(incCol).count() &&
               COUNT * NUM_THREADS > recordVal(incCol).count());

        ASSERT(0 < recordVal(upCol).total() &&
               TOTAL * NUM_THREADS > recordVal(upCol).total());
        ASSERT(0 < recordVal(incCol).total() &&
               COUNT * NUM_THREADS > recordVal(incCol).total());

        ASSERT(0 == recordVal(upCol).min());
        ASSERT(1 == recordVal(incCol).min());

        ASSERT(COUNT - 1 >= recordVal(upCol).max());
        ASSERT(1         == recordVal(incCol).max());
    }
}

void DynamicIntMacroConcurrencyTest::runTest()
{
    bsl::function<void()> job = Corp::bdlf::BindUtil::bind(
                                      &DynamicIntMacroConcurrencyTest::execute,
                                      this);
    for (int i = 0; i < d_pool.numThreads(); ++i) {
        d_pool.enqueueJob(job);
    }
    d_pool.drain();
}

// ------------------- case 16: TlsIntMacroConcurrencyTest ------------------
#if 0
// The thread local macro has been removed until (hopefully) thread local
// variables are supported on all platforms.

class TlsIntMacroConcurrencyTest {
    // Invoke a set of operations operations synchronously.

    // DATA
    bdlmt::FixedThreadPool  d_pool;
    bslmt::Barrier          d_barrier;
    bslma::Allocator       *d_allocator_p;
    bslmt::Mutex            d_mutex;
    // PRIVATE MANIPULATORS
    void execute();
        // Execute a single test.

  public:

    // CREATORS
    TlsIntMacroConcurrencyTest(int               numThreads,
                               bslma::Allocator *basicAllocator)
    : d_pool(numThreads, 1000, basicAllocator)
    , d_barrier(numThreads)
    , d_allocator_p(basicAllocator)
    {
        d_pool.start();
    }

    ~TlsIntMacroConcurrencyTest() {}

    //  MANIPULATORS
    void runTest();
        // Run the test.
};

void TlsIntMacroConcurrencyTest::execute()
{
    // Using 4 metrics 'TEST_CATEGORY.UPDATE', 'TEST_CATEGORY.INCREMENT',
    // 'ENABLED_CATEGORY.UPDATE' and 'ENABLED_CATEGORY.INCREMENT': use the
    // increment macro on the 'INCREMENT' metrics and the update macro on the
    // 'UPDATE' metrics, while enabling and disabling the category
    // 'ENABLED_CATEGORY'.  Verify collectors being used by the macros holds a
    // value that matches the expected value (or expected range of values) for
    // the metric.  Note that this test uses
    // 'BALM::CollectorRepository::collectAndReset', which modifies the
    // metric values as it collects them, so verification can only be performed
    // on a single thread.

    const int   COUNT            = 30;
    const char *TEST_CATEGORY    = "TestCategory";
    const char *ENABLED_CATEGORY = "EnabledCategory";
    const char *INCREMENT        = "Increment";
    const char *UPDATE           = "Update";

    BALM::MetricsManager &mgr = *DefaultManager::instance();
    Repository& repository = mgr.collectorRepository();
    Registry&   registry   = mgr.metricRegistry();
    Corp::bslma::Allocator *Z = d_allocator_p;
    const Category *C2 = mgr.metricRegistry().getCategory(ENABLED_CATEGORY);

    d_barrier.wait();
    for (int i = 0; i < COUNT; ++i) {
        BALM_UPDATE_THREAD_LOCAL_INT_METRIC(TEST_CATEGORY, UPDATE, i);
        BALM_INCREMENT_THREAD_LOCAL_INT_METRIC(TEST_CATEGORY, INCREMENT);

        bool enabled = 0 == i % 2;

        mgr.setCategoryEnabled(C2, enabled);

        BALM_UPDATE_THREAD_LOCAL_INT_METRIC(ENABLED_CATEGORY, UPDATE, i);
        BALM_INCREMENT_THREAD_LOCAL_INT_METRIC(ENABLED_CATEGORY, INCREMENT);
    }
    d_barrier.wait();

    // Verification must be performed on a single thread because
    // 'BALM::CollectorRepository::collectAndReset' modifies the object
    // state when it collects the metric values.

    static bool executedTest; executedTest = false;
    d_barrier.wait();
    if (0 != d_mutex.tryLock()) {
        d_barrier.wait();
        ASSERT(executedTest);
        return;
    }

    const int NUM_THREADS = d_pool.numThreads();
    const int TOTAL = (COUNT * (COUNT - 1)) / 2;
    {
        // Verify basic macro behavior for 'TEST_CATEGORY'.
        BALM::MetricId upId  = registry.getId(TEST_CATEGORY,
                                             UPDATE);
        BALM::MetricId incId = registry.getId(TEST_CATEGORY,
                                             INCREMENT);

        ColSPtrVector cols(Z);
        IColSPtrVector intCols(Z);
        ASSERT(NUM_THREADS == repository.getAddedCollectors(&cols,
                                                            &intCols,
                                                            upId));
        ASSERT(0           == cols.size());
        ASSERT(NUM_THREADS == intCols.size());
        cols.clear(); intCols.clear();

        ASSERT(NUM_THREADS == repository.getAddedCollectors(&cols,
                                                            &intCols,
                                                            incId));
        ASSERT(0           == cols.size());
        ASSERT(NUM_THREADS == intCols.size());

        bsl::vector<BALM::MetricRecord> records(Z);
        repository.collectAndReset(&records, upId.category());
        ASSERT(2 == records.size());
        BALM::MetricRecord upRecord, incRecord;
        if (records[0].metricId() == upId) {
            upRecord = records[0]; incRecord = records[1];
        }
        else {
            BSLS_ASSERT(records[0].metricId() == incId);
            upRecord = records[1]; incRecord = records[0];
        }

        ASSERT(COUNT * NUM_THREADS == upRecord.count());
        ASSERT(COUNT * NUM_THREADS == incRecord.count());

        ASSERT(TOTAL * NUM_THREADS == upRecord.total());
        ASSERT(COUNT * NUM_THREADS == incRecord.total());

        ASSERT(0 == upRecord.min());
        ASSERT(1 == incRecord.min());

        ASSERT(COUNT - 1 == upRecord.max());
        ASSERT(1         == incRecord.max());
    }
    {
        // Verify macros respect enabled status by examining
        // 'ENABLED_CATEGORY' metric values.

        BALM::MetricId upId  = registry.getId(ENABLED_CATEGORY, UPDATE);
        BALM::MetricId incId = registry.getId(ENABLED_CATEGORY, INCREMENT);

        ColSPtrVector cols(Z);
        IColSPtrVector intCols(Z);
        ASSERT(NUM_THREADS == repository.getAddedCollectors(&cols,
                                                            &intCols,
                                                            upId));
        ASSERT(0           == cols.size());
        ASSERT(NUM_THREADS == intCols.size());
        cols.clear(); intCols.clear();

        ASSERT(NUM_THREADS == repository.getAddedCollectors(&cols,
                                                            &intCols,
                                                            incId));
        ASSERT(0           == cols.size());
        ASSERT(NUM_THREADS == intCols.size());

        bsl::vector<BALM::MetricRecord> records(Z);
        repository.collectAndReset(&records, upId.category());
        ASSERT(2 == records.size());
        BALM::MetricRecord upRecord, incRecord;
        if (records[0].metricId() == upId) {
            upRecord = records[0]; incRecord = records[1];
        }
        else {
            BSLS_ASSERT(records[0].metricId() == incId);
            upRecord = records[1]; incRecord = records[0];
        }

        ASSERT(0 < upRecord.count() &&
               COUNT * NUM_THREADS > upRecord.count());
        ASSERT(0 < incRecord.count() &&
               COUNT * NUM_THREADS > incRecord.count());

        ASSERT(0 < upRecord.total() &&
               TOTAL * NUM_THREADS > upRecord.total());
        ASSERT(0 < incRecord.total() &&
               COUNT * NUM_THREADS > incRecord.total());

        ASSERT(0 == upRecord.min());
        ASSERT(1 == incRecord.min());

        ASSERT(COUNT - 1 >= upRecord.max());
        ASSERT(1         == incRecord.max());
    }

    executedTest = true;
    d_barrier.wait();
    d_mutex.unlock();
}

void TlsIntMacroConcurrencyTest::runTest()
{
    bsl::function<void()> job = Corp::bdlf::BindUtil::bind(
                                          &TlsIntMacroConcurrencyTest::execute,
                                          this);
    for (int i = 0; i < d_pool.numThreads(); ++i) {
        d_pool.enqueueJob(job);
    }
    d_pool.drain();
}

#endif

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Example 2 - Updating a Metric
///- - - - - - - - - - - - - - -
// Once a metrics manager is initialized, we can use the various macros to
// record metric values.  In this second example, we collect metrics from a
// hypothetical event-processing function.  We use 'BALM_METRICS_UPDATE' to
// record the size of the data being processed to a metric named
// "msgSize", and the elapsed time (in milliseconds) to process the event to a
// metric named "elapsedTime".  Finally, we use 'BALM_METRICS_INCREMENT'
// to record a count of failures to a metric named "failureCount".
// Note that we do not use the '*_DYNAMIC_*' variants of the
// 'BALM_METRICS_UPDATE' or 'BALM_METRICS_INCREMENT' macros because the
// category and metric names are constant across all applications of the macro
// at a particular instantiation point (the 'DYNAMIC' variants look up the
// category and metric name on each application, which would incur unnecessary
// runtime overhead).
//..
    int processEvent(int eventId, const bsl::string& eventMessage)
        // Process the event described by the specified 'eventId' and
        // 'eventMessage'.  Return 0 on success, and a non-zero
        // value otherwise.
    {
        int returnCode = 0;

        BALM_METRICS_UPDATE("processEvent", "msgSize", eventMessage.size());
        BALM_METRICS_TIME_BLOCK_MILLISECONDS("processingEvent", "elapsedTime");

        // Process 'data'.

        if (0 != returnCode) {
            BALM_METRICS_INCREMENT("processEvent", "failureCount");
        }

        return returnCode;
    }
//..
///Example 3 - Using 'BALM_METRICS_IF_CATEGORY_ENABLED'
/// - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this next example, we use 'BALM_METRICS_IF_CATEGORY_ENABLED' to
// conditionally disable a (relatively) expensive operation involved in
// computing a metric value.  The 'processEvent2' function, defined below, uses
// a 'bsls::Stopwatch' to record the elapsed system, user, and wall time,
// involved in processing the event.  The system calls used (via
// 'bsls::Stopwatch') to record the elapsed time may be relatively expensive,
// so we use 'BALM_METRICS_IF_CATEGORY_ENABLED' to ensure we only perform
// those operations if metrics collection is enabled.  Finally, we use
// 'BALM_METRICS_UPDATE3' to update the three metrics, this is (slightly) more
// efficient than updating each metric individually using 'BALM_METRIC_UPDATE'.
//..
    int processEvent2(int eventId, const bsl::string& eventMessage)
        // Process the event described by the specified 'eventId' and
        // 'eventMessage'.  Return 0 on success, and a non-zero
        // value otherwise.
    {
        int returnCode = 0;

        Corp::bsls::Stopwatch stopwatch;
        BALM_METRICS_IF_CATEGORY_ENABLED("processEvent2") {
            stopwatch.start(true);
        }

        // Process 'data'.

        BALM_METRICS_IF_CATEGORY_ENABLED("processEvent2") {
            double systemTime, userTime, wallTime;
            stopwatch.accumulatedTimes(&systemTime, &userTime, &wallTime);
            BALM_METRICS_UPDATE3("processEvent2",
                                 "systemTime", systemTime,
                                 "userTime",   userTime,
                                 "wallTime",   wallTime);
        }

        return returnCode;
    }
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

    Corp::bslma::TestAllocator testAlloc;
    Corp::bslma::TestAllocator *Z = &testAlloc;
    Corp::bslma::TestAllocator defaultAllocator;
    Corp::bslma::DefaultAllocatorGuard guard(&defaultAllocator);

    switch (test) { case 0:  // Zero is always the leading case.
      case 19: {
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

        using namespace BloombergLP;

///Usage
///-----
// The following examples demonstrate how to configure, collect, and publish
// metrics.
//
///Example 1 - Create and access the default 'balm::MetricsManager' instance
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates how to create the default 'balm::MetricManager'
// instance and perform a trivial configuration.
//
// Create a 'balm::DefaultMetricsManagerScopedGuard' to manage the lifetime of
// the default metrics manager instance, and provide it a stream ('stdout')
// that we want to publish metrics to.  Note that the default metrics
// manager is intended to be created and released by the *owner* of  'main'.
// The instance should be created during the initialization of an
// application (while the task has a single thread) and released just prior to
// termination (when there is similarly a single thread).
//..
//  int main(int argc, char *argv[])
    {
    // ...

        balm::DefaultMetricsManagerScopedGuard managerGuard(bsl::cout);
//..
// Once the default instance has been created, it can be accessed using the
// 'instance' operation
//..
       balm::MetricsManager *manager = balm::DefaultMetricsManager::instance();
       ASSERT(0 != manager);
//..
// Note that the default metrics manager will be released when the
// 'managerGuard' exits this scoped and is destroyed.  Clients that choose to
// explicitly call 'balm::DefaultMetricsManager::create()' must also explicitly
// call 'balm::DefaultMetricsManager::release()'.

    }
    } break;
      case 18: {
        // --------------------------------------------------------------------
        // Testing:
        //    MACROS using 'logEmptyName'.
        //
        // Concerns:
        //    That all macros using 'logEmptyName' is producing a warning
        //    message if the 'CATEGORY' or 'METRIC' parameters are empty
        //    strings.
        //
        // Plan:
        //    Invoke 'BALM_METRICS_UPDATE' with an empty string as parameter
        //    and verify that the expected warning message is logged.  Also
        //    verify that a warning message is not issued if all the parameters
        //    are non-empty.  Invoke 'BALM_METRICS_UPDATEn' with an empty
        //    string for parameter 'METRICn' and verify that a warning message
        //    is logged.  Invoke 'BALM_METRICS_INT_UPDATE' with non-empty
        //    parameters and verify that no warning is issued.  Invoke
        //    'BALM_METRICS_INT_UPDATE' again with empty parameters, and verify
        //    that a warning message is logged.  Perform the same test on
        //    'BALM_METRICS_INT_UPDATEn' as with 'BALM_METRICS_UPDATE'.
        //
        // Testing:
        //    BALM_METRICS_UPDATE(CATEGORY, NAME, VALUE)
        //    BALM_METRICS_UPDATE2(CATEGORY, METRIC1, VALUE1, ...)
        //    BALM_METRICS_UPDATE3(CATEGORY, METRIC1, VALUE1, ...)
        //    BALM_METRICS_UPDATE4(CATEGORY, METRIC1, VALUE1, ...)
        //    BALM_METRICS_UPDATE5(CATEGORY, METRIC1, VALUE1, ...)
        //    BALM_METRICS_UPDATE6(CATEGORY, METRIC1, VALUE1, ...)
        //    BALM_METRICS_INT_UPDATE(CATEGORY, NAME, VALUE
        //    BALM_METRICS_INT_UPDATE2(CATEGORY, METRIC1, VALUE1, ...)
        //    BALM_METRICS_INT_UPDATE3(CATEGORY, METRIC1, VALUE1, ...)
        //    BALM_METRICS_INT_UPDATE4(CATEGORY, METRIC1, VALUE1, ...)
        //    BALM_METRICS_INT_UPDATE5(CATEGORY, METRIC1, VALUE1, ...)
        //    BALM_METRICS_INT_UPDATE6(CATEGORY, METRIC1, VALUE1, ...)
        //
        // --------------------------------------------------------------------

        MessageObserver observer;
        Corp::ball::LoggerManagerConfiguration configuration;
        Corp::ball::LoggerManager& manager =
            Corp::ball::LoggerManager::initSingleton(&observer, configuration);

        manager.setDefaultThresholdLevels(Corp::ball::Severity::e_OFF,
                                          Corp::ball::Severity::e_WARN,
                                          Corp::ball::Severity::e_OFF,
                                          Corp::ball::Severity::e_OFF);

        if (verbose) cout << endl <<
            "Test BALM_METRICS_UPDATE warning messages" << endl;

        Corp::bslma::TestAllocator defaultAllocator;
        Corp::bslma::DefaultAllocatorGuard guard(&defaultAllocator);
        Corp::bslma::TestAllocator testAllocator;

        BALM::DefaultMetricsManagerScopedGuard scopedGuard(&testAllocator);
        BALM::MetricsManager& mgr = *DefaultManager::instance();
        Registry&   registry   = mgr.metricRegistry();
        Repository& repository = mgr.collectorRepository();

        observer.clearMessage();

        // Get the line number of the next line.  Must be followed by the
        // BALM_METRICS_UPDATE for the ASSERT to work correctly.

        const int line_number = L_ + 1;
        BALM_METRICS_UPDATE(" ", "A", 0.0);

        // Construct expected warning message.

        bsl::stringstream message;
        message << "Empty category \" \" added at " << __FILE__ << ":"
            << line_number;

        ASSERT(observer.getLastMessage() == message.str());

        // Just check whether a warning message is created for the rest of the
        // test case.

        observer.clearMessage();
        BALM_METRICS_UPDATE("A", "B", 0.0);
        ASSERT(observer.getLastMessage() == "");

        observer.clearMessage();
        BALM_METRICS_UPDATE("", "B", 0.0);
        ASSERT(observer.getLastMessage() != "");

        observer.clearMessage();
        BALM_METRICS_UPDATE("A", "", 0.0);
        ASSERT(observer.getLastMessage() != "");

        observer.clearMessage();
        BALM_METRICS_UPDATE2("A", "B", 0.0, "", 0.0);
        ASSERT(observer.getLastMessage() != "");

        observer.clearMessage();
        BALM_METRICS_UPDATE3("A", "B", 0.0, "C", 0.0, "", 0.0);
        ASSERT(observer.getLastMessage() != "");

        observer.clearMessage();
        BALM_METRICS_UPDATE4("A", "B", 0.0, "C", 0.0, "D", 0.0, "", 0.0);
        ASSERT(observer.getLastMessage() != "");

        observer.clearMessage();
        BALM_METRICS_UPDATE5("A", "B", 0.0, "C", 0.0, "D", 0.0, "E", 0.0,
                "", 0.0);
        ASSERT(observer.getLastMessage() != "");

        observer.clearMessage();
        BALM_METRICS_UPDATE6("A", "B", 0.0, "C", 0.0, "D", 0.0, "E", 0.0,
                "F", 0.0, "", 0.0);
        ASSERT(observer.getLastMessage() != "");

        // Test BALM_METRICS_INT_UPDATE

        if (verbose) cout << endl <<
            "Test BALM_METRICS_INT_UPDATE warning messages" << endl;

        observer.clearMessage();
        BALM_METRICS_INT_UPDATE("A", "B", 0.0);
        ASSERT(observer.getLastMessage() == "");

        observer.clearMessage();
        BALM_METRICS_INT_UPDATE("", "B", 0.0);
        ASSERT(observer.getLastMessage() != "");

        observer.clearMessage();
        BALM_METRICS_INT_UPDATE("A", "", 0.0);
        ASSERT(observer.getLastMessage() != "");

        observer.clearMessage();
        BALM_METRICS_INT_UPDATE2("A", "B", 0.0, "", 0.0);
        ASSERT(observer.getLastMessage() != "");

        observer.clearMessage();
        BALM_METRICS_INT_UPDATE3("A", "B", 0.0, "C", 0.0, "", 0.0);
        ASSERT(observer.getLastMessage() != "");

        observer.clearMessage();
        BALM_METRICS_INT_UPDATE4("A", "B", 0.0, "C", 0.0, "D", 0.0, "", 0.0);
        ASSERT(observer.getLastMessage() != "");

        observer.clearMessage();
        BALM_METRICS_INT_UPDATE5("A", "B", 0.0, "C", 0.0, "D", 0.0, "E", 0.0,
                "", 0.0);
        ASSERT(observer.getLastMessage() != "");

        observer.clearMessage();
        BALM_METRICS_INT_UPDATE6("A", "B", 0.0, "C", 0.0, "D", 0.0, "E", 0.0,
                "F", 0.0, "", 0.0);
        ASSERT(observer.getLastMessage() != "");
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // Testing:
        //    Warning messages created by 'logEmptyName'.
        //
        // Concerns:
        //    That 'logEmptyName' is detecting an empty name correctly and
        //    logging the correct warning message.
        //
        // Plan:
        //    Specify a set of objects containing the name, type and whether or
        //    not a warning is expected.  For each object in the set, invoke
        //   'logEmptyName' against the object and verify that expected the
        //   warning message is received.
        //
        // Testing:
        //    void balm::Metrics_Helper::logEmptyName(NameType    type,
        //                                           const char *name,
        //                                           const char *file,
        //                                           int         line);
        //
        // --------------------------------------------------------------------

        MessageObserver observer;
        Corp::ball::LoggerManagerConfiguration configuration;
        Corp::ball::LoggerManager& manager =
            Corp::ball::LoggerManager::initSingleton(&observer, configuration);

        manager.setDefaultThresholdLevels(Corp::ball::Severity::e_OFF,
                                          Corp::ball::Severity::e_WARN,
                                          Corp::ball::Severity::e_OFF,
                                          Corp::ball::Severity::e_OFF);

        if (verbose) cout << endl
            << "Test logEmptyName" << endl
            << "=================" << endl;


        struct {
            const char                           *d_name;
            const BALM::Metrics_Helper::NameType  d_type;
            const bool                            d_expect_warning;
        } NAME_TEST[] = {
            {"",   BALM::Metrics_Helper::e_TYPE_CATEGORY, true},
            {" ",  BALM::Metrics_Helper::e_TYPE_METRIC,   true},
            {"  ", BALM::Metrics_Helper::e_TYPE_METRIC,   true},
            {"A",  BALM::Metrics_Helper::e_TYPE_METRIC,   false}
        };

        const int NUM_NAME = sizeof(NAME_TEST)/sizeof(*NAME_TEST);
        static const char *type_string[] = {
            "category",
            "metric",
        };

        for (int i = 0; i < NUM_NAME; ++i) {

            typedef BALM::Metrics_Helper Helper;
            const int line_number = L_;
            observer.clearMessage();
            Helper::logEmptyName(NAME_TEST[i].d_name, NAME_TEST[i].d_type,
                            __FILE__, line_number);
            if (NAME_TEST[i].d_expect_warning) {

                // Construct expected warning message.

                bsl::stringstream message;
                message << "Empty " << type_string[NAME_TEST[i].d_type]
                    << " \"" << NAME_TEST[i].d_name <<  "\" added at "
                    << __FILE__ << ":" << line_number;

                ASSERT(observer.getLastMessage() == message.str());
            } else {
                ASSERT(observer.getLastMessage() == "");
            }
        }

    } break;
      case 16: {
#if 0
// The thread local macro has been removed until (hopefully) thread local
// variables are supported on all platforms.

        // --------------------------------------------------------------------
        // CONCURRENCY TEST: 'THREAD_LOCAL' MACROS
        //
        // Testing:
        //     Thread-safety of thread-local macros
        //
        //     Note that each thread executing a thread-local macro should
        //     have statically cache a different balm::IntegerCollector object.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TEST CONCURRENCY: THREAD_LOCAL INT MACROS\n"
                          << "=========================================\n";

        typedef TlsIntMacroConcurrencyTest TestClass;

        Corp::bslma::TestAllocator defaultAllocator;
        Corp::bslma::DefaultAllocatorGuard guard(&defaultAllocator);
        Corp::bslma::TestAllocator testAllocator;

        BALM::DefaultMetricsManagerScopedGuard scopedGuard(&testAllocator);
        BALM::MetricsManager &mgr = *DefaultManager::instance();
        Repository& repository = mgr.collectorRepository();
        Registry&   registry   = mgr.metricRegistry();
        const int NUM_THREADS = 10;
        {

            TestClass tester(NUM_THREADS, &testAllocator);
            tester.runTest();
        }
#endif
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // CONCURRENCY TEST: 'DYNAMIC' MACROS
        //
        // Testing:
        //     Thread-safety of dynamic macros
        //
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TEST CONCURRENCY: DYNAMIC INT MACROS" << endl
                          << "====================================" << endl;

        typedef DynamicIntMacroConcurrencyTest TestClass;

        Corp::bslma::TestAllocator defaultAllocator;
        Corp::bslma::DefaultAllocatorGuard guard(&defaultAllocator);
        Corp::bslma::TestAllocator testAllocator;

        BALM::DefaultMetricsManagerScopedGuard scopedGuard(&testAllocator);
        BALM::MetricsManager &mgr = *DefaultManager::instance();
        Repository& repository = mgr.collectorRepository();

        const int NUM_THREADS = 10;
        {

            TestClass tester(NUM_THREADS, &testAllocator);
            tester.runTest();
        }

      } break;
      case 14: {
        // --------------------------------------------------------------------
        // CONCURRENCY TEST: STANDARD MACROS
        //
        // Testing:
        //     Thread-safety of standard macros
        //
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TEST CONCURRENCY: STANDARD INT MACROS" << endl
                          << "=====================================" << endl;

        typedef StandardIntMacroConcurrencyTest TestClass;

        Corp::bslma::TestAllocator defaultAllocator;
        Corp::bslma::DefaultAllocatorGuard guard(&defaultAllocator);
        Corp::bslma::TestAllocator testAllocator;

        BALM::DefaultMetricsManagerScopedGuard scopedGuard(&testAllocator);
        BALM::MetricsManager &mgr = *DefaultManager::instance();
        Repository& repository = mgr.collectorRepository();

        const int NUM_THREADS = 10;
        {

            TestClass tester(NUM_THREADS, &testAllocator);
            tester.runTest();
        }

      } break;
      case 13: {
#if 0
// The thread local macro has been removed until (hopefully) thread local
// variables are supported on all platforms.

        // --------------------------------------------------------------------
        // CONCURRENCY TEST: 'THREAD_LOCAL' MACROS
        //
        // Testing:
        //     Thread-safety of thread-local macros
        //
        //     Note that each thread executing a thread-local macro should
        //     have statically cache a different balm::Collector object.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TEST CONCURRENCY: THREAD_LOCAL MACROS" << endl
                          << "=====================================" << endl;

        typedef TlsMacroConcurrencyTest TestClass;

        Corp::bslma::TestAllocator defaultAllocator;
        Corp::bslma::DefaultAllocatorGuard guard(&defaultAllocator);
        Corp::bslma::TestAllocator testAllocator;

        BALM::DefaultMetricsManagerScopedGuard scopedGuard(&testAllocator);
        BALM::MetricsManager &mgr = *DefaultManager::instance();
        Repository& repository = mgr.collectorRepository();
        Registry&   registry   = mgr.metricRegistry();
        const int NUM_THREADS = 10;
        {

            TestClass tester(NUM_THREADS, &testAllocator);
            tester.runTest();
        }
#endif
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // CONCURRENCY TEST: 'DYNAMIC' MACROS
        //
        // Testing:
        //     Thread-safety of dynamic macros
        //
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TEST CONCURRENCY: DYNAMIC MACROS" << endl
                          << "================================" << endl;

        typedef DynamicMacroConcurrencyTest TestClass;

        Corp::bslma::TestAllocator defaultAllocator;
        Corp::bslma::DefaultAllocatorGuard guard(&defaultAllocator);
        Corp::bslma::TestAllocator testAllocator;

        BALM::DefaultMetricsManagerScopedGuard scopedGuard(&testAllocator);
        BALM::MetricsManager &mgr = *DefaultManager::instance();
        Repository& repository = mgr.collectorRepository();

        const int NUM_THREADS = 10;
        {

            TestClass tester(NUM_THREADS, &testAllocator);
            tester.runTest();
        }

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // CONCURRENCY TEST: STANDARD MACROS
        //
        // Testing:
        //     Thread-safety of standard macros
        //
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TEST CONCURRENCY: STANDARD MACROS" << endl
                          << "=================================" << endl;

        typedef StandardMacroConcurrencyTest TestClass;

        Corp::bslma::TestAllocator defaultAllocator;
        Corp::bslma::DefaultAllocatorGuard guard(&defaultAllocator);
        Corp::bslma::TestAllocator testAllocator;

        BALM::DefaultMetricsManagerScopedGuard scopedGuard(&testAllocator);
        BALM::MetricsManager &mgr = *DefaultManager::instance();
        Repository& repository = mgr.collectorRepository();

        const int NUM_THREADS = 10;
        {

            TestClass tester(NUM_THREADS, &testAllocator);
            tester.runTest();
        }

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING: 'BALM_IF_CATEGORY_ENABLED'
        //
        // Testing:
        //   BALM_METRICS_IF_CATEGORY_ENABLED(CATEGORY)
        //
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING: BALM_IF_CATEGORY_ENABLEDS" << endl
                          << "==================================" << endl;

        if (veryVerbose) cout << "\tVerify basic behavior" << endl;
        {
            ASSERT(!balmMetricsIfCategoryEnabledTestA());

            BALM::DefaultMetricsManagerScopedGuard guard(Z);
            BALM::MetricsManager *manager  =
                                       BALM::DefaultMetricsManager::instance();
            BALM::MetricRegistry& registry = manager->metricRegistry();

            ASSERT(balmMetricsIfCategoryEnabledTestA());

            registry.setCategoryEnabled(registry.getCategory("A"), false);
            ASSERT(!breathingTestIfEnabledA());

            registry.setCategoryEnabled(registry.getCategory("A"), true);
            ASSERT(breathingTestIfEnabledA());
        }
        ASSERT(!breathingTestIfEnabledA());

        if (veryVerbose) cout << "\tVerify category cache is static" << endl;
        {
            ASSERT(!balmMetricsIfCategoryEnabledTestA());

            BALM::DefaultMetricsManagerScopedGuard guard(Z);
            BALM::MetricsManager *manager  =
                                       BALM::DefaultMetricsManager::instance();
            BALM::MetricRegistry& registry = manager->metricRegistry();

            ASSERT(balmMetricsIfCategoryEnabledTestA());
            ASSERT(balmMetricsIfCategoryEnabledTest("A"));
            ASSERT(balmMetricsIfCategoryEnabledTest("B"));

            registry.setCategoryEnabled(registry.getCategory("B"), false);

            ASSERT(balmMetricsIfCategoryEnabledTestA());
            ASSERT(balmMetricsIfCategoryEnabledTest("A"));
            ASSERT(balmMetricsIfCategoryEnabledTest("B"));

            registry.setCategoryEnabled(registry.getCategory("A"), false);

            ASSERT(!balmMetricsIfCategoryEnabledTestA());
            ASSERT(!balmMetricsIfCategoryEnabledTest("A"));
            ASSERT(!balmMetricsIfCategoryEnabledTest("B"));

            registry.setCategoryEnabled(registry.getCategory("B"), true);

            ASSERT(!balmMetricsIfCategoryEnabledTestA());
            ASSERT(!balmMetricsIfCategoryEnabledTest("A"));
            ASSERT(!balmMetricsIfCategoryEnabledTest("B"));

            registry.setCategoryEnabled(registry.getCategory("A"), true);

            ASSERT(balmMetricsIfCategoryEnabledTestA());
            ASSERT(balmMetricsIfCategoryEnabledTest("A"));
            ASSERT(balmMetricsIfCategoryEnabledTest("B"));
        }
        ASSERT(!balmMetricsIfCategoryEnabledTestA());
        ASSERT(!balmMetricsIfCategoryEnabledTest("A"));
        ASSERT(!balmMetricsIfCategoryEnabledTest("B"));
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING MACRO'S REPORTED TIME UNITS
        //
        // Concerns:
        //    That the '_TIME_BLOCK_*' report in the correct time units
        //
        // Testing:
        //     BALM_METRICS_TIME_BLOCK(CATEGORY, METRIC, TIME_UNITS)
        //     BALM_METRICS_TIME_BLOCK_SECONDS(CATEGORY, METRIC)
        //     BALM_METRICS_TIME_BLOCK_MILLISECONDS(CATEGORY, METRIC)
        //     BALM_METRICS_TIME_BLOCK_MICROSECONDS(CATEGORY, METRIC)
        //     BALM_METRICS_TIME_BLOCK_NANOSECONDS(CATEGORY, METRIC)
        //     BALM_METRICS_DYNAMIC_TIME_BLOCK(CATEGORY, METRIC, TIME_UNITS)
        //     BALM_METRICS_DYNAMIC_TIME_BLOCK_SECONDS(CATEGORY, METRIC)
        //     BALM_METRICS_DYNAMIC_TIME_BLOCK_MILLISECONDS(CATEGORY, METRIC)
        //     BALM_METRICS_DYNAMIC_TIME_BLOCK_MICROSECONDS(CATEGORY, METRIC)
        //     BALM_METRICS_DYNAMIC_TIME_BLOCK_NANOSECONDS(CATEGORY, METRIC)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "MACROS REPORTED TIME UNITS\n"
                          << "==========================\n";

        typedef BALM::StopwatchScopedGuard TU; // Time unit enumeration

        {
            BALM::DefaultMetricsManagerScopedGuard guard(Z);
            MetricsManager& manager = *BALM::DefaultMetricsManager::instance();
            Repository&    repository = manager.collectorRepository();

            // Create one version for every time unit, and one for the default.
            BALM::Collector *a_S  = repository.getDefaultCollector("A", "S");
            BALM::Collector *a_Ms = repository.getDefaultCollector("A", "Ms");
            BALM::Collector *a_Us = repository.getDefaultCollector("A", "Us");
            BALM::Collector *a_Ns = repository.getDefaultCollector("A", "Ns");

            BALM::Collector *b_S  = repository.getDefaultCollector("B", "S");
            BALM::Collector *b_Ms = repository.getDefaultCollector("B", "Ms");
            BALM::Collector *b_Us = repository.getDefaultCollector("B", "Us");
            BALM::Collector *b_Ns = repository.getDefaultCollector("B", "Ns");

            BALM::Collector *c_D  = repository.getDefaultCollector("C", "D");
            BALM::Collector *c_S  = repository.getDefaultCollector("C", "S");
            BALM::Collector *c_Ms = repository.getDefaultCollector("C", "Ms");
            BALM::Collector *c_Us = repository.getDefaultCollector("C", "Us");
            BALM::Collector *c_Ns = repository.getDefaultCollector("C", "Ns");

            Corp::bsls::Stopwatch sw;
            sw.start();
            {
                BALM_METRICS_TIME_BLOCK_SECONDS("A", "S");
                BALM_METRICS_TIME_BLOCK_MILLISECONDS("A", "Ms");
                BALM_METRICS_TIME_BLOCK_MICROSECONDS("A", "Us");
                BALM_METRICS_TIME_BLOCK_NANOSECONDS("A", "Ns");

                BALM_METRICS_TIME_BLOCK_SECONDS("B", "S");
                BALM_METRICS_TIME_BLOCK_MILLISECONDS("B", "Ms");
                BALM_METRICS_TIME_BLOCK_MICROSECONDS("B", "Us");
                BALM_METRICS_TIME_BLOCK_NANOSECONDS("B", "Ns");

                BALM_METRICS_TIME_BLOCK("C", "S",  TU::k_SECONDS);
                BALM_METRICS_TIME_BLOCK("C", "Ms", TU::k_MILLISECONDS);
                BALM_METRICS_TIME_BLOCK("C", "Us", TU::k_MICROSECONDS);
                BALM_METRICS_TIME_BLOCK("C", "Ns", TU::k_NANOSECONDS);

                Corp::bslmt::ThreadUtil::sleep(
                                          Corp::bsls::TimeInterval(50 * .001));

                sw.stop();
            }
            double expected = sw.elapsedTime();
            BALM::MetricRecord rA_S  = recordVal(a_S);
            BALM::MetricRecord rA_Ms = recordVal(a_Ms);
            BALM::MetricRecord rA_Us = recordVal(a_Us);
            BALM::MetricRecord rA_Ns = recordVal(a_Ns);

            BALM::MetricRecord rB_S  = recordVal(b_S);
            BALM::MetricRecord rB_Ms = recordVal(b_Ms);
            BALM::MetricRecord rB_Us = recordVal(b_Us);
            BALM::MetricRecord rB_Ns = recordVal(b_Ns);

            BALM::MetricRecord rC_S  = recordVal(c_S);
            BALM::MetricRecord rC_Ms = recordVal(c_Ms);
            BALM::MetricRecord rC_Us = recordVal(c_Us);
            BALM::MetricRecord rC_Ns = recordVal(c_Ns);

            ASSERT(within(rA_S.total(),  TU::k_SECONDS,      expected, 1.0));
            ASSERT(within(rA_Ms.total(), TU::k_MILLISECONDS, expected, 1.0));
            ASSERT(within(rA_Us.total(), TU::k_MICROSECONDS, expected, 1.0));
            ASSERT(within(rA_Ns.total(), TU::k_NANOSECONDS,  expected, 1.0));

            ASSERT(within(rB_S.total(),  TU::k_SECONDS,      expected, 1.0));
            ASSERT(within(rB_Ms.total(), TU::k_MILLISECONDS, expected, 1.0));
            ASSERT(within(rB_Us.total(), TU::k_MICROSECONDS, expected, 1.0));
            ASSERT(within(rB_Ns.total(), TU::k_NANOSECONDS,  expected, 1.0));

            ASSERT(within(rC_S.total(),  TU::k_SECONDS,      expected, 1.0));
            ASSERT(within(rC_Ms.total(), TU::k_MILLISECONDS, expected, 1.0));
            ASSERT(within(rC_Us.total(), TU::k_MICROSECONDS, expected, 1.0));
            ASSERT(within(rC_Ns.total(), TU::k_NANOSECONDS,  expected, 1.0));
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'BALM_METRICS_TIME_BLOCK' and
        //         'BALM_METRICS_DYNAMIC_TIME_BLOCK'
        //
        // Concerns:
        //    That the '*_TIME_BLOCK' macros record the elapsed time of a
        //    block of code to the indicated metric using the default metrics
        //    manager.  That the macros function if the default metrics
        //    manager has not been created, or has been destroyed.  That the
        //    static macro statically caches the metric id to update.
        //
        // Testing:
        //     BALM_METRICS_TIME_BLOCK(CATEGORY, METRIC)
        //     BALM_METRICS_DYNAMIC_TIME_BLOCK(CATEGORY, METRIC)
        // --------------------------------------------------------------------

        typedef BALM::StopwatchScopedGuard TU; // Time unit enumeration
        if (verbose) cout
            << endl
            << "TESTING 'BALM_METRICS_TIME_BLOCK' & "
            << "'BALM_METRICS_DYNAMIC_TIME_BLOCK'\n"
            << "===========================================================\n";
        {
            if (veryVerbose) cout << "\tTest without default metrics manager"
                                  << endl;
            BALM_METRICS_TIME_BLOCK("A", "A", TU::k_SECONDS);
            BALM_METRICS_DYNAMIC_TIME_BLOCK("A", "A", TU::k_SECONDS);
        }

        {
            if (veryVerbose) cout << "\tTest with default metrics manager"
                                  << endl;

            BALM::DefaultMetricsManagerScopedGuard guard;
            MetricsManager& manager = *BALM::DefaultMetricsManager::instance();
            Repository&    repository = manager.collectorRepository();
            BALM::Collector *cA = repository.getDefaultCollector("A", "1");
            BALM::Collector *cB = repository.getDefaultCollector("B", "2");

            BALM::MetricId  idA = manager.metricRegistry().getId("A","1");
            BALM::MetricId  idB = manager.metricRegistry().getId("B","2");

            Corp::bsls::Stopwatch timer;
            {
                timer.start();

                BALM_METRICS_TIME_BLOCK("A", "1", TU::k_SECONDS);
                ASSERT(BALM::MetricRecord(idA) == recordVal(cA));

                Corp::bslmt::ThreadUtil::microSleep(50000, 0);
                timer.stop();
            }

            double expected = timer.elapsedTime();
            BALM::MetricRecord record = recordVal(cA);
            ASSERT(1 == record.count());
            ASSERT(within(record.total(), TU::k_SECONDS, expected, 1.0))
            ASSERT(within(record.min(), TU::k_SECONDS, expected, 1.0));
            ASSERT(within(record.max(), TU::k_SECONDS, expected, 1.0));

            {
                timer.start();

                BALM_METRICS_DYNAMIC_TIME_BLOCK("B", "2", TU::k_SECONDS);
                ASSERT(BALM::MetricRecord(idB) == recordVal(cB));
                Corp::bslmt::ThreadUtil::microSleep(500, 0);

                Corp::bslmt::ThreadUtil::microSleep(50000, 0);
                timer.stop();
            }

            expected = timer.elapsedTime();
            record = recordVal(cB);
            ASSERT(1 == record.count());
            ASSERT(within(record.total(), TU::k_SECONDS, expected, 1.0))
            ASSERT(within(record.min(), TU::k_SECONDS, expected, 1.0));
            ASSERT(within(record.max(), TU::k_SECONDS, expected, 1.0));
        }
        {
            if (veryVerbose) cout << "\tTest with multiple macros in a block"
                                  << endl;

            BALM::DefaultMetricsManagerScopedGuard guard;
            MetricsManager& manager = *BALM::DefaultMetricsManager::instance();
            Repository&     rep     = manager.collectorRepository();

            {
                BALM_METRICS_TIME_BLOCK("A", "1", TU::k_SECONDS);
                BALM_METRICS_TIME_BLOCK("B", "2", TU::k_SECONDS);
                BALM_METRICS_TIME_BLOCK("C", "3", TU::k_SECONDS);
                BALM_METRICS_TIME_BLOCK("D", "4", TU::k_SECONDS);

                BALM_METRICS_DYNAMIC_TIME_BLOCK("E", "5", TU::k_SECONDS);
                BALM_METRICS_DYNAMIC_TIME_BLOCK("F", "6", TU::k_SECONDS);
                BALM_METRICS_DYNAMIC_TIME_BLOCK("G", "7", TU::k_SECONDS);
                BALM_METRICS_DYNAMIC_TIME_BLOCK("H", "8", TU::k_SECONDS);

                ASSERT(0 ==
                       recordVal(rep.getDefaultCollector("A", "1")).count());
                ASSERT(0 ==
                       recordVal(rep.getDefaultCollector("B", "2")).count());
                ASSERT(0 ==
                       recordVal(rep.getDefaultCollector("C", "3")).count());
                ASSERT(0 ==
                       recordVal(rep.getDefaultCollector("D", "4")).count());
                ASSERT(0 ==
                       recordVal(rep.getDefaultCollector("E", "5")).count());
                ASSERT(0 ==
                       recordVal(rep.getDefaultCollector("F", "6")).count());
                ASSERT(0 ==
                       recordVal(rep.getDefaultCollector("G", "7")).count());
                ASSERT(0 ==
                       recordVal(rep.getDefaultCollector("H", "8")).count());
            }
            ASSERT(1 ==
                   recordVal(rep.getDefaultCollector("A", "1")).count());
            ASSERT(1 ==
                   recordVal(rep.getDefaultCollector("B", "2")).count());
            ASSERT(1 ==
                   recordVal(rep.getDefaultCollector("C", "3")).count());
            ASSERT(1 ==
                   recordVal(rep.getDefaultCollector("D", "4")).count());
            ASSERT(1 ==
                   recordVal(rep.getDefaultCollector("E", "5")).count());
            ASSERT(1 ==
                   recordVal(rep.getDefaultCollector("F", "6")).count());
            ASSERT(1 ==
                   recordVal(rep.getDefaultCollector("G", "7")).count());
            ASSERT(1 ==
                   recordVal(rep.getDefaultCollector("H", "8")).count());
        }
        {
            if (veryVerbose) cout << "\tTest with static vs dynamic behavior"
                                  << endl;

            const char *CATEGORIES[] = {"A", "B", "C", "Test", "Dummy" };
            const int NUM_CATEGORIES = sizeof(CATEGORIES)/sizeof(*CATEGORIES);

            BALM::DefaultMetricsManagerScopedGuard guard;
            MetricsManager& manager = *BALM::DefaultMetricsManager::instance();
            Repository&     rep     = manager.collectorRepository();

            for (int i = 0; i < NUM_CATEGORIES; ++i) {
                BALM_METRICS_TIME_BLOCK(CATEGORIES[i], "static",
                                        TU::k_SECONDS);
                BALM_METRICS_DYNAMIC_TIME_BLOCK(CATEGORIES[i], "dynamic",
                                                TU::k_SECONDS);
            }

            // Verify static values
            ASSERT(NUM_CATEGORIES ==
                   recordVal(rep.getDefaultCollector(CATEGORIES[0],
                                                "static")).count());
            for (int i = 1; i < NUM_CATEGORIES; ++i) {
                ASSERT(0 ==
                       recordVal(rep.getDefaultCollector(CATEGORIES[i],
                                                    "static")).count());
            }

            // Verify dynamic values
            for (int i = 0; i < NUM_CATEGORIES; ++i) {
                ASSERT(1 ==
                       recordVal(rep.getDefaultCollector(CATEGORIES[i],
                                                    "dynamic")).count());
            }
        }
        {
            if (veryVerbose) cout << "\tTest macro behavior after metrics "
                                  << " manager has been destroyed"
                                  << endl;

            // Verify macros do not crash after the default metrics manager
            // has been removed.
            BALM::DefaultMetricsManager::create();
            MetricsManager& manager = *BALM::DefaultMetricsManager::instance();
            Repository&     rep     = manager.collectorRepository();
            {
                staticTimeEmptyFunction("A", "1");
                dynamicTimeEmptyFunction("B", "2");
            }
            ASSERT(1 == recordVal(rep.getDefaultCollector("A","1")).count());
            ASSERT(1 == recordVal(rep.getDefaultCollector("B","2")).count());

            BALM::DefaultMetricsManager::destroy();
            {
                staticTimeEmptyFunction("A", "1");
                dynamicTimeEmptyFunction("B", "2");
            }
        }
      } break;
      case 7: {
#if 0
// The thread local macro has been removed until (hopefully) thread local
// variables are supported on all platforms.

        // --------------------------------------------------------------------
        // TESTING: 'BALM_METRICS_THREAD_LOCAL_INT_UPDATE',
        //          'BALM_INCREMENT_THREADLOCALMETRIC'
        //
        // Concerns:
        //    That the two standard macros
        //    ('BALM_METRICS_THREAD_LOCAL_INT_UPDATE' and
        //    'BALM_METRICS_THREAD_LOCAL_INT_INCREMENT') correctly update the
        //    appropriate metric, statically cache the identified metrics
        //    identifier, and respect the supplied categories 'enabled'
        //    property.
        //
        // Plan:
        //   Verify that invoking the macros without a default metrics manager
        //   has no effect.
        //
        //   Specify a set S of unique object values having various minor or
        //   subtle differences.  For each value create an "oracle" collector,
        //   then invoke 'BALM_METRICS_THREAD_LOCAL_INT_UPDATE' and
        //   'BALM_METRICS_THREAD_LOCAL_INT_INCREMENT' on the identified metric
        //   and perform the corresponding operation on the "oracle" collector.
        //   Verify the collector underlying the metric has the same value as
        //   the "oracle" collector.  Also verify that only the collector for
        //   the first identified metric is updated.
        //
        //   Perform the same test again, but enabled or disable the metric's
        //   category on each iteration.
        //
        // Testing:
        //    BALM_METRICS_THREAD_LOCAL_INT_UPDATE(CATEGORY, NAME, VALUE)
        //    BALM_METRICS_THREAD_LOCAL_INT_INCREMENT(CATEGORY, NAME)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING: THREADLOCAL MACROS\n"
                          << "===========================\n";

        const char *IDS[] = {"A", "B", "AB", "TEST"};
        const int NUM_IDS = sizeof IDS / sizeof *IDS;
        int UPDATES[] = { 0, 12, -1321123, 2131241, 1321,
                            43145, 1, -1, INT_MIN + 1, INT_MAX - 1};
        const int NUM_UPDATES = sizeof(UPDATES)/sizeof(*UPDATES);

        if (veryVerbose)
            cout << "\tverify macros are a no-op without a metrics manager.\n";
        {
            for (int i = 0; i < NUM_IDS; ++i) {
                BALM_METRICS_THREAD_LOCAL_INT_INCREMENT(IDS[i], "increment");
                BALM_METRICS_THREAD_LOCAL_INT_UPDATE(IDS[i], "update",
                                                     UPDATES[i]);
            }
        }

        if (veryVerbose)
            cout << "\tverify thread-local macros are applied correctly.\n";

        {
            BALM::DefaultMetricsManagerScopedGuard guard(Z);
            BALM::MetricsManager& mgr = *DefaultManager::instance();
            Registry&   registry   = mgr.metricRegistry();
            Repository& repository = mgr.collectorRepository();

            BALM::MetricId updateId(registry.getId(IDS[0], "update"));
            BALM::MetricId incrementId(registry.getId(IDS[0], "increment"));
            BALM::IntegerCollector  expUpdate(updateId);
            BALM::IntegerCollector  expIncrement(incrementId);
            for (int i = 0; i < NUM_IDS; ++i) {
                for (int j = 0; j < NUM_UPDATES; ++j) {
                    BALM_METRICS_THREAD_LOCAL_INT_INCREMENT(IDS[i],
                                                           "increment");
                    BALM_METRICS_THREAD_LOCAL_INT_UPDATE(IDS[i], "update",
                                                        UPDATES[i]);
                    expIncrement.update(1);
                    expUpdate.update(UPDATES[i]);
                }
            }
            BALM::IntegerCollector *upCol  =
                               repository.getDefaultIntegerCollector(updateId);
            BALM::IntegerCollector *incCol =
                            repository.getDefaultIntegerCollector(incrementId);
            ASSERT(recordVal(&expUpdate) == recordVal(upCol));
            ASSERT(recordVal(&expIncrement) == recordVal(incCol));

            ASSERT(0 == defaultAllocator.numBytesInUse());

            // Verify only the first identified metric (i.e., a 'CATEGORY',
            // 'NAME' pair) for which the macro was invoked was modified.
            bsl::vector<BALM::Collector *> cols(Z);
            bsl::vector<BALM::IntegerCollector *> intCols(Z);
            for (int i = 1; i < NUM_IDS; ++i) {
                BALM::MetricId updateId(registry.getId(IDS[i], "update"));
                BALM::MetricId incrementId(registry.getId(IDS[i],
                                           "increment"));

                ASSERT(0 == repository.getAddedCollectors(&cols,
                                                          &intCols,
                                                          updateId));
                ASSERT(0 == repository.getAddedCollectors(&cols,
                                                          &intCols,
                                                          incrementId));
            }
        }
        if (veryVerbose)
            cout << "\tverify thread-local macros respects 'isActive' flag.\n";

        {
            BALM::DefaultMetricsManagerScopedGuard guard(Z);
            BALM::MetricsManager& mgr = *DefaultManager::instance();
            Registry&   registry   = mgr.metricRegistry();
            Repository& repository = mgr.collectorRepository();

            BALM::MetricId updateId(registry.getId(IDS[0], "update"));
            BALM::MetricId incrementId(registry.getId(IDS[0], "increment"));
            BALM::IntegerCollector  expUpdate(updateId);
            BALM::IntegerCollector  expIncrement(incrementId);
            for (int i = 0; i < NUM_IDS; ++i) {
                for (int j = 0; j < NUM_UPDATES; ++j) {

                    bool enabled = 0 == j % 2;
                    registry.setCategoryEnabled(updateId.category(), enabled);
                    BALM_METRICS_THREAD_LOCAL_INT_INCREMENT(IDS[i],
                                                           "increment");
                    BALM_METRICS_THREAD_LOCAL_INT_UPDATE(IDS[i], "update",
                                                         UPDATES[i]);
                    if (enabled) {
                        expIncrement.update(1);
                        expUpdate.update(UPDATES[i]);
                    }
                }
            }
            BALM::IntegerCollector *upCol  =
                               repository.getDefaultIntegerCollector(updateId);
            BALM::IntegerCollector *incCol =
                            repository.getDefaultIntegerCollector(incrementId);
            ASSERT(recordVal(&expUpdate) == recordVal(upCol));
            ASSERT(recordVal(&expIncrement) == recordVal(incCol));

            ASSERT(0 == defaultAllocator.numBytesInUse());

            // Verify only the first identified metric (i.e., a 'CATEGORY',
            // 'NAME' pair) for which the macro was invoked was modified.
            bsl::vector<BALM::Collector *> cols(Z);
            bsl::vector<BALM::IntegerCollector *> intCols(Z);
            for (int i = 1; i < NUM_IDS; ++i) {
                BALM::MetricId updateId(registry.getId(IDS[i], "update"));
                BALM::MetricId incrementId(registry.getId(IDS[i],
                                           "increment"));

                ASSERT(0 == repository.getAddedCollectors(&cols,
                                                          &intCols,
                                                          updateId));
                ASSERT(0 == repository.getAddedCollectors(&cols,
                                                          &intCols,
                                                          incrementId));
            }
        }
#endif
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING: 'BALM_METRICS_DYNAMIC_INT_UPDATE',
        //          'BALM_METRICS_DYNAMIC_INT_INCREMENT'
        //
        // Concerns:
        //    That the two dynamic macros ('BALM_METRICS_DYNAMIC_INT_UPDATE'
        //    and 'BALM_METRICS_DYNAMIC_INT_INCREMENT') correctly update the
        //    appropriate metric and respect the supplied categories 'enabled'
        //    property.
        //
        // Plan:
        //   Verify that invoking the macros without a default metrics manager
        //   has no effect.
        //
        //   Specify a set S of unique object values having various minor or
        //   subtle differences.  For each value create an "oracle" collector,
        //   then invoke 'BALM_METRICS_DYNAMIC_INT_UPDATE' and
        //   'BALM_METRICS_DYNAMIC_INT_INCREMENT' on the identified metric and
        //   perform the corresponding operation on the "oracle" collector.
        //   Verify the collector underlying the metric has the same value as
        //   the "oracle" collector.
        //
        //   Perform the same test again, but enabled or disable the metric's
        //   category on each iteration.
        //
        // Testing:
        //    BALM_METRICS_DYNAMIC_INT_UPDATE(CATEGORY, NAME, VALUE)
        //    BALM_METRICS_DYNAMIC_INCREMENT(CATEGORY, NAME)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING: DYNAMIC MACROS\n"
                          << "========================\n";

        const char *IDS[] = {"A", "B", "AB", "TEST"};
        const int NUM_IDS = sizeof IDS / sizeof *IDS;
        int UPDATES[] = { 0, 12, -1321123, 2131241, 1321,
                            43145, 1, -1, INT_MIN + 1, INT_MAX - 1};
        const int NUM_UPDATES = sizeof(UPDATES)/sizeof(*UPDATES);

        if (veryVerbose)
            cout << "\tverify macros are a no-op without a metrics manager.\n";
        {
            for (int i = 0; i < NUM_IDS; ++i) {
                BALM_METRICS_DYNAMIC_INCREMENT(IDS[i], "increment");
                BALM_METRICS_DYNAMIC_INT_UPDATE(IDS[i], "update", UPDATES[i]);
            }
        }

        if (veryVerbose)
            cout << "\tverify dynamic macros are applied correctly.\n";
        {
            BALM::DefaultMetricsManagerScopedGuard guard(Z);
            BALM::MetricsManager& mgr = *DefaultManager::instance();
            Registry&   registry   = mgr.metricRegistry();
            Repository& repository = mgr.collectorRepository();

            for (int i = 0; i < NUM_IDS; ++i) {
                BALM::MetricId updateId(registry.getId(IDS[i], "update"));
                BALM::MetricId incrementId(registry.getId(IDS[i],
                                                          "increment"));
                BALM::IntegerCollector expUpdate(updateId);
                BALM::IntegerCollector expIncrement(incrementId);
                BALM::IntegerCollector *upCol  =
                            repository.getDefaultIntegerCollector(updateId);
                BALM::IntegerCollector *incCol =
                            repository.getDefaultIntegerCollector(incrementId);
                for (int j = 0; j < NUM_UPDATES; ++j) {
                    BALM_METRICS_DYNAMIC_INCREMENT(IDS[i], "increment");
                    BALM_METRICS_DYNAMIC_INT_UPDATE(IDS[i], "update",
                                                    UPDATES[j]);
                    expIncrement.update(1);
                    expUpdate.update(UPDATES[j]);
                    ASSERT(recordVal(&expUpdate) == recordVal(upCol));
                    ASSERT(recordVal(&expIncrement) == recordVal(incCol));
                }
            }
        }
        if (veryVerbose)
            cout << "\tverify dynamic macros respect 'isActive' flag.\n";

        {
            BALM::DefaultMetricsManagerScopedGuard guard(Z);
            BALM::MetricsManager& mgr = *DefaultManager::instance();
            Registry&   registry   = mgr.metricRegistry();
            Repository& repository = mgr.collectorRepository();

            for (int i = 0; i < NUM_IDS; ++i) {
                BALM::MetricId  updateId(registry.getId(IDS[i], "update"));
                BALM::MetricId  incrementId(registry.getId(IDS[i],
                                                          "increment"));
                BALM::IntegerCollector expUpdate(updateId);
                BALM::IntegerCollector expIncrement(incrementId);
                BALM::IntegerCollector *upCol  =
                            repository.getDefaultIntegerCollector(updateId);
                BALM::IntegerCollector *incCol =
                            repository.getDefaultIntegerCollector(incrementId);
                for (int j = 0; j < NUM_UPDATES; ++j) {
                    bool enabled = 0 == j % 2;

                    registry.setCategoryEnabled(updateId.category(), enabled);
                    BALM_METRICS_DYNAMIC_INCREMENT(IDS[i], "increment");
                    BALM_METRICS_DYNAMIC_INT_UPDATE(IDS[i], "update",
                                                    UPDATES[j]);
                    if (enabled) {
                        expIncrement.update(1);
                        expUpdate.update(UPDATES[j]);
                    }
                    ASSERT(recordVal(&expUpdate) == recordVal(upCol));
                    ASSERT(recordVal(&expIncrement) == recordVal(incCol));

                }
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING: 'BALM_METRICS_INT_UPDATE', 'BALM_METRICS_INT_INCREMENT'
        //          'BALM_UPDATE_INT_TYPED_METRIC'
        // Concerns:
        //    That the two standard macros ('BALM_METRICS_INT_UPDATE' and
        //    'BALM_METRICS_INT_INCREMENT') correctly update the appropriate
        //    metric, statically cache the identified metrics identifier, and
        //    respect the supplied categories 'enabled' property.
        //
        // Plan:
        //   Verify that invoking the macros without a default metrics manager
        //   has no effect.
        //
        //   Specify a set S of unique object values having various minor or
        //   subtle differences.  For each value create an "oracle" collector,
        //   then invoke 'BALM_METRICS_INT_UPDATE' and
        //   'BALM_METRICS_INT_INCREMENT' on the identified metric and perform
        //   the corresponding operation on the "oracle" collector.  Verify
        //   the collector underlying the metric has the same value as the
        //   "oracle" collector.  Also verify that only the collector for the
        //   first identified metric is updated.
        //
        //   Perform the same test again, but enabled or disable the metric's
        //   category on each iteration.
        //
        // Testing:
        //    BALM_METRICS_INT_UPDATE(CATEGORY, NAME, VALUE
        //    BALM_METRICS_TYPED_INT_UPDATE(CATEGORY, NAME, TYPE, VALUE)
        //    BALM_METRICS_INT_INCREMENT(CATEGORY, NAME
        //    BALM_METRICS_INT_UPDATE2(CATEGORY, METRIC1, VALUE1, ...)
        //    BALM_METRICS_INT_UPDATE3(CATEGORY, METRIC1, VALUE1, ...)
        //    BALM_METRICS_INT_UPDATE4(CATEGORY, METRIC1, VALUE1, ...)
        //    BALM_METRICS_INT_UPDATE5(CATEGORY, METRIC1, VALUE1, ...)
        //    BALM_METRICS_INT_UPDATE6(CATEGORY, METRIC1, VALUE1, ...)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING: STANDARD INT MACROS\n"
                          << "============================\n";

        const char *IDS[] = {"A", "B", "AB", "TEST"};
        const int NUM_IDS = sizeof IDS / sizeof *IDS;

        int UPDATES[] = { 0, 12, -1321123, 2131241, 1321,
                            43145, 1, -1, INT_MIN + 1, INT_MAX - 1};
        const int NUM_UPDATES = sizeof(UPDATES)/sizeof(*UPDATES);

        if (veryVerbose)
            cout << "\tverify macros are a no-op without a metrics manager.\n";
        {
            for (int i = 0; i < NUM_IDS; ++i) {
                BALM_METRICS_INCREMENT(IDS[i], "increment");
                BALM_METRICS_INT_UPDATE(IDS[i], "update", UPDATES[i]);
                BALM_METRICS_TYPED_INT_UPDATE(IDS[i], "t-up", UPDATES[i],
                                              Type::e_TOTAL);
                BALM_METRICS_TYPED_INCREMENT(IDS[i], "t-inc", Type::e_MIN);
            }
        }

        if (veryVerbose)
            cout << "\tverify macros are applied correctly.\n";

        {
            BALM::DefaultMetricsManagerScopedGuard guard(Z);
            BALM::MetricsManager& mgr = *DefaultManager::instance();
            Registry&   registry   = mgr.metricRegistry();
            Repository& repository = mgr.collectorRepository();

            BALM::MetricId updateId(registry.getId(IDS[0], "update"));
            BALM::MetricId incId(registry.getId(IDS[0], "increment"));
            BALM::MetricId tupdateId(registry.getId(IDS[0], "Tupdate"));
            BALM::MetricId tincId(registry.getId(IDS[0], "Tincrement"));

            BALM::IntegerCollector expUpdate(updateId);
            BALM::IntegerCollector expIncrement(incId);
            BALM::IntegerCollector expTUpdate(tupdateId);
            BALM::IntegerCollector expTIncrement(tincId);
            for (int i = 0; i < NUM_IDS; ++i) {
                for (int j = 0; j < NUM_UPDATES; ++j) {
                    BALM_METRICS_INCREMENT(IDS[i], "increment");
                    BALM_METRICS_INT_UPDATE(IDS[i], "update", UPDATES[j]);
                    BALM_METRICS_TYPED_INT_UPDATE(IDS[i], "Tupdate",
                                                  UPDATES[j],
                                                  Type::e_TOTAL);
                    BALM_METRICS_TYPED_INCREMENT(IDS[i], "Tincrement",
                                                 Type::e_MIN);
                    expIncrement.update(1);
                    expUpdate.update(UPDATES[j]);
                    expTUpdate.update(UPDATES[j]);
                    expTIncrement.update(1);
                }
            }
            ASSERT(Type::e_TOTAL ==
                   tupdateId.description()->preferredPublicationType());
            ASSERT(Type::e_MIN   ==
                   tincId.description()->preferredPublicationType());

            BALM::IntegerCollector *upCol  =
                repository.getDefaultIntegerCollector(updateId);
            BALM::IntegerCollector *incCol =
                repository.getDefaultIntegerCollector(incId);
            BALM::IntegerCollector *tupCol =
                repository.getDefaultIntegerCollector(tupdateId);
            BALM::IntegerCollector *tincCol =
                repository.getDefaultIntegerCollector(tincId);

            ASSERT(recordVal(&expUpdate)     == recordVal(upCol));
            ASSERT(recordVal(&expIncrement)  == recordVal(incCol));
            ASSERT(recordVal(&expTUpdate)    == recordVal(tupCol));
            ASSERT(recordVal(&expTIncrement) == recordVal(tincCol));

            // Verify only the first identified metric (i.e., a 'CATEGORY',
            // 'NAME' pair) for which the macro was invoked was modified.
            ColSPtrVector cols(Z);
            IColSPtrVector intCols(Z);
            for (int i = 1; i < NUM_IDS; ++i) {
                BALM::MetricId updateId(registry.getId(IDS[i], "update"));
                BALM::MetricId incId(registry.getId(IDS[i], "increment"));
                BALM::MetricId tupdateId(registry.getId(IDS[i], "Tupdate"));
                BALM::MetricId tincId(registry.getId(IDS[i], "Tincrement"));

                ASSERT(0 == repository.getAddedCollectors(&cols,
                                                          &intCols,
                                                          updateId));
                ASSERT(0 == repository.getAddedCollectors(&cols,
                                                          &intCols,
                                                          incId));
                ASSERT(0 == repository.getAddedCollectors(&cols,
                                                          &intCols,
                                                          tupdateId));
                ASSERT(0 == repository.getAddedCollectors(&cols,
                                                          &intCols,
                                                          tincId));
            }
        }
        if (veryVerbose)
            cout << "\tverify macros respects 'isActive' flag.\n";

        {
            BALM::DefaultMetricsManagerScopedGuard guard(Z);
            BALM::MetricsManager& mgr = *DefaultManager::instance();
            Registry&   registry   = mgr.metricRegistry();
            Repository& repository = mgr.collectorRepository();

            BALM::MetricId updateId(registry.getId(IDS[0], "update"));
            BALM::MetricId incId(registry.getId(IDS[0], "increment"));
            BALM::MetricId tupdateId(registry.getId(IDS[0], "Tupdate"));
            BALM::MetricId tincId(registry.getId(IDS[0], "Tincrement"));

            BALM::IntegerCollector expUpdate(updateId);
            BALM::IntegerCollector expIncrement(incId);
            BALM::IntegerCollector expTUpdate(tupdateId);
            BALM::IntegerCollector expTIncrement(tincId);
            for (int i = 0; i < NUM_IDS; ++i) {
                for (int j = 0; j < NUM_UPDATES; ++j) {
                    bool enabled = 0 == j % 2;
                    registry.setCategoryEnabled(updateId.category(), enabled);
                    BALM_METRICS_INCREMENT(IDS[i], "increment");
                    BALM_METRICS_INT_UPDATE(IDS[i], "update", UPDATES[j]);
                    BALM_METRICS_TYPED_INT_UPDATE(IDS[i], "Tupdate",
                                                  UPDATES[j],
                                                  Type::e_TOTAL);
                    BALM_METRICS_TYPED_INCREMENT(IDS[i], "Tincrement",
                                                 Type::e_MIN);
                    if (enabled) {
                        expIncrement.update(1);
                        expUpdate.update(UPDATES[j]);
                        expTUpdate.update(UPDATES[j]);
                        expTIncrement.update(1);
                    }
                }
            }

            ASSERT(Type::e_TOTAL ==
                   tupdateId.description()->preferredPublicationType());
            ASSERT(Type::e_MIN   ==
                   tincId.description()->preferredPublicationType());

            BALM::IntegerCollector *upCol  =
                repository.getDefaultIntegerCollector(updateId);
            BALM::IntegerCollector *incCol =
                repository.getDefaultIntegerCollector(incId);
            BALM::IntegerCollector *tupCol =
                repository.getDefaultIntegerCollector(tupdateId);
            BALM::IntegerCollector *tincCol =
                repository.getDefaultIntegerCollector(tincId);

            ASSERT(recordVal(&expUpdate)     == recordVal(upCol));
            ASSERT(recordVal(&expIncrement)  == recordVal(incCol));
            ASSERT(recordVal(&expTUpdate)    == recordVal(tupCol));
            ASSERT(recordVal(&expTIncrement) == recordVal(tincCol));

            ASSERT(0 == defaultAllocator.numBytesInUse());

            // Verify only the first identified metric (i.e., a 'CATEGORY',
            // 'NAME' pair) for which the macro was invoked was modified.
            ColSPtrVector  cols(Z);
            IColSPtrVector intCols(Z);
            for (int i = 1; i < NUM_IDS; ++i) {
                BALM::MetricId updateId(registry.getId(IDS[i], "update"));
                BALM::MetricId incId(registry.getId(IDS[i], "increment"));
                BALM::MetricId tupdateId(registry.getId(IDS[i], "Tupdate"));
                BALM::MetricId tincId(registry.getId(IDS[i], "Tincrement"));

                ASSERT(0 == repository.getAddedCollectors(&cols,
                                                          &intCols,
                                                          updateId));
                ASSERT(0 == repository.getAddedCollectors(&cols,
                                                          &intCols,
                                                          incId));
                ASSERT(0 == repository.getAddedCollectors(&cols,
                                                          &intCols,
                                                          tupdateId));
                ASSERT(0 == repository.getAddedCollectors(&cols,
                                                          &intCols,
                                                          tincId));
            }
        }
        if (veryVerbose)
            cout << "\tverify macro behaves correctly if the default metrics "
                 << "manager is destroyed (and recreated).\n";
        {
            for (int testIter = 0; testIter < 5; ++testIter) {
                // Create a new default metrics manager for this test.

                bool initializeDefault = (0 == testIter % 2);
                BALM::MetricsManager *mgr = 0;
                if (initializeDefault) {
                    mgr = BALM::DefaultMetricsManager::create(Z);
                }
                else {
                    mgr = new (*Z) BALM::MetricsManager(Z);
                }
                Registry&   registry   = mgr->metricRegistry();
                Repository& repository = mgr->collectorRepository();

                BALM::MetricId updateId(registry.getId(IDS[0], "update"));
                BALM::MetricId incId(registry.getId(IDS[0], "increment"));
                BALM::MetricId tupdateId(registry.getId(IDS[0], "Tupdate"));
                BALM::MetricId tincId(registry.getId(IDS[0], "Tincrement"));

                BALM::IntegerCollector expUpdate(updateId);
                BALM::IntegerCollector expIncrement(incId);
                BALM::IntegerCollector expTUpdate(tupdateId);
                BALM::IntegerCollector expTIncrement(tincId);
                for (int i = 0; i < NUM_IDS; ++i) {
                    for (int j = 0; j < NUM_UPDATES; ++j) {
                        BALM_METRICS_INCREMENT(IDS[i], "increment");
                        BALM_METRICS_INT_UPDATE(IDS[i], "update", UPDATES[j]);
                        BALM_METRICS_TYPED_INT_UPDATE(IDS[i], "Tupdate",
                                                     UPDATES[j],
                                                     Type::e_TOTAL);
                        BALM_METRICS_TYPED_INCREMENT(IDS[i], "Tincrement",
                                                     Type::e_MIN);
                        expIncrement.update(1);
                        expUpdate.update(UPDATES[j]);
                        expTUpdate.update(UPDATES[j]);
                        expTIncrement.update(1);
                    }
                }
                if (initializeDefault) {
                    BALM::IntegerCollector *upCol   =
                        repository.getDefaultIntegerCollector(updateId);
                    BALM::IntegerCollector *incCol  =
                        repository.getDefaultIntegerCollector(incId);
                    BALM::IntegerCollector *tupCol  =
                        repository.getDefaultIntegerCollector(tupdateId);
                    BALM::IntegerCollector *tincCol =
                        repository.getDefaultIntegerCollector(tincId);

                    ASSERT(recordVal(&expUpdate)     == recordVal(upCol));
                    ASSERT(recordVal(&expIncrement)  == recordVal(incCol));
                    ASSERT(recordVal(&expTUpdate)    == recordVal(tupCol));
                    ASSERT(recordVal(&expTIncrement) == recordVal(tincCol));
                    BALM::DefaultMetricsManager::destroy();
                }
                else {
                    Z->deleteObjectRaw(mgr);
                }
            }

            ASSERT(0 == defaultAllocator.numBytesInUse());
        }
        if (veryVerbose)  cout << "\tVerify multiple metric variants.\n";
        {
            BALM::DefaultMetricsManagerScopedGuard guard(Z);
            BALM::MetricsManager  *manager =
                                       BALM::DefaultMetricsManager::instance();
            BALM::MetricRegistry& registry = manager->metricRegistry();
            BALM::CollectorRepository& repository =
                                                manager->collectorRepository();

            ASSERT(0 == defaultAllocator.numBytesInUse());

            const Id A_ID = registry.getId("A", "A");
            const Id B_ID = registry.getId("A", "B");
            const Id C_ID = registry.getId("A", "C");
            const Id D_ID = registry.getId("A", "D");
            const Id E_ID = registry.getId("A", "E");
            const Id F_ID = registry.getId("A", "F");
            const Id G_ID = registry.getId("A", "G");

            for (int i = 0; i < 10; ++i) {
                BALM_METRICS_INT_UPDATE2("A",
                                         "A", 1, "B", 1);
                BALM_METRICS_INT_UPDATE3("A",
                                         "A", 2, "B", 2, "C", 2);
                BALM_METRICS_INT_UPDATE4("A",
                                         "A", 3, "B", 3, "C", 3, "D", 3);
                BALM_METRICS_INT_UPDATE5("A",
                                         "A", 4, "B", 4, "C", 4, "D", 4,
                                         "E", 4);
                BALM_METRICS_INT_UPDATE6("A",
                                         "A", 5, "B", 5, "C", 5, "D", 5,
                                         "E", 5, "F", 5);
            }

            BALM::IntegerCollector *A_COL =
                repository.getDefaultIntegerCollector("A", "A");
            BALM::IntegerCollector *B_COL =
                repository.getDefaultIntegerCollector("A", "B");
            BALM::IntegerCollector *C_COL =
                repository.getDefaultIntegerCollector("A", "C");
            BALM::IntegerCollector *D_COL =
                repository.getDefaultIntegerCollector("A", "D");
            BALM::IntegerCollector *E_COL =
                repository.getDefaultIntegerCollector("A", "E");
            BALM::IntegerCollector *F_COL =
                repository.getDefaultIntegerCollector("A", "F");
            BALM::IntegerCollector *G_COL =
                repository.getDefaultIntegerCollector("A", "G");

            if (veryVeryVerbose) {
                P_(recordVal(A_COL)); P_(recordVal(B_COL));
                P_(recordVal(C_COL)); P(recordVal(D_COL));
                P_(recordVal(E_COL)); P_(recordVal(F_COL));
                P(recordVal(G_COL));
            }
            ASSERT(
                  BALM::MetricRecord(A_ID, 50, 150, 1, 5) == recordVal(A_COL));
            ASSERT(
                  BALM::MetricRecord(B_ID, 50, 150, 1, 5) == recordVal(B_COL));
            ASSERT(
                  BALM::MetricRecord(C_ID, 40, 140, 2, 5) == recordVal(C_COL));
            ASSERT(
                  BALM::MetricRecord(D_ID, 30, 120, 3, 5) == recordVal(D_COL));
            ASSERT(
                  BALM::MetricRecord(E_ID, 20,  90, 4, 5) == recordVal(E_COL));
            ASSERT(
                  BALM::MetricRecord(F_ID, 10,  50, 5, 5) == recordVal(F_COL));
            ASSERT(
                  BALM::MetricRecord(G_ID)                == recordVal(G_COL));

            ColSPtrVector  cols(Z);
            IColSPtrVector intCols(Z);
            ASSERT(0 == repository.getAddedCollectors(&cols,&intCols,A_ID));
            ASSERT(0 == repository.getAddedCollectors(&cols,&intCols,B_ID));
            ASSERT(0 == repository.getAddedCollectors(&cols,&intCols,C_ID));
            ASSERT(0 == repository.getAddedCollectors(&cols,&intCols,D_ID));
            ASSERT(0 == repository.getAddedCollectors(&cols,&intCols,E_ID));
            ASSERT(0 == repository.getAddedCollectors(&cols,&intCols,F_ID));
            ASSERT(0 == repository.getAddedCollectors(&cols,&intCols,G_ID));
            ASSERT(0 == defaultAllocator.numBytesInUse());
        }
      } break;
      case 4: {
#if 0
// The thread local macro has been removed until (hopefully) thread local
// variables are supported on all platforms.

        // --------------------------------------------------------------------
        // TESTING: 'BALM_METRICS_THREAD_LOCAL_UPDATE',
        //          'BALM_INCREMENT_THREADLOCALMETRIC'
        //
        // Concerns:
        //    That the two standard macros ('BALM_METRICS_THREAD_LOCAL_UPDATE'
        //    and 'BALM_METRICS_THREAD_LOCAL_INCREMENT') correctly update the
        //    appropriate metric, statically cache the identified metrics
        //    identifier, and respect the supplied categories 'enabled'
        //    property.
        //
        // Plan:
        //   Verify that invoking the macros without a default metrics manager
        //   has no effect.
        //
        //   Specify a set S of unique object values having various minor or
        //   subtle differences.  For each value create an "oracle" collector,
        //   then invoke 'BALM_METRICS_THREAD_LOCAL_UPDATE' and
        //   'BALM_METRICS_THREAD_LOCAL_INCREMENT' on the identified metric and
        //   perform the corresponding operation on the "oracle" collector.
        //   Verify the collector underlying the metric has the same value as
        //   the "oracle" collector.  Also verify that only the collector for
        //   the first identified metric is updated.
        //
        //   Perform the same test again, but enabled or disable the metric's
        //   category on each iteration.
        //
        // Testing:
        //    BALM_METRICS_THREAD_LOCAL_UPDATE(CATEGORY, NAME, VALUE)
        //    BALM_METRICS_THREAD_LOCAL_INCREMENT(CATEGORY, NAME)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING: THREADLOCAL MACROS\n"
                          << "===========================\n";

        const char *IDS[] = {"A", "B", "AB", "TEST"};
        const int NUM_IDS = sizeof IDS / sizeof *IDS;
        double UPDATES[] = { 0.0, 12.0, -1321123, 2131241, 1321.5,
                             43145.1, .0001, -1.00001, -.002342};
        const int NUM_UPDATES = sizeof(UPDATES)/sizeof(*UPDATES);

        if (veryVerbose)
            cout << "\tverify macros are a no-op without a metrics manager.\n";
        {
            for (int i = 0; i < NUM_IDS; ++i) {
                BALM_METRICS_THREAD_LOCAL_INCREMENT(IDS[i], "increment");
                BALM_METRICS_THREAD_LOCAL_UPDATE(IDS[i], "update", UPDATES[i]);
            }
        }

        if (veryVerbose)
            cout << "\tverify thread-local macros are applied correctly.\n";

        {
            BALM::DefaultMetricsManagerScopedGuard guard(Z);
            BALM::MetricsManager& mgr = *DefaultManager::instance();
            Registry&   registry   = mgr.metricRegistry();
            Repository& repository = mgr.collectorRepository();

            BALM::MetricId updateId(registry.getId(IDS[0], "update"));
            BALM::MetricId incId(registry.getId(IDS[0], "increment"));
            BALM::Collector  expUpdate(updateId);
            BALM::Collector  expIncrement(incId);
            for (int i = 0; i < NUM_IDS; ++i) {
                for (int j = 0; j < NUM_UPDATES; ++j) {
                    BALM_METRICS_THREAD_LOCAL_INCREMENT(IDS[i], "increment");
                    BALM_METRICS_THREAD_LOCAL_UPDATE(IDS[i], "update",
                                                   UPDATES[i]);
                    expIncrement.update(1);
                    expUpdate.update(UPDATES[i]);
                }
            }
            BALM::Collector *upCol  = repository.getDefaultCollector(updateId);
            BALM::Collector *incCol = repository.getDefaultCollector(incId);
            ASSERT(recordVal(&expUpdate) == recordVal(upCol));
            ASSERT(recordVal(&expIncrement) == recordVal(incCol));

            ASSERT(0 == defaultAllocator.numBytesInUse());

            // Verify only the first identified metric (i.e., a 'CATEGORY',
            // 'NAME' pair) for which the macro was invoked was modified.
            bsl::vector<BALM::Collector *> cols(Z);
            bsl::vector<BALM::IntegerCollector *> intCols(Z);
            for (int i = 1; i < NUM_IDS; ++i) {
                BALM::MetricId updateId(registry.getId(IDS[i], "update"));
                BALM::MetricId incId(registry.getId(IDS[i], "increment"));

                ASSERT(0 == repository.getAddedCollectors(&cols,
                                                          &intCols,
                                                          updateId));
                ASSERT(0 == repository.getAddedCollectors(&cols,
                                                          &intCols,
                                                          incId));
            }
        }
        if (veryVerbose)
            cout << "\tverify thread-local macros respects 'isActive' flag.\n";

        {
            BALM::DefaultMetricsManagerScopedGuard guard(Z);
            BALM::MetricsManager& mgr = *DefaultManager::instance();
            Registry&   registry   = mgr.metricRegistry();
            Repository& repository = mgr.collectorRepository();

            BALM::MetricId updateId(registry.getId(IDS[0], "update"));
            BALM::MetricId incId(registry.getId(IDS[0], "increment"));
            BALM::Collector  expUpdate(updateId);
            BALM::Collector  expIncrement(incId);
            for (int i = 0; i < NUM_IDS; ++i) {
                for (int j = 0; j < NUM_UPDATES; ++j) {

                    bool enabled = 0 == j % 2;
                    registry.setCategoryEnabled(updateId.category(), enabled);
                    BALM_METRICS_THREAD_LOCAL_INCREMENT(IDS[i], "increment");
                    BALM_METRICS_THREAD_LOCAL_UPDATE(IDS[i], "update",
                                                   UPDATES[i]);
                    if (enabled) {
                        expIncrement.update(1);
                        expUpdate.update(UPDATES[i]);
                    }
                }
            }
            BALM::Collector *upCol  = repository.getDefaultCollector(updateId);
            BALM::Collector *incCol = repository.getDefaultCollector(incId);
            ASSERT(recordVal(&expUpdate) == recordVal(upCol));
            ASSERT(recordVal(&expIncrement) == recordVal(incCol));

            ASSERT(0 == defaultAllocator.numBytesInUse());

            // Verify only the first identified metric (i.e., a 'CATEGORY',
            // 'NAME' pair) for which the macro was invoked was modified.
            bsl::vector<BALM::Collector *> cols(Z);
            bsl::vector<BALM::IntegerCollector *> intCols(Z);
            for (int i = 1; i < NUM_IDS; ++i) {
                BALM::MetricId updateId(registry.getId(IDS[i], "update"));
                BALM::MetricId incId(registry.getId(IDS[i], "increment"));

                ASSERT(0 == repository.getAddedCollectors(&cols,
                                                          &intCols,
                                                          updateId));
                ASSERT(0 == repository.getAddedCollectors(&cols,
                                                          &intCols,
                                                          incId));
            }
        }
#endif
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING: 'BALM_METRICS_DYNAMIC_UPDATE',
        // 'BALM_METRICS_DYNAMIC_INCREMENT'
        //
        // Concerns: That the two dynamic macros ('BALM_METRICS_DYNAMIC_UPDATE'
        // and 'BALM_METRICS_DYNAMIC_INCREMENT') correctly update the
        // appropriate metric and respect the supplied categories 'enabled'
        // property.
        //
        // Plan: Verify that invoking the macros without a default metrics
        // manager has no effect.
        //
        // Specify a set S of unique object values having various minor or
        // subtle differences.  For each value create an "oracle" collector,
        // then invoke 'BALM_METRICS_DYNAMIC_UPDATE' and
        // 'BALM_METRICS_DYNAMIC_INCREMENT' on the identified metric and
        // perform the corresponding operation on the "oracle" collector.
        // Verify the collector underlying the metric has the same value as the
        // "oracle" collector.
        //
        // Perform the same test again, but enabled or disable the metric's
        // category on each iteration.
        //
        // Testing:
        //   BALM_METRICS_DYNAMIC_UPDATE(CATEGORY, NAME, VALUE)
        //   BALM_METRICS_DYNAMIC_INCREMENT(CATEGORY, NAME)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING: DYNAMIC MACROS\n"
                          << "========================\n";

        const char *IDS[] = {"A", "B", "AB", "TEST"};
        const int NUM_IDS = sizeof IDS / sizeof *IDS;
        double UPDATES[] = { 0.0, 12.0, -1321123, 2131241, 1321.5,
                             43145.1, .0001, -1.00001, -.002342};
        const int NUM_UPDATES = sizeof(UPDATES)/sizeof(*UPDATES);

        if (veryVerbose)
            cout << "\tverify macros are a no-op without a metrics manager.\n";
        {
            for (int i = 0; i < NUM_IDS; ++i) {
                BALM_METRICS_DYNAMIC_UPDATE(IDS[i], "update", UPDATES[i]);
            }
        }

        if (veryVerbose)
            cout << "\tverify dynamic macros are applied correctly.\n";
        {
            BALM::DefaultMetricsManagerScopedGuard guard(Z);
            BALM::MetricsManager& mgr = *DefaultManager::instance();
            Registry&   registry   = mgr.metricRegistry();
            Repository& repository = mgr.collectorRepository();

            for (int i = 0; i < NUM_IDS; ++i) {
                BALM::MetricId  updateId(registry.getId(IDS[i], "update"));
                BALM::Collector expUpdate(updateId);
                BALM::Collector *upCol  =
                                      repository.getDefaultCollector(updateId);
                for (int j = 0; j < NUM_UPDATES; ++j) {
                    BALM_METRICS_DYNAMIC_UPDATE(IDS[i], "update", UPDATES[j]);
                    expUpdate.update(UPDATES[j]);
                    ASSERT(recordVal(&expUpdate) == recordVal(upCol));
                }
            }
        }
        if (veryVerbose)
            cout << "\tverify dynamic macros respect 'isActive' flag.\n";

        {
            BALM::DefaultMetricsManagerScopedGuard guard(Z);
            BALM::MetricsManager& mgr = *DefaultManager::instance();
            Registry&   registry   = mgr.metricRegistry();
            Repository& repository = mgr.collectorRepository();

            for (int i = 0; i < NUM_IDS; ++i) {
                BALM::MetricId  updateId(registry.getId(IDS[i], "update"));
                BALM::Collector expUpdate(updateId);
                BALM::Collector *upCol  =
                                  repository.getDefaultCollector(updateId);
                for (int j = 0; j < NUM_UPDATES; ++j) {
                    bool enabled = 0 == j % 2;
                    registry.setCategoryEnabled(updateId.category(), enabled);
                    BALM_METRICS_DYNAMIC_UPDATE(IDS[i], "update", UPDATES[j]);
                    if (enabled) {
                        expUpdate.update(UPDATES[j]);
                    }
                    ASSERT(recordVal(&expUpdate) == recordVal(upCol));
                }
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING: 'BALM_METRICS_UPDATE', 'BALM_METRICS_INCREMENT'
        //          'BALM_METRICS_TYPED_UPDATE'
        //
        // Concerns:
        //    That the three standard macros ('BALM_METRICS_UPDATE' and
        //    'BALM_METRICS_INCREMENT' and 'BALM_METRICS_TYPED_UPDATE')
        //    correctly update the appropriate metric, statically cache the
        //    identified metrics identifier, and respect the supplied
        //    categories 'enabled' property.
        //
        // Plan:
        //   Verify that invoking the macros without a default metrics manager
        //   has no effect.
        //
        //   Specify a set S of unique object values having various minor or
        //   subtle differences.  For each value create an "oracle" collector,
        //   then invoke 'BALM_METRICS_UPDATE' and 'BALM_METRICS_INCREMENT' on
        //   the identified metric and perform the corresponding operation on
        //   the "oracle" collector.  Verify the collector underlying the
        //   metric has the same value as the "oracle" collector.  Also verify
        //   that only the collector for the first identified metric is
        //   updated.
        //
        //   Perform the same test again, but enabled or disable the metric's
        //   category on each iteration.
        //
        // Testing:
        //    BALM_METRICS_UPDATE(CATEGORY, NAME, VALUE)
        //    BALM_METRICS_TYPED_UPDATE(CATEGORY, NAME,PUBLICATION_TYPE,VALUE)
        //    BALM_METRICS_INCREMENT(CATEGORY, NAME)
        //    BALM_METRICS_UPDATE2(CATEGORY, METRIC1, VALUE1, ...)
        //    BALM_METRICS_UPDATE3(CATEGORY, METRIC1, VALUE1, ...)
        //    BALM_METRICS_UPDATE4(CATEGORY, METRIC1, VALUE1, ...)
        //    BALM_METRICS_UPDATE5(CATEGORY, METRIC1, VALUE1, ...)
        //    BALM_METRICS_UPDATE6(CATEGORY, METRIC1, VALUE1, ...)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING: STANDARD MACROS\n"
                          << "========================\n";

        const char *IDS[] = {"A", "B", "AB", "TEST"};
        const int NUM_IDS = sizeof IDS / sizeof *IDS;
        double UPDATES[] = { 0.0, 12.0, -1321123, 2131241, 1321.5,
                             43145.1, .0001, -1.00001, -.002342};
        const int NUM_UPDATES = sizeof(UPDATES)/sizeof(*UPDATES);

        if (veryVerbose)
            cout << "\tverify macros are a no-op without a metrics manager.\n";
        {
            for (int i = 0; i < NUM_IDS; ++i) {
                BALM_METRICS_UPDATE(IDS[i], "update", UPDATES[i]);
                BALM_METRICS_TYPED_UPDATE(IDS[i], "t-up", UPDATES[i],
                                         Type::e_TOTAL);
            }
        }

        if (veryVerbose)
            cout << "\tverify macros are applied correctly.\n";

        {
            BALM::DefaultMetricsManagerScopedGuard guard(Z);
            BALM::MetricsManager& mgr = *DefaultManager::instance();
            Registry&   registry   = mgr.metricRegistry();
            Repository& repository = mgr.collectorRepository();

            BALM::MetricId updateId(registry.getId(IDS[0], "update"));
            BALM::MetricId tupdateId(registry.getId(IDS[0], "Tupdate"));

            BALM::Collector expUpdate(updateId);
            BALM::Collector expTUpdate(tupdateId);
            for (int i = 0; i < NUM_IDS; ++i) {
                for (int j = 0; j < NUM_UPDATES; ++j) {
                    BALM_METRICS_UPDATE(IDS[i], "update", UPDATES[j]);
                    BALM_METRICS_TYPED_UPDATE(IDS[i], "Tupdate", UPDATES[j],
                                             Type::e_TOTAL);
                    expUpdate.update(UPDATES[j]);
                    expTUpdate.update(UPDATES[j]);
                }
            }
            ASSERT(Type::e_TOTAL ==
                   tupdateId.description()->preferredPublicationType());

            BALM::Collector *upCol  = repository.getDefaultCollector(updateId);
            BALM::Collector *tupCol =
                                     repository.getDefaultCollector(tupdateId);

            ASSERT(recordVal(&expUpdate)     == recordVal(upCol));
            ASSERT(recordVal(&expTUpdate)    == recordVal(tupCol));

            // Verify only the first identified metric (i.e., a 'CATEGORY',
            // 'NAME' pair) for which the macro was invoked was modified.
            ColSPtrVector cols(Z);
            IColSPtrVector intCols(Z);
            for (int i = 1; i < NUM_IDS; ++i) {
                BALM::MetricId updateId(registry.getId(IDS[i], "update"));
                BALM::MetricId tupdateId(registry.getId(IDS[i], "Tupdate"));

                ASSERT(0 == repository.getAddedCollectors(&cols,
                                                          &intCols,
                                                          updateId));
                ASSERT(0 == repository.getAddedCollectors(&cols,
                                                          &intCols,
                                                          tupdateId));
            }
        }
        if (veryVerbose)
            cout << "\tverify macros respects 'isActive' flag.\n";

        {
            BALM::DefaultMetricsManagerScopedGuard guard(Z);
            BALM::MetricsManager& mgr = *DefaultManager::instance();
            Registry&   registry     = mgr.metricRegistry();
            Repository& repository   = mgr.collectorRepository();

            BALM::MetricId updateId(registry.getId(IDS[0], "update"));
            BALM::MetricId tupdateId(registry.getId(IDS[0], "Tupdate"));

            BALM::Collector expUpdate(updateId);
            BALM::Collector expTUpdate(tupdateId);
            for (int i = 0; i < NUM_IDS; ++i) {
                for (int j = 0; j < NUM_UPDATES; ++j) {
                    bool enabled = 0 == j % 2;
                    registry.setCategoryEnabled(updateId.category(), enabled);
                    BALM_METRICS_UPDATE(IDS[i], "update", UPDATES[j]);
                    BALM_METRICS_TYPED_UPDATE(IDS[i], "Tupdate", UPDATES[j],
                                             Type::e_MIN);
                    if (enabled) {
                        expUpdate.update(UPDATES[j]);
                        expTUpdate.update(UPDATES[j]);
                    }
                }
            }

            ASSERT(Type::e_MIN ==
                   tupdateId.description()->preferredPublicationType());

            BALM::Collector *upCol  = repository.getDefaultCollector(updateId);
            BALM::Collector *tupCol =
                                     repository.getDefaultCollector(tupdateId);

            ASSERT(recordVal(&expUpdate)     == recordVal(upCol));
            ASSERT(recordVal(&expTUpdate)    == recordVal(tupCol));

            ASSERT(0 == defaultAllocator.numBytesInUse());

            // Verify only the first identified metric (i.e., a 'CATEGORY',
            // 'NAME' pair) for which the macro was invoked was modified.
            ColSPtrVector  cols(Z);
            IColSPtrVector intCols(Z);
            for (int i = 1; i < NUM_IDS; ++i) {
                BALM::MetricId updateId(registry.getId(IDS[i], "update"));
                BALM::MetricId tupdateId(registry.getId(IDS[i], "Tupdate"));

                ASSERT(0 == repository.getAddedCollectors(&cols,
                                                          &intCols,
                                                          updateId));
                ASSERT(0 == repository.getAddedCollectors(&cols,
                                                          &intCols,
                                                          tupdateId));
            }
        }
        if (veryVerbose)
            cout << "\tverify macro behaves correctly if the default metrics "
                 << "manager is destroyed (and recreated).\n";
        {
            for (int testIter = 0; testIter < 5; ++testIter) {
                // Create a new default metrics manager for this test.

                bool initializeDefault = (0 == testIter % 2);
                BALM::MetricsManager *mgr = 0;
                if (initializeDefault) {
                    mgr = BALM::DefaultMetricsManager::create(Z);
                }
                else {
                    mgr = new (*Z) BALM::MetricsManager(Z);
                }
                Registry&   registry   = mgr->metricRegistry();
                Repository& repository = mgr->collectorRepository();

                BALM::MetricId updateId(registry.getId(IDS[0], "update"));
                BALM::MetricId tupdateId(registry.getId(IDS[0], "Tupdate"));

                BALM::Collector expUpdate(updateId);
                BALM::Collector expTUpdate(tupdateId);
                for (int i = 0; i < NUM_IDS; ++i) {
                    for (int j = 0; j < NUM_UPDATES; ++j) {
                        BALM_METRICS_UPDATE(IDS[i], "update", UPDATES[j]);
                        BALM_METRICS_TYPED_UPDATE(IDS[i],
                                                  "Tupdate",
                                                  UPDATES[j],
                                                  Type::e_TOTAL);
                        expUpdate.update(UPDATES[j]);
                        expTUpdate.update(UPDATES[j]);
                    }
                }
                if (initializeDefault) {
                    BALM::Collector *upCol   =
                                      repository.getDefaultCollector(updateId);
                    BALM::Collector *tupCol  =
                                     repository.getDefaultCollector(tupdateId);

                    ASSERT(recordVal(&expUpdate)     == recordVal(upCol));
                    ASSERT(recordVal(&expTUpdate)    == recordVal(tupCol));
                    BALM::DefaultMetricsManager::destroy();
                }
                else {
                    Z->deleteObjectRaw(mgr);
                }
            }

            ASSERT(0 == defaultAllocator.numBytesInUse());
        }
        if (veryVerbose) cout << "\tVerify multiple metric variants.\n";
        {
            BALM::DefaultMetricsManagerScopedGuard guard(Z);
            BALM::MetricsManager  *manager =
                                       BALM::DefaultMetricsManager::instance();
            BALM::MetricRegistry& registry = manager->metricRegistry();
            BALM::CollectorRepository& repository =
                                                manager->collectorRepository();

            ASSERT(0 == defaultAllocator.numBytesInUse());

            const Id A_ID = registry.getId("A", "A");
            const Id B_ID = registry.getId("A", "B");
            const Id C_ID = registry.getId("A", "C");
            const Id D_ID = registry.getId("A", "D");
            const Id E_ID = registry.getId("A", "E");
            const Id F_ID = registry.getId("A", "F");
            const Id G_ID = registry.getId("A", "G");

            for (int i = 0; i < 10; ++i) {
                BALM_METRICS_UPDATE2("A",
                                     "A", 1.0, "B", 1.0);
                BALM_METRICS_UPDATE3("A",
                                     "A", 2.0, "B", 2.0, "C", 2.0);
                BALM_METRICS_UPDATE4("A",
                                     "A", 3.0, "B", 3.0, "C", 3.0, "D", 3.0);
                BALM_METRICS_UPDATE5("A",
                                     "A", 4.0, "B", 4.0, "C", 4.0, "D", 4.0,
                                     "E", 4.0);
                BALM_METRICS_UPDATE6("A",
                                     "A", 5.0, "B", 5.0, "C", 5.0, "D", 5.0,
                                     "E", 5.0, "F", 5.0);
            }

            BALM::Collector *A_COL = repository.getDefaultCollector("A", "A");
            BALM::Collector *B_COL = repository.getDefaultCollector("A", "B");
            BALM::Collector *C_COL = repository.getDefaultCollector("A", "C");
            BALM::Collector *D_COL = repository.getDefaultCollector("A", "D");
            BALM::Collector *E_COL = repository.getDefaultCollector("A", "E");
            BALM::Collector *F_COL = repository.getDefaultCollector("A", "F");
            BALM::Collector *G_COL = repository.getDefaultCollector("A", "G");

            if (veryVeryVerbose) {
                P_(recordVal(A_COL)); P_(recordVal(B_COL));
                P_(recordVal(C_COL)); P(recordVal(D_COL));
                P_(recordVal(E_COL)); P_(recordVal(F_COL));
                P(recordVal(G_COL));
            }
            ASSERT(
                  BALM::MetricRecord(A_ID, 50, 150, 1, 5) == recordVal(A_COL));
            ASSERT(
                  BALM::MetricRecord(B_ID, 50, 150, 1, 5) == recordVal(B_COL));
            ASSERT(
                  BALM::MetricRecord(C_ID, 40, 140, 2, 5) == recordVal(C_COL));
            ASSERT(
                  BALM::MetricRecord(D_ID, 30, 120, 3, 5) == recordVal(D_COL));
            ASSERT(
                  BALM::MetricRecord(E_ID, 20,  90, 4, 5) == recordVal(E_COL));
            ASSERT(
                  BALM::MetricRecord(F_ID, 10,  50, 5, 5) == recordVal(F_COL));
            ASSERT(
                  BALM::MetricRecord(G_ID)                == recordVal(G_COL));

            ColSPtrVector  cols(Z);
            IColSPtrVector intCols(Z);
            ASSERT(0 == repository.getAddedCollectors(&cols,&intCols,A_ID));
            ASSERT(0 == repository.getAddedCollectors(&cols,&intCols,B_ID));
            ASSERT(0 == repository.getAddedCollectors(&cols,&intCols,C_ID));
            ASSERT(0 == repository.getAddedCollectors(&cols,&intCols,D_ID));
            ASSERT(0 == repository.getAddedCollectors(&cols,&intCols,E_ID));
            ASSERT(0 == repository.getAddedCollectors(&cols,&intCols,F_ID));
            ASSERT(0 == repository.getAddedCollectors(&cols,&intCols,G_ID));
            ASSERT(0 == defaultAllocator.numBytesInUse());
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

        if (veryVerbose)
            cout << "\tTesting BALM_METRICS_UPDATE, BALM_METRICS_INCREMENT"
                 << endl;
        {
            BALM::DefaultMetricsManagerScopedGuard guard(Z);
            ASSERT(0 == defaultAllocator.numBytesInUse());

            BALM::MetricsManager  *manager =
                                       BALM::DefaultMetricsManager::instance();
            BALM::MetricRegistry& registry = manager->metricRegistry();
            BALM::CollectorRepository& repository =
                                                manager->collectorRepository();

            ASSERT(0 == defaultAllocator.numBytesInUse());

            const Id A_ID = registry.getId("A", "A");
            const Id B_ID = registry.getId("B", "B");
            const Id C_ID = registry.getId("C", "C");
            const Id D_ID = registry.getId("D", "D");
            const Id E_ID = registry.getId("E", "E");
            const Id F_ID = registry.getId("F", "F");

            for (int i = 0; i < 10; ++i) {
                BALM_METRICS_UPDATE("A", "A", 5);
                BALM_METRICS_DYNAMIC_UPDATE("B", "B", 5);

            }
            ColSPtrVector  cols(Z);
            IColSPtrVector intCols(Z);
            ASSERT(0 == repository.getAddedCollectors(&cols,&intCols,A_ID));
            ASSERT(0 == repository.getAddedCollectors(&cols,&intCols,B_ID));

            BALM::Collector *A_COL = repository.getDefaultCollector("A", "A");
            BALM::Collector *B_COL = repository.getDefaultCollector("B", "B");
            BALM::Collector *C_COL = repository.getDefaultCollector("C", "C");

            ASSERT(BALM::MetricRecord(A_ID, 10, 50, 5, 5) == recordVal(A_COL));
            ASSERT(BALM::MetricRecord(B_ID, 10, 50, 5, 5) == recordVal(B_COL));
            ASSERT(BALM::MetricRecord(C_ID) == recordVal(C_COL));

            A_COL->reset(); B_COL->reset(); C_COL->reset();

            const char *IDS[] = {"A", "B", "C" };
            const int NUM_IDS = sizeof IDS / sizeof *IDS;
            for (int i = 0; i < NUM_IDS; ++i) {
                BALM_METRICS_UPDATE(IDS[i], IDS[i], i + 1);
            }
            ASSERT(0 == repository.getAddedCollectors(&cols,&intCols,A_ID));
            ASSERT(0 == repository.getAddedCollectors(&cols,&intCols,B_ID));
            ASSERT(0 == repository.getAddedCollectors(&cols,&intCols,C_ID));

            ASSERT(BALM::MetricRecord(A_ID, 3, 6, 1, 3) == recordVal(A_COL));
            ASSERT(BALM::MetricRecord(B_ID) == recordVal(B_COL));
            ASSERT(BALM::MetricRecord(C_ID) == recordVal(C_COL));

            A_COL->reset(); B_COL->reset(); C_COL->reset();
            for (int i = 0; i < NUM_IDS; ++i) {
                BALM_METRICS_DYNAMIC_UPDATE(IDS[i], IDS[i], i + 1);
            }
            ASSERT(0 == repository.getAddedCollectors(&cols,&intCols,A_ID));
            ASSERT(0 == repository.getAddedCollectors(&cols,&intCols,B_ID));
            ASSERT(0 == repository.getAddedCollectors(&cols,&intCols,C_ID));

            ASSERT(BALM::MetricRecord(A_ID, 1, 1, 1, 1) == recordVal(A_COL));
            ASSERT(BALM::MetricRecord(B_ID, 1, 2, 2, 2) == recordVal(B_COL));
            ASSERT(BALM::MetricRecord(C_ID, 1, 3, 3, 3) == recordVal(C_COL));

            A_COL->reset(); B_COL->reset();

            ASSERT(Type::e_UNSPECIFIED ==
                   A_ID.description()->preferredPublicationType());
            ASSERT(Type::e_UNSPECIFIED ==
                   B_ID.description()->preferredPublicationType());

            if (veryVerbose)
                cout << "\tTesting TYPED MACROS" << endl;

            for (int i = 0; i < 10; ++i) {
                BALM_METRICS_TYPED_UPDATE("A", "A", 5.0, Type::e_RATE);
                BALM_METRICS_TYPED_UPDATE("B", "B", 1.0, Type::e_MIN);
            }
            ASSERT(BALM::MetricRecord(A_ID, 10, 50, 5, 5) == recordVal(A_COL));
            ASSERT(BALM::MetricRecord(B_ID, 10, 10, 1, 1) == recordVal(B_COL));

            ASSERT(Type::e_RATE ==
                   A_ID.description()->preferredPublicationType());
            ASSERT(Type::e_MIN  ==
                   B_ID.description()->preferredPublicationType());
        }

        if (veryVerbose)
            cout << "\tTesting BALM_METRICS_IF_CATEGORY_ENABLED"
                 << endl;
        {
            ASSERT(!breathingTestIfEnabledA());

            BALM::DefaultMetricsManagerScopedGuard guard(Z);
            BALM::MetricsManager *manager  =
                                       BALM::DefaultMetricsManager::instance();
            BALM::MetricRegistry& registry = manager->metricRegistry();

            ASSERT(breathingTestIfEnabledA());
            registry.setCategoryEnabled(registry.getCategory("A"), false);
            ASSERT(!breathingTestIfEnabledA());

            registry.setCategoryEnabled(registry.getCategory("A"), true);
            ASSERT(breathingTestIfEnabledA());
        }
        ASSERT(!breathingTestIfEnabledA());

        if (veryVerbose) {
            cout << "\tTesting BALM_METRICS_UPDATE[0-5]" << bsl::endl;
        }
        {
            BALM::DefaultMetricsManagerScopedGuard guard(Z);
            BALM::MetricsManager  *manager =
                                       BALM::DefaultMetricsManager::instance();
            BALM::MetricRegistry& registry = manager->metricRegistry();
            BALM::CollectorRepository& repository =
                                                manager->collectorRepository();

            ASSERT(0 == defaultAllocator.numBytesInUse());

            const Id A_ID = registry.getId("A", "A");
            const Id B_ID = registry.getId("A", "B");
            const Id C_ID = registry.getId("A", "C");
            const Id D_ID = registry.getId("A", "D");
            const Id E_ID = registry.getId("A", "E");
            const Id F_ID = registry.getId("A", "F");
            const Id G_ID = registry.getId("A", "G");

            for (int i = 0; i < 10; ++i) {
                BALM_METRICS_UPDATE2("A",
                                     "A", 1.0, "B", 1.0);
                BALM_METRICS_UPDATE3("A",
                                     "A", 2.0, "B", 2.0, "C", 2.0);
                BALM_METRICS_UPDATE4("A",
                                     "A", 3.0, "B", 3.0, "C", 3.0, "D", 3.0);
                BALM_METRICS_UPDATE5("A",
                                     "A", 4.0, "B", 4.0, "C", 4.0, "D", 4.0,
                                     "E", 4.0);
                BALM_METRICS_UPDATE6("A",
                                     "A", 5.0, "B", 5.0, "C", 5.0, "D", 5.0,
                                     "E", 5.0, "F", 5.0);
            }

            BALM::Collector *A_COL = repository.getDefaultCollector("A", "A");
            BALM::Collector *B_COL = repository.getDefaultCollector("A", "B");
            BALM::Collector *C_COL = repository.getDefaultCollector("A", "C");
            BALM::Collector *D_COL = repository.getDefaultCollector("A", "D");
            BALM::Collector *E_COL = repository.getDefaultCollector("A", "E");
            BALM::Collector *F_COL = repository.getDefaultCollector("A", "F");
            BALM::Collector *G_COL = repository.getDefaultCollector("A", "G");

            if (veryVeryVerbose) {
                P_(recordVal(A_COL)); P_(recordVal(B_COL));
                P_(recordVal(C_COL)); P(recordVal(D_COL));
                P_(recordVal(E_COL)); P_(recordVal(F_COL));
                P(recordVal(G_COL));
            }
            ASSERT(BALM::MetricRecord(A_ID, 50, 150, 1, 5) ==
                                                             recordVal(A_COL));
            ASSERT(BALM::MetricRecord(B_ID, 50, 150, 1, 5) ==
                                                             recordVal(B_COL));
            ASSERT(BALM::MetricRecord(C_ID, 40, 140, 2, 5) ==
                                                             recordVal(C_COL));
            ASSERT(BALM::MetricRecord(D_ID, 30, 120, 3, 5) ==
                                                             recordVal(D_COL));
            ASSERT(BALM::MetricRecord(E_ID, 20,  90, 4, 5) ==
                                                             recordVal(E_COL));
            ASSERT(BALM::MetricRecord(F_ID, 10,  50, 5, 5) ==
                                                             recordVal(F_COL));
            ASSERT(BALM::MetricRecord(G_ID)                ==
                                                             recordVal(G_COL));

            ColSPtrVector  cols(Z);
            IColSPtrVector intCols(Z);
            ASSERT(0 == repository.getAddedCollectors(&cols,&intCols,A_ID));
            ASSERT(0 == repository.getAddedCollectors(&cols,&intCols,B_ID));
            ASSERT(0 == repository.getAddedCollectors(&cols,&intCols,C_ID));
            ASSERT(0 == repository.getAddedCollectors(&cols,&intCols,D_ID));
            ASSERT(0 == repository.getAddedCollectors(&cols,&intCols,E_ID));
            ASSERT(0 == repository.getAddedCollectors(&cols,&intCols,F_ID));
            ASSERT(0 == repository.getAddedCollectors(&cols,&intCols,G_ID));
            ASSERT(0 == defaultAllocator.numBytesInUse());
        }
        if (veryVerbose) {
            cout << "\tTesting BALM_METRICS_INT_UPDATE[0-5]" << bsl::endl;
        }
        {
            BALM::DefaultMetricsManagerScopedGuard guard(Z);
            BALM::MetricsManager  *manager =
                                 BALM::DefaultMetricsManager::instance();
            BALM::MetricRegistry& registry = manager->metricRegistry();
            BALM::CollectorRepository& repository =
                                                manager->collectorRepository();

            ASSERT(0 == defaultAllocator.numBytesInUse());

            const Id A_ID = registry.getId("A", "A");
            const Id B_ID = registry.getId("A", "B");
            const Id C_ID = registry.getId("A", "C");
            const Id D_ID = registry.getId("A", "D");
            const Id E_ID = registry.getId("A", "E");
            const Id F_ID = registry.getId("A", "F");
            const Id G_ID = registry.getId("A", "G");

            for (int i = 0; i < 10; ++i) {
                BALM_METRICS_INT_UPDATE2("A",
                                         "A", 1, "B", 1);
                BALM_METRICS_INT_UPDATE3("A",
                                         "A", 2, "B", 2, "C", 2);
                BALM_METRICS_INT_UPDATE4("A",
                                         "A", 3, "B", 3, "C", 3, "D", 3);
                BALM_METRICS_INT_UPDATE5("A",
                                         "A", 4, "B", 4, "C", 4, "D", 4,
                                         "E", 4);
                BALM_METRICS_INT_UPDATE6("A",
                                         "A", 5, "B", 5, "C", 5, "D", 5,
                                         "E", 5, "F", 5);
            }

            BALM::IntegerCollector *A_COL =
                repository.getDefaultIntegerCollector("A", "A");
            BALM::IntegerCollector *B_COL =
                repository.getDefaultIntegerCollector("A", "B");
            BALM::IntegerCollector *C_COL =
                repository.getDefaultIntegerCollector("A", "C");
            BALM::IntegerCollector *D_COL =
                repository.getDefaultIntegerCollector("A", "D");
            BALM::IntegerCollector *E_COL =
                repository.getDefaultIntegerCollector("A", "E");
            BALM::IntegerCollector *F_COL =
                repository.getDefaultIntegerCollector("A", "F");
            BALM::IntegerCollector *G_COL =
                repository.getDefaultIntegerCollector("A", "G");

            if (veryVeryVerbose) {
                P_(recordVal(A_COL)); P_(recordVal(B_COL));
                P_(recordVal(C_COL)); P(recordVal(D_COL));
                P_(recordVal(E_COL)); P_(recordVal(F_COL));
                P(recordVal(G_COL));
            }
            ASSERT(BALM::MetricRecord(A_ID, 50, 150, 1, 5) ==
                                                             recordVal(A_COL));
            ASSERT(BALM::MetricRecord(B_ID, 50, 150, 1, 5) ==
                                                             recordVal(B_COL));
            ASSERT(BALM::MetricRecord(C_ID, 40, 140, 2, 5) ==
                                                             recordVal(C_COL));
            ASSERT(BALM::MetricRecord(D_ID, 30, 120, 3, 5) ==
                                                             recordVal(D_COL));
            ASSERT(BALM::MetricRecord(E_ID, 20,  90, 4, 5) ==
                                                             recordVal(E_COL));
            ASSERT(BALM::MetricRecord(F_ID, 10,  50, 5, 5) ==
                                                             recordVal(F_COL));
            ASSERT(BALM::MetricRecord(G_ID)                ==
                                                             recordVal(G_COL));

            ColSPtrVector  cols(Z);
            IColSPtrVector intCols(Z);
            ASSERT(0 == repository.getAddedCollectors(&cols,&intCols,A_ID));
            ASSERT(0 == repository.getAddedCollectors(&cols,&intCols,B_ID));
            ASSERT(0 == repository.getAddedCollectors(&cols,&intCols,C_ID));
            ASSERT(0 == repository.getAddedCollectors(&cols,&intCols,D_ID));
            ASSERT(0 == repository.getAddedCollectors(&cols,&intCols,E_ID));
            ASSERT(0 == repository.getAddedCollectors(&cols,&intCols,F_ID));
            ASSERT(0 == repository.getAddedCollectors(&cols,&intCols,G_ID));
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
