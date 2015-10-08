// bslmt_turnstile.cpp                                                -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bslmt_turnstile.h>

#include <bslmt_barrier.h>        // testing only
#include <bslmt_mutex.h>          // testing only
#include <bslmt_threadutil.h>

#include <bsls_systemtime.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslmt_turnstile_cpp,"$Id$ $CSID$")

namespace BloombergLP {
namespace {

enum { k_MICROSECS_PER_SECOND = 1000 * 1000 };

}  // close unnamed namespace

                             // ---------------
                             // class Turnstile
                             // ---------------

// CREATORS
bslmt::Turnstile::Turnstile(double rate, const bsls::TimeInterval& startTime)
{
    reset(rate, startTime);
}

// MANIPULATORS
void bslmt::Turnstile::reset(double rate, const bsls::TimeInterval& startTime)
{
    d_interval  = static_cast<Int64>(k_MICROSECS_PER_SECOND / rate);
    d_timestamp = bsls::SystemTime::nowMonotonicClock().totalMicroseconds();
    d_nextTurn  = d_timestamp + startTime.totalMicroseconds();
}

bsls::Types::Int64 bslmt::Turnstile::waitTurn()
{
    enum { k_MIN_TIMER_RESOLUTION = 10 * 1000 };
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

        if (waitTime >= k_MIN_TIMER_RESOLUTION) {
            int waitInt = static_cast<int>(waitTime);
            if (waitInt == waitTime) {
                // This is only good up to 'waitTime == ~35 minutes'

                ThreadUtil::microSleep(waitInt);
            }
            else {
                // This will work so long as 'waitTime < ~68 years'

                int waitSecs  = static_cast<int>((waitTime
                                                  / k_MICROSECS_PER_SECOND));
                int waitUSecs = static_cast<int>((waitTime
                                                  % k_MICROSECS_PER_SECOND));
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
bsls::Types::Int64 bslmt::Turnstile::lagTime() const
{
    Int64 nowUSecs = bsls::SystemTime::nowMonotonicClock().totalMicroseconds();
    d_timestamp = nowUSecs;

    Int64 delta = nowUSecs - d_nextTurn;
    return delta > 0 ? delta : 0;
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
