// bdlma_smallsequentialallocator.h                                   -*-C++-*-
#ifndef INCLUDED_BDLMA_SMALLSEQUENTIALALLOCATOR
#define INCLUDED_BDLMA_SMALLSEQUENTIALALLOCATOR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a managed allocator using dynamically-allocated buffers.
//
//@CLASSES:
//   bdlma::SmallSequentialAllocator: managed allocator using dynamic buffers
//
//@SEE_ALSO: bdlma_smallsequentialpool, bdlma_sequentialallocator
//
//@DESCRIPTION: This component provides a concrete mechanism,
// 'bdlma::SmallSequentialAllocator', that implements the
// 'bdlma::ManagedAllocator' protocol and efficiently allocates heterogeneous
// memory blocks (of varying, user-specified sizes) from a
// dynamically-allocated buffers:
//..
//   ,-------------------------------.
//  ( bdlma::SmallSequentialAllocator )
//   `-------------------------------'
//                |         ctor/dtor
//                |         allocateAndExpand
//                |         reserveCapacity
//                |         rewind
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
// The class rates the appellation "small" because its footprint is
// significantly less than that of 'bdlma::SequentialAllocator', a similar
// class that uses a different policy for its 'rewind' method.  See
// {Comparison: 'bdlma::SequentialAllocator' and
// 'bdlma::SmallSequentialAllocator'}.
//
// If an allocation request exceeds the remaining free memory space in the
// buffer, the allocator either replenishes its buffer with new memory to
// satisfy the request, or returns a separate memory block, depending on
// whether the request size exceeds an optionally-specified threshold.
// Allocations that exceed that threshold are deemed "large" blocks.  See
// {Optional 'maxBufferSize' Parameter} and {Optional 'growthStrategy'
// Parameter} for details of specifying the "large" block threshold.  Note that
// "large" block requests are simply passed through to the pool's allocator and
// bypass the features that make allocations more efficient.  See
// {'bdlma_smallsequentialpool'|Example 1: Using 'bdlma::SmallSequentialPool'
// for Efficient Allocations}.
//
// The 'release' method releases all memory allocated through the allocator, as
// does the destructor.  The 'rewind' method releases all memory allocated
// through the allocator and returns to the underlying allocator *only* memory
// that was allocated outside of the typical buffer growth of the allocator
// (i.e., large blocks).  Note that individually allocated memory blocks cannot
// be separately deallocated.
//
// The 'release' method releases all memory allocated through the pool, as does
// the destructor.  Note that the 'deallocate' method is a no-op.
//
// The 'rewind' method releases all memory allocated through the pool *except*
// for the last non-"large" block, if any.  Using 'rewind' instead of 'release'
// can save on reallocations when a pool object is being reused.  See
// {'bdlma_smallsequentialpool'|'Example 3: Iterative Pool Reuse}.
//
///Comparison: 'bdlma::SmallSequentialAllocator' & 'bdlma::SmallSequentialPool'
///----------------------------------------------------------------------------
// The main difference between a 'bdlma::SmallSequentialAllocator' and a
// 'bdlma::SmallSequentialPool' is that, very often, a
// 'bdlma::SmallSequentialAllocator' is managed through a 'bslma::Allocator'
// pointer.  Hence, every call to the 'allocate' method invokes a virtual
// function call, which is slower than invoking the non-virtual 'allocate'
// method on a 'bdlma::SmallSequentialPool'.  However, since
// 'bslma::Allocator *' is widely used across BDE interfaces,
// 'bdlma::SmallSequentialAllocator' is more general purpose than a
// 'bdlma::SmallSequentialPool'.
//
///Optional 'initialSize' Parameter
///--------------------------------
// An optional 'initialSize' parameter can be supplied at construction to
// specify the initial size of the buffer.  If 'initialSize' is not supplied,
// an implementation-defined value is used for the initial size of the buffer.
//
///Optional 'maxBufferSize' Parameter
/// - - - - - - - - - - - - - - - - -
// If 'initialSize' is specified, an optional 'maxBufferSize' parameter can be
// supplied at construction to specify the maximum buffer size by geometric
// growth.  Requests in excess of 'maxBufferSize' by allocating (and returning)
// a "large" block of the requested size.  The behavior is undefined unless
// 'intialSize <= maxBufferSize'.  Note that 'reserveCapacity' always ensures
// that the requested number of bytes is available (allocating a new buffer if
// necessary) regardless of whether the size of the request exceeds
// 'maxBufferSize'.
//
// "Large" allocations are never candidates for preservation across invocations
// of 'rewind'.  Consequently, when pool objects are being reused (see
// {'bdlma_smallsequentialpool'|Example 3}), an unusually large allocation
// cannot be committed to the pool object for the remainder of its lifetime.
//
///Optional 'growthStrategy' Parameter
///-----------------------------------
// An optional 'growthStrategy' parameter can be supplied at construction to
// specify the growth rate of the dynamically-allocated buffers.  See
// {'bsls_blockgrowth'} for more details.  The buffers can grow either
// geometrically or remain constant in size.  If 'growthStrategy' is not
// specified, geometric growth is used.
//
// If the constant size growth strategy is chosen, additional buffers have the
// the specified 'initialSize' (see {Optional 'initialSize' Parameter}) or an
// implementation-defined value.  If an allocation request exceeds the growth
// size the policy switches to geometric growth for that request.  Allocation
// requests in excess of 'maxBufferSize' are satisfied by providing a single
// "large" block of the requested size.  The fallback from constant to
// geometric growth can be suppressed by constructing the pool with
// 'initialSize' equal to 'maxBufferSize'.
//
///Optional 'alignmentStrategy' Parameter
///--------------------------------------
// An optional 'alignmentStrategy' parameter can be supplied at construction to
// specify the memory alignment strategy.  Allocated memory blocks can either
// follow maximum alignment, natural alignment, or 1-byte alignment.  If
// 'alignmentStrategy' is not specified, natural alignment is used.  See
// 'bsls_alignment' for more details.
//
///Comparison: 'bdlma::SequentialAllocator' & 'bdlma::SmallSequentialAllocator'
///----------------------------------------------------------------------------
// The two classes, 'bdlma::SequentialAllocator' and
// 'bdlma::SmallSequentialAllocator', offer identical interfaces and nearly
// identical semantics.  The sole difference is that
// 'bslma::SequentialAllocator' class retains across calls to the 'rewind'
// method all memory blocks allocated under the growth strategy specified on
// construction whereas the much-smaller-footprint
// 'bdlma::SmallSequentialAllocator' class retains only the last memory block
// allocated under the growth strategy and releases the rest.  Note that the
// last block allocated is as large or larger than all previously allocated
// blocks.  Both classes release all allocated blocks that exceeded the growth
// strategy (i.e., "large" blocks).
//
///Usage
///-----
// This section illustrates intended use of this component.  Also see
// {'bdlma_smallsequentialpool'|Usage}.
//
///Example 1: Using a 'bdlma::SmallSequentialAllocator'
/// - - - - - - - - - - - - - - - - - - - - - - - - - -
// Allocators are often supplied, at construction, to objects requiring
// dynamically-allocated memory.  For example, consider the following
// 'xyza::DoubleStack' class whose constructor takes a 'bslma::Allocator *'.
//
// The header file (elided) is:
//..
//  // xyaz_doublestack.h
//
//  // ...
//
//  namespace xyza {
//                          // =================
//                          // class DoubleStack
//                          // =================
//
//  class DoubleStack {
//      // This class implements a stack that stores 'double' values.
//
//      // DATA
//      double           *d_stack_p;      // dynamically-allocated array
//      int               d_capacity;     // physical capacity of stack
//      int               d_length;       // next available index in stack
//      bslma::Allocator *d_allocator_p;  // memory allocator (held, not owned)
//
//    private:
//      // PRIVATE MANIPULATORS
//      void increaseCapacity();
//          // Increase the capacity of this stack by at least one element.
//
//      // NOT IMPLEMENTED
//      DoubleStack(const DoubleStack&)            BSLS_KEYWORD_DELETED;
//      DoubleStack& operator=(const DoubleStack&) BSLS_KEYWORD_DELETED;
//
//    public:
//      // CREATORS
//      explicit DoubleStack(bslma::Allocator *basicAllocator = 0);
//          // Create an empty stack that stores 'double' values.  Optionally
//          // specify a 'basicAllocator' used to supply memory.  If
//          // 'basicAllocator' is 0, the currently installed default allocator
//          // is used.
//
//      ~DoubleStack();
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
//  // ========================================================================
//  //                             INLINE DEFINITIONS
//  // ========================================================================
//
//                          // -----------------
//                          // class DoubleStack
//                          // -----------------
//
//  // MANIPULATORS
//  inline
//  void DoubleStack::push(double value)
//  {
//      if (d_capacity <= d_length) {
//          increaseCapacity();
//      }
//      d_stack_p[d_length++] = value;
//  }
//
//  // ...
//
//  }  // close package namespace
//..
// The implementation file (elided) is:
//..
//  // my_doublestack.cpp
//
//  namespace xyza {
//
//                          // -----------------
//                          // class DoubleStack
//                          // -----------------
//
//  // PRIVATE MANIPULATORS
//  void DoubleStack::increaseCapacity()
//  {
//      // Implementation elided.
//      // ...
//  }
//
//  // CREATORS
//  DoubleStack::DoubleStack(bslma::Allocator *basicAllocator)
//  : d_capacity(1)
//  , d_length(0)
//  , d_allocator_p(bslma::Default::allocator(basicAllocator))
//  {
//      d_stack_p = static_cast<double *>(
//                    d_allocator_p->allocate(d_capacity * sizeof *d_stack_p));
//  }
//
//  // ...
//..
// Note that this implementation set the initial capacity to 1 and
// pre-allocates space for one data value in the constructor.
//..
//  DoubleStack::~DoubleStack()
//  {
//      // CLASS INVARIANTS
//      assert(d_allocator_p);
//      assert(d_stack_p);
//      assert(0        <= d_length);
//      assert(d_length <= d_capacity);
//
//      d_allocator_p->deallocate(d_stack_p);
//  }
// Note that, when the allocator passed in is a
// 'bdlma::SmallSequentialAllocator', the 'deallocate' method is a no-op, and
// the memory is not actually reclaimed until the destruction of the allocator
// object.
//..
//
//  // ...
//
//  }  // close package namespace
//..
// In 'main', users can create a 'bdlma::SmallSequentialAllocator' and pass it
// to the constructor of 'xyza::DoubleStack':
//..
//  int main()
//      // Run a program that uses a 'bdlma::SmallSequentialAllocator' object.
//  {
//      bdlma::SmallSequentialAllocator smallSequentialAlloc;
//      xyza::DoubleStack               dstack(&smallSequentialAlloc);
//
//      // ....
//
//      return 0;
//  }
//..

#include <bdlscm_version.h>

#include <bdlma_managedallocator.h>
#include <bdlma_smallsequentialpool.h>

#include <bslma_allocator.h>

#include <bsls_alignment.h>
#include <bsls_assert.h>
#include <bsls_blockgrowth.h>
#include <bsls_keyword.h>
#include <bsls_performancehint.h>
#include <bsls_review.h>
#include <bsls_types.h>

#include <bsl_cstddef.h>  // 'bsl::size_t'

namespace BloombergLP {
namespace bdlma {

                        // ==============================
                        // class SmallSequentialAllocator
                        // ==============================

class SmallSequentialAllocator : public ManagedAllocator {
    // This class implements the 'ManagedAllocator' protocol to provide a fast
    // allocator that dispenses heterogeneous blocks of memory (of varying,
    // user-specified sizes) from a sequence of dynamically-allocated buffers.
    // Memory for the buffers is supplied by an (optional) allocator supplied
    // at construction; if no allocator is supplied, the currently installed
    // default allocator is used.  If an allocation exceeds the remaining free
    // memory space in the current buffer, the allocator replenishes its buffer
    // with new memory to satisfy the request.  This class is *exception*
    // *neutral*: If memory cannot be allocated, the behavior is defined by the
    // (optional) allocator specified at construction.

    // DATA
    SmallSequentialPool d_sequentialPool;  // manager for allocated memory
                                           // blocks

  private:
    // NOT IMPLEMENTED
    SmallSequentialAllocator(const SmallSequentialAllocator&)
                                                         BSLS_KEYWORD_DELETED;
    SmallSequentialAllocator& operator=(const SmallSequentialAllocator&)
                                                         BSLS_KEYWORD_DELETED;

  public:
    // CREATORS
    explicit
    SmallSequentialAllocator(bslma::Allocator            *basicAllocator = 0);
    explicit
    SmallSequentialAllocator(bsls::BlockGrowth::Strategy  growthStrategy,
                             bslma::Allocator            *basicAllocator = 0);
    explicit
    SmallSequentialAllocator(bsls::Alignment::Strategy    alignmentStrategy,
                             bslma::Allocator            *basicAllocator = 0);
    SmallSequentialAllocator(bsls::BlockGrowth::Strategy  growthStrategy,
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
        // that no limit is imposed on the size of the buffers when geometric
        // growth is used.  Also note that when constant growth is used, the
        // size of the buffers will always be the same as the
        // implementation-defined value.

    explicit
    SmallSequentialAllocator(int                          initialSize);
    explicit
    SmallSequentialAllocator(bsl::size_t                  initialSize,
                             bslma::Allocator            *basicAllocator = 0);
    SmallSequentialAllocator(bsl::size_t                  initialSize,
                             bsls::BlockGrowth::Strategy  growthStrategy,
                             bslma::Allocator            *basicAllocator = 0);
    SmallSequentialAllocator(bsl::size_t                  initialSize,
                             bsls::Alignment::Strategy    alignmentStrategy,
                             bslma::Allocator            *basicAllocator = 0);
    SmallSequentialAllocator(bsl::size_t                  initialSize,
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
        // natural alignment is used.  By specifying an 'initialSize', the
        // construction of this sequential allocator incurs a memory
        // allocation.  The behavior is undefined unless '0 < initialSize'.
        // Note that no limit is imposed on the size of the buffers when
        // geometric growth is used.  Also note that when constant growth is
        // used, the size of the buffers will always be the same as
        // 'initialSize'.  Finally note that
        // 'SmallSequentialAllocator(int initialSize' is provided to avoid
        // ambiguous definitions.

    SmallSequentialAllocator(bsl::size_t                  initialSize,
                             bsl::size_t                  maxBufferSize,
                             bslma::Allocator            *basicAllocator = 0);
    SmallSequentialAllocator(bsl::size_t                  initialSize,
                             bsl::size_t                  maxBufferSize,
                             bsls::BlockGrowth::Strategy  growthStrategy,
                             bslma::Allocator            *basicAllocator = 0);
    SmallSequentialAllocator(bsl::size_t                  initialSize,
                             bsl::size_t                  maxBufferSize,
                             bsls::Alignment::Strategy    alignmentStrategy,
                             bslma::Allocator            *basicAllocator = 0);
    SmallSequentialAllocator(bsl::size_t                  initialSize,
                             bsl::size_t                  maxBufferSize,
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
        // when constant growth is used, the size of the buffers will always be
        // the same as 'initialSize'.

    virtual ~SmallSequentialAllocator() BSLS_KEYWORD_OVERRIDE;
        // Destroy this sequential allocator.  All memory allocated from this
        // allocator is released.

    // MANIPULATORS
    virtual void *allocate(bsl::size_t size) BSLS_KEYWORD_OVERRIDE;
        // Return the address of a contiguous block of memory of the specified
        // 'size' (in bytes) according to the alignment strategy specified at
        // construction.  If 'size' is 0, no memory is allocated and 0 is
        // returned.  If the allocation request exceeds the remaining free
        // memory space in the buffer, use the allocator supplied at
        // construction to allocate a new buffer, then allocate memory from the
        // new buffer.

    void *allocateAndExpand(bsl::size_t *size);
        // Return the address of a contiguous block of memory of at least the
        // specified '*size' (in bytes), and load the actual amount of memory
        // allocated in '*size'.  If '*size' is 0, return 0 with no effect.  If
        // the allocation request exceeds the remaining free memory space in
        // the current buffer, use the allocator supplied at construction to
        // allocate a new buffer, then allocate memory from the new buffer.

    virtual void deallocate(void *address) BSLS_KEYWORD_OVERRIDE;
        // This method has no effect on the memory block at the specified
        // 'address' as all memory allocated by this allocator is managed.  The
        // behavior is undefined unless 'address' is 0, or was allocated by
        // this allocator and has not already been deallocated.

    virtual void release() BSLS_KEYWORD_OVERRIDE;
        // Release all memory allocated through this allocator and return to
        // the underlying allocator *all* memory.  The allocator is reset to
        // its default-constructed state, retaining the alignment and growth
        // strategies, and the initial and maximum buffer sizes in effect
        // following construction.  The effect of subsequently -- to this
        // invocation of 'release' -- using a pointer obtained from this object
        // prior to this call to 'release' is undefined.

    void reserveCapacity(bsl::size_t numBytes);
        // Reserve sufficient memory to satisfy allocation requests for at
        // least the specified 'numBytes' without replenishment (i.e., without
        // dynamic allocation).  If 'numBytes' is 0, no memory is reserved.
        // This method ignores 'maxBufferSize' even if it is supplied at
        // construction.  Note that, due to alignment effects, it is possible
        // that not all 'numBytes' of memory will be used for allocation before
        // triggering dynamic allocation.

    void rewind();
        // Release all memory allocated through this pool and return to the
        // underlying allocator all but the last block, if any, allocated by
        // the growth strategy specified on construction *and* all memory
        // allocated outside of that strategy (i.e., large blocks).  The
        // retained memory, if any, will be used to satisfy subsequent
        // allocations.  The effect of subsequently using a pointer obtained
        // from this object prior to this call to 'rewind' is undefined.  Note
        // that the retained memory block is as large or larger than any other
        // block obtained by the growth strategy.

    bsl::size_t truncate(void        *address,
                         bsl::size_t  originalSize,
                         bsl::size_t  newSize);
        // Reduce the amount of memory allocated at the specified 'address' of
        // the specified 'originalSize' (in bytes) to the specified 'newSize'.
        // Return 'newSize' after truncating, or 'originalSize' if the memory
        // block at 'address' cannot be truncated.  This method can only
        // 'truncate' the memory block returned by the most recent 'allocate'
        // request from this allocator, and otherwise has no effect.  The
        // behavior is undefined unless the memory block at 'address' was
        // originally allocated by this allocator, the size of the memory block
        // at 'address' is 'originalSize', 'newSize <= originalSize', and
        // 'release' was not called after allocating the memory block at
        // 'address'.
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                        // ------------------------------
                        // class SmallSequentialAllocator
                        // ------------------------------

// CREATORS
inline
SmallSequentialAllocator::
SmallSequentialAllocator(bslma::Allocator *basicAllocator)
: d_sequentialPool(basicAllocator)
{
}

inline
SmallSequentialAllocator::
SmallSequentialAllocator(bsls::BlockGrowth::Strategy  growthStrategy,
                         bslma::Allocator            *basicAllocator)
: d_sequentialPool(growthStrategy, basicAllocator)
{
}

inline
SmallSequentialAllocator::
SmallSequentialAllocator(bsls::Alignment::Strategy  alignmentStrategy,
                         bslma::Allocator          *basicAllocator)
: d_sequentialPool(alignmentStrategy, basicAllocator)
{
}

inline
SmallSequentialAllocator::
SmallSequentialAllocator(bsls::BlockGrowth::Strategy  growthStrategy,
                         bsls::Alignment::Strategy    alignmentStrategy,
                         bslma::Allocator            *basicAllocator)
: d_sequentialPool(growthStrategy, alignmentStrategy, basicAllocator)
{
}

inline
SmallSequentialAllocator::
SmallSequentialAllocator(int initialSize)
: d_sequentialPool(initialSize)
{
    BSLS_ASSERT(0 < initialSize);
}

inline
SmallSequentialAllocator::
SmallSequentialAllocator(bsl::size_t             initialSize,
                         bslma::Allocator       *basicAllocator)
: d_sequentialPool(initialSize, basicAllocator)
{
    BSLS_ASSERT(0 < initialSize);
}

inline
SmallSequentialAllocator::
SmallSequentialAllocator(bsl::size_t                  initialSize,
                         bsls::BlockGrowth::Strategy  growthStrategy,
                         bslma::Allocator            *basicAllocator)
: d_sequentialPool(initialSize, growthStrategy, basicAllocator)
{
    BSLS_ASSERT(0 < initialSize);
}

inline
SmallSequentialAllocator::
SmallSequentialAllocator(bsl::size_t                initialSize,
                         bsls::Alignment::Strategy  alignmentStrategy,
                         bslma::Allocator          *basicAllocator)
: d_sequentialPool(initialSize, alignmentStrategy, basicAllocator)
{
    BSLS_ASSERT(0 < initialSize);
}

inline
SmallSequentialAllocator::
SmallSequentialAllocator(bsl::size_t                  initialSize,
                         bsls::BlockGrowth::Strategy  growthStrategy,
                         bsls::Alignment::Strategy    alignmentStrategy,
                         bslma::Allocator            *basicAllocator)
: d_sequentialPool(initialSize,
                   growthStrategy,
                   alignmentStrategy,
                   basicAllocator)
{
    BSLS_ASSERT(0 < initialSize);
}

inline
SmallSequentialAllocator::
SmallSequentialAllocator(bsl::size_t       initialSize,
                         bsl::size_t       maxBufferSize,
                         bslma::Allocator *basicAllocator)
: d_sequentialPool(initialSize, maxBufferSize, basicAllocator)
{
    BSLS_ASSERT(0           <  initialSize);
    BSLS_ASSERT(initialSize <= maxBufferSize);
}

inline
SmallSequentialAllocator::
SmallSequentialAllocator(bsl::size_t                  initialSize,
                         bsl::size_t                  maxBufferSize,
                         bsls::BlockGrowth::Strategy  growthStrategy,
                         bslma::Allocator            *basicAllocator)
: d_sequentialPool(initialSize, maxBufferSize, growthStrategy, basicAllocator)
{
    BSLS_ASSERT(0           <  initialSize);
    BSLS_ASSERT(initialSize <= maxBufferSize);
}

inline
SmallSequentialAllocator::
SmallSequentialAllocator(bsl::size_t                initialSize,
                         bsl::size_t                maxBufferSize,
                         bsls::Alignment::Strategy  alignmentStrategy,
                         bslma::Allocator          *basicAllocator)
: d_sequentialPool(initialSize,
                   maxBufferSize,
                   alignmentStrategy,
                   basicAllocator)
{
    BSLS_ASSERT(0           <  initialSize);
    BSLS_ASSERT(initialSize <= maxBufferSize);
}

inline
SmallSequentialAllocator::
SmallSequentialAllocator(bsl::size_t                  initialSize,
                         bsl::size_t                  maxBufferSize,
                         bsls::BlockGrowth::Strategy  growthStrategy,
                         bsls::Alignment::Strategy    alignmentStrategy,
                         bslma::Allocator            *basicAllocator)
: d_sequentialPool(initialSize,
                   maxBufferSize,
                   growthStrategy,
                   alignmentStrategy,
                   basicAllocator)
{
    BSLS_ASSERT(0           <  initialSize);
    BSLS_ASSERT(initialSize <= maxBufferSize);
}

// MANIPULATORS
inline
void *SmallSequentialAllocator::allocate(bsl::size_t size)
{
    return d_sequentialPool.allocate(size);
}

inline
void *SmallSequentialAllocator::allocateAndExpand(bsl::size_t *size)
{
    return d_sequentialPool.allocateAndExpand(size);
}

inline
void SmallSequentialAllocator::deallocate(void *)
{
}

inline
void SmallSequentialAllocator::release()
{
    d_sequentialPool.release();
}

inline
void SmallSequentialAllocator::reserveCapacity(bsl::size_t numBytes)
{
    d_sequentialPool.reserveCapacity(numBytes);
}

inline
void SmallSequentialAllocator::rewind()
{
    d_sequentialPool.rewind();
}

inline
bsl::size_t SmallSequentialAllocator::truncate(void        *address,
                                               bsl::size_t  originalSize,
                                               bsl::size_t  newSize)
{
    BSLS_ASSERT(address);

    return d_sequentialPool.truncate(address, originalSize, newSize);
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
