// balst_stacktraceconfigurationutil.t.cpp                            -*-C++-*-

#include <balst_stacktraceconfigurationutil.h>

#include <bslim_testutil.h>

#include <bsl_iostream.h>

#include <bsl_cstdlib.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
// [ 2] isResolutionDisabled();
// [ 2] disableResolution();
// [ 2] enableResolution();
// ----------------------------------------------------------------------------
// [ 3] USAGE EXAMPLE
// [ 1] BREATHING TEST
// ----------------------------------------------------------------------------

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
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
//                        GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef balst::StackTraceConfigurationUtil   Util;

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test    = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int verbose = argc > 2;

    switch (test) { case 0:
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 That the usage example compiles and works.
        //
        // Plan:
        //: 1 Compile and run the exact code in the usage example.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "USAGE EXAMPLE\n"
                             "=============\n";
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Evaluating Boolean Value
// - - - - - - - - - - - - - - - - - -
// If neither 'enableResolution' nor 'disableResolution' have been called, the
// default value of 'isResolutionDisabled' is 'false'.
//..
    ASSERT(false ==
                   balst::StackTraceConfigurationUtil::isResolutionDisabled());
//..
// After that, the value tracks whether 'disableResolution' or
// 'enableResolution' has been called.
//..
    balst::StackTraceConfigurationUtil::disableResolution();

    ASSERT(true == balst::StackTraceConfigurationUtil::isResolutionDisabled());

    balst::StackTraceConfigurationUtil::enableResolution();

    ASSERT(false ==
                   balst::StackTraceConfigurationUtil::isResolutionDisabled());
//..
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // ALL TRANSITIONS TEST
        //
        // Concerns:
        //: 1 Test the default value.
        //:
        //: 2 Test all possible transitions of the state.
        //
        // Plan:
        //: 1 Test the default value.
        //:
        //: 2 Test all possible transitiions.
        //
        // Testing:
        //   isResolutionDisabled();
        //   disableResolution();
        //   enableResolution();
        // --------------------------------------------------------------------

        if (verbose) cout << "ALL TRANSITIONS TEST\n"
                             "====================\n";

        // Default value.

        ASSERT(! Util::isResolutionDisabled());
        ASSERT(! Util::isResolutionDisabled());
        ASSERT(! Util::isResolutionDisabled());

        // enabled -> enabled

        Util::enableResolution();
        ASSERT(! Util::isResolutionDisabled());
        ASSERT(! Util::isResolutionDisabled());
        ASSERT(! Util::isResolutionDisabled());

        Util::enableResolution();
        ASSERT(! Util::isResolutionDisabled());
        ASSERT(! Util::isResolutionDisabled());
        ASSERT(! Util::isResolutionDisabled());

        // enabled -> disabled

        Util::disableResolution();
        ASSERT(Util::isResolutionDisabled());
        ASSERT(Util::isResolutionDisabled());
        ASSERT(Util::isResolutionDisabled());

        // disabled -> disabled

        Util::disableResolution();
        ASSERT(Util::isResolutionDisabled());
        ASSERT(Util::isResolutionDisabled());
        ASSERT(Util::isResolutionDisabled());

        // disabled -> enabled

        Util::enableResolution();
        ASSERT(! Util::isResolutionDisabled());
        ASSERT(! Util::isResolutionDisabled());
        ASSERT(! Util::isResolutionDisabled());
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Randomly call 'enable' and 'disable' and observe 'isEnabled'.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "BREATHING TEST\n"
                             "==============\n";

        ASSERT(! Util::isResolutionDisabled());
        ASSERT(! Util::isResolutionDisabled());
        ASSERT(! Util::isResolutionDisabled());
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
