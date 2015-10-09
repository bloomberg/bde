// bdlmt_multiprioritythreadpool.cpp                                  -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlmt_multiprioritythreadpool.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmt_multiprioritythreadpool_cpp,"$Id$ $CSID$")

///IMPLEMENTATION NOTES
///--------------------
// The states of 'd_threadStartState' and 'd_threadSuspendState' are defined by
// the enums 'StartType' and 'SuspendType', respectively, defined below under
// 'LOCAL TYPES'.
//
// All changes of 'd_threadStartState' and 'd_threadSuspendState' are guarded
// by 'd_mutex'.  Also note that ALL transitions of those two variables into
// transitional states (states ending with 'ING') are guarded by 'd_metaMutex',
// and that mutex is never released until neither of those variables are in a
// transitional state.
//
// Also note that if the thread pool is suspended and 'd_metaMutex' is not
// locked, then all threads are either stopped, or waiting on condition
// 'd_resumeCondition'.
//
// The third orthogonal property of the thread pool, the enabled / disabled
// state of the queue, is much simpler.  It is just turned on and off at will,
// and the functionality is fully encapsulated in the queue.  The enabled /
// disabled state only affects the 'enqueueJob' method, pops from the queue can
// happen regardless of that state.
//
// Things get a bit tricky when starting or stopping a thread pool that is
// suspended.  When starting a thread pool that is suspended, it is important
// that the threads all wait before processing any jobs, and similarly, when
// stopping, they must stop before processing any other jobs.

#include <bslma_default.h>

#include <bdlf_bind.h>
#include <bdlf_memfn.h>

#include <bslmt_barrier.h>
#include <bslmt_lockguard.h>

#include <bslma_allocator.h>

#include <bsls_assert.h>

#include <bsl_csignal.h>

namespace BloombergLP {

namespace {

enum StartState {       // type of 'd_threadStartState'
    e_STARTING,
    e_STARTED,
    e_STOPPING,
    e_STOPPED
};

enum SuspendState {     // type of 'd_threadSuspendState'
    e_SUSPENDING,
    e_SUSPENDED,
//  e_RESUMING,   // not used - we go straight from suspended to resumed
    e_RESUMED
};

}  // close unnamed namespace

namespace bdlmt {
                       // -----------------------------
                       // class MultipriorityThreadPool
                       // -----------------------------

// PRIVATE MANIPULATORS
void MultipriorityThreadPool::worker()
{
    {
        bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

        if (e_STOPPING == d_threadStartState) {
            return;                                                   // RETURN
        }
        BSLS_ASSERT(e_STARTING == d_threadStartState);

        if (d_numThreads == ++d_numStartedThreads) {
            d_threadStartState = e_STARTED;
            d_allThreadsStartedCondition.broadcast();
        }
        else {
            do {
                d_allThreadsStartedCondition.wait(&d_mutex);
            } while (e_STARTING == d_threadStartState);
            // Note that state might have been changed to 'e_STOPPING' if any
            // threads failed to start.
        }
    }

    while (1) {
        if (e_STARTED != d_threadStartState ||
            e_RESUMED != d_threadSuspendState) {
            bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

            do {
                // Note 'd_metaMutex' may or may not be locked when we get
                // here.

                if (e_STOPPING == d_threadStartState) {
                    --d_numStartedThreads;
                    return;                                           // RETURN
                }
                BSLS_ASSERT(e_STARTED == d_threadStartState);

                BSLS_ASSERT(e_SUSPENDED != d_threadSuspendState);
                if (e_SUSPENDING == d_threadSuspendState) {
                    if (d_numThreads == ++d_numSuspendedThreads) {
                        d_threadSuspendState = e_SUSPENDED;
                        d_allThreadsSuspendedCondition.broadcast();
                    }
                    do {
                        d_resumeCondition.wait(&d_mutex);
                    } while (e_RESUMED != d_threadSuspendState
                          && e_STARTED == d_threadStartState);
                    --d_numSuspendedThreads;
                }

                // If we were suspended and then told to stop, we will pass
                // through this point within 'e_SUSPENDED' and 'e_STOPPING'
                // states.
            } while (e_STARTED != d_threadStartState
                  || e_RESUMED != d_threadSuspendState);
        }  // release mutex

        {
            ThreadFunctor job;

            // Retrieve the next job, blocking until one is available.
            d_queue.popFront(&job);

            if (job) {
                // Run the job.
                ++d_numActiveThreads;
                job();                                                // INVOKE
                --d_numActiveThreads;
            }
        }
    }
}

// CREATORS
MultipriorityThreadPool::MultipriorityThreadPool(
                                              int               numThreads,
                                              int               numPriorities,
                                              bslma::Allocator *basicAllocator)
: d_queue(numPriorities, bslma::Default::allocator(basicAllocator))
, d_threadGroup(bslma::Default::allocator(basicAllocator))
, d_numThreads(numThreads)
, d_threadStartState(e_STOPPED)
, d_threadSuspendState(e_RESUMED)
, d_numStartedThreads(0)
, d_numSuspendedThreads(0)
, d_numActiveThreads(0)
{
    BSLS_ASSERT(k_MAX_NUM_PRIORITIES >= numPriorities);
    BSLS_ASSERT(1 <= numPriorities);
    BSLS_ASSERT(1 <= numThreads);
}

MultipriorityThreadPool::MultipriorityThreadPool(
                              int                             numThreads,
                              int                             numPriorities,
                              const bslmt::ThreadAttributes&  threadAttributes,
                              bslma::Allocator               *basicAllocator)
: d_queue(numPriorities, bslma::Default::allocator(basicAllocator))
, d_threadAttributes(threadAttributes)
, d_threadGroup(bslma::Default::allocator(basicAllocator))
, d_numThreads(numThreads)
, d_threadStartState(e_STOPPED)
, d_threadSuspendState(e_RESUMED)
, d_numStartedThreads(0)
, d_numSuspendedThreads(0)
, d_numActiveThreads(0)
{
    BSLS_ASSERT(k_MAX_NUM_PRIORITIES >= numPriorities);
    BSLS_ASSERT(1 <= numPriorities);
    BSLS_ASSERT(1 <= numThreads);

    // Force all threads to be joinable.
    d_threadAttributes.setDetachedState(
                                   bslmt::ThreadAttributes::e_CREATE_JOINABLE);
}

MultipriorityThreadPool::~MultipriorityThreadPool()
{
    BSLS_ASSERT(e_STOPPED == d_threadStartState);
}

// MANIPULATORS
int MultipriorityThreadPool::enqueueJob(const ThreadFunctor& job,
                                        int                  priority)
{
    BSLS_ASSERT((unsigned) priority < (unsigned) d_queue.numPriorities());
    // checks '0 <= priority < numPriorities()'

    return d_queue.pushBack(job, priority);
}

int MultipriorityThreadPool::enqueueJob(bslmt_ThreadFunction  jobFunction,
                                        void                 *jobData,
                                        int                   priority)
{
    return enqueueJob(bdlf::BindUtil::bind(jobFunction, jobData), priority);
}

void MultipriorityThreadPool::enableQueue()
{
    d_queue.enable();
}

void MultipriorityThreadPool::disableQueue()
{
    d_queue.disable();
}

int MultipriorityThreadPool::startThreads()
{
    bslmt::LockGuard<bslmt::Mutex> metaLock(&d_metaMutex);
    int rc = 0;

    if (e_STARTED == d_threadStartState) {
        return 0;                                                     // RETURN
    }
    BSLS_ASSERT(e_STOPPED == d_threadStartState);

#if defined(BSLS_PLATFORM_OS_UNIX)
    sigset_t oldBlockSet, newBlockSet;  // set of signals to be blocked in
                                        // managed threads
    sigfillset(&newBlockSet);

    static const int signals[] = {      // synchronous signals
         SIGBUS, SIGFPE, SIGILL, SIGSEGV, SIGSYS, SIGABRT, SIGTRAP
        #if !defined(BSLS_PLATFORM_OS_CYGWIN) || defined(SIGIOT)
        ,SIGIOT
        #endif
    };

    enum {
        NUM_SIGNALS = sizeof signals / sizeof *signals
    };

    for (int i = 0; i < NUM_SIGNALS; ++i) {
        sigdelset(&newBlockSet, signals[i]);
    }

    // Block all synchronous signals.
    pthread_sigmask(SIG_BLOCK, &newBlockSet, &oldBlockSet);
#endif

    // 'workerFunctor' calls the private 'worker' method.

    const ThreadFunctor workerFunctor =
            bdlf::MemFnUtil::memFn(&MultipriorityThreadPool::worker, this);

    {
        bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

        d_threadStartState = e_STARTING;
        if (e_SUSPENDED == d_threadSuspendState) {
            // This is necessary because when we wait on
            // 'd_allThreadsStartedCondition', we need to be waiting for a
            // state transition to 'e_SUSPENDED' to confirm our wait is over.
            d_threadSuspendState = e_SUSPENDING;
        }

        int startedThreads = d_threadGroup.addThreads(workerFunctor,
                                                      d_numThreads,
                                                      d_threadAttributes);
        if (d_numThreads == startedThreads) {
            if (e_SUSPENDING == d_threadSuspendState) {
                do {
                    d_allThreadsSuspendedCondition.wait(&d_mutex);
                } while (e_SUSPENDED != d_threadSuspendState);
            }
            else {
                do {
                    d_allThreadsStartedCondition.wait(&d_mutex);
                } while (e_STARTING == d_threadStartState);
            }
            BSLS_ASSERT(e_STARTED      == d_threadStartState);
            BSLS_ASSERT(d_numThreads == d_numStartedThreads);
        }
        else {
            // start failed -- shut down all the threads

            d_threadStartState = e_STOPPING;
            d_allThreadsStartedCondition.broadcast();

            // Note that if we are in the 'e_SUSPENDING' state, the threads
            // will wait on this condition before they wait on the suspended
            // condition.  Once released from the started condition, they will
            // run straight into the check for 'e_STOPPED' before reaching the
            // wait on a suspended condition.

            {
                bslmt::LockGuardUnlock<bslmt::Mutex> unlock(&d_mutex);
                d_threadGroup.joinAll();
            }

            if (e_SUSPENDING == d_threadSuspendState) {
                d_threadSuspendState = e_SUSPENDED;
            }
            d_threadStartState = e_STOPPED;

            rc = -1;
        }
    }

#if defined(BSLS_PLATFORM_OS_UNIX)
    // Restore the mask.
    pthread_sigmask(SIG_SETMASK, &oldBlockSet, &newBlockSet);
#endif

    BSLS_ASSERT(e_STARTED   == d_threadStartState
             || e_STOPPED   == d_threadStartState);
    BSLS_ASSERT(e_RESUMED   == d_threadSuspendState
             || e_SUSPENDED == d_threadSuspendState);

    return rc;
}

void MultipriorityThreadPool::stopThreads()
{
    bslmt::LockGuard<bslmt::Mutex> metaLock(&d_metaMutex);
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    if (e_STOPPED == d_threadStartState) {
        return;                                                       // RETURN
    }

    BSLS_ASSERT(e_STARTED == d_threadStartState);
    d_threadStartState = e_STOPPING;

    // Give waiting threads a chance to stop.
    if (e_SUSPENDED == d_threadSuspendState) {
        BSLS_ASSERT(0 == numActiveThreads());
        d_resumeCondition.broadcast();
    }
    else {
        const ThreadFunctor nullJob;
        // Push high-priority null jobs into multi-priority queue, in case
        // threads are already blocking on pops of the queue for input.
        // 'worker' will do a no-op when it encounters these jobs.  There might
        // be fewer than 'numThreads()' threads to stop, but extra null jobs do
        // no harm.
        d_queue.pushFrontMultipleRaw(nullJob, 0, d_numThreads);
    }

    {
        bslmt::LockGuardUnlock<bslmt::Mutex> unlock(&d_mutex);
        d_threadGroup.joinAll();
    }

    d_threadStartState = e_STOPPED;

    BSLS_ASSERT(0 == d_numStartedThreads);
    BSLS_ASSERT(0 == d_numSuspendedThreads);
    BSLS_ASSERT(0 == d_numActiveThreads);
}

void MultipriorityThreadPool::suspendProcessing()
{
    bslmt::LockGuard<bslmt::Mutex> metaLock(&d_metaMutex);
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    if (e_SUSPENDED == d_threadSuspendState) {
        return;                                                       // RETURN
    }
    BSLS_ASSERT(e_RESUMED == d_threadSuspendState);

    if (e_STOPPED == d_threadStartState) {
        d_threadSuspendState = e_SUSPENDED;

        BSLS_ASSERT(0 == d_numStartedThreads);
        BSLS_ASSERT(0 == d_numSuspendedThreads);
        return;                                                       // RETURN
    }

    d_threadSuspendState = e_SUSPENDING;

    const ThreadFunctor nullJob;

    // Push high-priority null jobs into multi-priority queue, in case threads
    // are already blocking on pops of the queue for input.  'worker' will do a
    // no-op when it encounters these jobs.
    d_queue.pushFrontMultipleRaw(nullJob, 0, d_numThreads);

    do {
        d_allThreadsSuspendedCondition.wait(&d_mutex);
    } while (e_SUSPENDED != d_threadSuspendState);

    BSLS_ASSERT(d_numThreads == d_numStartedThreads);
    BSLS_ASSERT(d_numThreads == d_numSuspendedThreads);
}

void MultipriorityThreadPool::resumeProcessing()
{
    bslmt::LockGuard<bslmt::Mutex> metaLock(&d_metaMutex);

    if (e_RESUMED == d_threadSuspendState) {
        return;                                                       // RETURN
    }
    BSLS_ASSERT(e_SUSPENDED == d_threadSuspendState);

    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    d_threadSuspendState = e_RESUMED;
    d_resumeCondition.broadcast();
}

void MultipriorityThreadPool::drainJobs()
{
    bslmt::LockGuard<bslmt::Mutex> metaLock(&d_metaMutex);

    // If these two conditions are not true, this method will hang.
    BSLS_ASSERT(e_STARTED == d_threadStartState);
    BSLS_ASSERT(e_RESUMED == d_threadSuspendState);

    bslmt::Barrier barrier(d_numThreads + 1);
    const ThreadFunctor barrierJob =
                       bdlf::MemFnUtil::memFn(&bslmt::Barrier::wait, &barrier);

    d_queue.pushBackMultipleRaw(barrierJob,
                                d_queue.numPriorities() - 1,
                                d_numThreads);

    barrier.wait();
}

void MultipriorityThreadPool::removeJobs()
{
    bslmt::LockGuard<bslmt::Mutex> metaLock(&d_metaMutex);

    d_queue.removeAll();
}

void MultipriorityThreadPool::shutdown()
{
    disableQueue();
    removeJobs();
    stopThreads();
}

// ACCESSORS
bool MultipriorityThreadPool::isEnabled() const
{
    return d_queue.isEnabled();
}

bool MultipriorityThreadPool::isStarted() const
{
    return e_STARTED == d_threadStartState;
}

bool MultipriorityThreadPool::isSuspended() const
{
    return e_SUSPENDED == d_threadSuspendState;
}

int MultipriorityThreadPool::numActiveThreads() const
{
    return d_numActiveThreads;
}

int MultipriorityThreadPool::numPriorities() const
{
    return d_queue.numPriorities();
}

int MultipriorityThreadPool::numPendingJobs() const
{
    return d_queue.length();
}

int MultipriorityThreadPool::numStartedThreads() const
{
    return d_numStartedThreads;
}

int MultipriorityThreadPool::numThreads() const
{
    return d_numThreads;
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
