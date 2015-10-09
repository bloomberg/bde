// bdlma_concurrentpool.h                                             -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLMA_CONCURRENTPOOL
#define INCLUDED_BDLMA_CONCURRENTPOOL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide thread-safe allocation of memory blocks of uniform size.
//
//@CLASSES:
//   bdlma::ConcurrentPool: thread-safe memory manager that allocates blocks
//
//@SEE_ALSO: bdema_pool
//
//@DESCRIPTION: This component implements a memory pool,
// 'bdlma::ConcurrentPool', that allocates and manages memory blocks of some
// uniform size specified at construction.  A 'bdlma::ConcurrentPool' object
// maintains an internal linked list of free memory blocks, and dispenses one
// block for each 'allocate' method invocation.  When a memory block is
// deallocated, it is returned to the free list for potential reuse.
//
// Whenever the linked list of free memory blocks is depleted, the
// 'bdlma::ConcurrentPool' replenishes the list by first allocating a large,
// contiguous "chunk" of memory, then splitting the chunk into multiple memory
// blocks.  A chunk and its constituent memory blocks can be depicted visually:
//..
//     +-----+--- memory blocks of uniform size
//     |     |
//   ----- ----- ------------
//  |     |     |     ...    |
//   =====^=====^============
//
//   \___________ __________/
//               V
//           a "chunk"
//..
// Note that the size of the allocated chunk is determined by both the growth
// strategy and maximum blocks per chunk, either of which can be optionally
// specified at construction (see the "Configuration at Construction" section).
//
///Configuration at Construction
///-----------------------------
// When creating a 'bdlma::ConcurrentPool', clients must specify the specific
// block size managed and dispensed by the pool.  Furthermore, clients can
// optionally configure:
//
//: 1 GROWTH STRATEGY -- geometrically growing chunk size starting from 1 (in
//:   terms of the number of memory blocks per chunk), or fixed chunk size.  If
//:   the growth strategy is not specified, geometric growth is used.
//: 2 MAX BLOCKS PER CHUNK -- the maximum number of memory blocks within a
//:   chunk.  If the maximum blocks per chunk is not specified, an
//:   implementation-defined default value is used.
//: 3 BASIC ALLOCATOR -- the allocator used to supply memory to replenish the
//:   internal pool.  If not specified, the currently installed default
//:   allocator (see 'bslma_default') is used.
//
// For example, if geometric growth is used and the maximum blocks per chunk is
// specified as 30, the chunk size grows geometrically, starting from 1, until
// the specified maximum blocks per chunk, as follows:
//..
//  1, 2, 4, 8, 16, 30, 30, 30 ...
//..
// If constant growth is used, the chunk size is always the specified maximum
// blocks per chunk (or an implementation-defined value if the maximum blocks
// per chunk is not specified), for example:
//..
//  30, 30, 30 ...
//..
// A default-constructed pool has an initial chunk size of 1 (i.e., the number
// of memory blocks of a given size allocated at once to replenish a pool's
// memory), and the pool's chunk size grows geometrically until it reaches an
// implementation-defined maximum, at which it is capped.  Finally, unless
// otherwise specified, all memory comes from the allocator that was the
// currently installed default allocator at the time the
// 'bdlma::ConcurrentPool' was created.
//
///Overloaded Global Operator 'new'
///--------------------------------
// This component overloads the global 'operator new' to allow convenient
// syntax for the construction of objects using a 'bdlma::ConcurrentPool'.  The
// 'new' operator supplied in this component takes a 'bdlma::ConcurrentPool'
// argument indicating the source of the memory.  Consider the following use of
// standard placement 'new' syntax (supplied by 'bsl_new.h') along with a
// 'bdlma::ConcurrentPool' to allocate an object of type 'T'.  Note that the
// size of 'T' must be the same or smaller than the 'blockSize' with which the
// pool is constructed:
//..
//  void f(bdlma::ConcurrentPool *pool)
//  {
//      assert(pool->blockSize() >= sizeof(T));
//
//      T *t = new (pool->allocate()) T(...);
//
//      // ...
//  }
//..
// This usage style is not exception-safe.  If the constructor of 'T' throws an
// exception, 'pool->deallocate' is never called.
//
// Supplying an overloaded global 'operator new':
//..
//  ::operator new(bsl::size_t size, bdlma::ConcurrentPool& pool);
//..
// allows for the following cleaner usage, which does not require the size
// calculation and guarantees that 'pool->deallocate' *is* called in case of an
// exception:
//..
//  void f(bdlma::ConcurrentPool *pool)
//  {
//      assert(pool->blockSize() >= sizeof(T));
//
//      T *t = new (*pool) T(...);
//
//      // ...
//..
// Also note that the analogous version of operator 'delete' should *not* be
// called directly.  Instead, this component provides a static template member
// function 'deleteObject', parameterized on 'TYPE':
//..
//      pool->deleteObject(t);
//  }
//..
// The above 'deleteObject' call is equivalent to performing the following:
//..
//  t->~TYPE();
//  pool->deallocate(t);
//..
// An overloaded operator 'delete' is supplied solely to allow the compiler to
// arrange for it to be called in case of an exception.
//
///Usage
///-----
// A 'bdlma::ConcurrentPool' can be used by node-based containers (such as
// lists, trees, and hash tables that hold multiple elements of uniform size)
// for efficient memory allocation of new elements.  The following container
// class, 'my_PooledArray', stores templatized values "out-of-place" as nodes
// in a 'vector' of pointers.  Since the size of each node is fixed and known
// *a priori*, the class uses a 'bdlma::ConcurrentPool' to allocate memory for
// the nodes to improve memory allocation efficiency:
//..
//  // my_poolarray.h
//
//  template <class T>
//  class my_PooledArray {
//      // This class implements a container that stores 'double' values
//      // out-of-place.
//
//      // DATA
//      bsl::vector<T *>      d_array_p;  // array of pooled elements
//      bdlma::ConcurrentPool d_pool;     // memory manager for array elements
//
//    private:
//      // Not implemented:
//      my_PooledArray(const my_PooledArray&);
//
//    public:
//      // CREATORS
//      explicit my_PooledArray(bslma::Allocator *basicAllocator = 0);
//          // Create a pooled array that stores the parameterized values
//          // "out-of-place".  Optionally specify a 'basicAllocator' used to
//          // supply memory.  If 'basicAllocator' is 0, the currently
//          // installed default allocator is used.
//
//      ~my_PooledArray();
//          // Destroy this array and all elements held by it.
//
//      // MANIPULATORS
//      void append(const T &value);
//          // Append the specified 'value' to this array.
//
//      void removeAll();
//          // Remove all elements from this array.
//
//      // ACCESSORS
//      int length() const;
//          // Return the number of elements in this array.
//
//      const T& operator[](int index) const;
//          // Return a reference to the non-modifiable value at the specified
//          // 'index' in this array.  The behavior is undefined unless
//          // '0 <= index < length()'.
//  };
//..
// In the 'removeAll' method, all elements are deallocated by invoking the
// pool's 'release' method.  This technique implies significant performance
// gain when the array contains many elements:
//..
//  // MANIPULATORS
//  template <class T>
//  inline
//  void my_PooledArray<T>::removeAll()
//  {
//      d_array_p.clear();
//      d_pool.release();
//  }
//
//  // ACCESSORS
//  template <class T>
//  inline
//  int my_PooledArray<T>::length() const
//  {
//      return static_cast<int>(d_array_p.size());
//  }
//
//  template <class T>
//  inline
//  const T& my_PooledArray<T>::operator[](int index) const
//  {
//      assert(0 <= index);
//      assert(index < length());
//
//      return *d_array_p[index];
//  }
//..
// Note that the growth strategy and maximum chunk size of the pool is left as
// the default value:
//..
//  // my_poolarray.cpp
//
//  // CREATORS
//  template <class T>
//  my_PooledArray<T>::my_PooledArray(bslma::Allocator *basicAllocator)
//  : d_array_p(basicAllocator)
//  , d_pool(sizeof(T), basicAllocator)
//  {
//  }
//..
// Since all memory is managed by 'd_pool', we do not have to explicitly invoke
// 'deleteObject' to reclaim outstanding memory.  The destructor of the pool
// will automatically deallocate all array elements:
//..
//  template <class T>
//  my_PooledArray<T>::~my_PooledArray()
//  {
//      // Elements are automatically deallocated when 'd_pool' is destroyed.
//  }
//..
// Note that the overloaded "placement" 'new' is used to allocate new nodes:
//..
//  template <class T>
//  void my_PooledArray<T>::append(const T& value)
//  {
//      T *tmp = new (d_pool) T(value);
//      d_array_p.push_back(tmp);
//  }
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLMT_MUTEX
#include <bslmt_mutex.h>
#endif

#ifndef INCLUDED_BDLMA_INFREQUENTDELETEBLOCKLIST
#include <bdlma_infrequentdeleteblocklist.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_DELETERHELPER
#include <bslma_deleterhelper.h>
#endif

#ifndef INCLUDED_BSLS_ALIGNMENTUTIL
#include <bsls_alignmentutil.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_ATOMIC
#include <bsls_atomic.h>
#endif

#ifndef INCLUDED_BSLS_ATOMICOPERATIONS
#include <bsls_atomicoperations.h>
#endif

#ifndef INCLUDED_BSLS_BLOCKGROWTH
#include <bsls_blockgrowth.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSL_CSTDDEF
#include <bsl_cstddef.h>
#endif

namespace BloombergLP {
namespace bdlma {

                           // ====================
                           // class ConcurrentPool
                           // ====================

class ConcurrentPool {
    // This class implements a memory pool that allocates and manages memory
    // blocks of some uniform size specified at construction.  This memory pool
    // maintains an internal linked list of free memory blocks, and dispenses
    // one block for each 'allocate' method invocation.  When a memory block is
    // deallocated, it is returned to the free list for potential reuse.
    //
    // This class guarantees thread safety while allocating or releasing
    // memory.

    // PRIVATE TYPES
    struct Link {
        // This 'struct' implements a link data structure that stores the
        // address of the next link, and is used to implement the internal
        // linked list of free memory blocks.  Note that this type is
        // replicated in 'bdlma_concurrentpool.cpp' to provide access to a
        // compatible type from static methods defined in 'bdema_pool.cpp'.

        union {
            bsls::AtomicOperations::AtomicTypes::Int d_refCount;
            bsls::AlignmentUtil::MaxAlignedType      d_dummy;
        };
        Link  *volatile d_next_p;   // pointer to next link
    };

    // DATA
    int              d_blockSize;  // size of each allocated memory block
                                   // returned to client

    int              d_internalBlockSize;
                                   // actual size of each block maintained on
                                   // free list (contains overhead for 'Link')

    int              d_chunkSize;  // current chunk size (in blocks-per-chunk)

    int              d_maxBlocksPerChunk;
                                   // maximum chunk size (in blocks-per-chunk)
    bsls::BlockGrowth::Strategy
                     d_growthStrategy;
                                   // growth strategy of the chunk size
    bsls::AtomicPointer<Link>
                     d_freeList;   // linked list of free memory blocks
    bdlma::InfrequentDeleteBlockList
                     d_blockList;  // memory manager for allocated memory

    bslmt::Mutex      d_mutex;      // protects access to the block list

    // PRIVATE MANIPULATORS
    void replenish();
        // Dynamically allocate a new chunk using the pool's underlying growth
        // strategy, and use the chunk to replenish the free memory list of
        // this pool.  The behavior is undefined unless the calling thread has
        // a lock on 'd_mutex'.

  private:
    // NOT IMPLEMENTED
    ConcurrentPool(const ConcurrentPool&);
    ConcurrentPool& operator=(const ConcurrentPool&);

  public:
    // CREATORS
    explicit ConcurrentPool(int               blockSize,
                            bslma::Allocator *basicAllocator = 0);
    ConcurrentPool(int                          blockSize,
                   bsls::BlockGrowth::Strategy  growthStrategy,
                   bslma::Allocator            *basicAllocator = 0);
    ConcurrentPool(int                          blockSize,
                   bsls::BlockGrowth::Strategy  growthStrategy,
                   int                          maxBlocksPerChunk,
                   bslma::Allocator            *basicAllocator = 0);
        // Create a memory pool that returns blocks of contiguous memory of the
        // specified 'blockSize' (in bytes) for each 'allocate' method
        // invocation.  Optionally specify a 'growthStrategy' used to control
        // the growth of internal memory chunks (from which memory blocks are
        // dispensed).  If 'growthStrategy' is not specified, geometric growth
        // is used.  Optionally specify 'maxBlocksPerChunk' as the maximum
        // chunk size.  If geometric growth is used, the chunk size grows
        // starting at 'blockSize', doubling in size until the size is exactly
        // 'blockSize * maxBlocksPerChunk'.  If constant growth is used, the
        // chunk size is always 'maxBlocksPerChunk'.  If 'maxBlocksPerChunk' is
        // not specified, an implementation-defined value is used.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  The behavior is undefined unless '1 <= blockSize' and
        // '1 <= maxBlocksPerChunk'.

    ~ConcurrentPool();
        // Destroy this pool, releasing all associated memory back to the
        // underlying allocator.

    // MANIPULATORS
    void *allocate();
        // Return the address of a contiguous block of memory having the fixed
        // block size specified at construction.

    void deallocate(void *address);
        // Relinquish the memory block at the specified 'address' back to this
        // pool object for reuse.  The behavior is undefined unless 'address'
        // is non-zero, was allocated by this pool, and has not already been
        // deallocated.

    template <class TYPE>
    void deleteObject(const TYPE *object);
        // Destroy the specified 'object' based on its dynamic type and then
        // use this pool to deallocate its memory footprint.  This method has
        // no effect if 'object' is 0.  The behavior is undefined unless
        // 'object', when cast appropriately to 'void *', was allocated using
        // this pool and has not already been deallocated.  Note that
        // 'dynamic_cast<void *>(object)' is applied if 'TYPE' is polymorphic,
        // and 'static_cast<void *>(object)' is applied otherwise.

    template <class TYPE>
    void deleteObjectRaw(const TYPE *object);
        // Destroy the specified 'object' and then use this pool to deallocate
        // its memory footprint.  This method has no effect if 'object' is 0.
        // The behavior is undefined unless 'object' is !not! a secondary base
        // class pointer (i.e., the address is (numerically) the same as when
        // it was originally dispensed by this pool), was allocated using this
        // pool, and has not already been deallocated.

    void release();
        // Relinquish all memory currently allocated via this pool object.

    void reserveCapacity(int numBlocks);
        // Reserve memory from this pool to satisfy memory requests for at
        // least the specified 'numBlocks' before the pool replenishes.  The
        // behavior is undefined unless '0 <= numBlocks'.

    // ACCESSORS
    int blockSize() const;
        // Return the size (in bytes) of the memory blocks allocated from this
        // pool object.  Note that all blocks dispensed by this pool have the
        // same size.
};

}  // close package namespace
}  // close enterprise namespace

// Note that the 'new' and 'delete' operators are declared outside the
// 'BloombergLP' namespace so that they do not hide the standard placement
// 'new' and 'delete' operators (i.e.,
// 'void *operator new(bsl::size_t, void *)' and
// 'void operator delete(void *)').
//
// Also note that only the scalar versions of operators 'new' and 'delete' are
// provided, because overloading 'new' (and 'delete') with their array versions
// would cause dangerous ambiguity.  Consider what would have happened had we
// overloaded the array version of 'operator new':
//..
//  void *operator new[](bsl::size_t size, BloombergLP::bdlma::Pool& pool);
//..
// A user of 'bdlma::Pool' may expect to be able to use array 'operator new' as
// follows:
//..
//   new (*pool) my_Type[...];
//..
// The problem is that this expression returns an array that cannot be safely
// deallocated.  On the one hand, there is no syntax in C++ to invoke an
// overloaded 'operator delete'; on the other hand, the pointer returned by
// 'operator new' cannot be passed to the 'deallocate' method directly because
// the pointer is different from the one returned by the 'allocate' method.
// The compiler offsets the value of this pointer by a header, which is used to
// maintain the number of objects in the array (so that 'operator delete' can
// destroy the right number of objects).

// FREE OPERATORS
void *operator new(bsl::size_t size, BloombergLP::bdlma::ConcurrentPool& pool);
    // Return a block of memory of the specified 'size' (in bytes) allocated
    // from the specified 'pool'.  The behavior is undefined unless 'size' is
    // the same or smaller than the 'blockSize' with which 'pool' was
    // constructed.  Note that an object may allocate additional memory

    // internally, requiring the allocator to be passed in as a constructor
    // argument:
    //..
    //  my_Type *newMyType(bdlma::ConcurrentPool *pool,
    //                     bslma::Allocator      *basicAllocator)
    //  {
    //      return new (*pool) my_Type(..., basicAllocator);
    //  }
    //..
    // Also note that the analogous version of 'operator delete' should not be
    // called directly.  Instead, this component provides a static template
    // member function, 'deleteObject', parameterized by 'TYPE':
    //..
    //  void deleteMyType(my_Type *t, bdlma::ConcurrentPool *pool)
    //  {
    //      pool->deleteObject(t);
    //  }
    //..
    // 'deleteObject' performs the following:
    //..
    //  t->~my_Type();
    //  pool->deallocate(t);
    //..

inline
void operator delete(void *address, BloombergLP::bdlma::ConcurrentPool& pool);
    // Use the specified 'pool' to deallocate the memory at the specified
    // 'address'.  The behavior is undefined unless 'address' was allocated
    // using 'pool' and has not already been deallocated.  This operator is
    // supplied solely to allow the compiler to arrange for it to be called in
    // case of an exception.  Client code should not call it; use
    // 'bdlma::ConcurrentPool::deleteObject()' instead.

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

namespace BloombergLP {
namespace bdlma {

                           // --------------------
                           // class ConcurrentPool
                           // --------------------

// MANIPULATORS
template<class TYPE>
inline
void ConcurrentPool::deleteObject(const TYPE *object)
{
    bslma::DeleterHelper::deleteObject(object, this);
}

template<class TYPE>
inline
void ConcurrentPool::deleteObjectRaw(const TYPE *object)
{
    bslma::DeleterHelper::deleteObjectRaw(object, this);
}

inline
void ConcurrentPool::release()
{
    d_mutex.lock();
    d_freeList = (Link*)0;
    d_blockList.release();
    d_mutex.unlock();
}

// ACCESSORS
inline
int ConcurrentPool::blockSize() const
{
    return d_blockSize;
}

}  // close package namespace
}  // close enterprise namespace

// FREE OPERATORS
inline
void *operator new(bsl::size_t size, BloombergLP::bdlma::ConcurrentPool& pool)
{
#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
    // gcc-4.8.1 introduced a new warning for unused typedefs, so this typedef
    // should only be present in SAFE mode builds (where it is used).

    typedef BloombergLP::bsls::AlignmentUtil Util;

    BSLS_ASSERT_SAFE(static_cast<int>(size) <= pool.blockSize()
                  && Util::calculateAlignmentFromSize(size)
                       <= Util::calculateAlignmentFromSize(pool.blockSize()));
#endif

    static_cast<void>(size);  // suppress "unused parameter" warnings
    return pool.allocate();
}

inline
void operator delete(void *address, BloombergLP::bdlma::ConcurrentPool& pool)
{
    pool.deallocate(address);
}

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
