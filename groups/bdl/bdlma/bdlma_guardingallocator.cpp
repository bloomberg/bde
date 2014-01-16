// bdlma_guardingallocator.cpp                                        -*-C++-*-
#include <bdlma_guardingallocator.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlma_guardingallocator_cpp,"$Id$ $CSID$")

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

#include <stdlib.h>    // 'valloc'
#include <sys/mman.h>  // 'mprotect'
#include <unistd.h>    // 'sysconf'

#endif

namespace BloombergLP {

namespace {

// HELPER FUNCTIONS

// Define the offset (in bytes) from the address returned to the user in which
// to stash reference addresses ('e_AFTER_USER_BLOCK' only).

const bslma::Allocator::size_type OFFSET =
                                       bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT;

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

#else

    return valloc(size);

#endif
}

void systemFree(void *address)
    // Return the memory block at the specified 'address' back to its
    // allocator.  The behavior is undefined unless 'address' was returned by
    // 'systemAlloc' and has not already been freed.
{
    BSLS_ASSERT(address);

#ifdef BSLS_PLATFORM_OS_WINDOWS

    VirtualFree(address, 0, MEM_RELEASE);

#else

    free(address);

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

#else

    return mprotect(static_cast<char *>(address), pageSize, PROT_NONE);

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

#else

    return mprotect(static_cast<char *>(address),
                    pageSize,
                    PROT_READ | PROT_WRITE);

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
                             ? paddedSize + OFFSET * 2
                             : paddedSize;

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

        *(void **)(static_cast<char *>(userAddress) - OFFSET)     = firstPage;
        *(void **)(static_cast<char *>(userAddress) - OFFSET * 2) = guardPage;
    }

    // Protect the guard page from read/write access.

    if (0 != systemProtect(guardPage, pageSize)) {
        systemFree(firstPage);
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

        firstPage = *(void **)(static_cast<char *>(address) - OFFSET);
        guardPage = *(void **)(static_cast<char *>(address) - OFFSET * 2);
    }

    // Unprotect the guard page and free the memory.

    const int rc = systemUnprotect(guardPage, pageSize);
    (void)rc;

    BSLS_ASSERT_OPT(0 == rc);

    systemFree(firstPage);
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright (C) 2013 Bloomberg Finance L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
