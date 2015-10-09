// bdlma_guardingallocator.cpp                                        -*-C++-*-
#include <bdlma_guardingallocator.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlma_guardingallocator_cpp,"$Id$ $CSID$")

#include <bslmf_assert.h>

#include <bsls_alignmentutil.h>
#include <bsls_assert.h>
#include <bsls_atomic.h>
#include <bsls_exceptionutil.h>      // 'BSLS_THROW'
#include <bsls_performancehint.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#include <bsl_cstddef.h>             // 'bsl::size_t'
#include <bsl_new.h>                 // 'bsl::bad_alloc'

#ifdef BSLS_PLATFORM_OS_WINDOWS

#include <windows.h>   // 'GetSystemInfo', 'VirtualAlloc', 'VirtualFree',
                       // 'VirtualProtect'
#else

#include <errno.h>     // 'errno'
#include <string.h>    // 'strerror'
#include <sys/mman.h>  // 'mmap', 'mprotect', 'munmap'
#include <unistd.h>    // 'sysconf'

#endif




namespace BloombergLP {
namespace {

// Define the offset (in bytes) from the address returned to the user in which
// to stash reference addresses ('e_AFTER_USER_BLOCK' only).

static const bslma::Allocator::size_type OFFSET =
                                       bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT;

struct AfterUserBlockDeallocationData
    // Helper struct storing the addresses we need for deallocation when the
    // guard page location is 'e_AFTER_USER_BLOCK'.
{
    void *d_firstPage; // address we need to deallocate
    void *d_guardPage; // address of the page we need to unprotect
};

AfterUserBlockDeallocationData *getDataBlockAddress(void *address)
    // Utility function to compute the 'AfterUserBlockDeallocationData*'
    // corresponding to the specified 'address'.
{
    return static_cast<AfterUserBlockDeallocationData*>(
            static_cast<void*>(
                static_cast<char *>(address) - OFFSET * 2));
}

// Assert that we can fit our struct into the available space.
BSLMF_ASSERT(sizeof(AfterUserBlockDeallocationData) <= OFFSET * 2);

// HELPER FUNCTIONS

int getSystemPageSize()
    // Return the size (in bytes) of a system memory page.
{
    static bsls::AtomicInt pageSize(0);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(0 == pageSize.loadRelaxed())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

#ifdef BSLS_PLATFORM_OS_WINDOWS

        SYSTEM_INFO info;
        GetSystemInfo(&info);
        pageSize = static_cast<int>(info.dwPageSize);

#else

        pageSize = static_cast<int>(sysconf(_SC_PAGESIZE));

#endif
    }

    return pageSize.loadRelaxed();
}

void *systemAlloc(bsl::size_t size)
    // Allocate a page-aligned block of memory of the specified 'size' (in
    // bytes), and return the address of the allocated block.  The behavior is
    // undefined unless 'size > 0'.
{
    BSLS_ASSERT(size > 0);

#ifdef BSLS_PLATFORM_OS_WINDOWS

    return VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
                                                                      // RETURN

#else

    void *address =  mmap(0,
                          size,
                          PROT_READ | PROT_WRITE,
                          MAP_ANON | MAP_PRIVATE,
                          -1,
                          0);

    if (MAP_FAILED == address) {
        return 0;                                                     // RETURN
    }

    return address;                                                   // RETURN

#endif
}

void systemFree(void *address, size_t size)
    // Return the memory block at the specified 'address' back to its
    // allocator.  The behavior is undefined unless 'address' was returned by
    // 'systemAlloc' and has not already been freed.
{
    BSLS_ASSERT(address);

#ifdef BSLS_PLATFORM_OS_WINDOWS

    VirtualFree(address, 0, MEM_RELEASE);
    (void) size;

#else

    // On some of our platforms, 'munmap' takes a 'char*' argument, while on
    // others it takes a 'void*'.  Casting to 'char*', which will work in both
    // cases.

    munmap(static_cast<char*>(address), size);

#endif
}

int systemProtect(void *address, int pageSize)
    // Protect from read/write access the page of memory at the specified
    // 'address' having the specified 'pageSize' (in bytes).  The behavior is
    // undefined unless 'pageSize == getSystemPageSize()'.
{
    BSLS_ASSERT(address);
    BSLS_ASSERT_SAFE(pageSize == getSystemPageSize());

#ifdef BSLS_PLATFORM_OS_WINDOWS

    DWORD oldProtect;

    return !VirtualProtect(address, pageSize, PAGE_NOACCESS, &oldProtect);
                                                                      // RETURN

#else

    return mprotect(static_cast<char *>(address), pageSize, PROT_NONE);
                                                                      // RETURN

#endif
}

int systemUnprotect(void *address, int pageSize)
    // Unprotect from read/write access the page of memory at the specified
    // 'address' having the specified 'pageSize' (in bytes).  The behavior is
    // undefined unless 'pageSize == getSystemPageSize()'.
{
    BSLS_ASSERT(address);
    BSLS_ASSERT_SAFE(pageSize == getSystemPageSize());

#ifdef BSLS_PLATFORM_OS_WINDOWS

    DWORD  oldProtect;

    return !VirtualProtect(address, pageSize, PAGE_READWRITE, &oldProtect);
                                                                      // RETURN

#else

    return mprotect(static_cast<char *>(address),
                    pageSize,
                    PROT_READ | PROT_WRITE);                          // RETURN

#endif
}

}  // close unnamed namespace

namespace bdlma {

                         // -----------------------
                         // class GuardingAllocator
                         // -----------------------

// CREATORS
GuardingAllocator::~GuardingAllocator()
{
}

// MANIPULATORS
void *GuardingAllocator::allocate(size_type size)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(0 == size)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return 0;                                                     // RETURN
    }

    const size_type paddedSize =
                          bsls::AlignmentUtil::roundUpToMaximalAlignment(size);

    // Adjust for additional memory needed to stash reference addresses when
    // 'e_AFTER_USER_BLOCK' is in use.

    const int adjustedSize = e_AFTER_USER_BLOCK == d_guardPageLocation
                           ? static_cast<int>(paddedSize + OFFSET * 2)
                           : static_cast<int>(paddedSize);

    // Calculate the number of pages to allocate, *not* counting the guard
    // page.

    const int pageSize = getSystemPageSize();
    const int numPages = (adjustedSize + pageSize - 1) / pageSize;

    const size_type totalSize = (numPages + 1) * pageSize;  // add 1 for guard

    void *firstPage = systemAlloc(totalSize);

    if (!firstPage) {
#ifdef BDE_BUILD_TARGET_EXC
        BSLS_THROW(bsl::bad_alloc());
#else
        return 0;                                                     // RETURN
#endif
    }

    void *userAddress;  // address to return to the caller
    void *guardPage;    // address of the guard page for this allocation

    if (e_BEFORE_USER_BLOCK == d_guardPageLocation) {
        // Protect the memory page before the block returned to the user.

        guardPage   = firstPage;
        userAddress = static_cast<char *>(firstPage) + pageSize;
    }
    else {
        // Protect the memory page after the block returned to the user.

        guardPage   = static_cast<char *>(firstPage) + (numPages * pageSize);
        userAddress = static_cast<char *>(guardPage) - paddedSize;

        // Stash the reference addresses required by 'deallocate'.

        AfterUserBlockDeallocationData *deallocData =
            getDataBlockAddress(userAddress);

        deallocData->d_firstPage = firstPage;
        deallocData->d_guardPage = guardPage;
    }

    // Save 'totalSize' - we'll need it for 'systemFree' in 'deallocate'.

    *(int *)(guardPage) = static_cast<int>(totalSize);

    // Protect the guard page from read/write access.

    if (0 != systemProtect(guardPage, pageSize)) {
        systemFree(firstPage, totalSize);
#ifdef BDE_BUILD_TARGET_EXC
        BSLS_THROW(bsl::bad_alloc());
#else
        return 0;                                                     // RETURN
#endif
    }

    return userAddress;
}

void GuardingAllocator::deallocate(void *address)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(0 == address)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return;                                                       // RETURN
    }

    const int pageSize = getSystemPageSize();

    void *firstPage;  // address of the first page of the allocation
    void *guardPage;  // address of the guard page

    if (e_BEFORE_USER_BLOCK == d_guardPageLocation) {
        // The memory page before the block returned to the user is protected.

        firstPage = static_cast<char *>(address) - pageSize;
        guardPage = firstPage;
    }
    else {
        // The memory page after the block returned to the user is protected.

        AfterUserBlockDeallocationData *deallocData =
            getDataBlockAddress(address);

        firstPage = deallocData->d_firstPage;
        guardPage = deallocData->d_guardPage;
    }

    // Unprotect the guard page and free the memory.

    const int rc = systemUnprotect(guardPage, pageSize);
    (void)rc;

    BSLS_ASSERT_OPT(0 == rc);

    size_t totalSize = *(int *)(guardPage);

    systemFree(firstPage, totalSize);
}

}  // close package namespace
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
