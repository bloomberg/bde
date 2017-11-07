// bdlmt_throttle.cpp                                                 -*-C++-*-

#include <bdlmt_throttle.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmt_throttle_cpp,"$Id$ $CSID$")

#include <bsl_algorithm.h>

                            // ----------------------
                            // struct bdlmt::Throttle
                            // ----------------------

namespace BloombergLP {
namespace bdlmt {

// MANIPULATORS
void Throttle::initialize(int                         maxBurstSize,
                          Int64                       nanosecondsPerActionLeak,
                          bsls::SystemClockType::Enum clockType)
{
    BSLS_ASSERT(0 <= maxBurstSize);
    BSLS_ASSERT(0 <= nanosecondsPerActionLeak);
    BSLS_ASSERT(maxBurstSize || nanosecondsPerActionLeak);
    BSLS_ASSERT(LLONG_MAX / bsl::max(maxBurstSize, 1) >=
                                                     nanosecondsPerActionLeak);

    BSLS_ASSERT(bsls::SystemClockType::e_MONOTONIC == clockType ||
                bsls::SystemClockType::e_REALTIME  == clockType);

    AtomicOps::setInt64(&d_prevLeakTime, -s_tenYearsNanoseconds);
    d_nanosecondsPerActionLeak = 0 == maxBurstSize
                               ? s_allowNoneNanoseconds
                               : nanosecondsPerActionLeak
                               ? nanosecondsPerActionLeak
                               : s_allowAllNanoseconds;

    // If 'd_nanosecondsPerActionLeak' is set to 'allow all' or 'allow none',
    // then it doesn't matter what 'd_nanosecondsPerTotalReset' is.

    d_nanosecondsPerTotalReset = maxBurstSize * nanosecondsPerActionLeak;
    d_maxBurstSize             = maxBurstSize;
    d_clockType                = clockType;
}

bool Throttle::requestPermission(const bsls::TimeInterval& now)
{
    // Special casing 'allow all' here prevents undefined behavior later in
    // the function due to overflowing signed arithmetic.

    if (s_allowAllNanoseconds == d_nanosecondsPerActionLeak) {
        return true;                                                  // RETURN
    }

    const Int64 currentTime  = now.totalNanoseconds();
    Int64       prevLeakTime = AtomicOps::getInt64Acquire(&d_prevLeakTime);
    while (true) {
        const Int64 timeDiff = currentTime - prevLeakTime;
        if (timeDiff < d_nanosecondsPerActionLeak) {
            return false;                                             // RETURN
        }
        const Int64 nextLeakTime = d_nanosecondsPerTotalReset <= timeDiff
                                 ? currentTime - d_nanosecondsPerTotalReset +
                                                     d_nanosecondsPerActionLeak
                                 : prevLeakTime + d_nanosecondsPerActionLeak;
        const Int64 swappedLeakTime = AtomicOps::testAndSwapInt64AcqRel(
                                                               &d_prevLeakTime,
                                                               prevLeakTime,
                                                               nextLeakTime);
        if (swappedLeakTime == prevLeakTime) {
            return true;                                              // RETURN
        }

        prevLeakTime = swappedLeakTime;
    }
}

bool Throttle::requestPermission(int                       numActions,
                                 const bsls::TimeInterval& now)
{
    BSLS_ASSERT(0 < numActions);

    if (d_maxBurstSize < numActions) {
        BSLS_ASSERT(0 == d_maxBurstSize);

        // It is best to deal with the 'allow none' case here.  We have to do
        // the above two conditions just for asserts on our way into the
        // method, and if we don't handle the case here we have to take
        // measures to prevent 'requiredTime' from overflowing and becoming
        // negative.

        return false;                                                 // RETURN
    }

    // Special casing 'allow all' here prevents undefined behavior later in
    // the function due to overflowing signed arithmetic.

    if (s_allowAllNanoseconds == d_nanosecondsPerActionLeak) {
        return true;                                                  // RETURN
    }

    const Int64 currentTime  = now.totalNanoseconds();
    const Int64 requiredTime = numActions * d_nanosecondsPerActionLeak;
    const Int64 lagTime      = d_nanosecondsPerTotalReset - requiredTime;

    Int64 prevLeakTime = AtomicOps::getInt64Acquire(&d_prevLeakTime);
    while (true) {
        const Int64 timeDiff = currentTime - prevLeakTime;
        if (timeDiff < requiredTime) {
            return false;                                             // RETURN
        }
        const Int64 nextLeakTime = d_nanosecondsPerTotalReset <= timeDiff
                                 ? currentTime - lagTime
                                 : prevLeakTime + requiredTime;
        const Int64 swappedLeakTime = AtomicOps::testAndSwapInt64AcqRel(
                                                               &d_prevLeakTime,
                                                               prevLeakTime,
                                                               nextLeakTime);
        if (swappedLeakTime == prevLeakTime) {
            return true;                                              // RETURN
        }

        prevLeakTime = swappedLeakTime;
    }
}

// ACCESSOR
int Throttle::nextPermit(bsls::TimeInterval *result, int numActions) const
{
    if (numActions <= 0 || d_maxBurstSize < numActions) {
        return -1;                                                    // RETURN
    }

    result->setTotalNanoseconds(AtomicOps::getInt64Acquire(&d_prevLeakTime) +
                                      numActions * d_nanosecondsPerActionLeak);
    return 0;
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2017 Bloomberg Finance L.P.
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
