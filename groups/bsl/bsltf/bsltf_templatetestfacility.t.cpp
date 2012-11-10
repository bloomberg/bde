// bsltf_templatetestfacility.t.cpp                                   -*-C++-*-
#include <bsltf_templatetestfacility.h>

#include <bslma_testallocator.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_default.h>

#include <bslmf_assert.h>
#include <bslmf_issame.h>

#include <bsls_assert.h>
#include <bsls_bsltestutil.h>

#include <stdio.h>
#include <stdlib.h>
#include <typeinfo>

using namespace BloombergLP;
using namespace BloombergLP::bsltf;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test provides a namespace, 'TemplateTestFacility', and a
// set of macros that facilitate the testing of templates with parameterized
// types.
//
//-----------------------------------------------------------------------------
// CLASS METHODS
// [ 5] void TemplateTestFacility::create(int value);
// [ 5] int TemplateTestFacility::getIndex(const TYPE& object);
//
// FREE FUNCTIONS
// [ 6] void debugprint(const EnumeratedTestType::Enum& obj);
// [ 6] void debugprint(const UnionTestType& obj);
// [ 6] void debugprint(const SimpleTestType& obj);
// [ 6] void debugprint(const AllocTestType& obj);
// [ 6] void debugprint(const BitwiseMoveableTestType& obj);
// [ 6] void debugprint(const AllocBitwiseMoveableTestType& obj);
// [ 6] void debugprint(const NonTypicalOverloadsTestType& obj);
// [ 6] void debugprint(const NonDefaultConstructibleTestType& obj);
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
// [ 7] USAGE EXAMPLE
// [ 2] ALIASES
//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
static int testStatus = 0;

namespace {

void aSsErT(bool b, const char *s, int i) {
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

static int verbose, veryVerbose, veryVeryVerbose, veryVeryVeryVerbose;

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using the 'BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE' Macro
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we demonstrate how to use
// 'BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE' to call a class method template
// for a list of types.
//
// First, we define a 'struct' template 'TestTemplate' taking in a
// parameterized 'TYPE' that has a class method, 'printTypeName':
//..
template <class TYPE>
struct TestTemplate {
    // This 'struct' provides a namespace for a simple test method.

    // CLASS METHODS
    static void printTypeName();
        // Prints the name of the parameterized 'TYPE' to the console.
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
//..

///Example 2: Writing a Type Independent Test Driver
///- - - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we demonstrate using the 'TemplateTestFacility' 'struct'
// and the macros provided by this component to test the default constructor
// and primary manipulator of a class template in the context of a typical
// BDE-style test driver.  Note that a goal of the demonstrated test is to
// validate the class template with a broad range of types emulating those with
// which the template might be instantiated.
//
// First, we define a simple class template, 'MyNullableValue', that we will
// later need to test:
//..
template <class TYPE>
class MyNullableValue {
    // This (value-semantic) class template extends the parameterized
    // 'TYPE' to include the notion of a "null" value.

    // DATA
    TYPE d_value;     // non-null value
    bool d_nullFlag;  // flag to indicate if the value is null

  public:
    // CREATORS
    MyNullableValue()
    // Create a 'MyNullableValue' that initially has a value of null.
    : d_nullFlag(true)
    {
    }

    // MyNullableValue(const MyNullableValue& original) = default;
        // Create a 'MyNullableValue' object having the same value as the
        // specified 'original' object.

    // ~MyNullableValue() = default;
        // Destroy this object.

    // MANIPULATORS
    // MyNullableValue& operator=(const MyNullableValue& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    void makeNull()
    // Set this object to the null value.
    {
        d_nullFlag = true;
    }

    void makeValue(const TYPE& value)
    // Set the value of this object to be that of the specified 'value'
    // of the parameterized 'TYPE'.
    {
        d_nullFlag = false;
        d_value = value;
    }

    // ACCESSORS
    bool isNull() const
    // Return 'true' if this object is null, and 'false' otherwise.
    {
        return d_nullFlag;
    }

    const TYPE& value() const {
        // Return a reference providing non-modifiable access to the
        // underlying object of the parameterized 'TYPE'.  The behavior is
        // undefined if the object is null.
        return d_value;
    }
};
//..
// Then, we define some aliases for the micros that will be used by the test
// driver:
//..
#define RUN_EACH_TYPE BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE
#define TEST_TYPES_REGULAR BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR
//..
// Next, we define a 'struct' template, 'MyTestDriver', that provides a
// namespace containing the test cases (here, only 'testCase2' is defined for
// brevity) of the test driver:
//..
template <class TYPE>
struct MyTestDriver {
    // This 'struct' provides a namespace for the class methods used to
    // implement the test driver.

    // TYPES
    typedef MyNullableValue<TYPE> Obj;
        // This 'typedef' provides an alias to the type under testing.

    static void testCase2();
        // Test primary manipulators.

};
//..
// Now, we define the implementation of 'MyTestDriver::testCase2':
//..
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
    //: 1 An object created using the default constructor (with or without
    //:   a supplied allocator) has the contractually specified value.
    //:
    //: 2 The 'makeValue' method sets the value of a object to any specified
    //:   value.
    //:
    //: 3 The 'makeNull' method set the value of a object to null.
    //:
    //: 4 Objects of different values can coexist.
    //:
    //: 5 The destructor does not modify other objects.
    //
    // Plan:
    //: 1 Default-construct an object and use the (as yet unproven) salient
    //:   attribute accessors to verify that the value of the object is the
    //:   null value.  (C-1)
    //:
    //: 2 Default-construct another object, and use the 'makeValue' method,
    //:   passing in an object created with the 'TemplateTestFacility::create'
    //:   class method template, to set the value of the object to a non-null
    //:   value.  Use the (as yet unproven) salient attribute accessors and the
    //:   'TemplateTestFacility::getIdentifier' class method template to verify
    //:   that the new object the expected value and the object created in P-1
    //:   still has the same value.  (C-2, 4)
    //:
    //: 3 Using the loop-based approach, for each identifier in a range of
    //:   integer identifiers:
    //:
    //:   1 Default-construct a modifiable object, 'mL', and use the (as yet
    //:     unproven) salient attribute accessors to verify the value of the
    //:     default constructed object is the null value.  (C-1)
    //:
    //:   2 Create an object of the parameterized 'TYPE', 'LV', using the
    //:     'TemplateTestFacility::create' class method template, specifying
    //:     the integer loop identifier.
    //:
    //:   3 Use the 'makeValue' method to set the value of 'mL' to 'LV'.  Use
    //:     the (as yet unproven) salient attribute accessors and the
    //:     'TemplateTestFacility::getIdentifier' class method template to verify
    //:     'mL' has the expected value.  (C-2)
    //:
    //:   4 Invoke the 'makeNull' method of 'mL'.  Use the attribute
    //:     accessors to verify the value of the object is now null.  (C-3)
    //:
    //: 4 Create an object in a nested block.  Below the block, verify the
    //:   objects created in P-1 and P-2 still have the same value.  (C-5)
    //
    // Testing:
    //   MyNullableValue();
    //   ~MyNullableValue();
    //   void makeNull();
    //   void MakeValue(const TYPE& value);
    // --------------------------------------------------------------------

    if (verbose) printf("\nDEFAULT CTOR, PRIMARY MANIPULATORS, AND DTOR"
                        "\n============================================\n");


    if (verbose) printf("\nTesting default constructor.\n");

    Obj mW; const Obj& W = mW;
    ASSERT(true == W.isNull());

    Obj mX; const Obj& X = mX;
    const TYPE XV = TemplateTestFacility::create<TYPE>(1);
    mX.makeValue(XV);
    ASSERT(1 == TemplateTestFacility::getIdentifier<TYPE>(X.value()));

    if (verbose) printf("\nTesting primary manipulators.\n");

    for (size_t ti = 0; ti < 10; ++ti) {

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
//..
// Notice that, we create objects of the parameterized 'TYPE' using the
// 'TemplateTestFacility::create' class method template specifying an integer
// identifier; the created object has a value that is uniquely associated with
// the integer identifier.
//
// Also notice that we verified that an object of the parameterized 'TYPE' has
// the expected value in two ways:
//
//: 1 By equal comparing (1) the integer identifier returned from calling the
//:   'TemplateTestFacility::getIdentifier' class method template (specifying
//:   the object), and (2) the integer identifier uniquely associated with the
//:   expected value of the object.
//:
//: 2 By directly using the equality comparison operator for the parameterized
//:   'TYPE'.  In general, the equality comparison operator is defined for all
//:   types intended for testing in the 'bsltf' package unless specified
//:   otherwise (e.g., bsltf::NonEqualComparableTestType).

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
};

template <class TYPE>
void TestHelper<TYPE>::breathingTest()
{
    if (veryVerbose)
        printf("\n==TYPE: %s==\n", typeid(TYPE).name());

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
        printf("\n==TYPE: %s==\n", typeid(TYPE).name());

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

#define BSLTF_TTF_TYPES0  BSLTF_TTF_TYPE_NAME(0)                              \

#define BSLTF_TTF_TYPES1  BSLTF_TTF_TYPES0                                    \
    ,BSLTF_TTF_TYPE_NAME(1)

#define BSLTF_TTF_TYPES2  BSLTF_TTF_TYPES1                                    \
    ,BSLTF_TTF_TYPE_NAME(2)

#define BSLTF_TTF_TYPES3  BSLTF_TTF_TYPES2                                    \
    ,BSLTF_TTF_TYPE_NAME(3)

#define BSLTF_TTF_TYPES4  BSLTF_TTF_TYPES3                                    \
    ,BSLTF_TTF_TYPE_NAME(4)

#define BSLTF_TTF_TYPES5  BSLTF_TTF_TYPES4                                    \
    ,BSLTF_TTF_TYPE_NAME(5)

#define BSLTF_TTF_TYPES6  BSLTF_TTF_TYPES5                                    \
    ,BSLTF_TTF_TYPE_NAME(6)

#define BSLTF_TTF_TYPES7  BSLTF_TTF_TYPES6                                    \
    ,BSLTF_TTF_TYPE_NAME(7)

#define BSLTF_TTF_TYPES8  BSLTF_TTF_TYPES7                                    \
    ,BSLTF_TTF_TYPE_NAME(8)

#define BSLTF_TTF_TYPES9  BSLTF_TTF_TYPES8                                    \
    ,BSLTF_TTF_TYPE_NAME(9)

#define BSLTF_TTF_TYPES10 BSLTF_TTF_TYPES9                                    \
    ,BSLTF_TTF_TYPE_NAME(10)

#define BSLTF_TTF_TYPES11  BSLTF_TTF_TYPES10                                  \
    ,BSLTF_TTF_TYPE_NAME(11)

#define BSLTF_TTF_TYPES12  BSLTF_TTF_TYPES11                                  \
    ,BSLTF_TTF_TYPE_NAME(12)

#define BSLTF_TTF_TYPES13  BSLTF_TTF_TYPES12                                  \
    ,BSLTF_TTF_TYPE_NAME(13)

#define BSLTF_TTF_TYPES14  BSLTF_TTF_TYPES13                                  \
    ,BSLTF_TTF_TYPE_NAME(14)

#define BSLTF_TTF_TYPES15  BSLTF_TTF_TYPES14                                  \
    ,BSLTF_TTF_TYPE_NAME(15)

#define BSLTF_TTF_TYPES16  BSLTF_TTF_TYPES15                                  \
    ,BSLTF_TTF_TYPE_NAME(16)

#define BSLTF_TTF_TYPES17  BSLTF_TTF_TYPES16                                  \
    ,BSLTF_TTF_TYPE_NAME(17)

#define BSLTF_TTF_TYPES18  BSLTF_TTF_TYPES17                                  \
    ,BSLTF_TTF_TYPE_NAME(18)

#define BSLTF_TTF_TYPES19  BSLTF_TTF_TYPES18                                  \
    ,BSLTF_TTF_TYPE_NAME(19)

#define BSLTF_TTF_TYPES20  BSLTF_TTF_TYPES19                                  \
    ,BSLTF_TTF_TYPE_NAME(20)

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
        printf("\n==TYPE: %s==\n", typeid(TYPE).name());
}

#define BSLTF_TTF_TEST4_SPECIALIZE(type, num)                                 \
template<>                                                                    \
void TestHelper<type>::test4Helper()                                          \
{                                                                             \
    if (veryVerbose)                                                          \
        printf("\n==TYPE: %s==\n", typeid(type).name());                      \
    g_invokedFlags[num] = true;                                               \
}

#define BSLTF_TTF_TEST4_NUM_PRIMITIVE_TYPES 5

BSLTF_TTF_TEST4_SPECIALIZE(signed char, 0)
BSLTF_TTF_TEST4_SPECIALIZE(size_t, 1)
BSLTF_TTF_TEST4_SPECIALIZE(TemplateTestFacility::ObjectPtr, 2)
BSLTF_TTF_TEST4_SPECIALIZE(TemplateTestFacility::FunctionPtr, 3)
BSLTF_TTF_TEST4_SPECIALIZE(TemplateTestFacility::MethodPtr, 4)

#define BSLTF_TTF_TEST4_NUM_USER_DEFINED_TYPES 7

BSLTF_TTF_TEST4_SPECIALIZE(EnumeratedTestType::Enum, 5)
BSLTF_TTF_TEST4_SPECIALIZE(UnionTestType, 6)
BSLTF_TTF_TEST4_SPECIALIZE(SimpleTestType, 7)
BSLTF_TTF_TEST4_SPECIALIZE(AllocTestType, 8)
BSLTF_TTF_TEST4_SPECIALIZE(BitwiseMoveableTestType, 9)
BSLTF_TTF_TEST4_SPECIALIZE(AllocBitwiseMoveableTestType, 10)
BSLTF_TTF_TEST4_SPECIALIZE(NonTypicalOverloadsTestType, 11)

#define BSLTF_TTF_TEST4_NUM_REGULAR_TYPES                                     \
(BSLTF_TTF_TEST4_NUM_PRIMITIVE_TYPES + BSLTF_TTF_TEST4_NUM_USER_DEFINED_TYPES)

#define BSLTF_TTF_TEST4_NUM_AWKWARD_TYPES 3

BSLTF_TTF_TEST4_SPECIALIZE(NonAssignableTestType, 12)
BSLTF_TTF_TEST4_SPECIALIZE(NonDefaultConstructibleTestType, 13)
BSLTF_TTF_TEST4_SPECIALIZE(NonEqualComparableTestType, 14)

#define BSLTF_TTF_TEST4_NUM_ALL_TYPES                                         \
(BSLTF_TTF_TEST4_NUM_REGULAR_TYPES + BSLTF_TTF_TEST4_NUM_AWKWARD_TYPES)

// Test Case 5 Aparatus

template <class TYPE>
void TestHelper<TYPE>::test5Helper()
{
    if (veryVerbose)
        printf("\n==TYPE: %s==\n", typeid(TYPE).name());

    for (int ti = 0; ti <= 127; ++ti) {
        TYPE X = TemplateTestFacility::create<TYPE>(ti);
        int value = TemplateTestFacility::getIdentifier<TYPE>(X);

        if (veryVeryVerbose) {
            P_(ti) P(value) }

        ASSERTV(ti, value, ti == value);
    }
}

} // close unnamed namespace

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {
    int test            = argc > 1 ? atoi(argv[1]) : 0;
    verbose             = argc > 2;
    veryVerbose         = argc > 3;
    veryVeryVerbose     = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 7: {
          if (verbose) printf("\nUSAGE EXAMPLE"
                              "\n=============\n");

// Now, we can instantiate and call the 'TestTemplate::printTypeName' class
// method template for each of the types 'int', 'char', and 'double' using the
// 'BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE' macro:
//..
          BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(TestTemplate,
                                                   printTypeName,
                                                   int, char, double);
//..
// Finally, we observe the console output:
//..
//  int
//  char
//  double
//..

//
// Finally, we invoke instantiate and call 'MyTestDriver::testCase2' for each
// of the types listed in 'BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR' using
// the 'BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE' macro:
//..
//    case 2 {
          // ------------------------------------------------------------------
          // DEFAULT CTOR & PRIMARY MANIPULATORS
          // ------------------------------------------------------------------

          if (verbose) printf("\nDEFAULT CTOR & PRIMARY MANIPULATORS"
                              "\n===================================\n");

          RUN_EACH_TYPE(MyTestDriver, testCase2, TEST_TYPES_REGULAR);
//    } break;
//..
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // 'debugprint' FREE FUNCTION
        //
        // Concerns:
        //: 1 Ensure that the 'debugprint' function overloads are defined for
        //:   all supported user-defined types intended for testing defined in
        //:   the 'bsltf' package.
        //:
        //: 2 Enusre that the 'debugprint' function overloads print the integer
        //:   specified to the 'TemplateTestFacility::create' class method used
        //:   to create the function argument.
        //:
        //: 3 Ensure that the macros defined in 'bsls_bsltestutil' supports the
        //:   types for which 'debugprint' are defined.
        //
        // Plan:
        //: 1 Create an object for each supported user-defined type using
        //:   'TemplateTestFacility::create' specifying a unique integer value
        //:   for each type.
        //:
        //: 2 Call 'debugprint' for each object created in P-1.  Manually
        //:   verify that all integer values used in P-1 are printed on the
        //:   console.  (C-1..2)
        //:
        //: 4 Call the 'BSLS_BSLTESTUTIL_P' macro for each object created in
        //:   P-1.  Manually verify that all integer values used in P-1 are
        //:   printed on the console.  (C-3)
        //
        // Testing:
        //   void debugprint(const EnumeratedTestType::Enum& obj);
        //   void debugprint(const UnionTestType& obj);
        //   void debugprint(const SimpleTestType& obj);
        //   void debugprint(const AllocTestType& obj);
        //   void debugprint(const BitwiseMoveableTestType& obj);
        //   void debugprint(const AllocBitwiseMoveableTestType& obj);
        //   void debugprint(const NonTypicalOverloadsTestType& obj);
        //   void debugprint(const NonDefaultConstructibleTestType& obj);
        // --------------------------------------------------------------------

        EnumeratedTestType::Enum o1 =
                     TemplateTestFacility::create<EnumeratedTestType::Enum>(1);

        UnionTestType o2 = TemplateTestFacility::create<UnionTestType>(2);

        SimpleTestType o3 = TemplateTestFacility::create<SimpleTestType>(3);

        AllocTestType o4 = TemplateTestFacility::create<AllocTestType>(4);

        BitwiseMoveableTestType o5 =
                      TemplateTestFacility::create<BitwiseMoveableTestType>(5);

        AllocBitwiseMoveableTestType o6 =
                 TemplateTestFacility::create<AllocBitwiseMoveableTestType>(6);

        NonTypicalOverloadsTestType o7 =
                  TemplateTestFacility::create<NonTypicalOverloadsTestType>(7);

        NonDefaultConstructibleTestType o8 =
              TemplateTestFacility::create<NonDefaultConstructibleTestType>(8);

        debugprint(o1);
        debugprint(o2);
        debugprint(o3);
        debugprint(o4);
        debugprint(o5);
        debugprint(o6);
        debugprint(o7);
        debugprint(o8);

        BSLS_BSLTESTUTIL_P(o1);
        BSLS_BSLTESTUTIL_P(o2);
        BSLS_BSLTESTUTIL_P(o3);
        BSLS_BSLTESTUTIL_P(o4);
        BSLS_BSLTESTUTIL_P(o5);
        BSLS_BSLTESTUTIL_P(o6);
        BSLS_BSLTESTUTIL_P(o7);
        BSLS_BSLTESTUTIL_P(o8);

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // CLASS METHODS 'create' and 'getIdentifier'
        //   Ensure that the class methods 'TemplateTestFacility::create' and
        //   'TemplateTestFacility::getIdentifier' behave according to their
        //   contracts.
        //
        // Concerns:
        //: 1 For all test types, invoking the 'TemplateTestFacility::create'
        //:   class method passing in an integer value return a new object of
        //:   that type.  Henceforce invoking the
        //:   'TemplateTestFacility::getIdentifier' class method passing in the
        //:   just created object returns the the previously used integer
        //:   value.
        //:
        //: 2 For all test types, the 'TemplateTestFacility::create' method
        //:   supports integer values from 0 to 127.
        //
        // Plan:
        //: 1 Create a function member template parameterized on a type and
        //:   does teh following: For each integer value from 0 to 127, create
        //:   an object using 'TemplateTestFacility::create' passing in the
        //:   integer value.  Verify that the integer value return from
        //:   'TemplateTestFacility::getIdentifier' compare equal to the value
        //:   passed to 'create'.
        //:
        //: 2 Invoke the 'BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE' macro
        //:   passing in the function member template defined in P-1 and
        //:   'BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL' to run the instances
        //:   of the template for all test types.  (C-1,2)
        //
        // Testing:
        //   void TemplateTestFacility::create(int value);
        //   int TemplateTestFacility::getIdentifier(const TYPE& object);
        // --------------------------------------------------------------------
        if (verbose) printf("\nCLASS METHODS 'create' and 'getIdentifier'"
                            "\n=====================================\n");

        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(
                                    TestHelper,
                                    test5Helper,
                                    BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // 'BSLTF_TEMPLATETESTFACILITY_TEST_TYPES*' MACROS
        //    Ensure that the macros defining the list of test types contain
        //    all the types specified their contracts.
        //
        // Concerns:
        //: 1 'BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE' contains all of
        //:   the primitive test types.
        //:
        //: 2 'BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_USER_DEFINED' contains all
        //:   of the user defined test types.
        //:
        //: 3 'BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR' contains all of
        //:   the regular test types.
        //:
        //: 4 'BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_AWKWARD' contains all of
        //:   the awkward test types.
        //:
        //: 5 'BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL' contains every test
        //:   type.
        //
        // Plan:
        //: 1 Create a function member template that specializes on every test
        //:   type.  For each type specialization, the template sets a unique
        //:   global flag values that is used to indicate whether the template
        //:   instance has been invoked.
        //:
        //: 2 Invoke instances of the template defined in P-1 parameterized on
        //:   each type in 'BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE'
        //:   using 'BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE'.  Verify that
        //:   the corresponding flags has been set.  (C-1)
        //:
        //: 3 Invoke instances of the template defined in P-1 parameterized on
        //:   each type in 'BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_USER_DEFINED'
        //:   using 'BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE'.  Verify that
        //:   the corresponding flags has been set.  (C-2)
        //:
        //: 4 Invoke instances of the template defined in P-1 parameterized on
        //:   each type in 'BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR'
        //:   using 'BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE'.  Verify that
        //:   the corresponding flags has been set.  (C-3)
        //:
        //: 5 Invoke instances of the template defined in P-1 parameterized on
        //:   each type in 'BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_AWKWARD'
        //:   using 'BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE'.  Verify that
        //:   the corresponding flags has been set.  (C-4)
        //:
        //: 2 Invoke instances of the template defined in P-1 parameterized on
        //:   each type in 'BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL' using
        //:   'BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE'.  Verify that the
        //:   corresponding flags has been set.  (C-5)
        //
        // Testing:
        //   BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE
        //   BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_USER_DEFINED
        //   BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR
        //   BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_AWKWARD
        //   BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL
        // --------------------------------------------------------------------

        if (verbose) printf("\n'BSLTF_TEMPLATETESTFACILITY_TEST_TYPES*' MACROS"
                            "\n==========================\n");

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
        // MACRO 'BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE' Ensure that the
        // 'BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE' macro behave according to
        // its contract.
        //
        // Concerns:
        //: 1 'BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE' invokes instances of a
        //:   function member template each parameterized on a type from a
        //:   specified list of types.
        //:
        //: 2 Up to 20 types are supported in the argument list of the macro.
        //:
        //: 3 Compilation will fail if more than 20 types are specified in the
        //:   macro.
        //
        // Plan:
        //: 1 Define 20 types, each with a method, 'value', that returns an
        //:   unique integer value.
        //:
        //: 2 Define an global array of 20 flags to indicate if a specific
        //:   instance of a template has been invoked.
        //:
        //: 3 Create a function member template that sets a value in the array
        //:   defined by P-2 at the identifier returned by the 'value' method
        //:   of the parameterized type.
        //:
        //: 4 Invoke 'BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE', passing the
        //:   function member template defined in P-3, and the list of types
        //:   defined in P-1.  Verify that all the corresponding flags in the
        //:   array defined by P-2 have been set.  (C-1..2)
        //:
        //: 5 Manually verify that specified a list of 21 types will cause the
        //:   compilation to fail.
        //
        // Testing:
        //   BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nMACRO 'BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE'"
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
        //: 1 The 'typedef' aliases defined in this component is of the correct
        //:   type.
        //
        // Testing:
        //   CONCERN: Aliases provided are of the correct type.
        // --------------------------------------------------------------------

        if (verbose) printf("\nALIASES"
                            "\n=======\n");

          BSLMF_ASSERT((bslmf::IsSame<TemplateTestFacility_StubClass*,
                                     TemplateTestFacility::ObjectPtr>::VALUE));

          BSLMF_ASSERT((bslmf::IsSame<
                                   void (*) (),
                                   TemplateTestFacility::FunctionPtr>::VALUE));

          BSLMF_ASSERT((bslmf::IsSame<
                                    int (TemplateTestFacility_StubClass::*) (),
                                    TemplateTestFacility::MethodPtr>::VALUE));

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Perform and ad-hoc test of the primary modifiers and accessors.
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
// Copyright (C) 2012 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
