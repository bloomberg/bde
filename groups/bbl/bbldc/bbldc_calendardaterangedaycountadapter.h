// bbldc_calendardaterangedaycountadapter.h                           -*-C++-*-
#ifndef INCLUDED_BBLDC_CALENDARDATERANGEDAYCOUNTADAPTER
#define INCLUDED_BBLDC_CALENDARDATERANGEDAYCOUNTADAPTER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a parameterized day-count convention implementation.
//
//@CLASSES:
//  bbldc::CalendarDateRangeDayCountAdapter: 'bbldc::DateRangeDayCount' adapter
//
//@DESCRIPTION: This component provides a parameterized (template)
// implementation, 'bbldc::CalendarDateRangeDayCountAdapter', of the
// 'bbldc::DateRangeDayCount' protocol.  The template argument can be any type
// supporting the following two class methods.
//..
//  int daysDiff(const bdlt::Date&     beginDate,
//               const bdlt::Date&     endDate,
//               const bdlt::Calendar& calendar);
//
//  double yearsDiff(const bdlt::Date&     beginDate,
//                   const bdlt::Date&     endDate,
//                   const bdlt::Calendar& calendar);
//..
// The template class 'bbldc::CalendarDateRangeDayCountAdapter' provides
// convenient support for run-time polymorphic choice of day-count conventions
// (via conventional use of a base-class pointer or reference) without having
// to implement each derived type explicitly.  In this sense,
// 'bbldc::CalendarDateRangeDayCountAdapter' adapts the various concrete
// calendar-based day-count convention classes (e.g., 'bbldc::CalendarBus252')
// to a run-time binding mechanism.
//
// The 'bbldc::DateRangeDayCount' protocol requires two methods, 'firstDate'
// and 'lastDate', that define a date range for which calculations are valid,
// to reflect the valid range of, say, a calendar required for the
// computations.  For "calendar" day-count implementations, the valid date
// range is the valid range of the calendar.
//
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
//  bdlt::Calendar calendar;
//  calendar.setValidRange(bdlt::Date(2003, 1, 1), bdlt::Date(2003, 12, 31));
//  calendar.addWeekendDay(bdlt::DayOfWeek::e_SUN);
//  calendar.addWeekendDay(bdlt::DayOfWeek::e_SAT);
//..
// Then, we define an instance of the adapted day-count convention and obtain a
// reference to the 'bbldc::DateRangeDayCount':
//..
//  const bbldc::CalendarDateRangeDayCountAdapter<bbldc::CalendarBus252>
//                                                             myDcc(calendar);
//  const bbldc::DateRangeDayCount&                            dcc = myDcc;
//..
// Next, create two 'bdlt::Date' variables, 'd1' and 'd2', with which to use
// the day-count convention methods:
//..
//  const bdlt::Date d1(2003, 10, 19);
//  const bdlt::Date d2(2003, 12, 31);
//..
// Now, use the base-class reference to compute the day count between the two
// dates:
//..
//  const int daysDiff = dcc.daysDiff(d1, d2);
//  assert(52 == daysDiff);
//..
// Finally, use the base-class reference to compute the year fraction between
// the two dates:
//..
//  const double yearsDiff = dcc.yearsDiff(d1, d2);
//  // Need fuzzy comparison since 'yearsDiff' is a 'double'.
//  assert(yearsDiff > 0.2063 && yearsDiff < 0.2064);
//..

#ifndef INCLUDED_BBLSCM_VERSION
#include <bblscm_version.h>
#endif

#ifndef INCLUDED_BBLDC_DATERANGEDAYCOUNT
#include <bbldc_daterangedaycount.h>
#endif

#ifndef INCLUDED_BDLT_CALENDAR
#include <bdlt_calendar.h>
#endif

#ifndef INCLUDED_BDLT_DATE
#include <bdlt_date.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP {
namespace bbldc {

                  // ======================================
                  // class CalendarDateRangeDayCountAdapter
                  // ======================================

template <class CONVENTION>
class CalendarDateRangeDayCountAdapter : public DateRangeDayCount {
    // This 'class' provides an "adapter" from the specified 'CONVENTION', that
    // requires a calendar to compute the day count and the year fraction, to
    // the 'bbldc::DateRangeDayCount' protocol that can be used for determining
    // values based on dates according to the day-count 'CONVENTION'.

    // DATA
    bdlt::Calendar d_calendar;  // calendar used in all calculations

  private:
    // NOT IMPLEMENTED
    CalendarDateRangeDayCountAdapter(const CalendarDateRangeDayCountAdapter&);
    CalendarDateRangeDayCountAdapter& operator=(
                                      const CalendarDateRangeDayCountAdapter&);

  public:
    // CREATORS
    CalendarDateRangeDayCountAdapter(
                                    const bdlt::Calendar&  calendar,
                                    bslma::Allocator      *basicAllocator = 0);
        // Create a day-count adapter that uses the specified 'calendar' during
        // invocations of 'daysDiff' and 'yearsDiff'.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    virtual ~CalendarDateRangeDayCountAdapter();
        // Destroy this object.

    // ACCESSORS
    int daysDiff(const bdlt::Date& beginDate, const bdlt::Date& endDate) const;
        // Return the (signed) number of days between the specified 'beginDate'
        // and 'endDate' as per the 'CONVENTION' template policy.  If
        // 'beginDate <= endDate', then the result is non-negative.  The
        // behavior is undefined unless, for the 'calendar' provided at
        // construction,
        // 'calendar.firstDate() <= beginDate <= calendar.lastDate()' and
        // 'calendar.firstDate() <= endDate <= calendar.lastDate()'.  Note that
        // reversing the order of 'beginDate' and 'endDate' negates the result.

    const bdlt::Date& firstDate() const;
        // Return a reference providing non-modifiable access to
        // 'calendar.firstDate()' for the 'calendar' provided at construction.
        // Note that this value is the earliest date in the valid range of this
        // day-count convention adaptation.

    const bdlt::Date& lastDate() const;
        // Return a reference providing non-modifiable access to
        // 'calendar.lastDate()' for the 'calendar' provided at construction.
        // Note that this value is the latest date in the valid range of this
        // day-count convention adaptation.

    double yearsDiff(const bdlt::Date& beginDate,
                     const bdlt::Date& endDate) const;
        // Return the (signed fractional) number of years between the specified
        // 'beginDate' and 'endDate' as per the 'CONVENTION' template policy.
        // If 'beginDate <= endDate', then the result is non-negative.  The
        // behavior is undefined unless, for the 'calendar' provided at
        // construction,
        // 'calendar.firstDate() <= beginDate <= calendar.lastDate()' and
        // 'calendar.firstDate() <= endDate <= calendar.lastDate()'.  Note that
        // reversing the order of 'beginDate' and 'endDate' negates the result;
        // specifically, '|yearsDiff(b, e) + yearsDiff(e, b)| <= 1.0e-15' for
        // all dates 'b' and 'e'.

                                  // Aspects

    bslma::Allocator *allocator() const;
        // Return the allocator used by this adapter to supply memory.
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                  // --------------------------------------
                  // class CalendarDateRangeDayCountAdapter
                  // --------------------------------------

// CREATORS
template <class CONVENTION>
inline
CalendarDateRangeDayCountAdapter<CONVENTION>::CalendarDateRangeDayCountAdapter(
                                         const bdlt::Calendar&  calendar,
                                         bslma::Allocator      *basicAllocator)
: d_calendar(calendar, basicAllocator)
{
}

template <class CONVENTION>
inline
CalendarDateRangeDayCountAdapter<CONVENTION>::
                                            ~CalendarDateRangeDayCountAdapter()
{
}

// ACCESSORS
template <class CONVENTION>
inline
int CalendarDateRangeDayCountAdapter<CONVENTION>::daysDiff(
                                               const bdlt::Date& beginDate,
                                               const bdlt::Date& endDate) const
{
    BSLS_ASSERT_SAFE(d_calendar.firstDate() <= beginDate);
    BSLS_ASSERT_SAFE(                      beginDate <= d_calendar.lastDate());
    BSLS_ASSERT_SAFE(d_calendar.firstDate() <= endDate);
    BSLS_ASSERT_SAFE(                        endDate <= d_calendar.lastDate());

    return CONVENTION::daysDiff(beginDate, endDate, d_calendar);
}

template <class CONVENTION>
inline
const bdlt::Date& CalendarDateRangeDayCountAdapter<CONVENTION>::
                                                              firstDate() const
{
    return d_calendar.firstDate();
}

template <class CONVENTION>
inline
const bdlt::Date& CalendarDateRangeDayCountAdapter<CONVENTION>::
                                                               lastDate() const
{
    return d_calendar.lastDate();
}

template <class CONVENTION>
inline
double CalendarDateRangeDayCountAdapter<CONVENTION>::yearsDiff(
                                               const bdlt::Date& beginDate,
                                               const bdlt::Date& endDate) const
{
    BSLS_ASSERT_SAFE(d_calendar.firstDate() <= beginDate);
    BSLS_ASSERT_SAFE(                      beginDate <= d_calendar.lastDate());
    BSLS_ASSERT_SAFE(d_calendar.firstDate() <= endDate);
    BSLS_ASSERT_SAFE(                        endDate <= d_calendar.lastDate());

    return CONVENTION::yearsDiff(beginDate, endDate, d_calendar);
}

                                  // Aspects

template <class CONVENTION>
inline
bslma::Allocator *CalendarDateRangeDayCountAdapter<CONVENTION>::
                                                              allocator() const
{
    return d_calendar.allocator();
}

}  // close package namespace
}  // close enterprise namespace

// TRAITS

namespace BloombergLP {
namespace bslma {

template <class CONVENTION>
struct UsesBslmaAllocator<bbldc::CalendarDateRangeDayCountAdapter<CONVENTION> >
                                                           : bsl::true_type {};

}  // close namespace bslma
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
