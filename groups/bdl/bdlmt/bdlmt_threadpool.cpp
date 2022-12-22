// bdlmt_threadpool.cpp                                               -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlmt_threadpool.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmt_threadpool_cpp,"$Id$ $CSID$")

#include <bslmt_lockguard.h>
#include <bsls_systemclocktype.h>
#include <bsls_systemtime.h>
#include <bsls_assert.h>
#include <bsls_platform.h>
#include <bsls_timeinterval.h>
#include <bsls_timeutil.h>
#include <bsls_types.h>

#include <bslmt_barrier.h>    // for testing only
#include <bslmt_lockguard.h>  // for testing only
#include <bslmt_threadattributes.h>     // for testing only
#include <bslmt_threadutil.h>     // for testing only

#if defined(BSLS_PLATFORM_OS_UNIX)
#include <bsl_c_signal.h>              // sigfillset
#endif

#include <bsl_climits.h>  // 'INT_MAX'
#include <bsl_cstdlib.h>

namespace BloombergLP {

namespace bdlmt {
                         // ==================
                         // ThreadPoolWaitNode
                         // ==================

struct ThreadPoolWaitNode {
    // This structure is used to implement the linked list of threads that are
    // waiting for a request to process.  Each thread has its own instance of
    // this structure (a local variable in the ThreadPool::workerThread).
    // When a thread finishes executing a job, if there are no pending jobs, it
    // will add itself to the head of the wait list.  This logic behaves the
    // same as a condition variable with the exception that it implements a
    // LIFO instead of a FIFO wait list logic.  This ensures that threads that
    // are truly idle will not wake until they have timed out.  When a new job
    // arrives, the thread will be signaled to process by using the 'd_jobCond'
    // condition variable.  Whether the thread really has a job or the
    // condition variable timed out on its wait is stored in the 'd_hasJob'
    // state value.  A thread that takes on a new job removes itself from the
    // wait list (not necessarily at the head of the list).

    bslmt::Condition             d_jobCond; // signaled when 'd_hasJob' is set

    bsls::AtomicPointer<ThreadPoolWaitNode>
                                 d_next;    // pointer to the next waiting
                                            // thread

    bsls::AtomicPointer<ThreadPoolWaitNode>
                                 d_prev;    // pointer to the previous waiting
                                            // thread

    bsls::AtomicInt              d_hasJob;  // 1 if a job has been enqueued, 0
                                            // otherwise

    // CREATORS
    ThreadPoolWaitNode();
        // Default constructor.
};
                            // ===============
                            // ThreadPoolEntry
                            // ===============

extern "C" void *ThreadPoolEntry(void *aThis)
    // Entry point for processing threads.
{
    ((bdlmt::ThreadPool*)aThis)->workerThread();
    return 0;
}

                            // ==================
                            // ThreadPoolWaitNode
                            // ==================

ThreadPoolWaitNode::ThreadPoolWaitNode()
: d_jobCond(bsls::SystemClockType::e_MONOTONIC)
{
}

                                // ----------
                                // ThreadPool
                                // ----------

// CLASS DATA
const char ThreadPool::s_defaultThreadName[16] = { "bdl.ThreadPool" };

// PRIVATE MANIPULATORS
void ThreadPool::doEnqueueJob(const Job& job)
{
    d_queue.push_back(job);
    wakeThreadIfNeeded();
}

void ThreadPool::doEnqueueJob(bslmf::MovableRef<Job> job)
{
    d_queue.push_back(bslmf::MovableRefUtil::move(job));
    wakeThreadIfNeeded();
}

void ThreadPool::wakeThreadIfNeeded()
{
    if (d_waitHead) {
        // Signal this thread (used in 'bdlmt::ThreadPool::workerThread'
        // below).

        d_waitHead->d_hasJob = 1;
        d_waitHead->d_jobCond.signal();

        // And pop the current thread from the wait list.

        d_waitHead = d_waitHead->d_next.load();
        if (d_waitHead) {
            d_waitHead->d_prev = 0;
        }
    }
}

int ThreadPool::startThreadIfNeeded()
{
    if (static_cast<int>(d_queue.size()) + d_numActiveThreads > d_threadCount
       && d_threadCount < d_maxThreads) {
        int rc = startNewThread();
        (void)rc;  // Suppress unused variable warning.

        if (0 == d_threadCount) {
            // We are unable to spawn the first thread.  The enqueued job will
            // never be processed.  Return error.

            return -1;                                                // RETURN
        }

        // In our existing code base, many Linux users unknowingly configure
        // thread stack size to be 64 megabytes by default.  If they ask for a
        // lot of threads, they run out of RAM and thread creation eventually
        // fails.  But by then they have many threads and their jobs get
        // processed and things work.  To avoid disturbing such jobs in
        // production, work if 'startNewThread' failed as long as
        // '0 != d_threadCount'.  The following safe assert will alert clients
        // in development to the problem.

        BSLS_ASSERT_SAFE(0 == rc && "Client is not getting as many threads as"
            "requested, check thread stack size.");
    }
    return 0;
}

}  // close package namespace

#if defined(BSLS_PLATFORM_OS_UNIX)

namespace bdlmt {

void ThreadPool::initBlockSet()
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
    static const int SIZE =
                        sizeof synchronousSignals / sizeof *synchronousSignals;

    for (int i = 0; i < SIZE; ++i) {
        sigdelset(&d_blockSet, synchronousSignals[i]);
    }
}
}  // close package namespace
#endif

namespace bdlmt {

int ThreadPool::startNewThread()
{
    bslmt::ThreadUtil::Handle handle;

#if defined(BSLS_PLATFORM_OS_UNIX)
    // block all synchronous signals

    sigset_t oldset;

    pthread_sigmask(SIG_BLOCK, &d_blockSet, &oldset);
#endif

    bslma::Allocator *alloc = d_queue.get_allocator().mechanism();
    int rc = bslmt::ThreadUtil::createWithAllocator(&handle,
                                                    d_threadAttributes,
                                                    ThreadPoolEntry,
                                                    this,
                                                    alloc);

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

void ThreadPool::workerThread()
{
    ThreadPoolWaitNode waitNode;
    Job functor;
    while (1) {
        // The functor has to be cleared when we are *not* holding the lock
        // because it might have some objects bound with non-trivial
        // destructors.

        bool functorWasSetFlag = false;
        if (functor) {
            functor = Job();
            functorWasSetFlag = true;
        }

        {
            bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
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
                waitNode.d_next = d_waitHead.load();
                d_waitHead = &waitNode;

                // Let this thread wait until either there is a job available
                // or 'd_maxIdleTime' elapses.

                if (d_minThreads <= d_numActiveThreads) {
                    // This thread should be removed if it times out.

                    bsls::TimeInterval endTime =
                        bsls::SystemTime::nowMonotonicClock() + d_maxIdleTime;
                    do {
                        if (waitNode.d_jobCond.timedWait(&d_mutex, endTime)) {
                            // This thread timed out its max idle time.

                            break;
                        }

                        // Else we may either have been signaled or awakened
                        // spuriously.  In the latter case, loop.

                    } while (!waitNode.d_hasJob &&
                             bsls::SystemTime::nowMonotonicClock() < endTime);
                }
                else {
                    // This thread should not be subject to a timeout, in order
                    // to maintain the minimum number of threads.

                    while (0 == waitNode.d_hasJob) {
                        waitNode.d_jobCond.wait(&d_mutex);
                    }
                }

                if (0 == waitNode.d_hasJob ) {
                    // We haven't been signaled, so must have timed out.
                    // Remove this node from the wait list, but note that it
                    // will be put at the head of the list in the next
                    // iteration.

                    if (waitNode.d_next) {
                        waitNode.d_next->d_prev = waitNode.d_prev.load();
                    }
                    if (waitNode.d_prev) {
                        waitNode.d_prev->d_next = waitNode.d_next.load();
                    }
                    else {
                        d_waitHead = waitNode.d_next.load();
                    }

                    // In addition, in the following case, we may simply shut
                    // down this thread.

                    if (d_threadCount > d_minThreads) {
                        --d_threadCount;
                        return;                                       // RETURN
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
                return;                                               // RETURN
            }

            ++d_numActiveThreads;
        }

        // Run the callback and keep measurements.

        bsls::Types::Int64 start  = bsls::TimeUtil::getTimer();
        functor();
        bsls::Types::Int64 finish = bsls::TimeUtil::getTimer();
        if (start < d_lastResetTime) {
            d_callbackTime.add(finish - d_lastResetTime);
        }
        else {
            d_callbackTime.add(finish - start);
        }
    } // while (1)
}

// CREATORS
ThreadPool::ThreadPool(const bslmt::ThreadAttributes&  threadAttributes,
                       int                             minThreads,
                       int                             maxThreads,
                       int                             maxIdleTime,
                       bslma::Allocator               *basicAllocator)
: d_queue(basicAllocator)
, d_threadAttributes(threadAttributes, basicAllocator)
, d_maxThreads(maxThreads)
, d_minThreads(minThreads)
, d_threadCount(0)
, d_createFailures(0)
, d_numActiveThreads(0)
, d_enabled(0)
, d_waitHead(0)
, d_lastResetTime(bsls::TimeUtil::getTimer()) // now
{
    BSLS_ASSERT(0          <= minThreads);
    BSLS_ASSERT(minThreads <= maxThreads);
    BSLS_ASSERT(0          <= maxIdleTime);

    d_maxIdleTime.setTotalMilliseconds(maxIdleTime);

    if (d_threadAttributes.threadName().empty()) {
        d_threadAttributes.setThreadName(s_defaultThreadName);
    }

    // Force all threads to be detached.

    d_threadAttributes.setDetachedState(
                                   bslmt::ThreadAttributes::e_CREATE_DETACHED);

#if defined(BSLS_PLATFORM_OS_UNIX)
    initBlockSet();
#endif
}

ThreadPool::ThreadPool(const bslmt::ThreadAttributes&  threadAttributes,
                       int                             minThreads,
                       int                             maxThreads,
                       bsls::TimeInterval              maxIdleTime,
                       bslma::Allocator               *basicAllocator)
: d_queue(basicAllocator)
, d_threadAttributes(threadAttributes, basicAllocator)
, d_maxThreads(maxThreads)
, d_minThreads(minThreads)
, d_threadCount(0)
, d_createFailures(0)
, d_maxIdleTime(maxIdleTime)
, d_numActiveThreads(0)
, d_enabled(0)
, d_waitHead(0)
, d_lastResetTime(bsls::TimeUtil::getTimer()) // now
{
    BSLS_ASSERT(0                        <= minThreads);
    BSLS_ASSERT(minThreads               <= maxThreads);
    BSLS_ASSERT(bsls::TimeInterval(0, 0) <= maxIdleTime);
    BSLS_ASSERT(INT_MAX                  >= maxIdleTime.totalMilliseconds());

    if (d_threadAttributes.threadName().empty()) {
        d_threadAttributes.setThreadName(s_defaultThreadName);
    }

    // Force all threads to be detached.

    d_threadAttributes.setDetachedState(
                                   bslmt::ThreadAttributes::e_CREATE_DETACHED);

#if defined(BSLS_PLATFORM_OS_UNIX)
    initBlockSet();
#endif
}

ThreadPool::~ThreadPool()
{
    shutdown();
}

// MANIPULATORS
void ThreadPool::drain()
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
    d_enabled = 0;

    while ((d_threadCount && d_queue.size()) || d_numActiveThreads) {
        d_drainCond.wait(&d_mutex);
    }
}

int ThreadPool::enqueueJob(const Job& functor)
{
    if (!functor) {
        // Abort here if the 'functor' is "unset".  This prevents a crash
        // inside 'workerThread' (where the context of 'functor' would be
        // lost).

        BSLS_ASSERT(0);
        bsl::abort();  // abort (for when 'assert' is removed by optimization)
    }

    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
    if (!d_enabled) {
        return -1;                                                    // RETURN
    }

    doEnqueueJob(functor);

    return startThreadIfNeeded();
}

int ThreadPool::enqueueJob(bslmf::MovableRef<Job> functor)
{
    if (!bslmf::MovableRefUtil::access(functor)) {
        // Abort here if the 'functor' is "unset".  This prevents a crash
        // inside 'workerThread' (where the context of 'functor' would be
        // lost).

        BSLS_ASSERT(0);
        bsl::abort();  // abort (for when 'assert' is removed by optimization)
    }

    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
    if (!d_enabled) {
        return -1;                                                    // RETURN
    }

    doEnqueueJob(bslmf::MovableRefUtil::move(functor));

    return startThreadIfNeeded();
}

void ThreadPool::shutdown()
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
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

double ThreadPool::resetPercentBusy()
{
    bsls::Types::Int64 now           = bsls::TimeUtil::getTimer();
    bsls::Types::Int64 lastResetTime = d_lastResetTime.swap(now);
    const double callbackTime = static_cast<double>(d_callbackTime.swap(0));

    // On some platforms, the "nanosecond" timers can be too coarse and no time
    // is perceived to elapse; this sets the minimum elapsed time to 1ns.

    double interval = static_cast<double>(now - lastResetTime);
    interval = 0 != interval ? interval : 1;

    double percentBusy = 100.0 / d_maxThreads * callbackTime / interval;
    return percentBusy;
}

int ThreadPool::start()
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
    d_enabled = 1;

    while (d_threadCount < d_minThreads) {
        if (0 != startNewThread()) {
            lock.release()->unlock();
            shutdown(); // terminate running threads.
            return -1;                                                // RETURN
        }
    }
    return 0;
}

void ThreadPool::stop()
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
    d_enabled = 0;

    for (int i = 0; i < d_threadCount; ++i) {
        doEnqueueJob(Job());
    }
    while (d_threadCount) {
        d_drainCond.wait(&d_mutex);
    }
}

// ACCESSORS
int ThreadPool::numActiveThreads() const
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
    return d_numActiveThreads;
}

int ThreadPool::numWaitingThreads() const
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
    return d_threadCount - d_numActiveThreads;
}

int ThreadPool::numPendingJobs() const
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
    return static_cast<int>(d_queue.size());
}

double ThreadPool::percentBusy() const
{
    bsls::Types::Int64 last = d_lastResetTime;
    double interval = static_cast<double>(bsls::TimeUtil::getTimer() - last);

    // On some platforms, the "nanosecond" timers can be too coarse and no time
    // is perceived to elapse; this sets the minimum elapsed time to 1ns.

    interval = 0 != interval ? interval : 1;

    double ratio = static_cast<double>(d_callbackTime) / interval;
    double percentBusy = 100.0 / d_maxThreads * ratio;

    return percentBusy;
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
