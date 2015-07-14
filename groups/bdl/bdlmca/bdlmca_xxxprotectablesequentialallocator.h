// bdlmca_xxxprotectablesequentialallocator.h                             -*-C++-*-
#ifndef INCLUDED_BDLMCA_XXXPROTECTABLESEQUENTIALALLOCATOR
#define INCLUDED_BDLMCA_XXXPROTECTABLESEQUENTIALALLOCATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide fast allocation of memory that can be write-protected.
//
//@CLASSES:
// bdlmca::ProtectableSequentialAllocator: fast protectable memory allocator
//
//@AUTHOR: Henry Verschell (hverschell)
//
//@SEE_ALSO: bdlma_xxxprotectablememoryscopedguard, bdlma_xxxprotectableblocklist
//
//@DESCRIPTION: This component provides an allocator,
// 'bdlmca::ProtectableSequentialAllocator', that implements the
// 'bdlma::ManagedAllocator' protocol and provides a sequential allocator that
// can apply access protection to the memory it allocates, preventing the
// memory from being modified.  This component defines a class,
// 'bdlmca::ProtectableSequentialAllocator', that allocates memory blocks of any
// requested size from an internal buffer.  If an allocation request exceeds
// the remaining free memory space in that buffer, the allocator replenishes
// its buffer with new memory to satisfy the request.  The rate at which the
// buffer expands can be configured using the optional 'bufferExpansionSize'
// constructor parameter.  The 'protect' method sets the access protection for
// all the memory managed by this allocator to a READ-ONLY state.  Writing to
// protected memory will cause a segmentation violation.  The 'unprotect'
// method sets the access protection for all the memory managed by this
// allocator to a READ-WRITE state.  The 'release' method and the destructor
// both unprotect and release all memory allocated through this allocator
// without regard to the current protection state; however, deallocating
// individual blocks of memory does not make them available for reuse.  The
// behavior for 'allocate' and 'deallocate' is undefined unless the allocator
// is in an unprotected state:
//..
//   ,------------------------------------.
//  ( bdlmca::ProtectableSequentialAllocator )
//   `------------------------------------'
//                     |                ctor/dtor
//                     |                expand
//                     |                protect
//                     |                reserveCapacity
//                     |                unprotect
//                     V
//         ,----------------------.
//        ( bdlma::ManagedAllocator )
//         `----------------------'
//                     |        release
//                     V
//            ,-----------------.
//           (  bslma::Allocator )
//            `-----------------'
//                             allocate
//                             deallocate
//..
///Thread Safety
///-------------
// 'bdlmca::ProtectableSequentialAllocator' is fully *thread-safe*, meaning any
// operation on the same allocator object can be safely invoked from any
// thread without having to be synchronized.
//
///Alignment Strategy
///------------------
// The 'bdema::ProtectableSequentialAllocator' allocates memory using one of
// the two alignment strategies described below (see 'bsls_alignment')
// optionally specified at construction: 1) MAXIMUM ALIGNMENT or
// 2) NATURAL ALIGNMENT.
//..
//     MAXIMUM ALIGNMENT: This strategy always allocates memory aligned with
//     the most restrictive alignment on the host platform.  This value is
//     defined by 'bsls::Alignment::MAX_ALIGNMENT'.
//
//     NATURAL ALIGNMENT: This strategy allocates memory whose alignment
//     depends on the requested number of bytes.  An instance of a fundamental
//     type ('int', etc.) is *naturally* *aligned* when its size evenly
//     divides its address.  An instance of an aggregate type has natural
//     alignment if the alignment of the most-restrictively aligned sub-object
//     evenly divides the address of the aggregate.  Natural alignment is
//     always at least as restrictive as the compiler's required alignment.
//     When only the size of an aggregate is known, and not its composition,
//     we compute the alignment by finding the largest integral power of 2 (up
//     to and including 'bsls::Alignment::MAX_ALIGNMENT') that divides the
//     requested (non-zero) number of bytes.  This computed alignment is at
//     least as restrictive as natural alignment.
//..
// The default strategy is NATURAL ALIGNMENT.
//
///'bufferExpansionSize' Parameter
///------------------------------
// The 'bdema::ProtectableSequentialAllocator' allows configurable internal
// memory buffer growth using the optional 'bufferExpansionSize' parameter
// specified at construction.  If 'bufferExpansionSize' is positive, the
// allocator always replenishes its buffer with 'bufferExpansionSize' bytes,
// rounded to the nearest multiple of the underlying
// 'bdlma::ProtectableBlockDispenser' object's minimum block size.  If,
// however, 'bufferExpansionSize' is negative, the pool initially replenishes
// its buffer with the underlying block dispenser's minimum block size.  Each
// time the buffer is depleted, the pool grows its buffer geometrically until
// the increment reaches or exceeds the absolute value of
// 'bufferExpansionSize', at which point the buffer will grow linearly in
// increments of the |'bufferExpansionSize'| (rounded up to the nearest
// multiple of the dispenser's minimum block size).  Finally, if
// 'bufferExpansionSize' is not specified or is 0, the pool initially
// replenishes its buffer with the underlying dispenser's minimum block size,
// and will grow its buffer geometrically "indefinitely" (i.e., until the
// system runs out of memory).
//
///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Implementing a Protectable Stack
///- - - - - - - - - - - - - - - - - - - - - -
// The following example uses the 'bdlmca::ProtectableSequentialAllocator' to
// create a protected stack of integers.  Integers can be pushed onto and
// popped off of the stack, but the memory in the stack is protected so that a
// segmentation violation will occur if any of the memory in the container is
// written to outside of the 'IntegerStack' container.  Since a sequential
// allocator will not release individual blocks of memory when deallocated,
// this container is not very efficient:
//..
//  class IntegerStack {
//      // This is a trivial implementation of a stack of ints whose data has
//      // READ-ONLY access protection.  It does not perform bounds checking.
//
//      // DATA
//      int                            *d_data_p;     // memory for the stack
//
//      int                             d_stackSize;  // index of top of stack
//
//      int                             d_maxSize;    // max stack size
//
//      bdlmca::ProtectableSequentialAllocator
//                                      d_allocator;  // owned memory allocator
//
//    private:
//      // NOT IMPLEMENTED
//      IntegerStack(const IntegerStack&);
//      IntegerStack& operator=(const IntegerStack&);
//
//      // PRIVATE TYPES
//      enum { INITIAL_SIZE = 1, GROW_FACTOR = 2 };
//
//..
// Note that the increaseSize() method below will waste the previously
// allocated memory because a sequential allocator does not provide a means to
// deallocate it.
//..
//      // PRIVATE MANIPULATORS
//      void increaseSize()
//          // Geometrically increase the size of this stack's memory by the
//          // growth factor.  The behavior is undefined unless the stack's
//          // allocator is in an unprotected state.
//      {
//          int      *oldData = d_data_p;
//          int       oldSize = d_maxSize;
//          d_maxSize *= GROW_FACTOR;
//          d_data_p = static_cast<int *>(d_allocator.allocate(
//                                                   d_maxSize * sizeof(int)));
//          bsl::copy(oldData, oldData + oldSize, d_data_p);
//      }
//
//
//    public:
//
//      // CREATORS
//      explicit IntegerStack(
//                     bdlma::ProtectableBlockDispenser *protectedDispenser = 0)
//          // Create an 'IntegerStack'.  Optionally specify a 'dispenser'
//          // used to supply protectable memory.  If 'dispenser' is not
//          // specified, the
//          // 'bdlma::NativeProtectableBlockDispenser::singleton()' is used.
//      : d_data_p()
//      , d_stackSize(0)
//      , d_maxSize(INITIAL_SIZE)
//      , d_allocator(protectedDispenser
//                       ? protectedDispenser
//                       : &bdlma::NativeProtectableBlockDispenser::singleton())
//      {
//          d_data_p = static_cast<int *>(d_allocator.allocate(
//                                                   d_maxSize * sizeof(int)));
//          d_allocator.protect();
//      }
//
//      ~IntegerStack()
//          // Destroy this object (and release its memory).
//      {
//          d_allocator.unprotect();
//      }
//
//..
// We must unprotect the dispenser before modifying or deallocating memory.
// Note that we use a 'bdlma::ProtectableMemoryScopedGuard' to assure that the
// memory will be re-protected in the event of an exception:
//..
//      // MANIPULATORS
//      void push(int value)
//          // Push the specified 'value' onto the stack.
//      {
//          bdlma::ProtectableMemoryScopedGuard
//                  <bdlmca::ProtectableSequentialAllocator> guard(&d_allocator);
//
//          if (d_stackSize >= d_maxSize) {
//              increaseSize();
//          }
//
//          // Sufficient room is guaranteed.
//          d_data_p[d_stackSize++] = value;
//      }
//
//      int pop()
//          // Remove the top value from the stack and return it.
//      {
//          // Memory is being read, not written, so there is no need to
//          // unprotect it.
//          return d_data_p[--d_stackSize];
//      }
//  };
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLMTT_LOCKGUARD
#include <bdlmtt_lockguard.h>
#endif

#ifndef INCLUDED_BDLMTT_XXXTHREAD
#include <bdlmtt_xxxthread.h>
#endif

#ifndef INCLUDED_BDLMA_MANAGEDALLOCATOR
#include <bdlma_managedallocator.h>
#endif

#ifndef INCLUDED_BDLMA_XXXPROTECTABLEBLOCKLIST
#include <bdlma_xxxprotectableblocklist.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLS_ALIGNMENT
#include <bsls_alignment.h>
#endif

#ifndef INCLUDED_BSLS_BLOCKGROWTH
#include <bsls_blockgrowth.h>
#endif

namespace BloombergLP {

namespace bdlma { class ProtectableBlockDispenser; }

namespace bdlmca {
                // ==========================================
                // class ProtectableSequentialAllocator
                // ==========================================

class ProtectableSequentialAllocator : public bdlma::ManagedAllocator {
    // This class defines a *fully thread-safe* memory allocator that dispenses
    // arbitrarily-sized blocks of memory from an internal buffer.  The memory
    // allocated by this allocator can be *protected* from modification by
    // calling 'protect'.  Writing to protected memory will cause a
    // segmentation violation.  If an allocation request exceeds the remaining
    // free memory space in the current buffer, the allocator replenishes its
    // buffer with enough memory to satisfy the request and returns the
    // specified portion of that buffer.  The 'release' method and the
    // destructor unprotect and release all memory managed by this allocator,
    // regardless of the protection state.  However, individual blocks of
    // memory are not released for reuse by 'deallocate'.  The behavior for
    // both 'allocate' and 'deallocate' is undefined unless the allocator is in
    // the unprotected state.

  public:
    // PUBLIC TYPES
    typedef bslma::Allocator::size_type size_type;

  private:
    // DATA
    mutable bdlmtt::Mutex    d_mutex;       // synchronize access to data

    int                    d_cursor;      // position of the next free byte

    bsls::BlockGrowth::Strategy
                           d_growthStrategy;
                                          // strategy to use for for growth

    bsls::Alignment::Strategy
                           d_strategy;    // strategy to use for alignment

    char                  *d_buffer;      // holds current free memory buffer

    size_type              d_bufSize;     // the size of the free buffer

    size_type              d_growthLimit; // limit of buffer growth

    size_type              d_size;        // total amount of allocated memory

    bdlma::ProtectableBlockList
                           d_blockList;   // provides memory

    bdlma::ProtectableBlockDispenser
                          *d_dispenser_p; // dispenser (held, not owned)

  private:
    // NOT IMPLEMENTED
    ProtectableSequentialAllocator(
                                  const ProtectableSequentialAllocator&);
    ProtectableSequentialAllocator& operator=(
                                  const ProtectableSequentialAllocator&);

    // PRIVATE MANIPULATORS
    void *allocateWithoutLock(size_type size);
        // Allocate a contiguous block of memory of the specified 'size' in
        // bytes and return the address of the start of the block.  If 'size'
        // is 0, no memory is allocated and 0 is returned.  The behavior is
        // undefined unless this allocator is in the unprotected state.  Note
        // that this operation does not lock 'd_mutex'.

  public:
    // CREATORS
    explicit ProtectableSequentialAllocator(
                          bdlma::ProtectableBlockDispenser *blockDispenser = 0);
    explicit ProtectableSequentialAllocator(
                          bsls::Alignment::Strategy        strategy,
                          bdlma::ProtectableBlockDispenser *blockDispenser = 0);
    ProtectableSequentialAllocator(
                          bsls::Alignment::Strategy        strategy,
                          bsls::BlockGrowth::Strategy      growthStrategy,
                          size_type                        bufferExpansionSize,
                          bdlma::ProtectableBlockDispenser *blockDispenser = 0);
        // Create a sequential allocator that dispenses protectable memory from
        // an internal buffer.  Optionally specify a 'strategy' to determine
        // the alignment used for allocating memory from the internal buffer
        // (as detailed in the component documentation); natural alignment is
        // used by default.  Optionally specify a 'growthStrategy' and
        // 'bufferExpansionSize' to configure the internal memory buffer growth
        // (as detailed in the component documentation).  If 'growthStrategy'
        // is 'bsls::BlockGrowth::BSLS_CONSTANT', the internal buffer will grow
        // linearly in increments of 'bufferExpansionSize'.  If
        // 'growthStrategy' is 'bsls::BlockGrowth::BSLS_GEOMETRIC' the internal
        // buffer will grow geometrically up to an increment of
        // 'bufferExpansionSize', after which point the buffer will grow
        // linearly in increments of 'bufferExpansionSize'.  If
        // 'growthStrategy' and 'bufferExpansionSize' are not supplied, or if
        // 'growthStrategy' is 'bsls::BlockGrowth::BSLS_GEOMETRIC' and
        // 'bufferExpansionSize' is 0, the internal buffer will grow
        // geometrically indefinitely (i.e., until available memory is
        // exhausted).  Optionally specify a 'blockDispenser' to supply
        // protectable blocks of memory.  If 'blockDispenser' is 0, the
        // 'bdlma::NativeProtectableBlockDispenser::singleton' is used.

    virtual ~ProtectableSequentialAllocator();
        // Destroy this object and release all memory currently allocated
        // through this allocator.

    // MANIPULATORS
    virtual void *allocate(size_type numBytes);
        // Allocate a contiguous block of memory of the specified 'numBytes'
        // and return the address of the start of the block.  If 'numBytes'
        // is 0, no memory is allocated and 0 is returned.  The behavior is
        // undefined unless this allocator is in the unprotected state.

    virtual void deallocate(void *address);
        // Relinquish ownership of the memory at the specified 'address'.  If
        // 'address' is 0, this function has no effect.  The behavior is
        // undefined unless 'address' was allocated using this allocator
        // and this allocator is in the unprotected state.
        //
        // This implementation does not release the memory for reuse.

    size_type expand(void      *address,
                     size_type  originalNumBytes,
                     size_type  maxNumBytes = 0);
        // Increase the amount of memory allocated at the specified 'address'
        // from the specified 'originalNumBytes' to the maximum amount that can
        // be obtained without replenishing the allocator's internal buffer, up
        // to the optionally specified 'maxNumBytes'.  Return the memory size
        // after expansion.  If 'maxNumBytes' is 0 then expand the memory
        // allocated at 'address' to the maximum amount possible without
        // growing the allocator's internal buffer.  The behavior is undefined
        // unless the call to this allocator that provided the 'address' was
        // performed with the 'originalNumBytes' and either
        // 'originalNumBytes < maxNumBytes' or 'maxNumBytes == 0'.

    virtual void release();
        // Unprotect and deallocate all memory managed by this object without
        // regard to its initial protection state.  After this operation
        // returns the allocator will be left in the unprotected state.

    void reserveCapacity(size_type numBytes);
        // Reserve sufficient memory to satisfy allocation requests for at
        // least the specified 'numBytes' without replenishment (i.e., without
        // internal allocation).

    void protect();
        // Set the access protection for the memory managed by this allocator
        // to READ-ONLY.  After this call returns, attempts to write to the
        // memory managed by this allocator will result in a segmentation
        // violation.  Note that this method has no effect if the memory
        // managed by this allocator is already in the protected state.

    void unprotect();
        // Set the access protection for the memory managed by this allocator
        // to READ/WRITE.  After this call returns, attempts to write to the
        // memory managed by this allocator will *not* result in a segmentation
        // violation.  Note that this method has no effect if the memory
        // managed by this allocator is already in the unprotected state

    // ACCESSORS
    bool isProtected() const;
        // Return 'true' if the memory manged by this allocator is in the
        // protected state, and 'false' otherwise.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                  // ------------------------------------------
                  // class ProtectableSequentialAllocator
                  // ------------------------------------------

// MANIPULATORS
inline
void ProtectableSequentialAllocator::protect()
{
    bdlmtt::LockGuard<bdlmtt::Mutex> guard(&d_mutex);
    d_blockList.protect();
}

inline
void ProtectableSequentialAllocator::unprotect()
{
    bdlmtt::LockGuard<bdlmtt::Mutex> guard(&d_mutex);
    d_blockList.unprotect();
}

// ACCESSORS
inline
bool ProtectableSequentialAllocator::isProtected() const
{
    bdlmtt::LockGuard<bdlmtt::Mutex> guard(&d_mutex);
    return d_blockList.isProtected();
}
}  // close package namespace

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
