// bcemt_threadgroup.h                                                -*-C++-*-
#ifndef INCLUDED_BCEMT_THREADGROUP
#define INCLUDED_BCEMT_THREADGROUP

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a container for managing a group of threads
//
//@AUTHOR: David Schumann (dschumann1)
//
//@CLASSES:
// bcemt_ThreadGroup: A container that manages a group of threads.
//
//@SEE_ALSO: bcemt_thread
//
//@DESCRIPTION: This component provides a simple mechanism for managing a group
// of threads.  The group is represented by an instance of the
// 'bcemt_ThreadGroup' class.  To use this component, the client code calls
// 'addThread', providing a function to be executed.  The specified function is
// executed in a new thread managed by the thread group (note that 'addThread'
// is thread-safe).  The 'joinAll' call blocks until all threads in the group
// have finished executing.
//
///Thread Safety
///-------------
// This component is thread-safe and thread-enabled, meaning that multiple
// threads may safely use their own instances or a shared instance of a
// 'bcemt_ThreadGroup' object.
//
///Usage
///-----
// The following usage example illustrates how 'bcemt_ThreadGroup' might be
// used in a typical test driver to simplify the execution of a common
// function in multiple threads.  Suppose that we are interested in creating a
// stress-test for the 'bcemt_Mutex' class.  The test is controlled by two
// parameters: the number of executions (defined by subsequent calls to 'lock'
// and 'unlock', and the amount of contention, defined by the number of threads
// accessing the mutex.  The test can be expressed as two functions.  The first
// is executed in each thread:
//..
//  void testMutex(
//          int         *value;
//          bcemt_Mutex *mutex,
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
//  bslma_TestAllocator ta;
//  {
//      const int NUM_ITERATIONS = 10000;
//      const int NUM_THREADS    = 8;
//
//      bcemt_Mutex   mutex;                     // object under test
//      int           value = 0;
//
//      bdef_Function<void(*)()> testFunc =
//        bdef_BindUtil::bind(&testMutex, &value, &mutex, NUM_ITERATIONS);
//
//      bcemt_ThreadGroup tg(&ta);
//      for (int i = 0; i < NUM_THREADS; ++i) {
//          ASSERT(0 == tg.addThread(testFunc));
//      }
//      tg.joinAll();
//      ASSERT(NUM_ITERATIONS * NUM_THREADS == value);
//  }
//  ASSERT(0 <  ta.numAllocation());
//  ASSERT(0 == ta.numBytesInUse());
//..

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCES_ATOMICTYPES
#include <bces_atomictypes.h>
#endif

#ifndef INCLUDED_BCEMT_MUTEX
#include <bcemt_mutex.h>
#endif

#ifndef INCLUDED_BCEMT_THREADATTRIBUTES
#include <bcemt_threadattributes.h>
#endif

#ifndef INCLUDED_BCEMT_THREADUTIL
#include <bcemt_threadutil.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BDEF_FUNCTION
#include <bdef_function.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef INCLUDED_BSLFWD_BSLMA_ALLOCATOR
#include <bslfwd_bslma_allocator.h>
#endif


namespace BloombergLP {

                           // ======================
                           // class bcep_ThreadGroup
                           // ======================

class bcemt_ThreadGroup {
    // This class provides a simple mechanism for managing a group of joinable
    // threads.  The destructor ensures that any running threads are detached
    // so that resources are not leaked.  This class is thread-enabled,
    // thread-safe, and exception-neutral.

    // PRIVATE TYPES
    typedef bsl::vector<bcemt_ThreadUtil::Handle> ThreadContainer;

    // INSTANCE DATA
    bces_AtomicInt    d_numThreads;
    ThreadContainer   d_threads;
    bcemt_Mutex       d_threadsMutex;

  private:
    // not implemented
    bcemt_ThreadGroup(const bcemt_ThreadGroup&);
    bcemt_ThreadGroup& operator=(const bcemt_ThreadGroup&);

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bcemt_ThreadGroup,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit
    bcemt_ThreadGroup(bslma_Allocator *basicAllocator = 0);
        // Create an empty thread group.  Optionally specify
        // 'basicAllocator' will be used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator
        // will be used.

    ~bcemt_ThreadGroup();
        // Destroy this object.  Any threads not joined will be allowed to
        // run independently, and will no longer be joinable.

    // MANIPULATORS
    int addThread(const bdef_Function<void(*)()>& functor);
    int addThread(const bdef_Function<void(*)()>& functor,
                  const bcemt_ThreadAttributes&   attributes);
        // Begin executing the specified invokable 'functor' in a new thread,
        // using the optionally specified thread 'attributes'.  Return 0 on
        // success, and a non-zero value otherwise.  Note that threads are
        // always created joinable, regardless of the mode specified in
        // 'attributes'.

    int addThreads(const bdef_Function<void(*)()>& functor,
                   int                             numThreads);
    int addThreads(const bdef_Function<void(*)()>& functor,
                   int                             numThreads,
                   const bcemt_ThreadAttributes&   attributes);
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
                           // class bcep_ThreadGroup
                           // ----------------------

// MANIPULATORS
inline
int bcemt_ThreadGroup::addThread(const bdef_Function<void(*)()>& functor)
{
    return addThread(functor, bcemt_ThreadAttributes());
}

inline
int bcemt_ThreadGroup::addThreads(const bdef_Function<void(*)()>& functor,
                                  int                             numThreads)
{
    return addThreads(functor, numThreads, bcemt_ThreadAttributes());
}

// ACCESSORS
inline
int bcemt_ThreadGroup::numThreads() const
{
   return d_numThreads.relaxedLoad();
}

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
