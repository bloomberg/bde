// bslmt_sluice.cpp                                                   -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bslmt_sluice.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslmt_sluice_cpp,"$Id$ $CSID$")

#include <bslmt_lockguard.h>

#include <bslmt_semaphore.h>    // for testing only
#include <bslmt_threadgroup.h>  // for testing only
#include <bslmt_threadutil.h>   // for testing only

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

namespace BloombergLP {

                    // ----------------------------------
                    // class Sluice::GenerationDescriptor
                    // ----------------------------------

// CREATORS
bslmt::Sluice::GenerationDescriptor::GenerationDescriptor(
                                         bsls::SystemClockType::Enum clockType)
: d_numThreads(0)
, d_numSignaled(0)
, d_sema(0, clockType)
, d_next(0)
{
}

                               // ------------
                               // class Sluice
                               // ------------

// CREATORS
bslmt::Sluice::Sluice(bslma::Allocator *basicAllocator)
: d_signaledGeneration(0)
, d_pendingGeneration(0)
, d_descriptorPool(0)
, d_clockType(bsls::SystemClockType::e_REALTIME)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

bslmt::Sluice::Sluice(bsls::SystemClockType::Enum  clockType,
                      bslma::Allocator            *basicAllocator)
: d_signaledGeneration(0)
, d_pendingGeneration(0)
, d_descriptorPool(0)
, d_clockType(clockType)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

bslmt::Sluice::~Sluice()
{
    BSLS_ASSERT(0 == d_signaledGeneration);
    BSLS_ASSERT(0 == d_pendingGeneration);

    GenerationDescriptor *g = d_descriptorPool;
    while (g) {
        GenerationDescriptor *c = g;
        g = g->d_next;

        BSLS_ASSERT(0 == c->d_numThreads);
        BSLS_ASSERT(0 == c->d_numSignaled);
        d_allocator_p->deleteObject(c);
    }
}

// MANIPULATORS
const void *bslmt::Sluice::enter()
{
    LockGuard<Mutex> lock(&d_mutex);

    GenerationDescriptor *g = d_pendingGeneration;
    if (0 == g) {

        g = d_descriptorPool;
        if (g) {
            // The pool is not empty.  Get a descriptor from the pool.
            d_descriptorPool = g->d_next;
        }
        else {
            // The pool is empty.  Allocate a new descriptor.
            g = new (*d_allocator_p) GenerationDescriptor(d_clockType);
        }

        d_pendingGeneration = g;
        BSLS_ASSERT(0 == g->d_numThreads);
        BSLS_ASSERT(0 == g->d_numSignaled);
    }

    ++g->d_numThreads;
    return g;
}

void bslmt::Sluice::wait(const void *token)
{
    GenerationDescriptor *g =
                static_cast<GenerationDescriptor *>(const_cast<void *>(token));

    for (;;) {
        g->d_sema.wait();

        LockGuard<Mutex> lock(&d_mutex);

        if (g->d_numSignaled) {
            const int numSignaled = --g->d_numSignaled;
            const int numThreads  = --g->d_numThreads;

            BSLS_ASSERT(numThreads >= numSignaled);
            BSLS_ASSERT(d_pendingGeneration != g);

            if (0 == numThreads) {
                // The last thread is responsible for cleanup.

                BSLS_ASSERT(d_signaledGeneration != g);

                g->d_next = d_descriptorPool;
                d_descriptorPool = g;
            }
            return;                                                   // RETURN
        }
        // Spurious wakeups happen because 'timedWait' may timeout on the
        // semaphore, but still consume a signal.
    }
}

int bslmt::Sluice::timedWait(const void                *token,
                             const bsls::TimeInterval&  timeout)
{
    GenerationDescriptor *g =
                static_cast<GenerationDescriptor *>(const_cast<void *>(token));

    for (;;) {
        int rc = g->d_sema.timedWait(timeout);

        LockGuard<Mutex> lock(&d_mutex);

        if (g->d_numSignaled) {
            BSLS_ASSERT(d_pendingGeneration != g);

            --g->d_numSignaled;

            rc = 0;
        }
        else if (0 == rc) {
            continue;
        }

        const int numThreads = --g->d_numThreads;

        if (0 == numThreads) {
            // The last thread is responsible for cleanup.

            if (d_signaledGeneration == g) {
                BSLS_ASSERT(0 != rc);
                d_signaledGeneration = 0;
            }

            if (d_pendingGeneration == g) {
                BSLS_ASSERT(0 != rc);
                d_pendingGeneration = 0;
            }

            g->d_next = d_descriptorPool;
            d_descriptorPool = g;
        }
        return rc;                                                    // RETURN
    }
}

void bslmt::Sluice::signalAll()
{
    LockGuard<Mutex> lock(&d_mutex);

    GenerationDescriptor *s = d_signaledGeneration;
    int scount = 0;
    if (s) {
        const int numThreads = s->d_numThreads;
        scount = numThreads - s->d_numSignaled;
        s->d_numSignaled = numThreads;

        d_signaledGeneration = 0;
    }

    GenerationDescriptor *p = d_pendingGeneration;
    int pcount = 0;
    if (p) {
        const int numThreads = p->d_numThreads;
        pcount = numThreads;
        p->d_numSignaled = numThreads;

        d_pendingGeneration = 0;
    }

    lock.release()->unlock();

    if (s) {
        s->d_sema.post(scount);
    }

    if (p) {
        p->d_sema.post(pcount);
    }
}

void bslmt::Sluice::signalOne()
{
    LockGuard<Mutex> lock(&d_mutex);

    GenerationDescriptor *g = d_signaledGeneration;
    if (0 == g) {
        g = d_pendingGeneration;
        if (0 == g) {
            // There are no threads to signal.  We are done.
            return;                                                   // RETURN
        }
        d_signaledGeneration = g;
        d_pendingGeneration  = 0;
    }

    const int numThreads  = g->d_numThreads;
    const int numSignaled = ++g->d_numSignaled;

    if (numThreads == numSignaled) {
        d_signaledGeneration = 0;
    }

    lock.release()->unlock();
    g->d_sema.post();
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
