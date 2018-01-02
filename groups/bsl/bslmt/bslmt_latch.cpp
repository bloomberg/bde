// bslmt_latch.cpp                                                    -*-C++-*-
#include <bslmt_latch.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslmt_latch_cpp,"$Id$ $CSID$")

#include <bslmt_barrier.h>        // for testing only
#include <bslmt_lockguard.h>
#include <bslmt_threadgroup.h>    // for testing only
#include <bslmt_threadutil.h>     // for testing only

namespace BloombergLP {
namespace bslmt {

                             // -----------
                             // class Latch
                             // -----------

// CREATORS
Latch::~Latch()
{
}

// MANIPULATORS
void Latch::arrive()
{
    countDown(1);
}

void Latch::arriveAndWait()
{
    countDown(1);
    wait();
}

void Latch::countDown(int numEvents)
{
    BSLS_ASSERT(numEvents > 0);

    // Point 1: The following operation requires release semantics to ensure
    // memory visibility for other threads using 'currentCount' and 'tryWait'
    // (see Points 2 and 3 below, respectively).

    int current, expected;
    do {
        current  = d_sigCount.loadRelaxed();
        expected = current - numEvents;
        BSLS_ASSERT(0 <= expected);

        if (0 == expected) {
            // 0 is a special case: by contract, only 1 thread can ever believe
            // it is about to decrement the count to 0 (because if there were
            // two threads in that situation, one of them would take the count
            // to 0 and then the other would illegally make the count
            // negative).  Thus, we can simply save a 0 under the lock,
            // broadcast on the condition variable, and exit the function.
            //
            // We want to save the 0 under the lock because in typical usage,
            // the waiting thread owns this structure and may destroy it once
            // it observes a 0.

            LockGuard<Mutex> lock(&d_mutex);

            BSLS_ASSERT(current == d_sigCount.loadRelaxed());
            d_sigCount.storeRelease(0);
            d_cond.broadcast();

            return;                                                   // RETURN
        }
    } while (current != d_sigCount.testAndSwapAcqRel(current, expected));
}

int Latch::timedWait(const bsls::TimeInterval &timeout)
{
    LockGuard<Mutex> lock(&d_mutex);

    // Note that the following operation does not require memory visibility
    // guarantees because they are already provided by the lock/unlock of
    // 'd_mutex'.

    int rc = 0;
    while (0 == rc && 0 < d_sigCount.loadRelaxed()) {
        rc = d_cond.timedWait(&d_mutex, timeout);
    }
    return rc;
}

void Latch::wait()
{
    LockGuard<Mutex> lock(&d_mutex);

    // Note that the following operation does not require memory visibility
    // guarantees because they are already provided by the lock/unlock of
    // 'd_mutex'.

    while (0 < d_sigCount.loadRelaxed()) {
        d_cond.wait(&d_mutex);
    }
}

// ACCESSORS
int Latch::currentCount() const
{
    // Point 2: The following operation requires acquire semantics to ensure
    // memory visibility for other threads using 'countDown' (see Point 1
    // above).  Note that while this method is not intended for
    // synchronization, clients may still use it in that way.

    return d_sigCount.loadAcquire();
}

bool Latch::tryWait() const
{
    // Point 3: The following operation requires acquire semantics to ensure
    // memory visibility for other threads using 'countDown' (see Point 1
    // above).

    return 0 == d_sigCount.loadAcquire();
}

}  // close package namespace
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
