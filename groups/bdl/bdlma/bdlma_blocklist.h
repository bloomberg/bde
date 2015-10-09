// bdlma_blocklist.h                                                  -*-C++-*-
#ifndef INCLUDED_BDLMA_BLOCKLIST
#define INCLUDED_BDLMA_BLOCKLIST

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide allocation and management of a sequence of memory blocks.
//
//@CLASSES:
//  bdlma::BlockList: memory manager that allocates and manages memory blocks
//
//@SEE_ALSO: bdlma_infrequentdeleteblocklist
//
//@DESCRIPTION: This component implements a low-level memory manager,
// 'bdlma::BlockList', that allocates and manages a sequence of memory blocks,
// each of a potentially different size as specified during the 'allocate'
// method's invocation.  The 'release' method of a 'bdlma::BlockList' object
// deallocates the entire sequence of outstanding memory blocks, as does its
// destructor.  Note that a 'bdlma::BlockList', at a minor memory expense,
// allows for individual items to be deallocated.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using a 'bdlma::BlockList' in a Memory Pool
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// A 'bdlma::BlockList' object is commonly used to supply memory to more
// elaborate memory managers that distribute parts of each (larger) allocated
// memory block supplied by the 'bdlma::BlockList' object.  The 'my_StrPool'
// memory pool manager shown below requests relatively large blocks of memory
// from its 'bdlma::BlockList' member object and distributes, via its
// 'allocate' method, memory chunks of varying sizes from each block.
//
// First, we define the interface of our 'my_StrPool' class:
//..
//  // my_strpool.h
//
//  class my_StrPool {
//
//      // DATA
//      int               d_blockSize;  // size of current memory block
//
//      char             *d_block_p;    // current free memory block
//
//      int               d_cursor;     // offset to next available byte in
//                                      // block
//
//      bdlma::BlockList  d_blockList;  // supplies managed memory blocks
//
//    private:
//      // PRIVATE MANIPULATORS
//      void *allocateBlock(int numBytes);
//          // Request a new memory block of at least the specified 'numBytes'
//          // size and allocate the initial 'numBytes' from this block.
//          // Return the address of the allocated memory.
//
//    private:
//      // NOT IMPLEMENTED
//      my_StrPool(const my_StrPool&);
//      my_StrPool& operator=(const my_StrPool&);
//
//    public:
//      // CREATORS
//      my_StrPool(bslma::Allocator *basicAllocator = 0);
//          // Create a memory manager.  Optionally specify a 'basicAllocator'
//          // used to supply memory.  If 'basicAllocator' is 0, the currently
//          // installed default allocator is used.
//
//      ~my_StrPool();
//          // Destroy this object and release all associated memory.
//
//      // MANIPULATORS
//      void *allocate(int numBytes);
//          // Allocate the specified 'numBytes' of memory and return its
//          // address.  If 'numBytes' is 0, return 0 with no other effect.
//          // The behavior is undefined unless '0 <= numBytes'.
//
//      void release();
//          // Release all memory currently allocated through this object.
//  };
//
//  // MANIPULATORS
//  inline
//  void my_StrPool::release()
//  {
//      d_blockList.release();
//      d_block_p = 0;
//  }
//..
// Finally, we provide the implementation of our 'my_StrPool' class:
//..
//  // my_strpool.cpp
//
//  enum {
//      k_INITIAL_SIZE  = 128,  // initial block size
//
//      k_GROWTH_FACTOR =   2,  // multiplicative factor by which to grow block
//
//      k_THRESHOLD     = 128   // size beyond which an individual block may be
//                              // allocated if it doesn't fit in current block
//  };
//
//  // PRIVATE MANIPULATORS
//  void *my_StrPool::allocateBlock(int numBytes)
//  {
//      assert(0 < numBytes);
//
//      if (k_THRESHOLD < numBytes) {
//          // Alloc separate block if above threshold.
//
//          return (char *)d_blockList.allocate(numBytes);            // RETURN
//      }
//      else {
//          if (d_block_p) {
//              // Do not increase block size if no current block.
//
//              d_blockSize *= k_GROWTH_FACTOR;
//          }
//          d_block_p = (char *)d_blockList.allocate(d_blockSize);
//          d_cursor = numBytes;
//          return d_block_p;                                         // RETURN
//      }
//  }
//
//  // CREATORS
//  my_StrPool::my_StrPool(bslma::Allocator *basicAllocator)
//  : d_blockSize(k_INITIAL_SIZE)
//  , d_block_p(0)
//  , d_blockList(basicAllocator)  // the blocklist knows about 'bslma_default'
//  {
//  }
//
//  my_StrPool::~my_StrPool()
//  {
//      assert(k_INITIAL_SIZE <= d_blockSize);
//      assert(d_block_p || (0 <= d_cursor && d_cursor <= d_blockSize));
//  }
//
//  // MANIPULATORS
//  void *my_StrPool::allocate(int numBytes)
//  {
//      assert(0 <= numBytes);
//
//      if (0 == numBytes) {
//          return 0;                                                 // RETURN
//      }
//
//      if (d_block_p && numBytes + d_cursor <= d_blockSize) {
//          char *p = d_block_p + d_cursor;
//          d_cursor += numBytes;
//          return p;                                                 // RETURN
//      }
//      else {
//          return allocateBlock(numBytes);                           // RETURN
//      }
//  }
//..
// In the code shown above, the 'my_StrPool' memory manager allocates from its
// 'bdlma::BlockList' member object an initial memory block of size
// 'k_INITIAL_SIZE'.  This size is multiplied by 'k_GROWTH_FACTOR' each time a
// depleted memory block is replaced by a newly-allocated block.  The
// 'allocate' method distributes memory from the current memory block
// piecemeal, except when the requested size either (1) is not available in the
// current block, or (2) exceeds the 'k_THRESHOLD_SIZE', in which case a
// separate memory block is allocated and returned.  When the 'my_StrPool'
// memory manager is destroyed, its 'bdlma::BlockList' member object is also
// destroyed, which, in turn, automatically deallocates all of its managed
// memory blocks.

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLS_ALIGNMENTUTIL
#include <bsls_alignmentutil.h>
#endif

namespace BloombergLP {
namespace bdlma {

                             // ===============
                             // class BlockList
                             // ===============

class BlockList {
    // This class implements a low-level memory manager that allocates and
    // manages a sequence of memory blocks -- each potentially of a different
    // size as specified during the invocation of the 'allocate' method.
    // Allocated blocks may be efficiently deallocated individually, i.e.,
    // potentially in constant time depending on the supplied allocator.  The
    // 'release' method deallocates the entire sequence of memory blocks, as
    // does the destructor.

    // TYPES
    struct Block {
        // This 'struct' overlays the beginning of each managed block of
        // allocated memory, implementing a doubly-linked list of managed
        // blocks, and thereby enabling constant-time deletions from, as well
        // as additions to, the list of blocks.

        Block                                *d_next_p;       // next pointer

        Block                               **d_addrPrevNext; // enable delete

        bsls::AlignmentUtil::MaxAlignedType   d_memory;       // force
                                                              // alignment
    };

    // DATA
    Block            *d_head_p;       // address of first block of memory (or
                                      // 0)

    bslma::Allocator *d_allocator_p;  // memory allocator (held, not owned)

  private:
    // NOT IMPLEMENTED
    BlockList(const BlockList&);
    BlockList& operator=(const BlockList&);

  public:
    // CREATORS
    explicit
    BlockList(bslma::Allocator *basicAllocator = 0);
        // Create an empty block list suitable for managing memory blocks of
        // varying sizes.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    ~BlockList();
        // Destroy this object and deallocate all outstanding memory blocks
        // managed by this object.

    // MANIPULATORS
    void *allocate(int size);
        // Return the address of a contiguous block of memory of the specified
        // 'size' (in bytes).  If 'size' is 0, no memory is allocated and 0 is
        // returned.  The returned memory is guaranteed to be maximally
        // aligned.  The behavior is undefined unless '0 <= size'.

    void deallocate(void *address);
        // Return the memory at the specified 'address' back to the associated
        // allocator.  If 'address' is 0, this function has no effect.  The
        // behavior is undefined unless 'address' was allocated by this object,
        // and has not already been deallocated.

    void release();
        // Deallocate all memory blocks currently managed by this object,
        // returning it to its default-constructed state.
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                             // ---------------
                             // class BlockList
                             // ---------------

// CREATORS
inline
BlockList::BlockList(bslma::Allocator *basicAllocator)
: d_head_p(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

}  // close package namespace
}  // close enterprise namespace

#endif

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
