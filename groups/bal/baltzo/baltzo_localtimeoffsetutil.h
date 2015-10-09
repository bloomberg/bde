// baltzo_localtimeoffsetutil.h                                       -*-C++-*-
#ifndef INCLUDED_BALTZO_LOCALTIMEOFFSETUTIL
#define INCLUDED_BALTZO_LOCALTIMEOFFSETUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide utilities for a 'bdetu_systemtime' local time callback.
//
//@CLASSES:
//  baltzo::LocalTimeOffsetUtil: utilities managing a local time callback
//
//@SEE_ALSO: bdetu::systemtime
//
//@DESCRIPTION: This component, 'baltzo::LocalTimeOffsetUtil', provides
// 'baltzo::LocalTimeOffsetUtil::loadLocalTimeOffset', a high performance
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
// A successful return from one of the 'configure' methods is a prerequisite to
// the use of most of the other functions provided here.  Most methods are
// thread-safe.  Refer to the function-level documentation for details.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using 'loadLocalTimeOffset' as the Local Time Offset Callback
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we must quickly generate time stamp values in local time (e.g., on
// records for a high frequency logger) and the default performance of the
// relevant methods of 'bdlt::CurrentTime' is inadequate.  Further suppose that
// we must do so arbitrary time values and time zones.  Those requirements can
// be met by installing the 'loadLocalTimeOffset' method of
// 'baltzo::LocalTimeOffsetUtil' as the local time callback used by
// 'bdlt::CurrentTime'.
//
// First, specify the time zone to be used by the callback and a UTC date time
// for the initial offset information in the cache.
//..
//  assert(0 == baltzo::LocalTimeOffsetUtil::updateCount());
//
//  int status = baltzo::LocalTimeOffsetUtil::configure("America/New_York",
//                                                      bdlt::Datetime(2013,
//                                                                       2,
//                                                                      26));
//  assert(0 == status);
//  assert(1 == baltzo::LocalTimeOffsetUtil::updateCount());
//
//  bsl::string timezone;
//
//  baltzo::LocalTimeOffsetUtil::loadTimezone(&timezone);
//  assert(0 == strcmp("America/New_York", timezone.c_str()));
//..
// Notice that the value returned by the 'updateCount' method is increased by
// one after then time zone information has been set.
//
// Then, use the 'setLoadLocalTimeOffsetCallback' method to set the
// 'loadLocalTimeOffset' of 'baltzo::LocalTimeOffsetUtil' as the local time
// offset callback used in 'bdlt::CurrentTime'.
//..
//  bdlt::LocalTimeOffset::LocalTimeOffsetCallback previousCallback =
//               baltzo::LocalTimeOffsetUtil::setLoadLocalTimeOffsetCallback();
//
//  assert(&baltzo::LocalTimeOffsetUtil::localTimeOffset
//      == bdlt::CurrentTime::localTimeOffsetCallback());
//..
// Notice that previously installed callback was saved so we can restore it, if
// needed.
//
// Now, calls to 'bdlt::CurrentTime' methods will use the method we installed.
// For example, we can check the time offset in New York for three dates of
// interest:
//..
//  int offsetInSeconds =
//      bdlt::LocalTimeOffset::localTimeOffset(bdlt::Datetime(2013, 2, 26))
//                                                             .totalSeconds();
//  assert(        0 == status);
//  assert(-5 * 3600 == offsetInSeconds);
//  assert(        1 == baltzo::LocalTimeOffsetUtil::updateCount());
//
//  baltzo::LocalTimeOffsetUtil::loadTimezone(&timezone);
//  assert(        0 == strcmp("America/New_York", timezone.c_str()));
//
//  offsetInSeconds =
//      bdlt::LocalTimeOffset::localTimeOffset(bdlt::Datetime(2013, 7, 4))
//                                                             .totalSeconds();
//  assert(-4 * 3600 == offsetInSeconds);
//  assert(        2 == baltzo::LocalTimeOffsetUtil::updateCount());
//  baltzo::LocalTimeOffsetUtil::loadTimezone(&timezone);
//  assert(        0 == strcmp("America/New_York", timezone.c_str()));
//
//  offsetInSeconds =
//      bdlt::LocalTimeOffset::localTimeOffset(bdlt::Datetime(2013, 12, 21))
//                                                             .totalSeconds();
//  assert(-5 * 3600 == offsetInSeconds);
//  assert(        3 == baltzo::LocalTimeOffsetUtil::updateCount());
//  baltzo::LocalTimeOffsetUtil::loadTimezone(&timezone);
//  assert(        0 == strcmp("America/New_York", timezone.c_str()));
//..
// Notice that the value returned by 'updateCount()' is unchanged by our first
// request, but incremented by the second and third request, which transitions
// into and then out of daylight saving time.  Also notice that the updates
// change the offset information but do not change the timezone.
//
// Finally, we restore the original local time callback.
//..
//  previousCallback = bdlt::LocalTimeOffset::setLocalTimeOffsetCallback(
//                                                           previousCallback);
//  ASSERT(previousCallback == &baltzo::LocalTimeOffsetUtil::localTimeOffset);
//..

#ifndef INCLUDED_BALTZO_LOCALTIMEPERIOD
#include <baltzo_localtimeperiod.h>
#endif

#ifndef INCLUDED_BALTZO_TIMEZONEUTIL
#include <baltzo_timezoneutil.h>
#endif

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BDLT_CURRENTTIME
#include <bdlt_currenttime.h>
#endif

#ifndef INCLUDED_BDLT_DATETIME
#include <bdlt_datetime.h>
#endif

#ifndef INCLUDED_BDLT_LOCALTIMEOFFSET
#include <bdlt_localtimeoffset.h>
#endif

#ifndef INCLUDED_BSLMT_RWMUTEX
#include <bslmt_rwmutex.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSLS_ATOMIC
#include <bsls_atomic.h>
#endif

namespace BloombergLP {
namespace baltzo {
                         // ==========================
                         // struct LocalTimeOffsetUtil
                         // ==========================

struct LocalTimeOffsetUtil {
    // This 'struct' provides a namespace for a 'bdetu_systemtime' local time
    // offset callback, and functions that manage the timezone information
    // reported by that callback.  All public methods are *thread-safe*.

    // CLASS DATA
  private:
    static bsls::AtomicInt s_updateCount;

    // PRIVATE CLASS METHODS
    static int configureImp(const char            *timezone,
                            const bdlt::Datetime&  utcDatetime);
        // Set the local time period information used by the
        // 'loadLocalTimeOffset' method according to the specified 'timezone'
        // at the specified 'utcDatetime'.  Return 0 on success, and a non-zero
        // value otherwise.  This method is *not* thread-safe.

    static LocalTimePeriod *privateLocalTimePeriod();
        // Return the address of the current local time period information.
        // This method is *not* thread-safe.

    static bslmt::RWMutex *privateLock();
        // Return the address of the lock controlling access to the local time
        // period information.  This method is *not* thread-safe.

    static bsl::string *privateTimezone();
        // Return the address of the time period information.  This method is
        // *not* thread-safe.

    // CLASS METHODS
  public:

                        // *** local time offset methods ***

    static bsls::TimeInterval localTimeOffset(
                                            const bdlt::Datetime& utcDatetime);
        // Return the offset of the local time from UTC for the specified
        // 'utcDatetime'.  This function is thread-safe.  The behavior is
        // undefined unless the local time zone has been previously established
        // by a call to the 'configure' method.  This method *is* thread-safe.

    static bdlt::LocalTimeOffset::LocalTimeOffsetCallback
                                              setLoadLocalTimeOffsetCallback();
        // Set 'loadLocalTimeOffset' as the local time offset callback of
        // 'bdlt::CurrentTime'.  Return the previously installed callback.
        // This method is *not* thread-safe.

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

    static int configure(const char            *timezone,
                         const bdlt::Datetime&  utcDatetime);
        // Set the local time period information used by the
        // 'loadLocalTimeOffset' method to that for the specified 'timezone' at
        // the specified 'utcDatetime'.  Return 0 on success, and a non-zero
        // value otherwise.  This method is *not* thread-safe.

                        // *** accessor methods ***

    static void loadLocalTimePeriod(LocalTimePeriod *localTimePeriod);
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

}  // close package namespace

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                         // --------------------------
                         // struct LocalTimeOffsetUtil
                         // --------------------------

// CLASS METHODS

                        // *** local time offset methods ***

inline
bdlt::LocalTimeOffset::LocalTimeOffsetCallback
baltzo::LocalTimeOffsetUtil::setLoadLocalTimeOffsetCallback()
{
    return bdlt::LocalTimeOffset::setLocalTimeOffsetCallback(
                                        &LocalTimeOffsetUtil::localTimeOffset);
}

                        // *** accessor methods ***

inline
int baltzo::LocalTimeOffsetUtil::updateCount()
{
    return s_updateCount;
}

}  // close enterprise namespace


#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
