// bbldc_terminatedbasicdaycountadapter.h                             -*-C++-*-
#ifndef INCLUDED_BBLDC_TERMINATEDBASICDAYCOUNTADAPTER
#define INCLUDED_BBLDC_TERMINATEDBASICDAYCOUNTADAPTER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a parameterized day-count convention implementation.
//
//@CLASSES:
//  bbldc::TerminatedBasicDayCountAdapter: 'bbldc::BasicDayCount' realization
//
//@DESCRIPTION: This component provides a parameterized (template)
// implementation, 'bbldc::TerminatedBasicDayCountAdapter', of the
// 'bbldc::BasicDayCount' protocol that allows for special handling of a
// termination date (e.g., maturity date).  The template argument can be any
// type supporting the following two class methods.
//..
//  int daysDiff(const bdlt::Date& beginDate,
//               const bdlt::Date& endDate,
//               const bdlt::Date& terminationDate);
//
//  double yearsDiff(const bdlt::Date& beginDate,
//                   const bdlt::Date& endDate,
//                   const bdlt::Date& terminationDate);
//..
// The template class 'bbldc::TerminatedBasicDayCountAdapter' provides
// convenient support for run-time polymorphic choice of day-count conventions
// (via conventional use of a base-class pointer or reference) without having
// to implement each derived type explicitly.  In this sense,
// 'bbldc::TerminatedBasicDayCountAdapter' adapts the various concrete
// "terminated" day-count convention classes (e.g.,
// 'bbldc::TerminatedIsda30360Eom') to a run-time binding mechanism.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Adapting 'bbldc::TerminatedIsda30360Eom'
///- - - - - - - - - - - - - - - - - - - - - - - - - -
// This example shows the procedure for using
// 'bbldc::TerminatedBasicDayCountAdapter' to adapt the
// 'bbldc::TerminatedIsda30360Eom' day-count convention to the
// 'bbldc::BasicDayCount' protocol, and then the use of the day-count methods.
//
// First, we define an instance of the adapted 'bbldc::TerminatedIsda30360Eom'
// day-count convention and obtain a reference to the 'bbldc::BasicDayCount':
//..
//  const bbldc::TerminatedBasicDayCountAdapter<bbldc::TerminatedIsda30360Eom>
//                                              myDcc(bdlt::Date(2004, 2, 29));
//  const bbldc::BasicDayCount& dcc = myDcc;
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

#ifndef INCLUDED_BBLDC_BASICDAYCOUNT
#include <bbldc_basicdaycount.h>
#endif

#ifndef INCLUDED_BDLT_DATE
#include <bdlt_date.h>
#endif

namespace BloombergLP {
namespace bbldc {

                   // ====================================
                   // class TerminatedBasicDayCountAdapter
                   // ====================================

template <class CONVENTION>
class TerminatedBasicDayCountAdapter : public BasicDayCount {
    // This 'class' provides an "adapter" from the specified 'CONVENTION' to
    // the 'bbldc::BasicDayCount' protocol that can be used for determining
    // values based on dates according to the day-count 'CONVENTION'.

    // DATA
    bdlt::Date d_terminationDate;  // termination date used in all calculations

  private:
    // NOT IMPLEMENTED
    TerminatedBasicDayCountAdapter(const TerminatedBasicDayCountAdapter&);
    TerminatedBasicDayCountAdapter& operator=(
                                        const TerminatedBasicDayCountAdapter&);

  public:
    // CREATORS
    TerminatedBasicDayCountAdapter(const bdlt::Date& terminationDate);
        // Create a day-count adapter that uses the specified 'terminationDate'
        // during.

    virtual ~TerminatedBasicDayCountAdapter();
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
        // If 'beginDate <= endDate', then the result is non-negative.  Note
        // that reversing the order of 'beginDate' and 'endDate' negates the
        // result; specifically,
        // '|yearsDiff(b, e) + yearsDiff(e, b)| <= 1.0e-15' for all dates 'b'
        // and 'e'.
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                   // ------------------------------------
                   // class TerminatedBasicDayCountAdapter
                   // ------------------------------------

// CREATORS
template <class CONVENTION>
inline
TerminatedBasicDayCountAdapter<CONVENTION>::
              TerminatedBasicDayCountAdapter(const bdlt::Date& terminationDate)
: d_terminationDate(terminationDate)
{
}

template <class CONVENTION>
inline
TerminatedBasicDayCountAdapter<CONVENTION>::~TerminatedBasicDayCountAdapter()
{
}

// ACCESSORS
template <class CONVENTION>
inline
int TerminatedBasicDayCountAdapter<CONVENTION>::daysDiff(
                                               const bdlt::Date& beginDate,
                                               const bdlt::Date& endDate) const
{
    return CONVENTION::daysDiff(beginDate, endDate, d_terminationDate);
}

template <class CONVENTION>
inline
double
TerminatedBasicDayCountAdapter<CONVENTION>::yearsDiff(
                                               const bdlt::Date& beginDate,
                                               const bdlt::Date& endDate) const
{
    return CONVENTION::yearsDiff(beginDate, endDate, d_terminationDate);
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
