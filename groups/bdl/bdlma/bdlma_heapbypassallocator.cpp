// bdlma_heapbypassallocator.cpp                                      -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlma_heapbypassallocator.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlma_heapbypassallocator_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>
#include <bsls_alignmentutil.h>
#include <bsls_platform.h>

#include <bsl_algorithm.h>

#ifdef BSLS_PLATFORM_OS_WINDOWS

#include <windows.h>

#elif defined(BSLS_PLATFORM_OS_UNIX)

#include <unistd.h>
#include <sys/mman.h>

#endif

namespace BloombergLP {
namespace bdlma {

                 // ========================================
                 // struct HeapBypassAllocator::BufferHeader
                 // ========================================

struct HeapBypassAllocator::BufferHeader {
    // This struct defines a link in a linked list of buffers allocated by
    // 'class HeapBypassAllocator'.  Each buffer header is located at the
    // beginning of the buffer it describes, and contains the size (in bytes)
    // of the buffer.

    BufferHeader                *d_nextBuffer;  // pointer to linked list of
                                                // buffers allocated after this
                                                // one

    bslma::Allocator::size_type  d_size;        // size (in bytes) of this
                                                // buffer
};
}  // close package namespace

                        // --------------------------
                        // bdlma::HeapBypassAllocator
                        // --------------------------

// PRIVATE CLASS METHODS
#if defined(BSLS_PLATFORM_OS_UNIX)

namespace bdlma {char *HeapBypassAllocator::map(size_type size)

{
    // Note that passing 'MAP_ANONYMOUS' and a null file descriptor tells
    // 'mmap' to use a special system file to map to.

    char *address = (char *)mmap(0,     // 'mmap' chooses what address to which
                                        // to map the memory
                                 size,
                                 PROT_READ | PROT_WRITE,
#ifdef BSLS_PLATFORM_OS_DARWIN
                                 MAP_ANON | MAP_PRIVATE,
#else
                                 MAP_ANONYMOUS | MAP_PRIVATE,
#endif
                                 -1,    // null file descriptor
                                 0);
    return (MAP_FAILED == address ? 0 : address);
}

void HeapBypassAllocator::unmap(void *address, size_type size) {
    // On some platforms, munmap takes a 'char *', on others, a 'void *'.

    munmap((char *)address, size);
}
}  // close package namespace
#elif defined(BSLS_PLATFORM_OS_WINDOWS)

namespace bdlma {char *HeapBypassAllocator::map(size_type size)

{
    char *address =
           (char *)VirtualAlloc(0,  // 'VirtualAlloc' chooses what address to
                                    // which to map the memory
                                size,
                                MEM_COMMIT | MEM_RESERVE,
                                PAGE_READWRITE);
    return NULL == address ? 0 : address;
}

void HeapBypassAllocator::unmap(void *address, size_type size)
{
    VirtualFree(address, 0, MEM_RELEASE);
}
}  // close package namespace
#else
#error unsupported platform
#endif

namespace bdlma {

// PRIVATE MANIPULATORS
int HeapBypassAllocator::replenish(size_type size)
{
    // round size up to a multiple of page size

    const size_type pageMask = d_pageSize - 1;

    // '%' can be very slow -- if 'd_pageSize' is a power of 2, use '&'

    const size_type mod = !(d_pageSize & pageMask) ? (size & pageMask)
                                                   :  size % d_pageSize;
    if (0 != mod) {
        size += d_pageSize - mod;
    }

    BufferHeader *newBuffer = (BufferHeader *)(void *)map(size);
    if (0 == newBuffer) {
        return -1;                                                    // RETURN
    }

    if (d_currentBuffer_p) {
        d_currentBuffer_p->d_nextBuffer = newBuffer;
    }
    else {
        d_firstBuffer_p = newBuffer;
    }
    d_currentBuffer_p = newBuffer;

    d_endOfBuffer_p = (char *)newBuffer + size;
    newBuffer->d_nextBuffer = 0;
    newBuffer->d_size = size;
    d_cursor_p = (char *)(newBuffer + 1);

    return 0;
}

// CREATORS
HeapBypassAllocator::HeapBypassAllocator()
: d_firstBuffer_p(0)
, d_currentBuffer_p(0)
, d_cursor_p(0)
, d_endOfBuffer_p(0)
, d_alignment(bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT)
{
#if defined(BSLS_PLATFORM_OS_UNIX)
    d_pageSize = ::sysconf(_SC_PAGESIZE);
#else // Windows
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    d_pageSize = si.dwAllocationGranularity;
#endif
#ifdef BSLS_PLATFORM_OS_HPUX
    // 128-bit alignment is required on HP-UX for bdesu_stacktrace,
    // 'BSLS_MAX_ALIGNMENT' at the time of this writing is only 64 bits on
    // HP-UX, which causes bus traps on alignment errors.

    struct {
        char      d_dummyChar;
        __float80 d_dummyFloat80;
    } s;

    d_alignment = (char *)&s.d_dummyFloat80 - &s.d_dummyChar;
#endif

    BSLS_ASSERT_SAFE(d_alignment >= bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT);
    BSLS_ASSERT_SAFE(0 == (d_alignment & (d_alignment - 1)));  // is power of 2
}

HeapBypassAllocator::~HeapBypassAllocator()
{
    BufferHeader *buffer = d_firstBuffer_p;
    while (buffer) {
        BufferHeader *nextBuffer = buffer->d_nextBuffer;
        unmap(buffer, buffer->d_size);
        buffer = nextBuffer;
    }
}

// MANIPULATORS
void *HeapBypassAllocator::allocate(size_type size)
{
    d_cursor_p = d_cursor_p + bsls::AlignmentUtil::calculateAlignmentOffset(
                                    d_cursor_p, static_cast<int>(d_alignment));
    if (d_endOfBuffer_p < d_cursor_p + size) {
        size_type blockSize = size + d_alignment + sizeof(BufferHeader);
        int sts = replenish(blockSize);    // 'replenish' will round up to
                                           // multiple of 'd_pageSize'
        if (0 != sts) {
            return 0;                                                 // RETURN
        }

        d_cursor_p = d_cursor_p
                   + bsls::AlignmentUtil::calculateAlignmentOffset(
                                    d_cursor_p, static_cast<int>(d_alignment));
    }

    BSLS_ASSERT(d_endOfBuffer_p >= d_cursor_p + size);

    char *address = d_cursor_p;
    d_cursor_p += size;
    return address;
}

void HeapBypassAllocator::deallocate(void *)
{
    // no-op
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
