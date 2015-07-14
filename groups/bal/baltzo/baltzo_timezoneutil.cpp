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

namespace baltzo {
                        // -------------------------
                        // class TimeZoneUtil
                        // -------------------------

// CLASS METHODS
int TimeZoneUtil::convertUtcToLocalTime(
                                        baltzo::LocalDatetime   *result,
                                        const char           *targetTimeZoneId,
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

int TimeZoneUtil::convertLocalToLocalTime(
                                     baltzo::LocalDatetime     *result,
                                     const char             *targetTimeZoneId,
                                     const bdlt::Datetime&    srcTime,
                                     const char             *srcTimeZoneId,
                                     DstPolicy::Enum  dstPolicy)
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

int TimeZoneUtil::convertLocalToLocalTime(
                                     bdlt::DatetimeTz        *result,
                                     const char             *targetTimeZoneId,
                                     const bdlt::Datetime&    srcTime,
                                     const char             *srcTimeZoneId,
                                     DstPolicy::Enum  dstPolicy)
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

int TimeZoneUtil::initLocalTime(
                                baltzo::LocalDatetime             *result,
                                LocalTimeValidity::Enum *resultValidity,
                                const bdlt::Datetime&            localTime,
                                const char                     *timeZoneId,
                                DstPolicy::Enum          dstPolicy)
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

int TimeZoneUtil::initLocalTime(baltzo::LocalDatetime     *result,
                                       const bdlt::Datetime&    localTime,
                                       const char             *timeZoneId,
                                       DstPolicy::Enum  dstPolicy)
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

int TimeZoneUtil::convertLocalToUtc(bdlt::Datetime          *result,
                                           const bdlt::Datetime&    localTime,
                                           const char             *timeZoneId,
                                           DstPolicy::Enum  dstPolicy)
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

int TimeZoneUtil::convertLocalToUtc(baltzo::LocalDatetime     *result,
                                           const bdlt::Datetime&    localTime,
                                           const char             *timeZoneId,
                                           DstPolicy::Enum  dstPolicy)
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

int TimeZoneUtil::loadLocalTimePeriodForUtc(
                                            LocalTimePeriod *result,
                                            const char             *timeZoneId,
                                            const bdlt::Datetime&    utcTime)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(timeZoneId);

    return TimeZoneUtilImp::loadLocalTimePeriodForUtc(
                                  result,
                                  timeZoneId,
                                  utcTime,
                                  DefaultZoneinfoCache::defaultCache());
}

int TimeZoneUtil::addInterval(
                                    baltzo::LocalDatetime        *result,
                                    const baltzo::LocalDatetime&  originalTime,
                                    const bsls::TimeInterval&   interval)
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



int TimeZoneUtil::validateLocalTime(
                                            bool                   *result,
                                            const bdlt::DatetimeTz&  localTime,
                                            const char             *timeZoneId)
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
}  // close package namespace

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
