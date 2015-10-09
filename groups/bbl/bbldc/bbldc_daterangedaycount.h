// bbldc_daterangedaycount.h                                          -*-C++-*-
#ifndef INCLUDED_BBLDC_DATERANGEDAYCOUNT
#define INCLUDED_BBLDC_DATERANGEDAYCOUNT

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a protocol for date-range limited day-count calculations.
//
//@CLASSES:
//  bbldc::DateRangeDayCount: protocol for date-range limited day-counts
//
//@SEE_ALSO: bbldc_basicdaterangedaycountadapter,
//           bbldc_perioddaterangedaycountadapter
//
//@DESCRIPTION: This component provides a protocol, 'bbldc::DateRangeDayCount',
// for implementing an arbitrary day-count convention.  Concrete
// implementations of this protocol may implement, say, the BUS-252 day-count
// convention, or a custom day-count convention appropriate for some niche
// market.
//
// Several of the components in 'bbldc' provide individual day-count convention
// support through interfaces that are functionally identical to the abstract
// interface provided by this component, except that they do not inherit from
// 'bbldc::DateRangeDayCount'.  In conjunction with the adapter components
// (e.g., 'bbldc_basicdaterangedaycountadapter'), 'bbldc::DateRangeDayCount' is
// intended to allow run-time binding of these and other similar day-count
// implementations.
//
// This protocol requires two methods, 'firstDate' and 'lastDate', that define
// a date range for which calculations are valid, to reflect the valid range
// of, say, a calendar required for the computations.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Definition and Use of a Concrete Day-Count Convention
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example shows the definition and use of a simple concrete day-count
// convention.  This functionality suffices to demonstrate the requisite steps
// for having a working day-count convention:
//: * Define a concrete day-count type derived from 'bbldc::DateRangeDayCount'.
//:
//: * Implement the four pure virtual methods.
//:
//: * Instantiate and use an object of the concrete type.
//
// First, define the (derived) 'my_DayCountConvention' class and implement its
// constructor inline (for convenience, directly within the derived-class
// definition):
//..
//  // my_daycountconvention.h
//
//  class my_DayCountConvention : public bbldc::DateRangeDayCount {
//      bdlt::Date d_firstDate;
//      bdlt::Date d_lastDate;
//    public:
//      my_DayCountConvention()
//                         : d_firstDate(1, 1, 1), d_lastDate(9999, 12, 31) { }
//      virtual ~my_DayCountConvention();
//      virtual int daysDiff(const bdlt::Date& beginDate,
//                           const bdlt::Date& endDate) const;
//          // Return the (signed) number of days between the specified ...
//      virtual const bdlt::Date& firstDate() const;
//          // Return a reference providing non-modifiable access to the ...
//      virtual const bdlt::Date& lastDate() const;
//          // Return a reference providing non-modifiable access to the ...
//      virtual double yearsDiff(const bdlt::Date& beginDate,
//                               const bdlt::Date& endDate) const;
//          // Return the (signed fractional) number of years between the ...
//  };
//..
// Then, implement the destructor.  Note, however, that we always implement a
// virtual destructor (non-inline) in the .cpp file (to indicate the *unique*
// location of the class's virtual table):
//..
//  // my_daycountconvention.cpp
//
//  // ...
//
//  my_DayCountConvention::~my_DayCountConvention() { }
//..
// Next, we implement the (virtual) 'daysDiff', 'firstDate', 'lastDate', and
// 'yearsDiff' methods, which incorporate the "policy" of what it means for
// this day-count convention to calculate day count, year fraction, and the
// valid range of the convention instance:
//..
//  int my_DayCountConvention::daysDiff(const bdlt::Date& beginDate,
//                                      const bdlt::Date& endDate) const
//  {
//      return endDate - beginDate;
//  }
//
//  const bdlt::Date& my_DayCountConvention::firstDate() const
//  {
//      return d_firstDate;
//  }
//
//  const bdlt::Date& my_DayCountConvention::lastDate() const
//  {
//      return d_lastDate;
//  }
//
//  double my_DayCountConvention::yearsDiff(const bdlt::Date& beginDate,
//                                          const bdlt::Date& endDate) const
//  {
//      return static_cast<double>(endDate - beginDate) / 365.0;
//  }
//..
// Then, create two 'bdlt::Date' variables, 'd1' and 'd2', to use with the
// 'my_DayCountConvention' object and its day-count convention methods:
//..
//  const bdlt::Date d1(2003, 10, 19);
//  const bdlt::Date d2(2003, 12, 31);
//..
// Next, we obtain a 'bbldc::DateRangeDayCount' reference from an instantiated
// 'my_DayCountConvention':
//..
//  my_DayCountConvention           myDcc;
//  const bbldc::DateRangeDayCount& dcc = myDcc;
//..
// Now, we compute the day count between the two dates:
//..
//  const int daysDiff = dcc.daysDiff(d1, d2);
//  assert(73 == daysDiff);
//..
// Finally, we compute the year fraction between the two dates:
//..
//  const double yearsDiff = dcc.yearsDiff(d1, d2);
//  // Need fuzzy comparison since 'yearsDiff' is a 'double'.
//  assert(0.1999 < yearsDiff && 0.2001 > yearsDiff);
//..

#ifndef INCLUDED_BBLSCM_VERSION
#include <bblscm_version.h>
#endif

namespace BloombergLP {
namespace bdlt { class Date; }
namespace bbldc {

                         // =======================
                         // class DateRangeDayCount
                         // =======================

class DateRangeDayCount {
    // This 'class' provides a protocol for determining values based on dates
    // according to derived implementations of specific day-count conventions.
    // The methods 'firstDate' and 'lastDate' define a date range for which
    // calculations are valid, to reflect the valid range of, say, a calendar
    // required for the computations.

  public:
    // CREATORS
    virtual ~DateRangeDayCount();
        // Destroy this object.

    // ACCESSORS
    virtual int daysDiff(const bdlt::Date& beginDate,
                         const bdlt::Date& endDate) const = 0;
        // Return the (signed) number of days between the specified 'beginDate'
        // and 'endDate'.  If 'beginDate <= endDate', then the result is
        // non-negative.  The behavior is undefined unless
        // 'firstDate() <= beginDate <= lastDate()' and
        // 'firstDate() <= endDate <= lastDate()'.  Note that reversing the
        // order of 'beginDate' and 'endDate' negates the result.

    virtual const bdlt::Date& firstDate() const = 0;
        // Return a reference providing non-modifiable access to the earliest
        // date in the valid range of this day-count convention.

    virtual const bdlt::Date& lastDate() const = 0;
        // Return a reference providing non-modifiable access to the latest
        // date in the valid range of this day-count convention.

    virtual double yearsDiff(const bdlt::Date& beginDate,
                             const bdlt::Date& endDate) const = 0;
        // Return the (signed fractional) number of years between the specified
        // 'beginDate' and 'endDate'.  If 'beginDate <= endDate', then the
        // result is non-negative.  The behavior is undefined unless
        // 'firstDate() <= beginDate <= lastDate()' and
        // 'firstDate() <= endDate <= lastDate()'.  Note that reversing the
        // order of 'beginDate' and 'endDate' negates the result; specifically,
        // '|yearsDiff(b, e) + yearsDiff(e, b)| <= 1.0e-15' for all dates 'b'
        // and 'e'.
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
