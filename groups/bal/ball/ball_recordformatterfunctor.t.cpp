// ball_recordformatterfunctor.t.cpp                                  -*-C++-*-
#include <ball_recordformatterfunctor.h>

#include <ball_record.h>
#include <ball_recordattributes.h>

#include <bslim_testutil.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test defines a struct containing a single typedef.
//-----------------------------------------------------------------------------
// [ 1] typedef ... Type;
//-----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE
// [ 1] BREATHING TEST
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

typedef ball::RecordFormatterFunctor Obj;

//=============================================================================
//                         HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

void myFormatter(bsl::ostream& stream, const ball::Record& record)
    // Write a simple representation of the specified 'record' to the
    // specified 'stream'.
{
    stream << "Log: " << record.fixedFields().message();
}

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

///Example 1: Defining a Record Formatter Function
/// - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to define a simple function that formats log records.  We
// can use `ball::RecordFormatterFunctor::Type` to declare a functor that
// conforms to the expected signature.
//
// First, we include the necessary headers and define a simple formatting
// function (defined at file scope above as `myFormatter`):
// ```
//  void myFormatter(bsl::ostream& stream, const ball::Record& record)
//  {
//      stream << "Log: " << record.fixedFields().message();
//  }
// ```
// Then, we can assign this function to a variable of the functor type:
// ```
   ball::RecordFormatterFunctor::Type formatter = &myFormatter;
// ```
// Finally, we can use this functor to format a record:
// ```
   ball::RecordAttributes attr;
   attr.setMessage("Hello");
   ball::Record record(attr, ball::UserFields());

   bsl::ostringstream oss;
   formatter(oss, record);
   ASSERT(oss.str() == "Log: Hello");
// ```
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        // 1. The typedef can be used to declare a functor.
        // 2. A function with the correct signature can be assigned to it.
        // 3. The functor can be invoked.
        //
        // Plan:
        // 1. Declare a variable of the typedef'd type.
        // 2. Assign a function to it.
        // 3. Invoke the functor and verify output.
        //
        // Testing:
        //   typedef ... Type;
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                          << "\n==============" << endl;

        if (verbose) cout << "\n 1. Declare a functor variable.\n";

        Obj::Type formatter;
        ASSERT(!formatter);  // default constructed is empty

        if (verbose) cout << "\n 2. Assign a function to it.\n";

        formatter = &myFormatter;
        ASSERT(formatter);   // now it's set

        if (verbose) cout << "\n 3. Invoke the functor.\n";

        ball::RecordAttributes attr;
        attr.setMessage("Test message");
        ball::Record record(attr, ball::UserFields());

        bsl::ostringstream oss;
        formatter(oss, record);

        ASSERT(oss.str() == "Log: Test message");

        if (verbose) cout << "Output: " << oss.str() << endl;
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
