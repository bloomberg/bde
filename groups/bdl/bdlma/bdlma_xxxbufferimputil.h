// bdlma_xxxbufferimputil.h                                              -*-C++-*-
#ifndef INCLUDED_BDLMA_XXXBUFFERIMPUTIL
#define INCLUDED_BDLMA_XXXBUFFERIMPUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide pure procedures for allocating memory from a buffer.
//
//@CLASSES:
//  bdlma::BufferImpUtil: pure procedures for allocating memory from a buffer
//
//@SEE_ALSO: bdlma_buffermanager
//
//@AUTHOR: Arthur Chiu (achiu21)
//
//@DESCRIPTION: This component provides pure procedures for allocating memory
// from a buffer using a specified memory alignment strategy.  Two procedures
// are provided: 'allocateFromBuffer' and 'allocateFromBufferRaw', each of
// which takes a buffer, the size of the buffer, a cursor pointing to the free
// memory within the buffer, the allocation size, and a memory alignment
// strategy.  A pointer to the allocated memory is returned, and the cursor
// passed in is updated to point to the portion of the buffer that contains the
// next available free memory.
//
// For example, suppose we initially have a 2-byte aligned buffer having a size
// of 5 bytes, and a cursor pointing to the first byte:
//..
//                          0     1     2     3     4
//                        _____ _____ _____ _____ _____
//  buffer (size = 5):   |  F  |  F  |  F  |  F  |  F  |          A - allocated
//                       `=====^=====^=====^=====^====='          F - free
//                          ^                                     W - wasted
//                          |
//                        cursor
//..
// Using natural alignment, 1 byte is allocated from the buffer using
// 'allocateFromBuffer':
//..
//  allocateFromBuffer(&cursor, buffer, bufferSize, 1
//                                      bsls::AlignmentStrategy::BSLS_NATURAL);
//..
// The cursor will be advanced as follows:
//..
//                          0     1     2     3     4
//                        _____ _____ _____ _____ _____
//  buffer (size = 5):   |  A  |  F  |  F  |  F  |  F  |          A - allocated
//                       `=====^=====^=====^=====^====='          F - free
//                                ^                               W - wasted
//                                |
//                              cursor
//..
// Suppose 'allocateFromBuffer' is then used to allocate 2 bytes:
//..
//  allocateFromBuffer(&cursor, buffer, bufferSize, 2,
//                                      bsls::AlignmentStrategy::BSLS_NATURAL);
//..
// The cursor will be advanced as follows (after taking into consideration the
// alignment strategy used):
//..
//                          0     1     2     3     4
//                        _____ _____ _____ _____ _____
//  buffer (size = 5):   |  A  |  W  |  A  |  A  |  F  |          A - allocated
//                       `=====^=====^=====^=====^====='          F - free
//                                                  ^             W - wasted
//                                                  |
//                                                cursor
//..
// The byte at (only) position 1 is skipped because of the natural alignment
// strategy (otherwise more bytes would have been skipped if maximum alignment
// was used).  See 'bsls_alignment' for more details about memory alignment.
//
///Raw vs. Non-Raw
///---------------
// The raw and non-raw versions differ in behavior only when the requested
// memory size is larger than the memory available within the provided buffer
// (after taking memory alignment into consideration).  The raw version results
// in undefined behavior, while the non-raw version returns 0.  Note that the
// safety of the non-raw version comes at the extra cost of conditional
// statement.  An example is that clients of the non-raw version must check the
// return value to ensure successful allocation.
//
///Usage
///-----
// This component is typically used by a class that manages a memory buffer.
// First, suppose we have a class that maintains a linked list of memory
// blocks, details of which are elided:
//..
//  class BlockList {
//      // ...
//  };
//..
// We can then create our memory manager using 'BlockList':
//..
//  class my_SequentialPool {
//      // This class allocates memory from an internal pool of memory buffers
//      // using natural alignment.  All allocated memory is managed internally
//      // by the pool and released when the pool is destroyed.
//
//      // DATA
//      char      *d_buffer_p;    // pointer to current buffer
//      int        d_bufferSize;  // size of the current buffer
//      int        d_cursor;      // byte offset to unused memory in buffer
//      BlockList  d_blockList;   // used to replenish memory
//
//    private:
//      // PRIVATE MANIPULATORS
//      void replenishBuffer(int size);
//          // Replenish the current buffer with memory that satisfies an
//          // allocation request having at least the specified 'size' (in
//          // bytes).
//
//    public:
//      // CREATORS
//      my_SequentialPool(bslma::Allocator *basicAllocator = 0);
//          // Create a memory pool that dispenses user-specified-sized blocks
//          // of memory.  Optionally specify a 'basicAllocator' used to supply
//          // memory.  If 'basicAllocator' is 0, the currently installed
//          // default allocator is used.
//
//      ~my_SequentialPool();
//          // Destroy this memory pool and release all associated memory.
//
//      // MANIPULATORS
//      void *allocate(int size);
//          // Return the address of a contiguous block of memory of the
//          // specified 'size' (in bytes).
//  };
//..
// The implementations of the constructor and destructor are elided since
// 'allocate' alone is sufficient to illustrate the use of
// 'bdlma::BufferImpUtil':
//..
//  void *my_SequentialPool::allocate(int size)
//  {
//      void *address = bdlma::BufferImpUtil::allocateFromBuffer(
//                                              &d_cursor,
//                                              d_buffer_p,
//                                              d_bufferSize,
//                                              size,
//                                              bsls::Alignment::BSLS_NATURAL);
//..
// Note that if there is insufficient space in 'd_buffer_p',
// 'allocateFromBuffer' returns 0:
//..
//      if (address) {
//          return address;                                           // RETURN
//      }
//
//      replenishBuffer(size);
//
//      return bdlma::BufferImpUtil::allocateFromBufferRaw(
//                                              &d_cursor,
//                                              d_buffer_p,
//                                              size,
//                                              bsls::Alignment::BSLS_NATURAL);
//  }
//..
// Note that the *raw* version is used because the contract of
// 'replenishBuffer' guarantees that the buffer will have sufficient space to
// satisfy the allocation request of the specified 'size'.

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLS_ALIGNMENT
#include <bsls_alignment.h>
#endif

namespace BloombergLP {

namespace bdlma {
                        // ==========================
                        // struct BufferImpUtil
                        // ==========================

struct BufferImpUtil {
    // This 'struct' provides a namespace for a suite of pure procedures
    // for allocating memory from a buffer.

    // CLASS METHODS
    static void *allocateFromBuffer(int                       *cursor,
                                    char                      *buffer,
                                    int                        bufferSize,
                                    int                        size,
                                    bsls::Alignment::Strategy  strategy);
        // Allocate a memory block of the specified 'size' (in bytes) from the
        // specified 'buffer' having the specified 'bufferSize' at the
        // specified 'cursor' position, using the specified alignment
        // 'strategy'.  Return the address of the allocated memory block if
        // 'buffer' contains sufficient available memory, and 0 otherwise.  The
        // 'cursor' is set to the first byte position immediately after the
        // allocated memory if there is sufficient memory, and not modified
        // otherwise.  The behavior is undefined unless '0 <= bufferSize',
        // '0 < size', '0 <= *cursor', and '*cursor <= bufferSize'.

    static void *allocateFromBufferRaw(int                       *cursor,
                                       char                      *buffer,
                                       int                        size,
                                       bsls::Alignment::Strategy  strategy);
        // Allocate a memory block of the specified 'size' (in bytes) from the
        // specified 'buffer' at the specified 'cursor' position, using the
        // specified alignment 'strategy'.  Return the address of the allocated
        // memory block.  The 'cursor' is set to the first byte position
        // immediately after the allocated memory.  The behavior is undefined
        // unless '0 < size', 'buffer' contains sufficient available memory,
        // and 'cursor' refers to a valid position in 'buffer' or 1 byte past
        // the end of 'buffer'.
};
}  // close package namespace

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
