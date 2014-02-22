// bdetu_systemtime.h                                                 -*-C++-*-
#ifndef INCLUDED_BDETU_SYSTEMTIME
#define INCLUDED_BDETU_SYSTEMTIME

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide utilities to retrieve the system time.
//
//@CLASSES:
//   bdetu_SystemTime: namespace for system-time procedures
//
//@SEE_ALSO: bdet_timeinterval
//
//@AUTHOR: Khalid Shafiq (kshafiq), Steven Breitstein (sbreitstein)
//
//@DESCRIPTION: This component provides static methods for retrieving system
// time.  The system time is expressed as a time interval between the current
// time and a pre-determined historical time, 00:00 UTC, January 1, 1970.  This
// component provides access to a monotonic clock, a realtime (wall) clock, and
// also a dynamically replaceable callback mechanism for determining system
// time; note that this mechanism is in addition to the two clocks and does not
// affect the 'now(bdetu_SystemClockType::Type clockType)',
// 'nowMonotonicClock', and 'nowRealtimeClock' methods.
//
// For applications that choose to define their own mechanism for determining
// system time, this component provides the ability to install a custom
// callback function.  The behavior is undefined unless the callback provided
// is epoch-based.  Note that if an application provides its own mechanism to
// retrieve the system time, this mechanism will be used by all calls to 'now',
// 'nowAsDatetimeUtc' and 'loadCurrentTime'.  Otherwise the default
// implementation will be used.  An application can always use the default
// implementation by calling the 'loadSystemTimeDefault' method explicitly.
//
// This component also provides a similar callback mechanism for users to
// customize the function that returns the the system's local time offset from
// UTC time.  Note that the callback mechanism used to determine the local time
// offset is used by both the 'nowAsDatetimeLocal' and 'localTimeOffset'
// functions.  By default, the callback used is 'loadLocalTimeOffsetDefault'.
// Clients can override the default callback function by calling the '
// 'setLoadLocalTimeOffsetCallback' function.
//
///Thread Safety
///-------------
// The functions provided by 'bdetu_SystemTime' are *thread-safe* (meaning they
// may be called concurrently from multiple threads) *except* for those
// functions that set and retrieve the callback functions.  The functions
// that are *not* thread-safe are:
//: o 'setLoadLocalTimeOffsetCallback'
//: o 'setSystemTimeCallback'
//: o 'currentCallback'
//: o 'currentLoadLocalTimeOffsetCallback'
//: o 'currentSystemTimeCallback'
// These functions may *not* be called concurrently with *any* other function,
// and are intended to be called at most once once in 'main' before any
// threads have been started.  In addition, supplied user-defined callback
// functions must be *thread-safe*.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Getting Current Time
///- - - - - - - - - - - - - - - -
// The following snippets of code illustrate how to use this utility component
// to obtain the system time by calling 'now', 'nowAsDatetimeUtc',
// 'loadCurrentTime', 'now(bdetu_SystemClockType::e_REALTIME)', or
// 'nowRealtimeClock'.
//..
//    bdet_TimeInterval i0;
//    assert(0 == i0);
//..
// Next call the utility function 'now' to obtain the system time.
//..
//    i0 = bdetu_SystemTime::now();
//    assert(0 != i0);
//..
// Next call the utility function 'nowAsDatetimeUtc' to obtain the system time.
//..
//    bdet_Datetime i1 = bdetu_SystemTime::nowAsDatetimeUtc();
//    assert(bdetu_Epoch::epoch() < i1);
//    bdet_DatetimeInterval dti = i1 - bdetu_Epoch::epoch();
//    assert(i0.totalMilliseconds() <= dti.totalMilliseconds());
//..
// Now call the utility function 'loadCurrentTime' to load the system time
// into i2;
//..
//    bdet_TimeInterval i2;
//    assert(0 == i2);
//    bdetu_SystemTime::loadCurrentTime(&i2);
//    assert(0 != i2);
//    assert(dti.totalMilliseconds() <= i2.totalMilliseconds());
//                                             //  Presumably, 0 < i0 < i1 < i2
//..
// Finally verify the 'now(bdetu_SystemClockType::e_REALTIME)' and
// 'nowRealtimeClock' methods return values close to the one returned by 'now'.
//..
//    assert(bdetu_SystemTime::now(bdetu_SystemClockType::e_REALTIME) - i0
//                                                  < bdet_TimeInterval(1, 0));
//    assert(bdetu_SystemTime::nowRealtimeClock() - i0
//                                                  < bdet_TimeInterval(1, 0));
//..
//
///Example 2: Using 'loadSystemTimeDefault'
/// - - - - - - - - - - - - - - - - - - - -
// The following snippets of code illustrate how to use 'loadSystemTimeDefault'
// function (Note that 'loadSystemTimeDefault') provides a default
// implementation to retrieve system time.
//
// First create a default object 'i3' of 'bdet_TimeInterval'.
//..
//    bdet_TimeInterval i3;
//    assert(0 == i3);
//..
// Next call the utility function 'loadSystemTimeDefault' to load the
// system time into 'i3'.
//..
//    bdetu_SystemTime::loadSystemTimeDefault(&i3);
//    assert(0 != i3);
//..
// Create another object 'i4' of 'bdet_TimeInterval'
//..
//    bdet_TimeInterval i4;
//    assert(0 == i4);
//..
// Then call the utility function 'loadSystemTimeDefault' again to load the
// system time into 'i4'.
//..
//    bdetu_SystemTime::loadSystemTimeDefault(&i4);
//    assert(i4 >= i3);
//..
//
///Example 3: Setting the System Time Callback
///- - - - - - - - - - - - - - - - - - - - - -
// For applications that choose to define their own mechanism for determining
// system time, the 'bdetu_SystemTime' utility provides the ability to install
// a custom system-time callback.
//
// First, we define the user-defined callback function 'getClientTime':
//..
//    void getClientTime(bdet_TimeInterval *result)
//    {
//        result->setInterval(1,1);
//    }
//..
// Then, store the address of the user-defined callback function
// 'getClientTime' into 'callback_user_ptr1':
//..
//    const bdetu_SystemTime::SystemTimeCallback callback_user_ptr1
//                                                            = &getClientTime;
//..
// Next, call the utility function 'setSystemTimeCallback' to load the
// user-defined callback function:
//..
//    bdetu_SystemTime::setSystemTimeCallback(callback_user_ptr1);
//    assert(callback_user_ptr1
//                           == bdetu_SystemTime::currentSystemTimeCallback());
//..
// Then, create object 'i5' and 'i6' of 'bdet_TimeInterval':
//..
//    bdet_TimeInterval i5;
//    assert(0 == i5);
//    bdet_TimeInterval i6;
//    assert(0 == i6);
//..
// Now, call the utility function 'now' and get the system time into 'i5':
//..
//    i5 = bdetu_SystemTime::now();
//    assert(1 == i5.seconds());
//    assert(1 == i5.nanoseconds());
//..
// Finally, call utility function 'loadCurrentTime' to load the system time
// into 'i6':
//..
//    bdetu_SystemTime::loadCurrentTime(&i6);
//    assert(1 == i6.seconds());
//    assert(1 == i6.nanoseconds());
//..
//
///Example 4: Using the Local Time Offset Callback
///- - - - - - - - - - - - - - - - - - - - - - - -
// Suppose one has to provide time stamp values that always reflect local time
// for a given location, even when local time transitions into and out of
// daylight saving time.  Further suppose that one must do this quite often
// (e.g., for every record in a high frequency log), so the performance of the
// default method for calculating local time offset is not adequate.  Creation
// and installation of a specialized user-defined callback for local time
// offset allows one to solve this problem.  Note that
// 'baetzo_localtimeoffsetutil' provides an efficient callback for computing
// the local time offset of all standard time zones.
//
// First, create a utility class that provides a method of type
// 'bdetu_SystemTime::LoadLocalTimeOffsetCallback' that is valid for the
// location of interest (New York) for the period of interest (the year 2013).
//..
//  struct MyLocalTimeOffsetUtilNewYork2013 {
//
//    private:
//      // DATA
//      static int           s_useCount;
//      static bdet_Datetime s_startOfDaylightSavingTime;  // UTC Datetime
//      static bdet_Datetime s_resumptionOfStandardTime;   // UTC Datetime
//
//    public:
//      // CLASS METHODS
//      static int loadLocalTimeOffset(int                  *result,
//                                    const bdet_Datetime&  utcDatetime);
//          // Load, into the specified 'result', the offset between the local
//          // time for the "America/New_York" timezone and UTC at the
//          // specified 'utcDatetime'.  The behavior is undefined unless
//          // '2013 == utcDatetime.date().year()'.
//
//      static int useCount();
//          // Return the number of invocations of the 'loadLocalTimeOffset'
//          // since the start of the process.
//  };
//
//  // DATA
//  int MyLocalTimeOffsetUtilNewYork2013::s_useCount = 0;
//
//  bdet_Datetime
//  MyLocalTimeOffsetUtilNewYork2013::s_startOfDaylightSavingTime(2013,
//                                                                   3,
//                                                                  10,
//                                                                   7);
//  bdet_Datetime
//  MyLocalTimeOffsetUtilNewYork2013::s_resumptionOfStandardTime(2013,
//                                                                 11,
//                                                                  3,
//                                                                  6);
//  // CLASS METHODS
//  int MyLocalTimeOffsetUtilNewYork2013::loadLocalTimeOffset(
//                                           int                  *result,
//                                           const bdet_Datetime&  utcDatetime)
//  {
//      assert(result);
//      assert(2013 == utcDatetime.date().year());
//
//      *result = utcDatetime < s_startOfDaylightSavingTime ? -18000:
//                utcDatetime < s_resumptionOfStandardTime  ? -14400:
//                                                            -18000;
//      ++s_useCount;
//      return 0;
//  }
//
//  int MyLocalTimeOffsetUtilNewYork2013::useCount()
//  {
//      return s_useCount;
//  }
//..
// Note that the transition times into and out of daylight savings for New York
// are given in UTC.  Also notice that we do not attempt to make the
// 'loadLocalTimeOffset' method 'inline', since we must take its address to
// install it as the callback.
//
// Next, we install this 'loadLocalTimeOffset' as the local time offset
// callback.
//..
//  bdetu_SystemTime::LoadLocalTimeOffsetCallback defaultCallback =
//                            bdetu_SystemTime::setLoadLocalTimeOffsetCallback(
//                                       &MyLocalTimeOffsetUtilNewYork2013::
//                                                        loadLocalTimeOffset);
//
//  assert(bdetu_SystemTime::loadLocalTimeOffsetDefault == defaultCallback);
//  assert(&MyLocalTimeOffsetUtilNewYork2013::loadLocalTimeOffset
//       == bdetu_SystemTime::currentLoadLocalTimeOffsetCallback());
//..
// Now, we can use the 'bdetu_SystemTime::loadLocalTimeOffset' method to obtain
// the local time offsets in New York on several dates of interest.  The
// increasing values from our 'useCount' method assures us that the callback we
// defined is indeed being used.
//..
//  assert(0 == MyLocalTimeOffsetUtilNewYork2013::useCount());
//
//  int offset;
//  bdet_Datetime     newYearsDay(2013,  1,  1);
//  bdet_Datetime independenceDay(2013,  7,  4);
//  bdet_Datetime     newYearsEve(2013, 12, 31);
//
//  bdetu_SystemTime::loadLocalTimeOffset(&offset, newYearsDay);
//  assert(-5 * 3600 == offset);
//  assert(        1 == MyLocalTimeOffsetUtilNewYork2013::useCount());
//
//  bdetu_SystemTime::loadLocalTimeOffset(&offset, independenceDay);
//  assert(-4 * 3600 == offset);
//  assert(        2 == MyLocalTimeOffsetUtilNewYork2013::useCount());
//
//  bdetu_SystemTime::loadLocalTimeOffset(&offset, newYearsEve);
//  assert(-5 * 3600 == offset);
//  assert(        3 == MyLocalTimeOffsetUtilNewYork2013::useCount());
//..
// Finally, to be neat, we restore the local time offset callback to the
// default callback:
//..
//  bdetu_SystemTime::setLoadLocalTimeOffsetCallback(defaultCallback);
//  assert(&bdetu_SystemTime::loadLocalTimeOffsetDefault
//      == bdetu_SystemTime::currentLoadLocalTimeOffsetCallback());
//..

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDETU_DATETIME
#include <bdetu_datetime.h>
#endif

#ifndef INCLUDED_BDETU_DATETIMEINTERVAL
#include <bdetu_datetimeinterval.h>
#endif

#ifndef INCLUDED_BDETU_EPOCH
#include <bdetu_epoch.h>
#endif

#ifndef INCLUDED_BDETU_SYSTEMCLOCKTYPE
#include <bdetu_systemclocktype.h>
#endif

#ifndef INCLUDED_BDET_DATETIME
#include <bdet_datetime.h>
#endif

#ifndef INCLUDED_BDET_DATETIMEINTERVAL
#include <bdet_datetimeinterval.h>
#endif

#ifndef INCLUDED_BDET_TIMEINTERVAL
#include <bdet_timeinterval.h>
#endif

namespace BloombergLP {

                            // ======================
                            // class bdetu_SystemTime
                            // ======================

struct bdetu_SystemTime {
    // This 'struct' provides a namespace for stateful system-time-retrieval
    // procedures.  These methods are alias-safe and exception-neutral.  The
    // use of a *subset* of these procedures is thread-safe (see
    // 'Thread Safety').

    // TYPES
    typedef void (*LoadLocalTimeOffsetCallback)(
                                     int                   *offsetInSecondsPtr,
                                     const bdet_Datetime&   utcDatetime);
        // 'LoadLocalTimeOffsetCallback' is an alias for the type of a function
        // that loads to the specified 'offsetInSecondsPtr' the offset of the
        // local time from UTC time for the specified 'utcDatetime'.  This
        // function must be thread-safe in multithreaded builds.  Note that the
        // installed callback function must have geographic information
        // specifying the local timezone.

    typedef void (*SystemTimeCallback)(bdet_TimeInterval *result);
        // 'SystemTimeCallback' is a callback function pointer for a callback
        // function that returns 'void' and takes as arguments a
        // 'bdet_TimeInterval' object 'result'.  This function must be
        // thread-safe in multithreaded builds.

  private:
    static LoadLocalTimeOffsetCallback s_loadLocalTimeOffsetCallback_p;
                                       // address of local-time callback

    static SystemTimeCallback          s_systime_callback_p;
                                       // address of system-time callback

  public:
    // CLASS METHODS

                        // ** now methods **

    static bdet_DatetimeInterval localTimeOffset();
        // Return a 'bdet_DatetimeInterval' value representing the difference
        // between the current local time and the current UTC time.  This
        // method uses the currently installed local-time-offset callback
        // mechanism.

    static bdet_TimeInterval now();
        // Return the 'bdet_TimeInterval' value representing the current system
        // time using the currently installed callback function.  Note that the
        // return value is an absolute offset since the epoch, and has the same
        // value in all time zones.

    static bdet_TimeInterval now(bdetu_SystemClockType::Type clockType);
        // Return the 'bdet_TimeInterval' value representing the current system
        // time according to the specified 'clock'.

    static bdet_Datetime nowAsDatetimeUtc();
        // Return the 'bdet_Datetime' value representing the current system
        // time using the currently installed callback function consistent with
        // 'now'.  Note that the returned value is in Utc.

    static bdet_Datetime nowAsDatetime();
        // Return the 'bdet_Datetime' value representing the current system
        // time using the currently installed callback function consistent with
        // 'now'.  Note that the returned value is in Utc.
        //
        // DEPRECATED: replaced by 'nowAsDatetimeUtc'

    static bdet_Datetime nowAsDatetimeGMT();
        // Return the 'bdet_Datetime' value representing the current system
        // time using the currently installed callback function consistent with
        // 'now'.  Note that the returned value is in Utc.
        //
        // DEPRECATED: replaced by 'nowAsDatetimeUtc'

    static bdet_Datetime nowAsDatetimeLocal();
        // Return the 'bdet_Datetime' value representing the current system
        // time in the local time zone using the currently installed system
        // time and local time offset callbacks.

    static bdet_TimeInterval nowMonotonicClock();
        // Return the 'bdet_TimeInterval' value representing the current system
        // time according to the monotonic clock.

    static bdet_TimeInterval nowRealtimeClock();
        // Return the 'bdet_TimeInterval' value representing the current system
        // time according to the realtime (wall) clock.

                        // ** load methods **

    static void loadCurrentTime(bdet_TimeInterval *result);
        // Load into the specified 'result', the current system time using
        // the currently installed system-time callback mechanism.

    static void loadLocalTimeOffset(int                   *result,
                                    const bdet_Datetime&  utcDatetime);
        // Load into the specified 'result' the offset in seconds between the
        // local time and the UTC time for the specified 'utcDatetime' using
        // the currently installed local time offset callback mechanism.

                        // ** default callbacks **

    static void loadLocalTimeOffsetDefault(int                  *result,
                                           const bdet_Datetime&  utcDatetime);
        // Load into the specified 'result' the offset in seconds of the local
        // time from UTC time for the specified 'utcDatetime'.  Note that the
        // local time zone is determined by the 'TZ' environment variable in
        // the same manner as the 'localtime' POSIX function.

    static void loadSystemTimeDefault(bdet_TimeInterval *result);
        // Load into the specified 'result' the current system time.  This is
        // the default system-time callback implementation for system time
        // retrieval.  The obtained system time is expressed as a time interval
        // between the current time and '00:00 UTC, January 1, 1970'.  On UNIX
        // (Solaris, LINUX and DG-UNIX) this function provides a microsecond
        // resolution.  On Windows (NT, WIN2000, 95, 98 etc) it provides a
        // resolution of 100 nanoseconds.

                        // ** set callbacks **

    static LoadLocalTimeOffsetCallback setLoadLocalTimeOffsetCallback(
                                         LoadLocalTimeOffsetCallback callback);
        // Install the user-specified custom 'callback' function to load the
        // offset in seconds between the local time and UTC time at a specified
        // UTC date and time.  Return the previously installed callback.

    static SystemTimeCallback
    setSystemTimeCallback(SystemTimeCallback callback);
        // Install the user-specified custom 'callback' function to retrieve
        // the system time.  Return the previously installed callback function.
        // The behavior of other methods in this component is undefined unless
        // 'callback' returns an absolute offset since the epoch time 00:00
        // UTC, January 1, 1970.

                        // ** get current callbacks **

    static SystemTimeCallback currentCallback();
        // Return the currently installed 'SystemTimeCallback' function.
        //
        // DEPRECATED: replaced by 'currentSystemTimeCallback'

    static LoadLocalTimeOffsetCallback currentLoadLocalTimeOffsetCallback();
        // Return the currently installed 'LoadLocalTimeOffsetCallback'
        // function.

    static SystemTimeCallback currentSystemTimeCallback();
        // Return the currently installed 'SystemTimeCallback' function.
};

// ==========================================================================
//                       INLINE FUNCTION DEFINITIONS
// ==========================================================================

                            // ----------------------
                            // class bdetu_SystemTime
                            // ----------------------

// CLASS METHODS

                        // ** now methods **

inline
bdet_DatetimeInterval bdetu_SystemTime::localTimeOffset()
{
    BSLS_ASSERT_SAFE(s_loadLocalTimeOffsetCallback_p);

    int offsetInSeconds;
    (*s_loadLocalTimeOffsetCallback_p)(&offsetInSeconds, nowAsDatetimeUtc());
    return bdet_DatetimeInterval(0, 0, 0, offsetInSeconds);
}

inline
bdet_TimeInterval bdetu_SystemTime::now()
{
    bdet_TimeInterval timeInterval;
    s_systime_callback_p(&timeInterval);
    return timeInterval;
}

inline
bdet_TimeInterval bdetu_SystemTime::now(bdetu_SystemClockType::Type clockType)
{
    switch (clockType) {
      case bdetu_SystemClockType::e_MONOTONIC: return nowMonotonicClock();
      case bdetu_SystemClockType::e_REALTIME:  return nowRealtimeClock();
      default:
        BSLS_ASSERT_OPT("Invalid clockType parameter value" && 0);
        return bdet_TimeInterval();
    }
}

inline
bdet_Datetime bdetu_SystemTime::nowAsDatetimeUtc()
{
    bdet_DatetimeInterval datetimeInterval;
    bdetu_DatetimeInterval::convertToDatetimeInterval(&datetimeInterval,
                                                      now());
    return bdetu_Epoch::epoch() + datetimeInterval;
}

inline
bdet_Datetime bdetu_SystemTime::nowAsDatetime()
{
    return nowAsDatetimeGMT();
}

inline
bdet_Datetime bdetu_SystemTime::nowAsDatetimeGMT()
{
    return nowAsDatetimeUtc();
}

    static bdet_TimeInterval nowMonotonicClock();
        // Return the 'bdet_TimeInterval' value representing the current system
        // time according to the monotonic clock.

    static bdet_TimeInterval nowRealtimeClock();

                        // ** load methods **

inline
void bdetu_SystemTime::loadCurrentTime(bdet_TimeInterval *result)
{
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(s_systime_callback_p);

    s_systime_callback_p(result);
}

inline
void bdetu_SystemTime::loadLocalTimeOffset(int                  *result,
                                           const bdet_Datetime&  utcDatetime)
{
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(s_loadLocalTimeOffsetCallback_p);

    (*s_loadLocalTimeOffsetCallback_p)(result, utcDatetime);
}

                        // ** set callbacks **

inline
bdetu_SystemTime::LoadLocalTimeOffsetCallback
bdetu_SystemTime::setLoadLocalTimeOffsetCallback(
                                         LoadLocalTimeOffsetCallback callback)
{
    LoadLocalTimeOffsetCallback previousCallback =
                                               s_loadLocalTimeOffsetCallback_p;
    s_loadLocalTimeOffsetCallback_p = callback;
    return previousCallback;
}

inline
bdetu_SystemTime::SystemTimeCallback
bdetu_SystemTime::setSystemTimeCallback(
                                 bdetu_SystemTime::SystemTimeCallback callback)
{
    bdetu_SystemTime::SystemTimeCallback previousCallback =
                                                          s_systime_callback_p;
    s_systime_callback_p = callback;
    return previousCallback;
}

                        // ** get current callbacks **

inline
bdetu_SystemTime::SystemTimeCallback
bdetu_SystemTime::currentCallback()
{
    return s_systime_callback_p;
}

inline
bdetu_SystemTime::LoadLocalTimeOffsetCallback
bdetu_SystemTime::currentLoadLocalTimeOffsetCallback()
{
    return s_loadLocalTimeOffsetCallback_p;
}

inline
bdetu_SystemTime::SystemTimeCallback
bdetu_SystemTime::currentSystemTimeCallback()
{
    return s_systime_callback_p;
}

}  // close namespace BloombergLP


#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2014
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
