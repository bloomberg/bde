// bsls_systemtime.h                                                  -*-C++-*-
#ifndef INCLUDED_BSLS_SYSTEMTIME
#define INCLUDED_BSLS_SYSTEMTIME

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide utilities to retrieve the system time.
//
//@CLASSES:
//   bsls::SystemTime: namespace for system-time functions
//
//@SEE_ALSO: bsls_timeinterval
//
//@DESCRIPTION: This component provides a 'struct', 'bsls::SystemTime', in
// which are defined a series of static methods for retrieving the current
// system time.  This component provides access to a monotonic clock and a
// real-time (wall) clock.
//
///Reference Time Point
///--------------------
// The 'bsls::TimeInterval' objects returned by functions in this component
// identify a time by providing a time interval from some fixed reference time
// point (or "epoch").  The clock types supported by 'bsls_systemtime' (see
// 'bsls_systemclocktype') define a reference time point as described in the
// table below:
//..
//  bsls::SystemClockType::Enum  Reference Time Point
//  ---------------------------  --------------------
//  e_REALTIME                   January 1, 1970 00:00.000 UTC (POSIX epoch)
//  e_MONOTONIC                  platform-dependent
//..
// The 'e_MONOTONIC' clock's reference time point is an unspecified,
// platform-dependent, value.  This means that the monotonic clock cannot be
// reliably used to determine the absolute wall clock time.  Monotonic clock
// times are frequently used to specify a fixed point in the future relative to
// the current (monotonic) clock time (e.g., for a timed-wait on a condition
// variable).  Note that the monotonic clock time may be (though certainly is
// not guaranteed to be) an arbitrary value relative to the process start time,
// meaning that 'bsls::TimeInterval' values from the monotonic clock should
// *not* be shared between processes.
//
///Thread Safety
///-------------
// The functions provided by 'bsls::SystemTime' are *thread-safe*.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Getting Current Wall Clock Time
/// - - - - - - - - - - - - - - - - - - - - -
// The following snippets of code illustrate how to use this utility component
// to obtain the system time by calling 'now' and 'nowRealtimeClock'.
//
// First, we call 'nowRealtimeClock', and set 't1', to the current time
// according to the real-time clock:
//..
//  bsls::TimeInterval t1 = bsls::SystemTime::nowRealtimeClock();
//
//  assert(bsls::TimeInterval() != t1);
//..
// Next, we sleep for 1 second:
//..
//  sleep(1);
//..
// Now, we call 'now', and supply 'e_REALTIME' to indicate a real-time clock
// value should be returned, and then set 't2' to the current time according
// to the real-time clock:
//..
//  bsls::TimeInterval t2 = bsls::SystemTime::now(
//                                          bsls::SystemClockType::e_REALTIME);
//
//  assert(bsls::TimeInterval() != t2);
//..
// Finally, we verify the interval between 't1' and 't2' is close to 1 second:
//..
//  bsls::TimeInterval interval = t2 - t1;
//
//  assert(bsls::TimeInterval(.9) <= interval &&
//                                   interval <= bsls::TimeInterval(1.1));
//..

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_SYSTEMCLOCKTYPE
#include <bsls_systemclocktype.h>
#endif

#ifndef INCLUDED_BSLS_TIMEINTERVAL
#include <bsls_timeinterval.h>
#endif

namespace BloombergLP {
namespace bsls {

                            // =================
                            // struct SystemTime
                            // =================

struct SystemTime {
    // This 'struct' provides a namespace for system-time-retrieval functions.

  public:
    // CLASS METHODS
    static TimeInterval now(SystemClockType::Enum clockType);
        // Return the 'TimeInterval' value representing the current system
        // time according to the specified 'clockType'.  The returned value is
        // the time interval between the reference time point defined for the
        // supplied 'clockType' (see {Reference Time Point}) and the current
        // time.

    static TimeInterval nowMonotonicClock();
        // Return the 'TimeInterval' value representing the current system time
        // according to the monotonic clock.  The returned value is the time
        // interval between the reference time point for the monotonic clock
        // (see {Reference Time Point}) and the current time.

    static TimeInterval nowRealtimeClock();
        // Return the 'TimeInterval' value representing the current system time
        // according to the real-time clock.  The returned value is the time
        // interval between the reference time point for the real-time clock
        // (see {Reference Time Point}) and the current time.
};

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                            // ----------------
                            // class SystemTime
                            // ----------------

// CLASS METHODS
inline
TimeInterval SystemTime::now(SystemClockType::Enum clockType)
{
    switch (clockType) {
      case SystemClockType::e_MONOTONIC: return nowMonotonicClock();  // RETURN
      case SystemClockType::e_REALTIME:  return nowRealtimeClock();   // RETURN
    }
    BSLS_ASSERT_OPT("Invalid clockType parameter value" && 0);
    return TimeInterval();
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
