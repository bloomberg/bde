// bslmt_recursivemuteximpl_win32.h                                   -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BSLMT_RECURSIVEMUTEXIMPL_WIN32
#define INCLUDED_BSLMT_RECURSIVEMUTEXIMPL_WIN32

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a win32 implementation of 'bslmt::RecursiveMutex'.
//
//@CLASSES:
//  bslmt::RecursiveMutexImpl<Win32Threads>: win32 specialization
//
//@SEE_ALSO: bslmt_recursivemutex
//
//@DESCRIPTION: This component provides an implementation of
// 'bslmt::RecursiveMutex' for Windows (win32),
// 'bslmt::RecursiveMutexImpl<Win32Threads>', via the template specialization:
//..
//  bslmt::RecursiveMutexImpl<Platform::Win32Threads>
//..
// This template class should not be used (directly) by client code.  Clients
// should instead use 'bslmt::RecursiveMutex'.
//
///Usage
///-----
// This component is an implementation detail of 'bslmt' and is *not* intended
// for direct client use.  It is subject to change without notice.  As such, a
// usage example is not provided.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMT_PLATFORM
#include <bslmt_platform.h>
#endif

#ifdef BSLMT_PLATFORM_WIN32_THREADS

// Platform specific implementation starts here.

#ifndef INCLUDED_BSLMT_MUTEXIMPL_WIN32
#include <bslmt_muteximpl_win32.h>
#endif

namespace BloombergLP {
namespace bslmt {

template <class THREAD_POLICY>
class RecursiveMutexImpl;

             // ================================================
             // class RecursiveMutexImpl<Platform::Win32Threads>
             // ================================================

template <>
class RecursiveMutexImpl<Platform::Win32Threads>
: public MutexImpl<Platform::Win32Threads> {
    // This class provides a full specialization of 'RecursiveMutexImpl' for
    // Windows.  Since the Windows specialization of 'MutexImpl' is recursive,
    // this type is effectively an alias for that one.
};

}  // close package namespace
}  // close enterprise namespace

#endif  // BSLMT_PLATFORM_WIN32_THREADS

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
