// bbldc_terminatedisda30360eom.h                                     -*-C++-*-
#ifndef INCLUDED_BBLDC_TERMINATEDISDA30360EOM
#define INCLUDED_BBLDC_TERMINATEDISDA30360EOM

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide stateless functions for ISDA 30/360 eom convention.
//
//@CLASSES:
//  bbldc::TerminatedIsda30360Eom: ISDA 30/360 eom stateless functions
//
//@DESCRIPTION: This component provides a 'struct',
// 'bbldc::TerminatedIsda30360Eom', that serves as a namespace for defining a
// suite of date-related functions used to compute the day count and year
// fraction between two dates as prescribed by the International Swaps and
// Derivatives Association (ISDA) 30/360 day-count convention with end-of-month
// (eom) adjustments.  In this day-count convention (also known as "30E/360
// ISDA"), each year is assumed to have 12 months and 360 days, with each month
// consisting of exactly 30 days.  Special end-of-month rule adjustments *are*
// made to account for the last day of February.  Note that in this day-count
// convention, the second date may or may not be adjusted depending on the
// termination date (e.g., maturity date).
//
///ISDA 30/360 eom Day Count Algorithm
///-----------------------------------
// Given 'beginDate' and 'endDate', let:
//..
// Ye =  year of earlier date           Yl =  year of later date
// Me = month of earlier date           Ml = month of later date
// De =   day of earlier date           Dl =   day of later date
//
//   o If Dl is the last day of the month, unless Dl is the termination date
//     and is in February, change Dl to 30.
//
//   o If De is the last day of the month, change De to 30.
//
// daysDiff ::= sign(endDate - beginDate) *
//                                   (Yl - Ye) * 360 + (Ml - Me) * 30 + Dl - De
//..
// Reference: http://www.isda.org/c_and_a/docs/30-360-2006ISDADefs.xls
//            (tab labeled "30E-360 ISDA")
//
// The year fraction is simply the day count divided by 360.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Computing Day Count and Year Fraction
///- - - - - - - - - - - - - - - - - - - - - - - -
// The following snippets of code illustrate how to use
// 'bbldc::TerminatedIsda30360Eom' methods.  First, create two 'bdlt::Date'
// variables, 'd1' and 'd2':
//..
//  const bdlt::Date d1(2004,  9, 30);
//  const bdlt::Date d2(2004, 12, 31);
//..
// Then, compute the day count between the two dates:
//..
//  const int daysDiff = bbldc::TerminatedIsda30360Eom::daysDiff(d1, d2);
//  assert(90 == daysDiff);
//..
// Finally, compute the year fraction between the two dates:
//..
//  const double yearsDiff = bbldc::TerminatedIsda30360Eom::yearsDiff(d1, d2);
//  assert(0.25 == yearsDiff);
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

                      // =============================
                      // struct TerminatedIsda30360Eom
                      // =============================

struct TerminatedIsda30360Eom {
    // This 'struct' provides a namespace for a suite of pure functions that
    // compute values based on dates according to the ISDA 30/360 end-of-month
    // day-count convention.

    // CLASS METHODS
    static int daysDiff(const bdlt::Date& beginDate,
                        const bdlt::Date& endDate,
                        const bdlt::Date& terminationDate = bdlt::Date());
        // Return the (signed) number of days between the specified 'beginDate'
        // and 'endDate', with the optionally specified 'terminationDate',
        // according to the ISDA 30/360 end-of-month day-count convention.  If
        // 'beginDate <= endDate', then the result is non-negative.  Note that
        // reversing the order of 'beginDate' and 'endDate' negates the result.
        // Also note that, in accordance with the convention definition, there
        // are no constraints upon the supplied dates.

    static double yearsDiff(const bdlt::Date& beginDate,
                            const bdlt::Date& endDate,
                            const bdlt::Date& terminationDate
                                                   = bdlt::Date(9999, 12, 31));
        // Return the (signed fractional) number of years between the specified
        // 'beginDate' and 'endDate', with the optionally specified
        // 'terminationDate', according to the ISDA 30/360 end-of-month
        // day-count convention.  If 'beginDate <= endDate', then the result is
        // non-negative.  Note that reversing the order of 'beginDate' and
        // 'endDate' negates the result; specifically,
        // '|yearsDiff(b, e) + yearsDiff(e, b)| <= 1.0e-15' for all dates 'b'
        // and 'e'.  Also note that, in accordance with the convention
        // definition, there are no constraints upon the supplied dates.
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                      // -----------------------------
                      // struct TerminatedIsda30360Eom
                      // -----------------------------

// CLASS METHODS
inline
double TerminatedIsda30360Eom::yearsDiff(const bdlt::Date& beginDate,
                                         const bdlt::Date& endDate,
                                         const bdlt::Date& terminationDate)
{
#if defined(BSLS_PLATFORM_CMP_GNU) && (BSLS_PLATFORM_CMP_VERSION >= 50301)
    // Storing the result value in a 'volatile double' removes extra-precision
    // available in floating-point registers.

    const volatile double rv =
#else
    const double rv =
#endif
                      static_cast<double>(daysDiff(beginDate,
                                                   endDate,
                                                   terminationDate)) / 360.0;

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
