// bdlqq_semaphoreimpl_win32.cpp                                      -*-C++-*-
#include <bdlqq_semaphoreimpl_win32.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlqq_semaphoreimpl_win32_cpp,"$Id$ $CSID$")

#include <bdlqq_lockguard.h>    // for testing only
#include <bdlqq_mutex.h>        // for testing only
#include <bdlqq_threadutil.h>   // for testing only

#include <bdlqq_platform.h>

#ifdef BDLQQ_PLATFORM_WIN32_THREADS

namespace BloombergLP {

           // ---------------------------------------------
           // class SemaphoreImpl<Platform::Win32Semaphore>
           // ---------------------------------------------

// MANIPULATORS
void bdlqq::SemaphoreImpl<bdlqq::Platform::Win32Semaphore>::post(int number)
{
    for (int i = 0; i < number; ++i) {
        post();
    }
}

int bdlqq::SemaphoreImpl<bdlqq::Platform::Win32Semaphore>::tryWait()
{
    for (int i = d_resources; i > 0; i = d_resources) {
        if (i == d_resources.testAndSwap(i, i - 1)) {
            return 0;
        }
    }
    return -1;
}

}  // close namespace BloombergLP

#endif  // BDLQQ_PLATFORM_WIN32_THREADS

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
