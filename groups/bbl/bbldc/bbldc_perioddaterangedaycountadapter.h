// bbldc_perioddaterangedaycountadapter.h                             -*-C++-*-
#ifndef INCLUDED_BBLDC_PERIODDATERANGEDAYCOUNTADAPTER
#define INCLUDED_BBLDC_PERIODDATERANGEDAYCOUNTADAPTER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a parameterized day-count convention implementation.
//
//@CLASSES:
//  bbldc::PeriodDateRangeDayCountAdapter: 'bbldc::DateRangeDayCount' adapter
//
//@DESCRIPTION: This component provides a parameterized (template)
// implementation, 'bbldc::PeriodDateRangeDayCountAdapter', of the
// 'bbldc::DateRangeDayCount' protocol.  The template argument can be any type
// supporting the following two class methods.
//..
//  int daysDiff(const bdlt::Date& beginDate, const bdlt::Date& endDate);
//
//  double yearsDiff(const bdlt::Date&              beginDate,
//                   const bdlt::Date&              endDate,
//                   const bsl::vector<bdlt::Date>& periodDate,
//                   double                         periodYearDiff);
//..
// The template class 'bbldc::PeriodDateRangeDayCountAdapter' provides
// convenient support for run-time polymorphic choice of day-count conventions
// (via conventional use of a base-class pointer or reference) without having
// to implement each derived type explicitly.  In this sense,
// 'bbldc::PeriodDateRangeDayCountAdapter' adapts the various concrete
// period-based day-count convention classes (e.g.,
// 'bbldc::PeriodIcmaActualActual') to a run-time binding mechanism.
//
// The 'bbldc::DateRangeDayCount' protocol requires two methods, 'firstDate'
// and 'lastDate', that define a date range for which calculations are valid,
// to reflect the valid range of, say, a calendar required for the
// computations.  For "period" day-count implementations, the valid date range
// is from the first to the last period date.
//
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
//  bsl::vector<bdlt::Date> sched;
//  sched.push_back(bdlt::Date(2003, 10, 1));
//  sched.push_back(bdlt::Date(2004,  1, 1));
//..
// Then, we define an instance of the adapted day-count convention and obtain a
// reference to the 'bbldc::DateRangeDayCount':
//..
//  const bbldc::PeriodDateRangeDayCountAdapter<bbldc::PeriodIcmaActualActual>
//                                                                 myDcc(sched,
//                                                                       0.25);
//  const bbldc::DateRangeDayCount&                                dcc = myDcc;
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

#ifndef INCLUDED_BBLDC_DATERANGEDAYCOUNT
#include <bbldc_daterangedaycount.h>
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

#ifndef INCLUDED_BSL_ITERATOR
#include <bsl_iterator.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {
namespace bbldc {

                   // ====================================
                   // class PeriodDateRangeDayCountAdapter
                   // ====================================

template <class CONVENTION>
class PeriodDateRangeDayCountAdapter : public DateRangeDayCount {
    // This 'class' provides an "adapter" from the specified 'CONVENTION', that
    // requires a set of periods to compute the year fraction, to the
    // 'bbldc::DateRangeDayCount' protocol that can be used for determining
    // values based on dates according to the day-count 'CONVENTION'.

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
    PeriodDateRangeDayCountAdapter(const PeriodDateRangeDayCountAdapter&);
    PeriodDateRangeDayCountAdapter& operator=(
                                        const PeriodDateRangeDayCountAdapter&);

  public:
    // CREATORS
    PeriodDateRangeDayCountAdapter(
                           const bsl::vector<bdlt::Date>&  periodDate,
                           double                          periodYearDiff,
                           bslma::Allocator               *basicAllocator = 0);
        // Create a day-count adapter that uses the specified 'periodDate' and
        // 'periodYearDiff' during invocations of 'yearsDiff'.  'periodDate'
        // provides the period starting dates and 'periodYearDiff' defines the
        // duration of each period (e.g., 0.25 for quarterly periods).
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  The behavior is undefined unless 'periodDate.size() >= 2' and
        // the values contained in 'periodDate' are unique and sorted from
        // minimum to maximum.

    virtual ~PeriodDateRangeDayCountAdapter();
        // Destroy this object.

    // ACCESSORS
    int daysDiff(const bdlt::Date& beginDate, const bdlt::Date& endDate) const;
        // Return the (signed) number of days between the specified 'beginDate'
        // and 'endDate' as per the 'CONVENTION' template policy.  If
        // 'beginDate <= endDate', then the result is non-negative.  Note that
        // reversing the order of 'beginDate' and 'endDate' negates the result.

    const bdlt::Date& firstDate() const;
        // Return a reference providing non-modifiable access to
        // 'periodDate.front()' for the 'periodDate' provided at construction.
        // Note that this value is the earliest date in the valid range of this
        // day-count convention adaptation.

    const bdlt::Date& lastDate() const;
        // Return a reference providing non-modifiable access to
        // 'periodDate.back()' for the 'periodDate' provided at construction.
        // Note that this value is the latest date in the valid range of this
        // day-count convention adaptation.

    double yearsDiff(const bdlt::Date& beginDate,
                     const bdlt::Date& endDate) const;
        // Return the (signed fractional) number of years between the specified
        // 'beginDate' and 'endDate' as per the 'CONVENTION' template policy.
        // If 'beginDate <= endDate', then the result is non-negative.  The
        // behavior is undefined unless, for the 'periodDate' provided at
        // construction, 'periodDate.front() <= beginDate <= periodDate.back()'
        // and 'periodDate.front() <= endDate <= periodDate.back()'.  Note that
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

                   // ------------------------------------
                   // class PeriodDateRangeDayCountAdapter
                   // ------------------------------------

// PRIVATE ACCESSORS
template <class CONVENTION>
bool PeriodDateRangeDayCountAdapter<CONVENTION>::isSortedAndUnique(
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
PeriodDateRangeDayCountAdapter<CONVENTION>::PeriodDateRangeDayCountAdapter(
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
PeriodDateRangeDayCountAdapter<CONVENTION>::~PeriodDateRangeDayCountAdapter()
{
}

// ACCESSORS
template <class CONVENTION>
inline
int PeriodDateRangeDayCountAdapter<CONVENTION>::daysDiff(
                                               const bdlt::Date& beginDate,
                                               const bdlt::Date& endDate) const
{
    return CONVENTION::daysDiff(beginDate, endDate);
}

template <class CONVENTION>
inline
const bdlt::Date& PeriodDateRangeDayCountAdapter<CONVENTION>::firstDate() const
{
    return d_periodDate.front();
}

template <class CONVENTION>
inline
const bdlt::Date& PeriodDateRangeDayCountAdapter<CONVENTION>::lastDate() const
{
    return d_periodDate.back();
}

template <class CONVENTION>
inline
double PeriodDateRangeDayCountAdapter<CONVENTION>::yearsDiff(
                                               const bdlt::Date& beginDate,
                                               const bdlt::Date& endDate) const
{
    BSLS_ASSERT_SAFE(d_periodDate.front() <= beginDate);
    BSLS_ASSERT_SAFE(                        beginDate <= d_periodDate.back());
    BSLS_ASSERT_SAFE(d_periodDate.front() <= endDate);
    BSLS_ASSERT_SAFE(                        endDate   <= d_periodDate.back());

    return CONVENTION::yearsDiff(beginDate,
                                 endDate,
                                 d_periodDate,
                                 d_periodYearDiff);
}

                                  // Aspects

template <class CONVENTION>
inline
bslma::Allocator *PeriodDateRangeDayCountAdapter<CONVENTION>::allocator() const
{
    return d_periodDate.get_allocator().mechanism();
}

}  // close package namespace
}  // close enterprise namespace

// TRAITS

namespace BloombergLP {
namespace bslma {

template <class CONVENTION>
struct UsesBslmaAllocator<bbldc::PeriodDateRangeDayCountAdapter<CONVENTION> >
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
