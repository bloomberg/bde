// balscm_versiontag.t.cpp                                            -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <balscm_versiontag.h>

#include <bsl_iostream.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstdio.h>

#include <bslim_testutil.h>

using namespace BloombergLP;
using namespace bsl;

static bool verbose = false;
static bool veryVerbose = false;

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

// ============================================================================
//                      STANDARD BDE TEST DRIVER MACROS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define Q   BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P   BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_  BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                      USAGE EXAMPLE HELPER FUNCTIONS
// ----------------------------------------------------------------------------

int newFunction()
    // Return 1
{
    return 1;
}

// int OldFunction()
// Not defined and never called due to conditional compilation

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;

    verbose = (argc > 2);
    veryVerbose = (argc > 3);

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 2: {
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

        if (verbose) cout << endl
                          << "TEST USAGE EXAMPLE" << endl
                          << "==================" << endl;

///Usage
///-----
// At compile time, the version of BAL can be used to select an older or newer
// way to accomplish a task, to enable new functionality, or to accommodate an
// interface change.  For example, if the name of a function changes (a rare
// occurrence, but potentially disruptive when it does happen), the impact on
// affected code can be minimized by conditionally calling the function by its
// old or new name using conditional compilation.  In the following, the '#if'
// preprocessor directive compares 'BAL_VERSION' (i.e., the latest BAL version,
// excluding the patch version) to a specified major and minor version composed
// using the 'BSL_MAKE_VERSION' macro:
//..
    #if BAL_VERSION > BSL_MAKE_VERSION(1, 3)
        // Call 'newFunction' for BAL versions later than 1.3.
        int result = newFunction();
    #else
        // Call 'oldFunction' for BAL version 1.3 or earlier.
        int result = oldFunction();
    #endif
//..

      } break;

      case 1: {
        //--------------------------------------------------------------------
        // TEST VERSION CONSISTENCY
        //
        // Concerns:
        //   That BAL_VERSION corresponds to the three components,
        //   BAL_VERSION_MAJOR, BAL_VERSION_MINOR, and BAL_VERSION_PATCH.
        //
        // Plan:
        //   Decompose BAL_VERSION into its three components and verify
        //   that they correspond to the defined macros.
        //
        // Testing:
        //   BAL_VERSION
        //   BAL_VERSION_MAJOR
        //   BAL_VERSION_MINOR
        //   BAL_VERSION_PATCH
        //--------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TEST VERSION CONSISTENCY" << endl
                          << "========================" << endl;

        int major = (BAL_VERSION / 10000) % 100;
        int minor = (BAL_VERSION / 100) % 100;
        int patch = BAL_VERSION % 100;

        ASSERT(BAL_VERSION_MAJOR == major);
        ASSERT(BAL_VERSION_MINOR == minor);

      } break;

      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
