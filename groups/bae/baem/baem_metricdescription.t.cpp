// baem_metricdescription.t.cpp  -*-C++-*-
#include <baem_metricdescription.h>

#include <baem_category.h>
#include <baem_metricformat.h>

#include <bcema_testallocator.h>
#include <bcemt_barrier.h>
#include <bcep_fixedthreadpool.h>
#include <bdef_bind.h>
#include <bslma_allocator.h>
#include <bslma_testallocator.h>
#include <bslma_defaultallocatorguard.h>

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
//                                 Overview
//                                 --------
// A 'bdema_MetricDescription' is a simple mechanism containing the address
// of a (non-modifiable) 'baem_Category' and the address of a null-terminated
// string.
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] baem_MetricDescription(const baem_Category *, const char *);
// [ 2] ~baem_MetricDescription();
// MANIPULATORS
// [ 3] void setName(const char *);
// [ 3] void setCategory(const baem_Category *)
// [ 5] void setPreferredPublicationType(baem_PublicationType::Value type);
// [ 6] void setFormat(const bcema_SharedPtr<const baem_MetricFormat>& );
// ACCESSORS
// [ 2] const char *name() const;
// [ 2] const baem_Category *category() const;
// [ 4] bsl::ostream& print(bsl::ostream& ) const;
// [ 5] baem_PublicationType::Value preferredPublicationType() const;
// [ 6] bcema_SharedPtr<const baem_MetricFormat> format() const;
// [ 7] void setUserData(UserDataKey key,  const void *value);
// [ 7] const void *userData(UserDataKey key) const;
// FREE OPERATORS
// [ 4] bsl::ostream& operator<<(bsl::ostream&, const baem_MetricDescription&);
//----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 8] CONCURRENCY TEST
// [ 9] USAGE EXAMPLE
//-----------------------------------------------------------------------------

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

typedef baem_Category           Category;
typedef baem_MetricDescription  Obj;
typedef baem_PublicationType    Type;
typedef baem_MetricFormat       Format;

//=============================================================================
//                  GLOBAL STUB CLASSES FOR TESTING
//-----------------------------------------------------------------------------

class ConcurrencyTest {
    // Invoke a set of operations operations synchronously.

    // DATA
    bcep_FixedThreadPool    d_pool;
    bcemt_Barrier           d_barrier;
    baem_MetricDescription *d_description_p;
    bslma_Allocator        *d_allocator_p;

    // PRIVATE MANIPULATORS
    void execute();
        // Execute a single test.

  public:

    // CREATORS
    ConcurrencyTest(int                      numThreads,
                    baem_MetricDescription  *description,
                    bslma_Allocator         *basicAllocator)
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

bcemt_Mutex m;
void ConcurrencyTest::execute()
{
    bslma_Allocator *Z = d_allocator_p;
    Obj *mX = d_description_p; const Obj *MX = mX;

    Format A(Z), B(Z), C(Z), D(Z), E(Z), F(Z);
    const Format *FORMATS[] = { &A, &B, &C, &D, &E, &F };
    const int NUM_FORMATS   = sizeof FORMATS / sizeof *FORMATS;

    Type::Value TYPES[] = { Type::BAEM_TOTAL,
                            Type::BAEM_COUNT,
                            Type::BAEM_MIN,
                            Type::BAEM_MAX,
                            Type::BAEM_AVG,
                            Type::BAEM_RATE,
                            Type::BAEM_UNSPECIFIED
    };
    const int NUM_TYPES = sizeof TYPES / sizeof *TYPES;

    F.setFormatSpec(Type::BAEM_MIN, baem_MetricFormatSpec(2, "%f"));

    ASSERT(!MX->format());

    for (int iteration = 0; iteration < 20; ++iteration) {
        d_barrier.wait();

        for (int j = 0; j < NUM_TYPES; ++j) {
            mX->setPreferredPublicationType(TYPES[j]);
            int x = (int)MX->preferredPublicationType();
            ASSERT(0 <= x && x < Type::BAEM_LENGTH);
        }

        for (int i = 0; i < NUM_FORMATS; ++i) {
            bcema_SharedPtr<const Format> fmt(FORMATS[i],
                                              bcema_SharedPtrNilDeleter(),
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

    bslma_TestAllocator testAlloc; bslma_TestAllocator *Z = &testAlloc;
    bslma_TestAllocator defaultAllocator;
    bslma_DefaultAllocatorGuard guard(&defaultAllocator);

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
// 'baem_MetricDescription' object.  We start by creating a category.  Then we
// use that category to create three metric description objects with different
// names.
//..
    baem_Category myCategory("MyCategory");
    baem_MetricDescription metricA(&myCategory, "A");
    baem_MetricDescription metricB(&myCategory, "B");
    baem_MetricDescription metricC(&myCategory, "C");
//..
// Once the metricdescription object have been created we can use the
// 'category' and 'name' methods to access their value.
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

        bcema_TestAllocator defaultAllocator;
        bslma_DefaultAllocatorGuard guard(&defaultAllocator);

        bcema_TestAllocator testAllocator;

        baem_Category c("category");
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

        baem_Category c("category");
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
        //    void setFormat(const bcema_SharedPtr<const baem_MetricFormat>& );
        //    bcema_SharedPtr<const baem_MetricFormat> format() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'setPreferredPublicationType'."
                          << endl;

        Format A(Z), B(Z), C(Z), D(Z), E(Z), F(Z);
        const Format *FORMATS[] = { &A, &B, &C, &D, &E, &F };
        const int NUM_FORMATS   = sizeof FORMATS / sizeof *FORMATS;

        baem_Category c("category");
        Obj mX(&c, "metric", Z); const Obj& MX = mX;
        ASSERT(!MX.format());
        for (int i = 0; i < NUM_FORMATS; ++i) {
            bcema_SharedPtr<const Format> fmt(FORMATS[i],
                                              bcema_SharedPtrNilDeleter(),
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
        //   void setPreferredPublicationType(baem_PublicationType::Value );
        //   baem_PublicationType::Value preferredPublicationType() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'setPreferredPublicationType'."
                          << endl;

        Type::Value TYPES[] = { Type::BAEM_TOTAL,
                                Type::BAEM_COUNT,
                                Type::BAEM_MIN,
                                Type::BAEM_MAX,
                                Type::BAEM_AVG,
                                Type::BAEM_RATE,
                                Type::BAEM_UNSPECIFIED
        };
        const int NUM_TYPES = sizeof TYPES / sizeof *TYPES;

        baem_Category c("category");
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
        //   operator<<(ostream&, const baem_MetricDescription&);
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
            mX.setPreferredPublicationType(Type::BAEM_MIN);
            mX.setFormat(bcema_SharedPtr<const Format>(
                             &fmt, bcema_SharedPtrNilDeleter(), Z));

            bsl::ostringstream buff;
            MX.printDescription(buff);
            bsl::string result(buff.str());

            const char *EXP = "[ A.metricA BAEM_MIN format: [ ]  "
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
        // void setCategory(const baem_Category *);
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
        // baem_MetricDescription(const baem_Category *, const char *);
        // ~baem_MetricDescription();
        // const char *name() const;
        // const baem_Category *category() const;
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
        ASSERT(Type::BAEM_UNSPECIFIED == MA.preferredPublicationType());
        ASSERT(Type::BAEM_UNSPECIFIED == MB.preferredPublicationType());
        ASSERT(Type::BAEM_UNSPECIFIED == MC.preferredPublicationType());
        ASSERT(!MA.format());
        ASSERT(!MB.format());
        ASSERT(!MC.format());

        Obj mX(CAT_A, ID_A, Z);  const Obj& MX = mX;
        ASSERT(CAT_A == MX.category());
        ASSERT(ID_A  == MX.name());
        ASSERT(!MX.format());
        ASSERT(Type::BAEM_UNSPECIFIED == MX.preferredPublicationType());

        mX.setName(ID_B);
        ASSERT(CAT_A == MX.category());
        ASSERT(ID_B  == MX.name());

        mX.setCategory(CAT_B);
        ASSERT(CAT_B == MX.category());
        ASSERT(ID_B == MX.name());

        baem_MetricFormat dummyFormat;
        bcema_SharedPtr<const baem_MetricFormat> formatSPtr(
                                    &dummyFormat,
                                    bcema_SharedPtrNilDeleter(),
                                    Z);

        mX.setFormat(formatSPtr);
        ASSERT(MX.format());
        ASSERT(&dummyFormat == MX.format().ptr());

        mX.setPreferredPublicationType(Type::BAEM_AVG);
        ASSERT(Type::BAEM_AVG == MX.preferredPublicationType());

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
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
