// bslstl_stopstate.cpp                                               -*-C++-*-
#include <bslstl_stopstate.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslstl_stopstate_cpp, "$Id$ $CSID$")

#include <bsls_platform.h>

#ifdef BSLS_PLATFORM_OS_WINDOWS
typedef unsigned long DWORD;
typedef int           BOOL;
extern "C" {
    __declspec(dllimport) DWORD __stdcall GetCurrentThreadId(void);
    __declspec(dllimport) DWORD __stdcall SleepEx(DWORD dwMilliseconds,
                                                  BOOL  bAlertable);
};
#else
#include <pthread.h>
#include <sched.h>
#endif

namespace BloombergLP {
namespace bslstl {
namespace {

unsigned long long currentThreadId()
    // Return the thread ID of the calling thread.  We cannot depend on
    // 'bslmt_threadutil', so we reimplement this functionality.
{
#if defined(BSLS_PLATFORM_OS_WINDOWS)
    return GetCurrentThreadId();
#elif defined(BSLS_PLATFORM_OS_DARWIN)
    return reinterpret_cast<unsigned long long>(pthread_self());
#else
    return static_cast<unsigned long long>(pthread_self());
#endif
}

void yield()
    // Yield the current time slice to the scheduler.  We cannot depend on
    // 'bslmt_threadutil', so we reimplement this functionality.
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    SleepEx(0, 0);
#else
    sched_yield();
#endif
}

}  // close unnamed namespace

                              // ---------------
                              // class StopState
                              // ---------------

// PRIVATE MANIPULATORS
void StopState::unlink(StopState_ListNode *node)
{
    node->d_next_p->d_prev_p = node->d_prev_p;
    node->d_prev_p->d_next_p = node->d_next_p;
    node->d_next_p = 0;
}

// CREATORS
StopState::StopState()
{
    d_head.d_prev_p = &d_head;
    d_head.d_next_p = &d_head;
}

void StopState::deregister(StopStateCallbackNode *node)
{
    {
        bsls::BslLockGuard stateGuard(&d_stateMutex);
        if (node->d_next_p) {
            unlink(node);
            return;                                                   // RETURN
        }
    }
    // Note that 'd_stoppingThread' does not need to be atomic.  If we reach
    // this line, it means the stopping thread has unlinked us, which means it
    // has already finished writing to 'd_stoppingThread'.  'd_stateMutex'
    // provides the necessary synchronization.
    if (d_stoppingThread == currentThreadId()) {
        // We are being called from the callback itself.  Obviously, we should
        // not wait for the callback to complete.  We also need to be sure not
        // to try to modify 'd_finished' when we return.
        d_currentCallback_p = 0;
        return;                                                       // RETURN
    }
    while (!node->d_finished.loadAcquire()) {
        yield();
    }
}

bool StopState::enregister(StopStateCallbackNode *node)
{
    bsls::BslLockGuard lg(&d_stateMutex);
    if (d_stopRequested.loadRelaxed()) {
        lg.release();
        d_stateMutex.unlock();
        node->invoke();
        return false;                                                 // RETURN
    }
    node->d_prev_p = d_head.d_prev_p;
    node->d_next_p = &d_head;
    d_head.d_prev_p->d_next_p = node;
    d_head.d_prev_p = node;
    return true;
}

bool StopState::requestStop()
{
    if (d_stopRequested.testAndSwapAcqRel(false, true)) {
        return false;                                                 // RETURN
    }
    d_stoppingThread = currentThreadId();
    while (true) {
        StopState_ListNode *currentNode;
        {
            bsls::BslLockGuard stateGuard(&d_stateMutex);
            currentNode = d_head.d_next_p;
            if (&d_head == currentNode) {
                // no more callbacks
                return true;                                          // RETURN
            }
            unlink(currentNode);
            // Do not hold the state lock while invoking the callback, because
            // other callbacks might need to deregister themselves.
        }
        d_currentCallback_p =
                             static_cast<StopStateCallbackNode *>(currentNode);
        d_currentCallback_p->invoke();
        if (d_currentCallback_p) {
            d_currentCallback_p->d_finished.storeRelease(true);
        }
    }
}

// ACCESSORS
bool StopState::stopRequested() const
{
    return d_stopRequested.loadAcquire();
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2023 Bloomberg Finance L.P.
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
