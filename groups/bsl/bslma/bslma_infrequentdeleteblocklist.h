// bslma_infrequentdeleteblocklist.h                                  -*-C++-*-
#ifndef INCLUDED_BSLMA_INFREQUENTDELETEBLOCKLIST
#define INCLUDED_BSLMA_INFREQUENTDELETEBLOCKLIST

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide allocation and management of a sequence of memory blocks.
//
//@INTERNAL_DEPRECATED: Use 'bdema_infrequentdeleteblocklist' instead.
//
//@CLASSES:
//  bslma::InfrequentDeleteBlockList: list of infrequently deleted blocks
//
//@SEE_ALSO: bdema_infrequentdeleteblocklist
//
//@AUTHOR: Shao-wei Hung (shung1)
//
//@DESCRIPTION: This component implements a memory manager that allocates and
// manages a sequence of memory blocks, each potentially of a different size as
// specified in the 'allocate' method's invocation.  The
// 'bslma::InfrequentDeleteBlockList' object's 'release' method deallocates the
// entire sequence of memory blocks, as does its destructor.
//
// This component does not allow individual items to be deallocated.
//
///Usage
///-----
// The 'bslma::InfrequentDeleteBlockList' object is commonly used to supply
// memory to more elaborate memory managers that distribute parts of a memory
// block supplied by the 'bslma::InfrequentDeleteBlockList' object.  The
// 'my_StrPool' memory pool manager shown below requests relatively large
// blocks of memory from its 'bslma::InfrequentDeleteBlockList' member object
// and distributes memory chunks of varying sizes from each block on demand:
//..
//  // my_strpool.h
//
//  class my_StrPool {
//      int   d_blockSize;  // size of current memory block
//      char *d_block_p;    // current free memory block
//      int   d_cursor;     // offset to address of next available byte
//      bslma::InfrequentDeleteBlockList d_blockList;
//                                       // supplies managed memory blocks
//
//    private:
//      void *allocateBlock(int numBytes);
//          // Request a new memory block of at least the specified 'numBytes'
//          // size and allocate the initial 'numBytes' from this block.
//          // Return the address of the allocated memory.
//
//    private: // not implemented
//      my_StrPool(const my_StrPool&);
//      my_StrPool& operator=(const my_StrPool&);
//
//    public:
//      my_StrPool(bslma::Allocator *basicAllocator = 0);
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
//          // Release all memory currently allocated through this instance.
//  };
//
//  inline
//  void *my_StrPool::allocate(int numBytes)
//  {
//      if (numBytes <= 0) {
//          return 0;
//      }
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
//      INITIAL_SIZE = 128, // initial block size
//      GROW_FACTOR  = 2,   // multiplicative factor to grow block size
//      THRESHOLD    = 128  // Size beyond which an individual block may be
//                          // allocated if it does not fit in the current
//                          // block.
//  };
//
//  void *my_StrPool::allocateBlock(int numBytes)
//  {
//      ASSERT(0 < numBytes);
//      if (THRESHOLD < numBytes) { // Alloc separate block if above threshold.
//          return (char *) d_blockList.allocate(numBytes);
//      }
//      else {
//          if (d_block_p) { // Don't increase block size if no current block.
//              d_blockSize *= GROW_FACTOR;
//          }
//          d_block_p = (char *) d_blockList.allocate(d_blockSize);
//          d_cursor = numBytes;
//          return d_block_p;
//      }
//  }
//
//  my_StrPool::my_StrPool(bslma::Allocator *basicAllocator)
//  : d_blockSize(INITIAL_SIZE)
//  , d_block_p(0)
//  , d_blockList(basicAllocator)
//  {
//  }
//
//  my_StrPool::~my_StrPool()
//  {
//      assert(INITIAL_SIZE <= d_blockSize);
//      assert(d_block_p || 0 <= d_cursor && d_cursor <= d_blockSize);
//  }
//..
// In the code above, the 'my_StrPool' memory manager allocates from its
// 'bslma::InfrequentDeleteBlockList' member object an initial memory block of
// size 'INITIAL_SIZE'.  This size is multiplied by 'GROW_FACTOR' each time a
// deplete memory block is replaced by a newly allocated block.  The 'allocate'
// method distributes memory from the current memory block piecemeal, except
// when the requested size: 1) is not available in the current block and 2)
// exceeds the 'THRESHOLD_SIZE', in which case a separate memory block is
// allocated and returned.  When the 'my_StrPool' memory manager is destroyed,
// its 'bslma::InfrequentDeleteBlockList' member object is also destroyed,
// which in turn automatically deallocates all of its managed memory blocks.

#ifdef BDE_OSS_TEST
#error "bslma_infrequentdeleteblocklist is deprecated"
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLS_ALIGNMENTUTIL
#include <bsls_alignmentutil.h>
#endif

namespace BloombergLP {


namespace bslma {

class Allocator;

                  // ===============================
                  // class InfrequentDeleteBlockList
                  // ===============================

class InfrequentDeleteBlockList {
    // This class implements a memory manager that allocates and manages a
    // sequence of memory blocks, each potentially of a different size as
    // specified in the 'allocate' method's invocation.  This object's
    // 'release' method deallocates the entire sequence of memory blocks, as
    // does its destructor.  Note that memory blocks can not be deallocated
    // individually.

    // TYPES
    struct Block {
        Block                               *d_next_p;
        bsls::AlignmentUtil::MaxAlignedType  d_memory;  // force alignment
    };

    // DATA
    Block           *d_head_p;       // refers to the first memory block
    Allocator       *d_allocator_p;  // holds (but does not own) the allocator

    // NOT IMPLEMENTED
    InfrequentDeleteBlockList(const InfrequentDeleteBlockList&);
    InfrequentDeleteBlockList&
                          operator=(const InfrequentDeleteBlockList&);

  public:
    // CREATORS
    InfrequentDeleteBlockList(Allocator *basicAllocator = 0);
        // Create a memory manager.  Optionally specify a 'basicAllocator' used
        // to supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    ~InfrequentDeleteBlockList();
        // Destroy this object and deallocate all memory blocks managed by this
        // object.

    // MANIPULATORS
    void *allocate(int numBytes);
        // Allocate a memory block of the specified 'numBytes' size and return
        // its address.  The returned memory is guaranteed to be maximally
        // aligned.  The behavior is undefined unless '0 <= numBytes'.

    void deallocate(void *address);
        // This method has no effect.

    void release();
        // Deallocate all memory blocks managed by this object.
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                  // -------------------------------
                  // class InfrequentDeleteBlockList
                  // -------------------------------

// CREATORS
inline
InfrequentDeleteBlockList::
               InfrequentDeleteBlockList(Allocator *basicAllocator)
: d_head_p(0)
, d_allocator_p(Default::allocator(basicAllocator))
{
}

// MANIPULATORS
inline
void InfrequentDeleteBlockList::deallocate(void *)
{
}

}  // close package namespace

#ifndef BDE_OMIT_TRANSITIONAL  // BACKWARD_COMPATIBILITY
// ===========================================================================
//                           BACKWARD COMPATIBILITY
// ===========================================================================

typedef bslma::InfrequentDeleteBlockList bslma_InfrequentDeleteBlockList;
    // This alias is defined for backward compatibility.
#endif  // BDE_OMIT_TRANSITIONAL -- BACKWARD COMPATIBILITY

}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
