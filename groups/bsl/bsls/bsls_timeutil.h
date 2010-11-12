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
//   bsls_TimeUtil: namespace for platform-neutral system-time pure procedures
//
//@AUTHOR: Tom Marshall (tmarshal)
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
// 'bsls_TimeUtil' high-resolution functions return time values as absolute
// nanoseconds from an arbitrary reference that will *in many cases* remain
// fixed within a single process (and among running processes on a single
// machine).  Absolute monotonic behavior is platform-dependent, however, as
// are accuracy and useful precision, despite the nominal nanosecond precision
// implied by the return value.  The user is advised to determine the actual
// performance on each platform of interest.
//
///Usage
///-----
// The following snippets of code illustrate how to use 'bsls_TimeUtil'
// functions to implement a very simple timer.  Only the most primitive
// functionality is illustrated.  See the 'bsls_Stopwatch' component for a
// better example of a timer interface.
//..
//  // my_timer.h
//
//  #ifndef INCLUDED_BSLS_TYPES
//  #include <bsls_types.h>   // bsls_Types::Int64
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
//     bsls_Types::Int64 d_startWallTime;   // time at creation (nsec)
//     bsls_Types::Int64 d_startUserTime;   // time at creation (nsec)
//     bsls_Types::Int64 d_startSystemTime; // time at creation (nsec)
//
//   public:
//     // CREATORS
//     my_Timer() {
//         d_startWallTime = bsls_TimeUtil::getTimer();
//         d_startUserTime = bsls_TimeUtil::getProcessUserTimer();
//         d_startSystemTime = bsls_TimeUtil::getProcessSystemTimer();
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
//     return (double) (bsls_TimeUtil::getTimer() - d_startWallTime) * 1.0E-9;
// }
//
// inline
// double my_Timer::elapsedUserTime()
// {
//     return (double) (bsls_TimeUtil::getProcessUserTimer() - d_startUserTime)
//                                                                    * 1.0E-9;
// }
//
// inline
// double my_Timer::elapsedSystemTime()
// {
//     return (double) (bsls_TimeUtil::getProcessSystemTimer()
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

#ifdef BSLS_PLATFORM__OS_UNIX
    #ifndef INCLUDED_TIME
    #include <time.h>
    #define INCLUDED_TIME
    #endif
#endif

#ifdef BSLS_PLATFORM__OS_AIX
    #ifndef INCLUDED_SYS_TIME
    #include <sys/time.h>
    #define INCLUDED_SYS_TIME
    #endif
#endif

namespace BloombergLP {

                            // ====================
                            // struct bsls_TimeUtil
                            // ====================

struct bsls_TimeUtil {
    // This 'struct' provides a namespace for a set of platform-neutral pure
    // procedures to access real-time system clock functionality.
    // High-resolution time functions intended for interval-timing return an
    // interval in nanoseconds (1 nsec = 1E-9 sec) as a platform-independent
    // 64-bit integer.
    //
    // For maximum performance on some platforms where fetching the native
    // clock is significantly faster than converting the fetched value to
    // nanoseconds, this class also provides a "raw" method returning an
    // opaque native time value and a conversion method returning a value in
    // nanoseconds.

    // TYPES
#if   defined BSLS_PLATFORM__OS_SOLARIS
        typedef struct { bsls_Types::Int64 d_opaque; } OpaqueNativeTime;
#elif defined BSLS_PLATFORM__OS_AIX
        typedef timebasestruct_t                       OpaqueNativeTime;
#elif defined BSLS_PLATFORM__OS_HPUX
        typedef struct { bsls_Types::Int64 d_opaque; } OpaqueNativeTime;
#elif defined BSLS_PLATFORM__OS_LINUX
        typedef timespec                               OpaqueNativeTime;
#elif defined BSLS_PLATFORM__OS_UNIX
        typedef timeval                                OpaqueNativeTime;
#elif defined BSLS_PLATFORM__OS_WINDOWS
        typedef struct { bsls_Types::Int64 d_opaque; } OpaqueNativeTime;
#endif

    // CLASS METHODS
    static bsls_Types::Int64 convertRawTime(OpaqueNativeTime rawTime);
        // Convert the specified 'rawTime' to a value in nanoseconds,
        // referenced to an arbitrary but fixed origin, and return the result
        // of the conversion.  Note that this method is thread-safe only if
        // 'initialize' has been called before.

    static bsls_Types::Int64 getProcessSystemTimer();
        // Return the instantaneous values of a platform-dependent timer for
        // the current process system time in absolute nanoseconds referenced
        // to an arbitrary but fixed origin.  Note that this method is thread-
        // safe only if 'initialize' has been called before.

    static void getProcessTimers(bsls_Types::Int64 *systemTimer,
                                 bsls_Types::Int64 *userTimer);
        // Load into the specified 'systemTimer' and 'userTimer' the
        // instantaneous values of platform-dependent system timer and user
        // timer in absolute nanoseconds referenced to an arbitrary but fixed
        // origin.  Note that this method is thread-safe only if 'initialize'
        // has been called before.

    static bsls_Types::Int64 getProcessUserTimer();
        // Return the instantaneous values of a platform-dependent timer for
        // the current process user time in absolute nanoseconds referenced to
        // an arbitrary but fixed origin.  Note that this method is thread-safe
        // only if 'initialize' has been called before.

    static bsls_Types::Int64 getTimer();
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

    static void initialize();
        // Do a platform-dependent initialization for the utilities.  Note that
        // only after a call to this method all the following methods are
        // guaranteed to be thread-safe.
};

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008, 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
