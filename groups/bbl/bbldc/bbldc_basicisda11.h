// bbldc_basicisda11.h                                                -*-C++-*-
#ifndef INCLUDED_BBLDC_BASICISDA11
#define INCLUDED_BBLDC_BASICISDA11

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide stateless functions for the ISDA 1/1 convention.
//
//@CLASSES:
//  bbldc::BasicIsda11: ISDA 1/1 stateless functions
//
//@DESCRIPTION: This component provides a `struct`, `bbldc::BasicIsda11`, that
// serves as a namespace for defining a suite of date-related functions used to
// compute the day count and the year fraction between two dates as per the
// ISDA 1/1 day-count convention.  In this day-count convention, the day count
// and year fraction between two dates is one if the second date is later than
// the first, zero if equal, and negative one if the first date is later than
// the second.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Computing Day Count and Year Fraction
/// - - - - - - - - - - - - - - - - - - - - - - - -
// The following snippets of code illustrate how to use `bbldc::BasicIsda11`
// methods.  First, create two `bdlt::Date` variables, `d1` and `d2`:
// ```
// const bdlt::Date d1(2003, 10, 19);
// const bdlt::Date d2(2003, 12, 31);
// ```
// Then, compute the day count between the two dates:
// ```
// const int daysDiff = bbldc::BasicIsda11::daysDiff(d1, d2);
// assert(1 == daysDiff);
// ```
// Finally, compute the year fraction between the two dates:
// ```
// const double yearsDiff = bbldc::BasicIsda11::yearsDiff(d1, d2);
// assert(1.0 == yearsDiff);
// ```

#include <bblscm_version.h>

#include <bdlt_date.h>

namespace BloombergLP {
namespace bbldc {

                       // ============================
                       // struct BasicIsda11
                       // ============================

/// This `struct` provides a namespace for a suite of pure functions that
/// compute values based on dates according to the ISDA 1/1 day-count
/// convention.
struct BasicIsda11 {

    // CLASS METHODS

    /// Return the (signed) number of days between the specified `beginDate`
    /// and `endDate` according to the ISDA 1/1 day-count convention.  If
    /// `beginDate < endDate`, then the result is one.    If
    /// `beginDate == endDate`, then the result is zero.  Note that reversing
    /// the order of `beginDate` and `endDate` negates the result.
    static int daysDiff(const bdlt::Date& beginDate,
                        const bdlt::Date& endDate);

    /// Return the (signed fractional) number of years between the specified
    /// `beginDate` and `endDate` according to the ISDA 1/1 day-count
    /// convention.  If `beginDate < endDate`, then the result is one.  If
    /// `beginDate == endDate`, then the result is zero.  Note that reversing
    /// the order of `beginDate` and `endDate` negates the result;
    /// specifically, `|yearsDiff(b, e) + yearsDiff(e, b)| <= 1.0e-15` for all
    /// dates `b` and `e`.
    static double yearsDiff(const bdlt::Date& beginDate,
                            const bdlt::Date& endDate);
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                           // ------------------
                           // struct BasicIsda11
                           // ------------------

// CLASS METHODS
inline
double BasicIsda11::yearsDiff(const bdlt::Date& beginDate,
                              const bdlt::Date& endDate)
{
    return beginDate < endDate ? 1.0 : beginDate == endDate ? 0.0 : -1.0;
}

inline
int BasicIsda11::daysDiff(const bdlt::Date& beginDate,
                                    const bdlt::Date& endDate)
{
    return beginDate < endDate ? 1 : beginDate == endDate ? 0 : -1;
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2025 Bloomberg Finance L.P.
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
