// bslmt_threadgroup.cpp                                              -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bslmt_threadgroup.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslmt_threadgroup_cpp,"$Id$ $CSID$")

#include <bslmt_lockguard.h>
#include <bslmt_semaphore.h>  // for testing only
#include <bslma_default.h>

#include <bsl_algorithm.h>

#include <bsls_assert.h>

namespace BloombergLP {
namespace {

                         // =======================
                         // class ThreadDetachGuard
                         // =======================

class ThreadDetachGuard {

    // INSTANCE DATA
    bslmt::ThreadUtil::Handle d_handle;

  public:
    // CREATORS
    explicit
    ThreadDetachGuard(bslmt::ThreadUtil::Handle handle)
    : d_handle(handle)
    {
    }

    ~ThreadDetachGuard()
    {
        if (bslmt::ThreadUtil::invalidHandle() != d_handle) {
            bslmt::ThreadUtil::detach(d_handle);
        }
    }

    // MANIPULATORS
    void release()
    {
        d_handle = bslmt::ThreadUtil::invalidHandle();
    }
};

inline
void bindJoin(bslmt::ThreadUtil::Handle handle)
{
    // Call 'join' with a null 'status' argument.  Required b/c the AIX
    // compiler does not handle binding functions with default arguments
    // correctly.

    bslmt::ThreadUtil::join(handle, (void**)0);
}

}  // close unnamed namespace

                            // -----------------
                            // class ThreadGroup
                            // -----------------

// CREATORS

bslmt::ThreadGroup::ThreadGroup(bslma::Allocator *basicAllocator)
: d_numThreads(0)
, d_threads(basicAllocator)
{
}

bslmt::ThreadGroup::~ThreadGroup()
{
    bsl::for_each(d_threads.begin(), d_threads.end(),
                  &ThreadUtil::detach);
}

// MANIPULATORS
void bslmt::ThreadGroup::addThread(const ThreadUtil::Handle& handle) {
    ThreadDetachGuard detachGuard(handle);
    LockGuard<Mutex> lockGuard(&d_threadsMutex);

    d_threads.push_back(handle);
    d_numThreads.addRelaxed(1);
    detachGuard.release();
}

void bslmt::ThreadGroup::joinAll()
{
    LockGuard<Mutex> guard(&d_threadsMutex);

    bsl::for_each(d_threads.begin(), d_threads.end(), &bindJoin);
    d_threads.clear();
    d_numThreads.storeRelaxed(0);
}

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
