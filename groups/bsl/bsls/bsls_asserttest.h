// bsls_asserttest.h                                                  -*-C++-*-
#ifndef INCLUDED_BSLS_ASSERTTEST
#define INCLUDED_BSLS_ASSERTTEST

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a test facility for assertion macros.
//
//@CLASSES:
//  bsls::AssertTest: namespace for "assert" validating functions
//  bsls::AssertTestHandlerGuard: guard for the negative testing assert-handler
//
//@MACROS:
//  BSLS_ASSERTTEST_ASSERT_PASS(EXPRESSION): macro success expected
//  BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPRESSION): "safe" macro failure expected
//  BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPRESSION): no origination check
//  BSLS_ASSERTTEST_ASSERT_FAIL(EXPRESSION): macro failure expected
//  BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPRESSION): no origination check
//  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPRESSION): "opt" macro failure expected
//  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPRESSION): no origination check
//  BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPRESSION): macro success expected
//  BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPRESSION): macro success expected
//  BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPRESSION): macro success expected
//  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPRESSION): macro success expected
//  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPRESSION): macro success expected
//
//@SEE_ALSO: bsls_assert, bsls_asserttestexception
//
//@AUTHOR: Alisdair Meredith (ameredit)
//
//@DESCRIPTION: This component provides a facility to test that 'BSLS_ASSERT_*'
// macros are used as intended, in the appropriate build modes, and have the
// expected effects.  The class 'bsls::AssertTest' provides a small set of
// static methods that can be used to support detailed test cases, especially
// in table-driven test scenarios.  Additionally, a set of macros automate use
// of these methods to support simple testing of single expressions.
//
// A testing-specific handler guard, 'bsls::AssertTestHandlerGuard', is also
// provided to be used wherever the 'BSLS_ASSERTTEST_*' macros are used.
//
///Negative Testing
///----------------
// "Negative testing" is the principle of testing for a negative result, which
// implies the function under test must fail in some way.  Testable failures
// typically occur when a function is called with values outside the defined
// contract: a well-implemented function will validate function arguments, in
// appropriate build modes, using the various 'BSLS_ASSERT' macros (see
// 'bsls_assert').  When a function fails as a result of an assertion, the
// default behavior is to terminate the program.  However, the 'bsls_assert'
// facility allows a user-supplied assertion-failure handler function to be
// installed, which can be used to build a test facility for expected
// assertions.
//
// One important issue to be aware of with negative testing is that you are
// testing undefined behavior within a program.  For the purpose of the test
// driver, the behavior of calling a function outside its contract is well-
// defined if it is guarded by assertions that are active in the current build
// mode.  However, it is important that those tests are not run if the assert
// macros are not active, otherwise truly undefined behavior will result, with
// potentially disastrous consequences.
//
///The Test Facility
///-----------------
//
///Installing the Assert-Failure Handler
///- - - - - - - - - - - - - - - - - - -
// The function 'bsls::AssertTest::failTestDriver' (and the parallel function
// 'bsls::AssertTest::failTestDriverByReview') is provided as the basis for a
// negative testing facility.  It can act as an assertion-failure handler
// function that throws an exception, of type 'bsls::AssertTestException',
// containing the text of the failed assertion, the name of the file where it
// triggered, and the relevant line number within that file.  The filename can
// be tested to ensure that the assertion was raised by the component under
// test, rather than by some deeper implementation detail as a consequence of
// the expected assertion not being present in the function under test.
//
// Once the function 'bsls::AssertTest::failTestDriver' has been registered as
// the active assertion-failure handler, a set of testing macros automate much
// of the boilerplate code involved in writing a negative test, so that a test
// can be effectively written as a single line.  This is an important quality
// for reading tests, to clearly see the test logic in action without being
// distracted by the surrounding machinery.
//
///Basic Test Macros
///- - - - - - - - -
// The four basic test macros are
//: o 'BSLS_ASSERTTEST_ASSERT_PASS'
//: o 'BSLS_ASSERTTEST_ASSERT_SAFE_FAIL'
//: o 'BSLS_ASSERTTEST_ASSERT_FAIL'
//: o 'BSLS_ASSERTTEST_ASSERT_OPT_FAIL'
// Each of these macros takes a single expression as an argument, tests whether
// an assertion is raised while evaluating that expression, and, if an
// assertion is both raised and expected, whether that assertion was raised by
// the component under test.
//
// A test failure is indicated by invoking 'ASSERT(EXPRESSION)', where 'ASSERT'
// is either a macro or function that must be defined by the test driver, and
// 'EXPRESSION' is an expression that evaluates to 'true' or 'false' according
// to whether the 'ASSERTTEST_ASSERT' macro was expected to '_PASS' or '_FAIL'.
//
// For example, if we have 'std::vector<int> v' and 'v' is empty, then the
// macro test 'BSLS_ASSERTTEST_ASSERT_SAFE_FAIL((v.back()))' will fail when the
// effective assertion-level is 'BSLS_ASSERT_LEVEL_ASSERT_SAFE' unless an
// assertion is raised.  However, if the assertion-level is not
// 'BSLS_ASSERT_LEVEL_ASSERT_SAFE', then the test will not be run.
//
///Raw Test Macros
///- - - - - - - -
// The three "raw" test macros are
//: o 'BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW'
//: o 'BSLS_ASSERTTEST_ASSERT_FAIL_RAW'
//: o 'BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW'
// These testing macros perform the same test as the corresponding basic
// testing macros, except that there is no check to confirm that the assertion
// originated in the component under test.
//
///Enabling Negative Testing
///- - - - - - - - - - - - -
// In order to enable the negative testing facility, you must:
//: o '#include' this component header, 'bsls_asserttest.h'.
//: o Supply an implementation of an 'ASSERT' macro in your test driver.
//: o Register 'bsls::AssertTest::failTestDriver' as the active
//:   assertion-failure handler.
//
///Validating Disabled Macro Expressions
///- - - - - - - - - - - - - - - - - - -
// An additional external macro, 'BSLS_ASSERTTEST_VALIDATE_DISABLED_MACROS',
// can be defined to control the compile time behavior of 'bsls_asserttest'.
// Enabling this macro configures all *disabled* asserttest macros to still
// instantiate their expressions (in a non-evaluated context) to be sure that
// the expression is still syntactically valid.  This can be used to ensure
// tests that are rarely enabled have valid expressions.
//
///Validating Macro Testing Levels
///- - - - - - - - - - - - - - - -
// Another external macro, 'BSLS_ASSERTTEST_CHECK_LEVEL', can be used to add an
// additional check that the assertion that fails is of the same level or
// narrower than the macro testing the assertion.  This will ensure that in all
// build modes where the assertion is enabled the test for that assertion will
// also be enabled.
//
///Addtional Test Pass Macros
/// - - - - - - - - - - - - -
// Five additional 'PASS' macros exist to parallel the remaining 'FAIL' macros.
//: o 'BSLS_ASSERTTEST_ASSERT_SAFE_PASS'
//: o 'BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW'
//: o 'BSLS_ASSERTTEST_ASSERT_PASS_RAW'
//: o 'BSLS_ASSERTTEST_ASSERT_OPT_PASS'
//: o 'BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW'
// These macros are all functionally identical to
// 'BSLS_ASSERTTEST_ASSERT_PASS'.  They exist so that 'PASS' checks format
// consistently with the corresponding negative tests they are associated with
// in a test driver.  See {Example 2}, below, for how this can help formatting
// assertion testing code.
//
///Usage
///-----
//
///Example 1: Testing Assertions In A Simple Vector Implementation
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// First we will demonstrate how "negative testing" might be used to verify
// that the correct assertions are in place on 'std::vector::operator[]'.  We
// start by supplying a primitive vector-like class that offers the minimal set
// of operations necessary to demonstrate the test case.
//..
//  template <class T>
//  class AssertTestVector {
//      // This class simulates a 'std::vector' with a fixed capacity of 10
//      // elements.
//
//    private:
//      // DATA
//      T   d_data[10];
//      int d_size;
//
//    public:
//      // CREATORS
//      AssertTestVector();
//          // Create an empty 'AssertTestVector' object.
//
//      // MANIPULATORS
//      void push_back(const T& value);
//          // Append the specified 'value' to the back of this object.
//          // The behavior is undefined unless this method has been called
//          // fewer than 10 times on this object.
//
//      // ACCESSORS
//      const T& operator[](int index) const;
//          // Return a reference with non-modifiable access to the object at
//          // the specified 'index' in this object.
//  };
//..
// Next we implement the support functions.
//..
//  template <class T>
//  AssertTestVector<T>::AssertTestVector()
//  : d_data()
//  , d_size()
//  {
//  }
//
//  template<class T>
//  void AssertTestVector<T>::push_back(const T& value)
//  {
//      BSLS_ASSERT_SAFE(d_size < 10);
//
//      d_data[d_size] = value;
//      ++d_size;
//  }
//..
// We conclude the definition of this support type with the implementation of
// the 'operator[]' overload.  Note the use of 'BSLS_ASSERT_SAFE', which is
// typical for function template definitions and inline function definitions.
// It is most appropriate in this case as the cost of evaluating each test is
// significant (> ~20%) compared to simply returning a reference to the result.
//..
//  template <class T>
//  const T& AssertTestVector<T>::operator[](int index) const
//  {
//      BSLS_ASSERT_SAFE(0 <= index);
//      BSLS_ASSERT_SAFE(     index < d_size);
//
//      return d_data[index];
//  }
//..
// Finally, we can write the function to test that the 'BSLS_ASSERT_SAFE'
// macros placed in 'operator[]' work as expected.  We want to validate that
// the assertions trigger when the function preconditions are violated; we
// further want to validate that the assertion macros are enabled in the build
// modes that we expect.  We start by defining some macro aliases that will
// make the test driver more readable.  These macro aliases are a common
// feature of test drivers.
//..
//  #define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
//  #define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
//  #define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
//  #define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)
//..
// Then we implement the test function itself.  Note that we check that
// exceptions are available in the current build mode, as the test macros rely
// on the exception facility in order to return their diagnostic results.  If
// exceptions are not available, there is nothing for a "negative test" to do.
//..
//  void testVectorArrayAccess()
//  {
//  #ifdef BDE_BUILD_TARGET_EXC
//      bsls::AssertTestHandlerGuard g;
//
//      AssertTestVector<void *> mA; const AssertTestVector<void *> &A = mA;
//
//      ASSERT_SAFE_FAIL(mA[-1]);
//      ASSERT_SAFE_FAIL(mA[ 0]);
//      ASSERT_SAFE_FAIL(mA[ 1]);
//
//      ASSERT_SAFE_FAIL( A[-1]);
//      ASSERT_SAFE_FAIL( A[ 0]);
//      ASSERT_SAFE_FAIL( A[ 1]);
//
//      mA.push_back(0);  // increase the length to one
//
//      ASSERT_SAFE_FAIL(mA[-1]);
//      ASSERT_PASS     (mA[ 0]);
//      ASSERT_SAFE_FAIL(mA[ 1]);
//
//      ASSERT_SAFE_FAIL( A[-1]);
//      ASSERT_PASS     ( A[ 0]);
//      ASSERT_SAFE_FAIL( A[ 1]);
//  #else   // BDE_BUILD_TARGET_EXC
//..
// If exceptions are not available, then we write a diagnostic message to the
// console alerting the user that this part of the test has not run, without
// failing the test.
//..
//      if (globalVerbose) printf(
//                         "\tDISABLED in this (non-exception) build mode.\n");
//
//  #endif  // BDE_BUILD_TARGET_EXC
//  }
//..
//
///Example 2: Using 'PASS' macros to help with formatting
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// When testing the various inputs to a function to be sure that some trigger
// an assertion and some are in contract, it often helps to align the testing
// macros so that the various arguments are easily readable in relation to one
// another.  We start by defining additional macro aliases to match the
// existing aliases already defined:
//..
//  #define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
//  #define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
//..
// Considering the function 'testVectorArrayAccess' from {Example 1}, we could
// instead implement it without padded white space by using 'ASSERT_SAFE_PASS'
// to replace 'ASSERT_PASS', matching the existing 'ASSERT_SAFE_FAIL' tests,
// like this:
//..
//  void testVectorArrayAccess2()
//  {
//  #ifdef BDE_BUILD_TARGET_EXC
//      bsls::AssertTestHandlerGuard g;
//
//      AssertTestVector<void *> mA; const AssertTestVector<void *> &A = mA;
//
//      ASSERT_SAFE_FAIL(mA[-1]);
//      ASSERT_SAFE_FAIL(mA[ 0]);
//      ASSERT_SAFE_FAIL(mA[ 1]);
//
//      ASSERT_SAFE_FAIL( A[-1]);
//      ASSERT_SAFE_FAIL( A[ 0]);
//      ASSERT_SAFE_FAIL( A[ 1]);
//
//      mA.push_back(0);  // increase the length to one
//
//      ASSERT_SAFE_FAIL(mA[-1]);
//      ASSERT_SAFE_PASS(mA[ 0]);
//      ASSERT_SAFE_FAIL(mA[ 1]);
//
//      ASSERT_SAFE_FAIL( A[-1]);
//      ASSERT_SAFE_PASS( A[ 0]);
//      ASSERT_SAFE_FAIL( A[ 1]);
//  #endif   // BDE_BUILD_TARGET_EXC
//  }
//..

#include <bsls_annotation.h>
#include <bsls_assert.h>
#include <bsls_asserttestexception.h>
#include <bsls_buildtarget.h>
#include <bsls_compilerfeatures.h>
#include <bsls_review.h>

#ifdef BDE_BUILD_TARGET_SAFE_2
    #define BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG true
#else
    #define BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG false
#endif

#ifdef BSLS_ASSERT_SAFE_IS_ACTIVE
    #define BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG true
#else
    #define BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG false
#endif

#ifdef BSLS_ASSERT_IS_ACTIVE
    #define BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG true
#else
    #define BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG false
#endif

#ifdef BSLS_ASSERT_OPT_IS_ACTIVE
    #define BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG true
#else
    #define BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG false
#endif

#define BSLS_ASSERTTEST_IS_ACTIVE(TYPE) (                                    \
    (   '\0' == TYPE[1]                                                      \
     || BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG                                    \
    )                                                                        \
    &&                                                                       \
    (   ('S' == TYPE[0] && BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG)          \
     || ('A' == TYPE[0] && BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG)               \
     || ('O' == TYPE[0] && BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG)           \
    )                                                                        \
)

#ifdef BSLS_ASSERTTEST_CHECK_LEVEL
    #define BSLS_ASSERTTEST_CHECK_LEVEL_ARG true
#else
    #define BSLS_ASSERTTEST_CHECK_LEVEL_ARG false
#endif

#define BSLS_ASSERTTEST_BRUTE_FORCE_IMP(RESULT, LVL, EXPRESSION_UNDER_TEST) {\
    try {                                                                    \
        EXPRESSION_UNDER_TEST;                                               \
                                                                             \
        ASSERT(bsls::AssertTest::tryProbe(RESULT,LVL));                      \
    }                                                                        \
    catch (const bsls::AssertTestException& e) {                             \
        ASSERT(bsls::AssertTest::catchProbe(RESULT,                          \
                                            BSLS_ASSERTTEST_CHECK_LEVEL_ARG, \
                                            LVL,                             \
                                            e,                               \
                                            __FILE__));                      \
    }                                                                        \
}

#define BSLS_ASSERTTEST_VALIDATE_DISABLED_MACROS
#ifdef BSLS_ASSERTTEST_VALIDATE_DISABLED_MACROS
    #define BSLS_ASSERTTEST_DISABLED_IMP(EXPRESSION_UNDER_TEST)              \
                               if (false) { EXPRESSION_UNDER_TEST ; } else {}
#else
    #define BSLS_ASSERTTEST_DISABLED_IMP(EXPERSSION_UNDER_TEST)
#endif

#if !defined(BDE_BUILD_TARGET_EXC)
// In non-exception enabled builds there is no way to safely use the
// ASSERT_FAIL macros as they require installing an assert-handler that throws
// a specific exception.  As ASSERT_FAIL negative tests require calling a
// method under test with out-of-contract values, running those tests, without
// a functioning assert-handler, would trigger undefined behavior with no
// protection, so we choose to simple not execute the test calls that are
// designed to fail by expanding the test macros to an empty statement, '{ }'.
// All of the ASSERT_PASS macros are expanded however, as such tests call
// methods with in-contract values, and they may still be needed to guarantee
// stateful side-effects required by the test-driver.

#define BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPRESSION_UNDER_TEST)              \
                                                   { EXPRESSION_UNDER_TEST; }

#define BSLS_ASSERTTEST_ASSERT_PASS(EXPRESSION_UNDER_TEST)                   \
                                                   { EXPRESSION_UNDER_TEST; }

#define BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPRESSION_UNDER_TEST)               \
                                                   { EXPRESSION_UNDER_TEST; }

#define BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPRESSION_UNDER_TEST)          \
                                                   { EXPRESSION_UNDER_TEST; }

#define BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPRESSION_UNDER_TEST)               \
                                                   { EXPRESSION_UNDER_TEST; }

#define BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPRESSION_UNDER_TEST)           \
                                                   { EXPRESSION_UNDER_TEST; }

#define BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPRESSION_UNDER_TEST)              \
                          BSLS_ASSERTTEST_DISABLED_IMP(EXPRESSION_UNDER_TEST)

#define BSLS_ASSERTTEST_ASSERT_FAIL(EXPRESSION_UNDER_TEST)                   \
                          BSLS_ASSERTTEST_DISABLED_IMP(EXPRESSION_UNDER_TEST)

#define BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPRESSION_UNDER_TEST)               \
                          BSLS_ASSERTTEST_DISABLED_IMP(EXPRESSION_UNDER_TEST)

#define BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPRESSION_UNDER_TEST)          \
                          BSLS_ASSERTTEST_DISABLED_IMP(EXPRESSION_UNDER_TEST)

#define BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPRESSION_UNDER_TEST)               \
                          BSLS_ASSERTTEST_DISABLED_IMP(EXPRESSION_UNDER_TEST)

#define BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPRESSION_UNDER_TEST)           \
                          BSLS_ASSERTTEST_DISABLED_IMP(EXPRESSION_UNDER_TEST)

#else // defined BDE_BUILD_TARGET_EXC

// The following macros are not expanded on the Microsoft compiler to avoid
// internal compiler errors in optimized builds, which are the result of
// attempts to optimize many try/catch blocks in large switch statements.  Note
// that the resulting test driver is just as thorough, but will report failure
// of a buggy library by simply crashing, rather than capturing and reporting
// the specific error detected.
#if (defined(BSLS_PLATFORM_CMP_MSVC) && defined(BDE_BUILD_TARGET_OPT))
#define BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPRESSION_UNDER_TEST)              \
                                                   { EXPRESSION_UNDER_TEST; }

#define BSLS_ASSERTTEST_ASSERT_PASS(EXPRESSION_UNDER_TEST)                   \
                                                   { EXPRESSION_UNDER_TEST; }

#define BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPRESSION_UNDER_TEST)               \
                                                   { EXPRESSION_UNDER_TEST; }
#else
#define BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPRESSION_UNDER_TEST)              \
             BSLS_ASSERTTEST_BRUTE_FORCE_IMP('P', 'S', EXPRESSION_UNDER_TEST)

#define BSLS_ASSERTTEST_ASSERT_PASS(EXPRESSION_UNDER_TEST)                   \
             BSLS_ASSERTTEST_BRUTE_FORCE_IMP('P', 'A', EXPRESSION_UNDER_TEST)

#define BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPRESSION_UNDER_TEST)               \
             BSLS_ASSERTTEST_BRUTE_FORCE_IMP('P', 'O', EXPRESSION_UNDER_TEST)
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#   define BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPRESSION_UNDER_TEST)           \
             BSLS_ASSERTTEST_BRUTE_FORCE_IMP('F', 'S', EXPRESSION_UNDER_TEST)
#else
#   define BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPRESSION_UNDER_TEST)           \
                          BSLS_ASSERTTEST_DISABLED_IMP(EXPRESSION_UNDER_TEST)
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
    #define BSLS_ASSERTTEST_ASSERT_FAIL(EXPRESSION_UNDER_TEST)               \
             BSLS_ASSERTTEST_BRUTE_FORCE_IMP('F', 'A', EXPRESSION_UNDER_TEST)
#else
    #define BSLS_ASSERTTEST_ASSERT_FAIL(EXPRESSION_UNDER_TEST)               \
                          BSLS_ASSERTTEST_DISABLED_IMP(EXPRESSION_UNDER_TEST)
#endif

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
    #define BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPRESSION_UNDER_TEST)           \
             BSLS_ASSERTTEST_BRUTE_FORCE_IMP('F', 'O', EXPRESSION_UNDER_TEST)
#else
    #define BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPRESSION_UNDER_TEST)           \
                          BSLS_ASSERTTEST_DISABLED_IMP(EXPRESSION_UNDER_TEST)
#endif

#define BSLS_ASSERTTEST_BRUTE_FORCE_IMP_RAW(RESULT,                          \
                                            LVL,                             \
                                            EXPRESSION_UNDER_TEST) {         \
    try {                                                                    \
        EXPRESSION_UNDER_TEST;                                               \
                                                                             \
        ASSERT(bsls::AssertTest::tryProbeRaw(RESULT,LVL));                   \
    }                                                                        \
    catch (const bsls::AssertTestException& e) {                             \
        ASSERT(bsls::AssertTest::catchProbeRaw(                              \
                                            RESULT,                          \
                                            BSLS_ASSERTTEST_CHECK_LEVEL_ARG, \
                                            LVL,                             \
                                            e));                             \
    }                                                                        \
}

#if defined(BSLS_PLATFORM_CMP_MSVC) && defined(BDE_BUILD_TARGET_OPT)
// The following MSVC-specific work-around avoids compilation issues with MSVC
// optimized builds.

#define BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPRESSION_UNDER_TEST)          \
                                                   { EXPRESSION_UNDER_TEST; }

#define BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPRESSION_UNDER_TEST)               \
                                                   { EXPRESSION_UNDER_TEST; }

#define BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPRESSION_UNDER_TEST)           \
                                                   { EXPRESSION_UNDER_TEST; }
#else
#define BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPRESSION_UNDER_TEST)          \
         BSLS_ASSERTTEST_BRUTE_FORCE_IMP_RAW('P', 'S', EXPRESSION_UNDER_TEST)

#define BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPRESSION_UNDER_TEST)               \
         BSLS_ASSERTTEST_BRUTE_FORCE_IMP_RAW('P', 'A', EXPRESSION_UNDER_TEST)

#define BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPRESSION_UNDER_TEST)           \
         BSLS_ASSERTTEST_BRUTE_FORCE_IMP_RAW('P', 'O', EXPRESSION_UNDER_TEST)
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
    #define BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPRESSION_UNDER_TEST)      \
         BSLS_ASSERTTEST_BRUTE_FORCE_IMP_RAW('F', 'S', EXPRESSION_UNDER_TEST)
#else
    #define BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPRESSION_UNDER_TEST)      \
                          BSLS_ASSERTTEST_DISABLED_IMP(EXPRESSION_UNDER_TEST)
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
    #define BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPRESSION_UNDER_TEST)           \
         BSLS_ASSERTTEST_BRUTE_FORCE_IMP_RAW('F', 'A', EXPRESSION_UNDER_TEST)
#else
    #define BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPRESSION_UNDER_TEST)           \
                          BSLS_ASSERTTEST_DISABLED_IMP(EXPRESSION_UNDER_TEST)
#endif

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
    #define BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPRESSION_UNDER_TEST)       \
        BSLS_ASSERTTEST_BRUTE_FORCE_IMP_RAW('F', 'O', EXPRESSION_UNDER_TEST)
#else
    #define BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPRESSION_UNDER_TEST)       \
                          BSLS_ASSERTTEST_DISABLED_IMP(EXPRESSION_UNDER_TEST)
#endif

#endif  // BDE_BUILD_TARGET_EXC

// Provide a deeper "include-guard" to protect against repeated inclusion by
// the test driver for this component.  This is necessary because, in order to
// test the macros and pre-processor logic that constitute key parts of this
// component, this header must be included multiple times by the test driver,
// which will deliberately '#undef' the main include-guard to achieve this
// effect.  The deeper include-guard protects the non-macro parts of this
// header that cannot be defined more than once.
#ifndef BSLS_ASSERTTEST_RECURSIVELY_INCLUDED_TESTDRIVER_GUARD
#define BSLS_ASSERTTEST_RECURSIVELY_INCLUDED_TESTDRIVER_GUARD

namespace BloombergLP {

namespace bsls {

                            // ================
                            // class AssertTest
                            // ================

struct AssertTest {
    // This utility 'struct' provides a suite of methods designed for use in
    // conjunction with preprocessor macros during the negative testing of
    // defensive checks using the facilities provided by the 'bsls_assert'
    // component.

    // CLASS METHODS

                    // Test Specification Validation

    static bool isValidAssertBuild(const char *specString);
        // Return 'true' if the specified 'specString' represents a valid
        // textual description of an assertion build type, and 'false'
        // otherwise.  'specString' is valid if it refers to a null-terminated
        // character string of length either 1 or 2, where the first character
        // is an (UPPERCASE) 'S', 'A', or 'O', and the second character, if
        // present, is the numeral '2'.  Note that 'S', 'A', and 'O' are
        // intended to represent the respective assertion types
        // 'BSLS_ASSERT_SAFE', 'BSLS_ASSERT', and 'BSLS_ASSERT_OPT', and the
        // optional '2' is intended to indicate that the component (and program
        // as a whole) was built with 'BDE_BUILD_TARGET_SAFE_2' defined.

    static bool isValidExpected(char specChar);
        // Return 'true' if the specified 'specChar' represents a valid
        // description of the expected result, and 'false' otherwise.
        // 'specChar' is valid if it is either an (UPPERCASE) 'P' (for Pass) or
        // 'F' (for Fail).  Note that 'P' is intended to represent the
        // expectation that the function under test will "pass" by satisfying
        // all preconditions, including the specific one guarded by the
        // assertion under test, while an 'F' is intended to indicate the
        // expectation that the specific assertion under test will "fail"
        // (thereby triggering a call to the currently-installed
        // assertion-failure handler).

    static bool isValidExpectedLevel(char specChar);
        // Return 'true' if the specified 'specChar' represents a valid
        // description of the expected assert level, and 'false' otherwise.
        // 'specChar' is valid if it is either a 'O' (for 'OPT'), 'S' (for
        // 'SAFE'), or 'A' (for 'ASSERT').

                            // Testing Apparatus

    static bool tryProbe(char expectedResult, char expectedLevel);
        // Return 'true' if the specified 'expectedResult' is 'P' (for Pass)
        // and the specified 'expectedLevel' is a valid level; otherwise,
        // return 'false'.  If 'expectedResult' is anything other than 'P' or
        // 'F' (for Fail), this function reports the invalid 'expectedResult'
        // value to 'stdout' before returning 'false'.  If 'expectedlevel' is
        // anything other than 'S', 'A', or 'O', this function reports the
        // invalid 'expectedLevel' value to 'stdout' before returning 'false'.

    static bool catchProbe(char                        expectedResult,
                           bool                        checkLevel,
                           char                        expectedLevel,
                           const AssertTestException&  caughtException,
                           const char                 *componentFileName);
        // Return 'true' if the specified 'expectedResult' is 'F' (for Fail),
        // the specified 'checkLevel' flag is 'false' or the 'expectedLevel' is
        // as wide or wider than the actual assertion failure level, the
        // specified 'caughtException' contains valid fields, and the specified
        // 'componentFileName' is either null or refers to the same (valid)
        // component name as the filename in 'caughtException'; otherwise,
        // return 'false'.  If 'expectedResult', 'componentFileName', or any
        // field of 'caughtException' is invalid (i.e., an invalid filename,
        // null or empty expression text, or a non-positive line number), this
        // function reports the invalid value(s) to 'stdout' before returning
        // 'false'.  If 'componentFileName' is not null, but does not reflect
        // the same component name as the otherwise valid filename in
        // 'caughtException', this function prints a message delineating the
        // mismatching deduced component names to 'stdout' before returning
        // 'false'.

    static bool tryProbeRaw(char expectedResult, char expectedLevel);
        // Return 'true' if the specified 'expectedResult' is 'P' (for Pass)
        // and the specified 'expectedLevel' is a valid level; otherwise,
        // return 'false'.  If 'expectedResult' is anything other than 'P' or
        // 'F' (for Fail), this function reports the invalid 'expectedResult'
        // value to 'stdout' before returning 'false'.  If 'expectedlevel' is
        // anything other than 'S', 'A', or 'O', this function reports the
        // invalid 'expectedLevel' value to 'stdout' before returning 'false'.

    static bool catchProbeRaw(char                       expectedResult,
                              bool                       checkLevel,
                              char                       expectedLevel,
                              const AssertTestException& caughtException);
        // Return 'true' if the specified 'expectedResult' is 'F' (for Fail),
        // the specified 'checkLevel' flag is 'false' or the 'expectedLevel' is
        // as wide or wider than the actual assertion failure level, and the
        // specified 'caughtException' contains valid fields; otherwise, return
        // 'false'.  If 'expectedResult', 'expectedLevel', or any field of
        // 'caughtException' is invalid (i.e., an invalid filename, null or
        // empty expression text, or a non-positive line number), this function
        // reports the invalid value(s) to 'stdout' before returning 'false'.

                        // Testing Failure Handlers

    BSLS_ANNOTATION_NORETURN
    static void failTestDriver(const AssertViolation &violation);
        // Throw an 'AssertTestException' having the pointer values 'text' and
        // 'file' and the integer 'line' from the specified 'violation' as its
        // salient attributes, provided that 'BDE_BUILD_TARGET_EXC' is defined;
        // otherwise, log an appropriate message and abort the program (similar
        // to 'Assert::failAbort').  Note that this function is intended to
        // have a signature compatible with a registered assertion-failure
        // handler function in 'bsls_assert'.

    static void failTestDriverByReview(const ReviewViolation &violation);
        // Throw an 'AssertTestException' having the 'comment', 'fileName', and
        // 'lineNumber' taken from the specified 'violation' as its salient
        // attributes, provided that 'BDE_BUILD_TARGET_EXC' is defined;
        // otherwise, log an appropriate message and abort the program (similar
        // to 'Assert::failAbort').  Note that this function is intended to
        // have a signature compatible with a registered assertion-failure
        // handler function in 'bsls_assert'.
};

                      // ============================
                      // class AssertTestHandlerGuard
                      // ============================

class AssertTestHandlerGuard {
    // This class provides a guard that will install and uninstall the negative
    // testing assertion handler, 'AssertTest::failTestDriver', within the
    // protected scope, as well as a corresponding review handler,
    // 'AssertTest::failTestDriverByReview'.

    // DATA
    AssertFailureHandlerGuard d_assertGuard;
    ReviewFailureHandlerGuard d_reviewGuard;

  public:
    // CREATORS
    AssertTestHandlerGuard();
        // Create a 'AssertTestHandlerGuard' object, installing the
        // 'AssertTest::failTestDriver' assertion handler.

    //! ~AssertTestHandlerGuard() = default;
        // Destroy this object and uninstall 'AssertTest::failTestDriver' as
        // the current assertion handler.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

inline
AssertTestHandlerGuard::AssertTestHandlerGuard()
: d_assertGuard(&AssertTest::failTestDriver)
, d_reviewGuard(&AssertTest::failTestDriverByReview)
{
}

}  // close package namespace

#ifndef BDE_OPENSOURCE_PUBLICATION  // BACKWARD_COMPATIBILITY
// ============================================================================
//                           BACKWARD COMPATIBILITY
// ============================================================================

typedef bsls::AssertTestHandlerGuard bsls_AssertTestHandlerGuard;
    // This alias is defined for backward compatibility.

typedef bsls::AssertTest bsls_AssertTest;
    // This alias is defined for backward compatibility.
#endif  // BDE_OPENSOURCE_PUBLICATION -- BACKWARD_COMPATIBILITY

}  // close enterprise namespace

#endif // test driver internal include guard

#endif

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
