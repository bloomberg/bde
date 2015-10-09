// bbldc_basicactual365fixed.h                                        -*-C++-*-
#ifndef INCLUDED_BBLDC_BASICACTUAL365FIXED
#define INCLUDED_BBLDC_BASICACTUAL365FIXED

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide stateless functions for the Actual/365 (fixed) convention.
//
//@CLASSES:
//  bbldc::BasicActual365Fixed: Actual/365 fixed convention stateless functions
//
//@DESCRIPTION: This component provides a 'struct',
// 'bbldc::BasicActual365Fixed', that serves as a namespace for defining a
// suite of date-related functions used to compute the day count and year
// fraction between two dates as per the Actual/365 (fixed) day-count
// convention.  In this day-count convention, we simply measure the number of
// days occurring in a time period, and to calculate years, divide that by 365.
// Note that this means the number of years between January 1, 2004 and January
// 1, 2005 comes out to about 1.00274.  No end-of-month rule adjustments are
// made.  Given 'beginDate' and 'endDate':
//..
//  yearsDiff ::= sign(endDate - beginDate) *
//                                 (days between beginDate and endDate) / 365.0
//..
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Computing Day Count and Year Fraction
/// - - - - - - - - - - - - - - - - - - - - - - - -
// The following snippets of code illustrate how to use
// 'bbldc::BasicActual365Fixed' methods.  First, create four 'bdlt::Date'
// variables:
//..
//  const bdlt::Date dA(2004, 2, 1);
//  const bdlt::Date dB(2004, 3, 1);
//  const bdlt::Date dC(2004, 5, 1);
//  const bdlt::Date dD(2005, 2, 1);
//..
// Then, compute the day count between some pairs of these dates:
//..
//  int daysDiff;
//  daysDiff = bbldc::BasicActual365Fixed::daysDiff(dA, dB);
//  assert( 29 == daysDiff);
//  daysDiff = bbldc::BasicActual365Fixed::daysDiff(dA, dC);
//  assert( 90 == daysDiff);
//  daysDiff = bbldc::BasicActual365Fixed::daysDiff(dA, dD);
//  assert(366 == daysDiff);
//  daysDiff = bbldc::BasicActual365Fixed::daysDiff(dB, dC);
//  assert( 61 == daysDiff);
//..
// Finally, compute the year fraction between some of the dates:
//..
//  double yearsDiff;
//  yearsDiff = bbldc::BasicActual365Fixed::yearsDiff(dA, dC);
//  // Need fuzzy comparison since 'yearsDiff' is a 'double'.
//  assert(yearsDiff > 0.2465 && yearsDiff < 0.2466);
//  yearsDiff = bbldc::BasicActual365Fixed::yearsDiff(dA, dD);
//  assert(yearsDiff > 1.0027 && yearsDiff < 1.0028);
//..

#ifndef INCLUDED_BBLSCM_VERSION
#include <bblscm_version.h>
#endif

#ifndef INCLUDED_BDLT_DATE
#include <bdlt_date.h>
#endif

namespace BloombergLP {
namespace bbldc {

                        // ==========================
                        // struct BasicActual365Fixed
                        // ==========================

struct BasicActual365Fixed {
    // This 'struct' provides a namespace for a suite of pure functions that
    // compute values based on dates according to the Actual/365 fixed
    // day-count convention.

    // CLASS METHODS
    static int daysDiff(const bdlt::Date& beginDate,
                        const bdlt::Date& endDate);
        // Return the (signed) number of days between the specified 'beginDate'
        // and 'endDate' according to the Actual/365 fixed day-count
        // convention.  If 'beginDate <= endDate', then the result is
        // non-negative.  Note that reversing the order of 'beginDate' and
        // 'endDate' negates the result.

    static double yearsDiff(const bdlt::Date& beginDate,
                            const bdlt::Date& endDate);
        // Return the (signed fractional) number of years between the specified
        // 'beginDate' and 'endDate' according to the Actual/365 fixed
        // day-count convention.  If 'beginDate <= endDate', then the result is
        // non-negative.  Note that reversing the order of 'beginDate' and
        // 'endDate' negates the result; specifically,
        // '|yearsDiff(b, e) + yearsDiff(e, b)| <= 1.0e-15' for all dates 'b'
        // and 'e'.
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                        // --------------------------
                        // struct BasicActual365Fixed
                        // --------------------------

// CLASS METHODS
inline
int BasicActual365Fixed::daysDiff(const bdlt::Date& beginDate,
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
