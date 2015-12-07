// bbldc_basicdaycountutil.h                                          -*-C++-*-
#ifndef INCLUDED_BBLDC_BASICDAYCOUNTUTIL
#define INCLUDED_BBLDC_BASICDAYCOUNTUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Support for day-count calculations of 'enum'-specified conventions.
//
//@CLASSES:
//  bbldc::BasicDayCountUtil: 'enum'-specified day-count calculation procedures
//
//@SEE_ALSO: bbldc_daycountconvention
//
//@DESCRIPTION: This component provides a 'struct', 'bbldc::BasicDayCountUtil',
// that defines a suite of date-related functions used to compute the day count
// and the year fraction between two dates as prescribed by an enumerated
// day-count convention.  Specifically, the 'daysDiff' and 'yearsDiff' methods
// defined in 'bbldc::BasicDayCountUtil' take a trailing
// 'DayCountConvention::Enum' argument indicating which particular day-count
// convention to apply.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Computing Day Count and Year Fraction
/// - - - - - - - - - - - - - - - - - - - - - - - -
// The following snippets of code illustrate how to use
// 'bbldc::BasicDayCountUtil' methods.  First, create two 'bdlt::Date'
// variables, 'd1' and 'd2':
//..
//  const bdlt::Date d1(2003, 10, 19);
//  const bdlt::Date d2(2003, 12, 31);
//..
// Now, compute the day count between 'd1' and 'd2' according to the ISDA
// Actual/Actual convention:
//..
//  const int daysDiff = bbldc::BasicDayCountUtil::daysDiff(
//                            d1,
//                            d2,
//                            bbldc::DayCountConvention::e_ISDA_ACTUAL_ACTUAL);
//  assert(73 == daysDiff);
//..
// Finally, compute the year fraction between the two dates according to the
// ISDA Actual/Actual convention:
//..
//  const double yearsDiff = bbldc::BasicDayCountUtil::yearsDiff(
//                            d1,
//                            d2,
//                            bbldc::DayCountConvention::e_ISDA_ACTUAL_ACTUAL);
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

                         // ========================
                         // struct BasicDayCountUtil
                         // ========================

struct BasicDayCountUtil {
    // This 'struct' provides a namespace for a suite of pure functions that
    // compute values based on dates according to enumerated day-count
    // conventions.

    // CLASS METHODS
    static int daysDiff(const bdlt::Date&        beginDate,
                        const bdlt::Date&        endDate,
                        DayCountConvention::Enum convention);
        // Return the (signed) number of days between the specified 'beginDate'
        // and 'endDate' according to the specified day-count 'convention'.
        // The behavior is undefined unless 'isSupported(convention)'.  If
        // 'beginDate <= endDate' then the result is non-negative.  Note that
        // reversing the order of 'beginDate' and 'endDate' negates the result.

    static bool isSupported(DayCountConvention::Enum convention);
        // Return 'true' if the specified 'convention' is valid for use in
        // 'daysDiff' and 'yearsDiff', and 'false' otherwise.

    static double yearsDiff(const bdlt::Date&        beginDate,
                            const bdlt::Date&        endDate,
                            DayCountConvention::Enum convention);
        // Return the (signed fractional) number of years between the specified
        // 'beginDate' and 'endDate' according to the specified day-count
        // 'convention'.  If 'beginDate <= endDate' then the result is
        // non-negative.  The behavior is undefined unless
        // 'isSupported(convention)'.  Note that reversing the order of
        // 'beginDate' and 'endDate' negates the result; specifically,
        // '|yearsDiff(b, e, c) + yearsDiff(e, b, c)| <= 1.0e-15' for all dates
        // 'b' and 'e', and day-count conventions 'c'.
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
