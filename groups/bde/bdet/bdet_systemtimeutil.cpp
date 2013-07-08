// bdet_systemtimeutil.cpp                                            -*-C++-*-
#include <bdet_systemtimeutil.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdet_systemtimeutil_cpp,"$Id$ $CSID$")

#include <bsls_performancehint.h>
#include <bsls_platform.h>

#ifdef BSLS_PLATFORM_OS_WINDOWS
#include <windows.h>
#else
// Before we include the Unix-specific header '<sys/time.h>' we include the
// standard header '<time.h>' to ensure that things like 'time_t' end up in
// both the 'std' namespace and the global namespace.  (Critical for Sunpro.)
#include <bsl_c_time.h>
#include <bsl_c_sys_time.h>
#ifdef BSLS_PLATFORM_OS_AIX
#include <sys/systemcfg.h>
#endif
#endif

#include <limits.h>  // for 'INT_MAX'

namespace BloombergLP {

                            // -------------------------
                            // class bdet_SystemTimeUtil
                            // -------------------------

// DATA
bdet_SystemTimeUtil::SystemTimeCallback
bdet_SystemTimeUtil::s_systime_callback_p =
                                    bdet_SystemTimeUtil::loadSystemTimeDefault;

bdet_SystemTimeUtil::LoadLocalTimeOffsetCallback
bdet_SystemTimeUtil::s_loadLocalTimeOffsetCallback_p =
                               bdet_SystemTimeUtil::loadLocalTimeOffsetDefault;

// CLASS METHODS

                        // ** now methods **

bdet_Datetime bdet_SystemTimeUtil::nowAsDatetimeLocal()
{
    int           offsetInSeconds;
    bdet_Datetime now = nowAsDatetimeUtc();

    (*s_loadLocalTimeOffsetCallback_p)(&offsetInSeconds, now);

    now.addSeconds(offsetInSeconds);

    return now;
}

                       // ** default callbacks **

void bdet_SystemTimeUtil::loadLocalTimeOffsetDefault(
                                             int                  *result,
                                             const bdet_Datetime&  utcDatetime)
{
    BSLS_ASSERT(result);

    bsl::time_t currentTime;
    int         status = bdet_EpochUtil::convertToTimeT(&currentTime,
                                                        utcDatetime);
    BSLS_ASSERT(0 == status);

    struct tm localTm;
    struct tm   gmtTm;
#if defined(BSLS_PLATFORM_OS_WINDOWS) || ! defined(BDE_BUILD_TARGET_MT)
    localTm = *localtime(&currentTime);
      gmtTm =    *gmtime(&currentTime);
#else
    localtime_r(&currentTime, &localTm);
       gmtime_r(&currentTime,   &gmtTm);
#endif

    bdet_Datetime localDatetime;
    bdet_Datetime   gmtDatetime;
    bdet_DatetimeUtil::convertFromTm(&localDatetime, localTm);
    bdet_DatetimeUtil::convertFromTm(  &gmtDatetime,   gmtTm);

    bdet_DatetimeInterval offset = localDatetime - gmtDatetime;

    BSLS_ASSERT(offset.totalSeconds() < INT_MAX);
    *result = offset.totalSeconds();
}

void bdet_SystemTimeUtil::loadSystemTimeDefault(bdet_TimeInterval *result)
{
    BSLS_ASSERT(result);

#ifdef BSLS_PLATFORM_OS_WINDOWS
    bsls::Types::Uint64 t;
    GetSystemTimeAsFileTime((FILETIME*)&t);
    t -= 116444736000000000ll; // windows epoch -> unix epoch
    result->setInterval(t / 10000000, (t % 10000000) * 100);
#elif BSLS_PLATFORM_OS_LINUX
    struct timespec tp;
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                         clock_gettime(CLOCK_REALTIME, &tp))) {
          result->setInterval(0, 0);
          return;
    }
    result->setInterval(tp.tv_sec, tp.tv_nsec);
#elif BSLS_PLATFORM_OS_AIX
    timebasestruct_t tb;
    read_real_time(&tb, TIMEBASE_SZ);
    time_base_to_time(&tb, TIMEBASE_SZ);
    result->setInterval(tb.tb_high, tb.tb_low);
#else
    timeval time_val;
    if (gettimeofday(&time_val, NULL) != 0) {
         result->setInterval(0, 0);
    }
    else {
        result->setInterval(time_val.tv_sec, time_val.tv_usec * 1000);
    }
#endif
}

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
