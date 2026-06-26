// ball_thresholddefaults.t.cpp                                       -*-C++-*-
#include <ball_thresholddefaults.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_iostream.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// The component under test is a namespace for 4 integral constants and a
// callback function type.
//
//-----------------------------------------------------------------------------
// [ 1] CONSTANTS
//-----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE
//-----------------------------------------------------------------------------

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define NL "\n"

// ============================================================================
//                             USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Example 1: Accessing Default Threshold Values
///- - - - - - - - - - - - - - - - - - - - - - -
// The following example demonstrates how to access the default threshold
// values provided by 'ball::ThresholdDefaults'.
//
// Suppose we are implementing a function that initializes threshold levels
// for a logging system.  We can use the constants provided by
// 'ball::ThresholdDefaults' to supply reasonable default values:
// ```
void initializeThresholds(int *recordLevel,
                          int *passLevel,
                          int *triggerLevel,
                          int *triggerAllLevel)
    // Initialize the specified threshold levels to their default values.
{
    *recordLevel     = ball::ThresholdDefaults::k_RECORD_LEVEL;
    *passLevel       = ball::ThresholdDefaults::k_PASS_LEVEL;
    *triggerLevel    = ball::ThresholdDefaults::k_TRIGGER_LEVEL;
    *triggerAllLevel = ball::ThresholdDefaults::k_TRIGGER_ALL_LEVEL;
}
// ```

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int        test = argc > 1 ? atoi(argv[1]) : 0;
    int     verbose = argc > 2;
    int veryVerbose = argc > 3;  (void)veryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        // 1. The usage example provided in the component header file compiles,
        //    links, and runs as shown.
        //
        // Plan:
        // 1. Incorporate usage example from header into test driver, remove
        //    leading comment characters, and replace 'assert' with 'ASSERT'.
        //    (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "USAGE EXAMPLE\n"
                             "=============\n";

///Example 1: Accessing Default Threshold Values
///- - - - - - - - - - - - - - - - - - - - - - -
// The following example demonstrates how to access the default threshold
// values provided by 'ball::ThresholdDefaults'.
//
// Suppose we are implementing a function that initializes threshold levels
// for a logging system.  We can use the constants provided by
// 'ball::ThresholdDefaults' to supply reasonable default values:
// ```
//  void initializeThresholds(int *recordLevel,
//                            int *passLevel,
//                            int *triggerLevel,
//                            int *triggerAllLevel)
//      // Initialize the specified threshold levels to their default values.
//  {
//      *recordLevel     = ball::ThresholdDefaults::k_RECORD_LEVEL;
//      *passLevel       = ball::ThresholdDefaults::k_PASS_LEVEL;
//      *triggerLevel    = ball::ThresholdDefaults::k_TRIGGER_LEVEL;
//      *triggerAllLevel = ball::ThresholdDefaults::k_TRIGGER_ALL_LEVEL;
//  }
// ```
// Then, we can call this function and verify it sets the values correctly:
// ```
        int recordLevel, passLevel, triggerLevel, triggerAllLevel;

        initializeThresholds(&recordLevel,
                             &passLevel,
                             &triggerLevel,
                             &triggerAllLevel);
// ```
// Finally, the default values provide a reasonable starting configuration
// where 'k_RECORD_LEVEL' (`e_OFF`, 0) disables recording of messages to
// memory, 'k_PASS_LEVEL' (`e_ERROR`, 64) passes only ERROR level messages and
// above, 'k_TRIGGER_LEVEL' (`e_OFF`, 0) disables trigger of publication on
// single message, and 'k_TRIGGER_ALL_LEVEL' (`e_OFF`, 0) disables trigger of
// publication on all messages:
// ```
        ASSERT(ball::Severity::e_OFF   == recordLevel   );
        ASSERT(ball::Severity::e_ERROR == passLevel      );
        ASSERT(ball::Severity::e_OFF   == triggerLevel   );
        ASSERT(ball::Severity::e_OFF   == triggerAllLevel);
// ```
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // CONSTANTS
        //
        // Concerns:
        // 1. 'k_RECORD_LEVEL' has the expected value (0).
        //
        // 2. 'k_PASS_LEVEL' has the expected value (64).
        //
        // 3. 'k_TRIGGER_LEVEL' has the expected value (0).
        //
        // 4. 'k_TRIGGER_ALL_LEVEL' has the expected value (0).
        //
        // Plan:
        // 1. Verify each constant has its expected value.  (C-1..4)
        //
        // Testing:
        //   k_RECORD_LEVEL
        //   k_PASS_LEVEL
        //   k_TRIGGER_LEVEL
        //   k_TRIGGER_ALL_LEVEL
        // --------------------------------------------------------------------

        if (verbose) cout << "CONSTANTS\n"
                             "=========\n";

        ASSERT(0  == ball::ThresholdDefaults::k_RECORD_LEVEL);
        ASSERT(64 == ball::ThresholdDefaults::k_PASS_LEVEL);
        ASSERT(0  == ball::ThresholdDefaults::k_TRIGGER_LEVEL);
        ASSERT(0  == ball::ThresholdDefaults::k_TRIGGER_ALL_LEVEL);

        if (verbose) {
            P(ball::ThresholdDefaults::k_RECORD_LEVEL);
            P(ball::ThresholdDefaults::k_PASS_LEVEL);
            P(ball::ThresholdDefaults::k_TRIGGER_LEVEL);
            P(ball::ThresholdDefaults::k_TRIGGER_ALL_LEVEL);
        }
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
