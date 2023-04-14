// bslmt_semaphoreimpl_darwin.cpp                                     -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bslmt_semaphoreimpl_darwin.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslmt_semaphoreimpl_darwin_cpp,"$Id$ $CSID$")

#ifdef BSLS_PLATFORM_OS_DARWIN

#include <bslmt_lockguard.h>    // for testing only
#include <bslmt_mutex.h>        // for testing only
#include <bslmt_threadutil.h>   // for testing only

#include <bsl_string.h>
#include <bsl_sstream.h>
#include <bsl_iomanip.h>
#include <bsl_ios.h>
#include <bsl_c_errno.h>
#include <bsls_types.h>

#include <stdio.h>         // 'snprintf'
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>      // needed before sys/posix_sem.h to define
                           // 'itimerval'
#include <sys/posix_sem.h> // 'PSEMNAMLEN'

#ifndef SEM_NAME_LEN
#define SEM_NAME_LEN PSEMNAMLEN
#endif

namespace BloombergLP {

              // ----------------------------------------------
              // class SemaphoreImpl<Platform::DarwinSemaphore>
              // ----------------------------------------------

const char *
bslmt::SemaphoreImpl<bslmt::Platform::DarwinSemaphore>::s_semaphorePrefix
    = "bslmt_semaphore_";

namespace {

void
makeUniqueName(char *buffer, const char *prefix, bsls::Types::UintPtr suffix)
    // Create a sufficiently unique name for a semaphore object.  Note that the
    // name of the semaphore shouldn't exceed SEM_NAME_LEN characters (31).
{
    snprintf(buffer, SEM_NAME_LEN, "%s%04x_%04x",
                                   prefix,
                                   (getpid() & 0xffff),
                                   static_cast<unsigned>(suffix & 0xffff));
}

}  // close unnamed namespace

// CREATORS
bslmt::SemaphoreImpl<bslmt::Platform::DarwinSemaphore>::SemaphoreImpl(
                                                                     int count)
{
    char semaphoreName[SEM_NAME_LEN + 1];

    makeUniqueName(semaphoreName,
                   s_semaphorePrefix,
                   reinterpret_cast<bsls::Types::UintPtr>(this));

    do {
        // create a named semaphore with exclusive access
        d_sem_p = ::sem_open(semaphoreName,
                             O_CREAT | O_EXCL,
                             S_IRUSR | S_IWUSR,
                             count);
    } while (d_sem_p == SEM_FAILED && (errno == EEXIST || errno == EINTR));

    BSLS_ASSERT_OPT(d_sem_p != SEM_FAILED);

    // At this point the current thread is the sole owner of the semaphore with
    // this name.  No other thread can create a semaphore with the same name
    // until we disassociate the name from the semaphore handle.  Note that
    // even though the name is unlinked from the semaphore, we still try to use
    // sufficiently unique names because if the process is killed before it
    // unlinks the name, no other process can create a semaphore with that
    // name.
    int result = ::sem_unlink(semaphoreName);

    (void) result;
    BSLS_ASSERT(result == 0);
}

// MANIPULATORS
void bslmt::SemaphoreImpl<bslmt::Platform::DarwinSemaphore>::post(int number)
{
    for (int i = 0; i < number; i++) {
        post();
    }
}

void
bslmt::SemaphoreImpl<bslmt::Platform::DarwinSemaphore>::wait()
{
    sem_t * sem_p = d_sem_p;
    int result = 0;

    do {
        result = ::sem_wait(sem_p);
    } while (result != 0 && errno == EINTR);

    BSLS_ASSERT(result == 0);
}

}  // close enterprise namespace

#endif  // BSLS_PLATFORM_OS_DARWIN

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
