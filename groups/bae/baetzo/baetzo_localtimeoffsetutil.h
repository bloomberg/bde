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
// methods.  That cached information is updated on receipt of a request with a
// datetime value outside of the range covered by the cached information.  As
// there are usually are only a few timezone transitions per year, the cache
// hit rate should be very high for typical applications.  The cached
// information might be invalidated by updates to the Zoneinfo database;
// however, those occur are also infrequent events.
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
//  assert(0 == strcmp("America/New_York",
//                     baetzo_LocalTimeOffsetUtil::timezone()));
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
//  status = bdetu_SystemTime::loadLocalTimeOffset(&offsetInSeconds,
//                                                 bdet_Datetime(2013,
//                                                                  2,
//                                                                 26));
//  assert(        0 == status);
//  assert(-5 * 3600 == offsetInSeconds);
//  assert(        1 == baetzo_LocalTimeOffsetUtil::updateCount());
//  assert(        0 == strcmp("America/New_York",
//                              baetzo_LocalTimeOffsetUtil::timezone()));
//
//  status = bdetu_SystemTime::loadLocalTimeOffset(&offsetInSeconds,
//                                                 bdet_Datetime(2013,
//                                                                  7,
//                                                                  4));
//  assert(        0 == status);
//  assert(-4 * 3600 == offsetInSeconds);
//  assert(        2 == baetzo_LocalTimeOffsetUtil::updateCount());
//  assert(        0 == strcmp("America/New_York",
//                              baetzo_LocalTimeOffsetUtil::timezone()));
//
//  status = bdetu_SystemTime::loadLocalTimeOffset(&offsetInSeconds,
//                                                 bdet_Datetime(2013,
//                                                                 12,
//                                                                 21));
//  assert(        0 == status);
//  assert(-5 * 3600 == offsetInSeconds);
//  assert(        3 == baetzo_LocalTimeOffsetUtil::updateCount());
//  assert(        0 == strcmp("America/New_York",
//                              baetzo_LocalTimeOffsetUtil::timezone()));
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

#ifndef INCLUDED_BCEMT_QLOCK
#include <bcemt_qlock.h>
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
    // This 'struct' provides a namespace for a 'bdeut_systemtime' local time
    // offset callback, and functions that manage the timezone information
    // reported by that callback.  All public methods are *thread-safe*.

    // CLASS DATA
  private:
    static bcemt_QLock      s_lock;
    static const char      *s_timezone;
    static bsls::AtomicInt  s_updateCount;

    // PRIVATE CLASS METHODS
    static int configureImp(const char           *timezone,
                            const bdet_Datetime&  utcDatetime);
        // Set the local time period information used by the
        // 'loadLocalTimeOffset' method according to the specified 'timezone'
        // at the specified 'utcDatetime'.  Return 0 on success, and a non-zero
        // value otherwise.  This method is *not* thread-safe.

    static baetzo_LocalTimePeriod *staticLocalTimePeriod();
        // Return the address of the current local time period information.

    // CLASS METHODS
  public:
    static void loadLocalTimeOffset(int                  *result,
                                    const bdet_Datetime&  utcDatetime);
        // Efficiently load to the specified 'result' the offset of the local
        // time from UTC for the specified 'utcDatetime'.  This function is
        // thread-safe.  The behavior is undefined unless the local time zone
        // has been previously established by a call to the 'configure' method.

    static const baetzo_LocalTimePeriod& localTimePeriod();
        // Return a reference providing non-modifiable access to the local time
        // period information currently used by the 'loadLocalTimeOffset'
        // method.  That information is updated when 'loadLocalTimeOffset' is
        // called with a 'utcDatetime' outside the range
        // 'localTimePeriod().utcStartTime()' (inclusive)
        // 'localTimePeriod().utcEndTime()' (exclusive).

    static bdetu_SystemTime::LoadLocalTimeOffsetCallback
                                              setLoadLocalTimeOffsetCallback();
        // Set 'loadLocalTimeOffset' as the local time offset callback of
        // 'bdetu_SystemTime'.  Return the previously installed callback.

    static int configure();
    static int configure(const char           *timezone);
    static int configure(const char           *timezone,
                         const bdet_Datetime&  utcDatetime);
        // Set the local time period information used by the
        // 'loadLocalTimeOffset' method to that for the time zone in the 'TZ'
        // environment variable at the current UTC datetime.  Return 0 on
        // success, and a non-zero value otherwise.  Optionally specify
        // 'timezone'.  Optionally specify the 'utcDatetime' of the local time
        // period information.  These methods are thread-safe.  The behavior is
        // undefined unless 'timezone' remains valid.  When 'timezone' is
        // obtained from the environment, it can be invalidated by a call to
        // the 'putenv' POSIX function.

    static const char *timezone();
        // Return the time zone identifier used to determine the local time
        // offset from UTC.

    static int updateCount();
        // Return the number of successful updates of the local time period
        // information since the start of the process.  This count is
        // incremented on calls to any of the 'setTimeZone' methods and when
        // 'loadLocalTimePeriod' is called with a 'utcDatetime' outside the
        // range of the current local time period information.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ---------------------------------
                        // struct baetzo_LocalTimeOffsetUtil
                        // ---------------------------------

// PRIVATE CLASS METHODS
inline
baetzo_LocalTimePeriod *baetzo_LocalTimeOffsetUtil::staticLocalTimePeriod()
{
    static baetzo_LocalTimePeriod localTimePeriod;
    return &localTimePeriod;
}

// CLASS METHODS
inline
const baetzo_LocalTimePeriod& baetzo_LocalTimeOffsetUtil::localTimePeriod()
{
    return *staticLocalTimePeriod();
}

inline
bdetu_SystemTime::LoadLocalTimeOffsetCallback
baetzo_LocalTimeOffsetUtil::setLoadLocalTimeOffsetCallback()
{
    return bdetu_SystemTime::setLoadLocalTimeOffsetCallback(
                             &baetzo_LocalTimeOffsetUtil::loadLocalTimeOffset);
}


inline
const char *baetzo_LocalTimeOffsetUtil::timezone()
{
    return s_timezone;
}

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
