// bcema_protectablesequentialallocator.h                             -*-C++-*-
#ifndef INCLUDED_BCEMA_PROTECTABLESEQUENTIALALLOCATOR
#define INCLUDED_BCEMA_PROTECTABLESEQUENTIALALLOCATOR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide fast allocation of memory that can be write-protected.
//
//@CLASSES:
// bcema_ProtectableSequentialAllocator: fast protectable memory allocator
//
//@AUTHOR: Henry Verschell (hverschell)
//
//@SEE_ALSO: bdema_protectablememoryscopedguard, bdema_protectableblocklist
//
//@DESCRIPTION: This component provides an allocator,
// 'bcema_ProtectableSequentialAllocator', that implements the
// 'bslma_ManagedAllocator' protocol and provides a sequential allocator that
// can apply access protection to the memory it allocates, preventing the
// memory from being modified.  This component defines a class,
// 'bcema_ProtectableSequentialAllocator', that allocates memory blocks of any
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
//  ( bcema_ProtectableSequentialAllocator )
//   `------------------------------------'
//                     |                ctor/dtor
//                     |                expand
//                     |                protect
//                     |                reserveCapacity
//                     |                unprotect
//                     V
//         ,----------------------.
//        ( bslma_ManagedAllocator )
//         `----------------------'
//                     |        release
//                     V
//            ,----------------.
//           (  bslma_Allocator )
//            `----------------'
//                             allocate
//                             deallocate
//..
///Thread Safety
///-------------
// 'bcema_ProtectableSequentialAllocator' is *thread-enabled*, meaning any
// operation on the same allocator object can be safely invoked from any
// thread without having to be synchronized.
//
///Alignment Strategy
///------------------
// The 'bdema_ProtectableSequentialAllocator' allocates memory using one of
// the two alignment strategies described below (see 'bsls_alignment')
// optionally specified at construction: 1) MAXIMUM ALIGNMENT or
// 2) NATURAL ALIGNMENT.
//..
//     MAXIMUM ALIGNMENT: This strategy always allocates memory aligned with
//     the most restrictive alignment on the host platform.  This value is
//     defined by 'bsls_Alignment::MAX_ALIGNMENT'.
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
//     to and including 'bsls_Alignment::MAX_ALIGNMENT') that divides the
//     requested (non-zero) number of bytes.  This computed alignment is at
//     least as restrictive as natural alignment.
//..
// The default strategy is NATURAL ALIGNMENT.
//
///'bufferExpansionSize' Parameter
///------------------------------
// The 'bdema_ProtectableSequentialAllocator' allows configurable internal
// memory buffer growth using the optional 'bufferExpansionSize' parameter
// specified at construction.  If 'bufferExpansionSize' is positive, the
// allocator always replenishes its buffer with 'bufferExpansionSize' bytes,
// rounded to the nearest multiple of the underlying
// 'bdema_ProtectableBlockDispenser' object's minimum block size.  If,
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
// The following example uses the 'bcema_ProtectableSequentialAllocator'
// to create a protected stack of integers.  Integers can be pushed onto and
// popped off of the stack, but the memory in the stack is protected so that a
// segmentation violation will occur if any of the memory in the container is
// written to outside of the 'IntegerStack' container.  Since a sequential
// allocator will not release individual blocks of memory when deallocated,
// this container is not very efficient:
//..
//    class IntegerStack {
//        // This is a trivial implementation of a stack of 'int' values whose
//        // data has READ-ONLY access protection.  It does not perform bounds
//        // checking.
//
//        // DATA
//        int                       *d_data_p;     // memory for the stack
//
//        int                        d_stackSize;  // index of top of stack
//
//        int                        d_maxSize;    // max stack size
//
//        bcema_ProtectableSequentialAllocator
//                                   d_allocator;  // owned memory allocator
//
//  // ...
//
//        // PRIVATE TYPES
//        enum { INITIAL_SIZE = 1, GROW_FACTOR = 2 };
//..
// Note that the 'increaseSize' method below will waste the previously
// allocated memory because a sequential allocator is incapable of releasing
// individual blocks of memory:
//..
//      private:
//        // PRIVATE MANIPULATORS
//        void increaseSize()
//            // Geometrically increase the size of this stack's memory by the
//            // growth factor.  The behavior is undefined unless the stacks
//            // allocator is in an unprotected state.
//        {
//            int *oldData = d_data_p;
//            int  oldSize = d_maxSize;
//            d_maxSize *= GROW_FACTOR;
//            d_data_p = (int *)d_allocator.allocate(d_maxSize * sizeof(int));
//            bsl::memcpy(d_data_p, oldData, sizeof(int) * oldSize);
//        }
//
//      public:
//        // CREATORS
//        IntegerStack(bdema_ProtectableBlockDispenser *dispenser = 0)
//            // Create an 'IntegerStack'.  Optionally specify a 'dispenser'
//            // used to supply protectable memory.  If 'dispenser' is not
//            // specified, the
//            // 'bdema_NativeProtectableBlockDispenser::singleton()' is used.
//        : d_stackSize(0)
//        , d_maxSize(INITIAL_SIZE)
//        , d_allocator(dispenser)
//        {
//            d_allocator.unprotect();
//            d_data_p = (int *)d_allocator.allocate(d_maxSize * sizeof(int));
//            d_allocator.protect();
//        }
//
//        ~IntegerStack()
//            // Destroy this object (and release its memory).
//        {
//        }
//
//..
// We must unprotect the dispenser before modifying or deallocating memory:
//..
//        // MANIPULATORS
//        void push(int value)
//            // Push the specified 'value' onto the stack.
//        {
//            d_allocator.unprotect();
//            if (d_stackSize >= d_maxSize) {
//                increaseSize();
//            }
//
//            // Sufficient room is guaranteed.
//            d_data_p[d_stackSize++] = value;
//            d_allocator.protect();
//        }
//
//        int pop()
//            // Remove the top value from the stack and return it.
//        {
//            // Memory is being read, not written, so there is no need to
//            // unprotect it.
//            return d_data_p[--d_stackSize];
//        }
//    };
//..

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCEMT_LOCKGUARD
#include <bcemt_lockguard.h>
#endif

#ifndef INCLUDED_BCEMT_THREAD
#include <bcemt_thread.h>
#endif

#ifndef INCLUDED_BDEMA_PROTECTABLEBLOCKLIST
#include <bdema_protectableblocklist.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_MANAGEDALLOCATOR
#include <bslma_managedallocator.h>
#endif

#ifndef INCLUDED_BSLS_ALIGNMENT
#include <bsls_alignment.h>
#endif

#ifndef INCLUDED_BSL_CLIMITS
#include <bsl_climits.h>       // for 'INT_MAX'
#endif

namespace BloombergLP {

class bdema_ProtectableBlockDispenser;

                // ==========================================
                // class bcema_ProtectableSequentialAllocator
                // ==========================================

class bcema_ProtectableSequentialAllocator : public bslma_ManagedAllocator {
    // This class defines a thread enabled memory allocator that dispenses
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

    // DATA
    mutable bcemt_Mutex    d_mutex;       // synchronize access to data

    int                    d_cursor;      // position of the next free byte

    char                  *d_buffer;      // holds current free memory buffer

    size_type              d_bufSize;     // the size of the free buffer

    int                    d_geometricGrowthLimit;
                                          // limit of geometric expansion

    int                    d_size;        // total amount of allocated memory

    bsls_Alignment::Strategy
                           d_strategy;    // strategy to use for alignment

    bdema_ProtectableBlockList
                           d_blockList;   // provides memory

    bdema_ProtectableBlockDispenser
                          *d_dispenser_p; // dispenser (held, not owned)

    // NOT IMPLEMENTED
    bcema_ProtectableSequentialAllocator(
                                  const bcema_ProtectableSequentialAllocator&);
    bcema_ProtectableSequentialAllocator& operator=(
                                  const bcema_ProtectableSequentialAllocator&);

  private:
    // PRIVATE MANIPULATORS
    void *allocateWithoutLock(size_type size);
        // Allocate a contiguous block of memory of the specified 'size' in
        // bytes and return the address of the start of the block.  If 'size'
        // is 0, no memory is allocated and 0 is returned.  The behavior is
        // undefined unless this allocator is in the unprotected state.  Note
        // that this operation does not lock 'd_mutex'.

  public:
    // CREATORS
    bcema_ProtectableSequentialAllocator(
                          bdema_ProtectableBlockDispenser *blockDispenser = 0);
    bcema_ProtectableSequentialAllocator(
                          bsls_Alignment::Strategy         strategy,
                          bdema_ProtectableBlockDispenser *blockDispenser = 0);
    bcema_ProtectableSequentialAllocator(
                          bsls_Alignment::Strategy         strategy,
                          int                              bufferExpansionSize,
                          bdema_ProtectableBlockDispenser *blockDispenser = 0);
        // Create a sequential allocator that dispenses protectable memory
        // from an internal buffer.  Optionally specify a 'strategy' to
        // determine the alignment used for allocating memory from the internal
        // buffer (as detailed in the component documentation); natural
        // alignment is used by default.  Optionally specify a
        // 'bufferExpansionSize' to configure the internal memory buffer growth
        // (as detailed in the component documentation).  If
        // 'bufferExpansionSize' is positive, the internal buffer will grow
        // linearly in increments of 'bufferExpansionSize'.  If
        // 'bufferExpansionSize' is negative the internal buffer will grow
        // geometrically up to an increment of |'bufferExpansionSize'|, after
        // which point the buffer will grow linearly in increments of
        // |'bufferExpansionSize'|.  If 'bufferExpansionSize' is not supplied
        // or 0 the internal buffer will grow geometrically indefinitely (i.e.,
        // until available memory is exhausted).  Optionally specify a
        // 'blockDispenser' to supply protectable blocks of memory.  If
        // 'blockDispenser' is 0, the
        // 'bdema_NativeProtectableBlockDispenser::singleton' is used.

    virtual ~bcema_ProtectableSequentialAllocator();
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

    int expand(void      *address,
               size_type  originalNumBytes,
               size_type  maxNumBytes = 0);
        // Increase the amount of memory allocated at the specified 'address'
        // from the specified 'originalNumBytes' to the maximum amount that
        // can be obtained without replenishing the allocator's internal
        // buffer, up to the optionally specified 'maxNumBytes'.  If
        // 'maxNumBytes' is 0 then expand the memory allocated at 'address' to
        // the maximum amount possible without growing the allocator's
        // internal buffer.  The behavior is undefined unless the call to this
        // allocator that provided the 'address' was performed with the
        // 'originalNumBytes' and 'originalNumBytes < maxNumBytes'.

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
                  // class bcema_ProtectableSequentialAllocator
                  // ------------------------------------------

// MANIPULATORS
inline
void bcema_ProtectableSequentialAllocator::protect()
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    d_blockList.protect();
}

inline
void bcema_ProtectableSequentialAllocator::unprotect()
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    d_blockList.unprotect();
}

// ACCESSORS
inline
bool bcema_ProtectableSequentialAllocator::isProtected() const
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    return d_blockList.isProtected();
}

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
