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
//   bdetu_SystemTime: namespace for stateful system-time procedures
//
//@SEE_ALSO: bdet_timeinterval
//
//@AUTHOR: Khalid Shafiq (kshafiq), Steven Breitstein (sbreitstein)
//
//@DESCRIPTION: This component provides static methods for retrieving system
// time.  The system time is expressed as a time interval between the current
// time and a pre-determined historical time, 00:00 UTC, January 1, 1970.  This
// component operates using a dynamically replaceable callback mechanism.  For
// applications that choose to define there own mechanism for determining
// system time, this component provides the ability to install a custom
// callback function.  The behavior is undefined unless the callback provided
// is epoch-based.  Note that if an application provides its own mechanism to
// retrieve the system time, this mechanism will be used by all calls to 'now',
// 'nowAsDatetimeUtc' and 'loadCurrentTime'.  Otherwise the default
// implementation will be used.  An application can always use the default
// implementation by calling the 'loadSystemTimeDefault' method explicitly.
//
// This component also provides a similar callback mechanism for users to
// customize the function that returns the differential between local time and
// UTC time.  Note that if an application provides its own mechanism to
// retrieve the local time offset, this mechanism will be used by all calls to
// 'nowAsDdatetimeLocal' and 'localTimeOffset'.  Otherwise the default
// implementation will be used.  An application can always use the default
// implementation by calling the 'loadLocalTimeOffsetDefault' method
// explicitly.
//
///Usage 1
///-------
// The following snippets of code illustrate how to use this utility component
// to obtain the system time by calling 'now', 'nowAsDatetimeUtc', or
// 'loadCurrentTime'.
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
///Usage 2
///-------
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
///Usage 3
///-------
// For applications that choose to define there own mechanism for determining
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
#if 0
///Usage 4
///-------
// For applications that choose to define there own mechanism for determining
// the differential between local time and UTC time, the 'bdetu_SystemTime'
// utility provides the ability to install a custom local-time-offset callback.
//
// First, we define the user-defined callback function 'getLocalTimeOffset':
//..
//    bdet_DatetimeInterval getLocalTimeOffset()
//    {
//        return bdet_DatetimeInterval(0, 1);  // an hour differential
//    }
//..
// Then, store the address of the user-defined callback function
// 'getLocalTimeOffset' into 'callback_user_ptr1':
//..
//    const bdetu_SystemTime::LocalTimeOffsetCallback callback_user_ptr1
//                                                       = &getLocalTimeOffset;
//..
// Then call the utility function 'setLocalTimeOffsetCallback' to load the
// user-defined local-time-offset callback function:
//..
//    bdetu_SystemTime::setLocalTimeOffsetCallback(callback_user_ptr1);
//    assert(callback_user_ptr1
//           == bdetu_SystemTime::currentLocalTimeOffsetCallback());
//..
// Next, we call the utility function 'localTimeOffset' to get the current
// local time offset:
//..
//    bdet_DatetimeInterval i7 = bdetu_SystemTime::localTimeOffset();
//    assert(0 == i7.days());
//    assert(1 == i7.hours());
//..
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
    // This class provides a namespace for stateful system-time-retrieval
    // procedures.  These methods are alias-safe and exception-neutral.  These
    // procedures are *not* thread-safe.  The behavior is undefined if an
    // application attempts to set a callback mechanism via
    // 'setSystemTimeCallback' in a multi-threaded environment after threads
    // have been started.  The behavior is also undefined if an application
    // attempts to retrieve the system time by calling either 'now',
    // 'nowAsDatetimeUtc', or 'loadCurrentTime' methods while another thread
    // attempts to setup a new callback mechanism.  To avoid runtime issues in
    // a multi-threaded environment, 'setSystemTimeCallback' should be called
    // at most once in 'main' before any threads have been started.

    // TYPES
    typedef void (*SystemTimeCallback)(bdet_TimeInterval *result);
        // 'SystemTimeCallback' is a callback function pointer for a callback
        // function that returns 'void' and takes as arguments a
        // 'bdet_TimeInterval' object 'result'.

#if 0
    typedef bdet_DatetimeInterval (*LocalTimeOffsetCallback)();
        // 'LocalTimeOffsetCallback' is a callback function pointer for a
        // callback function that returns the time difference between local
        // time and UTC time.

    typedef bdet_DatetimeInterval (*GetLocalTimeOffsetCallback)(
                                                       bdet_TimeInterval  now,
                                                       void              *arg);
        // 'GetLocalTimeOffsetCallback' is a function that retrieve the offset
        // between local time and UTC when called with the specified, 'now'
        // (the current system time), and the specified opaque argument pointer
        // 'arg' (which must define the local timezone in a manner known to
        // 'callback').

    typedef struct {
        GetLocalTimeOffsetCallback  d_callback;
        void                       *d_arg;
    } GetLocalTimeOffsetCallbackSpec;
#endif

    typedef int (*LoadLocalTimeOffsetCallback)(
                                     int                   *offsetInSecondsPtr,
                                     const bdet_Datetime&   utcDatetime);
        // 'LoadLocalTimeOffsetCallback' is a function that loads to the
        // specified 'offsetInSecondsPtr' the offset of the local time from UTC
        // time for the specified 'utcDatetime'.  The function returns 0 on
        // success, and a non-zero value otherwise.  Note that the installed
        // callback function must have geographic information specifying the
        // local timezone.

  private:
    static SystemTimeCallback          s_systime_callback_p;
                                       // address of system-time callback
#if 0
    static LocalTimeOffsetCallback  s_localtimeoffset_callback_p;
                                       // address of local-time offset callback

    static GetLocalTimeOffsetCallbackSpec
                                   *s_getLocalTimeOffsetCallbackSpec_p;
                                       // address of get local-time callback
                                       // specifcation
#endif
    static LoadLocalTimeOffsetCallback s_loadLocalTimeOffsetCallback_p;
                                       // address of local-time callback

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

    static bdet_Datetime nowAsDatetime();
        // Return a 'bdet_Datetime' value representing the current system time
        // using the currently installed callback function consistent with
        // 'now'.  Note that the returned value is in Utc.
        //
        // DEPRECATED: replaced by 'nowAsDatetimeUtc'

    static bdet_Datetime nowAsDatetimeGMT();
        // Return a 'bdet_Datetime' value representing the current system time
        // using the currently installed callback function consistent with
        // 'now'.  Note that the returned value is in Utc.
        //
        // DEPRECATED: replaced by 'nowAsDatetimeUtc'

    static bdet_Datetime nowAsDatetimeLocal();
        // Return a 'bdet_Datetime' value representing the current system time
        // in the local time zone using the currently installed callback
        // function consistent with 'now' and the currently installed callback
        // function for obtaining the local time offset from UTC.

                        // ** XXX **

    static bdet_DatetimeInterval localTimeOffset();
        // Return a 'bdet_DatetimeInterval' value representing the current
        // difference between the local time and the UTC time.  This method
        // uses the currently installed local-time-offset callback mechanism.

    static void loadCurrentTime(bdet_TimeInterval *result);
        // Load into the specified 'result', the current system time using
        // the currently installed system-time callback mechanism.

    static int loadLocalTimeOffset(int                   *result,
                                   const bdet_Datetime&  utcDatetime);
        // Load into the specified 'result' the offset in seconds between the
        // local time and the UTC time for the specified 'utcDatetime'.  Return
        // 0 on success, and a non-zero value otherwise.  This method uses the
        // currently installed local-time-offset callback mechanism.

    static void loadSystemTimeDefault(bdet_TimeInterval *result);
        // Load into the specified 'result' the current system time.  This is
        // the default system-time callback implementation for system time
        // retrieval.  The obtained system time is expressed as a time interval
        // between the current time and '00:00 UTC, January 1, 1970'.  On UNIX
        // (Solaris, LINUX and DG-UNIX) this function provides a microsecond
        // resolution.  On Windows (NT, WIN2000, 95, 98 etc) it provides a
        // resolution of 100 nanoseconds.

    static int loadLocalTimeOffsetDefault(int                  *result,
                                          const bdet_Datetime&  utcDatetime);
        // Load into the specified 'result' offset in seconds of the local time
        // from UTC time for the specified 'utcDatetime'.  Return 0 on success,
        // and a non-zero value otherwise.  Note that the local time zone is
        // determined by the 'TZ' environment variable in the same manner as
        // the 'localtime' POSIX function.

#if 0
    static bdet_DatetimeInterval loadLocalTimeOffsetDefault();
        // Return a 'bdet_DatetimeInterval' value representing the current
        // differential between the local time and the UTC time.  This is the
        // default local-time-offset callback implementation for local time
        // offset retrieval.

    static bdet_DatetimeInterval getLocalTimeOffset();
        // Return a 'bdet_DatetimeInterval' value representing the current
        // differential between the local time and the UTC time using the
        // currently installed 'GetLocalTimeOffsetCallback' function and its
        // opaque argument pointer.

    static bdet_DatetimeInterval getLocalTimeOffset(
                                             const bdet_Datetime& utcDatetime);
        // Return a 'bdet_DatetimeInterval' value representing the current
        // differential between the local time and UTC time at the specified
        // 'utcDatetime'.
#endif

                        // ** set callbacks **

    static SystemTimeCallback
    setSystemTimeCallback(SystemTimeCallback callback);
        // Install the user-specified custom 'callback' function to retrieve
        // the system time.  Return the previously installed callback function.
        // The behavior of other methods in this component will be corrupted
        // unless 'callback' returns an absolute offset since the epoch time
        // 00:00 UTC, January 1, 1970.

#if 0
    static LocalTimeOffsetCallback
    setLocalTimeOffsetCallback(LocalTimeOffsetCallback callback);
        // Install the user-specified custom 'callback' function to retrieve
        // the offset between local time and UTC time.  Return the previously
        // installed callback function.  The behavior of other methods in this
        // component will be corrupted unless 'callback' returns a correct
        // offset.

    static GetLocalTimeOffsetCallbackSpec *
    setGetLocalTimeOffsetCallback(
                                 GetLocalTimeOffsetCallbackSpec *callbackSpec);
        // Install from the specified 'callbackSpec' a callback function to
        // retrieve the offset between local time and UTC when called with the
        // current system time and with the opaque argument pointer from
        // 'callbackSpec'.  The opaque argument must define the local timezone
        // in a manner known to the callback function).  Return the previously
        // installed callback specification.  The behavior of other methods in
        // this component will be corrupted unless 'callback' returns a correct
        // offset.
#endif
    static LoadLocalTimeOffsetCallback setLoadLocalTimeOffsetCallback(
                                         LoadLocalTimeOffsetCallback callback);
        // Install the user-specified custom 'callback' function to load the
        // offset in seconds between the local time and UTC time at a specified
        // UTC date and time.  Return the previously installed callback.

                        // ** get callbacks **

    static SystemTimeCallback currentSystemTimeCallback();
        // Return the currently installed 'SystemTimeCallback' function.

#if 0
    static LocalTimeOffsetCallback currentLocalTimeOffsetCallback();
        // Return the currently installed 'LocalTimeOffsetCallback' function.

    static GetLocalTimeOffsetCallbackSpec *
                                       currentGetLocalTimeOffsetCallbackSpec();
        // Return the currently installed 'GetLocalTimeOffsetCallback'
        // function and its opaque argument pointer.
#endif

    static LoadLocalTimeOffsetCallback currentLoadLocalTimeOffsetCallback();
        // Return the currently installed 'LoadLocalTimeOffsetCallback'
        // function.
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
bdet_TimeInterval bdetu_SystemTime::now()
{
    bdet_TimeInterval timeInterval;
    s_systime_callback_p(&timeInterval);
    return timeInterval;
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

                        // ** XXX callbacks **

inline
bdet_DatetimeInterval bdetu_SystemTime::localTimeOffset()
{
    BSLS_ASSERT_SAFE(s_loadLocalTimeOffsetCallback_p);

    int offsetInSeconds;
    int status = (*s_loadLocalTimeOffsetCallback_p)(&offsetInSeconds,
                                                    nowAsDatetimeUtc());
    BSLS_ASSERT_SAFE(0 == status);

    return bdet_DatetimeInterval(0, 0, 0, offsetInSeconds);
}

inline
void bdetu_SystemTime::loadCurrentTime(bdet_TimeInterval *result)
{
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(s_systime_callback_p);
    s_systime_callback_p(result);
}

inline
int bdetu_SystemTime::loadLocalTimeOffset(int                  *result,
                                          const bdet_Datetime&  utcDatetime)
{
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(s_loadLocalTimeOffsetCallback_p);
    return (*s_loadLocalTimeOffsetCallback_p)(result, utcDatetime);
}

#if 0
inline
bdet_DatetimeInterval bdetu_SystemTime::getLocalTimeOffset()
{
    BSLS_ASSERT_SAFE(s_getLocalTimeOffsetCallbackSpec_p);
    BSLS_ASSERT_SAFE(s_getLocalTimeOffsetCallbackSpec_p->d_callback);
    return (*s_getLocalTimeOffsetCallbackSpec_p->d_callback)(
                                    now(),
                                    s_getLocalTimeOffsetCallbackSpec_p->d_arg);
}
#endif

                        // ** set callbacks **

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

#if 0
inline
bdetu_SystemTime::LocalTimeOffsetCallback
bdetu_SystemTime::setLocalTimeOffsetCallback(LocalTimeOffsetCallback callback)
{
    bdetu_SystemTime::LocalTimeOffsetCallback
                               previousCallback = s_localtimeoffset_callback_p;
    s_localtimeoffset_callback_p = callback;
    return previousCallback;
}

inline
bdetu_SystemTime::GetLocalTimeOffsetCallbackSpec *
bdetu_SystemTime::setGetLocalTimeOffsetCallback(
                                  GetLocalTimeOffsetCallbackSpec *callbackSpec)
{
    GetLocalTimeOffsetCallbackSpec *previousCallbackSpec =
                                            s_getLocalTimeOffsetCallbackSpec_p;
    s_getLocalTimeOffsetCallbackSpec_p = callbackSpec;
    return previousCallbackSpec;
}
#endif

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

                        // ** get callbacks **

inline
bdetu_SystemTime::SystemTimeCallback
bdetu_SystemTime::currentSystemTimeCallback()
{
    return s_systime_callback_p;
}

#if 0
inline
bdetu_SystemTime::LocalTimeOffsetCallback
bdetu_SystemTime::currentLocalTimeOffsetCallback()
{
    return s_localtimeoffset_callback_p;
}

inline
bdetu_SystemTime::GetLocalTimeOffsetCallbackSpec *
bdetu_SystemTime::currentGetLocalTimeOffsetCallbackSpec()
{
    return s_getLocalTimeOffsetCallbackSpec_p;
}
#endif

inline
bdetu_SystemTime::LoadLocalTimeOffsetCallback
bdetu_SystemTime::currentLoadLocalTimeOffsetCallback()
{
    return s_loadLocalTimeOffsetCallback_p;
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
