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
#include <bslmt_saturatedtimeconversionimputil.h>

#include <bsls_systemtime.h>

// Include 'synchapi.h' here to check that our declarations of windows API
// functions and types do not conflict with 'synchapi'.

#ifdef BSLS_PLATFORM_OS_WINDOWS
#include <windows.h>
#include <synchapi.h>
#endif

#ifdef BSLMT_PLATFORM_WIN32_THREADS

namespace BloombergLP {

               // -------------------------------------------
               // class ConditionImpl<Platform::Win32Threads>
               // -------------------------------------------

// MANIPULATORS
int bslmt::ConditionImpl<bslmt::Platform::Win32Threads>::timedWait(
                                            Mutex                     *mutex,
                                            const bsls::TimeInterval&  timeout)
{
    LPCRITICAL_SECTION mtx = &mutex->nativeMutex();
    unsigned int       duration;

    bslmt::SaturatedTimeConversionImpUtil::toMillisec(
                      &duration, timeout - bsls::SystemTime::now(d_clockType));

    BOOL ret = SleepConditionVariableCS(
                          reinterpret_cast<_RTL_CONDITION_VARIABLE *>(&d_cond),
                          mtx,
                          static_cast<DWORD>(duration));
    if (ret != 0) {
        return 0;                                                     // RETURN
    }
    DWORD err = GetLastError();
    return ERROR_TIMEOUT == err ? -1 : -2;
}

int bslmt::ConditionImpl<bslmt::Platform::Win32Threads>::wait(Mutex *mutex)
{
    LPCRITICAL_SECTION mtx = &mutex->nativeMutex();
    BOOL ret = SleepConditionVariableCS(
                          reinterpret_cast<_RTL_CONDITION_VARIABLE *>(&d_cond),
                          mtx,
                          INFINITE);
    return 0 == ret ? -2 : 0;
}

}  // close enterprise namespace

#endif  // BSLMT_PLATFORM_WIN32_THREADS

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
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
