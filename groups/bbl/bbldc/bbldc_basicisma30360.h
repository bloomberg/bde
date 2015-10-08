// bbldc_basicisma30360.h                                             -*-C++-*-
#ifndef INCLUDED_BBLDC_BASICISMA30360
#define INCLUDED_BBLDC_BASICISMA30360

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide stateless functions for the ISMA 30/360 convention.
//
//@CLASSES:
//  bbldc::BasicIsma30360: ISMA 30/360 convention stateless functions
//
//@DESCRIPTION: This component provides a 'struct', 'bbldc::BasicIsma30360',
// that serves as a namespace for defining a suite of date-related functions
// used to compute the day count and year fraction between two dates as per the
// ISMA 30/360 day-count convention.  In this day-count convention (also known
// as "European 30/360"), each year is assumed to have 12 months and 360 days,
// with each month consisting of exactly 30 days.  End-of-month rule
// adjustments are *not* made to account for the last day of February.
//
///ISMA 30/360 Day Count Algorithm
///-------------------------------
// Given 'beginDate' and 'endDate', let:
//..
// Ye =  year of earlier date           Yl =  year of later date
// Me = month of earlier date           Ml = month of later date
// De =   day of earlier date           Dl =   day of later date
//
//   o If De is 31, change De to 30.
//
//   o If Dl is 31, change Dl to 30.
//
// daysDiff ::= sign(endDate - beginDate) *
//                                   (Yl - Ye) * 360 + (Ml - Me) * 30 + Dl - De
//..
// Reference: Formulae For Yield And Other Calculations (1992)
//            ISBN: 0-9515474-0-2
//
// Note that in this day-count convention, dates ('De' and 'Dl') are adjusted
// independently of each other.
//
// The year fraction is simply the day count divided by 360.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Computing Day Count and Year Fraction
/// - - - - - - - - - - - - - - - - - - - - - - - -
// The following snippets of code illustrate how to use 'bbldc::BasicIsma30360'
// methods.  First, create two 'bdlt::Date' variables, 'd1' and 'd2':
//..
//  const bdlt::Date d1(2004,  9, 30);
//  const bdlt::Date d2(2004, 12, 31);
//..
// Then, compute the day count between the two dates:
//..
//  const int daysDiff = bbldc::BasicIsma30360::daysDiff(d1, d2);
//  assert(90 == daysDiff);
//..
// Finally, compute the year fraction between the two dates:
//..
//  const double yearsDiff = bbldc::BasicIsma30360::yearsDiff(d1, d2);
//  assert(0.25 == yearsDiff);
//..

#ifndef INCLUDED_BBLSCM_VERSION
#include <bblscm_version.h>
#endif

namespace BloombergLP {
namespace bdlt {

class Date;

}  // close namespace bdlt

namespace bbldc {

                          // =====================
                          // struct BasicIsma30360
                          // =====================

struct BasicIsma30360 {
    // This 'struct' provides a namespace for a suite of pure functions that
    // compute values based on dates according to the ISMA 30/360 day-count
    // convention.

    // CLASS METHODS
    static int daysDiff(const bdlt::Date& beginDate,
                        const bdlt::Date& endDate);
        // Return the (signed) number of days between the specified 'beginDate'
        // and 'endDate' according to the ISMA 30/360 day-count convention.  If
        // 'beginDate <= endDate', then the result is non-negative.  Note that
        // reversing the order of 'beginDate' and 'endDate' negates the result.

    static double yearsDiff(const bdlt::Date& beginDate,
                            const bdlt::Date& endDate);
        // Return the (signed fractional) number of years between the specified
        // 'beginDate' and 'endDate' according to the ISMA 30/360 day-count
        // convention.  If 'beginDate <= endDate', then the result is
        // non-negative.  Note that reversing the order of 'beginDate' and
        // 'endDate' negates the result; specifically,
        // '|yearsDiff(b, e) + yearsDiff(e, b)| <= 1.0e-15' for all dates 'b'
        // and 'e'.
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
