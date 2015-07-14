// bdlmtt_semaphoreimpl_darwin.cpp                                     -*-C++-*-
#include <bdlmtt_semaphoreimpl_darwin.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmtt_semaphoreimpl_darwin_cpp,"$Id$ $CSID$")

#ifdef BSLS_PLATFORM_OS_DARWIN

#include <bdlmtt_lockguard.h>    // for testing only
#include <bdlmtt_mutex.h>        // for testing only
#include <bdlmtt_threadutil.h>   // for testing only

#include <bsl_string.h>
#include <bsl_sstream.h>
#include <bsl_iomanip.h>
#include <bsl_ios.h>
#include <bsl_c_errno.h>
#include <bsls_types.h>

#include <unistd.h>
#include <sys/stat.h>

namespace BloombergLP {

         // ---------------------------------------------------------
         // class bdlmtt::SemaphoreImpl<bdlmtt::Platform::DarwinSemaphore>
         // ---------------------------------------------------------

const char *
bdlmtt::SemaphoreImpl<bdlmtt::Platform::DarwinSemaphore>::s_semaphorePrefix
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

namespace bdlmtt {
// CREATORS
SemaphoreImpl<bdlmtt::Platform::DarwinSemaphore>::SemaphoreImpl(
                                                                     int count)
{
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

    (void) result;
    BSLS_ASSERT(result == 0);
}

// MANIPULATORS
void SemaphoreImpl<bdlmtt::Platform::DarwinSemaphore>::post(int number)
{
    for (int i = 0; i < number; i++) {
        post();
    }
}

void
SemaphoreImpl<bdlmtt::Platform::DarwinSemaphore>::wait()
{
    sem_t * sem_p = d_sem_p;
    int result = 0;

    do {
        result = ::sem_wait(sem_p);
    } while (result != 0 && errno == EINTR);

    BSLS_ASSERT(result == 0);
}
}  // close package namespace

}  // close namespace BloombergLP

#endif  // BSLS_PLATFORM_OS_DARWIN
