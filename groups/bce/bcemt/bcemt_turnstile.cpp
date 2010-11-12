// bcemt_turnstile.cpp   -*-C++-*-
#include <bcemt_turnstile.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcemt_turnstile_cpp,"$Id$ $CSID$")


#include <bcemt_barrier.h>    // for testing only

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
    d_interval = (bsls_PlatformUtil::Int64) (MICROSECS_PER_SECOND / rate);

    bdet_TimeInterval now = bdetu_SystemTime::now();
    d_timestamp = now.totalMicroseconds();
    d_nextTurn  = d_timestamp + startTime.totalMicroseconds();
}

// MANIPULATORS

bsls_PlatformUtil::Int64 bcemt_Turnstile::waitTurn()
{
    typedef bsls_PlatformUtil::Int64 Int64;

    enum { MAX_TIMER_RESOLUTION = 10 * 1000 };
        // Assume that maximum timer resolution applicable to "sleep" on all
        // supported platforms is 10 milliseconds.

    Int64 timestamp = d_timestamp;
    Int64 nextTurn  = d_nextTurn.swap(d_nextTurn + d_interval);
    Int64 waitTime  = 0;

    if (nextTurn > timestamp) {
        bdet_TimeInterval now = bdetu_SystemTime::now();
        d_timestamp.swap(now.totalMicroseconds());

        waitTime = nextTurn - now.totalMicroseconds();

        if (waitTime >= MAX_TIMER_RESOLUTION) {
            bcemt_ThreadUtil::microSleep(waitTime);
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
    d_interval.swap((bsls_PlatformUtil::Int64) (MICROSECS_PER_SECOND / rate));

    bdet_TimeInterval now = bdetu_SystemTime::now();
    d_timestamp.swap(now.totalMicroseconds());
    d_nextTurn.swap(d_timestamp + startTime.totalMicroseconds());
}

// ACCESSORS

bsls_PlatformUtil::Int64 bcemt_Turnstile::lagTime() const
{
    bdet_TimeInterval now = bdetu_SystemTime::now();
    d_timestamp.swap(now.totalMicroseconds());

    bsls_PlatformUtil::Int64 delta = now.totalMicroseconds() - d_nextTurn;
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
