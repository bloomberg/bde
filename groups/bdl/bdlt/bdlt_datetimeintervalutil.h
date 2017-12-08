// bdlt_datetimeintervalutil.h                                        -*-C++-*-
#ifndef INCLUDED_BDLT_DATETIMEINTERVALUTIL
#define INCLUDED_BDLT_DATETIMEINTERVALUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide non-primitive operations on 'bdlt::DatetimeInterval'.
//
//@CLASSES:
//   bdlt::DatetimeIntervalUtil: Utilities for 'bdlt::DatetimeInterval'
//
//@SEE_ALSO: bdlt_datetimeinterval
//
//@DESCRIPTION: This component provides non-primitive operations on
// 'bdlt::DatetimeInterval' objects.  In particular,
// 'bdlt::DatetimeIntervalUtil' supplies factory methods for
// 'bdlt::DatetimeInterval' objects.
//
// This utility component provides the following (static) methods:
//..
//   DatetimeInterval makeDays(int days);
//   DatetimeInterval makeHours(int hours);
//   DatetimeInterval makeMicroseconds(int microseconds);
//   DatetimeInterval makeMilliseconds(int milliseconds);
//   DatetimeInterval makeMinutes(int minutes);
//   DatetimeInterval makeSeconds(int seconds);
//..
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Simple usage of the various 'make*' functions.
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example simply shows how we can create a 'DatetimeInterval' object with
// 1 day, 2 hours, 3 minutes, 4 seconds, 5 millisecond, and 6 microseconds by
// composition, either with 'DatetimeInterval' constructor, or with 'make*'
// functions.  The results are then compared to each other and to a
// 'DatetimeInterval' that was created in one step.
//
// First, start with a default (0) 'DatetimeInterval'.
//..
//  DatetimeInterval m;
//  DatetimeInterval d;
//..
// Next, add 1 day to it, and assert that both methods are equal.
//..
//  m += DatetimeIntervalUtil::makeDays(1);
//  d += DatetimeInterval(1, 0, 0, 0, 0, 0);
//  assert(m == d);
//..
// Then, add 2 hours to it, and assert that both methods are equal.
//..
//  m += DatetimeIntervalUtil::makeDays(1);
//  d += DatetimeInterval(0, 2, 0, 0, 0, 0);
//  assert(m == d);
//..
// Next, add 3 minutes to it, and assert that both methods are equal.
//..
//  m += DatetimeIntervalUtil::makeMinutes(3);
//  d += DatetimeInterval(0, 0, 3, 0, 0, 0);
//  assert(m == d);
//..
// Then, add 4 seconds to it, and assert that both methods are equal.
//..
//  m += DatetimeIntervalUtil::makeSeconds(4);
//  d += DatetimeInterval(0, 0, 0, 4, 0, 0);
//  assert(m == d);
//..
// Next, add 5 milliseconds to it, and assert that both methods are equal.
//..
//  m += DatetimeIntervalUtil::makeMilliseconds(5);
//  d += DatetimeInterval(0, 0, 0, 0, 5, 0);
//  assert(m == d);
//..
// Then, add 6 microseconds to it, and assert that both methods are equal.
//..
//  m += DatetimeIntervalUtil::makeMicroseconds(6);
//  d += DatetimeInterval(0, 0, 0, 0, 0, 6);
//  assert(m == d);
//..
// Finally, we create an create a 'DatetimeInterval' with  the final value and
// compare to the objects built in steps.
//..
//  DatetimeInterval f(1, 2, 3, 4, 5, 6);
//  assert(f == m);
//  assert(f == d);
//..

#include <bdlscm_version.h>
#include <bdlt_datetimeinterval.h>

namespace BloombergLP {
namespace bdlt {

                            // ===========================
                            // struct DatetimeIntervalUtil
                            // ===========================

struct DatetimeIntervalUtil {
    // This utility 'struct' provides a namespace for a suite of functions
    // operating on objects of type 'DatetimeInterval'.

  public:
    // CLASS METHODS
    static DatetimeInterval makeDays(int days);
        // Return a 'DatetimeInterval' object having the value given by the
        // specified 'days', and zero for all other attributes.

    static DatetimeInterval makeHours(int hours);
        // Return a 'DatetimeInterval' object having the value given by the
        // specified 'hours', and zero for all other attributes.

    static DatetimeInterval makeMicroseconds(int microseconds);
        // Return a 'DatetimeInterval' object having the value given by the
        // specified 'microseconds)', and zero for all other attributes.

    static DatetimeInterval makeMilliseconds(int milliseconds);
        // Return a 'DatetimeInterval' object having the value given by the
        // specified 'milliseconds', and zero for all other attributes.

    static DatetimeInterval makeMinutes(int minutes);
        // Return a 'DatetimeInterval' object having the value given by the
        // specified 'minutes', and zero for all other attributes.

    static DatetimeInterval makeSeconds(int seconds);
        // Return a 'DatetimeInterval' object having the value given by the
        // specified 'seconds', and zero for all other attributes.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                            // ---------------------------
                            // struct DatetimeIntervalUtil
                            // ---------------------------

inline
DatetimeInterval DatetimeIntervalUtil::makeDays(int days)
{
    return DatetimeInterval(days);
}

inline
DatetimeInterval DatetimeIntervalUtil::makeHours(int hours)
{
    return DatetimeInterval(0, hours);
}

inline
DatetimeInterval DatetimeIntervalUtil::makeMicroseconds(int microseconds)
{
    return DatetimeInterval(0, 0, 0, 0, 0, microseconds);
}

inline
DatetimeInterval DatetimeIntervalUtil::makeMilliseconds(int milliseconds)
{
    return DatetimeInterval(0, 0, 0, 0, milliseconds);
}

inline
DatetimeInterval DatetimeIntervalUtil::makeMinutes(int minutes)
{
    return DatetimeInterval(0, 0, minutes);
}

inline
DatetimeInterval DatetimeIntervalUtil::makeSeconds(int seconds)
{
    return DatetimeInterval(0, 0, 0, seconds);
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
