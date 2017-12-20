// bslmt_semaphoreimpl_counted.h                                      -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BSLMT_SEMAPHOREIMPL_COUNTED
#define INCLUDED_BSLMT_SEMAPHOREIMPL_COUNTED

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an implementation of 'bslmt::Semaphore' with count.
//
//@CLASSES:
//  bslmt::SemaphoreImpl<CountedSemaphore>: semaphore specialization with count
//
//@SEE_ALSO: bslmt_semaphore
//
//@DESCRIPTION: This component provides an implementation of
// 'bslmt::Semaphore', 'bslmt::SemaphoreImpl<CountedSemaphore>', via the
// template specialization:
//..
//  bslmt::SemaphoreImpl<Platform::CountedSemaphore>
//..
// This template class should not be used (directly) by client code.  Clients
// should instead use 'bslmt::Semaphore'.
//
// This implementation of 'bslmt::Semaphore' is intended for platforms where a
// separate count must be maintained.  'bslmt::Semaphore' supports large
// values, but the native semaphores provided on some platforms are restricted
// to a relatively small range of values (e.g., '[ 0 .. 32000 ]' on AIX) and on
// some other platforms do not provide a count at all (Darwin).  To support
// uniform usage across platforms, this component maintains the count of the
// semaphore in a separate atomic integer.  'post' is only invoked on the
// underlying semaphore when it is known there are threads blocked on it.
//
///Usage
///-----
// This component is an implementation detail of 'bslmt' and is *not* intended
// for direct client use.  It is subject to change without notice.  As such, a
// usage example is not provided.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMT_PLATFORM
#include <bslmt_platform.h>
#endif

#ifdef BSLMT_PLATFORM_COUNTED_SEMAPHORE

// Platform-specific implementation starts here.

#ifndef INCLUDED_BSLS_ATOMIC
#include <bsls_atomic.h>
#endif

#ifndef BSLMT_SEMAPHOREIMPL_PTHREAD
#include <bslmt_semaphoreimpl_pthread.h>
#endif

#ifndef BSLMT_SEMAPHOREIMPL_DARWIN
#include <bslmt_semaphoreimpl_darwin.h>
#endif

namespace BloombergLP {
namespace bslmt {

template <class SEMAPHORE_POLICY>
class SemaphoreImpl;

             // ===============================================
             // class SemaphoreImpl<Platform::CountedSemaphore>
             // ===============================================

template <>
class SemaphoreImpl<Platform::CountedSemaphore> {
    // This class provides a full specialization of 'SemaphoreImpl' with a
    // separate count variable.  This implementation maintains the value of the
    // semaphore in a separate atomic integer count, so as to allow for
    // semaphore count on platforms where a semaphore implementation doesn't
    // provide the count or the provided count has very limited range of
    // values.

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
    SemaphoreImpl(int count);
        // Create a semaphore.

    ~SemaphoreImpl();
        // Destroy a semaphore

    // MANIPULATORS
    void post();
        // Atomically increment the count of this semaphore.

    void post(int number);
        // Atomically increment the count of this semaphore by the specified
        // 'number'.  The behavior is undefined unless 'number > 0'.

    int tryWait();
        // Decrement the count of this semaphore if it is positive and return
        // 0.  Return a non-zero value otherwise.

    void wait();
        // Block until the count of this semaphore is a positive value and
        // atomically decrement it.

    // ACCESSORS
    int getValue() const;
        // Return the current value of this semaphore.
};

}  // close package namespace

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

             // -----------------------------------------------
             // class SemaphoreImpl<Platform::CountedSemaphore>
             // -----------------------------------------------

// CREATORS
inline
bslmt::SemaphoreImpl<bslmt::Platform::CountedSemaphore>::SemaphoreImpl(
                                                                     int count)
: d_resources(count)
, d_sem(0)
{
}

inline
bslmt::SemaphoreImpl<bslmt::Platform::CountedSemaphore>::~SemaphoreImpl()
{
}

// MANIPULATORS
inline
void bslmt::SemaphoreImpl<bslmt::Platform::CountedSemaphore>::post()
{
    if (++d_resources <= 0) {
        d_sem.post();
    }
}

inline
void bslmt::SemaphoreImpl<bslmt::Platform::CountedSemaphore>::post(int number)
{
    for (int i = 0; i < number; ++i) {
        post();
    }
}

inline
int bslmt::SemaphoreImpl<bslmt::Platform::CountedSemaphore>::tryWait()
{
    for (int i = d_resources; i > 0; i = d_resources) {
        if (i == d_resources.testAndSwap(i, i - 1)) {
            return 0;
        }
    }

    return -1;
}

inline
void bslmt::SemaphoreImpl<bslmt::Platform::CountedSemaphore>::wait()
{
    if (--d_resources >= 0) {
        return;
    }

    d_sem.wait();
}

// ACCESSORS
inline
int bslmt::SemaphoreImpl<bslmt::Platform::CountedSemaphore>::getValue() const
{
    const int v = d_resources;
    return v > 0 ? v : 0;
}

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
