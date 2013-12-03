// bdlma_infrequentdeleteblocklist.h                                  -*-C++-*-
#ifndef INCLUDED_BDLMA_INFREQUENTDELETEBLOCKLIST
#define INCLUDED_BDLMA_INFREQUENTDELETEBLOCKLIST

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide allocation and management of infrequently deleted blocks.
//
//@CLASSES:
//  bdlma::InfrequentDeleteBlockList: manager of infrequently deleted blocks
//
//@SEE_ALSO: bdlma_blocklist
//
//@DESCRIPTION: This component implements a low-level memory manager,
// 'bdlma::InfrequentDeleteBlockList', that allocates and manages a sequence of
// memory blocks, each of a potentially different size as specified during the
// 'allocate' method's invocation.  The 'release' method of a
// 'bdlma::InfrequentDeleteBlockList' object deallocates the entire sequence of
// outstanding memory blocks, as does its destructor.  Note that, in contrast
// to 'bdlma::BlockList', the 'bdlma::InfrequentDeleteBlockList' class does
// *not* support the deallocation of individual items.  In particular, although
// 'bdlma::InfrequentDeleteBlockList' has a 'deallocate' method, that method
// has no effect.
//
///Usage
///-----
// A 'bdlma::InfrequentDeleteBlockList' object is commonly used to supply
// memory to more elaborate memory managers that distribute parts of each
// (larger) allocated memory block supplied by the
// 'bdlma::InfrequentDeleteBlockList' object.  The 'my_StrPool' memory pool
// manager shown below requests relatively large blocks of memory from its
// 'bdlma::InfrequentDeleteBlockList' member object and distributes memory
// chunks of varying sizes from each block on demand:
//..
//  // my_strpool.h
//
//  class my_StrPool {
//
//      // DATA
//      char           *d_block_p;    // current memory block
//
//      int             d_blockSize;  // size of current memory block
//
//      int             d_cursor;     // offset to next available byte in block
//
//      bdlma::InfrequentDeleteBlockList
//                      d_blockList;  // supplies managed memory blocks
//
//    private:
//      // PRIVATE MANIPULATORS
//      void *allocateBlock(int numBytes);
//          // Request a memory block of at least the specified 'numBytes' size
//          // and allocate the initial 'numBytes' from this block.  Return the
//          // address of the allocated memory.
//
//    private:
//      // NOT IMPLEMENTED
//      my_StrPool(const my_StrPool&);
//      my_StrPool& operator=(const my_StrPool&);
//
//    public:
//      // CREATORS
//      explicit
//      my_StrPool(bslma::Allocator *basicAllocator = 0);
//          // Create a string pool.  Optionally specify a 'basicAllocator'
//          // used to supply memory.  If 'basicAllocator' is 0, the currently
//          // installed default allocator is used.
//
//      ~my_StrPool();
//          // Destroy this object and release all associated memory.
//
//      // MANIPULATORS
//      void *allocate(int size);
//          // Return the address of a contiguous block of memory of the
//          // specified 'size' (in bytes).  If 'size' is 0, no memory is
//          // allocated and 0 is returned.  The behavior is undefined unless
//          // 'size >= 0'.
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
//
//  // ...
//
//  // my_strpool.cpp
//
//  enum {
//      INITIAL_SIZE  = 128,  // initial block size
//
//      GROWTH_FACTOR =   2,  // multiplicative factor by which to grow block
//
//      THRESHOLD     = 128   // size beyond which an individual block may be
//                            // allocated if it doesn't fit in current block
//  };
//
//  // PRIVATE MANIPULATORS
//  void *my_StrPool::allocateBlock(int numBytes)
//  {
//      assert(0 < numBytes);
//
//      if (THRESHOLD < numBytes) { // Alloc separate block if above threshold.
//          return reinterpret_cast<char *>(
//                                  d_blockList.allocate(numBytes));  // RETURN
//      }
//
//      if (d_block_p) {  // Don't increase block size if no current block.
//          d_blockSize *= GROWTH_FACTOR;
//      }
//      d_block_p = reinterpret_cast<char*>(d_blockList.allocate(d_blockSize));
//      d_cursor  = numBytes;
//
//      return d_block_p;
//  }
//
//  // CREATORS
//  my_StrPool::my_StrPool(bslma::Allocator *basicAllocator)
//  : d_block_p(0)
//  , d_blockSize(INITIAL_SIZE)
//  , d_cursor(0)
//  , d_blockList(basicAllocator)  // the blocklist knows about 'bslma_default'
//  {
//  }
//
//  my_StrPool::~my_StrPool()
//  {
//      assert(INITIAL_SIZE <= d_blockSize);
//      assert(d_block_p || (0 <= d_cursor && d_cursor <= d_blockSize));
//  }
//
//  // MANIPULATORS
//  void *my_StrPool::allocate(int size)
//  {
//      if (0 == size) {
//          return 0;                                                 // RETURN
//      }
//
//      if (d_block_p && size + d_cursor <= d_blockSize) {
//          char *p = d_block_p + d_cursor;
//          d_cursor += size;
//          return p;                                                 // RETURN
//      }
//      else {
//          return allocateBlock(size);
//      }
//  }
//..
// In the code shown above, the 'my_StrPool' memory manager allocates from its
// 'bdlma::InfrequentDeleteBlockList' member object an initial memory block of
// size 'INITIAL_SIZE'.  This size is multiplied by 'GROWTH_FACTOR' each time a
// depleted memory block is replaced by a newly-allocated block.  The
// 'allocate' method distributes memory from the current memory block
// piecemeal, except when the requested size (1) is not available in the
// current block, or (2) exceeds the 'THRESHOLD', in which case a separate
// memory block is allocated and returned.  When the 'my_StrPool' memory
// manager is destroyed, its 'bdlma::InfrequentDeleteBlockList' member object
// is also destroyed, which in turn automatically deallocates all of its
// managed memory blocks.

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

                  // ===============================
                  // class InfrequentDeleteBlockList
                  // ===============================

class InfrequentDeleteBlockList {
    // This class implements a low-level memory manager that allocates and
    // manages a sequence of memory blocks -- each potentially of a different
    // size as specified during the invocation of the 'allocate' method.  The
    // 'release' method deallocates the entire sequence of memory blocks, as
    // does the destructor.  Note that memory blocks cannot be deallocated
    // individually.

    // PRIVATE TYPES
    struct Block {
        // This 'struct' overlays the beginning of each managed block of
        // allocated memory, implementing a singly-linked list of managed
        // blocks, and thereby enabling constant-time additions to the list of
        // blocks.

        Block                               *d_next_p;  // next pointer
        bsls::AlignmentUtil::MaxAlignedType  d_memory;  // force alignment
    };

    // DATA
    Block            *d_head_p;       // address of 1st block of memory (or 0)
    bslma::Allocator *d_allocator_p;  // memory allocator (held, not owned)

  private:
    // NOT IMPLEMENTED
    InfrequentDeleteBlockList(const InfrequentDeleteBlockList&);
    InfrequentDeleteBlockList& operator=(const InfrequentDeleteBlockList&);

  public:
    // CREATORS
    explicit
    InfrequentDeleteBlockList(bslma::Allocator *basicAllocator = 0);
        // Create an empty block list suitable for managing memory blocks of
        // varying sizes.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    ~InfrequentDeleteBlockList();
        // Destroy this object and deallocate all outstanding memory blocks
        // managed by this object.

    // MANIPULATORS
    void *allocate(int size);
        // Return the address of a contiguous block of memory of the specified
        // 'size' (in bytes).  If 'size' is 0, no memory is allocated and 0 is
        // returned.  The returned memory is guaranteed to be maximally
        // aligned.  The behavior is undefined unless '0 <= size'.

    void deallocate(void *address);
        // This method has no effect on the memory block at the specified
        // 'address' as all memory allocated by this object is managed.  The
        // behavior is undefined unless 'address' was allocated by this
        // object, and has not already been released.

    void release();
        // Deallocate all memory blocks currently managed by this object,
        // returning it to its default-constructed state.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                  // -------------------------------
                  // class InfrequentDeleteBlockList
                  // -------------------------------

// CREATORS
inline
InfrequentDeleteBlockList::InfrequentDeleteBlockList(
                                              bslma::Allocator *basicAllocator)
: d_head_p(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

// MANIPULATORS
inline
void InfrequentDeleteBlockList::deallocate(void *)
{
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2012 Bloomberg L.P.
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
