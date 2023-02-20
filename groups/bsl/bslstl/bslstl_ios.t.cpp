// bslstl_ios.t.cpp                                                   -*-C++-*-
#include <bslstl_ios.h>

#include <bslstl_istringstream.h>
#include <bslstl_ostringstream.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <stdio.h>
#include <stdlib.h>  // 'atoi'

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test mostly just imports 'std' names to the 'bsl'.  The
// only exception at the moment is the 'defaultfloat' manipulator.  It has been
// available only since C++11 but we provide it as an extension to C++03 mode
// as well.
// ----------------------------------------------------------------------------
// [ 1] ios_base& defaultfloat(ios_base& ios);

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

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

// ============================================================================
//                  PRINTF FORMAT MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

// ============================================================================
//                         OTHER MACROS
// ----------------------------------------------------------------------------

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
static bool verbose;
static bool veryVerbose;
static bool veryVeryVerbose;
static bool veryVeryVeryVerbose;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int            test = argc > 1 ? atoi(argv[1]) : 0;
                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'bsl::defaultfloat'
        //
        // Concerns:
        //: 1 Insertion of the 'bsl::defaultfloat' manipulator to ostream
        //:   ('<<') unsets the 'ios_base::scientific' & 'ios_base::scientific'
        //:   flags.
        //:
        //: 2 Extraction of the 'bsl::defaultfloat' manipulator from istream
        //:   ('>>') unsets the 'ios_base::scientific' & 'ios_base::scientific'
        //:   flags.
        //
        // Plan:
        //: 1 Ensure that 'floatfield == (scientific | fixed)'.
        //:
        //: 2 Create a default-constructed instance of any 'ostream' subclass
        //:   and verify that the 'floatfield' is unset.
        //:
        //: 3 Apply the 'scientific' manipulator and verify that the
        //:   'floatfield' flag is set (part of it).
        //:
        //: 4 Apply the 'defaultfloat' manipulator and verify that the
        //:   'floatfield' flag is unset.
        //:
        //: 5 Apply the 'fixed' manipulator and verify that the 'floatfield'
        //:   flag is set (part of it).
        //:
        //: 6 Apply the 'defaultfloat' manipulator and verify that the
        //:   'floatfield' flag is unset.
        //:
        //: 7 Repeat steps 2-6 for any 'istream' subclass.
        //
        // Testing:
        //   ios_base& defaultfloat(ios_base& ios);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'bsl::defaultfloat'"
                            "\n===========================\n");

        using bsl::ios_base;
        ASSERT(ios_base::floatfield ==
              (ios_base::scientific | ios_base::fixed));

        // Verify the insertion to 'ostream'
        {
            bsl::ostringstream os;
            ASSERT(!(os.flags() & ios_base::floatfield));

            // Set 'ios_base::scientific' flag
            os << bsl::scientific;
            ASSERT(os.flags() & ios_base::floatfield);

            // Clear 'ios_base::scientific' flag
            os << bsl::defaultfloat;
            ASSERT(!(os.flags() & ios_base::floatfield));

            // Set 'ios_base::fixed' flag
            os << bsl::fixed;
            ASSERT(os.flags() & ios_base::floatfield);

            // Clear 'ios_base::fixed' flag
            os << bsl::defaultfloat;
            ASSERT(!(os.flags() & ios_base::floatfield));
        }

        // Verify the extraction from 'istream'
        {
            bsl::istringstream is;
            ASSERT(!(is.flags() & ios_base::floatfield));

            // Set 'ios_base::scientific' flag
            is >> bsl::scientific;
            ASSERT(is.flags() & ios_base::floatfield);

            // Clear 'ios_base::scientific' flag
            is >> bsl::defaultfloat;
            ASSERT(!(is.flags() & ios_base::floatfield));

            // Set 'ios_base::fixed' flag
            is >> bsl::fixed;
            ASSERT(is.flags() & ios_base::floatfield);

            // Clear 'ios_base::fixed' flag
            is >> bsl::defaultfloat;
            ASSERT(!(is.flags() & ios_base::floatfield));
        }
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-Zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2023 Bloomberg Finance L.P.
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
