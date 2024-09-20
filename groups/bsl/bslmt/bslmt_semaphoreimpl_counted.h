// bslmt_semaphoreimpl_counted.h                                      -*-C++-*-
#ifndef INCLUDED_BSLMT_SEMAPHOREIMPL_COUNTED
#define INCLUDED_BSLMT_SEMAPHOREIMPL_COUNTED

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an implementation of `bslmt::Semaphore` with count.
//
//@CLASSES:
//  bslmt::SemaphoreImpl<CountedSemaphore>: semaphore specialization with count
//
//@SEE_ALSO: bslmt_semaphore
//
//@DESCRIPTION: This component provides an implementation of
// `bslmt::Semaphore`, `bslmt::SemaphoreImpl<CountedSemaphore>`, via the
// template specialization:
// ```
// bslmt::SemaphoreImpl<Platform::CountedSemaphore>
// ```
// This template class should not be used (directly) by client code.  Clients
// should instead use `bslmt::Semaphore`.
//
// This implementation of `bslmt::Semaphore` is intended for platforms where a
// separate count must be maintained.  `bslmt::Semaphore` supports large
// values, but the native semaphores provided on some platforms are restricted
// to a relatively small range of values (e.g., `[ 0 .. 32000 ]` on AIX) and on
// some other platforms do not provide a count at all (Darwin).  To support
// uniform usage across platforms, this component maintains the count of the
// semaphore in a separate atomic integer.  `post` is only invoked on the
// underlying semaphore when it is known there are threads blocked on it.
//
///Usage
///-----
// This component is an implementation detail of `bslmt` and is *not* intended
// for direct client use.  It is subject to change without notice.  As such, a
// usage example is not provided.

#include <bslscm_version.h>

#include <bslmt_platform.h>

#ifdef BSLMT_PLATFORM_COUNTED_SEMAPHORE

// Platform-specific implementation starts here.

#include <bsls_atomic.h>

#include <bslmt_semaphoreimpl_pthread.h>
#include <bslmt_semaphoreimpl_darwin.h>

namespace BloombergLP {
namespace bslmt {

template <class SEMAPHORE_POLICY>
class SemaphoreImpl;

             // ===============================================
             // class SemaphoreImpl<Platform::CountedSemaphore>
             // ===============================================

/// This class provides a full specialization of `SemaphoreImpl` with a
/// separate count variable.  This implementation maintains the value of the
/// semaphore in a separate atomic integer count, so as to allow for
/// semaphore count on platforms where a semaphore implementation doesn't
/// provide the count or the provided count has very limited range of
/// values.
template <>
class SemaphoreImpl<Platform::CountedSemaphore> {

    // DATA
    bsls::AtomicInt d_resources; // if positive, number of available resources
                                 // if negative: number of waiting threads

    SemaphoreImpl<Platform::CountedSemaphoreImplPolicy>
                    d_sem;       // platform semaphore implementation

    // NOT IMPLEMENTED
    SemaphoreImpl(const SemaphoreImpl&);
    SemaphoreImpl& operator=(const SemaphoreImpl&);

  public:
    // CREATORS

    /// Create a semaphore.  This method does not return normally unless
    /// there are sufficient system resources to construct the object.
    SemaphoreImpl(int count);

    /// Destroy a semaphore
    ~SemaphoreImpl();

    // MANIPULATORS

    /// Atomically increment the count of this semaphore.
    void post();

    /// Atomically increment the count of this semaphore by the specified
    /// `number`.  The behavior is undefined unless `number > 0`.
    void post(int number);

    /// Decrement the count of this semaphore if it is positive and return
    /// 0.  Return a non-zero value otherwise.
    int tryWait();

    /// Block until the count of this semaphore is a positive value and
    /// atomically decrement it.
    void wait();

    // ACCESSORS

    /// Return the current value of this semaphore.
    int getValue() const;
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

             // -----------------------------------------------
             // class SemaphoreImpl<Platform::CountedSemaphore>
             // -----------------------------------------------

// CREATORS
inline
SemaphoreImpl<bslmt::Platform::CountedSemaphore>::SemaphoreImpl(
                                                                     int count)
: d_resources(count)
, d_sem(0)
{
}

inline
SemaphoreImpl<bslmt::Platform::CountedSemaphore>::~SemaphoreImpl()
{
}

// MANIPULATORS
inline
void SemaphoreImpl<bslmt::Platform::CountedSemaphore>::post()
{
    if (++d_resources <= 0) {
        d_sem.post();
    }
}

inline
void SemaphoreImpl<bslmt::Platform::CountedSemaphore>::post(int number)
{
    for (int i = 0; i < number; ++i) {
        post();
    }
}

inline
int SemaphoreImpl<bslmt::Platform::CountedSemaphore>::tryWait()
{
    for (int i = d_resources; i > 0; i = d_resources) {
        if (i == d_resources.testAndSwap(i, i - 1)) {
            return 0;
        }
    }

    return -1;
}

inline
void SemaphoreImpl<bslmt::Platform::CountedSemaphore>::wait()
{
    if (--d_resources >= 0) {
        return;
    }

    d_sem.wait();
}

// ACCESSORS
inline
int SemaphoreImpl<bslmt::Platform::CountedSemaphore>::getValue() const
{
    const int v = d_resources;
    return v > 0 ? v : 0;
}

}  // close package namespace
}  // close enterprise namespace

#endif

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
