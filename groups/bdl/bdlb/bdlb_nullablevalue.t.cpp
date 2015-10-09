// bdlb_nullablevalue.t.cpp                                           -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlb_nullablevalue.h>

#include <bslim_testutil.h>

#include <bslalg_constructorproxy.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>
#include <bslma_usesbslmaallocator.h>

#include <bsls_asserttest.h>

#include <bsltf_templatetestfacility.h>
#include <bsltf_testvaluesarray.h>

#include <bslx_testinstream.h>
#include <bslx_testoutstream.h>

#include <bsl_cstdlib.h>    // 'atoi'
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// This component implements a wrapper for a value-semantic type, and itself
// exhibits value-semantic properties.
//
// Global Concerns:
//   o No memory is allocated from the global-allocator.
//-----------------------------------------------------------------------------
// TYPEDEFS
// [ 3] typedef TYPE ValueType;
//
// CREATORS
// [ 3] bdlb::NullableValue();
// [ 3] bdlb::NullableValue(bslma::Allocator *basicAllocator);
// [ 6] bdlb::NullableValue(const bdlb::NullableValue& original);
// [ 6] bdlb::NullableValue(const bdlb::NullableValue& original, *ba);
// [ 9] bdlb::NullableValue(const TYPE& value);
// [ 9] bdlb::NullableValue(const TYPE& value, *ba);
// [11] bdlb::NullableValue(const bdlb::NullableValue<ORIGINAL>&o);
// [11] bdlb::NullableValue(const bdlb::NullableValue<ORIGINAL>&o,*ba);
// [ 3] ~bdlb::NullableValue();
//
// MANIPULATORS
// [ 7] operator=(const bdlb::NullableValue& rhs);
// [12] operator=(const bdlb::NullableValue<OTHER_TYPE>& rhs);
// [10] operator=(const TYPE& rhs);
// [12] operator=(const OTHER_TYPE& rhs);
// [13] void swap(bdlb::NullableValue<TYPE>& other);
// [ 3] TYPE& makeValue(const TYPE& value);
// [12] TYPE& makeValue(const OTHER_TYPE& value);
// [10] TYPE& makeValue();
// [ 8] STREAM& bdexStreamIn(STREAM& stream, int version);
// [10] void reset();
// [10] TYPE& value();
// [14] TYPE valueOr(const TYPE& ) const;
// [15] const TYPE *valueOr(const TYPE *) const;
// [16] const TYPE *valueOrNull() const;
//
// ACCESSORS
// [ 8] STREAM& bdexStreamOut(STREAM& stream, int version) const;
// [ 3] bool isNull() const;
// [ 8] int maxSupportedBdexVersion(int) const;
// [ 4] print(bsl::ostream& s,int l=0,int spl=4) const;
// [ 3] const TYPE& value() const;
//
// FREE OPERATORS
// [ 5] operator==(const bdlb::NullableValue<LHS_TYPE>&,<RHS_TYPE>&);
// [ 5] operator!=(const bdlb::NullableValue<LHS_TYPE>&,<RHS_TYPE>&);
// [17] operator==(const bdlb::NullableValue<TYPE>&,const TYPE&);
// [17] operator==(const TYPE&,const bdlb::NullableValue<TYPE>&,);
// [17] operator!=(const bdlb::NullableValue<TYPE>&,const TYPE&);
// [17] operator!=(const TYPE&,const bdlb::NullableValue<TYPE>&,);
// [ 4] operator<<(bsl::ostream&,const bdlb::NullableValue<TYPE>&);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST 1: Using 'bsl::string'
// [ 2] BREATHING TEST 2: Using 'int'
// [19] USAGE EXAMPLE
// ----------------------------------------------------------------------------

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

// ============================================================================
//                       GLOBAL TEST VALUES
// ----------------------------------------------------------------------------

static bool         verbose;
static bool     veryVerbose;
static bool veryVeryVerbose;
static bool veryVeryVeryVerbose;

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

enum MessageType {
    // Type used for testing 'makeValue' in case 12.

    JUNK,
    IMPORTANT
};

class Recipient {
    // This 'class' is used for testing 'makeValue' in case 12.  The 'class'
    // has an explicit conversion constructor such that if the implementation
    // of 'makeValue' relies on implicit conversion from 'OTHER_TYPE' to
    // 'TYPE', the code will not compile.

    // DATA
    MessageType d_msgType;

    // FRIENDS
    friend bool operator==(const Recipient&, const Recipient&);

  public:
    // CREATORS
    explicit Recipient(const MessageType& msgType)
        // Create a 'Recipient' object using the specified 'msgType'.
    : d_msgType(msgType)
    {
    }

    // MANIPULATORS
    Recipient& operator=(const MessageType& rhs)
    {
        d_msgType = rhs;

        return *this;
    }

    // ACCESSORS
    MessageType getMsgType() const
    {
        return d_msgType;
    }
};

// FREE OPERATORS
bool operator==(const Recipient& lhs, const Recipient& rhs)
{
    return lhs.d_msgType == rhs.d_msgType;
}

struct Swappable {

    // PUBLIC CLASS DATA
    static int s_swapCalled;

    // PUBLIC DATA
    int d_value;

    // CLASS METHODS
    static void swapReset()
    {
        s_swapCalled = 0;
    }

    static bool swapCalled()
    {
        return 0 != s_swapCalled;
    }

    // CREATORS
    Swappable(int v)
    : d_value(v)
    {
    }
};

// FREE OPERATORS
bool operator==(const Swappable& lhs, const Swappable& rhs)
{
    return lhs.d_value == rhs.d_value;
}

// PUBLIC CLASS DATA
int Swappable::s_swapCalled = 0;

void swap(Swappable& a, Swappable& b)
{
    ++Swappable::s_swapCalled;

    bsl::swap(a.d_value, b.d_value);
}

#define RUN_EACH_TYPE BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE
#define TEST_TYPES                                                            \
        signed char,                                                          \
        size_t,                                                               \
        bsltf::TemplateTestFacility::ObjectPtr,                               \
        bsltf::TemplateTestFacility::FunctionPtr,                             \
        bsltf::TemplateTestFacility::MethodPtr,                               \
        bsltf::EnumeratedTestType::Enum,                                      \
        bsltf::UnionTestType,                                                 \
        bsltf::SimpleTestType,                                                \
        bsltf::AllocTestType,                                                 \
        bsltf::BitwiseMoveableTestType,                                       \
        bsltf::AllocBitwiseMoveableTestType
    // This list of test types is a combination of
    // BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE and *_USER_DEFINED,
    // minus 'NonTypicalOverloadsTestType' which does not work with
    // 'bslalg::ConstructorProxy'.

template <class TEST_TYPE>
class TestDriver {
    // This template class provides a namespace for testing
    // 'bdlb::NullableValue'.  The template parameter 'TEST_TYPE' specifies the
    // type contained in the nullable object.

  private:
    // PRIVATE TYPES
    typedef bdlb::NullableValue<TEST_TYPE> Obj;
        // Type under test.

    typedef bslalg::ConstructorProxy<Obj> ObjWithAllocator;
        // Wrapper for 'Obj' whose constructor takes an allocator.

    typedef bsltf::TestValuesArray<TEST_TYPE> TestValues;
        // Array of test values of 'TEST_TYPE'.

  public:
    static void testCase14();
        // Test 'T valueOr(const T&)'.

    static void testCase15();
        // Test 'const T *valueOr(const T *)'.

    static void testCase16();
        // Test 'valueOrNull'.

    static void testCase17();
        // Test comparisons with the contained 'TYPE'.
};

template <class TEST_TYPE>
void TestDriver<TEST_TYPE>::testCase14()
{
    // ------------------------------------------------------------------------
    // TESTING: 'T valueOr(const T&)'
    //
    // Concerns:
    //: 1 'valueOr' returns the supplied value if the nullable value is null.
    //:
    //: 2 'valueOr' returns the contained value if the nullable value is
    //:   non-null.
    //:
    //: 3 'valueOr' returns by value.
    //:
    //: 4 'valueOr' can be called on a 'const' object.
    //
    // Plan:
    //: 1 Create a member-function pointer matching the expected signature, and
    //:   assign 'valueOr' to the function.  (C-3)
    //:
    //: 2 Call 'valueOr' for a null nullable value and verify that it returns
    //:   the supplied value.  (C-2)
    //:
    //: 3 For a series of test values, assign the nullable value to the test
    //:   value, call 'valueOr', and verify the return value is the test value.
    //:   (C-2, C-4)
    //
    // Testing:
    //   TYPE valueOr(const TYPE&) const;
    // ------------------------------------------------------------------------

    const TestValues VALUES;
    const int        NUM_VALUES = static_cast<int>(VALUES.size());

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::TestAllocator oa("object", veryVeryVeryVerbose);

    bslma::DefaultAllocatorGuard dag(&da);

    if (veryVerbose) {
        cout << "\tCompile-time verify the function returns by value\n";
    }
    {
        typedef TEST_TYPE (Obj::*MemberFunction)(const TEST_TYPE& type) const;
        MemberFunction memberFunction = &Obj::valueOr;
        (void)&memberFunction;
    }

    if (veryVerbose) {
        cout << "\tVerify null nullable values return 0\n";
    }
    {
        ObjWithAllocator object(&oa);
        Obj& x = object.object(); const Obj& X = x;

        ASSERT(0 == oa.numBlocksInUse());
        ASSERT(0 == da.numBlocksInUse());

        for (int i = 0; i < NUM_VALUES; ++i) {

            ASSERT(VALUES[i] == x.valueOr(VALUES[i]));
            ASSERT(VALUES[i] == X.valueOr(VALUES[i]));

            ASSERT(true == X.isNull());

            ASSERT(0 == oa.numBlocksInUse());
        }
    }

    if (veryVerbose) {
        cout << "\tVerify non-null nullable values return underlying value\n";
    }
    {
        ObjWithAllocator object(&oa);
        Obj& x = object.object(); const Obj& X = x;

        for (int i = 0; i < NUM_VALUES; ++i) {
            ASSERT(0 == oa.numBlocksInUse());
            ASSERT(0 == da.numBlocksInUse());

            ASSERT(VALUES[i] == x.valueOr(VALUES[i]));
            ASSERT(VALUES[i] == X.valueOr(VALUES[i]));

            x = VALUES[0];

            bslma::TestAllocatorMonitor oam(&oa);
            bslma::TestAllocatorMonitor dam(&da);

            ASSERT(VALUES[0] == x.valueOr(VALUES[i]));
            ASSERT(VALUES[0] == X.valueOr(VALUES[i]));

            ASSERT(i == 0 || VALUES[i] != x.valueOr(VALUES[i]));
            ASSERT(i == 0 || VALUES[i] != X.valueOr(VALUES[i]));

            bool usesAllocator = bslma::UsesBslmaAllocator<TEST_TYPE>::value;
            ASSERT(usesAllocator ? dam.isTotalUp() : dam.isTotalSame());
            ASSERT(oam.isInUseSame());

            x.reset();
        }

        ASSERT(0 == oa.numBlocksInUse());
        ASSERT(0 == da.numBlocksInUse());

    }
}

template <class TEST_TYPE>
void TestDriver<TEST_TYPE>::testCase15()
{
    // ------------------------------------------------------------------------
    // TESTING: 'const T *valueOr(const T *)'
    //
    // Concerns:
    //: 1 'valueOr' returns the supplied value if the nullable value is null.
    //:
    //: 2 'valueOr' returns the contained value if the nullable value is
    //:   non-null.
    //:
    //: 3 'valueOr' returns an address.
    //:
    //: 4 'valueOr' can be called on a 'const' object.
    //:
    //: 5 No memory is requested of any allocator (global, default, this
    //:   object, supplied object).
    //
    // Plan:
    //: 1 Create a member-function pointer matching the expected signature,
    //:   and assign 'valueOr' to the function.  (C-3)
    //:
    //: 2 Call 'valueOr' for a null nullable value and verify that it
    //:   returns a reference to the supplied value.  (C-2)
    //:
    //: 3 For a series of test values, assign the nullable value to the test
    //:   value, call 'valueOr', and verify the return value is a reference to
    //:   the contained value.  (C-2, C-4)
    //
    // Testing:
    //   const TYPE *valueOr(const TYPE *) const;
    // ------------------------------------------------------------------------

    const TestValues VALUES;
    const int        NUM_VALUES = static_cast<int>(VALUES.size());

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::TestAllocator oa("object", veryVeryVeryVerbose);

    bslma::DefaultAllocatorGuard dag(&da);

    if (veryVerbose) {
        cout << "\tCompile-time verify the function returns an address\n";
    }
    {
        typedef const TEST_TYPE *
                              (Obj::*MemberFunction)(const TEST_TYPE *) const;
        MemberFunction memberFunction = &Obj::valueOr;
        (void)&memberFunction;
    }

    if (veryVerbose) {
        cout << "\tVerify null nullable values return 0\n";
    }
    {
        ObjWithAllocator object(&oa);
        Obj& x = object.object(); const Obj& X = x;

        ASSERT(0 == oa.numBlocksInUse());
        ASSERT(0 == da.numBlocksInUse());

        for (int i = 0; i < NUM_VALUES; ++i) {

            ASSERT(VALUES[i] == *x.valueOr(&VALUES[i]));
            ASSERT(VALUES[i] == *X.valueOr(&VALUES[i]));

            ASSERT(&VALUES[i] == x.valueOr(&VALUES[i]));
            ASSERT(&VALUES[i] == X.valueOr(&VALUES[i]));

            ASSERT(true == X.isNull());

            ASSERT(0 == oa.numBlocksInUse());
        }
    }

    if (veryVerbose) {
        cout << "\tVerify non-null nullable values return underlying value\n";
    }
    {
        ObjWithAllocator object(&oa);
        Obj& x = object.object(); const Obj& X = x;

        for (int i = 0; i < NUM_VALUES; ++i) {
            ASSERT(0 == oa.numBlocksInUse());
            ASSERT(0 == da.numBlocksInUse());

            ASSERT(VALUES[i] == *x.valueOr(&VALUES[i]));
            ASSERT(VALUES[i] == *X.valueOr(&VALUES[i]));

            ASSERT(&VALUES[i] == x.valueOr(&VALUES[i]));
            ASSERT(&VALUES[i] == X.valueOr(&VALUES[i]));

            x = VALUES[0];

            bslma::TestAllocatorMonitor oam(&oa);

            ASSERT(VALUES[0] == *x.valueOr(&VALUES[i]));
            ASSERT(VALUES[0] == *X.valueOr(&VALUES[i]));

            ASSERT(i == 0 || VALUES[i] != *x.valueOr(&VALUES[i]));
            ASSERT(i == 0 || VALUES[i] != *X.valueOr(&VALUES[i]));

            ASSERT(&VALUES[i] != x.valueOr(&VALUES[i]));
            ASSERT(&VALUES[i] != X.valueOr(&VALUES[i]));

            ASSERT(&X.value() == x.valueOr(&VALUES[i]));
            ASSERT(&X.value() == X.valueOr(&VALUES[i]));


            ASSERT(oam.isInUseSame());
            ASSERT(0 == da.numBlocksInUse());

            x.reset();
        }

        ASSERT(0 == oa.numBlocksInUse());
        ASSERT(0 == da.numBlocksInUse());

    }
}

template <class TEST_TYPE>
void TestDriver<TEST_TYPE>::testCase16()
{
    // ------------------------------------------------------------------------
    // TESTING: 'valueOrNull'
    //
    // Concerns:
    //: 1 'valueOrNull' returns 0 if the nullable value is null.
    //:
    //: 2 'valueOrNull' returns the address of the non-modifiable value if the
    //:   nullable value is non-null.
    //:
    //: 3 The returned address, if not 0, remains valid until the nullable
    //:   value is destroyed.
    //:
    //: 4 No memory allocation is performed.
    //:
    //: 5 'valueOrNull' can be called on a 'const' object.
    //
    // Plan:
    //: 1 Call 'valueOrNull' for a null nullable value and verify that it
    //:   returns 0.  (C-1)
    //:
    //: 2 For a series of test values, assign the nullable value to the test
    //:   value, call 'valueOrNull', and verify the return value.  (C-2..5)
    //
    // Testing:
    //   const TYPE *valueOrNull() const;
    // ------------------------------------------------------------------------

    const TestValues VALUES;
    const int        NUM_VALUES = static_cast<int>(VALUES.size());

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::TestAllocator oa("object", veryVeryVeryVerbose);

    bslma::DefaultAllocatorGuard dag(&da);
    if (veryVerbose) {
        cout << "\tVerify null nullable values return 0\n";
    }
    {
        ObjWithAllocator object(&oa);
        Obj& x = object.object(); const Obj& X = x;

        ASSERT(0 == x.valueOrNull());
        ASSERT(0 == X.valueOrNull());

        ASSERT(0 == oa.numBlocksInUse());
        ASSERT(0 == da.numBlocksInUse());

        x = VALUES[0];

        ASSERT(0 != x.valueOrNull());
        ASSERT(0 != X.valueOrNull());

        x.reset();

        ASSERT(0 == x.valueOrNull());
        ASSERT(0 == X.valueOrNull());

        ASSERT(0 == oa.numBlocksInUse());
        ASSERT(0 == da.numBlocksInUse());
    }

    if (veryVerbose) {
        cout << "\tVerify non-null nullable values return underlying value\n";
    }
    {
        ObjWithAllocator object(&oa);
        Obj& x = object.object(); const Obj& X = x;

        for (int i = 0; i < NUM_VALUES; ++i) {
            ASSERT(0 == x.valueOrNull());
            ASSERT(0 == X.valueOrNull());

            ASSERT(0 == oa.numBlocksInUse());
            ASSERT(0 == da.numBlocksInUse());

            x = VALUES[i];

            ASSERT(0 != x.valueOrNull());
            ASSERT(0 != X.valueOrNull());

            bslma::TestAllocatorMonitor oam(&oa);

            ASSERT(X.valueOrNull() == x.valueOrNull());
            ASSERT(&X.value()      == X.valueOrNull());

            const TEST_TYPE *valuePtr = X.valueOrNull();

            ASSERT(VALUES[i] == *valuePtr);

            ASSERT(oam.isInUseSame());
            ASSERT(0 == da.numBlocksInUse());

            x.reset();
        }
    }
}

template <class TEST_TYPE>
void TestDriver<TEST_TYPE>::testCase17()
{
    // ------------------------------------------------------------------------
    // TESTING: Comparison with the contained 'TYPE'
    //
    // Concerns:
    //: 1 Comparing a value with a null value always returns that the values
    //:   are not the same.
    //:
    //: 2 Comparing a value with a nullable value having the same value returns
    //:   that the values are the same.
    //:
    //: 3 Comparing a value with a nullable value having a different value
    //:   returns that the values are not the same.
    //:
    //: 4 Both operators can be called for 'const' objects.
    //:
    //: 5 No memory is allocated.
    //
    // Plan:
    //: 1 Create a null nullable value and verify it does not compare equal
    //:   (using the 4 different operator variants) to any non-null test value.
    //:   (C-1)
    //:
    //: 2 Create a non-null nullable value for a series of test values and
    //:   verify it compares equal (using the 4 different operator variants)
    //:   only to the same test value.  (C-2..3)
    //
    // Testing:
    //   bool operator==(const bdlb::NullableValue<TYPE>&, const TYPE&);
    //   bool operator==(const TYPE&, const bdlb::NullableValue<TYPE>&);
    //   bool operator!=(const bdlb::NullableValue<TYPE>&, const TYPE&);
    //   bool operator!=(const TYPE&, const bdlb::NullableValue<TYPE>&);
    // ------------------------------------------------------------------------

    const TestValues VALUES;
    const int        NUM_VALUES = static_cast<int>(VALUES.size());

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::TestAllocator oa("object", veryVeryVeryVerbose);

    bslma::DefaultAllocatorGuard dag(&da);

    if (veryVerbose) {
        cout << "\tVerify null nullable values are different from any value\n";
    }
    {
        ObjWithAllocator object(&oa);
        Obj& x = object.object(); const Obj& X = x;

        ASSERT(0 == oa.numBlocksInUse());
        ASSERT(0 == da.numBlocksInUse());

        for (int i = 0; i < NUM_VALUES; ++i) {

            ASSERT(!(VALUES[i] == X));
            ASSERT( (VALUES[i] != X));

            ASSERT(!(X == VALUES[i]));
            ASSERT( (X != VALUES[i]));
        }
    }

    if (veryVerbose) {
        cout << "\tVerify non-null nullable values compare equal "
             << "to a value of the contained type\n";
    }
    {
        ObjWithAllocator object(&oa);
        Obj& x = object.object(); const Obj& X = x;

        for (int i = 0; i < NUM_VALUES; ++i) {
            for (int j = 0; j < NUM_VALUES; ++j) {

                ASSERT(0 == oa.numBlocksInUse());
                ASSERT(0 == da.numBlocksInUse());

                x = VALUES[i];

                bslma::TestAllocatorMonitor oam(&oa);

                bool areSame = i == j;

                ASSERT( areSame == (VALUES[j] == X));
                ASSERT(!areSame == (VALUES[j] != X));

                ASSERT( areSame == (X == VALUES[j]));
                ASSERT(!areSame == (X != VALUES[j]));

                ASSERT(oam.isInUseSame());
                ASSERT(0 == da.numBlocksInUse());

                x.reset();
            }
        }
        ASSERT(0 == oa.numBlocksInUse());
        ASSERT(0 == da.numBlocksInUse());
    }
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;

    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bslma::TestAllocator  testAllocator(veryVeryVeryVerbose);
    bslma::TestAllocator *ALLOC = &testAllocator;
    bslma::TestAllocator globalAllocator(veryVeryVeryVerbose);

    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:  // Zero is always the leading case.
      case 19: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

///Usage
///-----
// The following snippets of code illustrate use of this component:
//
// First, create a nullable 'int' object:
//..
    bdlb::NullableValue<int> nullableInt;
    ASSERT( nullableInt.isNull());
//..
// Next, give the 'int' object the value 123 (making it non-null):
//..
    nullableInt.makeValue(123);
    ASSERT(!nullableInt.isNull());
    ASSERT(123 == nullableInt.value());
//..
// Finally, reset the object to its default constructed state (i.e., null):
//..
    nullableInt.reset();
    ASSERT( nullableInt.isNull());
//..

      } break;
      case 18: {
        // --------------------------------------------------------------------
        // NEGATIVE TESTING OF ASSERT ON INVALID USE OF NULL VALUE
        // --------------------------------------------------------------------

        typedef int                            ValueType;
        typedef bdlb::NullableValue<ValueType> Obj;

        Obj mX1;  const Obj& X1 = mX1;

        bsls::AssertTestHandlerGuard guard;

        ASSERT(X1.isNull());

        ASSERT_SAFE_FAIL(0 == X1.value());

        mX1 = 5;

        ASSERT_SAFE_PASS(5 == X1.value());
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // Comparison with the contained 'TYPE'
        // --------------------------------------------------------------------

          if (verbose) cout << "\nTesting: Comparison with contained 'TYPE'"
                            << "\n=========================================\n";

        RUN_EACH_TYPE(TestDriver, testCase17, TEST_TYPES);

      } break;
      case 16: {
        // --------------------------------------------------------------------
        // valueOrNull
        // --------------------------------------------------------------------

          if (verbose) cout << "\nTesting: valueOrNull"
                            << "\n====================\n";

        RUN_EACH_TYPE(TestDriver, testCase16, TEST_TYPES);

      } break;
      case 15: {
        // --------------------------------------------------------------------
        // valueOr
        // --------------------------------------------------------------------

          if (verbose) cout << "\nTesting: const T *valueOr(const T *)"
                            << "\n====================================\n";

          RUN_EACH_TYPE(TestDriver, testCase15, TEST_TYPES);

      } break;
      case 14: {
        // --------------------------------------------------------------------
        // valueOr
        // --------------------------------------------------------------------

          if (verbose) cout << "\nTesting: T valueOr(const T&)"
                            << "\n============================\n";

        RUN_EACH_TYPE(TestDriver, testCase14, TEST_TYPES);

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING SWAP METHOD
        //
        // Concerns:
        //   1. Swap of two null objects is a no-op,
        //   2. Swap of null and non-null moves the value from one object to
        //      another without calling swap for the value type,
        //   3. Swap of two non-null objects calls swap for the value type.
        //
        // Plan:
        //   Create a value type class, 'Swappable', with a swap method
        //   instrumented to track swap calls.  Instantiate
        //   'bdlb::NullableValue' with that type and execute operations needed
        //   to verify the concerns.
        //
        // Testing:
        //   void swap(bdlb::NullableValue<TYPE>& other);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Swap Method"
                          << "\n==================="
                          << endl;

        using bsl::swap;
        {
            Swappable obj1(1);
            Swappable obj2(2);
            const Swappable Zobj1(obj1);
            const Swappable Zobj2(obj2);

            ASSERT(obj1 == Zobj1);
            ASSERT(obj2 == Zobj2);

            ASSERT(!Swappable::swapCalled());
            swap(obj1, obj2);
            ASSERT( Swappable::swapCalled());

            ASSERT(obj2 == Zobj1);
            ASSERT(obj1 == Zobj2);
        }

        {
            // Swap of two null objects is a no-op.

            bdlb::NullableValue<Swappable> nullObj1;
            bdlb::NullableValue<Swappable> nullObj2;

            Swappable::swapReset();
            swap(nullObj1, nullObj2);

            ASSERT(!Swappable::swapCalled());
            ASSERT(nullObj1.isNull());
            ASSERT(nullObj2.isNull());
        }

        {
            // Swap of null and non-null moves the value from one object to
            // the other without calling swap for the value type.

            bdlb::NullableValue<Swappable> nonNullObj(Swappable(10));
            bdlb::NullableValue<Swappable> nonNullObjCopy(nonNullObj);
            bdlb::NullableValue<Swappable> nullObj;

            Swappable::swapReset();
            swap(nonNullObj, nullObj);

            ASSERT(!Swappable::swapCalled());
            ASSERT(nonNullObjCopy == nullObj);
            ASSERT(nonNullObj.isNull());
        }

        {
            // Swap of two non-null objects calls swap for the value type.

            bdlb::NullableValue<Swappable> obj1(Swappable(10));
            bdlb::NullableValue<Swappable> obj2(Swappable(20));

            bdlb::NullableValue<Swappable> obj1Copy(obj1);
            bdlb::NullableValue<Swappable> obj2Copy(obj2);

            Swappable::swapReset();
            swap(obj1, obj2);

            ASSERT(Swappable::swapCalled());
            ASSERT(obj1 == obj2Copy);
            ASSERT(obj2 == obj1Copy);
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING CONVERSION ASSIGNMENT OPERATIONS
        //
        // Concerns:
        //   - That convertible underlying types convert.
        //   - That types with an explicit conversion constructor will properly
        //     compile and work with 'bdlb::NullableValue'.
        //   - That types for which there is no conversion do not compile
        //     (we will do this test by hand for now, but could use template
        //     magic later, perhaps, to ensure that non-compile is enforced
        //     by the compiler).
        //
        // Plan:
        //   Conduct the regular test using 'int' and 'double'.  Then try
        //   'bsl::string' and 'char *' to observe with allocators involved.
        //   Then try 'Recipient' and 'Message' to test for explicit conversion
        //   constructors.  Finally, try 'int' and 'bsl::string' by hand and
        //   observe that it fails to compile.
        //
        // Testing:
        //   operator=(const bdlb::NullableValue<OTHER_TYPE>& rhs);
        //   operator=(const OTHER_TYPE& rhs);
        //   TYPE& makeValue(const OTHER_TYPE& value);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Conversion Assignment Operations"
                          << "\n========================================"
                          << endl;

        if (verbose) cout << "\nUsing 'int' and 'double." << endl;

        {
            typedef int    ValueType1;
            typedef double ValueType2;

            typedef bdlb::NullableValue<ValueType1> ObjType1;
            typedef bdlb::NullableValue<ValueType2> ObjType2;

            const ValueType1 VALUE1 = 123;
            const ValueType2 VALUE2 = 456;

            if (verbose) cout << "\tcopy assignment" << endl;

            const ObjType1 OBJ1(VALUE1);
                  ObjType2 obj2(VALUE2);

            ASSERT(VALUE1 == OBJ1.value());
            ASSERT(VALUE2 == obj2.value());

            obj2 = OBJ1;

            ASSERT(VALUE1 == OBJ1.value());
            ASSERT(VALUE1 == obj2.value());

            if (verbose) cout << "\tvalue assignment" << endl;

            obj2 = VALUE2;

            ASSERT(VALUE1 == OBJ1.value());
            ASSERT(VALUE2 == obj2.value());

            obj2 = VALUE1;

            ASSERT(VALUE1 == OBJ1.value());
            ASSERT(VALUE1 == obj2.value());

            if (verbose) cout << "\tmake value" << endl;

            obj2 = VALUE2;

            ASSERT(VALUE1 == OBJ1.value());
            ASSERT(VALUE2 == obj2.value());

            obj2.makeValue(VALUE1);

            ASSERT(VALUE1 == OBJ1.value());
            ASSERT(VALUE1 == obj2.value());
        }

        if (verbose) cout << "\nUsing 'double' and 'int." << endl;

        {
            typedef double ValueType1;
            typedef int    ValueType2;

            typedef bdlb::NullableValue<ValueType1> ObjType1;
            typedef bdlb::NullableValue<ValueType2> ObjType2;

            const ValueType1 VALUE1 = 123;
            const ValueType2 VALUE2 = 456;

            if (verbose) cout << "\tcopy assignment" << endl;

            const ObjType1 OBJ1(VALUE1);
                  ObjType2 obj2(VALUE2);

            ASSERT(VALUE1 == OBJ1.value());
            ASSERT(VALUE2 == obj2.value());

            obj2 = OBJ1;

            ASSERT(VALUE1 == OBJ1.value());
            ASSERT(VALUE1 == obj2.value());

            if (verbose) cout << "\tvalue assignment" << endl;

            obj2 = VALUE2;

            ASSERT(VALUE1 == OBJ1.value());
            ASSERT(VALUE2 == obj2.value());

            obj2 = VALUE1;

            ASSERT(VALUE1 == OBJ1.value());
            ASSERT(VALUE1 == obj2.value());

            if (verbose) cout << "\tmake value" << endl;

            obj2 = VALUE2;

            ASSERT(VALUE1 == OBJ1.value());
            ASSERT(VALUE2 == obj2.value());

            obj2.makeValue(VALUE1);

            ASSERT(VALUE1 == OBJ1.value());
            ASSERT(VALUE1 == obj2.value());
        }

        if (verbose) cout
                << "\nUsing 'bsl::string' and 'char *' + ALLOC." << endl;

        {
            typedef const char *ValueType1;
            typedef bsl::string ValueType2;

            typedef bdlb::NullableValue<ValueType1> ObjType1;
            typedef bdlb::NullableValue<ValueType2> ObjType2;

            const ValueType1 VALUE1 = "abc";
            const ValueType2 VALUE2 = "def";

            if (verbose) cout << "\tcopy assignment" << endl;

            const ObjType1 OBJ1(VALUE1);
                  ObjType2 obj2(VALUE2, ALLOC);

            ASSERT(VALUE1 == OBJ1.value());
            ASSERT(VALUE2 == obj2.value());

            obj2 = OBJ1;

            ASSERT(VALUE1 == OBJ1.value());
            ASSERT(VALUE1 == obj2.value());

            if (verbose) cout << "\tvalue assignment" << endl;

            obj2 = VALUE2;

            ASSERT(VALUE1 == OBJ1.value());
            ASSERT(VALUE2 == obj2.value());

            obj2 = VALUE1;

            ASSERT(VALUE1 == OBJ1.value());
            ASSERT(VALUE1 == obj2.value());

            if (verbose) cout << "\tmake value" << endl;

            obj2 = VALUE2;

            ASSERT(VALUE1 == OBJ1.value());
            ASSERT(VALUE2 == obj2.value());

            obj2.makeValue(VALUE1);

            ASSERT(VALUE1 == OBJ1.value());
            ASSERT(VALUE1 == obj2.value());
        }

        // Making sure 'makeValue' works with explicit constructors.

        if (verbose) cout
                << "\nUsing 'Recipient' and 'MessageType'." << endl;

        {
            typedef MessageType ValueType1;
            typedef Recipient   ValueType2;

            typedef bdlb::NullableValue<ValueType1> ObjType1;
            typedef bdlb::NullableValue<ValueType2> ObjType2;

            const ValueType1 VALUE1 = IMPORTANT;
            const ValueType2 VALUE2(JUNK);

            if (verbose) cout << "\tcopy assignment" << endl;

            const ObjType1 OBJ1(VALUE1);
                  ObjType2 obj2(VALUE2);

            ASSERT(VALUE1 == OBJ1.value());
            ASSERT(VALUE2 == obj2.value());

            obj2 = OBJ1;

            ASSERT(VALUE1 == OBJ1.value());
            ASSERT(VALUE1 == obj2.value().getMsgType());

            if (verbose) cout << "\tvalue assignment" << endl;

            obj2 = VALUE2;

            ASSERT(VALUE1 == OBJ1.value());
            ASSERT(VALUE2 == obj2.value());

            obj2 = VALUE1;

            ASSERT(VALUE1 == OBJ1.value());
            ASSERT(VALUE1 == obj2.value().getMsgType());

            if (verbose) cout << "\tmake value" << endl;

            obj2 = VALUE2;

            ASSERT(VALUE1 == OBJ1.value());
            ASSERT(VALUE2 == obj2.value());

            obj2.makeValue(VALUE1);

            ASSERT(VALUE1 == OBJ1.value());
            ASSERT(VALUE1 == obj2.value().getMsgType());
        }

//#define SOMETHING_ELSE_THAT_SHOULD_NOT_WORK
#ifdef SOMETHING_ELSE_THAT_SHOULD_NOT_WORK
        if (verbose) cout
                << "\nUsing 'bsl::string' and 'int'." << endl;

        {
            typedef int         ValueType1;
            typedef bsl::string ValueType2;

            typedef bdlb::NullableValue<ValueType1> ObjType1;
            typedef bdlb::NullableValue<ValueType2> ObjType2;

            const ValueType1 VALUE1 = 123;
            const ValueType2 VALUE2 = "def";

            if (verbose) cout << "\tcopy assignment" << endl;

            const ObjType1 OBJ1(VALUE1);
                  ObjType2 obj2(VALUE2, ALLOC);

            ASSERT(VALUE1 == OBJ1.value());
            ASSERT(VALUE2 == obj2.value());

            obj2 = OBJ1;

            ASSERT(VALUE1 == OBJ1.value());
            //ASSERT(VALUE1 == obj2.value());

            if (verbose) cout << "\tvalue assignment" << endl;

            obj2 = VALUE2;

            ASSERT(VALUE1 == OBJ1.value());
            ASSERT(VALUE2 == obj2.value());

            obj2 = VALUE1;

            ASSERT(VALUE1 == OBJ1.value());
            //ASSERT(VALUE1 == obj2.value());

            if (verbose) cout << "\tmake value" << endl;

            obj2 = VALUE2;

            ASSERT(VALUE1 == OBJ1.value());
            ASSERT(VALUE2 == obj2.value());

            obj2.makeValue(VALUE1);

            ASSERT(VALUE1 == OBJ1.value());
            //ASSERT(VALUE1 == obj2.value());
        }
#endif

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING CONVERSION CONSTRUCTORS
        //
        // Concerns:
        //   - That convertible underlying types convert.
        //   - That types for which there is no conversion do not compile
        //     (we will do this test by hand for now, but could use template
        //     magic later, perhaps, to ensure that non-compile is enforced
        //     by the compiler).
        //
        // Plan:
        //   Conduct the regular test using 'int' and 'double'.  Then try
        //   'bsl::string' and 'char *' to observe with allocators involved
        //   Finally, try 'int' and 'bsl::string' by hand and observe that it
        //   fails to compile.
        //
        // Testing:
        //   bdlb::NullableValue(const bdlb::NullableValue<ORIGINAL>&o);
        //   bdlb::NullableValue(const bdlb::NullableValue<ORIGINAL>&o,*ba);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Conversion Constructors"
                          << "\n===============================" << endl;

        if (verbose) cout << "\nUsing 'int' and 'double." << endl;

        {
            typedef int                             ValueType1;
            typedef double                          ValueType2;

            typedef bdlb::NullableValue<ValueType1> ObjType1;
            typedef bdlb::NullableValue<ValueType2> ObjType2;

            const ValueType1 VALUE1 = 123;

            const ObjType1 OBJ1(VALUE1);
            const ObjType2 OBJ2(OBJ1);

            ASSERT(VALUE1             == OBJ1.value());
            ASSERT(ValueType2(VALUE1) == OBJ2.value());
        }

        if (verbose) cout << "\nUsing 'double' and 'int'." << endl;

        {
            typedef double                          ValueType1;
            typedef int                             ValueType2;

            typedef bdlb::NullableValue<ValueType1> ObjType1;
            typedef bdlb::NullableValue<ValueType2> ObjType2;

            const ValueType1 VALUE1 = 123;

            const ObjType1 OBJ1(VALUE1);
            const ObjType2 OBJ2(OBJ1);

            ASSERT(VALUE1             == OBJ1.value());
            ASSERT(ValueType2(VALUE1) == OBJ2.value());
        }

        if (verbose) cout << "\nUsing 'bsl::string' and 'char *' + ALLOC."
                          << endl;

        {
            typedef char *                          ValueType1;
            typedef bsl::string                     ValueType2;

            typedef bdlb::NullableValue<ValueType1> ObjType1;
            typedef bdlb::NullableValue<ValueType2> ObjType2;

            char p[] = "Hello";

            const ValueType1 VALUE1 = p;

            const ObjType1 OBJ1(VALUE1);
            const ObjType2 OBJ2(OBJ1, ALLOC);  // <<<=== ALLOC !!!

            ASSERT(VALUE1             == OBJ1.value());
            ASSERT(ValueType2(VALUE1) == OBJ2.value());
        }

//#define SOMETHING_THAT_SHOULD_NOT_WORK
#ifdef SOMETHING_THAT_SHOULD_NOT_WORK

        if (verbose) cout << "\nUsing 'bsl::string' and 'int'." << endl;

        {
            typedef int                             ValueType1;
            typedef bsl::string                     ValueType2;

            typedef bdlb::NullableValue<ValueType1> ObjType1;
            typedef bdlb::NullableValue<ValueType2> ObjType2;

            const ValueType1 VALUE1 = 123;

            const ObjType1 OBJ1(VALUE1);
            const ObjType2 OBJ2(OBJ1);
            const ObjType2 OBJ3(OBJ1, ALLOC);

            ASSERT(VALUE1             == OBJ1.value());
            //ASSERT(ValueType2(VALUE1) == OBJ2.value());
        }
#endif

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING VALUE ASSIGNMENT METHODS
        //
        // Concerns:
        //   - Make sure we can change values.
        //
        // Plan:
        //   Conduct the test using 'int' (does not use allocator) and
        //   'bsl::string' (uses allocator) for 'TYPE'.
        //
        // Testing:
        //   TYPE& operator=(const TYPE& rhs);
        //   TYPE& makeValue();
        //   void reset();
        //   TYPE& value();
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Value Assignment Methods"
                          << "\n================================" << endl;

        if (verbose) cout << "\nUsing 'bdlb::NullableValue<int>." << endl;

        {
            typedef int                            ValueType;
            typedef bdlb::NullableValue<ValueType> Obj;

            const ValueType VALUE1 = 123;
            const ValueType VALUE2 = 456;

            Obj mA;  Obj& A = mA;
            ASSERT (A.isNull());

            mA = VALUE1;
            ASSERT (!A.isNull());
            ASSERT (VALUE1 == A.value());

            mA.makeValue();
            ASSERT (!A.isNull());
            ASSERT (ValueType() == A.value());

            mA.value() = VALUE2;
            ASSERT (!A.isNull());
            ASSERT (VALUE2 == A.value());

            mA.reset();
            ASSERT (A.isNull());
        }

        if (verbose) cout << "\nUsing bdlb::NullableValue<bsl::string>."
                          << endl;

        {
            typedef bsl::string                    ValueType;
            typedef bdlb::NullableValue<ValueType> Obj;

            const ValueType VALUE1 = "abc";
            const ValueType VALUE2 = "def";

            Obj mA;  Obj& A = mA;
            ASSERT (A.isNull());

            mA = VALUE1;
            ASSERT (!A.isNull());
            ASSERT (VALUE1 == A.value());

            mA.makeValue();
            ASSERT (!A.isNull());
            ASSERT (ValueType() == A.value());

            mA.value() = VALUE2;
            ASSERT (!A.isNull());
            ASSERT (VALUE2 == A.value());

            mA.reset();
            ASSERT (A.isNull());
        }

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING VALUE CONSTRUCTORS
        //
        // Concerns:
        //   - Any value must be able to be copy constructed without affecting
        //     its argument.
        //   - Should work with and without a supplied allocator.
        //
        // Plan:
        //   Conduct the test using 'int' (does not use allocator) and
        //   'bsl::string' (uses allocator) for 'TYPE'.
        //
        // Testing:
        //   bdlb::NullableValue(const TYPE& value);
        //   bdlb::NullableValue(const TYPE& value, *ba);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Value Constructors"
                          << "\n==========================" << endl;

        if (verbose) cout << "\nUsing 'bdlb::NullableValue<int>." << endl;

        {
            typedef int                            ValueType;
            typedef bdlb::NullableValue<ValueType> Obj;

            const ValueType VALUE1 = 123;
            const ValueType VALUE2 = 456;

            const Obj A(VALUE1);
            ASSERT(VALUE1 == A.value());

            const Obj B(VALUE2);
            ASSERT(VALUE2 == B.value());
        }

        if (verbose) cout << "\nUsing bdlb::NullableValue<bsl::string>."
                          << endl;

        {
            typedef bsl::string                    ValueType;
            typedef bdlb::NullableValue<ValueType> Obj;

            const ValueType VALUE1 = "abc";
            const ValueType VALUE2 = "def";

            const Obj A(VALUE1, ALLOC);
            ASSERT(VALUE1 == A.value());

            const Obj B(VALUE2, ALLOC);
            ASSERT(VALUE2 == B.value());
        }

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING BDEX STREAMING OPERATIONS
        //
        // Concerns:
        //   That the stream operations work.
        //
        // Plan:
        //   Use 'int' for 'TYPE' - stream it out and stream it back in.
        //
        // Testing:
        //   STREAM& bdexStreamIn(STREAM& stream, int version);
        //   STREAM& bdexStreamOut(STREAM& stream, int version) const;
        //   int maxSupportedBdexVersion(int) const;
        // --------------------------------------------------------------------

        typedef bslx::TestInStream  In;
        typedef bslx::TestOutStream Out;
        const int VERSION_SELECTOR = 20140601;

        {
            typedef int                            ValueType;
            typedef bdlb::NullableValue<ValueType> Obj;

            const Obj X(123);

            Out       out(VERSION_SELECTOR);
            const int VERSION = X.maxSupportedBdexVersion(VERSION_SELECTOR);

            X.bdexStreamOut(out, VERSION);

            const char *const OD  = out.data();
            const int         LOD = static_cast<int>(out.length());
            In                in(OD, LOD);              ASSERT(in);
                                                        ASSERT(!in.isEmpty());
            Obj               t;                        ASSERT(X != t);

            t.bdexStreamIn(in, VERSION);                ASSERT(X == t);
            ASSERT(in);                                 ASSERT(in.isEmpty());
        }
        {
            typedef int                            ValueType;
            typedef bdlb::NullableValue<ValueType> Obj;

            const Obj X;
            Out       out(VERSION_SELECTOR);
            const int VERSION = X.maxSupportedBdexVersion(VERSION_SELECTOR);

            X.bdexStreamOut(out, VERSION);

            const char *const OD  = out.data();
            const int         LOD = static_cast<int>(out.length());
            In                in(OD, LOD);              ASSERT(in);
                                                        ASSERT(!in.isEmpty());
            Obj               t(123);                   ASSERT(X != t);

            t.bdexStreamIn(in, VERSION);                ASSERT(X == t);
            ASSERT(in);                                 ASSERT(in.isEmpty());
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY ASSIGNMENT OPERATOR
        //
        // Concerns:
        //   Any value must be assignable to an object having any initial value
        //   without affecting the rhs operand value.  Also, any object must be
        //   assignable to itself.
        //
        // Plan:
        //   Use 'bsl::string' for 'TYPE'.
        //
        //   Specify a set of unique values.  Construct and initialize all
        //   combinations (u, v) in the cross product.  Copy construct a
        //   control w from v, assign v to u, and assert that w == u and
        //   w == v.  Then test aliasing by copy constructing a control w from
        //   each u, assigning u to itself, and verifying that w == u.
        //
        // Testing:
        //    operator=(const bdlb::NullableValue& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Copy Assignment Operator"
                          << "\n================================" << endl;

        {
            typedef bsl::string                    ValueType;
            typedef bdlb::NullableValue<ValueType> Obj;

            const int NUM_VALUES = 3;

            Obj mX[NUM_VALUES];

            const ValueType VALUE1 = "123";
            const ValueType VALUE2 = "456";

            mX[1].makeValue(VALUE1);
            mX[2].makeValue(VALUE2);

            for (int i = 0; i < NUM_VALUES; ++i) {
                Obj mU(mX[i], ALLOC);  const Obj& U = mU;

                for (int j = 0; j < NUM_VALUES; ++j) {
                    Obj mV(mX[j], ALLOC);  const Obj& V = mV;

                    Obj mW(V, ALLOC);  const Obj& W = mW;

                    mU = V;

                    LOOP2_ASSERT(U, W, U == W);
                    LOOP2_ASSERT(V, W, V == W);
                }
            }

            for (int i = 0; i < NUM_VALUES; ++i) {
                Obj mU(mX[i], ALLOC);  const Obj& U = mU;
                Obj mW(U,     ALLOC);  const Obj& W = mW;

                mU = U;

                LOOP2_ASSERT(U, W, U == W);
            }
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR
        //
        // Concerns:
        //   Any value must be able to be copy constructed without affecting
        //   its argument.
        //
        // Plan:
        //   Conduct the test using 'int' (does not use allocator) and
        //   'bsl::string' (uses allocator) for 'TYPE'.
        //
        //   Specify a set whose elements have substantial and varied
        //   differences in value.  For each element in S, construct and
        //   initialize identical objects W and X using tested methods.  Then
        //   copy construct Y from X and use the equality operator to assert
        //   that both X and Y have the same value as W.
        //
        // Testing:
        //   bdlb::NullableValue(const bdlb::NullableValue& original);
        //   bdlb::NullableValue(const bdlb::NullableValue& original, *ba);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Copy Constructor"
                          << "\n========================" << endl;

        if (verbose) cout << "\nUsing 'bdlb::NullableValue<int>." << endl;

        {
            typedef int                            ValueType;
            typedef bdlb::NullableValue<ValueType> Obj;

            const int NUM_VALUES = 3;

            Obj mX[NUM_VALUES];
            Obj mW[NUM_VALUES];

            const ValueType VALUE1 = 123;
            const ValueType VALUE2 = 456;

            mX[1].makeValue(VALUE1);
            mW[1].makeValue(VALUE1);

            mX[2].makeValue(VALUE2);
            mW[2].makeValue(VALUE2);

            for (int i = 0; i < NUM_VALUES; ++i) {
                const Obj& X = mX[i];
                const Obj& W = mW[i];

                Obj mY(X);  const Obj& Y = mY;

                if (veryVerbose) {
                    T_ P_(i) P_(W) P_(X) P(Y)
                }

                LOOP2_ASSERT(X, W, X == W);
                LOOP2_ASSERT(Y, W, Y == W);
            }
        }

        if (verbose) cout << "\nUsing bdlb::NullableValue<bsl::string>."
                          << endl;

        {
            typedef bsl::string                    ValueType;
            typedef bdlb::NullableValue<ValueType> Obj;

            const int NUM_VALUES = 3;

            Obj mX[NUM_VALUES];
            Obj mW[NUM_VALUES];

            const ValueType VALUE1 = "123";
            const ValueType VALUE2 = "456";

            mX[1].makeValue(VALUE1);
            mW[1].makeValue(VALUE1);

            mX[2].makeValue(VALUE2);
            mW[2].makeValue(VALUE2);

            for (int i = 0; i < NUM_VALUES; ++i) {
                const Obj& X = mX[i];
                const Obj& W = mW[i];

                Obj mY(X, ALLOC);  const Obj& Y = mY;

                if (veryVerbose) {
                    T_ P_(i) P_(W) P_(X) P(Y)
                }

                LOOP2_ASSERT(X, W, X == W);
                LOOP2_ASSERT(Y, W, Y == W);
            }
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATORS
        //
        // Concerns:
        //   The '==' operator must return 'false' for objects that are very
        //   similar but still different, but must return 'true' for objects
        //   that are exactly the same.  Likewise, 'operator!=' must return
        //   'true' for objects that are very similar but still different, but
        //   must return 'false' for objects that are exactly the same.
        //
        //   That objects of different, but comparable types that have the
        //   same value compare equal.
        //
        // Plan:
        //   Use 'int' for 'TYPE'.  Construct a set of objects containing
        //   similar but different values.  Loop through the cross product of
        //   the test data.  For each tuple, use the '==' and '!=' operators
        //   and check their return value for correctness.
        //
        // Testing:
        //   operator==(const bdlb::NullableValue<LHS_TYPE>&,<RHS_TYPE>&);
        //   operator!=(const bdlb::NullableValue<LHS_TYPE>&,<RHS_TYPE>&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Equality Operators"
                          << "\n==========================" << endl;

        typedef int                            ValueType;
        typedef bdlb::NullableValue<ValueType> Obj;

        const int NUM_VALUES = 3;

        Obj objArray[NUM_VALUES];

        objArray[1].makeValue(123);
        objArray[2].makeValue(234);

        for (int i = 0; i < NUM_VALUES; ++i) {
            const Obj& U = objArray[i];

            if (veryVerbose) { T_ P_(i) P(U) }

            for (int j = 0; j < NUM_VALUES; ++j) {
                const Obj& V = objArray[j];

                if (veryVeryVerbose) { T_ T_ P_(j) P(V) }

                const bool isSame = (i == j);
                LOOP2_ASSERT(U, V,  isSame == (U == V));
                LOOP2_ASSERT(U, V, !isSame == (U != V));
            }
        }

        {
             const int    I1 = 1;
             const double D1 = 1;

             const int    I2 = 2;
             const double D2 = 2;

             typedef bdlb::NullableValue<int>    Ni;
             typedef bdlb::NullableValue<double> Nd;

             const Ni ni0;
             const Ni ni1(I1);
             const Ni ni2(I2);

             const Nd nd0;
             const Nd nd1(D1);
             const Ni nd2(D2);

             ASSERT(1 == (ni0 == ni0));
             ASSERT(0 == (ni0 != ni0));
             ASSERT(1 == (ni0 == nd0));
             ASSERT(0 == (ni0 != nd0));

             ASSERT(0 == (ni0 == ni1));
             ASSERT(1 == (ni0 != ni1));
             ASSERT(0 == (ni0 == nd1));
             ASSERT(1 == (ni0 != nd1));

             ASSERT(0 == (ni0 == ni2));
             ASSERT(1 == (ni0 != ni2));
             ASSERT(0 == (ni0 == nd2));
             ASSERT(1 == (ni0 != nd2));

             ASSERT(0 == (ni1 == ni0));
             ASSERT(1 == (ni1 != ni0));
             ASSERT(0 == (ni1 == nd0));
             ASSERT(1 == (ni1 != nd0));

             ASSERT(1 == (ni1 == ni1));
             ASSERT(0 == (ni1 != ni1));
             ASSERT(1 == (ni1 == nd1));
             ASSERT(0 == (ni1 != nd1));

             ASSERT(0 == (ni1 == ni2));
             ASSERT(1 == (ni1 != ni2));
             ASSERT(0 == (ni1 == nd2));
             ASSERT(1 == (ni1 != nd2));

             ASSERT(0 == (ni2 == ni0));
             ASSERT(1 == (ni2 != ni0));
             ASSERT(0 == (ni2 == nd0));
             ASSERT(1 == (ni2 != nd0));

             ASSERT(0 == (ni2 == ni1));
             ASSERT(1 == (ni2 != ni1));
             ASSERT(0 == (ni2 == nd1));
             ASSERT(1 == (ni2 != nd1));

             ASSERT(1 == (ni2 == ni2));
             ASSERT(0 == (ni2 != ni2));
             ASSERT(1 == (ni2 == nd2));
             ASSERT(0 == (ni2 != nd2));

             ASSERT(1 == (nd0 == ni0));
             ASSERT(0 == (nd0 != ni0));
             ASSERT(1 == (nd0 == nd0));
             ASSERT(0 == (nd0 != nd0));

             ASSERT(0 == (nd0 == ni1));
             ASSERT(1 == (nd0 != ni1));
             ASSERT(0 == (nd0 == nd1));
             ASSERT(1 == (nd0 != nd1));

             ASSERT(0 == (nd0 == ni2));
             ASSERT(1 == (nd0 != ni2));
             ASSERT(0 == (nd0 == nd2));
             ASSERT(1 == (nd0 != nd2));

             ASSERT(0 == (nd1 == ni0));
             ASSERT(1 == (nd1 != ni0));
             ASSERT(0 == (nd1 == nd0));
             ASSERT(1 == (nd1 != nd0));

             ASSERT(1 == (nd1 == ni1));
             ASSERT(0 == (nd1 != ni1));
             ASSERT(1 == (nd1 == nd1));
             ASSERT(0 == (nd1 != nd1));

             ASSERT(0 == (nd1 == ni2));
             ASSERT(1 == (nd1 != ni2));
             ASSERT(0 == (nd1 == nd2));
             ASSERT(1 == (nd1 != nd2));

             ASSERT(0 == (nd2 == ni0));
             ASSERT(1 == (nd2 != ni0));
             ASSERT(0 == (nd2 == nd0));
             ASSERT(1 == (nd2 != nd0));

             ASSERT(0 == (nd2 == ni1));
             ASSERT(1 == (nd2 != ni1));
             ASSERT(0 == (nd2 == nd1));
             ASSERT(1 == (nd2 != nd1));

             ASSERT(1 == (nd2 == ni2));
             ASSERT(0 == (nd2 != ni2));
             ASSERT(1 == (nd2 == nd2));
             ASSERT(0 == (nd2 != nd2));
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING PRINT METHOD AND OUTPUT (<<) OPERATOR
        //   We need to test the 'print' method and the '<<' operator.
        //
        // Concerns:
        //   The print method and output (<<) operator must work.
        //
        // Plan:
        //   Conduct the test using 'int' for 'TYPE'.
        //
        //   For a set of values, check that the 'print' method and output (<<)
        //   operator work as expected.
        //
        // Testing:
        //   print(bsl::ostream& s,int l=0,int spl=4) const;
        //   operator<<(bsl::ostream&,const bdlb::NullableValue<TYPE>&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Print Method & Output (<<) Operator"
                          << "\n==========================================="
                          << endl;

        typedef int                            ValueType;
        typedef bdlb::NullableValue<ValueType> Obj;

        const ValueType VALUE1          = 123;
        const char      NULL_RESULT[]   = "NULL";
        const char      VALUE1_RESULT[] = "123";

        if (verbose) cout << "\nTesting 'print' Method." << endl;
        {
            {
                Obj mX;  const Obj& X = mX;
                bsl::stringstream ss;
                ASSERT(&ss == &X.print(ss, 0, -1));
                ASSERTV(ss.str(), NULL_RESULT == ss.str());
            }
            {
                Obj mX;  const Obj& X = mX;
                mX.makeValue(VALUE1);
                bsl::stringstream ss;
                ASSERT(&ss == &X.print(ss, 0, -1));
                ASSERTV(ss.str(), VALUE1_RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nTesting Output (<<) Operator." << endl;
        {
            {
                Obj mX;  const Obj& X = mX;
                bsl::stringstream ss;
                ASSERT(&ss == &(ss << X));
                ASSERTV(ss.str(), NULL_RESULT == ss.str());
            }
            {
                Obj mX;  const Obj& X = mX;
                mX.makeValue(VALUE1);
                bsl::stringstream ss;
                ASSERT(&ss == &(ss << X));
                ASSERTV(ss.str(), VALUE1_RESULT == ss.str());
            }
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS AND BASIC ACCESSORS
        //   This test will verify that the primary manipulators are working as
        //   expected.  Also, we test that the basic accessors are working as
        //   expected.
        //
        // Concerns:
        //   * The default constructor must create a null object.
        //   * 'makeValue()' must set the value to the one constructed with the
        //     default constructor.
        //   * The 'makeValue(const TYPE&)' function must set the value
        //     appropriately.
        //
        // Plan:
        //   Conduct the test using 'int' (does not use allocator) and
        //   'bsl::string' (uses allocator) for 'TYPE'.  (Check that
        //   'ValueType' is the right size in each case.)
        //
        //   First, verify the default constructor by testing that the
        //   resulting object is null.
        //
        //   Next, verify that the 'makeValue' function works by making a value
        //   equal to the value passed into 'makeValue'.
        //
        //   Note that the destructor is exercised on each configuration as the
        //   object being tested leaves scope.
        //
        // Testing:
        //   typedef TYPE ValueType;
        //   bdlb::NullableValue();
        //   bdlb::NullableValue(bslma::Allocator *basicAllocator);
        //   ~bdlb::NullableValue();
        //   TYPE& makeValue();
        //   TYPE& makeValue(const TYPE& rhs);
        //   bool isNull() const;
        //   const TYPE& value() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Primary Manipulators & Basic Accessors"
                          << "\n=============================================="
                          << endl;

        if (verbose) cout << "\nUsing 'bdlb::NullableValue<int>'." << endl;

        {
            typedef int                            ValueType;
            typedef bdlb::NullableValue<ValueType> Obj;

            ASSERT(sizeof(ValueType) == sizeof(Obj::ValueType));

            if (veryVerbose) cout << "\tTesting default constructor." << endl;

            {
                Obj mX;  const Obj& X = mX;
                if (veryVeryVerbose) { T_ T_ P(X) }
                ASSERT(X.isNull());
            }

            if (veryVerbose) cout << "\tTesting 'makeValue'." << endl;

            {
                Obj mX;  const Obj& X = mX;

                mX.makeValue();
                if (veryVeryVerbose) { T_ T_ P(X) }
                ASSERT(!X.isNull());
                LOOP_ASSERT(X.value(), ValueType() == X.value());
            }

            {
                Obj mX;  const Obj& X = mX;

                mX.makeValue(3);  // set some random value
                mX.makeValue();   // reset to default
                if (veryVeryVerbose) { T_ T_ P(X) }
                ASSERT(!X.isNull());
                LOOP_ASSERT(X.value(), ValueType() == X.value());
            }

            {
                Obj mX;  const Obj& X = mX;

                const ValueType VALUE1 = 123;

                mX.makeValue(VALUE1);
                if (veryVeryVerbose) { T_ T_ P(X) }
                ASSERT(!X.isNull());
                LOOP_ASSERT(X.value(), VALUE1 == X.value());
            }

            {
                Obj mX;  const Obj& X = mX;

                const ValueType VALUE1 = 123;
                const ValueType VALUE2 = 456;

                mX.makeValue(VALUE1);
                mX.makeValue(VALUE2);
                if (veryVeryVerbose) { T_ T_ P(X) }
                ASSERT(!X.isNull());
                LOOP_ASSERT(X.value(), VALUE2 == X.value());
            }
        }

        if (verbose) cout << "\nUsing 'bdlb::NullableValue<bsl::string>'."
                          << endl;

        {
            typedef bsl::string                    ValueType;
            typedef bdlb::NullableValue<ValueType> Obj;

            ASSERT(sizeof(ValueType) == sizeof(Obj::ValueType));

            if (veryVerbose) cout << "\tTesting default constructor." << endl;

            {
                Obj mX(ALLOC);  const Obj& X = mX;
                if (veryVeryVerbose) { T_ T_ P(X) }
                ASSERT(X.isNull());
            }

            if (veryVerbose) cout << "\tTesting 'makeValue'." << endl;

            {
                Obj mX(ALLOC);  const Obj& X = mX;

                mX.makeValue();
                if (veryVeryVerbose) { T_ T_ P(X) }
                ASSERT(!X.isNull());
                LOOP_ASSERT(X.value(), ValueType() == X.value());
            }

            {
                Obj mX(ALLOC);  const Obj& X = mX;

                mX.makeValue("3");  // set some random value
                mX.makeValue();     // reset to default
                if (veryVeryVerbose) { T_ T_ P(X) }
                ASSERT(!X.isNull());
                LOOP_ASSERT(X.value(), ValueType() == X.value());
            }

            {
                Obj mX(ALLOC);  const Obj& X = mX;

                const ValueType VALUE1 = "123";

                mX.makeValue(VALUE1);
                if (veryVeryVerbose) { T_ T_ P(X) }
                ASSERT(!X.isNull());
                LOOP_ASSERT(X.value(), VALUE1 == X.value());
            }

            {
                Obj mX(ALLOC);  const Obj& X = mX;

                const ValueType VALUE1 = "123";
                const ValueType VALUE2 = "456";

                mX.makeValue(VALUE1);
                mX.makeValue(VALUE2);
                if (veryVeryVerbose) { T_ T_ P(X) }
                ASSERT(!X.isNull());
                LOOP_ASSERT(X.value(), VALUE2 == X.value());
            }
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // BREATHING TEST 2: Using 'int'
        //   This test exercises basic functionality, but tests nothing.
        //
        // Concerns:
        //   We want to demonstrate a base-line level of correct operation of
        //   the following methods and operators:
        //     - default and copy constructors.
        //     - the assignment operator (including aliasing).
        //     - equality operators: 'operator==' and 'operator!='.
        //     - primary manipulators: 'makeValue' and 'reset'.
        //     - basic accessors: 'value' and 'isNull'.
        //
        // Plan:
        //   Create four test objects using the default, initializing, and copy
        //   constructors.  Exercise the basic value-semantic methods and the
        //   equality operators using the test objects.  Invoke the primary
        //   manipulator [5, 6, 7], copy constructor [2, 4], assignment
        //   operator without [9, 9] and with [10] aliasing.  Use the basic
        //   accessors to verify the expected results.  Display object values
        //   frequently in verbose mode.  Note that 'VA', 'VB' and 'VC' denote
        //   unique, but otherwise arbitrary, object values, while 'U' denotes
        //   the valid, but "unknown", default object value.
        //
        //   1. Create an object x1 (init. to VA)    { x1:VA                  }
        //   2. Create an object x2 (copy of x1)     { x1:VA x2:VA            }
        //   3. Create an object x3 (default ctor)   { x1:VA x2:VA x3:U       }
        //   4. Create an object x4 (copy of x3)     { x1:VA x2:VA x3:U  x4:U }
        //   5. Set x3 using 'makeValue' (set to VB) { x1:VA x2:VA x3:VB x4:U }
        //   6. Change x1 using 'reset'              { x1:U  x2:VA x3:VB x4:U }
        //   7. Change x1 ('makeValue', set to VC)   { x1:VC x2:VA x3:VB x4:U }
        //   8. Assign x2 = x1                       { x1:VC x2:VC x3:VB x4:U }
        //   9. Assign x2 = x3                       { x1:VC x2:VB x3:VB x4:U }
        //  10. Assign x1 = x1 (aliasing)            { x1:VC x2:VB x3:VB x4:U }
        //
        // Testing:
        //   BREATHING TEST 2: Using 'int'
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST 2: Using 'int'"
                          << "\n=============================" << endl;

        typedef int                            ValueType;
        typedef bdlb::NullableValue<ValueType> Obj;

        // possible values
        const ValueType VA(123);
        const ValueType VB(234);
        const ValueType VC(345);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 1. Create an object x1 (init. to VA)."
                             "\t\t{ x1:VA }" << endl;
        Obj mX1(VA);  const Obj& X1 = mX1;
        if (veryVerbose) { cout << '\t'; P(X1); }

        if (veryVerbose) cout << "\ta. Check initial state of x1." << endl;
        ASSERT(!X1.isNull());
        ASSERT(VA == X1.value());

        if (veryVerbose) cout << "\tb. Try equality operators: x1 <op> x1."
                              << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 2. Create an object x2 (copy of x1)."
                             "\t\t{ x1:VA x2:VA }" << endl;
        Obj mX2(X1);  const Obj& X2 = mX2;
        if (veryVerbose) { cout << '\t'; P(X2); }

        if (veryVerbose) cout << "\ta. Check initial state of x2." << endl;
        ASSERT(!X2.isNull());
        ASSERT(VA == X2.value());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x2 <op> x1, x2." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 3. Create an object x3 (default ctor)."
                             "\t\t{ x1:VA x2:VA x3:U }" << endl;
        Obj mX3;  const Obj& X3 = mX3;
        if (veryVerbose) { cout << '\t'; P(X3); }

        if (veryVerbose) cout << "\ta. Check initial state of x3." << endl;
        ASSERT(X3.isNull());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x3 <op> x1, x2, x3." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 4. Create an object x4 (copy of x3)."
                             "\t\t{ x1:VA x2:VA x3:U x4:U }" << endl;
        Obj mX4(X3);  const Obj& X4 = mX4;
        if (veryVerbose) { cout << '\t'; P(X4); }

        if (veryVerbose) cout << "\ta. Check initial state of x4." << endl;
        ASSERT(X4.isNull());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x4 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X4 == X1));        ASSERT(1 == (X4 != X1));
        ASSERT(0 == (X4 == X2));        ASSERT(1 == (X4 != X2));
        ASSERT(1 == (X4 == X3));        ASSERT(0 == (X4 != X3));
        ASSERT(1 == (X4 == X4));        ASSERT(0 == (X4 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 5. Set x3 using '=' (set to VB)."
                             "\t\t{ x1:VA x2:VA x3:VB x4:U }" << endl;
        mX3.makeValue(VB);
        if (veryVerbose) { cout << '\t'; P(X3); }

        if (veryVerbose) cout << "\ta. Check new state of x3." << endl;
        ASSERT(!X3.isNull());
        ASSERT(VB == X3.value());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x3 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));
        ASSERT(0 == (X3 == X4));        ASSERT(1 == (X3 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 6. Change x1 using 'reset'."
                             "\t\t\t{ x1:U x2:VA x3:VB x4:U }" << endl;
        mX1.reset();
        if (veryVerbose) { cout << '\t'; P(X1); }

        if (veryVerbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(X1.isNull());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x1 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(1 == (X1 == X4));        ASSERT(0 == (X1 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 7. Change x1 ('makeValue', set to VC)."
                             "\t\t{ x1:VC x2:VA x3:VB x4:U }" << endl;
        mX1.makeValue(VC);
        if (veryVerbose) { cout << '\t'; P(X1); }

        if (veryVerbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(!X1.isNull());
        ASSERT(VC == X1.value());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x1 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(0 == (X1 == X4));        ASSERT(1 == (X1 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 8. Assign x2 = x1."
                             "\t\t\t\t{ x1:VC x2:VC x3:VB x4:U }" << endl;
        mX2 = X1;
        if (veryVerbose) { cout << '\t'; P(X2); }

        if (veryVerbose) cout << "\ta. Check new state of x2." << endl;
        ASSERT(!X2.isNull());
        ASSERT(VC == X2.value());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(0 == (X2 == X3));        ASSERT(1 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 9. Assign x2 = x3."
                             "\t\t\t\t{ x1:VC x2:VB x3:VB x4:U }" << endl;
        mX2 = X3;
        if (veryVerbose) { cout << '\t'; P(X2); }

        if (veryVerbose) cout << "\ta. Check new state of x2." << endl;
        ASSERT(!X2.isNull());
        ASSERT(VB == X2.value());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X2 == X1));        ASSERT(1 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(1 == (X2 == X3));        ASSERT(0 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 10. Assign x1 = x1."
                             "\t\t\t\t{ x1:VC x2:VB x3:VB x4:U }" << endl;
        mX1 = X1;
        if (veryVerbose) { cout << '\t'; P(X1); }

        if (veryVerbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(!X1.isNull());
        ASSERT(VC == X1.value());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x1 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(0 == (X1 == X4));        ASSERT(1 == (X1 != X4));

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST 1: Using 'bsl::string'
        //   This test exercises basic functionality, but tests nothing.
        //
        // Concerns:
        //   We want to demonstrate a base-line level of correct operation of
        //   the following methods and operators:
        //     - default and copy constructors.
        //     - the assignment operator (including aliasing).
        //     - equality operators: 'operator==' and 'operator!='.
        //     - primary manipulators: 'makeValue' and 'reset'.
        //     - basic accessors: 'value' and 'isSet'.
        //
        // Plan:
        //   Create four test objects using the default, initializing, and copy
        //   constructors.  Exercise the basic value-semantic methods and the
        //   equality operators using the test objects.  Invoke the primary
        //   manipulator [5, 6, 7], copy constructor [2, 4], assignment
        //   operator without [9, 9] and with [10] aliasing.  Use the basic
        //   accessors to verify the expected results.  Display object values
        //   frequently in verbose mode.  Note that 'VA', 'VB' and 'VC' denote
        //   unique, but otherwise arbitrary, object values, while 'U' denotes
        //   the valid, but "unknown", default object value.
        //
        //   1. Create an object x1 (init. to VA)    { x1:VA                  }
        //   2. Create an object x2 (copy of x1)     { x1:VA x2:VA            }
        //   3. Create an object x3 (default ctor)   { x1:VA x2:VA x3:U       }
        //   4. Create an object x4 (copy of x3)     { x1:VA x2:VA x3:U  x4:U }
        //   5. Set x3 using 'makeValue' (set to VB) { x1:VA x2:VA x3:VB x4:U }
        //   6. Change x1 using 'reset'              { x1:U  x2:VA x3:VB x4:U }
        //   7. Change x1 ('makeValue', set to VC)   { x1:VC x2:VA x3:VB x4:U }
        //   8. Assign x2 = x1                       { x1:VC x2:VC x3:VB x4:U }
        //   9. Assign x2 = x3                       { x1:VC x2:VB x3:VB x4:U }
        //  10. Assign x1 = x1 (aliasing)            { x1:VC x2:VB x3:VB x4:U }
        //
        // Testing:
        //   BREATHING TEST 1: Using 'bsl::string'
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST 1: Using 'bsl::string'"
                          << "\n=====================================" << endl;

        typedef bsl::string                    ValueType;
        typedef bdlb::NullableValue<ValueType> Obj;

        // possible values
        const ValueType VA("The");
        const ValueType VB("Breathing");
        const ValueType VC("Test");

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 1. Create an object x1 (init. to VA)."
                             "\t\t{ x1:VA }" << endl;
        Obj mX1(VA);  const Obj& X1 = mX1;
        if (veryVerbose) { cout << '\t'; P(X1); }

        if (veryVerbose) cout << "\ta. Check initial state of x1." << endl;
        ASSERT(!X1.isNull());
        ASSERT(VA == X1.value());

        if (veryVerbose) cout << "\tb. Try equality operators: x1 <op> x1."
                              << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 2. Create an object x2 (copy of x1)."
                             "\t\t{ x1:VA x2:VA }" << endl;
        Obj mX2(X1);  const Obj& X2 = mX2;
        if (veryVerbose) { cout << '\t'; P(X2); }

        if (veryVerbose) cout << "\ta. Check initial state of x2." << endl;
        ASSERT(!X2.isNull());
        ASSERT(VA == X2.value());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x2 <op> x1, x2." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 3. Create an object x3 (default ctor)."
                             "\t\t{ x1:VA x2:VA x3:U }" << endl;
        Obj mX3;  const Obj& X3 = mX3;
        if (veryVerbose) { cout << '\t'; P(X3); }

        if (veryVerbose) cout << "\ta. Check initial state of x3." << endl;
        ASSERT(X3.isNull());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x3 <op> x1, x2, x3." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 4. Create an object x4 (copy of x3)."
                             "\t\t{ x1:VA x2:VA x3:U x4:U }" << endl;
        Obj mX4(X3);  const Obj& X4 = mX4;
        if (veryVerbose) { cout << '\t'; P(X4); }

        if (veryVerbose) cout << "\ta. Check initial state of x4." << endl;
        ASSERT(X4.isNull());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x4 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X4 == X1));        ASSERT(1 == (X4 != X1));
        ASSERT(0 == (X4 == X2));        ASSERT(1 == (X4 != X2));
        ASSERT(1 == (X4 == X3));        ASSERT(0 == (X4 != X3));
        ASSERT(1 == (X4 == X4));        ASSERT(0 == (X4 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 5. Set x3 using '=' (set to VB)."
                             "\t\t{ x1:VA x2:VA x3:VB x4:U }" << endl;
        mX3.makeValue(VB);
        if (veryVerbose) { cout << '\t'; P(X3); }

        if (veryVerbose) cout << "\ta. Check new state of x3." << endl;
        ASSERT(!X3.isNull());
        ASSERT(VB == X3.value());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x3 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));
        ASSERT(0 == (X3 == X4));        ASSERT(1 == (X3 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 6. Change x1 using 'reset'."
                             "\t\t\t{ x1:U x2:VA x3:VB x4:U }" << endl;
        mX1.reset();
        if (veryVerbose) { cout << '\t'; P(X1); }

        if (veryVerbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(X1.isNull());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x1 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(1 == (X1 == X4));        ASSERT(0 == (X1 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 7. Change x1 ('makeValue', set to VC)."
                             "\t\t{ x1:VC x2:VA x3:VB x4:U }" << endl;
        mX1.makeValue(VC);
        if (veryVerbose) { cout << '\t'; P(X1); }

        if (veryVerbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(!X1.isNull());
        ASSERT(VC == X1.value());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x1 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(0 == (X1 == X4));        ASSERT(1 == (X1 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 8. Assign x2 = x1."
                             "\t\t\t\t{ x1:VC x2:VC x3:VB x4:U }" << endl;
        mX2 = X1;
        if (veryVerbose) { cout << '\t'; P(X2); }

        if (veryVerbose) cout << "\ta. Check new state of x2." << endl;
        ASSERT(!X2.isNull());
        ASSERT(VC == X2.value());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(0 == (X2 == X3));        ASSERT(1 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 9. Assign x2 = x3."
                             "\t\t\t\t{ x1:VC x2:VB x3:VB x4:U }" << endl;
        mX2 = X3;
        if (veryVerbose) { cout << '\t'; P(X2); }

        if (veryVerbose) cout << "\ta. Check new state of x2." << endl;
        ASSERT(!X2.isNull());
        ASSERT(VB == X2.value());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X2 == X1));        ASSERT(1 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(1 == (X2 == X3));        ASSERT(0 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 10. Assign x1 = x1."
                             "\t\t\t\t{ x1:VC x2:VB x3:VB x4:U }" << endl;
        mX1 = X1;
        if (veryVerbose) { cout << '\t'; P(X1); }

        if (veryVerbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(!X1.isNull());
        ASSERT(VC == X1.value());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x1 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(0 == (X1 == X4));        ASSERT(1 == (X1 != X4));

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    ASSERT(0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
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
