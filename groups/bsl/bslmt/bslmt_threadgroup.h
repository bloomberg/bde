// bslmt_threadgroup.h                                                -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BSLMT_THREADGROUP
#define INCLUDED_BSLMT_THREADGROUP

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a container for managing a group of threads.
//
//@CLASSES:
// bslmt::ThreadGroup: A container that manages a group of threads.
//
//@SEE_ALSO: bslmt_threadutil, bslmt_threadattributes
//
//@DESCRIPTION: This component provides a simple mechanism for managing a group
// of threads.  The group is represented by an instance of the
// 'bslmt::ThreadGroup' class.  To use this component, the client code calls
// 'addThread', providing a function to be executed.  The specified function is
// executed in a new thread managed by the thread group (note that 'addThread'
// is thread-safe).  The 'joinAll' call blocks until all threads in the group
// have finished executing.
//
///Thread Safety
///-------------
// This component is thread-safe and thread-enabled, meaning that multiple
// threads may safely use their own instances or a shared instance of a
// 'bslmt::ThreadGroup' object.
//
///Usage
///-----
// The following usage example illustrates how 'bslmt::ThreadGroup' might be
// used in a typical test driver to simplify the execution of a common function
// in multiple threads.  Suppose that we are interested in creating a
// stress-test for the 'bslmt::Mutex' class.  The test is controlled by two
// parameters: the number of executions (defined by subsequent calls to 'lock'
// and 'unlock', and the amount of contention, defined by the number of threads
// accessing the mutex.  The test can be expressed as two functions.  The first
// is executed in each thread via a functor object:
//..
//  class MutexTestJob {
//      int           d_numIterations;
//      int          *d_value_p;
//      bslmt::Mutex *d_mutex_p;
//
//    public:
//      MutexTestJob(int numIterations, int *value, bslmt::Mutex *mutex)
//      : d_numIterations(numIterations)
//      , d_value_p(value)
//      , d_mutex_p(mutex)
//      {}
//
//      void operator()() {
//          for (int i = 0; i < d_numIterations; ++i) {
//              bslmt::LockGuard<bslmt::Mutex> guard(d_mutex_p);
//              ++*d_value_p;
//          }
//      }
//  };
//..
// The second executes the main body of the test:
//..
//  bslma::TestAllocator ta;
//  {
//      const int NUM_ITERATIONS = 10000;
//      const int NUM_THREADS    = 8;
//
//      bslmt::Mutex   mutex;                     // object under test
//      int            value = 0;
//
//      MutexTestJob testJob(NUM_ITERATIONS, &value, &mutex);
//
//      bslmt::ThreadGroup threadGroup(&ta);
//      for (int i = 0; i < NUM_THREADS; ++i) {
//          assert(0 == threadGroup.addThread(testJob));
//      }
//      threadGroup.joinAll();
//      assert(NUM_ITERATIONS * NUM_THREADS == value);
//  }
//  assert(0 <  ta.numAllocations());
//  assert(0 == ta.numBytesInUse());
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_ATOMIC
#include <bsls_atomic.h>
#endif

#ifndef INCLUDED_BSLMT_MUTEX
#include <bslmt_mutex.h>
#endif

#ifndef INCLUDED_BSLMT_THREADATTRIBUTES
#include <bslmt_threadattributes.h>
#endif

#ifndef INCLUDED_BSLMT_THREADUTIL
#include <bslmt_threadutil.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {
namespace bslmt {

                            // =================
                            // class ThreadGroup
                            // =================

class ThreadGroup {
    // This class provides a simple mechanism for managing a group of joinable
    // threads.  The destructor ensures that any running threads are detached
    // so that resources are not leaked.  This class is thread-enabled,
    // thread-safe, and exception-neutral.

    // PRIVATE TYPES
    typedef bsl::vector<ThreadUtil::Handle> ThreadContainer;

    // INSTANCE DATA
    bsls::AtomicInt d_numThreads;
    ThreadContainer d_threads;
    Mutex           d_threadsMutex;

    // PRIVATE MANIPULATORS
    void addThread(const ThreadUtil::Handle& handle);
        // Add the specified 'handle' to the 'd_threads' container.  If an
        // exception is thrown, 'handle' will be released.

  private:
    // not implemented
    ThreadGroup(const ThreadGroup&);
    ThreadGroup& operator=(const ThreadGroup&);

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(ThreadGroup,
                                 bslalg::TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit
    ThreadGroup(bslma::Allocator *basicAllocator = 0);
        // Create an empty thread group.  Optionally specify 'basicAllocator'
        // will be used to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator will be used.

    ~ThreadGroup();
        // Destroy this object.  Any threads not joined will be allowed to run
        // independently, and will no longer be joinable.

    // MANIPULATORS
    template <class INVOKABLE>
    int addThread(const INVOKABLE& functor);
    template <class INVOKABLE>
    int addThread(const INVOKABLE&        functor,
                  const ThreadAttributes& attributes);
        // Begin executing the specified invokable 'functor' in a new thread,
        // using the optionally specified thread 'attributes'.  Return 0 on
        // success, and a non-zero value otherwise.  'INVOKABLE' shall be a
        // copy-constructible type having the equivalent of
        // 'void operator()()'.  Note that threads are always created joinable,
        // regardless of the mode specified in 'attributes'.

    template <class INVOKABLE>
    int addThreads(const INVOKABLE& functor, int numThreads);
    template <class INVOKABLE>
    int addThreads(const INVOKABLE&        functor,
                   int                     numThreads,
                   const ThreadAttributes& attributes);
        // Begin executing the specified invokable 'functor' in the specified
        // new 'numThreads', using the optionally specified thread
        // 'attributes'.  Return 'numThreads' on success, or the number of
        // threads successfully started otherwise.  'INVOKABLE' shall be a
        // copy-constructible type having the equivalent of
        // 'void operator()()'.  Note that threads are always created joinable,
        // regardless of the mode specified in 'attributes'.

    void joinAll();
        // Block the calling thread until all threads started in this group
        // have finished executing.

    // ACCESSORS
    int numThreads() const;
        // Return a snapshot of the number of threads started in this group
        // that have not been joined.
};

}  // close package namespace

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                            // -----------------
                            // class ThreadGroup
                            // -----------------

// MANIPULATORS
template<class INVOKABLE>
inline
int bslmt::ThreadGroup::addThread(const INVOKABLE& functor)
{
    return addThread(functor, ThreadAttributes());
}

template<class INVOKABLE>
inline
int bslmt::ThreadGroup::addThreads(const INVOKABLE& functor, int numThreads)
{
    return addThreads(functor, numThreads, ThreadAttributes());
}

template<class INVOKABLE>
int bslmt::ThreadGroup::addThread(const INVOKABLE&        functor,
                                  const ThreadAttributes& attributes)
{
    ThreadUtil::Handle handle;
    int rc = 1;
    if (ThreadAttributes::e_CREATE_JOINABLE != attributes.detachedState()) {
        ThreadAttributes newAttributes(attributes);
        newAttributes.setDetachedState(
                                ThreadAttributes::e_CREATE_JOINABLE);
        rc = ThreadUtil::create(&handle, newAttributes, functor);
    }
    else {
        rc = ThreadUtil::create(&handle, attributes, functor);
    }

    if (0 == rc) {
        addThread(handle);
    }
    return rc;
}

template<class INVOKABLE>
int bslmt::ThreadGroup::addThreads(const INVOKABLE&        functor,
                                   int                     numThreads,
                                   const ThreadAttributes& attributes)
{
    BSLS_ASSERT(0 <= numThreads);

    int numAdded;
    for (numAdded = 0; numAdded < numThreads; ++numAdded) {
        if (0 != addThread(functor, attributes)) {
            break;
        }
    }
    return numAdded;
}

// ACCESSORS
inline
int bslmt::ThreadGroup::numThreads() const
{
   return d_numThreads.loadRelaxed();
}

}  // close enterprise namespace

#endif

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
