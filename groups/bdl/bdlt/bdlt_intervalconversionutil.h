// bdlt_intervalconversionutil.h                                      -*-C++-*-
#ifndef INCLUDED_BDLT_INTERVALCONVERSIONUTIL
#define INCLUDED_BDLT_INTERVALCONVERSIONUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functions to convert between time-interval representations.
//
//@CLASSES:
//  bdlt::IntervalConversionUtil: functions to convert time intervals
//
//@SEE_ALSO: bdlt_datetimeinterval, bsls_timeinterval
//
//@DESCRIPTION: This component provides a utility 'struct',
// 'bdlt::IntervalConversionUtil', that defines functions to convert between
// C++ value types providing different representations of time intervals,
// (e.g., 'bsls::TimeInterval' and 'bdlt::DatetimeInterval').
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Interfacing With an API That Uses 'bsls::TimeInterval'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Some APIs, such as 'bsls::SystemTime', use 'bsls::TimeInterval' in their
// interface.  In order to use those APIs in components implemented in terms of
// 'bdlt::DatetimeInterval', it is necessary to convert between the
// 'bsls::TimeInterval' and 'bdlt::DatetimeInterval' representations for a time
// interval.  This conversion can be accomplished conveniently using
// 'bdlt::IntervalConversionUtil'.
//
// Suppose we wish to pass the system time -- as returned by
// 'bsls::SystemTime::nowRealtimeClock' -- to a function that displays a time
// that is represented as a 'bdlt::DatetimeInterval' since the UNIX epoch.
//
// First, we include the declaration of the function that displays a
// 'bdlt::DatetimeInterval':
//..
//  void displayTime(const bdlt::DatetimeInterval& timeSinceEpoch);
//..
// Then, we obtain the current system time from 'bsls::SystemTime', and store
// it in a 'bsls::TimeInterval':
//..
//  bsls::TimeInterval systemTime = bsls::SystemTime::nowRealtimeClock();
//..
// Now, we convert the 'bsls::TimeInterval' into a 'bdlt::DatetimeInterval'
// using 'convertToDatetimeInterval':
//..
//  bdlt::DatetimeInterval timeSinceEpoch =
//         bdlt::IntervalConversionUtil::convertToDatetimeInterval(systemTime);
//
//  assert(timeSinceEpoch.totalMilliseconds() ==
//                                             systemTime.totalMilliseconds());
//..
// Finally, we display the time by passing the converted value to
// 'displayTime':
//..
//  displayTime(timeSinceEpoch);
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLT_DATETIMEINTERVAL
#include <bdlt_datetimeinterval.h>
#endif

#ifndef INCLUDED_BDLT_TIMEUNITRATIO
#include <bdlt_timeunitratio.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_BUILDTARGET
#include <bsls_buildtarget.h>
#endif

#ifndef INCLUDED_BSLS_TIMEINTERVAL
#include <bsls_timeinterval.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_CLIMITS
#include <bsl_climits.h>
#endif

namespace BloombergLP {
namespace bdlt {

                        // =============================
                        // struct IntervalConversionUtil
                        // =============================

struct IntervalConversionUtil {
    // This utility 'struct', 'IntervalConversionUtil', defines functions to
    // convert between 'bsls::TimeInterval' and 'bdlt::DatetimeInterval'
    // representations of time intervals.

  private:
    // PRIVATE TYPES
    typedef TimeUnitRatio Ratio;

    // PRIVATE CLASS DATA

#ifdef BSLS_ASSERT_SAFE_IS_ACTIVE
                          // DatetimeInterval Limits

    static const bsls::Types::Int64 k_DATETIME_INTERVAL_MILLISECONDS_MAX =
                  INT_MAX
                      * Ratio::k_MILLISECONDS_PER_DAY
                      + Ratio::k_MILLISECONDS_PER_DAY
                      - 1;

    static const bsls::Types::Int64 k_DATETIME_INTERVAL_MILLISECONDS_MIN =
                                    -1 * k_DATETIME_INTERVAL_MILLISECONDS_MAX
                                       - Ratio::k_MILLISECONDS_PER_DAY;

    static const bsls::Types::Int64 k_DATETIME_INTERVAL_SECONDS_MAX      =
                                    k_DATETIME_INTERVAL_MILLISECONDS_MAX
                                        / Ratio::k_MILLISECONDS_PER_SECOND;

    static const bsls::Types::Int64 k_DATETIME_INTERVAL_SECONDS_MIN      =
                                    k_DATETIME_INTERVAL_MILLISECONDS_MIN
                                        / Ratio::k_MILLISECONDS_PER_SECOND;

    static const bsls::Types::Int64 k_DATETIME_INTERVAL_NSEC_REMAINDER_MAX =
                                    (Ratio::k_MILLISECONDS_PER_SECOND - 1)
                                        * Ratio::k_NANOSECONDS_PER_MILLISECOND;

    static const bsls::Types::Int64 k_DATETIME_INTERVAL_NSEC_REMAINDER_MIN =
                                    (1 - Ratio::k_MILLISECONDS_PER_SECOND)
                                        * Ratio::k_NANOSECONDS_PER_MILLISECOND;
#endif

  public:
    // CLASS METHODS
    static DatetimeInterval convertToDatetimeInterval(
                                          const bsls::TimeInterval&  interval);
        // Return as a 'bdlt::DatetimeInterval' the (approximate) value of the
        // specified 'interval' truncated toward zero, to millisecond
        // resolution.  The behavior is undefined unless the value of
        // 'interval', expressed with nanosecond precision, is within the range
        // of time intervals supported by a 'DatetimeInterval' -- i.e.,
        // 'DT_MIN * 10^6 <= TI_NSECS <= DT_MAX * 10^6', where 'TI_NSECS' is
        // the total number of nanoseconds in 'interval', 'DT_MIN' is the
        // lowest (negative) value expressible by 'DatetimeInterval', and
        // 'DT_MAX' is the highest (positive) value expressible by
        // 'DatetimeInterval'.

    static bsls::TimeInterval convertToTimeInterval(
                                            const DatetimeInterval&  interval);
        // Return as a 'bsls::TimeInterval' the value of the specified
        // 'interval'.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                       // -----------------------------
                       // struct IntervalConversionUtil
                       // -----------------------------

// CLASS METHODS
inline
DatetimeInterval IntervalConversionUtil::convertToDatetimeInterval(
                                            const bsls::TimeInterval& interval)
{
    // Check that the value of 'interval' is within the valid range supported
    // by 'Dateinterval'.  Note that if the value of 'interval' is within the
    // valid range, we can call 'bsls::TimeInterval::totalMilliseconds' without
    // invoking undefined behavior.

    BSLS_ASSERT_SAFE(k_DATETIME_INTERVAL_SECONDS_MIN < interval.seconds()
                     || (   k_DATETIME_INTERVAL_SECONDS_MIN ==
                                interval.seconds()
                         && k_DATETIME_INTERVAL_NSEC_REMAINDER_MIN <=
                                interval.nanoseconds()));

    BSLS_ASSERT_SAFE(interval.seconds() < k_DATETIME_INTERVAL_SECONDS_MAX
                     || (   k_DATETIME_INTERVAL_SECONDS_MAX ==
                                interval.seconds()
                         && interval.nanoseconds() <=
                                k_DATETIME_INTERVAL_NSEC_REMAINDER_MAX));

    return DatetimeInterval(0,  // days
                            0,  // hours
                            0,  // minutes
                            0,  // seconds
                            interval.totalMilliseconds());
}

inline
bsls::TimeInterval IntervalConversionUtil::convertToTimeInterval(
                                              const DatetimeInterval& interval)
{
    return bsls::TimeInterval(interval.totalSeconds(),
                              interval.milliseconds() *
                                  Ratio::k_NANOSECONDS_PER_MILLISECOND_32);
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2014 Bloomberg Finance L.P.
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
