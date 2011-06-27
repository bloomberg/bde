// baetzo_timezoneutil.cpp                                            -*-C++-*-
#include <baetzo_timezoneutil.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baetzo_timezoneutil_cpp,"$Id$ $CSID$")

#include <baetzo_datafileloader.h>            // for testing
#include <baetzo_defaultzoneinfocache.h>
#include <baetzo_localtimeperiod.h>
#include <baetzo_testloader.h>                // for testing
#include <baetzo_timezoneutilimp.h>

#include <bdet_datetime.h>
#include <bdet_datetimeinterval.h>
#include <bdet_datetimetz.h>

namespace BloombergLP {

                        // -------------------------
                        // class baetzo_TimeZoneUtil
                        // -------------------------

// CLASS METHODS
int baetzo_TimeZoneUtil::convertUtcToLocalTime(
                                        baet_LocalDatetime   *result,
                                        const char           *targetTimeZoneId,
                                        const bdet_Datetime&  utcTime)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(targetTimeZoneId);

    bdet_DatetimeTz resultTz;
    const int rc = convertUtcToLocalTime(&resultTz, targetTimeZoneId, utcTime);

    if (0 != rc) {
        return rc;                                                    // RETURN
    }

    result->setDatetimeTz(resultTz);
    result->setTimeZoneId(targetTimeZoneId);

    return 0;
}

int baetzo_TimeZoneUtil::convertLocalToLocalTime(
                                     baet_LocalDatetime     *result,
                                     const char             *targetTimeZoneId,
                                     const bdet_Datetime&    srcTime,
                                     const char             *srcTimeZoneId,
                                     baetzo_DstPolicy::Enum  dstPolicy)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(targetTimeZoneId);
    BSLS_ASSERT(srcTimeZoneId);

    bdet_DatetimeTz resultTz;
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

int baetzo_TimeZoneUtil::convertLocalToLocalTime(
                                     bdet_DatetimeTz        *result,
                                     const char             *targetTimeZoneId,
                                     const bdet_Datetime&    srcTime,
                                     const char             *srcTimeZoneId,
                                     baetzo_DstPolicy::Enum  dstPolicy)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(targetTimeZoneId);
    BSLS_ASSERT(srcTimeZoneId);

    bdet_DatetimeTz localSrcTime;
    const int rc = initLocalTime(&localSrcTime,
                                 srcTime,
                                 srcTimeZoneId,
                                 dstPolicy);

    if (0 != rc) {
        return rc;                                                    // RETURN
    }

    return convertLocalToLocalTime(result, targetTimeZoneId, localSrcTime);
}

int baetzo_TimeZoneUtil::initLocalTime(
                                baet_LocalDatetime             *result,
                                baetzo_LocalTimeValidity::Enum *resultValidity,
                                const bdet_Datetime&            localTime,
                                const char                     *timeZoneId,
                                baetzo_DstPolicy::Enum          dstPolicy)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(resultValidity);
    BSLS_ASSERT(timeZoneId);

    bdet_DatetimeTz resultTz;
    baetzo_LocalTimeValidity::Enum validityStatus;
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

int baetzo_TimeZoneUtil::initLocalTime(baet_LocalDatetime     *result,
                                       const bdet_Datetime&    localTime,
                                       const char             *timeZoneId,
                                       baetzo_DstPolicy::Enum  dstPolicy)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(timeZoneId);

    bdet_DatetimeTz resultTz;
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

int baetzo_TimeZoneUtil::convertLocalToUtc(bdet_Datetime          *result,
                                           const bdet_Datetime&    localTime,
                                           const char             *timeZoneId,
                                           baetzo_DstPolicy::Enum  dstPolicy)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(timeZoneId);

    bdet_DatetimeTz localTimeTz;
    const int rc = initLocalTime(&localTimeTz,
                                 localTime,
                                 timeZoneId,
                                 dstPolicy);
    if (0 != rc) {
        return rc;                                                    // RETURN
    }

    *result = localTimeTz.gmtDatetime();

    return 0;
}

int baetzo_TimeZoneUtil::convertLocalToUtc(baet_LocalDatetime     *result,
                                           const bdet_Datetime&    localTime,
                                           const char             *timeZoneId,
                                           baetzo_DstPolicy::Enum  dstPolicy)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(timeZoneId);

    bdet_DatetimeTz localTimeTz;
    const int rc = initLocalTime(&localTimeTz,
                                 localTime,
                                 timeZoneId,
                                 dstPolicy);
    if (0 != rc) {
        return rc;                                                    // RETURN
    }

    result->setDatetimeTz(bdet_DatetimeTz(localTimeTz.gmtDatetime(), 0));
    result->setTimeZoneId("Etc/UTC");

    return 0;
}

int baetzo_TimeZoneUtil::loadLocalTimePeriodForUtc(
                                            baetzo_LocalTimePeriod *result,
                                            const char             *timeZoneId,
                                            const bdet_Datetime&    utcTime)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(timeZoneId);

    return baetzo_TimeZoneUtilImp::loadLocalTimePeriodForUtc(
                                  result,
                                  timeZoneId,
                                  utcTime,
                                  baetzo_DefaultZoneinfoCache::defaultCache());
}

int baetzo_TimeZoneUtil::addInterval(
                                    baet_LocalDatetime           *result,
                                    const baet_LocalDatetime&     originalTime,
                                    const bdet_DatetimeInterval&  interval)
{
    BSLS_ASSERT(result);

    bdet_Datetime utcTime = originalTime.datetimeTz().gmtDatetime();
    utcTime += interval;

    return convertUtcToLocalTime(result,
                                 originalTime.timeZoneId().c_str(),
                                 utcTime);
}


int baetzo_TimeZoneUtil::validateLocalTime(
                                            bool                   *result,
                                            const bdet_DatetimeTz&  localTime,
                                            const char             *timeZoneId)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(timeZoneId);

    baetzo_LocalTimePeriod timePeriod;
    int rc = loadLocalTimePeriodForUtc(&timePeriod,
                                       timeZoneId,
                                       localTime.gmtDatetime());
    if (0 != rc) {
        return rc;                                                    // RETURN
    }

    const int offset          = timePeriod.descriptor().utcOffsetInSeconds();
    const int offsetInMinutes = offset / 60;

    *result = (localTime.offset() == offsetInMinutes);
    return 0;
                                                                              }

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
