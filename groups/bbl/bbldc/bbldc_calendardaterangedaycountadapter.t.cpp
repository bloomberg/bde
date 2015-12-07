// bbldc_calendardaterangedaycountadapter.t.cpp                       -*-C++-*-

#include <bbldc_calendardaterangedaycountadapter.h>

#include <bbldc_calendarbus252.h>

#include <bdlt_calendar.h>
#include <bdlt_date.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>

#include <bsl_cstdlib.h>     // 'atoi'
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                              TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test consists of two forwarding methods that forward to
// static member function implementations within the template parameter class
// that compute the day count and year fraction between two dates and two
// methods to complete the protocol implementation.  The general plan is that
// the methods are tested with two different template parameters to ensure the
// forwarding methods forward correctly and the other methods return the
// expected value.
// ----------------------------------------------------------------------------
// [ 1] CalendarDateRangeDayCountAdapter(calendar, basicAllocator);
// [ 1] ~CalendarDateRangeDayCountAdapter();
// [ 1] int daysDiff(beginDate, endDate) const;
// [ 1] const bdlt::Date& firstDate() const;
// [ 1] const bdlt::Date& lastDate() const;
// [ 1] double yearsDiff(beginDate, endDate) const;
// [ 1] bslma::Allocator *allocator() const;
// ----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE
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
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int  test        = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose     = argc > 2;
    bool veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    bslma::TestAllocator defaultAllocator("default", veryVerbose);
    bslma::Default::setDefaultAllocator(&defaultAllocator);

    switch (test) { case 0:
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

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Adapting 'bbldc::CalendarBus252'
///- - - - - - - - - - - - - - - - - - - - - -
// This example shows the procedure for using
// 'bbldc::CalendarDateRangeDayCountAdapter' to adapt the
// 'bbldc::CalendarBus252' day-count convention to the
// 'bbldc::DateRangeDayCount' protocol, and then the use of the day-count
// methods.  First, we create a 'calendar' with a valid range spanning 2003 and
// typical weekend days:
//..
    bdlt::Calendar calendar;
    calendar.setValidRange(bdlt::Date(2003, 1, 1), bdlt::Date(2003, 12, 31));
    calendar.addWeekendDay(bdlt::DayOfWeek::e_SUN);
    calendar.addWeekendDay(bdlt::DayOfWeek::e_SAT);
//..
// Then, we define an instance of the adapted day-count convention and obtain a
// reference to the 'bbldc::DateRangeDayCount':
//..
    const bbldc::CalendarDateRangeDayCountAdapter<bbldc::CalendarBus252>
                                                               myDcc(calendar);
    const bbldc::DateRangeDayCount&                            dcc = myDcc;
//..
// Next, create two 'bdlt::Date' variables, 'd1' and 'd2', with which to use
// the day-count convention methods:
//..
    const bdlt::Date d1(2003, 10, 19);
    const bdlt::Date d2(2003, 12, 31);
//..
// Now, use the base-class reference to compute the day count between the two
// dates:
//..
    const int daysDiff = dcc.daysDiff(d1, d2);
    ASSERT(52 == daysDiff);
//..
// Finally, use the base-class reference to compute the year fraction between
// the two dates:
//..
    const double yearsDiff = dcc.yearsDiff(d1, d2);
    // Need fuzzy comparison since 'yearsDiff' is a 'double'.
    ASSERT(yearsDiff > 0.2063 && yearsDiff < 0.2064);
//..
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // INHERITANCE MECHANISM
        //   Verify the inheritance mechanism works as expected.
        //
        // Concerns:
        //: 1 The adaptation of a day-count convention class compiles and links
        //:   (all virtual functions are defined).
        //:
        //: 2 The functions are in fact virtual and accessible from the
        //:  'bbldc::DateRangeDayCount' base class.
        //:
        //: 3 The values bound at construction are correctly forwarded to the
        //:   methods.
        //:
        //: 4 The destructor works as expected.
        //:
        //: 5 The constructor has the internal memory management system hooked
        //:   up properly so that *all* internally allocated memory draws from
        //:   the same user-supplied allocator whenever one is specified and
        //:   the 'allocator' accessor return value is as expected.
        //:
        //: 6 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Construct an adapted object of a class (which is derived from
        //:  'bbldc::DateRangeDayCount') and bind a 'bbldc::DateRangeDayCount'
        //:  reference to the object.  Using the base class reference, invoke
        //:  the 'daysDiff', 'firstDate', 'lastDate', and 'yearsDiff' methods.
        //:  Verify that the correct implementations of the methods are called.
        //:  (C-1..3)
        //:
        //: 2 The destructor is empty so the concern is trivially satisfied.
        //:   (C-4)
        //:
        //: 3 Create an object using the constructor with and without passing
        //:   in an allocator and verify the allocator is stored using the
        //:   'allocator' accessor.
        //:
        //: 4 Verify defensive checks are triggered for invalid values.  (C-6)
        //
        // Testing:
        //   CalendarDateRangeDayCountAdapter(calendar, basicAllocator);
        //   ~CalendarDateRangeDayCountAdapter();
        //   int daysDiff(beginDate, endDate) const;
        //   const bdlt::Date& firstDate() const;
        //   const bdlt::Date& lastDate() const;
        //   double yearsDiff(beginDate, endDate) const;
        //   bslma::Allocator *allocator() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "INHERITANCE MECHANISM" << endl
                          << "=====================" << endl;

        bdlt::Calendar        mCalendar;
        const bdlt::Calendar& CALENDAR = mCalendar;
        mCalendar.setValidRange(bdlt::Date(1990,  1,  1),
                                bdlt::Date(1999, 12, 31));

        bdlt::Date DATE1(1992, 2, 1);
        bdlt::Date DATE2(1993, 3, 1);
        bdlt::Date DATE3(1993, 2, 1);
        bdlt::Date DATE4(1996, 2, 1);

        if (verbose) cout << "\nTesting 'daysDiff'" << endl;
        {
            {
                bbldc::CalendarDateRangeDayCountAdapter<
                                           bbldc::CalendarBus252> mX(CALENDAR);

                const bbldc::DateRangeDayCount& protocol = mX;

                ASSERT( 394 == protocol.daysDiff(DATE1, DATE2));
                ASSERT(1095 == protocol.daysDiff(DATE3, DATE4));
            }
        }

        if (verbose) cout << "\nTesting 'firstDate' and 'lastDate'" << endl;
        {
            {
                bbldc::CalendarDateRangeDayCountAdapter<
                                           bbldc::CalendarBus252> mX(CALENDAR);

                const bbldc::DateRangeDayCount& protocol = mX;

                ASSERT(CALENDAR.firstDate() == protocol.firstDate());
                ASSERT(CALENDAR.lastDate()  == protocol.lastDate());
            }
        }

        if (verbose) cout << "\nTesting 'yearsDiff'" << endl;
        {
            {
                bbldc::CalendarDateRangeDayCountAdapter<
                                           bbldc::CalendarBus252> mX(CALENDAR);

                const bbldc::DateRangeDayCount& protocol = mX;

                double diff1 = 1.5635 - protocol.yearsDiff(DATE1, DATE2);
                ASSERT(-0.00005 <= diff1 && diff1 <= 0.00005);

                double diff2 = 4.3452 - protocol.yearsDiff(DATE3, DATE4);
                ASSERT(-0.00005 <= diff2 && diff2 <= 0.00005);
            }
        }

        if (verbose) cout << "\nTesting 'allocator'" << endl;
        {
            {
                bbldc::CalendarDateRangeDayCountAdapter<
                                           bbldc::CalendarBus252> mX(CALENDAR);

                const bbldc::CalendarDateRangeDayCountAdapter<
                                        bbldc::CalendarBus252>& X = mX;

                ASSERT(&defaultAllocator == X.allocator());
            }
            {
                bslma::TestAllocator sa("supplied", veryVerbose);

                bbldc::CalendarDateRangeDayCountAdapter<
                                      bbldc::CalendarBus252> mX(CALENDAR, &sa);

                const bbldc::CalendarDateRangeDayCountAdapter<
                                        bbldc::CalendarBus252>& X = mX;

                ASSERT(&sa == X.allocator());
            }
        }

        { // negative testing
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            bbldc::CalendarDateRangeDayCountAdapter<
                                           bbldc::CalendarBus252> mX(CALENDAR);

            const bbldc::CalendarDateRangeDayCountAdapter<
                                        bbldc::CalendarBus252>& X = mX;

            ASSERT_SAFE_PASS(X.daysDiff(bdlt::Date(1990,  1,  1),
                                        bdlt::Date(1999, 12, 31)));

            ASSERT_SAFE_FAIL(X.daysDiff(bdlt::Date(1989, 12, 31),
                                        bdlt::Date(1999, 12, 31)));

            ASSERT_SAFE_FAIL(X.daysDiff(bdlt::Date(1990,  1,  1),
                                        bdlt::Date(2000,  1,  1)));

            ASSERT_SAFE_PASS(X.yearsDiff(bdlt::Date(1990,  1,  1),
                                         bdlt::Date(1999, 12, 31)));

            ASSERT_SAFE_FAIL(X.yearsDiff(bdlt::Date(1989, 12, 31),
                                         bdlt::Date(1999, 12, 31)));

            ASSERT_SAFE_FAIL(X.yearsDiff(bdlt::Date(1990,  1,  1),
                                         bdlt::Date(2000,  1,  1)));
        }
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT == FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
