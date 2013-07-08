// bdet_systemtimeutil.h                                              -*-C++-*-
#ifndef INCLUDED_BDET_SYSTEMTIMEUTIL
#define INCLUDED_BDET_SYSTEMTIMEUTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide utilities to get the current UTC and local times.
//
//@CLASSES:
//   bdet_SystemTimeUtil: namespace for system-time procedures
//
//@SEE_ALSO: bdet_timeinterval
//
//@AUTHOR: Khalid Shafiq (kshafiq), Steven Breitstein (sbreitstein)
//
//@DESCRIPTION: This component provides static methods for retrieving system
// time.  The system time is expressed as a time interval between the current
// time and a pre-determined historical time, 00:00 UTC, January 1, 1970.  This
// component operates using a dynamically replaceable callback mechanism.  For
// applications that choose to define their own mechanism for determining
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
///Thread-Safety
///-------------
// The functions provided by 'bdet_SystemTimeUtil' are *thread-safe* (meaning
// they may be called concurrently from multiple threads) *except* for those
// functions that set and retrieve the callback functions.  The functions
// that are *not* thread-safe are:
//: o 'setLoadLocalTimeOffsetCallback'
//: o 'setSystemTimeCallback'
//: o 'currentLoadLocalTimeOffsetCallback'
//: o 'currentSystemTimeCallback'
// These functions may *not* be called concurrently with *any* other function,
// and are intended to be called at most once in 'main' before any threads have
// been started.  In addition, supplied user-defined callback functions must be
// *thread-safe*.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Getting Current Time
///- - - - - - - - - - - - - - - -
// The following snippets of code illustrate how to use this utility component
// to obtain the system time by calling 'now', 'nowAsDatetimeUtc', or
// 'loadCurrentTime':
//..
//  bdet_TimeInterval i0;
//  assert(0 == i0);
//..
// Next call the utility function 'now' to obtain the system time:
//..
//  i0 = bdet_SystemTimeUtil::now();
//  assert(0 != i0);
//..
// Next call the utility function 'nowAsDatetimeUtc' to obtain the system time:
//..
//  bdet_Datetime i1 = bdet_SystemTimeUtil::nowAsDatetimeUtc();
//  assert(bdet_EpochUtil::epoch() < i1);
//  bdet_DatetimeInterval dti = i1 - bdet_EpochUtil::epoch();
//  assert(i0.totalMilliseconds() <= dti.totalMilliseconds());
//..
// Now call the utility function 'loadCurrentTime' to load the system time into
// 'i2':
//..
//  bdet_TimeInterval i2;
//  assert(0 == i2);
//  bdet_SystemTimeUtil::loadCurrentTime(&i2);
//  assert(0 != i2);
//  assert(dti.totalMilliseconds() <= i2.totalMilliseconds());
//                                             //  Presumably, 0 < i0 < i1 < i2
//..
//
///Example 2: Using 'loadSystemTimeDefault'
/// - - - - - - - - - - - - - - - - - - - -
// The following snippets of code illustrate how to use the
// 'loadSystemTimeDefault' function.  (Note that 'loadSystemTimeDefault'
// provides a default implementation to retrieve system time.)
//
// First, create a default object 'i3' of type 'bdet_TimeInterval':
//..
//  bdet_TimeInterval i3;
//  assert(0 == i3);
//..
// Next, call the utility function 'loadSystemTimeDefault' to load the
// system time into 'i3':
//..
//  bdet_SystemTimeUtil::loadSystemTimeDefault(&i3);
//  assert(0 != i3);
//..
// Create another object 'i4' of type 'bdet_TimeInterval':
//..
//  bdet_TimeInterval i4;
//  assert(0 == i4);
//..
// Then call the utility function 'loadSystemTimeDefault' again to load the
// system time into 'i4':
//..
//  bdet_SystemTimeUtil::loadSystemTimeDefault(&i4);
//  assert(i4 >= i3);
//..
//
///Example 3: Setting the System Time Callback
///- - - - - - - - - - - - - - - - - - - - - -
// For applications that choose to define their own mechanism for determining
// system time, the 'bdet_SystemTimeUtil' utility provides the ability to
// install a custom system-time callback.
//
// First, we define the user-defined callback function 'getClientTime':
//..
//  void getClientTime(bdet_TimeInterval *result)
//  {
//      result->setInterval(1,1);
//  }
//..
// Then, store the address of the user-defined callback function
// 'getClientTime' into 'callback_user_ptr1':
//..
//  const bdet_SystemTimeUtil::SystemTimeCallback callback_user_ptr1
//                                                            = &getClientTime;
//..
// Next, call the utility function 'setSystemTimeCallback' to load the
// user-defined callback function:
//..
//  bdet_SystemTimeUtil::setSystemTimeCallback(callback_user_ptr1);
//  assert(callback_user_ptr1
//                        == bdet_SystemTimeUtil::currentSystemTimeCallback());
//..
// Then, create object 'i5' and 'i6' of type 'bdet_TimeInterval':
//..
//  bdet_TimeInterval i5;
//  assert(0 == i5);
//  bdet_TimeInterval i6;
//  assert(0 == i6);
//..
// Now, call the utility function 'now' and get the system time into 'i5':
//..
//  i5 = bdet_SystemTimeUtil::now();
//  assert(1 == i5.seconds());
//  assert(1 == i5.nanoseconds());
//..
// Finally, call utility function 'loadCurrentTime' to load the system time
// into 'i6':
//..
//  bdet_SystemTimeUtil::loadCurrentTime(&i6);
//  assert(1 == i6.seconds());
//  assert(1 == i6.nanoseconds());
//..
//
///Example 4: Using the Local Time Offset Callback
///- - - - - - - - - - - - - - - - - - - - - - - -
// Suppose one has to provide timestamp values that always reflect local time
// for a given location, even when local time transitions into and out of
// daylight-saving time.  Further suppose that one must do this quite often
// (e.g., for every record in a high frequency log), so the performance of the
// default method for calculating local time offset is not adequate.  Creation
// and installation of a specialized user-defined callback for the local time
// offset allows one to solve this problem.  Note that
// 'baetzo_localtimeoffsetutil' provides an efficient callback for computing
// the local time offset of all standard time zones.
//
// First, create a utility class that provides a method of type
// 'bdet_SystemTimeUtil::LoadLocalTimeOffsetCallback' that is valid for the
// location of interest (New York) for the period of interest (the year 2013):
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
//          // time for the "America/New_York" time zone and UTC at the
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
// Note that the transition times into and out of daylight-saving time for New
// York are given in UTC.  Also note that we do not attempt to make the
// 'loadLocalTimeOffset' method 'inline', since we must take its address to
// install it as the callback.
//
// Next, we install this 'loadLocalTimeOffset' as the local time offset
// callback:
//..
//  bdet_SystemTimeUtil::LoadLocalTimeOffsetCallback defaultCallback =
//                         bdet_SystemTimeUtil::setLoadLocalTimeOffsetCallback(
//                                       &MyLocalTimeOffsetUtilNewYork2013::
//                                                        loadLocalTimeOffset);
//
//  assert(bdet_SystemTimeUtil::loadLocalTimeOffsetDefault == defaultCallback);
//  assert(&MyLocalTimeOffsetUtilNewYork2013::loadLocalTimeOffset
//       == bdet_SystemTimeUtil::currentLoadLocalTimeOffsetCallback());
//..
// Now, we can use the 'bdet_SystemTimeUtil::loadLocalTimeOffset' method to
// obtain the local time offsets in New York on several dates of interest.  The
// increasing values from our 'useCount' method assures us that the callback we
// defined is indeed being used:
//..
//  assert(0 == MyLocalTimeOffsetUtilNewYork2013::useCount());
//
//  int offset;
//  bdet_Datetime     newYearsDay(2013,  1,  1);
//  bdet_Datetime independenceDay(2013,  7,  4);
//  bdet_Datetime     newYearsEve(2013, 12, 31);
//
//  bdet_SystemTimeUtil::loadLocalTimeOffset(&offset, newYearsDay);
//  assert(-5 * 3600 == offset);
//  assert(        1 == MyLocalTimeOffsetUtilNewYork2013::useCount());
//
//  bdet_SystemTimeUtil::loadLocalTimeOffset(&offset, independenceDay);
//  assert(-4 * 3600 == offset);
//  assert(        2 == MyLocalTimeOffsetUtilNewYork2013::useCount());
//
//  bdet_SystemTimeUtil::loadLocalTimeOffset(&offset, newYearsEve);
//  assert(-5 * 3600 == offset);
//  assert(        3 == MyLocalTimeOffsetUtilNewYork2013::useCount());
//..
// Finally, to be neat, we restore the local time offset callback to the
// default callback:
//..
//  bdet_SystemTimeUtil::setLoadLocalTimeOffsetCallback(defaultCallback);
//  assert(&bdet_SystemTimeUtil::loadLocalTimeOffsetDefault
//      == bdet_SystemTimeUtil::currentLoadLocalTimeOffsetCallback());
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDET_DATETIME
#include <bdet_datetime.h>
#endif

#ifndef INCLUDED_BDET_DATETIMEINTERVAL
#include <bdet_datetimeinterval.h>
#endif

#ifndef INCLUDED_BDET_DATETIMEUTIL
#include <bdet_datetimeutil.h>
#endif

#ifndef INCLUDED_BDET_EPOCHUTIL
#include <bdet_epochutil.h>
#endif

#ifndef INCLUDED_BDET_INTERVALCONVERSIONUTIL
#include <bdet_intervalconversionutil.h>
#endif

#ifndef INCLUDED_BDET_TIMEINTERVAL
#include <bdet_timeinterval.h>
#endif

namespace BloombergLP {

                            // =========================
                            // class bdet_SystemTimeUtil
                            // =========================

struct bdet_SystemTimeUtil {
    // This 'struct' provides a namespace for system time procedures.
    //
    // The functions provided are:
    //: o *exception-neutral* (agnostic)
    //:
    //: o *thread-safe*, *except* for those functions that set or obtain the
    //:   callback functions.  See {Thread Safety}.

    // TYPES
    typedef void (*LoadLocalTimeOffsetCallback)(
                                      int                  *offsetInSecondsPtr,
                                      const bdet_Datetime&  utcDatetime);
        // 'LoadLocalTimeOffsetCallback' is an alias for the type of a function
        // that loads to the specified 'offsetInSecondsPtr' the offset of the
        // local time from UTC time for the specified 'utcDatetime'.  This
        // function must be thread-safe in multit-hreaded builds.  Note that
        // the installed callback function must have geographic information
        // specifying the local time zone.

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

    static bdet_TimeInterval now();
        // Return a 'bdet_TimeInterval' value representing the current system
        // time using the currently installed callback function.  Note that the
        // return value is an absolute offset since the epoch, and has the same
        // value in all time zones.

    static bdet_Datetime nowAsDatetimeUtc();
        // Return a 'bdet_Datetime' value representing the current system time
        // using the currently installed callback function consistent with
        // 'now'.  Note that the returned value is in Utc.

    static bdet_Datetime nowAsDatetimeLocal();
        // Return a 'bdet_Datetime' value representing the current system time
        // in the local time zone using the currently installed system time and
        // local time offset callbacks.

    static bdet_DatetimeInterval localTimeOffset();
        // Return a 'bdet_DatetimeInterval' value representing the difference
        // between the current local time and the current UTC time.  This
        // method uses the currently installed local-time-offset callback
        // mechanism.

                        // ** load methods **

    static void loadCurrentTime(bdet_TimeInterval *result);
        // Load into the specified 'result', the current system time using
        // the currently installed system-time callback mechanism.

    static void loadLocalTimeOffset(int                  *result,
                                    const bdet_Datetime&  utcDatetime);
        // Load into the specified 'result' the offset in seconds between the
        // local time and the UTC time for the specified 'utcDatetime' using
        // the currently installed local time offset callback mechanism.

                        // ** default callbacks **

    static void loadLocalTimeOffsetDefault(int                  *result,
                                           const bdet_Datetime&  utcDatetime);
        // Load into the specified 'result' offset in seconds of the local time
        // from UTC time for the specified 'utcDatetime'.  Note that the local
        // time zone is determined by the 'TZ' environment variable in the same
        // manner as the 'localtime' POSIX function.

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

    static LoadLocalTimeOffsetCallback currentLoadLocalTimeOffsetCallback();
        // Return the currently installed 'LoadLocalTimeOffsetCallback'
        // function.

    static SystemTimeCallback currentSystemTimeCallback();
        // Return the currently installed 'SystemTimeCallback' function.
};

// ============================================================================
//                       INLINE FUNCTION DEFINITIONS
// ============================================================================

                            // -------------------------
                            // class bdet_SystemTimeUtil
                            // -------------------------

// CLASS METHODS

                        // ** now methods **

inline
bdet_TimeInterval bdet_SystemTimeUtil::now()
{
    bdet_TimeInterval timeInterval;
    s_systime_callback_p(&timeInterval);
    return timeInterval;
}

inline
bdet_Datetime bdet_SystemTimeUtil::nowAsDatetimeUtc()
{
    bdet_DatetimeInterval datetimeInterval;
    bdet_IntervalConversionUtil::timeIntervalToDatetimeInterval(
                                                             &datetimeInterval,
                                                             now());
    return bdet_EpochUtil::epoch() + datetimeInterval;
}

inline
bdet_DatetimeInterval bdet_SystemTimeUtil::localTimeOffset()
{
    BSLS_ASSERT_SAFE(s_loadLocalTimeOffsetCallback_p);

    int offsetInSeconds;
    (*s_loadLocalTimeOffsetCallback_p)(&offsetInSeconds, nowAsDatetimeUtc());
    return bdet_DatetimeInterval(0, 0, 0, offsetInSeconds);
}

                        // ** load methods **

inline
void bdet_SystemTimeUtil::loadCurrentTime(bdet_TimeInterval *result)
{
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(s_systime_callback_p);

    s_systime_callback_p(result);
}

inline
void bdet_SystemTimeUtil::loadLocalTimeOffset(
                                             int                  *result,
                                             const bdet_Datetime&  utcDatetime)
{
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(s_loadLocalTimeOffsetCallback_p);

    (*s_loadLocalTimeOffsetCallback_p)(result, utcDatetime);
}

                        // ** set callbacks **

inline
bdet_SystemTimeUtil::LoadLocalTimeOffsetCallback
bdet_SystemTimeUtil::setLoadLocalTimeOffsetCallback(
                                         LoadLocalTimeOffsetCallback callback)
{
    LoadLocalTimeOffsetCallback previousCallback =
                                               s_loadLocalTimeOffsetCallback_p;
    s_loadLocalTimeOffsetCallback_p = callback;
    return previousCallback;
}

inline
bdet_SystemTimeUtil::SystemTimeCallback
bdet_SystemTimeUtil::setSystemTimeCallback(
                              bdet_SystemTimeUtil::SystemTimeCallback callback)
{
    bdet_SystemTimeUtil::SystemTimeCallback previousCallback =
                                                          s_systime_callback_p;
    s_systime_callback_p = callback;
    return previousCallback;
}

                        // ** get current callbacks **

inline
bdet_SystemTimeUtil::LoadLocalTimeOffsetCallback
bdet_SystemTimeUtil::currentLoadLocalTimeOffsetCallback()
{
    return s_loadLocalTimeOffsetCallback_p;
}

inline
bdet_SystemTimeUtil::SystemTimeCallback
bdet_SystemTimeUtil::currentSystemTimeCallback()
{
    return s_systime_callback_p;
}

}  // close namespace BloombergLP

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
