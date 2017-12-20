// bbldc_basicdaterangedaycountadapter.h                              -*-C++-*-
#ifndef INCLUDED_BBLDC_BASICDATERANGEDAYCOUNTADAPTER
#define INCLUDED_BBLDC_BASICDATERANGEDAYCOUNTADAPTER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a parameterized day-count convention implementation.
//
//@CLASSES:
//  bbldc::BasicDateRangeDayCountAdapter: 'bbldc::DateRangeDayCount' adapter
//
//@DESCRIPTION: This component provides a parameterized (template)
// implementation, 'bbldc::BasicDateRangeDayCountAdapter', of the
// 'bbldc::DateRangeDayCount' protocol.  The template argument can be any type
// supporting the following two class methods.
//..
//  int daysDiff(const bdlt::Date& beginDate, const bdlt::Date& endDate);
//
//  double yearsDiff(const bdlt::Date& beginDate, const bdlt::Date& endDate);
//..
// The template class 'bbldc::BasicDateRangeDayCountAdapter' provides
// convenient support for run-time polymorphic choice of day-count conventions
// (via conventional use of a base-class pointer or reference) without having
// to implement each derived type explicitly.  In this sense,
// 'bbldc::BasicDateRangeDayCountAdapter' adapts the various concrete "basic"
// day-count convention classes (e.g., 'bbldc::BasicIsma30360') to a run-time
// binding mechanism.
//
// The 'bbldc::DateRangeDayCount' protocol requires two methods, 'firstDate'
// and 'lastDate', that define a date range for which calculations are valid,
// to reflect the valid range of, say, a calendar required for the
// computations.  For "basic" day-count implementations, the valid date range
// is identical to the range of 'bdlt::Date'.
//
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
//  const bbldc::BasicDateRangeDayCountAdapter<bbldc::BasicIsma30360> myDcc =
//               bbldc::BasicDateRangeDayCountAdapter<bbldc::BasicIsma30360>();
//
//  const bbldc::DateRangeDayCount& dcc = myDcc;
//..
// Then, create two 'bdlt::Date' variables, 'd1' and 'd2', with which to use
// the day-count convention methods:
//..
//  const bdlt::Date d1(2003, 10, 18);
//  const bdlt::Date d2(2003, 12, 31);
//..
// Now, use the base-class reference to compute the day count between the two
// dates:
//..
//  const int daysDiff = dcc.daysDiff(d1, d2);
//  assert(72 == daysDiff);
//..
// Finally, use the base-class reference to compute the year fraction between
// the two dates:
//..
//  const double yearsDiff = dcc.yearsDiff(d1, d2);
//  // Need fuzzy comparison since 'yearsDiff' is a 'double'.
//  assert(0.1999 < yearsDiff && 0.2001 > yearsDiff);
//..

#ifndef INCLUDED_BBLSCM_VERSION
#include <bblscm_version.h>
#endif

#ifndef INCLUDED_BBLDC_DATERANGEDAYCOUNT
#include <bbldc_daterangedaycount.h>
#endif

#ifndef INCLUDED_BDLT_DATE
#include <bdlt_date.h>
#endif

namespace BloombergLP {
namespace bbldc {

                   // ===================================
                   // class BasicDateRangeDayCountAdapter
                   // ===================================

template <class CONVENTION>
class BasicDateRangeDayCountAdapter : public DateRangeDayCount {
    // This 'class' provides an "adapter" from the specified 'CONVENTION' to
    // the 'bbldc::DateRangeDayCount' protocol that can be used for determining
    // values based on dates according to the day-count 'CONVENTION'.

  public:
    // ACCESSORS
    int daysDiff(const bdlt::Date& beginDate, const bdlt::Date& endDate) const;
        // Return the (signed) number of days between the specified 'beginDate'
        // and 'endDate' as per the 'CONVENTION' template policy.  If
        // 'beginDate <= endDate', then the result is non-negative.  Note that
        // reversing the order of 'beginDate' and 'endDate' negates the result.

    const bdlt::Date& firstDate() const;
        // Return a reference providing non-modifiable access to a 'bdlt::Date'
        // with the value 1/1/1.  Note that this value is the earliest date in
        // the valid range of the adapted day-count convention.

    const bdlt::Date& lastDate() const;
        // Return a reference providing non-modifiable access to a 'bdlt::Date'
        // with the value 9999/12/31.  Note that this value is the latest date
        // in the valid range of the adapted day-count convention.

    double yearsDiff(const bdlt::Date& beginDate,
                     const bdlt::Date& endDate) const;
        // Return the (signed fractional) number of years between the specified
        // 'beginDate' and 'endDate' as per the 'CONVENTION' template policy.
        // If 'beginDate <= endDate', then the result is non-negative.  Note
        // that reversing the order of 'beginDate' and 'endDate' negates the
        // result; specifically,
        // '|yearsDiff(b, e) + yearsDiff(e, b)| <= 1.0e-15' for all dates 'b'
        // and 'e'.
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                   // -----------------------------------
                   // class BasicDateRangeDayCountAdapter
                   // -----------------------------------

// ACCESSORS
template <class CONVENTION>
inline
int BasicDateRangeDayCountAdapter<CONVENTION>::daysDiff(
                                               const bdlt::Date& beginDate,
                                               const bdlt::Date& endDate) const
{
    return CONVENTION::daysDiff(beginDate, endDate);
}

template <class CONVENTION>
inline
const bdlt::Date& BasicDateRangeDayCountAdapter<CONVENTION>::firstDate() const
{
    static bdlt::Date firstDate(1, 1, 1);
    return firstDate;
}

template <class CONVENTION>
inline
const bdlt::Date& BasicDateRangeDayCountAdapter<CONVENTION>::lastDate() const
{
    static bdlt::Date lastDate(9999, 12, 31);
    return lastDate;
}

template <class CONVENTION>
inline
double BasicDateRangeDayCountAdapter<CONVENTION>::yearsDiff(
                                               const bdlt::Date& beginDate,
                                               const bdlt::Date& endDate) const
{
    return CONVENTION::yearsDiff(beginDate, endDate);
}

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
