// bdlmt_multiqueuethreadpool.cpp                                     -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlmt_multiqueuethreadpool.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmt_multiqueuethreadpool_cpp,"$Id$ $CSID$")

#include <bdlf_bind.h>
#include <bdlf_memfn.h>

#include <bslmt_barrier.h>
#include <bslmt_lockguard.h>
#include <bslmt_readlockguard.h>
#include <bslmt_semaphore.h>
#include <bslmt_threadutil.h>

#include <bslma_default.h>

#include <bsls_assert.h>

#include <bsl_memory.h>
#include <bsl_vector.h>

namespace BloombergLP {

namespace {
    // Internal helper functions.

void noOp() { }
    // This function does nothing.

void setIntAndPost(bsls::AtomicInt  *intValue,
                   int               value,
                   bslmt::Semaphore *semaphore) {
    // Set the specified 'intValue' to the specified 'value' and post on the
    // specified 'semaphore'.
    *intValue = value;
    semaphore->post();
}
    

}  // close unnamed namespace

namespace bdlmt {
                      // --------------------------------
                      // class MultiQueueThreadPool_Queue
                      // --------------------------------

// CREATORS
inline
MultiQueueThreadPool_Queue::MultiQueueThreadPool_Queue(
                                              bslma::Allocator *basicAllocator)
: d_list(basicAllocator)
, d_state(MultiQueueThreadPool_Queue::e_ENQUEUING_ENABLED)
, d_pauseState(MultiQueueThreadPool_Queue::e_RUNNING)
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
    d_state.storeRelaxed(e_ENQUEUING_ENABLED);
    d_pauseState.storeRelaxed(e_RUNNING);
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
    if (MultiQueueThreadPool_Queue::e_ENQUEUING_ENABLED ==
        d_state.loadRelaxed()) {
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
    if (MultiQueueThreadPool_Queue::e_ENQUEUING_ENABLED ==
        d_state.loadRelaxed()) {
        d_list.push_front(functor);
        rc = 0;
        ++d_numEnqueued;
    }
    return rc;
}

inline
int MultiQueueThreadPool_Queue::forceFront(const Job& functor)
{
    int rc = 1;
    if (MultiQueueThreadPool_Queue::e_DELETING != d_state.loadRelaxed()) {
        d_list.push_front(functor);
        rc = 0;
        ++d_numEnqueued;
    }
    return rc;
}

inline
void MultiQueueThreadPool_Queue::prepareForDeletion()
{
    d_state.storeRelaxed(MultiQueueThreadPool_Queue::e_DELETING);
}

inline
void MultiQueueThreadPool_Queue::enable()
{
    if (MultiQueueThreadPool_Queue::e_DELETING != d_state.loadRelaxed()) {
        d_state.storeRelaxed(MultiQueueThreadPool_Queue::e_ENQUEUING_ENABLED);
    }
}

inline
void MultiQueueThreadPool_Queue::disable()
{
    if (MultiQueueThreadPool_Queue::e_DELETING != d_state.loadRelaxed()) {
        d_state.storeRelaxed(MultiQueueThreadPool_Queue::e_ENQUEUING_DISABLED);
    }
}

inline
bool MultiQueueThreadPool_Queue::isEnabled() const
{
    int state = d_state.loadRelaxed();
    return (e_DELETING != state && e_ENQUEUING_ENABLED == state);
}

inline
void MultiQueueThreadPool_Queue::numProcessedReset(int *numDequeued,
                                                   int *numEnqueued)
{
    // Implementation note: This is not entirely thread-consistent, though
    // thread safe.  If in between the two 'swap' operations the number
    // enqueued changes, we can get a slightly inconsistent picture.
    *numDequeued = d_numDequeued.swap(0);
    *numEnqueued = d_numEnqueued.swap(0);
}

// ACCESSORS
inline
int MultiQueueThreadPool_Queue::length() const
{
    return static_cast<int>(d_list.size());
}

inline
void MultiQueueThreadPool_Queue::numProcessed(int *numDequeued,
                                              int *numEnqueued) const
{
    *numDequeued = d_numDequeued;
    *numEnqueued = d_numEnqueued;
}

                  // ---------------------------------------
                  // class MultiQueueThreadPool_QueueContext
                  // ---------------------------------------

// CREATORS
inline
MultiQueueThreadPool_QueueContext::MultiQueueThreadPool_QueueContext(
                                              bslma::Allocator *basicAllocator)
: d_queue(basicAllocator)
, d_isChanging(false)
, d_processingCb(bsl::allocator_arg_t(),
                 bsl::allocator<QueueProcessorCb>(basicAllocator))
, d_destroyFlag(false)
, d_processor(bslmt::ThreadUtil::invalidHandle())
{
    d_lock.initialize();
}

inline
void MultiQueueThreadPool_QueueContext::reset()
{
   d_queue.reset();
   d_processingCb = QueueProcessorCb();
   d_destroyFlag = false;
   d_isChanging = false;
   d_processor = bslmt::ThreadUtil::invalidHandle();
}

inline
MultiQueueThreadPool_QueueContext::
    ~MultiQueueThreadPool_QueueContext()
{
}

// ACCESSORS
inline
bslmt::QLock& MultiQueueThreadPool_QueueContext::mutex() const
{
    return d_lock;
}
}  // close package namespace

                     // ---------------------------------
                     // class bdlmt::MultiQueueThreadPool
                     // ---------------------------------

// TYPES
typedef bdlcc::ObjectCatalogIter<bdlmt::MultiQueueThreadPool_QueueContext*>
                    RegistryIterator;
    // This type is provided for notational convenience when iterating over the
    // queue registry.

typedef bsl::pair<int, bdlmt::MultiQueueThreadPool_QueueContext*>
                    RegistryValue;
    // This type is provided for notational convenience when iterating over the
    // queue registry.

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

void MultiQueueThreadPool::deleteQueueCb(int                    id,
                                         const CleanupFunctor&  cleanupFunctor,
                                         bslmt::Barrier        *barrier)
{

    d_registryLock.lockWrite();

    MultiQueueThreadPool_QueueContext *context = 0;
    int rc = d_queueRegistry.remove(id, &context);
    BSLS_ASSERT(0 == rc);
    (void)rc;

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

    bslmt::QLockGuard guard(&context->mutex());
    context->d_processor = bslmt::ThreadUtil::self();
    BSLS_ASSERT(0 < context->d_queue.d_numPendingJobs);

    {
        Job functor(context->d_queue.popFront());
        guard.unlock();
        ++d_numDequeued;

        functor();
        context->d_processor = bslmt::ThreadUtil::invalidHandle();
    }

    // Other threads may enqueue new jobs between processing the dequeued
    // functor and re-checking the queue length.

    if (context->d_destroyFlag) {
        --d_numActiveQueues;
        d_queuePool.releaseObject(context);
    }
    else {
        int pauseState = context->d_queue.d_pauseState.loadRelaxed();

        // If the queue is pausing, mark it paused now. We will not resubmit
        // the processing callback.
        
        if (MultiQueueThreadPool_Queue::e_PAUSING == pauseState) {
            pauseState = context->d_queue.d_pauseState =
                MultiQueueThreadPool_Queue::e_PAUSED;
        }
            
        // Reduce the number of pending jobs; if it reaches 0, or the 
        // queue was paused during the execution of the user callback, mark
        // the queue deactivated.   Otherwise, re-enqueue the processing
        // callback.
        
        if (0 == --context->d_queue.d_numPendingJobs ||
            MultiQueueThreadPool_Queue::e_PAUSED == pauseState) {
            --d_numActiveQueues;
        }
        else {
            // Enqueue the processing callback for this queue.
            int status = d_threadPool_p->enqueueJob(context->d_processingCb);
            BSLS_ASSERT(0 == status);
            (void)status;
        }
    }
}

int MultiQueueThreadPool::enqueueJobImpl(int          id,
                                         const Job   &functor,
                                         EnqueueType  type)
{
    MultiQueueThreadPool_QueueContext *context;
    int                                     rc = 1;
    bslmt::ReadLockGuard<bslmt::RWMutex> regGuard(&d_registryLock);
    if (STATE_RUNNING == d_state.loadRelaxed() &&
        0             == d_queueRegistry.find(id, &context)) {
        bslmt::QLockGuard guard(&context->mutex());
        int               status = -1;
        switch (type) {
          case e_FRONT: {
              status = context->d_queue.pushFront(functor);
              break;
          }
          case e_FRONT_FORCE: {
              status = context->d_queue.forceFront(functor);
              break;
          }
          case e_BACK: {
              status = context->d_queue.pushBack(functor);
              break;
          }
          case e_DELETION: {
              status = context->d_queue.forceFront(functor);
              context->d_queue.prepareForDeletion();
              break;
          }
        }

        if (0 == status) {
            bool isPaused = MultiQueueThreadPool_Queue::e_RUNNING !=
                context->d_queue.d_pauseState.loadRelaxed();
            if (isPaused && e_DELETION != type) {
                // if paused, and not deleting, increment the
                // number of pending jobs but do not activate the queue
                ++context->d_queue.d_numPendingJobs;
            } else {
                guard.unlock();
            
                if (1 == ++context->d_queue.d_numPendingJobs) {
                    ++d_numActiveQueues;
                    
                    BSLS_ASSERT(context->d_processingCb);
                    
                    // Enqueue the processing callback for this queue.
                    status = 
                        d_threadPool_p->enqueueJob(context->d_processingCb);
                    BSLS_ASSERT(0 == status);
                }
            }
            rc = 0;
        }
    }
    return rc;
}

// CREATORS
MultiQueueThreadPool::MultiQueueThreadPool(
                              const bslmt::ThreadAttributes&  threadAttributes,
                              int                             minThreads,
                              int                             maxThreads,
                              int                             maxIdleTime,
                              bslma::Allocator               *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_threadPoolIsOwned(true)
, d_queuePool(-1, basicAllocator)
, d_queueRegistry(basicAllocator)
, d_state(STATE_STOPPED)
{
    d_threadPool_p = new (*d_allocator_p) ThreadPool(threadAttributes,
                                                     minThreads,
                                                     maxThreads,
                                                     maxIdleTime,
                                                     d_allocator_p);
}

MultiQueueThreadPool::MultiQueueThreadPool(ThreadPool       *threadPool,
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
    if (STATE_RUNNING == d_state.loadRelaxed()) {
        MultiQueueThreadPool_QueueContext *context = d_queuePool.getObject();

        context->d_processingCb = bdlf::BindUtil::bind(
                                      &MultiQueueThreadPool::processQueueCb,
                                      this,
                                      context);

        id = d_queueRegistry.add(context);
    }
    return id;
}

int MultiQueueThreadPool::deleteQueue(int                   id,
                                      const CleanupFunctor& cleanupFunctor)
{
    Job job = bdlf::BindUtil::bind(&MultiQueueThreadPool::deleteQueueCb,
                                   this,
                                   id,
                                   cleanupFunctor,
                                   (bslmt::Barrier *)0);

    return enqueueJobImpl(id, job, e_DELETION);
}

int MultiQueueThreadPool::deleteQueue(int id)
{
    bslmt::Barrier barrier(2);    // block in calling and execution threads

    Job job = bdlf::BindUtil::bind(&MultiQueueThreadPool::deleteQueueCb,
                                   this,
                                   id,
                                   CleanupFunctor(&noOp),
                                   &barrier);

    int rc = enqueueJobImpl(id, job, e_DELETION);
    if (0 == rc) {
        barrier.wait();
    }
    return rc;
}

int MultiQueueThreadPool::enableQueue(int id)
{
    MultiQueueThreadPool_QueueContext *context;
    int                                rc = 1;
    bslmt::ReadLockGuard<bslmt::RWMutex> regGuard(&d_registryLock);
    if (STATE_RUNNING == d_state.loadRelaxed() &&
        0 == d_queueRegistry.find(id, &context)) {
        context->d_queue.enable();
        rc = 0;
    }
    return rc;
}

int MultiQueueThreadPool::disableQueue(int id)
{
    MultiQueueThreadPool_QueueContext *context;
    int                                     rc = 1;
    bslmt::ReadLockGuard<bslmt::RWMutex> regGuard(&d_registryLock);
    if (STATE_RUNNING == d_state.loadRelaxed() &&
        0 == d_queueRegistry.find(id, &context)) {
        context->d_queue.disable();
        rc = 0;
    }
    return rc;
}

bool MultiQueueThreadPool::isEnabled(int id) const
{
    MultiQueueThreadPool_QueueContext *context = 0;
    bslmt::ReadLockGuard<bslmt::RWMutex> regGuard(&d_registryLock);
    return (STATE_RUNNING == d_state.loadRelaxed() &&
                                    0 == d_queueRegistry.find(id, &context) &&
                                                context->d_queue.isEnabled());
}

int MultiQueueThreadPool::drainQueue(int id)
{
    MultiQueueThreadPool_QueueContext *context;

    bslmt::ReadLockGuard<bslmt::RWMutex> regGuard(&d_registryLock);
    int rc = d_queueRegistry.find(id, &context);
    regGuard.release()->unlock();
    if (0 == rc) {
        // Wait until the queue is emptied.
        while (0 < context->d_queue.d_numPendingJobs) {
            bslmt::ThreadUtil::yield();
        }
    }

    return rc;
}

int MultiQueueThreadPool::start()
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_stateLock);
    if (STATE_RUNNING == d_state.loadRelaxed()) {
        return 0;                                                     // RETURN
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
        d_state.storeRelaxed(STATE_RUNNING);
    }
    d_registryLock.unlock();

    return rc;
}

void MultiQueueThreadPool::drain()
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_stateLock);
    if (STATE_STOPPED == d_state.loadRelaxed()) {
        return;
    }

    BSLS_ASSERT(STATE_RUNNING == d_state.loadRelaxed());

    // Wait until all queues are emptied.
    while (0 < d_numActiveQueues) {
        bslmt::ThreadUtil::yield();
    }

    if (d_threadPoolIsOwned) {
        d_threadPool_p->drain();
        BSLS_ASSERT(0 <  d_threadPool_p->numWaitingThreads());
        BSLS_ASSERT(0 == d_threadPool_p->numActiveThreads());
        d_threadPool_p->start();
    }
}

int MultiQueueThreadPool::changePauseState(int id, bool paused)
{
    MultiQueueThreadPool_QueueContext *context;
    bslmt::ReadLockGuard<bslmt::RWMutex> regGuard(&d_registryLock);
    if (STATE_RUNNING != d_state.loadRelaxed() ||
        0 != d_queueRegistry.find(id, &context)) {
        return 1;
    }
    
    bslmt::QLockGuard guard(&context->mutex());

    bool isPaused = MultiQueueThreadPool_Queue::e_RUNNING !=
        context->d_queue.d_pauseState.loadRelaxed();
    if (paused == isPaused || context->d_isChanging) {
        // Already in the intended state, or another thread is in the process
        // of changing state. Either represents a failure under the
        // contract.
        return 1;
    }

    if (bslmt::ThreadUtil::areEqual(bslmt::ThreadUtil::self(),
                                    context->d_processor)) {
        // Invoked from a job within the queue. We can simply set 
        // the paused flag and it will be observed by the processing
        // function after this job completes.
        context->d_queue.d_pauseState = paused
            ? MultiQueueThreadPool_Queue::e_PAUSED
            : MultiQueueThreadPool_Queue::e_RUNNING;
        return 0;
    } 

    // The state change needs to happen in the queue processing thread.
    // In the case of pausing, it needs to happen there because the contract
    // requires it; in the case of resuming, it needs to happen there to avoid
    // racing with pausing.  Set a flag to indicating we are waiting for a state
    // change.
    context->d_isChanging = true;
    
    bslmt::Semaphore semaphore;
    int newState = paused
        ? MultiQueueThreadPool_Queue::e_PAUSING
        : MultiQueueThreadPool_Queue::e_RUNNING;
    Job job = bdlf::BindUtil::bind(&setIntAndPost, 
                                   &context->d_queue.d_pauseState,
                                   newState, 
                                   &semaphore);
    if (paused) {
        // enqueueJobImpl locks the registry and context locks, so invoke while
        // both are unlocked
        guard.unlock();
        bslmt::ReadLockGuardUnlock<bslmt::RWMutex> regUnlock(&d_registryLock);
        if (0 != enqueueJobImpl(id, job, e_FRONT_FORCE)) {
            // queue was deleted (no need to reset d_isChanging flag, this queue
            // is in a terminal state)
            return 1;
        }

        semaphore.wait();
    } else {
        // we can't use enqueueJobImpl, because we need to increment the number
        // of jobs but unconditionally re-enqueue the processing callback
        // (to effect the resumption) so we need to do it manually.  At this
        // point we still hold the registry and context locks.
        context->d_queue.forceFront(job);
        ++context->d_queue.d_numPendingJobs;
        ++d_numActiveQueues;
        
        int status = 
            d_threadPool_p->enqueueJob(context->d_processingCb);
        BSLS_ASSERT(0 == status);

        // now unlock the registry and context locks to wait on the semaphore.
        guard.unlock();
        bslmt::ReadLockGuardUnlock<bslmt::RWMutex> regUnlock(&d_registryLock);
        semaphore.wait();
    }        

    // We have released the lock protecting 'd_queueRegistry' so the queue may
    // have been deleted and 'context' must be re-found.  Note we hold a read
    // lock on d_registryLock here.
    
    if (0 != d_queueRegistry.find(id, &context)) {
        // queue was deleted from under us
        return 1;
    }

    bslmt::QLockGuard stateChangeGuard(&context->mutex());

    // If pausing, additionally wait for the state to change from e_PAUSING to
    // e_PAUSED; this prevents another thread from enqueing a job before
    // processQueueCb() has decided not to resubmit the processing
    // callback.  It is safe to spin here as we're only waiting for the
    // few instructions in processQueueCb() between posting the semaphore
    // and updating the pause state.
    while (paused && MultiQueueThreadPool_Queue::e_PAUSED !=
           context->d_queue.d_pauseState) ;
    
    context->d_isChanging = false;
    return 0;
}

void MultiQueueThreadPool::stop()
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_stateLock);
    if (STATE_STOPPED == d_state.loadRelaxed()) {
        return;                                                       // RETURN
    }

    d_registryLock.lockWrite();
    d_state.storeRelaxed(STATE_STOPPED);    // disables all queues
    d_registryLock.unlock();

    // Wait until all queues are emptied.
    while (0 < d_numActiveQueues) {
        bslmt::ThreadUtil::yield();
    }

    if (d_threadPoolIsOwned) {
        d_threadPool_p->drain();
        BSLS_ASSERT(0 <  d_threadPool_p->numWaitingThreads());
        BSLS_ASSERT(0 == d_threadPool_p->numActiveThreads());
    }
}

void MultiQueueThreadPool::shutdown()
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_stateLock);
    d_registryLock.lockWrite();
    d_state.storeRelaxed(STATE_STOPPED);    // disables all queues
    d_registryLock.unlock();

    // Wait until all queues are emptied.
    while (0 < d_numActiveQueues) {
        bslmt::ThreadUtil::yield();                                     // SPIN
    }

    // Delete all queues.  Since removing queues requires a write lock on the
    // object catalog, we have to extract the queue IDs, and then delete the
    // queues using the extracted list.  We do not need a lock because there
    // are no active queues, and all other public functions will fail to verify
    // 'd_state'.

    bsl::vector<int> qids;
    qids.reserve(numQueues());
    for (RegistryIterator it(d_queueRegistry); it; ++it) {
        RegistryValue rv = it();
        qids.push_back(rv.first);
    }

    for (bsl::vector<int>::iterator it = qids.begin(); it != qids.end();
                                                                        ++it) {
        MultiQueueThreadPool_QueueContext *context = 0;
        int status = d_queueRegistry.remove(*it, &context);
        (void)status; BSLS_ASSERT(0 == status);
        BSLS_ASSERT(0 == context->d_queue.d_numPendingJobs ||
                    context->d_queue.d_pauseState.loadRelaxed());
        d_queuePool.releaseObject(context);
    }

    if (d_threadPoolIsOwned) {
        d_threadPool_p->stop();
        BSLS_ASSERT(0 == d_threadPool_p->numWaitingThreads());
        BSLS_ASSERT(0 == d_threadPool_p->numActiveThreads());
    }
}

// ACCESSORS
bool MultiQueueThreadPool::isPaused(int id) const
{
    MultiQueueThreadPool_QueueContext *context;

    bslmt::ReadLockGuard<bslmt::RWMutex> regGuard(&d_registryLock);
    if (0 == d_queueRegistry.find(id, &context)) {
        bslmt::QLockGuard guard(&context->mutex());
        return context->d_queue.d_pauseState.loadRelaxed();          // RETURN
    }
    return false;
}

int MultiQueueThreadPool::numElements(int id) const
{
    MultiQueueThreadPool_QueueContext *context;

    bslmt::ReadLockGuard<bslmt::RWMutex> regGuard(&d_registryLock);
    if (0 == d_queueRegistry.find(id, &context)) {
        return context->d_queue.length();                            // RETURN
    }
    return -1;
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
