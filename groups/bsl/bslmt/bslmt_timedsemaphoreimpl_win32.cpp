// bslmt_timedsemaphoreimpl_win32.cpp                                 -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bslmt_timedsemaphoreimpl_win32.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslmt_timedsemaphoreimpl_win32_cpp,"$Id$ $CSID$")

#ifdef BSLMT_PLATFORM_WIN32_THREADS

#include <bslmt_saturatedtimeconversionimputil.h>

#include <bslmt_lockguard.h>     // for testing only
#include <bslmt_mutex.h>         // for testing only
#include <bslmt_threadutil.h>    // for testing only

#include <bsls_systemtime.h>

namespace BloombergLP {

              // ---------------------------------------------
              // class TimedSemaphoreImpl<Win32TimedSemaphore>
              // ---------------------------------------------

// MANIPULATORS
int
bslmt::TimedSemaphoreImpl<bslmt::Platform::Win32TimedSemaphore>::timedWait(
                                             const bsls::TimeInterval& timeout)
{
    DWORD milliTimeout = 0;
    bsls::TimeInterval now = bsls::SystemTime::now(d_clockType);
    if (timeout > now) {
        bsls::TimeInterval reltime = timeout - now;
        SaturatedTimeConversionImpUtil::toMillisec(&milliTimeout, reltime);
    }

    return WaitForSingleObject(d_handle, milliTimeout);
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
