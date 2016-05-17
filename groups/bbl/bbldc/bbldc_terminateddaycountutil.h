// bbldc_terminateddaycountutil.h                                     -*-C++-*-
#ifndef INCLUDED_BBLDC_TERMINATEDDAYCOUNTUTIL
#define INCLUDED_BBLDC_TERMINATEDDAYCOUNTUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Support for day-count calculations of 'enum'-specified conventions.
//
//@CLASSES:
//  bbldc::TerminatedDayCountUtil: 'enum'-specified day-count calculations
//
//@SEE_ALSO: bbldc_daycountconvention, bbldc_terminatedbus252
//
//@DESCRIPTION: This component provides a 'struct',
// 'bbldc::TerminatedDayCountUtil', that defines a suite of date-related
// functions used to compute the day count and the year fraction between two
// dates, with potential special handling of a termination date (e.g., maturity
// date), as prescribed by an enumerated day-count convention.  Specifically,
// the 'daysDiff' and 'yearsDiff' methods defined in
// 'bbldc::TerminatedDayCountUtil' take a trailing 'DayCountConvention::Enum'
// argument indicating which particular day-count convention to apply.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Computing Day Count and Year Fraction
///- - - - - - - - - - - - - - - - - - - - - - - -
// The following snippets of code illustrate how to use
// 'bbldc::TerminatedDayCountUtil' methods.  First, create three 'bdlt::Date'
// variables, 'd1', 'd2', and 'dt':
//..
//  const bdlt::Date d1(2003, 10, 18);
//  const bdlt::Date d2(2003, 12, 31);
//  const bdlt::Date dt(2004,  2, 29);
//..
// Then, compute the day count between 'd1' and 'd2' according to the ISDA
// 30/360 EOM day-count convention with termination date 'dt':
//..
//  const int daysDiff = bbldc::TerminatedDayCountUtil::daysDiff(
//                               d1,
//                               d2,
//                               dt,
//                               bbldc::DayCountConvention::e_ISDA_30_360_EOM);
//  assert(72 == daysDiff);
//..
// Finally, compute the year fraction between the two dates according to the
// ISDA 30/360 EOM day-count convention with termination date 'dt':
//..
//  const double yearsDiff = bbldc::TerminatedDayCountUtil::yearsDiff(
//                               d1,
//                               d2,
//                               dt,
//                               bbldc::DayCountConvention::e_ISDA_30_360_EOM);
//
//  // Need fuzzy comparison since 'yearsDiff' is a 'double'.
//  assert(0.1999 < yearsDiff && 0.2001 > yearsDiff);
//..

#ifndef INCLUDED_BBLSCM_VERSION
#include <bblscm_version.h>
#endif

#ifndef INCLUDED_BBLDC_DAYCOUNTCONVENTION
#include <bbldc_daycountconvention.h>
#endif

namespace BloombergLP {
namespace bdlt { class Date; }
namespace bbldc {

                      // =============================
                      // struct TerminatedDayCountUtil
                      // =============================

struct TerminatedDayCountUtil {
    // This 'struct' provides a namespace for a suite of pure functions that
    // compute values based on dates according to enumerated day-count
    // conventions.

    // CLASS METHODS
    static int daysDiff(const bdlt::Date&        beginDate,
                        const bdlt::Date&        endDate,
                        const bdlt::Date&        terminationDate,
                        DayCountConvention::Enum convention);
        // Return the (signed) number of days between the specified 'beginDate'
        // and 'endDate' according to the specified day-count 'convention' with
        // the specified 'terminationDate'.  If 'beginDate <= endDate' then the
        // result is non-negative.  The behavior is undefined unless
        // 'isSupported(convention)'.  Note that reversing the order of
        // 'beginDate' and 'endDate' negates the result and that the result is
        // 0 when 'beginDate == endDate'.

    static bool isSupported(DayCountConvention::Enum convention);
        // Return 'true' if the specified 'convention' is valid for use in
        // 'daysDiff' and 'yearsDiff', and 'false' otherwise.

    static double yearsDiff(const bdlt::Date&        beginDate,
                            const bdlt::Date&        endDate,
                            const bdlt::Date&        terminationDate,
                            DayCountConvention::Enum convention);
        // Return the (signed fractional) number of years between the specified
        // 'beginDate' and 'endDate' according to the specified day-count
        // 'convention' with the specified 'terminationDate'.  If
        // 'beginDate <= endDate' then the result is non-negative.  The
        // behavior is undefined unless 'isSupported(convention)'.  Note that
        // reversing the order of 'beginDate' and 'endDate' negates the result;
        // specifically,
        // '|yearsDiff(b, e, t, c) + yearsDiff(e, b, t, c)| <= 1.0e-15' for all
        // valid dates 'b', 'e', and 't', and day-count conventions 'c'.
};

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
