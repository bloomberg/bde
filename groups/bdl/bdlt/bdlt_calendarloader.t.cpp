// bdlt_calendarloader.t.cpp                                          -*-C++-*-
#include <bdlt_calendarloader.h>

#include <bdlt_date.h>             // for testing only
#include <bdlt_dayofweek.h>        // for testing only
#include <bdlt_dayofweekset.h>     // for testing only
#include <bdlt_packedcalendar.h>   // for testing only

#include <bslim_testutil.h>

#include <bsls_protocoltest.h>

#include <bsl_cstdlib.h>      // 'atoi'
#include <bsl_iostream.h>
#include <bsl_string.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                              TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test defines a protocol class the purpose of which is to
// provide an interface for loading calendars.
//
// Global Concerns:
//: o The test driver is robust w.r.t. reuse in other, similar components.
//: o It is possible to create a concrete implementation the protocol.
// ----------------------------------------------------------------------------
// CREATORS
// [ 1] virtual ~CalendarLoader();
//
// MANIPULATORS
// [ 1] virtual int load(PackedCalendar *result, const char *name) = 0;
// ----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE

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

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Implementing the 'bdlt::CalendarLoader' Protocol
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates an elided concrete implementation of the
// 'bdlt::CalendarLoader' protocol that interprets calendar information
// contained in ASCII strings that are formatted using JSON.  Note that, in
// general, an implementation of 'bdlt::CalendarLoader' must obtain calendar
// information from *some* data source.  Our elided implementation leaves it
// unspecified as to where the JSON strings are obtained (i.e., whether from a
// file system, a database, a local or remote service, etc.).
//
// First, we show the JSON format that our calendar loader accepts.  For
// simplicity, we omit support for holiday codes and weekend-days transitions:
//..
//  {
//      "firstDate":   "YYYY-MM-DD",
//      "lastDate":    "YYYY-MM-DD",
//      "weekendDays": [ wd, ... ],
//      "holidays":    [ "YYYY-MM-DD", ... ]
//  }
//..
// Note that "YYYY-MM-DD" is an ISO 8601 representation for the value of a
// 'bdlt::Date' object and 'wd' is an integer in the range '[1 .. 7]'.  The
// range used for specifying weekend days corresponds directly to the
// 'bdlt::DayOfWeek::Enum' enumeration, '[e_SUN = 1 .. e_SAT]' (see
// 'bdlt_dayofweek').  We assume that the four JSON attributes, "firstDate",
// "lastDate", "weekendDays", and "holidays", must occur in the JSON string in
// the order in which they appear in the above display, but only "firstDate"
// and "lastDate" are *required* attributes.
//
// Then, we define the interface of our implementation:
//..
    class MyCalendarLoader : public bdlt::CalendarLoader {
        // This class provides a concrete implementation of the
        // 'bdlt::CalendarLoader' protocol (an abstract interface) for loading
        // a calendar.  This elided implementation obtains calendar information
        // from ASCII strings formatted using JSON.  The source of the strings
        // is unspecified.

      public:
        // CREATORS
        MyCalendarLoader();
            // Create a 'MyCalendarLoader' object.

        virtual ~MyCalendarLoader();
            // Destroy this object.

        // MANIPULATORS
        virtual int load(bdlt::PackedCalendar *result,
                         const char           *calendarName);
            // Load, into the specified 'result', the calendar identified by
            // the specified 'calendarName'.  Return 0 on success, and a
            // non-zero value otherwise.  If the calendar corresponding to
            // 'calendarName' is not found, 1 is returned with no effect on
            // '*result'.  If a non-zero value other than 1 is returned
            // (indicating a different error), '*result' is valid, but its
            // value is undefined.
    };
//..
// Next, we implement the creators, trivially, as 'MyCalendarLoader' does not
// contain any instance data members:
//..
    // CREATORS
    inline
    MyCalendarLoader::MyCalendarLoader()
    {
    }

    inline
    MyCalendarLoader::~MyCalendarLoader()
    {
    }
//..
// Then, we implement the 'load' function:
//..
    // MANIPULATORS
    int MyCalendarLoader::load(bdlt::PackedCalendar *result,
                               const char           * /* calendarName */)
    {
//..
// Next, we look up the calendar identified by 'calendarName' and load the
// corresponding text into a 'bsl::string' object, 'json' (as stated earlier,
// we do not specify in this example from where the calendar information is
// obtained):
//..
        // Obtain the information for the calendar identified by 'calendarName'
        // from the unspecified data source and load it into the 'json' string.

        bsl::string json;

        // Since a JSON parser is not available to 'bdlt', this example assumes
        // that 'json' is populated with the following specific data:
        //..
        //  {
        //      "firstDate":   "1990-01-01",
        //      "lastDate":    "1990-12-31",
        //      "weekendDays": [ 1, 7 ],
        //      "holidays":    [ "1990-05-28", "1990-07-04", "1990-09-03" ]
        //  }
        //..
        // Similarly, we hard-wire the value of a status flag, 'rc', to
        // indicate that this string was successfully retrieved from the data
        // source.

        int rc = 0;  // success obtaining calendar information

        if (rc != 0) {
            return 1;                                                 // RETURN
        }
//..
// Note that the non-zero value 1 is returned only in the case where the
// calendar information corresponding to 'calendarName' cannot be found (per
// the contract for the 'load' method).
//
// Then, we parse the "firstDate" and "lastDate" attributes from the 'json'
// string, loading the results into like-named variables:
//..
        // Parse the "firstDate" and "lastDate" JSON attributes and load the
        // results into 'firstDate' and 'lastDate', respectively.  It is an
        // error if either of the "firstDate" or "lastDate" attributes are
        // missing, or if they are out of order.

        bdlt::Date firstDate;
        bdlt::Date lastDate;

        // For the purposes of this Usage, we hard-wire the first and last
        // dates that are hypothetically parsed from the 'json' string, and
        // set the 'rc' status flag indicating that parsing succeeded.

        firstDate.setYearMonthDay(1990,  1,  1);
        lastDate.setYearMonthDay( 1990, 12, 31);
        rc = 0;  // success parsing "firstDate" and "lastDate" attributes

        if (rc != 0 || firstDate > lastDate) {
            return 2;                                                 // RETURN
        }

        result->removeAll();

        result->setValidRange(firstDate, lastDate);
//..
// Next, we parse the "weekendDays" attribute from 'json' and load the result
// into a 'bdlt::DayOfWeekSet' object, 'dayOfWeekSet':
//..
        // For the purposes of this Usage, we hard-wire a boolean flag
        // indicating that the "weekendDays" attribute was hypothetically
        // detected in the 'json' string.

        bool isWeekendDaysAttributePresent = true;

        if (isWeekendDaysAttributePresent) {

            // Parse the "weekendDays" JSON attribute and load 'dayOfWeekSet'
            // with the result.

            bdlt::DayOfWeekSet dayOfWeekSet;

            // For the purposes of this Usage, we hard-wire the weekend days
            // that are hypothetically parsed from the 'json' string, and set
            // the 'rc' status flag indicating that parsing succeeded.

            dayOfWeekSet.add(bdlt::DayOfWeek::e_SUN);
            dayOfWeekSet.add(bdlt::DayOfWeek::e_SAT);
            rc = 0;  // success parsing "weekendDays" attribute

            if (rc != 0) {
                return 3;                                             // RETURN
            }

            result->addWeekendDays(dayOfWeekSet);
        }
//..
// Now, we parse the "holidays" attribute from 'json' and load the result into
// a 'bsl::vector<bdlt::Date>' object, 'holidays':
//..
        // For the purposes of this Usage, we hard-wire a boolean flag
        // indicating that the "holidays" attribute was hypothetically detected
        // in the 'json' string.

        bool isHolidaysAttributePresent = true;

        if (isHolidaysAttributePresent) {

            // Parse the "holidays" JSON attribute and load 'holidays' with the
            // result.

            bsl::vector<bdlt::Date> holidays;

            // For the purposes of this Usage, we hard-wire the holidays that
            // are hypothetically parsed from the 'json' string, and set the
            // 'rc' status flag indicating that parsing succeeded.

            holidays.push_back(bdlt::Date(1990,  5, 28));  // Memorial Day
            holidays.push_back(bdlt::Date(1990,  7,  4));  // Independence Day
            holidays.push_back(bdlt::Date(1990,  9,  3));  // Labor Day
            rc = 0;  // success parsing "holidays" attribute

            if (rc != 0) {
                return 4;                                             // RETURN
            }

            bsl::vector<bdlt::Date>::const_iterator it = holidays.begin();

            while (it != holidays.end()) {
                const bdlt::Date& holiday = *it;

                if (holiday < firstDate || holiday > lastDate) {
                    return 5;                                         // RETURN
                }

                result->addHoliday(holiday);

                ++it;
            }
        }
//..
// Note that the 'addHoliday' method can extend the range of the calendar.  Our
// calendar loader instead imposes the requirement that the dates specified in
// the "holidays" JSON attribute must be within the range
// '[firstDate .. lastDate]'.
//
// Finally, we return 0 indicating success:
//..
        return 0;
    }
//..

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
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

namespace {

typedef bdlt::CalendarLoader ProtocolClass;

struct ProtocolClassTestImp : bsls::ProtocolTestImp<ProtocolClass> {
    int load(bdlt::PackedCalendar *, const char *) { return markDone(); }
};

}  // close unnamed namespace

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int         test = argc > 1 ? atoi(argv[1]) : 0;
    bool     verbose = argc > 2;
    bool veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

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
        //: 1 Incorporate usage example from header into test driver, replace
        //:   leading comment characters with spaces, replace 'assert' with
        //:   'ASSERT', and insert 'if (veryVerbose)' before all output
        //:   operations.  (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

        bdlt::PackedCalendar calendar;
        MyCalendarLoader     loader;

        if (veryVerbose) {
            cout << "BEFORE:" << endl;
            calendar.print(cout);
        }

        ASSERT(0 == loader.load(&calendar, "dummyCalendarName"));

        if (veryVerbose) {
            cout << endl << "AFTER:" << endl;
            calendar.print(cout);
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // PROTOCOL TEST
        //   Ensure this class is a properly defined protocol.
        //
        // Concerns:
        //: 1 The protocol is abstract: no objects of it can be created.
        //:
        //: 2 The protocol has no data members.
        //:
        //: 3 The protocol has a virtual destructor.
        //:
        //: 4 All methods of the protocol are pure virtual.
        //:
        //: 5 All methods of the protocol are publicly accessible.
        //
        // Plan:
        //: 1 Define a concrete derived implementation, 'ProtocolClassTestImp',
        //:   of the protocol.
        //:
        //: 2 Create an object of the 'bsls::ProtocolTest' class template
        //:   parameterized by 'ProtocolClassTestImp', and use it to verify
        //:   that:
        //:
        //:   1 The protocol is abstract. (C-1)
        //:
        //:   2 The protocol has no data members. (C-2)
        //:
        //:   3 The protocol has a virtual destructor. (C-3)
        //:
        //: 3 Use the 'BSLS_PROTOCOLTEST_ASSERT' macro to verify that
        //:   non-creator methods of the protocol are:
        //:
        //:   1 virtual, (C-4)
        //:
        //:   2 publicly accessible. (C-5)
        //
        // Testing:
        //   virtual ~CalendarLoader();
        //   virtual int load(PackedCalendar *result, const char *name) = 0;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "PROTOCOL TEST" << endl
                                  << "=============" << endl;

        if (verbose) cout << "\nCreate a test object.\n";

        bsls::ProtocolTest<ProtocolClassTestImp> testObj(veryVerbose);

        if (verbose) cout << "\nVerify that the protocol is abstract.\n";

        ASSERT(testObj.testAbstract());

        if (verbose) cout << "\nVerify that there are no data members.\n";

        ASSERT(testObj.testNoDataMembers());

        if (verbose) cout << "\nVerify that the destructor is virtual.\n";

        ASSERT(testObj.testVirtualDestructor());

        if (verbose) cout << "\nVerify that methods are public and virtual.\n";

        BSLS_PROTOCOLTEST_ASSERT(testObj, load(0, 0));

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
