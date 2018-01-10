// bdlt_datetimeintervalutil.t.cpp                                    -*-C++-*-
#include <bdlt_datetimeintervalutil.h>

#include <bdlt_datetimeinterval.h>
#include <bdlt_currenttime.h>
#include <bslim_testutil.h>

#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// This test driver uses the standard table-based test case implementation
// techniques to compare the make* functions with the DatetimeInterval
// constructor.
//-----------------------------------------------------------------------------
//
// CLASS METHODS
// [ 1] DatetimeInterval makeDays(int days);
// [ 1] DatetimeInterval makeHours(Int64 hours);
// [ 1] DatetimeInterval makeMinutes(Int64 minutes);
// [ 1] DatetimeInterval makeSeconds(Int64 seconds);
// [ 1] DatetimeInterval makeMilliseconds(Int64 milliseconds);
// [ 1] DatetimeInterval makeMicroseconds(Int64 microseconds);
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

//=============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdlt::DatetimeIntervalUtil  Util;
typedef bdlt::DatetimeInterval  Interval;

typedef bsls::Types::Int64  Int64;
typedef bsls::Types::Uint64 Uint64;

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int                 test = argc > 1 ? atoi(argv[1]) : 0;
    const bool             verbose = argc > 2;
    const bool         veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nUSAGE EXAMPLE"
                 << "\n=============" << endl;
        }

///Example 1: Simple Usage of the Various 'make*' Functions
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example shows how we can create a 'bdlt::DatetimeInterval' objects
// having values of 1 day, 2 hours, 3 minutes, 4 seconds, 5 millisecond, and 6
// microseconds by using the 'bdlt::DatetimeInterval' constructor and, more
// readably, by using the 'make*' functions.
//
// First, start with a default (0) 'bdlt::DatetimeInterval':
//..
    bdlt::DatetimeInterval m;
    bdlt::DatetimeInterval d;
//..
// Next, add 1 day to it, and assert that both objects are equal:
//..
    m += bdlt::DatetimeIntervalUtil::makeDays(1);
    d += bdlt::DatetimeInterval(1, 0, 0, 0, 0, 0);
    ASSERT(m == d);
//..
// Then, add 2 hours to it, and assert that both objects are equal:
//..
    m += bdlt::DatetimeIntervalUtil::makeHours(2);
    d += bdlt::DatetimeInterval(0, 2, 0, 0, 0, 0);
    ASSERT(m == d);
//..
// Next, add 3 minutes to it, and assert that both objects are equal:
//..
    m += bdlt::DatetimeIntervalUtil::makeMinutes(3);
    d += bdlt::DatetimeInterval(0, 0, 3, 0, 0, 0);
    ASSERT(m == d);
//..
// Then, add 4 seconds to it, and assert that both objects are equal:
//..
    m += bdlt::DatetimeIntervalUtil::makeSeconds(4);
    d += bdlt::DatetimeInterval(0, 0, 0, 4, 0, 0);
    ASSERT(m == d);
//..
// Next, add 5 milliseconds to it, and assert that both objects are equal:
//..
    m += bdlt::DatetimeIntervalUtil::makeMilliseconds(5);
    d += bdlt::DatetimeInterval(0, 0, 0, 0, 5, 0);
    ASSERT(m == d);
//..
// Then, add 6 microseconds to it, and assert that both objects are equal:
//..
    m += bdlt::DatetimeIntervalUtil::makeMicroseconds(6);
    d += bdlt::DatetimeInterval(0, 0, 0, 0, 0, 6);
    ASSERT(m == d);
//..
// Finally, we create an create a 'DatetimeInterval' with  the final value and
// compare to the objects built in steps.
//..
    bdlt::DatetimeInterval f(1, 2, 3, 4, 5, 6);
    ASSERT(f == m);
    ASSERT(f == d);
//..
//
///Example 2: How to Improve Readability Using the 'make*' Functions
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example shows how we can create a 'bdlt::Datetime' objects having a
// value of now + 2 hours and 30 minutes by using the 'bdlt::DatetimeInterval'
// constructor or, more readably by using the 'make*' functions.
//
// First, create a 'bdlt::Datetime' object having the current time:
//..
    bdlt::Datetime now = bdlt::CurrentTime::utc();
//..
// Now, create the 'bdlt::DatetimeInterval' objects and assign the desired
// values to them using the 'makeHours' and 'makeMinutes' functions, and using
// the 'bdlt::DatetimeInterval' constructor:
//..
    bdlt::Datetime nextEventTime = now
                                 + bdlt::DatetimeIntervalUtil::makeHours(2)
                                 + bdlt::DatetimeIntervalUtil::makeMinutes(30);
    bdlt::Datetime altEventTime  = now
                                 + bdlt::DatetimeInterval(0, 2, 30, 0, 0, 0);
//..
// Finally, assert that both results are equal:
//..
    ASSERT(nextEventTime == altEventTime);
//..
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'make*' MANIPULATORS
        //   Verify the 'make*' methods work as expected.  This testing
        //   assumes that the 'DatetimeInterval' constructor, and equality
        //   operator work correctly.
        //
        // Concerns:
        //: 1 The "time" values are forwarded correctly to the
        //:  'DatetimeInterval' object.
        //:
        //: 2 Each of the factory methods take parameters of their intended
        //:   type.  That is 'int' for days and 'bsls::Types::Int64' for all of
        //:    the others.
        //
        // Plan:
        //: 1 Using the array based test values, compare the
        //:   'DatetimeInterval' objects resulting for the 'make*' methods
        //:   with ones created by a call to 'DatetimeInterval' constructor.
        //:   (C-1)
        //
        // Testing:
        //   DatetimeInterval makeDays(int days);
        //   DatetimeInterval makeHours(Int64 hours);
        //   DatetimeInterval makeMinutes(Int64 minutes);
        //   DatetimeInterval makeSeconds(Int64 seconds);
        //   DatetimeInterval makeMilliseconds(Int64 milliseconds);
        //   DatetimeInterval makeMicroseconds(Int64 microseconds);
        // --------------------------------------------------------------------

        const Int64 k_BIG = static_cast<Int64>(INT_MAX)+1;

        if (verbose) cout << endl
                          << "TESTING 'make*' MANIPULATORS" << endl
                          << "============================" << endl;

        {
            static const struct {
                int   d_day;
                Int64 d_hour;
                Int64 d_minute;
                Int64 d_second;
                Int64 d_msec;
                Int64 d_usec;
            } VALUES[] = {
                {      0,    0,    0,    0,    0,    0 },
                {      0,    0,    0,    0,    0,  999 },
                {      0,    0,    0,    0,  999,    0 },
                {      0,    0,    0,   59,    0,    0 },
                {      0,    0,   59,    0,    0,    0 },
                {      0,   23,    0,    0,    0,    0 },
                {     12,    0,    0,    0,    0,    0 },
                {    999,    0,    0,    0,    0,    0 },
                {      0,    0,    0,    0,    0, -999 },
                {      0,    0,    0,    0, -999,    0 },
                {      0,    0,    0,  -59,    0,    0 },
                {      0,    0,  -59,    0,    0,    0 },
                {      0,  -23,    0,    0,    0,    0 },
                {    -12,    0,    0,    0,    0,    0 },
                {   -999,    0,    0,    0,    0,    0 },
                {      0,   23,   22,   21,  209,    0 },
                {      1,   23,   22,   21,  210,    0 },
                {      2,   23,   22,   21,  210,  117 },
                {      0,  -23,  -22,  -21, -209,    0 },
                {     -1,  -23,  -22,  -21, -210,    0 },
                {     -2,  -23,  -22,  -21, -210, -117 },
                {      0,    0,    0,    0,    0,k_BIG },
                {      0,    0,    0,    0,k_BIG,    0 },
                {      0,    0,    0,k_BIG,    0,    0 },
                {      0,    0,k_BIG,    0,    0,    0 },
                {      0,k_BIG,    0,    0,    0,    0 },
                {INT_MAX,    0,    0,    0,    0,    0 },
            };

            const bsl::size_t NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (bsl::size_t i = 0; i < NUM_VALUES; ++i) {
                const int   DAY    = VALUES[i].d_day;
                const Int64 HOUR   = VALUES[i].d_hour;
                const Int64 MINUTE = VALUES[i].d_minute;
                const Int64 SECOND = VALUES[i].d_second;
                const Int64 MSEC   = VALUES[i].d_msec;
                const Int64 USEC   = VALUES[i].d_usec;

                if (veryVerbose) {
                    T_;
                    P_(DAY) P_(HOUR); P_(MINUTE); P_(SECOND); P_(MSEC);
                    P(USEC);
                }

                Interval xA = Util::makeDays(DAY);
                Interval yA(DAY, 0, 0, 0, 0, 0);
                LOOP1_ASSERT(i, xA == yA);

                Interval xB = Util::makeHours(HOUR);
                Interval yB(0, HOUR, 0, 0, 0, 0);
                LOOP1_ASSERT(i, xB == yB);

                Interval xC = Util::makeMinutes(MINUTE);
                Interval yC(0, 0, MINUTE, 0, 0, 0);
                LOOP1_ASSERT(i, xC == yC);

                Interval xD = Util::makeSeconds(SECOND);
                Interval yD(0, 0, 0, SECOND, 0, 0);
                LOOP1_ASSERT(i, xD == yD);

                Interval xE = Util::makeMilliseconds(MSEC);
                Interval yE(0, 0, 0, 0, MSEC, 0);
                LOOP1_ASSERT(i, xE == yE);

                Interval xF = Util::makeMicroseconds(USEC);
                Interval yF(0, 0, 0, 0, 0, USEC);
                LOOP1_ASSERT(i, xF == yF);
            }
            if (veryVerbose) cout << endl;
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
// Copyright 2017 Bloomberg Finance L.P.
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
