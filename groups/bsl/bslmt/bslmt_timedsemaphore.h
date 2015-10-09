// bslmt_timedsemaphore.h                                             -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BSLMT_TIMEDSEMAPHORE
#define INCLUDED_BSLMT_TIMEDSEMAPHORE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a timed semaphore class.
//
//@CLASSES:
//  bslmt::TimedSemaphore: timed semaphore class
//
//@SEE_ALSO: bslmt_semaphore
//
//@DESCRIPTION: This component defines a portable and efficient thread
// synchronization primitive.  In particular, 'bslmt::TimedSemaphore' is an
// efficient synchronization primitive that enables sharing of a counted number
// of resources or exclusive access.
//
// 'bslmt::TimedSemaphore' differs from 'bslmt::Semaphore' in that the former
// supports a 'timedWait' method, whereas the latter does not.  In addition,
// 'bslmt::Semaphore' has a 'getValue' accessor, whereas
// 'bslmt::TimedSemaphore' does not.  In the case of the timed semaphore,
// 'getValue' cannot be implemented efficiently on all platforms, so that
// method is *intentionally* not provided.
//
///Supported Clock-Types
///---------------------
// The component 'bsls::SystemClockType' supplies the enumeration indicating
// the system clock on which timeouts supplied to other methods should be
// based.  If the clock type indicated at construction is
// 'bsls::SystemClockType::e_REALTIME', the timeout should be expressed as an
// absolute offset since 00:00:00 UTC, January 1, 1970 (which matches the epoch
// used in 'bsls::SystemTime::now(bsls::SystemClockType::e_REALTIME)'.  If the
// clock type indicated at construction is
// 'bsls::SystemClockType::e_MONOTONIC', the timeout should be expressed as an
// absolute offset since the epoch of this clock (which matches the epoch used
// in 'bsls::SystemTime::now(bsls::SystemClockType::e_MONOTONIC)'.
//
///Usage
///-----
// This example illustrates a very simple queue where potential clients can
// push integers to a queue, and later retrieve the integer values from the
// queue in FIFO order.  It illustrates two potential uses of semaphores: to
// enforce exclusive access, and to allow resource sharing.
//..
//  class IntQueue {
//      // FIFO queue of integer values.
//
//      // DATA
//      bdlc::Queue<int>      d_queue;       // underlying queue
//      bslmt::TimedSemaphore d_mutexSem;    // mutual-access semaphore
//      bslmt::TimedSemaphore d_resourceSem; // resource-availability semaphore
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

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMT_TIMEDSEMAPHOREIMPL_POSIXADV
#include <bslmt_timedsemaphoreimpl_posixadv.h>
#endif

#ifndef INCLUDED_BSLMT_TIMEDSEMAPHOREIMPL_PTHREAD
#include <bslmt_timedsemaphoreimpl_pthread.h>
#endif

#ifndef INCLUDED_BSLMT_TIMEDSEMAPHOREIMPL_WIN32
#include <bslmt_timedsemaphoreimpl_win32.h>
#endif

#ifndef INCLUDED_BSLMT_PLATFORM
#include <bslmt_platform.h>
#endif

#ifndef INCLUDED_BSLS_SYSTEMCLOCKTYPE
#include <bsls_systemclocktype.h>
#endif

#ifndef INCLUDED_BSLS_TIMEINTERVAL
#include <bsls_timeinterval.h>
#endif

namespace BloombergLP {
namespace bslmt {

template <class TIMED_SEMAPHORE_POLICY>
class TimedSemaphoreImpl;

                           // ====================
                           // class TimedSemaphore
                           // ====================

class TimedSemaphore {
    // This class implements a portable timed semaphore type for thread
    // synchronization.  It forwards all requests to an appropriate
    // platform-specific implementation.

    // DATA
    TimedSemaphoreImpl<Platform::TimedSemaphorePolicy> d_impl;
                                            // platform-specific implementation
    // NOT IMPLEMENTED
    TimedSemaphore(const TimedSemaphore&);
    TimedSemaphore& operator=(const TimedSemaphore&);

  public:
    // CREATORS
    explicit
    TimedSemaphore(
    bsls::SystemClockType::Enum clockType = bsls::SystemClockType::e_REALTIME);
        // Create a timed semaphore initially having a count of 0.  Optionally
        // specify a 'clockType' indicating the type of the system clock
        // against which the 'bsls::TimeInterval' timeouts passed to the
        // 'timedWait' method are to be interpreted.  If 'clockType' is not
        // specified then the realtime system clock is used.

    explicit
    TimedSemaphore(
    int                         count,
    bsls::SystemClockType::Enum clockType = bsls::SystemClockType::e_REALTIME);
        // Create a timed semaphore initially having the specified 'count'.
        // Optionally specify a 'clockType' indicating the type of the system
        // clock against which the 'bsls::TimeInterval' timeouts passed to the
        // 'timedWait' method are to be interpreted.  If 'clockType' is not
        // specified then the realtime system clock is used.

    ~TimedSemaphore();
        // Destroy this timed semaphore.

    // MANIPULATORS
    void post();
        // Atomically increment the count of this timed semaphore.

    void post(int value);
        // Atomically increase the count of this timed semaphore by the
        // specified 'value'.  The behavior is undefined unless 'value > 0'.

    int timedWait(const bsls::TimeInterval& timeout);
        // Block until the count of this semaphore is a positive value, or
        // until the specified 'timeout' expires.  The 'timeout' is an absolute
        // time represented as an interval from some epoch, which is determined
        // by the clock indicated at construction (see {'Supported
        // Clock-Types'} in the component documentation).  If the 'timeout' did
        // not expire before the count attained a positive value, atomically
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

}  // close package namespace

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                           // --------------------
                           // class TimedSemaphore
                           // --------------------

// CREATORS
inline
bslmt::TimedSemaphore::TimedSemaphore(bsls::SystemClockType::Enum clockType)
: d_impl(clockType)
{
}

inline
bslmt::TimedSemaphore::TimedSemaphore(int                         count,
                                      bsls::SystemClockType::Enum clockType)
: d_impl(count, clockType)
{
}

inline
bslmt::TimedSemaphore::~TimedSemaphore()
{
}

// MANIPULATORS
inline
void bslmt::TimedSemaphore::post()
{
    d_impl.post();
}

inline
void bslmt::TimedSemaphore::post(int value)
{
    d_impl.post(value);
}

inline
int bslmt::TimedSemaphore::timedWait(const bsls::TimeInterval& timeout)
{
    return d_impl.timedWait(timeout);
}

inline
int bslmt::TimedSemaphore::tryWait()
{
    return d_impl.tryWait();
}

inline
void bslmt::TimedSemaphore::wait()
{
    d_impl.wait();
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
