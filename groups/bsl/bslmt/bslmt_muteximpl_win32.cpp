// bslmt_muteximpl_win32.cpp                                          -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bslmt_muteximpl_win32.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslmt_muteximpl_win32_cpp,"$Id$ $CSID$")

#include <bslmf_assert.h>
#include <bsls_platform.h>

// Include 'windows.h' here to check that our declarations of windows API
// functions and types do not conflict with 'windows.h'.  Also, verify that the
// size of the buffer allocated for the critical section is sufficient.

#ifdef BSLS_PLATFORM_OS_WINDOWS
#include <windows.h>
#endif

#ifdef BSLMT_PLATFORM_WIN32_THREADS

namespace BloombergLP {

BSLMF_ASSERT(sizeof(CRITICAL_SECTION) ==
             sizeof(void *) * bslmt::MutexImpl<bslmt::Platform::Win32Threads>::
                                               k_CRITICAL_SECTION_BUFFER_SIZE);

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
