// bdema_heapbypassallocator.cpp                                      -*-C++-*-
#include <bdema_heapbypassallocator.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdema_heapbypassallocator_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>
#include <bsls_alignmentutil.h>
#include <bsls_platform.h>

#include <bsl_algorithm.h>

#ifdef BSLS_PLATFORM__OS_WINDOWS

#include <windows.h>

#elif defined(BSLS_PLATFORM__OS_UNIX)

#include <unistd.h>
#include <sys/mman.h>

#endif

namespace BloombergLP {

                // ==============================================
                // struct bdema_HeapBypassAllocator::BufferHeader
                // ==============================================

struct bdema_HeapBypassAllocator::BufferHeader {
    // This struct defines a link in a linked list of buffers allocated by
    // 'class bdema_HeapBypassAllocator'.  Each buffer header is located at the
    // beginning of the buffer it describes, and contains the size (in bytes)
    // of the buffer.

    BufferHeader               *d_nextBuffer;   // pointer to linked list of
                                                // buffers allocated after this
                                                // one

    bdema_Allocator::size_type  d_size;         // size (in bytes) of this
                                                // buffer
};

                          // -------------------------
                          // bdema_HeapBypassAllocator
                          // -------------------------

// PRIVATE CLASS METHODS
#if defined(BSLS_PLATFORM__OS_UNIX)
char *bdema_HeapBypassAllocator::map(size_type size)
{
    // Note that passing 'MAP_ANONYMOUS' and a null file descriptor tells
    // 'mmap' to use a special system file to map to.

    char *address = (char *)mmap(0,     // 'mmap' chooses what address to which
                                        // to map the memory
                                 size,
                                 PROT_READ | PROT_WRITE,
                                 MAP_ANONYMOUS | MAP_PRIVATE,
                                 -1,    // null file descriptor
                                 0);
    return (MAP_FAILED == address ? 0 : address);
}

void bdema_HeapBypassAllocator::unmap(void *address, size_type size) {
    // On some platforms, munmap takes a 'char *', on others, a 'void *'.

    munmap((char *)address, size);
}
#elif defined(BSLS_PLATFORM__OS_WINDOWS)
char *bdema_HeapBypassAllocator::map(size_type size)
{
    char *address = 
           (char *)VirtualAlloc(0,  // 'VirtualAlloc' chooses what address to
                                    // which to map the memory
                                size,
                                MEM_COMMIT | MEM_RESERVE,
                                PAGE_READWRITE);
    return NULL == address ? 0 : address;
}

void bdema_HeapBypassAllocator::unmap(void *address, size_type size)
{
    VirtualFree(address, 0, MEM_RELEASE);
}
#else
#error unsupported platform
#endif

// PRIVATE MANIPULATORS
int bdema_HeapBypassAllocator::replenish(size_type size)
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
bdema_HeapBypassAllocator::bdema_HeapBypassAllocator()
: d_firstBuffer_p(0)
, d_currentBuffer_p(0)
, d_cursor_p(0)
, d_endOfBuffer_p(0)
, d_alignment(bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT)
{
#if defined(BSLS_PLATFORM__OS_UNIX)
    d_pageSize = ::sysconf(_SC_PAGESIZE);
#else // Windows
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    d_pageSize = si.dwAllocationGranularity;
#endif
#ifdef BSLS_PLATFORM__OS_HPUX
    // 128-bit alignment is required on HP-UX for bdesu_stacktrace,
    // 'BSLS_MAX_ALIGNMENT' at the time of this writing is only 64 bits on
    // HP-UX, which causes bus traps on alignment errors.

    struct {
        char      d_dummyChar;
        __float80 d_dummyFloat80;
    } s;

    d_alignment = (char *)&s.d_dummyFloat80 - &s.d_dummyChar;
#endif

    BSLS_ASSERT_SAFE(d_alignment >= bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT);
    BSLS_ASSERT_SAFE(0 == (d_alignment & (d_alignment - 1)));  // is power of 2
}

bdema_HeapBypassAllocator::~bdema_HeapBypassAllocator()
{
    BufferHeader *buffer = d_firstBuffer_p;
    while (buffer) {
        BufferHeader *nextBuffer = buffer->d_nextBuffer;
        unmap(buffer, buffer->d_size);
        buffer = nextBuffer;
    }
}

// MANIPULATORS
void *bdema_HeapBypassAllocator::allocate(size_type size)
{
    if (size < 0) {
        return 0;                                                     // RETURN
    }

    d_cursor_p = d_cursor_p + bsls_AlignmentUtil::calculateAlignmentOffset(
                                                      d_cursor_p, d_alignment);
    if (d_endOfBuffer_p < d_cursor_p + size) {
        size_type blockSize = size + d_alignment + sizeof(BufferHeader);
        if (blockSize < 0) {
            return 0;                                                 // RETURN
        }

        int sts = replenish(blockSize);    // 'replenish' will round up to
                                           // multiple of 'd_pageSize'
        if (0 != sts) {
            return 0;                                                 // RETURN
        }

        d_cursor_p = d_cursor_p + bsls_AlignmentUtil::calculateAlignmentOffset(
                                                      d_cursor_p, d_alignment);
    }

    BSLS_ASSERT(d_endOfBuffer_p >= d_cursor_p + size);

    char *address = d_cursor_p;
    d_cursor_p += size;
    return address;
}

void bdema_HeapBypassAllocator::deallocate(void *)
{
    // no-op
}

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------------------------------------------------------
