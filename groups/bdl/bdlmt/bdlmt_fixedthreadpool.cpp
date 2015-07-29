// bdlmt_fixedthreadpool.cpp            -*-C++-*-
#include <bdlmt_fixedthreadpool.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmt_fixedthreadpool_cpp,"$Id$ $CSID$")

#include <bdlmtt_lockguard.h>
#include <bdlmtt_xxxthread.h>

#include <bdlf_function.h>
#include <bdlf_memfn.h>
#include <bdlt_currenttime.h>

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

namespace bdlmt {
                         // --------------------------
                         // class FixedThreadPool
                         // --------------------------

// PRIVATE MANIPULATORS
void FixedThreadPool::processJobs()
{
    while (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                        BCEP_RUN == d_control.loadRelaxed())) {
        Job functor;

        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                              d_queue.tryPopFront(&functor))) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            ++d_numThreadsWaiting;

            if (BCEP_RUN == d_control && d_queue.isEmpty()) {
                d_queueSemaphore.wait();
            }

            d_numThreadsWaiting.addRelaxed(-1);
        }
        else {
            functor();
        }
    }
}

void FixedThreadPool::drainQueue()
{
    while (BCEP_DRAIN == d_control.loadRelaxed()) {
        Job functor;

        const int ret = d_queue.tryPopFront(&functor);
        if (ret) {
            return;
        }

        functor();
    }
}

void FixedThreadPool::waitWorkerThreads()
{
    bdlmtt::LockGuard<bdlmtt::Mutex> lock(&d_gateMutex);

    while (d_numThreadsReady != d_numThreads) {
        d_threadsReadyCond.wait(&d_gateMutex);
    }
}

void FixedThreadPool::releaseWorkerThreads()
{
    bdlmtt::LockGuard<bdlmtt::Mutex> lock(&d_gateMutex);
    d_numThreadsReady = 0;
    ++d_gateCount;
    d_gateCond.broadcast();

    // d_gateMutex.unlock() emits a release barrier.
}

void FixedThreadPool::interruptWorkerThreads()
{
    bdlmtt::LockGuard<bdlmtt::Mutex> lock(&d_gateMutex); // acquire barrier

    int numThreadsWaiting = d_numThreadsWaiting;

    for (int i = 0; i < numThreadsWaiting; ++i) {
        // Wake up waiting threads.

        d_queueSemaphore.post();
    }
}

void FixedThreadPool::workerThread()
{
    int gateCount = d_gateCount;

    while (1) {
        {
            bdlmtt::LockGuard<bdlmtt::Mutex> lock(&d_gateMutex);

            ++d_numThreadsReady;
            d_threadsReadyCond.signal();

            while (gateCount == d_gateCount) {
                d_gateCond.wait(&d_gateMutex);
            }

            gateCount = d_gateCount;
        }

        int control = d_control.loadRelaxed();

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

int FixedThreadPool::startNewThread()
{
#if defined(BSLS_PLATFORM_OS_UNIX)
    // Block all asynchronous signals.

    sigset_t oldset;
    pthread_sigmask(SIG_BLOCK, &d_blockSet, &oldset);
#endif

    bdlf::Function<void(*)()> workerThreadFunc = bdlf::MemFnUtil::memFn(
            &FixedThreadPool::workerThread, this);

    int rc = d_threadGroup.addThread(workerThreadFunc, d_threadAttributes);

#if defined(BSLS_PLATFORM_OS_UNIX)
    // Restore the mask.

    pthread_sigmask(SIG_SETMASK, &oldset, &d_blockSet);
#endif

    return rc;
}

// CREATORS

FixedThreadPool::FixedThreadPool(
        const bcemt_Attribute&  threadAttributes,
        int                     numThreads,
        int                     maxQueueSize,
        bslma::Allocator       *basicAllocator)
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

FixedThreadPool::FixedThreadPool(int               numThreads,
                                           int               maxQueueSize,
                                           bslma::Allocator *basicAllocator)
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

FixedThreadPool::~FixedThreadPool()
{
    shutdown();
}

// MANIPULATORS

int FixedThreadPool::enqueueJob(const Job& functor)
{
    BSLS_ASSERT(functor);

    const int ret = d_queue.pushBack(functor);

    if (0 == ret && d_numThreadsWaiting) {
        // Wake up waiting threads.

        d_queueSemaphore.post();
    }

    return ret;
}

int FixedThreadPool::tryEnqueueJob(const Job& functor)
{
    BSLS_ASSERT(functor);

    const int ret = d_queue.tryPushBack(functor);

    if (0 == ret && d_numThreadsWaiting) {
        // Wake up waiting threads.

        d_queueSemaphore.post();
    }
    return ret;
}

void FixedThreadPool::drain()
{
    bdlmtt::LockGuard<bdlmtt::Mutex> lock(&d_metaMutex);

    if (BCEP_RUN == d_control.loadRelaxed()) {
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

void FixedThreadPool::shutdown()
{
    bdlmtt::LockGuard<bdlmtt::Mutex> lock(&d_metaMutex);

    if (BCEP_RUN == d_control.loadRelaxed()) {
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

int FixedThreadPool::start()
{
    bdlmtt::LockGuard<bdlmtt::Mutex> lock(&d_metaMutex);

    if (BCEP_STOP != d_control.loadRelaxed()) {
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

void FixedThreadPool::stop()
{
    bdlmtt::LockGuard<bdlmtt::Mutex> lock(&d_metaMutex);

    if (BCEP_RUN == d_control.loadRelaxed()) {
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
}  // close package namespace

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
