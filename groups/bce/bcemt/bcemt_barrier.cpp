// bcemt_barrier.cpp               -*-C++-*-
#include <bcemt_barrier.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcemt_barrier_cpp,"$Id$ $CSID$")

#include <bcemt_lockguard.h>
#include <bcemt_threadutil.h> // for yield
#include <bsls_assert.h>

namespace BloombergLP {

bcemt_Barrier::~bcemt_Barrier()
{
    while (1) {

        {
            bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);
            if (0 == d_numPending) break;
        }

        bcemt_ThreadUtil::yield();
    }

    BSLS_ASSERT( 0 == d_numWaiting );
}

void bcemt_Barrier::wait()
{
    bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);
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

int bcemt_Barrier::timedWait(const bdet_TimeInterval &timeout)
{
    bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);
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
                d_sigCount == prevSigCount ) {
                --d_numWaiting;
                return -1;
            }
        }
        --d_numPending;
    }
    return 0;
}

} // namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
