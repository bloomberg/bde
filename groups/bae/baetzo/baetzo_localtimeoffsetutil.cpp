// baetzo_localtimeoffsetutil.cpp                                     -*-C++-*-
#include <baetzo_localtimeoffsetutil.h>

#include <bcemt_readlockguard.h>
#include <bcemt_writelockguard.h>
#include <bdet_datetime.h>
#include <bslma_default.h>

namespace BloombergLP {

                        // ---------------------------------
                        // struct baetzo_LocalTimeOffsetUtil
                        // ---------------------------------

// PRIVATE CLASS METHODS
inline
int baetzo_LocalTimeOffsetUtil::configureImp(const char           *timezone,
                                             const bdet_Datetime&  utcDatetime)
{
    BSLS_ASSERT(timezone);

    int retval = baetzo_TimeZoneUtil::loadLocalTimePeriodForUtc(
                                                      privateLocalTimePeriod(),
                                                      timezone,
                                                      utcDatetime);
    if (retval) {
        return retval;                                                // RETURN
    }
    privateTimezone()->assign(timezone);
    ++s_updateCount;
    return retval;
}

inline
baetzo_LocalTimePeriod *baetzo_LocalTimeOffsetUtil::privateLocalTimePeriod()
{
    static baetzo_LocalTimePeriod localTimePeriod(
                                            bslma::Default::globalAllocator());
    return &localTimePeriod;
}

inline
bcemt_RWMutex *baetzo_LocalTimeOffsetUtil::privateLock()
{
    static bcemt_RWMutex lock;
    return &lock;
}

bsl::string *baetzo_LocalTimeOffsetUtil::privateTimezone()
{
    static bsl::string timezone(bslma::Default::globalAllocator());
    return &timezone;
}

// CLASS DATA
bsls::AtomicInt baetzo_LocalTimeOffsetUtil::s_updateCount(0);

// CLASS METHODS

                        // *** local time offset methods ***

void baetzo_LocalTimeOffsetUtil::loadLocalTimeOffset(
                                             int                  *result,
                                             const bdet_Datetime&  utcDatetime)
{
    BSLS_ASSERT(result);

    bcemt_ReadLockGuard<bcemt_RWMutex> readLockGuard(privateLock());

    const baetzo_LocalTimePeriod *localTimePeriod = privateLocalTimePeriod();

    if (utcDatetime <  localTimePeriod->utcStartTime()
     || utcDatetime >= localTimePeriod->utcEndTime()) {

        readLockGuard.release()->unlock();

        {
            bcemt_WriteLockGuard<bcemt_RWMutex> writeLockGuard(privateLock());

            if (utcDatetime <  localTimePeriod->utcStartTime()
             || utcDatetime >= localTimePeriod->utcEndTime()) {

                int status = configureImp(privateTimezone()->c_str(),
                                          utcDatetime);
                BSLS_ASSERT(0 == status);
            }

            *result = localTimePeriod->descriptor().utcOffsetInSeconds();
        }
    } else {
        *result = localTimePeriod->descriptor().utcOffsetInSeconds();
    }
}

                        // *** configure methods ***

int  baetzo_LocalTimeOffsetUtil::configure()
{
    const char *timezone = getenv("TZ");
    if (!timezone) {
        return -1;                                                    // RETURN
    }

    bcemt_WriteLockGuard<bcemt_RWMutex> writeLockGuard(privateLock());
    return configureImp(timezone, bdetu_SystemTime::nowAsDatetimeUtc());
}

int baetzo_LocalTimeOffsetUtil::configure(const char *timezone)
{
    BSLS_ASSERT_SAFE(timezone);

    bcemt_WriteLockGuard<bcemt_RWMutex> writeLockGuard(privateLock());
    return configureImp(timezone, bdetu_SystemTime::nowAsDatetimeUtc());
}

int baetzo_LocalTimeOffsetUtil::configure(const char           *timezone,
                                          const bdet_Datetime&  utcDatetime)
{
    BSLS_ASSERT_SAFE(timezone);

    bcemt_WriteLockGuard<bcemt_RWMutex> writeLockGuard(privateLock());
    return configureImp(timezone, utcDatetime);
}

                        // *** accessor methods ***

void baetzo_LocalTimeOffsetUtil::loadLocalTimePeriod(
                                       baetzo_LocalTimePeriod *localTimePeriod)
{
    BSLS_ASSERT(localTimePeriod);

    bcemt_ReadLockGuard<bcemt_RWMutex> readLockGuard(privateLock());
    *localTimePeriod = *privateLocalTimePeriod();
}


void baetzo_LocalTimeOffsetUtil::loadTimezone(bsl::string *timezone)
{
    BSLS_ASSERT(timezone);

    bcemt_ReadLockGuard<bcemt_RWMutex> readLockGuard(privateLock());
    *timezone = *privateTimezone();
}

}  // close enterprise namespace
