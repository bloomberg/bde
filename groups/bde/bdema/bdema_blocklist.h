// bdema_blocklist.h                                                  -*-C++-*-
#ifndef INCLUDED_BDEMA_BLOCKLIST
#define INCLUDED_BDEMA_BLOCKLIST

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide allocation and management of a sequence of memory blocks.
//
//@CLASSES:
//   bdema_BlockList: memory manager that allocates and manages memory blocks
//
//@AUTHOR: Shao-wei Hung (shung1)
//
//@SEE_ALSO: bdema_infrequentdeleteblocklist
//
//@DESCRIPTION: This component implements a low-level memory manager,
// 'bdema_Blocklist', that allocates and manages a sequence of memory blocks,
// each of a potentially different size as specified during the 'allocate'
// method's invocation.  The 'release' method of a 'bdema_BlockList' object
// deallocates the entire sequence of outstanding memory blocks, as does its
// destructor.  Note that a 'bdem_Blocklist', at a minor memory expense, allows
// for individual items to be deallocated (see also
// 'bdema_infrequentdeleteblocklist').
//
///Usage
///-----
// A 'bdema_BlockList' object is commonly used to supply memory to more
// elaborate memory managers that distribute parts of each (larger) allocated
// memory block supplied by the 'bdema_BlockList' object.  The 'my_StrPool'
// memory pool manager shown below requests relatively large blocks of memory
// from its 'bdema_BlockList' member object and distributes memory chunks of
// varying sizes from each block on demand:
//..
//  // my_strpool.h
//
//  class my_StrPool {
//      int              d_blockSize; // size of current memory block
//      char            *d_block_p;   // current free memory block
//      int              d_cursor;    // offset to next available byte in block
//      bdema_BlockList  d_blockList; // supplies managed memory blocks
//
//    private:
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
//      my_StrPool(bslma_Allocator *basicAllocator = 0);
//          // Create a memory manager using the specified 'basicAllocator' to
//          // supply memory.  If 'basicAllocator' is 0, the currently
//          // installed default allocator is used.
//
//      ~my_StrPool();
//          // Destroy this object and release all associated memory.
//
//      void *allocate(int numBytes);
//          // Allocate the specified 'numBytes' of memory and return its
//          // address.
//
//      void release();
//          // Release all memory currently allocated through this object.
//  };
//
//  inline
//  void *my_StrPool::allocate(int numBytes)
//  {
//      if (numBytes <= 0) {
//          return 0;
//      }
//
//      if (d_block_p && numBytes + d_cursor <= d_blockSize) {
//          char *p = d_block_p + d_cursor;
//          d_cursor += numBytes;
//          return p;
//      }
//      else {
//          return allocateBlock(numBytes);
//      }
//  }
//
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
//      GROWTH_FACTOR = 2,    // multiplicative factor by which to grow block
//
//      THRESHOLD     = 128   // size, beyond which, an individual block may be
//                            // allocated if it doesn't fit in current block
//  };
//
//  void *my_StrPool::allocateBlock(int numBytes)
//  {
//      BSLS_ASSERT(0 < numBytes);
//
//      if (THRESHOLD < numBytes) { // Alloc separate block if above threshold.
//          return (char *) d_blockList.allocate(numBytes);
//      }
//      else {
//          if (d_block_p) { // Don't increase block size if no current block.
//              d_blockSize *= GROWTH_FACTOR;
//          }
//          d_block_p = (char *) d_blockList.allocate(d_blockSize);
//          d_cursor = numBytes;
//          return d_block_p;
//      }
//  }
//
//  my_StrPool::my_StrPool(bslma_Allocator *basicAllocator)
//  : d_blockSize(INITIAL_SIZE)
//  , d_block_p(0)
//  , d_blockList(basicAllocator)  // the blocklist knows about 'bslma_default'
//  {
//  }
//
//  my_StrPool::~my_StrPool()
//  {
//      BSLS_ASSERT(INITIAL_SIZE <= d_blockSize);
//      BSLS_ASSERT(d_block_p || 0 <= d_cursor && d_cursor <= d_blockSize);
//  }
//..
// In the code shown above, the 'my_StrPool' memory manager allocates from its
// 'bdema_BlockList' member object an initial memory block of size
// 'INITIAL_SIZE'.  This size is multiplied by 'GROWTH_FACTOR' each time a
// depleted memory block is replaced by a newly allocated block.  The
// 'allocate' method distributes memory from the current memory block
// piecemeal, except when the requested size (1) is not available in the
// current block, or (2) exceeds the 'THRESHOLD_SIZE', in which case a separate
// memory block is allocated and returned.  When the 'my_StrPool' memory
// manager is destroyed, its 'bdema_BlockList' member object is also destroyed,
// which in turn automatically deallocates all of its managed memory blocks.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLS_ALIGNMENTUTIL
#include <bsls_alignmentutil.h>
#endif

#ifndef INCLUDED_BSLFWD_BSLMA_ALLOCATOR
#include <bslfwd_bslma_allocator.h>
#endif

namespace BloombergLP {

                        // =====================
                        // class bdema_BlockList
                        // =====================

class bdema_BlockList {
    // This class implements a low-level memory manager that allocates and
    // manages a sequence of memory blocks -- each potentially of a different
    // size as specified during the invocation of the 'allocate' method.
    // Allocated blocks may be efficiently deallocated individually, i.e.,
    // potentially in constant time depending on the supplied allocator.
    // This object's 'release' method deallocates the entire sequence of memory
    // blocks, as does its destructor.

    // TYPES
    struct Block {
        // This 'struct' overlays the beginning of each managed block of
        // allocated memory, implementing a doubly-linked list of managed
        // blocks, and thereby enabling constant-time deletions from, as well
        // as additions to, the list of blocks.

        Block                               *d_next_p;       // next pointer
        Block                              **d_addrPrevNext; // enable delete
        bsls_AlignmentUtil::MaxAlignedType   d_memory;       // force alignment
    };

    // DATA
    Block           *d_head_p;       // address of first block of memory (or 0)
    bslma_Allocator *d_allocator_p;  // memory allocator; held, but not owned

  private:
    // NOT IMPLEMENTED
    bdema_BlockList(const bdema_BlockList&);
    bdema_BlockList& operator=(const bdema_BlockList&);

  public:
    // CREATORS
    bdema_BlockList(bslma_Allocator *basicAllocator = 0);
        // Create an empty block list suitable for managing memory blocks of
        // varying sizes.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    ~bdema_BlockList();
        // Destroy this object and deallocate all outstanding memory blocks
        // managed by this object.

    // MANIPULATORS
    void *allocate(int numBytes);
        // Allocate a memory block of the specified 'numBytes' size and return
        // its address.  The returned memory is guaranteed to be maximally
        // aligned.  The behavior is undefined unless '0 <= numBytes'.

    void deallocate(void *address);
        // Return the memory at the specified 'address' back to the associated
        // allocator.  If 'address' is 0, this function has no effect.  The
        // behavior is undefined if 'address' was not allocated using this
        // memory manager, or has already been deallocated.

    void release();
        // Deallocate all memory blocks currently managed by this object,
        // returning it to its default-constructed state.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ---------------------
                        // class bdema_BlockList
                        // ---------------------

// CREATORS
inline
bdema_BlockList::bdema_BlockList(bslma_Allocator *basicAllocator)
: d_head_p(0)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
