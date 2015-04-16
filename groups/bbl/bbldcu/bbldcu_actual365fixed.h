// bbldcu_actual365fixed.h                                            -*-C++-*-
#ifndef INCLUDED_BBLDCU_ACTUAL365FIXED
#define INCLUDED_BBLDCU_ACTUAL365FIXED

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide support for the Actual/365 (fixed) convention.
//
//@CLASSES:
//  bbldcu::Actual365Fixed: Actual/365 fixed convention stateless functions
//
//@DESCRIPTION: This component provides a 'struct', 'bbldcu::Actual365Fixed',
// that defines a suite of date-related functions, which can be used to compute
// the day-count and year-fraction between two dates as per the Actual/365
// (fixed) convention.  In this convention, we simply measure the Julian days
// that have occurred in a time period, and to calculate years, divide that by
// 365.  Note that this means the number of years between January 1, 2004 and
// January 1, 2005 comes out to about 1.00274.  No end-of-month rule
// adjustments are made.  Given 'beginDate' and 'endDate':
//..
//  yearsDiff ::= sign(endDate - beginDate) *
//                          (Julian days between beginDate and endDate) / 365.0
//..
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Computing Day-Count and Year-Fraction
///- - - - - - - - - - - - - - - - - - - - - - - -
// The following snippets of code illustrate how to use
// 'bbldcu::Actual365Fixed' methods.  First, create four 'bdlt::Dates':
//..
//  const bdlt::Date dA(2004, 2, 1);
//  const bdlt::Date dB(2004, 3, 1);
//  const bdlt::Date dC(2004, 5, 1);
//  const bdlt::Date dD(2005, 2, 1);
//..
// Then, compute the day-count between some pairs of these dates:
//..
//  int daysDiff;
//  daysDiff = bbldcu::Actual365Fixed::daysDiff(dA, dB);
//  assert( 29 == daysDiff);
//  daysDiff = bbldcu::Actual365Fixed::daysDiff(dA, dC);
//  assert( 90 == daysDiff);
//  daysDiff = bbldcu::Actual365Fixed::daysDiff(dA, dD);
//  assert(366 == daysDiff);
//  daysDiff = bbldcu::Actual365Fixed::daysDiff(dB, dC);
//  assert( 61 == daysDiff);
//..
// Finally, compute the year-fraction between some of these dates:
//..
//  double yearsDiff;
//  yearsDiff = bbldcu::Actual365Fixed::yearsDiff(dA, dC);
//  assert(yearsDiff > 0.2465 && yearsDiff < 0.2466);
//  yearsDiff = bbldcu::Actual365Fixed::yearsDiff(dA, dD);
//  assert(yearsDiff > 1.0027 && yearsDiff < 1.0028);
//..

#ifndef INCLUDED_BBLSCM_VERSION
#include <bblscm_version.h>
#endif

#ifndef INCLUDED_BDLT_DATE
#include <bdlt_date.h>
#endif

namespace BloombergLP {
namespace bbldcu {

                           // =====================
                           // struct Actual365Fixed
                           // =====================

struct Actual365Fixed {
    // This 'struct' provides a namespace for a suite of pure functions that
    // compute values based on dates according to the Actual/365 fixed
    // convention.

    // CLASS METHODS
    static int daysDiff(const bdlt::Date& beginDate,
                        const bdlt::Date& endDate);
        // Return the number of days between the specified 'beginDate' and
        // 'endDate' according to the Actual/365 fixed  convention.  If
        // 'beginDate <= endDate', then the result is non-negative.  Note that
        // reversing the order of 'beginDate' and 'endDate' negates the result.

    static double yearsDiff(const bdlt::Date& beginDate,
                            const bdlt::Date& endDate);
        // Return the number of years between the specified 'beginDate' and
        // 'endDate' according to the Actual/365 fixed  convention.  If
        // 'beginDate <= endDate', then the result is non-negative.  Note that
        // reversing the order of 'beginDate' and 'endDate' negates the result;
        // specifically '|yearsDiff(b, e) + yearsDiff(e, b)| <= 1.0e-15'.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                           // ---------------------
                           // struct Actual365Fixed
                           // ---------------------

// CLASS METHODS
inline
int Actual365Fixed::daysDiff(const bdlt::Date& beginDate,
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
