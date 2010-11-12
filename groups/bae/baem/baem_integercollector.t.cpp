// baem_integercollector.t.cpp  -*-C++-*-
#include <baem_integercollector.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bcema_testallocator.h>
#include <bcemt_barrier.h>
#include <bcep_fixedthreadpool.h>
#include <bdef_bind.h>

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
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The 'baem_IntegerCollector' is a mechanism for collecting and recording
// aggregated metric values.  Ensure values can be accumulated into and read
// out of the container, and that the operations are thread safe.
//-----------------------------------------------------------------------------
// CREATORS
// [ 3]  baem_Collector(const baem_MetricId& metric);
// [ 3]  ~baem_Collector();
//
// MANIPULATORS
// [ 7]  void reset();
// [ 6]  void loadAndReset(baem_MetricRecord *records);
// [ 2]  void update(int value);
// [ 5]  void accumulateCountTotalMinMax(int count,
//                                       int total,
//                                       int min,
//                                       int max);
// [ 4]  void setCountTotalMinMax(int count, int total, int min, int max);
//
// ACCESSORS
// [ 2]  const baem_MetricId& metric() const;
// [ 2]  void load(baem_MetricRecord *record) const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 8] CONCURRENCY TEST
// [ 9] USAGE EXAMPLE

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

typedef baem_IntegerCollector  Obj;
typedef baem_MetricRecord      Rec;
typedef baem_MetricDescription Desc;
typedef baem_MetricId          Id;

//=============================================================================
//                  GLOBAL STUB CLASSES FOR TESTING
//-----------------------------------------------------------------------------

class ConcurrencyTest {
    // Invoke a set of operations operations synchronously.

    // DATA
    bcep_FixedThreadPool   d_pool;
    bcemt_Barrier          d_barrier;
    baem_IntegerCollector *d_collector_p;
    bslma_Allocator       *d_allocator_p;

    // PRIVATE MANIPULATORS
    void execute();
        // Execute a single test.

  public:

    // CREATORS
    ConcurrencyTest(int                    numThreads,
                 baem_IntegerCollector *collector,
                 bslma_Allocator       *basicAllocator)
    : d_pool(numThreads, 1000, basicAllocator)
    , d_barrier(numThreads)
    , d_collector_p(collector)
    , d_allocator_p(basicAllocator)
    {
        d_pool.start();
    }

    ~ConcurrencyTest() {}

    //  MANIPULATORS
    void runTest();
        // Run the test.
};

void ConcurrencyTest::execute()
{

    Obj *mX = d_collector_p; const Obj *MX = mX;

    baem_MetricRecord empty(MX->metricId());
    baem_MetricRecord r1(MX->metricId(), 1, 2, 3, 5);
    baem_MetricRecord r2(MX->metricId(), 7, 8, 9, 10);

    // Test simultaneous set and loads, verify values are set/loaded
    // atomically.
    d_barrier.wait();
    for(int i = 0; i < 10; ++i) {
        baem_MetricRecord  result;
        baem_MetricRecord *rec = (0 == i % 2) ? &r1 : &r2;
        mX->setCountTotalMinMax(rec->count(),
                                rec->total(),
                                rec->min(),
                                rec->max());
        MX->load(&result);
        ASSERT(r1 == result || r2 == result);

    }
    d_barrier.wait();

    mX->reset();

    // Test simultaneous accumulate and loads, verify values are
    // accumulated/loaded atomically.
    d_barrier.wait();
    for(int i = 0; i < 10; ++i) {
        baem_MetricRecord  result;
        mX->accumulateCountTotalMinMax(1, 1, -i, i);
        MX->load(&result);

        ASSERT(result.count() >= i);
        ASSERT(result.total() >= i);
        ASSERT(result.count() == result.total());
        ASSERT(result.min() <= -i);
        ASSERT(result.max() >= i);
        ASSERT(result.min() == -result.max());
    }
    d_barrier.wait();

    mX->reset();

    // Test simultaneous set and loadAndReset, verify values are loaded and
    // reset atomically.
    d_barrier.wait();
    for(int i = 0; i < 10; ++i) {
        baem_MetricRecord  result;
        mX->setCountTotalMinMax(r1.count(), r1.total(), r1.min(), r1.max());
        mX->loadAndReset(&result);

        ASSERT(result == r1 || result == empty);
    }
    d_barrier.wait();
}

void ConcurrencyTest::runTest()
{
    bdef_Function<void(*)()> job = bdef_BindUtil::bindA(
                                                     d_allocator_p,
                                                     &ConcurrencyTest::execute,
                                                     this);
    for (int i = 0; i < d_pool.numThreads(); ++i) {
        d_pool.enqueueJob(job);
    }
    d_pool.drain();
}

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------


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

    baem_Category cat_A("A", true);
    Desc desc_A(&cat_A, "A"); const Desc *DESC_A = &desc_A;
    Desc desc_B(&cat_A, "B"); const Desc *DESC_B = &desc_B;
    Desc desc_C(&cat_A, "C"); const Desc *DESC_C = &desc_C;
    Desc desc_D(&cat_A, "D"); const Desc *DESC_D = &desc_D;
    Desc desc_E(&cat_A, "E"); const Desc *DESC_E = &desc_E;

    Id metric_A(DESC_A); const Id& METRIC_A = metric_A;
    Id metric_B(DESC_B); const Id& METRIC_B = metric_B;
    Id metric_C(DESC_C); const Id& METRIC_C = metric_C;
    Id metric_D(DESC_D); const Id& METRIC_D = metric_D;
    Id metric_E(DESC_E); const Id& METRIC_E = metric_E;

    switch (test) { case 0:  // Zero is always the leading case.
      case 9: {
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
// The following example creates a 'baem_IntegerCollector', modifies its
// values, then collects a 'baem_MetricRecord'.
//
// We start by creating 'baem_MetricId' object by hand; however, in practice
// an id should be obtained from a 'baem_MetricRegistry' object (such as the
// one owned by a 'baem_MetricsManager').
//..
    baem_Category           myCategory("MyCategory");
    baem_MetricDescription  description(&myCategory, "MyMetric");
    baem_MetricId           myMetric(&description);
//..
// Now we create a 'baem_IntegerCollector' object using 'myMetric' and use the
// 'update' method to update its collected value.
//..
    baem_IntegerCollector collector(myMetric);

    collector.update(1);
    collector.update(3);
//..
// The collector accumulated the values 1 and 3.  The result should have a
// count of 2, a total of 4 (3 + 1), a max of 3 (max(3,1)), and a min of 1
// (min(3,1)).
//..
    baem_MetricRecord record;
    collector.loadAndReset(&record);

        ASSERT(myMetric == record.metricId());
        ASSERT(2        == record.count());
        ASSERT(4        == record.total());
        ASSERT(1        == record.min());
        ASSERT(3        == record.max());
//..

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // CONCURRENCY TEST
        //
        // Testing:
        //     Thread-safety of manipulator methods
        //
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TEST CONCURRENCY" << endl
                                  << "================" << endl;

        bcema_TestAllocator defaultAllocator;
        bslma_DefaultAllocatorGuard guard(&defaultAllocator);

        bcema_TestAllocator testAllocator;
        baem_IntegerCollector mX(METRIC_A);
        {
            ConcurrencyTest tester(10, &mX, &defaultAllocator);
            tester.runTest();
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING ADDITIONAL MANIPULATOR: reset
        //
        // Concerns:
        //   reset
        //
        // Plan:
        //   Create a table of test values.  Iterate over the test values
        //   set them on the container.  Verify load returns the
        //   correct value, call reset, verify the value is the default value.
        //
        // Testing:
        // void reset()
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting reset." << endl;
        struct {
            const Desc *d_id;
            int         d_count;
            int         d_total;
            int         d_min;
            int         d_max;
        } VALUES [] = {
            {      0,      0,        0,           0,           0 },
            { DESC_A,      1,        1,           1,           1 },
            { DESC_B,      1,        2,           3,           4 },
            { DESC_C,     -1,       -2,          -3,          -4 },
            { DESC_D, 100000,  2000000,     3000000,     4000000 },
            { DESC_E, INT_MAX, INT_MAX, INT_MIN + 1, INT_MAX - 1 },
            { DESC_A, INT_MAX, INT_MIN, INT_MAX - 1, INT_MIN + 1 }
        };
        const int NUM_VALUES = sizeof(VALUES)/sizeof(*VALUES);


        for (int i = 0; i < NUM_VALUES; ++i) {
            baem_MetricRecord r1, r2;
            Obj mX(Id(VALUES[i].d_id)); const Obj& MX = mX;
            mX.setCountTotalMinMax(VALUES[i].d_count,
                                   VALUES[i].d_total,
                                   VALUES[i].d_min,
                                   VALUES[i].d_max);
            MX.load(&r1);
            ASSERT(VALUES[i].d_id    == r1.metricId());
            ASSERT(VALUES[i].d_count == r1.count());
            ASSERT(VALUES[i].d_total == r1.total());
            ASSERT(VALUES[i].d_min   == r1.min());
            ASSERT(VALUES[i].d_max   == r1.max());

            mX.reset();
            MX.load(&r2);
            ASSERT(VALUES[i].d_id    == r2.metricId());
            ASSERT(0                 == r2.count());
            ASSERT(0.0               == r2.total());
            ASSERT(Rec::DEFAULT_MIN  == r2.min());
            ASSERT(Rec::DEFAULT_MAX  == r2.max());
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING ADDITIONAL MANIPULATOR: loadAndReset
        //
        // Concerns:
        //   loadAndReset
        //
        // Plan:
        //   Create a table of test values.  Iterate over the test values
        //   set them on the container.  Verify loadAndReset returns the
        //   correct values and that the collector is left in the correct
        //   state.
        //
        // Testing:
        // void loadAndReset(baem_MetricRecord *);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting loadAndReset." << endl;
        struct {
            const Desc *d_id;
            int         d_count;
            int         d_total;
            int         d_min;
            int         d_max;
        } VALUES [] = {
            {      0,      0,        0,           0,           0 },
            { DESC_A,      1,        1,           1,           1 },
            { DESC_B,      1,        2,           3,           4 },
            { DESC_C,     -1,       -2,          -3,          -4 },
            { DESC_D, 100000,  2000000,     3000000,     4000000 },
            { DESC_E, INT_MAX, INT_MAX, INT_MIN + 1, INT_MAX - 1 },
            { DESC_A, INT_MAX, INT_MIN, INT_MAX - 1, INT_MIN + 1 }
        };
        const int NUM_VALUES = sizeof(VALUES)/sizeof(*VALUES);


        for (int i = 0; i < NUM_VALUES; ++i) {
            baem_MetricRecord r1, r2, r3;
            Obj mX(Id(VALUES[i].d_id)); const Obj& MX = mX;
            mX.setCountTotalMinMax(VALUES[i].d_count,
                                   VALUES[i].d_total,
                                   VALUES[i].d_min,
                                   VALUES[i].d_max);
            MX.load(&r1);
            ASSERT(VALUES[i].d_id    == r1.metricId());
            ASSERT(VALUES[i].d_count == r1.count());
            ASSERT(VALUES[i].d_total == r1.total());
            ASSERT(VALUES[i].d_min   == r1.min());
            ASSERT(VALUES[i].d_max   == r1.max());

            mX.loadAndReset(&r2);
            ASSERT(VALUES[i].d_id    == r2.metricId());
            ASSERT(VALUES[i].d_count == r2.count());
            ASSERT(VALUES[i].d_total == r2.total());
            ASSERT(VALUES[i].d_min   == r2.min());
            ASSERT(VALUES[i].d_max   == r2.max());

            MX.load(&r3);
            ASSERT(VALUES[i].d_id    == r3.metricId());
            ASSERT(0                 == r3.count());
            ASSERT(0.0               == r3.total());
            ASSERT(Rec::DEFAULT_MIN  == r3.min());
            ASSERT(Rec::DEFAULT_MAX  == r3.max());
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING ADDITIONAL MANIPULATOR: accumulateCountTotalMinMax
        //
        // Concerns:
        //   accumulateCountTotalMinMax
        //
        // Plan:
        //   Create a table of test values.  Iterate over the test values
        //   selecting the first value, then iterate over the test values
        //   starting at that first value and accumulate the values in the
        //   collector.  Maintain a separate count, total, min, and max and
        //   verify the values returned by the collector.
        //
        // Testing:
        // void accumulateCountTotalMinMax(int , int , int , int );
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting accumulateCountTotalMinMax." << endl;

        struct {
            int d_count;
            int d_total;
            int d_min;
            int d_max;
        } VALUES [] = {
            {         0,         0,               0,                0 },
            {         1,         1,               1,                1 },
            {      1210,   INT_MAX,     INT_MIN + 1,      INT_MAX - 1 },
            {         1,         2,               3,                4 },
            {        10,        -2,              -3,               -4 },
            {      123110, INT_MIN,     INT_MAX - 1,      INT_MIN + 1 },
            {     -12311,   231413,           -1123,           410001 },
         };
        const int NUM_VALUES = sizeof(VALUES)/sizeof(*VALUES);

        for (int i = 0; i < NUM_VALUES; ++i) {
            Obj mX(0); const Obj& MX = mX;

            int                      count = 0;
            bsls_PlatformUtil::Int64 total = 0;
            int                      min   = VALUES[i].d_min;
            int                      max   = VALUES[i].d_max;
            for (int j = 0; j < NUM_VALUES; ++j) {
                int idx = (i + j) % NUM_VALUES;
                baem_MetricRecord r;

                count += VALUES[idx].d_count;
                total += VALUES[idx].d_total;
                min   =  bsl::min(VALUES[idx].d_min, min);
                max   =  bsl::max(VALUES[idx].d_max, max);

                mX.accumulateCountTotalMinMax(VALUES[idx].d_count,
                                              VALUES[idx].d_total,
                                              VALUES[idx].d_min,
                                              VALUES[idx].d_max);
                MX.load(&r);
                ASSERT(0     == r.metricId());
                ASSERT(count == r.count());
                ASSERT(total == r.total());
                ASSERT(min   == r.min());
                ASSERT(max   == r.max());
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING ADDITIONAL MANIPULATOR: setCountTotalMinMax
        //
        // Concerns:
        //   setCountTotalMinMax
        //
        // Plan:
        //   Create a table of test values and set them to the object, verify
        //   that values are set correctly.
        //
        // Testing:
        // void setCountTotalMinMax(int , int , int , int );
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting setCountTotalMinMax." << endl;

        struct {
            int d_count;
            int d_total;
            int d_min;
            int d_max;
        } VALUES [] = {
            {       0,           0,           0,           0 },
            {       1,           1,           1,           1 },
            {       1,           2,           3,           4 },
            {      -1,          -2,          -3,          -4 },
            {  100000,     2000000,     3000000,     4000000 },
            { INT_MAX,     INT_MAX, INT_MIN + 1, INT_MAX - 1 },
            { INT_MAX,     INT_MIN, INT_MAX - 1, INT_MIN + 1 }
        };
        const int NUM_VALUES = sizeof(VALUES)/sizeof(*VALUES);

        Obj mX(0); const Obj& MX = mX;
        for (int i = 0; i < NUM_VALUES; ++i) {
            baem_MetricRecord r;
            mX.setCountTotalMinMax(VALUES[i].d_count,
                                   VALUES[i].d_total,
                                   VALUES[i].d_min,
                                   VALUES[i].d_max);
            MX.load(&r);
            ASSERT(0                 == r.metricId());
            ASSERT(VALUES[i].d_count == r.count());
            ASSERT(VALUES[i].d_total == r.total());
            ASSERT(VALUES[i].d_min   == r.min());
            ASSERT(VALUES[i].d_max   == r.max());
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING CONSTRUCTORS
        //
        // Concerns:
        //   Test the constructor arguments
        //
        // Plan:
        //   Verify the constructor by passing a value from a table of values
        //   and verifying the object is initialized with the value..
        //
        //
        // Testing:
        //   baem_Collector(const baem_MetricId^ )
        //   ~baem_Collector()
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting constructor." << endl;

        const Desc *IDS[]  = { 0, DESC_A, DESC_B, DESC_C, DESC_D, DESC_E };
        const int NUM_IDS = sizeof(IDS)/sizeof(*IDS);
        for (int i = 0; i < NUM_IDS; ++i) {
            Obj mX(IDS[i]); const Obj& MX = mX;
            ASSERT(IDS[i] == MX.metricId());
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING BASIC MANIPULATORS and ACCESSORS (BOOTSTRAP):
        //
        // Concerns:
        //   The primary fields must be correctly modifiable and accessible.
        //
        // Plan:
        //   First, verify the constructor by testing the value of the
        //   resulting object.
        //
        //   Next, for a sequence of independent test values, add elements to
        //   the container and used the basic accessors to verify the
        //   modification.
        //
        // Testing:
        //   baem_Collector(const baem_MetricId *)
        //   void update(int );
        //
        //   const baem_MetricId *metric() const;
        //   void load(baem_MetricRecord *record) const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting basic mainpulator and accessors."
                          << endl;

        int UPDATES[] = { 0, 12, -1321123, 2131241, 1321,
                            43145, 1, -1, INT_MIN + 1, INT_MAX - 1};
        const int NUM_UPDATES = sizeof(UPDATES)/sizeof(*UPDATES);

        for (int i = 0; i < NUM_UPDATES; ++i) {
            const Desc *METRIC = (const Desc *)(i + 1);
            Obj mX(METRIC); const Obj& MX = mX;


            baem_MetricRecord r1, r2;
            MX.load(&r1);
            ASSERT(METRIC           == r1.metricId().description());
            ASSERT(0                == r1.count());
            ASSERT(0                == r1.total());
            ASSERT(Rec::DEFAULT_MIN == r1.min());
            ASSERT(Rec::DEFAULT_MAX == r1.max());

            bsls_PlatformUtil::Int64 total = 0;
            int                      min   = UPDATES[i];
            int                      max   = UPDATES[i];
            for (int j = 0; j < NUM_UPDATES; ++j) {
                const int INDEX = (i + j) % NUM_UPDATES;
                mX.update(UPDATES[INDEX]);

                total += UPDATES[INDEX];
                min   = bsl::min(min, UPDATES[INDEX]);
                max   = bsl::max(max, UPDATES[INDEX]);

                MX.load(&r1);
                ASSERT(METRIC == r1.metricId().description());
                ASSERT(j + 1  == r1.count());
                ASSERT(total  == r1.total());
                ASSERT(min    == r1.min());
                LOOP2_ASSERT(max, r1.max(), max    == r1.max());

                MX.load(&r2);
                ASSERT(r1 == r2);
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Developers' Sandbox.
        //
        // Plan:
        //   Perform ad-hoc test of the primary modifiers and accessors.
        //
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        Obj mX(METRIC_A); const Obj& MX = mX;
        Obj mY(METRIC_B); const Obj& MY = mY;

        ASSERT(METRIC_A == MX.metricId());
        ASSERT(METRIC_B == MY.metricId());

        baem_MetricRecord r1, r2;

        MX.load(&r1);
        ASSERT(METRIC_A == r1.metricId());
        ASSERT(0  == r1.count());
        ASSERT(0  == r1.total());
        ASSERT(Rec::DEFAULT_MIN == r1.min());
        ASSERT(Rec::DEFAULT_MAX == r1.max());

        MY.load(&r2);
        ASSERT(METRIC_B == r2.metricId());
        ASSERT(0  == r2.count());
        ASSERT(0  == r2.total());
        ASSERT(Rec::DEFAULT_MIN == r2.min());
        ASSERT(Rec::DEFAULT_MAX == r2.max());

        mX.update(1);
        MX.load(&r1);
        ASSERT(METRIC_A == r1.metricId());
        ASSERT(1  == r1.count());
        ASSERT(1  == r1.total());
        ASSERT(1  == r1.min());
        ASSERT(1  == r1.max());

        mX.update(2);
        MX.load(&r1);
        ASSERT(METRIC_A == r1.metricId());
        ASSERT(2  == r1.count());
        ASSERT(3  == r1.total());
        ASSERT(1  == r1.min());
        ASSERT(2  == r1.max());

        mX.update(-5);
        MX.load(&r1);
        ASSERT(METRIC_A == r1.metricId());
        ASSERT(3  == r1.count());
        ASSERT(-2 == r1.total());
        ASSERT(-5 == r1.min());
        ASSERT(2  == r1.max());

        mX.reset();
        MX.load(&r1);
        ASSERT(METRIC_A == r1.metricId());
        ASSERT(0  == r1.count());
        ASSERT(0  == r1.total());
        ASSERT(Rec::DEFAULT_MIN == r1.min());
        ASSERT(Rec::DEFAULT_MAX == r1.max());

        mX.update(3);
        mX.loadAndReset(&r1);
        ASSERT(METRIC_A == r1.metricId());
        ASSERT(1  == r1.count());
        ASSERT(3  == r1.total());
        ASSERT(3  == r1.min());
        ASSERT(3 == r1.max());

        MX.load(&r1);
        ASSERT(METRIC_A == r1.metricId());
        ASSERT(0  == r1.count());
        ASSERT(0  == r1.total());
        ASSERT(Rec::DEFAULT_MIN == r1.min());
        ASSERT(Rec::DEFAULT_MAX == r1.max());
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
