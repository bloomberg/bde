// bdlqq_threadgroup.cpp                                              -*-C++-*-
#include <bdlqq_threadgroup.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlqq_threadgroup_cpp,"$Id$ $CSID$")

#include <bdlqq_lockguard.h>
#include <bdlqq_semaphore.h>  // for testing only
#include <bdlf_bind.h>
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
    bdlqq::ThreadUtil::Handle d_handle;

  public:
    // CREATORS
    explicit
    ThreadDetachGuard(bdlqq::ThreadUtil::Handle handle)
      : d_handle(handle)
      {
      }

    ~ThreadDetachGuard()
      {
          if (bdlqq::ThreadUtil::invalidHandle() != d_handle) {
              bdlqq::ThreadUtil::detach(d_handle);
          }
      }

    // MANIPULATORS
    void release()
      {
          d_handle = bdlqq::ThreadUtil::invalidHandle();
      }
};

inline
void bindJoin(bdlqq::ThreadUtil::Handle handle)
{
    // Call 'join' with a null 'status' argument.  Required b/c the AIX
    // compiler does not handle binding functions with default arguments
    // correctly.

    bdlqq::ThreadUtil::join(handle, (void**)0);
}

}  // close unnamed namespace

                             // -----------------
                             // class ThreadGroup
                             // -----------------

// CREATORS

bdlqq::ThreadGroup::ThreadGroup(bslma::Allocator *basicAllocator)
: d_numThreads(0)
, d_threads(basicAllocator)
{
}

bdlqq::ThreadGroup::~ThreadGroup()
{
    bsl::for_each(d_threads.begin(), d_threads.end(),
                  &ThreadUtil::detach);
}

// MANIPULATORS

int bdlqq::ThreadGroup::addThread(const bdlf::Function<void(*)()>& functor,
                                  const ThreadAttributes&          attributes)
{
    ThreadUtil::Handle handle;
    int rc = 1;
    if (ThreadAttributes::e_CREATE_JOINABLE !=
                                                  attributes.detachedState()) {
        ThreadAttributes newAttributes(attributes);
        newAttributes.setDetachedState(
                                ThreadAttributes::e_CREATE_JOINABLE);
        rc = ThreadUtil::create(&handle, newAttributes, functor);
    }
    else {
        rc = ThreadUtil::create(&handle, attributes, functor);
    }

    if (0 == rc) {
        ThreadDetachGuard            detachGuard(handle);
        LockGuard<Mutex> lockGuard(&d_threadsMutex);

        d_threads.push_back(handle);
        d_numThreads.addRelaxed(1);
        detachGuard.release();
    }
    return rc;
}

int bdlqq::ThreadGroup::addThreads(const bdlf::Function<void(*)()>& functor,
                                   int                              numThreads,
                                   const ThreadAttributes&          attributes)
{
    BSLS_ASSERT(0 <= numThreads);

    int numAdded = 0;
    for (int i = 0; i < numThreads; ++i) {
        if (0 != addThread(functor, attributes)) {
            break;
        }
        ++numAdded;
    }
    return numAdded;
}

void bdlqq::ThreadGroup::joinAll()
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
