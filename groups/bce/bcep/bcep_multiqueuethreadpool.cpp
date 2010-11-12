// bcep_multiqueuethreadpool.cpp        -*-C++-*-
#include <bcep_multiqueuethreadpool.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcep_multiqueuethreadpool_cpp,"$Id$ $CSID$")

#include <bcemt_barrier.h>
#include <bcemt_lockguard.h>

#include <bdef_bind.h>
#include <bdef_function.h>
#include <bdef_memfn.h>

#include <bslma_default.h>
#include <bsls_assert.h>

#include <bsl_vector.h>

namespace BloombergLP {

namespace {
    // Internal helper functions.

void noOp() { }
    // This function does nothing.

}  // close unnamed namespace

                   // -------------------------------------
                   // class bcep_MultiQueueThreadPool_Queue
                   // -------------------------------------

// CREATORS
inline
bcep_MultiQueueThreadPool_Queue::bcep_MultiQueueThreadPool_Queue(
        bslma_Allocator *basicAllocator)
: d_list(basicAllocator)
, d_state(bcep_MultiQueueThreadPool_Queue::BCEP_ENQUEUEING_ENABLED)
{
}

inline
bcep_MultiQueueThreadPool_Queue::~bcep_MultiQueueThreadPool_Queue()
{
}

inline
void bcep_MultiQueueThreadPool_Queue::reset()
{
    d_list.clear();
    d_numEnqueued    = 0;
    d_numDequeued    = 0;
    d_numPendingJobs = 0;
    d_state          = BCEP_ENQUEUEING_ENABLED;
}

// MANIPULATORS
inline
bcep_MultiQueueThreadPool_Queue::Job
bcep_MultiQueueThreadPool_Queue::popFront()
{
    Job functor(d_list.front());
    d_list.pop_front();
    ++d_numDequeued;
    return functor;
}

inline
int bcep_MultiQueueThreadPool_Queue::pushBack(const Job& functor)
{
    int rc = 1;
    if (bcep_MultiQueueThreadPool_Queue::BCEP_ENQUEUEING_ENABLED == d_state) {
        d_list.push_back(functor);
        rc = 0;
        ++d_numEnqueued;
    }
    return rc;
}

inline
int bcep_MultiQueueThreadPool_Queue::pushFront(const Job& functor)
{
    int rc = 1;
    if (bcep_MultiQueueThreadPool_Queue::BCEP_ENQUEUEING_BLOCKED != d_state) {
        d_list.push_front(functor);
        rc = 0;
        ++d_numEnqueued;
    }
    return rc;
}

inline
void bcep_MultiQueueThreadPool_Queue::block()
{
    d_state = bcep_MultiQueueThreadPool_Queue::BCEP_ENQUEUEING_BLOCKED;
}

inline
void bcep_MultiQueueThreadPool_Queue::enable()
{
    if (bcep_MultiQueueThreadPool_Queue::BCEP_ENQUEUEING_BLOCKED != d_state) {
        d_state = bcep_MultiQueueThreadPool_Queue::BCEP_ENQUEUEING_ENABLED;
    }
}

inline
void bcep_MultiQueueThreadPool_Queue::disable()
{
    if (bcep_MultiQueueThreadPool_Queue::BCEP_ENQUEUEING_BLOCKED != d_state) {
        d_state = bcep_MultiQueueThreadPool_Queue::BCEP_ENQUEUEING_DISABLED;
    }
}

inline
void bcep_MultiQueueThreadPool_Queue::numProcessedReset(int *numDequeued,
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
int bcep_MultiQueueThreadPool_Queue::length() const
{
    return d_list.size();
}

inline
void bcep_MultiQueueThreadPool_Queue::numProcessed(int *numDequeued,
                                                   int *numEnqueued) const
{
    *numDequeued = d_numDequeued;
    *numEnqueued = d_numEnqueued;
}

               // --------------------------------------------
               // class bcep_MultiQueueThreadPool_QueueContext
               // --------------------------------------------

// CREATORS
inline
bcep_MultiQueueThreadPool_QueueContext::
    bcep_MultiQueueThreadPool_QueueContext(
        bslma_Allocator *basicAllocator)
: d_queue(basicAllocator)
, d_processingCb(basicAllocator)
, d_destroyFlag(false)
{
}

inline
void bcep_MultiQueueThreadPool_QueueContext::reset()
{
   d_queue.reset();
   d_processingCb.clear();
   d_destroyFlag = false;
}

inline
bcep_MultiQueueThreadPool_QueueContext::
    ~bcep_MultiQueueThreadPool_QueueContext()
{
}

// ACCESSORS
inline
bces_SpinLock& bcep_MultiQueueThreadPool_QueueContext::mutex() const
{
    return d_lock;
}

                      // -------------------------------
                      // class bcep_MultiQueueThreadPool
                      // -------------------------------

// TYPES
typedef bcec_ObjectCatalogIter<bcep_MultiQueueThreadPool_QueueContext*>
                    RegistryIterator;
    // This type is provided for notational convenience when iterating
    // over the queue registry.

typedef bsl::pair<int, bcep_MultiQueueThreadPool_QueueContext*>
                    RegistryValue;
    // This type is provided for notational convenience when iterating
    // over the queue registry.

enum {
    // Internal running states.

    STATE_STOPPED,
    STATE_RUNNING
};

// PRIVATE MANIPULATORS
void bcep_MultiQueueThreadPool::createQueueContextCb(void *memory)
{
    BSLS_ASSERT(memory);

    new (memory) bcep_MultiQueueThreadPool_QueueContext(d_allocator_p);
}

void bcep_MultiQueueThreadPool::deleteQueueCb(
        int                    id,
        const CleanupFunctor&  cleanupFunctor,
        bcemt_Barrier         *barrier)
{

    d_registryLock.lockWrite();

    bcep_MultiQueueThreadPool_QueueContext *context;
    int rc = d_queueRegistry.remove(id, &context);
    BSLS_ASSERT(0 == rc);

    context->d_destroyFlag = true;

    d_registryLock.unlock();

    if (barrier) {
        barrier->wait();
    }
    else if (cleanupFunctor) {
        cleanupFunctor();
    }
}

void bcep_MultiQueueThreadPool::processQueueCb(
        bcep_MultiQueueThreadPool_QueueContext *context)
{
    BSLS_ASSERT(context);

    bcemt_LockGuard<bces_SpinLock> guard(&context->mutex());

    BSLS_ASSERT(0 < context->d_queue.d_numPendingJobs);

    {
        Job functor(context->d_queue.popFront());
        bces_SpinLock *mutex = guard.release();
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

int bcep_MultiQueueThreadPool::enqueueJobImpl(int        id,
                                              const Job& functor,
                                              int        where)
{
    bcep_MultiQueueThreadPool_QueueContext *context;
    int                                     rc = 1;
    d_registryLock.lockRead();
    if (STATE_RUNNING == d_state && 0 == d_queueRegistry.find(id, &context))
    {
        bcemt_LockGuard<bces_SpinLock> guard(&context->mutex());
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
            bces_SpinLock *mutex = guard.release();
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
bcep_MultiQueueThreadPool::bcep_MultiQueueThreadPool(
        const bcemt_Attribute& threadAttributes,
        int                    minThreads,
        int                    maxThreads,
        int                    maxIdleTime,
        bslma_Allocator       *basicAllocator)
: d_allocator_p(bslma_Default::allocator(basicAllocator))
, d_threadPoolIsOwned(true)
, d_queuePool(-1, basicAllocator)
, d_queueRegistry(basicAllocator)
, d_state(STATE_STOPPED)
{
    d_threadPool_p = new (*d_allocator_p)
                       bcep_ThreadPool(threadAttributes, minThreads,
                                       maxThreads, maxIdleTime, d_allocator_p);
}

bcep_MultiQueueThreadPool::bcep_MultiQueueThreadPool(
        bcep_ThreadPool *threadPool,
        bslma_Allocator *basicAllocator)
: d_allocator_p(bslma_Default::allocator(basicAllocator))
, d_threadPool_p(threadPool)
, d_threadPoolIsOwned(false)
, d_queuePool(-1, basicAllocator)
, d_queueRegistry(basicAllocator)
, d_state(STATE_STOPPED)
{
    BSLS_ASSERT(threadPool);
}

bcep_MultiQueueThreadPool::~bcep_MultiQueueThreadPool()
{
    shutdown();

    if (d_threadPoolIsOwned) {
        d_allocator_p->deleteObjectRaw(d_threadPool_p);
    }
}

// MANIPULATORS
int bcep_MultiQueueThreadPool::createQueue()
{
    int id = 0;
    if (STATE_RUNNING == d_state) {
        bcep_MultiQueueThreadPool_QueueContext *context =
                                                    d_queuePool.getObject();

        typedef bcep_MultiQueueThreadPool_QueueContext::QueueProcessorCb
                                                              QueueProcessorCb;

        context->d_processingCb
            = QueueProcessorCb(bdef_BindUtil::bind(
                                  bdef_MemFnUtil::memFn(
                                    &bcep_MultiQueueThreadPool::processQueueCb,
                                    this),
                                  context));

        id = d_queueRegistry.add(context);
    }
    return id;
}

int bcep_MultiQueueThreadPool::deleteQueue(
        int                   id,
        const CleanupFunctor& cleanupFunctor)
{
    Job job(bdef_BindUtil::bind(bdef_MemFnUtil::memFn(
                                     &bcep_MultiQueueThreadPool::deleteQueueCb,
                                     this),
                                id,
                                cleanupFunctor,
                                (bcemt_Barrier*)0));

    return enqueueJobImpl(id, job, BCEP_ENQUEUE_FRONT);
}

int bcep_MultiQueueThreadPool::deleteQueue(
        int                id)
{
    bcemt_Barrier barrier(2);    // block in calling and execution threads

    Job job(bdef_BindUtil::bind(bdef_MemFnUtil::memFn(
                                     &bcep_MultiQueueThreadPool::deleteQueueCb,
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

int bcep_MultiQueueThreadPool::enableQueue(int id)
{
    bcep_MultiQueueThreadPool_QueueContext *context;
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

int bcep_MultiQueueThreadPool::disableQueue(int id)
{
    bcep_MultiQueueThreadPool_QueueContext *context;
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

int bcep_MultiQueueThreadPool::drainQueue(int id)
{
    bcep_MultiQueueThreadPool_QueueContext *context;

    d_registryLock.lockRead();
    int rc = d_queueRegistry.find(id, &context);
    d_registryLock.unlock();
    if (0 == rc) {
        // Wait until the queue is emptied.
        while (0 < context->d_queue.d_numPendingJobs) {
            bcemt_ThreadUtil::yield();
        }
    }

    return rc;
}

void bcep_MultiQueueThreadPool::start()
{
    bcemt_LockGuard<bces_SpinLock>(&this->d_stateLock);
    if (STATE_RUNNING == d_state) {
        return;
    }
    d_registryLock.lockWrite();
    for (RegistryIterator it(d_queueRegistry); it; ++it) {
        RegistryValue rv = it();
        rv.second->d_queue.enable();
    }

    if (d_threadPoolIsOwned) {
        d_threadPool_p->start();
    }
    d_state = STATE_RUNNING;
    d_registryLock.unlock();
}

void bcep_MultiQueueThreadPool::drain()
{
    bcemt_LockGuard<bces_SpinLock> guard(&this->d_stateLock);
    if (STATE_STOPPED == d_state) {
        return;
    }

    BSLS_ASSERT(STATE_RUNNING == d_state);

    // Wait until all queues are emptied.
    while (0 < d_numActiveQueues) {
        bcemt_ThreadUtil::yield();
    }

    if (d_threadPoolIsOwned) {
        d_threadPool_p->drain();
        BSLS_ASSERT(0 <  d_threadPool_p->numWaitingThreads());
        BSLS_ASSERT(0 == d_threadPool_p->numActiveThreads());
        d_threadPool_p->start();
    }
}

void bcep_MultiQueueThreadPool::stop()
{
    bcemt_LockGuard<bces_SpinLock> guard(&this->d_stateLock);
    if (STATE_STOPPED == d_state) {
        return;
    }

    d_registryLock.lockWrite();
    d_state = STATE_STOPPED;    // disables all queues
    d_registryLock.unlock();

    // Wait until all queues are emptied.
    while (0 < d_numActiveQueues) {
        bcemt_ThreadUtil::yield();
    }

    if (d_threadPoolIsOwned) {
        d_threadPool_p->drain();
        BSLS_ASSERT(0 <  d_threadPool_p->numWaitingThreads());
        BSLS_ASSERT(0 == d_threadPool_p->numActiveThreads());
    }
}

void bcep_MultiQueueThreadPool::shutdown()
{
    bcemt_LockGuard<bces_SpinLock> guard(&this->d_stateLock);
    d_registryLock.lockWrite();
    d_state = STATE_STOPPED;    // disables all queues
    d_registryLock.unlock();

    // Wait until all queues are emptied.
    while (0 < d_numActiveQueues) {
        bcemt_ThreadUtil::yield();                                      // SPIN
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
        bcep_MultiQueueThreadPool_QueueContext *context;
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
int bcep_MultiQueueThreadPool::numElements(int id) const
{
    bcep_MultiQueueThreadPool_QueueContext *context;
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

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
