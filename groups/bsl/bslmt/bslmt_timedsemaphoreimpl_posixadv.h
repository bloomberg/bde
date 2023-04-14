// bslmt_timedsemaphoreimpl_posixadv.h                                -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BSLMT_TIMEDSEMAPHOREIMPL_POSIXADV
#define INCLUDED_BSLMT_TIMEDSEMAPHOREIMPL_POSIXADV

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide "advanced" POSIX implementation of 'bslmt::TimedSemaphore'.
//
//@CLASSES:
//  bslmt::TimedSemaphoreImpl<PosixAdvTimedSemaphore>: POSIXa specialization
//
//@SEE_ALSO: bslmt_timedsemaphore
//
//@DESCRIPTION: This component provides an implementation of
// 'bslmt::TimedSemaphore',
// 'bslmt::TimedSemaphoreImpl<PosixAdvTimedSemaphore>', for conforming POSIX
// platforms via the template specialization:
//..
//  bslmt::TimedSemaphoreImpl<Platform::PosixAdvTimedSemaphore>
//..
// This template class should not be used (directly) by client code.  Clients
// should instead use 'bslmt::TimedSemaphore'.
//
// This implementation of 'bslmt::TimedSemaphore' is preferred over that
// defined in 'bslmt_timedsemaphoreimpl_pthread' on platforms that support
// advanced realtime POSIX extensions (e.g., 'sem_timedwait').
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
// This component is an implementation detail of 'bslmt' and is *not* intended
// for direct client use.  It is subject to change without notice.  As such, a
// usage example is not provided.

#include <bslscm_version.h>

#include <bslmt_platform.h>

#ifdef BSLMT_PLATFORM_POSIXADV_TIMEDSEMAPHORE

// Platform-specific implementation starts here.

#include <bsls_assert.h>
#include <bsls_systemclocktype.h>
#include <bsls_timeinterval.h>

#include <semaphore.h>

namespace BloombergLP {
namespace bslmt {

template <class TIMED_SEMAPHORE_POLICY>
class TimedSemaphoreImpl;

             // ================================================
             // class TimedSemaphoreImpl<PosixAdvTimedSemaphore>
             // ================================================

template <>
class TimedSemaphoreImpl<Platform::PosixAdvTimedSemaphore> {
    // This class implements a timed semaphore in terms of POSIX operations.
    // Note that only certain platforms provide 'sem_timedwait'; on those that
    // do not, 'TimedSemaphoreImpl<PthreadTimedSemaphore>' is used.

    // DATA
    sem_t                       d_sem;        // POSIX timed semaphore

    bsls::SystemClockType::Enum d_clockType;  // clock type used for 'absTime'
                                              // in 'timedWait'

    // NOT IMPLEMENTED
    TimedSemaphoreImpl(const TimedSemaphoreImpl&);
    TimedSemaphoreImpl& operator=(const TimedSemaphoreImpl&);

  public:
    // TYPES
    enum { e_TIMED_OUT = -1 };
        // The value 'timedWait' returns when a timeout occurs.

    // CREATORS
    explicit
    TimedSemaphoreImpl(bsls::SystemClockType::Enum clockType
                                          = bsls::SystemClockType::e_REALTIME);
        // Create a timed semaphore initially having a count of 0.  Optionally
        // specify a 'clockType' indicating the type of the system clock
        // against which the 'bsls::TimeInterval' 'absTime' timeouts passed to
        // the 'timedWait' method are to be interpreted (see {Supported
        // Clock-Types} in the component documentation).  If 'clockType' is not
        // specified then the realtime system clock is used.  This method does
        // not return normally unless there are sufficient system resources to
        // construct the object.

    explicit
    TimedSemaphoreImpl(int                         count,
                       bsls::SystemClockType::Enum clockType
                                          = bsls::SystemClockType::e_REALTIME);
        // Create a timed semaphore initially having the specified 'count'.
        // Optionally specify a 'clockType' indicating the type of the system
        // clock against which the 'bsls::TimeInterval' 'absTime' timeouts
        // passed to the 'timedWait' method are to be interpreted (see
        // {Supported Clock-Types} in the component documentation).  If
        // 'clockType' is not specified then the realtime system clock is used.
        // This method does not return normally unless there are sufficient
        // system resources to construct the object.

    ~TimedSemaphoreImpl();
        // Destroy this semaphore object.

    // MANIPULATORS
    void post();
        // Atomically increment the count of the semaphore.

    void post(int number);
        // Atomically increment the count by the specified 'number' of the
        // semaphore.  The behavior is undefined unless 'number' is a positive
        // value.

    int timedWait(const bsls::TimeInterval& absTime);
        // Block until the count of this semaphore is a positive value, or
        // until the specified 'absTime' timeout expires.  'absTime' is an
        // *absolute* time represented as an interval from some epoch, which is
        // determined by the clock indicated at construction (see {Supported
        // Clock-Types} in the component documentation).  If the 'absTime'
        // timeout did not expire before the count attained a positive value,
        // atomically decrement the count and return 0; otherwise, return a
        // non-zero value with no effect on the count.

    int tryWait();
        // Decrement the count of this semaphore if it is positive and return
        // 0.  Return a non-zero value otherwise.

    void wait();
        // Block until the count is a positive value and atomically decrement
        // it.

    // ACCESSORS
    bsls::SystemClockType::Enum clockType() const;
        // Return the clock type used for timeouts.
};

}  // close package namespace

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

             // ------------------------------------------------
             // class TimedSemaphoreImpl<PosixAdvTimedSemaphore>
             // ------------------------------------------------

// CREATORS
inline
bslmt::TimedSemaphoreImpl<bslmt::Platform::PosixAdvTimedSemaphore>::
    TimedSemaphoreImpl(bsls::SystemClockType::Enum clockType)
: d_clockType(clockType)
{
    int result = ::sem_init(&d_sem, 0, 0);  (void)result;
    BSLS_ASSERT_OPT(-1 != result);
}

inline
bslmt::TimedSemaphoreImpl<bslmt::Platform::PosixAdvTimedSemaphore>::
    TimedSemaphoreImpl(int count, bsls::SystemClockType::Enum clockType)
: d_clockType(clockType)
{
    int result = ::sem_init(&d_sem, 0, count);  (void)result;
    BSLS_ASSERT_OPT(-1 != result);
}

inline
bslmt::TimedSemaphoreImpl<bslmt::Platform::PosixAdvTimedSemaphore>::
    ~TimedSemaphoreImpl()
{
    ::sem_destroy(&d_sem);
}

// MANIPULATORS
inline
void bslmt::TimedSemaphoreImpl<bslmt::Platform::PosixAdvTimedSemaphore>::post()
{
    ::sem_post(&d_sem);
}

inline
int bslmt::TimedSemaphoreImpl<bslmt::Platform::PosixAdvTimedSemaphore>::
    tryWait()
{
    return ::sem_trywait(&d_sem);
}

// ACCESSORS
inline
bsls::SystemClockType::Enum
bslmt::TimedSemaphoreImpl<bslmt::Platform::PosixAdvTimedSemaphore>::
                                                           clockType() const
{
    return d_clockType;
}

}  // close enterprise namespace

#endif  // BSLMT_PLATFORM_POSIX_THREADS

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
