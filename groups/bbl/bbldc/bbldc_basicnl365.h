// bbldc_basicnl365.h                                                 -*-C++-*-
#ifndef INCLUDED_BBLDC_BASICNL365
#define INCLUDED_BBLDC_BASICNL365

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide stateless functions for the NL/365 convention.
//
//@CLASSES:
//  bbldc::BasicNl365: NL/365 convention stateless functions
//
//@DESCRIPTION: This component provides a 'struct', 'bbldc::BasicNl365', that
// serves as a namespace for defining a suite of date-related functions used to
// compute the day count and the year fraction between two dates as per the
// NL/365 day-count convention.  In this day-count convention, we simply
// measure the number of non-leap days occurring in a time period, and to
// calculate years, divide that by 365.  Note that this means the number of
// years between January 1, 2004 and January 1, 2005 is 1.0.  No end-of-month
// rule adjustments are made.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Computing Day Count and Year Fraction
///- - - - - - - - - - - - - - - - - - - - - - - -
// The following snippets of code illustrate how to use 'bbldc::BasicNl365'
// methods.  First, create four 'bdlt::Date' variables:
//..
//  const bdlt::Date dA(2004, 2, 1);
//  const bdlt::Date dB(2004, 3, 1);
//  const bdlt::Date dC(2004, 5, 1);
//  const bdlt::Date dD(2005, 2, 1);
//..
// Then, compute the day count between some pairs of these dates:
//..
//  int daysDiff;
//  daysDiff = bbldc::BasicNl365::daysDiff(dA, dB);
//  assert( 28 == daysDiff);
//  daysDiff = bbldc::BasicNl365::daysDiff(dA, dC);
//  assert( 89 == daysDiff);
//  daysDiff = bbldc::BasicNl365::daysDiff(dA, dD);
//  assert(365 == daysDiff);
//  daysDiff = bbldc::BasicNl365::daysDiff(dB, dC);
//  assert( 61 == daysDiff);
//..
// Finally, compute the year fraction between some of the dates:
//..
//  double yearsDiff;
//  yearsDiff = bbldc::BasicNl365::yearsDiff(dA, dC);
//  // Need fuzzy comparison since 'yearsDiff' is a 'double'.
//  assert(yearsDiff < 0.2439 && yearsDiff > 0.2438);
//  yearsDiff = bbldc::BasicNl365::yearsDiff(dA, dD);
//  assert(1.0 == yearsDiff);
//..

#ifndef INCLUDED_BBLSCM_VERSION
#include <bblscm_version.h>
#endif

#ifndef INCLUDED_BDLT_DATE
#include <bdlt_date.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

namespace BloombergLP {
namespace bbldc {

                            // =================
                            // struct BasicNl365
                            // =================

struct BasicNl365 {
    // This 'struct' provides a namespace for a suite of pure functions that
    // compute values based on dates according to the NL/365 day-count
    // convention.

    // CLASS METHODS
    static int daysDiff(const bdlt::Date& beginDate,
                        const bdlt::Date& endDate);
        // Return the (signed) number of days between the specified 'beginDate'
        // and 'endDate' according to the NL/365 day-count convention.  If
        // 'beginDate <= endDate' then the result is non-negative.  Note that
        // reversing the order of 'beginDate' and 'endDate' negates the result.

    static double yearsDiff(const bdlt::Date& beginDate,
                            const bdlt::Date& endDate);
        // Return the (signed fractional) number of years between the specified
        // 'beginDate' and 'endDate' according to the NL/365 day-count
        // convention.  If 'beginDate <= endDate' then the result is
        // non-negative.  Note that reversing the order of 'beginDate' and
        // 'endDate' negates the result; specifically,
        // '|yearsDiff(b, e) + yearsDiff(e, b)| <= 1.0e-15' for all dates 'b'
        // and 'e'.
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                            // -----------------
                            // struct BasicNl365
                            // -----------------

// CLASS METHODS
inline
double BasicNl365::yearsDiff(const bdlt::Date& beginDate,
                             const bdlt::Date& endDate)
{
#if defined(BSLS_PLATFORM_CMP_GNU) && (BSLS_PLATFORM_CMP_VERSION >= 50301)
    // Storing the result value in a 'volatile double' removes extra-precision
    // available in floating-point registers.

    const volatile double rv =
#else
    const double rv =
#endif
                      daysDiff(beginDate, endDate) / 365.0;

    return rv;
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2017 Bloomberg Finance L.P.
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
