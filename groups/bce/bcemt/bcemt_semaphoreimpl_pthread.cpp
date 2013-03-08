// bcemt_semaphoreimpl_pthread.cpp                                    -*-C++-*-
#include <bcemt_semaphoreimpl_pthread.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcemt_semaphoreimpl_pthread_cpp,"$Id$ $CSID$")

#ifdef BCES_PLATFORM_POSIX_THREADS

#include <bcemt_lockguard.h>    // for testing only
#include <bcemt_mutex.h>        // for testing only
#include <bcemt_threadutil.h>   // for testing only

#include <bsl_c_errno.h>

#if defined(BSLS_PLATFORM_OS_DARWIN)
#include <bsls_types.h>
#include <bsl_sstream.h>
#include <bsl_iomanip.h>
#include <unistd.h>
#include <sys/stat.h>
#endif

namespace BloombergLP {

             // --------------------------------------------------------
             // class bcemt_SemaphoreImpl<bces_Platform::PosixSemaphore>
             // --------------------------------------------------------

#if defined(BSLS_PLATFORM_OS_DARWIN)
const char *
bcemt_SemaphoreImpl<bces_Platform::PosixSemaphore>::s_semaphorePrefix
    = "bcemt_semaphore_";

namespace {

bsl::string makeUniqueName(const char *prefix, bsls::Types::UintPtr suffix)
    // Create a sufficiently unique name for a semaphore object.  Note that the
    // name of the semaphore shouldn't exceed SEM_NAME_LEN characters (31).
{
    bsl::ostringstream out;
    out << prefix << bsl::hex << (getpid() & 0xffff) << '_'
                              << (suffix & 0xffff);
    return out.str();
}

}
#endif

// CREATORS
bcemt_SemaphoreImpl<bces_Platform::PosixSemaphore>::bcemt_SemaphoreImpl(
                                                                     int count)
{
#if defined(BSLS_PLATFORM_OS_DARWIN)
    bsl::string semaphoreName(
        makeUniqueName(s_semaphorePrefix,
                       reinterpret_cast<bsls::Types::UintPtr>(this)));

    do {
        // create a named semaphore with exclusive access
        d_sem_p = ::sem_open(semaphoreName.c_str(),
                             O_CREAT | O_EXCL,
                             S_IRUSR | S_IWUSR,
                             count);
    } while (d_sem_p == SEM_FAILED && (errno == EEXIST || errno == EINTR));

    BSLS_ASSERT(d_sem_p != SEM_FAILED);

    // At this point the current thread is the sole owner of the semaphore with
    // this name.  No other thread can create a semaphore with the same name
    // until we disassociate the name from the semaphore handle.  Note that
    // even though the name is unlinked from the semaphore, we still try to use
    // sufficiently unique names because if the process is killed before it
    // unlinks the name, no other process can create a semaphore with that
    // name.
    int result = ::sem_unlink(semaphoreName.c_str());
#else
    int result = ::sem_init(&d_sem, 0, count);
#endif

    (void) result;
    BSLS_ASSERT(result == 0);
}

// MANIPULATORS
void bcemt_SemaphoreImpl<bces_Platform::PosixSemaphore>::post(int number)
{
    for (int i = 0; i < number; i++) {
        post();
    }
}

void
bcemt_SemaphoreImpl<bces_Platform::PosixSemaphore>::wait()
{
#if defined(BSLS_PLATFORM_OS_DARWIN)
    sem_t * sem_p = d_sem_p;
#else
    sem_t * sem_p = &d_sem;
#endif

    int result = 0;

    do {
        result = ::sem_wait(sem_p);
    } while (result != 0 && errno == EINTR);

    BSLS_ASSERT(result == 0);
}

}  // close namespace BloombergLP

#endif  // BCES_PLATFORM_POSIX_THREADS

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
