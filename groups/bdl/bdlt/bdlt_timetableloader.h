// bdlt_timetableloader.h                                             -*-C++-*-
#ifndef INCLUDED_BDLT_TIMETABLELOADER
#define INCLUDED_BDLT_TIMETABLELOADER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a protocol (or pure interface) for loading timetables.
//
//@CLASSES:
//  bdlt::TimetableLoader: pure interface for loading timetables
//
//@SEE_ALSO: bdlt_timetablecache, bdlt_timetable
//
//@DESCRIPTION: This component provides a protocol, 'bdlt::TimetableLoader',
// for loading timetables from a specific source.  Each repository of timetable
// information can be supported by a distinct implementation of the
// 'TimetableLoader' protocol.  The protocol's primary method, 'load', loads a
// timetable into a 'bdlt::Timetable' object.  The timetable to load is
// identified by name, which is specified by a null-terminated C-style string
// (i.e., 'const char *').
//
///Thread Safety
///-------------
// Unless otherwise documented, a single timetable loader object is not safe
// for concurrent access by multiple threads.  Classes derived from
// 'bdlt::TimetableLoader' that are specifically designed for concurrent access
// must be documented as such.  Unless specifically documented otherwise,
// separate objects of classes derived from 'bdlt::TimetableLoader' may safely
// be used in separate threads.
//
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
//  class MyTimetableLoader : public bdlt::TimetableLoader {
//      // This class provides a concrete implementation of the
//      // 'bdlt::TimetableLoader' protocol (an abstract interface) for loading
//      // a timetable.  This elided implementation obtains timetable
//      // information from ASCII strings formatted using JSON.  The source of
//      // the strings is unspecified.
//
//    public:
//      // CREATORS
//      MyTimetableLoader();
//          // Create a 'MyTimetableLoader' object.
//
//      virtual ~MyTimetableLoader();
//          // Destroy this object.
//
//      // MANIPULATORS
//      virtual int load(bdlt::Timetable *result, const char *timetableName);
//          // Load, into the specified 'result', the timetable identified by
//          // the specified 'timetableName'.  Return 0 on success, and a
//          // non-zero value otherwise.  If the timetable corresponding to
//          // 'timetableName' is not found, 1 is returned with no effect on
//          // '*result'.  If a non-zero value other than 1 is returned
//          // (indicating a different error), '*result' is valid, but its
//          // value is undefined.
//  };
//..
// Next, we implement the creators, trivially, as 'MyTimetableLoader' does not
// contain any instance data members:
//..
//  // CREATORS
//  inline
//  MyTimetableLoader::MyTimetableLoader()
//  {
//  }
//
//  inline
//  MyTimetableLoader::~MyTimetableLoader()
//  {
//  }
//..
// Then, we implement the 'load' function:
//..
//  // MANIPULATORS
//  int MyTimetableLoader::load(bdlt::Timetable *result,
//                              const char      * /* timetableName */)
//  {
//..
// Next, we look up the timetable identified by 'timetableName' and load the
// corresponding text into a 'bsl::string' object, 'json' (as stated earlier,
// we do not specify in this example from where the timetable information is
// obtained):
//..
//      // Obtain the information for the timetable identified by
//      // 'timetableName' from the unspecified data source and load it into
//      // the 'json' string.
//
//      bsl::string json;
//
//      // Since a JSON parser is not available to 'bdlt', this example assumes
//      // that 'json' is populated with the following specific data:
//      //..
//      //  {
//      //      "firstDate":             "2018-01-01",
//      //      "lastDate":              "2018-12-31",
//      //      "initialTransitionCode": 1,
//      //      "transitions":
//      //        [
//      //          {
//      //            "datetime": "2018-05-28T09:00:00",
//      //            "code":     2
//      //          },
//      //          {
//      //            "datetime": "2018-05-28T18:00:00",
//      //            "code":     3
//      //          }
//      //        ]
//      //  }
//      //..
//      // Similarly, we hard-wire the value of a status flag, 'rc', to
//      // indicate that this string was successfully retrieved from the data
//      // source.
//
//      int rc = 0;  // success obtaining timetable information
//
//      if (rc != 0) {
//          return 1;                                                 // RETURN
//      }
//..
// Note that the non-zero value 1 is returned only in the case where the
// timetable information corresponding to 'timetableName' cannot be found (per
// the contract for the 'load' method).
//
// Then, we parse the "firstDate" and "lastDate" attributes from the 'json'
// string, loading the results into like-named variables:
//..
//      // Parse the "firstDate" and "lastDate" JSON attributes and load the
//      // results into 'firstDate' and 'lastDate', respectively.  It is an
//      // error if either of the "firstDate" or "lastDate" attributes are
//      // missing, or if they are out of order.
//
//      bdlt::Date firstDate;
//      bdlt::Date lastDate;
//
//      // For the purposes of this Usage, we hard-wire the first and last
//      // dates that are hypothetically parsed from the 'json' string, and
//      // set the 'rc' status flag indicating that parsing succeeded.
//
//      firstDate.setYearMonthDay(2018,  1,  1);
//      lastDate.setYearMonthDay( 2018, 12, 31);
//      rc = 0;  // success parsing "firstDate" and "lastDate" attributes
//
//      if (rc != 0 || firstDate > lastDate) {
//          return 2;                                                 // RETURN
//      }
//
//      result->reset();
//
//      result->setValidRange(firstDate, lastDate);
//..
// Next, we parse the "initialTransitionCode" attribute from 'json':
//..
//      // For the purposes of this Usage, we hard-wire a boolean flag
//      // indicating that the "initialTransitionCode" attribute was
//      // hypothetically detected in the 'json' string.
//
//      bool isInitialTransitionCodePresent = true;
//
//      if (isInitialTransitionCodePresent) {
//
//          // For the purposes of this Usage, we hard-wire the initial
//          // transition code that is hypothetically parsed from the 'json'
//          // string, and set the 'rc' status flag indicating that parsing
//          // succeeded.
//
//          int code = 1;
//
//          rc = 0;  // success parsing "initialTransitionCode" attribute
//
//          if (rc != 0) {
//              return 3;                                             // RETURN
//          }
//
//          result->setInitialTransitionCode(code);
//      }
//..
// Now, we parse the "transitions" attribute from 'json' and load the result
// into a 'bsl::vector<bsl::pair<bdlt::Datetime, int> >' object, 'transitions':
//..
//      // For the purposes of this Usage, we hard-wire a boolean flag
//      // indicating that the "transitions" attribute was hypothetically
//      // detected in the 'json' string.
//
//      bool isTransitionsPresent = true;
//
//      if (isTransitionsPresent) {
//
//          // Parse the "transitions" JSON attribute and load 'transitions'
//          // with the result.
//
//          bsl::vector<bsl::pair<bdlt::Datetime, int> > transitions;
//
//          // For the purposes of this Usage, we hard-wire the transitions
//          // that are hypothetically parsed from the 'json' string, and set
//          // the 'rc' status flag indicating that parsing succeeded.
//
//          transitions.push_back(bsl::pair<bdlt::Datetime, int>(
//                                        bdlt::Datetime(2018, 5, 28,  9), 2));
//          transitions.push_back(bsl::pair<bdlt::Datetime, int>(
//                                        bdlt::Datetime(2018, 5, 28, 18), 3));
//
//          rc = 0;  // success parsing "transitions" attribute
//
//          if (rc != 0) {
//              return 4;                                             // RETURN
//          }
//
//          bsl::vector<bsl::pair<bdlt::Datetime, int> >::const_iterator it =
//                                                         transitions.begin();
//
//          while (it != transitions.end()) {
//              const bdlt::Date& date = it->first.date();
//
//              if (date < firstDate || date > lastDate || -1 > it->second) {
//                  return 5;                                         // RETURN
//              }
//
//              result->addTransition(
//                             it->first,
//                             it->second >= 0
//                                 ? it->second
//                                 : bdlt::Timetable::k_UNSET_TRANSITION_CODE);
//
//              ++it;
//          }
//      }
//..
// Our timetable loader imposes the requirement that the dates specified in the
// "transitions" JSON attribute must be within the range
// '[firstDate .. lastDate]' and the transition codes are non-negative or
// unset.
//
// Finally, we return 0 indicating success:
//..
//      return 0;
//  }
//..

#include <bdlscm_version.h>

namespace BloombergLP {
namespace bdlt {

class Timetable;

                          // =====================
                          // class TimetableLoader
                          // =====================

class TimetableLoader {
    // This class defines a protocol used to load timetables from a specific
    // source.  Each repository of timetable information can be supported by a
    // distinct implementation of this protocol.

  public:
    // CREATORS
    virtual ~TimetableLoader();
        // Destroy this object.

    // MANIPULATORS
    virtual int load(Timetable *result, const char *timetableName) = 0;
        // Load, into the specified 'result', the timetable identified by the
        // specified 'timetableName'.  Return 0 on success, and a non-zero
        // value otherwise.  If the timetable corresponding to 'timetableName'
        // is not found, 1 is returned with no effect on '*result'.  If a
        // non-zero value other than 1 is returned (indicating a different
        // error), '*result' is valid, but its value is undefined.
};

}  // close package namespace
}  // close enterprise namespace

#endif

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
