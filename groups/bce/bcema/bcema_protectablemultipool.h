// bcema_protectablemultipool.h                                       -*-C++-*-
#ifndef INCLUDED_BCEMA_PROTECTABLEMULTIPOOL
#define INCLUDED_BCEMA_PROTECTABLEMULTIPOOL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a manager of pools of memory that can be write-protected.
//
//@CLASSES:
//   bcema_ProtectableMultipool: protectable pools of memory
//
//@SEE_ALSO: bdema_protectableblocklist, bcema_multipool
//
//@AUTHOR: Henry Verschell (hverschell)
//
//@DESCRIPTION: [!DEPRECATED!] This component implements a memory manager,
// 'bcema_Multipool', that maintains a configurable number of 'bcema_Pool'
// objects, each dispensing memory blocks of a unique size, and provides
// modifiers to change the access protection of that managed memory.  The
// 'bcema_Pool' objects are placed in an array, starting at index 0, with each
// successive pool managing memory blocks of a size twice that of the previous
// pool.  Each multipool allocation (deallocation) request allocates memory
// from (returns memory to the internal pool managing memory blocks of the
// smallest size not less than the requested size, or else from a separately
// managed list of memory blocks, if no internal pool managing memory block of
// sufficient size exists.  The 'protect' method sets the access protection
// for all the memory managed by this allocator to a READ-ONLY state.  Writing
// to memory in the protected state will cause a segmentation violation.  The
// 'unprotect' method sets the access protection for all the memory managed by
// this allocator to a READ-WRITE state.  The 'release' method and the
// destructor both release all memory allocated through this memory manager
// without regard to the current protection state.  The behavior for
// 'allocate' and 'deallocate' is undefined unless the pool is in an
// unprotected state.
//
///Thread Safety
///-------------
// 'bcema_ProtectableMultipool' is *partially* *thread-enabled*, meaning the
// accessors and the 'allocate' and 'deallocate' manipulators can be safely
// invoked from any thread.  However, it is *not* *safe* to invoke 'release',
// 'protect', or 'unprotect' while an instance is being accessed or modified
// on another thread (including using the thread-aware accessors and
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
// The following example uses a 'bcema_ProtectableMultipool' object
// to create a stack of integers.  Integers can be pushed onto and popped off
// of the stack, but the memory in the stack is protected so that a
// segmentation violation will occur if any of the data in the stack is
// modified outside of the 'IntegerStack' container:
//..
//    class IntegerStack {
//        // This is a trivial implementation of a stack of 'int' values whose
//        // data has READ-ONLY access protection.
//
//        int                        *d_data_p;      // allocated stack
//                                                   // memory
//
//        int                         d_numElements; // next unused slot
//
//        int                         d_capacity;    // current maximum # of
//                                                   // elements
//
//        bcema_ProtectableMultipool  d_pool;        // memory pool
//
//  // ...
//
//        // Note that the following constants, as well as the implementation
//        // of the 'increaseSize' method, should generally be defined in
//        // the .cpp file.
//
//        enum { INITIAL_SIZE = 1, GROW_FACTOR = 2 , NUM_POOLS = 10 };
//
//      private:
//        // PRIVATE MANIPULATORS
//        void increaseSize()
//            // Geometrically increase the capacity of the stack by the
//            // growth factor.  The behavior is undefined unless the stack's
//            // allocator is in an unprotected state.
//        {
//            int *oldData = d_data_p;
//            int  oldSize = d_capacity;
//            d_capacity *= GROW_FACTOR;
//            d_data_p = (int *)d_pool.allocate(
//                                             d_capacity * sizeof(*d_data_p));
//            bsl::memcpy(d_data_p, oldData, sizeof(*d_data_p) * oldSize);
//            d_pool.deallocate(oldData);
//        }
//
//      public:
//        // CREATORS
//        IntegerStack(bdema_ProtectableBlockDispenser *blockDispenser = 0)
//            // Create an 'IntegerStack'.  Optionally specify a
//            // 'blockDispenser' to supply protectable blocks of memory.  If
//            // 'blockDispenser' is 0, the
//            // 'bdema_NativeProtectableBlockDispenser::singleton' is used.
//        : d_numElements(0)
//        , d_capacity(INITIAL_SIZE)
//        , d_pool(NUM_POOLS, blockDispenser)
//        {
//            d_data_p = (int *)d_pool.allocate(d_capacity *
//                                              sizeof(*d_data_p));
//            d_pool.protect();
//        }
//
//        ~IntegerStack()
//            // Destroy this object and release its memory.
//        {
//        }
//..
// We must unprotect the multipool before modifying or deallocating memory:
//..
//        // MANIPULATORS
//        void push(int value)
//            // Push the specified 'value' onto the stack.
//        {
//            d_pool.unprotect();
//            if (d_numElements >= d_capacity) {
//                increaseSize();
//            }
//            d_data_p[d_numElements++] = value;
//            d_pool.protect();
//        }
//
//        int pop()
//            // Remove the top value from the stack and return it.
//        {
//            // Memory is read, not written, so there is no need to unprotect
//            // the multipool.
//            return d_data_p[--d_numElements];
//        }
//
//  // ...
//
//    };
//..

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCEMA_MULTIPOOL
#include <bcema_multipool.h>
#endif

#ifndef INCLUDED_BCEMA_PROTECTABLEBLOCKDISPENSERADAPTER
#include <bcema_protectableblockdispenseradapter.h>
#endif

#ifndef INCLUDED_BCEMA_PROTECTABLESEQUENTIALALLOCATOR
#include <bcema_protectablesequentialallocator.h>
#endif

#ifndef INCLUDED_BCEMT_THREAD
#include <bcemt_thread.h>
#endif

#ifndef INCLUDED_BDEMA_PROTECTABLEBLOCKLIST
#include <bdema_protectableblocklist.h>
#endif

#ifndef INCLUDED_BSLMA_DELETERHELPER
#include <bslma_deleterhelper.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>
#endif

namespace BloombergLP {

class bdema_ProtectableBlockDispenser;

                // ================================
                // class bcema_ProtectableMultipool
                // ================================

class bcema_ProtectableMultipool {
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
    // the unprotected state.  Requests for "large" memory chunks (larger than
    // can be accommodated by the largest individual pool) are handled
    // directly by the underlying dispenser.
    //
    // 'bcema_ProtectableMultipool' is *partially* *thread-enabled*, meaning
    // the accessors and the 'allocate' and 'deallocate' manipulators can be
    // safely invoked from any thread; however, it is *not* *safe* to invoke
    // 'release', 'protect', or 'unprotect' while an instance is being
    // accessed or modified on another thread (including using the thread-aware
    // accessors and manipulators).  The 'protect', 'unprotect', and 'release'
    // operations always require explicit synchronization in user space.

    // DATA
    bcemt_Mutex                    d_mutex;            // synchronize access

    bdema_ProtectableBlockList     d_blockList;        // overflow "pool"

    bcema_ProtectableBlockDispenserAdapter
                                   d_dispenserAdapter; // thread-enabled
                                                       // wrapper for dispenser
    bcema_ProtectableSequentialAllocator
                                   d_allocator;        // underlies pools

    bcema_Multipool               *d_pools_p;          // multi pool

    // NOT IMPLEMENTED
    bcema_ProtectableMultipool(const bcema_ProtectableMultipool&);
    bcema_ProtectableMultipool& operator=(const bcema_ProtectableMultipool&);

  public:
    // PUBLIC TYPES:
    typedef bsls_PlatformUtil::size_type size_type;  // type for allocate size

    // CREATORS
    bcema_ProtectableMultipool(
                         int                              numMemoryPools,
                         bdema_ProtectableBlockDispenser *blockDispenser = 0);
        // Create a protectable multipool having the specified
        // 'numMemoryPools'.  Optionally specify a 'blockDispenser' to supply
        // protectable blocks of memory.  If 'blockDispenser' is 0, the
        // 'bdema_NativeProtectableBlockDispenser::singleton' is used.  The
        // 'i'th pool, for 'i' in '[ 0 .. numMemoryPools - 1 ]', manages blocks
        // of size 'bsls_Alignment::MAX_ALIGNMENT * 2 ^ i'.  Memory requests
        // larger than the maximum pooled block size are managed directly by
        // the underlying allocator.  The behavior is undefined unless
        // '1 <= numMemoryPools'.  Note that, in order to indicate specific
        // pool-replenishment strategies, use the other constructor.

    ~bcema_ProtectableMultipool();
        // Destroy this object and release all memory managed by it.

    // MANIPULATORS
    void *allocate(size_type numBytes);
        // Return a newly-allocated block of memory of (at least) the specified
        // 'numBytes'.  The alignment of the address returned is the maximum
        // alignment for any fundamental type defined for this platform.  The
        // behavior is undefined unless this allocator is in the unprotected
        // state and 'numBytes >= 0'.  Note that the allocated memory will
        // always be managed by this object, but will not be pooled if
        // 'numBytes > maxPooledObjectSize()'.

    void deallocate(void *address);
        // Deallocate the memory block at the specified 'address' back to this
        // multipool for reuse.  The behavior is undefined unless 'address'
        // was allocated from this multipool object, has not since been
        // deallocated, and this multipool is in the unprotected state.

    template<class TYPE>
    void deleteObject(const TYPE *object);
        // Destroy the specified 'object' based on its dynamic type and then
        // use this multipool to deallocate its memory footprint.  Do nothing
        // if 'object' is a null pointer.  The behavior is undefined unless
        // 'object', when cast appropriately to 'void*', was allocated using
        // this multipool has not already been deallocated, and this multipool
        // is in the unprotected state.  Note that
        // 'dynamic_cast<void*>(object)' is applied if 'TYPE' is polymorphic,
        // and 'static_cast<void*>(object)' is applied otherwise.

    template <class TYPE>
    void deleteObjectRaw(const TYPE *object);
        // Destroy the specified 'object' and then use this multipool to
        // deallocate its memory footprint.  Do nothing if 'object' is a null
        // pointer.  The behavior is undefined unless 'object' is !not! a
        // secondary base class pointer (i.e., the address is (numerically) the
        // same as when it was originally dispensed by this multipool), was
        // allocated using this multipool, and has not already been
        // deallocated.

    void release();
        // Unprotect and deallocate all memory managed by this object without
        // regard to its initial protection state.  After this operation
        // returns, the allocator will be left in the unprotected state.
        // The behavior is undefined if this method is called from one thread
        // while another thread is accessing or modifying the memory pool
        // (i.e., this method is *not* thread-safe).

    void reserveCapacity(size_type size, size_type numObjects);
        // Reserve memory from this multipool to satisfy memory requests for at
        // least the specified 'numBlocks' having the specified 'size' (in
        // bytes) before the pool replenishes.  The behavior is undefined
        // unless '1 <= size <= maxPooledBlockSize()', and '0 <= numBlocks'.

    void protect();
        // Set the access protection for the memory managed by this multipool
        // memory manager to READ-ONLY.  After this call returns, attempts to
        // write to the memory managed by this object will result in a
        // segmentation violation.  The behavior is undefined if this method is
        // called from one thread while another thread is accessing or
        // modifying the memory pool (i.e., this method is *not* thread-safe).
        // Note that this method has no effect if the memory managed by this
        // object is already in the protected state.

    void unprotect();
        // Set the access protection for the memory managed by this multipool
        // memory manager to READ/WRITE.  After this call returns, attempts to
        // write to the memory managed by this object will *not* result in a
        // segmentation violation.  The behavior is undefined if this method is
        // called from one thread while another thread is accessing or
        // modifying the memory pool (i.e., this method is *not* thread-safe).
        // Note that this method has no effect if the memory managed by this
        // object is already in the unprotected state.

    // ACCESSORS
    bool isProtected() const;
        // Return 'true' if the memory manged by this object is in the
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

                   // --------------------------------
                   // class bcema_ProtectableMultipool
                   // --------------------------------

// CREATORS
inline
bcema_ProtectableMultipool::~bcema_ProtectableMultipool()
{
}

// MANIPULATORS
template<class TYPE>
inline
void bcema_ProtectableMultipool::deleteObject(const TYPE *object)
{
    bslma_DeleterHelper::deleteObject(object, this);
}

template <class TYPE>
inline
void bcema_ProtectableMultipool::deleteObjectRaw(const TYPE *object)
{
    bslma_DeleterHelper::deleteObjectRaw(object, this);
}

inline
void bcema_ProtectableMultipool::protect()
{
    d_blockList.protect();
    d_allocator.protect();
}

inline
void bcema_ProtectableMultipool::unprotect()
{
    d_blockList.unprotect();
    d_allocator.unprotect();
}

// ACCESSORS
inline
bool bcema_ProtectableMultipool::isProtected() const
{
    return d_allocator.isProtected();
}

inline
int bcema_ProtectableMultipool::numPools() const
{
    return d_pools_p->numPools();
}

inline
int bcema_ProtectableMultipool::maxPooledBlockSize() const
{
    return d_pools_p->maxPooledBlockSize();
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
