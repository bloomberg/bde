// bslmt_timedsemaphore.h                                             -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BSLMT_TIMEDSEMAPHORE
#define INCLUDED_BSLMT_TIMEDSEMAPHORE

#include <bsls_ident.h>
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
// 'bsls::SystemClockType' supplies the enumeration indicating the system clock
// on which timeouts supplied to other methods should be based.  If the clock
// type indicated at construction is 'bsls::SystemClockType::e_REALTIME', the
// 'absTime' argument passed to the 'timedWait' method should be expressed as
// an *absolute* offset since 00:00:00 UTC, January 1, 1970 (which matches the
// epoch used in 'bsls::SystemTime::now(bsls::SystemClockType::e_REALTIME)'.
// If the clock type indicated at construction is
// 'bsls::SystemClockType::e_MONOTONIC', the 'absTime' argument passed to the
// 'timedWait' method should be expressed as an *absolute* offset since the
// epoch of this clock (which matches the epoch used in
// 'bsls::SystemTime::now(bsls::SystemClockType::e_MONOTONIC)'.
//
// On platforms that support 'bsl::chrono', there are constructors that take
// 'bsl::chrono'-style clocks.  If the clock type indicated at construction is
// 'bsl::chrono::system_clock', then the results will be the same as if
// 'bsls::SystemClockType::e_REALTIME' was indicated.  If the clock type
// indicated at construction is 'bsl::chrono::steady_clock', then the results
// will be the same as if 'bsls::SystemClockType::e_MONOTONIC' was indicated.
// Constructing from a user-defined clock is not supported.
//
///Usage
///-----
// This example illustrates a very simple queue where potential clients can
// push integers to a queue, and later retrieve the integer values from the
// queue in FIFO order.  It illustrates two potential uses of semaphores: to
// enforce exclusive access, and to allow resource sharing.  This queue allows
// clients to set a limit on how long they wait to retrieve values.
//..
//  class IntQueue {
//      // FIFO queue of integer values.
//
//      // DATA
//      bsl::deque<int>       d_queue;       // underlying queue
//      bslmt::TimedSemaphore d_resourceSem; // resource-availability semaphore
//      bslmt::TimedSemaphore d_mutexSem;    // mutual-access semaphore
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
//      int getInt(int *result, int maxWaitSeconds = 0);
//          // Load the first integer in this queue into the specified 'result'
//          // and return 0 unless the operation takes more than the optionally
//          // specified 'maxWaitSeconds', in which case return a nonzero value
//          // and leave 'result' unmodified.
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
//  , d_resourceSem(bsls::SystemClockType::e_MONOTONIC)
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
//  int IntQueue::getInt(int *result, int maxWaitSeconds)
//  {
//      // Waiting for resources.
//      if (0 == maxWaitSeconds) {
//          d_resourceSem.wait();
//      } else {
//          bsls::TimeInterval absTime = bsls::SystemTime::nowMonotonicClock()
//              .addSeconds(maxWaitSeconds);
//          int rc = d_resourceSem.timedWait(absTime);
//          if (0 != rc) {
//             return rc;
//          }
//      }
//
//      // 'd_mutexSem' is used for exclusive access.
//      d_mutexSem.wait();       // lock
//      *result = d_queue.back();
//      d_queue.pop_back();
//      d_mutexSem.post();       // unlock
//
//      return 0;
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

#include <bslscm_version.h>

#include <bslmt_chronoutil.h>
#include <bslmt_platform.h>
#include <bslmt_timedsemaphoreimpl_posixadv.h>
#include <bslmt_timedsemaphoreimpl_pthread.h>
#include <bslmt_timedsemaphoreimpl_win32.h>

#include <bsls_libraryfeatures.h>
#include <bsls_systemclocktype.h>
#include <bsls_systemtime.h>
#include <bsls_timeinterval.h>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
#include <bsl_chrono.h>
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
    // TYPES
    enum { e_TIMED_OUT =
             TimedSemaphoreImpl<Platform::TimedSemaphorePolicy>::e_TIMED_OUT };
        // The value 'timedWait' returns when a timeout occurs.

    // CREATORS
    explicit
    TimedSemaphore(
    bsls::SystemClockType::Enum clockType = bsls::SystemClockType::e_REALTIME);
        // Create a timed semaphore initially having a count of 0.  Optionally
        // specify a 'clockType' indicating the type of the system clock
        // against which the 'absTime' timeouts passed to the 'timedWait'
        // methods are to be interpreted (see {Supported Clock-Types} in the
        // component-level documentation).  If 'clockType' is not specified
        // then the realtime system clock is used.  This method does
        // not return normally unless there are sufficient system resources to
        // construct the object.

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    explicit
    TimedSemaphore(const bsl::chrono::system_clock&);
        // Create a timed semaphore initially having a count of 0.  Use the
        // realtime system clock as the clock against which the 'absTime'
        // timeouts passed to the 'timedWait' methods are interpreted (see
        // {Supported Clock-Types} in the component-level documentation).
        // This method does not return normally unless there are sufficient
        // system resources to construct the object.

    explicit
    TimedSemaphore(const bsl::chrono::steady_clock&);
        // Create a timed semaphore initially having a count of 0.  Use the
        // monotonic system clock as the clock against which the 'absTime'
        // timeouts passed to the 'timedWait' methods are interpreted (see
        // {Supported Clock-Types} in the component-level documentation).
        // This method does not return normally unless there are sufficient
        // system resources to construct the object.
#endif

    explicit
    TimedSemaphore(
    int                         count,
    bsls::SystemClockType::Enum clockType = bsls::SystemClockType::e_REALTIME);
        // Create a timed semaphore initially having the specified 'count'.
        // Optionally specify a 'clockType' indicating the type of the system
        // clock against which the 'absTime' timeouts passed to the 'timedWait'
        // methods are to be interpreted (see {Supported Clock-Types} in the
        // component-level documentation).  If 'clockType' is not specified
        // then the realtime system clock is used.  This method does not return
        // normally unless there are sufficient system resources to construct
        // the object.  The behavior is undefined unless '0 <= count'.

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    TimedSemaphore(int count, const bsl::chrono::system_clock&);
        // Create a timed semaphore initially having the specified 'count'.
        // Use the realtime system clock as the clock against which the
        // 'absTime' timeouts passed to the 'timedWait' methods are interpreted
        // (see {Supported Clock-Types} in the component-level documentation).
        // This method does not return normally unless there are sufficient
        // system resources to construct the object.  The behavior is undefined
        // unless '0 <= count'.

    TimedSemaphore(int count, const bsl::chrono::steady_clock&);
        // Create a timed semaphore initially having the specified 'count'.
        // Use the monotonic system clock as the clock against which the
        // 'absTime' timeouts passed to the 'timedWait' methods are interpreted
        // (see {Supported Clock-Types} in the component-level documentation).
        // This method does not return normally unless there are sufficient
        // system resources to construct the object.  The behavior is undefined
        // unless '0 <= count'.
#endif

    ~TimedSemaphore();
        // Destroy this timed semaphore.

    // MANIPULATORS
    void post();
        // Atomically increment the count of this timed semaphore.

    void post(int value);
        // Atomically increase the count of this timed semaphore by the
        // specified 'value'.  The behavior is undefined unless 'value > 0'.

    int timedWait(const bsls::TimeInterval& absTime);
        // Block until the count of this semaphore is a positive value, or
        // until the specified 'absTime' timeout expires.  'absTime' is an
        // *absolute* time represented as an interval from some epoch, which is
        // determined by the clock indicated at construction (see {Supported
        // Clock-Types} in the component-level documentation).  If the
        // 'absTime' timeout did not expire before the count attained a
        // positive value, atomically decrement the count and return 0.  If the
        // 'absTime' timeout did expire, return 'e_TIMED_OUT' with no effect
        // on the count.  Any other value indicates that an error has occurred.
        // Errors are unrecoverable.  After an error, the semaphore may be
        // destroyed, but any other use has undefined behavior.  On Windows
        // platforms, this method may return 'e_TIMED_OUT' slightly before
        // 'absTime'.

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    template <class CLOCK, class DURATION>
    int timedWait(const bsl::chrono::time_point<CLOCK, DURATION>& absTime);
        // Block until the count of this semaphore is a positive value, or
        // until the specified 'absTime' timeout expires.  'absTime' is an
        // *absolute* time represented by a time point with respect to some
        // epoch, which is determined by the clock associated with the time
        // point.  If the 'absTime' timeout did not expire before the count
        // attained a positive value, atomically decrement the count and return
        // 0.  If the 'absTime' timeout did expire, return 'e_TIMED_OUT' with
        // no effect on the count.  Any other value indicates that an error has
        // occurred.  Errors are unrecoverable.  After an error, the semaphore
        // may be destroyed, but any other use has undefined behavior.  On
        // Windows platforms, this method may return 'e_TIMED_OUT' slightly
        // before 'absTime'.
#endif

    int tryWait();
        // If the count of this timed semaphore is positive, atomically
        // decrement the count and return 0; otherwise, return a non-zero value
        // with no effect on the count.

    void wait();
        // Block until the count of this timed semaphore is a positive value,
        // then atomically decrement the count and return.

    // ACCESSORS
    bsls::SystemClockType::Enum clockType() const;
        // Return the clock type used for timeouts.
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

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
inline
bslmt::TimedSemaphore::TimedSemaphore(const bsl::chrono::system_clock&)
: d_impl(bsls::SystemClockType::e_REALTIME)
{
}

inline
bslmt::TimedSemaphore::TimedSemaphore(const bsl::chrono::steady_clock&)
: d_impl(bsls::SystemClockType::e_MONOTONIC)
{
}
#endif

inline
bslmt::TimedSemaphore::TimedSemaphore(int                         count,
                                      bsls::SystemClockType::Enum clockType)
: d_impl(count, clockType)
{
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
inline
bslmt::TimedSemaphore::TimedSemaphore(int                              count,
                                      const bsl::chrono::system_clock&)
: d_impl(count, bsls::SystemClockType::e_REALTIME)
{
}

inline
bslmt::TimedSemaphore::TimedSemaphore(int                              count,
                                      const bsl::chrono::steady_clock&)
: d_impl(count, bsls::SystemClockType::e_MONOTONIC)
{
}
#endif

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
int bslmt::TimedSemaphore::timedWait(const bsls::TimeInterval& absTime)
{
    return d_impl.timedWait(absTime);
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
template <class CLOCK, class DURATION>
inline
int bslmt::TimedSemaphore::timedWait(
                       const bsl::chrono::time_point<CLOCK, DURATION>& absTime)
{
    return bslmt::ChronoUtil::timedWait(this, absTime);
}
#endif

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

// ACCESSORS
inline
bsls::SystemClockType::Enum
bslmt::TimedSemaphore::clockType() const
{
    return d_impl.clockType();
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2023 Bloomberg Finance L.P.
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
