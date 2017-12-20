// bslmt_mutexassert.cpp                                              -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bslmt_mutexassert.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslmt_mutex_cpp,"$Id$ $CSID$")

#include <bslmt_mutex.h>
#include <bslmt_platform.h>
#include <bslmt_threadutil.h>     // for testing only

#include <bsls_assert.h>
#include <bsls_platform.h>

#ifdef BSLS_PLATFORM_OS_WINDOWS
#include <windows.h>
#endif

namespace BloombergLP {

void bslmt::MutexAssert_Imp::assertIsLockedImpl(Mutex      *mutex,
                                                const char *text,
                                                const char *file,
                                                int         line)
{
#if   defined(BSLMT_PLATFORM_POSIX_THREADS)

    if (0 == mutex->tryLock()) {    // mutex isn't recursive in pthreads
        // The mutex was unlocked.

        mutex->unlock();    // restore mutex to the state it was originally in
        bsls::Assert::invokeHandler(text, file, line);
    }

#elif defined(BSLMT_PLATFORM_WIN32_THREADS)

    if (-1 == mutex->nativeMutex().LockCount) {
        // The mutex was unlocked.

        bsls::Assert::invokeHandler(text, file, line);
    }

#else
# error unrecognized platform
#endif
}

}  // close enterprise namespace

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
