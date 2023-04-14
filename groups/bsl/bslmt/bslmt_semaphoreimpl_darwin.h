// bslmt_semaphoreimpl_darwin.h                                       -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BSLMT_SEMAPHOREIMPL_DARWIN
#define INCLUDED_BSLMT_SEMAPHOREIMPL_DARWIN

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a Darwin implementation of 'bslmt::Semaphore'.
//
//@CLASSES:
//  bslmt::SemaphoreImpl<DarwinSemaphore>: semaphore specialization for Darwin
//
//@SEE_ALSO: bslmt_semaphore
//
//@DESCRIPTION: This component provides an implementation of 'bslmt::Semaphore'
// for POSIX threads ("pthreads") according to the POSIX support on Darwin
// platform, 'bslmt::SemaphoreImpl<DarwinSemaphore>', via the template
// specialization:
//..
//  bslmt::SemaphoreImpl<Platform::DarwinSemaphore>
//..
// This template class should not be used (directly) by client code.  Clients
// should instead use 'bslmt::Semaphore'.
//
///Usage
///-----
// This component is an implementation detail of 'bslmt' and is *not* intended
// for direct client use.  It is subject to change without notice.  As such, a
// usage example is not provided.

#include <bslscm_version.h>

#include <bsls_assert.h>
#include <bsls_platform.h>

#include <bslmt_platform.h>

#ifdef BSLS_PLATFORM_OS_DARWIN
#include <semaphore.h>

namespace BloombergLP {
namespace bslmt {

template <class SEMAPHORE_POLICY>
class SemaphoreImpl;

              // ==============================================
              // class SemaphoreImpl<Platform::DarwinSemaphore>
              // ==============================================

template <>
class SemaphoreImpl<Platform::DarwinSemaphore> {
    // This class provides a full specialization of 'SemaphoreImpl' for
    // pthreads on Darwin.  The implementation provided here defines an
    // efficient proxy for the 'sem_t' pthread type, and related operations.

    // DATA
    sem_t             *d_sem_p;           // pointer to native semaphore handle
    static const char *s_semaphorePrefix; // prefix for a unique semaphore name

    // NOT IMPLEMENTED
    SemaphoreImpl(const SemaphoreImpl&);
    SemaphoreImpl& operator=(const SemaphoreImpl&);

  public:
    // CREATORS
    SemaphoreImpl(int count);
        // Create a semaphore initialized to the specified 'count'.  This
        // method does not return normally unless there are sufficient system
        // resources to construct the object.

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

              // ----------------------------------------------
              // class SemaphoreImpl<Platform::DarwinSemaphore>
              // ----------------------------------------------

// CREATORS
inline
bslmt::SemaphoreImpl<bslmt::Platform::DarwinSemaphore>::~SemaphoreImpl()
{
    int result = ::sem_close(d_sem_p);

    (void) result;
    BSLS_ASSERT(result == 0);
}

// MANIPULATORS
inline
void bslmt::SemaphoreImpl<bslmt::Platform::DarwinSemaphore>::post()
{
    int result = ::sem_post(d_sem_p);

    (void) result;
    BSLS_ASSERT(result == 0);
}

inline
int bslmt::SemaphoreImpl<bslmt::Platform::DarwinSemaphore>::tryWait()
{
    return ::sem_trywait(d_sem_p);

}

// ACCESSORS
inline
int bslmt::SemaphoreImpl<bslmt::Platform::DarwinSemaphore>::getValue() const
{
    // Not implemented on Darwin, but sem_getvalue still returns success.
    BSLS_ASSERT(false &&
            "sem_getvalue is optional in POSIX and not implemented on Darwin");
    return 0;
}

}  // close enterprise namespace

#endif  // BSLS_PLATFORM_OS_DARWIN

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
