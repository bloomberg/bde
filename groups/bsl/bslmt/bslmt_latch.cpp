// bslmt_latch.cpp                                                    -*-C++-*-
#include <bslmt_latch.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslmt_latch_cpp,"$Id$ $CSID$")

#include <bslmt_lockguard.h>

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
    BSLS_ASSERT(numEvents  > 0);
    BSLS_ASSERT(d_sigCount >= numEvents);

    // Point 1: The following operation requires release semantics to ensure
    // memory visibility for other threads using 'currentCount' and 'tryWait'
    // (see Points 2 and 3 below, respectively).

    if (0 == d_sigCount.addAcqRel(-numEvents)) {
        LockGuard<Mutex> lock(&d_mutex);

        d_cond.broadcast();
    }
}

void Latch::wait()
{
    LockGuard<Mutex> lock(&d_mutex);

    // Note that the following operation does not require memory visibility
    // guarantees because they are already provided by the lock/unlock of
    // 'd_mutex'.

    while (d_sigCount.loadRelaxed() > 0) {
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
