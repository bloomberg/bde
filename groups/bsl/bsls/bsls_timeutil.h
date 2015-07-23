// bsls_timeutil.h                                                    -*-C++-*-
#ifndef INCLUDED_BSLS_TIMEUTIL
#define INCLUDED_BSLS_TIMEUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a platform-neutral functional interface to system clocks.
//
//@CLASSES:
//  bsls::TimeUtil: namespace for platform-neutral system-time pure procedures
//
//@SEE_ALSO: bsls_stopwatch
//
//@DESCRIPTION: This component provides a set of platform-neutral pure
// procedures to access real-time system clock functionality.  High-resolution
// time functions intended for interval-timing return a time interval in
// nanoseconds (1 nsec = 1E-9 sec) as a 64-bit integer.
//
///Accuracy and Precision
///----------------------
// 'bsls::TimeUtil' high-resolution functions return time values as absolute
// nanoseconds from an arbitrary reference that will *in many cases* remain
// fixed within a single process (and among running processes on a single
// machine).  Absolute monotonic behavior is platform-dependent, however, as
// are accuracy and useful precision, despite the nominal nanosecond precision
// implied by the return value.  The user is advised to determine the actual
// performance on each platform of interest.
//
///Accuracy on Windows
///- - - - - - - - - -
// On certain windows platform configurations, 'bsls::TimeUtil::getTimer' and
// 'bsls::TimeUtil::getRawTimer' can produce unreliable results.  On some
// machines, these high-resolution functions have been observed to run at
// inconsistent speeds, with worst cases as slow as half the speed of actual
// wall time.  This is known behavior of the underlying high-performance timer
// function 'QueryPerformanceCounter', upon which the Windows implementation of
// 'bsls::TimeUtil' relies.
//
///CPU Scaling
/// -  -  -  -
// The behavior of the timer on windows platforms depends on the interaction of
// operating system, BIOS, and processor, and certain combinations of the three
// (particularly older ones) are vulnerable to timer inaccuracy.  For example,
// frequently the 'QueryPerformanceCounter' function that 'TimeUtil' uses on
// Windows will utilize the CPU's timestamp counter (TSC), and CPUs with speed
// scaling mechanisms such as SpeedStep (frequently used for power management)
// will generally see the clock speed vary with the CPU frequency.  However,
// newer processors often provide an 'Invariant TSC' that solves this
// problem.  Also versions of Windows starting with Vista may internally handle
// the inconsistency by automatically using a lower resolution, but accurate,
// counter on processors that do not provide an 'Invariant TSC'.
//
///Multi-Core Issues
/// -  -  -  -  -  -
// In addition, on multi-core machines, each call to 'QueryPerformanceCounter'
// may read the TSC from a different CPU.  The TSCs of the CPUs may be out of
// sync, resulting in slightly inconsistent or even non-monotonic behavior.
//
// Reference: http://support.microsoft.com/kb/895980
//
///Ensuring Accurate Timers on Windows
///- - - - - - - - - - - - - - - - - -
// If a Windows machine appears to have a slow and/or inconsistent
// high-resolution timer, it can be reconfigured to avoid using the TSC.  On
// Windows XP and earlier versions, add the parameter '/usepmtimer' to the
// operating system's boot configuration in 'boot.ini'.  On Windows Vista and
// later, run the following command as an administrator:
//..
//  bcdedit /set useplatformclock true
//..
// Note that unless the machine has a High Performance Event Timer (HPET) and
// it has been enabled in the BIOS, these steps might reduce the resolution of
// the 'bsls::TimeUtil' high-resolution functions from the nanosecond range to
// the microsecond range (or worse).
//
///Precision on Windows
/// - - - - - - - - - -
// Providing that the underlying timer is capable of supporting the
// 'QueryPerformanceCounter' interface, 'getTimer' and 'convertRawTime' will
// perform their calculations to nanosecond precision based on the values
// reported by 'QueryPerformanceCounter'.  Because of overflow concerns, these
// routines do not simply divide the result of 'QueryPerformanceCounter' by the
// result of 'QueryPerformanceFrequency'.  In the course of calculating the
// final nanosecond-precision time, there are two places where some precision
// might be lost.  Therefore, the times reported by 'getTimer' and
// 'convertRawTime' may be as much as two nanoseconds less than the actual time
// expressed by the 'QueryPerformanceCounter' interface.  Note that the times
// will still be monotonically non-decreasing.
//
///Usage
///-----
// The following snippets of code illustrate how to use 'bsls::TimeUtil'
// functions to implement a very simple timer.  Only the most primitive
// functionality is illustrated.  See the 'bsls::Stopwatch' component for a
// better example of a timer interface.
//..
//  // my_timer.h
//
//  #ifndef INCLUDED_BSLS_TYPES
//  #include <bsls_types.h>   // bsls::Types::Int64
//  #endif
//
//  #ifndef INCLUDED_BSLS_TIMEUTIL
//  #include <bsls_timeutil.h>
//  #endif
//
// class my_Timer {
//     // This class implements a simple interval timer that is created in
//     // the "running" state, and may be queried for its cumulative
//     // interval (as a 'double', in seconds) but never stopped or reset.
//
//     bsls::Types::Int64 d_startWallTime;   // time at creation (nsec)
//     bsls::Types::Int64 d_startUserTime;   // time at creation (nsec)
//     bsls::Types::Int64 d_startSystemTime; // time at creation (nsec)
//
//   public:
//     // CREATORS
//     my_Timer() {
//         d_startWallTime = bsls::TimeUtil::getTimer();
//         d_startUserTime = bsls::TimeUtil::getProcessUserTimer();
//         d_startSystemTime = bsls::TimeUtil::getProcessSystemTimer();
//     }
//         // Create a timer object initialized with the times at creation.
//         // All values returned by subsequent calls to 'elapsed<...>Time()'
//         // are with respect to this creation time.
//
//     ~my_Timer() {}
//
//     // ACCESSORS
//     double elapsedWallTime();
//         // Return the total elapsed time in seconds since the creation of
//         // this timer object.
//     double elapsedUserTime();
//         // Return the elapsed user time in seconds since the creation of
//         // this timer object.
//     double elapsedSystemTime();
//         // Return the elapsed system time in seconds since the creation of
//         // this timer object.
// };
//
// inline
// double my_Timer::elapsedWallTime()
// {
//     return (double) (bsls::TimeUtil::getTimer() - d_startWallTime) * 1.0E-9;
// }
//
// inline
// double my_Timer::elapsedUserTime()
// {
//     return (double) (bsls::TimeUtil::getProcessUserTimer()
//                                                 - d_startUserTime) * 1.0E-9;
// }
//
// inline
// double my_Timer::elapsedSystemTime()
// {
//     return (double) (bsls::TimeUtil::getProcessSystemTimer()
//                                               - d_startSystemTime) * 1.0E-9;
// }
//  // ...
//..
// The 'my_Timer' object may be used to time some section of code at runtime as
// follows:
//..
//  // ...
//  {
//      my_Timer tw;
//      for (int i = 0; i < 1000000; ++i) {
//          // ...
//      }
//      double dTw = tw.elapsedWallTime();
//      my_Timer tu;
//      for (int i = 0; i < 1000000; ++i) {
//          // ...
//      }
//      double dTu = tu.elapsedUserTime();
//      my_Timer ts;
//      for (int i = 0; i < 1000000; ++i) {
//          // ...
//      }
//      double dTs = ts.elapsedSystemTime();
//      std::cout << "elapsed wall time:   " << dTw << std::endl
//                << "elapsed user time:   " << dTu << std::endl
//                << "elapsed system time: " << dTs << std::endl;
//  }
//..

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifdef BSLS_PLATFORM_OS_UNIX
    #ifndef INCLUDED_TIME_H
    #include <time.h>
    #define INCLUDED_TIME_H
    #endif
#endif

#if defined(BSLS_PLATFORM_OS_AIX) || defined(BSLS_PLATFORM_OS_FREEBSD) || defined(BSLS_PLATFORM_OS_DARWIN)
    #ifndef INCLUDED_SYS_TIME_H
    #include <sys/time.h>
    #define INCLUDED_SYS_TIME_H
    #endif
#endif

namespace BloombergLP {

namespace bsls {

                            // ===============
                            // struct TimeUtil
                            // ===============

struct TimeUtil {
    // This 'struct' provides a namespace for a set of platform-neutral pure
    // procedures to access real-time system clock functionality.
    // High-resolution time functions intended for interval-timing return an
    // interval in nanoseconds (1 nsec = 1E-9 sec) as a platform-independent
    // 64-bit integer.
    //
    // For maximum performance on some platforms where fetching the native
    // clock is significantly faster than converting the fetched value to
    // nanoseconds, this class also provides a "raw" method returning an opaque
    // native time value and a conversion method returning a value in
    // nanoseconds.

    // TYPES
#if   defined BSLS_PLATFORM_OS_SOLARIS
    typedef struct { Types::Int64 d_opaque; } OpaqueNativeTime;
#elif defined BSLS_PLATFORM_OS_AIX
    typedef timebasestruct_t                  OpaqueNativeTime;
#elif defined BSLS_PLATFORM_OS_HPUX
    typedef struct { Types::Int64 d_opaque; } OpaqueNativeTime;
#elif defined(BSLS_PLATFORM_OS_LINUX) || defined(BSLS_PLATFORM_OS_CYGWIN)
    typedef timespec                          OpaqueNativeTime;
#elif defined BSLS_PLATFORM_OS_DARWIN
    typedef struct { Types::Int64 d_opaque; } OpaqueNativeTime;
#elif defined BSLS_PLATFORM_OS_UNIX
    typedef timeval                           OpaqueNativeTime;
#elif defined BSLS_PLATFORM_OS_WINDOWS
    typedef struct { Types::Int64 d_opaque; } OpaqueNativeTime;
#endif

    // CLASS METHODS

                                  // Initializers

    static void initialize();
        // Do a platform-dependent initialization for the utilities.  Note that
        // the other methods in this component are guaranteed to be thread-safe
        // only after calling this method.

                                  // Operations

    static Types::Int64 convertRawTime(OpaqueNativeTime rawTime);
        // Convert the specified 'rawTime' to a value in nanoseconds,
        // referenced to an arbitrary but fixed origin, and return the result
        // of the conversion.  Note that this method is thread-safe only if
        // 'initialize' has been called before.

    static Types::Int64 getProcessSystemTimer();
        // Return the instantaneous values of a platform-dependent timer for
        // the current process system time in absolute nanoseconds referenced
        // to an arbitrary but fixed origin.  Note that this method is thread-
        // safe only if 'initialize' has been called before.

    static void getProcessTimers(Types::Int64 *systemTimer,
                                 Types::Int64 *userTimer);
        // Load into the specified 'systemTimer' and 'userTimer' the
        // instantaneous values of platform-dependent system timer and user
        // timer in absolute nanoseconds referenced to an arbitrary but fixed
        // origin.  Note that this method is thread-safe only if 'initialize'
        // has been called before.

    static Types::Int64 getProcessUserTimer();
        // Return the instantaneous values of a platform-dependent timer for
        // the current process user time in absolute nanoseconds referenced to
        // an arbitrary but fixed origin.  Note that this method is thread-safe
        // only if 'initialize' has been called before.

    static Types::Int64 getTimer();
        // Return the instantaneous value of a platform-dependent system timer
        // in absolute nanoseconds referenced to an arbitrary but fixed origin.
        // Note that this method is thread-safe only if 'initialize' has been
        // called before.

    static void getTimerRaw(OpaqueNativeTime *timeValue);
        // Load into the specified 'timeValue' the value of an opaque,
        // platform-dependent type representing the current time.  'timeValue'
        // must be converted by the 'convertRawTime' method to conventional
        // units (nanoseconds).  This method is intended to facilitate accurate
        // timing of small segments of code, and care must be used in
        // interpreting the results.  Note that this method is thread-safe only
        // if 'initialize' has been called before.

};

}  // close package namespace


}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
