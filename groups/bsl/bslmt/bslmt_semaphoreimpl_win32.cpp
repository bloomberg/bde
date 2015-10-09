// bslmt_semaphoreimpl_win32.cpp                                      -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bslmt_semaphoreimpl_win32.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslmt_semaphoreimpl_win32_cpp,"$Id$ $CSID$")

#include <bslmt_lockguard.h>    // for testing only
#include <bslmt_mutex.h>        // for testing only
#include <bslmt_threadutil.h>   // for testing only

#ifdef BSLMT_PLATFORM_WIN32_THREADS

namespace BloombergLP {

              // ---------------------------------------------
              // class SemaphoreImpl<Platform::Win32Semaphore>
              // ---------------------------------------------

// MANIPULATORS
void bslmt::SemaphoreImpl<bslmt::Platform::Win32Semaphore>::post(int number)
{
    for (int i = 0; i < number; ++i) {
        post();
    }
}

int bslmt::SemaphoreImpl<bslmt::Platform::Win32Semaphore>::tryWait()
{
    for (int i = d_resources; i > 0; i = d_resources) {
        if (i == d_resources.testAndSwap(i, i - 1)) {
            return 0;
        }
    }
    return -1;
}

}  // close enterprise namespace

#endif  // BSLMT_PLATFORM_WIN32_THREADS

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
