// bsls_fuzztest.h                                                    -*-C++-*-
#ifndef INCLUDED_BSLS_FUZZTEST
#define INCLUDED_BSLS_FUZZTEST

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide macros for use in fuzz testing narrow-contract functions.
//
//@CLASSES:
//  bsls::FuzzTestPreconditionTracker: utility for tracking assert violations
//  bsls::FuzzTestHandlerGuard: guard for fuzz testing assert-handler
//
//@MACROS:
//  BSLS_FUZZTEST_EVALUATE(EXPRESSION): wrapper for narrow contract function
//  BSLS_FUZZTEST_EVALUATE_RAW(EXPRESSION): wrapper with no origination check
//
//@SEE_ALSO: bsls_preconditions
//
//@DESCRIPTION: This component provides two macros, 'BSLS_FUZZTEST_EVALUATE'
// and 'BSLS_FUZZTEST_EVALUATE_RAW', that can be used in fuzz testing narrow
// contract functions.  They are intended to be used in conjunction with
// 'bsls::FuzzTestHandlerGuard' as well as 'BSLS_PRECONDITIONS_BEGIN' and
// 'BSLS_PRECONDITIONS_END'.
//
// When fuzzing narrow contract functions, if we do not wish to "massage" the
// data we pass to the function (as this may be error-prone and might introduce
// bias into the tested input) we must address the issue that we will often
// invoke the function out of contract, and this will cause the function to
// assert, and the test to end prematurely.  The macros defined in this
// component solve this issue by detecting the location of precondition
// violations.  Functions with narrow contracts that are to be tested must be
// decorated with the 'BSLS_PRECONDITIONS_BEGIN' and 'BSLS_PRECONDITIONS_END'
// macros.  These macros must be placed just before and after the function
// preconditions are checked.
//
// All these macros are intended to be used in fuzzing builds in which
// 'BDE_ACTIVATE_FUZZ_TESTING' is defined.  For our purposes, those
// preconditions that fail in the function under test (i.e., the one invoked
// by 'BSLS_FUZZTEST_EVALUATE') are treated differently from all other
// precondition failures.  We refer to these preconditions as "top-level"
// preconditions.  If a top-level precondition fails -- and the assertion is
// not from another component -- the execution will continue: we do not wish to
// stop the fuzz test if we simply invoked the narrow contract function under
// test out of contract.  We wish to detect only subsequent assertions (i.e.,
// not in the top-level), or assertions from other components.
//
// The 'BSLS_FUZZTEST_EVALUATE_RAW' macro does not check if the assertion
// originates from another component, though, like the non-'RAW' version, it
// ignores only top-level assertions.  This behavior is desirable in cases in
// which a function delegates its implementation and associated precondition
// checks to a different component.  In such cases, a precondition failure
// ought not cause the fuzz test to end.
//
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
//  double newtonsSqrt(double x)
//      // Return the square root of the specified 'x' according to Newton's
//      // method.  The behavior is undefined unless 'x > 0'.
//  {
//      BSLS_PRECONDITIONS_BEGIN();
//      BSLS_ASSERT(x > 0);
//      BSLS_PRECONDITIONS_END();
//
//      double guess = 1.0;
//      for (int ii = 0; ii < 100; ++ii) {
//          guess = (guess + x / guess) / 2;
//      }
//      return guess;
//  }
//
//  double mySqrt(double x)
//      // Return the square root of the specified 'x'.  The behavior is
//      // undefined unless 'x >= 0'.
//  {
//      BSLS_PRECONDITIONS_BEGIN();
//      BSLS_ASSERT(x >= 0);
//      BSLS_PRECONDITIONS_END();
//      return newtonsSqrt(x);
//  }
//..
// Then, for the illustration of 'BSLS_FUZZTEST_EVALUATE_RAW', we define a
// class, 'Timer', containing a 'start' function that uses in its
// implementation a narrow contract function, 'setInterval', from another
// component, 'bsls::TimeInterval'.  This function, 'setInterval', has
// precondition checks that are surrounded by 'BEGIN' and 'END'.
//..
//  class Timer
//      // This class implements a simple interval timer.
//  {
//    private:
//      // DATA
//      bsls::TimeInterval d_timeout;  // timeout seconds and nanoseconds
//
//    public:
//      // MANIPULATORS
//      void start(bsls::Types::Int64 seconds, int nanoseconds)
//          // Start the countdown with a timer having the value given by the
//          // sum of the specified integral number of 'seconds' and
//          // 'nanoseconds'.  The behavior is undefined unless the total
//          // number of seconds in the resulting time interval can be
//          // represented with a 64-bit signed integer (see
//          // 'TimeInterval::isValid').
//      {
//          d_timeout.setInterval(seconds, nanoseconds);
//          //...
//      }
//  };
//..
// Next, implement 'LLVMFuzzerTestOneInput'.  We first select the test case
// number based on the supplied fuzz data.
//..
//  extern "C"
//  int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
//      // Use the specified 'data' array of 'size' bytes as input to methods
//      // of this component and return zero.
//  {
//      int         test;
//      if (data && size) {
//          test = static_cast<unsigned char>(*data) % 100;
//          ++data;
//          --size;
//      }
//      else {
//          test = 0;
//      }
//
//      switch (test) { case 0:  // Zero is always the leading case.
//..
// Then, we implement the test case to illustrate the use of
// 'BSLS_FUZZTEST_EVALUATE'.
//..
//        case 2: {
//          // ----------------------------------------------------------------
//          // 'mySqrt'
//          //
//          // Concerns:
//          //: 1. That 'mySqrt' does not invoke the original assertion handler
//          //:    for any 'input' value.
//          //
//          // Testing: double mySqrt(double x);
//          // ----------------------------------------------------------------
//          if (size < sizeof(double)) {
//              return 0;                                             // RETURN
//          }
//          double input;
//          memcpy(&input, data, sizeof(double));
//..
// Next, we set up the handler guard that installs the precondition handlers.
//..
//          bsls::FuzzTestHandlerGuard hg;
//..
// Now, we invoke the function under test (i.e., 'mySqrt') with the
// 'BSLS_FUZZTEST_EVALUATE' macro.
//..
//          BSLS_FUZZTEST_EVALUATE(mySqrt(input));
//..
// If the 'input' value obtained from the fuzz data is positive (e.g., 4.0),
// the 'mySqrt' implementation generates correct results without any errors.
// For negative inputs (e.g., -4.0), because the precondition violation occurs
// in the top level, execution of the test does not halt.  If 0 is passed as
// the input, 'mySqrt' forwards it to 'newtonsSqrt' where a second-level
// assertion occurs and execution halts, indicating a defect in the
// implementation of 'mySqrt'.
//..
//        } break;
//..
// Next, we implement the test case to illustrate the use of
// 'BSLS_FUZZTEST_EVALUATE_RAW'.
//..
//        case 1: {
//          // ----------------------------------------------------------------
//          // 'Timer::start'
//          //
//          // Concerns:
//          //: 1 That 'start', when invoked with the 'RAW' macro, does not
//          //:   invoke the original assertion handler.
//          //
//          // Testing:
//          //   void Timer::start(Int64 seconds, int nanoseconds);
//          // ----------------------------------------------------------------
//
//          if (size < sizeof(bsls::Types::Int64) + sizeof(int)) {
//              return 0;                                             // RETURN
//          }
//          bsls::Types::Int64 seconds;
//          int                nanoseconds;
//          memcpy(&seconds, data, sizeof(bsls::Types::Int64));
//          memcpy(&nanoseconds,
//                 data + sizeof(bsls::Types::Int64),
//                 sizeof(int));
//..
// Now, we set up the handler guard that installs the precondition handlers.
//..
//          bsls::FuzzTestHandlerGuard hg;
//..
// Finally, we invoke the function under test with the
// 'BSLS_FUZZTEST_EVALUATE_RAW' macro.
//..
//          Timer t;
//          BSLS_FUZZTEST_EVALUATE_RAW(t.start(seconds, nanoseconds));
//..
// If the total number of seconds resulting from the sum of 'seconds' and
// 'nanoseconds' cannot be represented with a 64-bit signed integer, a
// top-level assertion failure from a different component will occur.  Because
// we have invoked 'start' with the 'RAW' macro, a component name check will
// not be performed, and execution will continue.
//..
//        } break;
//        default: {
//        } break;
//      }
//
//      if (testStatus > 0) {
//          BSLS_ASSERT_INVOKE("FUZZ TEST FAILURES");
//      }
//
//      return 0;
//  }
//..
// Note that the use of 'bslim::FuzzUtil' and 'bslim::FuzzDataView' can
// simplify the consumption of fuzz data.

#include <bsls_assert.h>
#include <bsls_fuzztestpreconditionexception.h>
#include <bsls_preconditions.h>

                             // =================
                             // Macro Definitions
                             // =================

#if defined(BDE_BUILD_TARGET_EXC)
#define BSLS_FUZZTEST_EVALUATE_IMP(X) do {                                    \
        try {                                                                 \
            BloombergLP::bsls::FuzzTestPreconditionTracker::initStaticState(  \
                                                                   __FILE__); \
            X;                                                                \
        }                                                                     \
        catch (BloombergLP::bsls::FuzzTestPreconditionException& ftpe) {      \
            BloombergLP::bsls::FuzzTestPreconditionTracker::handleException(  \
                                                                       ftpe); \
        }                                                                     \
    } while (false)

#define BSLS_FUZZTEST_EVALUATE_RAW_IMP(X) do {                                \
        try {                                                                 \
            BloombergLP::bsls::FuzzTestPreconditionTracker::initStaticState(  \
                                                                   __FILE__); \
            X;                                                                \
        }                                                                     \
        catch (BloombergLP::bsls::FuzzTestPreconditionException& ftpe) {      \
        }                                                                     \
    } while (false)

#else
#define BSLS_FUZZTEST_EVALUATE_IMP(X) do {                                    \
        X;                                                                    \
    } while (false)
#define BSLS_FUZZTEST_EVALUATE_RAW_IMP(X) do {                                \
        X;                                                                    \
    } while (false)
#endif  // defined(BDE_BUILD_TARGET_EXC)

#ifdef BDE_ACTIVATE_FUZZ_TESTING

#define BSLS_FUZZTEST_EVALUATE(X) BSLS_FUZZTEST_EVALUATE_IMP(X)

#define BSLS_FUZZTEST_EVALUATE_RAW(X) BSLS_FUZZTEST_EVALUATE_RAW_IMP(X)

#else
#define BSLS_FUZZTEST_EVALUATE(X) do {                                        \
        X;                                                                    \
    } while (false)

#define BSLS_FUZZTEST_EVALUATE_RAW(X) do {                                    \
        X;                                                                    \
    } while (false)

#endif  // defined(BDE_ACTIVATE_FUZZ_TESTING)
namespace BloombergLP {
namespace bsls {

                         // =================================
                         // class FuzzTestPreconditionTracker
                         // =================================

struct FuzzTestPreconditionTracker {
    // This utility class is used by the preprocessor macros to appropriately
    // handle precondition violations that occur in different levels and
    // components.

  private:
    // CLASS DATA
    static const char *s_file_p;        // filename passed to 'initStaticState'

    static bool
        s_isInFirstPreconditionBlock;   // flag indicating whether the first
                                        // 'BEGIN/END' block has been closed

    static int         s_level;         // nesting level of 'BEGIN'/'END' call

  public:
    // CLASS METHODS
    static void handleException(
                               const FuzzTestPreconditionException& exception);
        // Invoke the assertion handler returned by
        // 'FuzzTestHandlerGuard::getOriginalAssertionHandler' if the assertion
        // violation wrapped by the specified 'exception' was encountered in a
        // component different from one supplied to 'initStaticState', and do
        // nothing otherwise.

    static void handlePreconditionViolation(const AssertViolation& violation);
        // Throw a 'FuzzTestPreconditionException' constructed from the
        // specified 'violation' if the assertion violation occurred after the
        // first invocation of 'handlePreconditionsBegin' but before the first
        // invocation of 'handlePreconditionsEnd', and invoke the assertion
        // handler returned by
        // 'FuzzTestHandlerGuard::getOriginalAssertionHandler' otherwise.

    static void handlePreconditionsBegin();
        // Increment the assertion block depth level counter.

    static void handlePreconditionsEnd();
        // Decrement the assertion block depth level counter and record that
        // the first precondition block has ended if the depth level changed to
        // 0.  The behavior is undefined unless the depth level is positive.

    static void initStaticState(const char *fileName);
        // Store the specified 'fileName' from the caller that invokes the
        // top-level function under test (via 'BSLS_FUZZTEST_EVALUATE(X)'), and
        // set the state to reflect that any precondition begin macro
        // encountered will be the first.
};

                         // ==========================
                         // class FuzzTestHandlerGuard
                         // ==========================

class FuzzTestHandlerGuard {
    // This class provides a guard that will install and uninstall three
    // handlers, one for assertion failure, one for 'BSLS_PRECONDITIONS_BEGIN',
    // and one for 'BSLS_PRECONDITIONS_END', within the protected scope.

  private:
    // CLASS DATA
    static Assert::ViolationHandler
        s_originalAssertionHandler;  // original assertion handler

  public:
    // CREATORS
    FuzzTestHandlerGuard();
        // Create a guard object, installing
        // 'FuzzTestPreconditionTracker::handlePreconditionViolation' as well
        // as the 'BEGIN/END' handler.  The behavior is undefined if the
        // current assertion handler is
        // 'FuzzTestPreconditionTracker::handlePreconditionViolation'.

    ~FuzzTestHandlerGuard();
        // Restore the failure handler that was in place when this object was
        // created, reset the precondition 'BEGIN/END' handlers to no-op, and
        // destroy this guard.

    // CLASS METHODS
    static Assert::ViolationHandler getOriginalAssertionHandler();
        // Return the original assertion handler.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

inline
FuzzTestHandlerGuard::FuzzTestHandlerGuard()
{
    BSLS_ASSERT(&FuzzTestPreconditionTracker::handlePreconditionViolation !=
                bsls::Assert::violationHandler());

    PreconditionsHandler::installHandlers(
        &FuzzTestPreconditionTracker::handlePreconditionsBegin,
        &FuzzTestPreconditionTracker::handlePreconditionsEnd);

    s_originalAssertionHandler = bsls::Assert::violationHandler();

    bsls::Assert::setViolationHandler(
        &FuzzTestPreconditionTracker::handlePreconditionViolation);
}

inline
FuzzTestHandlerGuard::~FuzzTestHandlerGuard()
{
    PreconditionsHandler::installHandlers(&PreconditionsHandler::noOpHandler,
                                          &PreconditionsHandler::noOpHandler);
    bsls::Assert::setViolationHandler(s_originalAssertionHandler);
}

inline
Assert::ViolationHandler FuzzTestHandlerGuard::getOriginalAssertionHandler()
{
    return s_originalAssertionHandler;
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2021 Bloomberg Finance L.P.
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
