// bsls_preconditions.t.cpp                                           -*-C++-*-
#include <bsls_preconditions.h>

#include <bsls_assert.h>
#include <bsls_bsltestutil.h>

#include <math.h>                 // sqrt
#include <stdio.h>
#include <stdlib.h>               // atoi(), rand()

using namespace BloombergLP;
using namespace std;

// ============================================================================
//                              TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test is intended to be used as part of fuzz testing
// narrow contract functions.  The two macros, 'BSLS_PRECONDITIONS_BEGIN' and
// 'BSLS_PRECONDITIONS_END' are defined to be empty if
// 'BDE_ACTIVATE_FUZZ_TESTING' is not defined.  The intended use is to place
// these macros before and after the preconditions of a function.
//
// ----------------------------------------------------------------------------
// [ 2] static void installHandlers(beginHandler, endHandler);
// [ 2] static PreconditionHandlerType getBeginHandler();
// [ 2] static PreconditionHandlerType getEndHandler();
// [ 2] static void invokeBeginHandler();
// [ 2] static void invokeEndHandler();
// ----------------------------------------------------------------------------
// [ 2] BSLS_PRECONDITIONS_BEGIN
// [ 2] BSLS_PRECONDITIONS_END
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] USAGE EXAMPLE
// ----------------------------------------------------------------------------

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
//                  HELPER CLASSES AND FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

struct TestPreconditionsBeginEnd
    // A helper 'struct' to facilitate the testing of callback functions.
{
    static int s_beginCount;
    static int s_endCount;

    static void handlePreconditionsBegin()
        // Increment 's_beginCount' by 1.
    {
        s_beginCount++;
    }

    static void handlePreconditionsEnd()
        // Increment 's_endCount' by 1.
    {
        s_endCount++;
    }

    static void reset()
        // Set the observable side-effect variables to '0'.
    {
        s_beginCount = 0;
        s_endCount = 0;
    }

    static void test()
        // Invoke 'BEGIN/END' macros and verify that the installed precondition
        // handlers were invoked.
    {
        bsls::PreconditionsHandler::installHandlers(&handlePreconditionsBegin,
                                                    &handlePreconditionsEnd);

        ASSERT(bsls::PreconditionsHandler::getBeginHandler() ==
               &handlePreconditionsBegin);
        ASSERT(bsls::PreconditionsHandler::getEndHandler() ==
               &handlePreconditionsEnd);

        {
            reset();
            BSLS_PRECONDITIONS_BEGIN_IMP();
            ASSERT(s_beginCount == 1);
            ASSERT(s_endCount == 0);
            BSLS_PRECONDITIONS_END_IMP();
            ASSERT(s_beginCount == 1);
            ASSERT(s_endCount == 1);
        }

        {
            reset();
            BSLS_PRECONDITIONS_BEGIN_IMP();
            ASSERT(s_beginCount == 1);
            ASSERT(s_endCount == 0);
            BSLS_PRECONDITIONS_END_IMP();
            ASSERT(s_beginCount == 1);
            ASSERT(s_endCount == 1);

            BSLS_PRECONDITIONS_BEGIN_IMP();
            ASSERT(s_beginCount == 2);
            ASSERT(s_endCount == 1);
            BSLS_PRECONDITIONS_END_IMP();
            ASSERT(s_beginCount == 2);
            ASSERT(s_endCount == 2);
        }

        {
            reset();
            BSLS_PRECONDITIONS_BEGIN_IMP();
            ASSERT(s_beginCount == 1);
            ASSERT(s_endCount == 0);
            {
                BSLS_PRECONDITIONS_BEGIN_IMP();
                ASSERT(s_beginCount == 2);
                ASSERT(s_endCount == 0);
                BSLS_PRECONDITIONS_END_IMP();
                ASSERT(s_beginCount == 2);
                ASSERT(s_endCount == 1);
            }
            BSLS_PRECONDITIONS_END_IMP();
            ASSERT(s_beginCount == 2);
            ASSERT(s_endCount == 2);
        }
    }
};

int TestPreconditionsBeginEnd::s_beginCount = 0;
int TestPreconditionsBeginEnd::s_endCount   = 0;

static void printFlags()
    // Print a diagnostic message to standard output if any of the preprocessor
    // flags of interest are defined, and their value if a value had been set.
    // An "Enter" and "Leave" message is printed unconditionally so there is
    // some report even if all of the flags are undefined.
{
    printf("printFlags: Enter\n");

    printf("\nprintFlags: bsls_preconditions Macros\n");

    printf("\nBSLS_PRECONDITIONS_BEGIN: ");
#ifdef BSLS_PRECONDITIONS_BEGIN
    printf("%s\n",
           STRINGIFY(BSLS_PRECONDITIONS_BEGIN()));
#else
    printf("UNDEFINED\n");
#endif

    printf("\nBSLS_PRECONDITIONS_END: ");
#ifdef BSLS_PRECONDITIONS_END
    printf("%s\n",
           STRINGIFY(BSLS_PRECONDITIONS_END()));
#else
    printf("UNDEFINED\n");
#endif

    printf("\n\nprintFlags: Leave\n");
}


//=============================================================================
//                               USAGE EXAMPLE
//-----------------------------------------------------------------------------
namespace usage_example {
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
    double mySqrt(double x)
        // Return the square root of the specified 'x'.  The behavior is
        // undefined unless 'x >= 0'.
    {
        BSLS_PRECONDITIONS_BEGIN();
        BSLS_ASSERT(0 <= x);
        BSLS_PRECONDITIONS_END();
        return sqrt(x);
    }
//..
// In a fuzz-enabled build, we would invoke this function inside the fuzz loop
// with 'BSLS_FUZZTEST_EVALUATE'.
}  // close namespace usage_example

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

    switch (test) { case 0:
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file must
        //:   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into driver, removing
        //    leading comment characters.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------
        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");
        usage_example::mySqrt(1);
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING MACROS AND PRECONDITIONSHANDLER
        //
        // Concerns:
        //: 1 That the handlers are installed via 'installHandlers' and invoked
        //:   correctly by 'BSLS_PRECONDITIONS_BEGIN' and
        //:   'BSLS_PRECONDITIONS_END' macros.
        //:
        //: 2 That 'getBeginHandler' and 'getEndHandler' return the previously
        //:   installed handlers.
        //
        // Plan:
        //: 1 Install custom 'BEGIN/END' handler functions using
        //:   'PreconditionsHandler::installHandlers'.  Then verify that the
        //:   handlers were invoked after calls to the macros.
        //:
        //: 2 Verify that the handlers were correctly installed.
        //
        // Testing:
        //   static void installHandlers(beginHandler, endHandler);
        //   BSLS_PRECONDITIONS_BEGIN
        //   BSLS_PRECONDITIONS_END
        //   static PreconditionHandlerType getBeginHandler();
        //   static PreconditionHandlerType getEndHandler();
        //   static void invokeBeginHandler();
        //   static void invokeEndHandler();
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING MACROS AND PRECONDITIONSHANDLER"
                            "\n=======================================\n");
        TestPreconditionsBeginEnd::test();
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

        if (verbose) printFlags();

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
