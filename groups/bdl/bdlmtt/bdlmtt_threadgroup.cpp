// bdlmtt_threadgroup.cpp   -*-C++-*-
#include <bdlmtt_threadgroup.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmtt_threadgroup_cpp,"$Id$ $CSID$")

#include <bdlmtt_lockguard.h>
#include <bdlmtt_semaphore.h>  // for testing only
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
    bdlmtt::ThreadUtil::Handle d_handle;

  public:
    // CREATORS
    explicit
    ThreadDetachGuard(bdlmtt::ThreadUtil::Handle handle)
      : d_handle(handle)
      {
      }

    ~ThreadDetachGuard()
      {
          if (bdlmtt::ThreadUtil::invalidHandle() != d_handle) {
              bdlmtt::ThreadUtil::detach(d_handle);
          }
      }

    // MANIPULATORS
    void release()
      {
          d_handle = bdlmtt::ThreadUtil::invalidHandle();
      }
};

inline
void bindJoin(bdlmtt::ThreadUtil::Handle handle)
{
    // Call 'join' with a null 'status' argument.  Required b/c the AIX
    // compiler does not handle binding functions with default arguments
    // correctly.

    bdlmtt::ThreadUtil::join(handle, (void**)0);
}

}  // close unnamed namespace

namespace bdlmtt {
                          // -----------------------
                          // class ThreadGroup
                          // -----------------------

// CREATORS

ThreadGroup::ThreadGroup(bslma::Allocator *basicAllocator)
: d_numThreads(0)
, d_threads(basicAllocator)
{
}

ThreadGroup::~ThreadGroup()
{
    bsl::for_each(d_threads.begin(), d_threads.end(),
                  &ThreadUtil::detach);
}

// MANIPULATORS

int ThreadGroup::addThread(const bdlf::Function<void(*)()>& functor,
                                 const ThreadAttributes&   attributes)
{
    ThreadUtil::Handle handle;
    int rc = 1;
    if (ThreadAttributes::BCEMT_CREATE_JOINABLE !=
                                                  attributes.detachedState()) {
        ThreadAttributes newAttributes(attributes);
        newAttributes.setDetachedState(
                                ThreadAttributes::BCEMT_CREATE_JOINABLE);
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

int ThreadGroup::addThreads(const bdlf::Function<void(*)()>& functor,
                                  int                             numThreads,
                                  const ThreadAttributes&   attributes)
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

void ThreadGroup::joinAll()
{
    LockGuard<Mutex> guard(&d_threadsMutex);

    bsl::for_each(d_threads.begin(), d_threads.end(), &bindJoin);
    d_threads.clear();
    d_numThreads.storeRelaxed(0);
}
}  // close package namespace

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007, 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
