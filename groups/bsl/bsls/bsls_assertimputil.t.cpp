// bsls_assertimputil.t.cpp                                           -*-C++-*-
#include <bsls_assertimputil.h>

#include <bsls_bsltestutil.h>

#include <cstdio>    // 'fprintf'
#include <cstdlib>   // 'atoi'
#include <cstring>   // 'strcmp'
#include <exception> // 'exception'

using namespace std;
using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// 'bsls::AssertImpUtil' is a utility class, where each function will be tested
// in a separate test case.  Any significant test machinery will be tested
// before any function whose test case relies upon it.
//-----------------------------------------------------------------------------
// [-1] void AssertImpUtil::failAbort();
// [-2] void AssertImpUtil::failSleep();
//-----------------------------------------------------------------------------
// [ 1] USAGE EXAMPLE: myAbort
// [ 2] USAGE EXAMPLE: mySleep

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

//=============================================================================
//                    GLOBAL CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

bool globalVerbose         = false;
bool globalVeryVerbose     = false;
bool globalVeryVeryVerbose = false;

//=============================================================================
//                             USAGE EXAMPLES
//-----------------------------------------------------------------------------
///Usage
///-----
// This section illustrates the intended use of this component.
//
///Example 1: Aborting the Current Process
///- - - - - - - - - - - - - - - - - - - -
// Suppose you are implementing an assertion handler that should cause a
// process to terminate when invoked.  In order to stop the process
// immediately, you would call 'failAbort' like this:
//..
void myAbort()
{
    bsls::AssertImpUtil::failAbort();
    // This code should never be reached.
}
//..
// This function would then abort the current process.
//
///Example 2: Sleeping Forever
///- - - - - - - - - - - - - -
// Suppose you want a process to no longer continue doing anything, but you
// want to leave it running in order to attach a debugger to it and diagnose
// the full state of your system.  In order to have your process sleep forever,
// you might call 'failSleep' like this:
//..
void mySleep()
{
    bsls::AssertImpUtil::failSleep();
    // This code should never be reached.
}
//..
// This function would then sleep forever and never return.



//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int            test = argc > 1 ? atoi(argv[1]) : 0;
    int         verbose = argc > 2;
    int     veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

            globalVerbose =         verbose;
        globalVeryVerbose =     veryVerbose;
    globalVeryVeryVerbose = veryVeryVerbose;

    printf( "TEST %s CASE %d\n", __FILE__, test);

    switch (test) { case 0:  // zero is always the leading case
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE #2
        //
        // Concerns:
        //: 1 'mySleep' should compile, but must be manually tested.
        //
        // Plan:
        //: 1 See above for usage example code compiling.
        //:
        //: 2 See case -2 for manual testing of 'failSleep'.
        //
        // Testing:
        //   USAGE EXAMPLE: mySleep
        // --------------------------------------------------------------------

        if (verbose) printf( "\nUSAGE EXAMPLE #2"
                             "\n================\n" );

        // mySleep();

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE #1
        //
        // Concerns:
        //: 1 'myAbort' should compile, but must be manually tested.
        //
        // Plan:
        //: 1 See above for usage example code compiling.
        //:
        //: 2 See case -1 for manual testing of 'failAbort'.
        //
        // Testing:
        //   USAGE EXAMPLE: myAbort
        // --------------------------------------------------------------------

        if (verbose) printf( "\nUSAGE EXAMPLE #1"
                             "\n================\n" );

        // myAbort();

      } break;
      case -1: {
        // --------------------------------------------------------------------
        // CALL ABORT FUNCTION
        //
        // Concerns:
        //: 1 The 'failAbort' function should abort immediately; this can't be
        //:   tested by a normally run unit test.
        //
        // Plan:
        //: 1 Run 'failAbort' so the caller can manually witness that the test
        //:   has aborted.
        //
        // Testing:
        //   void AssertImpUtil::failAbort();
        // --------------------------------------------------------------------

        if (verbose) printf( "\nCALL ABORT FUNCTION"
                             "\n===================\n" );

        fprintf(stderr, "THIS TEST SHOULD NOW ABORT.\n");

        bsls::AssertImpUtil::failAbort();

        ASSERT(0 && "Should not be reached");

      } break;
      case -2: {
        // --------------------------------------------------------------------
        // CALL SLEEP FUNCTION
        //
        // Concerns:
        //: 1 The 'failSleep' function should sleep immediately; this can't be
        //:   tested by a normally run unit test.
        //
        // Plan:
        //: 1 Run 'failSleep' so the caller can manually witness that the test
        //:   has slept.
        //
        // Testing:
        //   void AssertImpUtil::failSleep();
        // --------------------------------------------------------------------

        if (verbose) printf( "\nCALL SLEEP FUNCTION"
                             "\n===================\n" );

        fprintf(stderr, "THIS TEST SHOULD NOW HANG INDEFINITELY.\n");

        bsls::AssertImpUtil::failSleep();

        ASSERT(0 && "Should not be reached");

      } break;
      default: {
          fprintf( stderr, "WARNING: CASE `%d` NOT FOUND.\n" , test);
          testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf( stderr, "Error, non-zero test status = %d.\n", testStatus );
    }

    return testStatus;
}
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
