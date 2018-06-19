// bdlt_timetableloader.t.cpp                                         -*-C++-*-
#include <bdlt_timetableloader.h>

#include <bdlt_date.h>          // for testing only
#include <bdlt_dayofweek.h>     // for testing only
#include <bdlt_dayofweekset.h>  // for testing only
#include <bdlt_timetable.h>     // for testing only

#include <bslim_testutil.h>

#include <bsls_protocoltest.h>

#include <bsl_cstdlib.h>        // 'atoi'
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
// provide an interface for loading timetables.
//
// Global Concerns:
//: o The test driver is robust w.r.t. reuse in other, similar components.
//: o It is possible to create a concrete implementation the protocol.
// ----------------------------------------------------------------------------
// CREATORS
// [ 1] virtual ~TimetableLoader();
//
// MANIPULATORS
// [ 1] virtual int load(Timetable *result, const char *name) = 0;
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
///Example 1: Implementing the 'bdlt::TimetableLoader' Protocol
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates an elided concrete implementation of the
// 'bdlt::TimetableLoader' protocol that interprets timetable information
// contained in ASCII strings that are formatted using JSON.  Note that, in
// general, an implementation of 'bdlt::TimetableLoader' must obtain timetable
// information from *some* data source.  Our elided implementation leaves it
// unspecified as to where the JSON strings are obtained (i.e., whether from a
// file system, a database, a local or remote service, etc.).
//
// First, we show the JSON format that our timetable loader accepts:
//..
//  {
//      "firstDate":             "YYYY-MM-DD",
//      "lastDate":              "YYYY-MM-DD",
//      "initialTransitionCode": code,
//      "transitions":
//        [
//          {
//            "datetime": "YYYY-MM-DDThh:mm:ss",
//            "code":     code
//          }
//        ]
//  }
//..
// Note that "YYYY-MM-DD" is an ISO 8601 representation for the value of a
// 'bdlt::Date' object, "YYYY-MM-DDThh:mm:ss" is an ISO 8601 representation for
// the value of a 'bdlt::Datetime' object, and 'code' is an integer greater
// than or equal to -1 (with -1 used to specify
// 'bdlt::Timetable::k_UNSET_TRANSITION_CODE').  We assume that the four JSON
// attributes, "firstDate", "lastDate", "initialTransitionCode", and
// "transitions", must occur in the JSON string in the order in which they
// appear in the above display, but only "firstDate" and "lastDate" are
// *required* attributes.
//
// Then, we define the interface of our implementation:
//..
    class MyTimetableLoader : public bdlt::TimetableLoader {
        // This class provides a concrete implementation of the
        // 'bdlt::TimetableLoader' protocol (an abstract interface) for loading
        // a timetable.  This elided implementation obtains timetable
        // information from ASCII strings formatted using JSON.  The source of
        // the strings is unspecified.

      public:
        // CREATORS
        MyTimetableLoader();
            // Create a 'MyTimetableLoader' object.

        virtual ~MyTimetableLoader();
            // Destroy this object.

        // MANIPULATORS
        virtual int load(bdlt::Timetable *result, const char *timetableName);
            // Load, into the specified 'result', the timetable identified by
            // the specified 'timetableName'.  Return 0 on success, and a
            // non-zero value otherwise.  If the timetable corresponding to
            // 'timetableName' is not found, 1 is returned with no effect on
            // '*result'.  If a non-zero value other than 1 is returned
            // (indicating a different error), '*result' is valid, but its
            // value is undefined.
    };
//..
// Next, we implement the creators, trivially, as 'MyTimetableLoader' does not
// contain any instance data members:
//..
    // CREATORS
    inline
    MyTimetableLoader::MyTimetableLoader()
    {
    }

    inline
    MyTimetableLoader::~MyTimetableLoader()
    {
    }
//..
// Then, we implement the 'load' function:
//..
    // MANIPULATORS
    int MyTimetableLoader::load(bdlt::Timetable *result,
                                const char      * /* timetableName */)
    {
//..
// Next, we look up the timetable identified by 'timetableName' and load the
// corresponding text into a 'bsl::string' object, 'json' (as stated earlier,
// we do not specify in this example from where the timetable information is
// obtained):
//..
        // Obtain the information for the timetable identified by
        // 'timetableName' from the unspecified data source and load it into
        // the 'json' string.

        bsl::string json;

        // Since a JSON parser is not available to 'bdlt', this example assumes
        // that 'json' is populated with the following specific data:
        //..
        //  {
        //      "firstDate":             "2018-01-01",
        //      "lastDate":              "2018-12-31",
        //      "initialTransitionCode": 1,
        //      "transitions":
        //        [
        //          {
        //            "datetime": "2018-05-28T09:00:00",
        //            "code":     2
        //          },
        //          {
        //            "datetime": "2018-05-28T18:00:00",
        //            "code":     3
        //          }
        //        ]
        //  }
        //..
        // Similarly, we hard-wire the value of a status flag, 'rc', to
        // indicate that this string was successfully retrieved from the data
        // source.

        int rc = 0;  // success obtaining timetable information

        if (rc != 0) {
            return 1;                                                 // RETURN
        }
//..
// Note that the non-zero value 1 is returned only in the case where the
// timetable information corresponding to 'timetableName' cannot be found (per
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

        firstDate.setYearMonthDay(2018,  1,  1);
        lastDate.setYearMonthDay( 2018, 12, 31);
        rc = 0;  // success parsing "firstDate" and "lastDate" attributes

        if (rc != 0 || firstDate > lastDate) {
            return 2;                                                 // RETURN
        }

        result->reset();

        result->setValidRange(firstDate, lastDate);
//..
// Next, we parse the "initialTransitionCode" attribute from 'json':
//..
        // For the purposes of this Usage, we hard-wire a boolean flag
        // indicating that the "initialTransitionCode" attribute was
        // hypothetically detected in the 'json' string.

        bool isInitialTransitionCodePresent = true;

        if (isInitialTransitionCodePresent) {

            // For the purposes of this Usage, we hard-wire the initial
            // transition code that is hypothetically parsed from the 'json'
            // string, and set the 'rc' status flag indicating that parsing
            // succeeded.

            int code = 1;

            rc = 0;  // success parsing "initialTransitionCode" attribute

            if (rc != 0) {
                return 3;                                             // RETURN
            }

            result->setInitialTransitionCode(code);
        }
//..
// Now, we parse the "transitions" attribute from 'json' and load the result
// into a 'bsl::vector<bsl::pair<bdlt::Datetime, int> >' object, 'transitions':
//..
        // For the purposes of this Usage, we hard-wire a boolean flag
        // indicating that the "transitions" attribute was hypothetically
        // detected in the 'json' string.

        bool isTransitionsPresent = true;

        if (isTransitionsPresent) {

            // Parse the "transitions" JSON attribute and load 'transitions'
            // with the result.

            bsl::vector<bsl::pair<bdlt::Datetime, int> > transitions;

            // For the purposes of this Usage, we hard-wire the transitions
            // that are hypothetically parsed from the 'json' string, and set
            // the 'rc' status flag indicating that parsing succeeded.

            transitions.push_back(bsl::pair<bdlt::Datetime, int>(
                                          bdlt::Datetime(2018, 5, 28,  9), 2));
            transitions.push_back(bsl::pair<bdlt::Datetime, int>(
                                          bdlt::Datetime(2018, 5, 28, 18), 3));

            rc = 0;  // success parsing "transitions" attribute

            if (rc != 0) {
                return 4;                                             // RETURN
            }

            bsl::vector<bsl::pair<bdlt::Datetime, int> >::const_iterator it =
                                                           transitions.begin();

            while (it != transitions.end()) {
                const bdlt::Date& date = it->first.date();

                if (date < firstDate || date > lastDate || -1 > it->second) {
                    return 5;                                         // RETURN
                }

                result->addTransition(
                               it->first,
                               it->second >= 0
                                   ? it->second
                                   : bdlt::Timetable::k_UNSET_TRANSITION_CODE);

                ++it;
            }
        }
//..
// Our timetable loader imposes the requirement that the dates specified in the
// "transitions" JSON attribute must be within the range
// '[firstDate .. lastDate]' and the transition codes are non-negative or
// unset.
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

typedef bdlt::TimetableLoader ProtocolClass;

struct ProtocolClassTestImp : bsls::ProtocolTestImp<ProtocolClass> {
    int load(bdlt::Timetable *, const char *) { return markDone(); }
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

        bdlt::Timetable timetable;
        MyTimetableLoader     loader;

        if (veryVerbose) {
            cout << "BEFORE:" << endl;
            timetable.print(cout);
        }

        ASSERT(0 == loader.load(&timetable, "dummyTimetableName"));

        if (veryVerbose) {
            cout << endl << "AFTER:" << endl;
            timetable.print(cout);
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
        //   virtual ~TimetableLoader();
        //   virtual int load(Timetable *result, const char *name) = 0;
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
