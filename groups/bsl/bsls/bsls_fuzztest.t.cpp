// bsls_fuzztest.t.cpp                                                -*-C++-*-
#include <bsls_fuzztest.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_preconditions.h>
#include <bsls_timeinterval.h>  // to test 'RAW' version of macro

#include <stdio.h>
#include <stdlib.h>             // atoi(), rand()
#include <string.h>             // strcmp()

using namespace BloombergLP;
using namespace std;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// This test driver is for the macros 'BSLS_FUZZTEST_EVALUATE' and
// 'BSLS_FUZZTEST_EVALUATE_RAW' which are used to fuzz test narrow contract
// functions.  In addition, the classes 'FuzzTestPreconditionTracker' and
// 'FuzzTestHandlerGuard' are tested.  Clients will typically use
// 'FuzzTestHandlerGuard' and the macros.
//
// The most basic scenario we test is one in which a narrow contract function
// (i.e., the function under test) invokes another narrow contract function
// 'out of contract'.  In this scenario, if the function under test has a
// precondition failure, we verify that the test continues.  If, however, the
// function invoked by the function under test has a precondition failure, we
// verify that the test halts with an indication of where the assertion
// occurred.
//
// Many of the test cases below are to verify intended behavior in the presence
// of nested 'BEGIN' invocations.  We also test the intended behavior for
// assertions from other components.
//
// Note that there are '_IMP' versions of the macros that are intended to be
// used when 'BDE_ACTIVATE_FUZZ_TESTING' is not defined.  We use these '_IMP'
// versions in this test driver.
//
// ----------------------------------------------------------------------------
// [ 4] BSLS_FUZZTEST_EVALUATE
// [ 5] BSLS_FUZZTEST_EVALUATE_RAW
// ----------------------------------------------------------------------------
// bsls::FuzzTestHandlerGuard
// [ 2] FuzzTestHandlerGuard();
// [ 2] ~FuzzTestHandlerGuard();
// [ 2] static ViolationHandler getOriginalAssertionHandler() const;
// ----------------------------------------------------------------------------
// bsls::FuzzTestPreconditionTracker
// [ 3] static void handlePreconditionViolation(const AssertViolation &a);
// [ 3] static void initStaticState(const char *fn, int ln);
// [ 3] static void handlePreconditionsBegin();
// [ 3] static void handlePreconditionsEnd();
// [ 3] static void handleException(const FuzzTestPreconditionException&);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 6] USAGE EXAMPLE

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

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

#define STRINGIFY2(...) "" #__VA_ARGS__
#define STRINGIFY(a) STRINGIFY2(a)

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

static void printFlags()
    // Print a diagnostic message to standard output if any of the preprocessor
    // flags of interest are defined, and their value if a value had been set.
    // An "Enter" and "Leave" message is printed unconditionally so there is
    // some report even if all of the flags are undefined.
{
    printf("printFlags: Enter\n");

    printf("\nprintFlags: bsls_fuzztest Macros\n");

    printf("\nBSLS_FUZZTEST_EVALUATE: ");
#ifdef BSLS_FUZZTEST_EVALUATE
    printf("%s\n",
           STRINGIFY(BSLS_FUZZTEST_EVALUATE(true)));
#else
    printf("UNDEFINED\n");
#endif

    printf("\nBSLS_FUZZTEST_EVALUATE_RAW: ");
#ifdef BSLS_FUZZTEST_EVALUATE_RAW
    printf("%s\n",
           STRINGIFY(BSLS_FUZZTEST_EVALUATE_RAW(true)));
#else
    printf("UNDEFINED\n");
#endif

    printf("\n\nprintFlags: Leave\n");
}

namespace test_case_common {
bool assertInDifferentComponent()
    // Invoke 'ti.addInterval' with inputs that will cause it to fail.  The
    // behavior is undefined unless 'ti.addInterval' succeeds.  Note that since
    // we are testing outside the fuzz harness (i.e., we are not in
    // 'LLVMFuzzerTestOneInput'), we cannot rely on other components employing
    // 'BSLS_PRECONDITIONS_BEGIN_IMP' (i.e., client code will employ the
    // non-'IMP' macros).  Also note that this function returns a 'bool' so it
    // can be invoked from 'BSLS_ASSERT', and the choice of 'true' is
    // arbitrary.
{
    BSLS_PRECONDITIONS_BEGIN_IMP();
    bsls::TimeInterval ti(9e18);
    BSLS_ASSERT(ti != ti.addInterval(9e18, 0));  // fails precondition
    BSLS_PRECONDITIONS_END_IMP();
    return true;
}

void assertInDifferentComponentInNestedBlock()
    // Invoke 'ti.addInterval' with inputs that will cause it to fail.  The
    // behavior is undefined unless 'ti.addInterval' succeeds.  Note that the
    // invocation of 'addInterval' is nested within a second level of
    // 'BEGIN/END' macros; and that since we are testing outside the fuzz
    // harness (i.e., we are not in 'LLVMFuzzerTestOneInput'), we cannot rely
    // on other components employing 'BSLS_PRECONDITIONS_BEGIN_IMP' (i.e.,
    // client code will employ the non-'IMP' macros).
{
    BSLS_PRECONDITIONS_BEGIN_IMP();
    BSLS_PRECONDITIONS_BEGIN_IMP();
    bsls::TimeInterval ti(9e18);
    BSLS_ASSERT(ti != ti.addInterval(9e18, 0));  // fails precondition
    BSLS_PRECONDITIONS_END_IMP();
    BSLS_PRECONDITIONS_END_IMP();
}
}  // close namespace test_case_common

namespace test_case_4 {
struct AssertLocation {
    enum Enum {
        NONE,
        FIRST_BLOCK_TOP_LEVEL,
        FIRST_BLOCK_SECOND_LEVEL,
        SECOND_BLOCK,
    };
};

void triggerAssertion(AssertLocation::Enum location)
    // Invoke the assertion handler at the specified 'location'.
{
    BSLS_PRECONDITIONS_BEGIN_IMP();
    if (AssertLocation::FIRST_BLOCK_TOP_LEVEL == location) {
        BSLS_ASSERT_INVOKE_NORETURN("FIRST_BLOCK_TOP_LEVEL");
    }
    {
        BSLS_PRECONDITIONS_BEGIN_IMP();
        if (AssertLocation::FIRST_BLOCK_SECOND_LEVEL == location){
            BSLS_ASSERT_INVOKE_NORETURN("FIRST_BLOCK_SECOND_LEVEL");
        }
        BSLS_PRECONDITIONS_END_IMP();
    }
    BSLS_PRECONDITIONS_END_IMP();

    BSLS_PRECONDITIONS_BEGIN_IMP();
    if (AssertLocation::SECOND_BLOCK == location){
        BSLS_ASSERT_INVOKE_NORETURN("SECOND_BLOCK");
    }
    BSLS_PRECONDITIONS_END_IMP();
}

void triggerAssertionInDifferentComponent()
    // Do nothing.  The behavior is undefined unless invocation to
    // 'test_case_common::assertInDifferentComponent' returns 'true'.
{
    BSLS_PRECONDITIONS_BEGIN_IMP();
    BSLS_ASSERT(test_case_common::assertInDifferentComponent());
    BSLS_PRECONDITIONS_END_IMP();
}

}  // close namespace test_case_4

//=============================================================================
//                             USAGE EXAMPLE
//-----------------------------------------------------------------------------
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example: Basic Usage of Macros
///- - - - - - - - - - - - - - - -
// The macros in this component rely upon the presence of related macros from
// 'bsls_preconditions'.  The fuzzing macros are typically used in a fuzzing
// build, in which case the entry point is 'LLVMFuzzerTestOneInput'.
//
// In this example, we illustrate the intended usage of two macros:
// 'BSLS_FUZZTEST_EVALUATE' and 'BSLS_FUZZTEST_EVALUATE_RAW'.
//
// First, in order to illustrate the use of 'BSLS_FUZZTEST_EVALUATE', we
// define two functions that implement the 'sqrt' function, both decorated
// with the precondition 'BEGIN' and 'END' macros.  'mySqrt' forwards its
// argument to 'newtonsSqrt', which has a slightly more restrictive
// precondition: 'mySqrt' accepts 0, while 'newtonsSqrt' does not.
//..
    double newtonsSqrt(double x)
        // Return the square root of the specified 'x' according to Newton's
        // method.  The behavior is undefined unless 'x > 0'.
    {
        BSLS_PRECONDITIONS_BEGIN_IMP();                           // _IMP ADDED
        BSLS_ASSERT(x > 0);
        BSLS_PRECONDITIONS_END_IMP();                             // _IMP ADDED

        double guess = 1.0;
        for (int ii = 0; ii < 100; ++ii) {
            guess = (guess + x / guess) / 2;
        }
        return guess;
    }

    double mySqrt(double x)
        // Return the square root of the specified 'x'.  The behavior is
        // undefined unless 'x >= 0'.
    {
        BSLS_PRECONDITIONS_BEGIN_IMP();                           // _IMP ADDED
        BSLS_ASSERT(x >= 0);
        BSLS_PRECONDITIONS_END_IMP();                             // _IMP ADDED
        return newtonsSqrt(x);
    }
//..
// Then, for the illustration of 'BSLS_FUZZTEST_EVALUATE_RAW', we define a
// class, 'Timer', containing a 'start' function that uses in its
// implementation a narrow contract function, 'setInterval', from another
// component, 'bsls::TimeInterval'.  This function, 'setInterval', has
// precondition checks that are surrounded by 'BEGIN' and 'END'.
//..
    class Timer
        // This class implements a simple interval timer.
    {
      private:
        // DATA
        bsls::TimeInterval d_timeout;  // timeout seconds and nanoseconds

      public:
        // MANIPULATORS
        void start(bsls::Types::Int64 seconds, int nanoseconds)
            // Start the countdown with a timer having the value given by the
            // sum of the specified integral number of 'seconds' and
            // 'nanoseconds'.  The behavior is undefined unless the total
            // number of seconds in the resulting time interval can be
            // represented with a 64-bit signed integer (see
            // 'TimeInterval::isValid').
        {
            #ifndef BDE_ACTIVATE_FUZZ_TESTING                          // ADDED
            BSLS_PRECONDITIONS_BEGIN_IMP();                            // ADDED
            #endif                                                     // ADDED
            d_timeout.setInterval(seconds, nanoseconds);
            #ifndef BDE_ACTIVATE_FUZZ_TESTING                          // ADDED
            BSLS_PRECONDITIONS_END_IMP();                              // ADDED
            #endif                                                     // ADDED
            //...
        }
    };
//..
// Next, implement 'LLVMFuzzerTestOneInput'.  We first select the test case
// number based on the supplied fuzz data.
//..
    extern "C"
    int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
        // Use the specified 'data' array of 'size' bytes as input to methods
        // of this component and return zero.
    {
        int         test;
        if (data && size) {
            test = static_cast<unsigned char>(*data) % 100;
            ++data;
            --size;
        }
        else {
            test = 0;
        }

        switch (test) { case 0:  // Zero is always the leading case.
//..
// Then, we implement the test case to illustrate the use of
// 'BSLS_FUZZTEST_EVALUATE'.
//..
          case 2: {
            // ----------------------------------------------------------------
            // 'mySqrt'
            //
            // Concerns:
            //: 1. That 'mySqrt' does not invoke the original assertion handler
            //:    for any 'input' value.
            //
            // Testing: double mySqrt(double x);
            // ----------------------------------------------------------------
            if (size < sizeof(double)) {
                return 0;                                             // RETURN
            }
            double input;
            memcpy(&input, data, sizeof(double));
//..
// Next, we set up the handler guard that installs the precondition handlers.
//..
            bsls::FuzzTestHandlerGuard hg;
//..
// Now, we invoke the function under test (i.e., 'mySqrt') with the
// 'BSLS_FUZZTEST_EVALUATE' macro.
//..
            BSLS_FUZZTEST_EVALUATE_IMP(mySqrt(input));            // _IMP ADDED
//..
// If the 'input' value obtained from the fuzz data is positive (e.g., 4.0),
// the 'mySqrt' implementation generates correct results without any errors.
// For negative inputs (e.g., -4.0), because the precondition violation occurs
// in the top level, execution of the test does not halt.  If 0 is passed as
// the input, 'mySqrt' forwards it to 'newtonsSqrt' where a second-level
// assertion occurs and execution halts, indicating a defect in the
// implementation of 'mySqrt'.
//..
          } break;
//..
// Next, we implement the test case to illustrate the use of
// 'BSLS_FUZZTEST_EVALUATE_RAW'.
//..
          case 1: {
            // ----------------------------------------------------------------
            // 'Timer::start'
            //
            // Concerns:
            //: 1. That 'start', when invoked with the 'RAW' macro, does not
            //:    invoke the original assertion handler.
            //
            // Testing: void Timer::start(Int64 seconds, int nanoseconds);
            // ----------------------------------------------------------------
            if (size < sizeof(bsls::Types::Int64) + sizeof(int)) {
                return 0;                                             // RETURN
            }
            bsls::Types::Int64 seconds;
            int                nanoseconds;
            memcpy(&seconds, data, sizeof(bsls::Types::Int64));
            memcpy(&nanoseconds,
                   data + sizeof(bsls::Types::Int64),
                   sizeof(int));
//..
// Now, we set up the handler guard that installs the precondition handlers.
//..
            bsls::FuzzTestHandlerGuard hg;
//..
// Finally, we invoke the function under test with the
// 'BSLS_FUZZTEST_EVALUATE_RAW' macro.
//..
            Timer t;
            BSLS_FUZZTEST_EVALUATE_RAW_IMP(
                                 t.start(seconds, nanoseconds));  // _IMP ADDED
//..
// If the total number of seconds resulting from the sum of 'seconds' and
// 'nanoseconds' cannot be represented with a 64-bit signed integer, a
// top-level assertion failure from a different component will occur.  Because
// we have invoked 'start' with the 'RAW' macro, a component name check will
// not be performed, and execution will continue.
//..
          } break;
          default: {
          } break;
        }

        if (testStatus > 0) {
            BSLS_ASSERT_INVOKE("FUZZ TEST FAILURES");
        }

        return 0;
    }
//..
// Note that the use of 'bslim::FuzzUtil' and 'bslim::FuzzDataView' can
// simplify the consumption of fuzz data.

//=============================================================================
//                              FUZZ TESTING
//-----------------------------------------------------------------------------

#ifdef BDE_ACTIVATE_FUZZ_TESTING
#define main test_driver_main
#endif

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;

    (void)        veryVerbose;  // unused variable warning
    (void)    veryVeryVerbose;  // unused variable warning

    printf( "TEST %s CASE %d\n", __FILE__, test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 6: {
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
        //:   leading comment characters.  (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // -------------------------------------------------------------------
        if (verbose) printf("USAGE EXAMPLE\n"
                            "=============\n");
#if defined(BDE_BUILD_TARGET_EXC)
        {
            // First, we invoke 'LLVMFuzzerTestOneInput' with input that does
            // not violate any preconditions.
            uint8_t myFuzzData[32];
            myFuzzData[0] = 2;  // invoke case 2 of 'LLVMFuzzerTestOneInput'
            double val    = 4.0;
            memcpy(myFuzzData + 1, &val, sizeof(double));
            LLVMFuzzerTestOneInput(myFuzzData, 1 + sizeof(double));
        }
        {
            // Then, we invoke 'LLVMFuzzerTestOneInput' with input that
            // violates a top-level precondition, which is handled by the
            // macro.
            uint8_t myFuzzData[32];
            myFuzzData[0] = 2;  // invoke case 2 of 'LLVMFuzzerTestOneInput'
            double val    = -4.0;
            memcpy(myFuzzData + 1, &val, sizeof(double));
            LLVMFuzzerTestOneInput(myFuzzData, 1 + sizeof(double));
        }
        {
            // Finally, we invoke a function that fails a precondition in a
            // different component.  Note that here we invoke 'addSeconds',
            // which has conditional inclusion of 'BSLS_PRECONDITIONS_BEGIN'
            // and 'END'.  We do this to achieve the desired behavior: that a
            // *top-level* precondition violation occurs in a different
            // component, regardless of the ufid (i.e., regardless of whether
            // the ufid contains 'fuzz').  Without the conditional, in a
            // fuzzing build, the violation in the other component would occur
            // in the second level.
            uint8_t myFuzzData[32];
            myFuzzData[0] = 1;  // invoke case 1 of 'LLVMFuzzerTestOneInput'
            bsls::Types::Int64 seconds     = 0x7FFFFFFFFFFFFFFF;
            int                nanoseconds = 2e9;
            memcpy(myFuzzData + 1, &seconds, sizeof(bsls::Types::Int64));
            memcpy(myFuzzData + 1 + sizeof(bsls::Types::Int64),
                   &nanoseconds,
                   sizeof(int));
            LLVMFuzzerTestOneInput(
                                 myFuzzData,
                                 1 + sizeof(bsls::Types::Int64) + sizeof(int));
        }
#endif  // defined(BDE_BUILD_TARGET_EXC)
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // BSLS_FUZZTEST_EVALUATE_RAW
        //
        // Concerns:
        //: 1 A top-level assertion failure in another component does not
        //:   invoke the original assertion handler.
        //:
        //: 2 A second level assertion failure in another component does invoke
        //:   the original assertion handler.
        //
        // Plan:
        //: 1 Invoke a function that asserts in a different component inside
        //:   a top-level 'BEGIN/END' block.  Verify that no assert violation
        //:   occurs.
        //:
        //: 2 Invoke a function that asserts in a different component inside
        //:   a nested 'BEGIN/END' block.  Verify that a violation occurs.
        //
        // Testing:
        //   BSLS_FUZZTEST_EVALUATE_RAW
        // --------------------------------------------------------------------
        if (verbose) printf("BSLS_FUZZTEST_EVALUATE_RAW\n"
                            "==========================\n");
#if defined(BDE_BUILD_TARGET_EXC)
        {
            bsls::AssertTestHandlerGuard g;
            bsls::FuzzTestHandlerGuard   hG;
            ASSERT_PASS(BSLS_FUZZTEST_EVALUATE_RAW_IMP(
                            test_case_common::assertInDifferentComponent()));
        }
        {
            bsls::AssertTestHandlerGuard g;
            bsls::FuzzTestHandlerGuard   hG;
            ASSERT_FAIL_RAW(BSLS_FUZZTEST_EVALUATE_RAW_IMP(
                test_case_common::assertInDifferentComponentInNestedBlock()));
        }
#endif  // defined(BDE_BUILD_TARGET_EXC)
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // BSLS_FUZZTEST_EVALUATE
        //
        // Concerns:
        //: 1 Invoking a narrow contract function with inputs that violate the
        //:   top-level preconditions does not invoke the original assertion
        //:   handler.
        //:
        //: 2 Invoking a narrow contract function with inputs that satisfy the
        //:   top-level preconditions but violate nested preconditions does
        //:   invoke the original assertion handler.  The location of the
        //:   subsequent precondition checks does not matter -- whether the
        //:   checks are within the top-level 'BSLS_PRECONDITIONS_BEGIN/END'
        //:   block or after.
        //:
        //: 3 Invoking a narrow contract function in contract does not invoke
        //:   the original assertion handler.
        //:
        //: 4 Invoking a narrow contract function with input that satisfies the
        //:   top-level preconditions but -- within the top-level
        //:   'BSLS_PRECONDITIONS_BEGIN/END' block -- violates the
        //:   preconditions of a function from a different component, does
        //:   invoke the original assertion handler.
        //
        // Plan:
        //: 1 Invoke a function that contains three different 'BEGIN/END'
        //:   blocks with different input arguments:
        //:
        //:   a. with an input that does not trigger any assertions inside a
        //:      'BEGIN/END' block.  Verify that no assert violation occurs.
        //:
        //:   b. with an input that triggers an assertion inside a top-level
        //:      'BEGIN/END' block.  Verify that no assert violation occurs.
        //:
        //:   c. with an input that triggers an assertion inside a second-level
        //:      'BEGIN/END' block.  Verify that a violation occurs.
        //:
        //:   d. with an input that triggers an assertion inside a second
        //:      top-level 'BEGIN/END' block.  Verify that a violation occurs.
        //:
        //: 2 Invoke a function with an input that satisfies
        //:   its preconditions and then -- within the initial 'BEGIN/END'
        //:   block -- invoke a narrow contract function from another component
        //:   out of contract.  Verify that a violation occurs.
        //:
        //
        // Testing:
        //   BSLS_FUZZTEST_EVALUATE
        // --------------------------------------------------------------------
        if (verbose) printf("BSLS_FUZZTEST_EVALUATE\n"
                            "======================\n");
#if defined(BDE_BUILD_TARGET_EXC)
        {
            bsls::AssertTestHandlerGuard g;
            bsls::FuzzTestHandlerGuard   hG;
            ASSERT_PASS(
                      BSLS_FUZZTEST_EVALUATE_IMP(test_case_4::triggerAssertion(
                      test_case_4::AssertLocation::NONE)));
            ASSERT_PASS(
                      BSLS_FUZZTEST_EVALUATE_IMP(test_case_4::triggerAssertion(
                      test_case_4::AssertLocation::FIRST_BLOCK_TOP_LEVEL)));
            ASSERT_FAIL(
                      BSLS_FUZZTEST_EVALUATE_IMP(test_case_4::triggerAssertion(
                      test_case_4::AssertLocation::FIRST_BLOCK_SECOND_LEVEL)));
            ASSERT_FAIL(
                      BSLS_FUZZTEST_EVALUATE_IMP(test_case_4::triggerAssertion(
                      test_case_4::AssertLocation::SECOND_BLOCK)));
        }
        {
            bsls::AssertTestHandlerGuard g;
            bsls::FuzzTestHandlerGuard   hG;
            BSLS_ASSERTTEST_ASSERT_FAIL_RAW(BSLS_FUZZTEST_EVALUATE_IMP(
                test_case_4::triggerAssertionInDifferentComponent()));
        }
#endif  // defined(BDE_BUILD_TARGET_EXC)
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING FUZZTEST PRECONDITION TRACKER
        //
        // Concerns:
        //: 1 That directly invoking the methods of
        //:   'FuzzTestPreconditionTracker' behaves as expected.
        //:
        //:   1 The filename that caused the precondition violation is the same
        //:     as the one reported by the assertion handler.
        //:
        //:   2 The expression that caused the assertion is the same as the one
        //:     returned by the assertion handler.
        //:
        //:   3 If the precondition is violated, either a
        //:     'FuzzTestPreconditionException' is thrown or the original
        //:     assertion handler is invoked, as expected.
        //:
        //:   4 Directly invoking 'handlePreconditionViolation' results in the
        //:     same behavior as triggering an assertion with 'BSLS_ASSERT'.
        //
        // Plan:
        //: 1 Trigger a top-level precondition violation and verify that the
        //:   methods behave as expected.
        //:
        //:   1 Directly invoke 'initStaticState' with the current filename.
        //:
        //:   2 Trigger a top-level precondition violation.
        //:
        //:   3 Verify that a 'FuzzTestPreconditionException' is thrown.
        //:
        //:   4 Verify that the filename and triggering expression match the
        //:     expected values.
        //:
        //: 2 Trigger a second-level precondition violation and verify that the
        //:   methods behave as expected.
        //:
        //:   1 Directly invoke 'initStaticState' with the current filename.
        //:
        //:   2 Trigger a second-level precondition violation.
        //:
        //:   3 Verify that the original assertion handler is invoked, which
        //:     throws an 'AssertTestException'.
        //:
        //:   4 Verify that the filename and triggering expression match the
        //:     expected values.
        //:
        //: 3 Trigger a top-level precondition violation from another component
        //:   and verify that the methods behave as expected.
        //:
        //:   1 Directly invoke 'initStaticState' with a different component
        //:     name passed.
        //:
        //:   2 Trigger a top-level precondition violation.
        //:
        //:   3 Verify that a 'FuzzTestPreconditionException' is thrown.
        //:
        //:   4 Verify that the filename and triggering expression match the
        //:     expected values.
        //:
        //:   5 Directly invoke 'handleException' and thereby verify that the
        //:     original assertion handler is invoked.
        //:
        //: 4 Manually invoke 'handlePreconditionViolation' and verify that the
        //:   methods behave as expected.
        //:
        //:   1 Create a dummy 'AssertViolation'.
        //:
        //:   2 Directly invoke 'initStaticState' with the current filename.
        //:
        //:   3 Manually invoke 'handlePreconditionViolation' with the created
        //:     'AssertViolation'.
        //:
        //:   4 Verify that an 'FuzzTestPreconditionException' is thrown.
        //:
        //:   5 Verify that the filename and triggering expression match the
        //:     expected values.
        //:
        //: 5 Trigger a top-level precondition violation and verify that the
        //:   methods behave as expected.
        //:
        //:   1 Directly invoke 'initStaticState' with the current filename.
        //:
        //:   2 Trigger a top-level precondition violation by ending the
        //:     second-level precondition check with 'handlePreconditionsEnd'.
        //:
        //:   3 Verify that a 'FuzzTestPreconditionException' is thrown.
        //:
        //:   4 Verify that the filename and triggering expression match the
        //:     expected values.
        //:
        //:   5 Directly invoke 'handleException' and thereby verify that the
        //:     original assertion handler is invoked.
        //:
        //
        // Testing:
        //   static void initStaticState(const char *fn, int ln);
        //   static void handlePreconditionsBegin();
        //   static void handlePreconditionsEnd();
        //   static void handleException(const FuzzTestPreconditionException&);
        //   static void handlePreconditionViolation(const AssertViolation &a);
        // --------------------------------------------------------------------
        if (verbose) printf("TESTING FUZZTEST PRECONDITION TRACKER\n"
                            "=====================================\n");
#if defined(BDE_BUILD_TARGET_EXC)
#if defined(BSLS_ASSERT_IS_ACTIVE)
        {
            if (veryVerbose)
                printf("TOP-LEVEL PRECONDITION VIOLATION (SAME ORIGIN)\n");
            bsls::AssertTestHandlerGuard g;
            bsls::FuzzTestHandlerGuard   hg;
            ASSERT_PASS(
                bsls::FuzzTestPreconditionTracker::initStaticState(__FILE__);
                try {
                    bsls::FuzzTestPreconditionTracker::
                        handlePreconditionsBegin();
                    BSLS_ASSERT(1 == 2);
                    BSLS_ASSERT_INVOKE_NORETURN("unreachable");
                } catch (bsls::FuzzTestPreconditionException& ftpe) {
                    ASSERT(0 == strcmp(ftpe.assertViolation().fileName(),
                                       __FILE__));
                    ASSERT(0 ==
                           strcmp(ftpe.assertViolation().comment(), "1 == 2"));
                    bsls::FuzzTestPreconditionTracker::handleException(ftpe);
                });
        }
        {
            if (veryVerbose)
                printf("SECOND-LEVEL PRECONDITION VIOLATION (SAME ORIGIN)\n");
            bsls::AssertTestHandlerGuard g;
            bsls::FuzzTestHandlerGuard   hg;
                // Note that 'ASSERT_FAIL' is not used in this case because we
                // want to explicitly catch the 'AssertTestException'.
            bsls::FuzzTestPreconditionTracker::initStaticState(__FILE__);
            try {
                bsls::FuzzTestPreconditionTracker::handlePreconditionsBegin();
                BSLS_ASSERT(true);
                bsls::FuzzTestPreconditionTracker::handlePreconditionsBegin();
                BSLS_ASSERT(1 == 2);
                BSLS_ASSERT_INVOKE_NORETURN("unreachable");
            }
            catch (bsls::AssertTestException& ex) {
                ASSERT(0 == strcmp(ex.filename(), __FILE__));
                ASSERT(0 == strcmp(ex.expression(), "1 == 2"));
            }
        }
        {
            if (veryVerbose)
                printf(
                    "TOP-LEVEL PRECONDITION VIOLATION (DIFFERENT ORIGIN)\n");
            bsls::AssertTestHandlerGuard g;
            bsls::FuzzTestHandlerGuard   hg;
            ASSERT_FAIL(
                bsls::FuzzTestPreconditionTracker::initStaticState(
                    "differentComponent.cpp");
                try {
                    bsls::FuzzTestPreconditionTracker::
                        handlePreconditionsBegin();
                    BSLS_ASSERT(1 == 2);
                    BSLS_ASSERT_INVOKE_NORETURN("unreachable");
                } catch (bsls::FuzzTestPreconditionException& ftpe) {
                    ASSERT(0 == strcmp(ftpe.assertViolation().fileName(),
                                       __FILE__));
                    ASSERT(0 ==
                           strcmp(ftpe.assertViolation().comment(), "1 == 2"));
                    bsls::FuzzTestPreconditionTracker::handleException(ftpe);
                });
        }
        {
            if (veryVerbose)
                printf("TOP-LEVEL PRECONDITION VIOLATION DIRECTLY INVOKING "
                       "'handlePreconditionViolation'\n");
            bsls::AssertViolation av("Expression that fails",
                                        __FILE__,
                                        __LINE__,
                                        bsls::Assert::k_LEVEL_ASSERT);

            bsls::AssertTestHandlerGuard g;
            bsls::FuzzTestHandlerGuard   hg;
            ASSERT_PASS(
                bsls::FuzzTestPreconditionTracker::initStaticState(__FILE__);
                try {
                    bsls::FuzzTestPreconditionTracker::
                        handlePreconditionsBegin();
                    bsls::FuzzTestPreconditionTracker::
                        handlePreconditionViolation(av);
                    BSLS_ASSERT_INVOKE_NORETURN("unreachable");
                } catch (bsls::FuzzTestPreconditionException& ftpe) {
                    ASSERT(0 == strcmp(ftpe.assertViolation().fileName(),
                                       __FILE__));
                    ASSERT(0 == strcmp(ftpe.assertViolation().comment(),
                                       "Expression that fails"));
                    bsls::FuzzTestPreconditionTracker::handleException(ftpe);
                });
        }
        {
            if (veryVerbose)
                printf("TOP-LEVEL PRECONDITION VIOLATION AFTER NESTED "
                       "BEGIN/END BLOCK\n");
            bsls::AssertTestHandlerGuard g;
            bsls::FuzzTestHandlerGuard   hg;
            ASSERT_PASS(
                bsls::FuzzTestPreconditionTracker::initStaticState(__FILE__);
                try {
                    bsls::FuzzTestPreconditionTracker::
                        handlePreconditionsBegin();
                    BSLS_ASSERT(true);
                    bsls::FuzzTestPreconditionTracker::
                        handlePreconditionsBegin();
                    BSLS_ASSERT(true);
                    bsls::FuzzTestPreconditionTracker::
                        handlePreconditionsEnd();
                    BSLS_ASSERT(1 == 2);
                    BSLS_ASSERT_INVOKE_NORETURN("unreachable");
                } catch (bsls::FuzzTestPreconditionException& ftpe) {
                    ASSERT(0 == strcmp(ftpe.assertViolation().fileName(),
                                       __FILE__));
                    ASSERT(0 ==
                           strcmp(ftpe.assertViolation().comment(), "1 == 2"));
                    bsls::FuzzTestPreconditionTracker::handleException(ftpe);
                });
        }
#endif  // defined(BSLS_ASSERT_IS_ACTIVE)
#endif  // defined(BDE_BUILD_TARGET_EXC)
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // FUZZTEST HANDLER GUARD
        //
        // Concerns:
        //: 1 That the guard installs handlers at construction.
        //:
        //: 2 Restores the original handlers at destruction.
        //
        // Plan:
        //: 1 Verify that the original assertion handler is the expected one
        //:   (i.e., 'failByAbort').
        //:
        //: 2 Create a guard inside a block, and verify, using
        //:   'PreconditionsHandler' and 'FuzzTestPreconditionTracker', that
        //:   the handlers were installed.
        //:
        //: 3 After the guard goes out of scope, verify that the original
        //:   handlers have been restored.
        //:
        // Testing:
        //   FuzzTestHandlerGuard();
        //   ~FuzzTestHandlerGuard();
        //   static ViolationHandler getOriginalAssertionHandler() const;
        // --------------------------------------------------------------------
        if (verbose)
            printf("FUZZTEST HANDLER GUARD\n"
                   "======================\n");

        ASSERT(&bsls::Assert::failByAbort == bsls::Assert::violationHandler());
        ASSERT(
            bsls::PreconditionsHandler::getBeginHandler() ==
            &bsls::PreconditionsHandler::noOpHandler);
        ASSERT(bsls::PreconditionsHandler::getEndHandler() ==
                &bsls::PreconditionsHandler::noOpHandler);
        {
            bsls::FuzzTestHandlerGuard hg;
            ASSERT(&bsls::Assert::failByAbort ==
                   hg.getOriginalAssertionHandler());
            ASSERT(&bsls::FuzzTestPreconditionTracker::
                       handlePreconditionViolation ==
                   bsls::Assert::violationHandler());
            ASSERT(
                bsls::PreconditionsHandler::getBeginHandler() ==
                &bsls::FuzzTestPreconditionTracker::handlePreconditionsBegin);

            ASSERT(bsls::PreconditionsHandler::getEndHandler() ==
                   &bsls::FuzzTestPreconditionTracker::handlePreconditionsEnd);
        }
        ASSERT(&bsls::Assert::failByAbort == bsls::Assert::violationHandler());
        ASSERT(
            bsls::PreconditionsHandler::getBeginHandler() ==
            &bsls::PreconditionsHandler::noOpHandler);
        ASSERT(bsls::PreconditionsHandler::getEndHandler() ==
                &bsls::PreconditionsHandler::noOpHandler);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //: 1 This test driver builds on all platforms.
        //
        // Plan:
        //: 1 Print out flags in verbose mode.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        if (verbose) {
            printFlags();
        }
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d` NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
