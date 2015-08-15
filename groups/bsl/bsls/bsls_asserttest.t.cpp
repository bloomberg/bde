// bsls_asserttest.t.cpp                                              -*-C++-*-
#include <bsls_asserttest.h>

#include <bsls_assert.h>
#include <bsls_bsltestutil.h>
#include <bsls_macroincrement.h>
#include <bsls_platform.h>

// limit ourselves to the "C" library for packages below 'bslstl'
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace BloombergLP;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
//                                  TBD
// ----------------------------------------------------------------------------
// [ 8] BSLS_ASSERTTEST_ASSERT_SAFE_PASS(FUNCTION)
// [ 9] BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(FUNCTION)
// [ 8] BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(FUNCTION)
// [ 9] BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(FUNCTION)
// [ 8] BSLS_ASSERTTEST_ASSERT_PASS(FUNCTION)
// [ 9] BSLS_ASSERTTEST_ASSERT_PASS_RAW(FUNCTION)
// [ 8] BSLS_ASSERTTEST_ASSERT_FAIL(FUNCTION)
// [ 9] BSLS_ASSERTTEST_ASSERT_FAIL_RAW(FUNCTION)
// [ 8] BSLS_ASSERTTEST_ASSERT_OPT_PASS(FUNCTION)
// [ 9] BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(FUNCTION)
// [ 8] BSLS_ASSERTTEST_ASSERT_OPT_FAIL(FUNCTION)
// [ 9] BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(FUNCTION)
// [ 4] BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG
// [ 4] BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG
// [ 4] BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG
// [ 4] BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG
// [ 4] BSLS_ASSERTTEST_IS_ACTIVE(TYPE)
// [ 2] bsls::AssertTest::isValidAssertBuild
// [ 3] bsls::AssertTest::isValidExpected
// [ 6] bsls::AssertTest::tryProbe
// [ 6] bsls::AssertTest::tryProbeRaw
// [ 7] bsls::AssertTest::catchProbe
// [ 7] bsls::AssertTest::catchProbeRaw
// [ 5] bsls::AssertTest::failTestDriver
// [10] bsls::AssertTestHandlerGuard::bsls::AssertTestHandlerGuard()
// [10] ~bsls::AssertTestHandlerGuard::bsls::AssertTestHandlerGuard()
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [11] Test case 1: vector
// [12] Test case 2: MyUtil::f
// [-1] Testing try-probes that write diagnostics to the console
// [-2] Testing catch-probes that write diagnostics to the console
//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
static int testStatus = 0;

static void aSsErT(bool b, const char *s, int i) {
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number


//=============================================================================
//                    GLOBAL CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

bool globalVerbose         = false;
bool globalVeryVerbose     = false;
bool globalVeryVeryVerbose = false;

//=============================================================================
//                  GLOBAL HELPER MACROS FOR TESTING
//-----------------------------------------------------------------------------
// These standard aliases will be defined below as part of the usage example.
//#define ASSERT_SAFE_PASS(FUNCTION) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(FUNCTION)
//#define ASSERT_SAFE_FAIL(FUNCTION) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(FUNCTION)
//
//#define ASSERT_PASS(FUNCTION)      BSLS_ASSERTTEST_ASSERT_PASS(FUNCTION)
//#define ASSERT_FAIL(FUNCTION)      BSLS_ASSERTTEST_ASSERT_FAIL(FUNCTION)
//
//#define ASSERT_OPT_PASS(FUNCTION)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(FUNCTION)
//#define ASSERT_OPT_FAIL(FUNCTION)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(FUNCTION)

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                 USAGE EXAMPLE EXTRACTED AS STAND-ALONE CODE
//-----------------------------------------------------------------------------

//..
// First we will demonstrate how "negative testing" might be used to verify
// that the correct assertions are in place on 'std::vector::operator[]'.  We
// start by supplying a primitive vector-like class that offers the minimal set
// of operations necessary to demonstrate the test case.
//..
    template <class T>
    class AssertTestVector {
        // This class simulates a 'std::vector' with a fixed capacity of 10
        // elements.

      private:
        // DATA
        T   d_data[10];
        int d_size;

      public:
        // CREATORS
        AssertTestVector();
            // Create an empty 'AssertTestVector' object.

        // MANIPULATORS
        void push_back(const T& value);
            // Append the specified 'value' to the back of this object.
            // The behavior is undefined unless this method has been called
            // fewer than 10 times on this object.

        // ACCESSORS
        const T& operator[](int index) const;
            // Return a reference with non-modifiable access to the object at
            // the specified 'index' in this object.
    };
//..
// Next we implement the support functions.
//..
    template <class T>
    AssertTestVector<T>::AssertTestVector()
    : d_data()
    , d_size()
    {
    }

    template<class T>
    void AssertTestVector<T>::push_back(const T& value)
    {
        BSLS_ASSERT_SAFE(d_size < 10);

        d_data[d_size] = value;
        ++d_size;
    }
//..
// We conclude the definition of this support type with the implementation of
// the 'operator[]' overload.  Note the use of 'BSLS_ASSERT_SAFE', which is
// typical for function template definitions and inline function definitions.
// It is most appropriate in this case as the cost of evaluating each test is
// significant (> ~20%) compared to simply returning a reference to the result.
//..
    template <class T>
    const T& AssertTestVector<T>::operator[](int index) const
    {
        (void) index;  // suppress 'unused parameter' compiler warning
        BSLS_ASSERT_SAFE(0 <= index);
        BSLS_ASSERT_SAFE(     index < d_size);

        return d_data[d_size];
    }
//..
// Finally, we can write the function to test that the 'BSLS_ASSERT_SAFE'
// macros placed in 'operator[]' work as expected.  We want to validate that
// the assertions trigger when the function preconditions are violated; we
// further want to validate that the assertion macros are enabled in the build
// modes that we expect.  We start by defining some macro aliases that will
// make the test driver more readable.  These macro aliases are a common
// feature of test drivers.
//..
    #define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
    #define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
    #define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
    #define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
    #define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
    #define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)
//..
// Then we implement the test function itself.  Note that we check that
// exceptions are available in the current build mode, as the test macros rely
// on the exception facility in order to return their diagnostic results.  If
// exceptions are not available, there is nothing for a "negative test" to do.
//..
    void testVectorArrayAccess()
    {
    #ifdef BDE_BUILD_TARGET_EXC
        bsls::AssertFailureHandlerGuard g(bsls::AssertTest::failTestDriver);

        AssertTestVector<void *> mA; const AssertTestVector<void *> &A = mA;

        ASSERT_SAFE_FAIL(mA[-1]);
        ASSERT_SAFE_FAIL(mA[ 0]);
        ASSERT_SAFE_FAIL(mA[ 1]);

        ASSERT_SAFE_FAIL( A[-1]);
        ASSERT_SAFE_FAIL( A[ 0]);
        ASSERT_SAFE_FAIL( A[ 1]);

        mA.push_back(0);  // increase the length to one

        ASSERT_SAFE_FAIL(mA[-1]);
        ASSERT_SAFE_PASS(mA[ 0]);
        ASSERT_SAFE_FAIL(mA[ 1]);

        ASSERT_SAFE_FAIL( A[-1]);
        ASSERT_SAFE_PASS( A[ 0]);
        ASSERT_SAFE_FAIL( A[ 1]);

    #endif  // BDE_BUILD_TARGET_EXC
    }
//..
// In order to better highlight the tools available for testing assertions, let
// us consider an arbitrary function in some user-defined component, which
// conveniently utilizes each of our assertion macros to catch calls outside
// the behavior defined by its contract.
//..
    struct MyUtil {
    #ifdef BDE_BUILD_TARGET_SAFE_2
      private:

        static void *s_trustedPointers[10];

        static bool isValid(void *pointer);
            // Returns 'true' if the specified 'pointer' is registered in the
            // array 's_trustedPointers', and 'false' otherwise.

      public:
    #endif

        static void f(int a, int b, int c, void *d);
            // This function invokes some arbitrary functionality.  The
            // behavior is undefined unless '0 <= a <= 5', '0 <= b <= 4',
            // '0 <= c <= 3', and 'd' is a valid pointer referring to some
            // object known by the utility.
    };
//..
// To illustrate the test facilities, we will validate the function arguments
// 'a', 'b', 'c', and 'd' with some additional assumptions.  We assume the
// cost of testing 'a' is significant compared to the unspecified body of 'f'.
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
// its value in this array.  Note that this artificial example is modeled
// after the behavior of some libraries that provide a "safe" STL mode that
// looks for invalid iterators in a similar way.
//..
    #ifdef BDE_BUILD_TARGET_SAFE_2
    void *MyUtil::s_trustedPointers[10] = {};

    bool MyUtil::isValid(void *pointer)
    {
        if (0 == pointer) {
            return true;
        }
        for (int i = 0; i != 10; ++i) {
            if (pointer == s_trustedPointers[i]) {
                return true;
            }
        }
        return false;
    }
    #endif

    void MyUtil::f(int a, int b, int c, void *d)
    {
        (void) a;  // suppress 'unused parameter' compiler warning
        (void) b;  // suppress 'unused parameter' compiler warning
        (void) c;  // suppress 'unused parameter' compiler warning
        (void) d;  // suppress 'unused parameter' compiler warning
        BSLS_ASSERT_SAFE(0 <= a);  BSLS_ASSERT_SAFE(a <= 5);
        BSLS_ASSERT     (0 <= b);  BSLS_ASSERT     (b <= 4);
        BSLS_ASSERT_OPT (0 <= c);  BSLS_ASSERT_OPT (c <= 3);

    #ifdef BDE_BUILD_TARGET_SAFE_2
        BSLS_ASSERT(isValid(d));  // assume small runtime overhead (< 10%)
    #endif

        // ...
        // ...              (body of function 'f')
        // ...
    }
//..
// Now that we have defined the contract for 'f' and the range of undefined
// behavior we hope to catch with assertions, we are ready to write the
// test case for this function.  In addition to verifying the (unspecified)
// behavior of 'f' when called with valid function arguments, we also want
// to verify that calling with invalid arguments triggers an assertion in the
// appropriate build modes.  As this will involve many tests, a table-
// driven approach is preferred.
//..
    void testMyUtilF()
    {
        // ...
        // ...          (test correct behavior of function 'f')
        // ...
//..
// First we announce that we are starting the negative testing portion of this
// test case.
//..
        if (globalVerbose) printf("\nNegative Testing\n");
//..
// Next we check that exceptions are enabled in the current build mode, as the
// assertion-testing mechanisms are built on top of exceptions.
//..
    #ifdef BDE_BUILD_TARGET_EXC
        {
            bsls::AssertFailureHandlerGuard g(
                                             bsls::AssertTest::failTestDriver);

//..
// Then we build the table of test values, listing the expected build mode for
// the assertion under test to be enabled, whether the assertion should pass or
// fail, and the set of arguments to pass to 'f' in order to test the relevant
// assertion.  Note that we pick values in the table to test the boundaries of
// defined behavior, verifying that out-of-band values trigger the assertions
// while the adjacent in-band values succeed.
//..
            static const struct {
                int         d_lineNumber;       // line # of row in this table
                const char *d_assertBuildType;  // "S", "S2", "A", "A2", "O",
                                                // "O2"
                char        d_expectedResult;   // assertion: 'P'ass or 'F'ail
                int         d_a;                // function arg #1
                int         d_b;                // function arg #2
                int         d_c;                // function arg #3
                void       *d_d_p;              // function arg #4
            } DATA[] = {
                //LINE  TYPE  RESULT   A    B    C    D
                //----  ----  ------  ---  ---  ---  ---

                // testing defensive checks for parameter 'a'

                { L_,   "S",   'F',   -1,   0,   0,   0 },
                { L_,   "S",   'P',    0,   0,   0,   0 },

                { L_,   "S",   'P',    5,   0,   0,   0 },
                { L_,   "S",   'F',    6,   0,   0,   0 },

                // testing defensive checks for parameter 'b'

                { L_,   "A",   'F',    0,  -1,   0,   0 },
                { L_,   "A",   'P',    0,   0,   0,   0 },

                { L_,   "A",   'P',    0,   4,   0,   0 },
                { L_,   "A",   'F',    0,   5,   0,   0 },

                // testing defensive checks for parameter 'c'

                { L_,   "O",   'F',    0,   0,  -1,   0 },
                { L_,   "O",   'P',    0,   0,   0,   0 },

                { L_,   "O",   'P',    0,   0,   3,   0 },
                { L_,   "O",   'F',    0,   0,   4,   0 },

                // testing defensive checks for parameter 'd'

                { L_,   "A2",  'P',    0,   0,   0,   0             },
                { L_,   "A2",  'F',    0,   0,   0,   (void *)0xBAD },

            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE   = DATA[ti].d_lineNumber;
                const char *const TYPE   = DATA[ti].d_assertBuildType;
                const char        RESULT = DATA[ti].d_expectedResult;
                const int         A      = DATA[ti].d_a;
                const int         B      = DATA[ti].d_b;
                const int         C      = DATA[ti].d_c;
                void       *const D      = DATA[ti].d_d_p;

                // Validate test description.
//..
// We should verify that the assert-related parameters describe a valid test
// configuration before running any tests.
//..
                LOOP_ASSERT(LINE, bsls::AssertTest::isValidAssertBuild(TYPE));
                LOOP_ASSERT(LINE, bsls::AssertTest::isValidExpected(RESULT));
//..
// Then, if we determine that there is no useful test to perform, continue with
// the next iteration of the loop.  Note that when a 'F'ail is expected, there
// is no meaningful test unless the build mode enables the relevant assertion.
//..
                // Skip this test if the relevant assert is not active in this
                // build.

                if ('F' == RESULT && !BSLS_ASSERTTEST_IS_ACTIVE(TYPE)) {
                    continue;
                }

                // The relevant assert is active in this build.
//..
// Finally, we verify that the assert fires (or not) as expected by hooking up
// a pair of try/catch probes.  We use 'LOOP_ASSERT' to verify that we get the
// correct behavior from 'f', whether an assertion throws an exception or not.
//..
                try {
                    if (globalVeryVerbose) {
                        T_ P_(TYPE) P_(RESULT) P_(A) P_(B) P_(C) P(D)
                    }

                    MyUtil::f(A, B, C, D);

                    LOOP_ASSERT(LINE, bsls::AssertTest::tryProbe(RESULT));
                }
                catch (const bsls::AssertTestException& e) {

                    LOOP_ASSERT(LINE, bsls::AssertTest::catchProbe(RESULT,
                                                                   e,
                                                                   __FILE__));
                }

            }  // table-driven 'for' loop
        }
    #else   // BDE_BUILD_TARGET_EXC
//..
// If exceptions are not available, then we write a diagnostic message to the
// console alerting the user that this part of the test has not run, without
// failing the test.
//..
        if (globalVerbose) printf(
                           "\tDISABLED in this (non-exception) build mode.\n");

    #endif  // BDE_BUILD_TARGET_EXC

    }
//..

//=============================================================================
// Forward declaration of functions to support test cases that may be
// implemented after 'main'.

void TestMacroBSLS_ASSERTTEST_IS_ACTIVE();
void TestMacroBSLS_ASSERTTEST_PASS_OR_FAIL();
void TestMacroBSLS_ASSERTTEST_PASS_OR_FAIL_RAW();
//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose = argc > 2;
    bool veryVerbose = argc > 3;
    bool veryVeryVerbose = argc > 4;

    globalVerbose         = verbose;
    globalVeryVerbose     = veryVerbose;
    globalVeryVeryVerbose = veryVeryVerbose;

    //bsls::Assert::setFailureHandler(&bsls::AssertTest::failTestDriver);

    setbuf(stdout, 0);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // zero is always the leading case
      case 12: {
        // --------------------------------------------------------------------
        // TESTING usage example 2
        //
        // Concerns:
        //: 1 The usage example provided in the component header file must
        //:   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Run the usage example.
        //
        // Testing:
        //   usage example 2
        // --------------------------------------------------------------------

        testMyUtilF();
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING usage example 1
        //
        // Concerns:
        //: 1 The usage example provided in the component header file must
        //:   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Run the usage example.
        //
        // Testing:
        //   usage example 1
        // --------------------------------------------------------------------

        testVectorArrayAccess();
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING bsls::AssertTestHandlerGuard
        //
        // Concerns:
        //: 1 The usage example provided in the component header file must
        //:   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Run the usage example.
        //
        // Testing:
        //   class bsls::AssertTestHandlerGuard
        // --------------------------------------------------------------------

        if (globalVerbose) printf("\nTESTING bsls::AssertTestHandlerGuard"
                                  "\n====================================\n");

        bsls::AssertFailureHandlerGuard outerGuard(&bsls::Assert::failAbort);

        ASSERT(&bsls::Assert::failAbort == bsls::Assert::failureHandler());
        {
            bsls::AssertTestHandlerGuard innerGuard;

            ASSERT(&bsls::AssertTest::failTestDriver ==
                                               bsls::Assert::failureHandler());
        }
        ASSERT(&bsls::Assert::failAbort == bsls::Assert::failureHandler());
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING BSLS_ASSERT_TEST "raw" macros
        //
        // Concerns:
        //   Each macro must not evaluate the passed expression unless enabled
        //   by the assert level that is in effect.  A '_PASS' macro should
        //   fail if and only if the passed expression raises an assertion
        //   through a 'BSLS_ASSERT' macro.  A '_FAIL' macro should fail if the
        //   passed expression does not raise an assertion.  Failure should not
        //   depend on the file where the assertion is raised.  Failure should
        //   be indicated by invoking an 'ASSERT' macro that is defined locally
        //   within the test driver.
        //
        // Plan:
        //   The results of these macros depend on the initial definition of a
        //   number of configuration macros, normally passed on the command
        //   line.  In order to test the full set of potential interactions,
        //   we shall take the unusual step of '#undef'ining the header guard
        //   for both this component and 'bsls_assert.h', in order to
        //   repeatedly '#include' these headers after setting up a "clean" set
        //   of configuration macros for each such inclusion to test.
        //   Note that the test code is extracted into a function defined after
        //   main as we will be using the preprocessor to avoid impacting any
        //   following test case that does not have the same issue.
        //
        // Testing:
        //   BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW
        //   BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW
        //   BSLS_ASSERTTEST_ASSERT_PASS_RAW
        //   BSLS_ASSERTTEST_ASSERT_FAIL_RAW
        //   BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW
        //   BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW
        // --------------------------------------------------------------------

        TestMacroBSLS_ASSERTTEST_PASS_OR_FAIL_RAW();
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING BSLS_ASSERT_TEST "basic" macros
        //
        // Concerns:
        //   Each macro must not evaluate the passed expression unless enabled
        //   by the assert level that is in effect.  A '_PASS' macro should
        //   fail if and only if the passed expression raises an assertion
        //   through a 'BSLS_ASSERT' macro.  A '_FAIL' macro should fail if the
        //   passed expression does not raise an assertion, or if the raised
        //   assertion originates in a file with a name that does not
        //   correspond to the component under test.  Failure should be
        //   indicated by invoking an 'ASSERT' macro that is defined locally
        //   within the test driver.
        //
        // Plan:
        //   The results of these macros depend on the initial definition of a
        //   number of configuration macros, normally passed on the command
        //   line.  In order to test the full set of potential interactions,
        //   we shall take the unusual step of '#undef'ining the header guard
        //   for both this component and 'bsls_assert.h', in order to
        //   repeatedly '#include' these headers after setting up a "clean" set
        //   of configuration macros for each such inclusion to test.
        //   Note that the test code is extracted into a function defined after
        //   main as we will be using the preprocessor to avoid impacting any
        //   following test case that does not have the same issue.
        //
        // Testing:
        //   BSLS_ASSERTTEST_ASSERT_SAFE_PASS
        //   BSLS_ASSERTTEST_ASSERT_SAFE_FAIL
        //   BSLS_ASSERTTEST_ASSERT_PASS
        //   BSLS_ASSERTTEST_ASSERT_FAIL
        //   BSLS_ASSERTTEST_ASSERT_OPT_PASS
        //   BSLS_ASSERTTEST_ASSERT_OPT_FAIL
        // --------------------------------------------------------------------

        TestMacroBSLS_ASSERTTEST_PASS_OR_FAIL();
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING catch-probes
        //
        // Concerns:
        //   Unless all three arguments to the catch-probe functions meet some
        //   validity constraints, the function must return 'false' and write
        //   a diagnostic message to the console.  Valid 'expectedResult' are
        //   either 'F' or 'P'.  Valid 'caughtException' attributes are a
        //   pointer to a non-empty null-terminated string for 'expression', a
        //   pointer to a non-empty null-terminated string for 'filename', and
        //   a non-negative 'lineNumber'.  Valid 'componentFileName' values are
        //   either a null pointer, or a pointer to a non-empty null-terminated
        //   string.
        //   When passed valid arguments, a catch-probe shall return 'false'
        //   unless 'expectedResult' == 'F' and either 'componentFileName' is
        //   null, or 'componentFileName' points to a filename that corresponds
        //   to the same component as referenced by the 'filename' attribute of
        //   'caughtException'.  Two filenames correspond to the same component
        //   if the file extension is either ".h", ".cpp" or ".t.cpp", and the
        //   filenames have the same value when stripped of both the extension
        //   and any leading pathname.
        //
        // Plan:
        //   We will take a table driven approach, verifying that each catch-
        //   probe returns the correct result combination of valid and invalid
        //   arguments.  As an 'expectedResult' of 'P' must always return
        //   'false', the table contains only the remaining arguments, and the
        //   "EXPECTED RESULT" from calling this function with 'F' for
        //   'expectedResult'.  Each iteration of the loop will test with both
        //   potential 'expectedResult' arguments, 'F' and 'P'.
        //
        //   Note that unless all three arguments to the catch-probe functions
        //   meet some validity constraints, the function will return 'false'
        //   and write a diagnostic message to the console.  As test drivers
        //   must run without such messages unless there is an error, we defer
        //   such test scenarios to test case -2.
        //
        //   By testing that two different sets of compatible names do not
        //   match each other, we do not need to add other valid component
        //   names into the set of potential invalid matches.
        //
        // Testing:
        //   bsls::AssertTest::catchProbe
        //   bsls::AssertTest::catchProbeRaw

        if (globalVerbose) printf("\nTESTING catch-probes"
                                  "\n====================\n");

        if (globalVerbose) printf("\nTESTING catchProbe"
                                  "\n==================\n");

        static const char * COMPATIBLE_NAMES_A[] = {
            "a.h",
            "a.cpp",
            "a.t.cpp",
            "/a.h",
            "/a.cpp",
            "/a.t.cpp",
            "//a.h",
            "/a.h/a.cpp",
            "/*/a.t.cpp",
        };
        const int NUM_COMPATIBLE_NAMES_A = sizeof COMPATIBLE_NAMES_A /
                                                    sizeof *COMPATIBLE_NAMES_A;

        static const char * COMPATIBLE_NAMES_ZZ[] = {
            "z.z.h",
            "z.z.cpp",
            "z.z.t.cpp",
            "/z.z.h",
            "/z.z.cpp",
            "/z.z.t.cpp",
            "//z.z.h",
            "/a.h/z.z.cpp", // deliberately use "a.h" and not "z.h" in the path
            "/*/z.z.t.cpp",
        };
        const int NUM_COMPATIBLE_NAMES_ZZ = sizeof COMPATIBLE_NAMES_ZZ /
                                                   sizeof *COMPATIBLE_NAMES_ZZ;

        // Ideally we would confirm the two sets of compatible names have the
        // same size using a static_assert, such as 'BSLMF_ASSERT'.
        // Unfortunately there is no portable tool available without creating
        // a dependency on a higher level component, so a run-time 'ASSERT'
        // must suffice.
        ASSERT(NUM_COMPATIBLE_NAMES_A == NUM_COMPATIBLE_NAMES_ZZ);

        // Note that any invalid component names in this list will produce
        // unwanted diagnostics on the terminal when running a passing test
        // driver.  We will test the invalid names in test case -2.
        // Note also that the invalid names should go into a second array,
        // as there are a couple of test results that would be different
        // when the incompatible name is tested against itself.
        static const char * INCOMPATIBLE_NAMES[] = {
            "aa.h",
            "t.cpp",
            "a.cpp.cpp.cpp",
            "a.t.t.cpp",
            "a.h.t.cpp",
            "a.cpp.h",
            "a.t.cpp.h",
        };
        const int NUM_INCOMPATIBLE_NAMES = sizeof INCOMPATIBLE_NAMES /
                                                    sizeof *INCOMPATIBLE_NAMES;

        static const struct {
            int         d_lineNumber;
            const char *d_expression;
            int         d_assertedLine;
        } DATA[] = {
            //     ASSERTED   LINE
            //LINE EXPRESSION NUMBER
            //---- ---------- ------
            { L_,  "0 != x",      13 },
            { L_,  "false",       42 },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE          = DATA[ti].d_lineNumber;
            const char *const EXPRESSION    = DATA[ti].d_expression;
            const int         ASSERTED_LINE = DATA[ti].d_assertedLine;

            for (int i = 0; i != NUM_COMPATIBLE_NAMES_A; ++i) {
                const bsls::AssertTestException EXCEPTION_AI(
                                                         EXPRESSION,
                                                         COMPATIBLE_NAMES_A[i],
                                                         ASSERTED_LINE);

                const bsls::AssertTestException EXCEPTION_ZZI(
                                                        EXPRESSION,
                                                        COMPATIBLE_NAMES_ZZ[i],
                                                        ASSERTED_LINE);

                bool testResult =
                            bsls::AssertTest::catchProbe('P', EXCEPTION_AI, 0);
                LOOP2_ASSERT(LINE, testResult, false == testResult);

                testResult = bsls::AssertTest::catchProbe('F',
                                                          EXCEPTION_AI,
                                                          0);
                LOOP2_ASSERT(LINE, testResult, true == testResult);

                testResult =
                           bsls::AssertTest::catchProbe('P', EXCEPTION_ZZI, 0);
                LOOP2_ASSERT(LINE, testResult, false == testResult);

                testResult =
                           bsls::AssertTest::catchProbe('F', EXCEPTION_ZZI, 0);
                LOOP2_ASSERT(LINE, testResult, true == testResult);

                for (int j = 0; j != NUM_COMPATIBLE_NAMES_A; ++j) {
                    const bsls::AssertTestException EXCEPTION_AJ(
                                                         EXPRESSION,
                                                         COMPATIBLE_NAMES_A[j],
                                                         ASSERTED_LINE);

                    const bsls::AssertTestException EXCEPTION_ZZJ(
                                                        EXPRESSION,
                                                        COMPATIBLE_NAMES_ZZ[j],
                                                        ASSERTED_LINE);

                    if (veryVerbose) {
                        P(EXCEPTION_AI.filename())
                        P(EXCEPTION_AJ.filename())
                        P(EXCEPTION_ZZI.filename())
                        P(EXCEPTION_ZZJ.filename())
                    }

                    testResult = bsls::AssertTest::catchProbe(
                                                        'P',
                                                        EXCEPTION_AI,
                                                        COMPATIBLE_NAMES_A[j]);
                    LOOP2_ASSERT(LINE, testResult, false == testResult);

                    testResult = bsls::AssertTest::catchProbe(
                                                       'P',
                                                       EXCEPTION_AI,
                                                       COMPATIBLE_NAMES_ZZ[j]);
                    LOOP2_ASSERT(LINE, testResult, false == testResult);

                    testResult = bsls::AssertTest::catchProbe(
                                                        'P',
                                                        EXCEPTION_ZZI,
                                                        COMPATIBLE_NAMES_A[j]);
                    LOOP2_ASSERT(LINE, testResult, false == testResult);

                    testResult = bsls::AssertTest::catchProbe(
                                                       'P',
                                                       EXCEPTION_ZZI,
                                                       COMPATIBLE_NAMES_ZZ[j]);
                    LOOP2_ASSERT(LINE, testResult, false == testResult);

                    testResult = bsls::AssertTest::catchProbe(
                                                        'F',
                                                        EXCEPTION_AI,
                                                        COMPATIBLE_NAMES_A[j]);
                    LOOP2_ASSERT(LINE, testResult, true == testResult);

                    testResult = bsls::AssertTest::catchProbe(
                                                       'F',
                                                       EXCEPTION_AI,
                                                       COMPATIBLE_NAMES_ZZ[j]);
                    LOOP2_ASSERT(LINE, testResult, false == testResult);

                    testResult = bsls::AssertTest::catchProbe(
                                                        'F',
                                                        EXCEPTION_ZZI,
                                                        COMPATIBLE_NAMES_A[j]);
                    LOOP2_ASSERT(LINE, testResult, false == testResult);

                    testResult = bsls::AssertTest::catchProbe(
                                                       'F',
                                                       EXCEPTION_ZZI,
                                                       COMPATIBLE_NAMES_ZZ[j]);
                    LOOP2_ASSERT(LINE, testResult, true == testResult);
                }

                for (int j = 0; j != NUM_INCOMPATIBLE_NAMES; ++j) {
                    if (veryVerbose) {
                        P_(EXCEPTION_AI.filename()) P(INCOMPATIBLE_NAMES[j])
                    }

                    testResult = bsls::AssertTest::catchProbe(
                                                        'P',
                                                        EXCEPTION_AI,
                                                        INCOMPATIBLE_NAMES[j]);
                    LOOP2_ASSERT(LINE, testResult, false == testResult);

                    testResult = bsls::AssertTest::catchProbe(
                                                        'F',
                                                        EXCEPTION_AI,
                                                        INCOMPATIBLE_NAMES[j]);
                    LOOP2_ASSERT(LINE, testResult, false == testResult);

                    if (veryVerbose) {
                        P_(EXCEPTION_ZZI.filename()) P(INCOMPATIBLE_NAMES[j])
                    }

                    testResult = bsls::AssertTest::catchProbe(
                                                        'P',
                                                        EXCEPTION_ZZI,
                                                        INCOMPATIBLE_NAMES[j]);
                    LOOP2_ASSERT(LINE, testResult, false == testResult);

                    testResult = bsls::AssertTest::catchProbe(
                                                        'F',
                                                        EXCEPTION_ZZI,
                                                        INCOMPATIBLE_NAMES[j]);
                    LOOP2_ASSERT(LINE, testResult, false == testResult);
                }
            }

            for (int i = 0; i != NUM_INCOMPATIBLE_NAMES; ++i) {
                const bsls::AssertTestException EXCEPTION_IN(
                                                         EXPRESSION,
                                                         INCOMPATIBLE_NAMES[i],
                                                         ASSERTED_LINE);

                bool testResult =
                            bsls::AssertTest::catchProbe('P', EXCEPTION_IN, 0);
                LOOP_ASSERT(LINE, false == testResult);

                testResult = bsls::AssertTest::catchProbe('F',
                                                          EXCEPTION_IN,
                                                          0);
                LOOP_ASSERT(LINE, true == testResult);

                for (int j = 0; j != NUM_COMPATIBLE_NAMES_A; ++j) {
                    testResult = bsls::AssertTest::catchProbe(
                                                        'P',
                                                        EXCEPTION_IN,
                                                        COMPATIBLE_NAMES_A[j]);
                    LOOP2_ASSERT(LINE, testResult, false == testResult);

                    testResult = bsls::AssertTest::catchProbe(
                                                       'P',
                                                       EXCEPTION_IN,
                                                       COMPATIBLE_NAMES_ZZ[j]);
                    LOOP2_ASSERT(LINE, testResult, false == testResult);

                    testResult = bsls::AssertTest::catchProbe(
                                                        'F',
                                                        EXCEPTION_IN,
                                                        COMPATIBLE_NAMES_A[j]);
                    LOOP2_ASSERT(LINE, testResult, false == testResult);

                    testResult = bsls::AssertTest::catchProbe(
                                                       'F',
                                                       EXCEPTION_IN,
                                                       COMPATIBLE_NAMES_ZZ[j]);
                    LOOP2_ASSERT(LINE, testResult, false == testResult);

               }

                for (int j = 0; j != NUM_INCOMPATIBLE_NAMES; ++j) {
                    if (veryVerbose) {
                        P_(EXCEPTION_IN.filename()) P(INCOMPATIBLE_NAMES[j])
                    }

                    testResult = bsls::AssertTest::catchProbe(
                                                        'P',
                                                        EXCEPTION_IN,
                                                        INCOMPATIBLE_NAMES[j]);
                    LOOP2_ASSERT(LINE, testResult, false == testResult);

                    testResult = bsls::AssertTest::catchProbe(
                                                        'F',
                                                        EXCEPTION_IN,
                                                        INCOMPATIBLE_NAMES[j]);
                    LOOP2_ASSERT(LINE, testResult, (i == j) == testResult);
                }
            }
        }

        if (verbose) printf("\nTESTING catchProbeRaw"
                            "\n=====================\n");

        ASSERT(false == bsls::AssertTest::catchProbeRaw('P'));
        ASSERT(true == bsls::AssertTest::catchProbeRaw('F'));
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING try-probes
        //
        // Concerns:
        //: 1 Each probe function returns 'true' if passed 'P' and 'false' if
        //:   passed 'F'.
        //: 2 Any other argument returns 'false' after writing a diagnostic
        //:   message regarding the bad argument(s) to the console.
        //: 3 Nothing is written to the console for the two recognized
        //:   arguments, 'F' and 'P'.
        //: 4 The recognized values 'F' and 'P' are case sensitive.
        //
        // Plan:
        //   A passing test case run in non-verbose mode should not write
        //   anything to the console, so the testing of all arguments other
        //   than 'F' or 'P' is deferred to the manually run test case, -1.
        //   Each try-probe function is called to confirm it produces the
        //   required value for the two supported arguments, 'F' must always
        //   return 'false' and 'P' must always return 'true'.
        //
        // Testing:
        //   bsls::AssertTest::tryProbe
        //   bsls::AssertTest::tryProbeRaw
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING try-probes"
                            "\n==================\n");

        if (verbose) printf("\nTESTING tryProbe"
                            "\n================\n");

        ASSERT(true == bsls::AssertTest::tryProbe('P'));
        ASSERT(false == bsls::AssertTest::tryProbe('F'));

        if (verbose) printf("\nTESTING tryProbeRaw"
                            "\n===================\n");

        ASSERT(true == bsls::AssertTest::tryProbeRaw('P'));
        ASSERT(false == bsls::AssertTest::tryProbeRaw('F'));
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING bsls::AssertTest::failTestDriver
        //
        // Concerns:
        //: 1 The function 'bsls::AssertTest::failTestDriver' can be installed
        //:   as an assertion-failure handler with 'bsls::Assert'.
        //: 2 The function throws an exception of type
        //:   'bsls::AssertTestException' whose attributes exactly match the
        //:   arguments to the function call.
        //
        // Plan:
        //   First, we will install 'bsls::AssertTest::failTestDriver' as the
        //   active assertion-failure handler function, demonstrating it has
        //   the required signature.  Then a table-driven approach will
        //   demonstrate that each call to the function throws an exception
        //   having the expected attributes.
        //
        // Testing:
        //   failTestDriver
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING bsls::AssertTest::failTestDriver"
                            "\n========================================\n");

#if !defined(BDE_BUILD_TARGET_EXC)
        if (verbose) {
            printf("\nTest not supported unless exceptions are available.\n");
        }
#else

        // First we install 'bsls::AssertTest::failTestDriver' as the active
        // assertion-failure handler, to verify it has the correct signature.
        bsls::AssertFailureHandlerGuard guard(
                                            &bsls::AssertTest::failTestDriver);

        // Next we verify that the installed assertion-failure handler throws
        // the expected exception when called directly.
        static const struct {
            int         d_lineNumber;       // line # of the row in this table
            const char *d_expression;
            const char *d_filename;
            int         d_testLine;
        } DATA[] = {
            //LINE  EXPRESSION  FILENAME  TESTLINE
            //----  ----------  -------   --------
            { L_,   "",         "",       0       },
            { L_,   "Testing",  "foo.h",  123     },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE       = DATA[ti].d_lineNumber;
            const char *const EXPRESSION = DATA[ti].d_expression;
            const char *const FILENAME   = DATA[ti].d_filename;
            const int         TESTLINE   = DATA[ti].d_testLine;

            if (veryVerbose) {
                T_ P_(EXPRESSION) P_(FILENAME) P(TESTLINE)
            }

            // Validate test description.
            try {
                bsls::AssertTest::failTestDriver(
                                               EXPRESSION, FILENAME, TESTLINE);
            }
            catch(const bsls::AssertTestException& ex) {
                LOOP3_ASSERT(LINE, EXPRESSION, ex.expression(),
                             0 == strcmp(EXPRESSION, ex.expression()));
                LOOP3_ASSERT(LINE, FILENAME, ex.filename(),
                             0 == strcmp(FILENAME, ex.filename()));
                LOOP3_ASSERT(LINE, TESTLINE, ex.lineNumber(),
                             TESTLINE == ex.lineNumber());
            }
        }
#endif
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING the macro BSLS_ASSERTTEST_IS_ACTIVE
        //
        // Concerns:
        //: 1 The macro expands to an expression that evaluates to either
        //:   'true' or 'false', according to the current build mode and the
        //:   string passed to the macro.
        //: 2 There are six valid strings, "S", "A", "O", "S2", "A2", and "O2",
        //:   each of which must be recognized, in all possible build modes.
        //
        // Plan:
        //   The result of this macro depends on the initial definition of a
        //   number of configuration macros, normally passed on the command
        //   line.  In order to test the full set of potential interactions,
        //   we shall take the unusual step of '#undef'ining the header guard
        //   for both this component and 'bsls_assert.h', in order to
        //   repeatedly '#include' these headers after setting up a "clean" set
        //   of configuration macros for each such inclusion to test.
        //   Each configuration shall validate the macro for all 6 valid
        //   strings.
        //   Note that the test code is extracted into a function defined after
        //   main as we will be using the preprocessor to avoid impacting any
        //   following test case that does not have the same issue.
        //
        // Testing:
        //   BSLS_ASSERTTEST_IS_ACTIVE

        TestMacroBSLS_ASSERTTEST_IS_ACTIVE();
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING bsls::AssertTest::isValidExpected
        //
        // Concerns:
        //: 1 The function should return 'true' for only two possible values,
        //:   'F' and 'P'.
        //: 2 All other test values return false.
        //: 3 The function is case-sensitive.
        //
        // Plan:
        //   As the set of characters is enumerable and small, iterate over all
        //   possible character values calling 'isValidExpected'.  The result
        //   may be 'true' for only two values, 'F' and 'P'.
        //
        // Testing:
        //   bsls::AssertTest::isValidExpected
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING isValidExpected"
                            "\n=======================\n");

        for (char c = CHAR_MIN; c != CHAR_MAX; ++c) {
            const bool expectedResult = 'F' == c || 'P' == c;
            if (veryVerbose) {
                T_ P_(c) P(expectedResult)
            }

            const bool testResult = bsls::AssertTest::isValidExpected(c);
            LOOP2_ASSERT(c, expectedResult, expectedResult == testResult);

        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING bsls::AssertTest::isValidAssertBuild
        //
        // Concerns:
        //: 1 The function returns 'true' for the six recognized string values,
        //:   and false for all other strings.
        //: 2 The string comparison is case sensitive.
        //: 3 The order of the characters is significant for two character
        //:   strings.
        //: 4 Neither leading nor trailing whitespace are valid.
        //: 5 The function gracefully handles edge cases like a null pointer,
        //:   or the empty string, "".
        //
        // Plan:
        //   Using a basic test-table, test a representative sample of 0, 1, 2
        //   and 3 character strings, covering the valid values, and simple
        //   permutation of those valid values.
        //
        // Testing:
        //   bsls::AssertTest::isValidAssertBuild
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING isValidAssertBuildType"
                            "\n==============================\n");

        static const struct {
            int         d_lineNumber;       // line # of the row in this table
            const char *d_assertBuildType;  // "S", "S2", "A", "A2", "O", "O2"
            bool        d_expectedResult;   // 'true' or 'false'

        } DATA[] = {

            //LINE  TYPE   RESULT
            //----  ----   ------
            { L_,   0,     false  },
            { L_,   "",    false  },
            { L_,   "S",   true   },
            { L_,   "A",   true   },
            { L_,   "O",   true   },
            { L_,   "X",   false  },
            { L_,   "S2",  true   },
            { L_,   "A2",  true   },
            { L_,   "O2",  true   },
            { L_,   "X2",  false  },
            { L_,   "SY",  false  },
            { L_,   "AY",  false  },
            { L_,   "OY",  false  },
            { L_,   "XY",  false  },
            { L_,   "S2Z", false  },
            { L_,   "A2Z", false  },
            { L_,   "O2Z", false  },
            { L_,   "X2Z", false  },
            { L_,   "s",   false  },
            { L_,   "a",   false  },
            { L_,   "o",   false  },
            { L_,   "s2",  false  },
            { L_,   "a2",  false  },
            { L_,   "o2",  false  },
            { L_,   "SS",  false  },
            { L_,   "AA",  false  },
            { L_,   "OO",  false  },
            { L_,   "2S",  false  },
            { L_,   "2A",  false  },
            { L_,   "2O",  false  },
            { L_,   " S",  false  },
            { L_,   " A",  false  },
            { L_,   " O",  false  },
            { L_,   " S2", false  },
            { L_,   " A2", false  },
            { L_,   " O2", false  },
            { L_,   "S ",  false  },
            { L_,   "A ",  false  },
            { L_,   "O ",  false  },
            { L_,   "S2 ", false  },
            { L_,   "A2 ", false  },
            { L_,   "O2 ", false  },

        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_lineNumber;
            const char *const TYPE   = DATA[ti].d_assertBuildType;
            bool              RESULT = DATA[ti].d_expectedResult;

            if (veryVerbose) {
                T_ P_(TYPE) P(RESULT)
            }

            // Validate test description.
            bool testResult = bsls::AssertTest::isValidAssertBuild(TYPE);
            LOOP3_ASSERT(LINE, TYPE, RESULT, RESULT == testResult);
        }  // table-driven 'for' loop
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   Want to observe the basic operation of this component.  This test
        //   will be affected by the current build flags, and must produce the
        //   correct result in each case.
        //
        // Plan:
        //   Install the negative testing assertion-failure handler, and then
        //   invoke the 'OPT' version of the test macros, as this is the form
        //   enabled in most build modes.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        bsls::AssertFailureHandlerGuard guard(
                                            &bsls::AssertTest::failTestDriver);

        BSLS_ASSERTTEST_ASSERT_OPT_PASS(BSLS_ASSERT_OPT(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(BSLS_ASSERT_OPT(false));

      } break;
      case -1: {
        // --------------------------------------------------------------------
        // TESTING bad characters with try probes
        //
        // Concerns:
        //: 1 Each probe function must return 'true' if passed 'P' and 'false'
        //:   if passed 'F'.
        //: 2 Any other argument must return 'false' after writing a diagnostic
        //:   message regarding the bad argument to the console.
        //: 3 Nothing is written to the console for the two recognized
        //:   arguments, 'F' and 'P'.
        //: 4 The recognized values 'F' and 'P' are case sensitive.
        //
        // Plan:
        //   Invoke each probe with every possible 'char' value.  Test that
        //   'true' is returned for only the argument 'P', that nothing is
        //   written to the console when the argument is 'P' or 'F', and that
        //   something is written to the console for any other argument.  The
        //   diagnostic message will be validated interactively by the user
        //   reading the console.
        //
        // Testing:
        //   bsls::AssertTest::tryProbe
        //   bsls::AssertTest::tryProbeRaw
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING bad characters with try probes"
                            "\n======================================\n");

        if (verbose) printf("\nTESTING bsls::AssertTest::tryProbe"
                            "\n==================================\n");

        for (char c = CHAR_MIN; c != CHAR_MAX; ++c) {
            if ('P' == c) {
                printf("== 'P' should not print a diagnostic\n");
                ASSERT(true == bsls::AssertTest::tryProbe(c));
            }
            else if ('F' == c) {
                printf("== 'F' should not print a diagnostic\n");
                ASSERT(false == bsls::AssertTest::tryProbe(c));
            }
            else {
                printf("-- invalid argumemt '%c' should print a diagnostic\n",
                       c);
                ASSERT(false == bsls::AssertTest::tryProbe(c));
            }
        }

        if (verbose) printf("\nTESTING bsls::AssertTest::tryProbeRaw"
                            "\n=====================================\n");

        for (char c = CHAR_MIN; c != CHAR_MAX; ++c) {
            if ('P' == c) {
                printf("== 'P' should not print a diagnostic\n");
                ASSERT(true == bsls::AssertTest::tryProbeRaw(c));
            }
            else if ('F' == c) {
                printf("== 'F' should not print a diagnostic\n");
                ASSERT(false == bsls::AssertTest::tryProbeRaw(c));
            }
            else {
                printf("-- invalid argumemt '%c' should print a diagnostic\n",
                       c);
                ASSERT(false == bsls::AssertTest::tryProbeRaw(c));
            }
        }
      } break;
      case -2: {
        // --------------------------------------------------------------------
        // TESTING catch-probes with bad arguments
        //
        // Concerns:
        //   Each probe function must return 'true' if passed 'P' and 'false'
        //   if passed 'F'.  Any other argument must return 'false' after
        //   writing a diagnostic message regarding the bad argument to the
        //   console.  Nothing should be written to the console for the two
        //   recognized arguments.  The recognized values 'F' and 'P' are
        //   case sensitive.
        //
        // Plan:
        //   Invoke each probe with every possible 'char' value.  Test that
        //   'true' is returned for only the argument 'P', that nothing is
        //   written to the console when the argument is 'P' or 'F', and that
        //   something is written to the console for any other argument.  The
        //   diagnostic message will be validated interactively by the user
        //   reading the console.
        //
        // Testing:
        //   bsls::AssertTest::tryProbe
        //   bsls::AssertTest::tryProbeRaw
        // --------------------------------------------------------------------

        if (globalVerbose) printf(
                                "\nTESTING catch-probes with bad arguments"
                                "\n=======================================\n");

        if (globalVerbose) printf("\nTESTING catchProbe"
                                  "\n==================\n");

        static const char * COMPATIBLE_NAMES_A[] = {
            "a.h",
            "a.cpp",
            "a.t.cpp",
            "/a.h",
            "/a.cpp",
            "/a.t.cpp",
            "//a.h",
            "/a.h/a.cpp",
            "/*/a.t.cpp",
        };
        const int NUM_COMPATIBLE_NAMES_A = sizeof COMPATIBLE_NAMES_A /
                                                    sizeof *COMPATIBLE_NAMES_A;

        static const char * COMPATIBLE_NAMES_ZZ[] = {
            "z.z.h",
            "z.z.cpp",
            "z.z.t.cpp",
            "/z.z.h",
            "/z.z.cpp",
            "/z.z.t.cpp",
            "//z.z.h",
            "/a.h/z.z.cpp", // deliberately use "a.h" and not "z.h" in the path
            "/*/z.z.t.cpp",
        };
        const int NUM_COMPATIBLE_NAMES_ZZ = sizeof COMPATIBLE_NAMES_ZZ /
                                                   sizeof *COMPATIBLE_NAMES_ZZ;

        // Ideally we would confirm the two sets of compatible names have the
        // same size using a static_assert, such as 'BSLMF_ASSERT'.
        // Unfortunately there is no portable tool available without creating
        // a dependency on a higher level component, so a run-time 'ASSERT'
        // must suffice.
        ASSERT(NUM_COMPATIBLE_NAMES_A == NUM_COMPATIBLE_NAMES_ZZ);

        // Note that any invalid component names in this list will produce
        // unwanted diagnostics on the terminal when running a passing test
        // driver.  We will test the invalid names in test case -2.
        // Note also that the invalid names should go into a second array,
        // as there are a couple of test results that would be different
        // when the incompatible name is tested against itself.
        static const char *const INCOMPATIBLE_NAMES[] = {
            "aa.h",
            "t.cpp",
            "a.cpp.cpp.cpp",
            "a.t.t.cpp",
            "a.h.t.cpp",
            "a.cpp.h",
            "a.t.cpp.h",
            "a.T.cpp",
            "zz.h.cpp",
         };
        const int NUM_INCOMPATIBLE_NAMES = sizeof INCOMPATIBLE_NAMES /
                                                    sizeof *INCOMPATIBLE_NAMES;

        static const char *const INVALID_NAMES[] = {
            "",
            "a",
            "a.",
            "a.hh",
            "a.cppp",
            "a.t.cppp",
            "a.tcpp",
            "acpp",
            "*",
            "a*",
            "A.*",
            "a.H",
            "a.Cpp",
            "a.cPp",
            "a.cpP",
            "A.CPP",
            "a.t.Cpp",
            "a.t.cPp",
            "a.t.cpP",
            "A.T.CPP",
        };
        const size_t NUM_INVALID_NAMES = sizeof INVALID_NAMES /
                                                         sizeof *INVALID_NAMES;

        static const struct {
            int         d_lineNumber;
            const char *d_expression;
            int         d_assertedLine;
            bool        d_result;
        } DATA[] = {
            //     ASSERTED   LINE     BEST
            //LINE EXPRESSION NUMBER   RESULT
            //---- ---------- ------   ------
            { L_,  "",             0,  false },
            { L_,  "0 != x",       0,  false },
            { L_,  "",            13,  false },
            { L_,  "0 != x",      13,  true  },
            { L_,  "false",       42,  true  },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE          = DATA[ti].d_lineNumber;
            const char *const EXPRESSION    = DATA[ti].d_expression;
            const int         ASSERTED_LINE = DATA[ti].d_assertedLine;
            const bool        RESULT        = DATA[ti].d_result;

            for (int i = 0; i != NUM_COMPATIBLE_NAMES_A; ++i) {
                if (veryVerbose) {
                    P_(i) P_(EXPRESSION) P_(ASSERTED_LINE) P(RESULT)
                    P_(COMPATIBLE_NAMES_A[i]) P(COMPATIBLE_NAMES_ZZ[i])
                }

                const bsls::AssertTestException EXCEPTION_AI(
                                                         EXPRESSION,
                                                         COMPATIBLE_NAMES_A[i],
                                                         ASSERTED_LINE);

                const bsls::AssertTestException EXCEPTION_ZZI(
                                                        EXPRESSION,
                                                        COMPATIBLE_NAMES_ZZ[i],
                                                        ASSERTED_LINE);

                bool testResult =
                            bsls::AssertTest::catchProbe('P', EXCEPTION_AI, 0);
                LOOP2_ASSERT(LINE, testResult, false == testResult);

                testResult = bsls::AssertTest::catchProbe('F',
                                                          EXCEPTION_AI,
                                                          0);
                LOOP3_ASSERT(LINE, RESULT, testResult, RESULT == testResult);

                testResult =
                           bsls::AssertTest::catchProbe('P', EXCEPTION_ZZI, 0);
                LOOP2_ASSERT(LINE, testResult, false == testResult);

                testResult =
                           bsls::AssertTest::catchProbe('F', EXCEPTION_ZZI, 0);
                LOOP3_ASSERT(LINE, RESULT, testResult, RESULT == testResult);

                for (int j = 0; j != NUM_COMPATIBLE_NAMES_A; ++j) {
                    const bsls::AssertTestException EXCEPTION_AJ(
                                                         EXPRESSION,
                                                         COMPATIBLE_NAMES_A[j],
                                                         ASSERTED_LINE);

                    const bsls::AssertTestException EXCEPTION_ZZJ(
                                                        EXPRESSION,
                                                        COMPATIBLE_NAMES_ZZ[j],
                                                        ASSERTED_LINE);

                    if (veryVerbose) {
                        P(EXCEPTION_AI.filename())
                        P(EXCEPTION_AJ.filename())
                        P(EXCEPTION_ZZI.filename())
                        P(EXCEPTION_ZZJ.filename())
                    }

                    testResult = bsls::AssertTest::catchProbe(
                                                        'P',
                                                        EXCEPTION_AI,
                                                        COMPATIBLE_NAMES_A[j]);
                    LOOP2_ASSERT(LINE, testResult, false == testResult);

                    testResult = bsls::AssertTest::catchProbe(
                                                       'P',
                                                       EXCEPTION_AI,
                                                       COMPATIBLE_NAMES_ZZ[j]);
                    LOOP2_ASSERT(LINE, testResult, false == testResult);

                    testResult = bsls::AssertTest::catchProbe(
                                                        'P',
                                                        EXCEPTION_ZZI,
                                                        COMPATIBLE_NAMES_A[j]);
                    LOOP2_ASSERT(LINE, testResult, false == testResult);

                    testResult = bsls::AssertTest::catchProbe(
                                                       'P',
                                                       EXCEPTION_ZZI,
                                                       COMPATIBLE_NAMES_ZZ[j]);
                    LOOP2_ASSERT(LINE, testResult, false == testResult);

                    testResult = bsls::AssertTest::catchProbe(
                                                        'F',
                                                        EXCEPTION_AI,
                                                        COMPATIBLE_NAMES_A[j]);
                    LOOP3_ASSERT(LINE, testResult, RESULT,
                                 testResult == RESULT);

                    testResult = bsls::AssertTest::catchProbe(
                                                       'F',
                                                       EXCEPTION_AI,
                                                       COMPATIBLE_NAMES_ZZ[j]);
                    LOOP3_ASSERT(LINE, testResult, RESULT,
                                 false == testResult);

                    testResult = bsls::AssertTest::catchProbe(
                                                        'F',
                                                        EXCEPTION_ZZI,
                                                        COMPATIBLE_NAMES_A[j]);
                    LOOP3_ASSERT(LINE, testResult, RESULT,
                                 false == testResult);

                    testResult = bsls::AssertTest::catchProbe(
                                                       'F',
                                                       EXCEPTION_ZZI,
                                                       COMPATIBLE_NAMES_ZZ[j]);
                    LOOP3_ASSERT(LINE, testResult, RESULT,
                                 testResult == RESULT);
                }

                for (int j = 0; j != NUM_INCOMPATIBLE_NAMES; ++j) {
                    if (veryVerbose) {
                        P_(EXCEPTION_AI.filename()) P(INCOMPATIBLE_NAMES[j])
                    }

                    testResult = bsls::AssertTest::catchProbe(
                                                        'P',
                                                        EXCEPTION_AI,
                                                        INCOMPATIBLE_NAMES[j]);
                    LOOP2_ASSERT(LINE, testResult, false == testResult);

                    testResult = bsls::AssertTest::catchProbe(
                                                        'F',
                                                        EXCEPTION_AI,
                                                        INCOMPATIBLE_NAMES[j]);
                    LOOP2_ASSERT(LINE, testResult, false == testResult);

                    if (veryVerbose) {
                        P_(EXCEPTION_ZZI.filename()) P(INCOMPATIBLE_NAMES[j])
                    }

                    testResult = bsls::AssertTest::catchProbe(
                                                        'P',
                                                        EXCEPTION_ZZI,
                                                        INCOMPATIBLE_NAMES[j]);
                    LOOP2_ASSERT(LINE, testResult, false == testResult);

                    testResult = bsls::AssertTest::catchProbe(
                                                        'F',
                                                        EXCEPTION_ZZI,
                                                        INCOMPATIBLE_NAMES[j]);
                    LOOP2_ASSERT(LINE, testResult, false == testResult);
                }

                for (size_t j = 0; j != NUM_INVALID_NAMES; ++j) {
                    if(veryVerbose) {
                        P_(EXCEPTION_AI.filename()) P(INVALID_NAMES[j])
                    }

                    testResult = bsls::AssertTest::catchProbe(
                                                            'P',
                                                             EXCEPTION_AI,
                                                             INVALID_NAMES[j]);
                    LOOP2_ASSERT(LINE, testResult, false == testResult);

                    testResult = bsls::AssertTest::catchProbe(
                                                             'F',
                                                             EXCEPTION_AI,
                                                             INVALID_NAMES[j]);
                    LOOP2_ASSERT(LINE, testResult, false == testResult);

                    if(veryVerbose) {
                        P_(EXCEPTION_ZZI.filename()) P(INVALID_NAMES[j])
                    }

                    testResult = bsls::AssertTest::catchProbe(
                                                             'P',
                                                             EXCEPTION_ZZI,
                                                             INVALID_NAMES[j]);
                    LOOP2_ASSERT(LINE, testResult, false == testResult);

                    testResult = bsls::AssertTest::catchProbe(
                                                             'F',
                                                             EXCEPTION_ZZI,
                                                             INVALID_NAMES[j]);
                    LOOP2_ASSERT(LINE, testResult, false == testResult);
                }
            }

            for (int i = 0; i != NUM_INCOMPATIBLE_NAMES; ++i) {
                const bsls::AssertTestException EXCEPTION_IN(
                                                         EXPRESSION,
                                                         INCOMPATIBLE_NAMES[i],
                                                         ASSERTED_LINE);

                bool testResult =
                            bsls::AssertTest::catchProbe('P', EXCEPTION_IN, 0);
                LOOP2_ASSERT(LINE, testResult, false == testResult);

                testResult = bsls::AssertTest::catchProbe('F',
                                                          EXCEPTION_IN,
                                                          0);
                LOOP3_ASSERT(LINE, RESULT, testResult, RESULT == testResult);

                for (int j = 0; j != NUM_COMPATIBLE_NAMES_A; ++j) {
                    testResult = bsls::AssertTest::catchProbe(
                                                        'P',
                                                        EXCEPTION_IN,
                                                        COMPATIBLE_NAMES_A[j]);
                    LOOP2_ASSERT(LINE, testResult, false == testResult);

                    testResult = bsls::AssertTest::catchProbe(
                                                       'P',
                                                       EXCEPTION_IN,
                                                       COMPATIBLE_NAMES_ZZ[j]);
                    LOOP2_ASSERT(LINE, testResult, false == testResult);

                    testResult = bsls::AssertTest::catchProbe(
                                                        'F',
                                                        EXCEPTION_IN,
                                                        COMPATIBLE_NAMES_A[j]);
                    LOOP2_ASSERT(LINE, testResult, false == testResult);

                    testResult = bsls::AssertTest::catchProbe(
                                                       'F',
                                                       EXCEPTION_IN,
                                                       COMPATIBLE_NAMES_ZZ[j]);
                    LOOP2_ASSERT(LINE, testResult, false == testResult);

               }

               for (int j = 0; j != NUM_INCOMPATIBLE_NAMES; ++j) {
                    const bool FAIL_RESULT = RESULT && (i == j);
                    if(veryVerbose) {
                        P_(i) P_(j) P(FAIL_RESULT)
                        P_(EXCEPTION_IN.filename()) P(INCOMPATIBLE_NAMES[j])
                    }

                    testResult = bsls::AssertTest::catchProbe(
                                                        'P',
                                                        EXCEPTION_IN,
                                                        INCOMPATIBLE_NAMES[j]);
                    LOOP2_ASSERT(LINE, testResult, false == testResult);

                    testResult = bsls::AssertTest::catchProbe(
                                                        'F',
                                                        EXCEPTION_IN,
                                                        INCOMPATIBLE_NAMES[j]);
                    LOOP3_ASSERT(LINE, FAIL_RESULT, testResult
                                                  , FAIL_RESULT == testResult);
                }

                for (size_t j = 0; j != NUM_INVALID_NAMES; ++j) {
                    if(veryVerbose) {
                        P_(EXCEPTION_IN.filename()) P(INVALID_NAMES[j])
                    }

                    testResult = bsls::AssertTest::catchProbe(
                                                             'P',
                                                             EXCEPTION_IN,
                                                             INVALID_NAMES[j]);
                    LOOP2_ASSERT(LINE, testResult, false == testResult);

                    testResult = bsls::AssertTest::catchProbe(
                                                             'F',
                                                             EXCEPTION_IN,
                                                             INVALID_NAMES[j]);
                    LOOP2_ASSERT(LINE, testResult, false == testResult);
                }
            }

            for (size_t i = 0; i != NUM_INVALID_NAMES; ++i) {
                const bsls::AssertTestException EXCEPTION_IN(EXPRESSION,
                                                             INVALID_NAMES[i],
                                                             ASSERTED_LINE);

                bool testResult =
                            bsls::AssertTest::catchProbe('P', EXCEPTION_IN, 0);
                LOOP2_ASSERT(LINE, testResult, false == testResult);

                testResult = bsls::AssertTest::catchProbe('F',
                                                          EXCEPTION_IN,
                                                          0);
                LOOP2_ASSERT(LINE, testResult, false == testResult);

                for (int j = 0; j != NUM_COMPATIBLE_NAMES_A; ++j) {
                    testResult = bsls::AssertTest::catchProbe(
                                                        'P',
                                                        EXCEPTION_IN,
                                                        COMPATIBLE_NAMES_A[j]);
                    LOOP2_ASSERT(LINE, testResult, false == testResult);

                    testResult = bsls::AssertTest::catchProbe(
                                                       'P',
                                                       EXCEPTION_IN,
                                                       COMPATIBLE_NAMES_ZZ[j]);
                    LOOP2_ASSERT(LINE, testResult, false == testResult);

                    testResult = bsls::AssertTest::catchProbe(
                                                        'F',
                                                        EXCEPTION_IN,
                                                        COMPATIBLE_NAMES_A[j]);
                    LOOP2_ASSERT(LINE, testResult, false == testResult);

                    testResult = bsls::AssertTest::catchProbe(
                                                       'F',
                                                       EXCEPTION_IN,
                                                       COMPATIBLE_NAMES_ZZ[j]);
                    LOOP2_ASSERT(LINE, testResult, false == testResult);

               }

                for (int j = 0; j != NUM_INCOMPATIBLE_NAMES; ++j) {
                    if(veryVerbose) {
                        P_(EXCEPTION_IN.filename()) P(INCOMPATIBLE_NAMES[j])
                    }

                    testResult = bsls::AssertTest::catchProbe(
                                                        'P',
                                                        EXCEPTION_IN,
                                                        INCOMPATIBLE_NAMES[j]);
                    LOOP2_ASSERT(LINE, testResult, false == testResult);

                    testResult = bsls::AssertTest::catchProbe(
                                                        'F',
                                                        EXCEPTION_IN,
                                                        INCOMPATIBLE_NAMES[j]);
                    LOOP2_ASSERT(LINE, testResult, false == testResult);
                }

                for (size_t j = 0; j != NUM_INVALID_NAMES; ++j) {
                    if(veryVerbose) {
                        P_(EXCEPTION_IN.filename()) P(INVALID_NAMES[j])
                    }

                    testResult = bsls::AssertTest::catchProbe(
                                                            'P',
                                                             EXCEPTION_IN,
                                                             INVALID_NAMES[j]);
                    LOOP2_ASSERT(LINE, testResult, false == testResult);

                    testResult = bsls::AssertTest::catchProbe(
                                                             'F',
                                                             EXCEPTION_IN,
                                                             INVALID_NAMES[j]);
                    LOOP2_ASSERT(LINE, testResult, false == testResult);
                }
            }
        }
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

void TestMacroBSLS_ASSERTTEST_IS_ACTIVE()
{
    if (globalVerbose) printf("\nTESTING BSLS_ASSERTTEST_IS_ACTIVE"
                              "\n=================================\n");

    if (globalVerbose)
        printf("\nWe need to write a running commentary\n");

//    bsls::Assert::setFailureHandler(&AssertFailed::failMacroTest);

//  Config macros    Configuration  Expected results
//  OVERRIDE SAFE2  OPT  DBG  SAFE   O A S O2 A2 S2
//  -------- -----  ---  ---  ----   - - - -- -- --
//  _NONE       X
//  _OPT        X    X               X      X
//  _DEBUG      X    X    X          X X    X  X
//  _SAFE       X    X    X    X     X X X  X  X  X
//  _NONE
//  _OPT             X               X
//  _DEBUG           X    X          X X
//  _SAFE            X    X    X     X X X

//===================== SAFE_2 LEVEL_NONE ===============================//

    // Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_NONE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

#undef BSLS_ASSERTTEST_ASSERT_FAIL
#undef BSLS_ASSERTTEST_ASSERT_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_OPT_FAIL
#undef BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_OPT_PASS
#undef BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW
#undef BSLS_ASSERTTEST_ASSERT_PASS
#undef BSLS_ASSERTTEST_ASSERT_PASS_RAW
#undef BSLS_ASSERTTEST_ASSERT_SAFE_FAIL
#undef BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_SAFE_PASS
#undef BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW

#undef BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_BRUTE_FORCE_IMP
#undef BSLS_ASSERTTEST_BRUTE_FORCE_IMP_RAW
#undef BSLS_ASSERTTEST_IS_ACTIVE
#undef BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG

    //  Define the macros for this test case
#define BDE_BUILD_TARGET_SAFE_2
#define BSLS_ASSERT_LEVEL_NONE

    //  Re-include the bsls_assert headers
#undef INCLUDED_BSLS_ASSERT
#undef INCLUDED_BSLS_ASSERTTEST
#include <bsls_assert.h>
#include <bsls_asserttest.h>

    //  Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
    // 'IS_ACTIVE' macros and the are values we intend to test.
#if !defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

    ASSERT(BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG);
    ASSERT(!BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG);
    ASSERT(!BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG);
    ASSERT(!BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG);

    {
        static const struct {
            int         d_lineNumber;       // line # of the row in this table
            const char *d_assertBuildType;  // "S", "S2", "A", "A2", "O", "O2"
            bool        d_expectedResult;   // assertion: 'P'ass or 'F'ail
        } DATA[] = {
            //LINE  TYPE  RESULT
            //----  ----  ------
            { L_,   "O",  false },
            { L_,   "O2", false },
            { L_,   "A",  false },
            { L_,   "A2", false },
            { L_,   "S",  false },
            { L_,   "S2", false },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_lineNumber;
            const char *const TYPE   = DATA[ti].d_assertBuildType;
            bool              RESULT = DATA[ti].d_expectedResult;

            if(globalVeryVerbose) {
                T_ P_(TYPE) P(RESULT)
            }

            // Validate test description.
            LOOP_ASSERT(LINE, bsls::AssertTest::isValidAssertBuild(TYPE));

            bool configResult = BSLS_ASSERTTEST_IS_ACTIVE(TYPE);
            LOOP3_ASSERT(TYPE, RESULT, configResult, RESULT == configResult);

        }  // table-driven 'for' loop
    }

//===================== SAFE_2 LEVEL_ASSERT_OPT =============================//

    // Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_NONE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

#undef BSLS_ASSERTTEST_ASSERT_FAIL
#undef BSLS_ASSERTTEST_ASSERT_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_OPT_FAIL
#undef BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_OPT_PASS
#undef BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW
#undef BSLS_ASSERTTEST_ASSERT_PASS
#undef BSLS_ASSERTTEST_ASSERT_PASS_RAW
#undef BSLS_ASSERTTEST_ASSERT_SAFE_FAIL
#undef BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_SAFE_PASS
#undef BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW

#undef BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_BRUTE_FORCE_IMP
#undef BSLS_ASSERTTEST_BRUTE_FORCE_IMP_RAW
#undef BSLS_ASSERTTEST_IS_ACTIVE
#undef BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG

    //  Define the macros for this test case
#define BDE_BUILD_TARGET_SAFE_2
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

    //  Re-include the bsls_assert headers
#undef INCLUDED_BSLS_ASSERT
#undef INCLUDED_BSLS_ASSERTTEST
#include <bsls_assert.h>
#include <bsls_asserttest.h>

    //  Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
    // 'IS_ACTIVE' macros and the are values we intend to test.
#if !defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

    ASSERT(BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG);
    ASSERT(BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG);
    ASSERT(!BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG);
    ASSERT(!BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG);

    {
        static const struct {
            int         d_lineNumber;       // line # of the row in this table
            const char *d_assertBuildType;  // "S", "S2", "A", "A2", "O", "O2"
            bool        d_expectedResult;   // assertion: 'P'ass or 'F'ail
        } DATA[] = {
            //LINE  TYPE  RESULT
            //----  ----  ------
            { L_,   "O",  true  },
            { L_,   "O2", true  },
            { L_,   "A",  false },
            { L_,   "A2", false },
            { L_,   "S",  false },
            { L_,   "S2", false },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_lineNumber;
            const char *const TYPE   = DATA[ti].d_assertBuildType;
            bool              RESULT = DATA[ti].d_expectedResult;

            if(globalVeryVerbose) {
                T_ P_(TYPE) P(RESULT)
            }

            // Validate test description.
            LOOP_ASSERT(LINE, bsls::AssertTest::isValidAssertBuild(TYPE));

            bool configResult = BSLS_ASSERTTEST_IS_ACTIVE(TYPE);
            LOOP3_ASSERT(TYPE, RESULT, configResult, RESULT == configResult);

        }  // table-driven 'for' loop
    }

//===================== SAFE_2 LEVEL_ASSERT ===============================//

    // Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_NONE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

#undef BSLS_ASSERTTEST_ASSERT_FAIL
#undef BSLS_ASSERTTEST_ASSERT_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_OPT_FAIL
#undef BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_OPT_PASS
#undef BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW
#undef BSLS_ASSERTTEST_ASSERT_PASS
#undef BSLS_ASSERTTEST_ASSERT_PASS_RAW
#undef BSLS_ASSERTTEST_ASSERT_SAFE_FAIL
#undef BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_SAFE_PASS
#undef BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW

#undef BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_BRUTE_FORCE_IMP
#undef BSLS_ASSERTTEST_BRUTE_FORCE_IMP_RAW
#undef BSLS_ASSERTTEST_IS_ACTIVE
#undef BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG

    //  Define the macros for this test case
#define BDE_BUILD_TARGET_SAFE_2
#define BSLS_ASSERT_LEVEL_ASSERT

    //  Re-include the bsls_assert headers
#undef INCLUDED_BSLS_ASSERT
#undef INCLUDED_BSLS_ASSERTTEST
#include <bsls_assert.h>
#include <bsls_asserttest.h>

    //  Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
    // 'IS_ACTIVE' macros and the are values we intend to test.
#if !defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

    ASSERT(BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG);
    ASSERT(BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG);
    ASSERT(BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG);
    ASSERT(!BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG);

    {
        static const struct {
            int         d_lineNumber;       // line # of the row in this table
            const char *d_assertBuildType;  // "S", "S2", "A", "A2", "O", "O2"
            bool        d_expectedResult;   // assertion: 'P'ass or 'F'ail
        } DATA[] = {
            //LINE  TYPE  RESULT
            //----  ----  ------
            { L_,   "O",  true  },
            { L_,   "O2", true  },
            { L_,   "A",  true  },
            { L_,   "A2", true  },
            { L_,   "S",  false },
            { L_,   "S2", false },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_lineNumber;
            const char *const TYPE   = DATA[ti].d_assertBuildType;
            bool              RESULT = DATA[ti].d_expectedResult;

            if(globalVeryVerbose) {
                T_ P_(TYPE) P(RESULT)
            }

            // Validate test description.
            LOOP_ASSERT(LINE, bsls::AssertTest::isValidAssertBuild(TYPE));

            bool configResult = BSLS_ASSERTTEST_IS_ACTIVE(TYPE);
            LOOP3_ASSERT(TYPE, RESULT, configResult, RESULT == configResult);

        }  // table-driven 'for' loop
    }

//===================== SAFE_2 LEVEL_ASSERT_SAFE ============================//

    // Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_NONE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

#undef BSLS_ASSERTTEST_ASSERT_FAIL
#undef BSLS_ASSERTTEST_ASSERT_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_OPT_FAIL
#undef BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_OPT_PASS
#undef BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW
#undef BSLS_ASSERTTEST_ASSERT_PASS
#undef BSLS_ASSERTTEST_ASSERT_PASS_RAW
#undef BSLS_ASSERTTEST_ASSERT_SAFE_FAIL
#undef BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_SAFE_PASS
#undef BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW

#undef BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_BRUTE_FORCE_IMP
#undef BSLS_ASSERTTEST_BRUTE_FORCE_IMP_RAW
#undef BSLS_ASSERTTEST_IS_ACTIVE
#undef BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG

    //  Define the macros for this test case
#define BDE_BUILD_TARGET_SAFE_2
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

    //  Re-include the bsls_assert headers
#undef INCLUDED_BSLS_ASSERT
#undef INCLUDED_BSLS_ASSERTTEST
#include <bsls_assert.h>
#include <bsls_asserttest.h>

    //  Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
    // 'IS_ACTIVE' macros and the are values we intend to test.
#if !defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

    ASSERT(BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG);
    ASSERT(BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG);
    ASSERT(BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG);
    ASSERT(BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG);

    {
        static const struct {
            int         d_lineNumber;       // line # of the row in this table
            const char *d_assertBuildType;  // "S", "S2", "A", "A2", "O", "O2"
            bool        d_expectedResult;   // assertion: 'P'ass or 'F'ail
        } DATA[] = {
            //LINE  TYPE  RESULT
            //----  ----  ------
            { L_,   "O",  true  },
            { L_,   "O2", true  },
            { L_,   "A",  true  },
            { L_,   "A2", true  },
            { L_,   "S",  true  },
            { L_,   "S2", true  },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_lineNumber;
            const char *const TYPE   = DATA[ti].d_assertBuildType;
            bool              RESULT = DATA[ti].d_expectedResult;

            if(globalVeryVerbose) {
                T_ P_(TYPE) P(RESULT)
            }

            // Validate test description.
            LOOP_ASSERT(LINE, bsls::AssertTest::isValidAssertBuild(TYPE));

            bool configResult = BSLS_ASSERTTEST_IS_ACTIVE(TYPE);
            LOOP3_ASSERT(TYPE, RESULT, configResult, RESULT == configResult);

        }  // table-driven 'for' loop
    }

//===================== LEVEL_NONE ===============================//

    // Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_NONE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

#undef INCLUDED_BSLS_ASSERTTEST
#undef BSLS_ASSERTTEST_ASSERT_FAIL
#undef BSLS_ASSERTTEST_ASSERT_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_OPT_FAIL
#undef BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_OPT_PASS
#undef BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW
#undef BSLS_ASSERTTEST_ASSERT_PASS
#undef BSLS_ASSERTTEST_ASSERT_PASS_RAW
#undef BSLS_ASSERTTEST_ASSERT_SAFE_FAIL
#undef BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_SAFE_PASS
#undef BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW

#undef BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_BRUTE_FORCE_IMP
#undef BSLS_ASSERTTEST_BRUTE_FORCE_IMP_RAW
#undef BSLS_ASSERTTEST_IS_ACTIVE
#undef BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG

    //  Define the macros for this test case
#define BSLS_ASSERT_LEVEL_NONE

    //  Re-include the bsls_assert headers
#undef INCLUDED_BSLS_ASSERT
#undef INCLUDED_BSLS_ASSERTTEST
#include <bsls_assert.h>
#include <bsls_asserttest.h>

    //  Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
    // 'IS_ACTIVE' macros and the are values we intend to test.
#if defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

    ASSERT(!BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG);
    ASSERT(!BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG);
    ASSERT(!BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG);
    ASSERT(!BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG);

    {
        static const struct {
            int         d_lineNumber;       // line # of the row in this table
            const char *d_assertBuildType;  // "S", "S2", "A", "A2", "O", "O2"
            bool        d_expectedResult;   // assertion: 'P'ass or 'F'ail
        } DATA[] = {
            //LINE  TYPE  RESULT
            //----  ----  ------
            { L_,   "O",  false },
            { L_,   "O2", false },
            { L_,   "A",  false },
            { L_,   "A2", false },
            { L_,   "S",  false },
            { L_,   "S2", false },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_lineNumber;
            const char *const TYPE   = DATA[ti].d_assertBuildType;
            bool              RESULT = DATA[ti].d_expectedResult;

            if(globalVeryVerbose) {
                T_ P_(TYPE) P(RESULT)
            }

            // Validate test description.
            LOOP_ASSERT(LINE, bsls::AssertTest::isValidAssertBuild(TYPE));

            bool configResult = BSLS_ASSERTTEST_IS_ACTIVE(TYPE);
            LOOP3_ASSERT(TYPE, RESULT, configResult, RESULT == configResult);

        }  // table-driven 'for' loop
    }

//===================== LEVEL_ASSERT_OPT ===============================//

    // Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_NONE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

#undef BSLS_ASSERTTEST_ASSERT_FAIL
#undef BSLS_ASSERTTEST_ASSERT_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_OPT_FAIL
#undef BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_OPT_PASS
#undef BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW
#undef BSLS_ASSERTTEST_ASSERT_PASS
#undef BSLS_ASSERTTEST_ASSERT_PASS_RAW
#undef BSLS_ASSERTTEST_ASSERT_SAFE_FAIL
#undef BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_SAFE_PASS
#undef BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW

#undef BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_BRUTE_FORCE_IMP
#undef BSLS_ASSERTTEST_BRUTE_FORCE_IMP_RAW
#undef BSLS_ASSERTTEST_IS_ACTIVE
#undef BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG

    //  Define the macros for this test case
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

    //  Re-include the bsls_assert headers
#undef INCLUDED_BSLS_ASSERT
#undef INCLUDED_BSLS_ASSERTTEST
#include <bsls_assert.h>
#include <bsls_asserttest.h>

    //  Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
    // 'IS_ACTIVE' macros and the are values we intend to test.
#if defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should not be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

    ASSERT(!BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG);
    ASSERT(BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG);
    ASSERT(!BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG);
    ASSERT(!BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG);

    {
        static const struct {
            int         d_lineNumber;       // line # of the row in this table
            const char *d_assertBuildType;  // "S", "S2", "A", "A2", "O", "O2"
            bool        d_expectedResult;   // assertion: 'P'ass or 'F'ail
        } DATA[] = {
            //LINE  TYPE  RESULT
            //----  ----  ------
            { L_,   "O",  true  },
            { L_,   "O2", false },
            { L_,   "A",  false },
            { L_,   "A2", false },
            { L_,   "S",  false },
            { L_,   "S2", false },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_lineNumber;
            const char *const TYPE   = DATA[ti].d_assertBuildType;
            bool              RESULT = DATA[ti].d_expectedResult;

            if(globalVeryVerbose) {
                T_ P_(TYPE) P(RESULT)
            }

            // Validate test description.
            LOOP_ASSERT(LINE, bsls::AssertTest::isValidAssertBuild(TYPE));

            bool configResult = BSLS_ASSERTTEST_IS_ACTIVE(TYPE);
            LOOP3_ASSERT(TYPE, RESULT, configResult, RESULT == configResult);

        }  // table-driven 'for' loop
    }

//===================== LEVEL_ASSERT ===============================//

    // Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_NONE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

#undef BSLS_ASSERTTEST_ASSERT_FAIL
#undef BSLS_ASSERTTEST_ASSERT_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_OPT_FAIL
#undef BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_OPT_PASS
#undef BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW
#undef BSLS_ASSERTTEST_ASSERT_PASS
#undef BSLS_ASSERTTEST_ASSERT_PASS_RAW
#undef BSLS_ASSERTTEST_ASSERT_SAFE_FAIL
#undef BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_SAFE_PASS
#undef BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW

#undef BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_BRUTE_FORCE_IMP
#undef BSLS_ASSERTTEST_BRUTE_FORCE_IMP_RAW
#undef BSLS_ASSERTTEST_IS_ACTIVE
#undef BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG

    //  Define the macros for this test case
#define BSLS_ASSERT_LEVEL_ASSERT

    //  Re-include the bsls_assert headers
#undef INCLUDED_BSLS_ASSERT
#undef INCLUDED_BSLS_ASSERTTEST
#include <bsls_assert.h>
#include <bsls_asserttest.h>

    //  Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
    // 'IS_ACTIVE' macros and the are values we intend to test.
#if defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should not be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

    ASSERT(!BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG);
    ASSERT(BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG);
    ASSERT(BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG);
    ASSERT(!BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG);

    {
        static const struct {
            int         d_lineNumber;       // line # of the row in this table
            const char *d_assertBuildType;  // "S", "S2", "A", "A2", "O", "O2"
            bool        d_expectedResult;   // assertion: 'P'ass or 'F'ail
        } DATA[] = {
            //LINE  TYPE  RESULT
            //----  ----  ------
            { L_,   "O",  true  },
            { L_,   "O2", false },
            { L_,   "A",  true  },
            { L_,   "A2", false },
            { L_,   "S",  false },
            { L_,   "S2", false },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_lineNumber;
            const char *const TYPE   = DATA[ti].d_assertBuildType;
            bool              RESULT = DATA[ti].d_expectedResult;

            if(globalVeryVerbose) {
                T_ P_(TYPE) P(RESULT)
            }

            // Validate test description.
            LOOP_ASSERT(LINE, bsls::AssertTest::isValidAssertBuild(TYPE));

            bool configResult = BSLS_ASSERTTEST_IS_ACTIVE(TYPE);
            LOOP3_ASSERT(TYPE, RESULT, configResult, RESULT == configResult);

        }  // table-driven 'for' loop
    }

//===================== LEVEL_ASSERT_SAFE ===============================//

    // Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_NONE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

#undef BSLS_ASSERTTEST_ASSERT_FAIL
#undef BSLS_ASSERTTEST_ASSERT_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_OPT_FAIL
#undef BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_OPT_PASS
#undef BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW
#undef BSLS_ASSERTTEST_ASSERT_PASS
#undef BSLS_ASSERTTEST_ASSERT_PASS_RAW
#undef BSLS_ASSERTTEST_ASSERT_SAFE_FAIL
#undef BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_SAFE_PASS
#undef BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW

#undef BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_BRUTE_FORCE_IMP
#undef BSLS_ASSERTTEST_BRUTE_FORCE_IMP_RAW
#undef BSLS_ASSERTTEST_IS_ACTIVE
#undef BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG

    //  Define the macros for this test case
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

    //  Re-include the bsls_assert headers
#undef INCLUDED_BSLS_ASSERT
#undef INCLUDED_BSLS_ASSERTTEST
#include <bsls_assert.h>
#include <bsls_asserttest.h>

    //  Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
    // 'IS_ACTIVE' macros and the are values we intend to test.
#if defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should not be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

    ASSERT(!BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG);
    ASSERT(BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG);
    ASSERT(BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG);
    ASSERT(BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG);

    {
        static const struct {
            int         d_lineNumber;       // line # of the row in this table
            const char *d_assertBuildType;  // "S", "S2", "A", "A2", "O", "O2"
            bool        d_expectedResult;   // assertion: 'P'ass or 'F'ail
        } DATA[] = {
            //LINE  TYPE  RESULT
            //----  ----  ------
            { L_,   "O",  true  },
            { L_,   "O2", false },
            { L_,   "A",  true  },
            { L_,   "A2", false },
            { L_,   "S",  true  },
            { L_,   "S2", false },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_lineNumber;
            const char *const TYPE   = DATA[ti].d_assertBuildType;
            bool              RESULT = DATA[ti].d_expectedResult;

            if(globalVeryVerbose) {
                T_ P_(TYPE) P(RESULT)
            }

            // Validate test description.
            LOOP_ASSERT(LINE, bsls::AssertTest::isValidAssertBuild(TYPE));

            bool configResult = BSLS_ASSERTTEST_IS_ACTIVE(TYPE);
            LOOP3_ASSERT(TYPE, RESULT, configResult, RESULT == configResult);

        }  // table-driven 'for' loop
    }

//--------------------------------------------------------------------------//

}

// These macros provide an easy way of switching test filenames for the tests
// that rely on the value of '__FILE__'.  Note that the macro
// 'BSLS_ASSERTTEST_THIS_FILENAME' should have the same value as the string
// literal returned by '__FILE__' before any '#line' directives are processed.
#define BSLS_ASSERTTEST_SET_DIFFERENT_FILENAME \
    BSLS_MACROINCREMENT(__LINE__) "bsls_fictionaltestcomponent.t.cpp"
#define BSLS_ASSERTTEST_RESET_THIS_FILENAME \
    BSLS_MACROINCREMENT(__LINE__) "bsls_asserttest.t.cpp"

// We will be testing 6 macros, with a further test to handle the 6 RAW macros
//   BSLS_ASSERTTEST_ASSERT_SAFE_PASS(FUNCTION)
//   BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(FUNCTION)
//   BSLS_ASSERTTEST_ASSERT_PASS(FUNCTION)
//   BSLS_ASSERTTEST_ASSERT_FAIL(FUNCTION)
//   BSLS_ASSERTTEST_ASSERT_OPT_PASS(FUNCTION)
//   BSLS_ASSERTTEST_ASSERT_OPT_FAIL(FUNCTION)

// The Requirement to make each call is that a macro called 'ASSERT' is defined
// to flag errors, if necessary, that are signaled by the 'Probe's.
//
// Concerns:
//   The necessary 'ASSERT' macro correctly diagnoses pass/fail conditions
//   Each of the tested macros is available only in the supported build modes
//   Each tested macro correctly identifies pass/fail conditions
//
// Test plan:
//   Set up each possible build configuration in the tried-and-tested fashion.
//   Install a new 'ASSERT' macro that will count expected fails, rather than
//   the default one from this test driver, that flags all fails as unexpected.

void TestMacroBSLS_ASSERTTEST_PASS_OR_FAIL()
{
    if (globalVerbose) printf("\nTESTING BSLS_ASSERTTEST_IS_ACTIVE"
                              "\n=================================\n");

    if (globalVerbose)
        printf("\nWe need to write a running commentary\n");

    bsls::Assert::setFailureHandler(&bsls::AssertTest::failTestDriver);

//===================== SAFE_2 LEVEL_NONE ===============================//

// Install the local ASSERT macro, that allows us to detect assert-fail test
// cases.
#undef ASSERT
#define ASSERT(X) { aSsErT((!(X)) == EXPECTED, #X, __LINE__); }

    bool EXPECTED = false;
    ASSERT(false);
    EXPECTED = true;
    ASSERT(true);

// Note that we must leave each test block with EXPECTED = true
//
//===================== TEST TABLE : EXPECTED RESULTS =======================//

    //  Config macros    Configuration  Expected results
//  OVERRIDE SAFE2  OPT  DBG  SAFE   O A S O2 A2 S2
//  -------- -----  ---  ---  ----   - - - -- -- --
//  _NONE       X
//  _OPT        X    X               X      X
//  _DEBUG      X    X    X          X X    X  X
//  _SAFE       X    X    X    X     X X X  X  X  X
//  _NONE
//  _OPT             X               X
//  _DEBUG           X    X          X X
//  _SAFE            X    X    X     X X X

//===================== SAFE_2 LEVEL_NONE ===============================//

    // Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_NONE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

#undef BSLS_ASSERTTEST_ASSERT_FAIL
#undef BSLS_ASSERTTEST_ASSERT_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_OPT_FAIL
#undef BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_OPT_PASS
#undef BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW
#undef BSLS_ASSERTTEST_ASSERT_PASS
#undef BSLS_ASSERTTEST_ASSERT_PASS_RAW
#undef BSLS_ASSERTTEST_ASSERT_SAFE_FAIL
#undef BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_SAFE_PASS
#undef BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW

#undef BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_BRUTE_FORCE_IMP
#undef BSLS_ASSERTTEST_BRUTE_FORCE_IMP_RAW
#undef BSLS_ASSERTTEST_IS_ACTIVE
#undef BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG

    //  Define the macros for this test case
#define BDE_BUILD_TARGET_SAFE_2
#define BSLS_ASSERT_LEVEL_NONE

    //  Re-include the bsls_assert headers
#undef INCLUDED_BSLS_ASSERT
#undef INCLUDED_BSLS_ASSERTTEST
#include <bsls_assert.h>
#include <bsls_asserttest.h>

    //  Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
    // 'IS_ACTIVE' macros and the are values we intend to test.
#if !defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

    ASSERT(BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG);
    ASSERT(!BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG);
    ASSERT(!BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG);
    ASSERT(!BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG);

    {
        EXPECTED = true;

        // Verify that *PASS macros are expanded in any build mode.

        BSLS_ASSERTTEST_ASSERT_SAFE_PASS(ASSERT(true));
        BSLS_ASSERTTEST_ASSERT_PASS(ASSERT(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(ASSERT(true));
    }

    {
        EXPECTED = false;

        // Force a fail if any of these macros expands the expression

        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(ASSERT(true));
        BSLS_ASSERTTEST_ASSERT_FAIL(ASSERT(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(ASSERT(true));
    }

    // Restore status-quo before starting the next test
    EXPECTED = true;

//===================== SAFE_2 LEVEL_ASSERT_OPT =============================//

    // Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_NONE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

#undef BSLS_ASSERTTEST_ASSERT_FAIL
#undef BSLS_ASSERTTEST_ASSERT_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_OPT_FAIL
#undef BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_OPT_PASS
#undef BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW
#undef BSLS_ASSERTTEST_ASSERT_PASS
#undef BSLS_ASSERTTEST_ASSERT_PASS_RAW
#undef BSLS_ASSERTTEST_ASSERT_SAFE_FAIL
#undef BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_SAFE_PASS
#undef BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW

#undef BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_BRUTE_FORCE_IMP
#undef BSLS_ASSERTTEST_BRUTE_FORCE_IMP_RAW
#undef BSLS_ASSERTTEST_IS_ACTIVE
#undef BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG

    //  Define the macros for this test case
#define BDE_BUILD_TARGET_SAFE_2
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

    //  Re-include the bsls_assert headers
#undef INCLUDED_BSLS_ASSERT
#undef INCLUDED_BSLS_ASSERTTEST
#include <bsls_assert.h>
#include <bsls_asserttest.h>

    //  Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
    // 'IS_ACTIVE' macros and the are values we intend to test.
#if !defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

    ASSERT(BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG);
    ASSERT(BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG);
    ASSERT(!BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG);
    ASSERT(!BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG);

    {
        struct Production {
            static void callAssertOpt(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callAssertSafe(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }
        };

        struct Safe2 {
            static void callAssertOpt(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
#endif
            }

            static void callAssert(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
#endif
            }

            static void callAssertSafe(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
#endif
            }
        };

        {
            EXPECTED = true;

            // Verify that *PASS macros are expanded in any build mode.

            BSLS_ASSERTTEST_ASSERT_SAFE_PASS(ASSERT(true));
            BSLS_ASSERTTEST_ASSERT_PASS(ASSERT(true));
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(ASSERT(true));
        }

        EXPECTED = false;
        // Force a fail if any of these macros expands the expression
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(ASSERT(true));
        BSLS_ASSERTTEST_ASSERT_FAIL(ASSERT(true));

        // Now test for expressions that should fail, indicating a user error
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssertOpt(false));

        EXPECTED = true;
        // Finally test for expressions that should pass
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssertOpt(false));
    }

#line BSLS_ASSERTTEST_SET_DIFFERENT_FILENAME

    {
        struct Production {
            static void callAssertOpt(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
                BSLS_ASSERT_OPT(pass);
            }

            static void callAssert(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
                BSLS_ASSERT(pass);
            }

            static void callAssertSafe(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
                BSLS_ASSERT_SAFE(pass);
            }
        };

        struct Safe2 {
            static void callAssertOpt(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
                BSLS_ASSERT_OPT(pass);
#endif
            }

            static void callAssert(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
                BSLS_ASSERT(pass);
#endif
            }

            static void callAssertSafe(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
                BSLS_ASSERT_SAFE(pass);
#endif
            }
        };

        // Restore filename before evaluating the test macros
#line BSLS_ASSERTTEST_RESET_THIS_FILENAME

        {
            EXPECTED = true;

            // Verify that *PASS macros are expanded in any build mode.

            BSLS_ASSERTTEST_ASSERT_SAFE_PASS(ASSERT(true));
            BSLS_ASSERTTEST_ASSERT_PASS(ASSERT(true));
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(ASSERT(true));
        }

        EXPECTED = false;
        // Force a fail if any of these macros expands the expression
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(ASSERT(true));
        BSLS_ASSERTTEST_ASSERT_FAIL(ASSERT(true));

        // Now test for expressions that should fail, indicating a user error
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssertOpt(true));
#if defined(BDE_BUILD_TARGET_EXC)
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssertOpt(false));
#endif

        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssertOpt(true));
#if defined(BDE_BUILD_TARGET_EXC)
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssertOpt(false));
#endif

        // Call out additional cases that fail when the wrong component raises
        // the assertion
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssertOpt(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssertOpt(false));

        EXPECTED = true;
        // Finally test for expressions that should pass
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssertOpt(true));

        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssertOpt(true));
    }

    // Restore status-quo before starting the next test
    EXPECTED = true;

//===================== SAFE_2 LEVEL_ASSERT ===============================//

    // Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_NONE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

#undef BSLS_ASSERTTEST_ASSERT_FAIL
#undef BSLS_ASSERTTEST_ASSERT_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_OPT_FAIL
#undef BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_OPT_PASS
#undef BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW
#undef BSLS_ASSERTTEST_ASSERT_PASS
#undef BSLS_ASSERTTEST_ASSERT_PASS_RAW
#undef BSLS_ASSERTTEST_ASSERT_SAFE_FAIL
#undef BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_SAFE_PASS
#undef BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW

#undef BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_BRUTE_FORCE_IMP
#undef BSLS_ASSERTTEST_BRUTE_FORCE_IMP_RAW
#undef BSLS_ASSERTTEST_IS_ACTIVE
#undef BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG

    //  Define the macros for this test case
#define BDE_BUILD_TARGET_SAFE_2
#define BSLS_ASSERT_LEVEL_ASSERT

    //  Re-include the bsls_assert headers
#undef INCLUDED_BSLS_ASSERT
#undef INCLUDED_BSLS_ASSERTTEST
#include <bsls_assert.h>
#include <bsls_asserttest.h>

    //  Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
    // 'IS_ACTIVE' macros and the are values we intend to test.
#if !defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

    ASSERT(BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG);
    ASSERT(BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG);
    ASSERT(BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG);
    ASSERT(!BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG);

    {
        struct Production {
            static void callAssertOpt(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callAssertSafe(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }
        };

        struct Safe2 {
            static void callAssertOpt(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
#endif
            }

            static void callAssert(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
#endif
            }

            static void callAssertSafe(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
#endif
            }
        };

        {
            EXPECTED = true;

            // Verify that *PASS macros are expanded in any build mode.

            BSLS_ASSERTTEST_ASSERT_SAFE_PASS(ASSERT(true));
            BSLS_ASSERTTEST_ASSERT_PASS(ASSERT(true));
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(ASSERT(true));
        }

        EXPECTED = false;
        // Force a fail if any of these macros expands the expression
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(ASSERT(true));

        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssertOpt(false));

        EXPECTED = true;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssertOpt(false));
    }

#line BSLS_ASSERTTEST_SET_DIFFERENT_FILENAME

    {
        struct Production {
            static void callAssertOpt(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callAssertSafe(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }
        };

        struct Safe2 {
            static void callAssertOpt(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
#endif
            }

            static void callAssert(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
#endif
            }

            static void callAssertSafe(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
#endif
            }
        };

        // Restore filename before evaluating the test macros
#line BSLS_ASSERTTEST_RESET_THIS_FILENAME

        {
            EXPECTED = true;

            // Verify that *PASS macros are expanded in any build mode.

            BSLS_ASSERTTEST_ASSERT_SAFE_PASS(ASSERT(true));
            BSLS_ASSERTTEST_ASSERT_PASS(ASSERT(true));
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(ASSERT(true));
        }


        EXPECTED = false;
        // Force a fail if any of these macros expands the expression
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(ASSERT(true));

        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssertOpt(false));

        // Call out additional cases that fail when the wrong component raises
        // the assertion
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssertOpt(false));
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssertOpt(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssertOpt(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssertOpt(false));

        EXPECTED = true;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssertOpt(true));

        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssertOpt(true));

        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssertOpt(true));

        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssertOpt(true));
   }

    // Restore status-quo before starting the next test
    EXPECTED = true;

//===================== SAFE_2 LEVEL_ASSERT_SAFE ============================//

    // Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_NONE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

#undef BSLS_ASSERTTEST_ASSERT_FAIL
#undef BSLS_ASSERTTEST_ASSERT_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_OPT_FAIL
#undef BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_OPT_PASS
#undef BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW
#undef BSLS_ASSERTTEST_ASSERT_PASS
#undef BSLS_ASSERTTEST_ASSERT_PASS_RAW
#undef BSLS_ASSERTTEST_ASSERT_SAFE_FAIL
#undef BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_SAFE_PASS
#undef BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW

#undef BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_BRUTE_FORCE_IMP
#undef BSLS_ASSERTTEST_BRUTE_FORCE_IMP_RAW
#undef BSLS_ASSERTTEST_IS_ACTIVE
#undef BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG

    //  Define the macros for this test case
#define BDE_BUILD_TARGET_SAFE_2
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

    //  Re-include the bsls_assert headers
#undef INCLUDED_BSLS_ASSERT
#undef INCLUDED_BSLS_ASSERTTEST
#include <bsls_assert.h>
#include <bsls_asserttest.h>

    //  Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
    // 'IS_ACTIVE' macros and the are values we intend to test.
#if !defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

    ASSERT(BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG);
    ASSERT(BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG);
    ASSERT(BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG);
    ASSERT(BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG);

    {
        struct Production {
            static void callAssertOpt(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callAssertSafe(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }
        };

        struct Safe2 {
            static void callAssertOpt(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
#endif
            }

            static void callAssert(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
#endif
            }

            static void callAssertSafe(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
#endif
            }
        };

        {
            EXPECTED = true;

            // Verify that *PASS macros are expanded in any build mode.

            BSLS_ASSERTTEST_ASSERT_SAFE_PASS(ASSERT(true));
            BSLS_ASSERTTEST_ASSERT_PASS(ASSERT(true));
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(ASSERT(true));
        }

        EXPECTED = false;
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS(Safe2::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssertOpt(false));

        EXPECTED = true;
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_SAFE_PASS(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(Safe2::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssertOpt(false));
    }

#line BSLS_ASSERTTEST_SET_DIFFERENT_FILENAME

    {
        struct Production {
            static void callAssertOpt(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callAssertSafe(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }
        };

        struct Safe2 {
            static void callAssertOpt(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
#endif
            }

            static void callAssert(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
#endif
            }

            static void callAssertSafe(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
#endif
            }
        };

        // Restore filename before evaluating the test macros
#line BSLS_ASSERTTEST_RESET_THIS_FILENAME

        {
            EXPECTED = true;

            // Verify that *PASS macros are expanded in any build mode.

            BSLS_ASSERTTEST_ASSERT_SAFE_PASS(ASSERT(true));
            BSLS_ASSERTTEST_ASSERT_PASS(ASSERT(true));
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(ASSERT(true));
        }

        EXPECTED = false;
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS(Safe2::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssertOpt(false));

        // Call out additional cases that fail when the wrong component raises
        // the assertion
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(Production::callAssertOpt(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(Safe2::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssertOpt(false));
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssertOpt(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssertOpt(false));

        EXPECTED = true;
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS(Production::callAssertOpt(true));

        BSLS_ASSERTTEST_ASSERT_SAFE_PASS(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS(Safe2::callAssertOpt(true));

        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssertOpt(true));

        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssertOpt(true));

        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssertOpt(true));

        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssertOpt(true));
    }

    // Restore status-quo before starting the next test
    EXPECTED = true;

//===================== LEVEL_NONE ===============================//

    // Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_NONE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

#undef BSLS_ASSERTTEST_ASSERT_FAIL
#undef BSLS_ASSERTTEST_ASSERT_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_OPT_FAIL
#undef BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_OPT_PASS
#undef BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW
#undef BSLS_ASSERTTEST_ASSERT_PASS
#undef BSLS_ASSERTTEST_ASSERT_PASS_RAW
#undef BSLS_ASSERTTEST_ASSERT_SAFE_FAIL
#undef BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_SAFE_PASS
#undef BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW

#undef BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_BRUTE_FORCE_IMP
#undef BSLS_ASSERTTEST_BRUTE_FORCE_IMP_RAW
#undef BSLS_ASSERTTEST_IS_ACTIVE
#undef BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG

    //  Define the macros for this test case
#define BSLS_ASSERT_LEVEL_NONE

    //  Re-include the bsls_assert headers
#undef INCLUDED_BSLS_ASSERT
#undef INCLUDED_BSLS_ASSERTTEST
#include <bsls_assert.h>
#include <bsls_asserttest.h>

    //  Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
    // 'IS_ACTIVE' macros and the are values we intend to test.
#if defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

    ASSERT(!BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG);
    ASSERT(!BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG);
    ASSERT(!BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG);
    ASSERT(!BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG);

    {
        EXPECTED = true;

        // Verify that *PASS macros are expanded in any build mode.

        BSLS_ASSERTTEST_ASSERT_SAFE_PASS(ASSERT(true));
        BSLS_ASSERTTEST_ASSERT_PASS(ASSERT(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(ASSERT(true));
    }

    {
        EXPECTED = false;
        // Force a fail if any of these macros expands the expression
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(ASSERT(true));
        BSLS_ASSERTTEST_ASSERT_FAIL(ASSERT(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(ASSERT(true));
    }

    // Restore status-quo before starting the next test
    EXPECTED = true;

//===================== LEVEL_ASSERT_OPT ===============================//

    // Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_NONE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

#undef BSLS_ASSERTTEST_ASSERT_FAIL
#undef BSLS_ASSERTTEST_ASSERT_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_OPT_FAIL
#undef BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_OPT_PASS
#undef BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW
#undef BSLS_ASSERTTEST_ASSERT_PASS
#undef BSLS_ASSERTTEST_ASSERT_PASS_RAW
#undef BSLS_ASSERTTEST_ASSERT_SAFE_FAIL
#undef BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_SAFE_PASS
#undef BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW

#undef BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_BRUTE_FORCE_IMP
#undef BSLS_ASSERTTEST_BRUTE_FORCE_IMP_RAW
#undef BSLS_ASSERTTEST_IS_ACTIVE
#undef BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG

    //  Define the macros for this test case
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

    //  Re-include the bsls_assert headers
#undef INCLUDED_BSLS_ASSERT
#undef INCLUDED_BSLS_ASSERTTEST
#include <bsls_assert.h>
#include <bsls_asserttest.h>

    //  Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
    // 'IS_ACTIVE' macros and the are values we intend to test.
#if defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should not be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

    ASSERT(!BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG);
    ASSERT(BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG);
    ASSERT(!BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG);
    ASSERT(!BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG);

    {
        struct Production {
            static void callAssertOpt(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callAssertSafe(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }
        };

        struct Safe2 {
            static void callAssertOpt(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
#endif
            }

            static void callAssert(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
#endif
            }

            static void callAssertSafe(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
#endif
            }
        };

        {
            EXPECTED = true;

            // Verify that *PASS macros are expanded in any build mode.

            BSLS_ASSERTTEST_ASSERT_SAFE_PASS(ASSERT(true));
            BSLS_ASSERTTEST_ASSERT_PASS(ASSERT(true));
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(ASSERT(true));
        }

        EXPECTED = false;
        // Force a fail if any of these macros expands the expression
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(ASSERT(true));
        BSLS_ASSERTTEST_ASSERT_FAIL(ASSERT(true));

        // Now test for expressions that should fail, indicating a user error
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssertOpt(false));

        EXPECTED = true;
        // Finally test for expressions that should pass
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssertOpt(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(false));
    }

#line BSLS_ASSERTTEST_SET_DIFFERENT_FILENAME

    {
        struct Production {
            static void callAssertOpt(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callAssertSafe(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }
        };

        struct Safe2 {
            static void callAssertOpt(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
#endif
            }

            static void callAssert(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
#endif
            }

            static void callAssertSafe(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
#endif
            }
        };

        // Restore filename before evaluating the test macros
#line BSLS_ASSERTTEST_RESET_THIS_FILENAME

        {
            EXPECTED = true;

            // Verify that *PASS macros are expanded in any build mode.

            BSLS_ASSERTTEST_ASSERT_SAFE_PASS(ASSERT(true));
            BSLS_ASSERTTEST_ASSERT_PASS(ASSERT(true));
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(ASSERT(true));
        }

        EXPECTED = false;
        // Force a fail if any of these macros expands the expression
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(ASSERT(true));
        BSLS_ASSERTTEST_ASSERT_FAIL(ASSERT(true));

        // Now test for expressions that should fail, indicating a user error
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssertOpt(false));

        // Call out additional cases that fail when the wrong component raises
        // the assertion
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssertOpt(false));

        EXPECTED = true;
        // Finally test for expressions that should pass
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssertOpt(true));

        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssertOpt(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(false));
    }

    // Restore status-quo before starting the next test
    EXPECTED = true;

//===================== LEVEL_ASSERT ===============================//

    // Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_NONE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

#undef BSLS_ASSERTTEST_ASSERT_FAIL
#undef BSLS_ASSERTTEST_ASSERT_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_OPT_FAIL
#undef BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_OPT_PASS
#undef BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW
#undef BSLS_ASSERTTEST_ASSERT_PASS
#undef BSLS_ASSERTTEST_ASSERT_PASS_RAW
#undef BSLS_ASSERTTEST_ASSERT_SAFE_FAIL
#undef BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_SAFE_PASS
#undef BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW

#undef BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_BRUTE_FORCE_IMP
#undef BSLS_ASSERTTEST_BRUTE_FORCE_IMP_RAW
#undef BSLS_ASSERTTEST_IS_ACTIVE
#undef BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG

    //  Define the macros for this test case
#define BSLS_ASSERT_LEVEL_ASSERT

    //  Re-include the bsls_assert headers
#undef INCLUDED_BSLS_ASSERT
#undef INCLUDED_BSLS_ASSERTTEST
#include <bsls_assert.h>
#include <bsls_asserttest.h>

    //  Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
    // 'IS_ACTIVE' macros and the are values we intend to test.
#if defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should not be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

    ASSERT(!BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG);
    ASSERT(BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG);
    ASSERT(BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG);
    ASSERT(!BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG);

    {
        struct Production {
            static void callAssertOpt(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callAssertSafe(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }
        };

        struct Safe2 {
            static void callAssertOpt(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
#endif
            }

            static void callAssert(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
#endif
            }

            static void callAssertSafe(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
#endif
            }
        };

        {
            EXPECTED = true;

            // Verify that *PASS macros are expanded in any build mode.

            BSLS_ASSERTTEST_ASSERT_SAFE_PASS(ASSERT(true));
            BSLS_ASSERTTEST_ASSERT_PASS(ASSERT(true));
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(ASSERT(true));
        }

        EXPECTED = false;
        // Force a fail if any of these macros expands the expression
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(ASSERT(true));

        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssertOpt(false));

        EXPECTED = true;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssertOpt(false));
    }

#line BSLS_ASSERTTEST_SET_DIFFERENT_FILENAME

    {
        struct Production {
            static void callAssertOpt(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callAssertSafe(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }
        };

        struct Safe2 {
            static void callAssertOpt(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
#endif
            }

            static void callAssert(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
#endif
            }

            static void callAssertSafe(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
#endif
            }
        };

        // Restore filename before evaluating the test macros
#line BSLS_ASSERTTEST_RESET_THIS_FILENAME

        {
            EXPECTED = true;

            // Verify that *PASS macros are expanded in any build mode.

            BSLS_ASSERTTEST_ASSERT_SAFE_PASS(ASSERT(true));
            BSLS_ASSERTTEST_ASSERT_PASS(ASSERT(true));
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(ASSERT(true));
        }

        EXPECTED = false;
        // Force a fail if any of these macros expands the expression
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(ASSERT(true));

        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssertOpt(false));

        // Call out additional cases that fail when the wrong component raises
        // the assertion
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssertOpt(false));

        EXPECTED = true;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssertOpt(true));

        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssertOpt(true));

        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssertOpt(false));
    }

    // Restore status-quo before starting the next test
    EXPECTED = true;

//===================== LEVEL_ASSERT_SAFE ===============================//

    // Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_NONE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

#undef BSLS_ASSERTTEST_ASSERT_FAIL
#undef BSLS_ASSERTTEST_ASSERT_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_OPT_FAIL
#undef BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_OPT_PASS
#undef BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW
#undef BSLS_ASSERTTEST_ASSERT_PASS
#undef BSLS_ASSERTTEST_ASSERT_PASS_RAW
#undef BSLS_ASSERTTEST_ASSERT_SAFE_FAIL
#undef BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_SAFE_PASS
#undef BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW

#undef BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_BRUTE_FORCE_IMP
#undef BSLS_ASSERTTEST_BRUTE_FORCE_IMP_RAW
#undef BSLS_ASSERTTEST_IS_ACTIVE
#undef BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG

    //  Define the macros for this test case
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

    //  Re-include the bsls_assert headers
#undef INCLUDED_BSLS_ASSERT
#undef INCLUDED_BSLS_ASSERTTEST
#include <bsls_assert.h>
#include <bsls_asserttest.h>

    //  Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
    // 'IS_ACTIVE' macros and the are values we intend to test.
#if defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should not be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

    ASSERT(!BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG);
    ASSERT(BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG);
    ASSERT(BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG);
    ASSERT(BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG);

    {
        struct Production {
            static void callAssertOpt(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callAssertSafe(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }
        };

        struct Safe2 {
            static void callAssertOpt(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
#endif
            }

            static void callAssert(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
#endif
            }

            static void callAssertSafe(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
#endif
            }
        };

        {
            EXPECTED = true;

            // Verify that *PASS macros are expanded in any build mode.

            BSLS_ASSERTTEST_ASSERT_SAFE_PASS(ASSERT(true));
            BSLS_ASSERTTEST_ASSERT_PASS(ASSERT(true));
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(ASSERT(true));
        }

        EXPECTED = false;
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(Safe2::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssertOpt(false));

        EXPECTED = true;
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_SAFE_PASS(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS(Safe2::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssertOpt(false));
    }

#line BSLS_ASSERTTEST_SET_DIFFERENT_FILENAME

    {
        struct Production {
            static void callAssertOpt(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callAssertSafe(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }
        };

        struct Safe2 {
            static void callAssertOpt(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
#endif
            }

            static void callAssert(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
#endif
            }

            static void callAssertSafe(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
#endif
            }
        };

        // Restore filename before evaluating the test macros
#line BSLS_ASSERTTEST_RESET_THIS_FILENAME

        {
            EXPECTED = true;

            // Verify that *PASS macros are expanded in any build mode.

            BSLS_ASSERTTEST_ASSERT_SAFE_PASS(ASSERT(true));
            BSLS_ASSERTTEST_ASSERT_PASS(ASSERT(true));
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(ASSERT(true));
        }

        EXPECTED = false;
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(Safe2::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssertOpt(false));

        // Call out additional cases that fail when the wrong component raises
        // the assertion
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssertOpt(false));

        EXPECTED = true;
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS(Production::callAssertOpt(true));

        BSLS_ASSERTTEST_ASSERT_SAFE_PASS(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS(Safe2::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssertOpt(true));

        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssertOpt(true));

        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssertOpt(false));
    }

    // Restore status-quo before starting the next test
    EXPECTED = true;

//--------------------------------------------------------------------------//
// Restore the default 'ASSERT' macro for this test driver.
#undef ASSERT
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

}

// We will be testing 6 RAW macros
//   BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(FUNCTION)
//   BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(FUNCTION)
//   BSLS_ASSERTTEST_ASSERT_PASS_RAW(FUNCTION)
//   BSLS_ASSERTTEST_ASSERT_FAIL_RAW(FUNCTION)
//   BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(FUNCTION)
//   BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(FUNCTION)

void TestMacroBSLS_ASSERTTEST_PASS_OR_FAIL_RAW()
{
    if (globalVerbose) printf("\nTESTING BSLS_ASSERTTEST_IS_ACTIVE"
                              "\n=================================\n");

    if (globalVerbose)
        printf("\nWe need to write a running commentary\n");

    bsls::Assert::setFailureHandler(&bsls::AssertTest::failTestDriver);

//===================== SAFE_2 LEVEL_NONE ===============================//

// Install the local ASSERT macro, that allows us to detect assert-fail test
// cases.
#undef ASSERT
#define ASSERT(X) { aSsErT((!(X)) == EXPECTED, #X, __LINE__); }

    bool EXPECTED = false;
    ASSERT(false);
    EXPECTED = true;
    ASSERT(true);

// Note that we must leave each test block with EXPECTED = true
//
//===================== TEST TABLE : EXPECTED RESULTS =======================//

    //  Config macros    Configuration  Expected results
//  OVERRIDE SAFE2  OPT  DBG  SAFE   O A S O2 A2 S2
//  -------- -----  ---  ---  ----   - - - -- -- --
//  _NONE       X
//  _OPT        X    X               X      X
//  _DEBUG      X    X    X          X X    X  X
//  _SAFE       X    X    X    X     X X X  X  X  X
//  _NONE
//  _OPT             X               X
//  _DEBUG           X    X          X X
//  _SAFE            X    X    X     X X X

//===================== SAFE_2 LEVEL_NONE ===============================//

    // Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_NONE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

#undef BSLS_ASSERTTEST_ASSERT_FAIL
#undef BSLS_ASSERTTEST_ASSERT_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_OPT_FAIL
#undef BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_OPT_PASS
#undef BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW
#undef BSLS_ASSERTTEST_ASSERT_PASS
#undef BSLS_ASSERTTEST_ASSERT_PASS_RAW
#undef BSLS_ASSERTTEST_ASSERT_SAFE_FAIL
#undef BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_SAFE_PASS
#undef BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW

#undef BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_BRUTE_FORCE_IMP
#undef BSLS_ASSERTTEST_BRUTE_FORCE_IMP_RAW
#undef BSLS_ASSERTTEST_IS_ACTIVE
#undef BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG

    //  Define the macros for this test case
#define BDE_BUILD_TARGET_SAFE_2
#define BSLS_ASSERT_LEVEL_NONE

    //  Re-include the bsls_assert headers
#undef INCLUDED_BSLS_ASSERT
#undef INCLUDED_BSLS_ASSERTTEST
#include <bsls_assert.h>
#include <bsls_asserttest.h>

    //  Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
    // 'IS_ACTIVE' macros and the are values we intend to test.
#if !defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

    ASSERT(BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG);
    ASSERT(!BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG);
    ASSERT(!BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG);
    ASSERT(!BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG);

    {
        EXPECTED = true;

        // Verify that *PASS macros are expanded in any build mode.

        BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(ASSERT(true));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(ASSERT(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(ASSERT(true));
    }

    {
        EXPECTED = false;
        // Force a fail if any of these macros expands the expression
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(ASSERT(true));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(ASSERT(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(ASSERT(true));
    }

    // Restore status-quo before starting the next test
    EXPECTED = true;

//===================== SAFE_2 LEVEL_ASSERT_OPT =============================//

    // Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_NONE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

#undef BSLS_ASSERTTEST_ASSERT_FAIL
#undef BSLS_ASSERTTEST_ASSERT_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_OPT_FAIL
#undef BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_OPT_PASS
#undef BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW
#undef BSLS_ASSERTTEST_ASSERT_PASS
#undef BSLS_ASSERTTEST_ASSERT_PASS_RAW
#undef BSLS_ASSERTTEST_ASSERT_SAFE_FAIL
#undef BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_SAFE_PASS
#undef BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW

#undef BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_BRUTE_FORCE_IMP
#undef BSLS_ASSERTTEST_BRUTE_FORCE_IMP_RAW
#undef BSLS_ASSERTTEST_IS_ACTIVE
#undef BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG

    //  Define the macros for this test case
#define BDE_BUILD_TARGET_SAFE_2
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

    //  Re-include the bsls_assert headers
#undef INCLUDED_BSLS_ASSERT
#undef INCLUDED_BSLS_ASSERTTEST
#include <bsls_assert.h>
#include <bsls_asserttest.h>

    //  Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
    // 'IS_ACTIVE' macros and the are values we intend to test.
#if !defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

    ASSERT(BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG);
    ASSERT(BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG);
    ASSERT(!BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG);
    ASSERT(!BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG);

    {
        struct Production {
            static void callAssertOpt(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callAssertSafe(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }
        };

        struct Safe2 {
            static void callAssertOpt(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
#endif
            }

            static void callAssert(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
#endif
            }

            static void callAssertSafe(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
#endif
            }
        };

        {
            EXPECTED = true;

            // Verify that *PASS macros are expanded in any build mode.

            BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(ASSERT(true));
            BSLS_ASSERTTEST_ASSERT_PASS_RAW(ASSERT(true));
            BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(ASSERT(true));
        }

        EXPECTED = false;
        // Force a fail if any of these macros expands the expression
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(ASSERT(true));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(ASSERT(true));

        // Now test for expressions that should fail, indicating a user error
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssertOpt(false));

        EXPECTED = true;
        // Finally test for expressions that should pass
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssertOpt(false));
    }

#line BSLS_ASSERTTEST_SET_DIFFERENT_FILENAME

    {
        struct Production {
            static void callAssertOpt(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callAssertSafe(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }
        };

        struct Safe2 {
            static void callAssertOpt(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
#endif
            }

            static void callAssert(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
#endif
            }

            static void callAssertSafe(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
#endif
            }
        };

        // Restore filename before evaluating the test macros
#line BSLS_ASSERTTEST_RESET_THIS_FILENAME

        {
            EXPECTED = true;

            // Verify that *PASS macros are expanded in any build mode.

            BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(ASSERT(true));
            BSLS_ASSERTTEST_ASSERT_PASS_RAW(ASSERT(true));
            BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(ASSERT(true));
        }

        EXPECTED = false;
        // Force a fail if any of these macros expands the expression
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(ASSERT(true));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(ASSERT(true));

        // Now test for expressions that should fail, indicating a user error
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssertOpt(false));

        EXPECTED = true;
        // Finally test for expressions that should pass
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssertOpt(false));
    }

    // Restore status-quo before starting the next test
    EXPECTED = true;

//===================== SAFE_2 LEVEL_ASSERT ===============================//

    // Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_NONE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

#undef BSLS_ASSERTTEST_ASSERT_FAIL
#undef BSLS_ASSERTTEST_ASSERT_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_OPT_FAIL
#undef BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_OPT_PASS
#undef BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW
#undef BSLS_ASSERTTEST_ASSERT_PASS
#undef BSLS_ASSERTTEST_ASSERT_PASS_RAW
#undef BSLS_ASSERTTEST_ASSERT_SAFE_FAIL
#undef BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_SAFE_PASS
#undef BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW

#undef BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_BRUTE_FORCE_IMP
#undef BSLS_ASSERTTEST_BRUTE_FORCE_IMP_RAW
#undef BSLS_ASSERTTEST_IS_ACTIVE
#undef BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG

    //  Define the macros for this test case
#define BDE_BUILD_TARGET_SAFE_2
#define BSLS_ASSERT_LEVEL_ASSERT

    //  Re-include the bsls_assert headers
#undef INCLUDED_BSLS_ASSERT
#undef INCLUDED_BSLS_ASSERTTEST
#include <bsls_assert.h>
#include <bsls_asserttest.h>

    //  Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
    // 'IS_ACTIVE' macros and the are values we intend to test.
#if !defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

    ASSERT(BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG);
    ASSERT(BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG);
    ASSERT(BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG);
    ASSERT(!BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG);

    {
        struct Production {
            static void callAssertOpt(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callAssertSafe(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }
        };

        struct Safe2 {
            static void callAssertOpt(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
#endif
            }

            static void callAssert(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
#endif
            }

            static void callAssertSafe(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
#endif
            }
        };

        {
            EXPECTED = true;

            // Verify that *PASS macros are expanded in any build mode.

            BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(ASSERT(true));
            BSLS_ASSERTTEST_ASSERT_PASS_RAW(ASSERT(true));
            BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(ASSERT(true));
        }

        EXPECTED = false;
        // Force a fail if any of these macros expands the expression
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(ASSERT(true));

        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssertOpt(false));

        EXPECTED = true;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssertOpt(false));
    }

#line BSLS_ASSERTTEST_SET_DIFFERENT_FILENAME

    {
        struct Production {
            static void callAssertOpt(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callAssertSafe(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }
        };

        struct Safe2 {
            static void callAssertOpt(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
#endif
            }

            static void callAssert(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
#endif
            }

            static void callAssertSafe(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
#endif
            }
        };

        // Restore filename before evaluating the test macros
#line BSLS_ASSERTTEST_RESET_THIS_FILENAME

        {
            EXPECTED = true;

            // Verify that *PASS macros are expanded in any build mode.

            BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(ASSERT(true));
            BSLS_ASSERTTEST_ASSERT_PASS_RAW(ASSERT(true));
            BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(ASSERT(true));
        }

        EXPECTED = false;
        // Force a fail if any of these macros expands the expression
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(ASSERT(true));

        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssertOpt(false));

        EXPECTED = true;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssertOpt(false));
   }

    // Restore status-quo before starting the next test
    EXPECTED = true;

//===================== SAFE_2 LEVEL_ASSERT_SAFE ============================//

    // Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_NONE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

#undef BSLS_ASSERTTEST_ASSERT_FAIL
#undef BSLS_ASSERTTEST_ASSERT_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_OPT_FAIL
#undef BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_OPT_PASS
#undef BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW
#undef BSLS_ASSERTTEST_ASSERT_PASS
#undef BSLS_ASSERTTEST_ASSERT_PASS_RAW
#undef BSLS_ASSERTTEST_ASSERT_SAFE_FAIL
#undef BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_SAFE_PASS
#undef BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW

#undef BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_BRUTE_FORCE_IMP
#undef BSLS_ASSERTTEST_BRUTE_FORCE_IMP_RAW
#undef BSLS_ASSERTTEST_IS_ACTIVE
#undef BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG

    //  Define the macros for this test case
#define BDE_BUILD_TARGET_SAFE_2
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

    //  Re-include the bsls_assert headers
#undef INCLUDED_BSLS_ASSERT
#undef INCLUDED_BSLS_ASSERTTEST
#include <bsls_assert.h>
#include <bsls_asserttest.h>

    //  Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
    // 'IS_ACTIVE' macros and the are values we intend to test.
#if !defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

    ASSERT(BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG);
    ASSERT(BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG);
    ASSERT(BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG);
    ASSERT(BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG);

    {
        struct Production {
            static void callAssertOpt(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callAssertSafe(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }
        };

        struct Safe2 {
            static void callAssertOpt(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
#endif
            }

            static void callAssert(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
#endif
            }

            static void callAssertSafe(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
#endif
            }
        };

        {
            EXPECTED = true;

            // Verify that *PASS macros are expanded in any build mode.

            BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(ASSERT(true));
            BSLS_ASSERTTEST_ASSERT_PASS_RAW(ASSERT(true));
            BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(ASSERT(true));
        }

        EXPECTED = false;
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(Production::callAssertSafe(
                                                                       false));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(Safe2::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssertOpt(false));

        EXPECTED = true;
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(Production::callAssertSafe(
                                                                       false));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(Safe2::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssertOpt(false));
    }

#line BSLS_ASSERTTEST_SET_DIFFERENT_FILENAME

    {
        struct Production {
            static void callAssertOpt(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callAssertSafe(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }
        };

        struct Safe2 {
            static void callAssertOpt(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
#endif
            }

            static void callAssert(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
#endif
            }

            static void callAssertSafe(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
#endif
            }
        };

        // Restore filename before evaluating the test macros
#line BSLS_ASSERTTEST_RESET_THIS_FILENAME

        {
            EXPECTED = true;

            // Verify that *PASS macros are expanded in any build mode.

            BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(ASSERT(true));
            BSLS_ASSERTTEST_ASSERT_PASS_RAW(ASSERT(true));
            BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(ASSERT(true));
        }

        EXPECTED = false;
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(Production::callAssertSafe(
                                                                       false));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(Safe2::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssertOpt(false));

        EXPECTED = true;
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(Production::callAssertSafe(
                                                                       false));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(Safe2::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssertOpt(false));
    }

    // Restore status-quo before starting the next test
    EXPECTED = true;

//===================== LEVEL_NONE ===============================//

    // Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_NONE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

#undef BSLS_ASSERTTEST_ASSERT_FAIL
#undef BSLS_ASSERTTEST_ASSERT_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_OPT_FAIL
#undef BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_OPT_PASS
#undef BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW
#undef BSLS_ASSERTTEST_ASSERT_PASS
#undef BSLS_ASSERTTEST_ASSERT_PASS_RAW
#undef BSLS_ASSERTTEST_ASSERT_SAFE_FAIL
#undef BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_SAFE_PASS
#undef BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW

#undef BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_BRUTE_FORCE_IMP
#undef BSLS_ASSERTTEST_BRUTE_FORCE_IMP_RAW
#undef BSLS_ASSERTTEST_IS_ACTIVE
#undef BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG

    //  Define the macros for this test case
#define BSLS_ASSERT_LEVEL_NONE

    //  Re-include the bsls_assert headers
#undef INCLUDED_BSLS_ASSERT
#undef INCLUDED_BSLS_ASSERTTEST
#include <bsls_assert.h>
#include <bsls_asserttest.h>

    //  Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
    // 'IS_ACTIVE' macros and the are values we intend to test.
#if defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

    ASSERT(!BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG);
    ASSERT(!BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG);
    ASSERT(!BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG);
    ASSERT(!BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG);

    {
        EXPECTED = true;

        // Verify that *PASS macros are expanded in any build mode.

        BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(ASSERT(true));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(ASSERT(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(ASSERT(true));
    }

    {
        EXPECTED = false;
        // Force a fail if any of these macros expands the expression
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(ASSERT(true));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(ASSERT(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(ASSERT(true));
    }

    // Restore status-quo before starting the next test
    EXPECTED = true;

//===================== LEVEL_ASSERT_OPT ===============================//

    // Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_NONE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

#undef BSLS_ASSERTTEST_ASSERT_FAIL
#undef BSLS_ASSERTTEST_ASSERT_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_OPT_FAIL
#undef BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_OPT_PASS
#undef BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW
#undef BSLS_ASSERTTEST_ASSERT_PASS
#undef BSLS_ASSERTTEST_ASSERT_PASS_RAW
#undef BSLS_ASSERTTEST_ASSERT_SAFE_FAIL
#undef BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_SAFE_PASS
#undef BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW

#undef BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_BRUTE_FORCE_IMP
#undef BSLS_ASSERTTEST_BRUTE_FORCE_IMP_RAW
#undef BSLS_ASSERTTEST_IS_ACTIVE
#undef BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG

    //  Define the macros for this test case
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

    //  Re-include the bsls_assert headers
#undef INCLUDED_BSLS_ASSERT
#undef INCLUDED_BSLS_ASSERTTEST
#include <bsls_assert.h>
#include <bsls_asserttest.h>

    //  Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
    // 'IS_ACTIVE' macros and the are values we intend to test.
#if defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should not be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

    ASSERT(!BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG);
    ASSERT(BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG);
    ASSERT(!BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG);
    ASSERT(!BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG);

    {
        struct Production {
            static void callAssertOpt(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callAssertSafe(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }
        };

        struct Safe2 {
            static void callAssertOpt(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
#endif
            }

            static void callAssert(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
#endif
            }

            static void callAssertSafe(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
#endif
            }
        };

        {
            EXPECTED = true;

            // Verify that *PASS macros are expanded in any build mode.

            BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(ASSERT(true));
            BSLS_ASSERTTEST_ASSERT_PASS_RAW(ASSERT(true));
            BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(ASSERT(true));
        }

        EXPECTED = false;
        // Force a fail if any of these macros expands the expression
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(ASSERT(true));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(ASSERT(true));

        // Now test for expressions that should fail, indicating a user error
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssertOpt(false));

        EXPECTED = true;
        // Finally test for expressions that should pass
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssertOpt(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssert(false));
    }

#line BSLS_ASSERTTEST_SET_DIFFERENT_FILENAME

    {
        struct Production {
            static void callAssertOpt(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callAssertSafe(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }
        };

        struct Safe2 {
            static void callAssertOpt(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
#endif
            }

            static void callAssert(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
#endif
            }

            static void callAssertSafe(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
#endif
            }
        };

        // Restore filename before evaluating the test macros
#line BSLS_ASSERTTEST_RESET_THIS_FILENAME

        {
            EXPECTED = true;

            // Verify that *PASS macros are expanded in any build mode.

            BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(ASSERT(true));
            BSLS_ASSERTTEST_ASSERT_PASS_RAW(ASSERT(true));
            BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(ASSERT(true));
        }

        EXPECTED = false;
        // Force a fail if any of these macros expands the expression
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(ASSERT(true));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(ASSERT(true));

        // Now test for expressions that should fail, indicating a user error
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssertOpt(false));

        EXPECTED = true;
        // Finally test for expressions that should pass
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssertOpt(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssert(false));
    }

    // Restore status-quo before starting the next test
    EXPECTED = true;

//===================== LEVEL_ASSERT ===============================//

    // Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_NONE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

#undef BSLS_ASSERTTEST_ASSERT_FAIL
#undef BSLS_ASSERTTEST_ASSERT_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_OPT_FAIL
#undef BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_OPT_PASS
#undef BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW
#undef BSLS_ASSERTTEST_ASSERT_PASS
#undef BSLS_ASSERTTEST_ASSERT_PASS_RAW
#undef BSLS_ASSERTTEST_ASSERT_SAFE_FAIL
#undef BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_SAFE_PASS
#undef BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW

#undef BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_BRUTE_FORCE_IMP
#undef BSLS_ASSERTTEST_BRUTE_FORCE_IMP_RAW
#undef BSLS_ASSERTTEST_IS_ACTIVE
#undef BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG

    //  Define the macros for this test case
#define BSLS_ASSERT_LEVEL_ASSERT

    //  Re-include the bsls_assert headers
#undef INCLUDED_BSLS_ASSERT
#undef INCLUDED_BSLS_ASSERTTEST
#include <bsls_assert.h>
#include <bsls_asserttest.h>

    //  Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
    // 'IS_ACTIVE' macros and the are values we intend to test.
#if defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should not be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

    ASSERT(!BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG);
    ASSERT(BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG);
    ASSERT(BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG);
    ASSERT(!BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG);

    {
        struct Production {
            static void callAssertOpt(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callAssertSafe(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }
        };

        struct Safe2 {
            static void callAssertOpt(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
#endif
            }

            static void callAssert(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
#endif
            }

            static void callAssertSafe(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
#endif
            }
        };

        {
            EXPECTED = true;

            // Verify that *PASS macros are expanded in any build mode.

            BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(ASSERT(true));
            BSLS_ASSERTTEST_ASSERT_PASS_RAW(ASSERT(true));
            BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(ASSERT(true));
        }

        EXPECTED = false;
        // Force a fail if any of these macros expands the expression
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(ASSERT(true));

        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssertOpt(false));

        EXPECTED = true;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssertOpt(false));
    }

#line BSLS_ASSERTTEST_SET_DIFFERENT_FILENAME

    {
        struct Production {
            static void callAssertOpt(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callAssertSafe(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }
        };

        struct Safe2 {
            static void callAssertOpt(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
#endif
            }

            static void callAssert(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
#endif
            }

            static void callAssertSafe(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
#endif
            }
        };

        // Restore filename before evaluating the test macros
#line BSLS_ASSERTTEST_RESET_THIS_FILENAME

        {
            EXPECTED = true;

            // Verify that *PASS macros are expanded in any build mode.

            BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(ASSERT(true));
            BSLS_ASSERTTEST_ASSERT_PASS_RAW(ASSERT(true));
            BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(ASSERT(true));
        }

        EXPECTED = false;
        // Force a fail if any of these macros expands the expression
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(ASSERT(true));

        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssertOpt(false));

        EXPECTED = true;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssertOpt(false));
    }

    // Restore status-quo before starting the next test
    EXPECTED = true;

//===================== LEVEL_ASSERT_SAFE ===============================//

    // Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_NONE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

#undef BSLS_ASSERTTEST_ASSERT_FAIL
#undef BSLS_ASSERTTEST_ASSERT_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_OPT_FAIL
#undef BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_OPT_PASS
#undef BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW
#undef BSLS_ASSERTTEST_ASSERT_PASS
#undef BSLS_ASSERTTEST_ASSERT_PASS_RAW
#undef BSLS_ASSERTTEST_ASSERT_SAFE_FAIL
#undef BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_SAFE_PASS
#undef BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW

#undef BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_BRUTE_FORCE_IMP
#undef BSLS_ASSERTTEST_BRUTE_FORCE_IMP_RAW
#undef BSLS_ASSERTTEST_IS_ACTIVE
#undef BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG

    //  Define the macros for this test case
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

    //  Re-include the bsls_assert headers
#undef INCLUDED_BSLS_ASSERT
#undef INCLUDED_BSLS_ASSERTTEST
#include <bsls_assert.h>
#include <bsls_asserttest.h>

    //  Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
    // 'IS_ACTIVE' macros and the are values we intend to test.
#if defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should not be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

    ASSERT(!BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG);
    ASSERT(BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG);
    ASSERT(BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG);
    ASSERT(BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG);

    {
        struct Production {
            static void callAssertOpt(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callAssertSafe(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }
        };

        struct Safe2 {
            static void callAssertOpt(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
#endif
            }

            static void callAssert(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
#endif
            }

            static void callAssertSafe(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
#endif
            }
        };

        {
            EXPECTED = true;

            // Verify that *PASS macros are expanded in any build mode.

            BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(ASSERT(true));
            BSLS_ASSERTTEST_ASSERT_PASS_RAW(ASSERT(true));
            BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(ASSERT(true));
        }

        EXPECTED = false;
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(Production::callAssertSafe(
                                                                       false));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(Safe2::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssertOpt(false));

        EXPECTED = true;
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(Production::callAssertSafe(
                                                                       false));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(Safe2::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssertOpt(false));
    }

#line BSLS_ASSERTTEST_SET_DIFFERENT_FILENAME

    {
        struct Production {
            static void callAssertOpt(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callAssertSafe(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }
        };

        struct Safe2 {
            static void callAssertOpt(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
#endif
            }

            static void callAssert(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
#endif
            }

            static void callAssertSafe(bool pass) {
                (void) pass;  // suppress 'unused parameter' compiler warning
#if defined(BDE_BUILD_TARGET_SAFE_2)
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
#endif
            }
        };

        // Restore filename before evaluating the test macros
#line BSLS_ASSERTTEST_RESET_THIS_FILENAME

        {
            EXPECTED = true;

            // Verify that *PASS macros are expanded in any build mode.

            BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(ASSERT(true));
            BSLS_ASSERTTEST_ASSERT_PASS_RAW(ASSERT(true));
            BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(ASSERT(true));
        }

        EXPECTED = false;
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(Production::callAssertSafe(
                                                                       false));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(Safe2::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssertOpt(false));

        EXPECTED = true;
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(Production::callAssertSafe(
                                                                       false));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(Safe2::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssertOpt(false));

        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssertSafe(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssertSafe(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssert(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssert(false));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssertOpt(true));
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssertOpt(false));
    }

    // Restore status-quo before starting the next test
    EXPECTED = true;

//--------------------------------------------------------------------------//
// Restore the default 'ASSERT' macro for this test driver.
#undef ASSERT
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

}

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
