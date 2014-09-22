// bsls_blockgrowth.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLS_BLOCKGROWTH
#define INCLUDED_BSLS_BLOCKGROWTH

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a namespace for memory block growth strategies.
//
//@CLASSES:
//  bsls::BlockGrowth: namespace for enumerated growth strategy values
//
//@SEE_ALSO: bsls_alignment
//
//@DESCRIPTION: This component provides a namespace for enumerating memory
// block growth strategies, and provides a function that converts each of these
// enumerators to its corresponding string representation.
//
///Block Growth Strategy
///---------------------
// This component supports two memory block growth strategies:
//
//: GEOMETRIC GROWTH: A container, pool or allocator that employs this
//:   strategy, as indicated by the enumerator 'BSLS_GEOMETRIC', grows its
//:   buffer geometrically.
//:
//: CONSTANT GROWTH: A container, pool or allocator that employs this strategy,
//:   as indicated by the enumerator 'BSLS_CONSTANT', locks the buffer growth.
//:   The new buffer is always the same size as the current buffer.
//
///Usage
///-----
// Memory block growth strategies are often used in memory managers and
// containers to control memory usage.  First of all, suppose we have a
// 'my_BlockList' class that manages a link list of memory blocks:
//..
//  class my_BlockList {
//      // ...
//  };
//..
// We can then create a memory manager class 'my_SequentialPool' that manages a
// pool of memory:
//..
//  class my_SequentialPool {
//      // This class implements a memory pool that dispenses (heterogeneous)
//      // blocks of memory (of varying, user-specified-sizes) from a sequence
//      // of dynamically allocated buffers.
//
//      // DATA
//      char         *d_currentBuffer_p;    // pointer to current buffer
//
//      int           d_currentBufferSize;  // size of current buffer
//
//      bsls::BlockGrowth::Strategy
//                    d_growthStrategy;     // growth strategy
//
//      my_BlockList  d_blockList;          // manager for all allocated memory
//                                          // blocks
//
//      // NOT IMPLEMENTED
//      my_SequentialPool(const my_SequentialPool&);
//      my_SequentialPool& operator=(const my_SequentialPool&);
//
//    private:
//      // PRIVATE MANIPULATORS
//      int calculateNextSize(int size);
//          // Return the next buffer size sufficient to satisfy a memory
//          // allocation request of the specified 'size' (in bytes).
//
//    public:
//      // CREATORS
//      my_SequentialPool(bsls::BlockGrowth::Strategy  strategy);
//          // Create a pool with the specified memory block growth 'strategy'.
//
//      // ...
//
//      // MANIPULATORS
//      void *allocate(int size);
//          // Return the address of a contiguous block of memory of the
//          // specified 'size' (in bytes).  If the pool cannot return the
//          // requested number of bytes, 'std::bad_alloc' will be thrown in an
//          // exception-enabled build, or the program will be aborted.  The
//          // behavior is undefined unless 'size > 0'.
//  };
//..
// The implementation for the rest of the class is elided as the function
// 'calculateNextSize' alone is sufficient to illustrate the use of this
// component:
//..
//  // PRIVATE MANIPULATORS
//  int my_SequentialPool::calculateNextSize(int size)
//  {
//      if (bsls::BlockGrowth::BSLS_CONSTANT == d_growthStrategy) {
//          return d_currentBufferSize;
//      }
//..
// Note that, if the growth strategy in effect is constant growth
// ('BSLS_CONSTANT'), the size of the internal buffers will always be the same.
// If 'size' is greater than the buffer size, the implementation of 'allocate'
// will return a block having the exact 'size' from the internal block list:
//..
//  int nextSize = d_currentBufferSize;
//
//  do {
//      nextSize *= 2;  // growth factor of 2
//  } while (nextSize < size);
//..
// Note that, if the growth strategy in effect is geometric growth
// ('BSLS_GEOMETRIC'), the size of the internal buffer grows geometrically by a
// factor of 2:
//..
//      return nextSize;
//  }
//..

namespace BloombergLP {

namespace bsls {

                        // ==================
                        // struct BlockGrowth
                        // ==================

struct BlockGrowth {
    // This struct provides a namespace for memory block growth strategies for
    // pools, allocators, containers, etc.

    // TYPES
    enum Strategy {
        BSLS_GEOMETRIC,  // Default.  Indicates that memory block sizes grow
                         // geometrically.

        BSLS_CONSTANT    // Indicates that memory block size is locked.
    };

    // CLASS METHODS
    static const char *toAscii(BlockGrowth::Strategy value);
        // Return the string representation of the specified enumerator
        // 'value'.  The string representation of 'value' matches its
        // corresponding enumerator name with the 'BSLS_' prefix elided.
};

}  // close package namespace


}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
