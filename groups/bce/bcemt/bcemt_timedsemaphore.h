// bcemt_timedsemaphore.h                                             -*-C++-*-
#ifndef INCLUDED_BCEMT_TIMEDSEMAPHORE
#define INCLUDED_BCEMT_TIMEDSEMAPHORE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a timed semaphore class.
//
//@CLASSES:
//  bcemt_TimedSemaphore: timed semaphore class
//
//@SEE_ALSO: bcemt_semaphore
//
//@AUTHOR: Guillaume Morin (gmorin1)
//
//@DESCRIPTION: This component defines a portable and efficient thread
// synchronization primitive.  In particular, 'bcemt_TimedSemaphore' is an
// efficient synchronization primitive that enables sharing of a counted number
// of resources or exclusive access.
//
// 'bcemt_TimedSemaphore' differs from 'bcemt_Semaphore' in that the former
// supports a 'timedWait' method, whereas the latter does not.  In addition,
// 'bcemt_Semaphore' has a 'getValue' accessor, whereas 'bcemt_TimedSemaphore'
// does not.  In the case of the timed semaphore, 'getValue' cannot be
// implemented efficiently on all platforms, so that method is *intentionally*
// not provided.
//
///Usage
///-----
// This example illustrates a very simple queue where potential clients can
// push integers to a queue, and later retrieve the integer values from the
// queue in FIFO order.  It illustrates two potential uses of semaphores:
// to enforce exclusive access, and to allow resource sharing.
//..
//  class IntQueue {
//      // FIFO queue of integer values.
//
//      // DATA
//      bdec_Queue<int>      d_queue;        // underlying queue
//      bcemt_TimedSemaphore d_mutexSem;     // mutual-access semaphore
//      bcemt_TimedSemaphore d_resourceSem;  // resource-availability semaphore
//
//      // NOT IMPLEMENTED
//      IntQueue(const IntQueue&);
//      IntQueue& operator=(const IntQueue&);
//
//    public:
//      // CREATORS
//      explicit IntQueue(bslma_Allocator *basicAllocator = 0);
//          // Create an 'IntQueue' object.  Optionally specified a
//          // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
//          // 0, the currently installed default allocator is used.
//
//      ~IntQueue();
//          // Destroy this 'IntQueue' object.
//
//      // MANIPULATORS
//      int getInt();
//          // Retrieve an integer from this 'IntQueue' object.  Integer values
//          // are obtained from the queue in FIFO order.
//
//      void pushInt(int value);
//          // Push the specified 'value' to this 'IntQueue' object.
//  };
//..
// Note that the 'IntQueue' constructor increments the count of the semaphore
// to 1 so that values can be pushed into the queue immediately following
// construction:
//..
//  // CREATORS
//  IntQueue::IntQueue(bslma_Allocator *basicAllocator)
//  : d_queue(basicAllocator)
//  {
//      d_mutexSem.post();
//  }
//
//  IntQueue::~IntQueue()
//  {
//      d_mutexSem.wait();  // Wait for potential modifier.
//  }
//
//  // MANIPULATORS
//  int IntQueue::getInt()
//  {
//      // Waiting for resources.
//      d_resourceSem.wait();    // TBD modify to use 'timedWait'
//
//      // 'd_mutexSem' is used for exclusive access.
//      d_mutexSem.wait();       // lock
//      const int ret = d_queue.back();
//      d_queue.popBack();
//      d_mutexSem.post();       // unlock
//
//      return ret;
//  }
//
//  void IntQueue::pushInt(int value)
//  {
//      d_mutexSem.wait();
//      d_queue.pushFront(value);
//      d_mutexSem.post();
//
//      d_resourceSem.post();  // Signal that we have resources available.
//  }
//..

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCEMT_TIMEDSEMAPHOREIMPL_POSIXADV
#include <bcemt_timedsemaphoreimpl_posixadv.h>
#endif

#ifndef INCLUDED_BCEMT_TIMEDSEMAPHOREIMPL_PTHREAD
#include <bcemt_timedsemaphoreimpl_pthread.h>
#endif

#ifndef INCLUDED_BCEMT_TIMEDSEMAPHOREIMPL_WIN32
#include <bcemt_timedsemaphoreimpl_win32.h>
#endif

#ifndef INCLUDED_BCES_PLATFORM
#include <bces_platform.h>
#endif

namespace BloombergLP {

template <class TIMED_SEMAPHORE_POLICY>
class bcemt_TimedSemaphoreImpl;

class bdet_TimeInterval;

                         // ==========================
                         // class bcemt_TimedSemaphore
                         // ==========================

class bcemt_TimedSemaphore {
    // This class implements a portable timed semaphore type for thread
    // synchronization.  It forwards all requests to an appropriate
    // platform-specific implementation.

    // DATA
    bcemt_TimedSemaphoreImpl<bces_Platform::TimedSemaphorePolicy>
                                   d_impl;  // platform-specific implementation
    // NOT IMPLEMENTED
    bcemt_TimedSemaphore(const bcemt_TimedSemaphore&);
    bcemt_TimedSemaphore& operator=(const bcemt_TimedSemaphore&);

  public:
    // CREATORS
    bcemt_TimedSemaphore();
        // Create a timed semaphore initially having a count of 0.

    explicit
    bcemt_TimedSemaphore(int count);
        // Create a timed semaphore initially having the specified 'count'.

    ~bcemt_TimedSemaphore();
        // Destroy this timed semaphore.

    // MANIPULATORS
    void post();
        // Atomically increment the count of this timed semaphore.

    void post(int value);
        // Atomically increase the count of this timed semaphore by the
        // specified 'value'.  The behavior is undefined unless 'value > 0'.

    int timedWait(const bdet_TimeInterval& timeout);
        // Block until the count of this semaphore is a positive value, or
        // until the specified 'timeout' (expressed as the !ABSOLUTE! time from
        // 00:00:00 UTC, January 1, 1970) expires.  If the 'timeout' did not
        // expire before the count attained a positive value, atomically
        // decrement the count and return 0; otherwise, return a non-zero value
        // with no effect on the count.

    int tryWait();
        // If the count of this timed semaphore is positive, atomically
        // decrement the count and return 0; otherwise, return a non-zero value
        // with no effect on the count.

    void wait();
        // Block until the count of this timed semaphore is a positive value,
        // then atomically decrement the count and return.
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                         // --------------------------
                         // class bcemt_TimedSemaphore
                         // --------------------------

// CREATORS
inline
bcemt_TimedSemaphore::bcemt_TimedSemaphore()
{
}

inline
bcemt_TimedSemaphore::bcemt_TimedSemaphore(int count)
: d_impl(count)
{
}

inline
bcemt_TimedSemaphore::~bcemt_TimedSemaphore()
{
}

// MANIPULATORS
inline
void bcemt_TimedSemaphore::post()
{
    d_impl.post();
}

inline
void bcemt_TimedSemaphore::post(int value)
{
    d_impl.post(value);
}

inline
int bcemt_TimedSemaphore::timedWait(const bdet_TimeInterval& timeout)
{
    return d_impl.timedWait(timeout);
}

inline
int bcemt_TimedSemaphore::tryWait()
{
    return d_impl.tryWait();
}

inline
void bcemt_TimedSemaphore::wait()
{
    d_impl.wait();
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
