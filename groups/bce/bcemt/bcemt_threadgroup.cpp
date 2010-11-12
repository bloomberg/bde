// bcemt_threadgroup.cpp   -*-C++-*-
#include <bcemt_threadgroup.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcemt_threadgroup_cpp,"$Id$ $CSID$")

#include <bcemt_lockguard.h>
#include <bcemt_semaphore.h>  // for testing only
#include <bdef_bind.h>
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
    bcemt_ThreadUtil::Handle d_handle;

  public:
    // CREATORS
    explicit
    ThreadDetachGuard(bcemt_ThreadUtil::Handle handle)
      : d_handle(handle)
      {
      }

    ~ThreadDetachGuard()
      {
          if (bcemt_ThreadUtil::invalidHandle() != d_handle) {
              bcemt_ThreadUtil::detach(d_handle);
          }
      }

    // MANIPULATORS
    void release()
      {
          d_handle = bcemt_ThreadUtil::invalidHandle();
      }
};

inline
void bindJoin(bcemt_ThreadUtil::Handle handle)
{
    // Call 'join' with a null 'status' argument.  Required b/c the AIX
    // compiler does not handle binding functions with default arguments
    // correctly.

    bcemt_ThreadUtil::join(handle, (void**)0);
}

}  // close unnamed namespace

                          // -----------------------
                          // class bcemt_ThreadGroup
                          // -----------------------

// CREATORS

bcemt_ThreadGroup::bcemt_ThreadGroup(bslma_Allocator *basicAllocator)
: d_numThreads(0)
, d_threads(basicAllocator)
{
}

bcemt_ThreadGroup::~bcemt_ThreadGroup()
{
    bsl::for_each(d_threads.begin(), d_threads.end(),
                  &bcemt_ThreadUtil::detach);
}

// MANIPULATORS

int bcemt_ThreadGroup::addThread(const bdef_Function<void(*)()>& functor,
                                 const bcemt_ThreadAttributes&   attributes)
{
    bcemt_ThreadUtil::Handle handle;
    int rc = 1;
    if (bcemt_ThreadAttributes::BCEMT_CREATE_JOINABLE !=
                                                  attributes.detachedState()) {
        bcemt_ThreadAttributes newAttributes(attributes);
        newAttributes.setDetachedState(
                                 bcemt_ThreadAttributes::BCEMT_CREATE_JOINABLE);
        rc = bcemt_ThreadUtil::create(&handle, newAttributes, functor);
    }
    else {
        rc = bcemt_ThreadUtil::create(&handle, attributes, functor);
    }

    if (0 == rc) {
        ThreadDetachGuard            detachGuard(handle);
        bcemt_LockGuard<bcemt_Mutex> lockGuard(&d_threadsMutex);

        d_threads.push_back(handle);
        d_numThreads.relaxedAdd(1);
        detachGuard.release();
    }
    return rc;
}

int bcemt_ThreadGroup::addThreads(const bdef_Function<void(*)()>& functor,
                                  int                             numThreads,
                                  const bcemt_ThreadAttributes&   attributes)
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

void bcemt_ThreadGroup::joinAll()
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_threadsMutex);

    bsl::for_each(d_threads.begin(), d_threads.end(), &bindJoin);
    d_threads.clear();
    d_numThreads.relaxedStore(0);
}

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007, 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
