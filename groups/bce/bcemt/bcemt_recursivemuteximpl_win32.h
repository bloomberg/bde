// bcemt_recursivemuteximpl_win32.h                                   -*-C++-*-
#ifndef INCLUDED_BCEMT_RECURSIVEMUTEXIMPL_WIN32
#define INCLUDED_BCEMT_RECURSIVEMUTEXIMPL_WIN32

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a win32 implementation of 'bcemt_RecursiveMutex'.
//
//@CLASSES:
//  bcemt_RecursiveMutexImpl<Win32Threads>: win32 specialization
//
//@SEE_ALSO: bcemt_recursivemutex
//
//@AUTHOR: Ilougino Rocha (irocha)
//
//@DESCRIPTION: This component provides an implementation of
// 'bcemt_RecursiveMutex' for Windows (win32) via the template specialization:
//..
//  bcemt_RecursiveMutexImpl<bces_Platform::Win32Threads>
//..
// This template class should not be used (directly) by client code.  Clients
// should instead use 'bcemt_RecursiveMutex'.
//
///Usage
///-----
// This component is an implementation detail of 'bcemt' and is *not* intended
// for direct client use.  It is subject to change without notice.  As such, a
// usage example is not provided.

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCES_PLATFORM
#include <bces_platform.h>
#endif

#ifdef BCES_PLATFORM__WIN32_THREADS

// Platform specific implementation starts here.

#ifndef INCLUDED_BCEMT_MUTEXIMPL_WIN32
#include <bcemt_muteximpl_win32.h>
#endif

namespace BloombergLP {

template <typename THREAD_POLICY>
class bcemt_RecursiveMutexImpl;

        // ===========================================================
        // class bcemt_RecursiveMutexImpl<bces_Platform::Win32Threads>
        // ===========================================================

template <>
class bcemt_RecursiveMutexImpl<bces_Platform::Win32Threads> :
                          public bcemt_MutexImpl<bces_Platform::Win32Threads> {
    // This class provides a full specialization of 'bcemt_RecursiveMutexImpl'
    // for Windows.  Since the Windows specialization of 'bcemt_MutexImpl'
    // is recursive, this type is effectively an alias for that one.
};

}  // close namespace BloombergLP

#endif  // BCES_PLATFORM__WIN32_THREADS

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
