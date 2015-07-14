// baltzo_localtimeoffsetutil.cpp                                     -*-C++-*-
#include <baltzo_localtimeoffsetutil.h>

#include <baltzo_defaultzoneinfocache.h>  // for testing only
#include <baltzo_testloader.h>            // for testing only
#include <baltzo_zoneinfocache.h>         // for testing only
#include <baltzo_zoneinfoutil.h>          // for testing only

#include <bdlmtt_readlockguard.h>
#include <bdlmtt_writelockguard.h>
#include <bdlt_datetime.h>
#include <bslma_default.h>

#include <bsl_cstdlib.h>  // getenv
#include <bsl_cstring.h>  // memcpy

namespace BloombergLP {

namespace baltzo {
                        // ---------------------------------
                        // struct LocalTimeOffsetUtil
                        // ---------------------------------

// PRIVATE CLASS METHODS
inline
int LocalTimeOffsetUtil::configureImp(const char           *timezone,
                                             const bdlt::Datetime&  utcDatetime)
{
    BSLS_ASSERT(timezone);

    int retval = TimeZoneUtil::loadLocalTimePeriodForUtc(
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
LocalTimePeriod *LocalTimeOffsetUtil::privateLocalTimePeriod()
{
    static LocalTimePeriod localTimePeriod(
                                            bslma::Default::globalAllocator());
    return &localTimePeriod;
}

inline
bdlmtt::RWMutex *LocalTimeOffsetUtil::privateLock()
{
    static bdlmtt::RWMutex lock;
    return &lock;
}

bsl::string *LocalTimeOffsetUtil::privateTimezone()
{
    static bsl::string timezone(bslma::Default::globalAllocator());
    return &timezone;
}
}  // close package namespace

// CLASS DATA
bsls::AtomicInt baltzo::LocalTimeOffsetUtil::s_updateCount(0);

namespace baltzo {
// CLASS METHODS

                        // *** local time offset methods ***

bsls::TimeInterval LocalTimeOffsetUtil::localTimeOffset(
                                            const bdlt::Datetime&  utcDatetime)
{
    bdlmtt::ReadLockGuard<bdlmtt::RWMutex> readLockGuard(privateLock());

    const LocalTimePeriod *localTimePeriod = privateLocalTimePeriod();

    int offsetInSeconds;

    if (utcDatetime <  localTimePeriod->utcStartTime()
     || utcDatetime >= localTimePeriod->utcEndTime()) {

        readLockGuard.release()->unlock();

        {
            bdlmtt::WriteLockGuard<bdlmtt::RWMutex> writeLockGuard(privateLock());

            if (utcDatetime <  localTimePeriod->utcStartTime()
             || utcDatetime >= localTimePeriod->utcEndTime()) {

                int status = configureImp(privateTimezone()->c_str(),
                                          utcDatetime);
                BSLS_ASSERT(0 == status);
            }

            offsetInSeconds = 
                            localTimePeriod->descriptor().utcOffsetInSeconds();
        }
    } else {
        offsetInSeconds = localTimePeriod->descriptor().utcOffsetInSeconds();
    }
    return bsls::TimeInterval(offsetInSeconds);
}

                        // *** configure methods ***

int  LocalTimeOffsetUtil::configure()
{
    const char *timezone = bsl::getenv("TZ");
    if (!timezone) {
        return -1;                                                    // RETURN
    }

    bdlmtt::WriteLockGuard<bdlmtt::RWMutex> writeLockGuard(privateLock());
    return configureImp(timezone, bdlt::CurrentTime::utc());
}

int LocalTimeOffsetUtil::configure(const char *timezone)
{
    BSLS_ASSERT_SAFE(timezone);

    bdlmtt::WriteLockGuard<bdlmtt::RWMutex> writeLockGuard(privateLock());
    return configureImp(timezone, bdlt::CurrentTime::utc());
}

int LocalTimeOffsetUtil::configure(const char           *timezone,
                                          const bdlt::Datetime&  utcDatetime)
{
    BSLS_ASSERT_SAFE(timezone);

    bdlmtt::WriteLockGuard<bdlmtt::RWMutex> writeLockGuard(privateLock());
    return configureImp(timezone, utcDatetime);
}

                        // *** accessor methods ***

void LocalTimeOffsetUtil::loadLocalTimePeriod(
                                       LocalTimePeriod *localTimePeriod)
{
    BSLS_ASSERT(localTimePeriod);

    bdlmtt::ReadLockGuard<bdlmtt::RWMutex> readLockGuard(privateLock());
    *localTimePeriod = *privateLocalTimePeriod();
}


void LocalTimeOffsetUtil::loadTimezone(bsl::string *timezone)
{
    BSLS_ASSERT(timezone);

    bdlmtt::ReadLockGuard<bdlmtt::RWMutex> readLockGuard(privateLock());
    *timezone = *privateTimezone();
}
}  // close package namespace

}  // close enterprise namespace

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2013
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
