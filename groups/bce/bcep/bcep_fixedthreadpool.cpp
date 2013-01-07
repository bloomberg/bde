// bcep_fixedthreadpool.cpp            -*-C++-*-
#include <bcep_fixedthreadpool.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcep_fixedthreadpool_cpp,"$Id$ $CSID$")

#include <bcemt_lockguard.h>
#include <bcemt_thread.h>

#include <bdef_function.h>
#include <bdef_memfn.h>
#include <bdetu_systemtime.h>

#include <bsls_assert.h>
#include <bsls_performancehint.h>
#include <bsls_platform.h>
#include <bsls_timeutil.h>

#if defined(BSLS_PLATFORM_OS_UNIX)
#include <bsl_c_signal.h>              // sigfillset
#endif

namespace {

#if defined(BSLS_PLATFORM_OS_UNIX)
void initBlockSet(sigset_t *blockSet)
{
    sigfillset(blockSet);

    const int synchronousSignals[] = {
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

    const int SIZE = sizeof synchronousSignals / sizeof *synchronousSignals;

    for (int i=0; i < SIZE; ++i) {
        sigdelset(blockSet, synchronousSignals[i]);
    }
}
#endif
}

namespace BloombergLP {

                         // --------------------------
                         // class bcep_FixedThreadPool
                         // --------------------------

// PRIVATE MANIPULATORS
void bcep_FixedThreadPool::processJobs()
{
    while (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                        BCEP_RUN == d_control.relaxedLoad())) {
        Job functor;

        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                              d_queue.tryPopFront(&functor))) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            ++d_numThreadsWaiting;

            if (BCEP_RUN == d_control && d_queue.isEmpty()) {
                d_queueSemaphore.wait();
            }

            d_numThreadsWaiting.relaxedAdd(-1);
        }
        else {
            functor();
        }
    }
}

void bcep_FixedThreadPool::drainQueue()
{
    while (BCEP_DRAIN == d_control.relaxedLoad()) {
        Job functor;

        const int ret = d_queue.tryPopFront(&functor);
        if (ret) {
            return;
        }

        functor();
    }
}

void bcep_FixedThreadPool::waitWorkerThreads()
{
    bcemt_LockGuard<bcemt_Mutex> lock(&d_gateMutex);

    while (d_numThreadsReady != d_numThreads) {
        d_threadsReadyCond.wait(&d_gateMutex);
    }
}

void bcep_FixedThreadPool::releaseWorkerThreads()
{
    bcemt_LockGuard<bcemt_Mutex> lock(&d_gateMutex);
    d_numThreadsReady = 0;
    ++d_gateCount;
    d_gateCond.broadcast();

    // d_gateMutex.unlock() emits a release barrier.
}

void bcep_FixedThreadPool::interruptWorkerThreads()
{
    bcemt_LockGuard<bcemt_Mutex> lock(&d_gateMutex); // acquire barrier

    int numThreadsWaiting = d_numThreadsWaiting;

    for (int i = 0; i < numThreadsWaiting; ++i) {
        // Wake up waiting threads.

        d_queueSemaphore.post();
    }
}

void bcep_FixedThreadPool::workerThread()
{
    int gateCount = d_gateCount;

    while (1) {
        {
            bcemt_LockGuard<bcemt_Mutex> lock(&d_gateMutex);

            ++d_numThreadsReady;
            d_threadsReadyCond.signal();

            while (gateCount == d_gateCount) {
                d_gateCond.wait(&d_gateMutex);
            }

            gateCount = d_gateCount;
        }

        int control = d_control.relaxedLoad();

        if (BCEP_RUN == control) {
            processJobs();
            control = d_control;
        }

        if (BCEP_DRAIN == control) {
            drainQueue();
        }
        else if (BCEP_SUSPEND == control) {
            continue;
        }
        else {
            BSLS_ASSERT(BCEP_STOP == control);
            return;
        }
    }
}

int bcep_FixedThreadPool::startNewThread()
{
#if defined(BSLS_PLATFORM_OS_UNIX)
    // Block all asynchronous signals.

    sigset_t oldset;
    pthread_sigmask(SIG_BLOCK, &d_blockSet, &oldset);
#endif

    bdef_Function<void(*)()> workerThreadFunc = bdef_MemFnUtil::memFn(
            &bcep_FixedThreadPool::workerThread, this);

    int rc = d_threadGroup.addThread(workerThreadFunc, d_threadAttributes);

#if defined(BSLS_PLATFORM_OS_UNIX)
    // Restore the mask.

    pthread_sigmask(SIG_SETMASK, &oldset, &d_blockSet);
#endif

    return rc;
}

// CREATORS

bcep_FixedThreadPool::bcep_FixedThreadPool(
        const bcemt_Attribute& threadAttributes,
        int                    numThreads,
        int                    maxQueueSize,
        bslma_Allocator       *basicAllocator)
: d_queue(maxQueueSize, basicAllocator)
, d_control(BCEP_STOP)
, d_gateCount(0)
, d_numThreadsReady(0)
, d_threadGroup(basicAllocator)
, d_threadAttributes(threadAttributes)
, d_numThreads(numThreads)
{
    BSLS_ASSERT_OPT(0 != d_numThreads);

    disable();

#if defined(BSLS_PLATFORM_OS_UNIX)
    initBlockSet(&d_blockSet);
#endif
}

bcep_FixedThreadPool::bcep_FixedThreadPool(int              numThreads,
                                           int              maxQueueSize,
                                           bslma_Allocator *basicAllocator)
: d_queue(maxQueueSize, basicAllocator)
, d_control(BCEP_STOP)
, d_gateCount(0)
, d_numThreadsReady(0)
, d_threadGroup(basicAllocator)
, d_numThreads(numThreads)
{
    BSLS_ASSERT_OPT(0 != d_numThreads);

    disable();

#if defined(BSLS_PLATFORM_OS_UNIX)
    initBlockSet(&d_blockSet);
#endif
}

bcep_FixedThreadPool::~bcep_FixedThreadPool()
{
    shutdown();
}

// MANIPULATORS

int bcep_FixedThreadPool::enqueueJob(const Job& functor)
{
    BSLS_ASSERT(functor);

    const int ret = d_queue.pushBack(functor);

    if (0 == ret && d_numThreadsWaiting) {
        // Wake up waiting threads.

        d_queueSemaphore.post();
    }

    return ret;
}

int bcep_FixedThreadPool::tryEnqueueJob(const Job& functor)
{
    BSLS_ASSERT(functor);

    const int ret = d_queue.tryPushBack(functor);

    if (0 == ret && d_numThreadsWaiting) {
        // Wake up waiting threads.

        d_queueSemaphore.post();
    }
    return ret;
}

void bcep_FixedThreadPool::drain()
{
    bcemt_LockGuard<bcemt_Mutex> lock(&d_metaMutex);

    if (BCEP_RUN == d_control.relaxedLoad()) {
        d_control = BCEP_DRAIN;

        // 'interruptWorkerThreads' emits an initial acquire barrier (mutex
        // lock).  Guaranteeing that no instructions in
        // 'interruptWorkerThreads' will be executed before the previous store.

        interruptWorkerThreads();
        waitWorkerThreads();

        d_control = BCEP_RUN;

        // 'releaseWorkerThreads' emits a release barrier so that the worker
        // threads can't return from wait without observing the previous store.

        releaseWorkerThreads();
    }
}

void bcep_FixedThreadPool::shutdown()
{
    bcemt_LockGuard<bcemt_Mutex> lock(&d_metaMutex);

    if (BCEP_RUN == d_control.relaxedLoad()) {
        d_queue.disable();
        d_control = BCEP_STOP;

        // 'interruptWorkerThreads' emits an initial acquire barrier (mutex
        // lock).  Guaranteeing that no instructions in
        // 'interruptWorkerThreads' will be executed before the previous store.

        interruptWorkerThreads();

        d_queue.removeAll();
        d_threadGroup.joinAll();
    }
}

int bcep_FixedThreadPool::start()
{
    bcemt_LockGuard<bcemt_Mutex> lock(&d_metaMutex);

    if (BCEP_STOP != d_control.relaxedLoad()) {
        return 0;
    }

    for (int i = d_threadGroup.numThreads(); i < d_numThreads; ++i)  {
        if (0 != startNewThread()) {

            releaseWorkerThreads();
            d_threadGroup.joinAll();
            return -1;
        }
    }

    waitWorkerThreads();

    d_queue.enable();
    d_control = BCEP_RUN;

    // 'releaseWorkerThreads' emits a release barrier so that the worker
    // threads can't return from wait without observing the previous store.

    releaseWorkerThreads();

    return 0;
}

void bcep_FixedThreadPool::stop()
{
    bcemt_LockGuard<bcemt_Mutex> lock(&d_metaMutex);

    if (BCEP_RUN == d_control.relaxedLoad()) {
        d_queue.disable();
        d_control = BCEP_DRAIN;

        // 'interruptWorkerThreads' has an initial acquire barrier (mutex
        // lock).  Guaranteeing that no instructions in
        // 'interruptWorkerThreads' will be executed before the previous store.

        interruptWorkerThreads();

        waitWorkerThreads();

        d_control = BCEP_STOP;

        // 'releaseWorkerThreads' emits a release barrier so that the worker
        // threads can't return from wait without observing the previous store.

        releaseWorkerThreads();
        d_threadGroup.joinAll();
    }
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
