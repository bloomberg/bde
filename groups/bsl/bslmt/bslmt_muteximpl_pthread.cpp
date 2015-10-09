// bslmt_muteximpl_pthread.cpp                                        -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bslmt_muteximpl_pthread.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslmt_muteximpl_pthread_cpp,"$Id$ $CSID$")

#include <bslmf_assert.h>
#include <bsls_assert.h>

#ifdef BSLMT_PLATFORM_POSIX_THREADS

namespace BloombergLP {

// Assert 'sizeof' assumption that is made in destructor (below).

BSLMF_ASSERT(
   0 == sizeof(bslmt::MutexImpl<bslmt::Platform::PosixThreads>) % sizeof(int));

                 // ---------------------------------------
                 // class MutexImpl<Platform::PosixThreads>
                 // ---------------------------------------

// CREATORS
bslmt::MutexImpl<bslmt::Platform::PosixThreads>::~MutexImpl()
{
    const int status = pthread_mutex_destroy(&d_lock);
    BSLS_ASSERT(0 == status);

#ifdef BDE_BUILD_TARGET_SAFE
    // Since we do not want to burden 'lock' and 'unlock' with the overhead of
    // 'BSLS_ASSERT_OPT' (as their 'status' values should always be 0 in a
    // well-behaved program), we overwrite 'd_lock' with garbage so as to
    // (potentially) trigger a fault on attempts to lock or unlock following
    // destruction.

    int       *pInt = reinterpret_cast<int *>(&d_lock);
    const int *pEnd = reinterpret_cast<const int *>(&d_lock + 1);

    while (pInt < pEnd) {
        *pInt = 0xdeadbeef;
        ++pInt;
    }
#endif
}

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
