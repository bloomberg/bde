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

#include <bslmt_latch.h>
#include <bslmt_lockguard.h>
#include <bslmt_readlockguard.h>
#include <bslmt_writelockguard.h>
#include <bslmt_semaphore.h>
#include <bslmt_threadutil.h>

#include <bslma_default.h>

#include <bsls_assert.h>

#include <bsl_memory.h>
#include <bsl_vector.h>

#include <bsl_iostream.h> // TBD

namespace BloombergLP {
namespace {

    // Internal helper functions.

void noOp() { }
    // This function does nothing.

}  // close unnamed namespace

namespace bdlmt {

                     // --------------------------------
                     // class MultiQueueThreadPool_Queue
                     // --------------------------------

// CREATORS
MultiQueueThreadPool_Queue::MultiQueueThreadPool_Queue(
                                              bslma::Allocator *basicAllocator)
: d_list(basicAllocator)
, d_threadPool_p(0)
, d_numActiveQueues_p(0)
, d_enqueueState(e_ENQUEUING_ENABLED)
, d_runState(e_NOT_SCHEDULED)
, d_lock()
, d_pauseBlock()
, d_pauseCount(0)
, d_processingCb(bsl::allocator_arg_t(), bsl::allocator<Job>(basicAllocator))
, d_processor(bslmt::ThreadUtil::invalidHandle())
{
}

MultiQueueThreadPool_Queue::~MultiQueueThreadPool_Queue()
{
}

void MultiQueueThreadPool_Queue::reset()
{
    d_list.clear();
    d_enqueueState   = e_ENQUEUING_ENABLED;
    d_runState       = e_NOT_SCHEDULED;
    d_pauseCount     = 0;
    d_processingCb   = Job();
    d_processor      = bslmt::ThreadUtil::invalidHandle();
}

// MANIPULATORS
void MultiQueueThreadPool_Queue::popFront()
{
    Job functor;
    {
        bslmt::LockGuard<bslmt::Mutex> guard(&d_lock);

        BSLS_ASSERT(!d_list.empty());

        if (e_PAUSING == d_runState) {
            d_runState = e_PAUSED;

            --*d_numActiveQueues_p;

            if (d_pauseCount) {
                d_pauseBlock.post(d_pauseCount);
                d_pauseCount = 0;
            }

            if (e_DELETING == d_enqueueState) {
                int status = d_threadPool_p->enqueueJob(d_list.front());

                BSLS_ASSERT(0 == status);  (void)status;
            }

            return;                                                   // RETURN
        }

        functor = d_list.front();
        d_list.pop_front();
        d_processor = bslmt::ThreadUtil::self();
    }

    functor();

    {
        bslmt::LockGuard<bslmt::Mutex> guard(&d_lock);

        d_processor = bslmt::ThreadUtil::invalidHandle();

        if (e_SCHEDULED == d_runState) {
            if (!d_list.empty()) {
                int status = d_threadPool_p->enqueueJob(d_processingCb);

                BSLS_ASSERT(0 == status);  (void)status;
            }
            else {
                d_runState = e_NOT_SCHEDULED;

                --*d_numActiveQueues_p;
            }
        }
        else {
            BSLS_ASSERT(e_PAUSING == d_runState);

            d_runState = e_PAUSED;

            --*d_numActiveQueues_p;

            if (d_pauseCount) {
                d_pauseBlock.post(d_pauseCount);
                d_pauseCount = 0;
            }

            if (e_DELETING == d_enqueueState) {
                BSLS_ASSERT(!d_list.empty());

                int status = d_threadPool_p->enqueueJob(d_list.front());

                BSLS_ASSERT(0 == status);  (void)status;
            }
        }
    }
}

int MultiQueueThreadPool_Queue::pushBack(const Job& functor)
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_lock);

    if (e_ENQUEUING_ENABLED == d_enqueueState) {
        d_list.push_back(functor);

        if (e_NOT_SCHEDULED == d_runState) {
            d_runState = e_SCHEDULED;

            ++*d_numActiveQueues_p;

            int status = d_threadPool_p->enqueueJob(d_processingCb);

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

        if (e_NOT_SCHEDULED == d_runState) {
            d_runState = e_SCHEDULED;

            ++*d_numActiveQueues_p;

            int status = d_threadPool_p->enqueueJob(d_processingCb);

            BSLS_ASSERT(0 == status);  (void)status;
        }

        return 0;                                                     // RETURN
    }

    return 1;
}

void MultiQueueThreadPool_Queue::prepareForDeletion(const Job& functor)
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_lock);

    d_enqueueState = e_DELETING;

    if (e_NOT_SCHEDULED == d_runState || e_PAUSED == d_runState) {
        int status = d_threadPool_p->enqueueJob(functor);

        BSLS_ASSERT(0 == status);  (void)status;
    }
    else {
        BSLS_ASSERT(e_SCHEDULED == d_runState || e_PAUSING == d_runState);

        d_runState = e_PAUSING;

        d_list.push_front(functor);
    }
}

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

        BSLS_ASSERT(e_SCHEDULED == d_runState);

        d_runState = e_PAUSING;

        if (bslmt::ThreadUtil::self() == d_processor) {
            return 0;                                                 // RETURN
        }

        ++d_pauseCount;
    }

    d_pauseBlock.wait();

    return 0;
}

int MultiQueueThreadPool_Queue::resume()
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_lock);

    if (e_PAUSED != d_runState) {
        return 1;                                                     // RETURN
    }

    if (!d_list.empty()) {
        d_runState = e_SCHEDULED;

        ++*d_numActiveQueues_p;

        int status = d_threadPool_p->enqueueJob(d_processingCb);

        BSLS_ASSERT(0 == status);  (void)status;
    }
    else {
        d_runState = e_NOT_SCHEDULED;
    }

    return 0;
}

// ACCESSORS
bool MultiQueueThreadPool_Queue::isEnabled() const
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_lock);

    return e_ENQUEUING_ENABLED == d_enqueueState;
}

bool MultiQueueThreadPool_Queue::isPaused() const
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_lock);

    return e_PAUSED == d_runState;
}

int MultiQueueThreadPool_Queue::length() const
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_lock);

    return static_cast<int>(d_list.size());
}

}  // close package namespace

                    // ---------------------------------
                    // class bdlmt::MultiQueueThreadPool
                    // ---------------------------------

// TYPES
typedef bdlcc::ObjectCatalogIter<bdlmt::MultiQueueThreadPool_Queue*>
                    RegistryIterator;
    // This type is provided for notational convenience when iterating over the
    // queue registry.

typedef bsl::pair<int, bdlmt::MultiQueueThreadPool_Queue*>
                    RegistryValue;
    // This type is provided for notational convenience when iterating over the
    // queue registry.

enum {
    // Internal running states.

    e_STATE_RUNNING,
    e_STATE_STOPPING,
    e_STATE_STOPPED
};

namespace bdlmt {

// PRIVATE ACCESSORS
MultiQueueThreadPool_Queue *MultiQueueThreadPool::lookupQueue(int id) const
{
    MultiQueueThreadPool_Queue *queue;

    if (   e_STATE_RUNNING != d_state
        ||               0 == d_threadPool_p->enabled()
        ||               0 != d_queueRegistry.find(id, &queue)) {
        return 0;                                                     // RETURN
    }

    return queue;
}

// PRIVATE MANIPULATORS
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

    bslmt::LockGuard<bslmt::Mutex> guardQueuePool(&d_queuePoolLock);

    d_queuePool.releaseObject(queue);
}

void MultiQueueThreadPool::processQueueCb(MultiQueueThreadPool_Queue *queue)
{
    BSLS_ASSERT(queue);

    ++d_numDequeued;
    queue->popFront();
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
, d_queuePool(-1, basicAllocator)
, d_queueRegistry(basicAllocator)
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

    bslmt::LockGuard<bslmt::Mutex> guardQueuePool(&d_queuePoolLock);

    MultiQueueThreadPool_Queue *queue = d_queuePool.getObject();

    queue->d_threadPool_p      = d_threadPool_p;
    queue->d_numActiveQueues_p = &d_numActiveQueues;
    queue->d_processingCb      = bdlf::BindUtil::bind(
                                         &MultiQueueThreadPool::processQueueCb,
                                         this,
                                         queue);

    return d_queueRegistry.add(queue);
}

int MultiQueueThreadPool::deleteQueue(int                   id,
                                      const CleanupFunctor& cleanupFunctor)
{
    bslmt::WriteLockGuard<bslmt::ReaderWriterMutex> guard(&d_lock);

    if (e_STATE_RUNNING != d_state) {
        return 1;                                                     // RETURN
    }

    MultiQueueThreadPool_Queue *queue = 0;

    int rc = d_queueRegistry.remove(id, &queue);
    if (rc) {
        return rc;                                                    // RETURN
    }

    Job job = bdlf::BindUtil::bind(&MultiQueueThreadPool::deleteQueueCb,
                                   this,
                                   queue,
                                   cleanupFunctor,
                                   (bslmt::Latch *)0);

    queue->prepareForDeletion(job);

    return 0;
}

int MultiQueueThreadPool::deleteQueue(int id)
{
    bslmt::Latch latch(1);

    {
        bslmt::WriteLockGuard<bslmt::ReaderWriterMutex> guard(&d_lock);

        if (e_STATE_RUNNING != d_state) {
            return 1;                                                 // RETURN
        }

        MultiQueueThreadPool_Queue *queue = 0;

        int rc = d_queueRegistry.remove(id, &queue);
        if (rc) {
            return rc;                                                // RETURN
        }

        Job job = bdlf::BindUtil::bind(&MultiQueueThreadPool::deleteQueueCb,
                                       this,
                                       queue,
                                       CleanupFunctor(&noOp),
                                       &latch);

        queue->prepareForDeletion(job);
    }

    latch.wait();

    return 0;
}

int MultiQueueThreadPool::enableQueue(int id)
{
    bslmt::ReadLockGuard<bslmt::ReaderWriterMutex> guard(&d_lock);

    MultiQueueThreadPool_Queue *queue = lookupQueue(id);

    if (0 == queue) {
        return 1;                                                     // RETURN
    }

    return queue->enable();
}

int MultiQueueThreadPool::disableQueue(int id)
{
    bslmt::ReadLockGuard<bslmt::ReaderWriterMutex> guard(&d_lock);

    MultiQueueThreadPool_Queue *queue = lookupQueue(id);

    if (0 == queue) {
        return 1;                                                     // RETURN
    }

    return queue->disable();
}

int MultiQueueThreadPool::drainQueue(int id)
{
    MultiQueueThreadPool_Queue *queue;
    int                         numPendingJobs;

    do {
        {
            bslmt::ReadLockGuard<bslmt::ReaderWriterMutex>
                                                   guard(&d_lock);

            int rc = d_queueRegistry.find(id, &queue);
            if (0 == rc) {
                numPendingJobs = queue->length();
            }
            else {
                return rc;                                            // RETURN
            }
        }
        if (numPendingJobs) {
            bslmt::ThreadUtil::yield();
        }
    } while (numPendingJobs);

    return 0;
}

int MultiQueueThreadPool::start()
{
    while (1) {
        bslmt::WriteLockGuard<bslmt::ReaderWriterMutex> guard(&d_lock);

        if (e_STATE_RUNNING == d_state) {
            return 0;                                                 // RETURN
        }
        else if (e_STATE_STOPPED == d_state) {
            for (RegistryIterator it(d_queueRegistry); it; ++it) {
                RegistryValue rv = it();
                rv.second->enable();
            }

            int rc = 0;
            if (d_threadPoolIsOwned) {
                rc = d_threadPool_p->start() ? -1 : 0;
            }

            if (0 == rc) {
                d_state = e_STATE_RUNNING;
            }

            return rc;                                                // RETURN
        }

        bslmt::ThreadUtil::yield();
    }
}

void MultiQueueThreadPool::drain()
{
    bslmt::ReadLockGuard<bslmt::ReaderWriterMutex> guard(&d_lock);

    if (   e_STATE_RUNNING != d_state
        || 0               == d_threadPool_p->enabled()) {
        return;                                                       // RETURN
    }

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

int MultiQueueThreadPool::pauseQueue(int id)
{
    bslmt::ReadLockGuard<bslmt::ReaderWriterMutex> guard(&d_lock);

    MultiQueueThreadPool_Queue *queue = lookupQueue(id);

    if (0 == queue) {
        return 1;                                                     // RETURN
    }

    return queue->pause();
}

int MultiQueueThreadPool::resumeQueue(int id)
{
    bslmt::ReadLockGuard<bslmt::ReaderWriterMutex> guard(&d_lock);

    MultiQueueThreadPool_Queue *queue = lookupQueue(id);

    if (0 == queue) {
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
            BSLS_ASSERT(0 <  d_threadPool_p->numWaitingThreads());
            BSLS_ASSERT(0 == d_threadPool_p->numActiveThreads());
        }

        d_state = e_STATE_STOPPED;
    }
}

void MultiQueueThreadPool::shutdown()
{
    {
        bslmt::WriteLockGuard<bslmt::ReaderWriterMutex> guard(&d_lock);

        if (e_STATE_STOPPED == d_state) {
            bslmt::LockGuard<bslmt::Mutex> guardQueuePool(&d_queuePoolLock);

            bsl::vector<MultiQueueThreadPool_Queue *> buffer;

            d_queueRegistry.removeAll(&buffer);

            for (bsl::size_t i = 0; i < buffer.size(); ++i) {
                d_queuePool.releaseObject(buffer[i]);
            }

            return;                                                   // RETURN
        }

        d_state = e_STATE_STOPPING;
    }

    // Wait until all queues are emptied.
    while (0 < d_numActiveQueues) {
        bslmt::ThreadUtil::yield();
    }

    bslmt::Latch *latch = 0;
    {
        bslmt::WriteLockGuard<bslmt::ReaderWriterMutex> guard(&d_lock);

        bsl::vector<int> qids;
        qids.reserve(d_queueRegistry.length());
        for (RegistryIterator it(d_queueRegistry); it; ++it) {
            RegistryValue rv = it();
            qids.push_back(rv.first);
        }

        if (d_queueRegistry.length()) {
            latch = new bslmt::Latch(d_queueRegistry.length());
        }

        for (bsl::vector<int>::iterator it = qids.begin();
             it != qids.end();
             ++it) {
            MultiQueueThreadPool_Queue *queue = 0;

            int rc = d_queueRegistry.remove(*it, &queue);
            BSLS_ASSERT(0 == rc);  (void)rc;

            Job job = bdlf::BindUtil::bind(
                                          &MultiQueueThreadPool::deleteQueueCb,
                                          this,
                                          queue,
                                          CleanupFunctor(&noOp),
                                          latch);

            queue->prepareForDeletion(job);
        }
    }

    if (latch) {
        latch->wait();
    }

    {
        bslmt::WriteLockGuard<bslmt::ReaderWriterMutex> guard(&d_lock);

        if (d_threadPoolIsOwned) {
            d_threadPool_p->stop();
            BSLS_ASSERT(0 == d_threadPool_p->numWaitingThreads());
            BSLS_ASSERT(0 == d_threadPool_p->numActiveThreads());
        }

        d_state = e_STATE_STOPPED;
    }
}

// ACCESSORS
bool MultiQueueThreadPool::isEnabled(int id) const
{
    MultiQueueThreadPool_Queue *queue;

    bslmt::ReadLockGuard<bslmt::ReaderWriterMutex> guard(&d_lock);

    if (0 == d_queueRegistry.find(id, &queue)) {
        return queue->isEnabled();
    }

    return false;
}

bool MultiQueueThreadPool::isPaused(int id) const
{
    MultiQueueThreadPool_Queue *queue;

    bslmt::ReadLockGuard<bslmt::ReaderWriterMutex> guard(&d_lock);

    if (0 == d_queueRegistry.find(id, &queue)) {
        return queue->isPaused();
    }

    return false;
}

int MultiQueueThreadPool::numElements(int id) const
{
    MultiQueueThreadPool_Queue *queue;

    bslmt::ReadLockGuard<bslmt::ReaderWriterMutex> guard(&d_lock);

    if (0 == d_queueRegistry.find(id, &queue)) {
        return queue->length();
    }

    return -1;
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
