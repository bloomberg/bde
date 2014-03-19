// baetzo_localtimeoffsetutil.h                                       -*-C++-*-
#ifndef INCLUDED_BAETZO_LOCALTIMEOFFSETUTIL
#define INCLUDED_BAETZO_LOCALTIMEOFFSETUTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide utilities for a 'bdetu_systemtime' local time callback.
//
//@CLASSES:
//  baetzo_LocalTimeOffsetUtil: utilities managing a local time callback
//
//@SEE_ALSO: bdetu::systemtime
//
//@AUTHOR: Steven Breitstein (sbreitstein)
//
//@DESCRIPTION: This component provides
// 'baetzo_LocalTimeOffsetUtil::loadLocalTimeOffset', a high performance
// 'bdetu_systemtime' local time offset callback function, which accesses the
// Zoneinfo database.  To achieve high performance, this function refers to a
// cached copy of local time period information (which includes the local time
// offset from UTC) that is populated by a call to one of the 'configure'
// methods.  The cache *must* be configured prior to the first call of
// 'loadLocalTimeOffset'.  That cached information is updated on receipt of a
// request with a datetime value outside of the range covered by the cached
// information.  As there are usually are only a few timezone transitions per
// year, the cache hit rate should be very high for typical applications.  The
// cached information might be invalidated by updates to the Zoneinfo database;
// however, those occur are also infrequent events.
//
// A successful return from one of the 'configure' methods is a prerequite
// to the use of most of the other functions provided here.  Most methods
// are thread-safe.  Refer to the function-level documentation for details.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using 'loadLocalTimeOffset' as the Local Time Offset Callback
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we must quickly generate time stamp values in local time (e.g., on
// records for a high frequency logger) and the default performance of the
// relevant methods of 'bdetu_SystemTime' is inadequate.  Further suppose that
// we must do so arbitrary time values and time zones.  Those requirements can
// be met by installing the 'loadLocalTimeOffset' method of
// 'baetzo_LocalTimeOffsetUtil' as the local time callback used by
// 'bdetu_SystemTime'.
//
// First, specify the time zone to be used by the callback and a UTC date time
// for the initial offset information in the cache.
//..
//  assert(0 == baetzo_LocalTimeOffsetUtil::updateCount());
//
//  int status = baetzo_LocalTimeOffsetUtil::configure("America/New_York",
//                                                      bdet_Datetime(2013,
//                                                                       2,
//                                                                      26));
//  assert(0 == status);
//  assert(1 == baetzo_LocalTimeOffsetUtil::updateCount());
//
//  bsl::string timezone;
//
//  baetzo_LocalTimeOffsetUtil::loadTimezone(&timezone);
//  assert(0 == strcmp("America/New_York", timezone.c_str()));
//..
// Notice that the value returned by the 'updateCount' method is increased by
// one after then time zone information has been set.
//
// Then, use the 'setLoadLocalTimeOffsetCallback' method to set the
// 'loadLocalTimeOffset' of 'baetzo_LocalTimeOffsetUtil' as the local time
// offset callback used in 'bdetu_SystemTime'.
//..
//  bdetu_SystemTime::LoadLocalTimeOffsetCallback previousCallback =
//                baetzo_LocalTimeOffsetUtil::setLoadLocalTimeOffsetCallback();
//
//  assert(&baetzo_LocalTimeOffsetUtil::loadLocalTimeOffset
//      == bdetu_SystemTime::currentLoadLocalTimeOffsetCallback());
//..
// Notice that previously installed callback was saved so we can restore it, if
// needed.
//
// Now, calls to 'bdetu_SystemTime' methods will use the method we installed.
// For example, we can check the time offset in New York for three dates of
// interest:
//..
//  int offsetInSeconds;
//
//  bdetu_SystemTime::loadLocalTimeOffset(&offsetInSeconds,
//                                        bdet_Datetime(2013,  2, 26));
//  assert(        0 == status);
//  assert(-5 * 3600 == offsetInSeconds);
//  assert(        1 == baetzo_LocalTimeOffsetUtil::updateCount());
//
//  baetzo_LocalTimeOffsetUtil::loadTimezone(&timezone);
//  assert(        0 == strcmp("America/New_York", timezone.c_str()));
//
//  bdetu_SystemTime::loadLocalTimeOffset(&offsetInSeconds,
//                                        bdet_Datetime(2013,  7,  4));
//  assert(-4 * 3600 == offsetInSeconds);
//  assert(        2 == baetzo_LocalTimeOffsetUtil::updateCount());
//  baetzo_LocalTimeOffsetUtil::loadTimezone(&timezone);
//  assert(        0 == strcmp("America/New_York", timezone.c_str()));
//
//  bdetu_SystemTime::loadLocalTimeOffset(&offsetInSeconds,
//                                        bdet_Datetime(2013, 12, 21));
//  assert(-5 * 3600 == offsetInSeconds);
//  assert(        3 == baetzo_LocalTimeOffsetUtil::updateCount());
//  baetzo_LocalTimeOffsetUtil::loadTimezone(&timezone);
//  assert(        0 == strcmp("America/New_York", timezone.c_str()));
//..
// Notice that the value returned by 'updateCount()' is unchanged by our first
// request, but incremented by the second and third request, which transitions
// into and then out of daylight saving time.  Also notice that the updates
// change the offset information but do not change the timezone.
//
// Finally, we restore the original local time callback.
//..
//  previousCallback = bdetu_SystemTime::setLoadLocalTimeOffsetCallback(
//                                                           previousCallback);
//  assert(previousCallback
//      == &baetzo_LocalTimeOffsetUtil::loadLocalTimeOffset);
//..

#ifndef INCLUDED_BAETZO_LOCALTIMEPERIOD
#include <baetzo_localtimeperiod.h>
#endif

#ifndef INCLUDED_BAETZO_TIMEZONEUTIL
#include <baetzo_timezoneutil.h>
#endif

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BDETU_SYSTEMTIME
#include <bdetu_systemtime.h>
#endif

#ifndef INCLUDED_BCEMT_RWMUTEX
#include <bcemt_rwmutex.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSLS_ATOMIC
#include <bsls_atomic.h>
#endif

namespace BloombergLP {

class bdet_Datetime;

                        // =================================
                        // struct baetzo_LocalTimeOffsetUtil
                        // =================================

struct baetzo_LocalTimeOffsetUtil {
    // This 'struct' provides a namespace for a 'bdetu_systemtime' local time
    // offset callback, and functions that manage the timezone information
    // reported by that callback.  All public methods are *thread-safe*.

    // CLASS DATA
  private:
    static bsls::AtomicInt s_updateCount;

    // PRIVATE CLASS METHODS
    static int configureImp(const char           *timezone,
                            const bdet_Datetime&  utcDatetime);
        // Set the local time period information used by the
        // 'loadLocalTimeOffset' method according to the specified 'timezone'
        // at the specified 'utcDatetime'.  Return 0 on success, and a non-zero
        // value otherwise.  This method is *not* thread-safe.

    static baetzo_LocalTimePeriod *privateLocalTimePeriod();
        // Return the address of the current local time period information.
        // This method is *not* thread-safe.

    static bcemt_RWMutex *privateLock();
        // Return the address of the lock controlling access to the local time
        // period information.  This method is *not* thread-safe.

    static bsl::string *privateTimezone();
        // Return the address of the time period information.  This method is
        // *not* thread-safe.

    // CLASS METHODS
  public:

                        // *** local time offset methods ***

    static void loadLocalTimeOffset(int                  *result,
                                    const bdet_Datetime&  utcDatetime);
        // Efficiently load to the specified 'result' the offset of the local
        // time from UTC for the specified 'utcDatetime'.  This function is
        // thread-safe.  The behavior is undefined unless the local time zone
        // has been previously established by a call to the 'configure' method.
        // This method *is* thread-safe.

    static bdetu_SystemTime::LoadLocalTimeOffsetCallback
                                              setLoadLocalTimeOffsetCallback();
        // Set 'loadLocalTimeOffset' as the local time offset callback of
        // 'bdetu_SystemTime'.  Return the previously installed callback.  This
        // method is *not* thread-safe.

                        // *** configure methods ***

    static int configure();
        // Set the local time period information used by the
        // 'loadLocalTimeOffset' method to that for the time zone in the 'TZ'
        // environment variable at the current UTC datetime.  Return 0 on
        // success, and a non-zero value otherwise.  This method is *not*
        // thread-safe.  The behavior is undefined if the environment changes
        // (e.g., a call to the 'putenv' POSIX function) during the invocation
        // of this method.

    static int configure(const char *timezone);
        // Set the local time period information used by the
        // 'loadLocalTimeOffset' method to that for specified 'timezone' at the
        // current UTC datetime.  Return 0 on success, and a non-zero value
        // otherwise.  This method is *not* thread-safe.

    static int configure(const char           *timezone,
                         const bdet_Datetime&  utcDatetime);
        // Set the local time period information used by the
        // 'loadLocalTimeOffset' method to that for the specified 'timezone' at
        // the specified 'utcDatetime'.  Return 0 on success, and a non-zero
        // value otherwise.  This method is *not* thread-safe.

                        // *** accessor methods ***

    static void loadLocalTimePeriod(baetzo_LocalTimePeriod *localTimePeriod);
        // Load to the specified 'localTimePeriod' the local time period
        // information currently used by the 'loadLocalTimeOffset' method.
        // That information is updated when 'loadLocalTimeOffset' is called
        // with a 'utcDatetime' outside the range
        // 'localTimePeriod().utcStartTime()' (inclusive)
        // 'localTimePeriod().utcEndTime()' (exclusive).  This method is *not*
        // thread-safe.  The behavior is undefined if this method is invoked
        // before the successful invocation of a 'configure' method.

    static void loadTimezone(bsl::string *timezone);
        // Load to the specified 'timezone' time zone identifier used to
        // determine the local time offset from UTC.  This method *is* not
        // thread-safe.  The behavior is undefined if this method is invoked
        // before the successful invocation of a 'configure' method.

    static int updateCount();
        // Return the number of successful updates of the local time period
        // information since the start of the process.  This count is
        // incremented on calls to any of the 'setTimeZone' methods and when
        // 'loadLocalTimePeriod' is called with a 'utcDatetime' outside the
        // range of the current local time period information.  This method
        // *is* thread-safe.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ---------------------------------
                        // struct baetzo_LocalTimeOffsetUtil
                        // ---------------------------------

// CLASS METHODS

                        // *** local time offset methods ***

inline
bdetu_SystemTime::LoadLocalTimeOffsetCallback
baetzo_LocalTimeOffsetUtil::setLoadLocalTimeOffsetCallback()
{
    return bdetu_SystemTime::setLoadLocalTimeOffsetCallback(
                             &baetzo_LocalTimeOffsetUtil::loadLocalTimeOffset);
}

                        // *** accessor methods ***

inline
int baetzo_LocalTimeOffsetUtil::updateCount()
{
    return s_updateCount;
}

}  // close namespace BloombergLP


#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2013
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
