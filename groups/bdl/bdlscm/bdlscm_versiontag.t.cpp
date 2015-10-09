// bdlscm_versiontag.t.cpp                                            -*-C++-*-

#include <bdlscm_versiontag.h>

#include <bslim_testutil.h>

#include <bsl_cstdlib.h>     // 'atoi'
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;

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
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

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
    bool verbose = argc > 2;
    bool veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

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

        if (verbose) cout << endl
                          << "TEST USAGE EXAMPLE" << endl
                          << "==================" << endl;

///Usage
///-----
// At compile time, the version of BDL can be used to select an older or newer
// way to accomplish a task, to enable new functionality, or to accommodate an
// interface change.  For example, if the name of a function changes (a rare
// occurrence, but potentially disruptive when it does happen), the impact on
// affected code can be minimized by conditionally calling the function by its
// old or new name using conditional compilation.  In the following, the '#if'
// preprocessor directive compares 'BDL_VERSION' (i.e., the latest BDL version,
// excluding the patch version) to a specified major and minor version composed
// using the 'BSL_MAKE_VERSION' macro:
//..
    #if BDL_VERSION > BSL_MAKE_VERSION(1, 3)
        // Call 'newFunction' for BDL versions later than 1.3.
        int result = newFunction();
    #else
        // Call 'oldFunction' for BDL version 1.3 or earlier.
        int result = oldFunction();
    #endif
//..
      } break;
      case 2: {
        //--------------------------------------------------------------------
        // TEST BDL_MAKE_VERSION MACRO
        //
        // Concerns:
        //   That BDL_MAKE_VERSION create a compile-time constant if its
        //   arguments are all compile-time constants.
        //   That BDL_MAKE_VERSION correctly composes a major, minor, and
        //   patch version number into a single integer.  Each component can
        //   be in the range 0-99.
        //
        // Plan:
        //   Use the result of BDL_MAKE_VERSION as an array dimension to
        //   prove that it is a compile-time constant.
        //   Using ad-hoc data selection, create a number of version values
        //   using the 'BDL_MAKE_VERSION' macro and verify that the expected
        //   value matches the actual value.
        //
        // Testing:
        //   BDL_MAKE_VERSION(major, minor)
        //--------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TEST BDL_MAKE_VERSION MACRO" << endl
                          << "===========================" << endl;

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
        //   That BDL_VERSION corresponds to the two components
        //   BDL_VERSION_MAJOR and BDL_VERSION_MINOR
        //
        // Plan:
        //   Decompose BDL_VERSION into its three components and verify
        //   that they correspond to the defined macros.
        //
        // Testing:
        //   BDL_VERSION
        //   BDL_VERSION_MAJOR
        //   BDL_VERSION_MINOR
        //--------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TEST VERSION CONSISTENCY" << endl
                          << "========================" << endl;

        int major = (BDL_VERSION / 10000) % 100;
        int minor = (BDL_VERSION / 100) % 100;

        ASSERT(BDL_VERSION_MAJOR == major);
        ASSERT(BDL_VERSION_MINOR == minor);

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
// Copyright 2012 Bloomberg Finance L.P.
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
