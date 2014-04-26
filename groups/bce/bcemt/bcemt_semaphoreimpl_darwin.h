// bcemt_semaphoreimpl_darwin.h                                       -*-C++-*-
#ifndef INCLUDED_BCEMT_SEMAPHOREIMPL_DARWIN
#define INCLUDED_BCEMT_SEMAPHOREIMPL_DARWIN

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a Darwin implementation of 'bcemt_Semaphore'.
//
//@CLASSES:
//  bcemt_SemaphoreImpl<DarwinSemaphore>: semaphore specialization for Darwin
//
//@SEE_ALSO: bcemt_semaphore
//
//@DESCRIPTION: This component provides an implementation of 'bcemt_Semaphore'
// for POSIX threads ("pthreads") according to the POSIX support on Darwin
// platform via the template specialization:
//..
//  bcemt_SemaphoreImpl<bces_Platform::DarwinSemaphore>
//..
// This template class should not be used (directly) by client code.  Clients
// should instead use 'bcemt_Semaphore'.
//
///Usage
///-----
// This component is an implementation detail of 'bcemt' and is *not* intended
// for direct client use.  It is subject to change without notice.  As such, a
// usage example is not provided.

#ifdef BSLS_PLATFORM_OS_DARWIN

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BCES_PLATFORM
#include <bces_platform.h>
#endif

#ifndef INCLUDED_SEMAPHORE
#include <semaphore.h>
#define INCLUDED_SEMAPHORE
#endif

namespace BloombergLP {

template <typename SEMAPHORE_POLICY>
class bcemt_SemaphoreImpl;

         // =========================================================
         // class bcemt_SemaphoreImpl<bces_Platform::DarwinSemaphore>
         // =========================================================

template <>
class bcemt_SemaphoreImpl<bces_Platform::DarwinSemaphore> {
    // This class provides a full specialization of 'bcemt_SemaphoreImpl' for
    // pthreads on Darwin.  The implementation provided here defines an
    // efficient proxy for the 'sem_t' pthread type, and related operations.

    // DATA
    sem_t             *d_sem_p;           // pointer to native semaphore handle
    static const char *s_semaphorePrefix; // prefix for a unique semaphore name

    // NOT IMPLEMENTED
    bcemt_SemaphoreImpl(const bcemt_SemaphoreImpl&);
    bcemt_SemaphoreImpl& operator=(const bcemt_SemaphoreImpl&);

  public:
    // CREATORS
    bcemt_SemaphoreImpl(int count);
        // Create a semaphore initialized to the specified 'count'.

    ~bcemt_SemaphoreImpl();
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

         // ---------------------------------------------------------
         // class bcemt_SemaphoreImpl<bces_Platform::DarwinSemaphore>
         // ---------------------------------------------------------

// CREATORS
inline
bcemt_SemaphoreImpl<bces_Platform::DarwinSemaphore>::~bcemt_SemaphoreImpl()
{
    int result = ::sem_close(d_sem_p);

    (void) result;
    BSLS_ASSERT(result == 0);
}

// MANIPULATORS
inline
void bcemt_SemaphoreImpl<bces_Platform::DarwinSemaphore>::post()
{
    int result = ::sem_post(d_sem_p);

    (void) result;
    BSLS_ASSERT(result == 0);
}

inline
int bcemt_SemaphoreImpl<bces_Platform::DarwinSemaphore>::tryWait()
{
    return ::sem_trywait(d_sem_p);

}

// ACCESSORS
inline
int bcemt_SemaphoreImpl<bces_Platform::DarwinSemaphore>::getValue() const
{
    // Not implemented on Darwin, but sem_getvalue still returns success.
    BSLS_ASSERT(false &&
            "sem_getvalue is optional in POSIX and not implemented on Darwin");
    return 0;
}

}  // close namespace BloombergLP

#endif  // BSLS_PLATFORM_OS_DARWIN

#endif
