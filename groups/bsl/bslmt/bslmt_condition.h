// bslmt_condition.h                                                  -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BSLMT_CONDITION
#define INCLUDED_BSLMT_CONDITION

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a portable, efficient condition variable.
//
//@CLASSES:
//  bslmt::Condition: portable intra-process signaling mechanism
//
//@SEE_ALSO: bslmt_mutex
//
//@DESCRIPTION: The 'bslmt::Condition' class provided by this component
// implements the concept of a *condition* *variable*, enabling multiple
// threads to communicate information about the state of shared data.  A
// condition variable is a signaling mechanism associated with a mutex, which
// in turn protects a data invariant.  A condition variable enables threads to
// wait for a predicate (i.e., logical expression) to become true, and to
// communicate to other threads that the predicate might be true.
//
// One or more threads can wait efficiently on a condition variable, either
// indefinitely or until some *absolute* time, by invoking one of the following
// methods of 'bslmt::Condition':
//..
//  int wait(bslmt::Mutex *mutex);
//  int timedWait(bslmt::Mutex *mutex, const bsls::TimeInterval& absTime);
//..
// The caller must lock the mutex before invoking these functions.  The
// 'bslmt::Condition' atomically releases the lock and waits, thereby
// preventing other threads from changing the predicate after the lock is
// released, but before the thread begins to wait.  The 'bslmt' package
// guarantees that this lock will be reacquired before returning from a call to
// the 'wait' and 'timedWait' methods, unless an error occurs.
//
// When invoking the 'timedWait' method, clients must specify, via the
// parameter 'absTime', a timeout after which the call will return even if the
// condition is not signaled.  'absTime' is expressed as a 'bsls::TimeInterval'
// object that holds an *absolute* time according to the clock type the
// 'bslmt::Condition' object is constructed with (the default clock is
// 'bsls::SystemClockType::e_REALTIME').  Clients should use the
// 'bsls::SystemTime::now(clockType)' utility method to obtain the current
// time.
//
// Other threads can indicate that the predicate is true by signaling or
// broadcasting the same 'bslmt::Condition' object.  A broadcast wakes up all
// waiting threads, whereas a signal wakes only one thread.  The client has no
// control over which thread will be signaled if multiple threads are waiting:
//..
//  void signal();
//  void broadcast();
//..
// A thread waiting on a condition variable may be signaled (i.e., the thread
// may wake up without an error), but find that the predicate is still false.
// This situation can arise for a few reasons: spurious wakeups produced by the
// operating system, intercepted wakeups, and loose predicates.  Therefore, a
// waiting thread should always check the predicate *after* (as well as before)
// the call to the 'wait' function.
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
///Usage
///-----
// Suppose we have a 'bslmt::Condition' object, 'condition', and a boolean
// predicate associated with 'condition' (represented here as a free function
// that returns a 'bool' value):
//..
//  bool predicate()
//      // Return 'true' if the invariant holds for 'condition', and 'false'
//      // otherwise.
//  {
//      return true;
//  }
//..
// The following usage pattern should always be followed:
//..
//    // ...
//
//    bslmt::Condition condition;
//    bslmt::Mutex     mutex;
//
//    mutex.lock();
//    while (false == predicate()) {
//        condition.wait(&mutex);
//    }
//
//    // Modify shared resources and adjust the predicate here.
//
//    mutex.unlock();
//
//    // ...
//..
// The usage pattern for a timed wait is similar, but has extra branches to
// handle a timeout:
//..
//    // ...
//
//    enum { e_TIMED_OUT = -1 };
//    bsls::TimeInterval absTime = bsls::SystemTime::nowRealtimeClock();
//
//    // Advance 'absTime' to some delta into the future here.
//
//    mutex.lock();
//    while (false == predicate()) {
//        const int status = condition.timedWait(&mutex, absTime);
//        if (e_TIMED_OUT == status) {
//            break;
//        }
//    }
//
//    if (false == predicate()) {
//        // The wait timed out and 'predicate' returned 'false'.  Perform
//        // timeout logic here.
//
//        // ...
//    }
//    else {
//        // The condition variable was either signaled or timed out and
//        // 'predicate' returned 'true'.  Modify shared resources and adjust
//        // predicate here.
//
//        // ...
//    }
//    mutex.unlock();
//
//    // ...
//..

#include <bslscm_version.h>

#include <bslmt_conditionimpl_pthread.h>
#include <bslmt_conditionimpl_win32.h>
#include <bslmt_platform.h>

#include <bsls_assert.h>
#include <bsls_libraryfeatures.h>
#include <bsls_timeinterval.h>
#include <bsls_systemclocktype.h>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
#include <bslmt_chronoutil.h>

#include <bsl_chrono.h>
#endif

namespace BloombergLP {
namespace bslmt {

template <class THREAD_POLICY>
class ConditionImpl;

class Mutex;

                             // ===============
                             // class Condition
                             // ===============

class Condition {
    // This 'class' implements a portable inter-thread signaling primitive.

    // DATA
    ConditionImpl<Platform::ThreadPolicy> d_imp;  // platform-specific
                                                  // implementation

    // NOT IMPLEMENTED
    Condition(const Condition&);
    Condition& operator=(const Condition&);

  public:
    // TYPES
    enum { e_TIMED_OUT = ConditionImpl<Platform::ThreadPolicy>::e_TIMED_OUT };
        // The value 'timedWait' returns when a timeout occurs.

    // CREATORS
    explicit
    Condition(bsls::SystemClockType::Enum clockType =
                                            bsls::SystemClockType::e_REALTIME);
        // Create a condition variable object.  Optionally specify a
        // 'clockType' indicating the type of the system clock against which
        // the 'bsls::TimeInterval' 'absTime' timeouts passed to the
        // 'timedWait' method are to be interpreted (see {Supported
        // Clock-Types} in the component-level documentation).  If 'clockType'
        // is not specified then the realtime system clock is used.  This
        // method does not return normally unless there are sufficient system
        // resources to construct the object.

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    explicit
    Condition(const bsl::chrono::system_clock&);
        // Create a condition variable object.  Use the realtime system clock
        // as the clock against which the 'absTime' timeouts passed to the
        // 'timedWait' methods are interpreted (see {Supported Clock-Types} in
        // the component-level documentation).  This method does not return
        // normally unless there are sufficient system resources to construct
        // the object.

    explicit
    Condition(const bsl::chrono::steady_clock&);
        // Create a condition variable object.  Use the monotonic system clock
        // as the clock against which the 'absTime' timeouts passed to the
        // 'timedWait' methods are interpreted (see {Supported Clock-Types} in
        // the component-level documentation).  This method does not return
        // normally unless there are sufficient system resources to construct
        // the object.
#endif

    ~Condition();
        // Destroy this condition variable object.

    // MANIPULATORS
    void broadcast();
        // Signal this condition variable object by waking up *all* threads
        // that are currently waiting on this condition.  If there are no
        // threads waiting on this condition, this method has no effect.

    void signal();
        // Signal this condition variable object by waking up a single thread
        // that is currently waiting on this condition.  If there are no
        // threads waiting on this condition, this method has no effect.

    int timedWait(Mutex *mutex, const bsls::TimeInterval& absTime);
        // Atomically unlock the specified 'mutex' and suspend execution of the
        // current thread until this condition object is "signaled" (i.e., one
        // of the 'signal' or 'broadcast' methods is invoked on this object) or
        // until the specified 'absTime' timeout expires, then re-acquire a
        // lock on the 'mutex'.  'absTime' is an *absolute* time represented as
        // an interval from some epoch, which is determined by the clock
        // indicated at construction (see {Supported Clock-Types} in the
        // component-level documentation), and is the earliest time at which
        // the timeout may occur.  The 'mutex' remains locked by the calling
        // thread upon returning from this function.  Return 0 on success, and
        // 'e_TIMED_OUT' on timeout.  Any other value indicates that an error
        //  has occurred.  After an error, the condition may be destroyed, but
        // any other use has undefined behavior.  The behavior is undefined
        // unless 'mutex' is locked by the calling thread prior to calling this
        // method.  Note that spurious wakeups are rare but possible, i.e.,
        // this method may succeed (return 0) and return control to the thread
        // without the condition object being signaled.  Also note that the
        // actual time of the timeout depends on many factors including system
        // scheduling and system timer resolution, and may be significantly
        // later than the time requested.

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    template <class CLOCK, class DURATION>
    int timedWait(Mutex                                           *mutex,
                  const bsl::chrono::time_point<CLOCK, DURATION>&  absTime);
        // Atomically unlock the specified 'mutex' and suspend execution of the
        // current thread until this condition object is "signaled" (i.e., one
        // of the 'signal' or 'broadcast' methods is invoked on this object) or
        // until the specified 'absTime' timeout expires, then re-acquire a
        // lock on the 'mutex'.  'absTime' is an *absolute* time represented as
        // an interval from some epoch, which is determined by the clock
        // associated with the time point, and is the earliest time at which
        // the timeout may occur.  The 'mutex' remains locked by the calling
        // thread upon returning from this function.  Return 0 on success, and
        // 'e_TIMED_OUT' on timeout.  Any other value indicates that an error
        // has occurred.  After an error, the condition may be destroyed, but
        // any other use has undefined behavior.  The behavior is undefined
        // unless 'mutex' is locked by the calling thread prior to calling this
        // method.  Note that spurious wakeups are rare but possible, i.e.,
        // this method may succeed (return 0) and return control to the thread
        // without the condition object being signaled.  Also note that the
        // actual time of the timeout depends on many factors including system
        // scheduling and system timer resolution, and may be significantly
        // later than the time requested.  Also note that the lock on 'mutex'
        // may be released and reacquired more than once before this method
        // returns.
#endif

    int wait(Mutex *mutex);
        // Atomically unlock the specified 'mutex' and suspend execution of the
        // current thread until this condition object is "signaled" (i.e.,
        // either 'signal' or 'broadcast' is invoked on this object in another
        // thread), then re-acquire a lock on the 'mutex'.  Return 0 on
        // success, and a non-zero value otherwise.  Spurious wakeups are rare
        // but possible; i.e., this method may succeed (return 0), and return
        // control to the thread without the condition object being signaled.
        // The behavior is undefined unless 'mutex' is locked by the calling
        // thread prior to calling this method.  Note that 'mutex' remains
        // locked by the calling thread upon return from this function.

    // ACCESSORS
    bsls::SystemClockType::Enum clockType() const;
        // Return the clock type used for timeouts.
};

}  // close package namespace

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                             // ---------------
                             // class Condition
                             // ---------------

// CREATORS
inline
bslmt::Condition::Condition(bsls::SystemClockType::Enum clockType)
: d_imp(clockType)
{
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
inline
bslmt::Condition::Condition(const bsl::chrono::system_clock&)
: d_imp(bsls::SystemClockType::e_REALTIME)
{
}

inline
bslmt::Condition::Condition(const bsl::chrono::steady_clock&)
: d_imp(bsls::SystemClockType::e_MONOTONIC)
{
}
#endif

inline
bslmt::Condition::~Condition()
{
}

// MANIPULATORS
inline
void bslmt::Condition::broadcast()
{
    d_imp.broadcast();
}

inline
void bslmt::Condition::signal()
{
    d_imp.signal();
}

inline
int bslmt::Condition::timedWait(Mutex                     *mutex,
                                const bsls::TimeInterval&  absTime)
{
    BSLS_ASSERT_SAFE(mutex);

    return d_imp.timedWait(mutex, absTime);
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
template <class CLOCK, class DURATION>
inline
int bslmt::Condition::timedWait(
                      Mutex                                           *mutex,
                      const bsl::chrono::time_point<CLOCK, DURATION>&  absTime)
{
    BSLS_ASSERT_SAFE(mutex);

    return bslmt::ChronoUtil::timedWait(this, mutex, absTime);
}
#endif

inline
int bslmt::Condition::wait(Mutex *mutex)
{
    BSLS_ASSERT_SAFE(mutex);

    return d_imp.wait(mutex);
}

// ACCESSORS
inline
bsls::SystemClockType::Enum bslmt::Condition::clockType() const
{
    return d_imp.clockType();
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
