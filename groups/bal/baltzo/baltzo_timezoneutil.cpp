// baltzo_timezoneutil.cpp                                            -*-C++-*-
#include <baltzo_timezoneutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(baltzo_timezoneutil_cpp,"$Id$ $CSID$")

#include <baltzo_datafileloader.h>            // for testing
#include <baltzo_defaultzoneinfocache.h>
#include <baltzo_localtimeperiod.h>
#include <baltzo_testloader.h>                // for testing
#include <baltzo_timezoneutilimp.h>

#include <bdlt_datetime.h>
#include <bdlt_datetimeinterval.h>
#include <bdlt_datetimetz.h>
#include <bsls_timeinterval.h>
#include <bdlt_intervalconversionutil.h>

namespace BloombergLP {

                             // ------------------
                             // class TimeZoneUtil
                             // ------------------

// CLASS METHODS
int baltzo::TimeZoneUtil::convertUtcToLocalTime(
                                       LocalDatetime         *result,
                                       const char            *targetTimeZoneId,
                                       const bdlt::Datetime&  utcTime)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(targetTimeZoneId);

    bdlt::DatetimeTz resultTz;
    const int rc = convertUtcToLocalTime(&resultTz, targetTimeZoneId, utcTime);

    if (0 != rc) {
        return rc;                                                    // RETURN
    }

    result->setDatetimeTz(resultTz);
    result->setTimeZoneId(targetTimeZoneId);

    return 0;
}

int baltzo::TimeZoneUtil::convertLocalToLocalTime(
                                       LocalDatetime         *result,
                                       const char            *targetTimeZoneId,
                                       const bdlt::Datetime&  srcTime,
                                       const char            *srcTimeZoneId,
                                       DstPolicy::Enum        dstPolicy)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(targetTimeZoneId);
    BSLS_ASSERT(srcTimeZoneId);

    bdlt::DatetimeTz resultTz;
    const int rc = convertLocalToLocalTime(&resultTz,
                                           targetTimeZoneId,
                                           srcTime,
                                           srcTimeZoneId,
                                           dstPolicy);
    if (0 != rc) {
        return rc;                                                    // RETURN
    }

    result->setDatetimeTz(resultTz);
    result->setTimeZoneId(targetTimeZoneId);

    return 0;
}

int baltzo::TimeZoneUtil::convertLocalToLocalTime(
                                       bdlt::DatetimeTz      *result,
                                       const char            *targetTimeZoneId,
                                       const bdlt::Datetime&  srcTime,
                                       const char            *srcTimeZoneId,
                                       DstPolicy::Enum        dstPolicy)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(targetTimeZoneId);
    BSLS_ASSERT(srcTimeZoneId);

    bdlt::DatetimeTz localSrcTime;
    const int rc = initLocalTime(&localSrcTime,
                                 srcTime,
                                 srcTimeZoneId,
                                 dstPolicy);

    if (0 != rc) {
        return rc;                                                    // RETURN
    }

    return convertLocalToLocalTime(result, targetTimeZoneId, localSrcTime);
}

int baltzo::TimeZoneUtil::initLocalTime(
                                      LocalDatetime            *result,
                                      LocalTimeValidity::Enum  *resultValidity,
                                      const bdlt::Datetime&     localTime,
                                      const char               *timeZoneId,
                                      DstPolicy::Enum           dstPolicy)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(resultValidity);
    BSLS_ASSERT(timeZoneId);

    bdlt::DatetimeTz resultTz;
    LocalTimeValidity::Enum validityStatus;
    const int rc = initLocalTime(&resultTz,
                                 &validityStatus,
                                 localTime,
                                 timeZoneId,
                                 dstPolicy);

    if (0 != rc) {
        return rc;                                                    // RETURN
    }

    result->setDatetimeTz(resultTz);
    result->setTimeZoneId(timeZoneId);
    *resultValidity = validityStatus;

    return 0;
}

int baltzo::TimeZoneUtil::initLocalTime(LocalDatetime         *result,
                                        const bdlt::Datetime&  localTime,
                                        const char            *timeZoneId,
                                        DstPolicy::Enum        dstPolicy)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(timeZoneId);

    bdlt::DatetimeTz resultTz;
    const int rc = initLocalTime(&resultTz,
                                 localTime,
                                 timeZoneId,
                                 dstPolicy);

    if (0 != rc) {
        return rc;                                                    // RETURN
    }

    result->setDatetimeTz(resultTz);
    result->setTimeZoneId(timeZoneId);

    return 0;
}

int baltzo::TimeZoneUtil::convertLocalToUtc(bdlt::Datetime        *result,
                                            const bdlt::Datetime&  localTime,
                                            const char            *timeZoneId,
                                            DstPolicy::Enum        dstPolicy)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(timeZoneId);

    bdlt::DatetimeTz localTimeTz;
    const int rc = initLocalTime(&localTimeTz,
                                 localTime,
                                 timeZoneId,
                                 dstPolicy);
    if (0 != rc) {
        return rc;                                                    // RETURN
    }

    *result = localTimeTz.utcDatetime();

    return 0;
}

int baltzo::TimeZoneUtil::convertLocalToUtc(LocalDatetime         *result,
                                            const bdlt::Datetime&  localTime,
                                            const char            *timeZoneId,
                                            DstPolicy::Enum        dstPolicy)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(timeZoneId);

    bdlt::DatetimeTz localTimeTz;
    const int rc = initLocalTime(&localTimeTz,
                                 localTime,
                                 timeZoneId,
                                 dstPolicy);
    if (0 != rc) {
        return rc;                                                    // RETURN
    }

    result->setDatetimeTz(bdlt::DatetimeTz(localTimeTz.utcDatetime(), 0));
    result->setTimeZoneId("Etc/UTC");

    return 0;
}

int baltzo::TimeZoneUtil::loadLocalTimePeriodForUtc(
                                             LocalTimePeriod       *result,
                                             const char            *timeZoneId,
                                             const bdlt::Datetime&  utcTime)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(timeZoneId);

    return TimeZoneUtilImp::loadLocalTimePeriodForUtc(
                                  result,
                                  timeZoneId,
                                  utcTime,
                                  DefaultZoneinfoCache::defaultCache());
}

int baltzo::TimeZoneUtil::addInterval(LocalDatetime             *result,
                                      const LocalDatetime&       originalTime,
                                      const bsls::TimeInterval&  interval)
{
    BSLS_ASSERT(result);

    bdlt::DatetimeInterval datetimeInterval;
    datetimeInterval =
        bdlt::IntervalConversionUtil::convertToDatetimeInterval(interval);

    bdlt::Datetime utcTime = originalTime.datetimeTz().utcDatetime();
    utcTime += datetimeInterval;

    return convertUtcToLocalTime(result,
                                 originalTime.timeZoneId().c_str(),
                                 utcTime);
}



int baltzo::TimeZoneUtil::validateLocalTime(
                                           bool                    *result,
                                           const bdlt::DatetimeTz&  localTime,
                                           const char              *timeZoneId)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(timeZoneId);

    LocalTimePeriod timePeriod;
    int rc = loadLocalTimePeriodForUtc(&timePeriod,
                                       timeZoneId,
                                       localTime.utcDatetime());
    if (0 != rc) {
        return rc;                                                    // RETURN
    }

    const int offset          = timePeriod.descriptor().utcOffsetInSeconds();
    const int offsetInMinutes = offset / 60;

    *result = (localTime.offset() == offsetInMinutes);
    return 0;
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
