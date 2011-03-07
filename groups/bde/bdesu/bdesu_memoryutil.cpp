// bdesu_memoryutil.cpp                                               -*-C++-*-
#include <bdesu_memoryutil.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdesu_memoryutil_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>
#include <bsls_platform.h>

#ifdef BSLS_PLATFORM__OS_WINDOWS
#ifndef INCLUDED_WINDOWS
#include <windows.h>
#define INCLUDED_WINDOWS
#endif
#else
#include <sys/types.h>
#include <sys/mman.h>
#include <bsl_c_errno.h>
#include <unistd.h>
#include <bsl_c_stdlib.h>
#endif

namespace BloombergLP {

#ifdef BSLS_PLATFORM__OS_WINDOWS

// Windows-specific implementations

int bdesu_MemoryUtil::protect(void *address, int numBytes, int mode)
{
    BSLS_ASSERT(address);
    BSLS_ASSERT(0 <= numBytes);

    // Separate read/write/exec access bits do not map to modes available in
    // Windows (there is no write-only mode).  So in certain cases, extra
    // permissions are granted.

    static const DWORD nativeMode[] = {
        PAGE_NOACCESS,          // BDESU_ACCESS_NONE
        PAGE_READONLY,          // BDESU_ACCESS_READ
        PAGE_READWRITE,         // BDESU_ACCESS_WRITE
        PAGE_READWRITE,         // BDESU_ACCESS_WRITE | BDESU_ACCESS_READ
        PAGE_EXECUTE,           // BDESU_ACCESS_EXECUTE
        PAGE_EXECUTE_READ,      // BDESU_ACCESS_EXECUTE | BDESU_ACCESS_READ
        PAGE_EXECUTE_READWRITE, // BDESU_ACCESS_EXECUTE | BDESU_ACCESS_WRITE
        PAGE_EXECUTE_READWRITE  // BDESU_ACCESS_EXECUTE | BDESU_ACCESS_WRITE
                                //                      | BDESU_ACCESS_READ
    };

    DWORD oldProtectMode;
    const int rc = VirtualProtect(address,
                                  numBytes,
                                  nativeMode[mode],
                                  &oldProtectMode);
    return rc ? 0 : -1;
}

int bdesu_MemoryUtil::pageSize()
{
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return si.dwAllocationGranularity;
}

void *bdesu_MemoryUtil::allocate(int numBytes)
{
    BSLS_ASSERT(0 <= numBytes);

    return VirtualAlloc(NULL, numBytes, MEM_COMMIT, PAGE_READWRITE);
}

int bdesu_MemoryUtil::deallocate(void *address)
{
    BSLS_ASSERT(address);

    return VirtualFree(address, 0, MEM_RELEASE) ? 0 : -1;
}

#else

// UNIX-specific implementations

int bdesu_MemoryUtil::pageSize()
{
    return ::sysconf(_SC_PAGESIZE);
}

int bdesu_MemoryUtil::protect(void *address, int numBytes, int mode)
{
    BSLS_ASSERT(address);
    BSLS_ASSERT(0 <= numBytes);

    int protect = 0;

    if (mode & BDESU_ACCESS_READ) {
        protect |= PROT_READ;
    }
    if (mode & BDESU_ACCESS_WRITE) {
        protect |= PROT_WRITE;
    }
    if (mode & BDESU_ACCESS_EXECUTE) {
        protect |= PROT_EXEC;
    }

    const int rc = ::mprotect(static_cast<char *>(address), numBytes, protect);
    BSLS_ASSERT(rc != EINVAL);  // TBD ???
    return rc;
}

void *bdesu_MemoryUtil::allocate(int numBytes)
{
    BSLS_ASSERT(0 <= numBytes);

    return ::valloc(numBytes);
}

int bdesu_MemoryUtil::deallocate(void *address)
{
    BSLS_ASSERT(address);

    ::free(address);
    return 0;
}

#endif

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007, 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
