// bdlqq_semaphoreimpl_pthread.cpp                                    -*-C++-*-
#include <bdlqq_semaphoreimpl_pthread.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlqq_semaphoreimpl_pthread_cpp,"$Id$ $CSID$")

#ifdef BDLQQ_PLATFORM_POSIX_SEMAPHORE

#include <bdlqq_lockguard.h>    // for testing only
#include <bdlqq_mutex.h>        // for testing only
#include <bdlqq_threadutil.h>   // for testing only

#include <bsl_c_errno.h>

namespace BloombergLP {

              // ---------------------------------------------
              // class SemaphoreImpl<Platform::PosixSemaphore>
              // ---------------------------------------------

// CREATORS
bdlqq::SemaphoreImpl<bdlqq::Platform::PosixSemaphore>::SemaphoreImpl(int count)
{
    int result = ::sem_init(&d_sem, 0, count);

    (void) result;
    BSLS_ASSERT(result == 0);
}

// MANIPULATORS
void bdlqq::SemaphoreImpl<bdlqq::Platform::PosixSemaphore>::post(int number)
{
    for (int i = 0; i < number; i++) {
        post();
    }
}

void
bdlqq::SemaphoreImpl<bdlqq::Platform::PosixSemaphore>::wait()
{
    sem_t * sem_p = &d_sem;
    int result = 0;

    do {
        result = ::sem_wait(sem_p);
    } while (result != 0 && errno == EINTR);

    BSLS_ASSERT(result == 0);
}

}  // close enterprise namespace

#endif  // BDLQQ_PLATFORM_POSIX_THREADS

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
