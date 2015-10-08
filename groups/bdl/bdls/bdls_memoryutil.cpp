// bdls_memoryutil.cpp                                                -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdls_memoryutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdls_memoryutil_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>
#include <bsls_platform.h>

#ifdef BSLS_PLATFORM_OS_WINDOWS
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

#ifdef BSLS_PLATFORM_OS_WINDOWS

namespace bdls {
// Windows-specific implementations

int MemoryUtil::protect(void *address, int numBytes, int mode)
{
    BSLS_ASSERT(address);
    BSLS_ASSERT(0 <= numBytes);

    // Separate read/write/exec access bits do not map to modes available in
    // Windows (there is no write-only mode).  So in certain cases, extra
    // permissions are granted.

    static const DWORD nativeMode[] = {
        PAGE_NOACCESS,          // k_ACCESS_NONE
        PAGE_READONLY,          // k_ACCESS_READ
        PAGE_READWRITE,         // k_ACCESS_WRITE
        PAGE_READWRITE,         // k_ACCESS_WRITE   | k_ACCESS_READ
        PAGE_EXECUTE,           // k_ACCESS_EXECUTE
        PAGE_EXECUTE_READ,      // k_ACCESS_EXECUTE | k_ACCESS_READ
        PAGE_EXECUTE_READWRITE, // k_ACCESS_EXECUTE | k_ACCESS_WRITE
        PAGE_EXECUTE_READWRITE  // k_ACCESS_EXECUTE | k_ACCESS_WRITE
                                //                  | k_ACCESS_READ
    };

    DWORD oldProtectMode;
    const int rc = VirtualProtect(address,
                                  numBytes,
                                  nativeMode[mode],
                                  &oldProtectMode);
    return rc ? 0 : -1;
}

int MemoryUtil::pageSize()
{
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return si.dwAllocationGranularity;
}

void *MemoryUtil::allocate(int numBytes)
{
    BSLS_ASSERT(0 <= numBytes);

    return VirtualAlloc(NULL, numBytes, MEM_COMMIT, PAGE_READWRITE);
}

int MemoryUtil::deallocate(void *address)
{
    BSLS_ASSERT(address);

    return VirtualFree(address, 0, MEM_RELEASE) ? 0 : -1;
}
}  // close package namespace

#else

namespace bdls {
// UNIX-specific implementations

int MemoryUtil::pageSize()
{
    return ::sysconf(_SC_PAGESIZE);
}

int MemoryUtil::protect(void *address, int numBytes, int mode)
{
    BSLS_ASSERT(address);
    BSLS_ASSERT(0 <= numBytes);

    int protect = 0;

    if (mode & k_ACCESS_READ) {
        protect |= PROT_READ;
    }
    if (mode & k_ACCESS_WRITE) {
        protect |= PROT_WRITE;
    }
    if (mode & k_ACCESS_EXECUTE) {
        protect |= PROT_EXEC;
    }

    const int rc = ::mprotect(static_cast<char *>(address), numBytes, protect);
    BSLS_ASSERT(rc != EINVAL);  // TBD ???
    return rc;
}

void *MemoryUtil::allocate(int numBytes)
{
    BSLS_ASSERT(0 <= numBytes);

    return ::valloc(numBytes);
}

int MemoryUtil::deallocate(void *address)
{
    BSLS_ASSERT(address);

    ::free(address);
    return 0;
}
}  // close package namespace

#endif

}  // close enterprise namespace

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
