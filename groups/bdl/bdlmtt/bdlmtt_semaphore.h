// bdlmtt_semaphore.h                                                  -*-C++-*-
#ifndef INCLUDED_BDLMTT_SEMAPHORE
#define INCLUDED_BDLMTT_SEMAPHORE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a semaphore class.
//
//@CLASSES:
//  bdlmtt::Semaphore: semaphore class
//
//@SEE_ALSO: bdlmtt_timedsemaphore
//
//@AUTHOR: Guillaume Morin (gmorin1)
//
//@DESCRIPTION: This component defines a portable and efficient thread
// synchronization primitive.  In particular, 'bdlmtt::Semaphore' is an efficient
// synchronization primitive that enables sharing of a counted number of
// resources or exclusive access.  The usage model of this facility is modeled
// on POSIX semaphores and Windows semaphores.
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
//      bsl::deque<int> d_queue;        // underlying queue
//      bdlmtt::Semaphore d_mutexSem;     // mutual-access semaphore
//      bdlmtt::Semaphore d_resourceSem;  // resource-availability semaphore
//
//      // NOT IMPLEMENTED
//      IntQueue(const IntQueue&);
//      IntQueue& operator=(const IntQueue&);
//
//    public:
//      // CREATORS
//      explicit IntQueue(bslma::Allocator *basicAllocator = 0);
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
//  IntQueue::IntQueue(bslma::Allocator *basicAllocator)
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
//      d_resourceSem.wait();
//
//      // 'd_mutexSem' is used for exclusive access.
//      d_mutexSem.wait();        // lock
//      const int ret = d_queue.back();
//      d_queue.pop_back();
//      d_mutexSem.post();        // unlock
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
//      d_resourceSem.post();  // Signal we have resources available.
//  }
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLMTT_SEMAPHOREIMPL_COUNTED
#include <bdlmtt_semaphoreimpl_counted.h>
#endif

#ifndef INCLUDED_BDLMTT_SEMAPHOREIMPL_PTHREAD
#include <bdlmtt_semaphoreimpl_pthread.h>
#endif

#ifndef INCLUDED_BDLMTT_SEMAPHOREIMPL_WIN32
#include <bdlmtt_semaphoreimpl_win32.h>
#endif

#ifndef INCLUDED_BDLMTT_PLATFORM
#include <bdlmtt_platform.h>
#endif

namespace BloombergLP {


namespace bdlmtt {template <typename SEMAPHORE_POLICY>
class SemaphoreImpl;

                         // =====================
                         // class Semaphore
                         // =====================

class Semaphore {
    // This class implements a portable semaphore type for thread
    // synchronization.  It forwards all requests to an appropriate
    // platform-specific implementation.

    // DATA
    SemaphoreImpl<bdlmtt::Platform::SemaphorePolicy>
                                   d_impl;  // platform-specific implementation

    // NOT IMPLEMENTED
    Semaphore(const Semaphore&);
    Semaphore& operator=(const Semaphore&);

  public:
    // CREATORS
    Semaphore();
        // Create a semaphore initially having a count of 0.

    explicit
    Semaphore(int count);
        // Create a semaphore initially having the specified 'count'.

    ~Semaphore();
        // Destroy this semaphore.

    // MANIPULATORS
    void post();
        // Atomically increment the count of this semaphore.

    void post(int value);
        // Atomically increase the count of this semaphore by the specified
        // 'value'.  The behavior is undefined unless 'value > 0'.

    int tryWait();
        // If the count of this semaphore is positive, atomically decrement the
        // count and return 0; otherwise, return a non-zero value with no
        // effect on the count.

    void wait();
        // Block until the count of this semaphore is a positive value, then
        // atomically decrement the count and return.

    // ACCESSORS
    int getValue() const;
        // Return the value of the current count of this semaphore.
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                         // ---------------------
                         // class Semaphore
                         // ---------------------

// CREATORS
inline
Semaphore::Semaphore()
: d_impl(0)
{
}

inline
Semaphore::Semaphore(int count)
: d_impl(count)
{
}

inline
Semaphore::~Semaphore()
{
}

inline
void Semaphore::post()
{
    d_impl.post();
}

inline
void Semaphore::post(int value)
{
    d_impl.post(value);
}

inline
int Semaphore::tryWait()
{
    return d_impl.tryWait();
}

inline
void Semaphore::wait()
{
    d_impl.wait();
}

// ACCESSORS
inline
int Semaphore::getValue() const
{
    return d_impl.getValue();
}
}  // close package namespace

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
