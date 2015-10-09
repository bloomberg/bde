// bdlt_calendarloader.h                                              -*-C++-*-
#ifndef INCLUDED_BDLT_CALENDARLOADER
#define INCLUDED_BDLT_CALENDARLOADER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a protocol (or pure interface) for loading calendars.
//
//@CLASSES:
//  bdlt::CalendarLoader: pure interface for loading calendars
//
//@SEE_ALSO: bdlt_calendarcache, bdlt_packedcalendar
//
//@DESCRIPTION: This component provides a protocol, 'bdlt::CalendarLoader', for
// loading calendars from a specific source.  Each repository of calendar
// information can be supported by a distinct implementation of the
// 'CalendarLoader' protocol.  The protocol's primary method, 'load', loads a
// calendar into a 'bdlt::PackedCalendar' object.  The calendar to load is
// identified by name, which is specified by a null-terminated C-style string
// (i.e., 'const char *').
//
///Thread Safety
///-------------
// Unless otherwise documented, a single calendar loader object is not safe for
// concurrent access by multiple threads.  Classes derived from
// 'bdlt::CalendarLoader' that are specifically designed for concurrent access
// must be documented as such.  Unless specifically documented otherwise,
// separate objects of classes derived from 'bdlt::CalendarLoader' may safely
// be used in separate threads.
//
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
//  class MyCalendarLoader : public bdlt::CalendarLoader {
//      // This class provides a concrete implementation of the
//      // 'bdlt::CalendarLoader' protocol (an abstract interface) for loading
//      // a calendar.  This elided implementation obtains calendar information
//      // from ASCII strings formatted using JSON.  The source of the strings
//      // is unspecified.
//
//    public:
//      // CREATORS
//      MyCalendarLoader();
//          // Create a 'MyCalendarLoader' object.
//
//      virtual ~MyCalendarLoader();
//          // Destroy this object.
//
//      // MANIPULATORS
//      virtual int load(bdlt::PackedCalendar *result,
//                       const char           *calendarName);
//          // Load, into the specified 'result', the calendar identified by
//          // the specified 'calendarName'.  Return 0 on success, and a
//          // non-zero value otherwise.  If the calendar corresponding to
//          // 'calendarName' is not found, 1 is returned with no effect on
//          // '*result'.  If a non-zero value other than 1 is returned
//          // (indicating a different error), '*result' is valid, but its
//          // value is undefined.
//  };
//..
// Next, we implement the creators, trivially, as 'MyCalendarLoader' does not
// contain any instance data members:
//..
//  // CREATORS
//  inline
//  MyCalendarLoader::MyCalendarLoader()
//  {
//  }
//
//  inline
//  MyCalendarLoader::~MyCalendarLoader()
//  {
//  }
//..
// Then, we implement the 'load' function:
//..
//  // MANIPULATORS
//  int MyCalendarLoader::load(bdlt::PackedCalendar *result,
//                             const char           *calendarName)
//  {
//..
// Next, we look up the calendar identified by 'calendarName' and load the
// corresponding text into a 'bsl::string' object, 'json' (as stated earlier,
// we do not specify in this example from where the calendar information is
// obtained):
//..
//      // Obtain the information for the calendar identified by 'calendarName'
//      // from the unspecified data source and load it into the 'json' string.
//
//      bsl::string json;
//
//      // Since a JSON parser is not available to 'bdlt', this example assumes
//      // that 'json' is populated with the following specific data:
//      //..
//      //  {
//      //      "firstDate":   "1990-01-01",
//      //      "lastDate":    "1990-12-31",
//      //      "weekendDays": [ 1, 7 ],
//      //      "holidays":    [ "1990-05-28", "1990-07-04", "1990-09-03" ]
//      //  }
//      //..
//      // Similarly, we hard-wire the value of a status flag, 'rc', to
//      // indicate that this string was successfully retrieved from the data
//      // source.
//
//      int rc = 0;  // success obtaining calendar information
//
//      if (rc != 0) {
//          return 1;                                                 // RETURN
//      }
//..
// Note that the non-zero value 1 is returned only in the case where the
// calendar information corresponding to 'calendarName' cannot be found (per
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
//      firstDate.setYearMonthDay(1990,  1,  1);
//      lastDate.setYearMonthDay( 1990, 12, 31);
//      rc = 0;  // success parsing "firstDate" and "lastDate" attributes
//
//      if (rc != 0 || firstDate > lastDate) {
//          return 2;                                                 // RETURN
//      }
//
//      result->removeAll();
//
//      result->setValidRange(firstDate, lastDate);
//..
// Next, we parse the "weekendDays" attribute from 'json' and load the result
// into a 'bdlt::DayOfWeekSet' object, 'dayOfWeekSet':
//..
//      // For the purposes of this Usage, we hard-wire a boolean flag
//      // indicating that the "weekendDays" attribute was hypothetically
//      // detected in the 'json' string.
//
//      bool isWeekendDaysAttributePresent = true;
//
//      if (isWeekendDaysAttributePresent) {
//
//          // Parse the "weekendDays" JSON attribute and load 'dayOfWeekSet'
//          // with the result.
//
//          bdlt::DayOfWeekSet dayOfWeekSet;
//
//          // For the purposes of this Usage, we hard-wire the weekend days
//          // that are hypothetically parsed from the 'json' string, and set
//          // the 'rc' status flag indicating that parsing succeeded.
//
//          dayOfWeekSet.add(bdlt::DayOfWeek::e_SUN);
//          dayOfWeekSet.add(bdlt::DayOfWeek::e_SAT);
//          rc = 0;  // success parsing "weekendDays" attribute
//
//          if (rc != 0) {
//              return 3;                                             // RETURN
//          }
//
//          result->addWeekendDays(dayOfWeekSet);
//      }
//..
// Now, we parse the "holidays" attribute from 'json' and load the result into
// a 'bsl::vector<bdlt::Date>' object, 'holidays':
//..
//      // For the purposes of this Usage, we hard-wire a boolean flag
//      // indicating that the "holidays" attribute was hypothetically detected
//      // in the 'json' string.
//
//      bool isHolidaysAttributePresent = true;
//
//      if (isHolidaysAttributePresent) {
//
//          // Parse the "holidays" JSON attribute and load 'holidays' with the
//          // result.
//
//          bsl::vector<bdlt::Date> holidays;
//
//          // For the purposes of this Usage, we hard-wire the holidays that
//          // are hypothetically parsed from the 'json' string, and set the
//          // 'rc' status flag indicating that parsing succeeded.
//
//          holidays.push_back(bdlt::Date(1990,  5, 28));  // Memorial Day
//          holidays.push_back(bdlt::Date(1990,  7,  4));  // Independence Day
//          holidays.push_back(bdlt::Date(1990,  9,  3));  // Labor Day
//          rc = 0;  // success parsing "holidays" attribute
//
//          if (rc != 0) {
//              return 4;                                             // RETURN
//          }
//
//          bsl::vector<bdlt::Date>::const_iterator it = holidays.begin();
//
//          while (it != holidays.end()) {
//              const bdlt::Date& holiday = *it;
//
//              if (holiday < firstDate || holiday > lastDate) {
//                  return 5;                                         // RETURN
//              }
//
//              result->addHoliday(holiday);
//
//              ++it;
//          }
//      }
//..
// Note that the 'addHoliday' method can extend the range of the calendar.  Our
// calendar loader instead imposes the requirement that the dates specified in
// the "holidays" JSON attribute must be within the range
// '[firstDate .. lastDate]'.
//
// Finally, we return 0 indicating success:
//..
//      return 0;
//  }
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

namespace BloombergLP {
namespace bdlt {

class PackedCalendar;

                         // ====================
                         // class CalendarLoader
                         // ====================

class CalendarLoader {
    // This class defines a protocol used to load calendars from a specific
    // source.  Each repository of calendar information can be supported by a
    // distinct implementation of this protocol.

  public:
    // CREATORS
    virtual ~CalendarLoader();
        // Destroy this object.

    // MANIPULATORS
    virtual int load(PackedCalendar *result, const char *calendarName) = 0;
        // Load, into the specified 'result', the calendar identified by the
        // specified 'calendarName'.  Return 0 on success, and a non-zero value
        // otherwise.  If the calendar corresponding to 'calendarName' is not
        // found, 1 is returned with no effect on '*result'.  If a non-zero
        // value other than 1 is returned (indicating a different error),
        // '*result' is valid, but its value is undefined.
};

}  // close package namespace
}  // close enterprise namespace

#endif

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
