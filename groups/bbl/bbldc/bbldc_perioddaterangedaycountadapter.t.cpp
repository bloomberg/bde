// bbldc_perioddaterangedaycountadapter.t.cpp                         -*-C++-*-

#include <bbldc_perioddaterangedaycountadapter.h>

#include <bbldc_periodicmaactualactual.h>

#include <bdls_testutil.h>

#include <bdlt_date.h>

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
// [ 1] PeriodDateRangeDayCountAdapter(pD, pYD, bA);
// [ 1] ~PeriodDateRangeDayCountAdapter();
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

#define ASSERT       BDLS_TESTUTIL_ASSERT
#define ASSERTV      BDLS_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BDLS_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BDLS_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BDLS_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BDLS_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BDLS_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BDLS_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BDLS_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BDLS_TESTUTIL_LOOP6_ASSERT

#define Q            BDLS_TESTUTIL_Q   // Quote identifier literally.
#define P            BDLS_TESTUTIL_P   // Print identifier and value.
#define P_           BDLS_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BDLS_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BDLS_TESTUTIL_L_  // current Line number

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
///Example 1: Adapting 'bbldc::PeriodIcmaActualActual'
///- - - - - - - - - - - - - - - - - - - - - - - - - -
// This example shows the procedure for using
// 'bbldc::PeriodDateRangeDayCountAdapter' to adapt the
// 'bbldc::PeriodIcmaActualActual' day-count convention to the
// 'bbldc::DateRangeDayCount' protocol, and then the use of the day-count
// methods.  First, we create a schedule of period dates, 'sched',
// corresponding to a quarterly payment ('periodYearDiff == 0.25'):
//..
    bsl::vector<bdlt::Date> sched;
    sched.push_back(bdlt::Date(2003, 10, 1));
    sched.push_back(bdlt::Date(2004,  1, 1));
//..
// Then, we define an instance of the adapted day-count convention and obtain a
// reference to the 'bbldc::DateRangeDayCount':
//..
    const bbldc::PeriodDateRangeDayCountAdapter<bbldc::PeriodIcmaActualActual>
                                                                   myDcc(sched,
                                                                         0.25);
    const bbldc::DateRangeDayCount&                                dcc = myDcc;
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
    ASSERT(73 == daysDiff);
//..
// Finally, use the base-class reference to compute the year fraction between
// the two dates:
//..
    const double yearsDiff = dcc.yearsDiff(d1, d2);
    // Need fuzzy comparison since 'yearsDiff' is a 'double'.
    ASSERT(yearsDiff > 0.1983 && yearsDiff < 0.1985);
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
        //   PeriodDateRangeDayCountAdapter(pD, pYD, bA);
        //   ~PeriodDateRangeDayCountAdapter();
        //   int daysDiff(beginDate, endDate) const;
        //   const bdlt::Date& firstDate() const;
        //   const bdlt::Date& lastDate() const;
        //   double yearsDiff(beginDate, endDate) const;
        //   bslma::Allocator *allocator() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "INHERITANCE MECHANISM" << endl
                          << "=====================" << endl;

        bsl::vector<bdlt::Date>        mSchedule;
        const bsl::vector<bdlt::Date>& SCHEDULE = mSchedule;
        {
            for (unsigned year = 1990; year <= 2006; ++year) {
                mSchedule.push_back(bdlt::Date(year, 1, 1));
            }
        }

        bdlt::Date DATE1(1992, 2, 1);
        bdlt::Date DATE2(1993, 3, 1);
        bdlt::Date DATE3(1993, 2, 1);
        bdlt::Date DATE4(1996, 2, 1);

        if (verbose) cout << "\nTesting 'daysDiff'" << endl;
        {
            {
                bbldc::PeriodDateRangeDayCountAdapter<
                              bbldc::PeriodIcmaActualActual> mX(SCHEDULE, 1.0);

                const bbldc::DateRangeDayCount& protocol = mX;

                ASSERT( 394 == protocol.daysDiff(DATE1, DATE2));
                ASSERT(1095 == protocol.daysDiff(DATE3, DATE4));
            }
        }

        if (verbose) cout << "\nTesting 'firstDate' and 'lastDate'" << endl;
        {
            {
                bbldc::PeriodDateRangeDayCountAdapter<
                              bbldc::PeriodIcmaActualActual> mX(SCHEDULE, 1.0);

                const bbldc::DateRangeDayCount& protocol = mX;

                ASSERT(SCHEDULE.front() == protocol.firstDate());
                ASSERT(SCHEDULE.back()  == protocol.lastDate());
            }
        }

        if (verbose) cout << "\nTesting 'yearsDiff'" << endl;
        {
            {
                bbldc::PeriodDateRangeDayCountAdapter<
                              bbldc::PeriodIcmaActualActual> mX(SCHEDULE, 1.0);

                const bbldc::DateRangeDayCount& protocol = mX;

                double diff1 = 1.0769 - protocol.yearsDiff(DATE1, DATE2);
                ASSERT(-0.00005 <= diff1 && diff1 <= 0.00005);

                double diff2 = 2.9998 - protocol.yearsDiff(DATE3, DATE4);
                ASSERT(-0.00005 <= diff2 && diff2 <= 0.00005);
            }
        }

        if (verbose) cout << "\nTesting 'allocator'" << endl;
        {
            {
                bbldc::PeriodDateRangeDayCountAdapter<
                              bbldc::PeriodIcmaActualActual> mX(SCHEDULE, 1.0);

                const bbldc::PeriodDateRangeDayCountAdapter<
                                        bbldc::PeriodIcmaActualActual>& X = mX;

                ASSERT(&defaultAllocator == X.allocator());
            }
            {
                bslma::TestAllocator sa("supplied", veryVerbose);

                bbldc::PeriodDateRangeDayCountAdapter<
                         bbldc::PeriodIcmaActualActual> mX(SCHEDULE, 1.0, &sa);

                const bbldc::PeriodDateRangeDayCountAdapter<
                                        bbldc::PeriodIcmaActualActual>& X = mX;

                ASSERT(&sa == X.allocator());
            }
        }

        { // negative testing
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            // 'periodDate' with no errors

            bsl::vector<bdlt::Date>        mA;
            const bsl::vector<bdlt::Date>& A = mA;
            {
                mA.push_back(bdlt::Date(2015, 1, 5));
                mA.push_back(bdlt::Date(2015, 2, 5));
                mA.push_back(bdlt::Date(2015, 3, 5));
                mA.push_back(bdlt::Date(2015, 4, 5));
                mA.push_back(bdlt::Date(2015, 5, 5));
            }

            // 'periodDate' with non-sorted values

            bsl::vector<bdlt::Date>        mE1;
            const bsl::vector<bdlt::Date>& E1 = mE1;
            {
                mE1.push_back(bdlt::Date(2015, 1, 5));
                mE1.push_back(bdlt::Date(2015, 3, 5));
                mE1.push_back(bdlt::Date(2015, 2, 5));
                mE1.push_back(bdlt::Date(2015, 4, 5));
                mE1.push_back(bdlt::Date(2015, 5, 5));
            }

            // 'periodDate' with non-unique values

            bsl::vector<bdlt::Date>        mE2;
            const bsl::vector<bdlt::Date>& E2 = mE2;
            {
                mE2.push_back(bdlt::Date(2015, 1, 5));
                mE2.push_back(bdlt::Date(2015, 2, 5));
                mE2.push_back(bdlt::Date(2015, 3, 5));
                mE2.push_back(bdlt::Date(2015, 3, 5));
                mE2.push_back(bdlt::Date(2015, 4, 5));
                mE2.push_back(bdlt::Date(2015, 5, 5));
            }

            // 'periodDate' with only one value

            bsl::vector<bdlt::Date>        mE3;
            const bsl::vector<bdlt::Date>& E3 = mE3;
            {
                mE3.push_back(bdlt::Date(2015, 1, 5));
            }

            // 'periodDate' with no values

            bsl::vector<bdlt::Date>        mE4;
            const bsl::vector<bdlt::Date>& E4 = mE4;

            ASSERT_PASS(bbldc::PeriodDateRangeDayCountAdapter<
                                       bbldc::PeriodIcmaActualActual>(A, 1.0));
            ASSERT_SAFE_FAIL(bbldc::PeriodDateRangeDayCountAdapter<
                                      bbldc::PeriodIcmaActualActual>(E1, 1.0));
            ASSERT_SAFE_FAIL(bbldc::PeriodDateRangeDayCountAdapter<
                                      bbldc::PeriodIcmaActualActual>(E2, 1.0));
            ASSERT_SAFE_FAIL(bbldc::PeriodDateRangeDayCountAdapter<
                                      bbldc::PeriodIcmaActualActual>(E3, 1.0));
            ASSERT_SAFE_FAIL(bbldc::PeriodDateRangeDayCountAdapter<
                                      bbldc::PeriodIcmaActualActual>(E4, 1.0));

            // verify out of range

            bsl::vector<bdlt::Date>        mL;
            const bsl::vector<bdlt::Date>& L = mL;
            {
                mL.push_back(bdlt::Date(1750, 1, 5));
                mL.push_back(bdlt::Date(1760, 2, 5));
            }

            bbldc::PeriodDateRangeDayCountAdapter<
                                     bbldc::PeriodIcmaActualActual> mX(L, 1.0);

            const bbldc::DateRangeDayCount& protocol = mX;

            ASSERT_SAFE_PASS(protocol.yearsDiff(bdlt::Date(1751, 1, 1),
                                                bdlt::Date(1753, 1, 1)));

            ASSERT_SAFE_FAIL(protocol.yearsDiff(bdlt::Date(1740, 1, 1),
                                                bdlt::Date(1753, 1, 1)));

            ASSERT_SAFE_FAIL(protocol.yearsDiff(bdlt::Date(1753, 1, 1),
                                                bdlt::Date(1740, 1, 1)));

            ASSERT_SAFE_FAIL(protocol.yearsDiff(bdlt::Date(1753, 1, 1),
                                                bdlt::Date(1780, 1, 1)));

            ASSERT_SAFE_FAIL(protocol.yearsDiff(bdlt::Date(1780, 1, 1),
                                                bdlt::Date(1753, 1, 1)));
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
