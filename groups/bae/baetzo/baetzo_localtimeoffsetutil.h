// baetzo_localtimeoffsetutil.h                                       -*-C++-*-
#ifndef INCLUDED_BAETZO_LOCALTIMEOFFSETUTIL
#define INCLUDED_BAETZO_LOCALTIMEOFFSETUTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide utilities for a 'bdetu::systemtime' local time callback.
//
//@CLASSES:
//  baetzo_LocalTimeOffsetUtil: utilities managing a local time callback
//
//@SEE_ALSO: bdetu::systemtime
//
//@AUTHOR: Steven Breitstein (sbreitstein)
//
//@DESCRIPTION: TBD
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: TBD
/// - - - - - - -
// TBD

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
    // This 'struct' provides a namespace for utility functions that

    // CLASS DATA
  private:
    static baetzo_LocalTimePeriod  s_localTimePeriod;
    static bcemt_QLock             s_lock;
    static const char             *s_timezone;
    static bsls::AtomicInt         s_updateCount;

    // PRIVATE CLASS METHODS
    static int setTimezone_imp(const char           *timezone,
                               const bdet_Datetime&  utcDatetime);
       // Set the local time period information used by the
       // 'loadLocalTimeOffset' method according to the specified 'timezone' at
       // the specified 'utcDatetime'.  Return 0 on success, and a non-zero
       // value otherwise.  This method is *not* thread-safe.

    // CLASS METHODS
  public:
   static int loadLocalTimeOffset(int                  *result,
                                  const bdet_Datetime&  utcDatetime);
       // Efficiently load to the specified 'result' the offset of the local
       // time from UTC for the specified 'utcDatetime'.  Return 0 on success,
       // and a non-zero value otherwise.  This function is thread-safe.  The
       // behavior is undefined unless the local time zone has been previously
       // established by a call to the 'setTimezone' method.

   static const baetzo_LocalTimePeriod& localTimePeriod();
       // Return a reference providing unmodifiable access to the local time
       // period information currently used by the 'loadLocalTimeOffset'
       // method.  That information is updated when 'loadLocalTimeOffset' is
       // called with a 'utcDatetime' outside the range
       // 'localTimePeriod().utcStartTime()' (inclusive)
       // 'localTimePeriod().utcEndTime()' (exclusive).

   static bdetu_SystemTime::LoadLocalTimeOffsetCallback
                                              setLoadLocalTimeOffsetCallback();
       // Set 'loadLocalTimeOffset' as the local time offset callback of
       // 'bdetu_SystemTime'.  Return the previously installed callback.

   static int setTimezone();
   static int setTimezone(const char           *timezone);
   static int setTimezone(const char           *timezone,
                          const bdet_Datetime&  utcDatetime);
       // Set the local time period information used by the
       // 'loadLocalTimeOffset' method according to the time zone in the 'TZ'
       // environment variable using the current UTC datetime.  Return 0 on
       // success, and a non-zero value otherwise.  Optionally specify
       // 'timezone'.  Optionally specify 'utcDatetime'.  These methods are
       // thread-safe.  The behavior is undefined unless 'timezone' remains
       // valid.  When 'timezone' is obtained from the environment, it can be
       // invalidated by a call to the 'putenv' POSIX function.

   static const char *timezone();
       // Return the time zone identifier used to determine the local time
       // offset from UTC.

   static int updateCount();
       // Return the number updates of the local time period information
       // (whether or not the update is successful) since the start of the
       // process.  This count is incremented on calls to any of the
       // 'setTimeZone' methods and when 'loadLocalTimePeriod' is called with a
       // 'utcDatetime' outside the range of the current local time period
       // information.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ---------------------------------
                        // struct baetzo_LocalTimeOffsetUtil
                        // ---------------------------------

// CLASS METHODS
inline
const baetzo_LocalTimePeriod& baetzo_LocalTimeOffsetUtil::localTimePeriod()
{
    return s_localTimePeriod;
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
