// bslmt_platform.h                                                   -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BSLMT_PLATFORM
#define INCLUDED_BSLMT_PLATFORM

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide platform-dependent thread-related trait definitions.
//
//@CLASSES:
//   Platform: namespace for platform-dependent thread-related traits
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component defines a set of traits that identify and
// describe a platform's concurrency properties.  For example, the
// 'ThreadPolicy' trait is ascribed a "value" (i.e., POSIX or Win32)
// appropriate for each supported platform.  The various concurrency traits are
// actually types declared in the 'Platform' 'struct'.  These types are
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

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_ATOMIC
#include <bsls_atomic.h>
#endif

namespace BloombergLP {
namespace bslmt {

                              // ==============
                              // class Platform
                              // ==============

struct Platform {
    // This 'struct' provides a namespace for concurrency trait definitions.

                       // 'ThreadPolicy' trait

    struct PosixThreads {};
    struct Win32Threads {};

    #ifdef BSLS_PLATFORM_OS_UNIX

    typedef PosixThreads ThreadPolicy;
    #define BSLMT_PLATFORM_POSIX_THREADS 1

    #endif

    #ifdef BSLS_PLATFORM_OS_WINDOWS

    typedef Win32Threads ThreadPolicy;
    #define BSLMT_PLATFORM_WIN32_THREADS 1

    #endif

                       // 'SemaphorePolicy' trait

    struct CountedSemaphore {};
    struct PosixSemaphore {};
    struct DarwinSemaphore {};
    struct Win32Semaphore {};

    #ifdef BSLS_PLATFORM_OS_UNIX

    #if defined(BSLS_PLATFORM_OS_AIX)

    // The POSIX semaphore on IBM has a maximum count of 32k.  Other POSIX
    // implementations support counts up to 'INT_MAX', and, historically,
    // 'bslmt::Semaphore' also supported 'INT_MAX'.  On AIX, use a semaphore
    // that maintains the count in a separate atomic integer to enable
    // consistent semaphore usage across platforms.

    typedef CountedSemaphore SemaphorePolicy;
    typedef PosixSemaphore CountedSemaphoreImplPolicy;
    #define BSLMT_PLATFORM_COUNTED_SEMAPHORE
    #define BSLMT_PLATFORM_POSIX_SEMAPHORE

    #elif defined(BSLS_PLATFORM_OS_DARWIN)

    // Darwin doesn't implement sem_getvalue.

    typedef CountedSemaphore SemaphorePolicy;
    typedef DarwinSemaphore CountedSemaphoreImplPolicy;
    #define BSLMT_PLATFORM_COUNTED_SEMAPHORE

    #else

    typedef PosixSemaphore SemaphorePolicy;
    #define BSLMT_PLATFORM_POSIX_SEMAPHORE

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
    #define BSLMT_PLATFORM_POSIXADV_TIMEDSEMAPHORE 1

    #else  // 'sem_timedwait' not available; use custom pthread-based semaphore

    typedef PthreadTimedSemaphore TimedSemaphorePolicy;

    #endif

    #endif

    #ifdef BSLS_PLATFORM_OS_WINDOWS

    typedef Win32TimedSemaphore TimedSemaphorePolicy;

    #endif

    enum {
      // This constant can be used in synchronization mechanisms to separate
      // member variables to prevent "false sharing".  For POWER cpus:
      // http://www.ibm.com/developerworks/power/library/pa-memory/index.html
      // has a simple program to determine the cache line size of the CPU.
      // Current Power cpus have 128-byte cache lines.
      //
      // On Solaris, to determine the cache line size on the local cpu, run:
      // ..
      //   prtconf -pv | grep -i l1-dcache-line-size | sort -u
      // .. Older sparc cpus have 32-byte cache lines, newer 64-byte cache
      // lines.  We'll assume 64-byte cache lines here.
      //
      // On Linux with 'sysfs' support,
      //..
      //  cat /sys/devices/system/cpu/cpu0/cache/index0/coherency_line_size
      //..
      // or
      //..
      //  cat /proc/cpuinfo | grep cache
      //..
      // Post SSE2 cpus have the clflush instruction which can be used to write
      // a program similar to the one mentioned above for POWER cpus.  Current
      // x86/x86_64 have 64-byte cache lines.
      //
      // It is obviously suboptimal to determine this at compile time.  We
      // might want to do this at runtime, but this would add at least one
      // level of indirection.
#ifdef BSLS_PLATFORM_CPU_POWERPC
        e_CACHE_LINE_SIZE = 128
#else
        e_CACHE_LINE_SIZE = 64
#endif
    };
};

}  // close package namespace
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
