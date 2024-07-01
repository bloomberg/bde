// bdlma_heapbypassallocator.h                                        -*-C++-*-
#ifndef INCLUDED_BDLMA_HEAPBYPASSALLOCATOR
#define INCLUDED_BDLMA_HEAPBYPASSALLOCATOR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Support memory allocation directly from virtual memory.
//
//@CLASSES:
//   bdlma::HeapBypassAllocator: memory allocator directly from virtual memory
//
//@SEE_ALSO: bsls_alignmentutil
//
//@DESCRIPTION: The 'bdlma::HeapBypassAllocator' class provided by this
// component implements a concrete allocator derived from the
// 'bslma::Allocator' interface that allocates memory directly from virtual
// memory, bypassing the heap.  Reserves virtual memory in chunks so as to grow
// infrequently.  The chunk size can optionally be chosen by providing an
// argument for 'replenishHint' in the constructor (see documentation for the
// constructor for advice on how to choose a good value for 'replenishHint').
// All memory allocated by this allocator is freed when the allocator's
// destructor is called, but not before.  A natural use case is for backing
// large, long-lived object pools, or cases where the heap may be corrupt.
//..
//                   ,---------------------------.
//                  (  bdlma::HeapBypassAllocator )
//                   `---------------------------'
//                                 |         ctor
//                                 V
//                        ,-----------------.
//                       (  bslma::Allocator )
//                        `-----------------'
//                                           dtor
//                                           allocate
//                                           deallocate      // no-op
//..
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Usage
/// - - - - - - - - - - -
// Here we allocate some memory using a heap bypass allocator, then write to
// that memory, then read from it and verify the values written are preserved.
//..
//  {
//      enum {
//          k_LENGTH       = 10 * 1000,
//          k_NUM_SEGMENTS = 60
//      };
//
//      bdlma::HeapBypassAllocator hbpa;
//..
// First, we allocate some segments:
//..
//      char *segments[k_NUM_SEGMENTS];
//      for (int i = 0; i < k_NUM_SEGMENTS; ++i) {
//          segments[i] = static_cast<char *>(hbpa.allocate(k_LENGTH));
//          BSLS_ASSERT(segments[i]);
//      }
//..
// Next, we write to the segments:
//..
//      char c = 'a';
//      for (int i = 0; i < k_NUM_SEGMENTS; ++i) {
//          char *segment = segments[i];
//          for (int j = 0; j < k_LENGTH; ++j) {
//              c = (c + 1) & 0x7f;
//              segment[j] = c;
//          }
//      }
//..
// Finally, we read from the segments and verify the written data is still
// there:
//..
//      c = 'a';
//      for (int i = 0; i < k_NUM_SEGMENTS; ++i) {
//          char *segment = segments[i];
//          for (int j = 0; j < k_LENGTH; ++j) {
//              c = (c + 1) & 0x7f;
//              BSLS_ASSERT(segment[j] == c);  (void)segment;
//          }
//      }
//..
// Memory is released upon destruction of object 'hbpa' when it goes out of
// scope.
//..
//  }
//..

#include <bdlscm_version.h>

#include <bslma_allocator.h>
#include <bslmt_mutex.h>
#include <bsls_alignmentutil.h>
#include <bsls_atomic.h>
#include <bsls_keyword.h>
#include <bsls_performancehint.h>
#include <bsls_types.h>

namespace BloombergLP {
namespace bdlma {

                        // =========================
                        // class HeapBypassAllocator
                        // =========================

class HeapBypassAllocator : public bslma::Allocator {
    // This class allows the caller to allocate memory directly from virtual
    // memory, without going through the heap like 'malloc' or 'new' would.
    // Note that the only way to free any memory allocated with this object is
    // to destroy the object, at which point all memory it has allocated is
    // freed.  Thread-safe.

    // PRIVATE TYPES
    struct Chunk {
        // Represents a chunk of memory from which allocations are subsequently
        // carved out.  Chunks are chained in order to allow iterating over
        // them so they can be freed when the allocator is destroyed.

        // PUBLIC DATA
        bsls::AtomicUint64  d_offset;  // byte offset from 'this' to free bytes
        size_type           d_size;    // total size, in bytes, of this chunk
        Chunk              *d_nextChunk_p;
    };

    // DATA
    bsls::AtomicPointer<Chunk> d_current;         // current chunk (list head)
    size_type                  d_chunkSize;       // minimum size of new chunks
    bslmt::Mutex               d_replenishMutex;  // acquired while allocating
    Chunk                      d_initialChunk;    // dummy chunk (at list tail)

  private:
    // NOT IMPLEMENTED
    HeapBypassAllocator(const HeapBypassAllocator&) BSLS_KEYWORD_DELETED;
    HeapBypassAllocator& operator=(
                              const HeapBypassAllocator&) BSLS_KEYWORD_DELETED;

  private:
    // PRIVATE CLASS METHODS
    static void *systemAllocate(size_type size);
        // Acquire a chunk of virtual memory of the specified 'size' bytes from
        // the operating system.  'size' must be a multiple of 'd_chunkSize'
        // and must be greater than 0.  Return the address of the allocated
        // chunk, or null if allocation fails.  Each successful call to
        // 'systemAllocate' must eventually be paired with a matching call to
        // 'systemFree'.

    static void systemFree(void *chunk, size_type size);
        // Free the specified 'chunk' of memory of the specified 'size' bytes
        // back to the operating system.  'chunk' must have been returned by a
        // previous call to 'systemAllocate' of the same size, and not
        // previously freed.

    // PRIVATE MANIPULATORS
    void init();
        // Initialize the linked list of chunks and round up 'd_chunkSize' to
        // a multiple of the system's page size.

    Chunk *replenish(size_type size);
        // Allocate a new chunk of at least the specified 'size' bytes and make
        // it available for subsequent calls of 'allocate' to carve out memory
        // from.  Return a pointer to the chunk on success, null otherwise.

  public:
    // CREATORS
    HeapBypassAllocator();
    explicit HeapBypassAllocator(size_type replenishHint);
        // Create an empty allocator object.  Optionally specify
        // 'replenishHint' as the minimum number of bytes to allocate at a time
        // when the allocator's memory needs to grow.  In a multi-threaded
        // scenario, the programmer should ensure that 'replenishHint' is large
        // enough so that multiple threads will rarely need to replenish at the
        // same time in order to minimize contention.

    ~HeapBypassAllocator() BSLS_KEYWORD_OVERRIDE;
        // Destroy this object, releasing all managed buffers of memory that it
        // has allocated.

    // MANIPULATORS
    void *allocate(bsls::Types::size_type size) BSLS_KEYWORD_OVERRIDE;
        // Allocate a buffer of memory having the specified 'size' (in bytes),
        // and alignment defined by 'bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT',
        // from virtual memory.  If 'size' is 0, return a null pointer with no
        // other effect.  If this allocator cannot return the requested number
        // of bytes, then it will throw a 'std::bad_alloc' exception in an
        // exception-enabled build, or else will abort the program in a non-
        // exception build.  The behavior is undefined unless '0 <= size'.

    void deallocate(void *) BSLS_KEYWORD_OVERRIDE;
        // This method has no effect for this heap bypass allocator.
};

//=============================================================================
//                            INLINE DEFINITIONS
//=============================================================================

                         // -------------------------
                         // class HeapBypassAllocator
                         // -------------------------
// MANIPULATORS
inline
void *HeapBypassAllocator::allocate(size_type size)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(size == 0)) {
        return 0;                                                     // RETURN
    }
    size = bsls::AlignmentUtil::roundUpToMaximalAlignment(size);
    Chunk *chunk = d_current.loadAcquire();
    do {
        bsls::Types::Uint64 offset = chunk->d_offset.addRelaxed(size);
        if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(offset <= chunk->d_size)) {
            return reinterpret_cast<char *>(chunk) + (offset - size); // RETURN
        }
        chunk = replenish(size);
    } while (chunk);
    return 0;
}

inline void HeapBypassAllocator::deallocate(void *)
{
}

}  // close package namespace
}  // close enterprise namespace

#endif

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
