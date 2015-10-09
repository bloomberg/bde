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

#include <unistd.h>
#include <sys/stat.h>

namespace BloombergLP {

              // ----------------------------------------------
              // class SemaphoreImpl<Platform::DarwinSemaphore>
              // ----------------------------------------------

const char *
bslmt::SemaphoreImpl<bslmt::Platform::DarwinSemaphore>::s_semaphorePrefix
    = "bslmt_semaphore_";

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

}  // close unnamed namespace

// CREATORS
bslmt::SemaphoreImpl<bslmt::Platform::DarwinSemaphore>::SemaphoreImpl(
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
