// bdes_timeutil.h              -*-C++-*-
#ifndef INCLUDED_BDES_TIMEUTIL
#define INCLUDED_BDES_TIMEUTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a platform-neutral functional interface to system clocks.
//
//@DEPRECATED: Use 'bsls_timeutil' instead.
//
//@CLASSES:
//   bdes_TimeUtil: namespace for platform-neutral system-time pure procedures
//
//@AUTHOR: Tom Marshall (tmarshal)
//
//@SEE_ALSO: bdes_stopwatch
//
//@DESCRIPTION: This component provides a set of platform-neutral pure
// procedures to access real-time system clock functionality.  High-resolution
// time functions intended for interval-timing return a time interval in
// nanoseconds (1 nsec = 1E-9 sec) as a 64-bit integer.
//
///Accuracy and Precision
///----------------------
// 'bdes_TimeUtil' high-resolution functions return time values as absolute
// nanoseconds from an arbitrary reference that will *in many cases* remain
// fixed within a single process (and among running processes on a single
// machine).  Absolute monotonic behavior is platform-dependent, however, as
// are accuracy and useful precision, despite the nominal nanosecond precision
// implied by the return value.  The user is advised to determine the actual
// performance on each platform of interest.
//
///Usage
///-----
// The following snippets of code illustrate how to use 'bdes_TimeUtil'
// functions to implement a very simple timer.  Only the most primitive
// functionality is illustrated.  See the 'bdes_Stopwatch' component for a
// better example of a timer interface.
//..
//  // my_timer.h
//
//  #ifndef INCLUDED_BDES_PLATFORMUTIL
//  #include <bdes_platformutil.h>   // bdes_PlatformUtil::Int64
//  #endif
//
//  #ifndef INCLUDED_BDES_TIMEUTIL
//  #include <bdes_timeutil.h>
//  #endif
//
// class my_Timer {
//     // This class implements a simple interval timer that is created in
//     // the "running" state, and may be queried for its cumulative
//     // interval (as a 'double', in seconds) but never stopped or reset.
//
//     bdes_PlatformUtil::Int64 d_startWallTime; // time at creation (nsec)
//     bdes_PlatformUtil::Int64 d_startUserTime; // time at creation (nsec)
//     bdes_PlatformUtil::Int64 d_startSystemTime; // time at creation (nsec)
//
//   public:
//     // CREATORS
//     my_Timer() {
//         d_startWallTime = bdes_TimeUtil::getTimer();
//         d_startUserTime = bdes_TimeUtil::getProcessUserTimer();
//         d_startSystemTime = bdes_TimeUtil::getProcessSystemTimer();
//     }
//         // Create a timer object initialized with the times at creation.
//         // All values returned by subsequent calls to 'elapsed<...>Time()'
//         // are with respect to this creation time.
//
//     ~my_Timer() {};
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
//     return (double) (bdes_TimeUtil::getTimer() - d_startWallTime) * 1.0E-9;
// }
//
// inline
// double my_Timer::elapsedUserTime()
// {
//     return (double) (bdes_TimeUtil::getProcessUserTimer() - d_startUserTime)
//                                                                    * 1.0E-9;
// }
//
// inline
// double my_Timer::elapsedSystemTime()
// {
//     return (double) (bdes_TimeUtil::getProcessSystemTimer()
//                                               - d_startSystemTime) * 1.0E-9;
// }
//  // ...
//..
// The 'my_Timer' object may be used to time some section of code at runtime
// as follows:
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
//      bsl::cout
//          << "elapsed wall time: " << dTw << bsl::endl
//          << "elapsed user time: " << dTu << bsl::endl
//          << "elapsed system time: " << dTs << bsl::endl;
//  }
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>  // required by 'bdes'
#endif

#ifndef INCLUDED_BSLS_TIMEUTIL
#include <bsls_timeutil.h>
#endif

namespace BloombergLP {

typedef bsls_TimeUtil bdes_TimeUtil;
    // This class provides a set of platform-neutral pure procedures to access
    // real-time system clock functionality.  High-resolution time functions
    // intended for interval-timing return an interval in nanoseconds
    // (1 nsec = 1E-9 sec) as a platform-independent 64-bit integer.

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
