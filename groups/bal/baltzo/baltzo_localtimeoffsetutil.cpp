// baltzo_localtimeoffsetutil.cpp                                     -*-C++-*-
#include <baltzo_localtimeoffsetutil.h>

#include <baltzo_defaultzoneinfocache.h>  // for testing only
#include <baltzo_testloader.h>            // for testing only
#include <baltzo_zoneinfocache.h>         // for testing only
#include <baltzo_zoneinfoutil.h>          // for testing only

#include <bslmt_readlockguard.h>
#include <bslmt_writelockguard.h>

#include <bdlt_datetime.h>

#include <bslma_default.h>

#include <bsls_assert.h>
#include <bsls_timeinterval.h>

#include <bsl_cstdlib.h>  // getenv
#include <bsl_cstring.h>  // memcpy

namespace BloombergLP {
                         // --------------------------
                         // struct LocalTimeOffsetUtil
                         // --------------------------

// PRIVATE CLASS METHODS
inline
int baltzo::LocalTimeOffsetUtil::configureImp(
                                            const char            *timezone,
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
baltzo::LocalTimePeriod *baltzo::LocalTimeOffsetUtil::privateLocalTimePeriod()
{
    static LocalTimePeriod localTimePeriod(bslma::Default::globalAllocator());
    return &localTimePeriod;
}

inline
bslmt::RWMutex *baltzo::LocalTimeOffsetUtil::privateLock()
{
    static bslmt::RWMutex lock;
    return &lock;
}

bsl::string *baltzo::LocalTimeOffsetUtil::privateTimezone()
{
    static bsl::string timezone(bslma::Default::globalAllocator());
    return &timezone;
}

// CLASS DATA
bsls::AtomicInt baltzo::LocalTimeOffsetUtil::s_updateCount(0);

// CLASS METHODS

                        // *** local time offset methods ***

bsls::TimeInterval baltzo::LocalTimeOffsetUtil::localTimeOffset(
                                             const bdlt::Datetime& utcDatetime)
{
    bslmt::ReadLockGuard<bslmt::RWMutex> readLockGuard(privateLock());

    const LocalTimePeriod *localTimePeriod = privateLocalTimePeriod();

    int offsetInSeconds;

    if (utcDatetime <  localTimePeriod->utcStartTime()
     || utcDatetime >= localTimePeriod->utcEndTime()) {

        readLockGuard.release()->unlock();

        {
            bslmt::WriteLockGuard<bslmt::RWMutex> writeLockGuard(
                                                                privateLock());

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

int baltzo::LocalTimeOffsetUtil::configure()
{
    const char *timezone = bsl::getenv("TZ");
    if (!timezone) {
        return -1;                                                    // RETURN
    }

    bslmt::WriteLockGuard<bslmt::RWMutex> writeLockGuard(privateLock());
    return configureImp(timezone, bdlt::CurrentTime::utc());
}

int baltzo::LocalTimeOffsetUtil::configure(const char *timezone)
{
    BSLS_ASSERT_SAFE(timezone);

    bslmt::WriteLockGuard<bslmt::RWMutex> writeLockGuard(privateLock());
    return configureImp(timezone, bdlt::CurrentTime::utc());
}

int baltzo::LocalTimeOffsetUtil::configure(const char            *timezone,
                                           const bdlt::Datetime&  utcDatetime)
{
    BSLS_ASSERT_SAFE(timezone);

    bslmt::WriteLockGuard<bslmt::RWMutex> writeLockGuard(privateLock());
    return configureImp(timezone, utcDatetime);
}

                        // *** accessor methods ***

void baltzo::LocalTimeOffsetUtil::loadLocalTimePeriod(
                                              LocalTimePeriod *localTimePeriod)
{
    BSLS_ASSERT(localTimePeriod);

    bslmt::ReadLockGuard<bslmt::RWMutex> readLockGuard(privateLock());
    *localTimePeriod = *privateLocalTimePeriod();
}


void baltzo::LocalTimeOffsetUtil::loadTimezone(bsl::string *timezone)
{
    BSLS_ASSERT(timezone);

    bslmt::ReadLockGuard<bslmt::RWMutex> readLockGuard(privateLock());
    *timezone = *privateTimezone();
}

}  // close enterprise namespace

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
