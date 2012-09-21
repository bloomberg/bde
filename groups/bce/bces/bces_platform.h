// bces_platform.h                                                    -*-C++-*-
#ifndef INCLUDED_BCES_PLATFORM
#define INCLUDED_BCES_PLATFORM

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide platform-dependent thread-related trait definitions.
//
//@CLASSES:
//   bces_Platform: namespace for platform-dependent thread-related traits
//
//@SEE_ALSO:
//
//@AUTHOR: Shawn Edwards (sedwards)
//
//@DESCRIPTION: This component defines a set of traits that identify and
// describe a platform's concurrency properties.  For example, the
// 'ThreadPolicy' trait is ascribed a "value" (i.e., POSIX or Win32)
// appropriate for each supported platform.  The various concurrency traits are
// actually types declared in the 'bces_Platform' 'struct'.  These types are
// intended to be used in specializing template implementations or to enable
// function overloading based on the prevalent system's characteristics.
//
// This component also defines a 'SemaphorePolicy' trait used for selecting a
// semaphore implementation.  Differences among POSIX implementations lead to
// different semaphore policies for the same 'ThreadPolicy'.
//
// Finally, this component defines a 'TimedSemaphorePolicy' trait used for
// selecting a timed-semaphore implementation.  POSIX platforms that do not
// have a native timed-wait for semaphores require a custom (pthread-based)
// implementation.

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

                            // ===================
                            // class bces_Platform
                            // ===================

namespace BloombergLP {

struct bces_Platform {
    // This 'struct' provides a namespace for concurrency trait definitions.

                       // 'ThreadPolicy' trait

    struct PosixThreads {};
    struct Win32Threads {};

    #ifdef BSLS_PLATFORM_OS_UNIX

    typedef PosixThreads ThreadPolicy;
    #define BCES_PLATFORM_POSIX_THREADS 1

    #endif

    #ifdef BSLS_PLATFORM_OS_WINDOWS

    typedef Win32Threads ThreadPolicy;
    #define BCES_PLATFORM_WIN32_THREADS 1

    #endif

                       // 'SemaphorePolicy' trait

    struct CountedPosixSemaphore {};
    struct PosixSemaphore {};
    struct Win32Semaphore {};

    #ifdef BSLS_PLATFORM_OS_UNIX

    #ifdef BSLS_PLATFORM_OS_AIX

    // The POSIX semaphore on IBM has a maximum count of 32k.  Other POSIX
    // implementations support counts up to 'INT_MAX', and, historically,
    // 'bcemt_Semaphore' also supported 'INT_MAX'.  On AIX, use a semaphore
    // that maintains the count in a separate atomic integer to enable
    // consistent semaphore usage across platforms.

    typedef CountedPosixSemaphore SemaphorePolicy;

    #else

    typedef PosixSemaphore SemaphorePolicy;

    #endif

    #endif

    #ifdef BSLS_PLATFORM_OS_WINDOWS

    typedef Win32Semaphore SemaphorePolicy;

    #endif

                       // 'TimedSemaphorePolicy' trait

    struct PosixAdvTimedSemaphore {};
    struct PthreadTimedSemaphore {};
    struct Win32TimedSemaphore {};

    #ifdef BSLS_PLATFORM_OS_UNIX

    #if defined(BSLS_PLATFORM_OS_SUNOS)   || \
        defined(BSLS_PLATFORM_OS_SOLARIS) || \
        defined(BSLS_PLATFORM_OS_LINUX)      \

    typedef PosixAdvTimedSemaphore TimedSemaphorePolicy;
    #define BCES_PLATFORM_POSIXADV_TIMEDSEMAPHORE 1

    #else  // 'sem_timedwait' not available; use custom pthread-based semaphore

    typedef PthreadTimedSemaphore TimedSemaphorePolicy;

    #endif

    #endif

    #ifdef BSLS_PLATFORM_OS_WINDOWS

    typedef Win32TimedSemaphore TimedSemaphorePolicy;

    #endif
};

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
