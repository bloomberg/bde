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

#include <bslmt_lockguard.h>
#include <bsls_bslexceptionutil.h>
#include <bsls_platform.h>

#include <bsl_new.h>

#if defined(BSLS_PLATFORM_OS_WINDOWS)

#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#elif defined(BSLS_PLATFORM_OS_UNIX)

#include <unistd.h>
#include <sys/mman.h>

#else

#error Unsupported platform

#endif

namespace BloombergLP {
namespace bdlma {

namespace {

enum {
    k_MAX_CHUNK_SIZE = 1u << 30,     // 1 GiB
    k_MIN_CHUNK_SIZE = 1u << 12,     // 4 KiB
    k_DEFAULT_CHUNK_SIZE = 1u << 22  // 4 MiB
};

}  // close unnamed namespace

                         // -------------------------
                         // class HeapBypassAllocator
                         // -------------------------

// PRIVATE CLASS METHODS
#if defined(BSLS_PLATFORM_OS_UNIX)

void *HeapBypassAllocator::systemAllocate(size_type size)
{
    void *raw = mmap(0,
                     static_cast<size_t>(size),
                     PROT_READ | PROT_WRITE,
#if defined(BSLS_PLATFORM_OS_DARWIN)
                     MAP_ANON | MAP_PRIVATE,
#else
                     MAP_ANONYMOUS | MAP_PRIVATE,
#endif
                     -1,
                     0);
    return raw == MAP_FAILED ? 0 : raw;
}

void HeapBypassAllocator::systemFree(void *chunk, size_type size)
{
    // On some platforms, munmap takes a 'char *', on others, a 'void *'
    munmap(reinterpret_cast<char *>(chunk), static_cast<size_t>(size));
}

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

void *HeapBypassAllocator::systemAllocate(size_type size)
{
    void *raw = VirtualAlloc(0,
                             static_cast<SIZE_T>(size),
                             MEM_COMMIT | MEM_RESERVE,
                             PAGE_READWRITE);
    return raw == NULL ? 0 : raw;
}

void HeapBypassAllocator::systemFree(void *chunk, size_type size)
{
    VirtualFree(chunk, 0, MEM_RELEASE);
}

#endif

// PRIVATE MANIPULATORS
void HeapBypassAllocator::init()
{
    // Configure initial empty chunk placeholder
    d_initialChunk.d_size = 0;

    // Determine page size
    size_type pageSize = 0;
#if defined(BSLS_PLATFORM_OS_WINDOWS)
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    pageSize = static_cast<size_type>(si.dwAllocationGranularity);
#elif defined(BSLS_PLATFORM_OS_UNIX)
    long ps = sysconf(_SC_PAGESIZE);
    pageSize = ps == -1 ? 0u : static_cast<size_type>(ps);
#endif
    if (pageSize) {
        // Round up chunk size to multiple of page size
        d_chunkSize += (pageSize - 1u);
        d_chunkSize = d_chunkSize / pageSize * pageSize;
    }
}

HeapBypassAllocator::Chunk *HeapBypassAllocator::replenish(size_type size)
{
    // Handle case where requested size (plus the chunk header itself) is
    // larger than the usual chunk size
    const size_type chunkHeaderSize =
                 bsls::AlignmentUtil::roundUpToMaximalAlignment(sizeof(Chunk));
    size_type       chunkSize = d_chunkSize;
    while (chunkSize < size + chunkHeaderSize) {
        chunkSize *= 2;
    }

    bslmt::LockGuard<bslmt::Mutex> guard(&d_replenishMutex);

    Chunk *chunk = d_current.loadRelaxed();
    if (chunk->d_offset.loadAcquire() + size <= chunk->d_size) {
        // Double-checked locking pattern; another thread must have replenished
        // already
        return chunk;                                                 // RETURN
    }

    char *raw = reinterpret_cast<char *>(systemAllocate(chunkSize));
    if (!raw) {  // failed to allocate
        bsls::BslExceptionUtil::throwBadAlloc();
        return 0;                                                     // RETURN
    }
    Chunk *newChunk = new (raw) Chunk;
    newChunk->d_offset.storeRelaxed(chunkHeaderSize);
    newChunk->d_size = chunkSize;
    newChunk->d_nextChunk_p = chunk;
    d_current.storeRelease(newChunk);
    return newChunk;
}

// CREATORS
HeapBypassAllocator::HeapBypassAllocator()
: d_current(&d_initialChunk)
, d_chunkSize(k_DEFAULT_CHUNK_SIZE)
{
    init();
}

HeapBypassAllocator::HeapBypassAllocator(size_type replenishHint)
: d_current(&d_initialChunk)
{
    // Round up to next power of 2
    d_chunkSize = k_MIN_CHUNK_SIZE;
    while (d_chunkSize < replenishHint && d_chunkSize < k_MAX_CHUNK_SIZE) {
        d_chunkSize <<= 1;
    }
    init();
}

HeapBypassAllocator::~HeapBypassAllocator()
{
    Chunk *chunk = d_current.loadRelaxed();
    while (chunk != &d_initialChunk) {
        Chunk *next = chunk->d_nextChunk_p;
        systemFree(chunk, chunk->d_size);
        chunk = next;
    }
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
