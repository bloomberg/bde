// balm_metricregistry.t.cpp                                          -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balm_metricregistry.h>

#include <balm_metricdescription.h>
#include <balm_metricid.h>
#include <balm_metricformat.h>
#include <balm_publicationtype.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>

#include <bslma_testallocator.h>
#include <bslmt_barrier.h>
#include <bdlmt_fixedthreadpool.h>
#include <bdlf_bind.h>

#include <bsl_c_stdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_functional.h>
#include <bsl_iostream.h>
#include <bsl_ostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

#if defined(BSLS_PLATFORM_CMP_MSVC)
#define snprintf _snprintf_s
#endif

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
// The 'balm::MetricRegistry' is a mechanism for registering category and
// metric identifiers.  This test must ensure that values can be added and
// found and that the container is thread safe.  Finally, this container
// guarantees a single address is provided for each unique string value
// returned (in either a metric name or category name).
// ----------------------------------------------------------------------------
// CREATORS
// [ 2]  balm::MetricRegistry(bslma::Allocator *);
// [ 2]  ~balm::MetricRegistry();
// MANIPULATORS
// [ 2]  balm::MetricId addId(const StringRef& , const StringRef& );
// [ 3]  balm::MetricId getId(const StringRef& , const StringRef& );
// [ 2]  const balm::Category *addCategory(const StringRef& category);
// [ 4]  const balm::Category *getCategory(const StringRef& category);
// [ 6]  void setCategoryEnabled(const balm::Category* , isEnabled);
// [ 6]  void setAllCategoriesEnabled(bool );
// [10]  void setPreferredPublicationType(const balm::MetricId& ,
//                                        balm::PublicationType::Value );
// [11]  void setFormat(const balm::MetricId& , const balm::MetricFormat& );
// [12]  int createUserDataKey();
// [13]  void setUserData(balm::MetricId, int, const void *);
// [14]  void setUserData(const char *, int, const void *,bool);
// ACCESSORS
// [ 2]  bsl::size_t numMetrics() const;
// [ 2]  bsl::size_t numCategories() const;
// [ 2]  balm::MetricId findId(const StringRef&, const StringRef& ) const;
// [ 2]  const balm::Category *findCategory(const StringRef& ) const;
// [ 5]  void getAllCategories(bsl::vector<const balm::Category *> *) const;
// [ 7]  bsl::ostream& print(bsl::ostream& , int, int) const;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 8] TESTING UNIQUE ADDRESSES FOR EQUAL STRING VALUES
// [ 9] BSLMA ALLOCATION EXCEPTION TEST
// [15] CONCURRENCY TEST
// [16] USAGE EXAMPLE

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

typedef balm::Category         Cat;
typedef balm::MetricId         Id;
typedef balm::MetricRegistry   Obj;
typedef balm::PublicationType  Type;
typedef balm::MetricFormat     Format;
typedef balm::MetricFormatSpec Spec;

// ============================================================================
//                        GLOBAL CLASSES FOR TESTING
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

class ConcurrencyTest {
    // Invoke a set of operations operations synchronously.

    // DATA
    bdlmt::FixedThreadPool  d_pool;
    bslmt::Barrier          d_barrier;
    balm::MetricRegistry   *d_registry_p;
    bslma::Allocator       *d_allocator_p;

    // PRIVATE MANIPULATORS
    void execute();
        // Execute a single test.

  public:

    // CREATORS
    ConcurrencyTest(int                   numThreads,
                    balm::MetricRegistry *registry,
                    bslma::Allocator     *basicAllocator)
    : d_pool(numThreads, 1000, basicAllocator)
    , d_barrier(numThreads)
    , d_registry_p(registry)
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
    // On each iteration of this test, a group of identifiers unique to the
    // iteration are created.  4 of those identifiers are shared by all threads
    // (but are unique to the test iteration), while 2 are unique across all
    // threads and iterations.  Then 'getId', 'addId', 'findId', 'getCategory',
    // 'addCategory', and 'findCategory' are all invoked using the created
    // identifiers, and the returned values are verified.  Finally,
    // 'getAllCategories' is invoked.  Note that the unique identifiers are
    // used to ensure a mixture of modifications and accesses occur
    // simultaneously on each iteration, and that some of those simultaneous
    // modifications and accesses are for the same identifier.

    const int NUM_THREADS = d_barrier.numThreads();
    Obj *mX = d_registry_p; const Obj *MX = mX;

    // Initialize testing constants.
    bslma::TestAllocator cAlloc;
    balm::MetricFormat formatA(&cAlloc);
    balm::MetricFormat formatB(&cAlloc);
    formatB.setFormatSpec(Type::e_TOTAL, Spec(1.0, "%f"));
    balm::MetricFormat formatC(&cAlloc);
    formatC.setFormatSpec(Type::e_TOTAL, Spec(1.0, "%f"));
    formatC.setFormatSpec(Type::e_AVG, Spec(1.0, "%d"));
    balm::MetricFormat formatD(&cAlloc);
    formatD.setFormatSpec(Type::e_MIN, Spec(2.0, "%f"));
    formatD.setFormatSpec(Type::e_MAX, Spec(2.0, "%d"));

    const balm::MetricFormat *FORMATS[] = {
        &formatA,
        &formatB,
        &formatC,
        &formatD,
    };
    const int NUM_FORMATS = sizeof FORMATS / sizeof *FORMATS;

    Type::Value TYPES[] = { Type::e_TOTAL,
                            Type::e_COUNT,
                            Type::e_MIN,
                            Type::e_MAX,
                            Type::e_AVG,
                            Type::e_RATE,
                            Type::e_UNSPECIFIED
    };
    const int NUM_TYPES = sizeof TYPES / sizeof *TYPES;

    d_barrier.wait();

    for(int i = 0; i < 10; ++i) {

        // Create 4 strings unique for this iteration.
        bsl::string iterStringA, iterStringB, iterStringC, iterStringD;
        stringId(&iterStringA, "A", i);
        stringId(&iterStringB, "B", i);
        stringId(&iterStringC, "C", i);
        stringId(&iterStringD, "D", i);
        const char *A_VAL = iterStringA.c_str();
        const char *B_VAL = iterStringB.c_str();
        const char *C_VAL = iterStringC.c_str();
        const char *D_VAL = iterStringD.c_str();

        // Create 2 strings unique across all threads & iterations.
        bsl::string uniqueString1, uniqueString2;
        stringId(&uniqueString1, "U1", bslmt::ThreadUtil::selfIdAsInt(), i);
        stringId(&uniqueString2, "U2", bslmt::ThreadUtil::selfIdAsInt(), i);
        const char *S1 = uniqueString1.c_str();
        const char *S2 = uniqueString2.c_str();

        bsl::vector<const balm::Category *> startCategories(d_allocator_p);
        MX->getAllCategories(&startCategories);

        // The expected value for number of iterations is computed as:
        // (iteration * # of shared categories) +
        // (iteration * # of threads * # unique categories per thread)
        ASSERT((i * 4) + (i * NUM_THREADS * 2) == startCategories.size());

        // Begin Test Iteration
        d_barrier.wait();

        // create new category with 'addCategory'.
        mX->addCategory(A_VAL);
        const Cat *CAT_A_1 = mX->findCategory(A_VAL);
        const Cat *CAT_A_2 = mX->getCategory(A_VAL);
        ASSERT(0 != CAT_A_1);
        ASSERT(0 != CAT_A_2);
        ASSERT(0 == bsl::strcmp(CAT_A_1->name(), A_VAL));
        ASSERT(CAT_A_1 == CAT_A_2);

        // create new category with 'getCategory'.
        const Cat *CAT_B_1 = mX->getCategory(B_VAL);
        const Cat *CAT_B_2 = mX->getCategory(B_VAL);
        ASSERT(0 != CAT_B_1);
        ASSERT(0 != CAT_B_2);
        ASSERT(0 == bsl::strcmp(CAT_B_1->name(), B_VAL));
        ASSERT(CAT_B_1 == CAT_B_2);

        // Create new metric description with 'addId'.
        mX->addId(A_VAL, A_VAL);
        Id METRIC_AA_1 = mX->findId(A_VAL, A_VAL);
        Id METRIC_AA_2 = mX->getId(A_VAL, A_VAL);
        ASSERT(METRIC_AA_1 == METRIC_AA_2);
        ASSERT(CAT_A_1     == METRIC_AA_1.category());
        ASSERT(0           == bsl::strcmp(METRIC_AA_1.metricName(), A_VAL));

        // Create new metric description with 'getId'.
        Id METRIC_AB_1 = mX->getId(A_VAL, B_VAL);
        Id METRIC_AB_2 = mX->getId(A_VAL, B_VAL);
        ASSERT(METRIC_AB_1 == METRIC_AB_2);
        ASSERT(CAT_A_1     == METRIC_AB_1.category());
        ASSERT(0           == bsl::strcmp(METRIC_AB_1.metricName(), B_VAL));

        // Create new metric description & category with 'addId'.
        mX->addId(C_VAL, A_VAL);
        Id METRIC_CA_1 = mX->getId(C_VAL, A_VAL);
        Id METRIC_CA_2 = mX->getId(C_VAL, A_VAL);
        ASSERT(METRIC_CA_1 == METRIC_CA_2);
        ASSERT(METRIC_CA_1.category() == MX->findCategory(C_VAL));
        ASSERT(0 == bsl::strcmp(METRIC_CA_1.metricName(), A_VAL));

        // Create new metric description & category with 'getId'.
        Id METRIC_DA_1 = mX->getId(D_VAL, A_VAL);
        Id METRIC_DA_2 = mX->getId(D_VAL, A_VAL);
        ASSERT(METRIC_DA_1 == METRIC_DA_2);
        ASSERT(METRIC_DA_1.category() == MX->findCategory(D_VAL));
        ASSERT(0 == bsl::strcmp(METRIC_DA_1.metricName(), A_VAL));

        // Use unique strings to ensure a category is created.
        const Cat *U_CAT = mX->addCategory(S1);
        ASSERT(0 != U_CAT);
        ASSERT(0 == bsl::strcmp(U_CAT->name(), S1));

        Id U_METRIC_11 = mX->addId(S1, S1);
        Id U_METRIC_12 = mX->getId(S1, S2);
        Id U_METRIC_21 = mX->getId(S2, S1);
        Id U_METRIC_22 = mX->addId(S2, S2);

        ASSERT(U_METRIC_11.isValid());
        ASSERT(U_METRIC_12.isValid());
        ASSERT(U_METRIC_21.isValid());
        ASSERT(U_METRIC_22.isValid());

        ASSERT(0 == bsl::strcmp(U_METRIC_11.categoryName(), S1));
        ASSERT(0 == bsl::strcmp(U_METRIC_12.categoryName(), S1));
        ASSERT(0 == bsl::strcmp(U_METRIC_21.categoryName(), S2));
        ASSERT(0 == bsl::strcmp(U_METRIC_22.categoryName(), S2));

        ASSERT(0 == bsl::strcmp(U_METRIC_11.metricName(), S1));
        ASSERT(0 == bsl::strcmp(U_METRIC_12.metricName(), S2));
        ASSERT(0 == bsl::strcmp(U_METRIC_21.metricName(), S1));
        ASSERT(0 == bsl::strcmp(U_METRIC_22.metricName(), S2));

        // Assign the series of preferred publication types.
        for (int j = 0; j < NUM_TYPES; ++j) {
            mX->setPreferredPublicationType(U_METRIC_11, (Type::Value)j);
            mX->setPreferredPublicationType(U_METRIC_12, (Type::Value)j);
            mX->setPreferredPublicationType(U_METRIC_21, (Type::Value)j);
            mX->setPreferredPublicationType(U_METRIC_22, (Type::Value)j);
            mX->setPreferredPublicationType(METRIC_AA_1, (Type::Value)j);
            mX->setPreferredPublicationType(METRIC_CA_1, (Type::Value)j);
        }

        for (int j = 0; j < NUM_FORMATS; ++j) {
            mX->setFormat(U_METRIC_11, *FORMATS[j]);
            mX->setFormat(U_METRIC_12, *FORMATS[j]);
            mX->setFormat(U_METRIC_21, *FORMATS[j]);
            mX->setFormat(U_METRIC_22, *FORMATS[j]);
            mX->setFormat(METRIC_AA_1, *FORMATS[j]);
            mX->setFormat(METRIC_CA_1, *FORMATS[j]);
        }

        // Run 'getAllCategories' while other threads are actively adding
        // categories and metric ids.
        bsl::vector<const balm::Category *> categories(d_allocator_p);
        MX->getAllCategories(&categories);

        mX->setAllCategoriesEnabled(i % 2);
        mX->setCategoryEnabled(U_CAT, !(i % 2));

        // The expected value for number of categories is computed as:
        // (iteration * # of shared categories) +
        // (iteration * # of threads * # unique categories per thread)
        //
        // This computation is used with the values i and i + 1 to create a
        // window of possible sizes at the end of the *current* threads
        // iteration (other threads may still be processing this iteration).
        ASSERT((i * 4) + (NUM_THREADS * i * 2) + 6 <= categories.size());
        ASSERT(((i + 1) * 4) + (NUM_THREADS * (i + 1) * 2) >=
               categories.size());

        // End test iteration.
        d_barrier.wait();

        ASSERT(!(i % 2) == U_CAT->enabled());
        ASSERT( (i % 2) == CAT_A_1->enabled());
        ASSERT( (i % 2) == CAT_A_2->enabled());
        ASSERT( (i % 2) == CAT_B_1->enabled());
        ASSERT( (i % 2) == CAT_B_2->enabled());

        ASSERT((Type::Value)NUM_TYPES - 1 ==
               U_METRIC_11.description()->preferredPublicationType());
        ASSERT((Type::Value)NUM_TYPES - 1 ==
               U_METRIC_12.description()->preferredPublicationType());
        ASSERT((Type::Value)NUM_TYPES - 1 ==
               U_METRIC_21.description()->preferredPublicationType());
        ASSERT((Type::Value)NUM_TYPES - 1 ==
               U_METRIC_22.description()->preferredPublicationType());
        ASSERT((Type::Value)NUM_TYPES - 1 ==
               METRIC_AA_1.description()->preferredPublicationType());
        ASSERT((Type::Value)NUM_TYPES - 1 ==
               METRIC_CA_1.description()->preferredPublicationType());

        ASSERT(*FORMATS[NUM_FORMATS-1] ==
               *U_METRIC_11.description()->format());
        ASSERT(*FORMATS[NUM_FORMATS-1] ==
               *U_METRIC_12.description()->format());
        ASSERT(*FORMATS[NUM_FORMATS-1] ==
               *U_METRIC_21.description()->format());
        ASSERT(*FORMATS[NUM_FORMATS-1] ==
               *U_METRIC_22.description()->format());
        ASSERT(*FORMATS[NUM_FORMATS-1] ==
               *METRIC_AA_1.description()->format());
        ASSERT(*FORMATS[NUM_FORMATS-1] ==
               *METRIC_CA_1.description()->format());

    }
}

void ConcurrencyTest::runTest()
{
    bsl::function<void()> job = bdlf::BindUtil::bind(&ConcurrencyTest::execute,
                                                      this);
    for (int i = 0; i < d_pool.numThreads(); ++i) {
        d_pool.enqueueJob(job);
    }
    d_pool.drain();
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
      case 16: {
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
// The following example illustrates how to create and use a
// 'balm::MetricRegistry'.  We start by creating a 'balm::MetricRegistry'
// object, 'registry', and then using this registry to create a
// 'balm::MetricId' for a metric "MetricA" belonging to the category
// "MyCategory" (i.e., "MyCategory.MetricA").
//..
    bslma::Allocator    *allocator = bslma::Default::allocator(0);
    balm::MetricRegistry  registry(allocator);

    balm::MetricId idA = registry.addId("MyCategory", "MetricA");
//..
// Now that we have created a metric, "MyCategory.MetricA", attempting to add
// the metric again will return an invalid id.  We retrieve the same
// identifier we have created using either 'getId' or 'findId'.
//..
    balm::MetricId invalidId = registry.addId("MyCategory", "MetricA");
          ASSERT(!invalidId.isValid());

    balm::MetricId idA_copy1 = registry.getId("MyCategory", "MetricA");
          ASSERT(idA_copy1.isValid());
          ASSERT(idA_copy1 == idA);

    balm::MetricId idA_copy2 = registry.findId("MyCategory", "MetricA");
          ASSERT(idA_copy2.isValid());
          ASSERT(idA_copy2 == idA);
//..
// We use the 'getId' method to add a new metric to the registry, then verify
// we can lookup the metric.
//..
    balm::MetricId idB = registry.getId("MyCategory", "MetricB");
          ASSERT(idB.isValid());
          ASSERT(idB == registry.getId("MyCategory", "MetricB"));
          ASSERT(idB == registry.findId("MyCategory", "MetricB"));
          ASSERT(!registry.addId("MyCategory", "MetricB").isValid());
//..
// Next we use 'getCategory' to find the address of the 'balm::Category'
// corresponding to "MyCategory".
//..
    const balm::Category *myCategory = registry.getCategory("MyCategory");
          ASSERT(myCategory == idA.category());
          ASSERT(myCategory == idB.category());
          ASSERT(myCategory->enabled());
//..
// Finally we use the 'setCategoryEnabled' method to disable the category
// "MyCategory":
//..
    registry.setCategoryEnabled(myCategory, false);
          ASSERT(!myCategory->enabled());
//..
      if (veryVerbose) {
          registry.print(bsl::cout, 1, 3);
      }

    } break;
      case 15: {
        // --------------------------------------------------------------------
        // CONCURRENCY TEST
        //
        // Testing:
        //     Thread-safety of 'getId', 'addId', 'getCategory',
        //     'addCategory', 'findCategory', 'findId', and
        //     'getAllCategories' methods.
        //
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TEST CONCURRENCY" << endl
                                  << "================" << endl;

        bslma::TestAllocator defaultAllocator;
        bslma::DefaultAllocatorGuard guard(&defaultAllocator);

        bslma::TestAllocator testAllocator;
        balm::MetricRegistry registry(&testAllocator);;
        {
            ConcurrencyTest tester(10, &registry, &defaultAllocator);
            tester.runTest();
        }
    } break;
      case 14:{
        // --------------------------------------------------------------------
        //  TESTING: 'setUserData'
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   int setUserDataKey(balm::MetricId, int, const void *);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "setUserData for a category" << endl
                          << "==========================" << endl;

        struct {
            const char *d_category;
            const char *d_name;
        } METRICS[] = {
            { "A",    "A" },
            { "A",    "B" },
            { "AA",   "A" },
            { "AA",   "B" },
            { "AAA",  "A" },
            { "AAA",  "B" },
            { "AAAA", "A" },
            { "AAAA", "B" },
            { "AAAB", "A" },
            { "AAAB", "B" },
            { "AABA", "A" },
            { "AABA", "B" },
        };
        const int NUM_METRICS = sizeof METRICS / sizeof *METRICS;

        bslma::TestAllocator  testAllocator;
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
            if (veryVerbose) {
                bsl::cout << "\tverify an explict category" << bsl::endl;
            }

            Obj mX(&testAllocator); const Obj& MX = mX;

            // Create a set of metrics and keys prior to the test.
            for (int i = 0;  i < NUM_METRICS; ++i) {
                mX.createUserDataKey();
                mX.getId(METRICS[i].d_category, METRICS[i].d_name);
            }

            // Perform test.
            for (int i = 0;  i < NUM_METRICS; ++i) {
                const void *VALUE = (const void *)(i + 1);
                mX.setUserData(METRICS[i].d_category, i, VALUE, false);

                // Verify only the metrics belonging to METRIC[i]'s category
                // have been set.
                for (int j = 0; j < NUM_METRICS; ++j) {
                    const void *EXP_VALUE =
                        (0 == bsl::strcmp(METRICS[i].d_category,
                                          METRICS[j].d_category))
                        ? VALUE
                        : 0;
                    balm::MetricId id = mX.getId(METRICS[j].d_category,
                                                METRICS[j].d_name);
                    ASSERT(EXP_VALUE == id.description()->userData(i));
                }
            }

            if (veryVerbose) {
                bsl::cout << "Metrics: " << bsl::endl;
                for (int i = 0; i < NUM_METRICS; ++i) {
                    balm::MetricId id = mX.getId(METRICS[i].d_category,
                                                METRICS[i].d_name);
                    id.description()->printDescription(bsl::cout);
                    bsl::cout << bsl::endl;
                }
            }

            // Verify the default values for newly create metrics:
            for (int i = 0; i < NUM_METRICS; ++i) {
                bsl::ostringstream uniqueId;
                uniqueId << "Metric i: " << i;
                bsl::string temp = uniqueId.str();

                balm::MetricId id = mX.getId(METRICS[i].d_category,
                                            temp.c_str());
                for (int j = 0; j < NUM_METRICS; ++j) {
                    const void *EXP_VALUE =
                        (0 == bsl::strcmp(METRICS[i].d_category,
                                          METRICS[j].d_category))
                        ? (const void *)(j + 1)
                        : 0;
                    LOOP4_ASSERT(*id.description(),
                                 j,
                                 EXP_VALUE,
                                 id.description()->userData(j),
                                 EXP_VALUE == id.description()->userData(j));
                }
            }
            ASSERT(0 == defaultAllocator.numBytesInUse());
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        {
            if (veryVerbose) {
                bsl::cout << "\tverify a category prefix" << bsl::endl;
            }

            Obj mX; const Obj& MX = mX;

            // Create a set of metrics and keys prior to the test.
            for (int i = 0;  i < NUM_METRICS; ++i) {
                mX.createUserDataKey();
                mX.getId(METRICS[i].d_category, METRICS[i].d_name);
            }

            // Perform test.
            for (int i = 0;  i < NUM_METRICS; ++i) {
                const void *VALUE = (const void *)(i + 1);
                mX.setUserData(METRICS[i].d_category, i, VALUE, true);

                P_(METRICS[i].d_category); P_(i); P(VALUE);
                // Verify metrics with categories whose names are a prefix of
                // to METRIC[i]'s category name have been been set.
                for (int j = 0; j < NUM_METRICS; ++j) {
                    bsl::string possiblePrefixCategory(METRICS[i].d_category);
                    bsl::string category(METRICS[j].d_category);

                    const void *EXP_VALUE =
                        0 == category.find(possiblePrefixCategory)
                        ? VALUE
                        : 0;
                    balm::MetricId id = mX.getId(METRICS[j].d_category,
                                                METRICS[j].d_name);
                    ASSERT(EXP_VALUE == id.description()->userData(i));
                }
            }

            if (veryVerbose) {
                bsl::cout << "Metrics: " << bsl::endl;
                for (int i = 0; i < NUM_METRICS; ++i) {
                    balm::MetricId id = mX.getId(METRICS[i].d_category,
                                                METRICS[i].d_name);
                    id.description()->printDescription(bsl::cout);
                    bsl::cout << bsl::endl;
                }
            }

            // Verify the default values for newly create metrics:
            for (int i = 0; i < NUM_METRICS; ++i) {
                bsl::ostringstream uniqueId;
                uniqueId << "Metric i: " << i;
                bsl::string temp = uniqueId.str();

                balm::MetricId id = mX.getId(METRICS[i].d_category,
                                            temp.c_str());
                for (int j = 0; j < NUM_METRICS; ++j) {
                    bsl::string category(METRICS[i].d_category);
                    bsl::string possiblePrefixCategory(METRICS[j].d_category);

                    const void *EXP_VALUE =
                        0 == category.find(possiblePrefixCategory)
                        ? (const void *)(j + 1)
                        : 0;
                    LOOP4_ASSERT(*id.description(),
                                 j,
                                 EXP_VALUE,
                                 id.description()->userData(j),
                                 EXP_VALUE == id.description()->userData(j));
                }
            }
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        //  TESTING: 'setUserData'
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   int setUserDataKey(balm::MetricId, int, const void *);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "setUserData for 1 metric" << endl
                          << "========================" << endl;

        struct {
            const char *d_category;
            const char *d_name;
        } METRICS[] = {
            { "A",    "A" },
            { "A",    "B" },
            { "AA",   "A" },
            { "AA",   "B" },
            { "AAA",  "A" },
            { "AAA",  "B" },
            { "AAAA", "A" },
            { "AAAA", "B" },
            { "AAAB", "A" },
            { "AAAB", "B" },
            { "AABA", "A" },
            { "AABA", "B" },
        };
        const int NUM_METRICS = sizeof METRICS / sizeof *METRICS;

        Obj mX; const Obj& MX = mX;

        // Create the keys we will use.  Note that this test assumes indices
        // are incrementally generated starting at 0.
        for (int i = 0; i < NUM_METRICS; ++i) {
            mX.createUserDataKey();
        }

        if (veryVerbose) {
            bsl::cout << "\tverify metric initial values" << bsl::endl;
        }
        const int NUM_KEYS = 5;
        for (int i = 0; i < NUM_METRICS; ++i) {
            mX.getId(METRICS[i].d_category, METRICS[i].d_name);
            for (int j = 0; j < NUM_KEYS; ++j) {
                ASSERT(0 ==  mX.getId(METRICS[i].d_category,
                                      METRICS[i].d_name).description()->
                                                                  userData(j));
            }
        }

        if (veryVerbose) {
            bsl::cout << "\tSet a value for each metric" << bsl::endl;
        }
        for (int i = 0; i < NUM_METRICS; ++i) {
            balm::MetricId id = mX.getId(METRICS[i].d_category,
                                        METRICS[i].d_name);
            mX.setUserData(id, i, (const void *)(i + 1));
            for (int j = 0; j < NUM_METRICS; ++j) {
                balm::MetricId testId = mX.getId(METRICS[j].d_category,
                                                METRICS[j].d_name);
                const void *EXP_VALUE = j == i ? (const void *)(i + 1) : 0;
                ASSERT(EXP_VALUE == testId.description()->userData(i));
            }
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        //  TESTING: 'createUserDataKey'
        //
        // Concerns:
        //    That 'createUserDataKey' returns a new unique key.
        //
        // Plan:
        //    Black box test: using a set, verify the keys returned by
        //       'createUserDataKey' are unique values >= 0.
        //    White box test: verify that the unique keys are sequential
        //      indices starting at 0.
        // Testing:
        //   int createUserDataKey();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "createUserDataKey" << endl
                          << "=================" << endl;

        const int NUM_KEYS = 10;
        {
            if (veryVerbose) {
                bsl::cout << "\tBlackbox test" << bsl::endl;
            }
            Obj mX; const Obj& MX = mX;
            bsl::set<int> keys(Z);
            for (int i = 0; i < NUM_KEYS; ++i) {
                int newKey = mX.createUserDataKey();
                ASSERT(keys.end() == keys.find(newKey));
                keys.insert(mX.createUserDataKey());
            }
        }
        {
            Obj mX; const Obj& MX = mX;
            if (veryVerbose) {
                bsl::cout << "\tWhitebox test" << bsl::endl;
            }
            for (int i = 0; i < NUM_KEYS; ++i) {
                ASSERT(i == mX.createUserDataKey());
            }
        }
        ASSERT(0 == defaultAllocator.numBytesInUse());
    } break;
      case 11: {
        // --------------------------------------------------------------------
        //  TESTING: 'setFormat'
        //
        // Concerns:
        //   That 'setFormat' sets the format for the indicated metric.
        //
        // Plan:
        //   For a sequence of independent test values use 'addId'.  Verify
        //   the format() is null, then set the format and verify the change.
        //
        // Testing:
        //   void setFormat(const balm::MetricId&     metricId,
        //                  const balm::MetricFormat& format);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'setFormat'." << endl;

        struct {
            const char *d_category;
            const char *d_name;
        } METRICS [] = {
            { "", "",    },
            { "C0", "M0" },
            { "C0", "M1" },
            { "C1", "M2" },
            { "C3", "M3" },
            { "C3", "M4" },
            { "C5", "M1" },
            { "C6", "M7" }
        };
        const int NUM_METRICS = sizeof METRICS / sizeof *METRICS;

        bslma::TestAllocator cAlloc;
        balm::MetricFormat formatA(&cAlloc);
        balm::MetricFormat formatB(&cAlloc);
        formatB.setFormatSpec(Type::e_TOTAL, Spec(1.0, "%f"));
        balm::MetricFormat formatC(&cAlloc);
        formatC.setFormatSpec(Type::e_TOTAL, Spec(1.0, "%f"));
        formatC.setFormatSpec(Type::e_AVG, Spec(1.0, "%d"));
        balm::MetricFormat formatD(&cAlloc);
        formatD.setFormatSpec(Type::e_MIN, Spec(2.0, "%f"));
        formatD.setFormatSpec(Type::e_MAX, Spec(2.0, "%d"));

        const balm::MetricFormat *FORMATS[] = {
            &formatA,
            &formatB,
            &formatC,
            &formatD,
        };
        const int NUM_FORMATS = sizeof FORMATS / sizeof *FORMATS;

        if (veryVerbose) cout << "\tVerify basic behavior." << endl;

        bslma::TestAllocator  testAllocator;
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
            Obj mX(&testAllocator); const Obj& MX = mX;
            for (int i = 0; i < NUM_METRICS; ++i) {
                const char *CAT  = METRICS[i].d_category;
                const char *NAME = METRICS[i].d_name;

                balm::MetricId id = mX.addId(CAT, NAME);
                ASSERT(!id.description()->format());

                for (int j = 0; j < NUM_FORMATS; ++j) {
                    mX.setFormat(id, *FORMATS[j]);
                    ASSERT(*FORMATS[j] == *id.description()->format());
                    ASSERT(FORMATS[j]  != id.description()->format().get());
                }
                ASSERT(0 == defaultAllocator.numBytesInUse());
            }
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

        if (veryVerbose) cout << "\tVerify pooled format strings" << endl;

        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
            Obj mX(&testAllocator); const Obj& MX = mX;
            balm::MetricId id1 = mX.addId("A", "A");
            balm::MetricId id2 = mX.addId("B", "B");
            mX.setFormat(id1, *FORMATS[1]);
            mX.setFormat(id2, *FORMATS[1]);

            const Format *f1 = id1.description()->format().get();
            const Format *f2 = id2.description()->format().get();

            ASSERT(f1 != f2);
            ASSERT(*f1 == *f2);

            // Verify the strings are shared.
            ASSERT(f1->formatSpec(Type::e_TOTAL)->format() ==
                   f2->formatSpec(Type::e_TOTAL)->format());
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
      } break;
      case 10: {
        // --------------------------------------------------------------------
        //  TESTING: 'setPreferredPublicationType'
        //
        // Concerns:
        //   That 'setPreferredPublicationType' sets the preferred publication
        //   type of the supplied metric.
        //
        // Plan:
        //   For a sequence of independent test values use 'addId'.  Verify
        //   the publicationType() is 'UNSPECIFIED', then set the publication
        //   type and verify the updated value.
        //
        // Testing:
        // void setPreferredPublicationType(const balm::MetricId&        ,
        //                                  balm::PublicationType::Value );
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'setPreferredPublicationType'."
                          << endl;

        struct {
            const char *d_category;
            const char *d_name;
        } METRICS [] = {
            { "", "",    },
            { "C0", "M0" },
            { "C0", "M1" },
            { "C1", "M2" },
            { "C3", "M3" },
            { "C3", "M4" },
            { "C5", "M1" },
            { "C6", "M7" }
        };
        const int NUM_METRICS = sizeof METRICS / sizeof *METRICS;

        Type::Value TYPES[] = { Type::e_TOTAL,
                                Type::e_COUNT,
                                Type::e_MIN,
                                Type::e_MAX,
                                Type::e_AVG,
                                Type::e_RATE,
                                Type::e_UNSPECIFIED
        };
        const int NUM_TYPES = sizeof TYPES / sizeof *TYPES;

        Obj mX(Z); const Obj& MX = mX;
        for (int i = 0; i < NUM_METRICS; ++i) {
            const char *CAT  = METRICS[i].d_category;
            const char *NAME = METRICS[i].d_name;

            balm::MetricId id = mX.addId(CAT, NAME);
            ASSERT(Type::e_UNSPECIFIED ==
                   id.description()->preferredPublicationType());

            for (int j = 0; j < NUM_TYPES; ++j) {
                mX.setPreferredPublicationType(id, TYPES[j]);
                ASSERT(TYPES[j] ==
                       id.description()->preferredPublicationType());
            }
        }
        ASSERT(0 == defaultAllocator.numBytesInUse());
    } break;
    case 9: {
        // --------------------------------------------------------------------
        // BSLMA ALLOCATION EXCEPTION TEST
        //
        // Concerns:
        //   That 'addId', 'getId', 'addCategory', and 'getCategory' are
        //   exception safe with respect to allocation.
        //
        // Plan:
        //   Use the 'BSLMA_EXCEPTION_TEST' to verify the manipulator methods
        //   of this object are exception neutral.
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting exception neutrality" << endl;

        struct {
            const char *d_category;
            const char *d_name;
        } METRICS [] = {
            { "", "",    },
            { "C0", "M0" },
            { "C0", "M1" },
            { "C1", "M2" },
            { "C3", "M3" },
            { "C3", "M4" },
            { "C5", "M1" },
            { "C6", "M7" }
        };
        const int NUM_METRICS = sizeof METRICS / sizeof *METRICS;

        {
            if (veryVerbose) {
                cout << "\tVerify 'getId ' is exception neutral"
                     << endl;
            }
            bsl::set<Id>         metrics;
            bslma::TestAllocator testAllocator;

            Obj mX(&testAllocator); const Obj& MX = mX;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                for (int i = 0; i < NUM_METRICS; ++i) {
                    const char *CAT  = METRICS[i].d_category;
                    const char *NAME = METRICS[i].d_name;

                    bsl::vector<const balm::Category *> allCategories;
                    MX.getAllCategories(&allCategories);
                    for (int i = 0; i < allCategories.size(); ++i) {
                        ASSERT(0 != allCategories[i]);
                    }
                    ASSERT(metrics.size() == MX.numMetrics());

                    Id metric = mX.getId(CAT, NAME);

                    ASSERT(metric.isValid());
                    ASSERT(metric == mX.getId(CAT, NAME));
                    ASSERT(false  == mX.addId(CAT, NAME).isValid());
                    ASSERT(metric == MX.findId(CAT, NAME));
                    ASSERT(0      != MX.findCategory(CAT));

                    metrics.insert(metric);
                }
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        }
        {
            if (veryVerbose) {
                cout << "\tVerify 'addId' is exception neutral"
                     << endl;
            }
            bsl::set<Id>         metrics;
            bslma::TestAllocator testAllocator;

            Obj mX(&testAllocator); const Obj& MX = mX;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                for (int i = 0; i < NUM_METRICS; ++i) {
                    const char *CAT  = METRICS[i].d_category;
                    const char *NAME = METRICS[i].d_name;

                    bsl::vector<const balm::Category *> allCategories;
                    MX.getAllCategories(&allCategories);
                    for (int i = 0; i < allCategories.size(); ++i) {
                        ASSERT(0 != allCategories[i]);
                    }
                    ASSERT(metrics.size() == MX.numMetrics());

                    Id metric = mX.addId(CAT, NAME);

                    ASSERT(false  == mX.addId(CAT, NAME).isValid())
                    ASSERT(0      != MX.findCategory(CAT));
                    ASSERT(MX.findId(CAT, NAME).isValid());

                    if (metric.isValid()) {
                        metrics.insert(metric);
                    }
                }
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        }

        const char *CATEGORIES[] = { "", "A", "B", "CAT_A", "CAT_B", "name" };
        const int NUM_CATEGORIES = sizeof CATEGORIES / sizeof *CATEGORIES;
        {
            if (veryVerbose) {
                cout << "\tVerify 'getCategory' is exception neutral"
                     << endl;
            }
            bsl::set<bsl::string> categories;
            bslma::TestAllocator  testAllocator;

            Obj mX(&testAllocator); const Obj& MX = mX;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                for (int i = 0; i < NUM_CATEGORIES; ++i) {
                    const char *CAT  = CATEGORIES[i];

                    bsl::vector<const balm::Category *> allCategories;
                    MX.getAllCategories(&allCategories);
                    for (int i = 0; i < allCategories.size(); ++i) {
                        ASSERT(0 != allCategories[i]);
                    }
                    ASSERT(categories.size() == MX.numCategories());

                    const Cat *CATEGORY = mX.getCategory(CAT);

                    ASSERT(0        != CATEGORY);
                    ASSERT(0        == mX.addCategory(CAT));
                    ASSERT(CATEGORY == mX.getCategory(CAT));
                    ASSERT(CATEGORY == MX.findCategory(CAT));

                    categories.insert(CAT);
                }
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

        }
        {
            if (veryVerbose) {
                cout << "\tVerify 'addCategory' is exception neutral"
                     << endl;
            }
            bsl::set<bsl::string> categories;
            bslma::TestAllocator  testAllocator;

            Obj mX(&testAllocator); const Obj& MX = mX;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                for (int i = 0; i < NUM_CATEGORIES; ++i) {
                    const char *CAT  = CATEGORIES[i];

                    bsl::vector<const balm::Category *> allCategories;
                    MX.getAllCategories(&allCategories);
                    for (int i = 0; i < allCategories.size(); ++i) {
                        ASSERT(0 != allCategories[i]);
                    }
                    ASSERT(categories.size() == MX.numCategories());

                    const Cat *CATEGORY = mX.addCategory(CAT);

                    ASSERT(0 == mX.addCategory(CAT));
                    if (0 != CATEGORY) {
                        categories.insert(CAT);
                    }
                }
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

        }
    } break;
    case 8: {
        // --------------------------------------------------------------------
        // TESTING UNIQUE ADDRESSES FOR EQUAL STRING VALUES
        //
        // Concerns:
        //   That 'addId', 'getId', 'addCategory', and 'getCategory' create
        //   'balm::MetricDescription' and 'balm::Category' objects that
        //   contain a unique address for each unique string value
        //
        // Plan:
        //   Perform a combination of 'addCategory', 'getCategory', 'addId',
        //   and 'getId' operations such that 'addCategory' & 'getCategory'
        //   reuse an identifier used for a metric, and 'addId' & 'getId'
        //   reuse an identifier used for a category.  Verify that the
        //   returned objects re-use the previously created null terminated
        //   string.
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting unique addresses for string values"
                          << endl;

        Obj mX(Z); const Obj& MX = mX;
        const Cat *CAT_A = mX.addCategory("A");
        const Cat *CAT_B = mX.getCategory("B");
        Id METRIC_AA = mX.addId("A", "A");
        Id METRIC_AB = mX.addId("A", "B");
        Id METRIC_AC = mX.addId("A", "C");
        Id METRIC_BA = mX.getId("B", "A");
        Id METRIC_BB = mX.getId("B", "B");
        Id METRIC_BD = mX.getId("B", "D");
        const Cat *CAT_C = mX.addCategory("C");
        const Cat *CAT_D = mX.getCategory("D");
        Id METRIC_EE = mX.addId("E", "E");
        Id METRIC_FF = mX.getId("F", "F");

        // Verify 'addId' reuses string addresses created by 'addCategory'.
        ASSERT(CAT_A->name() == METRIC_AA.metricName());

        // Verify 'addId' reuses string addresses created by getCategory'.
        ASSERT(CAT_B->name() == METRIC_AB.metricName());

        // Verify 'getId' reuses string addresses created by 'addCategory'.
        ASSERT(CAT_A->name() == METRIC_BA.metricName());

        // Verify 'getId' reuses string addresses created by getCategory'.
        ASSERT(CAT_B->name() == METRIC_BB.metricName());

        // Verify 'addCategory' reuses string addresses create by 'addId'.
        ASSERT(CAT_C->name() == METRIC_AC.metricName());

        // Verify 'getCategory' reuses string addresses create by 'addId'.
        ASSERT(CAT_D->name() == METRIC_BD.metricName());

        // Verify 'addId' with equal category and name returns the same string
        // address.
        ASSERT(METRIC_EE.metricName() == METRIC_EE.categoryName())

        // Verify 'getId' with equal category and name returns the same string
        // address.
        ASSERT(METRIC_FF.metricName() == METRIC_FF.categoryName())

    } break;
    case 7: {
        // --------------------------------------------------------------------
        // TESTING: 'print'
        //
        // Concerns:
        //   The output operator properly writes formatted information
        //
        // Plan:
        //   For each of a small representative set of object values, use
        //   'ostrstream' to write that object's value to a character buffer
        //   and then compare the contents of that buffer with the expected
        //   output format.
        //
        // Testing:
        //   ostream& print(ostream&, int, int);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting output operators" << endl;
        Obj mX(Z); const Obj& MX = mX;
        mX.addId("A", "A");
        mX.addId("A", "B");
        mX.addId("B", "A");
        mX.addId("B", "B");

        bsl::ostringstream singleline;
        const char *SINGLE_LINE =
            "[  [  [ A ENABLED ]   A.A   A.B  ]  "
               "[  [ B ENABLED ]   B.A   B.B  ] ] ";
        mX.print(singleline, 0, -1);

        if (veryVerbose) {
            P(SINGLE_LINE);
            P(singleline.str());
        }
        ASSERT(0 == bsl::strcmp(SINGLE_LINE, singleline.str().c_str()));

        bsl::ostringstream multiline;
        const char *MULTI_LINE =
            "   [\n"
            "      [  [ A ENABLED ]\n"
            "         A.A\n"
            "         A.B\n"
            "      ]\n"
            "      [  [ B ENABLED ]\n"
            "         B.A\n"
            "         B.B\n"
            "      ]\n"
            "   ]\n";

        mX.print(multiline, 1, 3);

        if (veryVerbose) {
            P(MULTI_LINE);
            P(multiline.str());
        }
        ASSERT(0 == bsl::strcmp(MULTI_LINE, multiline.str().c_str()));
    } break;
    case 6: {
        // --------------------------------------------------------------------
        //  TESTING: 'setCategoryEnabled', 'setAllCategoriesEnabled'
        //
        // Concerns:
        //   That 'setCategoryEnabled' sets the boolean 'enabled' property of
        //   the supplied category with no side effects.
        //
        // Plan:
        //   For a sequence of independent test values use 'addCategory' and
        //   'addId' to add metric categories.  For each added category,
        //   disable the category, verify the category is disabled and no
        //   other categories were effected, then re-enabled the category, and
        //   verify no other categories are effected.
        //
        // Testing:
        //   void setCategoryEnabled(const balm::Category* , bool );
        //   void setAllCategoriesEnabled(bool );
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'setCategoryEnabled'."
                          << endl;

        const char *CATEGORIES[] = { "", "A", "B", "CAT_A", "CAT_B", "name" };
        const int NUM_CATEGORIES = sizeof CATEGORIES / sizeof *CATEGORIES;

        bsl::vector<const balm::Category *> categories(Z);

        Obj mX(Z); const Obj& MX = mX;
        for (int i = 0; i < NUM_CATEGORIES; ++i) {
            if (i % 2 == 0) {
                categories.push_back(mX.addCategory(CATEGORIES[i]));
            }
            else {
                categories.push_back(mX.addId(CATEGORIES[i], "A").category());
            }
        }
        ASSERT(categories.size() == NUM_CATEGORIES);

        for (int i = 0; i < NUM_CATEGORIES; ++i) {
            const Cat *category = categories[i];

            ASSERT(category->enabled());
            ASSERT(MX.findCategory(category->name())->enabled());

            mX.setCategoryEnabled(category, false);
            ASSERT(!category->enabled());
            ASSERT(!MX.findCategory(category->name())->enabled());

            for (int j = 0; j < NUM_CATEGORIES; ++j) {
                bool isEnabled = i != j;
                ASSERT(isEnabled == categories[j]->enabled());
                ASSERT(isEnabled ==
                       MX.findCategory(categories[j]->name())->enabled());
            }

            mX.setCategoryEnabled(category, true);
            ASSERT(category->enabled());
            ASSERT(MX.findCategory(category->name())->enabled());
            for (int j = 0; j < NUM_CATEGORIES; ++j) {
                ASSERT(categories[j]->enabled());
                ASSERT(MX.findCategory(categories[j]->name())->enabled());
            }
        }

        mX.setAllCategoriesEnabled(false);
        for (int i = 0; i < categories.size(); ++i) {
            const Cat *category = categories[i];
            ASSERT(!category->enabled());
        }
        const Cat *newCat1 = mX.addCategory("newCat1");
        ASSERT(!newCat1->enabled());
        categories.push_back(newCat1);

        mX.setAllCategoriesEnabled(true);
        for (int i = 0; i < categories.size(); ++i) {
            const Cat *category = categories[i];
            ASSERT(category->enabled());
        }
        const Cat *newCat2 = mX.addCategory("newCat2");
        ASSERT(newCat2->enabled());
        categories.push_back(newCat2);

        ASSERT(0 == defaultAllocator.numBytesInUse());
      } break;
      case 5: {
        // --------------------------------------------------------------------
        //  TESTING: 'getAllCategories'
        //
        // Concerns:
        //   That 'getAllCategories' returns all the categories added to the
        //   metric registry (either by adding a category, or by adding a
        //   metric id).
        //
        //   That 'getAllCategories' appends to the supplied vector, and
        //   performs 1 (or fewer) memory allocations.
        //
        // Plan:
        //   For a sequence of independent test values use 'addCategory' and
        //   'addId' to add metric categories.  Use 'getAllCategories' and
        //   verify the returned vector contains the set of expected category
        //   addresses.
        //
        //   Then, for a sequence of independent test values use 'addCategory'
        //   and 'addId' to add metric categories.  Create a vector containing
        //   an initial sequence of elements and Use 'getAllCategories' and
        //   verify that only one (or fewer) allocations were made and that
        //   the categories were appended to the end of the supplied vector.
        //
        // Testing:
        //   void getAllCategories(bsl::vector<const balm::Category *>*) const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'getAllCategories'."
                          << endl;

        const char *CATEGORIES[] = { "", "A", "B", "CAT_A", "CAT_B", "name" };
        const int NUM_CATEGORIES = sizeof CATEGORIES / sizeof *CATEGORIES;
        {
            if (veryVerbose) {
                cout << "\tVerify 'getAllCategories' will return all created "
                     << " categories." << endl;
            }

            bsl::set<const balm::Category *>    categorySet(Z);
            bsl::vector<const balm::Category *> categories(Z);

            Obj mX(Z); const Obj& MX = mX;
            MX.getAllCategories(&categories);
            ASSERT(0 == categories.size());
            for (int i = 0; i < NUM_CATEGORIES; ++i) {
                const char *CATEGORY = CATEGORIES[i];

                if (i % 2 == 0) {
                    mX.addCategory(CATEGORY);
                }
                else {
                    mX.addId(CATEGORY,"A");
                }
                categorySet.insert(MX.findCategory(CATEGORY));

                categories.clear();
                ASSERT(i + 1 == categorySet.size());
                ASSERT(i + 1 == MX.numCategories());

                MX.getAllCategories(&categories);

                ASSERT(i + 1 == categories.size());

                bsl::set<const balm::Category *> tempSet(categories.begin(),
                                                        categories.end());
                ASSERT(categorySet == tempSet);
            }
            ASSERT(0 == defaultAllocator.numBytesInUse());
        }
        {
            if (veryVerbose) {
                cout << "\tVerify 'getAllCategories' appends to the existing "
                     << " vector and only performs a single allocation."
                     << endl;
            }

            bsl::vector<const balm::Category *> initialVector(Z);
            initialVector.push_back(0);
            initialVector.push_back((const balm::Category *)1);
            initialVector.push_back((const balm::Category *)2);
            initialVector.push_back((const balm::Category *)3);
            const int INITIAL_SIZE = initialVector.size();

            Obj mX(Z); const Obj& MX = mX;
            {
                bsl::vector<const balm::Category *>
                                                   categories(initialVector,Z);
                int exp_allocations = allocator.numAllocations();
                MX.getAllCategories(&categories);
                ASSERT(categories      == initialVector);
                ASSERT(exp_allocations <= allocator.numAllocations());
            }
            for (int i = 0; i < NUM_CATEGORIES; ++i) {
                const char *CATEGORY = CATEGORIES[i];

                if (i % 2 == 0) {
                    mX.addCategory(CATEGORY);
                }
                else {
                    mX.addId(CATEGORY,"A");
                }

                bsl::vector<const balm::Category *> exp_categories(Z);
                MX.getAllCategories(&exp_categories);

                bsl::vector<const balm::Category *>
                                                   categories(initialVector,Z);
                int exp_allocations = allocator.numAllocations() + 1;
                MX.getAllCategories(&categories);
                ASSERT(exp_allocations <= allocator.numAllocations());

                ASSERT(initialVector.size() + MX.numCategories() ==
                       categories.size());

                // Verify that 'categories' starts with 'initialVector'.
                for (int i = 0; i < INITIAL_SIZE; ++i) {
                    ASSERT(initialVector[i] == categories[i]);
                }
                // Verify that 'categories' ends with 'exp_categories'.
                for (int i = INITIAL_SIZE; i < categories.size(); ++i) {
                    ASSERT(exp_categories[i - INITIAL_SIZE] ==
                           categories[i]);
                }
            }
            ASSERT(0 == defaultAllocator.numBytesInUse());
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        //  TESTING: 'getCategory'
        //
        // Concerns:
        //   That 'getId' always returns a valid category address.  That if an
        //   category does not already exist, a new category is created.
        //
        // Plan:
        //   For a sequence of independent test values use 'addCategory' to
        //   add a metric category, then use 'getCategory' to retrieve that
        //   value.
        //
        //   Next, for a sequence of independent test values use 'getCategory'
        //   to add a category.
        //
        // Testing:
        //   const balm::Category *getCategory(const StringRef& category);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'getCategory'."
                          << endl;

        const char *CATEGORIES[] = { "", "A", "B", "CAT_A", "CAT_B", "name" };
        const int NUM_CATEGORIES = sizeof CATEGORIES / sizeof *CATEGORIES;
        {
            if (veryVerbose) {
                cout << "\tVerify 'getCategory 'will return a previously "
                     << " created category." << endl;
            }
            Obj mX(Z); const Obj& MX = mX;
            for (int i = 0; i < NUM_CATEGORIES; ++i) {
                const char *CATEGORY = CATEGORIES[i];

                const Cat *exp_cat = mX.addCategory(CATEGORY);
                ASSERT(0 != exp_cat);

                const Cat *cat = mX.getCategory(CATEGORY);
                ASSERT(0       != cat);
                ASSERT(exp_cat == cat);

                ASSERT(0     == MX.numMetrics());
                ASSERT(i + 1 == MX.numCategories());
            }
            ASSERT(0 == defaultAllocator.numBytesInUse());
        }
        {
            if (veryVerbose) {
                cout << "\tVerify 'getCategory ' will create new categories."
                     << endl;
            }
            Obj mX(Z); const Obj& MX = mX;
            for (int i = 0; i < NUM_CATEGORIES; ++i) {
                const char *CATEGORY = CATEGORIES[i];

                const Cat *cat = mX.getCategory(CATEGORY);
                ASSERT(0 != cat);
                ASSERT(0 == bsl::strcmp(cat->name(), CATEGORY));
                ASSERT(cat->enabled());

                ASSERT(0   == mX.addCategory(CATEGORY));
                ASSERT(cat == MX.findCategory(CATEGORY));
                ASSERT(cat == mX.addId(CATEGORY, "A").category());

                ASSERT(i + 1 == MX.numMetrics());
                ASSERT(i + 1 == MX.numCategories());

            }
            ASSERT(0 == defaultAllocator.numBytesInUse());
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        //  TESTING: 'getId'
        //
        // Concerns:
        //   That 'getId' always returns a valid identifier.  That if an
        //   identifier does not already exist, a new id is created.  That a
        //   created id's 'category()' holds the unique category address for
        //   that category.
        //
        // Plan:
        //   For a sequence of independent test values use addId to add a
        //   metric id, then use getId to retrieve that value.
        //
        //   Next, for a sequence of independent test values use getId to add a
        //   metric id.
        //
        //   Finally, for a sequence of independent test values use
        //   the 'addCategory' manipulator to add a category, then use
        //   the 'getId' manipulator to add a metric id, the verify the
        //   returned metric id's category address equals the one returned from
        //   'addCategory'.
        //
        // Testing:
        //   balm::MetricId getId(const StringRef& , const StringRef& );
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'getId'."
                          << endl;

        struct {
            const char *d_category;
            const char *d_name;
        } METRICS [] = {
            { "" , ""   },
            { "C0", "M0"},
            { "C0", "M1"},
            { "C1", "M2"},
            { "C3", "M3"}
        };
        const int NUM_METRICS = sizeof METRICS / sizeof *METRICS;

        {
            if (veryVerbose) {
                cout << "\tVerify 'getId 'will return a previously created id."
                     << endl;
            }
            bsl::set<bsl::string> categoryNames(Z);
            Obj mX(Z); const Obj& MX = mX;
            for (int i = 0; i < NUM_METRICS; ++i) {
                const char *CATEGORY = METRICS[i].d_category;
                const char *NAME     = METRICS[i].d_name;
                categoryNames.insert(CATEGORY);

                Id id    = mX.addId(CATEGORY, NAME);
                ASSERT(id.isValid());
                Id getid = mX.getId(CATEGORY, NAME);

                ASSERT(id == getid);
                ASSERT(id.description() == getid.description());
                ASSERT(id.category()    == getid.category());

                ASSERT(i + 1                == MX.numMetrics());
                ASSERT(categoryNames.size() == MX.numCategories());
            }
            ASSERT(0 == defaultAllocator.numBytesInUse());
        }
        {
            if (veryVerbose) {
                cout << "\tVerify 'getId 'will create new ids."
                     << endl;
            }
            bsl::set<bsl::string> categoryNames(Z);
            Obj mX(Z); const Obj& MX = mX;
            for (int i = 0; i < NUM_METRICS; ++i) {
                const char *CATEGORY = METRICS[i].d_category;
                const char *NAME     = METRICS[i].d_name;
                categoryNames.insert(CATEGORY);

                Id id = mX.getId(CATEGORY, NAME);
                ASSERT(id.isValid());
                ASSERT(0 != id.description());
                ASSERT(0 != id.category());
                ASSERT(0 == bsl::strcmp(id.metricName(), NAME));
                ASSERT(0 == bsl::strcmp(id.categoryName(), CATEGORY));
                ASSERT(id.category()->enabled());

                ASSERT(!mX.addId(CATEGORY, NAME).isValid());
                ASSERT(id == MX.findId(CATEGORY, NAME));

                ASSERT(i + 1                == MX.numMetrics());
                ASSERT(categoryNames.size() == MX.numCategories());
            }
            ASSERT(0 == defaultAllocator.numBytesInUse());
        }
        {
            if (veryVerbose) {
                cout << "\tVerify 'getId' will return ids with the correct "
                     << "category address" << endl;
            }
            bsl::set<bsl::string> categoryNames(Z);
            Obj mX(Z); const Obj& MX = mX;
            for (int i = 0; i < NUM_METRICS; ++i) {
                const char *CATEGORY = METRICS[i].d_category;
                const char *NAME     = METRICS[i].d_name;
                categoryNames.insert(CATEGORY);

                mX.addCategory(CATEGORY);
                const Cat *cat = MX.findCategory(CATEGORY);

                Id id = mX.getId(CATEGORY, NAME);
                ASSERT(id.isValid());
                ASSERT(0 != id.description());
                ASSERT(cat == id.category());
                ASSERT(0 == bsl::strcmp(id.metricName(), NAME));
                ASSERT(0 == bsl::strcmp(id.categoryName(), CATEGORY));

                ASSERT(i + 1                == MX.numMetrics());
                ASSERT(categoryNames.size() == MX.numCategories());
            }
            ASSERT(0 == defaultAllocator.numBytesInUse());
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING BASIC MANIPULATORS and ACCESSORS (BOOTSTRAP):
        //
        // Concerns:
        //   The primary fields must be correctly modifiable and accessible.
        //
        //   That 'addId' will return a valid id for a new metric, and an
        //   invalid id for previously registered metric.  That 'findId' will
        //   return the correct added id.  That 'findCategory' for the
        //   metric's category will return an address equal to the id's
        //   'category()'.  That 'numMetrics' and 'numCategories' reflects the
        //   number of unique metrics and categories added to the registry.
        //
        //   That 'addCategory' will return a valid address for a new
        //   category and 0 if the category was previously registered.  That
        //   'findCategory' will return the correct address for an added
        //   category.  That a metric added to registry belonging to that
        //   category has the correct 'category' address.
        //
        //   That modifying the registry does not allocate memory from the
        //   default allocator.
        //
        // Plan:
        //   First, verify the default constructor by testing the value of the
        //   resulting object.
        //
        //   Next, for a sequence of independent metric id values, add
        //   elements to the container and used the basic accessors to verify
        //   the modification.
        //
        //   Finally, for a sequence of independent category values, add
        //   elements to the container and used the basic accessors to verify
        //   the modification.
        //
        // Testing:
        //  balm::MetricRegistry(bslma::Allocator *);
        //   ~balm::MetricRegistry();
        //   balm::MetricId addId(const StringRef& , const StringRef& );
        //   const balm::Category *addCategory(const StringRef& category);
        //   bsl::size_t numMetrics() const;
        //   bsl::size_t numCategories() const;
        //   balm::MetricId findId(const StringRef&, const StringRef& ) const;
        //   const balm::Category *findCategory(const StringRef& ) const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting basic mainpulator and accessors."
                          << endl;

        {
            if (veryVerbose) {
                cout << "\tTest default constructor." << endl;
            }
            Obj mX(Z); const Obj& MX = mX;
            ASSERT(0 == MX.numMetrics());
            ASSERT(0 == MX.numCategories());
            ASSERT(0 == defaultAllocator.numBytesInUse());
        }
        struct {
            const char *d_category;
            const char *d_name;
        } METRICS [] = {
            { "", "",    },
            { "C0", "M0" },
            { "C0", "M1" },
            { "C1", "M2" },
            { "C3", "M3" },
        };
        const int NUM_METRICS = sizeof METRICS / sizeof *METRICS;
        {
            if (veryVerbose) {
                cout << "\tTest adding metrics." << endl;
            }
            bsl::set<bsl::string> categoryNames(Z);

            Obj mX(Z); const Obj& MX = mX;
            for (int i = 0; i < NUM_METRICS; ++i) {
                const char *CATEGORY = METRICS[i].d_category;
                const char *NAME     = METRICS[i].d_name;
                categoryNames.insert(CATEGORY);

                // Add a new id and verify the returned properties.
                Id id = mX.addId(CATEGORY, NAME);
                ASSERT(id.isValid());
                ASSERT(0 != id.description());
                ASSERT(0 != id.category());
                ASSERT(0 == bsl::strcmp(id.metricName(), NAME));
                ASSERT(0 == bsl::strcmp(id.categoryName(), CATEGORY));
                ASSERT(id.category()->enabled());

                // Attempt to find the id.
                Id foundId = MX.findId(CATEGORY, NAME);
                ASSERT(foundId.isValid());
                ASSERT(foundId == id);

                // Attempt to add the id a second time
                Id invalidId = mX.addId(CATEGORY, NAME);
                ASSERT(!invalidId.isValid());

                // Attempt to find the category.
                const Cat *foundCat = MX.findCategory(CATEGORY);
                ASSERT(id.category() == foundCat);
                ASSERT(0 == mX.addCategory(CATEGORY));

                ASSERT(i + 1 == MX.numMetrics());
                ASSERT(categoryNames.size() == MX.numCategories());
            }
            ASSERT(NUM_METRICS == MX.numMetrics());
            ASSERT(categoryNames.size() == MX.numCategories());

            const Cat *NEW_CAT = mX.addCategory("NewCategory");
            ASSERT(0 != NEW_CAT);
            ASSERT(0 == bsl::strcmp("NewCategory", NEW_CAT->name()));
            ASSERT(NEW_CAT->enabled());

            ASSERT(0 == defaultAllocator.numBytesInUse());
        }

        const char *CATEGORIES[] = { "", "A", "B", "CAT_A", "CAT_B", "name" };
        const int NUM_CATEGORIES = sizeof CATEGORIES / sizeof *CATEGORIES;
        {
            if (veryVerbose) {
                cout << "\tTest adding categories." << endl;
            }

            Obj mX(Z); const Obj& MX = mX;
            for (int i = 0; i < NUM_CATEGORIES; ++i) {
                const char *CATEGORY = CATEGORIES[i];

                const Cat *cat = mX.addCategory(CATEGORY);
                ASSERT(0 != cat);
                ASSERT(0 == bsl::strcmp(cat->name(), CATEGORY));
                ASSERT(cat->enabled());

                ASSERT(0   == mX.addCategory(CATEGORY));
                ASSERT(cat == MX.findCategory(CATEGORY));

                Id id = mX.addId(CATEGORY, "Metric");
                ASSERT(id.isValid());
                ASSERT(cat == id.category());
                ASSERT(0   == bsl::strcmp(id.categoryName(), CATEGORY));
                ASSERT(0   == bsl::strcmp(id.metricName(), "Metric"));

                ASSERT(i + 1 == MX.numMetrics());
                ASSERT(i + 1 == MX.numCategories());
            }
            ASSERT(0 == defaultAllocator.numBytesInUse());
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

        {
            Obj mX; const Obj& MX = mX;

            if (veryVerbose) cout << "\tAdd and categories 'CA' and 'CB' and "
                                  << "verify initial state.\n";

            const Cat *CA = mX.addCategory("A");
            const Cat *CB = mX.addCategory("B");
            ASSERT(0  != CA);
            ASSERT(0  == mX.addCategory("A"));
            ASSERT(CA == mX.getCategory("A"));
            ASSERT(0  != CB);
            ASSERT(0  == mX.addCategory("B"));
            ASSERT(CB == mX.getCategory("B"));
            ASSERT(CA != CB);
            ASSERT(0 == bsl::strcmp(CA->name(), "A"));
            ASSERT(0 == bsl::strcmp(CB->name(), "B"));
            ASSERT(CA->enabled());
            ASSERT(CB->enabled());

            if (veryVerbose) cout << "\tget categories 'CC' and 'CD' and "
                                  << "verify initial state.\n";

            const Cat *CC = mX.getCategory("C");
            const Cat *CD = mX.getCategory("D");
            ASSERT(0  != CC);
            ASSERT(0  != CD);
            ASSERT(0  == mX.addCategory("C"));
            ASSERT(0  == mX.addCategory("D"));
            ASSERT(CC == mX.getCategory("C"));
            ASSERT(CD == mX.getCategory("D"));
            ASSERT(0 == bsl::strcmp(CC->name(), "C"));
            ASSERT(0 == bsl::strcmp(CD->name(), "D"));

            if (veryVerbose) cout << "\tAdd and get ids 'MAA', 'MAB', "
                                  << "'MEA', 'MEB' and verify initial "
                                  << "state.\n";

            Id MAA = mX.addId("A", "A");
            Id MAB = mX.addId("A", "B");
            Id MEA = mX.addId("E", "A");
            Id MEB = mX.addId("E", "B");

            ASSERT(MAA.isValid());
            ASSERT(MAB.isValid());
            ASSERT(MEA.isValid());
            ASSERT(MEB.isValid());
            ASSERT(MAA.category() == MAB.category());
            ASSERT(MEA.category() == MEB.category());

            ASSERT(CA == MAA.category());
            ASSERT(CA == MAB.category());

            ASSERT(0 == bsl::strcmp(MAA.metricName(), "A"));
            ASSERT(0 == bsl::strcmp(MAB.metricName(), "B"));
            ASSERT(0 == bsl::strcmp(MEA.metricName(), "A"));
            ASSERT(0 == bsl::strcmp(MEB.metricName(), "B"));

            ASSERT(0 == bsl::strcmp(MAA.categoryName(), "A"));
            ASSERT(0 == bsl::strcmp(MAB.categoryName(), "A"));
            ASSERT(0 == bsl::strcmp(MEA.categoryName(), "E"));
            ASSERT(0 == bsl::strcmp(MEB.categoryName(), "E"));

            ASSERT(CA == MAA.category());
            ASSERT(CA == MAB.category());

            if (veryVerbose) cout << "\tAdd and get ids 'MAA', 'MAB', 'MEA', "
                                  << "'MEB' and verify initial state.\n";

            ASSERT(!mX.addId("A", "A").isValid());
            ASSERT(!mX.addId("A", "B").isValid());
            ASSERT(!mX.addId("E", "A").isValid());
            ASSERT(!mX.addId("E", "B").isValid());

            ASSERT(MAA == mX.getId("A", "A"));
            ASSERT(MAB == mX.getId("A", "B"));
            ASSERT(MEA == mX.getId("E", "A"));
            ASSERT(MEB == mX.getId("E", "B"));
        }
        {
            if (veryVerbose) cout << "\tVerify setUserData and userData\n";

            const int NUM_KEYS = 5;
            struct {
                const char *d_category;
                const char *d_name;
            } METRICS[] = {
                { "A",    "A" },
                { "A",    "B" },
                { "AA",   "A" },
                { "AA",   "B" },
                { "AAA",  "A" },
                { "AAA",  "B" },
                { "AAAA", "A" },
                { "AAAA", "B" },
                { "AAAB", "A" },
                { "AAAB", "B" },
                { "AABA", "A" },
                { "AABA", "B" },
            };
            Obj mX; const Obj& MX = mX;
            const int NUM_METRICS = sizeof(METRICS)/sizeof(*METRICS);

            for (int i = 0; i < NUM_KEYS; ++i) {
                ASSERT(i == mX.createUserDataKey());
            }

            const void *VALUE1 = (const void *)1;
            const void *VALUE2 = (const void *)2;
            const void *VALUE3 = (const void *)3;

            // Verify all keys are initially 0
            for (int i = 0; i < NUM_METRICS; ++i) {
                mX.getId(METRICS[i].d_category, METRICS[i].d_name);
                for (int j = 0; j < NUM_KEYS; ++j) {
                    ASSERT(0 ==  mX.getId(METRICS[i].d_category,
                                          METRICS[i].d_name).description()->
                                                                  userData(j));
                }
            }

            // Set a key for a metric to all possible values
            for (int i = 0; i < NUM_KEYS; ++i) {
                balm::MetricId id = mX.getId("A", "A");
                mX.setUserData(id, i, (const void *)i);
                ASSERT((const void *)i == id.description()->userData(i));
                mX.setUserData(id, i, 0);
                ASSERT(0 == id.description()->userData(i));
            }

            // Set a key for a specific category.
            const int TEST_KEY1 = 1;
            mX.setUserData("AAA", TEST_KEY1, VALUE2);

            ASSERT(VALUE2 ==
                   mX.getId("AAA", "A").description()->userData(TEST_KEY1));
            ASSERT(VALUE2 ==
                   mX.getId("AAA", "B").description()->userData(TEST_KEY1));
            ASSERT(0 ==
                   mX.getId("AAAA", "A").description()->userData(TEST_KEY1));

            // Create a new metric in the category, and verify that it uses
            // the user data as a default.
            ASSERT(VALUE2 ==
                   mX.getId("AAA", "C").description()->userData(TEST_KEY1));

            // Set a key for a category prefix
            const int TEST_KEY2 = 2;
            mX.setUserData("AAA", TEST_KEY2, VALUE2, true);

            ASSERT(VALUE2 ==
                   mX.getId("AAA", "A").description()->userData(TEST_KEY2));
            ASSERT(VALUE2 ==
                   mX.getId("AAA", "B").description()->userData(TEST_KEY2));
            ASSERT(VALUE2 ==
                   mX.getId("AAAA", "A").description()->userData(TEST_KEY2));
            ASSERT(VALUE2 ==
                   mX.getId("AAAA", "B").description()->userData(TEST_KEY2));
            ASSERT(VALUE2 ==
                   mX.getId("AAAB", "A").description()->userData(TEST_KEY2));
            ASSERT(VALUE2 ==
                   mX.getId("AAAB", "B").description()->userData(TEST_KEY2));
            ASSERT(0 ==
                   mX.getId("AABA", "A").description()->userData(TEST_KEY2));

            // Create a new metric in the category, and verify that it uses
            // the user data as a default.
            ASSERT(VALUE2 ==
                   mX.getId("AAA", "D").description()->userData(TEST_KEY2));
            ASSERT(VALUE2 ==
                   mX.getId("AAAA", "D").description()->userData(TEST_KEY2));
            ASSERT(VALUE2 ==
                   mX.getId("AAAB", "D").description()->userData(TEST_KEY2));
            ASSERT(0 ==
                   mX.getId("AABA", "D").description()->userData(TEST_KEY2));
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
