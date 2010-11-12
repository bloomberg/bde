// baem_metrics.t.cpp  -*-C++-*-
#include <baem_metrics.h>

#include <baem_metricregistry.h>
#include <baem_metricsample.h>
#include <baem_publisher.h>

#include <bcema_testallocator.h>
#include <bcemt_barrier.h>
#include <bcep_fixedthreadpool.h>

#include <bdef_bind.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bsls_assert.h>

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
// The macros defined in this component primarily provide user-friendly access
// to a 'baem_Collector' object.  Most of the tests can be performed by
// creating an "oracle" 'baem_Collector' object and verifying that operations
// performed on with a macro have the same effect as the same operation
// performed directly on the "oracle" 'baem_Collector'.  Note that, the
// "STANDARD" macro variants (i.e., 'BAEM_METRICS_UPDATE') and the
// "THREAD_LOCAL" macro variants (i.e., 'BAEM_METRICS_THREAD_LOCAL_UPDATE')
// have more complex behavior with respect to their statically cached (or
// thread-local statically cached) 'baem_Collector' object.
//-----------------------------------------------------------------------------
// MACROS
// [ 2] BAEM_METRICS_UPDATE(CATEGORY, NAME, VALUE)
// [ 2] BAEM_METRICS_INCREMENT(CATEGORY, NAME)
// [ 2] BAEM_METRICS_TYPED_UPDATE(CATEGORY, NAME, VALUE, PREFERRED_TYPE)
// [ 2] BAEM_METRICS_TYPED_INCREMENT(CATEGORY, NAME, PREFERRED_TYPE)
// [ 3] BAEM_METRICS_DYNAMIC_UPDATE(CATEGORY, NAME, VALUE)
// [ 3] BAEM_METRICS_DYNAMIC_INCREMENT(CATEGORY, NAME)
// [ 4] BAEM_METRICS_THREAD_LOCAL_UPDATE(CATEGORY, NAME, VALUE)
// [ 4] BAEM_METRICS_THREAD_LOCAL_INCREMENT(CATEGORY, NAME)
// [ 5] BAEM_METRICS_INT_UPDATE(CATEGORY, NAME, VALUE)
// [ 5] BAEM_METRICS_INT_INCREMENT(CATEGORY, NAME)
// [ 5] BAEM_METRICS_TYPED_INT_UPDATE(CATEGORY, NAME, VALUE)
// [ 5] BAEM_METRICS_TYPED_INT_INCREMENT(CATEGORY, NAME)
// [ 6] BAEM_METRICS_DYNAMIC_INT_UPDATE(CATEGORY, NAME, VALUE)
// [ 6] BAEM_METRICS_DYNAMIC_INT_INCREMENT(CATEGORY, NAME)
// [ 7] BAEM_METRICS_THREAD_LOCAL_INT_UPDATE(CATEGORY, NAME, VALUE)
// [ 7] BAEM_METRICS_THREAD_LOCAL_INT_INCREMENT(CATEGORY, NAME)
// [ 8] BEAM_METRICS_TIME_BLOCK(CATEGORY, METRIC, TIME_UNITS)
// [ 8] BAEM_METRICS_DYNAMIC_TIME_BLOCK(CATEGORY, METRIC, TIME_UNITS)
// [ 9] BEAM_METRICS_TIME_BLOCK_SECONDS(CATEGORY, METRIC)
// [ 9] BEAM_METRICS_TIME_BLOCK_MILLISECONDS(CATEGORY, METRIC)
// [ 9] BEAM_METRICS_TIME_BLOCK_MICROSECONDS(CATEGORY, METRIC)
// [ 9] BEAM_METRICS_TIME_BLOCK_NANOSECONDS(CATEGORY, METRIC)
// [ 9] BAEM_METRICS_DYNAMIC_TIME_BLOCK_SECONDS(CATEGORY, METRIC)
// [ 9] BAEM_METRICS_DYNAMIC_TIME_BLOCK_MILLISECONDS(CATEGORY, METRIC)
// [ 9] BAEM_METRICS_DYNAMIC_TIME_BLOCK_MICROSECONDS(CATEGORY, METRIC)
// [ 9] BAEM_METRICS_DYNAMIC_TIME_BLOCK_NANOSECONDS(CATEGORY, METRIC)
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [11] CONCURRENCY TEST: STANDARD MACROS
// [12] CONCURRENCY TEST: DYNAMIC MACROS
// [13] CONCURRENCY TEST: THREAD_LOCAL MACROS
// [14] CONCURRENCY TEST: STANDARD INT MACROS
// [15] CONCURRENCY TEST: DYNAMIC INT MACROS
// [16] CONCURRENCY TEST: THREAD_LOCAL INT MACROS
// [17] USAGE EXAMPLE
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
typedef baem_IntegerCollector      IntCollector;
typedef baem_MetricId              Id;
typedef baem_Category              Category;
typedef baem_PublicationType       Type;
typedef baem_MetricsManager        MetricsManager;
typedef baem_StopwatchScopedGuard  SWGuard;

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

inline
baem_MetricRecord recordVal(const baem_IntegerCollector *collector)
    // Return the current record value of the specified 'collector'.
{
    baem_MetricRecord record;
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
                             SWGuard::Units  units = SWGuard::BAEM_SECONDS)
    // Record, using the static 'BAEM_TIME_BLOCK' macro, an empty function to
    // a metric identified by the specified 'category' and 'name'.  Optionally
    // specify the 'units' to report elapsed time values.  If no 'units' value
    // is supplied, time is reported in seconds.  Note that the identity of
    // the metric is defined on the *first* invocation of this macro.
{
    BAEM_METRICS_TIME_BLOCK(category, metric, units);
}

void dynamicTimeEmptyFunction(const char *category, const char *metric)
    // Record, using the 'BAEM_DYNAMIC_TIME_BLOCK_SECONDS' macro, an empty
    // function to a metric identified by the specified 'category' and 'name'.
{
    BAEM_METRICS_DYNAMIC_TIME_BLOCK_SECONDS(category, metric);
}

bool breathingTestIfEnabledA()
{
    BAEM_METRICS_IF_CATEGORY_ENABLED("A") {
        return true;
    }
    return false;
}

bool baemMetricsIfCategoryEnabledTestA()
{
    BAEM_METRICS_IF_CATEGORY_ENABLED("A") {
        return true;
    }
    return false;
}

bool baemMetricsIfCategoryEnabledTest(const char *category)
{
    BAEM_METRICS_IF_CATEGORY_ENABLED(category) {
        return true;
    }
    return false;
}

// ------------------- case 11 StandardMacroConcurrencyTest -----------------

class StandardMacroConcurrencyTest {
    // Invoke a set of operations operations synchronously.

    // DATA
    bcep_FixedThreadPool   d_pool;
    bcemt_Barrier          d_barrier;
    bslma_Allocator       *d_allocator_p;

    // PRIVATE MANIPULATORS
    void execute();
        // Execute a single test.

  public:

    // CREATORS
    StandardMacroConcurrencyTest(int              numThreads,
                                 bslma_Allocator *basicAllocator)
    : d_pool(numThreads, 1000, basicAllocator)
    , d_barrier(numThreads)
    , d_allocator_p(basicAllocator)
    {
        d_pool.start();
    }

    ~StandardMacroConcurrencyTest() {}

    //  MANIPULATORS
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

    baem_MetricsManager &mgr = *DefaultManager::instance();
    Repository&   repository = mgr.collectorRepository();

    d_barrier.wait();
    for (int i = 0; i < COUNT; ++i) {
        BAEM_METRICS_UPDATE("A",
                            "A", 1.0);
        BAEM_METRICS_UPDATE2("A",
                             "A", 2.0, "B", 2.0);
        BAEM_METRICS_UPDATE3("A",
                             "A", 3.0, "B", 3.0, "C", 3.0);
        BAEM_METRICS_UPDATE4("A",
                             "A", 4.0, "B", 4.0, "C", 4.0, "D", 4.0);
        BAEM_METRICS_UPDATE5("A",
                             "A", 5.0, "B", 5.0, "C", 5.0, "D", 5.0,
                             "E", 5.0);
        BAEM_METRICS_UPDATE6("A",
                             "A", 6.0, "B", 6.0, "C", 6.0, "D", 6.0,
                             "E", 6.0, "F", 6.0);
        BAEM_METRICS_TYPED_UPDATE("A", "A", 7.0, Type::BAEM_TOTAL);

        bool enabled = 0 == i % 2;

        mgr.setCategoryEnabled("B", enabled);

        BAEM_METRICS_UPDATE("B",
                            "A", 1.0);
        BAEM_METRICS_UPDATE2("B",
                             "A", 2.0, "B", 2.0);
        BAEM_METRICS_UPDATE3("B",
                             "A", 3.0, "B", 3.0, "C", 3.0);
        BAEM_METRICS_UPDATE4("B",
                             "A", 4.0, "B", 4.0, "C", 4.0, "D", 4.0);
        BAEM_METRICS_UPDATE5("B",
                             "A", 5.0, "B", 5.0, "C", 5.0, "D", 5.0,
                             "E", 5.0);
        BAEM_METRICS_UPDATE6("B",
                             "A", 6.0, "B", 6.0, "C", 6.0, "D", 6.0,
                             "E", 6.0, "F", 6.0);
        BAEM_METRICS_TYPED_UPDATE("B", "A", 7.0, Type::BAEM_TOTAL);
    }
    d_barrier.wait();

    const int REPS = COUNT * d_pool.numThreads();
    {
        // Verify basic macro behavior for category A

        baem_Collector *A = repository.getDefaultCollector("A", "A");
        baem_Collector *B = repository.getDefaultCollector("A", "B");
        baem_Collector *C = repository.getDefaultCollector("A", "C");
        baem_Collector *D = repository.getDefaultCollector("A", "D");
        baem_Collector *E = repository.getDefaultCollector("A", "E");
        baem_Collector *F = repository.getDefaultCollector("A", "F");

        ASSERT(baem_MetricRecord(A->metricId(), 7 * REPS, 28 * REPS, 1, 7) ==
               recordVal(A));
        ASSERT(baem_MetricRecord(B->metricId(), 5 * REPS, 20 * REPS, 2, 6) ==
               recordVal(B));
        ASSERT(baem_MetricRecord(C->metricId(), 4 * REPS, 18 * REPS, 3, 6) ==
               recordVal(C));
        ASSERT(baem_MetricRecord(D->metricId(), 3 * REPS, 15 * REPS, 4, 6) ==
               recordVal(D));
        ASSERT(baem_MetricRecord(E->metricId(), 2 * REPS, 11 * REPS, 5, 6) ==
               recordVal(E));
        ASSERT(baem_MetricRecord(F->metricId(), 1 * REPS,  6 * REPS, 6, 6) ==
               recordVal(F));
    }
    {
        // Verify macros respect enabled status by examining
        // 'ENABLED_CATEGORY' metric values.
        baem_Collector *A = repository.getDefaultCollector("B", "A");
        baem_Collector *B = repository.getDefaultCollector("B", "B");
        baem_Collector *C = repository.getDefaultCollector("B", "C");
        baem_Collector *D = repository.getDefaultCollector("B", "D");
        baem_Collector *E = repository.getDefaultCollector("B", "E");
        baem_Collector *F = repository.getDefaultCollector("B", "F");

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
    bdef_Function<void(*)()> job = bdef_BindUtil::bindA(
                                 d_allocator_p,
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
    bcep_FixedThreadPool   d_pool;
    bcemt_Barrier          d_barrier;
    bslma_Allocator       *d_allocator_p;

    // PRIVATE MANIPULATORS
    void execute();
        // Execute a single test.

  public:
    // CREATORS
    DynamicMacroConcurrencyTest(int              numThreads,
                                 bslma_Allocator *basicAllocator)
    : d_pool(numThreads, 1000, basicAllocator)
    , d_barrier(numThreads)
    , d_allocator_p(basicAllocator)
    {
        d_pool.start();
    }

    ~DynamicMacroConcurrencyTest() {}

    //  MANIPULATORS
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

    baem_MetricsManager &mgr = *DefaultManager::instance();
    Repository&   repository = mgr.collectorRepository();
    const Category *C2 = mgr.metricRegistry().getCategory(ENABLED_CATEGORY);

    d_barrier.wait();
    for (int i = 0; i < COUNT; ++i) {
        BAEM_METRICS_DYNAMIC_UPDATE(TEST_CATEGORY, UPDATE, i);

        bool enabled = 0 == i % 2;

        mgr.setCategoryEnabled(C2, enabled);

        BAEM_METRICS_DYNAMIC_UPDATE(ENABLED_CATEGORY, UPDATE, i);
    }
    d_barrier.wait();
    const int NUM_THREADS = d_pool.numThreads();
    const int TOTAL = (COUNT * (COUNT - 1)) / 2;
    {
        // Verify basic macro behavior for 'TEST_CATEGORY'.

        baem_Collector *upCol  = repository.getDefaultCollector(TEST_CATEGORY,
                                                         UPDATE);

        ASSERT(COUNT * NUM_THREADS == recordVal(upCol).count());
        ASSERT(TOTAL * NUM_THREADS == recordVal(upCol).total());

        ASSERT(0 == recordVal(upCol).min());

        ASSERT(COUNT - 1 == recordVal(upCol).max());
    }
    {
        // Verify macros respect enabled status by examining
        // 'ENABLED_CATEGORY' metric values.

        baem_Collector *upCol  =
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
    bdef_Function<void(*)()> job = bdef_BindUtil::bindA(
                                        d_allocator_p,
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
    bcep_FixedThreadPool   d_pool;
    bcemt_Barrier          d_barrier;
    bslma_Allocator       *d_allocator_p;
    bcemt_Mutex            d_mutex;
    // PRIVATE MANIPULATORS
    void execute();
        // Execute a single test.

  public:

    // CREATORS
    TlsMacroConcurrencyTest(int              numThreads,
                                 bslma_Allocator *basicAllocator)
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
    // 'baem_CollectorRepository::collectAndReset', which modifies the metric
    // values as it collects them, so verification can only be performed on a
    // single thread.

    const int   COUNT            = 30;
    const char *TEST_CATEGORY    = "TestCategory";
    const char *ENABLED_CATEGORY = "EnabledCategory";
    const char *INCREMENT        = "Increment";
    const char *UPDATE           = "Update";

    baem_MetricsManager &mgr = *DefaultManager::instance();
    Repository& repository = mgr.collectorRepository();
    Registry&   registry   = mgr.metricRegistry();
    bslma_Allocator *Z = d_allocator_p;
    const Category *C2 = mgr.metricRegistry().getCategory(ENABLED_CATEGORY);

    d_barrier.wait();
    for (int i = 0; i < COUNT; ++i) {
        BAEM_METRICS_THREAD_LOCAL_UPDATE(TEST_CATEGORY, UPDATE, i);
        BAEM_METRICS_THREAD_LOCAL_INCREMENT(TEST_CATEGORY, INCREMENT);

        bool enabled = 0 == i % 2;

        mgr.setCategoryEnabled(C2, enabled);

        BAEM_METRICS_THREAD_LOCAL_UPDATE(ENABLED_CATEGORY, UPDATE, i);
        BAEM_METRICS_THREAD_LOCAL_INCREMENT(ENABLED_CATEGORY, INCREMENT);
    }
    d_barrier.wait();

    // Verification must be performed on a single thread because
    // 'baem_CollectorRepository::collectAndReset' modifies the object state
    // when it collects the metric values.
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
        baem_MetricId upId  = registry.getId(TEST_CATEGORY,
                                             UPDATE);
        baem_MetricId incId = registry.getId(TEST_CATEGORY,
                                             INCREMENT);

        bsl::vector<baem_Collector *> cols(Z);
        bsl::vector<baem_IntegerCollector *> intCols(Z);
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

        bsl::vector<baem_MetricRecord> records(Z);
        repository.collectAndReset(&records, upId.category());
        ASSERT(2 == records.size());
        baem_MetricRecord upRecord, incRecord;
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

        baem_MetricId upId  = registry.getId(ENABLED_CATEGORY, UPDATE);
        baem_MetricId incId = registry.getId(ENABLED_CATEGORY, INCREMENT);

        bsl::vector<baem_Collector *> cols(Z);
        bsl::vector<baem_IntegerCollector *> intCols(Z);
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

        bsl::vector<baem_MetricRecord> records(Z);
        repository.collectAndReset(&records, upId.category());
        ASSERT(2 == records.size());
        baem_MetricRecord upRecord, incRecord;
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
    bdef_Function<void(*)()> job = bdef_BindUtil::bindA(
                                 d_allocator_p,
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
    bcep_FixedThreadPool   d_pool;
    bcemt_Barrier          d_barrier;
    bslma_Allocator       *d_allocator_p;

    // PRIVATE MANIPULATORS
    void execute();
        // Execute a single test.

  public:

    // CREATORS
    StandardIntMacroConcurrencyTest(int              numThreads,
                                 bslma_Allocator *basicAllocator)
    : d_pool(numThreads, 1000, basicAllocator)
    , d_barrier(numThreads)
    , d_allocator_p(basicAllocator)
    {
        d_pool.start();
    }

    ~StandardIntMacroConcurrencyTest() {}

    //  MANIPULATORS
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
    baem_MetricsManager &mgr = *DefaultManager::instance();
    Repository&   repository = mgr.collectorRepository();
    d_barrier.wait();
    for (int i = 0; i < COUNT; ++i) {
        BAEM_METRICS_INCREMENT("A", "A");
        BAEM_METRICS_INT_UPDATE("A",
                                "A", 1);
        BAEM_METRICS_INT_UPDATE2("A",
                                 "A", 2, "B", 2);
        BAEM_METRICS_INT_UPDATE3("A",
                                 "A", 3, "B", 3, "C", 3);
        BAEM_METRICS_INT_UPDATE4("A",
                                 "A", 4, "B", 4, "C", 4, "D", 4);
        BAEM_METRICS_INT_UPDATE5("A",
                                 "A", 5, "B", 5, "C", 5, "D", 5,
                                 "E", 5);
        BAEM_METRICS_INT_UPDATE6("A",
                                 "A", 6, "B", 6, "C", 6, "D", 6,
                                 "E", 6, "F", 6);
        BAEM_METRICS_TYPED_INT_UPDATE("A", "A", 7, Type::BAEM_TOTAL);
        BAEM_METRICS_TYPED_INCREMENT("A", "A", Type::BAEM_TOTAL);

        bool enabled = 0 == i % 2;

        mgr.setCategoryEnabled("B", enabled);

        BAEM_METRICS_INCREMENT("B", "A");
        BAEM_METRICS_INT_UPDATE("B",
                                "A", 1);
        BAEM_METRICS_INT_UPDATE2("B",
                                 "A", 2, "B", 2);
        BAEM_METRICS_INT_UPDATE3("B",
                                 "A", 3, "B", 3, "C", 3);
        BAEM_METRICS_INT_UPDATE4("B",
                                 "A", 4, "B", 4, "C", 4, "D", 4);
        BAEM_METRICS_INT_UPDATE5("B",
                                 "A", 5, "B", 5, "C", 5, "D", 5,
                                 "E", 5);
        BAEM_METRICS_INT_UPDATE6("B",
                                 "A", 6, "B", 6, "C", 6, "D", 6,
                                 "E", 6, "F", 6);
        BAEM_METRICS_TYPED_INT_UPDATE("B", "A", 7, Type::BAEM_TOTAL);
        BAEM_METRICS_TYPED_INCREMENT("B", "A", Type::BAEM_TOTAL);
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

        ASSERT(baem_MetricRecord(A->metricId(), 9 * REPS, 30 * REPS, 1, 7) ==
               recordVal(A));
        ASSERT(baem_MetricRecord(B->metricId(), 5 * REPS, 20 * REPS, 2, 6) ==
               recordVal(B));
        ASSERT(baem_MetricRecord(C->metricId(), 4 * REPS, 18 * REPS, 3, 6) ==
               recordVal(C));
        ASSERT(baem_MetricRecord(D->metricId(), 3 * REPS, 15 * REPS, 4, 6) ==
               recordVal(D));
        ASSERT(baem_MetricRecord(E->metricId(), 2 * REPS, 11 * REPS, 5, 6) ==
               recordVal(E));
        ASSERT(baem_MetricRecord(F->metricId(), 1 * REPS,  6 * REPS, 6, 6) ==
               recordVal(F));
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
    bdef_Function<void(*)()> job = bdef_BindUtil::bindA(
                                    d_allocator_p,
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
    bcep_FixedThreadPool   d_pool;
    bcemt_Barrier          d_barrier;
    bslma_Allocator       *d_allocator_p;

    // PRIVATE MANIPULATORS
    void execute();
        // Execute a single test.

  public:
    // CREATORS
    DynamicIntMacroConcurrencyTest(int              numThreads,
                                 bslma_Allocator *basicAllocator)
    : d_pool(numThreads, 1000, basicAllocator)
    , d_barrier(numThreads)
    , d_allocator_p(basicAllocator)
    {
        d_pool.start();
    }

    ~DynamicIntMacroConcurrencyTest() {}

    //  MANIPULATORS
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

    baem_MetricsManager &mgr = *DefaultManager::instance();
    Repository&   repository = mgr.collectorRepository();
    const Category *C2 = mgr.metricRegistry().getCategory(ENABLED_CATEGORY);

    d_barrier.wait();
    for (int i = 0; i < COUNT; ++i) {
        BAEM_METRICS_DYNAMIC_INT_UPDATE(TEST_CATEGORY, UPDATE, i);
        BAEM_METRICS_DYNAMIC_INCREMENT(TEST_CATEGORY, INCREMENT);

        bool enabled = 0 == i % 2;

        mgr.setCategoryEnabled(C2, enabled);

        BAEM_METRICS_DYNAMIC_INT_UPDATE(ENABLED_CATEGORY, UPDATE, i);
        BAEM_METRICS_DYNAMIC_INCREMENT(ENABLED_CATEGORY, INCREMENT);
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
    bdef_Function<void(*)()> job = bdef_BindUtil::bindA(
                                      d_allocator_p,
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
    bcep_FixedThreadPool   d_pool;
    bcemt_Barrier          d_barrier;
    bslma_Allocator       *d_allocator_p;
    bcemt_Mutex            d_mutex;
    // PRIVATE MANIPULATORS
    void execute();
        // Execute a single test.

  public:

    // CREATORS
    TlsIntMacroConcurrencyTest(int              numThreads,
                                 bslma_Allocator *basicAllocator)
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
    // 'baem_CollectorRepository::collectAndReset', which modifies the metric
    // values as it collects them, so verification can only be performed on a
    // single thread.

    const int   COUNT            = 30;
    const char *TEST_CATEGORY    = "TestCategory";
    const char *ENABLED_CATEGORY = "EnabledCategory";
    const char *INCREMENT        = "Increment";
    const char *UPDATE           = "Update";

    baem_MetricsManager &mgr = *DefaultManager::instance();
    Repository& repository = mgr.collectorRepository();
    Registry&   registry   = mgr.metricRegistry();
    bslma_Allocator *Z = d_allocator_p;
    const Category *C2 = mgr.metricRegistry().getCategory(ENABLED_CATEGORY);

    d_barrier.wait();
    for (int i = 0; i < COUNT; ++i) {
        BAEM_UPDATE_THREAD_LOCAL_INT_METRIC(TEST_CATEGORY, UPDATE, i);
        BAEM_INCREMENT_THREAD_LOCAL_INT_METRIC(TEST_CATEGORY, INCREMENT);

        bool enabled = 0 == i % 2;

        mgr.setCategoryEnabled(C2, enabled);

        BAEM_UPDATE_THREAD_LOCAL_INT_METRIC(ENABLED_CATEGORY, UPDATE, i);
        BAEM_INCREMENT_THREAD_LOCAL_INT_METRIC(ENABLED_CATEGORY, INCREMENT);
    }
    d_barrier.wait();

    // Verification must be performed on a single thread because
    // 'baem_CollectorRepository::collectAndReset' modifies the object state
    // when it collects the metric values.
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
        baem_MetricId upId  = registry.getId(TEST_CATEGORY,
                                             UPDATE);
        baem_MetricId incId = registry.getId(TEST_CATEGORY,
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

        bsl::vector<baem_MetricRecord> records(Z);
        repository.collectAndReset(&records, upId.category());
        ASSERT(2 == records.size());
        baem_MetricRecord upRecord, incRecord;
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

        baem_MetricId upId  = registry.getId(ENABLED_CATEGORY, UPDATE);
        baem_MetricId incId = registry.getId(ENABLED_CATEGORY, INCREMENT);

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

        bsl::vector<baem_MetricRecord> records(Z);
        repository.collectAndReset(&records, upId.category());
        ASSERT(2 == records.size());
        baem_MetricRecord upRecord, incRecord;
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
    bdef_Function<void(*)()> job = bdef_BindUtil::bindA(
                                        d_allocator_p,
                                        &TlsIntMacroConcurrencyTest::execute,
                                        this);
    for (int i = 0; i < d_pool.numThreads(); ++i) {
        d_pool.enqueueJob(job);
    }
    d_pool.drain();
}

#endif

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Example 2 - Updating a Metric
///- - - - - - - - - - - - - - -
// Once a metrics manager is initialized, we can use the various macros to
// record metric values.  In this second example, we collect metrics from a
// hypothetical event-processing function.  We use 'BAEM_METRICS_UPDATE' to
// record the size of the data being processed to a metric named
// "msgSize", and the elapsed time (in milliseconds) to process the event to a
// metric named "elapsedTime".  Finally, we use 'BAEM_METRICS_INCREMENT'
// to record a count of failures to a metric named "failureCount".
// Note that we do not use the '*_DYNAMIC_*' variants of the
// 'BAEM_METRICS_UPDATE' or 'BAEM_METRICS_INCREMENT' macros because the
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

        BAEM_METRICS_UPDATE("processEvent", "msgSize", eventMessage.size());
        BAEM_METRICS_TIME_BLOCK_MILLISECONDS("processingEvent", "elapsedTime");

        // Process 'data'.

        if (0 != returnCode) {
            BAEM_METRICS_INCREMENT("processEvent", "failureCount");
        }

        return returnCode;
    }
//..
///Example 3 - Using 'BAEM_METRICS_IF_CATEGORY_ENABLED'
///- - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this next example, we use 'BAEM_METRICS_IF_CATEGORY_ENABLED' to
// conditionally disable a (relatively) expensive operation involved in
// computing a metric value.  The 'processEvent2' function, defined below, uses
// a 'bsls_Stopwatch' to record the elapsed system, user, and wall time,
// involved in processing the event.  The system calls used (via
// 'bsls_Stopwatch') to record the elapsed time may be relatively expensive,
// so we use 'BAEM_METRICS_IF_CATEGORY_ENABLED' to ensure we only perform
// those operations if metrics collection is enabled.  Finally, we use
// 'BAEM_METRICS_UPDATE3' to update the three metrics, this is (slightly) more
// efficient than updating each metric individually using 'BAEM_METRIC_UPDATE'.
//..
    int processEvent2(int eventId, const bsl::string& eventMessage)
        // Process the event described by the specified 'eventId' and
        // 'eventMessage'.  Return 0 on success, and a non-zero
        // value otherwise.
    {
        int returnCode = 0;

        bsls_Stopwatch stopwatch;
        BAEM_METRICS_IF_CATEGORY_ENABLED("processEvent2") {
           stopwatch.start(true);
        }

        // Process 'data'.

        BAEM_METRICS_IF_CATEGORY_ENABLED("processEvent2") {
           double systemTime, userTime, wallTime;
           stopwatch.accumulatedTimes(&systemTime, &userTime, &wallTime);
           BAEM_METRICS_UPDATE3("processEvent2",
                                "systemTime", systemTime,
                                "userTime",   userTime,
                                "wallTime",   wallTime);
        }

        return returnCode;
    }
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
      case 17: {
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
// explicitly call 'baem_DefaultMetricsManager::create()' must also explicitly
// call 'baem_DefaultMetricsManager::release()'.

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
        //     have statically cache a different baem_IntegerCollector object.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TEST CONCURRENCY: THREAD_LOCAL INT MACROS\n"
                          << "=========================================\n";

        typedef TlsIntMacroConcurrencyTest TestClass;

        bcema_TestAllocator defaultAllocator;
        bslma_DefaultAllocatorGuard guard(&defaultAllocator);
        bcema_TestAllocator testAllocator;

        baem_DefaultMetricsManagerScopedGuard scopedGuard(&testAllocator);
        baem_MetricsManager &mgr = *DefaultManager::instance();
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

        bcema_TestAllocator defaultAllocator;
        bslma_DefaultAllocatorGuard guard(&defaultAllocator);
        bcema_TestAllocator testAllocator;

        baem_DefaultMetricsManagerScopedGuard scopedGuard(&testAllocator);
        baem_MetricsManager &mgr = *DefaultManager::instance();
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

        bcema_TestAllocator defaultAllocator;
        bslma_DefaultAllocatorGuard guard(&defaultAllocator);
        bcema_TestAllocator testAllocator;

        baem_DefaultMetricsManagerScopedGuard scopedGuard(&testAllocator);
        baem_MetricsManager &mgr = *DefaultManager::instance();
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
        //     have statically cache a different baem_Collector object.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TEST CONCURRENCY: THREAD_LOCAL MACROS" << endl
                          << "=====================================" << endl;

        typedef TlsMacroConcurrencyTest TestClass;

        bcema_TestAllocator defaultAllocator;
        bslma_DefaultAllocatorGuard guard(&defaultAllocator);
        bcema_TestAllocator testAllocator;

        baem_DefaultMetricsManagerScopedGuard scopedGuard(&testAllocator);
        baem_MetricsManager &mgr = *DefaultManager::instance();
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

        bcema_TestAllocator defaultAllocator;
        bslma_DefaultAllocatorGuard guard(&defaultAllocator);
        bcema_TestAllocator testAllocator;

        baem_DefaultMetricsManagerScopedGuard scopedGuard(&testAllocator);
        baem_MetricsManager &mgr = *DefaultManager::instance();
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

        bcema_TestAllocator defaultAllocator;
        bslma_DefaultAllocatorGuard guard(&defaultAllocator);
        bcema_TestAllocator testAllocator;

        baem_DefaultMetricsManagerScopedGuard scopedGuard(&testAllocator);
        baem_MetricsManager &mgr = *DefaultManager::instance();
        Repository& repository = mgr.collectorRepository();

        const int NUM_THREADS = 10;
        {

            TestClass tester(NUM_THREADS, &testAllocator);
            tester.runTest();
        }

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING: 'BAEM_IF_CATEGORY_ENABLED'
        //
        // Testing:
        //   BAEM_METRICS_IF_CATEGORY_ENABLED(CATEGORY)
        //
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING: BAEM_IF_CATEGORY_ENABLEDS" << endl
                          << "==================================" << endl;

        if (veryVerbose) cout << "\tVerify basic behavior" << endl;
        {
            ASSERT(!baemMetricsIfCategoryEnabledTestA());

            baem_DefaultMetricsManagerScopedGuard guard(Z);
            baem_MetricsManager *manager  =
                                        baem_DefaultMetricsManager::instance();
            baem_MetricRegistry& registry = manager->metricRegistry();

            ASSERT(baemMetricsIfCategoryEnabledTestA());

            registry.setCategoryEnabled(registry.getCategory("A"), false);
            ASSERT(!breathingTestIfEnabledA());

            registry.setCategoryEnabled(registry.getCategory("A"), true);
            ASSERT(breathingTestIfEnabledA());
        }
        ASSERT(!breathingTestIfEnabledA());

        if (veryVerbose) cout << "\tVerify category cache is static" << endl;
        {
            ASSERT(!baemMetricsIfCategoryEnabledTestA());

            baem_DefaultMetricsManagerScopedGuard guard(Z);
            baem_MetricsManager *manager  =
                                        baem_DefaultMetricsManager::instance();
            baem_MetricRegistry& registry = manager->metricRegistry();

            ASSERT(baemMetricsIfCategoryEnabledTestA());
            ASSERT(baemMetricsIfCategoryEnabledTest("A"));
            ASSERT(baemMetricsIfCategoryEnabledTest("B"));

            registry.setCategoryEnabled(registry.getCategory("B"), false);

            ASSERT(baemMetricsIfCategoryEnabledTestA());
            ASSERT(baemMetricsIfCategoryEnabledTest("A"));
            ASSERT(baemMetricsIfCategoryEnabledTest("B"));

            registry.setCategoryEnabled(registry.getCategory("A"), false);

            ASSERT(!baemMetricsIfCategoryEnabledTestA());
            ASSERT(!baemMetricsIfCategoryEnabledTest("A"));
            ASSERT(!baemMetricsIfCategoryEnabledTest("B"));

            registry.setCategoryEnabled(registry.getCategory("B"), true);

            ASSERT(!baemMetricsIfCategoryEnabledTestA());
            ASSERT(!baemMetricsIfCategoryEnabledTest("A"));
            ASSERT(!baemMetricsIfCategoryEnabledTest("B"));

            registry.setCategoryEnabled(registry.getCategory("A"), true);

            ASSERT(baemMetricsIfCategoryEnabledTestA());
            ASSERT(baemMetricsIfCategoryEnabledTest("A"));
            ASSERT(baemMetricsIfCategoryEnabledTest("B"));
        }
        ASSERT(!baemMetricsIfCategoryEnabledTestA());
        ASSERT(!baemMetricsIfCategoryEnabledTest("A"));
        ASSERT(!baemMetricsIfCategoryEnabledTest("B"));
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING MACRO'S REPORTED TIME UNITS
        //
        // Concerns:
        //    That the '_TIME_BLOCK_*' report in the correct time units
        //
        // Testing:
        //     BAEM_METRICS_TIME_BLOCK(CATEGORY, METRIC, TIME_UNITS)
        //     BAEM_METRICS_TIME_BLOCK_SECONDS(CATEGORY, METRIC)
        //     BAEM_METRICS_TIME_BLOCK_MILLISECONDS(CATEGORY, METRIC)
        //     BAEM_METRICS_TIME_BLOCK_MICROSECONDS(CATEGORY, METRIC)
        //     BAEM_METRICS_TIME_BLOCK_NANOSECONDS(CATEGORY, METRIC)
        //     BAEM_METRICS_DYNAMIC_TIME_BLOCK(CATEGORY, METRIC, TIME_UNITS)
        //     BAEM_METRICS_DYNAMIC_TIME_BLOCK_SECONDS(CATEGORY, METRIC)
        //     BAEM_METRICS_DYNAMIC_TIME_BLOCK_MILLISECONDS(CATEGORY, METRIC)
        //     BAEM_METRICS_DYNAMIC_TIME_BLOCK_MICROSECONDS(CATEGORY, METRIC)
        //     BAEM_METRICS_DYNAMIC_TIME_BLOCK_NANOSECONDS(CATEGORY, METRIC)
        // --------------------------------------------------------------------

        typedef baem_StopwatchScopedGuard TU; // Time unit enumeration
        if (verbose) cout << endl
                          << "MACROS REPORTED TIME UNITS\n"
                          << "==========================\n";

        {
            baem_DefaultMetricsManagerScopedGuard guard(Z);
            MetricsManager& manager = *baem_DefaultMetricsManager::instance();
            Repository&    repository = manager.collectorRepository();

            // Create one version for every time unit, and one for the default.
            baem_Collector *a_S  = repository.getDefaultCollector("A", "S");
            baem_Collector *a_Ms = repository.getDefaultCollector("A", "Ms");
            baem_Collector *a_Us = repository.getDefaultCollector("A", "Us");
            baem_Collector *a_Ns = repository.getDefaultCollector("A", "Ns");

            baem_Collector *b_S  = repository.getDefaultCollector("B", "S");
            baem_Collector *b_Ms = repository.getDefaultCollector("B", "Ms");
            baem_Collector *b_Us = repository.getDefaultCollector("B", "Us");
            baem_Collector *b_Ns = repository.getDefaultCollector("B", "Ns");

            baem_Collector *c_D  = repository.getDefaultCollector("C", "D");
            baem_Collector *c_S  = repository.getDefaultCollector("C", "S");
            baem_Collector *c_Ms = repository.getDefaultCollector("C", "Ms");
            baem_Collector *c_Us = repository.getDefaultCollector("C", "Us");
            baem_Collector *c_Ns = repository.getDefaultCollector("C", "Ns");

            bsls_Stopwatch sw;
            sw.start();
            {
                BAEM_METRICS_TIME_BLOCK_SECONDS("A", "S");
                BAEM_METRICS_TIME_BLOCK_MILLISECONDS("A", "Ms");
                BAEM_METRICS_TIME_BLOCK_MICROSECONDS("A", "Us");
                BAEM_METRICS_TIME_BLOCK_NANOSECONDS("A", "Ns");

                BAEM_METRICS_TIME_BLOCK_SECONDS("B", "S");
                BAEM_METRICS_TIME_BLOCK_MILLISECONDS("B", "Ms");
                BAEM_METRICS_TIME_BLOCK_MICROSECONDS("B", "Us");
                BAEM_METRICS_TIME_BLOCK_NANOSECONDS("B", "Ns");

                BAEM_METRICS_TIME_BLOCK("C", "S", TU::BAEM_SECONDS);
                BAEM_METRICS_TIME_BLOCK("C", "Ms", TU::BAEM_MILLISECONDS);
                BAEM_METRICS_TIME_BLOCK("C", "Us", TU::BAEM_MICROSECONDS);
                BAEM_METRICS_TIME_BLOCK("C", "Ns", TU::BAEM_NANOSECONDS);

                bcemt_ThreadUtil::sleep(bdet_TimeInterval(50 * .001));

                sw.stop();
            }
            double expected = sw.elapsedTime();
            baem_MetricRecord rA_S  = recordVal(a_S);
            baem_MetricRecord rA_Ms = recordVal(a_Ms);
            baem_MetricRecord rA_Us = recordVal(a_Us);
            baem_MetricRecord rA_Ns = recordVal(a_Ns);

            baem_MetricRecord rB_S  = recordVal(b_S);
            baem_MetricRecord rB_Ms = recordVal(b_Ms);
            baem_MetricRecord rB_Us = recordVal(b_Us);
            baem_MetricRecord rB_Ns = recordVal(b_Ns);

            baem_MetricRecord rC_S  = recordVal(c_S);
            baem_MetricRecord rC_Ms = recordVal(c_Ms);
            baem_MetricRecord rC_Us = recordVal(c_Us);
            baem_MetricRecord rC_Ns = recordVal(c_Ns);

            ASSERT(within(rA_S.total(),  TU::BAEM_SECONDS,     expected, 1.0));
            ASSERT(within(rA_Ms.total(), TU::BAEM_MILLISECONDS,
                                                               expected, 1.0));
            ASSERT(within(rA_Us.total(), TU::BAEM_MICROSECONDS,
                                                               expected, 1.0));
            ASSERT(within(rA_Ns.total(), TU::BAEM_NANOSECONDS, expected, 1.0));

            ASSERT(within(rB_S.total(),  TU::BAEM_SECONDS,     expected, 1.0));
            ASSERT(within(rB_Ms.total(), TU::BAEM_MILLISECONDS,
                                                               expected, 1.0));
            ASSERT(within(rB_Us.total(), TU::BAEM_MICROSECONDS,
                                                               expected, 1.0));
            ASSERT(within(rB_Ns.total(), TU::BAEM_NANOSECONDS, expected, 1.0));

            ASSERT(within(rC_S.total(),  TU::BAEM_SECONDS,     expected, 1.0));
            ASSERT(within(rC_Ms.total(), TU::BAEM_MILLISECONDS,
                                                               expected, 1.0));
            ASSERT(within(rC_Us.total(), TU::BAEM_MICROSECONDS,
                                                               expected, 1.0));
            ASSERT(within(rC_Ns.total(), TU::BAEM_NANOSECONDS, expected, 1.0));
        }
      };
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'BAEM_METRICS_TIME_BLOCK' and
        //         'BAEM_METRICS_DYNAMIC_TIME_BLOCK'
        //
        // Concerns:
        //    That the '*_TIME_BLOCK' macros record the elapsed time of a
        //    block of code to the indicated metric using the default metrics
        //    manager.  That the macros function if the default metrics
        //    manager has not been created, or has been destroyed.  That the
        //    static macro statically caches the metric id to update.
        //
        // Testing:
        //     BAEM_METRICS_TIME_BLOCK(CATEGORY, METRIC)
        //     BAEM_METRICS_DYNAMIC_TIME_BLOCK(CATEGORY, METRIC)
        // --------------------------------------------------------------------
        typedef baem_StopwatchScopedGuard TU; // Time unit enumeration
        if (verbose) cout
            << endl
            << "TESTING 'BAEM_METRICS_TIME_BLOCK' & "
            << "'BAEM_METRICS_DYNAMIC_TIME_BLOCK'\n"
            << "===========================================================\n";
        {
            if (veryVerbose) cout << "\tTest without default metrics manager"
                                  << endl;
            BAEM_METRICS_TIME_BLOCK("A", "A", TU::BAEM_SECONDS);
            BAEM_METRICS_DYNAMIC_TIME_BLOCK("A", "A", TU::BAEM_SECONDS);
        }

        {
            if (veryVerbose) cout << "\tTest with default metrics manager"
                                  << endl;

            baem_DefaultMetricsManagerScopedGuard guard;
            MetricsManager& manager = *baem_DefaultMetricsManager::instance();
            Repository&    repository = manager.collectorRepository();
            baem_Collector *cA = repository.getDefaultCollector("A", "1");
            baem_Collector *cB = repository.getDefaultCollector("B", "2");

            baem_MetricId  idA = manager.metricRegistry().getId("A","1");
            baem_MetricId  idB = manager.metricRegistry().getId("B","2");

            bsls_Stopwatch timer;
            {
                timer.start();

                BAEM_METRICS_TIME_BLOCK("A", "1", TU::BAEM_SECONDS);
                ASSERT(baem_MetricRecord(idA) == recordVal(cA));

                bcemt_ThreadUtil::microSleep(50000, 0);
                timer.stop();
            }

            double expected = timer.elapsedTime();
            baem_MetricRecord record = recordVal(cA);
            ASSERT(1 == record.count());
            ASSERT(within(record.total(), TU::BAEM_SECONDS, expected, 1.0))
            ASSERT(within(record.min(), TU::BAEM_SECONDS, expected, 1.0));
            ASSERT(within(record.max(), TU::BAEM_SECONDS, expected, 1.0));

            {
                timer.start();

                BAEM_METRICS_DYNAMIC_TIME_BLOCK("B", "2", TU::BAEM_SECONDS);
                ASSERT(baem_MetricRecord(idB) == recordVal(cB));
                bcemt_ThreadUtil::microSleep(500, 0);

                bcemt_ThreadUtil::microSleep(50000, 0);
                timer.stop();
            }

            expected = timer.elapsedTime();
            record = recordVal(cB);
            ASSERT(1 == record.count());
            ASSERT(within(record.total(), TU::BAEM_SECONDS, expected, 1.0))
            ASSERT(within(record.min(), TU::BAEM_SECONDS, expected, 1.0));
            ASSERT(within(record.max(), TU::BAEM_SECONDS, expected, 1.0));
        }
        {
            if (veryVerbose) cout << "\tTest with multiple macros in a block"
                                  << endl;

            baem_DefaultMetricsManagerScopedGuard guard;
            MetricsManager& manager = *baem_DefaultMetricsManager::instance();
            Repository&     rep     = manager.collectorRepository();

            {
                BAEM_METRICS_TIME_BLOCK("A", "1", TU::BAEM_SECONDS);
                BAEM_METRICS_TIME_BLOCK("B", "2", TU::BAEM_SECONDS);
                BAEM_METRICS_TIME_BLOCK("C", "3", TU::BAEM_SECONDS);
                BAEM_METRICS_TIME_BLOCK("D", "4", TU::BAEM_SECONDS);

                BAEM_METRICS_DYNAMIC_TIME_BLOCK("E", "5", TU::BAEM_SECONDS);
                BAEM_METRICS_DYNAMIC_TIME_BLOCK("F", "6", TU::BAEM_SECONDS);
                BAEM_METRICS_DYNAMIC_TIME_BLOCK("G", "7", TU::BAEM_SECONDS);
                BAEM_METRICS_DYNAMIC_TIME_BLOCK("H", "8", TU::BAEM_SECONDS);

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

            baem_DefaultMetricsManagerScopedGuard guard;
            MetricsManager& manager = *baem_DefaultMetricsManager::instance();
            Repository&     rep     = manager.collectorRepository();

            for (int i = 0; i < NUM_CATEGORIES; ++i) {
                BAEM_METRICS_TIME_BLOCK(CATEGORIES[i], "static",
                                        TU::BAEM_SECONDS);
                BAEM_METRICS_DYNAMIC_TIME_BLOCK(CATEGORIES[i], "dynamic",
                                                TU::BAEM_SECONDS);
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
            baem_DefaultMetricsManager::create();
            MetricsManager& manager = *baem_DefaultMetricsManager::instance();
            Repository&     rep     = manager.collectorRepository();
            {
                staticTimeEmptyFunction("A", "1");
                dynamicTimeEmptyFunction("B", "2");
            }
            ASSERT(1 == recordVal(rep.getDefaultCollector("A","1")).count());
            ASSERT(1 == recordVal(rep.getDefaultCollector("B","2")).count());

            baem_DefaultMetricsManager::destroy();
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
        // TESTING: 'BAEM_METRICS_THREAD_LOCAL_INT_UPDATE',
        //          'BAEM_INCREMENT_THREADLOCALMETRIC'
        //
        // Concerns:
        //    That the two standard macros
        //    ('BAEM_METRICS_THREAD_LOCAL_INT_UPDATE' and
        //    'BAEM_METRICS_THREAD_LOCAL_INT_INCREMENT') correctly update the
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
        //   then invoke 'BAEM_METRICS_THREAD_LOCAL_INT_UPDATE' and
        //   'BAEM_METRICS_THREAD_LOCAL_INT_INCREMENT' on the identified metric
        //   and perform the corresponding operation on the "oracle" collector.
        //   Verify the collector underlying the metric has the same value as
        //   the "oracle" collector.  Also verify that only the collector for
        //   the first identified metric is updated.
        //
        //   Perform the same test again, but enabled or disable the metric's
        //   category on each iteration.
        //
        // Testing:
        //    BAEM_METRICS_THREAD_LOCAL_INT_UPDATE(CATEGORY, NAME, VALUE)
        //    BAEM_METRICS_THREAD_LOCAL_INT_INCREMENT(CATEGORY, NAME)
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
                BAEM_METRICS_THREAD_LOCAL_INT_INCREMENT(IDS[i], "increment");
                BAEM_METRICS_THREAD_LOCAL_INT_UPDATE(IDS[i], "update",
                                                     UPDATES[i]);
            }
        }

        if (veryVerbose)
            cout << "\tverify thread-local macros are applied correctly.\n";

        {
            baem_DefaultMetricsManagerScopedGuard guard(Z);
            baem_MetricsManager& mgr = *DefaultManager::instance();
            Registry&   registry   = mgr.metricRegistry();
            Repository& repository = mgr.collectorRepository();

            baem_MetricId updateId(registry.getId(IDS[0], "update"));
            baem_MetricId incrementId(registry.getId(IDS[0], "increment"));
            baem_IntegerCollector  expUpdate(updateId);
            baem_IntegerCollector  expIncrement(incrementId);
            for (int i = 0; i < NUM_IDS; ++i) {
                for (int j = 0; j < NUM_UPDATES; ++j) {
                    BAEM_METRICS_THREAD_LOCAL_INT_INCREMENT(IDS[i],
                                                           "increment");
                    BAEM_METRICS_THREAD_LOCAL_INT_UPDATE(IDS[i], "update",
                                                        UPDATES[i]);
                    expIncrement.update(1);
                    expUpdate.update(UPDATES[i]);
                }
            }
            baem_IntegerCollector *upCol  =
                               repository.getDefaultIntegerCollector(updateId);
            baem_IntegerCollector *incCol =
                            repository.getDefaultIntegerCollector(incrementId);
            ASSERT(recordVal(&expUpdate) == recordVal(upCol));
            ASSERT(recordVal(&expIncrement) == recordVal(incCol));

            ASSERT(0 == defaultAllocator.numBytesInUse());

            // Verify only the first identified metric (i.e., a 'CATEGORY',
            // 'NAME' pair) for which the macro was invoked was modified.
            bsl::vector<baem_Collector *> cols(Z);
            bsl::vector<baem_IntegerCollector *> intCols(Z);
            for (int i = 1; i < NUM_IDS; ++i) {
                baem_MetricId updateId(registry.getId(IDS[i], "update"));
                baem_MetricId incrementId(registry.getId(IDS[i], "increment"));

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
            baem_DefaultMetricsManagerScopedGuard guard(Z);
            baem_MetricsManager& mgr = *DefaultManager::instance();
            Registry&   registry   = mgr.metricRegistry();
            Repository& repository = mgr.collectorRepository();

            baem_MetricId updateId(registry.getId(IDS[0], "update"));
            baem_MetricId incrementId(registry.getId(IDS[0], "increment"));
            baem_IntegerCollector  expUpdate(updateId);
            baem_IntegerCollector  expIncrement(incrementId);
            for (int i = 0; i < NUM_IDS; ++i) {
                for (int j = 0; j < NUM_UPDATES; ++j) {

                    bool enabled = 0 == j % 2;
                    registry.setCategoryEnabled(updateId.category(), enabled);
                    BAEM_METRICS_THREAD_LOCAL_INT_INCREMENT(IDS[i],
                                                           "increment");
                    BAEM_METRICS_THREAD_LOCAL_INT_UPDATE(IDS[i], "update",
                                                         UPDATES[i]);
                    if (enabled) {
                        expIncrement.update(1);
                        expUpdate.update(UPDATES[i]);
                    }
                }
            }
            baem_IntegerCollector *upCol  =
                               repository.getDefaultIntegerCollector(updateId);
            baem_IntegerCollector *incCol =
                            repository.getDefaultIntegerCollector(incrementId);
            ASSERT(recordVal(&expUpdate) == recordVal(upCol));
            ASSERT(recordVal(&expIncrement) == recordVal(incCol));

            ASSERT(0 == defaultAllocator.numBytesInUse());

            // Verify only the first identified metric (i.e., a 'CATEGORY',
            // 'NAME' pair) for which the macro was invoked was modified.
            bsl::vector<baem_Collector *> cols(Z);
            bsl::vector<baem_IntegerCollector *> intCols(Z);
            for (int i = 1; i < NUM_IDS; ++i) {
                baem_MetricId updateId(registry.getId(IDS[i], "update"));
                baem_MetricId incrementId(registry.getId(IDS[i], "increment"));

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
        // TESTING: 'BAEM_METRICS_DYNAMIC_INT_UPDATE',
        //          'BAEM_METRICS_DYNAMIC_INT_INCREMENT'
        //
        // Concerns:
        //    That the two dynamic macros ('BAEM_METRICS_DYNAMIC_INT_UPDATE'
        //    and 'BAEM_METRICS_DYNAMIC_INT_INCREMENT') correctly update the
        //    appropriate metric and respect the supplied categories 'enabled'
        //    property.
        //
        // Plan:
        //   Verify that invoking the macros without a default metrics manager
        //   has no effect.
        //
        //   Specify a set S of unique object values having various minor or
        //   subtle differences.  For each value create an "oracle" collector,
        //   then invoke 'BAEM_METRICS_DYNAMIC_INT_UPDATE' and
        //   'BAEM_METRICS_DYNAMIC_INT_INCREMENT' on the identified metric and
        //   perform the corresponding operation on the "oracle" collector.
        //   Verify the collector underlying the metric has the same value as
        //   the "oracle" collector.
        //
        //   Perform the same test again, but enabled or disable the metric's
        //   category on each iteration.
        //
        // Testing:
        //    BAEM_METRICS_DYNAMIC_INT_UPDATE(CATEGORY, NAME, VALUE)
        //    BAEM_METRICS_DYNAMIC_INCREMENT(CATEGORY, NAME)
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
                BAEM_METRICS_DYNAMIC_INCREMENT(IDS[i], "increment");
                BAEM_METRICS_DYNAMIC_INT_UPDATE(IDS[i], "update", UPDATES[i]);
            }
        }

        if (veryVerbose)
            cout << "\tverify dynamic macros are applied correctly.\n";
        {
            baem_DefaultMetricsManagerScopedGuard guard(Z);
            baem_MetricsManager& mgr = *DefaultManager::instance();
            Registry&   registry   = mgr.metricRegistry();
            Repository& repository = mgr.collectorRepository();

            for (int i = 0; i < NUM_IDS; ++i) {
                baem_MetricId updateId(registry.getId(IDS[i], "update"));
                baem_MetricId incrementId(registry.getId(IDS[i],
                                                          "increment"));
                baem_IntegerCollector expUpdate(updateId);
                baem_IntegerCollector expIncrement(incrementId);
                baem_IntegerCollector *upCol  =
                            repository.getDefaultIntegerCollector(updateId);
                baem_IntegerCollector *incCol =
                            repository.getDefaultIntegerCollector(incrementId);
                for (int j = 0; j < NUM_UPDATES; ++j) {
                    BAEM_METRICS_DYNAMIC_INCREMENT(IDS[i], "increment");
                    BAEM_METRICS_DYNAMIC_INT_UPDATE(IDS[i], "update",
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
            baem_DefaultMetricsManagerScopedGuard guard(Z);
            baem_MetricsManager& mgr = *DefaultManager::instance();
            Registry&   registry   = mgr.metricRegistry();
            Repository& repository = mgr.collectorRepository();

            for (int i = 0; i < NUM_IDS; ++i) {
                baem_MetricId  updateId(registry.getId(IDS[i], "update"));
                baem_MetricId  incrementId(registry.getId(IDS[i],
                                                          "increment"));
                baem_IntegerCollector expUpdate(updateId);
                baem_IntegerCollector expIncrement(incrementId);
                baem_IntegerCollector *upCol  =
                            repository.getDefaultIntegerCollector(updateId);
                baem_IntegerCollector *incCol =
                            repository.getDefaultIntegerCollector(incrementId);
                for (int j = 0; j < NUM_UPDATES; ++j) {
                    bool enabled = 0 == j % 2;

                    registry.setCategoryEnabled(updateId.category(), enabled);
                    BAEM_METRICS_DYNAMIC_INCREMENT(IDS[i], "increment");
                    BAEM_METRICS_DYNAMIC_INT_UPDATE(IDS[i], "update",
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
        // TESTING: 'BAEM_METRICS_INT_UPDATE', 'BAEM_METRICS_INT_INCREMENT'
        //          'BAEM_UPDATE_INT_TYPED_METRIC'
        // Concerns:
        //    That the two standard macros ('BAEM_METRICS_INT_UPDATE' and
        //    'BAEM_METRICS_INT_INCREMENT') correctly update the appropriate
        //    metric, statically cache the identified metrics identifier, and
        //    respect the supplied categories 'enabled' property.
        //
        // Plan:
        //   Verify that invoking the macros without a default metrics manager
        //   has no effect.
        //
        //   Specify a set S of unique object values having various minor or
        //   subtle differences.  For each value create an "oracle" collector,
        //   then invoke 'BAEM_METRICS_INT_UPDATE' and
        //   'BAEM_METRICS_INT_INCREMENT' on the identified metric and perform
        //   the corresponding operation on the "oracle" collector.  Verify
        //   the collector underlying the metric has the same value as the
        //   "oracle" collector.  Also verify that only the collector for the
        //   first identified metric is updated.
        //
        //   Perform the same test again, but enabled or disable the metric's
        //   category on each iteration.
        //
        // Testing:
        //    BAEM_METRICS_INT_UPDATE(CATEGORY, NAME, VALUE
        //    BAEM_METRICS_TYPED_INT_UPDATE(CATEGORY, NAME, TYPE, VALUE)
        //    BAEM_METRICS_INT_INCREMENT(CATEGORY, NAME
        //    BAEM_METRICS_INT_UPDATE2(CATEGORY, METRIC1, VALUE1, ...)
        //    BAEM_METRICS_INT_UPDATE3(CATEGORY, METRIC1, VALUE1, ...)
        //    BAEM_METRICS_INT_UPDATE4(CATEGORY, METRIC1, VALUE1, ...)
        //    BAEM_METRICS_INT_UPDATE5(CATEGORY, METRIC1, VALUE1, ...)
        //    BAEM_METRICS_INT_UPDATE6(CATEGORY, METRIC1, VALUE1, ...)
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
                BAEM_METRICS_INCREMENT(IDS[i], "increment");
                BAEM_METRICS_INT_UPDATE(IDS[i], "update", UPDATES[i]);
                BAEM_METRICS_TYPED_INT_UPDATE(IDS[i], "t-up", UPDATES[i],
                                              Type::BAEM_TOTAL);
                BAEM_METRICS_TYPED_INCREMENT(IDS[i], "t-inc", Type::BAEM_MIN);
            }
        }

        if (veryVerbose)
            cout << "\tverify macros are applied correctly.\n";

        {
            baem_DefaultMetricsManagerScopedGuard guard(Z);
            baem_MetricsManager& mgr = *DefaultManager::instance();
            Registry&   registry   = mgr.metricRegistry();
            Repository& repository = mgr.collectorRepository();

            baem_MetricId updateId(registry.getId(IDS[0], "update"));
            baem_MetricId incId(registry.getId(IDS[0], "increment"));
            baem_MetricId tupdateId(registry.getId(IDS[0], "Tupdate"));
            baem_MetricId tincId(registry.getId(IDS[0], "Tincrement"));

            baem_IntegerCollector expUpdate(updateId);
            baem_IntegerCollector expIncrement(incId);
            baem_IntegerCollector expTUpdate(tupdateId);
            baem_IntegerCollector expTIncrement(tincId);
            for (int i = 0; i < NUM_IDS; ++i) {
                for (int j = 0; j < NUM_UPDATES; ++j) {
                    BAEM_METRICS_INCREMENT(IDS[i], "increment");
                    BAEM_METRICS_INT_UPDATE(IDS[i], "update", UPDATES[j]);
                    BAEM_METRICS_TYPED_INT_UPDATE(IDS[i], "Tupdate",
                                                  UPDATES[j],
                                                  Type::BAEM_TOTAL);
                    BAEM_METRICS_TYPED_INCREMENT(IDS[i], "Tincrement",
                                                 Type::BAEM_MIN);
                    expIncrement.update(1);
                    expUpdate.update(UPDATES[j]);
                    expTUpdate.update(UPDATES[j]);
                    expTIncrement.update(1);
                }
            }
            ASSERT(Type::BAEM_TOTAL ==
                   tupdateId.description()->preferredPublicationType());
            ASSERT(Type::BAEM_MIN   ==
                   tincId.description()->preferredPublicationType());

            baem_IntegerCollector *upCol  =
                repository.getDefaultIntegerCollector(updateId);
            baem_IntegerCollector *incCol =
                repository.getDefaultIntegerCollector(incId);
            baem_IntegerCollector *tupCol =
                repository.getDefaultIntegerCollector(tupdateId);
            baem_IntegerCollector *tincCol =
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
                baem_MetricId updateId(registry.getId(IDS[i], "update"));
                baem_MetricId incId(registry.getId(IDS[i], "increment"));
                baem_MetricId tupdateId(registry.getId(IDS[i], "Tupdate"));
                baem_MetricId tincId(registry.getId(IDS[i], "Tincrement"));

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
            baem_DefaultMetricsManagerScopedGuard guard(Z);
            baem_MetricsManager& mgr = *DefaultManager::instance();
            Registry&   registry   = mgr.metricRegistry();
            Repository& repository = mgr.collectorRepository();

            baem_MetricId updateId(registry.getId(IDS[0], "update"));
            baem_MetricId incId(registry.getId(IDS[0], "increment"));
            baem_MetricId tupdateId(registry.getId(IDS[0], "Tupdate"));
            baem_MetricId tincId(registry.getId(IDS[0], "Tincrement"));

            baem_IntegerCollector expUpdate(updateId);
            baem_IntegerCollector expIncrement(incId);
            baem_IntegerCollector expTUpdate(tupdateId);
            baem_IntegerCollector expTIncrement(tincId);
            for (int i = 0; i < NUM_IDS; ++i) {
                for (int j = 0; j < NUM_UPDATES; ++j) {
                    bool enabled = 0 == j % 2;
                    registry.setCategoryEnabled(updateId.category(), enabled);
                    BAEM_METRICS_INCREMENT(IDS[i], "increment");
                    BAEM_METRICS_INT_UPDATE(IDS[i], "update", UPDATES[j]);
                    BAEM_METRICS_TYPED_INT_UPDATE(IDS[i], "Tupdate",
                                                  UPDATES[j],
                                                  Type::BAEM_TOTAL);
                    BAEM_METRICS_TYPED_INCREMENT(IDS[i], "Tincrement",
                                                 Type::BAEM_MIN);
                    if (enabled) {
                        expIncrement.update(1);
                        expUpdate.update(UPDATES[j]);
                        expTUpdate.update(UPDATES[j]);
                        expTIncrement.update(1);
                    }
                }
            }

            ASSERT(Type::BAEM_TOTAL ==
                   tupdateId.description()->preferredPublicationType());
            ASSERT(Type::BAEM_MIN   ==
                   tincId.description()->preferredPublicationType());

            baem_IntegerCollector *upCol  =
                repository.getDefaultIntegerCollector(updateId);
            baem_IntegerCollector *incCol =
                repository.getDefaultIntegerCollector(incId);
            baem_IntegerCollector *tupCol =
                repository.getDefaultIntegerCollector(tupdateId);
            baem_IntegerCollector *tincCol =
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
                baem_MetricId updateId(registry.getId(IDS[i], "update"));
                baem_MetricId incId(registry.getId(IDS[i], "increment"));
                baem_MetricId tupdateId(registry.getId(IDS[i], "Tupdate"));
                baem_MetricId tincId(registry.getId(IDS[i], "Tincrement"));

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
                baem_MetricsManager *mgr = 0;
                if (initializeDefault) {
                    mgr = baem_DefaultMetricsManager::create(Z);
                }
                else {
                    mgr = new (*Z) baem_MetricsManager(Z);
                }
                Registry&   registry   = mgr->metricRegistry();
                Repository& repository = mgr->collectorRepository();

                baem_MetricId updateId(registry.getId(IDS[0], "update"));
                baem_MetricId incId(registry.getId(IDS[0], "increment"));
                baem_MetricId tupdateId(registry.getId(IDS[0], "Tupdate"));
                baem_MetricId tincId(registry.getId(IDS[0], "Tincrement"));

                baem_IntegerCollector expUpdate(updateId);
                baem_IntegerCollector expIncrement(incId);
                baem_IntegerCollector expTUpdate(tupdateId);
                baem_IntegerCollector expTIncrement(tincId);
                for (int i = 0; i < NUM_IDS; ++i) {
                    for (int j = 0; j < NUM_UPDATES; ++j) {
                        BAEM_METRICS_INCREMENT(IDS[i], "increment");
                        BAEM_METRICS_INT_UPDATE(IDS[i], "update", UPDATES[j]);
                        BAEM_METRICS_TYPED_INT_UPDATE(IDS[i], "Tupdate",
                                                     UPDATES[j],
                                                     Type::BAEM_TOTAL);
                        BAEM_METRICS_TYPED_INCREMENT(IDS[i], "Tincrement",
                                                     Type::BAEM_MIN);
                        expIncrement.update(1);
                        expUpdate.update(UPDATES[j]);
                        expTUpdate.update(UPDATES[j]);
                        expTIncrement.update(1);
                    }
                }
                if (initializeDefault) {
                    baem_IntegerCollector *upCol   =
                        repository.getDefaultIntegerCollector(updateId);
                    baem_IntegerCollector *incCol  =
                        repository.getDefaultIntegerCollector(incId);
                    baem_IntegerCollector *tupCol  =
                        repository.getDefaultIntegerCollector(tupdateId);
                    baem_IntegerCollector *tincCol =
                        repository.getDefaultIntegerCollector(tincId);

                    ASSERT(recordVal(&expUpdate)     == recordVal(upCol));
                    ASSERT(recordVal(&expIncrement)  == recordVal(incCol));
                    ASSERT(recordVal(&expTUpdate)    == recordVal(tupCol));
                    ASSERT(recordVal(&expTIncrement) == recordVal(tincCol));
                    baem_DefaultMetricsManager::destroy();
                }
                else {
                    Z->deleteObjectRaw(mgr);
                }
            }

            ASSERT(0 == defaultAllocator.numBytesInUse());
        }
        if (veryVerbose)  cout << "\tVerify multiple metric variants.\n";
        {
            baem_DefaultMetricsManagerScopedGuard guard(Z);
            baem_MetricsManager  *manager =
                                        baem_DefaultMetricsManager::instance();
            baem_MetricRegistry& registry = manager->metricRegistry();
            baem_CollectorRepository& repository =
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
                BAEM_METRICS_INT_UPDATE2("A",
                                         "A", 1, "B", 1);
                BAEM_METRICS_INT_UPDATE3("A",
                                         "A", 2, "B", 2, "C", 2);
                BAEM_METRICS_INT_UPDATE4("A",
                                         "A", 3, "B", 3, "C", 3, "D", 3);
                BAEM_METRICS_INT_UPDATE5("A",
                                         "A", 4, "B", 4, "C", 4, "D", 4,
                                         "E", 4);
                BAEM_METRICS_INT_UPDATE6("A",
                                         "A", 5, "B", 5, "C", 5, "D", 5,
                                         "E", 5, "F", 5);
            }

            baem_IntegerCollector *A_COL =
                repository.getDefaultIntegerCollector("A", "A");
            baem_IntegerCollector *B_COL =
                repository.getDefaultIntegerCollector("A", "B");
            baem_IntegerCollector *C_COL =
                repository.getDefaultIntegerCollector("A", "C");
            baem_IntegerCollector *D_COL =
                repository.getDefaultIntegerCollector("A", "D");
            baem_IntegerCollector *E_COL =
                repository.getDefaultIntegerCollector("A", "E");
            baem_IntegerCollector *F_COL =
                repository.getDefaultIntegerCollector("A", "F");
            baem_IntegerCollector *G_COL =
                repository.getDefaultIntegerCollector("A", "G");

            if (veryVeryVerbose) {
                P_(recordVal(A_COL)); P_(recordVal(B_COL));
                P_(recordVal(C_COL)); P(recordVal(D_COL));
                P_(recordVal(E_COL)); P_(recordVal(F_COL));
                P(recordVal(G_COL));
            }
            ASSERT(baem_MetricRecord(A_ID, 50, 150, 1, 5) == recordVal(A_COL));
            ASSERT(baem_MetricRecord(B_ID, 50, 150, 1, 5) == recordVal(B_COL));
            ASSERT(baem_MetricRecord(C_ID, 40, 140, 2, 5) == recordVal(C_COL));
            ASSERT(baem_MetricRecord(D_ID, 30, 120, 3, 5) == recordVal(D_COL));
            ASSERT(baem_MetricRecord(E_ID, 20,  90, 4, 5) == recordVal(E_COL));
            ASSERT(baem_MetricRecord(F_ID, 10,  50, 5, 5) == recordVal(F_COL));
            ASSERT(baem_MetricRecord(G_ID)                == recordVal(G_COL));

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
        // TESTING: 'BAEM_METRICS_THREAD_LOCAL_UPDATE',
        //          'BAEM_INCREMENT_THREADLOCALMETRIC'
        //
        // Concerns:
        //    That the two standard macros ('BAEM_METRICS_THREAD_LOCAL_UPDATE'
        //    and 'BAEM_METRICS_THREAD_LOCAL_INCREMENT') correctly update the
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
        //   then invoke 'BAEM_METRICS_THREAD_LOCAL_UPDATE' and
        //   'BAEM_METRICS_THREAD_LOCAL_INCREMENT' on the identified metric and
        //   perform the corresponding operation on the "oracle" collector.
        //   Verify the collector underlying the metric has the same value as
        //   the "oracle" collector.  Also verify that only the collector for
        //   the first identified metric is updated.
        //
        //   Perform the same test again, but enabled or disable the metric's
        //   category on each iteration.
        //
        // Testing:
        //    BAEM_METRICS_THREAD_LOCAL_UPDATE(CATEGORY, NAME, VALUE)
        //    BAEM_METRICS_THREAD_LOCAL_INCREMENT(CATEGORY, NAME)
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
                BAEM_METRICS_THREAD_LOCAL_INCREMENT(IDS[i], "increment");
                BAEM_METRICS_THREAD_LOCAL_UPDATE(IDS[i], "update", UPDATES[i]);
            }
        }

        if (veryVerbose)
            cout << "\tverify thread-local macros are applied correctly.\n";

        {
            baem_DefaultMetricsManagerScopedGuard guard(Z);
            baem_MetricsManager& mgr = *DefaultManager::instance();
            Registry&   registry   = mgr.metricRegistry();
            Repository& repository = mgr.collectorRepository();

            baem_MetricId updateId(registry.getId(IDS[0], "update"));
            baem_MetricId incId(registry.getId(IDS[0], "increment"));
            baem_Collector  expUpdate(updateId);
            baem_Collector  expIncrement(incId);
            for (int i = 0; i < NUM_IDS; ++i) {
                for (int j = 0; j < NUM_UPDATES; ++j) {
                    BAEM_METRICS_THREAD_LOCAL_INCREMENT(IDS[i], "increment");
                    BAEM_METRICS_THREAD_LOCAL_UPDATE(IDS[i], "update",
                                                   UPDATES[i]);
                    expIncrement.update(1);
                    expUpdate.update(UPDATES[i]);
                }
            }
            baem_Collector *upCol  = repository.getDefaultCollector(updateId);
            baem_Collector *incCol = repository.getDefaultCollector(incId);
            ASSERT(recordVal(&expUpdate) == recordVal(upCol));
            ASSERT(recordVal(&expIncrement) == recordVal(incCol));

            ASSERT(0 == defaultAllocator.numBytesInUse());

            // Verify only the first identified metric (i.e., a 'CATEGORY',
            // 'NAME' pair) for which the macro was invoked was modified.
            bsl::vector<baem_Collector *> cols(Z);
            bsl::vector<baem_IntegerCollector *> intCols(Z);
            for (int i = 1; i < NUM_IDS; ++i) {
                baem_MetricId updateId(registry.getId(IDS[i], "update"));
                baem_MetricId incId(registry.getId(IDS[i], "increment"));

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
            baem_DefaultMetricsManagerScopedGuard guard(Z);
            baem_MetricsManager& mgr = *DefaultManager::instance();
            Registry&   registry   = mgr.metricRegistry();
            Repository& repository = mgr.collectorRepository();

            baem_MetricId updateId(registry.getId(IDS[0], "update"));
            baem_MetricId incId(registry.getId(IDS[0], "increment"));
            baem_Collector  expUpdate(updateId);
            baem_Collector  expIncrement(incId);
            for (int i = 0; i < NUM_IDS; ++i) {
                for (int j = 0; j < NUM_UPDATES; ++j) {

                    bool enabled = 0 == j % 2;
                    registry.setCategoryEnabled(updateId.category(), enabled);
                    BAEM_METRICS_THREAD_LOCAL_INCREMENT(IDS[i], "increment");
                    BAEM_METRICS_THREAD_LOCAL_UPDATE(IDS[i], "update",
                                                   UPDATES[i]);
                    if (enabled) {
                        expIncrement.update(1);
                        expUpdate.update(UPDATES[i]);
                    }
                }
            }
            baem_Collector *upCol  = repository.getDefaultCollector(updateId);
            baem_Collector *incCol = repository.getDefaultCollector(incId);
            ASSERT(recordVal(&expUpdate) == recordVal(upCol));
            ASSERT(recordVal(&expIncrement) == recordVal(incCol));

            ASSERT(0 == defaultAllocator.numBytesInUse());

            // Verify only the first identified metric (i.e., a 'CATEGORY',
            // 'NAME' pair) for which the macro was invoked was modified.
            bsl::vector<baem_Collector *> cols(Z);
            bsl::vector<baem_IntegerCollector *> intCols(Z);
            for (int i = 1; i < NUM_IDS; ++i) {
                baem_MetricId updateId(registry.getId(IDS[i], "update"));
                baem_MetricId incId(registry.getId(IDS[i], "increment"));

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
        // TESTING: 'BAEM_METRICS_DYNAMIC_UPDATE',
        //          'BAEM_METRICS_DYNAMIC_INCREMENT'
        //
        // Concerns:
        //    That the two dynamic macros ('BAEM_METRICS_DYNAMIC_UPDATE' and
        //    'BAEM_METRICS_DYNAMIC_INCREMENT') correctly update the
        //    appropriate metric and respect the supplied categories 'enabled'
        //    property.
        //
        // Plan:
        //   Verify that invoking the macros without a default metrics manager
        //   has no effect.
        //
        //   Specify a set S of unique object values having various minor or
        //   subtle differences.  For each value create an "oracle" collector,
        //   then invoke 'BAEM_METRICS_DYNAMIC_UPDATE' and
        //   'BAEM_METRICS_DYNAMIC_INCREMENT' on the identified metric and
        //   perform the corresponding operation on the "oracle" collector.
        //   Verify the collector underlying the metric has the same value as
        //   the "oracle" collector.
        //
        //   Perform the same test again, but enabled or disable the metric's
        //   category on each iteration.
        //
        // Testing:
        //    BAEM_METRICS_DYNAMIC_UPDATE(CATEGORY, NAME, VALUE)
        //    BAEM_METRICS_DYNAMIC_INCREMENT(CATEGORY, NAME)
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
                BAEM_METRICS_DYNAMIC_UPDATE(IDS[i], "update", UPDATES[i]);
            }
        }

        if (veryVerbose)
            cout << "\tverify dynamic macros are applied correctly.\n";
        {
            baem_DefaultMetricsManagerScopedGuard guard(Z);
            baem_MetricsManager& mgr = *DefaultManager::instance();
            Registry&   registry   = mgr.metricRegistry();
            Repository& repository = mgr.collectorRepository();

            for (int i = 0; i < NUM_IDS; ++i) {
                baem_MetricId  updateId(registry.getId(IDS[i], "update"));
                baem_Collector expUpdate(updateId);
                baem_Collector *upCol  =
                                      repository.getDefaultCollector(updateId);
                for (int j = 0; j < NUM_UPDATES; ++j) {
                    BAEM_METRICS_DYNAMIC_UPDATE(IDS[i], "update", UPDATES[j]);
                    expUpdate.update(UPDATES[j]);
                    ASSERT(recordVal(&expUpdate) == recordVal(upCol));
                }
            }
        }
        if (veryVerbose)
            cout << "\tverify dynamic macros respect 'isActive' flag.\n";

        {
            baem_DefaultMetricsManagerScopedGuard guard(Z);
            baem_MetricsManager& mgr = *DefaultManager::instance();
            Registry&   registry   = mgr.metricRegistry();
            Repository& repository = mgr.collectorRepository();

            for (int i = 0; i < NUM_IDS; ++i) {
                baem_MetricId  updateId(registry.getId(IDS[i], "update"));
                baem_Collector expUpdate(updateId);
                baem_Collector *upCol  =
                                  repository.getDefaultCollector(updateId);
                for (int j = 0; j < NUM_UPDATES; ++j) {
                    bool enabled = 0 == j % 2;
                    registry.setCategoryEnabled(updateId.category(), enabled);
                    BAEM_METRICS_DYNAMIC_UPDATE(IDS[i], "update", UPDATES[j]);
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
        // TESTING: 'BAEM_METRICS_UPDATE', 'BAEM_METRICS_INCREMENT'
        //          'BAEM_METRICS_TYPED_UPDATE'
        //
        // Concerns:
        //    That the three standard macros ('BAEM_METRICS_UPDATE' and
        //    'BAEM_METRICS_INCREMENT' and 'BAEM_METRICS_TYPED_UPDATE')
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
        //   then invoke 'BAEM_METRICS_UPDATE' and 'BAEM_METRICS_INCREMENT' on
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
        //    BAEM_METRICS_UPDATE(CATEGORY, NAME, VALUE)
        //    BAEM_METRICS_TYPED_UPDATE(CATEGORY, NAME,PUBLICATION_TYPE,VALUE)
        //    BAEM_METRICS_INCREMENT(CATEGORY, NAME)
        //    BAEM_METRICS_UPDATE2(CATEGORY, METRIC1, VALUE1, ...)
        //    BAEM_METRICS_UPDATE3(CATEGORY, METRIC1, VALUE1, ...)
        //    BAEM_METRICS_UPDATE4(CATEGORY, METRIC1, VALUE1, ...)
        //    BAEM_METRICS_UPDATE5(CATEGORY, METRIC1, VALUE1, ...)
        //    BAEM_METRICS_UPDATE6(CATEGORY, METRIC1, VALUE1, ...)
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
                BAEM_METRICS_UPDATE(IDS[i], "update", UPDATES[i]);
                BAEM_METRICS_TYPED_UPDATE(IDS[i], "t-up", UPDATES[i],
                                         Type::BAEM_TOTAL);
            }
        }

        if (veryVerbose)
            cout << "\tverify macros are applied correctly.\n";

        {
            baem_DefaultMetricsManagerScopedGuard guard(Z);
            baem_MetricsManager& mgr = *DefaultManager::instance();
            Registry&   registry   = mgr.metricRegistry();
            Repository& repository = mgr.collectorRepository();

            baem_MetricId updateId(registry.getId(IDS[0], "update"));
            baem_MetricId tupdateId(registry.getId(IDS[0], "Tupdate"));

            baem_Collector expUpdate(updateId);
            baem_Collector expTUpdate(tupdateId);
            for (int i = 0; i < NUM_IDS; ++i) {
                for (int j = 0; j < NUM_UPDATES; ++j) {
                    BAEM_METRICS_UPDATE(IDS[i], "update", UPDATES[j]);
                    BAEM_METRICS_TYPED_UPDATE(IDS[i], "Tupdate", UPDATES[j],
                                             Type::BAEM_TOTAL);
                    expUpdate.update(UPDATES[j]);
                    expTUpdate.update(UPDATES[j]);
                }
            }
            ASSERT(Type::BAEM_TOTAL ==
                   tupdateId.description()->preferredPublicationType());

            baem_Collector *upCol  = repository.getDefaultCollector(updateId);
            baem_Collector *tupCol = repository.getDefaultCollector(tupdateId);

            ASSERT(recordVal(&expUpdate)     == recordVal(upCol));
            ASSERT(recordVal(&expTUpdate)    == recordVal(tupCol));

            // Verify only the first identified metric (i.e., a 'CATEGORY',
            // 'NAME' pair) for which the macro was invoked was modified.
            ColSPtrVector cols(Z);
            IColSPtrVector intCols(Z);
            for (int i = 1; i < NUM_IDS; ++i) {
                baem_MetricId updateId(registry.getId(IDS[i], "update"));
                baem_MetricId tupdateId(registry.getId(IDS[i], "Tupdate"));

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
            baem_DefaultMetricsManagerScopedGuard guard(Z);
            baem_MetricsManager& mgr = *DefaultManager::instance();
            Registry&   registry     = mgr.metricRegistry();
            Repository& repository   = mgr.collectorRepository();

            baem_MetricId updateId(registry.getId(IDS[0], "update"));
            baem_MetricId tupdateId(registry.getId(IDS[0], "Tupdate"));

            baem_Collector expUpdate(updateId);
            baem_Collector expTUpdate(tupdateId);
            for (int i = 0; i < NUM_IDS; ++i) {
                for (int j = 0; j < NUM_UPDATES; ++j) {
                    bool enabled = 0 == j % 2;
                    registry.setCategoryEnabled(updateId.category(), enabled);
                    BAEM_METRICS_UPDATE(IDS[i], "update", UPDATES[j]);
                    BAEM_METRICS_TYPED_UPDATE(IDS[i], "Tupdate", UPDATES[j],
                                             Type::BAEM_MIN);
                    if (enabled) {
                        expUpdate.update(UPDATES[j]);
                        expTUpdate.update(UPDATES[j]);
                    }
                }
            }

            ASSERT(Type::BAEM_MIN ==
                   tupdateId.description()->preferredPublicationType());

            baem_Collector *upCol  = repository.getDefaultCollector(updateId);
            baem_Collector *tupCol = repository.getDefaultCollector(tupdateId);

            ASSERT(recordVal(&expUpdate)     == recordVal(upCol));
            ASSERT(recordVal(&expTUpdate)    == recordVal(tupCol));

            ASSERT(0 == defaultAllocator.numBytesInUse());

            // Verify only the first identified metric (i.e., a 'CATEGORY',
            // 'NAME' pair) for which the macro was invoked was modified.
            ColSPtrVector  cols(Z);
            IColSPtrVector intCols(Z);
            for (int i = 1; i < NUM_IDS; ++i) {
                baem_MetricId updateId(registry.getId(IDS[i], "update"));
                baem_MetricId tupdateId(registry.getId(IDS[i], "Tupdate"));

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
                baem_MetricsManager *mgr = 0;
                if (initializeDefault) {
                    mgr = baem_DefaultMetricsManager::create(Z);
                }
                else {
                    mgr = new (*Z) baem_MetricsManager(Z);
                }
                Registry&   registry   = mgr->metricRegistry();
                Repository& repository = mgr->collectorRepository();

                baem_MetricId updateId(registry.getId(IDS[0], "update"));
                baem_MetricId tupdateId(registry.getId(IDS[0], "Tupdate"));

                baem_Collector expUpdate(updateId);
                baem_Collector expTUpdate(tupdateId);
                for (int i = 0; i < NUM_IDS; ++i) {
                    for (int j = 0; j < NUM_UPDATES; ++j) {
                        BAEM_METRICS_UPDATE(IDS[i], "update", UPDATES[j]);
                        BAEM_METRICS_TYPED_UPDATE(IDS[i],
                                                  "Tupdate",
                                                  UPDATES[j],
                                                  Type::BAEM_TOTAL);
                        expUpdate.update(UPDATES[j]);
                        expTUpdate.update(UPDATES[j]);
                    }
                }
                if (initializeDefault) {
                    baem_Collector *upCol   =
                                      repository.getDefaultCollector(updateId);
                    baem_Collector *tupCol  =
                                     repository.getDefaultCollector(tupdateId);

                    ASSERT(recordVal(&expUpdate)     == recordVal(upCol));
                    ASSERT(recordVal(&expTUpdate)    == recordVal(tupCol));
                    baem_DefaultMetricsManager::destroy();
                }
                else {
                    Z->deleteObjectRaw(mgr);
                }
            }

            ASSERT(0 == defaultAllocator.numBytesInUse());
        }
        if (veryVerbose) cout << "\tVerify multiple metric variants.\n";
        {
            baem_DefaultMetricsManagerScopedGuard guard(Z);
            baem_MetricsManager  *manager =
                                        baem_DefaultMetricsManager::instance();
            baem_MetricRegistry& registry = manager->metricRegistry();
            baem_CollectorRepository& repository =
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
                BAEM_METRICS_UPDATE2("A",
                                     "A", 1.0, "B", 1.0);
                BAEM_METRICS_UPDATE3("A",
                                     "A", 2.0, "B", 2.0, "C", 2.0);
                BAEM_METRICS_UPDATE4("A",
                                     "A", 3.0, "B", 3.0, "C", 3.0, "D", 3.0);
                BAEM_METRICS_UPDATE5("A",
                                     "A", 4.0, "B", 4.0, "C", 4.0, "D", 4.0,
                                     "E", 4.0);
                BAEM_METRICS_UPDATE6("A",
                                     "A", 5.0, "B", 5.0, "C", 5.0, "D", 5.0,
                                     "E", 5.0, "F", 5.0);
            }

            baem_Collector *A_COL = repository.getDefaultCollector("A", "A");
            baem_Collector *B_COL = repository.getDefaultCollector("A", "B");
            baem_Collector *C_COL = repository.getDefaultCollector("A", "C");
            baem_Collector *D_COL = repository.getDefaultCollector("A", "D");
            baem_Collector *E_COL = repository.getDefaultCollector("A", "E");
            baem_Collector *F_COL = repository.getDefaultCollector("A", "F");
            baem_Collector *G_COL = repository.getDefaultCollector("A", "G");

            if (veryVeryVerbose) {
                P_(recordVal(A_COL)); P_(recordVal(B_COL));
                P_(recordVal(C_COL)); P(recordVal(D_COL));
                P_(recordVal(E_COL)); P_(recordVal(F_COL));
                P(recordVal(G_COL));
            }
            ASSERT(baem_MetricRecord(A_ID, 50, 150, 1, 5) == recordVal(A_COL));
            ASSERT(baem_MetricRecord(B_ID, 50, 150, 1, 5) == recordVal(B_COL));
            ASSERT(baem_MetricRecord(C_ID, 40, 140, 2, 5) == recordVal(C_COL));
            ASSERT(baem_MetricRecord(D_ID, 30, 120, 3, 5) == recordVal(D_COL));
            ASSERT(baem_MetricRecord(E_ID, 20,  90, 4, 5) == recordVal(E_COL));
            ASSERT(baem_MetricRecord(F_ID, 10,  50, 5, 5) == recordVal(F_COL));
            ASSERT(baem_MetricRecord(G_ID)                == recordVal(G_COL));

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
            cout << "\tTesting BAEM_METRICS_UPDATE, BAEM_METRICS_INCREMENT"
                 << endl;
        {
            baem_DefaultMetricsManagerScopedGuard guard(Z);
            ASSERT(0 == defaultAllocator.numBytesInUse());

            baem_MetricsManager  *manager =
                                        baem_DefaultMetricsManager::instance();
            baem_MetricRegistry& registry = manager->metricRegistry();
            baem_CollectorRepository& repository =
                                                manager->collectorRepository();

            ASSERT(0 == defaultAllocator.numBytesInUse());

            const Id A_ID = registry.getId("A", "A");
            const Id B_ID = registry.getId("B", "B");
            const Id C_ID = registry.getId("C", "C");
            const Id D_ID = registry.getId("D", "D");
            const Id E_ID = registry.getId("E", "E");
            const Id F_ID = registry.getId("F", "F");

            for (int i = 0; i < 10; ++i) {
                BAEM_METRICS_UPDATE("A", "A", 5);
                BAEM_METRICS_DYNAMIC_UPDATE("B", "B", 5);

            }
            ColSPtrVector  cols(Z);
            IColSPtrVector intCols(Z);
            ASSERT(0 == repository.getAddedCollectors(&cols,&intCols,A_ID));
            ASSERT(0 == repository.getAddedCollectors(&cols,&intCols,B_ID));

            baem_Collector *A_COL = repository.getDefaultCollector("A", "A");
            baem_Collector *B_COL = repository.getDefaultCollector("B", "B");
            baem_Collector *C_COL = repository.getDefaultCollector("C", "C");

            ASSERT(baem_MetricRecord(A_ID, 10, 50, 5, 5) == recordVal(A_COL));
            ASSERT(baem_MetricRecord(B_ID, 10, 50, 5, 5) == recordVal(B_COL));
            ASSERT(baem_MetricRecord(C_ID) == recordVal(C_COL));

            A_COL->reset(); B_COL->reset(); C_COL->reset();

            const char *IDS[] = {"A", "B", "C" };
            const int NUM_IDS = sizeof IDS / sizeof *IDS;
            for (int i = 0; i < NUM_IDS; ++i) {
                BAEM_METRICS_UPDATE(IDS[i], IDS[i], i + 1);
            }
            ASSERT(0 == repository.getAddedCollectors(&cols,&intCols,A_ID));
            ASSERT(0 == repository.getAddedCollectors(&cols,&intCols,B_ID));
            ASSERT(0 == repository.getAddedCollectors(&cols,&intCols,C_ID));

            ASSERT(baem_MetricRecord(A_ID, 3, 6, 1, 3) == recordVal(A_COL));
            ASSERT(baem_MetricRecord(B_ID) == recordVal(B_COL));
            ASSERT(baem_MetricRecord(C_ID) == recordVal(C_COL));

            A_COL->reset(); B_COL->reset(); C_COL->reset();
            for (int i = 0; i < NUM_IDS; ++i) {
                BAEM_METRICS_DYNAMIC_UPDATE(IDS[i], IDS[i], i + 1);
            }
            ASSERT(0 == repository.getAddedCollectors(&cols,&intCols,A_ID));
            ASSERT(0 == repository.getAddedCollectors(&cols,&intCols,B_ID));
            ASSERT(0 == repository.getAddedCollectors(&cols,&intCols,C_ID));

            ASSERT(baem_MetricRecord(A_ID, 1, 1, 1, 1) == recordVal(A_COL));
            ASSERT(baem_MetricRecord(B_ID, 1, 2, 2, 2) == recordVal(B_COL));
            ASSERT(baem_MetricRecord(C_ID, 1, 3, 3, 3) == recordVal(C_COL));

            A_COL->reset(); B_COL->reset();

            ASSERT(Type::BAEM_UNSPECIFIED ==
                   A_ID.description()->preferredPublicationType());
            ASSERT(Type::BAEM_UNSPECIFIED ==
                   B_ID.description()->preferredPublicationType());

            if (veryVerbose)
                cout << "\tTesting TYPED MACROS" << endl;

            for (int i = 0; i < 10; ++i) {
                BAEM_METRICS_TYPED_UPDATE("A", "A", 5.0, Type::BAEM_RATE);
                BAEM_METRICS_TYPED_UPDATE("B", "B", 1.0, Type::BAEM_MIN);
            }
            ASSERT(baem_MetricRecord(A_ID, 10, 50, 5, 5) == recordVal(A_COL));
            ASSERT(baem_MetricRecord(B_ID, 10, 10, 1, 1) == recordVal(B_COL));

            ASSERT(Type::BAEM_RATE ==
                   A_ID.description()->preferredPublicationType());
            ASSERT(Type::BAEM_MIN  ==
                   B_ID.description()->preferredPublicationType());
        }

        if (veryVerbose)
            cout << "\tTesting BAEM_METRICS_IF_CATEGORY_ENABLED"
                 << endl;
        {
            ASSERT(!breathingTestIfEnabledA());

            baem_DefaultMetricsManagerScopedGuard guard(Z);
            baem_MetricsManager *manager  =
                                        baem_DefaultMetricsManager::instance();
            baem_MetricRegistry& registry = manager->metricRegistry();

            ASSERT(breathingTestIfEnabledA());
            registry.setCategoryEnabled(registry.getCategory("A"), false);
            ASSERT(!breathingTestIfEnabledA());

            registry.setCategoryEnabled(registry.getCategory("A"), true);
            ASSERT(breathingTestIfEnabledA());
        }
        ASSERT(!breathingTestIfEnabledA());

        if (veryVerbose) {
            cout << "\tTesting BAEM_METRICS_UPDATE[0-5]" << bsl::endl;
        }
        {
            baem_DefaultMetricsManagerScopedGuard guard(Z);
            baem_MetricsManager  *manager =
                                        baem_DefaultMetricsManager::instance();
            baem_MetricRegistry& registry = manager->metricRegistry();
            baem_CollectorRepository& repository =
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
                BAEM_METRICS_UPDATE2("A",
                                     "A", 1.0, "B", 1.0);
                BAEM_METRICS_UPDATE3("A",
                                     "A", 2.0, "B", 2.0, "C", 2.0);
                BAEM_METRICS_UPDATE4("A",
                                     "A", 3.0, "B", 3.0, "C", 3.0, "D", 3.0);
                BAEM_METRICS_UPDATE5("A",
                                     "A", 4.0, "B", 4.0, "C", 4.0, "D", 4.0,
                                     "E", 4.0);
                BAEM_METRICS_UPDATE6("A",
                                     "A", 5.0, "B", 5.0, "C", 5.0, "D", 5.0,
                                     "E", 5.0, "F", 5.0);
            }

            baem_Collector *A_COL = repository.getDefaultCollector("A", "A");
            baem_Collector *B_COL = repository.getDefaultCollector("A", "B");
            baem_Collector *C_COL = repository.getDefaultCollector("A", "C");
            baem_Collector *D_COL = repository.getDefaultCollector("A", "D");
            baem_Collector *E_COL = repository.getDefaultCollector("A", "E");
            baem_Collector *F_COL = repository.getDefaultCollector("A", "F");
            baem_Collector *G_COL = repository.getDefaultCollector("A", "G");

            if (veryVeryVerbose) {
                P_(recordVal(A_COL)); P_(recordVal(B_COL));
                P_(recordVal(C_COL)); P(recordVal(D_COL));
                P_(recordVal(E_COL)); P_(recordVal(F_COL));
                P(recordVal(G_COL));
            }
            ASSERT(baem_MetricRecord(A_ID, 50, 150, 1, 5) == recordVal(A_COL));
            ASSERT(baem_MetricRecord(B_ID, 50, 150, 1, 5) == recordVal(B_COL));
            ASSERT(baem_MetricRecord(C_ID, 40, 140, 2, 5) == recordVal(C_COL));
            ASSERT(baem_MetricRecord(D_ID, 30, 120, 3, 5) == recordVal(D_COL));
            ASSERT(baem_MetricRecord(E_ID, 20,  90, 4, 5) == recordVal(E_COL));
            ASSERT(baem_MetricRecord(F_ID, 10,  50, 5, 5) == recordVal(F_COL));
            ASSERT(baem_MetricRecord(G_ID)                == recordVal(G_COL));

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
            cout << "\tTesting BAEM_METRICS_INT_UPDATE[0-5]" << bsl::endl;
        }
        {
            baem_DefaultMetricsManagerScopedGuard guard(Z);
            baem_MetricsManager  *manager =
                                        baem_DefaultMetricsManager::instance();
            baem_MetricRegistry& registry = manager->metricRegistry();
            baem_CollectorRepository& repository =
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
                BAEM_METRICS_INT_UPDATE2("A",
                                         "A", 1, "B", 1);
                BAEM_METRICS_INT_UPDATE3("A",
                                         "A", 2, "B", 2, "C", 2);
                BAEM_METRICS_INT_UPDATE4("A",
                                         "A", 3, "B", 3, "C", 3, "D", 3);
                BAEM_METRICS_INT_UPDATE5("A",
                                         "A", 4, "B", 4, "C", 4, "D", 4,
                                         "E", 4);
                BAEM_METRICS_INT_UPDATE6("A",
                                         "A", 5, "B", 5, "C", 5, "D", 5,
                                         "E", 5, "F", 5);
            }

            baem_IntegerCollector *A_COL =
                repository.getDefaultIntegerCollector("A", "A");
            baem_IntegerCollector *B_COL =
                repository.getDefaultIntegerCollector("A", "B");
            baem_IntegerCollector *C_COL =
                repository.getDefaultIntegerCollector("A", "C");
            baem_IntegerCollector *D_COL =
                repository.getDefaultIntegerCollector("A", "D");
            baem_IntegerCollector *E_COL =
                repository.getDefaultIntegerCollector("A", "E");
            baem_IntegerCollector *F_COL =
                repository.getDefaultIntegerCollector("A", "F");
            baem_IntegerCollector *G_COL =
                repository.getDefaultIntegerCollector("A", "G");

            if (veryVeryVerbose) {
                P_(recordVal(A_COL)); P_(recordVal(B_COL));
                P_(recordVal(C_COL)); P(recordVal(D_COL));
                P_(recordVal(E_COL)); P_(recordVal(F_COL));
                P(recordVal(G_COL));
            }
            ASSERT(baem_MetricRecord(A_ID, 50, 150, 1, 5) == recordVal(A_COL));
            ASSERT(baem_MetricRecord(B_ID, 50, 150, 1, 5) == recordVal(B_COL));
            ASSERT(baem_MetricRecord(C_ID, 40, 140, 2, 5) == recordVal(C_COL));
            ASSERT(baem_MetricRecord(D_ID, 30, 120, 3, 5) == recordVal(D_COL));
            ASSERT(baem_MetricRecord(E_ID, 20,  90, 4, 5) == recordVal(E_COL));
            ASSERT(baem_MetricRecord(F_ID, 10,  50, 5, 5) == recordVal(F_COL));
            ASSERT(baem_MetricRecord(G_ID)                == recordVal(G_COL));

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
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
