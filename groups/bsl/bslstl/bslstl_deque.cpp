// bslstl_deque.cpp                                                   -*-C++-*-
#include <bslstl_deque.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

//
// IMPLEMENTATION NOTES: A deque is constructed as an array, 'd_blocks' of
// pointers to fixed-sized blocks of elements.  The array of pointers may have
// any number of unused elements at the beginning and/or end.  Elements can be
// inserted or removed from either end without moving any existing elements,
// simply by adding new blocks or removing empty ones.  A deque iterator
// contains a pointer 'd_value_p', to the referenced element and also a
// pointer, 'd_blockPtr_p', which points to the 'd_blocks' slot containing the
// pointer to the element's block (see diagram below).  The 'd_start' iterator
// points to the first used slot in 'd_blocks' and to the first used elements
// within the first block.  The 'd_finish' iterator points to the last used
// slot in 'd_blocks' and one past the last used element within the last block.
// 'd_blocksLength' holds the length of 'd_blocks', including unused slots.
//..
//   a-n : current elements in the deque
//   _   : uninitialized/destructed elements
//
//           d_blocks ---------+  (d_blocksLength == 6)
//                             |
//                             V
//                           +---+
//                           | . |
//                           |   |        +- d_start.d_value_p
//                           +---+        |
//                           |   |    +---V---+
//  d_start.d_blockPtr_p --> | o----> | __abc |
//                           |   |    +-------+
//                           +---+
//                           |   |    +-------+
//                           | o----> | defgh |
//                           |   |    +-------+
//                           +---+                  Blocks
//                           |   |    +-------+
//                           | o----> | ijklm |
//                           |   |    +-------+
//                           +---+
//                           |   |    +-------+
//  d_finish.d_blockPtr_p -> | o----> | n____ |
//                           |   |    +--^----+
//                           +---+       |
//                           | . |       +- d_finish.d_value_p
//                           |   |
//                           +---+
//
//..
///INVARIANTS
/// - - - - -
//..
// Class 'bslstl::DequeIterator' invariants for non-singular iterators:
//   - 'd_blockPtr_p' always points to a valid pointer to a valid block.
//   - 'd_value_p' always points within range '[blockBegin(), blockEnd())'
//   - Thus 'd_value_p' never points past the end of a block.
//
// Class 'bslstl_Deque' invariants:
//   - There is always at least one block allocated ('d_blocksLength' > 0).
//   - 'd_start' always points into the first allocated block.
//   - 'd_finish' always points into the last allocated block.
//   - The last block is never full.  Thus if 'size()' == 'BLOCK_LENGTH'
//     then there must be two blocks.
//   - 'd_finish' never points past the end of a block.
//   - The first block has zero or more uninitialized elements at the
//     beginning, the last block has one or more uninitialized elements at
//     the end.  All other blocks are full.
//   - The 'd_blocks' array may contain unused pointers at the beginning
//     and/or end.  The closed range,
//     '[d_start.d_blockPtr_p, d_finish.d_blockPtr_p]', is the range of
//     valid pointers within the 'd_blocks' array.
//
// For deques constructed with the 'RAW_INIT' argument ("raw" deques), the
// above invariants do not apply.  The following invariants do apply:
//   - A raw deque has singular values for 'd_start' and 'd_finish'.  (A
//     singular 'IteratorImp' value is one where both members are nil.)
//   - A raw deque has in constructed with 'd_blocks == 0' and
//     'd_blocksLength == 0'.
//   - 'd_blocks' and 'd_blocksLength' may be set to valid values, but no
//     blocks are allocated.
//   - Once one or more blocks are allocated and 'd_start' and 'd_finish'
//     set to point into those blocks, the deque is no longer raw and must
//     adhere to the normal deque invariants, above.
//   - Raw deques can be destructed and can be used with certain internal
//     operations, but must never be visible to the user.
//..

#include <bslmf_assert.h>

#include <bsls_assert.h>

#include <bslstl_string.h>   // for testing only
#include <bslstl_vector.h>   // for testing only

namespace bsl {

                           // --------------------
                           // class bsl::Deque_Imp
                           // --------------------

struct Deque_Imp {
    // This 'struct' must have the same layout as a 'bslstl_Deque' minus the
    // 'ContainerBase' inherited portion.

    // TYPES
    struct IteratorImp {
        char **d_blockPtr_p;
        char  *d_value_p;
    };

    // DATA
    void          *d_blocks;        // array of pointer to blocks
    std::size_t    d_blocksLength;  // length of d_blocks array
    IteratorImp    d_start;         // iterator to first element
    IteratorImp    d_finish;        // iterator to one past last element
};

// MANIPULATORS
void Deque_Util::move(void *dst, void *src)
{
    BSLS_ASSERT(dst);
    BSLS_ASSERT(src);

    Deque_Imp& dstDeque     = *static_cast<Deque_Imp *>(dst);
    Deque_Imp& srcDeque     = *static_cast<Deque_Imp *>(src);

    dstDeque.d_blocks       = srcDeque.d_blocks;
    dstDeque.d_blocksLength = srcDeque.d_blocksLength;
    dstDeque.d_start        = srcDeque.d_start;
    dstDeque.d_finish       = srcDeque.d_finish;

    srcDeque.d_blocks       = 0;  // put back in a raw state
}

void Deque_Util::swap(void *a, void *b)
{
    BSLS_ASSERT(a);
    BSLS_ASSERT(b);

    Deque_Imp& aDeque     = *static_cast<Deque_Imp *>(a);
    Deque_Imp& bDeque     = *static_cast<Deque_Imp *>(b);

    Deque_Imp temp;
    temp.d_blocks         = bDeque.d_blocks;
    temp.d_blocksLength   = bDeque.d_blocksLength;
    temp.d_start          = bDeque.d_start;
    temp.d_finish         = bDeque.d_finish;

    bDeque.d_blocks       = aDeque.d_blocks;
    bDeque.d_blocksLength = aDeque.d_blocksLength;
    bDeque.d_start        = aDeque.d_start;
    bDeque.d_finish       = aDeque.d_finish;

    aDeque.d_blocks       = temp.d_blocks;
    aDeque.d_blocksLength = temp.d_blocksLength;
    aDeque.d_start        = temp.d_start;
    aDeque.d_finish       = temp.d_finish;
}

}  // close namespace bsl

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
