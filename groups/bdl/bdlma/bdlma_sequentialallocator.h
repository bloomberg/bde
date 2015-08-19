// bdlma_sequentialallocator.h                                        -*-C++-*-
#ifndef INCLUDED_BDLMA_SEQUENTIALALLOCATOR
#define INCLUDED_BDLMA_SEQUENTIALALLOCATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a managed allocator using dynamically-allocated buffers.
//
//@CLASSES:
//   bdlma::SequentialAllocator: managed allocator using dynamic buffers
//
//@SEE_ALSO: bdlma_infrequentdeleteblocklist, bdlma_sequentialpool
//
//@DESCRIPTION: This component provides a concrete mechanism,
// 'bdlma::SequentialAllocator', that implements the 'bdlma::ManagedAllocator'
// protocol and efficiently allocates heterogeneous memory blocks (of varying,
// user-specified sizes) from a dynamically-allocated internal buffer:
//..
//   ,--------------------------.
//  ( bdlma::SequentialAllocator )
//   `--------------------------'
//                |         ctor/dtor
//                |         allocateAndExpand
//                |         reserveCapacity
//                |         truncate
//                V
//    ,-----------------------.
//   ( bdlma::ManagedAllocator )
//    `-----------------------'
//                |         release
//                V
//       ,----------------.
//      ( bslma::Allocator )
//       `----------------'
//                          allocate
//                          deallocate
//..
// If an allocation request exceeds the remaining free memory space in the
// internal buffer, the allocator either replenishes its buffer with new memory
// to satisfy the request, or returns a separate memory block, depending on
// whether the request size exceeds an optionally-specified maximum buffer
// size.  The 'release' method releases all memory allocated through the
// allocator, as does the destructor.  Note that individually allocated memory
// blocks cannot be separately deallocated.
//
// The main difference between a 'bdlma::SequentialAllocator' and a
// 'bdlma::SequentialPool' is that, very often, a 'bdlma::SequentialAllocator'
// is managed through a 'bslma::Allocator' pointer.  Hence, every call to the
// 'allocate' method invokes a virtual function call, which is slower than
// invoking the non-virtual 'allocate' method on a 'bdlma::SequentialPool'.
// However, since 'bslma::Allocator *' is widely used across BDE interfaces,
// 'bdlma::SequentialAllocator' is more general purpose than a
// 'bdlma::SequentialPool'.
//
///Optional 'initialSize' Parameter
///--------------------------------
// An optional 'initialSize' parameter can be supplied at construction to
// specify the initial size of the internal buffer.  If 'initialSize' is not
// supplied, an implementation-defined value is used for the initial size of
// the internal buffer.
//
///Optional 'maxBufferSize' Parameter
/// - - - - - - - - - - - - - - - - -
// If 'initialSize' is specified, an optional 'maxBufferSize' parameter can be
// supplied at construction to specify the maximum buffer size for geometric
// growth.  Once the internal buffer grows up to the 'maxBufferSize', further
// requests that exceed this size will be served by a separate memory block
// instead of the internal buffer.  The behavior is undefined unless
// 'maxBufferSize >= initialSize'.  Note that 'reserveCapacity' always ensures
// that the requested number of bytes is available (allocating a new internal
// buffer if necessary) regardless of whether the size of the request exceeds
// 'maxBufferSize'.
//
///Optional 'growthStrategy' Parameter
///-----------------------------------
// An optional 'growthStrategy' parameter can be supplied at construction to
// specify the growth rate of the dynamically-allocated buffers.  The buffers
// can grow either geometrically or remain constant in size.  If
// 'growthStrategy' is not specified, geometric growth is used.  See
// 'bsls_blockgrowth' for more details.
//
///Optional 'alignmentStrategy' Parameter
///--------------------------------------
// An optional 'alignmentStrategy' parameter can be supplied at construction to
// specify the memory alignment strategy.  Allocated memory blocks can either
// follow maximum alignment, natural alignment, or 1-byte alignment.  If
// 'alignmentStrategy' is not specified, natural alignment is used.  See
// 'bsls_alignment' for more details.
//
///Usage
///-----
// Allocators are often supplied, at construction, to objects requiring
// dynamically-allocated memory.  For example, consider the following
// 'my_DoubleStack' class whose constructor takes a 'bslma::Allocator *':
//..
//  // my_doublestack.h
//  // ...
//
//  class my_DoubleStack {
//      // This class implements a stack that stores 'double' values.
//
//      // DATA
//      double           *d_stack_p;      // dynamically-allocated array
//      int               d_size;         // physical capacity of stack
//      int               d_length;       // next available index in stack
//      bslma::Allocator *d_allocator_p;  // memory allocator (held, not owned)
//
//    private:
//      // PRIVATE MANIPULATORS
//      void increaseCapacity();
//          // Increase the capacity of this stack by at least one element.
//
//      // Not implemented:
//      my_DoubleStack(const my_DoubleStack&);
//
//    public:
//      // CREATORS
//      explicit my_DoubleStack(bslma::Allocator *basicAllocator = 0);
//          // Create a stack that stores 'double' values.  Optionally specify
//          // a 'basicAllocator' used to supply memory.  If 'basicAllocator'
//          // is 0, the currently installed default allocator is used.
//
//      ~my_DoubleStack();
//          // Destroy this stack and all elements held by it.
//
//      // ...
//
//      // MANIPULATORS
//      void push(double value);
//          // Push the specified 'value' onto this stack.
//
//      // ...
//  };
//
//  // ...
//
//  // MANIPULATORS
//  inline
//  void my_DoubleStack::push(double value)
//  {
//      if (d_length >= d_size) {
//          increaseCapacity();
//      }
//      d_stack_p[d_length++] = value;
//  }
//
//  // ...
//
//  // my_doublestack.cpp
//
//  // PRIVATE MANIPULATORS
//  void my_DoubleStack::increaseCapacity()
//  {
//      // Implementation elided.
//      // ...
//  }
//
//  // CREATORS
//  my_DoubleStack::my_DoubleStack(bslma::Allocator *basicAllocator)
//  : d_size(1)
//  , d_length(0)
//  , d_allocator_p(bslma::Default::allocator(basicAllocator))
//  {
//      d_stack_p = static_cast<double *>(
//                        d_allocator_p->allocate(d_size * sizeof *d_stack_p));
//  }
//..
// Note that, when the allocator passed in is a 'bdlma::SequentialAllocator',
// the 'deallocate' method is a no-op, and all memory is reclaimed during the
// destruction of the allocator:
//..
//  my_DoubleStack::~my_DoubleStack()
//  {
//      // CLASS INVARIANTS
//      assert(d_allocator_p);
//      assert(d_stack_p);
//      assert(0 <= d_length);
//      assert(0 <= d_size);
//      assert(d_length <= d_size);
//
//      d_allocator_p->deallocate(d_stack_p);
//  }
//
//  // ...
//..
// In 'main', users can create a 'bdlma::SequentialAllocator' and pass it to
// the constructor of 'my_DoubleStack':
//..
//      bdlma::SequentialAllocator sequentialAlloc;
//      my_DoubleStack dstack(&sequentialAlloc);
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLMA_MANAGEDALLOCATOR
#include <bdlma_managedallocator.h>
#endif

#ifndef INCLUDED_BDLMA_SEQUENTIALPOOL
#include <bdlma_sequentialpool.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLS_ALIGNMENT
#include <bsls_alignment.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_BLOCKGROWTH
#include <bsls_blockgrowth.h>
#endif

namespace BloombergLP {
namespace bdlma {

                        // =========================
                        // class SequentialAllocator
                        // =========================

class SequentialAllocator : public ManagedAllocator {
    // This class implements the 'ManagedAllocator' protocol to provide a fast
    // allocator that dispenses heterogeneous blocks of memory (of varying,
    // user-specified sizes) from a sequence of dynamically-allocated buffers.
    // Memory for the internal buffers is supplied by an (optional) allocator
    // supplied at construction; if no allocator is supplied, the currently
    // installed default allocator is used.  If an allocation exceeds the
    // remaining free memory space in the current buffer, the allocator
    // replenishes its internal buffer with new memory to satisfy the request.
    // This class is *exception* *neutral*: If memory cannot be allocated, the
    // behavior is defined by the (optional) allocator specified at
    // construction.

    // DATA
    SequentialPool d_sequentialPool;  // manager for allocated memory blocks

  private:
    // NOT IMPLEMENTED
    SequentialAllocator(const SequentialAllocator&);
    SequentialAllocator& operator=(const SequentialAllocator&);

  public:
    // CREATORS
    explicit
    SequentialAllocator(bslma::Allocator *basicAllocator = 0);
    explicit
    SequentialAllocator(bsls::BlockGrowth::Strategy  growthStrategy,
                        bslma::Allocator            *basicAllocator = 0);
    explicit
    SequentialAllocator(bsls::Alignment::Strategy  alignmentStrategy,
                        bslma::Allocator          *basicAllocator = 0);
    SequentialAllocator(bsls::BlockGrowth::Strategy  growthStrategy,
                        bsls::Alignment::Strategy    alignmentStrategy,
                        bslma::Allocator            *basicAllocator = 0);
        // Create a sequential allocator for allocating memory blocks from a
        // sequence of dynamically-allocated buffers.  Optionally specify a
        // 'basicAllocator' used to supply memory for the dynamically-allocated
        // buffers.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  Optionally specify a 'growthStrategy' used to
        // control buffer growth.  If no 'growthStrategy' is specified,
        // geometric growth is used.  Optionally specify an 'alignmentStrategy'
        // used to control alignment of allocated memory blocks.  If no
        // 'alignmentStrategy' is specified, natural alignment is used.  Note
        // that no limit is imposed on the size of the internal buffers when
        // geometric growth is used.

    explicit
    SequentialAllocator(int initialSize, bslma::Allocator *basicAllocator = 0);
    SequentialAllocator(int                          initialSize,
                        bsls::BlockGrowth::Strategy  growthStrategy,
                        bslma::Allocator            *basicAllocator = 0);
    SequentialAllocator(int                        initialSize,
                        bsls::Alignment::Strategy  alignmentStrategy,
                        bslma::Allocator          *basicAllocator = 0);
    SequentialAllocator(int                          initialSize,
                        bsls::BlockGrowth::Strategy  growthStrategy,
                        bsls::Alignment::Strategy    alignmentStrategy,
                        bslma::Allocator            *basicAllocator = 0);
        // Create a sequential allocator for allocating memory blocks from a
        // sequence of dynamically-allocated buffers, of which the initial
        // buffer has the specified 'initialSize' (in bytes).  Optionally
        // specify a 'basicAllocator' used to supply memory for the
        // dynamically-allocated buffers.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.  Optionally specify a
        // 'growthStrategy' used to control buffer growth.  If no
        // 'growthStrategy' is specified, geometric growth is used.  Optionally
        // specify an 'alignmentStrategy' used to control alignment of
        // allocated memory blocks.  If no 'alignmentStrategy' is specified,
        // natural alignment is used.  An implementation-defined value is used
        // as the initial size of the internal buffer.  The behavior is
        // undefined unless '0 < initialSize'.  Note that no limit is imposed
        // on the size of the internal buffers when geometric growth is used.
        // Also note that when constant growth is used, the size of the
        // internal buffers will always be the same as the
        // implementation-defined value.

    SequentialAllocator(int               initialSize,
                        int               maxBufferSize,
                        bslma::Allocator *basicAllocator = 0);
    SequentialAllocator(int                          initialSize,
                        int                          maxBufferSize,
                        bsls::BlockGrowth::Strategy  growthStrategy,
                        bslma::Allocator            *basicAllocator = 0);
    SequentialAllocator(int                        initialSize,
                        int                        maxBufferSize,
                        bsls::Alignment::Strategy  alignmentStrategy,
                        bslma::Allocator          *basicAllocator = 0);
    SequentialAllocator(int                          initialSize,
                        int                          maxBufferSize,
                        bsls::BlockGrowth::Strategy  growthStrategy,
                        bsls::Alignment::Strategy    alignmentStrategy,
                        bslma::Allocator            *basicAllocator = 0);
        // Create a sequential allocator for allocating memory blocks from a
        // sequence of dynamically-allocated buffers, of which the initial
        // buffer has the specified 'initialSize' (in bytes), and the buffer
        // growth is limited to the specified 'maxBufferSize'.  Optionally
        // specify a 'basicAllocator' used to supply memory for the
        // dynamically-allocated buffers.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.  Optionally specify a
        // 'growthStrategy' used to control buffer growth.  If no
        // 'growthStrategy' is specified, geometric growth is used.  Optionally
        // specify an 'alignmentStrategy' used to control alignment of
        // allocated memory blocks.  If no 'alignmentStrategy' is specified,
        // natural alignment is used.  The behavior is undefined unless
        // '0 < initialSize' and 'initialSize <= maxBufferSize'.  Note that
        // when constant growth is used, the size of the internal buffers will
        // always be the same as 'initialSize'.

    virtual ~SequentialAllocator();
        // Destroy this sequential allocator.  All memory allocated from this
        // allocator is released.

    // MANIPULATORS
    virtual void *allocate(size_type size);
        // Return the address of a contiguous block of memory of the specified
        // 'size' (in bytes) according to the alignment strategy specified at
        // construction.  If 'size' is 0, no memory is allocated and 0 is
        // returned.  If the allocation request exceeds the remaining free
        // memory space in the current internal buffer, use the allocator
        // supplied at construction to allocate a new internal buffer, then
        // allocate memory from the new buffer.

    void *allocateAndExpand(size_type *size);
        // Return the address of a contiguous block of memory of at least the
        // specified '*size' (in bytes), and load the actual amount of memory
        // allocated into '*size'.  If '*size' is 0, return 0 with no effect.
        // If the allocation request exceeds the remaining free memory space in
        // the current internal buffer, use the allocator supplied at
        // construction to allocate a new internal buffer, then allocate memory
        // from the new buffer.

    virtual void deallocate(void *address);
        // This method has no effect on the memory block at the specified
        // 'address' as all memory allocated by this allocator is managed.  The
        // behavior is undefined unless 'address' is 0, or was allocated by
        // this allocator and has not already been deallocated.

    virtual void release();
        // Release all memory allocated through this allocator.  The allocator
        // is reset to its default constructed state, retaining the alignment
        // and growth strategy supplied at construction (if any) after this
        // call.

    void reserveCapacity(int numBytes);
        // Reserve sufficient memory to satisfy allocation requests for at
        // least the specified 'numBytes' without replenishment (i.e., without
        // dynamic allocation).  If 'numBytes' is 0, no memory is reserved.
        // This method ignores 'maxBufferSize' even if it is supplied at
        // construction.  The behavior is undefined unless '0 <= numBytes'.
        // Note that, due to alignment effects, it is possible that not all
        // 'numBytes' of memory will be used for allocation before triggering
        // dynamic allocation.

    int truncate(void *address, int originalSize, int newSize);
        // Reduce the amount of memory allocated at the specified 'address' of
        // the specified 'originalSize' (in bytes) to the specified 'newSize'.
        // Return 'newSize' after truncating, or 'originalSize' if the memory
        // at 'address' cannot be truncated.  This method can only 'truncate'
        // the memory block returned by the most recent 'allocate' request from
        // this allocator, and otherwise has no effect.  The behavior is
        // undefined unless the memory at 'address' was originally allocated by
        // this allocator, the size of the memory block at 'address' is
        // 'originalSize', 'newSize <= originalSize', '0 <= newSize', and
        // 'release' was not called after allocating the memory block at
        // 'address'.
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                        // -------------------------
                        // class SequentialAllocator
                        // -------------------------

// CREATORS
inline
SequentialAllocator::
SequentialAllocator(bslma::Allocator *basicAllocator)
: d_sequentialPool(basicAllocator)
{
}

inline
SequentialAllocator::
SequentialAllocator(bsls::BlockGrowth::Strategy  growthStrategy,
                    bslma::Allocator            *basicAllocator)
: d_sequentialPool(growthStrategy, basicAllocator)
{
}

inline
SequentialAllocator::
SequentialAllocator(bsls::Alignment::Strategy  alignmentStrategy,
                    bslma::Allocator          *basicAllocator)
: d_sequentialPool(alignmentStrategy, basicAllocator)
{
}

inline
SequentialAllocator::
SequentialAllocator(bsls::BlockGrowth::Strategy  growthStrategy,
                    bsls::Alignment::Strategy    alignmentStrategy,
                    bslma::Allocator            *basicAllocator)
: d_sequentialPool(growthStrategy, alignmentStrategy, basicAllocator)
{
}

inline
SequentialAllocator::
SequentialAllocator(int               initialSize,
                    bslma::Allocator *basicAllocator)
: d_sequentialPool(initialSize, basicAllocator)
{
    BSLS_ASSERT_SAFE(0 < initialSize);
}

inline
SequentialAllocator::
SequentialAllocator(int                          initialSize,
                    bsls::BlockGrowth::Strategy  growthStrategy,
                    bslma::Allocator            *basicAllocator)
: d_sequentialPool(initialSize, growthStrategy, basicAllocator)
{
    BSLS_ASSERT_SAFE(0 < initialSize);
}

inline
SequentialAllocator::
SequentialAllocator(int                        initialSize,
                    bsls::Alignment::Strategy  alignmentStrategy,
                    bslma::Allocator          *basicAllocator)
: d_sequentialPool(initialSize, alignmentStrategy, basicAllocator)
{
    BSLS_ASSERT_SAFE(0 < initialSize);
}

inline
SequentialAllocator::
SequentialAllocator(int                          initialSize,
                    bsls::BlockGrowth::Strategy  growthStrategy,
                    bsls::Alignment::Strategy    alignmentStrategy,
                    bslma::Allocator            *basicAllocator)
: d_sequentialPool(initialSize,
                   growthStrategy,
                   alignmentStrategy,
                   basicAllocator)
{
    BSLS_ASSERT_SAFE(0 < initialSize);
}

inline
SequentialAllocator::
SequentialAllocator(int               initialSize,
                    int               maxBufferSize,
                    bslma::Allocator *basicAllocator)
: d_sequentialPool(initialSize, maxBufferSize, basicAllocator)
{
    BSLS_ASSERT_SAFE(0 < initialSize);
    BSLS_ASSERT_SAFE(initialSize <= maxBufferSize);
}

inline
SequentialAllocator::
SequentialAllocator(int                          initialSize,
                    int                          maxBufferSize,
                    bsls::BlockGrowth::Strategy  growthStrategy,
                    bslma::Allocator            *basicAllocator)
: d_sequentialPool(initialSize, maxBufferSize, growthStrategy, basicAllocator)
{
    BSLS_ASSERT_SAFE(0 < initialSize);
    BSLS_ASSERT_SAFE(initialSize <= maxBufferSize);
}

inline
SequentialAllocator::
SequentialAllocator(int                        initialSize,
                    int                        maxBufferSize,
                    bsls::Alignment::Strategy  alignmentStrategy,
                    bslma::Allocator          *basicAllocator)
: d_sequentialPool(initialSize,
                   maxBufferSize,
                   alignmentStrategy,
                   basicAllocator)
{
    BSLS_ASSERT_SAFE(0 < initialSize);
    BSLS_ASSERT_SAFE(initialSize <= maxBufferSize);
}

inline
SequentialAllocator::
SequentialAllocator(int                          initialSize,
                    int                          maxBufferSize,
                    bsls::BlockGrowth::Strategy  growthStrategy,
                    bsls::Alignment::Strategy    alignmentStrategy,
                    bslma::Allocator            *basicAllocator)
: d_sequentialPool(initialSize,
                   maxBufferSize,
                   growthStrategy,
                   alignmentStrategy,
                   basicAllocator)
{
    BSLS_ASSERT_SAFE(0 < initialSize);
    BSLS_ASSERT_SAFE(initialSize <= maxBufferSize);
}

// MANIPULATORS
inline
void SequentialAllocator::deallocate(void *)
{
}

inline
void SequentialAllocator::release()
{
    d_sequentialPool.release();
}

inline
int SequentialAllocator::truncate(void *address, int originalSize, int newSize)
{
    return d_sequentialPool.truncate(address, originalSize, newSize);
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
