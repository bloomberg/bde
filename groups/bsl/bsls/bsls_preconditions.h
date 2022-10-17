// bsls_preconditions.h                                               -*-C++-*-
#ifndef INCLUDED_BSLS_PRECONDITIONS
#define INCLUDED_BSLS_PRECONDITIONS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")


//@PURPOSE: Provide macros for use in fuzz testing narrow contract functions.
//
//@CLASSES:
// bsls::PreconditionsHandler: for begin/end callback management functions
//
//@MACROS:
//  BSLS_PRECONDITIONS_BEGIN: mark the start of function preconditions
//  BSLS_PRECONDITIONS_END: mark the end of function preconditions
//
//@SEE_ALSO: bsls_fuzztest
//
//@DESCRIPTION: This component provides macros, 'BSLS_PRECONDITIONS_BEGIN' and
// 'BSLS_PRECONDITIONS_END', to facilitate fuzz testing narrow contract
// functions.  When fuzz testing is not enabled, the macros expand to nothing.
// When fuzz testing is enabled, the macros invoke a dynamic handler function
// via 'bsls::PreconditionsHandler'.
//
// 'BSLS_PRECONDITIONS_BEGIN' is used as a marker to identify where
// precondition checks are begun, while 'BSLS_PRECONDITIONS_END' is used as a
// marker to identify where precondition checks are complete.  These macros
// should always be used as a pair, and always at the very beginning of a
// function, surrounding the function preconditions.
//
///Usage
///-----
// Since the macros contained in this component are intended to be used in
// conjunction with the macros defined in 'bsls_fuzztest', this test driver
// contains only the simplest USAGE EXAMPLE.  See the USAGE EXAMPLE in
// {'bsls_fuzztest'} for a fuller treatment.
//
// The following example shows the use of 'BSLS_PRECONDITIONS_BEGIN' and
// 'BSLS_PRECONDITIONS_END' in the definition of a narrow contract function.
// These macros are to be placed around the function precondition checks,
// immediately before and after.
//..
//  double mySqrt(double x)
//      // Return the square root of the specified 'x'.  The behavior is
//      // undefined unless 'x >= 0'.
//  {
//      BSLS_PRECONDITIONS_BEGIN();
//      BSLS_ASSERT(0 <= x);
//      BSLS_PRECONDITIONS_END();
//      return sqrt(x);
//  }
//..
// In a fuzz-enabled build, we would invoke this function inside the fuzz loop
// with 'BSLS_FUZZTEST_EVALUATE'.

#include <bsls_assert.h>
#include <bsls_atomicoperations.h> // 'AtomicTypes'
#include <bsls_consteval.h>
#include <bsls_pointercastutil.h>

                             // =================
                             // Macro Definitions
                             // =================
#define BSLS_PRECONDITIONS_BEGIN_IMP() do {                                   \
        bsls::PreconditionsHandler::invokeBeginHandler();                     \
    } while (false)

#define BSLS_PRECONDITIONS_END_IMP() do {                                     \
        bsls::PreconditionsHandler::invokeEndHandler();                       \
    } while (false)

#if defined(BDE_ACTIVATE_FUZZ_TESTING) &&                                     \
    defined(BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED_IS_ACTIVE)

    #define BSLS_PRECONDITIONS_BEGIN() do {                                   \
        if (!BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED) {                          \
            BSLS_PRECONDITIONS_BEGIN_IMP();                                   \
        }                                                                     \
    } while (false)

    #define BSLS_PRECONDITIONS_END() do {                                     \
        if (!BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED) {                          \
            BSLS_PRECONDITIONS_END_IMP();                                     \
        }                                                                     \
    } while (false)

#else  // fuzzing not enabled or 'consteval' not active

#define BSLS_PRECONDITIONS_BEGIN() do {} while(false)
#define BSLS_PRECONDITIONS_END() do {} while(false)

#endif

namespace BloombergLP {
namespace bsls {

                         // ===========================
                         // class PreconditionsHandler
                         // ===========================
class PreconditionsHandler {
    // This utility class maintains pointers containing the addresses of
    // functions invoked by the 'BSLS_PRECONDITIONS_BEGIN' and
    // 'BSLS_PRECONDITIONS_END' macros, and provides methods to
    //  manipulate and utilize those functions.

  private:
    // CLASS DATA
    static AtomicOperations::AtomicTypes::Pointer
        s_beginHandler;  // begin handler function
    static AtomicOperations::AtomicTypes::Pointer
        s_endHandler;    // end handler function

  public:
    // TYPES
    typedef void (*PreconditionHandlerType)();
        // 'PreconditionHandlerType' is an alias for a pointer to a function
        // returning 'void' and taking no parameters.

    // CLASS METHODS
    static
    PreconditionHandlerType getBeginHandler();
        // Return the previously installed 's_beginHandler'.

    static
    PreconditionHandlerType getEndHandler();
        // Return the previously installed 's_endHandler'.

    static void installHandlers(PreconditionHandlerType beginHandler,
                                PreconditionHandlerType endHandler);
        // Store the specified 'beginHandler' and 'endHandler' function
        // pointers to the 'static' member variables.

    static void invokeBeginHandler();
        // Invoke the previously installed 's_beginHandler' function.

    static void invokeEndHandler();
        // Invoke the previously installed 's_endHandler' function.

    static void noOpHandler();
        // Do nothing.
};

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
