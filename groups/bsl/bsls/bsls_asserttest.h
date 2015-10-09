// bsls_asserttest.h                                                  -*-C++-*-
#ifndef INCLUDED_BSLS_ASSERTTEST
#define INCLUDED_BSLS_ASSERTTEST

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a test facility for assertion macros.
//
//@CLASSES:
//  bsls::AssertTest: namespace for "assert" validating functions
//  bsls::AssertTestHandlerGuard: guard for the negative testing assert-handler
//
//@MACROS:
//  BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPRESSION): "safe" macro success expected
//  BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPRESSION): no origination check
//  BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPRESSION): "safe" macro failure expected
//  BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPRESSION): no origination check
//  BSLS_ASSERTTEST_ASSERT_PASS(EXPRESSION): macro success expected
//  BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPRESSION): no origination check
//  BSLS_ASSERTTEST_ASSERT_FAIL(EXPRESSION): macro failure expected
//  BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPRESSION): no origination check
//  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPRESSION): "opt" macro success expected
//  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPRESSION): no origination check
//  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPRESSION): "opt macro failure expected
//  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPRESSION): no origination check
//
//@SEE_ALSO: bsls_assert, bsls_asserttestexception
//
//@DESCRIPTION: This component provides a facility to test that 'BSLS_ASSERT_*'
// macros are used as intended, in the appropriate build modes, and have the
// expected effects.  The class 'bsls::AssertTest' provides a small set of
// static methods that can be used to support detailed test cases, especially
// in table-driven test scenarios.  Additionally, a set of macros automate use
// of these methods to support simple testing of single expressions.
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
// The function 'bsls::AssertTest::failTestDriver' is provided as the basis for
// a negative testing facility.  It can act as an assertion-failure handler
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
// The six basic test macros are
//: o 'BSLS_ASSERTTEST_ASSERT_SAFE_PASS'
//: o 'BSLS_ASSERTTEST_ASSERT_SAFE_FAIL'
//: o 'BSLS_ASSERTTEST_ASSERT_PASS'
//: o 'BSLS_ASSERTTEST_ASSERT_FAIL'
//: o 'BSLS_ASSERTTEST_ASSERT_OPT_PASS'
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
// The six "raw" test macros are
//: o 'BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW'
//: o 'BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW'
//: o 'BSLS_ASSERTTEST_ASSERT_PASS_RAW'
//: o 'BSLS_ASSERTTEST_ASSERT_FAIL_RAW'
//: o 'BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW'
//: o 'BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW'
// These testing macros perform the same test as the corresponding basic
// testing macros, except that there is no check to confirm that the assertion
// originated in the component under test.
//
///Table-Driven Negative Testing
///- - - - - - - - - - - - - - -
// The supplied test macros are useful for testing a small set of boundary
// conditions, but for more detailed contracts a table-driven approach works
// best.  The 'bsls::AssertTest' class provides a pair of "test probe"
// functions that can be hooked up as a pair in a 'try'/'catch' block to
// validate each row of a supplied test table.  See the second usage example
// below for more details.
//
///Enabling Negative Testing
///- - - - - - - - - - - - -
// In order to enable the negative testing facility, you must:
//: o '#include' this component header, 'bsls_asserttest.h'.
//: o Supply an implementation of an 'ASSERT' macro in your test driver.
//: o Register 'bsls::AssertTest::failTestDriver' as the active
//:   assertion-failure handler.
//
///Usage
///-----
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
//      return d_data[d_size];
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
//  #define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
//  #define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
//  #define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
//  #define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
//  #define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
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
//      bsls::AssertFailureHandlerGuard g(bsls::AssertTest::failTestDriver);
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
//
//  #endif  // BDE_BUILD_TARGET_EXC
//  }
//..
// In order to better highlight the tools available for testing assertions, let
// us consider an arbitrary function in some user-defined component, which
// conveniently utilizes each of our assertion macros to catch calls outside
// the behavior defined by its contract.
//..
//  struct MyUtil {
//  #ifdef BDE_BUILD_TARGET_SAFE_2
//    private:
//
//      static void *s_trustedPointers[10];
//
//      static bool isValid(void *pointer);
//          // Returns 'true' if the specified 'pointer' is registered in the
//          // array 's_trustedPointers', and 'false' otherwise.
//
//    public:
//  #endif
//
//      static void f(int a, int b, int c, void *d);
//          // This function invokes some arbitrary functionality.  The
//          // behavior is undefined unless '0 <= a <= 5', '0 <= b <= 4',
//          // '0 <= c <= 3', and 'd' is a valid pointer referring to some
//          // object known by the utility.
//  };
//..
// To illustrate the test facilities, we will validate the function arguments
// 'a', 'b', 'c', and 'd' with some additional assumptions.  We assume the cost
// of testing 'a' is significant compared to the unspecified body of 'f'.
// Likewise, we assume that testing the validity of 'b' is relatively
// efficient, while not only can 'c' be tested efficiently, but the unspecified
// behavior of 'f' will have potentially catastrophic consequences if 'c' is
// not valid, such as writing bad/corrupt values to some external data source.
// Finally, we would like to test that 'd' is a valid pointer.  In general,
// this is a non-trivial problem, but it becomes much simpler if we can afford
// some extra bookkeeping.  Therefore, if we are building in a 'SAFE_2' mode,
// we will add an additional cache of data, 's_trustedPointers', which will
// maintain a list of known valid pointers that work with this facility.
// Pointers will be registered and de-registered through calls to other methods
// in the 'MyUtil' class, and a valid pointer can be detected by searching for
// its value in this array.  Note that this artificial example is modeled after
// the behavior of some libraries that provide a "safe" STL mode that looks for
// invalid iterators in a similar way.
//..
//  #ifdef BDE_BUILD_TARGET_SAFE_2
//  void *MyUtil::s_trustedPointers[10] = {};
//
//  bool MyUtil::isValid(void *pointer)
//  {
//      if (0 == pointer) {
//          return true;
//      }
//      for (int i = 0; i != 10; ++i) {
//          if (pointer == s_trustedPointers[i]) {
//              return true;
//          }
//      }
//      return false;
//  }
//  #endif
//
//  void MyUtil::f(int a, int b, int c, void *d)
//  {
//      BSLS_ASSERT_SAFE(0 <= a);  BSLS_ASSERT_SAFE(a <= 5);
//      BSLS_ASSERT     (0 <= b);  BSLS_ASSERT     (b <= 4);
//      BSLS_ASSERT_OPT (0 <= c);  BSLS_ASSERT_OPT (c <= 3);
//
//  #ifdef BDE_BUILD_TARGET_SAFE_2
//      BSLS_ASSERT(isValid(d));  // assume small runtime overhead (< 10%)
//  #endif
//
//      // ...
//      // ...              (body of function 'f')
//      // ...
//  }
//..
// Now that we have defined the contract for 'f' and the range of undefined
// behavior we hope to catch with assertions, we are ready to write the test
// case for this function.  In addition to verifying the (unspecified) behavior
// of 'f' when called with valid function arguments, we also want to verify
// that calling with invalid arguments triggers an assertion in the appropriate
// build modes.  As this will involve many tests, a table- driven approach is
// preferred.
//..
//  void testMyUtilF()
//  {
//      // ...
//      // ...          (test correct behavior of function 'f')
//      // ...
//..
// First we announce that we are starting the negative testing portion of this
// test case.
//..
//  if (globalVerbose) printf("\nNegative Testing\n");
//..
// Next we check that exceptions are enabled in the current build mode, as the
// assertion-testing mechanisms are built on top of exceptions.
//..
//  #ifdef BDE_BUILD_TARGET_EXC
//      {
//          bsls::AssertFailureHandlerGuard g(
//                                           bsls::AssertTest::failTestDriver);
//
//..
// Then we build the table of test values, listing the expected build mode for
// the assertion under test to be enabled, whether the assertion should pass or
// fail, and the set of arguments to pass to 'f' in order to test the relevant
// assertion.  Note that we pick values in the table to test the boundaries of
// defined behavior, verifying that out-of-band values trigger the assertions
// while the adjacent in-band values succeed.
//..
//  static const struct {
//      int         d_lineNumber;       // line # of row in this table
//      const char *d_assertBuildType;  // "S", "S2", "A", "A2", "O",
//                                      // "O2"
//      char        d_expectedResult;   // assertion: 'P'ass or 'F'ail
//      int         d_a;                // function arg #1
//      int         d_b;                // function arg #2
//      int         d_c;                // function arg #3
//      void       *d_d_p;              // function arg #4
//  } DATA[] = {
//      //LINE  TYPE  RESULT   A    B    C    D
//      //----  ----  ------  ---  ---  ---  ---
//
//      // testing defensive checks for parameter 'a'
//
//      { L_,   "S",   'F',   -1,   0,   0,   0 },
//      { L_,   "S",   'P',    0,   0,   0,   0 },
//
//      { L_,   "S",   'P',    5,   0,   0,   0 },
//      { L_,   "S",   'F',    6,   0,   0,   0 },
//
//      // testing defensive checks for parameter 'b'
//
//      { L_,   "A",   'F',    0,  -1,   0,   0 },
//      { L_,   "A",   'P',    0,   0,   0,   0 },
//
//      { L_,   "A",   'P',    0,   4,   0,   0 },
//      { L_,   "A",   'F',    0,   5,   0,   0 },
//
//      // testing defensive checks for parameter 'c'
//
//      { L_,   "O",   'F',    0,   0,  -1,   0 },
//      { L_,   "O",   'P',    0,   0,   0,   0 },
//
//      { L_,   "O",   'P',    0,   0,   3,   0 },
//      { L_,   "O",   'F',    0,   0,   4,   0 },
//
//      // testing defensive checks for parameter 'd'
//
//      { L_,   "A2",  'P',    0,   0,   0,   0             },
//      { L_,   "A2",  'F',    0,   0,   0,   (void *)0xBAD },
//
//  };
//  const int NUM_DATA = sizeof DATA / sizeof *DATA;
//
//  for (int ti = 0; ti < NUM_DATA; ++ti) {
//      const int         LINE   = DATA[ti].d_lineNumber;
//      const char *const TYPE   = DATA[ti].d_assertBuildType;
//      const char        RESULT = DATA[ti].d_expectedResult;
//      const int         A      = DATA[ti].d_a;
//      const int         B      = DATA[ti].d_b;
//      const int         C      = DATA[ti].d_c;
//      void       *const D      = DATA[ti].d_d_p;
//
//      // Validate test description.
//..
// We should verify that the assert-related parameters describe a valid test
// configuration before running any tests.
//..
//  LOOP_ASSERT(LINE, bsls::AssertTest::isValidAssertBuild(TYPE));
//  LOOP_ASSERT(LINE, bsls::AssertTest::isValidExpected(RESULT));
//..
// Then, if we determine that there is no useful test to perform, continue with
// the next iteration of the loop.  Note that when a 'F'ail is expected, there
// is no meaningful test unless the build mode enables the relevant assertion.
//..
//  // Skip this test if the relevant assert is not active in this
//  // build.
//
//  if ('F' == RESULT && !BSLS_ASSERTTEST_IS_ACTIVE(TYPE)) {
//      continue;
//  }
//
//  // The relevant assert is active in this build.
//..
// Finally, we verify that the assert fires (or not) as expected by hooking up
// a pair of try/catch probes.  We use 'LOOP_ASSERT' to verify that we get the
// correct behavior from 'f', whether an assertion throws an exception or not.
//..
//              try {
//                  if (globalVeryVerbose) {
//                      T_ P_(TYPE) P_(RESULT) P_(A) P_(B) P_(C) P(D)
//                  }
//
//                  MyUtil::f(A, B, C, D);
//
//                  LOOP_ASSERT(LINE, bsls::AssertTest::tryProbe(RESULT));
//              }
//              catch (const bsls::AssertTestException& e) {
//
//                  LOOP_ASSERT(LINE, bsls::AssertTest::catchProbe(RESULT,
//                                                                 e,
//                                                                 __FILE__));
//              }
//
//          }  // table-driven 'for' loop
//      }
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
//
//  }
//..

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_ASSERTTESTEXCEPTION
#include <bsls_asserttestexception.h>
#endif

#ifdef BSLS_ASSERTTEST_NORETURN
#error BSLS_ASSERTTEST_NORETURN must be a macro scoped locally to this file
#endif

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

#define BSLS_ASSERTTEST_IS_ACTIVE(TYPE) (                                \
    (   '\0' == TYPE[1]                                                  \
     || BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG                                \
    )                                                                    \
    &&                                                                   \
    (   ('S' == TYPE[0] && BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG)      \
     || ('A' == TYPE[0] && BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG)           \
     || ('O' == TYPE[0] && BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG)       \
    )                                                                    \
)

#define BSLS_ASSERTTEST_BRUTE_FORCE_IMP(RESULT, EXPRESSION_UNDER_TEST) {  \
    try {                                                                 \
        EXPRESSION_UNDER_TEST;                                            \
                                                                          \
        ASSERT(bsls::AssertTest::tryProbe(RESULT));                       \
    }                                                                     \
    catch (const bsls::AssertTestException& e) {                          \
        if (!bsls::AssertTest::catchProbe(RESULT, e, __FILE__)) {         \
            if ('P' == RESULT) {                                          \
               ASSERT(false && "Unexpected assertion");                   \
            }                                                             \
            else {                                                        \
               ASSERT(false &&                                            \
               "(Expected) assertion raised by a lower level component"); \
            }                                                             \
        }                                                                 \
    }                                                                     \
}

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

# define BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPRESSION_UNDER_TEST) \
         { EXPRESSION_UNDER_TEST; }

# define BSLS_ASSERTTEST_ASSERT_PASS(EXPRESSION_UNDER_TEST) \
         { EXPRESSION_UNDER_TEST; }

# define BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPRESSION_UNDER_TEST) \
         { EXPRESSION_UNDER_TEST; }

# define BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPRESSION_UNDER_TEST) \
         { EXPRESSION_UNDER_TEST; }

# define BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPRESSION_UNDER_TEST) \
         { EXPRESSION_UNDER_TEST; }

# define BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPRESSION_UNDER_TEST) \
         { EXPRESSION_UNDER_TEST; }

# define BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPRESSION_UNDER_TEST) { }
# define BSLS_ASSERTTEST_ASSERT_FAIL(EXPRESSION_UNDER_TEST) { }
# define BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPRESSION_UNDER_TEST) { }
# define BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPRESSION_UNDER_TEST) { }
# define BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPRESSION_UNDER_TEST) { }
# define BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPRESSION_UNDER_TEST) { }

#else // defined BDE_BUILD_TARGET_EXC

// The following macros are not expanded on the Microsoft compiler to avoid
// internal compiler errors in optimized builds, which are the result of
// attempts to optimize many try/catch blocks in large switch statements.  Note
// that the resulting test driver is just as thorough, but will report failure
// of a buggy library by simply crashing, rather than capturing and reporting
// the specific error detected.
#if (defined(BSLS_PLATFORM_CMP_MSVC) && defined(BDE_BUILD_TARGET_OPT))
# define BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPRESSION_UNDER_TEST) \
         { EXPRESSION_UNDER_TEST; }

# define BSLS_ASSERTTEST_ASSERT_PASS(EXPRESSION_UNDER_TEST) \
         { EXPRESSION_UNDER_TEST; }

# define BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPRESSION_UNDER_TEST) \
         { EXPRESSION_UNDER_TEST; }
#else
# define BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPRESSION_UNDER_TEST) \
         BSLS_ASSERTTEST_BRUTE_FORCE_IMP('P', EXPRESSION_UNDER_TEST)

# define BSLS_ASSERTTEST_ASSERT_PASS(EXPRESSION_UNDER_TEST) \
         BSLS_ASSERTTEST_BRUTE_FORCE_IMP('P', EXPRESSION_UNDER_TEST)

# define BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPRESSION_UNDER_TEST) \
         BSLS_ASSERTTEST_BRUTE_FORCE_IMP('P', EXPRESSION_UNDER_TEST)
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#   define BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPRESSION_UNDER_TEST) \
       BSLS_ASSERTTEST_BRUTE_FORCE_IMP('F', EXPRESSION_UNDER_TEST)
#else
#   define BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPRESSION_UNDER_TEST)
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
    #define BSLS_ASSERTTEST_ASSERT_FAIL(EXPRESSION_UNDER_TEST) \
        BSLS_ASSERTTEST_BRUTE_FORCE_IMP('F', EXPRESSION_UNDER_TEST)
#else
    #define BSLS_ASSERTTEST_ASSERT_FAIL(EXPRESSION_UNDER_TEST)
#endif

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
    #define BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPRESSION_UNDER_TEST) \
        BSLS_ASSERTTEST_BRUTE_FORCE_IMP('F', EXPRESSION_UNDER_TEST)
#else
    #define BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPRESSION_UNDER_TEST)
#endif

#define BSLS_ASSERTTEST_BRUTE_FORCE_IMP_RAW(RESULT, EXPRESSION_UNDER_TEST) { \
    try {                                                                    \
        EXPRESSION_UNDER_TEST;                                               \
                                                                             \
        ASSERT(bsls::AssertTest::tryProbe(RESULT));                          \
    }                                                                        \
    catch (const bsls::AssertTestException&) {                               \
        ASSERT(bsls::AssertTest::catchProbeRaw(RESULT));                     \
    }                                                                        \
}

#if defined(BSLS_PLATFORM_CMP_MSVC) && defined(BDE_BUILD_TARGET_OPT)
// The following MSVC specific work-around avoids compilation issues with
// MSVC optimized builds.

# define BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPRESSION_UNDER_TEST) \
         { EXPRESSION_UNDER_TEST; }

# define BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPRESSION_UNDER_TEST) \
         { EXPRESSION_UNDER_TEST; }

# define BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPRESSION_UNDER_TEST) \
         { EXPRESSION_UNDER_TEST; }
#else
#define BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPRESSION_UNDER_TEST) \
        BSLS_ASSERTTEST_BRUTE_FORCE_IMP_RAW('P', EXPRESSION_UNDER_TEST)

#define BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPRESSION_UNDER_TEST) \
        BSLS_ASSERTTEST_BRUTE_FORCE_IMP_RAW('P', EXPRESSION_UNDER_TEST)

#define BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPRESSION_UNDER_TEST) \
        BSLS_ASSERTTEST_BRUTE_FORCE_IMP_RAW('P', EXPRESSION_UNDER_TEST)
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
    #define BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPRESSION_UNDER_TEST) \
        BSLS_ASSERTTEST_BRUTE_FORCE_IMP_RAW('F', EXPRESSION_UNDER_TEST)
#else
    #define BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPRESSION_UNDER_TEST)
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
    #define BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPRESSION_UNDER_TEST) \
        BSLS_ASSERTTEST_BRUTE_FORCE_IMP_RAW('F', EXPRESSION_UNDER_TEST)
#else
    #define BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPRESSION_UNDER_TEST)
#endif

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
    #define BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPRESSION_UNDER_TEST) \
        BSLS_ASSERTTEST_BRUTE_FORCE_IMP_RAW('F', EXPRESSION_UNDER_TEST)
#else
    #define BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPRESSION_UNDER_TEST)
#endif

#endif  // BDE_BUILD_TARGET_EXC

// Note that a portable syntax for 'noreturn' will be available once we have
// access to conforming C++0x compilers.
//# define BSLS_ASSERTTEST_NORETURN [[noreturn]]

#if defined(BSLS_PLATFORM_CMP_MSVC)
#   define BSLS_ASSERTTEST_NORETURN __declspec(noreturn)
#else
#   define BSLS_ASSERTTEST_NORETURN
#endif

// Provide a deeper "include-guard" to protect against repeated inclusion by
// the test driver for this component.  This is necessary because, in order to
// test the macros and pre-processor logic that constitute key parts of this
// component, this header must be included multiple times by the test driver,
// which will deliberately '#undef' the main include-guard to achieve this
// effect.  The deeper include-guard protects the non-macro parts of this
// header that cannot be defined more than once.
#ifndef RECURSIVELY_INCLUDED_BSLS_ASSERTTEST_TEST_GUARD
#define RECURSIVELY_INCLUDED_BSLS_ASSERTTEST_TEST_GUARD

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

                            // Testing Apparatus

    static bool tryProbe(char expectedResult);
        // Return 'true' if the specified 'expectedResult' is 'P' (for Pass),
        // and 'false' otherwise.  If 'expectedResult' is anything other than
        // 'P' or 'F' (for Fail), this function reports the invalid
        // 'expectedResult' value to 'stdout' before returning 'false'.

    static bool catchProbe(char                        expectedResult,
                           const AssertTestException&  caughtException,
                           const char                 *componentFileName);
        // Return 'true' if the specified 'expectedResult' is 'F' (for Fail),
        // the specified 'caughtException' contains valid fields, and the
        // specified 'componentFileName' is either null or refers to the same
        // (valid) component name as the filename in 'caughtException';
        // otherwise, return 'false'.  If 'expectedResult',
        // 'componentFileName', or any field of 'caughtException' is invalid
        // (i.e., an invalid filename, null or empty expression text, or a
        // non-positive line number), this function reports the invalid
        // value(s) to 'stdout' before returning 'false'.  If
        // 'componentFileName' is not null, but does not reflect the same
        // component name as the otherwise valid filename in 'caughtException',
        // this function prints a message delineating the mismatching deduced
        // component names to 'stdout' before returning 'false'.

    static bool tryProbeRaw(char expectedResult);
        // Return 'true' if the specified 'expectedResult' is 'P' (for Pass),
        // and 'false' otherwise.  If 'expectedResult' is anything other than
        // 'P' or 'F' (for Fail), this function reports the invalid
        // 'expectedResult' value to 'stdout' before returning 'false'.

    static bool catchProbeRaw(char expectedResult);
        // Return 'true' if the specified 'expectedResult' is 'F' (for Fail),
        // and 'false' otherwise.  If 'expectedResult' is anything other than
        // 'F' or 'P' (for Pass), this function reports the invalid
        // 'expectedResult' value to 'stdout' before returning 'false'.

    BSLS_ASSERTTEST_NORETURN
    static void failTestDriver(const char *text, const char *file, int line);
        // Throw a 'AssertTestException' having the specified pointer values
        // 'text' and 'file' and the specified integer 'line' as its salient
        // attributes, provided that 'BDE_BUILD_TARGET_EXC' is defined;
        // otherwise, write an appropriate message to 'stderr' and abort the
        // program (similar to 'Assert::failAbort').  Note that this function
        // is intended to have a signature compatible with a registered
        // assertion-failure handler function in 'bsls_assert'.
};

                   // ===============================
                   // class AssertFailureHandlerGuard
                   // ===============================

class AssertTestHandlerGuard {
    // This class provides a guard that will install and uninstall the negative
    // testing assertion handler, 'AssertTest::failTestDriver', within the
    // protected scope.

    // DATA
    AssertFailureHandlerGuard d_guard;

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
: d_guard(&AssertTest::failTestDriver)
{
}

}  // close package namespace


}  // close enterprise namespace

#endif // test driver internal include guard

#undef BSLS_ASSERTTEST_NORETURN

#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
