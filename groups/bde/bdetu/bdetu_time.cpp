// bdetu_time.cpp                                                     -*-C++-*-
#include <bdetu_time.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdetu_time_cpp,"$Id$ $CSID$")

#include <bdet_datetime.h>
#include <bdet_timeinterval.h>

#include <bdetu_epoch.h>
#include <bdetu_systemtime.h>

#include <bslmf_assert.h>

namespace BloombergLP {

BSLMF_ASSERT(4 == sizeof(int));

                             // =================
                             // struct bdetu_Time
                             // =================

#ifndef BDE_OMIT_INTERNAL_DEPRECATED

// CLASS METHODS
bdet_Time bdetu_Time::currentTime()
{
    bdet_Time result;
    bdetu_Time::loadCurrentTime(&result);
    return result;
}

void bdetu_Time::loadCurrentTime(bdet_Time *result)
{
    bdet_TimeInterval currentTimeInterval;
    bdet_Datetime     currentDateTime;

    bdetu_SystemTime::loadCurrentTime(&currentTimeInterval);
    bdetu_Epoch::convertFromTimeInterval(&currentDateTime,
    currentTimeInterval);

    result->setHour(currentDateTime.hour());
    result->setMinute(currentDateTime.minute());
    result->setSecond(currentDateTime.second());
    result->setMillisecond(currentDateTime.millisecond());
}

#endif // BDE_OMIT_INTERNAL_DEPRECATED

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
