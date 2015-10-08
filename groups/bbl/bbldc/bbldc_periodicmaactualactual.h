// bbldc_periodicmaactualactual.h                                     -*-C++-*-
#ifndef INCLUDED_BBLDC_PERIODICMAACTUALACTUAL
#define INCLUDED_BBLDC_PERIODICMAACTUALACTUAL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide stateless functions for period-based ICMA Actual/Actual.
//
//@CLASSES:
//  bbldc::PeriodIcmaActualActual: ICMA Act/Act convention stateless functions
//
//@DESCRIPTION: This component provides a 'struct',
// 'bbldc::PeriodIcmaActualActual', that serves as a namespace for defining a
// suite of date-related functions used to compute the day count and the year
// fraction between two dates as per the ICMA Actual/Actual day-count
// convention.  In this day-count convention, the day count between two dates
// is exactly the number of days occurring in the time period and the year
// fraction is the number of full periods multiplied by the period weighting
// ('periodYearDiff') plus front-stub and back-stub adjustments.  The periods
// are defined by the 'periodDate' elements representing, say, bond coupon
// payment dates with the first period starting on 'periodDate[0]' and ending
// on 'periodDate[1], the second period starting on 'periodDate[1]' and ending
// on 'periodDate[2], and the 'i'-th period starting on 'periodDate[i - 1]' and
// ending on 'periodDate[i]'.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Computing Day Count and Year Fraction
/// - - - - - - - - - - - - - - - - - - - - - - - -
// The following snippets of code illustrate how to use
// 'bbldc::PeriodIcmaActualActual' methods.  First, create two 'bdlt::Date'
// variables, 'd1' and 'd2':
//..
//  const bdlt::Date d1(2003, 10, 19);
//  const bdlt::Date d2(2003, 12, 31);
//..
// Then, create a schedule of period dates, 'sched', corresponding to a
// quarterly payment ('periodYearDiff == 0.25'):
//..
//  bsl::vector<bdlt::Date> sched;
//  sched.push_back(bdlt::Date(2003, 10, 1));
//  sched.push_back(bdlt::Date(2004,  1, 1));
//..
// Next, compute the day count between 'd1' and 'd2':
//..
//  const int daysDiff = bbldc::PeriodIcmaActualActual::daysDiff(d1, d2);
//  assert(73 == daysDiff);
//..
// Finally, compute the year fraction between the two dates:
//..
//  const double yearsDiff = bbldc::PeriodIcmaActualActual::yearsDiff(d1,
//                                                                    d2,
//                                                                    sched,
//                                                                    0.25);
//  // Need fuzzy comparison since 'yearsDiff' is a 'double'.
//  assert(yearsDiff > 0.1983 && yearsDiff < 0.1985);
//..

#ifndef INCLUDED_BBLSCM_VERSION
#include <bblscm_version.h>
#endif

#ifndef INCLUDED_BDLT_DATE
#include <bdlt_date.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {
namespace bbldc {

                      // =============================
                      // struct PeriodIcmaActualActual
                      // =============================

struct PeriodIcmaActualActual {
    // This 'struct' provides a namespace for a suite of pure functions that
    // compute values based on dates according to the ICMA Actual/Actual
    // day-count convention.

    // CLASS METHODS
    static int daysDiff(const bdlt::Date& beginDate,
                        const bdlt::Date& endDate);
        // Return the (signed) number of days between the specified 'beginDate'
        // and 'endDate' according to the ICMA Actual/Actual day-count
        // convention.  If 'beginDate <= endDate', then the result is
        // non-negative.  Note that reversing the order of 'beginDate' and
        // 'endDate' negates the result.

    static double yearsDiff(const bdlt::Date&              beginDate,
                            const bdlt::Date&              endDate,
                            const bsl::vector<bdlt::Date>& periodDate,
                            double                         periodYearDiff);
        // Return the (signed fractional) number of years between the specified
        // 'beginDate' and 'endDate' according to the ICMA Actual/Actual
        // day-count convention with periods starting on the specified
        // 'periodDate' values and each period having a duration of the
        // specified 'periodYearDiff' years (e.g., 0.25 for quarterly periods).
        // If 'beginDate <= endDate', then the result is non-negative.  The
        // behavior is undefined unless 'periodDate.size() >= 2', the values
        // contained in 'periodDate' are unique and sorted from minimum to
        // maximum, 'periodDate.front() <= beginDate <= periodDate.back()', and
        // 'periodDate.front() <= endDate <= periodDate.back()'.  Note that
        // reversing the order of 'beginDate' and 'endDate' negates the result;
        // specifically,
        // '|yearsDiff(b, e, pd, pyd) + yearsDiff(e, b, pd, pyd)| <= 1.0e-15'
        // for all dates 'b' and 'e', periods 'pd', and year fraction per
        // period 'pyd'.
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                      // -----------------------------
                      // struct PeriodIcmaActualActual
                      // -----------------------------

// CLASS METHODS
inline
int PeriodIcmaActualActual::daysDiff(const bdlt::Date& beginDate,
                                     const bdlt::Date& endDate)
{
    return endDate - beginDate;
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
