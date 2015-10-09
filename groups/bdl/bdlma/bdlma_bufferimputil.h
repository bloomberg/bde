// bdlma_bufferimputil.h                                              -*-C++-*-
#ifndef INCLUDED_BDLMA_BUFFERIMPUTIL
#define INCLUDED_BDLMA_BUFFERIMPUTIL

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
//@DESCRIPTION: This component provides a 'struct', 'bdlma::BufferImpUtil',
// that implements procedures for allocating memory from a buffer using an
// indicated memory alignment strategy.  Each of the procedures take a buffer,
// the size of the buffer, a cursor pointing to the free memory within the
// buffer, and the allocation size.  Two of the procedures,
// 'allocateFromBuffer' and 'allocateFromBufferRaw', take an additional
// argument that specifies the memory alignment strategy to apply.  The other
// six procedures apply a specific memory alignment strategy as indicated by
// their names (e.g., 'allocateNaturallyAlignedFromBuffer' and
// 'allocateMaximallyAlignedFromBufferRaw').  In all cases, a pointer to the
// allocated memory is returned, and the cursor passed in is updated to point
// to the portion of the buffer that contains the next available free memory.
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
// Using natural alignment, suppose 1 byte is allocated from the buffer using
// 'allocateFromBuffer':
//..
//  BufferImpUtil::allocateFromBuffer(&cursor, buffer, bufferSize, 1
//                                    bsls::AlignmentStrategy::BSLS_NATURAL);
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
//  BufferImpUtil::allocateFromBuffer(&cursor, buffer, bufferSize, 2,
//                                    bsls::AlignmentStrategy::BSLS_NATURAL);
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
// strategy (otherwise, more bytes would have been skipped if maximum alignment
// was used).  See 'bsls_alignment' for more details about memory alignment.
//
///Raw versus Non-Raw
///------------------
// The raw and non-raw versions differ in behavior only when the requested
// memory size is larger than the memory available within the provided buffer
// (after taking memory alignment into consideration).  The raw versions result
// in undefined behavior, while the non-raw versions return 0.  Note that the
// safety of the non-raw versions comes at the extra cost of a conditional
// statement.  For example, clients of the non-raw versions must check the
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
//      int        d_bufferSize;  // size (in bytes) of the current buffer
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
//      explicit my_SequentialPool(bslma::Allocator *basicAllocator = 0);
//          // Create a memory pool that dispenses heterogeneous blocks of
//          // memory (of varying, user-specified sizes).  Optionally specify a
//          // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
//          // 0, the currently installed default allocator is used.
//
//      ~my_SequentialPool();
//          // Destroy this memory pool and release all associated memory.
//
//      // MANIPULATORS
//      void *allocate(int size);
//          // Return the address of a contiguous block of naturally-aligned
//          // memory of the specified 'size' (in bytes).  The behavior is
//          // undefined unless '0 < size'.
//  };
//..
// The implementations of the constructor and destructor are elided since
// 'allocate' alone is sufficient to illustrate the use of
// 'bdlma::BufferImpUtil':
//..
//  void *my_SequentialPool::allocate(int size)
//  {
//      assert(0 < size);
//
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

                           // ====================
                           // struct BufferImpUtil
                           // ====================

struct BufferImpUtil {
    // This 'struct' provides a namespace for a suite of pure procedures for
    // allocating memory from a buffer.

    // CLASS METHODS
    static void *allocateFromBuffer(int                       *cursor,
                                    char                      *buffer,
                                    int                        bufferSize,
                                    int                        size,
                                    bsls::Alignment::Strategy  strategy);
        // Allocate a memory block of the specified 'size' (in bytes) from the
        // specified 'buffer' having the specified 'bufferSize' (in bytes) at
        // the specified 'cursor' position, using the specified alignment
        // 'strategy'.  Return the address of the allocated memory block if
        // 'buffer' contains sufficient available memory, and 0 otherwise.  The
        // 'cursor' is set to the first byte position immediately after the
        // allocated memory if there is sufficient memory, and not modified
        // otherwise.  The behavior is undefined unless '0 <= bufferSize',
        // '0 < size', '0 <= *cursor', and '*cursor <= bufferSize'.

    static void *allocateMaximallyAlignedFromBuffer(int  *cursor,
                                                    char *buffer,
                                                    int   bufferSize,
                                                    int   size);
        // Allocate a maximally-aligned memory block of the specified 'size'
        // (in bytes) from the specified 'buffer' having the specified
        // 'bufferSize' (in bytes) at the specified 'cursor' position.  Return
        // the address of the allocated memory block if 'buffer' contains
        // sufficient available memory, and 0 otherwise.  The 'cursor' is set
        // to the first byte position immediately after the allocated memory if
        // there is sufficient memory, and not modified otherwise.  The
        // behavior is undefined unless '0 <= bufferSize', '0 < size',
        // '0 <= *cursor', and '*cursor <= bufferSize'.

    static void *allocateNaturallyAlignedFromBuffer(int  *cursor,
                                                    char *buffer,
                                                    int   bufferSize,
                                                    int   size);
        // Allocate a naturally-aligned memory block of the specified 'size'
        // (in bytes) from the specified 'buffer' having the specified
        // 'bufferSize' (in bytes) at the specified 'cursor' position.  Return
        // the address of the allocated memory block if 'buffer' contains
        // sufficient available memory, and 0 otherwise.  The 'cursor' is set
        // to the first byte position immediately after the allocated memory if
        // there is sufficient memory, and not modified otherwise.  The
        // behavior is undefined unless '0 <= bufferSize', '0 < size',
        // '0 <= *cursor', and '*cursor <= bufferSize'.

    static void *allocateOneByteAlignedFromBuffer(int  *cursor,
                                                  char *buffer,
                                                  int   bufferSize,
                                                  int   size);
        // Allocate a 1-byte-aligned memory block of the specified 'size' (in
        // bytes) from the specified 'buffer' having the specified 'bufferSize'
        // (in bytes) at the specified 'cursor' position.  Return the address
        // of the allocated memory block if 'buffer' contains sufficient
        // available memory, and 0 otherwise.  The 'cursor' is set to the first
        // byte position immediately after the allocated memory if there is
        // sufficient memory, and not modified otherwise.  The behavior is
        // undefined unless '0 <= bufferSize', '0 < size', '0 <= *cursor', and
        // '*cursor <= bufferSize'.

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
        // and 'cursor' refers to a valid position in 'buffer'.

    static void *allocateMaximallyAlignedFromBufferRaw(int  *cursor,
                                                       char *buffer,
                                                       int   size);
        // Allocate a maximally-aligned memory block of the specified 'size'
        // (in bytes) from the specified 'buffer' at the specified 'cursor'
        // position.  Return the address of the allocated memory block.  The
        // 'cursor' is set to the first byte position immediately after the
        // allocated memory.  The behavior is undefined unless '0 < size',
        // 'buffer' contains sufficient available memory, and 'cursor' refers
        // to a valid position in 'buffer'.

    static void *allocateNaturallyAlignedFromBufferRaw(int  *cursor,
                                                       char *buffer,
                                                       int   size);
        // Allocate a naturally-aligned memory block of the specified 'size'
        // (in bytes) from the specified 'buffer' at the specified 'cursor'
        // position.  Return the address of the allocated memory block.  The
        // 'cursor' is set to the first byte position immediately after the
        // allocated memory.  The behavior is undefined unless '0 < size',
        // 'buffer' contains sufficient available memory, and 'cursor' refers
        // to a valid position in 'buffer'.

    static void *allocateOneByteAlignedFromBufferRaw(int  *cursor,
                                                     char *buffer,
                                                     int   size);
        // Allocate a 1-byte-aligned memory block of the specified 'size' (in
        // bytes) from the specified 'buffer' at the specified 'cursor'
        // position.  Return the address of the allocated memory block.  The
        // 'cursor' is set to the first byte position immediately after the
        // allocated memory.  The behavior is undefined unless '0 < size',
        // 'buffer' contains sufficient available memory, and 'cursor' refers
        // to a valid position in 'buffer'.
};

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
