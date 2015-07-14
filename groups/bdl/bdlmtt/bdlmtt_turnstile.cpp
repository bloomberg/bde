// bdlmtt_turnstile.cpp                                                -*-C++-*-
#include <bdlmtt_turnstile.h>

#include <bdlmtt_barrier.h>        // testing only
#include <bdlmtt_mutex.h>          // testing only
#include <bdlmtt_threadutil.h>

#include <bsls_systemtime.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmtt_turnstile_cpp,"$Id$ $CSID$")


namespace BloombergLP {

namespace {

enum { MICROSECS_PER_SECOND = 1000 * 1000 };

}  // close unnamed namespace

namespace bdlmtt {
                           // ---------------------
                           // class Turnstile
                           // ---------------------

// CREATORS
Turnstile::Turnstile(double                   rate,
                                 const bsls::TimeInterval& startTime)
{
    reset(rate, startTime);
}

// MANIPULATORS
void Turnstile::reset(double                   rate,
                            const bsls::TimeInterval& startTime)
{
    d_interval  = static_cast<Int64>(MICROSECS_PER_SECOND / rate);
    d_timestamp = bsls::SystemTime::nowMonotonicClock().totalMicroseconds();
    d_nextTurn  = d_timestamp + startTime.totalMicroseconds();
}

bsls::Types::Int64 Turnstile::waitTurn()
{
    enum { MIN_TIMER_RESOLUTION = 10 * 1000 };
        // Assume that minimum timer resolution applicable to "sleep" on all
        // supported platforms is 10 milliseconds.

    Int64 timestamp = d_timestamp;
    Int64 interval  = d_interval;
    Int64 nextTurn  = d_nextTurn.add(interval) - interval;
    Int64 waitTime  = 0;

    if (nextTurn > timestamp) {
        Int64 nowUSec =
                     bsls::SystemTime::nowMonotonicClock().totalMicroseconds();
        d_timestamp = nowUSec;

        waitTime = nextTurn - nowUSec;

        if (waitTime >= MIN_TIMER_RESOLUTION) {
          int waitInt = static_cast<int>(waitTime);
            if (waitInt == waitTime) {
                // This is only good up to 'waitTime == ~35 minutes'

                ThreadUtil::microSleep(waitInt);
            }
            else {
                // This will work so long as 'waitTime < ~68 years'

                int waitSecs  = static_cast<int>((waitTime
                                                  / MICROSECS_PER_SECOND));
                int waitUSecs = static_cast<int>((waitTime
                                                  % MICROSECS_PER_SECOND));
                ThreadUtil::microSleep(waitUSecs, waitSecs);
            }
        }
        else {
            waitTime = 0;
        }
    }

    return waitTime;
}

// ACCESSORS
bsls::Types::Int64 Turnstile::lagTime() const
{
    Int64 nowUSecs = bsls::SystemTime::nowMonotonicClock().totalMicroseconds();
    d_timestamp = nowUSecs;

    Int64 delta = nowUSecs - d_nextTurn;
    return delta > 0 ? delta : 0;
}
}  // close package namespace

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007, 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
