// bdlt_datetimeimputil.t.cpp                                         -*-C++-*-
#include <bdlt_datetimeimputil.h>

#include <bdlt_date.h>
#include <bdlt_timeunitratio.h>

#include <bslim_testutil.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                              TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test implements a single utility 'struct', that defines
// a set of constants.  There is no executable code.  Therefore the component
// can be tested in a single test case that simply checks the values defined in
// the 'struct'.
// ----------------------------------------------------------------------------
// [ 1] static const bsls::Types::Int64  k_1970_01_01_TOTAL_SECONDS;
// [ 1] static const bsls::Types::Int64  k_2470_01_01_TOTAL_SECONDS;
// [ 1] static const bsls::Types::Uint64 k_0001_01_01_VALUE;
// [ 1] static const bsls::Types::Uint64 k_1970_01_01_VALUE;
// [ 1] static const bsls::Types::Uint64 k_2470_01_01_VALUE;
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
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int  test        = argc > 1 ? atoi(argv[1]) : 0;
    const bool verbose     = argc > 2;
    const bool veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'DatetimeImpUtil' VALUES
        //
        // Concerns:
        //: 1 That each value defined in 'DatetimeImpUtil' is correct.
        //
        // Plan:
        //: 1 Using brute force, compare each explicit value defined in
        //:   'DatetimeImpUtil' to a value calculated using 'Date' and
        //:   'TimeUnitRatio'.  (C-1)
        //
        // Testing:
        //   static const bsls::Types::Int64  k_1970_01_01_TOTAL_SECONDS;
        //   static const bsls::Types::Int64  k_2470_01_01_TOTAL_SECONDS;
        //   static const bsls::Types::Uint64 k_0001_01_01_VALUE;
        //   static const bsls::Types::Uint64 k_1970_01_01_VALUE;
        //   static const bsls::Types::Uint64 k_2470_01_01_VALUE;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'DatetimeImpUtil' VALUES" << endl
                          << "================================" << endl;

        ASSERT(bdlt::DatetimeImpUtil::k_1970_01_01_TOTAL_SECONDS
                              == (bdlt::Date(1970, 1, 1) - bdlt::Date(1, 1, 1))
                                             * bdlt::TimeUnitRatio::k_S_PER_D);

        ASSERT(bdlt::DatetimeImpUtil::k_2470_01_01_TOTAL_SECONDS
                              == (bdlt::Date(2470, 1, 1) - bdlt::Date(1, 1, 1))
                                             * bdlt::TimeUnitRatio::k_S_PER_D);

        ASSERT(bdlt::DatetimeImpUtil::k_0001_01_01_VALUE
                      == (bdlt::Date(9999, 12, 31) - bdlt::Date(1, 1, 1) + 1));

        ASSERT(bdlt::DatetimeImpUtil::k_1970_01_01_VALUE
                      == (bdlt::Date(1970, 1, 1) - bdlt::Date(1, 1, 1))
                                  * bdlt::TimeUnitRatio::k_US_PER_D
                                  + bdlt::DatetimeImpUtil::k_0001_01_01_VALUE);

        ASSERT(bdlt::DatetimeImpUtil::k_2470_01_01_VALUE
                      == (bdlt::Date(2470, 1, 1) - bdlt::Date(1970, 1, 1))
                                  * bdlt::TimeUnitRatio::k_NS_PER_D
                                  + bdlt::DatetimeImpUtil::k_1970_01_01_VALUE);

        if (veryVerbose) {
            P(bdlt::DatetimeImpUtil::k_1970_01_01_TOTAL_SECONDS);
            P(bdlt::DatetimeImpUtil::k_2470_01_01_TOTAL_SECONDS);
            P(bdlt::DatetimeImpUtil::k_0001_01_01_VALUE);
            P(bdlt::DatetimeImpUtil::k_1970_01_01_VALUE);
            P(bdlt::DatetimeImpUtil::k_2470_01_01_VALUE);
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
// Copyright 2016 Bloomberg Finance L.P.
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
