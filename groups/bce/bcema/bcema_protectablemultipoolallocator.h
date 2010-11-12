// bcema_protectablemultipoolallocator.h                              -*-C++-*-
#ifndef INCLUDED_BCEMA_PROTECTABLEMULTIPOOLALLOCATOR
#define INCLUDED_BCEMA_PROTECTABLEMULTIPOOLALLOCATOR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide an allocator from write-protectable pools of memory.
//
//@CLASSES:
//   bcema_ProtectableMultipoolAllocator: allocator from protectable pools
//
//@SEE_ALSO: bcema_protectablemultipool
//
//@AUTHOR: Henry Verschell (hverschell)
//
//@DESCRIPTION: [!DEPRECATED!] This component provides an allocator,
// 'bcema_ProtectableMultipoolAllocator', that implements the
// 'bslma_ManagedAllocator' protocol and provides a partially thread-enabled
// protectable memory manager for a set of memory pools, each dispensing
// fixed-sized blocks of memory of a different size, and providing modifiers to
// change the access protection of that managed memory.  A multipool
// allocation (or deallocation) request allocates memory from (or returns
// memory to) the appropriate internal pool, or else to a separately managed
// list of memory blocks.  The 'protect' method sets the access protection for
// all the memory managed by this allocator to a READ-ONLY state.  Writing to
// memory in the protected state will cause a segmentation violation.  The
// 'unprotect' method sets the access protection for all the memory managed by
// this allocator to a READ-WRITE state.  The 'release' method and the
// destructor both release all memory allocated through this memory manager
// without regard to the current protection state.  The behavior for 'allocate'
// and 'deallocate' is undefined unless the allocator is in an unprotected
// state:
//..
//   ,-----------------------------------.
//  ( bcema_ProtectableMultipoolAllocator )
//   `-----------------------------------'
//                     |                ctor/dtor
//                     |                isProtected
//                     |                maxPooledObjectSize
//                     |                numPools
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
//
///Thread Safety
///-------------
// 'bcema_ProtectableMultipoolAllocator' is *partially* *thread-enabled*,
// meaning the accessors and the 'allocate' and 'deallocate' manipulators can
// be safely invoked from any thread.  However, it is *not* *safe* to invoke
// 'release', 'protect', or 'unprotect' while an instance is being accessed or
// modified on another thread (including using the thread-aware accessors and
// manipulators).  The 'protect', 'unprotect', and 'release' operations always
// require explicit synchronization by the client.
//
///Pooling Strategy
///----------------
// The 'numMemoryPools' parameter supplied at construction specifies the
// number of pools.  Each pool dispenses memory blocks of a different fixed
// size.  The 'i'th pool, for 'i' in  '[ 0 .. numMemoryPools - 1 ]', manages
// blocks of size 'bsls_Alignment::MAX_ALIGNMENT * 2 ^ i'.  Thus the smallest
// pooled block size is always 'bsls_Alignment::MAX_ALIGNMENT'; each
// subsequent pool doubles the size of the blocks managed up to a maximum
// pooled block size of
// 'bsls_Alignment::MAX_ALIGNMENT * 2 ^ (numMemoryPools - 1)'.  Any allocation
// request larger than this maximum size will not be pooled, but will instead
// be handled directly by a call to the underlying allocator (indicated at
// construction).  The allocated memory will still be managed by the
// multipool, but any efficiency that pooling might have provided will be
// lost.  The user should therefore be careful when selecting the number of
// pools for a protectable multipool object.
//
///Usage
///-----
// The following snippets of code depict basic usage of a
// 'bcema_ProtectableMultipoolAllocator'.  The example structures are contrived
// to illustrate how allocation requests of a given size are distributed among
// the pools managed by a multipool allocator.  First define three objects of
// geometrically increasing size:
//..
//     #include <bsls_alignmentutil.h>
//
//      const int MIN_SIZE = bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT;
//
//      struct Small {
//          char x[MIN_SIZE];
//      };
//
//      struct Medium {
//          char x[MIN_SIZE * 2];
//      };
//
//      struct Large {  // sizeof(Large) == MIN_SIZE * 4
//          Small  s1, s2;
//          Medium m;
//      };
//..
// Next create a 'bcema_ProtectableMultipoolAllocator' that manages three
// pools:
//..
//      const int NUM_POOLS = 3;
//      bcema_ProtectableMultipoolAllocator pmpa(NUM_POOLS);
//..
// Reserve capacity in the three pools so that each has sufficient storage
// to allocate *at* *least* eight objects before they *may* need to be
// replenished:
//..
//      const int NUM_OBJS = 8;
//      int objSize        = MIN_SIZE;
//      for (int i = 0; i < NUM_POOLS; ++i) {
//          pmpa.reserveCapacity(objSize, NUM_OBJS);
//          objSize *= 2;
//      }
//..
// Allocate an object from each of the three pools:
//..
//      Small  *pS = new(pmpa.allocate(sizeof(Small)))  Small;    assert(pS);
//      Medium *pM = new(pmpa.allocate(sizeof(Medium))) Medium;   assert(pM);
//      Large  *pL = new(pmpa.allocate(sizeof(Large)))  Large;    assert(pL);
//..
// Allocate a buffer from the "overflow" pool.  All objects whose size exceed
// 'MIN_SIZE * 2 ^ (NUM_POOLS - 1)' are allocated from this pool:
//..
//      char *pBuf = (char *) pmpa.allocate(5 * MIN_SIZE);        assert(pBuf);
//..
// Protect the allocated memory from accidental writes using the 'protect'
// method.  Attempting to modify memory that is in the protected state will
// cause a segmentation violation:
//..
//      pmpa.protect();
//
//      // pBuf[0] = 0;  // Segmentation Violation!
//..
// Unprotect the allocated memory so that it can be modified:
//..
//      pmpa.unprotect();
//
//      pBuf[0] = 0;     // OK
//
//..
// Explicitly deallocate two of the objects previously allocated:
//..
//      pmpa.deallocate(pS);
//      pmpa.deallocate(pL);
//      // 'pS' and 'pL' are no longer valid addresses.
//..
// Deallocate all remaining outstanding objects:
//..
//      pmpa.release();
//      // Now 'pM' and 'pBuf' are also invalid addresses.
//..

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCEMA_PROTECTABLEMULTIPOOL
#include <bcema_protectablemultipool.h>
#endif

#ifndef INCLUDED_BSLMA_MANAGEDALLOCATOR
#include <bslma_managedallocator.h>
#endif

namespace BloombergLP {

class bdema_ProtectableBlockDispenser;

                // =========================================
                // class bcema_ProtectableMultipoolAllocator
                // =========================================

class bcema_ProtectableMultipoolAllocator : public bslma_ManagedAllocator {
    // [!DEPRECATED!] This class implements a "partially" thread-enabled
    // mechanism for managing memory using a set of memory pools of
    // geometrically increasing (fixed) allocation size.  The 'allocate' and
    // 'deallocate' methods of 'bcema_ProtectableMultipoolAllocator'
    // respectively allocate memory from and return memory to the internal
    // pool managing memory of the smallest size satisfying the requested
    // size; if no internal pool of the required size exists, the memory is
    // allocated from or returned to a separately managed list of memory
    // blocks.  The 'protect' method sets the access protection for all the
    // memory managed by this allocator to the protected (READ-ONLY) state,
    // and 'unprotect' sets the access protection to the unprotected
    // (READ-WRITE) state.  The 'release' method and the destructor both
    // unprotect and release all memory allocated through this memory manager
    // without regard to the current protection state.  The behavior is
    // undefined for 'allocate' and 'deallocate' unless this allocator is in
    // an unprotected state.  Requests for "large" memory chunks (larger than
    // can be accommodated by the largest individual pool) are handled
    // directly by the underlying dispenser.
    //
    // 'bcema_ProtectableMultipoolAllocator' is *partially* *thread-enabled*,
    // meaning the accessors and the 'allocate' and 'deallocate' manipulators
    // can be safely invoked from any thread; however, it is *not* *safe* to
    // invoke 'release', 'protect', or 'unprotect' while an instance is being
    // accessed or modified on another thread (including using the thread-aware
    // accessors and manipulators).  The 'protect', 'unprotect', and 'release'
    // operations always require explicit synchronization in user space.

    // DATA
    bcema_ProtectableMultipool d_multipool;  // held protectable multipool

    // NOT IMPLEMENTED
    bcema_ProtectableMultipoolAllocator(
                                   const bcema_ProtectableMultipoolAllocator&);
    bcema_ProtectableMultipoolAllocator& operator=(
                                   const bcema_ProtectableMultipoolAllocator&);

  public:
    // CREATORS
    bcema_ProtectableMultipoolAllocator(
                          int                              numMemoryPools,
                          bdema_ProtectableBlockDispenser *blockDispenser = 0);
        // Create a protectable multipool allocator having the specified
        // 'numMemoryPools'.  Optionally specify a 'blockDispenser' to supply
        // protectable blocks of memory.  If 'blockDispenser' is 0, the
        // 'bdema_NativeProtectableBlockDispenser::singleton' is used.  The
        // 'i'th pool, for 'i' in '[ 0 .. numMemoryPools - 1 ]', manages blocks
        // of size 'bsls_Alignment::MAX_ALIGNMENT * 2 ^ i'.  Memory requests
        // larger than the maximum pooled block size are managed directly by
        // the underlying allocator.  The behavior is undefined unless
        // '1 <= numMemoryPools'.  Note that, in order to indicate specific
        // pool-replenishment strategies, use the other constructor.

    virtual ~bcema_ProtectableMultipoolAllocator();
        // Destroy this object and release all memory managed by it.

    // MANIPULATORS
    virtual void *allocate(size_type size);
        // Return a newly-allocated block of memory of (at least) the specified
        // 'numBytes'.  If 'numBytes' is 0, a null pointer is returned
        // with no other effect.  The alignment of the address returned
        // is the maximum alignment for any fundamental type defined for this
        // platform.  The behavior is undefined unless this allocator is in the
        // unprotected state.  Note that the allocated memory will always be
        // managed by this object, but will not be pooled if
        // 'numBytes > maxPooledObjectSize()'.

    virtual void deallocate(void *address);
        // Deallocate the memory block at the specified 'address' back to this
        // protectable multipool allocator for reuse.  The behavior is
        // undefined unless 'address' was allocated from this multipool
        // allocator, has not since been deallocated, and this multipool
        // allocator is in the unprotected state.

    virtual void release();
        // Unprotect and deallocate all memory managed by this object without
        // regard to its initial protection state.  After this operation
        // returns, the allocator will be left in the unprotected state.
        // The behavior is undefined if this method is called from one thread
        // while another thread is accessing or modifying the memory pool
        // (i.e., this method is *not* thread-safe).

    void reserveCapacity(size_type size, size_type numObjects);
        // Reserve memory from this multipool allocator to satisfy memory
        // requests for at least the specified 'numObjects' having the
        // specified 'size' (in bytes) before the pool replenishes.  If 'size'
        // is 0, this method has no effect.  The behavior is undefined unless
        // '0 <= size', 'size < maxPooledBlockSize()', and '0 <= numObjects'.

    void protect();
        // Set the access protection for the memory managed by this protectable
        // multipool allocator to READ-ONLY.  After this call returns, attempts
        // to write to the memory managed by this object will result in a
        // segmentation violation.  The behavior is undefined if this method is
        // called from one thread while another thread is accessing or
        // modifying the memory pool (i.e., this method is *not* thread-safe).
        // Note that this method has no effect if the memory managed by this
        // object is already in the protected state.

    void unprotect();
        // Set the access protection for the memory managed by this protectable
        // multipool allocator to READ/WRITE.  After this call returns,
        // attempts to write to the memory managed by this object will *not*
        // result in a segmentation violation.  The behavior is undefined if
        // this method is called from one thread while another thread is
        // accessing or modifying the memory pool (i.e., this method is *not*
        // thread-safe).  Note that this method has no effect if the memory
        // managed by this object is already in the unprotected state.

    // ACCESSORS
    bool isProtected() const;
        // Return 'true' if the memory manged by this allocator object is in a
        // protected state, and 'false' otherwise.

    int numPools() const;
        // Return the number of pools managed by this object (supplied at
        // construction).

    int maxPooledBlockSize() const;
        // Return the maximum size of an object that will be pooled by this
        // multipool.  This value is defined to be:
        //..
        //    bsls_Alignment::MAX_ALIGNMENT * 2 ^ (numPools() - 1)
        //..
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                // -----------------------------------------
                // class bcema_ProtectableMultipoolAllocator
                // -----------------------------------------

// CREATORS
inline
bcema_ProtectableMultipoolAllocator::bcema_ProtectableMultipoolAllocator(
                              int                              numMemoryPools,
                              bdema_ProtectableBlockDispenser *blockDispenser)
: d_multipool(numMemoryPools, blockDispenser)
{
}

inline
bcema_ProtectableMultipoolAllocator::~bcema_ProtectableMultipoolAllocator()
{
}

// MANIPULATORS
inline
void bcema_ProtectableMultipoolAllocator::protect()
{
    d_multipool.protect();
}

inline
void bcema_ProtectableMultipoolAllocator::unprotect()
{
    d_multipool.unprotect();
}

// ACCESSORS
inline
bool bcema_ProtectableMultipoolAllocator::isProtected() const
{
    return d_multipool.isProtected();
}

inline
int bcema_ProtectableMultipoolAllocator::numPools() const
{
    return d_multipool.numPools();
}

inline
int bcema_ProtectableMultipoolAllocator::maxPooledBlockSize() const
{
    return d_multipool.maxPooledBlockSize();
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
