// bdlt_datetimeintervalutil.h                                        -*-C++-*-
#ifndef INCLUDED_BDLT_DATETIMEINTERVALUTIL
#define INCLUDED_BDLT_DATETIMEINTERVALUTIL

// BDE_VERIFY pragma: -FABC01 we order functions by increasing resolution
// BDE_VERIFY pragma: -SEG03

#include <bsls_ident.h>
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
//  bdlt::DatetimeInterval makeDays(int days);
//  bdlt::DatetimeInterval makeHours(bsls::Types::Int64 hours);
//  bdlt::DatetimeInterval makeMinutes(bsls::Types::Int64 minutes);
//  bdlt::DatetimeInterval makeSeconds(bsls::Types::Int64 seconds);
//  bdlt::DatetimeInterval makeMilliseconds(bsls::Types::Int64 milliseconds);
//  bdlt::DatetimeInterval makeMicroseconds(bsls::Types::Int64 microseconds);
//..
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Simple Usage of the Various 'make*' Functions
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example shows how we can create a 'bdlt::DatetimeInterval' objects
// having values of 1 day, 2 hours, 3 minutes, 4 seconds, 5 millisecond, and 6
// microseconds by using the 'bdlt::DatetimeInterval' constructor and, more
// readably, by using the 'make*' functions.
//
// First, start with a default (0) 'bdlt::DatetimeInterval':
//..
//  bdlt::DatetimeInterval m;
//  bdlt::DatetimeInterval d;
//..
// Next, add 1 day to it, and assert that both objects are equal:
//..
//  m += bdlt::DatetimeIntervalUtil::makeDays(1);
//  d += bdlt::DatetimeInterval(1, 0, 0, 0, 0, 0);
//  assert(m == d);
//..
// Then, add 2 hours to it, and assert that both objects are equal:
//..
//  m += bdlt::DatetimeIntervalUtil::makeHours(2);
//  d += bdlt::DatetimeInterval(0, 2, 0, 0, 0, 0);
//  assert(m == d);
//..
// Next, add 3 minutes to it, and assert that both objects are equal:
//..
//  m += bdlt::DatetimeIntervalUtil::makeMinutes(3);
//  d += bdlt::DatetimeInterval(0, 0, 3, 0, 0, 0);
//  assert(m == d);
//..
// Then, add 4 seconds to it, and assert that both objects are equal:
//..
//  m += bdlt::DatetimeIntervalUtil::makeSeconds(4);
//  d += bdlt::DatetimeInterval(0, 0, 0, 4, 0, 0);
//  assert(m == d);
//..
// Next, add 5 milliseconds to it, and assert that both objects are equal:
//..
//  m += bdlt::DatetimeIntervalUtil::makeMilliseconds(5);
//  d += bdlt::DatetimeInterval(0, 0, 0, 0, 5, 0);
//  assert(m == d);
//..
// Then, add 6 microseconds to it, and assert that both objects are equal:
//..
//  m += bdlt::DatetimeIntervalUtil::makeMicroseconds(6);
//  d += bdlt::DatetimeInterval(0, 0, 0, 0, 0, 6);
//  assert(m == d);
//..
// Finally, we create an create a 'DatetimeInterval' with  the final value and
// compare to the objects built in steps:
//..
//  bdlt::DatetimeInterval f(1, 2, 3, 4, 5, 6);
//  assert(f == m);
//  assert(f == d);
//..
//
///Example 2: How to Improve Readability Using the 'make*' Functions
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example shows how we can create a 'bdlt::Datetime' objects having a
// value of now + 2 hours and 30 minutes by using the 'bdlt::DatetimeInterval'
// constructor and, more readably, by using the 'make*' functions.
//
// First, create a 'bdlt::Datetime' object having the current time:
//..
//  bdlt::Datetime now = bdlt::CurrentTime::now();
//..
// Now, create the 'bdlt::DatetimeInterval' objects and assign the desired
// values to them using the 'makeHours' and 'makeMinutes' functions, and using
// the 'bdlt::DatetimeInterval' constructor:
//..
//  bdlt::Datetime nextEventTime = now
//                               + bdlt::DatetimeIntervalUtil::makeHours(2)
//                               + bdlt::DatetimeIntervalUtil::makeMinutes(30);
//  bdlt::Datetime altEventTime  = now
//                               + bdlt::DatetimeInterval(0, 2, 30, 0, 0, 0);
//..
// Finally, assert that both results are equal:
//..
//  assert(nextEventTime == altEventTime);
//..

#include <bdlscm_version.h>
#include <bdlt_datetimeinterval.h>
#include <bsls_types.h>

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
        // Return a 'DatetimeInterval' object representing an duration of the
        // specified 'days' (exactly).

    static DatetimeInterval makeHours(bsls::Types::Int64 hours);
        // Return a 'DatetimeInterval' object representing an duration of the
        // specified 'hours' (exactly).

    static DatetimeInterval makeMinutes(bsls::Types::Int64 minutes);
        // Return a 'DatetimeInterval' object representing an duration of the
        // specified 'minutes' (exactly).

    static DatetimeInterval makeSeconds(bsls::Types::Int64 seconds);
        // Return a 'DatetimeInterval' object representing an duration of the
        // specified 'seconds' (exactly).

    static DatetimeInterval makeMilliseconds(bsls::Types::Int64 milliseconds);
        // Return a 'DatetimeInterval' object representing an duration of the
        // specified 'milliseconds' (exactly).

    static DatetimeInterval makeMicroseconds(bsls::Types::Int64 microseconds);
        // Return a 'DatetimeInterval' object representing an duration of the
        // specified 'microseconds' (exactly).
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
DatetimeInterval DatetimeIntervalUtil::makeHours(bsls::Types::Int64 hours)
{
    return DatetimeInterval(0, hours);
}

inline
DatetimeInterval DatetimeIntervalUtil::makeMinutes(bsls::Types::Int64 minutes)
{
    return DatetimeInterval(0, 0, minutes);
}

inline
DatetimeInterval DatetimeIntervalUtil::makeSeconds(bsls::Types::Int64 seconds)
{
    return DatetimeInterval(0, 0, 0, seconds);
}

inline
DatetimeInterval DatetimeIntervalUtil::makeMilliseconds(
                                               bsls::Types::Int64 milliseconds)
{
    return DatetimeInterval(0, 0, 0, 0, milliseconds);
}

inline
DatetimeInterval DatetimeIntervalUtil::makeMicroseconds(
                                               bsls::Types::Int64 microseconds)
{
    return DatetimeInterval(0, 0, 0, 0, 0, microseconds);
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
