// baetzo_localtimeoffsetutil.cpp                                     -*-C++-*-
#include <baetzo_localtimeoffsetutil.h>

#include <bdet_datetime.h>
#include <bslma_default.h>

namespace BloombergLP {

                        // ---------------------------------
                        // struct baetzo_LocaltimeOffsetUtil
                        // ---------------------------------

// CLASS DATA
baetzo_LocalTimePeriod  baetzo_LocaltimeOffsetUtil::s_localTimePeriod(
                                            bslma::Default::globalAllocator());
bcemt_QLock             baetzo_LocaltimeOffsetUtil::s_lock =
                                                       BCEMT_QLOCK_INITIALIZER;
const char             *baetzo_LocaltimeOffsetUtil::s_timezone = 0;

// CLASS METHODS
int baetzo_LocaltimeOffsetUtil::loadLocalTimeOffset(
                                             int                  *result,
                                             const bdet_Datetime&  utcDatetime)
{
    BSLS_ASSERT(result);

    int status = 0;

    {
       bcemt_QLockGuard qLockGuard(&s_lock);
    
       if (utcDatetime <  s_localTimePeriod.utcStartTime()
        || utcDatetime >= s_localTimePeriod.utcEndTime()) {

           status = setTimezone(s_timezone, utcDatetime);
       }
    }

    if (status) {
        return status;                                                // RETURN
    }

    *result = s_localTimePeriod.descriptor().utcOffsetInSeconds();
    return 0;
}

int  baetzo_LocaltimeOffsetUtil::setTimezone()
{
    const char *tz       = getenv("TZ");
    const char *timezone = tz ? tz : "Etc/GMT";
    return setTimezone(timezone);
}

int baetzo_LocaltimeOffsetUtil::setTimezone(const char *timezone)
{
    BSLS_ASSERT_SAFE(timezone);
    return setTimezone(timezone, bdetu_SystemTime::nowAsDatetimeUtc());
}

int baetzo_LocaltimeOffsetUtil::setTimezone(const char           *timezone,
                                            const bdet_Datetime&  utcDatetime)
{
    BSLS_ASSERT_SAFE(timezone);
    s_timezone = timezone;
    return baetzo_TimeZoneUtil::loadLocalTimePeriodForUtc(&s_localTimePeriod,
                                                          s_timezone,
                                                          utcDatetime);
}

}  // close enterprise namespace
