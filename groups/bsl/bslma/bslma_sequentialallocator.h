// bslma_sequentialallocator.h                                        -*-C++-*-
#ifndef INCLUDED_BSLMA_SEQUENTIALALLOCATOR
#define INCLUDED_BSLMA_SEQUENTIALALLOCATOR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Support fast memory allocation for objects of varying sizes.
//
//@INTERNAL_DEPRECATED: Use 'bdlma_bufferedsequentialallocator' instead.
//
//@CLASSES:
//  bslma::SequentialAllocator: fast variable-size memory allocator
//
//@SEE_ALSO: bdlma_sequentialallocator, bdlma_bufferedsequentialallocator
//
//@DESCRIPTION: This component provides an allocator,
// 'bslma::SequentialAllocator', that implements the 'bslma::ManagedAllocator'
// protocol and allocates memory blocks of any requested size, from an internal
// buffer (pool) or a user-supplied buffer.  If an allocation request exceeds
// the remaining free memory space in the pool, the pool either 1) replenishes
// its buffer with new memory to satisfy the request, or 2) returns a separate
// memory block, depending on whether the request size exceeds an optionally
// specified maximum buffer size.  By default, buffer growth is not capped.
// The 'release' method releases all memory allocated through this allocator,
// as does the destructor.  Note, however, that individual allocated blocks of
// memory cannot be separately deallocated.
//..
//   ,--------------------------.
//  ( bslma::SequentialAllocator )
//   `--------------------------'
//                |         ctor/dtor
//                |         allocateAndExpand
//                |         expand
//                |         reserveCapacity
//                |         truncate
//                V
//    ,-----------------------.
//   ( bslma::ManagedAllocator )
//    `-----------------------'
//                |        release
//                V
//       ,-----------------.
//      (  bslma::Allocator )
//       `-----------------'
//                       allocate
//                       deallocate
//..
//
///Alignment Strategy
///------------------
// The 'bslma::SequentialPool' allocates memory using one of the two alignment
// strategies (defined in 'bslma_bufferallocator') optionally specified at
// construction: 1) MAXIMUM ALIGNMENT or 2) NATURAL ALIGNMENT.
//
//: 1 MAXIMUM ALIGNMENT: This strategy always allocates memory aligned with the
//:   most restrictive alignment on the host platform.  The value is defined by
//:   'bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT'.
//:
//: 2 NATURAL ALIGNMENT: This strategy allocates memory whose alignment depends
//:   on the requested number of bytes.  An object of a fundamental type
//:   ('int', etc.) is *naturally* *aligned* when it's size evenly divides its
//:   address.  An object of an aggregate type has natural alignment if the
//:   alignment of the most-restrictively aligned sub-object evenly divides the
//:   address of the aggregate.  Natural alignment is always at least as
//:   restrictive as the compiler's required alignment.  When only the size of
//:   an aggregate is known, and not its composition, we compute the alignment
//:   by finding the largest integral power of 2 (up to and including
//:   'bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT') that divides the requested
//:   (non-zero) number of bytes.  This computed alignment is guaranteed to be
//:   at least as restrictive as any sub-object within the aggregate.
//
// The default strategy is NATURAL ALIGNMENT.
//
///Optional 'buffer' Parameter
///---------------------------
// A buffer can be supplied to a 'bslma::SequentialAllocator' object at
// construction in which case the allocator will try to satisfy allocation
// requests using this buffer before switching to a dynamically-allocated
// internal pool.  Once the allocator is using an internal pool, it will not
// try to satisfy any subsequent allocation requests from the supplied buffer.
// Note that the allocator does *not* take ownership of the buffer.  Also note
// that 'bufferSize' may be specified using a positive or negative value to
// indicate a buffer growth strategy (see "Internal Buffer Growth").
//
///Optional 'initialSize' Parameter
///--------------------------------
// In lieu of an externally-supplied buffer, a value for the 'initialSize'
// parameter may be supplied at construction to specify the initial size of the
// internal pool.  If neither a buffer nor an 'initialSize' is specified, an
// implementation-defined value is used for an initial size of the internal
// pool.  Note that 'initialSize' may be specified using a positive or negative
// value to indicate a buffer growth strategy (see "Internal Buffer Growth").
//
///Internal Buffer Growth
///----------------------
// A 'bslma::SequentialAllocator' replenishes its internal buffer if the
// current buffer cannot satisfy an allocation request.  It does so by one of
// two growth strategies:
//
//: Constant Growth: The new buffer is always of the same size as the current
//:   buffer (possibly supplied at construction).
//:
//: Geometric Growth: The new buffer will be geometrically larger than the
//:   current buffer up to an optionally-specified maximum limit.
//
// If a 'bufferSize' (and corresponding 'buffer') or 'initialSize' is supplied
// at construction, the sign of its value implicitly specifies which growth
// strategy to use.  A positive value indicates Constant Growth, whereas a a
// negative value indicates Geometric Growth.  If neither 'bufferSize' nor
// 'initialSize' is supplied, Geometric Growth is used.  The optional
// 'maxBufferSize' parameter may be used to place a cap on Geometric Growth
// ('maxBufferSize' is ignored if Constant Growth is in effect).  If no value
// is specified for 'maxBufferSize', there is no cap on Geometric Growth.  Note
// that 'reserveCapacity' always ensures that the requested number of bytes is
// available (allocating a new internal pool if necessary) irrespective of
// whether the size of the request exceeds 'maxBufferSize'.
//
///Usage
///-----
// Allocators are often supplied to objects requiring dynamically-allocated
// memory at construction.  For example, consider the following
// 'my_DoubleStack' class, parameterized by a 'bslma::Allocator':
//..
//  // my_doublestack.h
//  // ...
//
//  namespace bslma { class Allocator; }
//
//  class my_DoubleStack {
//      // DATA
//      double           *d_stack_p;      // dynamically-allocated array
//      int               d_size;         // physical capacity this stack
//      int               d_length;       // next available index in stack
//      bslma::Allocator *d_allocator_p;  // memory allocator (held, not owned)
//
//      // FRIENDS
//      friend class my_DoubleStackIter;
//
//    private:
//      // PRIVATE MANIPULATORS
//      void increaseSize();
//          // Increase the capacity of this stack by at least one element.
//
//    public:
//      // CREATORS
//      my_DoubleStack(bslma::Allocator       *basicAllocator = 0);
//      my_DoubleStack(const my_DoubleStack&  original,
//                     bslma::Allocator       *basicAllocator = 0);
//      ~my_DoubleStack();
//
//      // MANIPULATORS
//      my_DoubleStack& operator=(const my_DoubleStack& rhs);
//      void push(double value);
//      void pop();
//
//      // ACCESSORS
//      const double& top() const;
//      bool isEmpty() const;
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
//      d_stack_p[d_length++] = item;
//  }
//
//  // ...
//..
// The stack interface takes an optional 'basicAllocator' supplied only at
// construction.  (We avoid use of the name 'allocator' so as not to conflict
// with the STL use of the word, which differs slightly.)  If non-zero, the
// stack holds a pointer to this allocator, but does not own it.  If no
// allocator is supplied, the implementation itself must either conditionally
// invoke global 'new' and 'delete' explicitly whenever dynamic memory must be
// managed (BAD IDEA) or (GOOD IDEA) install a default allocator that adapts
// use of these global operators to the 'bslma_allocator' interface.  In actual
// practice, however, we might want the default to be run-time settable from a
// central location (see 'bslma_default').
//..
//  // my_doublestack.cpp
//  // ...
//  #include <my_doublestack.h>
//  #include <bslma_allocator.h>
//  #include <bslma_default.h>  // adapter for 'new' and 'delete'
//
//  enum { INITIAL_SIZE = 1, GROW_FACTOR = 2 };
//
//  // ...
//
//  // CREATORS
//  my_DoubleStack::my_DoubleStack(bslma::Allocator *basicAllocator)
//  : d_size(INITIAL_SIZE)
//  , d_length(0)
//  , d_allocator_p(basicAllocator)
//  {
//      assert(d_allocator_p);
//      d_stack_p = (double *)
//                  d_allocator_p->allocate(d_size * sizeof *d_stack_p);
//  }
//
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
//..
// Even in this simplified implementation, all use of the allocator protocol is
// relegated to the '.cpp' file.  Subsequent use of the allocator is
// demonstrated by the following file-scope static reallocation function:
//..
//  static
//  void reallocate(double **array, int newSize, int length,
//                  bslma::Allocator *basicAllocator)
//      // Reallocate memory in the specified 'array' to have the specified
//      // 'newSize' using the specified 'basicAllocator'.  The specified
//      // 'length' number of leading elements are preserved.  Given that the
//      // internal policy of class 'my_DoubleStack' requires that the physical
//      // capacity of the container may grow but never shrink, the behavior is
//      // undefined unless length <= newSize.
//  {
//      assert(array);
//      assert(1 <= newSize);
//      assert(0 <= length);
//      assert(basicAllocator);
//      assert(length <= newSize);        // enforce class invariant
//
//      double *tmp = *array;             // support exception neutrality
//      *array = (double *) basicAllocator->allocate(newSize * sizeof **array);
//
//      // COMMIT POINT
//
//      std::memcpy(*array, tmp, length * sizeof **array);
//      basicAllocator->deallocate(tmp);
//  }
//
//  void my_DoubleStack::increaseSize()
//  {
//      int proposedNewSize = d_size * GROW_FACTOR;     // reallocate can throw
//      assert(proposedNewSize > d_length);
//      reallocate(&d_stack_p, proposedNewSize, d_length, d_allocator_p);
//      d_size = proposedNewSize;                       // we're committed
//  }
//..

#ifdef BDE_OPENSOURCE_PUBLICATION // DEPRECATED
#error "bslma_sequentialallocator is deprecated"
#endif
#include <bslscm_version.h>

#include <bslma_bufferallocator.h>
#include <bslma_managedallocator.h>
#include <bslma_sequentialpool.h>

namespace BloombergLP {


namespace bslma {

class Allocator;

                       // =========================
                       // class SequentialAllocator
                       // =========================

class SequentialAllocator : public ManagedAllocator {
    // This class implements the 'ManagedAllocator' protocol to provide a fast
    // allocator of arbitrarily-sized blocks of memory.  Both the 'release'
    // method and the destructor atomically delete all memory managed by this
    // allocator; the 'deallocate' method, however, has no effect for this
    // class.

    // DATA
    SequentialPool d_sequentialPool;  // sequential pool mechanism

    // NOT IMPLEMENTED
    SequentialAllocator(const SequentialAllocator&);
    SequentialAllocator& operator=(const SequentialAllocator&);

  public:
    // CREATORS
    explicit SequentialAllocator(Allocator *basicAllocator = 0);
    explicit SequentialAllocator(
                       BufferAllocator::AlignmentStrategy  strategy,
                       Allocator                          *basicAllocator = 0);
    explicit SequentialAllocator(int        initialSize,
                                 Allocator *basicAllocator = 0);
    SequentialAllocator(
                       int                                 initialSize,
                       BufferAllocator::AlignmentStrategy  strategy,
                       Allocator                          *basicAllocator = 0);
        // Create a sequential allocator for allocating memory blocks from an
        // internal buffer.  Optionally specify an alignment 'strategy' used to
        // align allocated memory blocks.  If 'strategy' is not specified,
        // *Natural Alignment* is used.  Optionally specify an 'initialSize'
        // the absolute value of which indicates the initial size (in bytes)
        // for the internal buffer.  If 'initialSize' is not specified, an
        // implementation-defined value is used.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  If an 'allocate'
        // or 'reserveCapacity' request cannot be satisfied from the current
        // buffer, a new buffer is allocated, the size of which is determined
        // by a buffer growth strategy implied by 'initialSize'.  If
        // 'initialSize' was specified and is negative, or if it was *not*
        // specified, the buffer growth strategy used is *Geometric Growth*;
        // otherwise it is *Constant Growth*.  (See the component level
        // documentation for further details.)  In either case, the new buffer
        // will have sufficient capacity to satisfy the request.  If *Geometric
        // Growth* is in effect, no limit is imposed on the size of buffers.

    SequentialAllocator(char      *buffer,
                        int        bufferSize,
                        Allocator *basicAllocator = 0);
    SequentialAllocator(
                       char                               *buffer,
                       int                                 bufferSize,
                       BufferAllocator::AlignmentStrategy  strategy,
                       Allocator                          *basicAllocator = 0);
        // Create a sequential allocator for allocating memory blocks initially
        // from the specified 'buffer' the size (in bytes) of which is
        // indicated by the absolute value of the specified 'bufferSize'.
        // Optionally specify an alignment 'strategy' used to align allocated
        // memory blocks.  If 'strategy' is not specified, *Natural Alignment*
        // is used.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  If an 'allocate' or 'reserveCapacity' request
        // cannot be satisfied from the current buffer, a new buffer is
        // allocated, the size of which is determined by a buffer growth
        // strategy implied by 'bufferSize'.  If 'bufferSize' is negative, the
        // buffer growth strategy used is *Geometric Growth*; otherwise it is
        // *Constant Growth*.  (See the component level documentation for
        // further details.)  In either case, the new buffer will have
        // sufficient capacity to satisfy the request.  If *Geometric Growth*
        // is in effect, no limit is imposed on the size of buffers.

    SequentialAllocator(int        initialSize,
                        int        maxBufferSize,
                        Allocator *basicAllocator = 0);
    SequentialAllocator(
                       int                                 initialSize,
                       int                                 maxBufferSize,
                       BufferAllocator::AlignmentStrategy  strategy,
                       Allocator                          *basicAllocator = 0);
        // Create a sequential allocator for allocating memory blocks from an
        // internal buffer the initial size (in bytes) of which is indicated by
        // the absolute value of the specified 'initialSize'.  The specified
        // 'maxBufferSize' indicates the maximum size (in bytes) allowed for
        // internally allocated buffers.  Optionally specify an alignment
        // 'strategy' used to align allocated memory blocks.  If 'strategy' is
        // not specified, *Natural Alignment* is used.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  If an 'allocate'
        // or 'reserveCapacity' request cannot be satisfied from the current
        // buffer, a new buffer is allocated, the size of which is determined
        // by a buffer growth strategy implied by 'initialSize'.  If
        // 'initialSize' is negative, the buffer growth strategy used is
        // *Geometric Growth*; otherwise it is *Constant Growth*.  (See the
        // component level documentation for further details.)  In either case,
        // the new buffer will have sufficient capacity to satisfy the request.
        // If *Geometric Growth* is in effect, the geometric progression of
        // buffer sizes is capped at 'maxBufferSize'.  The behavior is
        // undefined unless '0 < maxBufferSize', and
        // '|initialSize| <= maxBufferSize'.  Note that 'maxBufferSize' is
        // ignored if 'initialSize > 0'.  Also note that 'maxBufferSize' may be
        // overridden by a sufficiently large value passed to 'allocate' or
        // 'reserveCapacity'.

    SequentialAllocator(char      *buffer,
                        int        bufferSize,
                        int        maxBufferSize,
                        Allocator *basicAllocator = 0);
    SequentialAllocator(
                       char                               *buffer,
                       int                                 bufferSize,
                       int                                 maxBufferSize,
                       BufferAllocator::AlignmentStrategy  strategy,
                       Allocator                          *basicAllocator = 0);
        // Create a sequential allocator for allocating memory blocks initially
        // from the specified 'buffer' the size (in bytes) of which is
        // indicated by the absolute value of the specified 'bufferSize'.  The
        // specified 'maxBufferSize' indicates the maximum size (in bytes)
        // allowed for internally allocated buffers.  Optionally specify an
        // alignment 'strategy' used to align allocated memory blocks.  If
        // 'strategy' is not specified, *Natural Alignment* is used.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  If an 'allocate' or 'reserveCapacity' request cannot be
        // satisfied from the current buffer, a new buffer is allocated, the
        // size of which is determined by a buffer growth strategy implied by
        // 'bufferSize'.  If 'bufferSize' is negative, the buffer growth
        // strategy used is *Geometric Growth*; otherwise it is *Constant
        // Growth*.  (See the component level documentation for further
        // details.)  In either case, the new buffer will have sufficient
        // capacity to satisfy the request.  If *Geometric Growth* is in
        // effect, the geometric progression of buffer sizes is capped at
        // 'maxBufferSize'.  The behavior is undefined unless
        // '0 < maxBufferSize', and '|bufferSize| <= maxBufferSize'.  Note that
        // 'maxBufferSize' is ignored if 'bufferSize > 0'.  Also note that
        // 'maxBufferSize' may be overridden by a sufficiently large value
        // passed to 'allocate' or 'reserveCapacity'.

    virtual ~SequentialAllocator();
        // Destroy this sequential allocator and release all associated memory.

    // MANIPULATORS
    virtual void *allocate(size_type numBytes);
        // Return a newly allocated block of memory of (at least) the specified
        // positive 'size' (bytes).  If 'size' is 0, a null pointer is returned
        // with no effect.  The behavior is undefined unless '0 <= size'.  Note
        // that the alignment of the address returned is the maximum alignment
        // for any fundamental type defined for the calling platform.

    void *allocateAndExpand(int *size);
        // Return memory of at least the specified '*size' and return the
        // actual amount of memory allocated in '*size'.  If '*size' is 0, no
        // memory is allocated and 0 is returned.  The behavior is undefined
        // unless '0 <= *size'.

    void *allocateAndExpand(int *size, int maxNumBytes);
        // Return memory of at least the specified '*size' and at most the
        // specified 'maxNumBytes'.  Also return the actual amount of memory
        // allocated in '*size'.  If '*size' is 0, no memory is allocated and 0
        // is returned.  The behavior is undefined unless
        // '0 <= *size <= maxNumBytes'.

    virtual void deallocate(void *address);
        // This method has no effect for this allocator.

    int expand(void *address, int originalNumBytes);
        // Increase the amount of memory allocated at the specified 'address'
        // from the specified 'originalNumBytes' to the maximum amount easily
        // obtainable.  Return the amount of memory available at 'address'
        // after the expansion.  The behavior is undefined unless the call to
        // this allocator that provided the 'address' was performed with the
        // 'originalNumBytes'.  Note that this function will not expand the
        // memory unless there have been no allocations since the allocation
        // for 'originalNumBytes'.

    int expand(void *address, int originalNumBytes, int maxNumBytes);
        // Increase the amount of memory allocated at the specified 'address'
        // from the specified 'originalNumBytes' to the maximum amount easily
        // obtainable up to the specified 'maxNumBytes'.  Return the amount of
        // memory available at 'address' after the expansion.  The behavior is
        // undefined unless the call to this allocator that provided the
        // 'address' was performed with the 'originalNumBytes' and
        // 'originalNumBytes < maxNumBytes'.  Note that this function will not
        // expand the memory unless there have been no allocations since the
        // allocation for 'originalNumBytes'.

    virtual void release();
        // Release all memory currently allocated through this allocator.

    virtual void reserveCapacity(int numBytes);
        // Reserve sufficient memory to satisfy allocation requests for at
        // least the specified 'numBytes' without replenishment (i.e., without
        // internal allocation).  The behavior is undefined unless
        // '0 <= numBytes'.

    int truncate(void *address, int originalNumBytes, int newNumBytes);
        // Reduce the amount of memory allocated at the specified 'address'
        // from the specified 'originalNumBytes' to the specified
        // 'newNumBytes'.  Return the amount of memory available at 'address'
        // after the truncation.  The behavior is undefined unless the call to
        // this allocator that provided the 'address' was performed with the
        // 'originalNumBytes' and 'newNumBytes <= originalNumBytes'.  Note that
        // this function will not truncate the memory unless there have been no
        // allocations since the allocation for 'originalNumBytes'.
};

// ============================================================================
//                          INLINE DEFINITIONS
// ============================================================================

// CREATORS
inline
SequentialAllocator::SequentialAllocator(Allocator *basicAllocator)
: d_sequentialPool(basicAllocator)
{
}

inline
SequentialAllocator::SequentialAllocator(
                            BufferAllocator::AlignmentStrategy  strategy,
                            Allocator                          *basicAllocator)
: d_sequentialPool(strategy, basicAllocator)
{
}

inline
SequentialAllocator::SequentialAllocator(int        initialSize,
                                         Allocator *basicAllocator)
: d_sequentialPool(initialSize, basicAllocator)
{
}

inline
SequentialAllocator::SequentialAllocator(
                            int                                 initialSize,
                            BufferAllocator::AlignmentStrategy  strategy,
                            Allocator                          *basicAllocator)
: d_sequentialPool(initialSize, strategy, basicAllocator)
{
}

inline
SequentialAllocator::SequentialAllocator(char      *buffer,
                                         int        bufferSize,
                                         Allocator *basicAllocator)
: d_sequentialPool(buffer, bufferSize, basicAllocator)
{
}

inline
SequentialAllocator::SequentialAllocator(
                            char                               *buffer,
                            int                                 bufferSize,
                            BufferAllocator::AlignmentStrategy  strategy,
                            Allocator                          *basicAllocator)
: d_sequentialPool(buffer, bufferSize, strategy, basicAllocator)
{
}

inline
SequentialAllocator::SequentialAllocator(int        initialSize,
                                         int        maxBufferSize,
                                         Allocator *basicAllocator)
: d_sequentialPool(initialSize, maxBufferSize, basicAllocator)
{
}

inline
SequentialAllocator::SequentialAllocator(
                            int                                 initialSize,
                            int                                 maxBufferSize,
                            BufferAllocator::AlignmentStrategy  strategy,
                            Allocator                          *basicAllocator)
: d_sequentialPool(initialSize, maxBufferSize, strategy, basicAllocator)
{
}

inline
SequentialAllocator::SequentialAllocator(char      *buffer,
                                         int        bufferSize,
                                         int        maxBufferSize,
                                         Allocator *basicAllocator)
: d_sequentialPool(buffer, bufferSize, maxBufferSize, basicAllocator)
{
}

inline
SequentialAllocator::SequentialAllocator(
                            char                               *buffer,
                            int                                 bufferSize,
                            int                                 maxBufferSize,
                            BufferAllocator::AlignmentStrategy  strategy,
                            Allocator                          *basicAllocator)
: d_sequentialPool(buffer, bufferSize, maxBufferSize, strategy, basicAllocator)
{
}

// MANIPULATORS
inline
void *SequentialAllocator::allocate(size_type numBytes)
{
    return d_sequentialPool.allocate(static_cast<int>(numBytes));
}

inline
void *SequentialAllocator::allocateAndExpand(int *size)
{
    return d_sequentialPool.allocateAndExpand(size);
}

inline
void *SequentialAllocator::allocateAndExpand(int *size, int maxNumBytes)
{
    return d_sequentialPool.allocateAndExpand(size, maxNumBytes);
}

inline
void SequentialAllocator::deallocate(void *)
{
}

inline
int SequentialAllocator::expand(void *address, int originalNumBytes)
{
    return d_sequentialPool.expand(address, originalNumBytes);
}

inline
int SequentialAllocator::expand(void *address,
                                int   originalNumBytes,
                                int   maxNumBytes)
{
    return d_sequentialPool.expand(address, originalNumBytes, maxNumBytes);
}

inline
void SequentialAllocator::release()
{
    d_sequentialPool.release();
}

inline
void SequentialAllocator::reserveCapacity(int numBytes)
{
    d_sequentialPool.reserveCapacity(numBytes);
}

inline
int SequentialAllocator::truncate(void *address,
                                  int   originalNumBytes,
                                  int   newNumBytes)
{
    return d_sequentialPool.truncate(address, originalNumBytes, newNumBytes);
}

}  // close package namespace

#ifndef BDE_OPENSOURCE_PUBLICATION  // BACKWARD_COMPATIBILITY
// ============================================================================
//                           BACKWARD COMPATIBILITY
// ============================================================================

typedef bslma::SequentialAllocator bslma_SequentialAllocator;
    // This alias is defined for backward compatibility.
#endif  // BDE_OPENSOURCE_PUBLICATION -- BACKWARD_COMPATIBILITY

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
