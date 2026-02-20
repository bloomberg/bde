// ball_recordformattertimezone.t.cpp                                 -*-C++-*-
#include <ball_recordformattertimezone.h>

#include <bslim_testutil.h>

#include <bsl_cstdlib.h>                      // atoi()
#include <bsl_iostream.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test defines an enumeration.
//-----------------------------------------------------------------------------
// [2] USAGE EXAMPLE
// [1] BREATHING TEST
//-----------------------------------------------------------------------------

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
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef ball::RecordFormatterTimezone Obj;

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int  test        = argc > 1 ? bsl::atoi(argv[1]) : 0;
    bool verbose     = argc > 2;
    bool veryVerbose = argc > 3;  (void)veryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        // 1. The usage example provided in the component header file compiles,
        //    links, and runs as shown.
        //
        // Plan:
        // 1. Incorporate usage example from header into test driver, remove
        //    leading comment characters, and replace `assert` with `ASSERT`.
        //    (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nUSAGE EXAMPLE"
                          << "\n=============" << endl;

///Example 1: Selecting Timestamp Timezone for Logging
/// - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we are implementing a logging system and need to allow users to
// configure whether timestamps should be rendered in UTC or local time.  We
// can use `ball::RecordFormatterTimezone::Enum` to represent this choice.
//
// First, we define a variable to hold the configured timezone setting,
// defaulting to UTC:
// ```
   ball::RecordFormatterTimezone::Enum timezone =
                                          ball::RecordFormatterTimezone::e_UTC;
// ```
// Then, based on user configuration, we might change this to local time:
// ```
   bool useLocalTime = true;  // from user configuration

   if (useLocalTime) {
       timezone = ball::RecordFormatterTimezone::e_LOCAL;
   }
// ```
// Finally, we can use this value to control timestamp formatting:
// ```
   if (ball::RecordFormatterTimezone::e_LOCAL == timezone) {
       // Format timestamp in local time
   }
   else {
       // Format timestamp in UTC
   }
// ```
// This allows timestamps in log records to be rendered in either UTC (useful
// for distributed systems and log aggregation) or local time (useful for
// local debugging and human readability).
// ```
        ASSERT(ball::RecordFormatterTimezone::e_LOCAL == timezone);
// ```
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        // 1. The class is sufficiently functional to enable comprehensive
        //    testing in subsequent test cases.
        //
        // Plan:
        // 1. Create an instance of each enumerator.
        // 2. Verify the enumerators have distinct values.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                          << "\n==============" << endl;

        if (verbose) cout << "\n 1. Create instances of each enumerator.\n";

        const Obj::Enum utc   = Obj::e_UTC;
        const Obj::Enum local = Obj::e_LOCAL;

        if (verbose) cout << "\n 2. Verify enumerators are distinct.\n";

        ASSERT(utc != local);

        if (verbose) cout << "\n 3. Verify values can be assigned.\n";

        Obj::Enum value = Obj::e_UTC;
        ASSERT(Obj::e_UTC == value);

        value = Obj::e_LOCAL;
        ASSERT(Obj::e_LOCAL == value);
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      } break;
    }
    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2025 Bloomberg Finance L.P.
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
