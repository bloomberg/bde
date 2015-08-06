// bdlqq_timedsemaphoreimpl_win32.cpp                                 -*-C++-*-
#include <bdlqq_timedsemaphoreimpl_win32.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlqq_timedsemaphoreimpl_win32_cpp,"$Id$ $CSID$")

#ifdef BDLQQ_PLATFORM_WIN32_THREADS

#include <bcemt_SaturatedTimeConversionimputil.h>

#include <bdlqq_lockguard.h>     // for testing only
#include <bdlqq_mutex.h>         // for testing only
#include <bdlqq_threadutil.h>    // for testing only

namespace BloombergLP {

           // ---------------------------------------------
           // class TimedSemaphoreImpl<Win32TimedSemaphore>
           // ---------------------------------------------

// MANIPULATORS
int
bdlqq::TimedSemaphoreImpl<bdlqq::Platform::Win32TimedSemaphore>::timedWait(
                                             const bsls::TimeInterval& timeout)
{
    DWORD milliTimeout = 0;
    bsls::TimeInterval now = bdlt::CurrentTime::now(d_clockType);
    if (timeout > now) {
        bsls::TimeInterval reltime = timeout - now;
        SaturatedTimeConversionImpUtil::toMillisec(&milliTimeout,
                                                         reltime);
    }

    return WaitForSingleObject(d_handle, milliTimeout);
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
