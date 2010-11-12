// bdema_sequentialallocator.h                                        -*-C++-*-
#ifndef INCLUDED_BDEMA_SEQUENTIALALLOCATOR
#define INCLUDED_BDEMA_SEQUENTIALALLOCATOR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a managed allocator using dynamically-allocated buffers.
//
//@CLASSES:
//   bdema_SequentialAllocator: managed allocator using dynamic buffer
//
//@SEE_ALSO: bdema_infrequentdeleteblocklist, bdema_sequentialpool
//
//@AUTHOR: Arthur Chiu (achiu21)
//
//@DESCRIPTION: This component provides a concrete mechanism,
// 'bdema_SequentialAllocator', that implements the 'bslma_ManagedAllocator'
// protocol and efficiently allocates heterogeneous memory blocks (of varying,
// user-specified sizes) from a dynamically-allocated internal buffer:
//..
//   ,-------------------------.
//  ( bdema_SequentialAllocator )
//   `-------------------------'
//                |         ctor/dtor
//                |         allocateAndExpand
//                |         reserveCapacity
//                |         truncate
//                V
//    ,----------------------.
//   ( bslma_ManagedAllocator )
//    `----------------------'
//                |        release
//                V
//       ,----------------.
//      (  bslma_Allocator )
//       `----------------'
//                       allocate
//                       deallocate
//..
// If an allocation request exceeds the remaining free memory space in the
// internal buffer, the allocator either replenishes its buffer with new memory
// to satisfy the request, or returns a separate memory block, depending on
// whether the request size exceeds an optionally-specified maximum buffer
// size.  The 'release' method releases all memory allocated through the
// allocator, as does the destructor.  Note that individually allocated memory
// blocks cannot be separately deallocated.
//
// The main difference between a 'bdema_SequentialAllocator' and a
// 'bdema_SequentialPool' is that, very often, a 'bdema_SequentialAllocator' is
// managed through a 'bslma_Allocator' pointer.  Hence, every call to the
// 'allocate' method invokes a virtual function call, which is slower than
// invoking the non-virtual 'allocate' method on a 'bdema_SequentialPool'.
// However, since 'bslma_Allocator *' is widely used across BDE interfaces,
// 'bdema_SequentialAllocator' is more general purposed than a
// 'bdema_SequentialPool'.
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
// follow maximum alignment or natural alignment.  If 'alignmentStrategy' is
// not specified, natural alignment is used.  See 'bsls_alignment' for more
// details.
//
///Usage
///-----
// Allocators are often supplied, at construction, to objects requiring
// dynamically-allocated memory.  For example, consider the following
// 'my_DoubleStack' class whose constructor takes a 'bslma_Allocator':
//..
//  // my_doublestack.h
//  // ...
//
//  class bslma_Allocator;
//
//  class my_DoubleStack {
//      // This class implements a stack that stores 'double' values.
//
//      // DATA
//      double          *d_stack_p;      // dynamically-allocated array
//      int              d_size;         // physical capacity of stack
//      int              d_length;       // next available index in stack
//      bslma_Allocator *d_allocator_p;  // memory allocator (held, not owned)
//
//    private:
//      // PRIVATE MANIPULATORS
//      void increaseSize();
//          // Increase the capacity of this stack by at least one element.
//
//    public:
//      // CREATORS
//      my_DoubleStack(bslma_Allocator *basicAllocator = 0);
//          // Create a stack that stores 'double'.  Optionally specify
//          // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0,
//          // the currently installed default allocator is used.
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
//          increaseSize();
//      }
//      d_stack_p[d_length++] = value;
//  }
//
//  // ...
//
//  // my_doublestack.cpp
//  #include <my_doublestack.h>
//
//  #include <bslma_allocator.h>
//  #include <bslma_default.h>
//
//  // PRIVATE MANIPULATORS
//  void my_DoubleStack::increaseSize()
//  {
//      // Implementation elided.
//      // ...
//  }
//
//  // CREATORS
//  my_DoubleStack::my_DoubleStack(bslma_Allocator *basicAllocator)
//  : d_size(1)
//  , d_length(0)
//  , d_allocator_p(bslma_Default::allocator(basicAllocator))
//  {
//      d_stack_p = static_cast<double *>(
//                        d_allocator_p->allocate(d_size * sizeof *d_stack_p));
//  }
//..
// Note that, when the allocator passed in is a 'bdema_SequentialAllocator',
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
// In 'main', users can create a 'bdema_SequentialAllocator' and pass it to the
// constructor of 'my_DoubleStack':
//..
//  int main()
//  {
//      bdema_SequentialAllocator sequentialAlloc;
//      my_DoubleStack dstack(&sequentialAlloc);
//      // ...
//  }
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEMA_SEQUENTIALPOOL
#include <bdema_sequentialpool.h>
#endif

#ifndef INCLUDED_BSLMA_MANAGEDALLOCATOR
#include <bslma_managedallocator.h>
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

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>
#endif

namespace BloombergLP {

class bslma_Allocator;

                      // ===============================
                      // class bdema_SequentialAllocator
                      // ===============================

class bdema_SequentialAllocator : public bslma_ManagedAllocator {
    // This class implements the 'bslma_ManagedAllocator' protocol to provide
    // a fast allocator that dispenses heterogeneous blocks of memory (of
    // varying, user-specified sizes) from a sequence of dynamically-allocated
    // buffers.  Memory for the internal buffers is supplied by an (optional)
    // allocator supplied at construction; if no allocator is supplied, the
    // currently installed default allocator is used.  If an allocation exceeds
    // the remaining free memory space in the current buffer, the allocator
    // replenishes its internal buffer with new memory to satisfy the request.
    // This class is *exception* *neutral*: If memory cannot be allocated, the
    // behavior is defined by the (optional) allocator specified at
    // construction.

    // DATA
    bdema_SequentialPool d_sequentialPool;  // manager for allocated memory
                                            // blocks

    // NOT IMPLEMENTED
    bdema_SequentialAllocator(const bdema_SequentialAllocator&);
    bdema_SequentialAllocator& operator=(const bdema_SequentialAllocator&);

  public:
    // CREATORS
    explicit bdema_SequentialAllocator(
                              bslma_Allocator             *basicAllocator = 0);
    explicit bdema_SequentialAllocator(
                              bsls_BlockGrowth::Strategy   growthStrategy,
                              bslma_Allocator             *basicAllocator = 0);
    explicit bdema_SequentialAllocator(
                              bsls_Alignment::Strategy     alignmentStrategy,
                              bslma_Allocator             *basicAllocator = 0);
    bdema_SequentialAllocator(bsls_BlockGrowth::Strategy   growthStrategy,
                              bsls_Alignment::Strategy     alignmentStrategy,
                              bslma_Allocator             *basicAllocator = 0);
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

    explicit bdema_SequentialAllocator(int                initialSize,
                                       bslma_Allocator   *basicAllocator = 0);
    bdema_SequentialAllocator(int                         initialSize,
                              bsls_BlockGrowth::Strategy  growthStrategy,
                              bslma_Allocator            *basicAllocator = 0);
    bdema_SequentialAllocator(int                         initialSize,
                              bsls_Alignment::Strategy    alignmentStrategy,
                              bslma_Allocator            *basicAllocator = 0);
    bdema_SequentialAllocator(int                         initialSize,
                              bsls_BlockGrowth::Strategy  growthStrategy,
                              bsls_Alignment::Strategy    alignmentStrategy,
                              bslma_Allocator            *basicAllocator = 0);
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


    bdema_SequentialAllocator(int                         initialSize,
                              int                         maxBufferSize,
                              bslma_Allocator            *basicAllocator = 0);
    bdema_SequentialAllocator(int                         initialSize,
                              int                         maxBufferSize,
                              bsls_BlockGrowth::Strategy  growthStrategy,
                              bslma_Allocator            *basicAllocator = 0);
    bdema_SequentialAllocator(int                         initialSize,
                              int                         maxBufferSize,
                              bsls_Alignment::Strategy    alignmentStrategy,
                              bslma_Allocator            *basicAllocator = 0);
    bdema_SequentialAllocator(int                         initialSize,
                              int                         maxBufferSize,
                              bsls_BlockGrowth::Strategy  growthStrategy,
                              bsls_Alignment::Strategy    alignmentStrategy,
                              bslma_Allocator            *basicAllocator = 0);
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
        // always be the same as 'initialSize'.  Also note that when constant
        // growth is used, the size of the internal buffers will always be the
        // same as 'initialSize'.

    virtual ~bdema_SequentialAllocator();
        // Destroy this sequential allocator.  All memory allocated from this
        // allocator is released.

    // MANIPULATORS
    virtual void *allocate(bsls_PlatformUtil::size_type size);
        // Return the address of a contiguous block of memory of the specified
        // 'size' (in bytes).  If 'size' is 0, no memory is allocated and 0 is
        // returned.  If the allocation request exceeds the remaining free
        // memory space in the current internal buffer, use the allocator
        // supplied at construction to allocate a new internal buffer, then
        // allocate memory from the new buffer.  The behavior is undefined
        // unless '0 <= size'.

    void *allocateAndExpand(bsls_PlatformUtil::size_type *size);
        // Return the address of a contiguous block of memory of at least the
        // specified '*size' (in bytes), and load the actual amount of memory
        // allocated into '*size'.  If '*size' is 0, return 0 with no effect.
        // If the allocation request exceeds the remaining free memory space in
        // the current internal buffer, use the allocator supplied at
        // construction to allocate a new internal buffer, then allocate memory
        // from the new buffer.  The behavior is undefined unless '0 <= *size'.

    virtual void deallocate(void *address);
        // This method has no effect on the memory block at the specified
        // 'address' as all memory allocated by this allocator is managed.  The
        // behavior is undefined unless 'address' was allocated by this
        // allocator, and has not already been deallocated.

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
        // Reduce the amount of memory allocated at the specified 'address'
        // of the specified 'originalSize' (in bytes) to the specified
        // 'newSize'.  Return 'newSize' after truncating, or 'originalSize' if
        // 'address' cannot be truncated.  This method can only 'truncate' the
        // memory block returned by the most recent 'allocate' request from
        // this allocator, and otherwise has no effect.  The behavior is
        // undefined unless the memory at 'address' was originally allocated by
        // this allocator, the size of the memory block at 'address' is
        // 'originalSize', 'newSize <= originalSize', '0 <= newSize', and
        // 'release' was not called after allocating the memory block at
        // 'address'.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                      // -------------------------------
                      // class bdema_SequentialAllocator
                      // -------------------------------

// CREATORS
inline
bdema_SequentialAllocator::
bdema_SequentialAllocator(bslma_Allocator *basicAllocator)
: d_sequentialPool(basicAllocator)
{
}

inline
bdema_SequentialAllocator::
bdema_SequentialAllocator(bsls_BlockGrowth::Strategy  growthStrategy,
                          bslma_Allocator            *basicAllocator)
: d_sequentialPool(growthStrategy, basicAllocator)
{
}

inline
bdema_SequentialAllocator::
bdema_SequentialAllocator(bsls_Alignment::Strategy  alignmentStrategy,
                          bslma_Allocator          *basicAllocator)
: d_sequentialPool(alignmentStrategy, basicAllocator)
{
}

inline
bdema_SequentialAllocator::
bdema_SequentialAllocator(bsls_BlockGrowth::Strategy  growthStrategy,
                          bsls_Alignment::Strategy    alignmentStrategy,
                          bslma_Allocator            *basicAllocator)
: d_sequentialPool(growthStrategy, alignmentStrategy, basicAllocator)
{
}

inline
bdema_SequentialAllocator::
bdema_SequentialAllocator(int              initialSize,
                          bslma_Allocator *basicAllocator)
: d_sequentialPool(initialSize, basicAllocator)
{
    BSLS_ASSERT_SAFE(0 < initialSize);
}

inline
bdema_SequentialAllocator::
bdema_SequentialAllocator(int                         initialSize,
                          bsls_BlockGrowth::Strategy  growthStrategy,
                          bslma_Allocator            *basicAllocator)
: d_sequentialPool(initialSize, growthStrategy, basicAllocator)
{
    BSLS_ASSERT_SAFE(0 < initialSize);
}

inline
bdema_SequentialAllocator::
bdema_SequentialAllocator(int                       initialSize,
                          bsls_Alignment::Strategy  alignmentStrategy,
                          bslma_Allocator          *basicAllocator)
: d_sequentialPool(initialSize, alignmentStrategy, basicAllocator)
{
    BSLS_ASSERT_SAFE(0 < initialSize);
}

inline
bdema_SequentialAllocator::
bdema_SequentialAllocator(int                         initialSize,
                          bsls_BlockGrowth::Strategy  growthStrategy,
                          bsls_Alignment::Strategy    alignmentStrategy,
                          bslma_Allocator            *basicAllocator)
: d_sequentialPool(initialSize,
                   growthStrategy,
                   alignmentStrategy,
                   basicAllocator)
{
    BSLS_ASSERT_SAFE(0 < initialSize);
}

inline
bdema_SequentialAllocator::
bdema_SequentialAllocator(int              initialSize,
                          int              maxBufferSize,
                          bslma_Allocator *basicAllocator)
: d_sequentialPool(initialSize, maxBufferSize, basicAllocator)
{
    BSLS_ASSERT_SAFE(0 < initialSize);
    BSLS_ASSERT_SAFE(initialSize <= maxBufferSize);
}

inline
bdema_SequentialAllocator::
bdema_SequentialAllocator(int                         initialSize,
                          int                         maxBufferSize,
                          bsls_BlockGrowth::Strategy  growthStrategy,
                          bslma_Allocator            *basicAllocator)
: d_sequentialPool(initialSize, maxBufferSize, growthStrategy, basicAllocator)
{
    BSLS_ASSERT_SAFE(0 < initialSize);
    BSLS_ASSERT_SAFE(initialSize <= maxBufferSize);
}

inline
bdema_SequentialAllocator::
bdema_SequentialAllocator(int                       initialSize,
                          int                       maxBufferSize,
                          bsls_Alignment::Strategy  alignmentStrategy,
                          bslma_Allocator          *basicAllocator)
: d_sequentialPool(initialSize,
                   maxBufferSize,
                   alignmentStrategy,
                   basicAllocator)
{
    BSLS_ASSERT_SAFE(0 < initialSize);
    BSLS_ASSERT_SAFE(initialSize <= maxBufferSize);
}

inline
bdema_SequentialAllocator::
bdema_SequentialAllocator(int                         initialSize,
                          int                         maxBufferSize,
                          bsls_BlockGrowth::Strategy  growthStrategy,
                          bsls_Alignment::Strategy    alignmentStrategy,
                          bslma_Allocator            *basicAllocator)
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
void bdema_SequentialAllocator::deallocate(void *)
{
}

inline
void bdema_SequentialAllocator::release()
{
    d_sequentialPool.release();
}

inline
int bdema_SequentialAllocator::truncate(void *address,
                                        int   originalSize,
                                        int   newSize)
{
    return d_sequentialPool.truncate(address, originalSize, newSize);
}

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
