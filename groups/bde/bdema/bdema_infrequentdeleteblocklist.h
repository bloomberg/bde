// bdema_infrequentdeleteblocklist.h                                  -*-C++-*-
#ifndef INCLUDED_BDEMA_INFREQUENTDELETEBLOCKLIST
#define INCLUDED_BDEMA_INFREQUENTDELETEBLOCKLIST

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide allocation and management of infrequently deleted blocks.
//
//@CLASSES:
//  bdema_InfrequentDeleteBlockList: manager of infrequently deleted blocks
//
//@AUTHOR: Shao-wei Hung (shung1)
//
//@SEE_ALSO: bdema_blocklist
//
//@DESCRIPTION: This component implements a low-level memory manager,
// 'bdema_InfrequentDeleteBlockList', that allocates and manages a sequence of
// memory blocks, each of a potentially different size as specified during the
// 'allocate' method's invocation.  The 'release' method of a
// 'bdema_InfrequentDeleteBlockList' object deallocates the entire sequence of
// outstanding memory blocks, as does its destructor.  Note that, in contrast
// to 'bdema_BlockList', the 'bdema_InfrequentDeleteBlockList' class does *not*
// support the deallocation of individual items.  In particular, although
// 'bdema_InfrequentDeleteBlockList' has a 'deallocate' method, that method has
// no effect.
//
///Usage
///-----
// A 'bdema_InfrequentDeleteBlockList' object is commonly used to supply memory
// to more elaborate memory managers that distribute parts of each (larger)
// allocated memory block supplied by the 'bdema_InfrequentDeleteBlockList'
// object.  The 'my_StrPool' memory pool manager shown below requests
// relatively large blocks of memory from its 'bdema_InfrequentDeleteBlockList'
// member object and distributes memory chunks of varying sizes from each block
// on demand:
//..
//  // my_strpool.h
//
//  class my_StrPool {
//
//      // DATA
//      int             d_blockSize;  // size of current memory block
//
//      char           *d_block_p;    // current free memory block
//
//      int             d_cursor;     // offset to next available byte in block
//
//      bdema_InfrequentDeleteBlockList
//                      d_blockList;  // supplies managed memory blocks
//
//    private:
//      // PRIVATE MANIPULATORS
//      void *allocateBlock(int numBytes);
//          // Request a memory block of at least the specified 'numBytes' size
//          // (in bytes), and allocate the initial 'numBytes' from this block.
//          // Return the address of the allocated memory.
//
//    private:
//      // NOT IMPLEMENTED
//      my_StrPool(const my_StrPool&);
//      my_StrPool& operator=(const my_StrPool&);
//
//    public:
//      // CREATORS
//      my_StrPool(bslma_Allocator *basicAllocator = 0);
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
//  void *my_StrPool::allocate(int size)
//  {
//      if (0 == size) {
//          return 0;
//      }
//
//      if (d_block_p && size + d_cursor <= d_blockSize) {
//          char *p = d_block_p + d_cursor;
//          d_cursor += size;
//          return p;
//      }
//      else {
//          return allocateBlock(size);
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
//  // PRIVATE MANIPULATORS
//  void *my_StrPool::allocateBlock(int numBytes)
//  {
//      BSLS_ASSERT(0 < numBytes);
//
//      if (THRESHOLD < numBytes) { // Alloc separate block if above threshold.
//          return reinterpret_cast<char *>(d_blockList.allocate(numBytes));
//      }
//
//      if (d_block_p) { // Don't increase block size if no current block.
//          d_blockSize *= GROWTH_FACTOR;
//      }
//      d_block_p = reinterpret_cast<char*>(d_blockList.allocate(d_blockSize));
//      d_cursor  = numBytes;
//      return d_block_p;
//  }
//
//  // CREATORS
//  my_StrPool::my_StrPool(bslma_Allocator *basicAllocator)
//  : d_block_p(0)
//  , d_blockSize(INITIAL_SIZE)
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
// 'bdema_InfrequentDeleteBlockList' member object an initial memory block of
// size 'INITIAL_SIZE'.  This size is multiplied by 'GROWTH_FACTOR' each time a
// depleted memory block is replaced by a newly allocated block.  The
// 'allocate' method distributes memory from the current memory block
// piecemeal, except when the requested size (1) is not available in the
// current block, or (2) exceeds the 'THRESHOLD_SIZE', in which case a separate
// memory block is allocated and returned.  When the 'my_StrPool' memory
// manager is destroyed, its 'bdema_InfrequentDeleteBlockList' member object is
// also destroyed, which in turn automatically deallocates all of its managed
// memory blocks.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLS_ALIGNMENTUTIL
#include <bsls_alignmentutil.h>
#endif

namespace BloombergLP {

class bslma_Allocator;

                  // =====================================
                  // class bdema_InfrequentDeleteBlockList
                  // =====================================

class bdema_InfrequentDeleteBlockList {
    // This class implements a low-level memory manager that allocates and
    // manages a sequence of memory blocks -- each potentially of a different
    // size as specified during the invocation of the 'allocate' method.  This
    // object's 'release' method deallocates the entire sequence of memory
    // blocks, as does its destructor.  Note that memory blocks cannot be
    // deallocated individually.

    // PRIVATE TYPES
    struct Block {
        // This 'struct' overlays the beginning of each managed block of
        // allocated memory, implementing a singly-linked list of managed
        // blocks, and thereby enabling constant-time additions to the list of
        // blocks.

        Block                              *d_next_p;  // next pointer
        bsls_AlignmentUtil::MaxAlignedType  d_memory;  // force alignment
    };

    // DATA
    Block           *d_head_p;       // address of first block of memory (or 0)
    bslma_Allocator *d_allocator_p;  // memory allocator; held, but not owned

  private:
    // NOT IMPLEMENTED
    bdema_InfrequentDeleteBlockList(const bdema_InfrequentDeleteBlockList&);
    bdema_InfrequentDeleteBlockList& operator=(
                                    const bdema_InfrequentDeleteBlockList&);

  public:
    // CREATORS
    bdema_InfrequentDeleteBlockList(bslma_Allocator *basicAllocator = 0);
        // Create an empty block list suitable for managing memory blocks of
        // varying sizes.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    ~bdema_InfrequentDeleteBlockList();
        // Destroy this object and deallocate all outstanding memory blocks
        // managed by this object.

    // MANIPULATORS
    void *allocate(int size);
        // Return the address of a contiguous block of memory of the specified
        // 'size' (in bytes).  If 'size' is 0, no memory is allocated and 0 is
        // returned.  The returned memory is guaranteed to be maximally
        // aligned.  The behavior is undefined unless '0 <= size'.

    void deallocate(void *address);
        // This method has no effect as all memory allocated by this allocator
        // is managed.

    void release();
        // Deallocate all memory blocks currently managed by this object,
        // returning it to its default-constructed state.
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                  // -------------------------------------
                  // class bdema_InfrequentDeleteBlockList
                  // -------------------------------------

// CREATORS
inline
bdema_InfrequentDeleteBlockList::bdema_InfrequentDeleteBlockList(
                                               bslma_Allocator *basicAllocator)
: d_head_p(0)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

// MANIPULATORS
inline
void bdema_InfrequentDeleteBlockList::deallocate(void *)
{
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
