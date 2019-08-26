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
#include <bsls_log.h>
#include <bsls_stackaddressutil.h>
#include <bsls_systemtime.h>
#include <bsls_timeinterval.h>

#include <bsl_memory.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace {

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

// PRIVATE MANIPULATORS
void MultiQueueThreadPool_Queue::setPaused()
{
    BSLS_ASSERT(e_PAUSING == d_runState);

    BSLMT_MUTEXASSERT_IS_LOCKED(&d_lock);

    d_runState = e_PAUSED;

    if (d_pauseCount) {
        d_pauseCondition.broadcast();
    }

    if (e_DELETING == d_enqueueState) {
        int status = d_multiQueueThreadPool_p->d_threadPool_p->
                                                    enqueueJob(d_list.front());

        BSLS_ASSERT(0 == status);  (void)status;

        // Note that 'd_numActiveQueues' is decremented at the completion of
        // 'deleteQueueCb', and hence should not be modified on this execution
        // path.
    }
    else {
        --d_multiQueueThreadPool_p->d_numActiveQueues;
    }
}

// CREATORS
MultiQueueThreadPool_Queue::MultiQueueThreadPool_Queue(
                                    MultiQueueThreadPool *multiQueueThreadPool,
                                    bslma::Allocator     *basicAllocator)
: d_multiQueueThreadPool_p(multiQueueThreadPool)
, d_list(basicAllocator)
, d_enqueueState(e_ENQUEUING_ENABLED)
, d_runState(e_NOT_SCHEDULED)
, d_lock()
, d_pauseCondition()
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

void MultiQueueThreadPool_Queue::drainWaitWhilePausing()
{
    // since the expected usage of this method requires waiting from when
    // signalled threads are awoken in 'waitWhilePausing' until they can exit
    // that method, the expected duration of waiting is short and spin-yielding
    // is appropriate

    bsls::TimeInterval start = bsls::SystemTime::nowRealtimeClock();
    bsls::TimeInterval logInterval(0, 100000000);  // 0.1s

    while (start.totalNanoseconds()) {
        bslmt::LockGuard<bslmt::Mutex> guard(&d_lock);
        if (d_pauseCount) {
            bslmt::ThreadUtil::yield();

            bsls::TimeInterval now = bsls::SystemTime::nowRealtimeClock();
            if ((now - start) >= logInterval) {
                start       =  now;
                logInterval += logInterval;

                char stackBuffer[1024];
                bsls::StackAddressUtil::formatCheapStack(stackBuffer, 1024);
                BSLS_LOG_WARN("Unexpected spins waiting for pause: %s",
                              stackBuffer);
            }
        }
        else {
            start.setTotalNanoseconds(0);
        }
    }
}

void MultiQueueThreadPool_Queue::executeFront()
{
    Job functor;
    {
        bslmt::LockGuard<bslmt::Mutex> guard(&d_lock);

        BSLS_ASSERT(!d_list.empty());

        if (e_PAUSING == d_runState) {
            setPaused();

            return;                                                   // RETURN
        }

        // If the queue is being deleted, the functor about to be popped is
        // 'deleteQueueCb' and is not counted in 'd_numEnqueued' so must not be
        // counted in 'd_numExecuted'.

        if (e_DELETING != d_enqueueState) {
            ++d_multiQueueThreadPool_p->d_numExecuted;
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

bool MultiQueueThreadPool_Queue::enqueueDeletion(
                                                const Job&    cleanupFunctor,
                                                bslmt::Latch *completionSignal)
{
    // Note that the queue is actually deleted by the thread pool while
    // executing the supplied 'functor' (which is
    // 'MultiQueueThreadPool::deleteQueueCb' bound with the required
    // arguments).

    bslmt::LockGuard<bslmt::Mutex> guard(&d_lock);

    d_enqueueState = e_DELETING;

    bool isProcessingThread = bslmt::ThreadUtil::self() == d_processor;

    Job job = bdlf::BindUtil::bind(&MultiQueueThreadPool::deleteQueueCb,
                                   d_multiQueueThreadPool_p,
                                   this,
                                   cleanupFunctor,
                                   isProcessingThread ? 0 : completionSignal);

    d_multiQueueThreadPool_p->d_numDeleted += static_cast<int>(d_list.size());

    if (e_NOT_SCHEDULED == d_runState || e_PAUSED == d_runState) {
        // Note that 'd_numActiveQueues' is decremented at the completion of
        // 'deleteQueueCb' so must be incremented here.

        ++d_multiQueueThreadPool_p->d_numActiveQueues;

        int rc = d_multiQueueThreadPool_p->d_threadPool_p->enqueueJob(job);

        BSLS_ASSERT(0 == rc);  (void)rc;
    }
    else {
        // Note that under no circumstance is 'd_numExecuted' incremented when
        // the callback about to be pushed executes so 'd_numEnqueued' must not
        // be incremented here.

        d_runState = e_PAUSING;

        d_list.push_front(job);
    }

    return isProcessingThread;
}

int MultiQueueThreadPool_Queue::initiatePause()
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_lock);

    if (   e_DELETING == d_enqueueState
        || e_PAUSING  == d_runState
        || e_PAUSED   == d_runState) {
        return 1;                                                     // RETURN
    }

    if (e_NOT_SCHEDULED == d_runState) {
        d_runState = e_PAUSED;
    }
    else {
        d_runState = e_PAUSING;
    }

    ++d_pauseCount;

    return 0;
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

    // If the queue is in the 'e_PAUSING' state, implying the pause will become
    // effective when the "currently running job" completes, instead of failing
    // to 'resume' (returning a non-zero value), the queue can be immediately
    // returned to the 'e_SCHEDULED' state.  There are two caveats.  The
    // 'e_PAUSING' state is used during deletion and there may be threads
    // waiting for the "currently running job" to complete.

    if (e_DELETING != d_enqueueState && e_PAUSING == d_runState) {
        d_runState = e_SCHEDULED;

        if (d_pauseCount) {
            d_pauseCondition.broadcast();
        }
        return 0;
    }

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

void MultiQueueThreadPool_Queue::waitWhilePausing()
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_lock);

    BSLS_ASSERT(0 < d_pauseCount);

    // do not block if the invoking thread is processing the "currently
    // executing job" or of there is nothing running on this queue (e.g., to
    // avoid deadlock when the threadpool has only one thread and that thread
    // is the one invoking this method)

    if (   bslmt::ThreadUtil::self()          != d_processor
        && bslmt::ThreadUtil::invalidHandle() != d_processor) {
        while (e_PAUSING == d_runState) {
            d_pauseCondition.wait(&d_lock);
        }
    }

    --d_pauseCount;
}

                    // ---------------------------------
                    // class bdlmt::MultiQueueThreadPool
                    // ---------------------------------

// PRIVATE MANIPULATORS
void MultiQueueThreadPool::deleteQueueCb(
                                  MultiQueueThreadPool_Queue *queue,
                                  const CleanupFunctor&       cleanup,
                                  bslmt::Latch               *completionSignal)
{
    BSLS_ASSERT(queue);

    if (cleanup) {
        cleanup();
    }

    queue->drainWaitWhilePausing();

    // Note that 'd_queuePool' does its own synchronization.

    d_queuePool.releaseObject(queue);

    if (completionSignal) {
        completionSignal->arrive();
    }

    --d_numActiveQueues;
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
, d_numExecuted(0)
, d_numEnqueued(0)
, d_numDeleted(0)
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
, d_numExecuted(0)
, d_numEnqueued(0)
, d_numDeleted(0)
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

    queue->enqueueDeletion(cleanupFunctor);

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

        isProcessor = queue->enqueueDeletion(CleanupFunctor(), &latch);
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
                // If the thread pool is shared, no further checks can be
                // performed.

                if (!d_threadPoolIsOwned) {
                    return;                                           // RETURN
                }

                // If the thread pool is not shared, wait for the pool to
                // drain.

                if (   0 == d_threadPool_p->numActiveThreads()
                    && 0 == d_threadPool_p->numPendingJobs()) {
                    return;                                           // RETURN
                }
            }
        }

        bslmt::ThreadUtil::yield();
    }
}

int MultiQueueThreadPool::pauseQueue(int id)
{
    MultiQueueThreadPool_Queue *queue;
    int rv;
    {
        bslmt::ReadLockGuard<bslmt::ReaderWriterMutex> guard(&d_lock);

        if (findIfUsable(id, &queue)) {
            return 1;                                                 // RETURN
        }

        rv = queue->initiatePause();
    }

    if (0 == rv) {
        // note that 'd_pauseCount' prevents 'queue' from becoming invalid

        queue->waitWhilePausing();
    }

    return rv;
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

        queue->enqueueDeletion(CleanupFunctor(), &latch);
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
// Copyright 2019 Bloomberg Finance L.P.
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
