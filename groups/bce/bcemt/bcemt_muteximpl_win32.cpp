// bcemt_muteximpl_win32.cpp                                          -*-C++-*-
#include <bcemt_muteximpl_win32.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcemt_muteximpl_win32_cpp,"$Id$ $CSID$")

#include <bslmf_assert.h>
#include <bsls_platform.h>

// Include 'windows.h' here to check that our declarations of windows API
// functions and types do not conflict with 'windows.h'.  Also, verify that
// the size of the buffer allocated for the critical section is sufficient.

#ifdef BSLS_PLATFORM__OS_WINDOWS
#include <windows.h>
#endif

#ifdef BCES_PLATFORM__WIN32_THREADS

namespace BloombergLP {

BSLMF_ASSERT(sizeof(CRITICAL_SECTION) == sizeof(void *) *
   bcemt_MutexImpl<bces_Platform::Win32Threads>::CRITICAL_SECTION_BUFFER_SIZE);

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
