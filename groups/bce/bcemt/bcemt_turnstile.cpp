// bcemt_turnstile.cpp   -*-C++-*-
#include <bcemt_turnstile.h>

#include <bcemt_barrier.h>        // testing only
#include <bcemt_mutex.h>          // testing only
#include <bcemt_threadutil.h>

#include <bdetu_systemtime.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcemt_turnstile_cpp,"$Id$ $CSID$")


namespace BloombergLP {

namespace {

enum { MICROSECS_PER_SECOND = 1000 * 1000 };

}  // close unnamed namespace

                           // ---------------------
                           // class bcemt_Turnstile
                           // ---------------------

// CREATORS
bcemt_Turnstile::bcemt_Turnstile(double                   rate,
                                 const bdet_TimeInterval& startTime)
{
    reset(rate, startTime);
}

// MANIPULATORS
bsls_Types::Int64 bcemt_Turnstile::waitTurn()
{
    enum { MIN_TIMER_RESOLUTION = 10 * 1000 };
        // Assume that minimum timer resolution applicable to "sleep" on all
        // supported platforms is 10 milliseconds.

    Int64 timestamp = d_timestamp;
    Int64 nextTurn  = d_nextTurn.add(d_interval) - d_interval;
    Int64 waitTime  = 0;

    if (nextTurn > timestamp) {
        Int64 nowUSec = bdetu_SystemTime::now().totalMicroseconds();
        d_timestamp = nowUSec;

        waitTime = nextTurn - nowUSec;

        if (waitTime >= MIN_TIMER_RESOLUTION) {
            int waitInt = (int) waitTime;
            if (waitInt == waitTime) {
                // This will stop working if 'waitTime > ~35 minutes'

                bcemt_ThreadUtil::microSleep(waitInt);
            }
            else {
                // This will work so long as 'waitTime < ~68 years'

                int waitSecs  = (int) (waitTime / MICROSECS_PER_SECOND);
                int waitUSecs = (int) (waitTime % MICROSECS_PER_SECOND);
                bcemt_ThreadUtil::microSleep(waitUSecs, waitSecs);
            }
        }
        else {
            waitTime = 0;
        }
    }

    return waitTime;
}

void bcemt_Turnstile::reset(double                   rate,
                            const bdet_TimeInterval& startTime)
{
    d_interval  = (Int64) (MICROSECS_PER_SECOND / rate);
    d_timestamp = bdetu_SystemTime::now().totalMicroseconds();
    d_nextTurn  = d_timestamp + startTime.totalMicroseconds();
}

// ACCESSORS
bsls_Types::Int64 bcemt_Turnstile::lagTime() const
{
    Int64 nowUSecs = bdetu_SystemTime::now().totalMicroseconds();
    d_timestamp = nowUSecs;

    Int64 delta = nowUSecs - d_nextTurn;
    return delta > 0 ? delta : 0;
}

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007, 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
