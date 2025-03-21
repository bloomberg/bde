// bsltf_templatetestfacility.t.cpp                                   -*-C++-*-
#include <bsltf_templatetestfacility.h>

#include <bslma_destructionutil.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bslmf_assert.h>
#include <bslmf_issame.h>

#include <bsls_assert.h>
#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>
#include <bsls_nameof.h>
#include <bsls_objectbuffer.h>
#include <bsls_outputredirector.h>
#include <bsls_platform.h>
#include <bsls_util.h>

#include <stdio.h>
#include <stdlib.h>
#include <typeinfo>

using namespace BloombergLP;
using namespace BloombergLP::bsltf;
using bsls::NameOf;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test provides a namespace, `TemplateTestFacility`, and a
// set of macros that facilitate the testing of templates with parameterized
// types.
//
//-----------------------------------------------------------------------------
// CLASS METHODS
// [ 5] void TemplateTestFacility::create(int value);
// [ 5] int TemplateTestFacility::getIndex(const TYPE& object);
// [ 6] void TemplateTestFacility::emplace<T>(T *, int, bslma::Allocator *);
// [ 6] void TemplateTestFacility::emplace<T>(T **, int, bslma::Allocator *);
//
// FREE FUNCTIONS
// [ 7] void debugprint(const EnumeratedTestType::Enum& obj);
// [ 7] void debugprint(const UnionTestType& obj);
// [ 7] void debugprint(const SimpleTestType& obj);
// [ 7] void debugprint(const AllocTestType& obj);
// [ 7] void debugprint(const MovableAllocTestType& obj);
// [ 7] void debugprint(const MoveOnlyAllocTestType& obj);
// [ 7] void debugprint(const BitwiseMoveableTestType& obj);
// [ 7] void debugprint(const AllocBitwiseMoveableTestType& obj);
// [ 7] void debugprint(const NonTypicalOverloadsTestType& obj);
// [ 7] void debugprint(const NonCopyConstructibleTestType& obj);
// [ 7] void debugprint(const NonDefaultConstructibleTestType& obj);
//
// MACROS
// [ 4] BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE
// [ 4] BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_USER_DEFINED
// [ 4] BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR
// [ 4] BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_AWKWARD
// [ 4] BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL
// [ 3] BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(CLASS, METHOD, ...)
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 8] USAGE EXAMPLE
// [ 2] ALIASES

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);
        fflush(stdout);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//              ADDITIONAL TEST MACROS FOR THIS TEST DRIVER
// ----------------------------------------------------------------------------

#if defined(BSLS_COMPILERFEATURES_SIMULATE_FORWARD_WORKAROUND)
# define BSL_DO_NOT_TEST_MOVE_FORWARDING 1
// Some compilers produce ambiguities when trying to construct our test types
// for `emplace`-type functionality with the C++03 move-emulation.  This is a
// compiler bug triggering in lower level components, so we simply disable
// those aspects of testing, and rely on the extensive test coverage on other
// platforms.
#endif

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

static bool verbose, veryVerbose, veryVeryVerbose, veryVeryVeryVerbose;

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using the `BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE` Macro
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we demonstrate how to use
// `BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE` to call a class method template
// for a list of types.
//
// First, we define a `struct` template `TestTemplate` taking in a
// parameterized `TYPE` that has a class method, `printTypeName`:
// ```

/// This `struct` provides a namespace for a simple test method.
template <class TYPE>
struct TestTemplate {

    // CLASS METHODS

    /// Prints the name of the parameterized `TYPE` to the console.
    static void printTypeName();
};

template <>
void TestTemplate<int>::printTypeName()
{
    printf("int\n");
}

template <>
void TestTemplate<char>::printTypeName()
{
    printf("char\n");
}

template <>
void TestTemplate<double>::printTypeName()
{
    printf("double\n");
}
// ```

///Example 2: Writing a Type Independent Test Driver
///- - - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we demonstrate using the `TemplateTestFacility` `struct`
// and the macros provided by this component to test the default constructor
// and primary manipulator of a class template in the context of a typical
// BDE-style test driver.  Note that a goal of the demonstrated test is to
// validate the class template with a broad range of types emulating those with
// which the template might be instantiated.
//
// First, we define a simple class template, `MyNullableValue`, that we will
// later need to test:
// ```

/// This (value-semantic) class template extends the parameterized
/// `TYPE` to include the notion of a "null" value.
template <class TYPE>
class MyNullableValue {

    // DATA
    TYPE d_value;     // non-null value
    bool d_nullFlag;  // flag to indicate if the value is null

  public:
    // CREATORS

    /// Create a `MyNullableValue` that initially has a value of null.
    MyNullableValue()
    : d_nullFlag(true)
    {
    }

    // MyNullableValue(const MyNullableValue& original) = default;
        // Create a `MyNullableValue` object having the same value as the
        // specified `original` object.

    // ~MyNullableValue() = default;
        // Destroy this object.

    // MANIPULATORS
    // MyNullableValue& operator=(const MyNullableValue& rhs) = default;
        // Assign to this object the value of the specified `rhs` object, and
        // return a reference providing modifiable access to this object.

    /// Set this object to the null value.
    void makeNull()
    {
        d_nullFlag = true;
    }

    /// Set the value of this object to be that of the specified `value`
    /// of the parameterized `TYPE`.
    void makeValue(const TYPE& value)
    {
        d_nullFlag = false;
        d_value = value;
    }

    // ACCESSORS

    /// Return `true` if this object is null, and `false` otherwise.
    bool isNull() const
    {
        return d_nullFlag;
    }

    /// Return a reference providing non-modifiable access to the
    /// underlying object of the parameterized `TYPE`.  The behavior is
    /// undefined if the object is null.
    const TYPE& value() const
    {
        return d_value;
    }
};
// ```
// Then, we define some aliases for the micros that will be used by the test
// driver:
// ```
#define RUN_EACH_TYPE BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE
#define TEST_TYPES_REGULAR BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR
// ```
// Next, we define a `struct` template, `MyTestDriver`, that provides a
// namespace containing the test cases (here, only `testCase2` is defined for
// brevity) of the test driver:
// ```

/// This `struct` provides a namespace for the class methods used to
/// implement the test driver.
template <class TYPE>
struct MyTestDriver {

    // TYPES

    /// This `typedef` provides an alias to the type under testing.
    typedef MyNullableValue<TYPE> Obj;

    /// Test primary manipulators.
    static void testCase2();

};
// ```
// Now, we define the implementation of `MyTestDriver::testCase2`:
// ```
template <class TYPE>
void MyTestDriver<TYPE>::testCase2()
{
    // --------------------------------------------------------------------
    // DEFAULT CTOR, PRIMARY MANIPULATORS, AND DTOR
    //   Ensure that we can use the default constructor to create an
    //   object (having the default-constructed value), use the primary
    //   manipulators to put that object into any state relevant for
    //   thorough testing, and use the destructor to destroy it safely.
    //
    // Concerns:
    // 1. An object created using the default constructor (with or without
    //    a supplied allocator) has the contractually specified value.
    //
    // 2. The `makeValue` method sets the value of a object to any specified
    //    value.
    //
    // 3. The `makeNull` method set the value of a object to null.
    //
    // 4. Objects of different values can coexist.
    //
    // 5. The destructor does not modify other objects.
    //
    // Plan:
    // 1. Default-construct an object and use the (as yet unproven) salient
    //    attribute accessors to verify that the value of the object is the
    //    null value.  (C-1)
    //
    // 2. Default-construct another object, and use the `makeValue` method,
    //    passing in an object created with the `TemplateTestFacility::create`
    //    class method template, to set the value of the object to a non-null
    //    value.  Use the (as yet unproven) salient attribute accessors and the
    //    `TemplateTestFacility::getIdentifier` class method template to verify
    //    that the new object the expected value and the object created in P-1
    //    still has the same value.  (C-2, 4)
    //
    // 3. Using the loop-based approach, for each identifier in a range of
    //    integer identifiers:
    //
    //   1. Default-construct a modifiable object, `mL`, and use the (as yet
    //      unproven) salient attribute accessors to verify the value of the
    //      default constructed object is the null value.  (C-1)
    //
    //   2. Create an object of the parameterized `TYPE`, `LV`, using the
    //      `TemplateTestFacility::create` class method template, specifying
    //      the integer loop identifier.
    //
    //   3. Use the `makeValue` method to set the value of `mL` to `LV`.  Use
    //      the (as yet unproven) salient attribute accessors and the
    //      `TemplateTestFacility::getIdentifier` class method template to
    //      verify `mL` has the expected value.  (C-2)
    //
    //   4. Invoke the `makeNull` method of `mL`.  Use the attribute
    //      accessors to verify the value of the object is now null.  (C-3)
    //
    // 4. Create an object in a nested block.  Below the block, verify the
    //    objects created in P-1 and P-2 still have the same value.  (C-5)
    //
    // Testing:
    //   MyNullableValue();
    //   ~MyNullableValue();
    //   void makeNull();
    //   void MakeValue(const TYPE& value);
    // --------------------------------------------------------------------

    if (verbose) printf("\nDEFAULT CTOR, PRIMARY MANIPULATORS, AND DTOR"
                        "\n============================================\n");


    if (verbose) printf("\n\tfor type: %s\n", NameOf<TYPE>().name());

    if (verbose) printf("\nTesting default constructor.\n");

    Obj mW; const Obj& W = mW;
    ASSERT(true == W.isNull());

    Obj mX; const Obj& X = mX;
    const TYPE XV = TemplateTestFacility::create<TYPE>(1);
    mX.makeValue(XV);

    ASSERTV(X.value(), TemplateTestFacility::getIdentifier(X.value()),
            1 == TemplateTestFacility::getIdentifier(X.value()));

    if (verbose) printf("\nTesting primary manipulators.\n");

    for (int ti = 0; ti != 10; ++ti) {

        if (veryVerbose) { T_ P(ti) }

        Obj mL; const Obj& L = mL;
        ASSERT(true == L.isNull());

        const TYPE LV = TemplateTestFacility::create<TYPE>(ti);

        mL.makeValue(LV);
        ASSERT(false == L.isNull());
        ASSERT(LV == L.value());

        mL.makeNull();
        ASSERT(true == L.isNull());
    }

    if (verbose) printf("\nTesting destructor.\n");
    {
        Obj Z;
    }

    ASSERT(true == W.isNull());
    ASSERT(XV == X.value());
}
// ```
// Notice that, we create objects of the parameterized `TYPE` using the
// `TemplateTestFacility::create` class method template specifying an integer
// identifier; the created object has a value that is uniquely associated with
// the integer identifier.
//
// Also notice that we verified that an object of the parameterized `TYPE` has
// the expected value in two ways:
//
// 1. By equal comparing (1) the integer identifier returned from calling the
//    `TemplateTestFacility::getIdentifier` class method template (specifying
//    the object), and (2) the integer identifier uniquely associated with the
//    expected value of the object.
//
// 2. By directly using the equality comparison operator for the parameterized
//    `TYPE`.  In general, the equality comparison operator is defined for all
//    types intended for testing in the `bsltf` package unless specified
//    otherwise (e.g., bsltf::NonEqualComparableTestType).

// ============================================================================
//                               TEST APPARATUS
// ----------------------------------------------------------------------------

namespace {

template <class TYPE>
struct TestHelper
{
    static void breathingTest();

    static void test3Helper();

    static void test4Helper();

    static void test5Helper();

    static void test6Helper();

    static void test7Helper();
};

template <class TYPE>
void TestHelper<TYPE>::breathingTest()
{
    if (veryVerbose)
        printf("\n==TYPE: %s==\n", NameOf<TYPE>().name());

    for (int ti = 0; ti < 128; ++ti) {
        TYPE obj = TemplateTestFacility::create<TYPE>(ti);
        int value = TemplateTestFacility::getIdentifier(obj);

        if (verbose) P(value);
        ASSERTV(ti, value, ti == value);
    }
};

bool g_invokedFlags[50] = { 0 }; // flags used to indicate whether a template
                                 // instance has been invoked.
void clearInvokedFlags()
{
    for (int ti = 0; ti < 50; ++ti) {
        g_invokedFlags[ti] = false;
    }
}

void checkInvokedFlags(int line, int begin, int numFlags)
{
    for (int ti = begin; ti < begin + numFlags; ++ti) {
        if (veryVerbose) { P_(ti) P(g_invokedFlags[ti]) }
        ASSERTV(line, ti, g_invokedFlags[ti]);
    }
}

// Test Case 3 Apparatus

template <class TYPE>
void TestHelper<TYPE>::test3Helper()
{
    if (veryVerbose)
        printf("\n==TYPE: %s==\n", NameOf<TYPE>().name());

    g_invokedFlags[TYPE::value()] = true;
}

#define BSLTF_TTF_DEFINE_TYPE(num)                                            \
class TestType ## num {                                                       \
  public:                                                                     \
    static int value() { return num; }                                        \
}

#define BSLTF_TTF_NUM_TYPES 15

BSLTF_TTF_DEFINE_TYPE(0);
BSLTF_TTF_DEFINE_TYPE(1);
BSLTF_TTF_DEFINE_TYPE(2);
BSLTF_TTF_DEFINE_TYPE(3);
BSLTF_TTF_DEFINE_TYPE(4);
BSLTF_TTF_DEFINE_TYPE(5);
BSLTF_TTF_DEFINE_TYPE(6);
BSLTF_TTF_DEFINE_TYPE(7);
BSLTF_TTF_DEFINE_TYPE(8);
BSLTF_TTF_DEFINE_TYPE(9);
BSLTF_TTF_DEFINE_TYPE(10);
BSLTF_TTF_DEFINE_TYPE(11);
BSLTF_TTF_DEFINE_TYPE(12);
BSLTF_TTF_DEFINE_TYPE(13);
BSLTF_TTF_DEFINE_TYPE(14);
BSLTF_TTF_DEFINE_TYPE(15);
BSLTF_TTF_DEFINE_TYPE(16);
BSLTF_TTF_DEFINE_TYPE(17);
BSLTF_TTF_DEFINE_TYPE(18);
BSLTF_TTF_DEFINE_TYPE(19);

#define BSLTF_TTF_TYPE_NAME(num) TestType ## num

#define BSLTF_TTF_TYPES0   BSLTF_TTF_TYPE_NAME(0)
#define BSLTF_TTF_TYPES1   BSLTF_TTF_TYPES0,  BSLTF_TTF_TYPE_NAME(1)
#define BSLTF_TTF_TYPES2   BSLTF_TTF_TYPES1,  BSLTF_TTF_TYPE_NAME(2)
#define BSLTF_TTF_TYPES3   BSLTF_TTF_TYPES2,  BSLTF_TTF_TYPE_NAME(3)
#define BSLTF_TTF_TYPES4   BSLTF_TTF_TYPES3,  BSLTF_TTF_TYPE_NAME(4)
#define BSLTF_TTF_TYPES5   BSLTF_TTF_TYPES4,  BSLTF_TTF_TYPE_NAME(5)
#define BSLTF_TTF_TYPES6   BSLTF_TTF_TYPES5,  BSLTF_TTF_TYPE_NAME(6)
#define BSLTF_TTF_TYPES7   BSLTF_TTF_TYPES6,  BSLTF_TTF_TYPE_NAME(7)
#define BSLTF_TTF_TYPES8   BSLTF_TTF_TYPES7,  BSLTF_TTF_TYPE_NAME(8)
#define BSLTF_TTF_TYPES9   BSLTF_TTF_TYPES8,  BSLTF_TTF_TYPE_NAME(9)
#define BSLTF_TTF_TYPES10  BSLTF_TTF_TYPES9,  BSLTF_TTF_TYPE_NAME(10)
#define BSLTF_TTF_TYPES11  BSLTF_TTF_TYPES10, BSLTF_TTF_TYPE_NAME(11)
#define BSLTF_TTF_TYPES12  BSLTF_TTF_TYPES11, BSLTF_TTF_TYPE_NAME(12)
#define BSLTF_TTF_TYPES13  BSLTF_TTF_TYPES12, BSLTF_TTF_TYPE_NAME(13)
#define BSLTF_TTF_TYPES14  BSLTF_TTF_TYPES13, BSLTF_TTF_TYPE_NAME(14)
#define BSLTF_TTF_TYPES15  BSLTF_TTF_TYPES14, BSLTF_TTF_TYPE_NAME(15)
#define BSLTF_TTF_TYPES16  BSLTF_TTF_TYPES15, BSLTF_TTF_TYPE_NAME(16)
#define BSLTF_TTF_TYPES17  BSLTF_TTF_TYPES16, BSLTF_TTF_TYPE_NAME(17)
#define BSLTF_TTF_TYPES18  BSLTF_TTF_TYPES17, BSLTF_TTF_TYPE_NAME(18)
#define BSLTF_TTF_TYPES19  BSLTF_TTF_TYPES18, BSLTF_TTF_TYPE_NAME(19)
#define BSLTF_TTF_TYPES20  BSLTF_TTF_TYPES19, BSLTF_TTF_TYPE_NAME(20)

#define BSLTF_TTF_TEST3_N(num)                                                \
clearInvokedFlags();                                                          \
BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(TestHelper,                          \
                                         test3Helper,                         \
                                         BSLTF_TTF_TYPES ## num);             \
checkInvokedFlags(L_, 0, num + 1);

// Test Case 4 Apparatus

template <class TYPE>
void TestHelper<TYPE>::test4Helper()
{
    if (veryVerbose)
        printf("\n==TYPE: %s==\n", NameOf<TYPE>().name());
}

#define BSLTF_TTF_TEST4_SPECIALIZE(type, num)                                 \
template<>                                                                    \
void TestHelper<type>::test4Helper()                                          \
{                                                                             \
    if (veryVerbose)                                                          \
        printf("\n==TYPE: %s==\n", NameOf<type>().name());                    \
    g_invokedFlags[num] = true;                                               \
}

#define BSLTF_TTF_TEST4_NUM_PRIMITIVE_TYPES 6

BSLTF_TTF_TEST4_SPECIALIZE(signed char, 0)
BSLTF_TTF_TEST4_SPECIALIZE(size_t, 1)
BSLTF_TTF_TEST4_SPECIALIZE(const char *, 2)
BSLTF_TTF_TEST4_SPECIALIZE(TemplateTestFacility::ObjectPtr, 3)
BSLTF_TTF_TEST4_SPECIALIZE(TemplateTestFacility::FunctionPtr, 4)
BSLTF_TTF_TEST4_SPECIALIZE(TemplateTestFacility::MethodPtr, 5)

#define BSLTF_TTF_TEST4_NUM_USER_DEFINED_TYPES 10

BSLTF_TTF_TEST4_SPECIALIZE(EnumeratedTestType::Enum, 6)
BSLTF_TTF_TEST4_SPECIALIZE(UnionTestType, 7)
BSLTF_TTF_TEST4_SPECIALIZE(SimpleTestType, 8)
BSLTF_TTF_TEST4_SPECIALIZE(AllocTestType, 9)
BSLTF_TTF_TEST4_SPECIALIZE(BitwiseCopyableTestType, 10)
BSLTF_TTF_TEST4_SPECIALIZE(BitwiseMoveableTestType, 11)
BSLTF_TTF_TEST4_SPECIALIZE(AllocBitwiseMoveableTestType, 12)
BSLTF_TTF_TEST4_SPECIALIZE(MovableTestType, 13)
BSLTF_TTF_TEST4_SPECIALIZE(MovableAllocTestType, 14)
BSLTF_TTF_TEST4_SPECIALIZE(NonTypicalOverloadsTestType, 15)

#define BSLTF_TTF_TEST4_NUM_REGULAR_TYPES                                     \
(BSLTF_TTF_TEST4_NUM_PRIMITIVE_TYPES + BSLTF_TTF_TEST4_NUM_USER_DEFINED_TYPES)

#define BSLTF_TTF_TEST4_NUM_AWKWARD_TYPES 3

BSLTF_TTF_TEST4_SPECIALIZE(NonAssignableTestType, 16)
BSLTF_TTF_TEST4_SPECIALIZE(NonDefaultConstructibleTestType, 17)
BSLTF_TTF_TEST4_SPECIALIZE(NonEqualComparableTestType, 18)

#define BSLTF_TTF_TEST4_NUM_ALL_TYPES                                         \
(BSLTF_TTF_TEST4_NUM_REGULAR_TYPES + BSLTF_TTF_TEST4_NUM_AWKWARD_TYPES)

// BSLTF_TTF_TEST4_SPECIALIZE(MoveOnlyAllocTestType, 8)

// Test Case 5 Apparatus

template <class TYPE>
void TestHelper<TYPE>::test5Helper()
{
    if (veryVerbose)
        printf("\n==TYPE: %s==\n", NameOf<TYPE>().name());

    for (int ti = 0; ti <= 127; ++ti) {
        TYPE X = TemplateTestFacility::create<TYPE>(ti);
        int value = TemplateTestFacility::getIdentifier(X);

        if (veryVeryVerbose) {
            P_(ti) P(value) }

        ASSERTV(ti, value, ti == value);
    }
}

// Test Case 6 Apparatus

template <class TYPE>
void TestHelper<TYPE>::test6Helper()
{
    if (veryVerbose)
        printf("\n==TYPE: %s==\n", NameOf<TYPE>().name());

    bslma::TestAllocator defaultAllocator("defaultAllocator",
                                          veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard defaultGuard(&defaultAllocator);

    bslma::TestAllocator objectAllocator("objectAllocator",
                                         veryVeryVeryVerbose);

    bsls::ObjectBuffer<TYPE> buffer;
    TYPE &mX = buffer.object();
    const TYPE &X = mX;
    TYPE *address = bsls::Util::addressOf(mX);

    for (int ti = 0; ti <= 127; ++ti) {
        TemplateTestFacility::emplace(address, ti, &objectAllocator);
        int value = TemplateTestFacility::getIdentifier(X);

        if (veryVeryVerbose) {
            P_(ti) P(value) }

        ASSERTV(ti, value, ti == value);

        bslma::DestructionUtil::destroy(address);
    }

    ASSERTV(defaultAllocator.numBlocksTotal(),
            0 == defaultAllocator.numBlocksTotal());

}

// Test Case 7 Apparatus

template <class TYPE>
void TestHelper<TYPE>::test7Helper()
{
    bsls::OutputRedirector redirect(bsls::OutputRedirector::e_STDOUT_STREAM,
                                    veryVerbose);

    if (veryVerbose) printf("\n==TYPE: %s==\n", NameOf<TYPE>().name());

    bslma::TestAllocator objectAllocator("objectAllocator",
                                         veryVeryVeryVerbose);

    bsls::ObjectBuffer<TYPE> buffer;
    TYPE &mX = buffer.object();
    const TYPE &X = mX;
    TYPE *address = bsls::Util::addressOf(mX);

    for (int ti = 0; ti <= 127; ++ti) {
        TemplateTestFacility::emplace(address, ti, &objectAllocator);

        if (!verbose) {
            redirect.enable();
        }

        using bsls::debugprint;  // otherwise the name is hidden by this
                                 // component

        debugprint(X);

        BSLS_BSLTESTUTIL_P(X);

        ASSERTV(X, true);

        bslma::DestructionUtil::destroy(address);
    }
}

}  // close unnamed namespace

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int            test = argc > 1 ? atoi(argv[1]) : 0;
                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 8: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        // 1. The usage example provided in the component header file compiles,
        //    links, and runs as shown.
        //
        // Plan:
        // 1. Incorporate usage example from header into test driver, remove
        //    leading comment characters, and replace `assert` with `ASSERT`.
        //    (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        bsls::OutputRedirector redirect(
                                       bsls::OutputRedirector::e_STDOUT_STREAM,
                                       veryVerbose);
        if (!verbose) {
            redirect.enable();
        }

// Now, we can instantiate and call the `TestTemplate::printTypeName` class
// method template for each of the types `int`, `char`, and `double` using the
// `BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE` macro:
// ```
        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(TestTemplate,
                                                 printTypeName,
                                                 int, char, double);
// ```
// Finally, we observe the console output:
// ```
//  int
//  char
//  double
// ```

//
// Finally, we invoke instantiate and call `MyTestDriver::testCase2` for each
// of the types listed in `BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR` using
// the `BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE` macro:
// ```
//    case 2 {
          // ------------------------------------------------------------------
          // DEFAULT CTOR & PRIMARY MANIPULATORS
          // ------------------------------------------------------------------

        if (verbose) printf("\nDEFAULT CTOR & PRIMARY MANIPULATORS"
                            "\n===================================\n");

        RUN_EACH_TYPE(MyTestDriver, testCase2, TEST_TYPES_REGULAR);
//    } break;
// ```
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING `debugprint` FREE FUNCTION
        //
        // Concerns:
        // 1. Ensure that the `debugprint` function overloads are defined for
        //    all supported user-defined types intended for testing defined in
        //    the `bsltf` package.
        //
        // 2. Ensure that the `debugprint` function overloads print the
        //    integer specified to the `TemplateTestFacility::create` class
        //    method used to create the function argument.
        //
        // 3. Ensure that the macros defined in `bsls_bsltestutil` supports the
        //    types for which `debugprint` are defined.
        //
        // Plan:
        // 1. Create an object for each supported user-defined type using
        //    `TemplateTestFacility::create` specifying a unique integer value
        //    for each type.
        //
        // 2. Call `debugprint` for each object created in P-1.  Manually
        //    verify that all integer values used in P-1 are printed on the
        //    console.  (C-1..2)
        //
        // 4. Call the `BSLS_BSLTESTUTIL_P` macro for each object created in
        //    P-1.  Manually verify that all integer values used in P-1 are
        //    printed on the console.  (C-3)
        //
        // Testing:
        //   void debugprint(const EnumeratedTestType::Enum& obj);
        //   void debugprint(const UnionTestType& obj);
        //   void debugprint(const SimpleTestType& obj);
        //   void debugprint(const AllocTestType& obj);
        //   void debugprint(const MovableAllocTestType& obj);
        //   void debugprint(const MoveOnlyAllocTestType& obj);
        //   void debugprint(const BitwiseMoveableTestType& obj);
        //   void debugprint(const AllocBitwiseMoveableTestType& obj);
        //   void debugprint(const NonTypicalOverloadsTestType& obj);
        //   void debugprint(const NonCopyConstructibleTestType& obj);
        //   void debugprint(const NonDefaultConstructibleTestType& obj);
        //   void debugprint(const WellBehavedMoveOnlyAllocTestType& obj);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING `debugprint` FREE FUNCTION"
                            "\n==================================\n");

        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(
                                    TestHelper,
                                    test7Helper,
                                    BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        // The following types are not in `all` yet
        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(
                                    TestHelper,
                                    test7Helper,
                                    bsltf::MoveOnlyAllocTestType,
                                    bsltf::WellBehavedMoveOnlyAllocTestType
                                    );

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) ||               \
    !defined(BSL_DO_NOT_TEST_MOVE_FORWARDING)
        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(
                                    TestHelper,
                                    test7Helper,
//                                    bsltf::AllocEmplacableTestType,
                                    bsltf::EmplacableTestType
                                    );
#endif

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // CLASS METHOD `emplace`
        //   Ensure that the class method `TemplateTestFacility::emplace`
        //   overloads behave according to their contracts.
        //
        // Concerns:
        // 1. For all test types, invoking the `TemplateTestFacility::emplace`
        //    class method passing in an integer value creates a new object of
        //    that type, at the specified address.  Henceforth invoking the
        //    `TemplateTestFacility::getIdentifier` class method passing in the
        //    just created object returns the previously used integer value.
        //
        // 2. For all test types, the `TemplateTestFacility::create` method
        //    supports integer values from 0 to 127.
        //
        // 3. All memory is supplied by the specified allocator.  No memory is
        //    supplied by the default or global allocators.
        //
        // Plan:
        // 1. Create a function member template parameterized on a type and
        //    does the following: For each integer value from 0 to 127, create
        //    an object using `TemplateTestFacility::emplace` passing in the
        //    integer value.  Verify that the integer value return from
        //    `TemplateTestFacility::getIdentifier` compare equal to the value
        //    passed to `create`.
        //
        // 2. Invoke the `BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE` macro
        //    passing in the function member template defined in P-1 and
        //    `BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL` to run the instances
        //    of the template for all test types.  (C-1,2)
        //
        // Testing:
        //   void emplace<TYPE>(TYPE *, int, bslma::Allocator *);
        //   void emplace<TYPE>(TYPE **, int, bslma::Allocator *);
        // --------------------------------------------------------------------

        if (verbose) printf("\nCLASS METHOD `emplace`"
                            "\n======================\n");

        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(
                                    TestHelper,
                                    test6Helper,
                                    BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(
                                    TestHelper,
                                    test6Helper,
                                    bsltf::MoveOnlyAllocTestType,
                                    bsltf::WellBehavedMoveOnlyAllocTestType);

        // The following types are not in `all` yet
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) ||               \
    !defined(BSL_DO_NOT_TEST_MOVE_FORWARDING)
        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(
                                    TestHelper,
                                    test6Helper,
//                                    bsltf::AllocEmplacableTestType,
                                    bsltf::EmplacableTestType
                                    );
#endif

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING CLASS METHODS `create` AND `getIdentifier`
        //   Ensure that the class methods `TemplateTestFacility::create` and
        //   `TemplateTestFacility::getIdentifier` behave according to their
        //   contracts.
        //
        // Concerns:
        // 1. For all test types, invoking the `TemplateTestFacility::create`
        //    class method passing in an integer value return a new object of
        //    that type.  Henceforth invoking the
        //    `TemplateTestFacility::getIdentifier` class method passing in the
        //    just created object returns the previously used integer value.
        //
        // 2. For all test types, the `TemplateTestFacility::create` method
        //    supports integer values from 0 to 127.
        //
        // Plan:
        // 1. Create a function member template parameterized on a type and
        //    does the following: For each integer value from 0 to 127, create
        //    an object using `TemplateTestFacility::create` passing in the
        //    integer value.  Verify that the integer value return from
        //    `TemplateTestFacility::getIdentifier` compare equal to the value
        //    passed to `create`.
        //
        // 2. Invoke the `BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE` macro
        //    passing in the function member template defined in P-1 and
        //    `BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL` to run the instances
        //    of the template for all test types.  (C-1,2)
        //
        // Testing:
        //   void TemplateTestFacility::create(int value);
        //   int TemplateTestFacility::getIdentifier(const TYPE& object);
        // --------------------------------------------------------------------

        if (verbose) printf(
                     "\nTESTING CLASS METHODS `create` AND `getIdentifier`"
                     "\n==================================================\n");

        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(
                                    TestHelper,
                                    test5Helper,
                                    BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        // The following types are not in `all` yet
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(
                                    TestHelper,
                                    test5Helper,
                                    bsltf::MoveOnlyAllocTestType,
//                                    bsltf::AllocEmplacableTestType,
                                    bsltf::EmplacableTestType
                                    );
#elif !defined(BSL_DO_NOT_TEST_MOVE_FORWARDING)
        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(
                                    TestHelper,
                                    test5Helper,
//                                    bsltf::AllocEmplacableTestType,
                                    bsltf::EmplacableTestType
                                    );
#endif
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // `BSLTF_TEMPLATETESTFACILITY_TEST_TYPES*` MACROS
        //   Ensure that the macros defining the list of test types contain all
        //   the types specified their contracts.
        //
        // Concerns:
        // 1. `BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE` contains all of
        //    the primitive test types.
        //
        // 2. `BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_USER_DEFINED` contains all
        //    of the user defined test types.
        //
        // 3. `BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR` contains all of
        //    the regular test types.
        //
        // 4. `BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_AWKWARD` contains all of
        //    the awkward test types.
        //
        // 5. `BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL` contains every test
        //    type.
        //
        // Plan:
        // 1. Create a function member template that specializes on every test
        //    type.  For each type specialization, the template sets a unique
        //    global flag values that is used to indicate whether the template
        //    instance has been invoked.
        //
        // 2. Invoke instances of the template defined in P-1 parameterized on
        //    each type in `BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE`
        //    using `BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE`.  Verify that
        //    the corresponding flags has been set.  (C-1)
        //
        // 3. Invoke instances of the template defined in P-1 parameterized on
        //    each type in `BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_USER_DEFINED`
        //    using `BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE`.  Verify that
        //    the corresponding flags has been set.  (C-2)
        //
        // 4. Invoke instances of the template defined in P-1 parameterized on
        //    each type in `BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR`
        //    using `BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE`.  Verify that
        //    the corresponding flags has been set.  (C-3)
        //
        // 5. Invoke instances of the template defined in P-1 parameterized on
        //    each type in `BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_AWKWARD`
        //    using `BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE`.  Verify that
        //    the corresponding flags has been set.  (C-4)
        //
        // 2. Invoke instances of the template defined in P-1 parameterized on
        //    each type in `BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL` using
        //    `BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE`.  Verify that the
        //    corresponding flags has been set.  (C-5)
        //
        // Testing:
        //   BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE
        //   BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_USER_DEFINED
        //   BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR
        //   BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_AWKWARD
        //   BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL
        // --------------------------------------------------------------------

        if (verbose) printf(
                        "\n'BSLTF_TEMPLATETESTFACILITY_TEST_TYPES*' MACROS"
                        "\n===============================================\n");

        if (verbose)
           printf("\nTest BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE.\n");

        clearInvokedFlags();
        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(TestHelper, test4Helper,
                            BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
        checkInvokedFlags(L_, 0, BSLTF_TTF_TEST4_NUM_PRIMITIVE_TYPES);


        if (verbose)
        printf("\nTest BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_USER_DEFINED.\n");

        clearInvokedFlags();
        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(
                           TestHelper,
                           test4Helper,
                           BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_USER_DEFINED);
        checkInvokedFlags(L_,
                          BSLTF_TTF_TEST4_NUM_PRIMITIVE_TYPES,
                          BSLTF_TTF_TEST4_NUM_USER_DEFINED_TYPES);

        if (verbose)
            printf("\nTest BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR.\n");

        clearInvokedFlags();
        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(TestHelper,
                            test4Helper,
                            BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
        checkInvokedFlags(L_, 0, BSLTF_TTF_TEST4_NUM_REGULAR_TYPES);

        if (verbose)
            printf("\nTest BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_AWKWARD.\n");

        clearInvokedFlags();
        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(TestHelper,
                            test4Helper,
                            BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_AWKWARD);
        checkInvokedFlags(L_,
                          BSLTF_TTF_TEST4_NUM_REGULAR_TYPES,
                          BSLTF_TTF_TEST4_NUM_AWKWARD_TYPES);

        if (verbose)
            printf("\nTest BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL.\n");

        clearInvokedFlags();
        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(TestHelper,
                            test4Helper,
                            BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);
        checkInvokedFlags(L_, 0, BSLTF_TTF_TEST4_NUM_ALL_TYPES);

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // MACRO `BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE`
        //   Ensure that the `BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE` macro
        //   behave according to its contract.
        //
        // Concerns:
        // 1. `BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE` invokes instances of a
        //    function member template each parameterized on a type from a
        //    specified list of types.
        //
        // 2. Up to 20 types are supported in the argument list of the macro.
        //
        // 3. Compilation will fail if more than 20 types are specified in the
        //    macro.
        //
        // Plan:
        // 1. Define 20 types, each with a method, `value`, that returns an
        //    unique integer value.
        //
        // 2. Define an global array of 20 flags to indicate if a specific
        //    instance of a template has been invoked.
        //
        // 3. Create a function member template that sets a value in the array
        //    defined by P-2 at the identifier returned by the `value` method
        //    of the parameterized type.
        //
        // 4. Invoke `BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE`, passing the
        //    function member template defined in P-3, and the list of types
        //    defined in P-1.  Verify that all the corresponding flags in the
        //    array defined by P-2 have been set.  (C-1..2)
        //
        // 5. Manually verify that specified a list of 21 types will cause the
        //    compilation to fail.
        //
        // Testing:
        //   BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nMACRO `BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE`"
                   "\n================================================\n");

        BSLTF_TTF_TEST3_N(0);
        BSLTF_TTF_TEST3_N(1);
        BSLTF_TTF_TEST3_N(2);
        BSLTF_TTF_TEST3_N(3);
        BSLTF_TTF_TEST3_N(4);
        BSLTF_TTF_TEST3_N(5);
        BSLTF_TTF_TEST3_N(6);
        BSLTF_TTF_TEST3_N(7);
        BSLTF_TTF_TEST3_N(8);
        BSLTF_TTF_TEST3_N(9);
        BSLTF_TTF_TEST3_N(10);
        BSLTF_TTF_TEST3_N(11);
        BSLTF_TTF_TEST3_N(12);
        BSLTF_TTF_TEST3_N(13);
        BSLTF_TTF_TEST3_N(14);
        BSLTF_TTF_TEST3_N(15);
        BSLTF_TTF_TEST3_N(16);
        BSLTF_TTF_TEST3_N(17);
        BSLTF_TTF_TEST3_N(18);
        BSLTF_TTF_TEST3_N(19);
        // BSLTF_TTF_TEST3_N(20);  // This will not compile.

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // ALIASES
        //
        // Concerns:
        // 1. The `typedef` aliases defined in this component is of the correct
        //    type.
        //
        // Testing:
        //   CONCERN: Aliases provided are of the correct type.
        // --------------------------------------------------------------------

        if (verbose) printf("\nALIASES"
                            "\n=======\n");

          BSLMF_ASSERT((bsl::is_same<TemplateTestFacility_StubClass*,
                                     TemplateTestFacility::ObjectPtr>::value));

          BSLMF_ASSERT((bsl::is_same<
                                   void (*) (),
                                   TemplateTestFacility::FunctionPtr>::value));

          BSLMF_ASSERT((bsl::is_same<
                                    int (TemplateTestFacility_StubClass::*) (),
                                    TemplateTestFacility::MethodPtr>::value));

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        // 1. The class is sufficiently functional to enable comprehensive
        //    testing in subsequent test cases.
        //
        // Plan:
        // 1. Perform and ad-hoc test of the primary modifiers and accessors.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(TestHelper,
                            breathingTest,
                            BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2017 Bloomberg Finance L.P.
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
