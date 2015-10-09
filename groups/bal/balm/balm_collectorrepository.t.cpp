// balm_collectorrepository.t.cpp                                     -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balm_collectorrepository.h>

#include <bslma_testallocator.h>
#include <bslmt_barrier.h>
#include <bdlmt_fixedthreadpool.h>
#include <bdlf_bind.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bsls_assert.h>

#include <bsl_algorithm.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_functional.h>
#include <bsl_iostream.h>
#include <bsl_map.h>
#include <bsl_ostream.h>
#include <bsl_set.h>
#include <bsl_sstream.h>
#include <bsl_vector.h>

#include <bsl_c_stdio.h>

#if defined(BSLS_PLATFORM_CMP_MSVC)
#define snprintf _snprintf_s
#endif

#include <bslim_testutil.h>

using namespace BloombergLP;

using bsl::cout;
using bsl::endl;
using bsl::flush;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] CollectorRepository(MetricRegistry *, bslma::Allocator *);
// [ 2] ~CollectorRepository();
// MANIPULATORS
// [ 7] void collect(v<MetricRecord> *, const Category *);
// [ 4] void collectAndReset(v<MetricRecord> *, const Category *);
// [ 6] getDefaultCollector(const StringRef&, const StringRef&);
// [ 3] getDefaultCollector(const MetricId&);
// [ 6] getDefaultIntegerCollector(const StringRef&, const StringRef&);
// [ 3] IntegerCollector *getDefaultIntegerCollector(const MetricId&);
// [ 5] addCollector(const StringRef&, const StringRef&);
// [ 2] addCollector(const MetricId& metricId);
// [ 5] addIntegerCollector(const StringRef&, const StringRef&);
// [ 2] addIntegerCollector(const MetricId&);
// [ 2] int getAddedCollectors(v<C *> *, v<IC *> *, const MetricId&);
// [ 2] MetricRegistry &registry();
// [ 4] void collectAndReset(v<MetricRecord> *, const Category *);
// ACCESSORS
// [ 2] int getAddedCollectors(v<C*> *, v<IC*> *, MetricId& ) const;
// [ 2] const MetricRegistry& registry() const;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 8] CONCURRENCY TEST
// [ 9] USAGE EXAMPLE

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

typedef balm::MetricRegistry      Registry;
typedef balm::CollectorRepository Obj;
typedef balm::Collector           Col;
typedef balm::IntegerCollector    ICol;
typedef balm::MetricId            Id;
typedef balm::MetricRecord        Rec;
typedef bsl::shared_ptr<Col>      ColSPtr;
typedef bsl::shared_ptr<ICol>     IColSPtr;
typedef bsl::vector<ColSPtr>      ColSPtrVector;
typedef bsl::vector<IColSPtr>     IColSPtrVector;

// ============================================================================
//                 GLOBAL CLASSES AND FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

void stringId(bsl::string *resultId, const char *heading, int value)
    // Populate the specified 'resultId' with a null-terminated string
    // identifier containing the specified 'heading' concatenated with the
    // specified 'value'.  The behavior is undefined if the resulting
    // identifier would be larger than 100 (including the null terminating
    // character).
{
    char buffer[100];
    int rc = snprintf(buffer, 100, "%s-%d", heading, value);
    BSLS_ASSERT(rc < 100);
    BSLS_ASSERT(rc > 0);
    *resultId = buffer;
}

void stringId(bsl::string *resultId,
              const char  *heading,
              int          value1,
              int          value2)
    // Populate the specified 'resultId' with a null-terminated string
    // identifier containing the specified 'heading' concatenated with the
    // specified 'value1' and 'value2'.  The behavior is undefined if the
    // resulting identifier would be larger than 100 (including the null
    // terminating character).
{
    char buffer[100];
    int rc = snprintf(buffer, 100, "%s-%d-%d", heading, value1, value2);
    BSLS_ASSERT(rc < 100);
    BSLS_ASSERT(rc > 0);
    *resultId = buffer;
}

bool recordLess(const Rec& lhs, const Rec& rhs)
{
    int cmp = bsl::strcmp(lhs.metricId().categoryName(),
                          rhs.metricId().categoryName());
    if (0 == cmp) {
        cmp = bsl::strcmp(lhs.metricId().metricName(),
                          rhs.metricId().metricName());
    }
    return cmp < 0;
}

class ThreadTester {
    // Invoke a set of operations operations synchronously.

    // DATA
    bdlmt::FixedThreadPool     d_pool;
    bslmt::Barrier             d_barrier;
    balm::CollectorRepository *d_repository_p;
    bslma::Allocator          *d_allocator_p;

    // PRIVATE MANIPULATORS
    void execute();
        // Execute a single test.

  public:

    // CREATORS
    ThreadTester(int                       numThreads,
                 balm::CollectorRepository *repository,
                 bslma::Allocator         *basicAllocator)
    : d_pool(numThreads, 1000, basicAllocator)
    , d_barrier(numThreads)
    , d_repository_p(repository)
    , d_allocator_p(basicAllocator)
    {
        d_pool.start();
    }

    ~ThreadTester() {}

    //  MANIPULATORS
    void runTest();
        // Run the test.
};

void ThreadTester::execute()
{

    // On each iteration of this test, a group of identifiers unique to the
    // iteration are created.  2 of those identifiers are shared by all threads
    // (but are unique to the test iteration), while 2 are unique across all
    // threads and iterations.
    //
    // Test 1: Create a set of category and metric ids using
    // these unique and partially unique ids, and call 'getDefaultCollector',
    // 'getDefaultIntegerCollector', 'addCollector', 'addIntegerCollector', and
    // 'getAddedCollectors' for each (category, id) combination; verify the
    // returned values are correct and set the collectors to a known
    // value.  Then, call 'collectAndReset' for each category and verify the
    // results.

    const int NUM_THREADS = d_pool.numThreads();
    Obj& mX = *d_repository_p; const Obj& MX = mX;
    Registry& reg = mX.registry(); const Registry& REG = MX.registry();

    // iterate the test 10 times.
    for (int i = 0; i < 10; ++i) {
        // Create 2 strings unique for this iteration.
        bsl::string iterStringA, iterStringB;
        stringId(&iterStringA, "A", i);
        stringId(&iterStringB, "B", i);
        const char *A_VAL = iterStringA.c_str();
        const char *B_VAL = iterStringB.c_str();

        // Create 2 strings unique across all threads & iterations.
        bsl::string uniqueString1, uniqueString2;
        stringId(&uniqueString1, "U1", bslmt::ThreadUtil::selfIdAsInt(), i);
        stringId(&uniqueString2, "U2", bslmt::ThreadUtil::selfIdAsInt(), i);
        const char *S1 = uniqueString1.c_str();
        const char *S2 = uniqueString2.c_str();

        const char *CATEGORIES[] = { A_VAL, B_VAL, S1, S2 };
        const int NUM_CATEGORIES = sizeof (CATEGORIES) / sizeof (*CATEGORIES);

        const char *IDS[] = {"A", "B", S1, S2 };
        const int NUM_IDS = sizeof (IDS) / sizeof (*IDS);

        d_barrier.wait();

        // Test 1.
        // For each (category, id) combination, invoke 'getDefaultCollector',
        // 'getDefaultIntegerCollector', 'addCollector',
        // 'addIntegerCollector', and 'getAddedCollectors'.  Finally, set
        // the collectors to a known (non-default) state.

        for (int j = 0; j < NUM_CATEGORIES; ++j) {
            const char *CATEGORY = CATEGORIES[j];
            for (int k = 0; k < NUM_IDS; ++k) {
                const char *ID = IDS[k];
                balm::MetricId id = reg.getId(CATEGORY, ID);
                Col  *col   = mX.getDefaultCollector(CATEGORY, ID);
                ICol *iCol  = mX.getDefaultIntegerCollector(CATEGORY, ID);
                Col  *col2  = mX.addCollector(CATEGORY, ID).get();
                ICol *iCol2 = mX.addIntegerCollector(CATEGORY, ID).get();

                ASSERT(id == col->metricId());
                ASSERT(id == iCol->metricId());
                ASSERT(id == col2->metricId());
                ASSERT(id == iCol2->metricId());

                ColSPtrVector cols;
                IColSPtrVector iCols;
                int count = mX.getAddedCollectors(&cols, &iCols, id);

                ASSERT(2 <= count);
                ASSERT(1 <= cols.size());
                ASSERT(1 <= iCols.size());

                ASSERT(col  == mX.getDefaultCollector(CATEGORY, ID));
                ASSERT(iCol == mX.getDefaultIntegerCollector(CATEGORY, ID));

                col->setCountTotalMinMax(1, 1, 1, 1);
                iCol->setCountTotalMinMax(1, 1, 1, 1);
                col2->setCountTotalMinMax(1, 1, 1, 1);
                iCol2->setCountTotalMinMax(1, 1, 1, 1);
            }
        }

        // Call 'collectAndReset' on each test category.
        for (int j = 0; j < NUM_CATEGORIES; ++j) {
            const char *CATEGORY = CATEGORIES[j];
            const balm::Category *category = REG.findCategory(CATEGORY);

            bsl::vector<Rec> records(d_allocator_p);
            mX.collectAndReset(&records, category);
            bsl::sort(records.begin(), records.end(), recordLess);

            // If the category is unique to this thread, we know what the
            // collected values should be.
            if (CATEGORY == S1 || CATEGORY == S2) {

                // This category is unique to the current thread.
                ASSERT(NUM_IDS == records.size());
                for (int k = 0; k < NUM_IDS; ++k) {
                    const char *ID = IDS[k];
                    balm::MetricId id = reg.getId(CATEGORY, ID);
                    ASSERT(id == records[k].metricId());
                    ASSERT(4  == records[k].count());
                    ASSERT(4  == records[k].total());
                    ASSERT(1  == records[k].min());
                    ASSERT(1  == records[k].max());
                }
            }
            else {
                // This category is shared by all threads.
                ASSERT(NUM_IDS <= records.size() &&
                       (2 * NUM_THREADS) + 2 >= records.size());
                balm::MetricId idA = reg.getId(CATEGORY, "A");
                balm::MetricId idB = reg.getId(CATEGORY, "B");
                ASSERT(idA == records[0].metricId());
                ASSERT(idB == records[1].metricId());

                ASSERT(records[0].count() == records[0].total());
                ASSERT(records[1].count() == records[1].total());
            }

        }
    }
}

void ThreadTester::runTest()
{
    bsl::function<void()> job = bdlf::BindUtil::bind(&ThreadTester::execute,
                                                      this);
    for (int i = 0; i < d_pool.numThreads(); ++i) {
        d_pool.enqueueJob(job);
    }
    d_pool.drain();
}

template <class T>
bool vectorEquals(const bsl::vector<T *>&                 lhs,
                  const bsl::vector<bsl::shared_ptr<T> >& rhs)
{
    if (lhs.size() != rhs.size()) {
        return false;                                                 // RETURN
    }

    bsl::vector<T *> copyLhs(lhs);
    bsl::vector<bsl::shared_ptr<T> > copyRhs(rhs);

    bsl::sort(copyLhs.begin(), copyLhs.end(), bsl::less<T *>());
    bsl::sort(copyRhs.begin(), copyRhs.end());

    typename bsl::vector<T *>::const_iterator lhsIt = copyLhs.begin();
    typename bsl::vector<bsl::shared_ptr<T> >::const_iterator rhsIt =
                                                             copyRhs.begin();
    for (; lhsIt != copyLhs.end(); ++lhsIt, ++rhsIt) {
        if (*lhsIt != rhsIt->get()) {
            return false;                                             // RETURN
        }
    }
    return true;
}

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

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

    bslma::TestAllocator allocator; bslma::TestAllocator *Z = &allocator;
    bslma::TestAllocator defaultAllocator;
    bslma::DefaultAllocatorGuard guard(&defaultAllocator);

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
// The following example illustrates creating a 'balm::CollectorRepository',
// then looking up collectors in that repository, and finally collecting values
// from the repository.  We start by creating creating a repository and
// looking up 2 collectors and 2 integer collectors.
//..
    bslma::Allocator    *allocator = bslma::Default::allocator(0);
    balm::MetricRegistry  metricRegistry(allocator);
    balm::CollectorRepository repository(&metricRegistry, allocator);

    balm::Collector *collector1 = repository.getDefaultCollector("Test", "C1");
    balm::Collector *collector2 = repository.getDefaultCollector("Test", "C2");
    balm::IntegerCollector *intCollector1 =
                           repository.getDefaultIntegerCollector("Test", "C3");
    balm::IntegerCollector *intCollector2 =
                           repository.getDefaultIntegerCollector("Test", "C4");

        ASSERT(collector1    != collector2);
        ASSERT(collector1    == repository.getDefaultCollector("Test", "C1"));
        ASSERT(intCollector1 != intCollector2);
        ASSERT(intCollector1 ==
               repository.getDefaultIntegerCollector("Test","C3"));
//..
// We now update the values in those collectors.
//..
    collector1->update(1.0);
    collector1->update(2.0);
    collector2->update(4.0);

    intCollector1->update(5);
    intCollector2->update(6);
//..
// We can use the repository to collect recorded values from the collectors it
// manages.  Since there are collectors for four metrics, there should be four
// recorded values.  Note the order in which the records are returned is
// undefined.
//..
    bsl::vector<balm::MetricRecord> records;
    repository.collectAndReset(&records, metricRegistry.getCategory("Test"));
        ASSERT(4 == records.size());
//..
// Finally we write the recorded values to the console.
//..
    bsl::vector<balm::MetricRecord>::const_iterator it;
    for (it = records.begin(); it != records.end(); ++it) {
         bsl::cout << *it << bsl::endl;
    }
//..
// The output of the for-loop should be:
//..
//  [ Test.C1: 2 3 1 2 ]
//  [ Test.C2: 1 4 4 4 ]
//  [ Test.C3: 1 5 5 5 ]
//  [ Test.C4: 1 6 6 6 ]
//..

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // CONCURRENCY TEST
        //
        // Testing:
        //     Thread-safety of manipulators and accessors
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TEST CONCURRENCY" << endl
                                  << "================" << endl;

        bslma::TestAllocator defaultAllocator;
        bslma::DefaultAllocatorGuard guard(&defaultAllocator);

        bslma::TestAllocator      testAllocator;
        balm::MetricRegistry      registry(&testAllocator);;
        balm::CollectorRepository mX(&registry, &testAllocator);
        {
            ThreadTester tester(10, &mX, &defaultAllocator);
            tester.runTest();
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING BASIC MANIPULATOR: 'collect'
        //
        // Concerns:
        //   That the 'collector' method properly collects values of all the
        //   collectors in the supplied category with no side effects.  That
        //   the collected records for a single metric are aggregated correctly
        //   (across multiple instances of 'balm::Collector' and
        //   'balm::IntegerCollector' for the same metric).
        //
        // Plan:
        //   For an independent sequences of metric ids and categories, select
        //   the category under test, then create multiple collectors (and
        //   integer collectors) for each metric id/category combination, and
        //   update the collectors, using identifiable values.  Then for the
        //   category under test, collect records using the 'collect' method
        //   and verify the records match the expected aggregates of the
        //   identifiable values, and verify the corresponding collectors for
        //   those metrics have not been modified to their default value.
        //   Finally, for all other categories (categories where
        //   'collect' has *not* been called), verify their collectors still
        //   contain their original values (i.e., they have not been modified).
        //
        // Testing:
        //   void collect(v<MetricRecord> *, const Category *);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'collectAndReset'" << endl;

        Registry reg(Z); const Registry& REG = reg;
        const char *CATEGORIES[] = {"A", "B", "C"};
        const int   NUM_CATEGORIES = sizeof (CATEGORIES)/sizeof (*CATEGORIES);
        const char *METRICS[] = { "A", "B", "C" };
        const int NUM_METRICS = sizeof (METRICS) / sizeof (*METRICS);
        const int  NUM_COLS = 3;

        // Iterator over the category being tested.
        for (int i = 0; i < NUM_CATEGORIES; ++i) {
            Obj mX(&reg, Z); const Obj& MX = mX;

            // Create collectors for all 3 categories and update them with test
            // values.
            for (int j = 0; j < NUM_CATEGORIES; ++j) {
                const char *CATEGORY = CATEGORIES[j];
                for (int k = 0; k < NUM_METRICS; ++k) {
                    balm::MetricId metric = reg.getId(CATEGORY, METRICS[k]);
                    for (int l = 0; l < NUM_COLS; ++l) {
                        Col *col   = mX.addCollector(metric).get();
                        ICol *iCol = mX.addIntegerCollector(metric).get();
                        if (i == j) {
                            // We are creating collector values for the metric
                            // under test ('i == j').
                            col->setCountTotalMinMax(k,  2 * k, -k, k);
                            iCol->setCountTotalMinMax(k, 2 * k, -k, k);
                        }
                        else {
                            // This category will not be collected, add a
                            // value to verify it has not been updated.
                            col->setCountTotalMinMax(100, 100,  100,  100);
                            iCol->setCountTotalMinMax(100, 100,  100,  100);
                        }
                    }
                }
            }

            // Collect values from the category under test.
            {
                const char *CATEGORY = CATEGORIES[i];
                const balm::Category *category = reg.getCategory(CATEGORY);

                bsl::vector<balm::MetricRecord> records(Z);
                mX.collect(&records, category);

                ASSERT(NUM_METRICS == records.size());
                bsl::sort(records.begin(), records.end(), recordLess);

                // Verify the collected values match the expected collected
                // values.
                for (int k = 0; k < NUM_METRICS; ++k) {
                    balm::MetricId metric = reg.getId(CATEGORY, METRICS[k]);
                    const Rec& R = records[k];
                    ASSERT(metric == R.metricId());
                    ASSERT(2 * k * NUM_COLS     == R.count());
                    ASSERT(4 * k * NUM_COLS == R.total());
                    ASSERT(-k               == R.min());
                    ASSERT( k               == R.max());
                }

                // Verify the collectors for the test category have been reset.
                for (int j = 0; j < NUM_METRICS; ++j) {
                    balm::MetricId metric = reg.getId(CATEGORY,METRICS[j]);
                    ColSPtrVector cols;
                    IColSPtrVector iCols;
                    mX.getAddedCollectors(&cols, &iCols, metric);
                    for (int k = 0; k < cols.size(); ++k) {
                        balm::MetricRecord E(metric,  j, 2 * j, -j, j);
                        balm::MetricRecord r1, r2;

                        cols[k]->load(&r1); iCols[k]->load(&r2);
                        LOOP3_ASSERT(i, j, k, E == r1);
                        LOOP3_ASSERT(i, j, k, E == r2);
                    }
                }
            }

            // Finally verify the collectors for other categories have not been
            // modified.
            for (int j = 0; j < NUM_CATEGORIES; ++j) {
                if (i == j) {
                    continue;
                }
                const char *CATEGORY = CATEGORIES[j];

                for (int k = 0; k < NUM_METRICS; ++k) {
                    balm::MetricId metric = reg.getId(CATEGORY,METRICS[j]);
                    ColSPtrVector cols(Z);
                    IColSPtrVector iCols(Z);
                    mX.getAddedCollectors(&cols, &iCols, metric);
                    for (int l = 0; l < cols.size(); ++l) {
                        balm::MetricRecord r1, r2;
                        cols[k]->load(&r1); iCols[k]->load(&r2);
                        ASSERT(metric == r1.metricId());
                        ASSERT(metric == r2.metricId());
                        ASSERT(100    == r1.count());
                        ASSERT(100    == r2.count());
                        ASSERT(100    == r1.total());
                        ASSERT(100    == r2.total());
                        ASSERT(100    == r1.min());
                        ASSERT(100    == r2.min());
                        ASSERT(100    == r1.max());
                        ASSERT(100    == r2.max());

                    }
                }
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING AUXILIARY MANIPULATORS: 'getDefaultCollector',
        //                                 'getDefaultIntegerCollector'
        //
        // Concerns:
        //   That 'getDefaultCollector' (and 'getDefaultIntegerCollector')
        //   returns the address of a valid 'balm::Collector' object (or
        //   'balm::IntegerCollector').
        //
        //   That 'getDefaultCollector' (and 'getDefaultIntegerCollector')
        //   create new metric ids when required.
        //
        // Plan:
        //
        //   For a sequence of metric ids, create a new
        //   'balm::CollectorRepository' and use 'getDefaultCollector' and
        //   'getDefaultIntegerCollector' to create new collector objects.
        //   Then verify subsequent calls to 'addCollector' create new
        //   collector whereas 'getDefaultCollector' returns the original
        //   collector.
        //
        //   Then, for a sequence of metric ids, create a new metric registry
        //   and collector repository and invoke 'getDefaultCollector' and
        //   'getDefaultIntegerCollector' - verify the operations return valid
        //   collectors and add a new id to the metric registry.
        //
        // Testing:
        //   getDefaultCollector(const StringRef&, const StringRef&);
        //   getDefaultIntegerCollector(const StringRef&, const StringRef&);
        // --------------------------------------------------------------------

        Registry reg(Z); const Registry& REG = reg;
        Id METRIC_AA = reg.getId("A", "A");

        struct {
            const char *d_category;
            const char *d_name;
        } METRICS [] = {
            { "A", "A" },
            { "A", "A" },
            { "A", "A" },
            { "A", "A" },
            { "A", "A" },
            { "A", "B" },
            { "A", "B" },
            { "A", "B" },
            { "B", "A" },
            { "B", "A" },
            { "B", "C" },
            { "C", "A" },
        };
        const int NUM_METRICS = sizeof METRICS / sizeof *METRICS;

        if (verbose) cout
            << "\nTesting aixilary mainpulators 'getDefaultCollector'"
            << " and 'getDefaultIntegerCollector'" << endl;

        {
            if (veryVerbose) {
                cout << "\tTest 'getDefaultCollector' followed by"
                     << " 'addCollector'" << endl;
            }

            const int NUM_ADDITIONAL = 5;
            for (int i = 0; i < NUM_METRICS; ++i) {
                const char *NAME     =  METRICS[i].d_name;
                const char *CATEGORY =  METRICS[i].d_category;

                Id id = reg.getId(CATEGORY, NAME);
                Obj mX(&reg, Z); const Obj& MX = mX;
                Col *col   = mX.getDefaultCollector(CATEGORY, NAME);
                ICol *iCol = mX.getDefaultIntegerCollector(CATEGORY, NAME);

                ASSERT(id   == col->metricId());
                ASSERT(id   == iCol->metricId());
                ASSERT(col  == mX.getDefaultCollector(id));
                ASSERT(iCol == mX.getDefaultIntegerCollector(id));

                for (int j = 0; j < NUM_ADDITIONAL; ++j) {
                    ASSERT(col  != mX.addCollector(CATEGORY, NAME).get());
                    ASSERT(iCol != mX.addIntegerCollector(id).get());
                    ASSERT(col  == mX.getDefaultCollector(id));
                    ASSERT(iCol == mX.getDefaultIntegerCollector(id));
                }

                ColSPtrVector colV(Z);
                IColSPtrVector iColV(Z);
                int num = mX.getAddedCollectors(&colV, &iColV, id);

                ASSERT(NUM_ADDITIONAL == colV.size());
                ASSERT(NUM_ADDITIONAL == iColV.size());

                for (int i = 0; i < NUM_ADDITIONAL; ++i) {
                    ASSERT(col  != colV[i].get());
                    ASSERT(iCol != iColV[i].get());
                }
            }
        }
        {
            if (veryVerbose) {
                cout << "\tTest 'getDefaultCollector' creates new metric ids "
                     << "when necessary." << endl;
            }
            for (int i = 0; i < NUM_METRICS; ++i) {
                const char *NAME     =  METRICS[i].d_name;
                const char *CATEGORY =  METRICS[i].d_category;

                {
                    Registry reg(Z); const Registry& REG = reg;
                    Obj mX(&reg, Z); const Obj& MX = mX;

                    ASSERT(!REG.findId(CATEGORY, NAME).isValid());
                    Col *col = mX.getDefaultCollector(CATEGORY, NAME);

                    ASSERT( REG.findId(CATEGORY, NAME).isValid());
                    ASSERT( REG.findId(CATEGORY, NAME) == col->metricId());
                }
                {
                    Registry reg(Z); const Registry& REG = reg;
                    Obj mX(&reg, Z); const Obj& MX = mX;

                    ASSERT(!REG.findId(CATEGORY, NAME).isValid());
                    ICol *iCol = mX.getDefaultIntegerCollector(CATEGORY, NAME);

                    ASSERT( REG.findId(CATEGORY, NAME).isValid());
                    ASSERT( REG.findId(CATEGORY, NAME) == iCol->metricId());
                }
            }
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING AUXILIARY MANIPULATORS: 'addCollector','addIntegerCollector'
        //
        // Concerns:
        //   That 'addCollector' (and 'addIntegerCollector') returns the
        //   address of a new valid 'balm::Collector' object (or
        //   'balm::IntegerCollector').
        //
        //   That 'addCollector' (and 'addIntegerCollector') create new metric
        //   ids when required.
        //
        // Plan:
        //
        //   For a sequence of metric ids, including duplicate ids,
        //   invoke 'addCollector' and 'addIntegerCollector' putting the
        //   pointers into an "oracle" container owned by the test.  Then
        //   verify the pointers in the "oracle" are unique, and that their
        //   values equal the response from 'getAddedCollectors'.
        //
        //   Then, for a sequence of metric ids, create a new metric registry
        //   and collector repository and invoked 'addCollector' and
        //   'addIntegerCollector' - verify the operations return valid
        //   collectors and add a new id to the metric registry.
        //
        // Testing:
        //   addCollector(const StringRef&, const StringRef&);
        //   addIntegerCollector(const StringRef&, const StringRef&);
        // --------------------------------------------------------------------

        Registry reg(Z); const Registry& REG = reg;
        Id METRIC_AA = reg.getId("A", "A");

        struct {
            const char *d_category;
            const char *d_name;
        } METRICS [] = {
            { "A", "A" },
            { "A", "A" },
            { "A", "A" },
            { "A", "A" },
            { "A", "A" },
            { "A", "B" },
            { "A", "B" },
            { "A", "B" },
            { "B", "A" },
            { "B", "A" },
            { "B", "C" },
            { "C", "A" },
        };
        const int NUM_METRICS = sizeof METRICS / sizeof *METRICS;

        if (verbose) cout << "\nTesting aixilary mainpulators 'addCollector'"
                          << " and 'addIntegerCollector'" << endl;

        {
            if (veryVerbose) {
                cout << "\tTest 'addCollector' creates valid new collectors."
                     << endl;
            }

            // Add collectors.
            Obj mX(&reg, Z); const Obj& MX = mX;
            bsl::set<Id> ids(Z);
            bsl::map<Id, bsl::vector<Col *> >  cols(Z);
            bsl::map<Id, bsl::vector<ICol *> > icols(Z);
            for (int i = 0; i < NUM_METRICS; ++i) {
                const char *NAME     =  METRICS[i].d_name;
                const char *CATEGORY =  METRICS[i].d_category;

                Id id = reg.getId(CATEGORY, NAME);
                ids.insert(id);

                Col *col   = mX.addCollector(CATEGORY, NAME).get();
                ICol *iCol = mX.addIntegerCollector(CATEGORY, NAME).get();

                ASSERT(0  != col);
                ASSERT(0  != iCol);
                ASSERT(id == col->metricId());
                ASSERT(id == iCol->metricId());

                cols[id].push_back(col);
                icols[id].push_back(iCol);
            }
            ASSERT(0 == defaultAllocator.numBytesInUse());

            // Verify the values added are unique and can be found using
            // 'getAddedCollectors'.
            bsl::set<Id>::const_iterator it = ids.begin();
            for (; it != ids.end(); ++it) {

                bsl::vector<Col *>&  exp_ColV  = cols[*it];
                bsl::vector<ICol *>& exp_iColV = icols[*it];

                {
                    // Verify the collectors are unique.
                    bsl::set<Col *>  colSet(exp_ColV.begin(),
                                            exp_ColV.end());
                    bsl::set<ICol *> iColSet(exp_iColV.begin(),
                                             exp_iColV.end());
                    ASSERT(colSet.size()  == exp_ColV.size());
                    ASSERT(iColSet.size() == exp_iColV.size());
                }

                ColSPtrVector colV(Z);
                IColSPtrVector iColV(Z);
                mX.getAddedCollectors(&colV, &iColV, *it);

                ASSERT(vectorEquals(exp_ColV, colV));
                ASSERT(vectorEquals(exp_iColV, iColV));
            }
            ASSERT(0 == defaultAllocator.numBytesInUse());
        }
        {
            if (veryVerbose) {
                cout << "\tTest 'addCollector' creates new metric ids when "
                     << "necessary." << endl;
            }
            for (int i = 0; i < NUM_METRICS; ++i) {
                const char *NAME     =  METRICS[i].d_name;
                const char *CATEGORY =  METRICS[i].d_category;

                {
                    Registry reg(Z); const Registry& REG = reg;
                    Obj mX(&reg, Z); const Obj& MX = mX;

                    ASSERT(!REG.findId(CATEGORY, NAME).isValid());
                    Col *col = mX.addCollector(CATEGORY, NAME).get();

                    ASSERT( REG.findId(CATEGORY, NAME).isValid());
                    ASSERT( REG.findId(CATEGORY, NAME) == col->metricId());
                }
                {
                    Registry reg(Z); const Registry& REG = reg;
                    Obj mX(&reg, Z); const Obj& MX = mX;

                    ASSERT(!REG.findId(CATEGORY, NAME).isValid());
                    ICol *iCol = mX.addIntegerCollector(CATEGORY, NAME).get();

                    ASSERT( REG.findId(CATEGORY, NAME).isValid());
                    ASSERT( REG.findId(CATEGORY, NAME) == iCol->metricId());
                }
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BASIC MANIPULATOR: 'collectAndReset'
        //
        // Concerns:
        //   That 'collectAndReset' properly collects values of all the
        //   collectors in the supplied category with no side effects.  That
        //   the collected records for a single metric are aggregated
        //   correctly (across multiple instances of 'balm::Collector' and
        //   'balm::IntegerCollector' for the same metric).
        //
        // Plan:
        //   For an independent sequences of metric ids and categories, select
        //   the category under test, then create multiple collectors (and
        //   integer collectors) for each metric id/category combination, and
        //   update the collectors, using identifiable values.  Then for the
        //   category under test, collect records using the 'collectAndReset'
        //   method and verify the records match the expected aggregates of the
        //   identifiable values, and verify the corresponding collectors for
        //   those metrics have been reset to their default value.  Finally,
        //   for all other categories (categories where 'collectAndReset' has
        //   *not* been called), verify their collectors still contain their
        //   original values (i.e., they have not been modified).
        //
        // Testing:
        //   void collectAndReset(v<MetricRecord> *, const Category *);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'collectAndReset'" << endl;

        Registry reg(Z); const Registry& REG = reg;
        const char *CATEGORIES[] = {"A", "B", "C"};
        const int   NUM_CATEGORIES = sizeof (CATEGORIES)/sizeof (*CATEGORIES);
        const char *METRICS[] = { "A", "B", "C" };
        const int NUM_METRICS = sizeof (METRICS) / sizeof (*METRICS);
        const int  NUM_COLS = 3;

        // Iterator over the category being tested.
        for (int i = 0; i < NUM_CATEGORIES; ++i) {
            Obj mX(&reg, Z); const Obj& MX = mX;

            // Create collectors for all 3 categories and update them with test
            // values.
            for (int j = 0; j < NUM_CATEGORIES; ++j) {
                const char *CATEGORY = CATEGORIES[j];
                for (int k = 0; k < NUM_METRICS; ++k) {
                    balm::MetricId metric = reg.getId(CATEGORY, METRICS[k]);
                    for (int l = 0; l < NUM_COLS; ++l) {
                        Col *col   = mX.addCollector(metric).get();
                        ICol *iCol = mX.addIntegerCollector(metric).get();
                        if (i == j) {
                            const int  M = (k % 2 == 0) ? 0 : 10 * (k + 1);
                            const int IM = (k % 2 == 1) ? 0 : 10 * (k + 1);
                            col->setCountTotalMinMax(k+1, 2*(k+1), -M, M);
                            iCol->setCountTotalMinMax(k+1, 2*(k+1), -IM, IM);
                        }
                        else {
                            col->setCountTotalMinMax(100, 100,  100,  100);
                            iCol->setCountTotalMinMax(100, 100,  100,  100);
                        }
                    }
                }
            }

            // Collect values from the category under test.
            {
                const char *CATEGORY = CATEGORIES[i];
                const balm::Category *category = reg.getCategory(CATEGORY);

                bsl::vector<balm::MetricRecord> records(Z);
                mX.collectAndReset(&records, category);

                ASSERT(NUM_METRICS == records.size());
                bsl::sort(records.begin(), records.end(), recordLess);

                // Verify the collected values match the expected collected
                // values.
                for (int k = 0; k < NUM_METRICS; ++k) {
                    balm::MetricId metric = reg.getId(CATEGORY, METRICS[k]);
                    const Rec& R = records[k];
                    ASSERT(metric == R.metricId());
                    ASSERT(2 * (k + 1) * NUM_COLS   == R.count());
                    ASSERT(4 * (k + 1) * NUM_COLS   == R.total());
                    ASSERT(-10 * (k + 1)            == R.min());
                    ASSERT( 10 * (k + 1)            == R.max());
                }

                // Verify the collectors for the test category have been reset.
                for (int j = 0; j < NUM_METRICS; ++j) {
                    balm::MetricId metric = reg.getId(CATEGORY,METRICS[j]);
                    ColSPtrVector cols;
                    IColSPtrVector iCols;
                    mX.getAddedCollectors(&cols, &iCols, metric);
                    for (int k = 0; k < cols.size(); ++k) {
                        balm::MetricRecord r1, r2;
                        cols[k]->load(&r1); iCols[k]->load(&r2);
                        ASSERT(balm::MetricRecord(metric) == r1);
                        ASSERT(balm::MetricRecord(metric) == r2);
                    }
                }
            }

            // Finally verify the collectors for other categories have not been
            // modified.
            for (int j = 0; j < NUM_CATEGORIES; ++j) {
                if (i == j) {
                    continue;
                }
                const char *CATEGORY = CATEGORIES[j];

                for (int k = 0; k < NUM_METRICS; ++k) {
                    balm::MetricId metric = reg.getId(CATEGORY,METRICS[j]);
                    ColSPtrVector cols(Z);
                    IColSPtrVector iCols(Z);
                    mX.getAddedCollectors(&cols, &iCols, metric);
                    for (int l = 0; l < cols.size(); ++l) {
                        balm::MetricRecord r1, r2;
                        cols[k]->load(&r1); iCols[k]->load(&r2);
                        ASSERT(metric == r1.metricId());
                        ASSERT(metric == r2.metricId());
                        ASSERT(100    == r1.count());
                        ASSERT(100    == r2.count());
                        ASSERT(100    == r1.total());
                        ASSERT(100    == r2.total());
                        ASSERT(100    == r1.min());
                        ASSERT(100    == r2.min());
                        ASSERT(100    == r1.max());
                        ASSERT(100    == r2.max());

                    }
                }
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING BASIC MANIPULATOR: 'getDefaultCollector',
        //                            'getDefaultIntegerCollector'
        //
        // Concerns:
        //   That 'getDefaultCollector' (and 'getDefaultIntegerCollector')
        //   returns the valid address of the default balm::Collector' (or
        //   'balm::IntegerCollector') object.
        //
        //   That 'getDefaultCollector'  (and 'getDefaultIntegerCollector') if
        //   called prior to 'addCollector' (or 'addIntegerCollector')
        //   properly creates a new collector and returns its address.
        //
        // Plan:
        //
        //   Next, for a sequence of metric ids, create a new
        //   'balm::CollectorRepository' and use 'getDefaultCollector' and
        //   'getDefaultIntegerCollector' to create new collector objects.
        //   Then verify subsequent calls to 'addCollector' create new
        //   collector whereas 'getDefaultCollector' returns the original
        //   collector.
        //
        // Testing:
        //   getDefaultCollector(const MetricId&);
        //   IntegerCollector *getDefaultIntegerCollector(const MetricId&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'getDefaultCollector' and "
                          << "'getDefaultIntegerCollector'" << endl;

        Registry reg(Z); const Registry& REG = reg;
        Id METRIC_AA = reg.getId("A", "A");

        struct {
            const char *d_category;
            const char *d_name;
        } METRICS [] = {
            { "A", "A" },
            { "A", "A" },
            { "A", "A" },
            { "A", "A" },
            { "A", "A" },
            { "A", "B" },
            { "A", "B" },
            { "A", "B" },
            { "B", "A" },
            { "B", "A" },
            { "B", "C" },
            { "C", "A" },
        };
        const int NUM_METRICS = sizeof METRICS / sizeof *METRICS;
        {
            if (veryVerbose) {
                cout << "\tTest 'addCollector' followed by "
                     << "'getDefaultCollector'" << endl;
            }

            // Add collectors.
            Obj mX(&reg, Z); const Obj& MX = mX;
            bsl::set<Id> ids(Z);
            bsl::map<Id, Col *>  defCols(Z);
            bsl::map<Id, ICol *> defICols(Z);
            bsl::map<Id, bsl::vector<Col *> >  cols(Z);
            bsl::map<Id, bsl::vector<ICol *> > iCols(Z);
            for (int i = 0; i < NUM_METRICS; ++i) {
                Id id = reg.getId(METRICS[i].d_category, METRICS[i].d_name);

                ids.insert(id);

                cols[id].push_back(mX.addCollector(id).get());
                iCols[id].push_back(mX.addIntegerCollector(id).get());

                Col  *col = mX.getDefaultCollector(id);
                ICol *iCol = mX.getDefaultIntegerCollector(id);

                ASSERT(defCols[id] == 0  || defCols[id] == col);
                ASSERT(defICols[id] == 0 || defICols[id] == iCol);

                defCols[id] = col; defICols[id] = iCol;

                bsl::set<Col *>  colSet(Z);
                bsl::set<ICol *> iColSet(Z);
                colSet.insert(cols[id].begin(), cols[id].end());
                iColSet.insert(iCols[id].begin(), iCols[id].end());

                ASSERT(0 == colSet.count(col));
                ASSERT(0 == iColSet.count(iCol));
            }
            ASSERT(0 == defaultAllocator.numBytesInUse());

            // Verify the values added are unique and can be found using
            // 'getAddedCollectors'.
            bsl::set<Id>::const_iterator it = ids.begin();
            for (; it != ids.end(); ++it) {
                bsl::vector<Col *>&  exp_ColV  = cols[*it];
                bsl::vector<ICol *>& exp_iColV = iCols[*it];

                ColSPtrVector colV(Z);
                IColSPtrVector iColV(Z);
                mX.getAddedCollectors(&colV, &iColV, *it);

                ASSERT(vectorEquals(exp_ColV, colV));
                ASSERT(vectorEquals(exp_iColV, iColV));
            }
            ASSERT(0 == defaultAllocator.numBytesInUse());
        }
        {
            if (veryVerbose) {
                cout << "\tTest 'getDefaultCollector' followed by "
                     << "'addCollector'" << endl;
            }

            const int NUM_ADDITIONAL = 5;
            for (int i = 0; i < NUM_METRICS; ++i) {
                Id id = reg.getId(METRICS[i].d_category, METRICS[i].d_name);

                Obj mX(&reg, Z); const Obj& MX = mX;
                Col *col   = mX.getDefaultCollector(id);
                ICol *iCol = mX.getDefaultIntegerCollector(id);

                ASSERT(id   == col->metricId());
                ASSERT(id   == iCol->metricId());
                ASSERT(col  == mX.getDefaultCollector(id));
                ASSERT(iCol == mX.getDefaultIntegerCollector(id));

                for (int j = 0; j < NUM_ADDITIONAL; ++j) {
                    ASSERT(col  != mX.addCollector(id).get());
                    ASSERT(iCol != mX.addIntegerCollector(id).get());
                    ASSERT(col  == mX.getDefaultCollector(id));
                    ASSERT(iCol == mX.getDefaultIntegerCollector(id));
                }

                ColSPtrVector colV(Z);
                IColSPtrVector iColV(Z);
                int num = mX.getAddedCollectors(&colV, &iColV, id);

                ASSERT(NUM_ADDITIONAL == colV.size());
                ASSERT(NUM_ADDITIONAL == iColV.size());
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING BASIC MANIPULATORS and ACCESSORS (BOOTSTRAP):
        //
        // Concerns:
        //   That 'addCollector' (and 'addIntegerCollector') returns the
        //   address of a new valid 'balm::Collector' object (or
        //   'balm::IntegerCollector').
        //
        //   That 'findCollectors' returns, in order of creation, the
        //   addresses of those collectors created by 'addCollector' and
        //   'addIntegerCollector'.
        //
        //   That 'findCollectors' appends to the supplied vectors and uses,
        //   at most, 1 memory allocation per output vector.
        //
        // Plan:
        //   First, verify the default constructor by testing the value of the
        //   resulting object.
        //
        //   Then, for a sequence of metric ids, including duplicate ids,
        //   invoke 'addCollector' and 'addIntegerCollector' putting the
        //   pointers into an "oracle" container owned by the test.  Then
        //   verify the pointers in the "oracle" are unique, and that their
        //   values equal the response from 'getAddedCollectors'.
        //
        //   Finally, call 'addCollector' and 'addIntegerCollector'
        //   repeatedly for a single metric id.  Populate a vector with an
        //   initial value.  Invoke 'getAddedCollectors' and verify that the
        //   results are appended to the initial value and that the method
        //   does no perform more than 2 memory allocations.
        //
        // Testing:
        //   addCollector(const MetricId& metricId);
        //   addIntegerCollector(const MetricId&);
        //   ~CollectorRepository();
        //   CollectorRepository(MetricRegistry *, bslma::Allocator *);
        //   const MetricRegistry& registry() const;
        //   int getAddedCollectors(v<C *> *, v<IC *> *, const MetricId&);
        //   int getAddedCollectors(v<C*> *, v<IC*> *, MetricId& ) const;
        //   MetricRegistry &registry();
        // --------------------------------------------------------------------

        Registry reg(Z); const Registry& REG = reg;
        Id METRIC_AA = reg.getId("A", "A");

        struct {
            const char *d_category;
            const char *d_name;
        } METRICS [] = {
            { "A", "A" },
            { "A", "A" },
            { "A", "A" },
            { "A", "A" },
            { "A", "A" },
            { "A", "B" },
            { "A", "B" },
            { "A", "B" },
            { "B", "A" },
            { "B", "A" },
            { "B", "C" },
            { "C", "A" },
        };
        const int NUM_METRICS = sizeof METRICS / sizeof *METRICS;

        if (verbose) cout << "\nTesting basic mainpulator and accessors."
                          << endl;

        {
            if (veryVerbose) {
                cout << "\tTest default constructor." << endl;
            }

            Obj mX(&reg, Z); const Obj& MX = mX;
            ASSERT(&REG == &mX.registry());
            ASSERT(&REG == &MX.registry());

            ColSPtrVector colV(Z);
            IColSPtrVector iColV(Z);
            int num = mX.getAddedCollectors(&colV, &iColV, METRIC_AA);
            ASSERT(0 == num);
            ASSERT(0 == colV.size());
            ASSERT(0 == iColV.size());
        }
        {
            if (veryVerbose) {
                cout << "\tTest 'addCollector', 'addIntegerCollector', "
                     << "'getAddedCollectors'. " << endl;
            }

            // Add collectors.
            Obj mX(&reg, Z); const Obj& MX = mX;
            bsl::set<Id> ids(Z);
            bsl::map<Id, bsl::vector<Col *> >  cols(Z);
            bsl::map<Id, bsl::vector<ICol *> > icols(Z);
            for (int i = 0; i < NUM_METRICS; ++i) {
                Id id = reg.getId(METRICS[i].d_category, METRICS[i].d_name);

                Col *collector  = mX.addCollector(id).get();
                ICol *iCollector = mX.addIntegerCollector(id).get();

                ASSERT(0  != collector);
                ASSERT(0  != iCollector);
                ASSERT(id == collector->metricId());
                ASSERT(id == iCollector->metricId());

                ids.insert(id);
                cols[id].push_back(collector);
                icols[id].push_back(iCollector);
            }
            ASSERT(0 == defaultAllocator.numBytesInUse());

            // Verify the values added are unique and can be found using
            // 'getAddedCollectors'.
            bsl::set<Id>::const_iterator it = ids.begin();
            for (; it != ids.end(); ++it) {

                bsl::vector<Col *>&  exp_ColV  = cols[*it];
                bsl::vector<ICol *>& exp_iColV = icols[*it];

                {
                    // Verify the collectors are unique.
                    bsl::set<Col *>  colSet(exp_ColV.begin(),
                                            exp_ColV.end());
                    bsl::set<ICol *> iColSet(exp_iColV.begin(),
                                             exp_iColV.end());
                    ASSERT(colSet.size()  == exp_ColV.size());
                    ASSERT(iColSet.size() == exp_iColV.size());
                }

                ColSPtrVector colV(Z);
                IColSPtrVector iColV(Z);
                const int NUM   = mX.getAddedCollectors(&colV, &iColV, *it);

                ASSERT(exp_ColV.size() + exp_iColV.size() == NUM);
                ASSERT(vectorEquals(exp_ColV, colV));
                ASSERT(vectorEquals(exp_iColV, iColV));
            }
            ASSERT(0 == defaultAllocator.numBytesInUse());
        }
        {
            if (veryVerbose) {
                cout << "\tVerify 'getCollector' appends to the supplied "
                     << "array and does not allocate more than once." << endl;
            }

            ColSPtrVector initialColV(Z);
            IColSPtrVector initialIColV(Z);

            ColSPtr dummyCol((Col *)0xDEAD, bslstl::SharedPtrNilDeleter(), 0);
            IColSPtr dummyICol((ICol *)0xDEAD,
                               bslstl::SharedPtrNilDeleter(),
                               0);
            initialColV.push_back(dummyCol);
            initialColV.push_back(dummyCol);
            initialIColV.push_back(dummyICol);
            initialIColV.push_back(dummyICol);

            const int NUM_METRICS = 20;
            Obj mX(&reg, Z); const Obj& MX = mX;
            for (int i = 0; i < NUM_METRICS; ++i) {
                mX.addCollector(METRIC_AA).get();
                mX.addIntegerCollector(METRIC_AA).get();

                ColSPtrVector  colV(initialColV, Z);
                IColSPtrVector iColV(initialIColV, Z);

                // Verify the number of allocations.
                int numAllocations = allocator.numAllocations();
                const int NUM = mX.getAddedCollectors(&colV,
                                                      &iColV,
                                                      METRIC_AA);
                ASSERT((numAllocations + 2) >= allocator.numAllocations());

                ASSERT(NUM == (i + 1) * 2);
                if (veryVeryVerbose) {
                    P_(i); P_(numAllocations+2); P(allocator.numAllocations());
                    P_(colV.size()); P(iColV.size());
                }
                ASSERT(initialColV.size()  + i + 1 == colV.size());
                ASSERT(initialIColV.size() + i + 1 == iColV.size());
                for (int j = 0; j < initialColV.size(); ++j) {
                    ASSERT(initialColV[j]  == colV[j]);
                    ASSERT(initialIColV[j] == iColV[j]);
                }
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Developers' Sandbox.
        //
        // Plan:
        //   Perform and ad-hoc test of the primary modifiers and accessors.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        Registry reg(Z); const Registry& REG = reg;

        Id METRIC_AA = reg.getId("A", "A");
        Id METRIC_AB = reg.getId("A", "B");
        Id METRIC_BA = reg.getId("B", "A");
        Id METRIC_BB = reg.getId("B", "B");
        {
            Obj mX(&reg, Z); const Obj& MX = mX;
            ASSERT(&REG == &mX.registry());
            ASSERT(&REG == &MX.registry());

            if (veryVerbose) {
                cout << "\tTest getDefaultCollector(const balm::MetricId& )"
                     << bsl::endl;
            }

            Col *COL_AA = mX.getDefaultCollector(METRIC_AA);
            Col *COL_AB = mX.getDefaultCollector(METRIC_AB);
            Col *COL_BA = mX.getDefaultCollector(METRIC_BA);
            Col *COL_BB = mX.getDefaultCollector(METRIC_BB);

            ASSERT(0 != COL_AA);
            ASSERT(0 != COL_AB);
            ASSERT(0 != COL_BA);
            ASSERT(0 != COL_BB);

            ASSERT(METRIC_AA == COL_AA->metricId());
            ASSERT(METRIC_AB == COL_AB->metricId());
            ASSERT(METRIC_BA == COL_BA->metricId());
            ASSERT(METRIC_BB == COL_BB->metricId());

            ASSERT(COL_AA == mX.getDefaultCollector(METRIC_AA));
            ASSERT(COL_AB == mX.getDefaultCollector(METRIC_AB));
            ASSERT(COL_BA == mX.getDefaultCollector(METRIC_BA));
            ASSERT(COL_BB == mX.getDefaultCollector(METRIC_BB));

            ASSERT(COL_AA == mX.getDefaultCollector("A", "A"));
            ASSERT(COL_AB == mX.getDefaultCollector("A", "B"));
            ASSERT(COL_BA == mX.getDefaultCollector("B", "A"));
            ASSERT(COL_BB == mX.getDefaultCollector("B", "B"));

            if (veryVerbose) {
                cout << "\tTest addCollector(const balm::MetricId& )"
                     << bsl::endl;
            }
            Col *COL_AA_2 = mX.addCollector(METRIC_AA).get();
            Col *COL_AB_2 = mX.addCollector(METRIC_AB).get();
            Col *COL_BA_2 = mX.addCollector(METRIC_BA).get();
            Col *COL_BB_2 = mX.addCollector(METRIC_BB).get();

            ASSERT(0 != COL_AA_2);
            ASSERT(0 != COL_AB_2);
            ASSERT(0 != COL_BA_2);
            ASSERT(0 != COL_BB_2);

            ASSERT(COL_AA_2 != COL_AA);
            ASSERT(COL_AB_2 != COL_AB);
            ASSERT(COL_BA_2 != COL_BA);
            ASSERT(COL_BB_2 != COL_BB);

            ASSERT(METRIC_AA == COL_AA_2->metricId());
            ASSERT(METRIC_AB == COL_AB_2->metricId());
            ASSERT(METRIC_BA == COL_BA_2->metricId());
            ASSERT(METRIC_BB == COL_BB_2->metricId());

            ASSERT(COL_AA == mX.getDefaultCollector(METRIC_AA));
            ASSERT(COL_AB == mX.getDefaultCollector(METRIC_AB));
            ASSERT(COL_BA == mX.getDefaultCollector(METRIC_BA));
            ASSERT(COL_BB == mX.getDefaultCollector(METRIC_BB));

            ASSERT(COL_AA == mX.getDefaultCollector("A", "A"));
            ASSERT(COL_AB == mX.getDefaultCollector("A", "B"));
            ASSERT(COL_BA == mX.getDefaultCollector("B", "A"));
            ASSERT(COL_BB == mX.getDefaultCollector("B", "B"));

        }
        {
            Obj mX(&reg, Z); const Obj& MX = mX;
            if (veryVerbose) {
                cout << "\tTest getDefaultCollector(const bslstl::StringRef &,"
                     << " const bslstl::StringRef& )" << endl;
            }

            Col *COL_AA = mX.getDefaultCollector("A", "A");
            Col *COL_AB = mX.getDefaultCollector("A", "B");
            Col *COL_BA = mX.getDefaultCollector("B", "A");
            Col *COL_BB = mX.getDefaultCollector("B", "B");

            ASSERT(0 != COL_AA);
            ASSERT(0 != COL_AB);
            ASSERT(0 != COL_BA);
            ASSERT(0 != COL_BB);

            ASSERT(METRIC_AA == COL_AA->metricId());
            ASSERT(METRIC_AB == COL_AB->metricId());
            ASSERT(METRIC_BA == COL_BA->metricId());
            ASSERT(METRIC_BB == COL_BB->metricId());

            ASSERT(COL_AA == mX.getDefaultCollector(METRIC_AA));
            ASSERT(COL_AB == mX.getDefaultCollector(METRIC_AB));
            ASSERT(COL_BA == mX.getDefaultCollector(METRIC_BA));
            ASSERT(COL_BB == mX.getDefaultCollector(METRIC_BB));

            ASSERT(COL_AA == mX.getDefaultCollector("A", "A"));
            ASSERT(COL_AB == mX.getDefaultCollector("A", "B"));
            ASSERT(COL_BA == mX.getDefaultCollector("B", "A"));
            ASSERT(COL_BB == mX.getDefaultCollector("B", "B"));
        }
        {
            Obj mX(&reg, Z); const Obj& MX = mX;
            ASSERT(&REG == &mX.registry());
            ASSERT(&REG == &MX.registry());

            if (veryVerbose) {
                cout << "\tTest getDefaultIntegerCollector(const "
                     << " balm::MetricId& )" << bsl::endl;
            }

            ICol *COL_AA = mX.getDefaultIntegerCollector(METRIC_AA);
            ICol *COL_AB = mX.getDefaultIntegerCollector(METRIC_AB);
            ICol *COL_BA = mX.getDefaultIntegerCollector(METRIC_BA);
            ICol *COL_BB = mX.getDefaultIntegerCollector(METRIC_BB);

            ASSERT(0 != COL_AA);
            ASSERT(0 != COL_AB);
            ASSERT(0 != COL_BA);
            ASSERT(0 != COL_BB);

            ASSERT(METRIC_AA == COL_AA->metricId());
            ASSERT(METRIC_AB == COL_AB->metricId());
            ASSERT(METRIC_BA == COL_BA->metricId());
            ASSERT(METRIC_BB == COL_BB->metricId());

            ASSERT(COL_AA == mX.getDefaultIntegerCollector(METRIC_AA));
            ASSERT(COL_AB == mX.getDefaultIntegerCollector(METRIC_AB));
            ASSERT(COL_BA == mX.getDefaultIntegerCollector(METRIC_BA));
            ASSERT(COL_BB == mX.getDefaultIntegerCollector(METRIC_BB));

            ASSERT(COL_AA == mX.getDefaultIntegerCollector("A", "A"));
            ASSERT(COL_AB == mX.getDefaultIntegerCollector("A", "B"));
            ASSERT(COL_BA == mX.getDefaultIntegerCollector("B", "A"));
            ASSERT(COL_BB == mX.getDefaultIntegerCollector("B", "B"));

            if (veryVerbose) {
                cout << "\tTest addIntegerCollector(const balm::MetricId& )"
                     << bsl::endl;
            }
            ICol *COL_AA_2 = mX.addIntegerCollector(METRIC_AA).get();
            ICol *COL_AB_2 = mX.addIntegerCollector(METRIC_AB).get();
            ICol *COL_BA_2 = mX.addIntegerCollector(METRIC_BA).get();
            ICol *COL_BB_2 = mX.addIntegerCollector(METRIC_BB).get();

            ASSERT(0 != COL_AA_2);
            ASSERT(0 != COL_AB_2);
            ASSERT(0 != COL_BA_2);
            ASSERT(0 != COL_BB_2);

            ASSERT(METRIC_AA == COL_AA_2->metricId());
            ASSERT(METRIC_AB == COL_AB_2->metricId());
            ASSERT(METRIC_BA == COL_BA_2->metricId());
            ASSERT(METRIC_BB == COL_BB_2->metricId());

            ASSERT(COL_AA == mX.getDefaultIntegerCollector(METRIC_AA));
            ASSERT(COL_AB == mX.getDefaultIntegerCollector(METRIC_AB));
            ASSERT(COL_BA == mX.getDefaultIntegerCollector(METRIC_BA));
            ASSERT(COL_BB == mX.getDefaultIntegerCollector(METRIC_BB));

            ASSERT(COL_AA == mX.getDefaultIntegerCollector("A", "A"));
            ASSERT(COL_AB == mX.getDefaultIntegerCollector("A", "B"));
            ASSERT(COL_BA == mX.getDefaultIntegerCollector("B", "A"));
            ASSERT(COL_BB == mX.getDefaultIntegerCollector("B", "B"));

        }
        {
            Obj mX(&reg, Z); const Obj& MX = mX;
            if (veryVerbose) {
                cout << "\tTest getDefaultIntegerCollector(const "
                     << "bslstl::StringRef &, const bslstl::StringRef& )"
                     << bsl::endl;
            }

            ICol *COL_AA = mX.getDefaultIntegerCollector("A", "A");
            ICol *COL_AB = mX.getDefaultIntegerCollector("A", "B");
            ICol *COL_BA = mX.getDefaultIntegerCollector("B", "A");
            ICol *COL_BB = mX.getDefaultIntegerCollector("B", "B");

            ASSERT(0 != COL_AA);
            ASSERT(0 != COL_AB);
            ASSERT(0 != COL_BA);
            ASSERT(0 != COL_BB);

            ASSERT(METRIC_AA == COL_AA->metricId());
            ASSERT(METRIC_AB == COL_AB->metricId());
            ASSERT(METRIC_BA == COL_BA->metricId());
            ASSERT(METRIC_BB == COL_BB->metricId());

            ASSERT(COL_AA == mX.getDefaultIntegerCollector(METRIC_AA));
            ASSERT(COL_AB == mX.getDefaultIntegerCollector(METRIC_AB));
            ASSERT(COL_BA == mX.getDefaultIntegerCollector(METRIC_BA));
            ASSERT(COL_BB == mX.getDefaultIntegerCollector(METRIC_BB));

            ASSERT(COL_AA == mX.getDefaultIntegerCollector("A", "A"));
            ASSERT(COL_AB == mX.getDefaultIntegerCollector("A", "B"));
            ASSERT(COL_BA == mX.getDefaultIntegerCollector("B", "A"));
            ASSERT(COL_BB == mX.getDefaultIntegerCollector("B", "B"));
        }
        {
            Obj mX(&reg, Z); const Obj& MX = mX;

            if (veryVerbose) {
                cout << "\tTest 'getAddedCollectors'" << bsl::endl;
            }
            Col *col1 = mX.addCollector(METRIC_AA).get();
            Col *col2 = mX.addCollector(METRIC_AA).get();
            Col *col3 = mX.addCollector(METRIC_AA).get();

            mX.addCollector(METRIC_AB).get();

            ICol *icol1 = mX.addIntegerCollector(METRIC_AA).get();
            ICol *icol2 = mX.addIntegerCollector(METRIC_AA).get();
            ICol *icol3 = mX.addIntegerCollector(METRIC_AA).get();

            mX.addIntegerCollector(METRIC_AB).get();

            Col  *expCol[]  = { col1, col2, col3 };
            ICol *expICol[] = { icol1, icol2, icol3 };
            const int NUM_COL = sizeof expCol / sizeof *expCol;

            bsl::sort(expCol,  expCol  + NUM_COL);
            bsl::sort(expICol, expICol + NUM_COL);

            ColSPtrVector collectors(Z);
            IColSPtrVector intCollectors(Z);
            int num = mX.getAddedCollectors(&collectors,
                                            &intCollectors,
                                            METRIC_AA);

            ASSERT(6 == num);
            ASSERT(3 == collectors.size());
            ASSERT(3 == intCollectors.size());

            for (int i = 0; i < 3; ++i) {
                LOOP3_ASSERT(i, expCol[i], collectors[i].get(),
                                           expCol[i]  == collectors[i].get());
                LOOP3_ASSERT(i, expICol[i], intCollectors[i].get(),
                                        expICol[i] == intCollectors[i].get());
            }
        }
        {

            Obj mX(&reg, Z); const Obj& MX = mX;

            if (veryVerbose) {
                cout << "\tTest 'collectAndReset'" << bsl::endl;
            }

            bsl::vector<Col *> collectors(Z);
            bsl::vector<ICol *> iCollectors(Z);

            Col *col1 = mX.addCollector(METRIC_AA).get();
            Col *col2 = mX.addCollector(METRIC_AA).get();
            Col *col3 = mX.addCollector(METRIC_AB).get();
            Col *col4 = mX.addCollector(METRIC_AB).get();
            Col *ignored1 = mX.addCollector(METRIC_BA).get();

            ICol *icol1 = mX.addIntegerCollector(METRIC_AA).get();
            ICol *icol2 = mX.addIntegerCollector(METRIC_AA).get();
            ICol *icol3 = mX.addIntegerCollector(METRIC_AB).get();
            ICol *icol4 = mX.addIntegerCollector(METRIC_AB).get();
            ICol *ignored2 = mX.addIntegerCollector(METRIC_BA).get();

            col1->update(1); col2->update(1);
            col3->update(1); col4->update(1);
            icol1->update(2); icol2->update(2);
            icol3->update(2); icol4->update(2);
            ignored1->update(100); ignored2->update(100);

            bsl::vector<Rec> records(Z);
            mX.collectAndReset(&records, METRIC_AA.category());
            bsl::sort(records.begin(), records.end(), recordLess);
            const balm::MetricRecord& a = records[0];
            const balm::MetricRecord& b = records[1];

            if (veryVerbose) {
                P_(a); P(b);
            }
            ASSERT(METRIC_AA == a.metricId());
            ASSERT(METRIC_AB == b.metricId());
            ASSERT(4 == a.count());
            ASSERT(4 == b.count());
            ASSERT(6 == a.total());
            ASSERT(6 == b.total());
            ASSERT(2 == a.max());
            ASSERT(2 == b.max());
            ASSERT(1 == a.min());
            ASSERT(1 == b.min());

            // Verify that the collectors have been reset.
            Rec x;
            col1->load(&x);
            ASSERT(Rec(METRIC_AA) == x);

            icol1->load(&x);
            ASSERT(Rec(METRIC_AA) == x);

        }

        ASSERT(0 == defaultAllocator.numBytesInUse());
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
