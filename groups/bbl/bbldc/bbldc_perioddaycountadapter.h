// bbldc_perioddaycountadapter.h                                      -*-C++-*-
#ifndef INCLUDED_BBLDC_PERIODDAYCOUNTADAPTER
#define INCLUDED_BBLDC_PERIODDAYCOUNTADAPTER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a parameterized day-count convention implementation.
//
//@CLASSES:
//  bbldc::PeriodDayCountAdapter: 'bbldc::DayCountInterface' realization
//
//@DESCRIPTION: This component provides a parameterized (template)
// implementation, 'bbldc::PeriodDayCountAdapter', of the
// 'bbldc::DayCountInterface' protocol.  The template argument can be any type
// supporting the following two class methods.
//..
//  int daysDiff(const bdlt::Date& beginDate,
//               const bdlt::Date& endDate) const;
//
//  double yearsDiff(const bdlt::Date&              beginDate,
//                   const bdlt::Date&              endDate,
//                   const bsl::vector<bdlt::Date>& periodDate,
//                   double                         periodYearDiff) const;
//..
// The template class 'bbldc::PeriodDayCountAdapter' provides convenient
// support for run-time polymorphic choice of day-count conventions (via
// conventional use of a base-class pointer or reference) without having to
// implement each derived type explicitly.  In this sense,
// 'bbldc::PeriodDayCountAdapter' adapts the various concrete period-based
// day-count convention classes (e.g., 'bbldc::PeriodIcmaActualActual') to a
// run-time binding mechanism.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Adapting 'bbldc::PeriodIcmaActualActual'
///- - - - - - - - - - - - - - - - - - - - - - - - - -
// This example shows the procedure for using 'bbldc::PeriodDayCountAdapter' to
// adapt the 'bbldc::PeriodIcmaActualActual' day-count convention to the
// 'bbldc::DayCountInterface', and then the use of the day-count methods.
// First, we create a schedule of period dates 'sched' corresponding to a
// quarterly payment ('periodYearDiff == 0.25'):
//..
//  bsl::vector<bdlt::Date> sched;
//  sched.push_back(bdlt::Date(2003, 10, 1));
//  sched.push_back(bdlt::Date(2004,  1, 1));
//..
// Then, we define an instance of the adapted day-count convention and obtain
// a reference to the 'bbldc::DayCountInterface':
//..
//  const bbldc::PeriodDayCountAdapter<bbldc::PeriodIcmaActualActual>
//                                                                 myDcc(sched,
//                                                                       0.25);
//  const bbldc::DayCountInterface&                                dcc = myDcc;
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
//  assert(73 == daysDiff);
//..
// Finally, use the base-class reference to compute the year fraction between
// the two dates:
//..
//  const double yearsDiff = dcc.yearsDiff(d1, d2);
//  // Need fuzzy comparison since 'yearsDiff' is a 'double'.
//  assert(yearsDiff > 0.1983 && yearsDiff < 0.1985);
//..

#ifndef INCLUDED_BBLSCM_VERSION
#include <bblscm_version.h>
#endif

#ifndef INCLUDED_BBLDC_DAYCOUNTINTERFACE
#include <bbldc_daycountinterface.h>
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

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_ITERATOR
#include <bsl_iterator.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

namespace bdlt {  class Date;  }

namespace bbldc {

                       // ===========================
                       // class PeriodDayCountAdapter
                       // ===========================

template <class CONVENTION>
class PeriodDayCountAdapter : public DayCountInterface {
    // This 'class' provides an "adapter" from the specified 'CONVENTION' to
    // the 'bbldc::DayCountInterface' that can be used for determining values
    // based on dates according to the day-count 'CONVENTION'.

    // DATA
    bsl::vector<bdlt::Date> d_periodDate;      // period starting dates

    double                  d_periodYearDiff;  // years for each period

  private:
    // PRIVATE ACCESSORS
    bool isSortedAndUnique(const bsl::vector<bdlt::Date>& periodDate) const;
        // Return 'true' if all values contained in the specified 'periodDate'
        // are unique and sorted from minimum to maximum value, and 'false'
        // otherwise.

    // NOT IMPLEMENTED
    PeriodDayCountAdapter();

  public:
    // CREATORS
    PeriodDayCountAdapter(const bsl::vector<bdlt::Date>&  periodDate,
                          double                          periodYearDiff,
                          bslma::Allocator               *basicAllocator = 0);
        // Create a day-count adapter that uses the specified 'periodDate' and
        // 'periodYearDiff' during invocations of 'yearsDiff'.  The
        // 'periodDate' provides the period starting dates and 'periodYearDiff'
        // defines the duration of each period (e.g., 0.25 for quarterly
        // periods).  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The behavior is undefined unless
        // 'periodDate.size() >= 2' and the values contained in 'periodDate'
        // are unique and sorted from minimum to maximum.

    virtual ~PeriodDayCountAdapter();
        // Destroy this object.

    // ACCESSORS
    int daysDiff(const bdlt::Date& beginDate, const bdlt::Date& endDate) const;
        // Return the (signed) number of days between the specified 'beginDate'
        // and 'endDate' as per the 'CONVENTION' template policy.  If
        // 'beginDate <= endDate', then the result is non-negative.  Note that
        // reversing the order of 'beginDate' and 'endDate' negates the result.

    double yearsDiff(const bdlt::Date& beginDate,
                     const bdlt::Date& endDate) const;
        // Return the (signed fractional) number of years between the specified
        // 'beginDate' and 'endDate' as per the 'CONVENTION' template policy.
        // If 'beginDate <= endDate', then the result is non-negative.  The
        // behavior is undefined unless, for the 'periodDate' provided at
        // construction, 'min(beginDate, endDate) >= periodDate.front()', and
        // 'max(beginDate, endDate) <= periodDate.back()'.  Note that reversing
        // the order of 'beginDate' and 'endDate' negates the result;
        // specifically, '|yearsDiff(b, e) + yearsDiff(e, b)| <= 1.0e-15' for
        // all dates 'b' and 'e'.

                                  // Aspects

    bslma::Allocator *allocator() const;
        // Return the allocator used by this adapter to supply memory.
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                       // ---------------------------
                       // class PeriodDayCountAdapter
                       // ---------------------------

// PRIVATE ACCESSORS
template <class CONVENTION>
bool PeriodDayCountAdapter<CONVENTION>::isSortedAndUnique(
                               const bsl::vector<bdlt::Date>& periodDate) const
{
    bsl::vector<bdlt::Date>::const_iterator begin = periodDate.begin();
    bsl::vector<bdlt::Date>::const_iterator end   = periodDate.end();

    if (begin == end) {
        return true;                                                  // RETURN
    }

    bsl::vector<bdlt::Date>::const_iterator prev = begin;
    bsl::vector<bdlt::Date>::const_iterator at   = begin + 1;

    while (at != end) {
        if (*prev >= *at) {
            return false;                                             // RETURN
        }
        prev = at++;
    }

    return true;
}

// CREATORS
template <class CONVENTION>
inline
PeriodDayCountAdapter<CONVENTION>::PeriodDayCountAdapter(
                                const bsl::vector<bdlt::Date>&  periodDate,
                                double                          periodYearDiff,
                                bslma::Allocator               *basicAllocator)
: d_periodDate(periodDate, basicAllocator)
, d_periodYearDiff(periodYearDiff)
{
    BSLS_ASSERT_SAFE(d_periodDate.size() >= 2);

    BSLS_ASSERT_SAFE(isSortedAndUnique(d_periodDate));
}

template <class CONVENTION>
inline
PeriodDayCountAdapter<CONVENTION>::~PeriodDayCountAdapter()
{
}

// ACCESSORS
template <class CONVENTION>
inline
int PeriodDayCountAdapter<CONVENTION>::daysDiff(
                                               const bdlt::Date& beginDate,
                                               const bdlt::Date& endDate) const
{
    return CONVENTION::daysDiff(beginDate, endDate);
}

template <class CONVENTION>
inline
double PeriodDayCountAdapter<CONVENTION>::yearsDiff(
                                               const bdlt::Date& beginDate,
                                               const bdlt::Date& endDate) const
{
    BSLS_ASSERT_SAFE(beginDate  >  endDate
                  || (beginDate >= d_periodDate.front()
                   && endDate   <= d_periodDate.back()));
    BSLS_ASSERT_SAFE(beginDate  <= endDate
                  || (endDate   >= d_periodDate.front()
                   && beginDate <= d_periodDate.back()));

    return CONVENTION::yearsDiff(beginDate,
                                 endDate,
                                 d_periodDate,
                                 d_periodYearDiff);
}

                                  // Aspects

template <class CONVENTION>
inline
bslma::Allocator *PeriodDayCountAdapter<CONVENTION>::allocator() const
{
    return d_periodDate.get_allocator().mechanism();
}

}  // close package namespace
}  // close enterprise namespace

// TRAITS
namespace BloombergLP {
namespace bslma {

template <class CONVENTION>
struct UsesBslmaAllocator<bbldc::PeriodDayCountAdapter<CONVENTION> >
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
