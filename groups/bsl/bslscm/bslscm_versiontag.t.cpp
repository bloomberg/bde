// bslscm_versiontag.t.cpp                                            -*-C++-*-

#include <bslscm_versiontag.h>

#include <bsls_bsltestutil.h>

#include <stdio.h>      // 'printf', 'fprintf'
#include <stdlib.h>     // 'atoi'

using namespace BloombergLP;

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
//                  USAGE EXAMPLE HELPER FUNCTIONS
//-----------------------------------------------------------------------------

int newFunction()
    // Return 1
{
    return 1;
}

// int OldFunction()
// Not defined and never called due to conditional compilation

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void)veryVerbose;          // suppress warning
    (void)veryVeryVerbose;      // suppress warning
    (void)veryVeryVeryVerbose;  // suppress warning

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 3: {
        //--------------------------------------------------------------------
        // TEST USAGE EXAMPLE
        //
        // Concern:
        //   That the usage example in the user documentation compiles and
        //   runs as expected.
        //
        // Plan:
        //   Use the exact text of the usage example from the user
        //   documentation, but change uses of 'assert' to 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        //--------------------------------------------------------------------

        if (verbose) printf("\nTEST USAGE EXAMPLE"
                            "\n==================\n");

// At compile time, the version of BSL can be used to select an older or newer
// way to accomplish a task, to enable new functionality, or to accommodate an
// interface change.  For example, if a function changed names (a rare
// occurrence, but disruptive when it does happen), disruption can be minimized
// by conditionally calling the old or new function name using conditional
// compilation.  The '#if' directive compares 'BSL_VERSION' to a specified
// major, minor, and patch version 4 composed using 'BSL_MAKE_VERSION':
//..
    #if BSL_VERSION > BSL_MAKE_VERSION(1, 2)
        // Call 'newFunction' for BSL version 1.2 and later:
        int result = newFunction();
    #else
        // Call 'oldFunction' for BSL older than version 1.2:
        int result = oldFunction();
    #endif

        ASSERT(result);
//..
      } break;

      case 2: {
        //--------------------------------------------------------------------
        // TEST BSL_MAKE_VERSION MACRO
        //
        // Concerns:
        //   That BSL_MAKE_VERSION create a compile-time constant if its
        //   arguments are all compile-time constants.
        //   That BSL_MAKE_VERSION correctly composes a major, minor, and
        //   patch version number into a single integer.  Each component can
        //   be in the range 0-99.
        //
        // Plan:
        //   Use the result of BSL_MAKE_VERSION as an array dimension to
        //   prove that it is a compile-time constant.
        //   Using ad-hoc data selection, create a number of version values
        //   using the 'BSL_MAKE_VERSION' macro and verify that the expected
        //   value matches the actual value.
        //
        // Testing:
        //   BSL_MAKE_VERSION(major, minor)
        //--------------------------------------------------------------------

        if (verbose) printf("\nTEST BSL_MAKE_VERSION MACRO"
                            "\n===========================\n");
#if 0

        // Test that 'BSL_MAKE_VERSION(0,1,2)' is a compile-time constant.
        static const char COMPILE_ASSERT[BSL_MAKE_VERSION(0,1,2)] = { 0 };
        ASSERT(sizeof(COMPILE_ASSERT) == 102);

        static struct {
            int d_line;
            int d_major;
            int d_minor;
            int d_patch;
            int d_version;
        } const DATA[] = {
            //line major minor patch  version
            //---- ----- ----- -----  -------
            {  L_,    0,    0,    0,        0 },
            {  L_,    0,    0,    1,        1 },
            {  L_,    0,    1,    0,      100 },
            {  L_,    0,    1,    1,      101 },
            {  L_,    1,    0,    0,    10000 },
            {  L_,    1,    0,    1,    10001 },
            {  L_,    1,    1,    0,    10100 },
            {  L_,   12,   34,   56,   123456 },
            {  L_,   99,   99,   99,   999999 },
            {  L_,    9,    9,    9,    90909 },
            {  L_,   10,   20,   30,   102030 }
        };

        static const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int LINE  = DATA[i].d_line;
            const int MAJOR = DATA[i].d_major;
            const int MINOR = DATA[i].d_minor;
            const int PATCH = DATA[i].d_patch;
            const int EXP   = DATA[i].d_version;

            LOOP_ASSERT(LINE, EXP == BSL_MAKE_VERSION(MAJOR, MINOR, PATCH));
        }
#endif
        static const char COMPILE_ASSERT[BSL_MAKE_VERSION(0,1)] = { 0 };
        ASSERT(sizeof(COMPILE_ASSERT) == 100);

        static struct {
            int d_line;
            int d_major;
            int d_minor;
            int d_version;
        } const DATA[] = {
            //line major minor version
            //---- ----- ----- -------
            {  L_,    0,    0,       0 },
            {  L_,    0,    1,     100 },
            {  L_,    1,    0,   10000 },
            {  L_,    1,    1,   10100 },
            {  L_,   12,   34,  123400 },
            {  L_,   99,   99,  999900 },
            {  L_,    9,    9,   90900 },
            {  L_,   10,   20,  102000 }
        };

        static const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int LINE  = DATA[i].d_line;
            const int MAJOR = DATA[i].d_major;
            const int MINOR = DATA[i].d_minor;
            const int EXP   = DATA[i].d_version;

            LOOP_ASSERT(LINE, EXP == BSL_MAKE_VERSION(MAJOR, MINOR));
        }

      } break;

      case 1: {
        //--------------------------------------------------------------------
        // TEST VERSION CONSISTENCY
        //
        // Concerns:
        //   That BSL_VERSION corresponds to the two components
        //   BSL_VERSION_MAJOR and BSL_VERSION_MINOR
        //
        // Plan:
        //   Decompose BSL_VERSION into its three components and verify
        //   that they correspond to the defined macros.
        //
        // Testing:
        //   BSL_VERSION
        //   BSL_VERSION_MAJOR
        //   BSL_VERSION_MINOR
        //--------------------------------------------------------------------

        if (verbose) printf("\nTEST VERSION CONSISTENCY"
                            "\n========================\n");

        int major = (BSL_VERSION / 10000) % 100;
        int minor = (BSL_VERSION / 100) % 100;

        ASSERT(BSL_VERSION_MAJOR == major);
        ASSERT(BSL_VERSION_MINOR == minor);

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
