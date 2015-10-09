// bbldc_basicdaterangedaycountadapter.t.cpp                          -*-C++-*-

#include <bbldc_basicdaterangedaycountadapter.h>

#include <bbldc_basicisdaactualactual.h>
#include <bbldc_basicisma30360.h>

#include <bdls_testutil.h>

#include <bdlt_date.h>

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
// [ 1] int daysDiff(beginDate, endDate) const;
// [ 1] const bdlt::Date& firstDate() const;
// [ 1] const bdlt::Date& lastDate() const;
// [ 1] double yearsDiff(beginDate, endDate) const;
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

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int  test    = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose = argc > 2;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

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
///Example 1: Adapting 'bbldc::BasicIsma30360'
///- - - - - - - - - - - - - - - - - - - - - -
// This example shows the procedure for using
// 'bbldc::BasicDateRangeDayCountAdapter' to adapt the 'bbldc::BasicIsma30360'
// day-count convention to the 'bbldc::DateRangeDayCount' protocol, and then
// the use of the day-count methods.  First, we define an instance of the
// adapted day-count convention and obtain a reference to the
// 'bbldc::DateRangeDayCount':
//..
    const bbldc::BasicDateRangeDayCountAdapter<bbldc::BasicIsma30360> myDcc =
                 bbldc::BasicDateRangeDayCountAdapter<bbldc::BasicIsma30360>();

    const bbldc::DateRangeDayCount& dcc = myDcc;
//..
// Then, create two 'bdlt::Date' variables, 'd1' and 'd2', with which to use
// the day-count convention methods:
//..
    const bdlt::Date d1(2003, 10, 18);
    const bdlt::Date d2(2003, 12, 31);
//..
// Now, use the base-class reference to compute the day count between the two
// dates:
//..
    const int daysDiff = dcc.daysDiff(d1, d2);
    ASSERT(72 == daysDiff);
//..
// Finally, use the base-class reference to compute the year fraction between
// the two dates:
//..
    const double yearsDiff = dcc.yearsDiff(d1, d2);
    // Need fuzzy comparison since 'yearsDiff' is a 'double'.
    ASSERT(0.1999 < yearsDiff && 0.2001 > yearsDiff);
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
        //
        // Plan:
        //: 1 Construct an adapted object of a class (which is derived from
        //:  'bbldc::DateRangeDayCount') and bind a 'bbldc::DateRangeDayCount'
        //:  reference to the object.  Using the base class reference, invoke
        //:  the 'daysDiff', 'firstDate', 'lastDate', and 'yearsDiff' methods.
        //:  Verify that the correct implementations of the methods are called.
        //:  (C-1..2)
        //
        // Testing:
        //   int daysDiff(beginDate, endDate) const;
        //   const bdlt::Date& firstDate() const;
        //   const bdlt::Date& lastDate() const;
        //   double yearsDiff(beginDate, endDate) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "INHERITANCE MECHANISM" << endl
                          << "=====================" << endl;

        bdlt::Date DATE1(1993, 12, 15);
        bdlt::Date DATE2(1993, 12, 31);
        bdlt::Date DATE3(2003,  2, 28);
        bdlt::Date DATE4(2004,  2, 29);

        if (verbose) cout << "\nTesting 'daysDiff(date1, date2)'" << endl;
        {
            {
                bbldc::BasicDateRangeDayCountAdapter<
                                              bbldc::BasicIsdaActualActual> mX;

                const bbldc::DateRangeDayCount& protocol = mX;

                ASSERT( 16 == protocol.daysDiff(DATE1, DATE2));
                ASSERT(366 == protocol.daysDiff(DATE3, DATE4));
            }

            {
                bbldc::BasicDateRangeDayCountAdapter<bbldc::BasicIsma30360> mX;

                const bbldc::DateRangeDayCount& protocol = mX;

                ASSERT( 15 == protocol.daysDiff(DATE1, DATE2));
                ASSERT(361 == protocol.daysDiff(DATE3, DATE4));
            }
        }

        if (verbose) cout << "\nTesting 'firstDate' and 'lastDate'" << endl;
        {
            {
                bbldc::BasicDateRangeDayCountAdapter<
                                              bbldc::BasicIsdaActualActual> mX;

                const bbldc::DateRangeDayCount& protocol = mX;

                ASSERT(bdlt::Date(   1,  1,  1)  == protocol.firstDate());
                ASSERT(bdlt::Date(9999, 12, 31)  == protocol.lastDate());
            }

            {
                bbldc::BasicDateRangeDayCountAdapter<bbldc::BasicIsma30360> mX;

                const bbldc::DateRangeDayCount& protocol = mX;

                ASSERT(bdlt::Date(   1,  1,  1)  == protocol.firstDate());
                ASSERT(bdlt::Date(9999, 12, 31)  == protocol.lastDate());
            }
        }

        if (verbose) cout << "\nTesting 'yearsDiff(date1, date2)'" << endl;
        {
            {
                bbldc::BasicDateRangeDayCountAdapter<
                                              bbldc::BasicIsdaActualActual> mX;

                bbldc::DateRangeDayCount& protocol = mX;

                double diff1 = 0.0438 - protocol.yearsDiff(DATE1, DATE2);
                ASSERT(-0.00005 <= diff1 && diff1 <= 0.00005);

                double diff2 = 1.0023 - protocol.yearsDiff(DATE3, DATE4);
                ASSERT(-0.00005 <= diff2 && diff2 <= 0.00005);
            }

            {
                bbldc::BasicDateRangeDayCountAdapter<bbldc::BasicIsma30360> mX;

                bbldc::DateRangeDayCount& protocol = mX;

                double diff1 = 0.0417 - protocol.yearsDiff(DATE1, DATE2);
                ASSERT(-0.00005 <= diff1 && diff1 <= 0.00005);

                double diff2 = 1.0028 - protocol.yearsDiff(DATE3, DATE4);
                ASSERT(-0.00005 <= diff2 && diff2 <= 0.00005);
            }
        }
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT == FOUND." << endl;
        testStatus = -1;
      }
    }

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
