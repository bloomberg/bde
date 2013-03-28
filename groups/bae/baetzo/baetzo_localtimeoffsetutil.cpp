// baetzo_localtimeoffsetutil.cpp                                     -*-C++-*-
#include <baetzo_localtimeoffsetutil.h>

#include <bdet_datetime.h>
#include <bslma_default.h>

namespace BloombergLP {

                        // ---------------------------------
                        // struct baetzo_LocalTimeOffsetUtil
                        // ---------------------------------

// PRIVATE CLASS METHODS
int baetzo_LocalTimeOffsetUtil::configureImp(const char           *timezone,
                                             const bdet_Datetime&  utcDatetime)
{
    BSLS_ASSERT(timezone);

    int retval = baetzo_TimeZoneUtil::loadLocalTimePeriodForUtc(
                                                       staticLocalTimePeriod(),
                                                       timezone,
                                                       utcDatetime);
    if (retval) {
        return retval;                                                // RETURN
    }
    s_timezone = timezone;
    ++s_updateCount;
    return retval;
}

// CLASS DATA
bcemt_QLock      baetzo_LocalTimeOffsetUtil::s_lock     =
                                                       BCEMT_QLOCK_INITIALIZER;
const char      *baetzo_LocalTimeOffsetUtil::s_timezone = 0;
bsls::AtomicInt  baetzo_LocalTimeOffsetUtil::s_updateCount(0);

// CLASS METHODS
void baetzo_LocalTimeOffsetUtil::loadLocalTimeOffset(
                                             int                  *result,
                                             const bdet_Datetime&  utcDatetime)
{
    BSLS_ASSERT(result);

    bcemt_QLockGuard qLockGuard(&s_lock);

    const baetzo_LocalTimePeriod *localTimePeriod = staticLocalTimePeriod();

    if (utcDatetime <  localTimePeriod->utcStartTime()
        || utcDatetime >= localTimePeriod->utcEndTime()) {

        int status = configureImp(s_timezone, utcDatetime);
        BSLS_ASSERT(0 == status);
    }

    *result = localTimePeriod->descriptor().utcOffsetInSeconds();
}

int  baetzo_LocalTimeOffsetUtil::configure()
{
    const char *timezone = getenv("TZ");
    if (!timezone) {
        return -1;                                                    // RETURN
    }

    bcemt_QLockGuard qLockGuard(&s_lock);
    return configureImp(timezone, bdetu_SystemTime::nowAsDatetimeUtc());
}

int baetzo_LocalTimeOffsetUtil::configure(const char *timezone)
{
    BSLS_ASSERT_SAFE(timezone);

    bcemt_QLockGuard qLockGuard(&s_lock);
    return configureImp(timezone, bdetu_SystemTime::nowAsDatetimeUtc());
}

int baetzo_LocalTimeOffsetUtil::configure(const char           *timezone,
                                          const bdet_Datetime&  utcDatetime)
{
    BSLS_ASSERT_SAFE(timezone);

    bcemt_QLockGuard qLockGuard(&s_lock);
    return configureImp(timezone, utcDatetime);
}

}  // close enterprise namespace
