// bdlmtt_recursivemuteximpl_win32.h                                   -*-C++-*-
#ifndef INCLUDED_BDLMTT_RECURSIVEMUTEXIMPL_WIN32
#define INCLUDED_BDLMTT_RECURSIVEMUTEXIMPL_WIN32

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a win32 implementation of 'bdlmtt::RecursiveMutex'.
//
//@CLASSES:
//  bdlmtt::RecursiveMutexImpl<Win32Threads>: win32 specialization
//
//@SEE_ALSO: bdlmtt_recursivemutex
//
//@AUTHOR: Ilougino Rocha (irocha)
//
//@DESCRIPTION: This component provides an implementation of
// 'bdlmtt::RecursiveMutex' for Windows (win32) via the template specialization:
//..
//  bdlmtt::RecursiveMutexImpl<bdlmtt::Platform::Win32Threads>
//..
// This template class should not be used (directly) by client code.  Clients
// should instead use 'bdlmtt::RecursiveMutex'.
//
///Usage
///-----
// This component is an implementation detail of 'bcemt' and is *not* intended
// for direct client use.  It is subject to change without notice.  As such, a
// usage example is not provided.

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLMTT_PLATFORM
#include <bdlmtt_platform.h>
#endif

#ifdef BDLMTT_PLATFORM_WIN32_THREADS

// Platform specific implementation starts here.

#ifndef INCLUDED_BDLMTT_MUTEXIMPL_WIN32
#include <bdlmtt_muteximpl_win32.h>
#endif

namespace BloombergLP {


namespace bdlmtt {template <typename THREAD_POLICY>
class RecursiveMutexImpl;

        // ===========================================================
        // class RecursiveMutexImpl<bdlmtt::Platform::Win32Threads>
        // ===========================================================

template <>
class RecursiveMutexImpl<bdlmtt::Platform::Win32Threads> :
                          public MutexImpl<bdlmtt::Platform::Win32Threads> {
    // This class provides a full specialization of 'RecursiveMutexImpl'
    // for Windows.  Since the Windows specialization of 'MutexImpl'
    // is recursive, this type is effectively an alias for that one.
};
}  // close package namespace

}  // close namespace BloombergLP

#endif  // BDLMTT_PLATFORM_WIN32_THREADS

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
