// bdlmtt_semaphoreimpl_counted.h                                      -*-C++-*-
#ifndef INCLUDED_BDLMTT_SEMAPHOREIMPL_COUNTED
#define INCLUDED_BDLMTT_SEMAPHOREIMPL_COUNTED

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an implementation of 'bdlmtt::Semaphore' with count.
//
//@CLASSES:
//  bdlmtt::SemaphoreImpl<CountedSemaphore>: semaphore specialization with count
//
//@SEE_ALSO: bdlmtt_semaphore
//
//@DESCRIPTION: This component provides an implementation of 'bdlmtt::Semaphore'
// via the template specialization:
//..
//  bdlmtt::SemaphoreImpl<bdlmtt::Platform::CountedSemaphore>
//..
// This template class should not be used (directly) by client code.  Clients
// should instead use 'bdlmtt::Semaphore'.
//
// This implementation of 'bdlmtt::Semaphore' is intended for platforms where a
// separate count must be maintained.  'bdlmtt::Semaphore' supports large values,
// but the native semaphores provided on some platforms are restricted to a
// relatively small range of values (e.g., '[ 0 .. 32000 ]' on AIX) and on
// some other platforms do not provide a count at all (Darwin).  To support
// uniform usage across platforms, this component maintains the count of the
// semaphore in a separate atomic integer.  'post' is only invoked on the
// underlying semaphore when it is known there are threads blocked on it.
//
///Usage
///-----
// This component is an implementation detail of 'bcemt' and is *not* intended
// for direct client use.  It is subject to change without notice.  As such, a
// usage example is not provided.

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLMTT_PLATFORM
#include <bdlmtt_platform.h>
#endif

#ifdef BDLMTT_PLATFORM_COUNTED_SEMAPHORE

// Platform-specific implementation starts here.

#ifndef INCLUDED_BSLS_ATOMIC
#include <bsls_atomic.h>
#endif

#ifndef BCEMT_SEMAPHOREIMPL_PTHREAD
#include <bdlmtt_semaphoreimpl_pthread.h>
#endif

#ifndef BCEMT_SEMAPHOREIMPL_DARWIN
#include <bdlmtt_semaphoreimpl_darwin.h>
#endif

namespace BloombergLP {


namespace bdlmtt {template <typename SEMAPHORE_POLICY>
class SemaphoreImpl;

         // ==========================================================
         // class SemaphoreImpl<bdlmtt::Platform::CountedSemaphore>
         // ==========================================================

template <>
class SemaphoreImpl<bdlmtt::Platform::CountedSemaphore> {
    // This class provides a full specialization of 'SemaphoreImpl' with
    // a separate count variable.  This implementation maintains the value of
    // the semaphore in a separate atomic integer count, so as to allow for
    // semaphore count on platforms where a semaphore implementation doesn't
    // provide the count or the provided count has very limited range of
    // values.

    // DATA
    bsls::AtomicInt d_resources; // if positive, number of available resources
                                 // if negative: number of waiting threads

    SemaphoreImpl<bdlmtt::Platform::CountedSemaphoreImplPolicy>
                   d_sem;        // platform semaphore implementation

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

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

         // ----------------------------------------------------------
         // class SemaphoreImpl<bdlmtt::Platform::CountedSemaphore>
         // ----------------------------------------------------------

// CREATORS
inline
SemaphoreImpl<bdlmtt::Platform::CountedSemaphore>::SemaphoreImpl(
                                                                     int count)
: d_resources(count)
, d_sem(0)
{
}

inline
SemaphoreImpl<bdlmtt::Platform::CountedSemaphore>::~SemaphoreImpl()
{
}

// MANIPULATORS
inline
void SemaphoreImpl<bdlmtt::Platform::CountedSemaphore>::post()
{
    if (++d_resources <= 0) {
        d_sem.post();
    }
}

inline
void
SemaphoreImpl<bdlmtt::Platform::CountedSemaphore>::post(int number)
{
    for (int i = 0; i < number; ++i) {
        post();
    }
}

inline
int SemaphoreImpl<bdlmtt::Platform::CountedSemaphore>::tryWait()
{
    for (int i = d_resources; i > 0; i = d_resources) {
        if (i == d_resources.testAndSwap(i, i - 1)) {
            return 0;
        }
    }

    return -1;
}

inline
void SemaphoreImpl<bdlmtt::Platform::CountedSemaphore>::wait()
{
    if (--d_resources >= 0) {
        return;
    }

    d_sem.wait();
}

// ACCESSORS
inline
int SemaphoreImpl<bdlmtt::Platform::CountedSemaphore>::getValue() const
{
    const int v = d_resources;
    return v > 0 ? v : 0;
}
}  // close package namespace

}  // close namespace BloombergLP

#endif

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2013
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
