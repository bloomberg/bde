// bdlqq_threadgroup.h                                                -*-C++-*-
#ifndef INCLUDED_BDLQQ_THREADGROUP
#define INCLUDED_BDLQQ_THREADGROUP

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a container for managing a group of threads
//
//@AUTHOR: David Schumann (dschumann1)
//
//@CLASSES:
// bdlqq::ThreadGroup: A container that manages a group of threads.
//
//@SEE_ALSO: bdlqq_xxxthread
//
//@DESCRIPTION: This component provides a simple mechanism for managing a group
// of threads.  The group is represented by an instance of the
// 'bdlqq::ThreadGroup' class.  To use this component, the client code calls
// 'addThread', providing a function to be executed.  The specified function is
// executed in a new thread managed by the thread group (note that 'addThread'
// is thread-safe).  The 'joinAll' call blocks until all threads in the group
// have finished executing.
//
///Thread Safety
///-------------
// This component is thread-safe and thread-enabled, meaning that multiple
// threads may safely use their own instances or a shared instance of a
// 'bdlqq::ThreadGroup' object.
//
///Usage
///-----
// The following usage example illustrates how 'bdlqq::ThreadGroup' might be
// used in a typical test driver to simplify the execution of a common
// function in multiple threads.  Suppose that we are interested in creating a
// stress-test for the 'bdlqq::Mutex' class.  The test is controlled by two
// parameters: the number of executions (defined by subsequent calls to 'lock'
// and 'unlock', and the amount of contention, defined by the number of threads
// accessing the mutex.  The test can be expressed as two functions.  The first
// is executed in each thread:
//..
//  void testMutex(
//          int         *value;
//          bdlqq::Mutex *mutex,
//          int          numIterations)
//  {
//      while (numIterations--) {
//          mutex->lock();
//          ++*value;
//          mutex->unlock();
//      }
//  }
//..
// The second executes the main body of the test:
//..
//  bslma::TestAllocator ta;
//  {
//      const int NUM_ITERATIONS = 10000;
//      const int NUM_THREADS    = 8;
//
//      bdlqq::Mutex   mutex;                     // object under test
//      int           value = 0;
//
//      bdlf::Function<void(*)()> testFunc =
//        bdlf::BindUtil::bind(&testMutex, &value, &mutex, NUM_ITERATIONS);
//
//      bdlqq::ThreadGroup tg(&ta);
//      for (int i = 0; i < NUM_THREADS; ++i) {
//          ASSERT(0 == tg.addThread(testFunc));
//      }
//      tg.joinAll();
//      ASSERT(NUM_ITERATIONS * NUM_THREADS == value);
//  }
//  ASSERT(0 <  ta.numAllocations());
//  ASSERT(0 == ta.numBytesInUse());
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLS_ATOMIC
#include <bsls_atomic.h>
#endif

#ifndef INCLUDED_BDLQQ_MUTEX
#include <bdlqq_mutex.h>
#endif

#ifndef INCLUDED_BDLQQ_THREADATTRIBUTES
#include <bdlqq_threadattributes.h>
#endif

#ifndef INCLUDED_BDLQQ_THREADUTIL
#include <bdlqq_threadutil.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BDLF_FUNCTION
#include <bdlf_function.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

namespace bdlqq {
                           // ======================
                           // class bcep::ThreadGroup
                           // ======================

class ThreadGroup {
    // This class provides a simple mechanism for managing a group of joinable
    // threads.  The destructor ensures that any running threads are detached
    // so that resources are not leaked.  This class is thread-enabled,
    // thread-safe, and exception-neutral.

    // PRIVATE TYPES
    typedef bsl::vector<ThreadUtil::Handle> ThreadContainer;

    // INSTANCE DATA
    bsls::AtomicInt    d_numThreads;
    ThreadContainer   d_threads;
    Mutex       d_threadsMutex;

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
        // Create an empty thread group.  Optionally specify
        // 'basicAllocator' will be used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator
        // will be used.

    ~ThreadGroup();
        // Destroy this object.  Any threads not joined will be allowed to
        // run independently, and will no longer be joinable.

    // MANIPULATORS
    int addThread(const bdlf::Function<void(*)()>& functor);
    int addThread(const bdlf::Function<void(*)()>& functor,
                  const ThreadAttributes&   attributes);
        // Begin executing the specified invokable 'functor' in a new thread,
        // using the optionally specified thread 'attributes'.  Return 0 on
        // success, and a non-zero value otherwise.  Note that threads are
        // always created joinable, regardless of the mode specified in
        // 'attributes'.

    int addThreads(const bdlf::Function<void(*)()>& functor,
                   int                             numThreads);
    int addThreads(const bdlf::Function<void(*)()>& functor,
                   int                             numThreads,
                   const ThreadAttributes&   attributes);
        // Begin executing the specified invokable 'functor' in the specified
        // new 'numThreads', using the specified thread 'attributes'.  Return
        // the number of threads added.  The behavior is undefined unless
        // '0 <= numThreads'.  Note that threads are always created joinable,
        // regardless of the mode specified in 'attributes'.

    void joinAll();
        // Block the calling thread until all threads started in this group
        // have finished executing.

    // ACCESSORS
    int numThreads() const;
        // Return a snapshot of the number of threads started in this group
        // that have not been joined.
};

// ============================================================================
//                          INLINE FUNCTION DEFINITIONS
// ============================================================================

                           // ----------------------
                           // class bcep::ThreadGroup
                           // ----------------------

// MANIPULATORS
inline
int ThreadGroup::addThread(const bdlf::Function<void(*)()>& functor)
{
    return addThread(functor, ThreadAttributes());
}

inline
int ThreadGroup::addThreads(const bdlf::Function<void(*)()>& functor,
                                  int                             numThreads)
{
    return addThreads(functor, numThreads, ThreadAttributes());
}

// ACCESSORS
inline
int ThreadGroup::numThreads() const
{
   return d_numThreads.loadRelaxed();
}
}  // close package namespace

}  // close namespace BloombergLP
#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
