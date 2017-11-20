// bdlmt_throttle.cpp                                                 -*-C++-*-

#include <bdlmt_throttle.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmt_throttle_cpp,"$Id$ $CSID$")

#include <bsl_algorithm.h>

//-----------------------------------------------------------------------------
// Implementation notes.
//
// It is important that 'Throttle' has no creators, private or public -- it
// must be aggregate initiailizable, and some compilers have bugs with regard
// to aggregate initializing objects with ANY creators declared.
//-----------------------------------------------------------------------------

                            // ----------------------
                            // struct bdlmt::Throttle
                            // ----------------------

namespace BloombergLP {
namespace bdlmt {

// MANIPULATORS
void Throttle::initialize(int                         maxSimultaneousActions,
                          Int64                       nanosecondsPerAction,
                          bsls::SystemClockType::Enum clockType)
{
    BSLS_ASSERT(0 <= maxSimultaneousActions);
    BSLS_ASSERT(0 <= nanosecondsPerAction);
    BSLS_ASSERT(maxSimultaneousActions || nanosecondsPerAction);
    BSLS_ASSERT(LLONG_MAX / bsl::max(maxSimultaneousActions, 1) >=
                                                     nanosecondsPerAction);

    BSLS_ASSERT(bsls::SystemClockType::e_MONOTONIC == clockType ||
                bsls::SystemClockType::e_REALTIME  == clockType);

    AtomicOps::setInt64(&d_prevLeakTime, -k_TEN_YEARS_NANOSECONDS);
    d_nanosecondsPerAction = 0 == maxSimultaneousActions
                           ? k_ALLOW_NONE
                           : nanosecondsPerAction
                           ? nanosecondsPerAction
                           : k_ALLOW_ALL;

    // If 'd_nanosecondsPerAction' is set to 'allow all' or 'allow none',
    // then it doesn't matter what 'd_nanosecondsPerTotalReset' is.

    d_nanosecondsPerTotalReset = maxSimultaneousActions * nanosecondsPerAction;
    d_maxSimultaneousActions             = 0 == nanosecondsPerAction
                               ? INT_MAX
                               : maxSimultaneousActions;
    d_clockType                = clockType;
}

bool Throttle::requestPermission(const bsls::TimeInterval& now)
{
    // Special casing 'allow all' here prevents undefined behavior later in
    // the function due to overflowing signed arithmetic.

    if (k_ALLOW_ALL == d_nanosecondsPerAction) {
        return true;                                                  // RETURN
    }

    const Int64 currentTime  = now.totalNanoseconds();
    Int64       prevLeakTime = AtomicOps::getInt64Acquire(&d_prevLeakTime);
    while (true) {
        const Int64 timeDiff = currentTime - prevLeakTime;
        if (timeDiff < d_nanosecondsPerAction) {
            return false;                                             // RETURN
        }
        const Int64 nextLeakTime = d_nanosecondsPerTotalReset <= timeDiff
                                 ? currentTime - d_nanosecondsPerTotalReset +
                                                         d_nanosecondsPerAction
                                 : prevLeakTime + d_nanosecondsPerAction;
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

    if (d_maxSimultaneousActions < numActions) {
        BSLS_ASSERT(0 == d_maxSimultaneousActions);

        // It is best to deal with the 'allow none' case here.  We have to do
        // the above two conditions just for asserts on our way into the
        // method, and if we don't handle the
        // 'd_maxSimultaneousActions < numActions' case here we risk undefined
        // behavior due to signed arithmetic overflow later on.

        return false;                                                 // RETURN
    }

    // Testing for 'k_ALLOW_ALL' here prevents undefined behavior later in
    // the function due to overflowing signed arithmetic.

    if (k_ALLOW_ALL == d_nanosecondsPerAction) {
        return true;                                                  // RETURN
    }

    const Int64 currentTime  = now.totalNanoseconds();
    const Int64 requiredTime = numActions * d_nanosecondsPerAction;
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
    if (numActions <= 0 || d_maxSimultaneousActions < numActions) {
        return -1;                                                    // RETURN
    }

    result->setTotalNanoseconds(AtomicOps::getInt64Acquire(
                           const_cast<AtomicTypes::Int64 *>(&d_prevLeakTime)) +
                                          numActions * d_nanosecondsPerAction);
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
