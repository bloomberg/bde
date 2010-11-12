// bcema_protectableblockdispenseradapter.h      -*-C++-*-
#ifndef INCLUDED_BCEMA_PROTECTABLEBLOCKDISPENSERADAPTER
#define INCLUDED_BCEMA_PROTECTABLEBLOCKDISPENSERADAPTER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide thread-safe adapter for 'bdema_ProtectableBlockDispenser'.
//
//@CLASSES:
//  bcema_ProtectableBlockDispenserAdapter: thread-enabled protocol adapter
//
//@SEE_ALSO: bdema_protectableblockdispenser, bcema_protectablemultipool
//
//@AUTHOR: Henry Verschell (hverschell)
//
//@DESCRIPTION: [!DEPRECATED!] This component defines an adapter for the
// 'bdema_ProtectableBlockDispenser' protocol that provides synchronization
// for operations on a dispenser supplied at construction using a mutex also
// supplied at construction.
//
///Thread Safety
///-------------
// 'bcema_ProtectableBlockDispenserAdapter' is *thread-enabled*, meaning any
// operation on the same instance can be safely invoked from any thread.
//
///Usage
///-----
// In the following elaborate usage example, we develop a simple
// 'ThreadEnabledProtectableMultipool' class containing a series of
// thread-enabled pools of memory.  We use a
// 'bcema_ProtectableBlockDispenserAdapter' to synchronize memory
// allocations *across* our thread-enabled pools.  For the purpose of this
// discussion, we first begin by defining a protectable pool:
//..
//  // threadenabledprotectablepool.h
//
//  class ThreadEnabledProtectablePool {
//      // This 'class' defines a thread-enabled protectable pool that
//      // dispenses memory blocks of a fixed size, specified at construction.
//
//      // PRIVATE TYPES
//      struct Link {
//          // This 'struct' holds a pointer to the next free memory block in
//          // the free memory list.
//
//          Link *d_next_p;
//      };
//
//      // DATA
//      const int                   d_objectSize;  // size of each individual
//                                                 // memory block
//
//      int                         d_numPooledObjects;
//                                                 // number of pooled objects
//                                                 // both in use and on the
//                                                 // free list
//
//      Link                       *d_freeList_p;  // list of free blocks
//
//      bdema_ProtectableBlockList  d_blockList;   // supplies free memory
//
//      bcemt_Mutex                 d_mutex;       // synchronize access to
//                                                 // non-'const' data
//
//      // NOT IMPLEMENTED
//      ThreadEnabledProtectablePool(const ThreadEnabledProtectablePool&);
//      ThreadEnabledProtectablePool& operator=(
//                                   const ThreadEnabledProtectablePool&);
//
//    private:
//      // PRIVATE MANIPULATORS
//      void replenish();
//          // Replenish the free memory list of this pool.
//
//    public:
//      // CREATORS
//      ThreadEnabledProtectablePool(
//                             int                              objectSize,
//                             bdema_ProtectableBlockDispenser *dispenser = 0);
//          // Create a 'ThreadEnabledProtectablePool' that will return memory
//          // blocks of the specified 'objectSize' for each 'allocate' method
//          // invocation.  Use the optionally specified 'dispenser' to
//          // supply protectable memory.  If 'dispenser' is 0, the
//          // 'bdema_NativeProtectableBlockDispenser::singleton()' is
//          // used.
//
//      ~ThreadEnabledProtectablePool();
//          // Destroy this protectable pool of memory and release its
//          // contents.
//
//      // MANIPULATORS
//      void *allocate();
//          // Return a newly-allocated block of memory of 'objectSize()'
//          // bytes.
//
//      void deallocate(void *address);
//          // Deallocate the memory block at the specified 'address' back to
//          // this pool for reuse.
//
//      void protect();
//          // Set the access protection for the memory managed by this pool
//          // to READ-ONLY.  After this call returns, attempts to
//          // modify the memory managed by this object will result in a
//          // segmentation violation.
//
//      void unprotect();
//          // Set the access protection for the memory managed by this pool
//          // to READ-WRITE.  After this call returns, attempts to
//          // write to the memory managed by this object will *not* result in
//          // a segmentation violation.
//
//      // ACCESSORS
//      int objectSize() const;
//          // Return the fixed allocation size of this pool.  This value is
//          // equal to the 'objectSize' supplied at construction rounded up
//          // to the next multiple of 'bsls_Alignment::MAX_ALIGNMENT' (in
//          // order to ensure aligned allocations).
//  };
//
//  // threadenabledprotectablepool.cpp
//
//  // STATIC HELPER FUNCTIONS
//  inline static
//  int roundUp(int x, int y)
//      // Round up the specified 'x' to the nearest whole integer multiple of
//      // the specified 'y'.  The behavior is undefined unless '0 <= x' and
//      // '0 < y'.
//  {
//      return (x + y - 1) / y * y;
//  }
//
//  // PRIVATE MANIPULATORS
//  void ThreadEnabledProtectablePool::replenish()
//  {
//      int numObjects = d_numPooledObjects ? d_numPooledObjects : 1;
//      bdema_MemoryBlockDescriptor block =
//                             d_blockList.allocate(d_objectSize * numObjects);
//
//      // Compute the actual number of objects that will fit in the allocated
//      // memory (the block list may return more memory than requested).
//      int actualNumObjects = block.size() / d_objectSize;
//
//      char *start = static_cast<char *>(block.address());
//      char *last  = start + (actualNumObjects - 1) * d_objectSize;
//      for (char *p = start; p < last; p += d_objectSize) {
//          ((Link *) p)->d_next_p = (Link *) (p + d_objectSize);
//      }
//      ((Link *) last)->d_next_p = d_freeList_p;
//      d_freeList_p = (Link *)start;
//
//      d_numPooledObjects += actualNumObjects;
//  }
//
//  // CREATORS
//  ThreadEnabledProtectablePool::ThreadEnabledProtectablePool(
//                                 int                              objectSize,
//                                 bdema_ProtectableBlockDispenser *dispenser)
//
//  : d_objectSize(roundUp(objectSize, bsls_Alignment::MAX_ALIGNMENT))
//  , d_numPooledObjects(0)
//  , d_freeList_p(0)
//  , d_blockList(dispenser)
//  {
//  }
//
//  ThreadEnabledProtectablePool::~ThreadEnabledProtectablePool()
//  {
//  }
//
//  // MANIPULATORS
//  void *ThreadEnabledProtectablePool::allocate()
//  {
//      bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
//      if (!d_freeList_p) {
//          replenish();
//      }
//      Link *link   = d_freeList_p;
//      d_freeList_p = link->d_next_p;
//      return link;
//  }
//
//  void ThreadEnabledProtectablePool::deallocate(void *address)
//  {
//      bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
//      Link *link     = static_cast<Link *>(address);
//      link->d_next_p = d_freeList_p;
//      d_freeList_p   = link;
//  }
//
//  void ThreadEnabledProtectablePool::protect()
//  {
//      d_blockList.protect();
//  }
//
//  void ThreadEnabledProtectablePool::unprotect()
//  {
//      d_blockList.unprotect();
//  }
//
//  // ACCESSORS
//  int ThreadEnabledProtectablePool::objectSize() const
//  {
//      return d_objectSize;
//  }
//..
// We use this thread-enabled protectable pool to create a thread-enabled
// protectable multipool; however, we use the
// 'bcema_ProtectableBlockDispenserAdapter' to prevent our (thread-enabled)
// pools from attempting simultaneous memory allocations from our, not
// necessarily thread-safe, 'bdema_ProtectableBlockDispenser'.  Note that the
// 'ThreadEnabledProtectableMultipool' is optionally supplied both an allocator
// and a dispenser at construction.  The supplied dispenser provides the
// protectable memory dispensed to clients of the multipool.  The supplied
// allocator is used to efficiently allocate the internal members 'd_pools_p'
// and 'd_poolSizes' (i.e., it avoids allocating an entire protectable page for
// the relatively small internal data members):
//..
//  // threadenabledprotectablemultipool.h
//
//  class ThreadEnabledProtectableMultipool {
//      // This class implements a thread-enabled mechanism for managing a set
//      // of memory pools, each dispensing fixed-sized memory blocks of
//      // a size supplied at construction.  The 'allocate' ('deallocate')
//      // method allocates memory from (returns memory to) the internal pool
//      // that manages memory of the smallest size that satisfies the
//      // requested size.  The 'ThreadEnabledProtectableMultipool' is
//      // optionally supplied both a 'bdema_ProtectableBlockDispenser' and a
//      // 'bslma_Allocator' at construction.  The protectable block dispenser
//      // provides protectable memory that is dispensed to clients of this
//      // multipool on calls to the 'allocate' operation; the allocator is
//      // used to provide (non-protectable) memory for internal use (the
//      // 'd_poolSizes' vector and 'd_pools_p' array are allocated with the
//      // allocator).
//
//      // PRIVATE TYPES
//      union Header {
//          // Stores the pool number of this item.
//
//          int                            d_pool;   // pool index
//          bsls_Alignment::MaxAlignedType d_dummy;  // force alignment
//      };
//
//      // DATA
//      bcemt_Mutex               d_mutex;       // synchronize access to data
//
//      bcema_ProtectableBlockDispenserAdapter
//                                d_dispenser;   // adapter for dispenser,
//                                               // allocates protected memory
//
//      const int                 d_numPools;    // number of pools
//
//      bsl::vector<int>          d_poolSizes;   // sorted array of pool
//                                               // object sizes
//      ThreadEnabledProtectablePool
//                               *d_pools_p;     // array of pools
//                                               // corresponding to
//                                               // 'd_poolSizes'
//
//      bslma_Allocator          *d_allocator_p; // allocate 'd_pools_p' array
//                                               // and 'd_poolSizes'
//
//      // NOT IMPLEMENTED
//      ThreadEnabledProtectableMultipool(
//                                   const ThreadEnabledProtectableMultipool&);
//      ThreadEnabledProtectablePool& operator=(
//                                   const ThreadEnabledProtectableMultipool&);
//
//    private:
//      // PRIVATE MANIPULATORS
//      void init(int *poolSizes);
//          // Initialize the internal structures 'd_pools_p' and
//          // 'd_poolSizes' to hold 'd_numPools' memory pools of the
//          // specified 'poolSizes'.
//
//    public:
//      // CREATORS
//      ThreadEnabledProtectableMultipool(
//                        int                              numMemoryPools,
//                        int                             *poolSizes,
//                        bslma_Allocator                 *basicAllocator = 0);
//      ThreadEnabledProtectableMultipool(
//                        int                              numMemoryPools,
//                        int                             *poolSizes,
//                        bdema_ProtectableBlockDispenser *dispenser,
//                        bslma_Allocator                 *basicAllocator = 0);
//          // Create a multipool having the specified 'numMemoryPools'
//          // dispensing fixed size blocks of the specified 'poolSizes'.
//          // Optionally specify a 'dispenser' used to supply protectable
//          // memory.  If 'dispenser' is not specified or 0, the
//          // 'bdema_NativeProtectableBlockDispenser::singleton()' is
//          // used.  Optionally specify a 'basicAllocator' used to supply
//          // non-managed memory for internal use by this multipool.  If
//          // 'basicAllocator' is 0, the currently installed
//          // default allocator is used.  The 'i'th pool, for 'i' in
//          // '[ 0 .. numMemoryPools - 1 ]' manages blocks of size
//          // 'poolSizes[i]' rounded up to the nearest multiple of
//          // 'bsls_Alignment::MAX_ALIGNMENT'.  The behavior is undefined
//          // unless '1 <= numMemoryPools', and 'poolSizes' has
//          // 'numMemoryPools' elements, none of which is 0.  Note that the
//          // indicated 'basicAllocator' is used to supply memory for
//          // internal data, whereas the 'dispenser' is used to supply
//          // managed memory to clients.
//
//      ~ThreadEnabledProtectableMultipool();
//          // Destroy this object and release all memory managed by it.
//
//       // MANIPULATORS
//       void *allocate(int numBytes);
//          // Return a newly-allocated block of memory of (at least) the
//          // specified positive 'numBytes'.  If 'numBytes' is 0 or if
//          // 'numBytes' is larger than the maximum pooled object size
//          // supplied at construction, a null pointer is returned with no
//          // effect.  The behavior is undefined unless '0 <= numBytes' and
//          // this multipool object is in the unprotected state.  Note that
//          // the alignment of the address returned is the maximum alignment
//          // for any fundamental type defined for this platform.
//
//       void deallocate(void *address);
//          // Deallocate the memory block at the specified 'address' back to
//          // this multipool for reuse.  The behavior is undefined unless
//          // 'address' was allocated from this multipool object, has not
//          // since been deallocated, and this multipool object is in the
//          // unprotected state.
//
//       void protect();
//          // Set the access protection for the memory managed by this
//          // multipool to READ-ONLY.  After this call returns, attempts to
//          // modify the memory managed by this object will result in a
//          // segmentation violation.
//
//       void unprotect();
//          // Set the access protection for the memory managed by this
//          // multipool to READ-WRITE.  After this call returns, attempts to
//          // write to the memory managed by this object will *not* result in
//          // a segmentation violation.
//  };
//
//  // threadenabledprotectablemultipool.cpp
//
//  // PRIVATE MANIPULATORS
//  void ThreadEnabledProtectableMultipool::init(int *poolSizes)
//  {
//      for (int i = 0; i < d_numPools; ++i) {
//          d_poolSizes.push_back(poolSizes[i]);
//      }
//
//      // Sort the pool sizes so that 'allocate()' can use a binary search to
//      // find the appropriate pool.
//      bsl::sort(d_poolSizes.begin(), d_poolSizes.end());
//
//      d_pools_p = (ThreadEnabledProtectablePool *)
//                   d_allocator_p->allocate(d_numPools * sizeof(*d_pools_p));
//
//      bslma_DeallocatorProctor<bslma_Allocator> autoPoolsDeallocator(
//                                                  d_pools_p, d_allocator_p);
//
//      int size = bsls_Alignment::MAX_ALIGNMENT;
//      bslma_AutoDestructor<ThreadEnabledProtectablePool>
//                                                     autoDtor(d_pools_p, 0);
//      for (int i = 0; i < d_numPools; ++i, ++autoDtor) {
//          new (d_pools_p + i) ThreadEnabledProtectablePool(
//                                              poolSizes[i] + sizeof(Header),
//                                              &d_dispenser);
//      }
//      autoDtor.release();
//      autoPoolsDeallocator.release();
//  }
//
//  // CREATORS
//  ThreadEnabledProtectableMultipool::ThreadEnabledProtectableMultipool(
//                        int                              numMemoryPools,
//                        int                             *poolSizes,
//                        bslma_Allocator                 *basicAllocator)
//  : d_dispenser(&d_mutex, 0)
//  , d_pools_p(0)
//  , d_poolSizes(basicAllocator)
//  , d_numPools(numMemoryPools)
//  , d_allocator_p(bslma_Default::allocator(basicAllocator))
//  {
//      init(poolSizes);
//  }
//
//  ThreadEnabledProtectableMultipool::ThreadEnabledProtectableMultipool(
//                        int                              numMemoryPools,
//                        int                             *poolSizes,
//                        bdema_ProtectableBlockDispenser *dispenser,
//                        bslma_Allocator                 *basicAllocator)
//  : d_dispenser(&d_mutex, dispenser)
//  , d_pools_p(0)
//  , d_poolSizes(basicAllocator)
//  , d_numPools(numMemoryPools)
//  , d_allocator_p(bslma_Default::allocator(basicAllocator))
//  {
//      init(poolSizes);
//  }
//
//  ThreadEnabledProtectableMultipool::~ThreadEnabledProtectableMultipool()
//  {
//      for (int i = 0; i < d_numPools; ++i) {
//          d_pools_p[i].~ThreadEnabledProtectablePool();
//      }
//      d_allocator_p->deallocate(d_pools_p);
//  }
//
//  // MANIPULATORS
//  void *ThreadEnabledProtectableMultipool::allocate(int numBytes)
//  {
//      if (0 == numBytes) {
//          return 0;
//      }
//
//      // Perform a binary search to find the appropriately-sized pool.
//      const int *pool = bsl::lower_bound(d_poolSizes.begin(),
//                                         d_poolSizes.end(),
//                                         numBytes);
//      if (pool == d_poolSizes.end()) {
//          return 0;  // There is no pool that can supply 'numBytes'.
//      }
//
//      // The index in the 'd_poolSizes' vector corresponds to the index in
//      // 'd_pools_p.
//
//      int index = pool - d_poolSizes.begin();
//      Header *block = (Header *)d_pools_p[index].allocate();
//      block->d_pool = index;
//      return block + 1;
//  }
//
//  void ThreadEnabledProtectableMultipool::deallocate(void *address)
//  {
//      Header *header = (Header *)address - 1;
//      d_pools_p[header->d_pool].deallocate(header);
//  }
//
//  void ThreadEnabledProtectableMultipool::protect()
//  {
//      for (int i = 0; i < d_numPools; ++i) {
//          d_pools_p[i].protect();
//      }
//  }
//
//  void ThreadEnabledProtectableMultipool::unprotect()
//  {
//      for (int i = 0; i < d_numPools; ++i) {
//          d_pools_p[i].unprotect();
//      }
//  }
//..

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BDEMA_MEMORYBLOCKDESCRIPTOR
#include <bdema_memoryblockdescriptor.h>
#endif

#ifndef INCLUDED_BDEMA_NATIVEPROTECTABLEBLOCKDISPENSER
#include <bdema_nativeprotectableblockdispenser.h>
#endif

#ifndef INCLUDED_BDEMA_PROTECTABLEBLOCKDISPENSER
#include <bdema_protectableblockdispenser.h>
#endif

namespace BloombergLP {

class bcemt_Mutex;

          // ============================================
          // class bcema_ProtectableBlockDispenserAdapter
          // ============================================

class bcema_ProtectableBlockDispenserAdapter :
                                       public bdema_ProtectableBlockDispenser {
    // This class defines an implementation of the
    // 'bdema_ProtectableBlockDispenser' protocol that "decorates" (wraps) a
    // concrete 'bdema_ProtectableBlockDispenser' to ensure thread-safe access
    // to the decorated dispenser.  Note that this class is local because it
    // is used by the 'bcema_ProtectableMultipool' to ensure that allocations
    // on an underlying (non-thread-enabled) 'bdema_ProtectableBlockDispenser'
    // object are synchronized.

    // DATA
    mutable bcemt_Mutex             *d_mutex_p;      // synchronizer for
                                                     // operations on the
                                                     // dispenser (held,
                                                     // not owned)

    bdema_ProtectableBlockDispenser *d_dispenser_p;  // dispenser (held, not
                                                     // owned)

    // NOT IMPLEMENTED
    bcema_ProtectableBlockDispenserAdapter(
                                const bcema_ProtectableBlockDispenserAdapter&);
    bcema_ProtectableBlockDispenserAdapter& operator=(
                                const bcema_ProtectableBlockDispenserAdapter&);

  public:
    // CREATORS
    bcema_ProtectableBlockDispenserAdapter(
                          bcemt_Mutex                     *mutex,
                          bdema_ProtectableBlockDispenser *blockDispenser = 0);
        // Create a thread-enabled dispenser adapter that will use the
        // specified 'mutex' to synchronize access from multiple threads to the
        // optionally-specified 'blockDispenser'.  If 'blockDispenser' is 0,
        // the 'bdema_NativeProtectableBlockDispenser::singleton' is used.

    virtual ~bcema_ProtectableBlockDispenserAdapter();
        // Destroy this thread-enabled dispenser adapter.

    // MANIPULATORS
    virtual bdema_MemoryBlockDescriptor allocate(size_type numBytes);
        // Allocate a block of at least the specified 'numBytes' and return a
        // descriptor of the block (containing the address and actual size of
        // the allocated memory).  If 'numBytes' is 0, a null block descriptor
        // is returned with no other effect.  If this dispenser cannot return
        // the requested number of bytes, it will throw a 'bsl::bad_alloc'
        // exception in an exception-enabled build, or else abort the program
        // in a non-exception build.  Note that the alignment of the address
        // returned conforms to the platform requirement for any object of the
        // specified 'numBytes'.

    virtual void deallocate(const bdema_MemoryBlockDescriptor& block);
        // Return the memory described by the specified 'block' to this
        // dispenser.  If the 'block' is a null block descriptor then this
        // function has no effect.  The behavior is undefined unless 'block'
        // was returned from a call to 'allocate' on this object, has not
        // since been deallocated, and is currently in an unprotected state.

    virtual int protect(const bdema_MemoryBlockDescriptor& block);
        // Set the access protection for the specified 'block' to be READ-ONLY.
        // Return 0 on success and a non-zero value otherwise.  This method has
        // no effect if 'block' is a null descriptor or was already protected;
        // concrete implementations of the 'bdema_ProtectableBlockDispenser'
        // protocol may differ in the status value that is returned in
        // those two cases.  The behavior is undefined unless 'block' was
        // returned from a call to 'allocate' on this object and has not
        // since been deallocated.

    virtual int unprotect(const bdema_MemoryBlockDescriptor& block);
        // Set the access protection for the specified 'block' to be
        // READ-WRITE.  Return 0 on success and a non-zero value otherwise.
        // This method has no effect if 'block' is a null descriptor or was
        // already unprotected; concrete implementations of the
        // 'bdema_ProtectableBlockDispenser' protocol may differ in the status
        // value that is returned in those two cases.  The behavior is
        // undefined unless 'block' was returned from a call to 'allocate' on
        // this object and has not since been deallocated.

    // ACCESSORS
    virtual int minimumBlockSize() const;
        // Return the minimum size of any block returned by this allocator.
        // Note that this value indicates the size of the block returned by
        // 'allocate(1)'.
        //
        // For this implementation, the minimum block size is the minimum
        // block size of the underlying dispenser indicated at construction.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

            // --------------------------------------------
            // class bcema_ProtectableBlockDispenserAdapter
            // --------------------------------------------

// CREATORS
inline
bcema_ProtectableBlockDispenserAdapter::bcema_ProtectableBlockDispenserAdapter(
                               bcemt_Mutex                     *mutex,
                               bdema_ProtectableBlockDispenser *blockDispenser)
: d_mutex_p(mutex)
, d_dispenser_p(
      0 == blockDispenser ? &bdema_NativeProtectableBlockDispenser::singleton()
                          : blockDispenser)
{
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
