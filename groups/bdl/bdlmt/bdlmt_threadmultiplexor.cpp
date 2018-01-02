// bdlmt_threadmultiplexor.cpp                                        -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlmt_threadmultiplexor.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmt_threadmultiplexor_cpp,"$Id$ $CSID$")

#include <bdlmt_fixedthreadpool.h> // for testing only

#include <bslma_default.h>

#include <bsls_assert.h>

namespace BloombergLP {

namespace bdlmt {
                          // -----------------------
                          // class ThreadMultiplexor
                          // -----------------------

// PRIVATE MANIPULATORS

int ThreadMultiplexor::processJobQueue()
{
    Job job;
    while (0 == d_jobQueue_p->tryPopFront(&job)) {
        int previousNumProcessors = d_numProcessors;
        if (previousNumProcessors < d_maxProcessors &&
            previousNumProcessors ==
                      d_numProcessors.testAndSwap(previousNumProcessors,
                                                  previousNumProcessors + 1)) {
            // Process the job
            job();
            --d_numProcessors;
        }
        else {
            int rc = d_jobQueue_p->tryPushBack(job);

            if (0 != rc || d_numProcessors == d_maxProcessors) {
                // Exit the loop if there was an error, or if the maximum
                // number of processors has been reached.  Otherwise, the
                // dispatching loop may not terminate (e.g., if all processing
                // threads are blocked).
                return rc;                                            // RETURN
            }
        }
    }

    return 0;
}

// CREATORS

ThreadMultiplexor::ThreadMultiplexor(int               maxProcessors,
                                     int               maxQueueSize,
                                     bslma::Allocator *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_jobQueue_p(0)
, d_numProcessors(0)
, d_maxProcessors(maxProcessors)
{
    BSLS_ASSERT(0 < maxProcessors);
    BSLS_ASSERT(0 < maxQueueSize);

    d_jobQueue_p = new (*d_allocator_p)
            bdlcc::FixedQueue<Job>(maxQueueSize, d_allocator_p);
}

ThreadMultiplexor::~ThreadMultiplexor()
{
    BSLS_ASSERT(0 == d_jobQueue_p->length());

    d_allocator_p->deleteObjectRaw(d_jobQueue_p);
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
