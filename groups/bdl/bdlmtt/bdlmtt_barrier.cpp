// bdlmtt_barrier.cpp                                                  -*-C++-*-
#include <bdlmtt_barrier.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmtt_barrier_cpp,"$Id$ $CSID$")

#include <bdlmtt_lockguard.h>
#include <bdlmtt_threadutil.h> // for yield
#include <bsls_assert.h>

namespace BloombergLP {

namespace bdlmtt {
Barrier::~Barrier()
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

int Barrier::timedWait(const bsls::TimeInterval &timeout)
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

void Barrier::wait()
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
}  // close package namespace

} // namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
