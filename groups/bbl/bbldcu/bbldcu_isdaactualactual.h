// bbldcu_isdaactualactual.h                                          -*-C++-*-
#ifndef INCLUDED_BBLDCU_ISDAACTUALACTUAL
#define INCLUDED_BBLDCU_ISDAACTUALACTUAL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide support for the ISDA Actual/Actual convention.
//
//@CLASSES:
//  bbldcu::IsdaActualActual: ISDA Actual/Actual convention stateless functions
//
//@DESCRIPTION: This component provides a 'struct', 'bbldcu::IsdaActualActual',
// that defines a suite of date-related functions, which can be used to compute
// the day-count and the year-fraction between two dates as per the ISDA
// Actual/Actual convention.  In this convention, the day-count between two
// dates is exactly the number of days separating the dates as per a
// conventional calendar.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Computing Day-Count and Year-Fraction
///- - - - - - - - - - - - - - - - - - - - - - - -
// The following snippets of code illustrate how to use
// 'bbldcu::IsdaActualActual' methods.  First, create two 'bdlt::Dates' 'd1'
// and 'd2':
//..
//  const bdlt::Date d1(2003, 10, 19);
//  const bdlt::Date d2(2003, 12, 31);
//..
// Then, compute the day-count between these two dates:
//..
//  const int daysDiff = bbldcu::IsdaActualActual::daysDiff(d1, d2);
//  assert(73 == daysDiff);
//..
// Finally, compute the year-fraction between these two dates:
//..
//  const double yearsDiff = bbldcu::IsdaActualActual::yearsDiff(d1, d2);
//  // Need fuzzy comparison since 'yearsDiff' is a double.  Expect
//  // '0.2 == yearsDiff'.
//  assert(yearsDiff > 0.1999 && yearsDiff < 0.2001);
//..

#ifndef INCLUDED_BBLSCM_VERSION
#include <bblscm_version.h>
#endif

#ifndef INCLUDED_BDLT_DATE
#include <bdlt_date.h>
#endif

namespace BloombergLP {
namespace bbldcu {

                          // =======================
                          // struct IsdaActualActual
                          // =======================

struct IsdaActualActual {
    // This 'struct' provides a namespace for a suite of pure functions that
    // compute values based on dates according to the ISDA Actual/Actual
    // convention.

    // CLASS METHODS
    static int daysDiff(const bdlt::Date& beginDate,
                        const bdlt::Date& endDate);
        // Return the number of days between the specified 'beginDate' and
        // 'endDate' according to the ISDA Actual/Actual convention.  If
        // 'beginDate <= endDate', then the result is non-negative.  Note that
        // reversing the order of 'beginDate' and 'endDate' negates the result.

    static double yearsDiff(const bdlt::Date& beginDate,
                            const bdlt::Date& endDate);
        // Return the number of years between the specified 'beginDate' and
        // 'endDate' according to the ISDA Actual/Actual convention.  If
        // 'beginDate <= endDate', then the result is non-negative.  The
        // behavior is undefined if either 'beginDate' or 'endDate' is in the
        // year 1752.  Note that reversing the order of 'beginDate' and
        // 'endDate' negates the result; specifically
        // '|yearsDiff(b, e) + yearsDiff(e, b)| <= 1.0e-15'.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                          // -----------------------
                          // struct IsdaActualActual
                          // -----------------------

// CLASS METHODS
inline
int IsdaActualActual::daysDiff(const bdlt::Date& beginDate,
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
