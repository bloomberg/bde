// bcep_threadpool.cpp            -*-C++-*-
#include <bcep_threadpool.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcep_threadpool_cpp,"$Id$ $CSID$")

#include <bcemt_lockguard.h>
#include <bdef_function.h>
#include <bdetu_systemtime.h>
#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsls_platform.h>
#include <bsls_platformutil.h>
#include <bsls_timeutil.h>

#include <bsl_deque.h>

#include <bcemt_barrier.h>    // for testing only
#include <bcemt_lockguard.h>  // for testing only
#include <bcemt_thread.h>     // for testing only

#if defined(BSLS_PLATFORM_OS_UNIX)
#include <bsl_c_signal.h>              // sigfillset
#endif

#include <bsl_cstdlib.h>

namespace BloombergLP {

                         // =======================
                         // bcep_ThreadPoolWaitNode
                         // =======================

struct bcep_ThreadPoolWaitNode {
    // This structure is used to implement the linked list of threads that are
    // waiting for a request to process.  Each thread has its own instance of
    // this structure (a local variable in the bcep_ThreadPool::workerThread).
    // When a thread finishes executing a job, if there are no pending jobs, it
    // will add itself to the head of the wait list.  This logic behaves the
    // same as a condition variable with the exception that it implements a
    // LIFO instead of a FIFO wait list logic.  This ensures that threads that
    // are truly idle will not wake until they have timed out.  When a new job
    // arrives, the thread will be signaled to process by using the
    // 'd_jobCond' condition variable.  Whether the thread really has a job
    // or the condition variable timed out on its wait is stored in the
    // 'd_hasJob' state value.  A thread that takes on a new job removes itself
    // from the wait list (not necessarily at the head of the list).

    bcemt_Condition                   d_jobCond; // signaled when 'd_hasJob'
                                                 // is set

    bcep_ThreadPoolWaitNode* volatile d_next;    // pointer to the next
                                                 // waiting thread

    bcep_ThreadPoolWaitNode* volatile d_prev;    // pointer to the previous
                                                 // waiting thread

    volatile int                      d_hasJob;  // 1 if a job has been
                                                 // enqueued, 0 otherwise
};

                         // ====================
                         // bcep_ThreadPoolEntry
                         // ====================

extern "C" void* bcep_ThreadPoolEntry(void *aThis)
{
    ((bcep_ThreadPool*)aThis)->workerThread();
    return 0;
}

                         // =======================
                         // bcep_ThreadPoolWaitNode
                         // =======================

// PRIVATE MANIPULATORS
void bcep_ThreadPool::doEnqueueJob(const Job& job)
{
    d_queue.push_back(job);
    if (d_waitHead) {
        // Signal this thread (used in 'bcep::ThreadPool::workerThread' below).

        d_waitHead->d_hasJob = 1;
        d_waitHead->d_jobCond.signal();

        // And pop the current thread from the wait list.

        d_waitHead = d_waitHead->d_next;
        if (d_waitHead) {
            d_waitHead->d_prev = 0;
        }
    }
}

#if defined(BSLS_PLATFORM_OS_UNIX)
void bcep_ThreadPool::initBlockSet()
{
    sigfillset(&d_blockSet);

    static const int synchronousSignals[] = {
        SIGBUS,
        SIGFPE,
        SIGILL,
        SIGSEGV,
        SIGSYS,
        SIGABRT,
        SIGTRAP,
    #if !defined(BSLS_PLATFORM_OS_CYGWIN) || defined(SIGIOT)
        SIGIOT
    #endif
    };
    static const int SIZE = sizeof synchronousSignals
                                                  / sizeof *synchronousSignals;

    for (int i = 0; i < SIZE; ++i) {
        sigdelset(&d_blockSet, synchronousSignals[i]);
    }
}
#endif

int bcep_ThreadPool::startNewThread()
{
    bcemt_ThreadUtil::Handle handle;

#if defined(BSLS_PLATFORM_OS_UNIX)
    // block all synchronous signals
    sigset_t oldset;

    pthread_sigmask(SIG_BLOCK, &d_blockSet, &oldset);
#endif

    int rc = bcemt_ThreadUtil::create(&handle,d_threadAttributes,
                                      bcep_ThreadPoolEntry, this);

#if defined(BSLS_PLATFORM_OS_UNIX)
    // Restore the mask
    pthread_sigmask(SIG_SETMASK, &oldset, &d_blockSet);
#endif

    if (0 == rc) {
        ++d_threadCount;
    }
    else {
        ++d_createFailures;
    }
    return rc;
}

void bcep_ThreadPool::workerThread()
{
    bcep_ThreadPoolWaitNode waitNode;
    Job functor;
    while (1) {
        // The functor has to be cleared when we are *not* holding the lock
        // because it might have some objects bound with non-trivial
        // destructors.

        bool functorWasSetFlag = false;
        if (functor) {
            functor.clear();
            functorWasSetFlag = true;
        }

        {
            bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);
            if (functorWasSetFlag) {
                --d_numActiveThreads;
            }

            while (d_queue.empty()) {
                if (0 == d_numActiveThreads) {
                    d_drainCond.broadcast();
                }

                // Attach the 'waitNode' of this thread to the head of the wait
                // list.

                waitNode.d_hasJob = 0;
                waitNode.d_prev = 0;
                if (d_waitHead) {
                    d_waitHead->d_prev = &waitNode;
                }
                waitNode.d_next = d_waitHead;
                d_waitHead = &waitNode;

                // Let this thread wait until either there is a job available
                // or 'd_maxIdleTime' elapses.

                ++d_numWaiting;
                if (d_minThreads <= d_numActiveThreads) {
                    // This thread should be removed if it times out.

                    bdet_TimeInterval endTime = bdetu_SystemTime::now()
                                               .addMilliseconds(d_maxIdleTime);
                    do {
                        if (waitNode.d_jobCond.timedWait(&d_mutex,endTime)) {
                            // This thread timed out its max idle time.

                            break;
                        }

                        // Else we may either have been signaled or awakened
                        // spuriously.  In the latter case, loop.

                    } while (!waitNode.d_hasJob &&
                             bdetu_SystemTime::now() < endTime);
                }
                else {
                    // This thread should not be subject to a timeout, in order
                    // to maintain the minimum number of threads.

                    while (0 == waitNode.d_hasJob) {
                        waitNode.d_jobCond.wait(&d_mutex);
                    }
                }
                --d_numWaiting;

                if (0 == waitNode.d_hasJob ) {
                    // We haven't been signaled, so must have timed out.
                    // Remove this node from the wait list, but note that it
                    // will be put at the head of the list in the next
                    // iteration.

                    if (waitNode.d_next) {
                        waitNode.d_next->d_prev = waitNode.d_prev;
                    }
                    if (waitNode.d_prev) {
                        waitNode.d_prev->d_next = waitNode.d_next;
                    }
                    else {
                        d_waitHead = waitNode.d_next;
                    }

                    // In addition, in the following case, we may simply shut
                    // down this thread.

                    if (d_threadCount > d_minThreads) {
                        --d_threadCount;
                        return;
                    }
                }
            }

            functor = d_queue.front();
            d_queue.pop_front();

            // Although user-enqueued functors cannot be null, 'stop()' and
            // 'shutdown()' enqueue null functors to signal to this thread that
            // it should shutdown.

            if (!functor) {
                --d_threadCount;
                if (0 == d_threadCount) {
                    d_drainCond.broadcast();
                }
                return;
            }

            ++d_numActiveThreads;
        }

        // Run the callback and keep measurements.

        bsls_PlatformUtil::Int64 start  = bsls_TimeUtil::getTimer();
        functor();
        bsls_PlatformUtil::Int64 finish = bsls_TimeUtil::getTimer();
        if (start < d_lastResetTime) {
            d_callbackTime.add(finish - d_lastResetTime);
        }
        else {
            d_callbackTime.add(finish - start);
        }
    } // while (1)
}

// CREATORS
bcep_ThreadPool::bcep_ThreadPool(const bcemt_Attribute& threadAttributes,
                                 int                    minThreads,
                                 int                    maxThreads,
                                 int                    maxIdleTime,
                                 bslma_Allocator       *basicAllocator)
: d_queue(basicAllocator)
, d_threadAttributes(threadAttributes)
, d_maxThreads(maxThreads)
, d_minThreads(minThreads)
, d_threadCount(0)
, d_createFailures(0)
, d_maxIdleTime(maxIdleTime)
, d_numActiveThreads(0)
, d_numWaiting(0)
, d_enabled(0)
, d_waitHead(0)
, d_lastResetTime(bsls_TimeUtil::getTimer()) // now
{
    // Force all threads to be detached.

    d_threadAttributes.setDetachedState(
                                       bcemt_Attribute::BCEMT_CREATE_DETACHED);

#if defined(BSLS_PLATFORM_OS_UNIX)
    initBlockSet();
#endif
}

bcep_ThreadPool::~bcep_ThreadPool()
{
    shutdown();
}

// MANIPULATORS
void bcep_ThreadPool::drain()
{
    bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);
    d_enabled = 0;

    while ((d_threadCount && d_queue.size()) || d_numActiveThreads) {
        d_drainCond.wait(&d_mutex);
    }
}

int bcep_ThreadPool::enqueueJob(const Job& functor)
{
    if (!functor) {
        // Abort here if the 'functor' is "unset".  This prevents a crash
        // inside 'workerThread' (where the context of 'functor' would be
        // lost).

        BSLS_ASSERT(0);
        bsl::abort();  // abort (for when 'assert' is removed by optimization)
    }

    bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);
    if (!d_enabled) {
        return -1;
    }

    doEnqueueJob(functor);

    if ((int) d_queue.size() + d_numActiveThreads > d_threadCount &&
        d_threadCount < d_maxThreads ) {
        startNewThread();
    }
    return 0;
}

void bcep_ThreadPool::shutdown()
{
    bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);
    d_enabled = 0;

    while (!d_queue.empty()) {
        d_queue.pop_front();
    }
    for (int i = 0; i < d_threadCount; ++i) {
        doEnqueueJob(Job());
    }
    while (d_threadCount) {
        d_drainCond.wait(&d_mutex);
    }
    d_queue.clear();
}

double bcep_ThreadPool::resetPercentBusy()
{
    bsls_PlatformUtil::Int64 now           = bsls_TimeUtil::getTimer();
    bsls_PlatformUtil::Int64 callbackTime  = d_callbackTime.swap(0);
    bsls_PlatformUtil::Int64 lastResetTime = d_lastResetTime.swap(now);

    // on some platforms, the "nanosecond" timers can be coarser.
    // so if no time has been perceived to elapse,
    // set the minimum elapsed time to 1ns.

    bsls_PlatformUtil::Int64 interval = now - lastResetTime;
    if (0 == interval) {

        // BSLS_ASSERT (callbackTime <= interval);
        ++interval;  // let set minimal value in measurement units

        // what was before
        // return 0.0;
    }

    double percentBusy = 100.0 / d_maxThreads * callbackTime / interval;
    return percentBusy;
}

int bcep_ThreadPool::start()
{
    bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);
    d_enabled = 1;

    while (d_threadCount < d_minThreads) {
        if (0 != startNewThread()) {
            lock.release()->unlock();
            shutdown(); // terminate running threads.
            return -1;
        }
    }
    return 0;
}

void bcep_ThreadPool::stop()
{
    bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);
    d_enabled = 0;

    for (int i = 0; i < d_threadCount; ++i) {
        doEnqueueJob(Job());
    }
    while (d_threadCount) {
        d_drainCond.wait(&d_mutex);
    }
}

// ACCESSORS
int bcep_ThreadPool::numActiveThreads() const
{
    bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);
    return d_numActiveThreads;
}

int bcep_ThreadPool::numWaitingThreads() const
{
    bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);
    return d_threadCount - d_numActiveThreads;
}

int bcep_ThreadPool::numPendingJobs() const
{
    bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);
    return d_queue.size();
}

double bcep_ThreadPool::percentBusy() const {
    bsls_PlatformUtil::Int64 last     =  d_lastResetTime;
    bsls_PlatformUtil::Int64 interval = bsls_TimeUtil::getTimer() - last;

    // on some platforms, the "nanosecond" timers can be coarser.
    // so if no time has been perceived to elapse,
    // set the minimum elapsed time to 1ns.

    if (0 == interval) {
        ++interval;  // let set minimal value in measurement units
    }

    double percentBusy = 100.0 / d_maxThreads * d_callbackTime / interval;

    return percentBusy;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
