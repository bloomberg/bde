// bslmt_barrier.cpp                                                  -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bslmt_barrier.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslmt_barrier_cpp,"$Id$ $CSID$")

#include <bslmt_lockguard.h>
#include <bslmt_threadutil.h> // for yield
#include <bsls_assert.h>

namespace BloombergLP {

bslmt::Barrier::~Barrier()
{
    while (1) {

        {
            LockGuard<Mutex> lock(&d_mutex);
            if (0 == d_numPending) break;
        }

        ThreadUtil::yield();
    }

    BSLS_ASSERT( 0 == d_numWaiting );
}

int bslmt::Barrier::timedWait(const bsls::TimeInterval &timeout)
{
    LockGuard<Mutex> lock(&d_mutex);
    int prevSigCount = d_sigCount;
    if (++d_numWaiting == d_numThreads) {
        ++d_sigCount;
        d_numPending += d_numThreads - 1;
        d_numWaiting = 0;
        d_cond.broadcast();
    }
    else {
        while (d_sigCount == prevSigCount) {
            if (d_cond.timedWait(&d_mutex, timeout) &&
                d_sigCount == prevSigCount) {
                --d_numWaiting;
                return -1;                                            // RETURN
            }
        }
        --d_numPending;
    }
    return 0;
}

void bslmt::Barrier::wait()
{
    LockGuard<Mutex> lock(&d_mutex);
    int sigCount = d_sigCount;
    if (++d_numWaiting == d_numThreads) {
        ++d_sigCount;
        d_numPending += d_numThreads - 1;
        d_numWaiting = 0;
        d_cond.broadcast();
    }
    else {
        while (d_sigCount == sigCount) {
            d_cond.wait(&d_mutex);
        }
        --d_numPending;
    }
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
