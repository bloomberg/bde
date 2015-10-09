// balm_metricdescription.t.cpp                                       -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balm_metricdescription.h>

#include <balm_category.h>
#include <balm_metricformat.h>

#include <bslma_testallocator.h>
#include <bslmt_barrier.h>
#include <bdlmt_fixedthreadpool.h>
#include <bdlf_bind.h>
#include <bslma_allocator.h>
#include <bslma_testallocator.h>
#include <bslma_defaultallocatorguard.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_functional.h>
#include <bsl_iostream.h>
#include <bsl_ostream.h>
#include <bsl_sstream.h>

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
// A 'balm::MetricDescription' is a simple mechanism containing the address of
// a (non-modifiable) 'balm::Category' and the address of a null-terminated
// string.
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] balm::MetricDescription(const balm::Category *, const char *);
// [ 2] ~balm::MetricDescription();
// MANIPULATORS
// [ 3] void setName(const char *);
// [ 3] void setCategory(const balm::Category *)
// [ 5] void setPreferredPublicationType(balm::PublicationType::Value type);
// [ 6] void setFormat(bsl::shared_ptr<const balm::MetricFormat>&);
// ACCESSORS
// [ 2] const char *name() const;
// [ 2] const balm::Category *category() const;
// [ 4] bsl::ostream& print(bsl::ostream& ) const;
// [ 5] balm::PublicationType::Value preferredPublicationType() const;
// [ 6] bsl::shared_ptr<const balm::MetricFormat> format() const;
// [ 7] void setUserData(UserDataKey key,  const void *value);
// [ 7] const void *userData(UserDataKey key) const;
// FREE OPERATORS
// [ 4] operator<<(ostream&, const balm::MetricDescription&);
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

typedef balm::Category           Category;
typedef balm::MetricDescription  Obj;
typedef balm::PublicationType    Type;
typedef balm::MetricFormat       Format;

// ============================================================================
//                      GLOBAL STUB CLASSES FOR TESTING
// ----------------------------------------------------------------------------

class ConcurrencyTest {
    // Invoke a set of operations operations synchronously.

    // DATA
    bdlmt::FixedThreadPool   d_pool;
    bslmt::Barrier           d_barrier;
    balm::MetricDescription *d_description_p;
    bslma::Allocator        *d_allocator_p;

    // PRIVATE MANIPULATORS
    void execute();
        // Execute a single test.

  public:

    // CREATORS
    ConcurrencyTest(int                      numThreads,
                    balm::MetricDescription  *description,
                    bslma::Allocator        *basicAllocator)
    : d_pool(numThreads, 1000, basicAllocator)
    , d_barrier(numThreads)
    , d_description_p(description)
    , d_allocator_p(basicAllocator)
    {
        d_pool.start();
    }

    ~ConcurrencyTest() {}

    //  MANIPULATORS
    void runTest();
        // Run the test.
};

bslmt::Mutex m;
void ConcurrencyTest::execute()
{
    bslma::Allocator *Z = d_allocator_p;
    Obj *mX = d_description_p; const Obj *MX = mX;

    Format A(Z), B(Z), C(Z), D(Z), E(Z), F(Z);
    const Format *FORMATS[] = { &A, &B, &C, &D, &E, &F };
    const int NUM_FORMATS   = sizeof FORMATS / sizeof *FORMATS;

    Type::Value TYPES[] = { Type::e_TOTAL,
                            Type::e_COUNT,
                            Type::e_MIN,
                            Type::e_MAX,
                            Type::e_AVG,
                            Type::e_RATE,
                            Type::e_UNSPECIFIED
    };
    const int NUM_TYPES = sizeof TYPES / sizeof *TYPES;

    F.setFormatSpec(Type::e_MIN, balm::MetricFormatSpec(2, "%f"));

    ASSERT(!MX->format());

    for (int iteration = 0; iteration < 20; ++iteration) {
        d_barrier.wait();

        for (int j = 0; j < NUM_TYPES; ++j) {
            mX->setPreferredPublicationType(TYPES[j]);
            int x = (int)MX->preferredPublicationType();
            ASSERT(0 <= x && x < Type::k_LENGTH);
        }

        for (int i = 0; i < NUM_FORMATS; ++i) {
            bsl::shared_ptr<const Format> fmt(FORMATS[i],
                                              bslstl::SharedPtrNilDeleter(),
                                              Z);
            mX->setFormat(fmt);
            ASSERT(MX->format());
        }

        d_barrier.wait();

        // Note that '&F' may not equal 'MX->format()'.
        ASSERT(F == *MX->format());

        // Prevent the shared pointers from other threads being destroyed
        // before the preceding comparison.
        d_barrier.wait();
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

    bslma::TestAllocator testAlloc; bslma::TestAllocator *Z = &testAlloc;
    bslma::TestAllocator defaultAllocator;
    bslma::DefaultAllocatorGuard guard(&defaultAllocator);

    Category cat_A("A"); const Category *CAT_A = &cat_A;
    Category cat_B("B"); const Category *CAT_B = &cat_B;
    Category cat_C("C"); const Category *CAT_C = &cat_C;
    Category cat_D("D"); const Category *CAT_D = &cat_D;

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
// The following example demonstrates how to create and access a
// 'balm::MetricDescription' object.  We start by creating a category.
//..
    balm::Category myCategory("MyCategory");
//..
// Then we use that category to create three metric description objects with
// different names:
//..
    balm::MetricDescription metricA(&myCategory, "A");
    balm::MetricDescription metricB(&myCategory, "B");
    balm::MetricDescription metricC(&myCategory, "C");
//..
// We can use the 'category' and 'name' methods to access their value.
//..
    ASSERT(&myCategory == metricA.category());
    ASSERT(&myCategory == metricB.category());
    ASSERT(&myCategory == metricC.category());

    ASSERT(0 == bsl::strcmp("A", metricA.name()));
    ASSERT(0 == bsl::strcmp("B", metricB.name()));
    ASSERT(0 == bsl::strcmp("C", metricC.name()));
//..
// Finally, we write all three metric descriptions to the console.
//..
    bsl::cout << "metricA: " << metricA << bsl::endl
              << "metricB: " << metricB << bsl::endl
              << "metricC: " << metricC << bsl::endl;
//..
// The console output will look like:
//..
// metricA: MyCategory.A
// metricB: MyCategory.B
// metricC: MyCategory.C
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

        bslma::TestAllocator defaultAllocator;
        bslma::DefaultAllocatorGuard guard(&defaultAllocator);

        bslma::TestAllocator testAllocator;

        balm::Category c("category");
        Obj mX(&c, "metric", Z); const Obj& MX = mX;

        {
            ConcurrencyTest tester(10, &mX, &defaultAllocator);
            tester.runTest();
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        //  TESTING: 'setUserData' and 'userData'
        //
        // Concerns:
        //    That 'setUserData' sets the indicated key, and 'userData'
        //    returns the assigned value for the indicated key, or 0 if the
        //    key has not been assigned a value.
        //
        // Plan:
        //    Iterate through a series of possible keys, verify the associated
        //    value is 0 (by default).  Assign a value to a key > 0.  Verify
        //    the value for that key, and verify the value for all other keys
        //    is still 0.  Iterate through the a series of keys, assigning
        //    their values, and verify the expected values of all keys.
        //
        // Testing:
        //   void setUserData(UserDataKey key,  const void *value);
        //   const void *userData(UserDataKey key) const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'setUserData' and 'userData'."
                          << endl;

        balm::Category c("category");
        Obj mX(&c, "metric", Z); const Obj& X = mX;

        const int NUM_IDS = 10;

        for (int i = 0; i < NUM_IDS; ++i) {
            ASSERT(0 == X.userData(i));
        }

        mX.setUserData(0, 0);
        for (int i = 0; i < NUM_IDS; ++i) {
            ASSERT(0 == X.userData(i));
        }

        mX.setUserData(NUM_IDS - 1, 0);
        for (int i = 0; i < NUM_IDS; ++i) {
            ASSERT(0 == X.userData(i));
        }

        for (int i = NUM_IDS - 1; i >= 0; --i) {
            mX.setUserData(i, (const void *)1);
            for (int j = 0; j < NUM_IDS; ++j) {
                const void *EXP_VAL = j < i ? 0 : (const void *)1;
                ASSERT(EXP_VAL == X.userData(j));
            }
        }

        for (int i = 0; i < NUM_IDS; ++i) {
            mX.setUserData(i, (const void *)2);
            for (int j = 0; j < NUM_IDS; ++j) {
                const void *EXP_VAL = j <= i
                                      ? (const void *)2
                                      : (const void *)1;
                ASSERT(EXP_VAL == X.userData(j));
            }
        }

        ASSERT(0 == defaultAllocator.numBytesInUse());
      } break;
      case 6: {
        // --------------------------------------------------------------------
        //  TESTING: 'setFormat' and 'format'
        //
        // Concerns:
        //   That 'setFormat' sets the format of the supplied metric, and
        //   'format' returns that format.
        //
        // Plan:
        //   For a set 'S' of possible format shared pointers, call
        //   'setFormat' and verify that the format has been set using
        //   'format'.  Note that this tests the *address* of the format.
        //
        // Testing:
        //    void setFormat(bsl::shared_ptr<const balm::MetricFormat>&);
        //    bsl::shared_ptr<const balm::MetricFormat> format() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'setPreferredPublicationType'."
                          << endl;

        Format A(Z), B(Z), C(Z), D(Z), E(Z), F(Z);
        const Format *FORMATS[] = { &A, &B, &C, &D, &E, &F };
        const int NUM_FORMATS   = sizeof FORMATS / sizeof *FORMATS;

        balm::Category c("category");
        Obj mX(&c, "metric", Z); const Obj& MX = mX;
        ASSERT(!MX.format());
        for (int i = 0; i < NUM_FORMATS; ++i) {
            bsl::shared_ptr<const Format> fmt(FORMATS[i],
                                              bslstl::SharedPtrNilDeleter(),
                                              Z);
            mX.setFormat(fmt);
            ASSERT(fmt == MX.format());
        }

        ASSERT(0 == defaultAllocator.numBytesInUse());
      } break;
      case 5: {
        // --------------------------------------------------------------------
        //  TESTING: 'setPreferredPublicationType' and
        //           'preferredPublicationType'
        //
        // Concerns:
        //   That 'setPreferredPublicationType' sets the preferred publication
        //   type of the supplied metric, and 'preferredPublicationType'
        //   returns the publication type.
        //
        // Plan:
        //   Create a metric description, verity that publicationType() is
        //   initially UNSPECIFIED, then for a sequence of independent test
        //   values, set the preferred publication type and verify the set
        //   value.
        //
        // Testing:
        //   void setPreferredPublicationType(balm::PublicationType::Value );
        //   balm::PublicationType::Value preferredPublicationType() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'setPreferredPublicationType'."
                          << endl;

        Type::Value TYPES[] = { Type::e_TOTAL,
                                Type::e_COUNT,
                                Type::e_MIN,
                                Type::e_MAX,
                                Type::e_AVG,
                                Type::e_RATE,
                                Type::e_UNSPECIFIED
        };
        const int NUM_TYPES = sizeof TYPES / sizeof *TYPES;

        balm::Category c("category");
        Obj mX(&c, "metric", Z); const Obj& MX = mX;
        for (int j = 0; j < NUM_TYPES; ++j) {
            mX.setPreferredPublicationType(TYPES[j]);
            ASSERT(TYPES[j] == MX.preferredPublicationType());
        }

        ASSERT(0 == defaultAllocator.numBytesInUse());
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR:
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
        //   operator<<(ostream&, const balm::MetricDescription&);
        //   ostream& print(ostream&);
        //   ostream& printDescription(ostream& );
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting output operators" << endl;
        struct {
            const Category *d_category;
            const char     *d_name;
            const char     *d_expected;
        } DATA [] = {
            {   CAT_A, "MetricA", "A.MetricA" },
            {   CAT_A, "MetricB", "A.MetricB" },
            {   CAT_B, "MetricC", "B.MetricC" },
            {   CAT_B, "MetricD", "B.MetricD" },
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            Obj mX(DATA[i].d_category, DATA[i].d_name, Z); const Obj& MX = mX;

            bsl::ostringstream printBuf, operBuf;
            MX.print(printBuf);
            operBuf << MX;

            bsl::string EXP(DATA[i].d_expected);
            bsl::string printVal(printBuf.str());
            bsl::string operVal(operBuf.str());

            if (veryVerbose) {
                P_(printVal); P_(operVal); P(EXP);
            }
            LOOP_ASSERT(i, EXP == printVal);
            LOOP_ASSERT(i, EXP == operVal);
        }

        {
            Format fmt(Z);
            Obj mX(CAT_A, "metricA", Z); const Obj& MX = mX;
            mX.setPreferredPublicationType(Type::e_MIN);
            mX.setFormat(bsl::shared_ptr<const Format>(
                             &fmt, bslstl::SharedPtrNilDeleter(), Z));

            bsl::ostringstream buff;
            MX.printDescription(buff);
            bsl::string result(buff.str());

            const char *EXP = "[ A.metricA MIN format: [ ]  "
                              "user data: ( ) ]";

            if (veryVerbose) {
                P_(result); P(EXP);
            }
            ASSERT(EXP == result);
        }
        ASSERT(0 == defaultAllocator.numBytesInUse());
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING MANIPULATORS
        //
        // Concerns:
        //   The manipulators properly modify the object under test.
        //
        // Plan:
        //   Specify a set S of (unique) values with substantial and varied
        //   differences in value.  Construct a set of objects from a default
        //   value and then use the manipulator methods to set the value to a
        //   value in the set 'S'.
        //
        // Testing:
        // void setName(const char *);
        // void setCategory(const balm::Category *);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Manipulators" << endl;
        struct {
            const Category *d_category;
            const char     *d_name;
        } DATA [] = {
            { CAT_A,               "" },
            { CAT_A,              "A" },
            { CAT_A,              "B" },
            { CAT_A,              " " },
            { CAT_A,        "metricA" },
            { CAT_A,        "metricB" },
            { CAT_A, "my test metric" },
            { CAT_B,               "" },
            { CAT_B,              "A" },
            { CAT_B,              "B" },
            { CAT_B,              " " },
            { CAT_B,        "metricA" },
            { CAT_B,        "metricB" },
            { CAT_B, "my test metric" },
            { CAT_C,               "" },
            { CAT_C,              "A" },
            { CAT_C,              "B" },
            { CAT_C,              " " },
            { CAT_C,        "metricA" },
            { CAT_C,        "metricB" },
            { CAT_C, "my test metric" }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            for (int j = 0; j < NUM_DATA; ++j) {
                Obj u(DATA[i].d_category, DATA[i].d_name, Z); const Obj& U = u;
                Obj w(DATA[j].d_category, DATA[j].d_name, Z); const Obj& W = w;

                ASSERT(DATA[i].d_name     == U.name());
                ASSERT(DATA[i].d_category == U.category());

                // Set the value.
                u.setCategory(DATA[j].d_category);
                u.setName(DATA[j].d_name);

                ASSERT(DATA[j].d_category == U.category());
                ASSERT(DATA[j].d_name     == U.name());
                ASSERT(0 == defaultAllocator.numBytesInUse());
            }
        }
        ASSERT(0 == defaultAllocator.numBytesInUse());
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING BASIC CONSTRUCTOR and ACCESSORS (BOOTSTRAP):
        //
        // Concerns:
        //   The primary fields must be correctly modifiable and accessible.
        //
        // Plan:
        //   For a sequence of independent test values, use the to
        //   create an object with a particular value.  Verify that value
        //   using the basic accessors.  Note that the destructor is exercised
        //   on each configuration as the object being tested leaves scope.
        //
        // Testing:
        // balm::MetricDescription(const balm::Category *, const char *);
        // ~balm::MetricDescription();
        // const char *name() const;
        // const balm::Category *category() const;
        // --------------------------------------------------------------------
          if (veryVerbose)
              cout << "\tTest constructor and basic accessors (bootstrap)"
                   << bsl::endl;

          struct {
              const Category *d_category;
              const char     *d_name;
          } DATA [] = {
              { CAT_A,               "" },
              { CAT_A,              "A" },
              { CAT_A,              "B" },
              { CAT_A,              " " },
              { CAT_A,        "metricA" },
              { CAT_A,        "metricB" },
              { CAT_A, "my test metric" },
              { CAT_B,               "" },
              { CAT_B,              "A" },
              { CAT_B,              "B" },
              { CAT_B,              " " },
              { CAT_B,        "metricA" },
              { CAT_B,        "metricB" },
              { CAT_B, "my test metric" },
              { CAT_C,               "" },
              { CAT_C,              "A" },
              { CAT_C,              "B" },
              { CAT_C,              " " },
              { CAT_C,        "metricA" },
              { CAT_C,        "metricB" },
              { CAT_C, "my test metric" }
          };
          const int NUM_DATA = sizeof DATA / sizeof *DATA;

          for (int i = 0; i < NUM_DATA; ++i) {
              Obj mX(DATA[i].d_category, DATA[i].d_name, Z);
              const Obj& MX = mX;

              ASSERT(DATA[i].d_category  == MX.category());
              ASSERT(DATA[i].d_name      == MX.name());
              ASSERT(0 == defaultAllocator.numBytesInUse());
              ASSERT(0 == testAlloc.numBytesInUse());
              ASSERT(0 == defaultAllocator.numBytesInUse());
          }
          ASSERT(0 == defaultAllocator.numBytesInUse());
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Developers' Sandbox.
        //
        // Testing:
        //   This "test" *exercises* basic functionality, but *tests* nothing.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        Category cat_A("A"); const Category *CAT_A = &cat_A;
        Category cat_B("B"); const Category *CAT_B = &cat_B;
        Category cat_C("C"); const Category *CAT_C = &cat_C;
        const char *ID_A = "A";
        const char *ID_B = "B";
        const char *ID_C = "C";

        Obj mA(CAT_A, ID_A, Z); const Obj &MA = mA;
        Obj mB(CAT_B, ID_B, Z); const Obj &MB = mB;
        Obj mC(CAT_C, ID_C, Z); const Obj &MC = mC;

        ASSERT(CAT_A == MA.category());
        ASSERT(CAT_B == MB.category());
        ASSERT(CAT_C == MC.category());
        ASSERT(ID_A  == MA.name());
        ASSERT(ID_B  == MB.name());
        ASSERT(ID_C  == MC.name());
        ASSERT(Type::e_UNSPECIFIED == MA.preferredPublicationType());
        ASSERT(Type::e_UNSPECIFIED == MB.preferredPublicationType());
        ASSERT(Type::e_UNSPECIFIED == MC.preferredPublicationType());
        ASSERT(!MA.format());
        ASSERT(!MB.format());
        ASSERT(!MC.format());

        Obj mX(CAT_A, ID_A, Z);  const Obj& MX = mX;
        ASSERT(CAT_A == MX.category());
        ASSERT(ID_A  == MX.name());
        ASSERT(!MX.format());
        ASSERT(Type::e_UNSPECIFIED == MX.preferredPublicationType());

        mX.setName(ID_B);
        ASSERT(CAT_A == MX.category());
        ASSERT(ID_B  == MX.name());

        mX.setCategory(CAT_B);
        ASSERT(CAT_B == MX.category());
        ASSERT(ID_B == MX.name());

        balm::MetricFormat dummyFormat;
        bsl::shared_ptr<const balm::MetricFormat> formatSPtr(
                                    &dummyFormat,
                                    bslstl::SharedPtrNilDeleter(),
                                    Z);

        mX.setFormat(formatSPtr);
        ASSERT(MX.format());
        ASSERT(&dummyFormat == MX.format().get());

        mX.setPreferredPublicationType(Type::e_AVG);
        ASSERT(Type::e_AVG == MX.preferredPublicationType());

        // Test 'setUserData' and 'userData'.
        for (int i = 0; i < 3; ++i) {
            ASSERT(0 == MX.userData(i));
        }
        mX.setUserData(3, (const void *)1);
        ASSERT((const void *)1 == MX.userData(3));
        for (int i = 0; i < 2; ++i) {
            ASSERT(0 == MX.userData(i));
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
