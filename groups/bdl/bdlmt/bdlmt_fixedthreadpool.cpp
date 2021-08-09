// bdlmt_fixedthreadpool.cpp                                          -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlmt_fixedthreadpool.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmt_fixedthreadpool_cpp,"$Id$ $CSID$")

#include <bdlf_memfn.h>

#include <bsls_nullptr.h>
#include <bsls_performancehint.h>
#include <bsls_platform.h>
#include <bsls_timeutil.h>

#if defined(BSLS_PLATFORM_OS_UNIX)
#include <bsl_c_signal.h>              // sigfillset
#endif

#include <bsl_functional.h>

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
}  // close unnamed namespace

namespace BloombergLP {
namespace bdlmt {

                          // ---------------------
                          // class FixedThreadPool
                          // ---------------------

// PRIVATE MANIPULATORS
void FixedThreadPool::workerThread()
{
    d_barrier.wait();  // initial synchronization in 'start'

    Job functor;

    do {
        if (d_drainFlag) {
            d_barrier.wait();  // pool threads acknowledge drain
            d_barrier.wait();  // pool threads may proceed
        }
        while (Queue::e_SUCCESS == d_queue.popFront(&functor)) {
            d_numActiveThreads.addAcqRel(1);
            functor();
            functor = bsl::nullptr_t();  // ensure destructor is called
            d_numActiveThreads.addAcqRel(-1);
        }
    } while (d_drainFlag);
}

int FixedThreadPool::startNewThread()
{
#if defined(BSLS_PLATFORM_OS_UNIX)
    // Block all asynchronous signals.

    sigset_t oldset;
    pthread_sigmask(SIG_BLOCK, &d_blockSet, &oldset);
#endif

    bsl::function<void()> workerThreadFunc =
                  bdlf::MemFnUtil::memFn(&FixedThreadPool::workerThread, this);

    int rc = d_threadGroup.addThread(workerThreadFunc, d_threadAttributes);

#if defined(BSLS_PLATFORM_OS_UNIX)
    // Restore the mask.

    pthread_sigmask(SIG_SETMASK, &oldset, &d_blockSet);
#endif

    return rc;
}

// CREATORS
FixedThreadPool::FixedThreadPool(
                             const bslmt::ThreadAttributes&  threadAttributes,
                             int                             numThreads,
                             int                             maxNumPendingJobs,
                             bslma::Allocator               *basicAllocator)
: d_queue(maxNumPendingJobs, basicAllocator)
, d_numActiveThreads(0)
, d_drainFlag(false)
, d_barrier(numThreads + 1)
, d_threadGroup(basicAllocator)
, d_threadAttributes(threadAttributes, basicAllocator)
, d_numThreads(numThreads)
{
    BSLS_ASSERT_OPT(1 <= numThreads);

    d_queue.disablePushBack();
    d_queue.disablePopFront();

#if defined(BSLS_PLATFORM_OS_UNIX)
    initBlockSet(&d_blockSet);
#endif
}

FixedThreadPool::FixedThreadPool(int               numThreads,
                                 int               maxNumPendingJobs,
                                 bslma::Allocator *basicAllocator)
: d_queue(maxNumPendingJobs, basicAllocator)
, d_numActiveThreads(0)
, d_drainFlag(false)
, d_barrier(numThreads + 1)
, d_threadGroup(basicAllocator)
, d_threadAttributes(basicAllocator)
, d_numThreads(numThreads)
{
    BSLS_ASSERT_OPT(1 <= numThreads);

    d_queue.disablePushBack();
    d_queue.disablePopFront();

#if defined(BSLS_PLATFORM_OS_UNIX)
    initBlockSet(&d_blockSet);
#endif
}

FixedThreadPool::~FixedThreadPool()
{
    shutdown();
}

// MANIPULATORS
int FixedThreadPool::start()
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_metaMutex);

    if (!d_queue.isPopFrontDisabled()) {
        return 0;                                                     // RETURN
    }

    for (int i = 0; i < d_numThreads; ++i)  {
        if (0 != startNewThread()) {
            // Submit a sufficient number of arrivals to the barrier to release
            // all threads ('d_numThreads + 1');

            for (int j = i ; j <= d_numThreads; ++j) {
                d_barrier.arrive();
            }

            d_threadGroup.joinAll();
            return -1;                                                // RETURN
        }
    }

    d_queue.enablePopFront();
    d_queue.enablePushBack();

    d_barrier.wait();

    return 0;
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2021 Bloomberg Finance L.P.
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
