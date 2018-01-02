// bdlmt_multiqueuethreadpool.cpp                                     -*-C++-*-

#include <bdlmt_multiqueuethreadpool.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmt_multiqueuethreadpool_cpp,"$Id$ $CSID$")

#include <bdlf_bind.h>
#include <bdlf_memfn.h>

#include <bslmt_latch.h>
#include <bslmt_lockguard.h>
#include <bslmt_threadutil.h>
#include <bslmt_writelockguard.h>

#include <bslma_default.h>

#include <bsls_assert.h>

#include <bsl_memory.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace {

void noOp() { }
    // This function does nothing.

void createMultiQueueThreadPool_Queue(
                             void                        *arena,
                             bslma::Allocator            *allocator,
                             bdlmt::MultiQueueThreadPool *multiQueueThreadPool)
    // Construct at the specified 'arena' a 'bdlmt::MultiQueueThreadPool_Queue'
    // initialized with the specified 'multiQueueThreadPool' and using the
    // specified 'allocator'.  Note that this function may be used as the
    // function required for the non-default object creation in
    // 'bdlcc::ObjectPool'.
{
    new (arena) bdlmt::MultiQueueThreadPool_Queue(multiQueueThreadPool,
                                                  allocator);
}

}  // close unnamed namespace

namespace bdlmt {

                     // --------------------------------
                     // class MultiQueueThreadPool_Queue
                     // --------------------------------

// CREATORS
MultiQueueThreadPool_Queue::MultiQueueThreadPool_Queue(
                                    MultiQueueThreadPool *multiQueueThreadPool,
                                    bslma::Allocator     *basicAllocator)
: d_multiQueueThreadPool_p(multiQueueThreadPool)
, d_list(basicAllocator)
, d_enqueueState(e_ENQUEUING_ENABLED)
, d_runState(e_NOT_SCHEDULED)
, d_lock()
, d_pauseBlock()
, d_pauseCount(0)
, d_processingCb(bdlf::BindUtil::bind(
                                     &MultiQueueThreadPool_Queue::executeFront,
                                     this))
, d_processor(bslmt::ThreadUtil::invalidHandle())
{
}

MultiQueueThreadPool_Queue::~MultiQueueThreadPool_Queue()
{
}

// MANIPULATORS
int MultiQueueThreadPool_Queue::enable()
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_lock);

    if (e_DELETING == d_enqueueState) {
        return 1;                                                     // RETURN
    }

    d_enqueueState = e_ENQUEUING_ENABLED;
    return 0;
}

int MultiQueueThreadPool_Queue::disable()
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_lock);

    if (e_DELETING == d_enqueueState) {
        return 1;                                                     // RETURN
    }

    d_enqueueState = e_ENQUEUING_DISABLED;
    return 0;
}

int MultiQueueThreadPool_Queue::pause()
{
    {
        bslmt::LockGuard<bslmt::Mutex> guard(&d_lock);

        if (   e_DELETING == d_enqueueState
            || e_PAUSING  == d_runState
            || e_PAUSED   == d_runState) {
            return 1;                                                 // RETURN
        }

        if (e_NOT_SCHEDULED == d_runState) {
            d_runState = e_PAUSED;

            return 0;                                                 // RETURN
        }

        d_runState = e_PAUSING;

        if (bslmt::ThreadUtil::self() == d_processor) {
            return 0;                                                 // RETURN
        }

        ++d_pauseCount;
    }

    d_pauseBlock.wait();

    return 0;
}

void MultiQueueThreadPool_Queue::executeFront()
{
    ++d_multiQueueThreadPool_p->d_numDequeued;

    Job functor;
    {
        bslmt::LockGuard<bslmt::Mutex> guard(&d_lock);

        BSLS_ASSERT(!d_list.empty());

        if (e_PAUSING == d_runState) {
            setPaused();

            return;                                                   // RETURN
        }

        functor = d_list.front();
        d_list.pop_front();
        d_processor = bslmt::ThreadUtil::self();
    }

    // Note that the appropriate 'd_runState' is a bit ambigoues at this point.
    // Since there is nothing scheduled in the thread pool, the state should
    // arguably be 'e_NOT_SCHEDULED'.  However, allowing work to be scheduled
    // during the execution of the 'functor' would be a bug.  Instead of
    // creating a new state to reflect this situation while the 'functor' is
    // executing, we leave 'd_runState' as 'e_SCHEDULED'.

    functor();

    // Note that 'pause' might be called while executing the functor since no
    // lock is held.

    {
        bslmt::LockGuard<bslmt::Mutex> guard(&d_lock);

        BSLS_ASSERT(bslmt::ThreadUtil::self() == d_processor);

        d_processor = bslmt::ThreadUtil::invalidHandle();

        // As per the above, at this point 'e_SCHEDULED' does not imply there
        // is a job queued in the thread pool.

        if (e_SCHEDULED == d_runState) {
            if (!d_list.empty()) {
                int status = d_multiQueueThreadPool_p->d_threadPool_p->
                                                    enqueueJob(d_processingCb);

                BSLS_ASSERT(0 == status);  (void)status;
            }
            else {
                d_runState = e_NOT_SCHEDULED;

                --d_multiQueueThreadPool_p->d_numActiveQueues;
            }
        }
        else {
            setPaused();
        }
    }
}

bool MultiQueueThreadPool_Queue::prepareForDeletion(
                                                 const Job& functor,
                                                 const Job& functorIfProcessor)
{
    // Note that the queue is actually deleted by the thread pool while
    // executing the supplied 'functor' (which is
    // 'MultiQueueThreadPool::deleteQueueCb' bound with the required
    // arguments).

    bslmt::LockGuard<bslmt::Mutex> guard(&d_lock);

    d_enqueueState = e_DELETING;

    if (e_NOT_SCHEDULED == d_runState || e_PAUSED == d_runState) {
        int status = d_multiQueueThreadPool_p->d_threadPool_p->
                                                           enqueueJob(functor);

        BSLS_ASSERT(0 == status);  (void)status;
    }
    else {
        d_runState = e_PAUSING;

        if (bslmt::ThreadUtil::self() == d_processor) {
            d_list.push_front(functorIfProcessor);

            return true;
        }

        d_list.push_front(functor);
    }

    return false;
}

int MultiQueueThreadPool_Queue::pushBack(const Job& functor)
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_lock);

    if (e_ENQUEUING_ENABLED == d_enqueueState) {
        d_list.push_back(functor);

        // Note that the following should match what is in 'pushFront'.

        if (e_NOT_SCHEDULED == d_runState) {
            d_runState = e_SCHEDULED;

            ++d_multiQueueThreadPool_p->d_numActiveQueues;

            int status = d_multiQueueThreadPool_p->d_threadPool_p->
                                                    enqueueJob(d_processingCb);

            BSLS_ASSERT(0 == status);  (void)status;
        }

        return 0;                                                     // RETURN
    }

    return 1;
}

int MultiQueueThreadPool_Queue::pushFront(const Job& functor)
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_lock);

    if (e_ENQUEUING_ENABLED == d_enqueueState) {
        d_list.push_front(functor);

        // Note that the following should match what is in 'pushBack'.

        if (e_NOT_SCHEDULED == d_runState) {

            d_runState = e_SCHEDULED;

            ++d_multiQueueThreadPool_p->d_numActiveQueues;

            int status = d_multiQueueThreadPool_p->d_threadPool_p->
                                                    enqueueJob(d_processingCb);

            BSLS_ASSERT(0 == status);  (void)status;
        }

        return 0;                                                     // RETURN
    }

    return 1;
}

void MultiQueueThreadPool_Queue::reset()
{
    d_list.clear();
    d_enqueueState = e_ENQUEUING_ENABLED;
    d_runState     = e_NOT_SCHEDULED;
    d_pauseCount   = 0;
    d_processor    = bslmt::ThreadUtil::invalidHandle();

}

int MultiQueueThreadPool_Queue::resume()
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_lock);

    if (e_PAUSED != d_runState) {
        return 1;                                                     // RETURN
    }

    if (!d_list.empty()) {
        int status = d_multiQueueThreadPool_p->d_threadPool_p->
                                                    enqueueJob(d_processingCb);

        if (0 != status) {
            return 1;
        }

        d_runState = e_SCHEDULED;

        ++d_multiQueueThreadPool_p->d_numActiveQueues;
    }
    else {
        d_runState = e_NOT_SCHEDULED;
    }

    return 0;
}

                    // ---------------------------------
                    // class bdlmt::MultiQueueThreadPool
                    // ---------------------------------

// PRIVATE MANIPULATORS
inline
void MultiQueueThreadPool::deleteQueueCb(
                                    MultiQueueThreadPool_Queue *queue,
                                    const CleanupFunctor&       cleanupFunctor,
                                    bslmt::Latch               *latch)
{
    BSLS_ASSERT(queue);

    if (latch) {
        latch->arrive();
    }
    else if (cleanupFunctor) {
        cleanupFunctor();
    }

    // Note that 'd_queuePool' does its own synchronization.

    d_queuePool.releaseObject(queue);
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
, d_queuePool(bdlf::BindUtil::bind(&createMultiQueueThreadPool_Queue,
                                   bdlf::PlaceHolders::_1,
                                   bdlf::PlaceHolders::_2,
                                   this),
              -1,
              basicAllocator)
, d_queueRegistry(basicAllocator)
, d_nextId(1)
, d_state(e_STATE_STOPPED)
, d_numActiveQueues(0)
, d_numDequeued(0)
, d_numEnqueued(0)
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
, d_queuePool(bdlf::BindUtil::bind(&createMultiQueueThreadPool_Queue,
                                   bdlf::PlaceHolders::_1,
                                   bdlf::PlaceHolders::_2,
                                   this),
              -1,
              basicAllocator)
, d_queueRegistry(basicAllocator)
, d_nextId(1)
, d_state(e_STATE_STOPPED)
, d_numActiveQueues(0)
, d_numDequeued(0)
, d_numEnqueued(0)
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
    bslmt::WriteLockGuard<bslmt::ReaderWriterMutex> guard(&d_lock);

    int id = d_nextId++;

    // Note that 'd_queuePool' does its own synchronization.

    d_queueRegistry[id] = d_queuePool.getObject();

    return id;
}

int MultiQueueThreadPool::deleteQueue(int                   id,
                                      const CleanupFunctor& cleanupFunctor)
{
    bslmt::WriteLockGuard<bslmt::ReaderWriterMutex> guard(&d_lock);

    MultiQueueThreadPool_Queue *queue = 0;

    if (findIfUsable(id, &queue)) {
        return 1;                                                     // RETURN
    }

    d_queueRegistry.erase(id);

    Job job = bdlf::BindUtil::bind(&MultiQueueThreadPool::deleteQueueCb,
                                   this,
                                   queue,
                                   cleanupFunctor,
                                   (bslmt::Latch *)0);

    queue->prepareForDeletion(job, job);

    return 0;
}

int MultiQueueThreadPool::deleteQueue(int id)
{
    bslmt::Latch latch(1);

    bool isProcessor;
    {
        bslmt::WriteLockGuard<bslmt::ReaderWriterMutex> guard(&d_lock);

        MultiQueueThreadPool_Queue *queue = 0;

        if (findIfUsable(id, &queue)) {
            return 1;                                                 // RETURN
        }

        d_queueRegistry.erase(id);

        Job job = bdlf::BindUtil::bind(&MultiQueueThreadPool::deleteQueueCb,
                                       this,
                                       queue,
                                       CleanupFunctor(&noOp),
                                       &latch);

        Job jobIfProcessor =
                     bdlf::BindUtil::bind(&MultiQueueThreadPool::deleteQueueCb,
                                          this,
                                          queue,
                                          CleanupFunctor(&noOp),
                                          (bslmt::Latch *)0);

        isProcessor = queue->prepareForDeletion(job, jobIfProcessor);
    }

    if (!isProcessor) {
        latch.wait();
    }

    return 0;
}

int MultiQueueThreadPool::enableQueue(int id)
{
    bslmt::ReadLockGuard<bslmt::ReaderWriterMutex> guard(&d_lock);

    MultiQueueThreadPool_Queue *queue;

    if (findIfUsable(id, &queue)) {
        return 1;                                                     // RETURN
    }

    return queue->enable();
}

int MultiQueueThreadPool::disableQueue(int id)
{
    bslmt::ReadLockGuard<bslmt::ReaderWriterMutex> guard(&d_lock);

    MultiQueueThreadPool_Queue *queue;

    if (findIfUsable(id, &queue)) {
        return 1;                                                     // RETURN
    }

    return queue->disable();
}

int MultiQueueThreadPool::drainQueue(int id)
{
    while (1) {
        {
            bslmt::ReadLockGuard<bslmt::ReaderWriterMutex>
                                                   guard(&d_lock);

            QueueRegistry::iterator iter = d_queueRegistry.find(id);

            if (d_queueRegistry.end() == iter) {
                return 1;                                             // RETURN
            }

            if (iter->second->isDrained()) {
                return 0;                                             // RETURN
            }
        }

        bslmt::ThreadUtil::yield();
    }

    return 0;
}

int MultiQueueThreadPool::start()
{
    while (1) {
        {
            bslmt::WriteLockGuard<bslmt::ReaderWriterMutex> guard(&d_lock);

            if (e_STATE_RUNNING == d_state) {
                return 0;                                             // RETURN
            }
            else if (e_STATE_STOPPED == d_state) {
                for (QueueRegistry::iterator it = d_queueRegistry.begin();
                     it != d_queueRegistry.end();
                     ++it) {
                    it->second->enable();
                }

                int rc = 0;
                if (d_threadPoolIsOwned) {
                    rc = d_threadPool_p->start() ? -1 : 0;
                }

                if (0 == rc) {
                    d_state = e_STATE_RUNNING;
                }

                return rc;                                            // RETURN
            }
        }

        bslmt::ThreadUtil::yield();
    }
}

void MultiQueueThreadPool::drain()
{
    while (1) {
        {
            bslmt::ReadLockGuard<bslmt::ReaderWriterMutex> guard(&d_lock);

            if (   e_STATE_STOPPED == d_state
                || 0               == d_threadPool_p->enabled()) {
                return;                                               // RETURN
            }

            if (0 == d_numActiveQueues) {
                if (d_threadPoolIsOwned) {
                    d_threadPool_p->drain();
                    d_threadPool_p->start();
                }

                return;                                               // RETURN
            }
        }

        bslmt::ThreadUtil::yield();
    }
}

int MultiQueueThreadPool::pauseQueue(int id)
{
    bslmt::ReadLockGuard<bslmt::ReaderWriterMutex> guard(&d_lock);

    MultiQueueThreadPool_Queue *queue;

    if (findIfUsable(id, &queue)) {
        return 1;                                                     // RETURN
    }

    return queue->pause();
}

int MultiQueueThreadPool::resumeQueue(int id)
{
    bslmt::ReadLockGuard<bslmt::ReaderWriterMutex> guard(&d_lock);

    MultiQueueThreadPool_Queue *queue;

    if (findIfUsable(id, &queue)) {
        return 1;                                                     // RETURN
    }

    return queue->resume();
}

void MultiQueueThreadPool::stop()
{
    {
        bslmt::WriteLockGuard<bslmt::ReaderWriterMutex> guard(&d_lock);

        if (e_STATE_STOPPED == d_state) {
            return;                                                   // RETURN
        }

        d_state = e_STATE_STOPPING;
    }

    // Wait until all queues are emptied.
    while (0 < d_numActiveQueues) {
        bslmt::ThreadUtil::yield();
    }

    {
        bslmt::WriteLockGuard<bslmt::ReaderWriterMutex> guard(&d_lock);

        if (d_threadPoolIsOwned) {
            d_threadPool_p->drain();
        }

        d_state = e_STATE_STOPPED;
    }
}

void MultiQueueThreadPool::shutdown()
{
    {
        bslmt::WriteLockGuard<bslmt::ReaderWriterMutex> guard(&d_lock);

        if (e_STATE_STOPPED == d_state || 0 == d_threadPool_p->enabled()) {
            // Note that 'd_queuePool' does its own synchronization.

            for (QueueRegistry::iterator it = d_queueRegistry.begin();
                 it != d_queueRegistry.end();
                 ++it) {
                d_queuePool.releaseObject(it->second);
            }

            d_queueRegistry.clear();
            d_nextId = 1;

            if (d_threadPoolIsOwned) {
                d_threadPool_p->stop();
            }

            d_state = e_STATE_STOPPED;

            return;                                                   // RETURN
        }

        d_state = e_STATE_STOPPING;
    }

    // Wait until all queues are emptied.
    while (0 < d_numActiveQueues) {
        bslmt::ThreadUtil::yield();
    }

    bslmt::WriteLockGuard<bslmt::ReaderWriterMutex> guard(&d_lock);

    bsl::size_t latchCount = d_queueRegistry.size();

    bslmt::Latch latch(static_cast<int>(latchCount));

    for (QueueRegistry::iterator it = d_queueRegistry.begin();
         it != d_queueRegistry.end();
         ++it) {
        MultiQueueThreadPool_Queue *queue = it->second;

        Job job = bdlf::BindUtil::bind(&MultiQueueThreadPool::deleteQueueCb,
                                       this,
                                       queue,
                                       CleanupFunctor(&noOp),
                                       &latch);

        queue->prepareForDeletion(job, job);
    }

    d_queueRegistry.clear();
    d_nextId = 1;

    guard.ptr()->unlock();

    if (latchCount) {
        latch.wait();
    }

    guard.ptr()->lockWrite();

    if (d_threadPoolIsOwned) {
        d_threadPool_p->stop();
    }

    d_state = e_STATE_STOPPED;
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2017 Bloomberg Finance L.P.
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
