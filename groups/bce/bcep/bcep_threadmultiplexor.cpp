// bcep_threadmultiplexor.cpp   -*-C++-*-
#include <bcep_threadmultiplexor.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcep_threadmultiplexor_cpp,"$Id$ $CSID$")

#include <bcep_fixedthreadpool.h> // for testing only

#include <bslma_default.h>

#include <bsls_assert.h>

namespace BloombergLP {

                        // ----------------------------
                        // class bcep_ThreadMultiplexor
                        // ----------------------------

// PRIVATE MANIPULATORS

int bcep_ThreadMultiplexor::processJobQueue()
{
    Job job;
    while (0 == d_jobQueue_p->tryPopFront(&job)) {
        int previousNumProcessors = d_numProcessors;
        if (previousNumProcessors < d_maxProcessors
         && previousNumProcessors == d_numProcessors.testAndSwap(
                                                previousNumProcessors,
                                                previousNumProcessors + 1))
        {
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

bcep_ThreadMultiplexor::bcep_ThreadMultiplexor(
        int              maxProcessors,
        int              maxQueueSize,
        bslma_Allocator *basicAllocator)
: d_allocator_p(bslma_Default::allocator(basicAllocator))
, d_jobQueue_p(0)
, d_numProcessors(0)
, d_maxProcessors(maxProcessors)
{
    BSLS_ASSERT(0 < maxProcessors);
    BSLS_ASSERT(0 < maxQueueSize);

    d_jobQueue_p = new (*d_allocator_p)
            bcec_FixedQueue<Job>(maxQueueSize, d_allocator_p);
}

bcep_ThreadMultiplexor::~bcep_ThreadMultiplexor()
{
    BSLS_ASSERT(0 == d_jobQueue_p->length());

    d_allocator_p->deleteObjectRaw(d_jobQueue_p);
}

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
