// bdlt_dayofweekutil.t.cpp                                           -*-C++-*-
#include <bdlt_dayofweekutil.h>

#include <bslim_testutil.h>

#include <bsls_asserttest.h>

#include <bsl_climits.h>       // 'INT_MAX', 'INT_MIN'
#include <bsl_iostream.h>      // 'cout'

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test consists of a single static member function that
// calculates the day of the week shifted from a given date by a certain number
// of days.
//-----------------------------------------------------------------------------
// [ 1] static DayOfWeek::Enum wrap(DayOfWeek::Enum dayOfWeek, int num);
//-----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE
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

typedef bdlt::DayOfWeekUtil   Util;
typedef bdlt::DayOfWeek::Enum Enum;

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    (void) veryVerbose;
    (void) veryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   This will test the usage examples provided in the component header
        //   file.
        //
        // Concerns:
        //: 1 The usage examples provided in the component header file must
        //:   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into driver, remove leading
        //:   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nUSAGE EXAMPLE\n"
                               "=============\n";

///Usage
///------
// This section illustrates intended use of this component.
//
///Example 1: Basic Usage
/// - - - - - - - - - - -
// Suppose we have some event occuring every ten days. Today is the day of the
// performance, Friday, and we want to know when the previous one took place
// and when the next one will be.
// First, we create a 'bdlt::DayOfWeek' variable for the current day:
//..
    bdlt::DayOfWeek::Enum current = bdlt::DayOfWeek::e_FRI;
//..
// Next, we calculate previous and following event days using
// 'bdlt::DayOfWeekUtil::wrap' function:
//..
    bdlt::DayOfWeek::Enum previous  = bdlt::DayOfWeekUtil::wrap(current, -10);
    bdlt::DayOfWeek::Enum following = bdlt::DayOfWeekUtil::wrap(current,  10);
//..
// Finally, we want to verify, that days of week were calculated correctly:
//..
    ASSERT(bdlt::DayOfWeek::e_TUE == previous );
    ASSERT(bdlt::DayOfWeek::e_MON == following);
//..
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'wrap'
        //
        // Concerns:
        //: 1 The 'wrap' function correctly conducts
        //:   'end of the week/beginning of the next week' rollover.
        //:
        //: 2 The 'wrap' function correctly conducts
        //:   'beginning of the week/end of the previous week' rollover.
        //:
        //: 3 The 'wrap' function correctly handles any shift from the range of
        //:   integer values (including null value and integer limits).
        //
        // Plan:
        //: 1 For each value from day-of-week enumeration call
        //:   bdlt::DayOfWeekUtil::wrap' function with different shifts (both
        //:   positive and negative, including zero and integer limits) and
        //:   verify the correctness of the results.  (C-1..3)
        //
        // Testing:
        //   static DayOfWeek::Enum wrap(DayOfWeek::Enum dayOfWeek, int num);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'wrap'\n"
                               "==============\n";

        const Enum SUN(bdlt::DayOfWeek::e_SUN);
        const Enum MON(bdlt::DayOfWeek::e_MON);
        const Enum TUE(bdlt::DayOfWeek::e_TUE);
        const Enum WED(bdlt::DayOfWeek::e_WED);
        const Enum THU(bdlt::DayOfWeek::e_THU);
        const Enum FRI(bdlt::DayOfWeek::e_FRI);
        const Enum SAT(bdlt::DayOfWeek::e_SAT);

        //                              +0/+7  +1   +2   +3   +4   +5   +6
        //                              -0/-7  -6   -5   -4   -3   -2   -1
        //                              -----  ---  ---  ---  ---  ---  ---
        const Enum SUN_UNITS_PACK[] = { SUN,   MON, TUE, WED, THU, FRI, SAT };
        const Enum MON_UNITS_PACK[] = { MON,   TUE, WED, THU, FRI, SAT, SUN };
        const Enum TUE_UNITS_PACK[] = { TUE,   WED, THU, FRI, SAT, SUN, MON };
        const Enum WED_UNITS_PACK[] = { WED,   THU, FRI, SAT, SUN, MON, TUE };
        const Enum THU_UNITS_PACK[] = { THU,   FRI, SAT, SUN, MON, TUE, WED };
        const Enum FRI_UNITS_PACK[] = { FRI,   SAT, SUN, MON, TUE, WED, THU };
        const Enum SAT_UNITS_PACK[] = { SAT,   SUN, MON, TUE, WED, THU, FRI };

        //                             +70  +10  +20  +30  +40  +50  +60
        //                             -70  -60  -50  -40  -30  -20  -10
        //                             ---  ---  ---  ---  ---  ---  ---
        const Enum SUN_TENS_PACK[] = { SUN, WED, SAT, TUE, FRI, MON, THU };
        const Enum MON_TENS_PACK[] = { MON, THU, SUN, WED, SAT, TUE, FRI };
        const Enum TUE_TENS_PACK[] = { TUE, FRI, MON, THU, SUN, WED, SAT };
        const Enum WED_TENS_PACK[] = { WED, SAT, TUE, FRI, MON, THU, SUN };
        const Enum THU_TENS_PACK[] = { THU, SUN, WED, SAT, TUE, FRI, MON };
        const Enum FRI_TENS_PACK[] = { FRI, MON, THU, SUN, WED, SAT, TUE };
        const Enum SAT_TENS_PACK[] = { SAT, TUE, FRI, MON, THU, SUN, WED };

        static const struct {
            int         d_line;           // source line number
            const Enum  d_originalValue;  // day-of-week to shift from
            const Enum *d_unitsValues;    // table of units expected results
            const Enum *d_tensValues;     // table of tens  expected results
        } DATA[] = {
            //line   origin  units expected   tens expected
            //----   ------  --------------   -------------
            { L_,    SUN,    SUN_UNITS_PACK,  SUN_TENS_PACK },
            { L_,    MON,    MON_UNITS_PACK,  MON_TENS_PACK },
            { L_,    TUE,    TUE_UNITS_PACK,  TUE_TENS_PACK },
            { L_,    WED,    WED_UNITS_PACK,  WED_TENS_PACK },
            { L_,    THU,    THU_UNITS_PACK,  THU_TENS_PACK },
            { L_,    FRI,    FRI_UNITS_PACK,  FRI_TENS_PACK },
            { L_,    SAT,    SAT_UNITS_PACK,  SAT_TENS_PACK },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE   = DATA[ti].d_line;
            const Enum  ORIGIN = DATA[ti].d_originalValue;
            const Enum *UNITS  = DATA[ti].d_unitsValues;
            const Enum *TENS   = DATA[ti].d_tensValues;

            if (veryVeryVerbose) {
                T_ P_(LINE) P(ORIGIN);
            }

            Enum zeroResult = Util::wrap(ORIGIN,  0);
            ASSERTV(LINE, zeroResult, UNITS[0] == zeroResult);

            for (int tj = 1; tj < 7 ; ++tj) {
                if (veryVeryVerbose) {
                    T_ T_ P(tj);
                }

                int  aNum    =     tj;
                int  sNum    = 0 - tj;
                Enum aResult = Util::wrap(ORIGIN, aNum);  // addition
                Enum sResult = Util::wrap(ORIGIN, sNum);  // subtraction

                ASSERTV(LINE, aNum, aResult, UNITS[    tj] == aResult);
                ASSERTV(LINE, sNum, sResult, UNITS[7 - tj] == sResult);
            }

            Enum aSevenResult = Util::wrap(ORIGIN, +7);
            Enum sSevenResult = Util::wrap(ORIGIN, -7);

            ASSERTV(LINE, aSevenResult, UNITS[0] == aSevenResult);
            ASSERTV(LINE, sSevenResult, UNITS[0] == sSevenResult);

            for (int tj = 1; tj < 7 ; ++tj) {
                if (veryVeryVerbose) {
                    T_ T_ P(tj);
                }

                int  aNum    =     tj * 10;
                int  sNum    = 0 - tj * 10;
                Enum aResult = Util::wrap(ORIGIN, aNum);
                Enum sResult = Util::wrap(ORIGIN, sNum);

                ASSERTV(LINE, aNum, aResult, TENS[    tj] == aResult);
                ASSERTV(LINE, sNum, sResult, TENS[7 - tj] == sResult);
            }

            Enum aSeventyResult = Util::wrap(ORIGIN, +70);
            Enum sSeventyResult = Util::wrap(ORIGIN, -70);

            ASSERTV(LINE, aSeventyResult, TENS[0] == aSeventyResult);
            ASSERTV(LINE, sSeventyResult, TENS[0] == sSeventyResult);

            Enum aLimitResult = Util::wrap(ORIGIN, INT_MAX);
            Enum sLimitResult = Util::wrap(ORIGIN, INT_MIN);

            const Enum A_EXP_LIMIT_RESULT = Util::wrap(ORIGIN, INT_MAX % 7);
            const Enum S_EXP_LIMIT_RESULT = Util::wrap(ORIGIN, INT_MIN % 7);

            ASSERTV(LINE, A_EXP_LIMIT_RESULT, aLimitResult,
                    A_EXP_LIMIT_RESULT  == aLimitResult);
            ASSERTV(LINE, S_EXP_LIMIT_RESULT, sLimitResult,
                    S_EXP_LIMIT_RESULT  == sLimitResult);

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
