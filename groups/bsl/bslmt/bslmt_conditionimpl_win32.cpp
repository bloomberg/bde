// bslmt_conditionimpl_win32.cpp                                      -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bslmt_conditionimpl_win32.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslmt_conditionimpl_win32_cpp,"$Id$ $CSID$")

#include <bslmt_mutex.h>

#ifdef BSLMT_PLATFORM_WIN32_THREADS

// #define BSLMT_WIN64_DEBUG

namespace BloombergLP {

               // -------------------------------------------
               // class ConditionImpl<Platform::Win32Threads>
               // -------------------------------------------

// MANIPULATORS
int bslmt::ConditionImpl<bslmt::Platform::Win32Threads>::timedWait(
                                            Mutex                     *mutex,
                                            const bsls::TimeInterval&  timeout)
{
    const void *sluiceToken = d_waitSluice.enter();
    mutex->unlock();
    const int rc = d_waitSluice.timedWait(sluiceToken, timeout);
    mutex->lock();

    return 0 == rc ? 0 : -1;
}

int bslmt::ConditionImpl<bslmt::Platform::Win32Threads>::wait(Mutex *mutex)
{
    const void *sluiceToken = d_waitSluice.enter();
    mutex->unlock();
    d_waitSluice.wait(sluiceToken);
    mutex->lock();

    return 0;
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
