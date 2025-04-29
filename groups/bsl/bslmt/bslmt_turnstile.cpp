// bslmt_turnstile.cpp                                                -*-C++-*-
#include <bslmt_turnstile.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslmt_turnstile_cpp,"$Id$ $CSID$")

#include <bslmt_threadutil.h>

#include <bsls_assert.h>
#include <bsls_systemtime.h>
#include <bsls_timeinterval.h>

namespace BloombergLP {
namespace {

enum { k_MICROSECS_PER_SECOND = 1000 * 1000 };

/// Increase the value stored at the specified `timestamp` to the current time.
/// If another thread has updated `timestamp`, this method might not modify the
/// stored value.  Return the current value of the `timestamp`.
static bsls::Types::Int64 updateTimestamp(bsls::AtomicInt64 *timestamp)
{
    bsls::Types::Int64 nowUSec =
                     bsls::SystemTime::nowMonotonicClock().totalMicroseconds();

    bsls::Types::Int64 last = *timestamp;
    if (last >= nowUSec) {
        return last;                                                  // RETURN
    }

    bsls::Types::Int64 expected;
    do {
        expected = last;
        last = timestamp->testAndSwap(expected, nowUSec);
        if (last >= nowUSec) {
            return last;                                              // RETURN
        }
    } while (last != expected);
    return nowUSec;
}

}  // close unnamed namespace

                             // ---------------
                             // class Turnstile
                             // ---------------

// CREATORS
bslmt::Turnstile::Turnstile(double                    rate,
                            const bsls::TimeInterval& startTime,
                            const bsls::TimeInterval& minTimeToCallSleep)
{
    d_timestamp = 0;
    reset(rate, startTime, minTimeToCallSleep);
}

// MANIPULATORS
void bslmt::Turnstile::reset(double                    rate,
                             const bsls::TimeInterval& startTime,
                             const bsls::TimeInterval& minTimeToCallSleep)
{
    d_interval  = static_cast<Int64>(
                           static_cast<double>(k_MICROSECS_PER_SECOND)/rate);
    Int64 nowUSec = updateTimestamp(&d_timestamp);
    d_nextTurn  = nowUSec + startTime.totalMicroseconds();

    d_minTimeToCallSleep = static_cast<int>(
                                       minTimeToCallSleep.totalMicroseconds());

    BSLS_ASSERT_SAFE(0 < d_minTimeToCallSleep);
    BSLS_ASSERT_SAFE(    d_minTimeToCallSleep <= d_interval);
}

bsls::Types::Int64 bslmt::Turnstile::waitTurn(bool sleep)
{
    Int64 nextTurn  = d_nextTurn.add(d_interval) - d_interval;

    if (nextTurn <= d_timestamp) {
        return 0;                                                     // RETURN
    }

    Int64 nowUSec  = updateTimestamp(&d_timestamp);
    Int64 waitTime = nextTurn - nowUSec;

    if (waitTime < d_minTimeToCallSleep) {
        return 0;                                                     // RETURN
    }
    if (sleep == false) {
        return waitTime;                                              // RETURN
    }

    // This will work so long as 'waitTime < ~68 years'
    int waitSecs  = static_cast<int>((waitTime / k_MICROSECS_PER_SECOND));
    int waitUSecs = static_cast<int>((waitTime % k_MICROSECS_PER_SECOND));
    ThreadUtil::microSleep(waitUSecs, waitSecs);

    return waitTime;
}

// ACCESSORS
bsls::Types::Int64 bslmt::Turnstile::lagTime() const
{
    Int64 nowUSecs = updateTimestamp(&d_timestamp);

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
