// bslim_gtestutil.t.cpp                                              -*-C++-*-
#include <bslim_gtestutil.h>

#include <bslim_testutil.h>

#include <bslma_testallocator.h>

#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_limits.h>
#include <bsl_sstream.h>

// Do not 'using' any namespaces.  We want to verify that everything works from
// the global namespace.

using bsl::cout;
using bsl::cerr;
using bsl::flush;
using bsl::endl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// Test this thing.
// ----------------------------------------------------------------------------
// [ 2] INDIVIDUAL CHARACTER TESTING
// ----------------------------------------------------------------------------
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

#define Q            BSLIM_TESTUTIL_Q     // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P     // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_    // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_    // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_    // current Line number

// ============================================================================
//            GLOBAL TYPES, CONSTANTS, AND VARIABLES FOR TESTING
// ----------------------------------------------------------------------------

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = (argc > 1) ? bsl::atoi(argv[1]) : 1;
    bool             verbose = (argc > 2); (void)verbose;
    bool         veryVerbose = (argc > 3); (void)veryVerbose;
    bool     veryVeryVerbose = (argc > 4); (void)veryVeryVerbose;
    bool veryVeryVeryVerbose = (argc > 5); (void)veryVeryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    BloombergLP::bslma::TestAllocator ta("test", veryVeryVeryVerbose);

    switch (test) { case 0:
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 Demonstrate the usage of this component.
        //
        // Plan:
        //: 1 Using 'PrintTo' to output a string.
        // --------------------------------------------------------------------

        if (verbose) cout << "USAGE EXAMPLE\n"
                             "=============\n";

///Usage
///-----
// Suppose we have a string 'str' that we want to output:
//..
    bsl::string str =
                   "No matter where you go, There you are! -- Buckaroo Banzai";
//..
// Call 'PrintTo', passing the string and a pointer to a 'bsl::ostream':
//..
    PrintTo(str, &cout);
    cout << endl;
//..
// Which results in the string being streamed to standard output, surrounded by
// double quotes:
//..
//  "No matter where you go, There you are! -- Buckaroo Banzai"
//..
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 That the function can stream a string.
        //:
        //: 2 If the stream is invalid, the function will not modify the
        //:   stream.
        //
        // Plan:
        //: 1 Stream a couple of strings to an 'ostringtream' and observe the
        //:   result.
        //:
        //: 2 Set the state of the 'ostringstream' to invalid, call the
        //:   function again, and observe that the stream has not been
        //:   modified.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                             "\n==============\n";

        bsl::ostringstream oss(&ta);
        bsl::string s("hello, world.\n", &ta);

        PrintTo(s, &oss);
        ASSERT(oss.good());
        bsl::string expected = '"' + s + '"';
        ASSERTV(oss.str(), expected, oss.str() == expected);

        oss.str("");
        s.assign("The rain in Spain falls mainly in the plain.\n");
        PrintTo(s, &oss);
        ASSERT(oss.good());
        expected = '"' + s + '"';
        ASSERTV(oss.str(), expected, oss.str() == expected);

        oss.str("");
        oss << "woof";
        ASSERTV(oss.str(), oss.str() == "woof");
        ASSERT(oss.good());
        oss.setstate(bsl::ios_base::failbit);
        ASSERT(!oss.good());
        PrintTo(s, &oss);
        ASSERTV(oss.str(), oss.str() == "woof");
        ASSERT(!oss.good());
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
