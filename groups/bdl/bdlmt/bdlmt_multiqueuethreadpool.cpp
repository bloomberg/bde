// bdlmt_multiqueuethreadpool.cpp        -*-C++-*-
#include <bdlmt_multiqueuethreadpool.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmt_multiqueuethreadpool_cpp,"$Id$ $CSID$")

#include <bdlmtt_barrier.h>
#include <bdlmtt_lockguard.h>

#include <bdlf_bind.h>
#include <bdlf_function.h>
#include <bdlf_memfn.h>

#include <bslma_default.h>
#include <bsls_assert.h>

#include <bsl_vector.h>

namespace BloombergLP {

namespace {
    // Internal helper functions.

void noOp() { }
    // This function does nothing.

}  // close unnamed namespace

namespace bdlmt {
                   // -------------------------------------
                   // class MultiQueueThreadPool_Queue
                   // -------------------------------------

// CREATORS
inline
MultiQueueThreadPool_Queue::MultiQueueThreadPool_Queue(
        bslma::Allocator *basicAllocator)
: d_list(basicAllocator)
, d_state(MultiQueueThreadPool_Queue::BCEP_ENQUEUEING_ENABLED)
{
}

inline
MultiQueueThreadPool_Queue::~MultiQueueThreadPool_Queue()
{
}

inline
void MultiQueueThreadPool_Queue::reset()
{
    d_list.clear();
    d_numEnqueued    = 0;
    d_numDequeued    = 0;
    d_numPendingJobs = 0;
    d_state          = BCEP_ENQUEUEING_ENABLED;
}

// MANIPULATORS
inline
MultiQueueThreadPool_Queue::Job
MultiQueueThreadPool_Queue::popFront()
{
    Job functor(d_list.front());
    d_list.pop_front();
    ++d_numDequeued;
    return functor;
}

inline
int MultiQueueThreadPool_Queue::pushBack(const Job& functor)
{
    int rc = 1;
    if (MultiQueueThreadPool_Queue::BCEP_ENQUEUEING_ENABLED == d_state) {
        d_list.push_back(functor);
        rc = 0;
        ++d_numEnqueued;
    }
    return rc;
}

inline
int MultiQueueThreadPool_Queue::pushFront(const Job& functor)
{
    int rc = 1;
    if (MultiQueueThreadPool_Queue::BCEP_ENQUEUEING_BLOCKED != d_state) {
        d_list.push_front(functor);
        rc = 0;
        ++d_numEnqueued;
    }
    return rc;
}

inline
void MultiQueueThreadPool_Queue::block()
{
    d_state = MultiQueueThreadPool_Queue::BCEP_ENQUEUEING_BLOCKED;
}

inline
void MultiQueueThreadPool_Queue::enable()
{
    if (MultiQueueThreadPool_Queue::BCEP_ENQUEUEING_BLOCKED != d_state) {
        d_state = MultiQueueThreadPool_Queue::BCEP_ENQUEUEING_ENABLED;
    }
}

inline
void MultiQueueThreadPool_Queue::disable()
{
    if (MultiQueueThreadPool_Queue::BCEP_ENQUEUEING_BLOCKED != d_state) {
        d_state = MultiQueueThreadPool_Queue::BCEP_ENQUEUEING_DISABLED;
    }
}

inline
void MultiQueueThreadPool_Queue::numProcessedReset(int *numDequeued,
                                                        int *numEnqueued)
{
    // Implementation note:
    // This is not entirely thread-consistent, though thread safe.
    // If in between the two 'swap' operations the number enqueued changes,
    // we can get a slightly inconsistent picture.
    *numDequeued = d_numDequeued.swap(0);
    *numEnqueued = d_numEnqueued.swap(0);
}

// ACCESSORS
inline
int MultiQueueThreadPool_Queue::length() const
{
    return d_list.size();
}

inline
void MultiQueueThreadPool_Queue::numProcessed(int *numDequeued,
                                                   int *numEnqueued) const
{
    *numDequeued = d_numDequeued;
    *numEnqueued = d_numEnqueued;
}

               // --------------------------------------------
               // class MultiQueueThreadPool_QueueContext
               // --------------------------------------------

// CREATORS
inline
MultiQueueThreadPool_QueueContext::
    MultiQueueThreadPool_QueueContext(
        bslma::Allocator *basicAllocator)
: d_queue(basicAllocator)
, d_processingCb(basicAllocator)
, d_destroyFlag(false)
{
}

inline
void MultiQueueThreadPool_QueueContext::reset()
{
   d_queue.reset();
   d_processingCb.clear();
   d_destroyFlag = false;
}

inline
MultiQueueThreadPool_QueueContext::
    ~MultiQueueThreadPool_QueueContext()
{
}

// ACCESSORS
inline
bdlmtt::SpinLock& MultiQueueThreadPool_QueueContext::mutex() const
{
    return d_lock;
}
}  // close package namespace

                      // -------------------------------
                      // class bdlmt::MultiQueueThreadPool
                      // -------------------------------

// TYPES
typedef bdlcc::ObjectCatalogIter<bdlmt::MultiQueueThreadPool_QueueContext*>
                    RegistryIterator;
    // This type is provided for notational convenience when iterating
    // over the queue registry.

typedef bsl::pair<int, bdlmt::MultiQueueThreadPool_QueueContext*>
                    RegistryValue;
    // This type is provided for notational convenience when iterating
    // over the queue registry.

enum {
    // Internal running states.

    STATE_STOPPED,
    STATE_RUNNING
};

namespace bdlmt {
// PRIVATE MANIPULATORS
void MultiQueueThreadPool::createQueueContextCb(void *memory)
{
    BSLS_ASSERT(memory);

    new (memory) MultiQueueThreadPool_QueueContext(d_allocator_p);
}

void MultiQueueThreadPool::deleteQueueCb(
        int                    id,
        const CleanupFunctor&  cleanupFunctor,
        bdlmtt::Barrier         *barrier)
{

    d_registryLock.lockWrite();

    MultiQueueThreadPool_QueueContext *context;
    int rc = d_queueRegistry.remove(id, &context);
    BSLS_ASSERT(0 == rc);

    context->d_destroyFlag = true;

    d_registryLock.unlock();
    --d_numDequeued;

    if (barrier) {
        barrier->wait();
    }
    else if (cleanupFunctor) {
        cleanupFunctor();
    }
}

void MultiQueueThreadPool::processQueueCb(
        MultiQueueThreadPool_QueueContext *context)
{
    BSLS_ASSERT(context);

    bdlmtt::LockGuard<bdlmtt::SpinLock> guard(&context->mutex());

    BSLS_ASSERT(0 < context->d_queue.d_numPendingJobs);

    {
        Job functor(context->d_queue.popFront());
        bdlmtt::SpinLock *mutex = guard.release();
        mutex->unlock();
        ++d_numDequeued;

        functor();
    }

    // Other threads may enqueue new jobs between processing the dequeued
    // functor and re-checking the queue length.

    if (context->d_destroyFlag) {
        --d_numActiveQueues;
        d_queuePool.releaseObject(context);
    }
    else {
        if (0 == --context->d_queue.d_numPendingJobs) {
            --d_numActiveQueues;
        }
        else {
            // Enqueue the processing callback for this queue.
            int status = d_threadPool_p->enqueueJob(context->d_processingCb);
            BSLS_ASSERT(0 == status);
        }
    }
}

int MultiQueueThreadPool::enqueueJobImpl(int        id,
                                              const Job& functor,
                                              int        where)
{
    MultiQueueThreadPool_QueueContext *context;
    int                                     rc = 1;
    d_registryLock.lockRead();
    if (STATE_RUNNING == d_state && 0 == d_queueRegistry.find(id, &context))
    {
        bdlmtt::LockGuard<bdlmtt::SpinLock> guard(&context->mutex());
        int                            status = -1;
        if (BCEP_ENQUEUE_FRONT == where) {
            // Only 'deleteQueue' requests are enqueued to the front of the
            // queue, and these require that the queue is also disabled.
            status = context->d_queue.pushFront(functor);
            context->d_queue.block();
        }
        else {
            status = context->d_queue.pushBack(functor);
        }

        if (0 == status) {
            bdlmtt::SpinLock *mutex = guard.release();
            mutex->unlock();

            if (1 == ++context->d_queue.d_numPendingJobs) {
                ++d_numActiveQueues;

                BSLS_ASSERT(context->d_processingCb);

                // Enqueue the processing callback for this queue.
                status = d_threadPool_p->enqueueJob(context->d_processingCb);
                BSLS_ASSERT(0 == status);
            }
            rc = 0;
        }
    }
    d_registryLock.unlock();
    return rc;
}

// CREATORS
MultiQueueThreadPool::MultiQueueThreadPool(
        const bdlmtt::ThreadAttributes& threadAttributes,
        int                    minThreads,
        int                    maxThreads,
        int                    maxIdleTime,
        bslma::Allocator      *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_threadPoolIsOwned(true)
, d_queuePool(-1, basicAllocator)
, d_queueRegistry(basicAllocator)
, d_state(STATE_STOPPED)
{
    d_threadPool_p = new (*d_allocator_p)
                       ThreadPool(threadAttributes, minThreads,
                                       maxThreads, maxIdleTime, d_allocator_p);
}

MultiQueueThreadPool::MultiQueueThreadPool(
        ThreadPool  *threadPool,
        bslma::Allocator *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_threadPool_p(threadPool)
, d_threadPoolIsOwned(false)
, d_queuePool(-1, basicAllocator)
, d_queueRegistry(basicAllocator)
, d_state(STATE_STOPPED)
{
    BSLS_ASSERT(threadPool);
}

MultiQueueThreadPool::~MultiQueueThreadPool()
{
    shutdown();

    if (d_threadPoolIsOwned) {
        d_allocator_p->deleteObjectRaw(d_threadPool_p);
    }
}

// MANIPULATORS
int MultiQueueThreadPool::createQueue()
{
    int id = 0;
    if (STATE_RUNNING == d_state) {
        MultiQueueThreadPool_QueueContext *context =
                                                    d_queuePool.getObject();

        typedef MultiQueueThreadPool_QueueContext::QueueProcessorCb
                                                              QueueProcessorCb;

        context->d_processingCb
            = QueueProcessorCb(bdlf::BindUtil::bind(
                                  bdlf::MemFnUtil::memFn(
                                    &MultiQueueThreadPool::processQueueCb,
                                    this),
                                  context));

        id = d_queueRegistry.add(context);
    }
    return id;
}

int MultiQueueThreadPool::deleteQueue(
        int                   id,
        const CleanupFunctor& cleanupFunctor)
{
    Job job(bdlf::BindUtil::bind(bdlf::MemFnUtil::memFn(
                                     &MultiQueueThreadPool::deleteQueueCb,
                                     this),
                                id,
                                cleanupFunctor,
                                (bdlmtt::Barrier*)0));

    return enqueueJobImpl(id, job, BCEP_ENQUEUE_FRONT);
}

int MultiQueueThreadPool::deleteQueue(
        int                id)
{
    bdlmtt::Barrier barrier(2);    // block in calling and execution threads

    Job job(bdlf::BindUtil::bind(bdlf::MemFnUtil::memFn(
                                     &MultiQueueThreadPool::deleteQueueCb,
                                     this),
                                id,
                                CleanupFunctor(&noOp),
                                &barrier));

    int rc = enqueueJobImpl(id, job, BCEP_ENQUEUE_FRONT);
    if (0 == rc) {
        barrier.wait();
    }
    return rc;
}

int MultiQueueThreadPool::enableQueue(int id)
{
    MultiQueueThreadPool_QueueContext *context;
    int                                     rc = 1;
    d_registryLock.lockRead();
    if (STATE_RUNNING == d_state
     && 0 == d_queueRegistry.find(id, &context)) {
        context->d_queue.enable();
        rc = 0;
    }
    d_registryLock.unlock();
    return rc;
}

int MultiQueueThreadPool::disableQueue(int id)
{
    MultiQueueThreadPool_QueueContext *context;
    int                                     rc = 1;
    d_registryLock.lockRead();
    if (STATE_RUNNING == d_state
     && 0 == d_queueRegistry.find(id, &context)) {
        context->d_queue.disable();
        rc = 0;
    }
    d_registryLock.unlock();
    return rc;
}

int MultiQueueThreadPool::drainQueue(int id)
{
    MultiQueueThreadPool_QueueContext *context;

    d_registryLock.lockRead();
    int rc = d_queueRegistry.find(id, &context);
    d_registryLock.unlock();
    if (0 == rc) {
        // Wait until the queue is emptied.
        while (0 < context->d_queue.d_numPendingJobs) {
            bdlmtt::ThreadUtil::yield();
        }
    }

    return rc;
}

int MultiQueueThreadPool::start()
{
    bdlmtt::LockGuard<bdlmtt::SpinLock> guard(&d_stateLock);
    if (STATE_RUNNING == d_state) {
        return 0;
    }
    d_registryLock.lockWrite();
    for (RegistryIterator it(d_queueRegistry); it; ++it) {
        RegistryValue rv = it();
        rv.second->d_queue.enable();
    }

    int rc = 0;
    if (d_threadPoolIsOwned) {
        rc = d_threadPool_p->start() ? -1 : 0;
    }
    if (!rc) {
        d_state = STATE_RUNNING;
    }
    d_registryLock.unlock();

    return rc;
}

void MultiQueueThreadPool::drain()
{
    bdlmtt::LockGuard<bdlmtt::SpinLock> guard(&d_stateLock);
    if (STATE_STOPPED == d_state) {
        return;
    }

    BSLS_ASSERT(STATE_RUNNING == d_state);

    // Wait until all queues are emptied.
    while (0 < d_numActiveQueues) {
        bdlmtt::ThreadUtil::yield();
    }

    if (d_threadPoolIsOwned) {
        d_threadPool_p->drain();
        BSLS_ASSERT(0 <  d_threadPool_p->numWaitingThreads());
        BSLS_ASSERT(0 == d_threadPool_p->numActiveThreads());
        d_threadPool_p->start();
    }
}

void MultiQueueThreadPool::stop()
{
    bdlmtt::LockGuard<bdlmtt::SpinLock> guard(&d_stateLock);
    if (STATE_STOPPED == d_state) {
        return;
    }

    d_registryLock.lockWrite();
    d_state = STATE_STOPPED;    // disables all queues
    d_registryLock.unlock();

    // Wait until all queues are emptied.
    while (0 < d_numActiveQueues) {
        bdlmtt::ThreadUtil::yield();
    }

    if (d_threadPoolIsOwned) {
        d_threadPool_p->drain();
        BSLS_ASSERT(0 <  d_threadPool_p->numWaitingThreads());
        BSLS_ASSERT(0 == d_threadPool_p->numActiveThreads());
    }
}

void MultiQueueThreadPool::shutdown()
{
    bdlmtt::LockGuard<bdlmtt::SpinLock> guard(&d_stateLock);
    d_registryLock.lockWrite();
    d_state = STATE_STOPPED;    // disables all queues
    d_registryLock.unlock();

    // Wait until all queues are emptied.
    while (0 < d_numActiveQueues) {
        bdlmtt::ThreadUtil::yield();                                      // SPIN
    }

    // Delete all queues.  Since removing queues requires a write lock
    // on the object catalog, we have to extract the queue IDs, and then
    // delete the queues using the extracted list.  We do not need a lock
    // because there are no active queues, and all other public functions
    // will fail to verify 'd_state'.

    bsl::vector<int> qids;
    qids.reserve(numQueues());
    for (RegistryIterator it(d_queueRegistry); it; ++it) {
        RegistryValue rv = it();
        qids.push_back(rv.first);
    }

    for (bsl::vector<int>::iterator it = qids.begin();
                                                      it != qids.end(); ++it) {
        MultiQueueThreadPool_QueueContext *context;
        int status = d_queueRegistry.remove(*it, &context);
        BSLS_ASSERT(0 == status);
        BSLS_ASSERT(0 == context->d_queue.d_numPendingJobs);
        d_queuePool.releaseObject(context);
    }

    if (d_threadPoolIsOwned) {
        d_threadPool_p->stop();
        BSLS_ASSERT(0 == d_threadPool_p->numWaitingThreads());
        BSLS_ASSERT(0 == d_threadPool_p->numActiveThreads());
    }
}

// ACCESSORS
int MultiQueueThreadPool::numElements(int id) const
{
    MultiQueueThreadPool_QueueContext *context;
    int                                     length = -1;
    d_registryLock.lockRead();
    if (0 == d_queueRegistry.find(id, &context)) {
        // Because 'context->d_queue.d_numPendingJobs' is decremented after
        // each job is dequeued and processed, the queue length is not
        // necessarily the same as the number of pending jobs!

        length = context->d_queue.length();
    }
    d_registryLock.unlock();
    return length;
}
}  // close package namespace

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
