// bdlma_concurrentpool.h                                             -*-C++-*-
#ifndef INCLUDED_BDLMA_CONCURRENTPOOL
#define INCLUDED_BDLMA_CONCURRENTPOOL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide thread-safe allocation of memory blocks of uniform size.
//
//@CLASSES:
//   bdlma::ConcurrentPool: thread-safe memory manager that allocates blocks
//
//@SEE_ALSO: bdlma_pool
//
//@DESCRIPTION: This component implements a memory pool,
// `bdlma::ConcurrentPool`, that allocates and manages memory blocks of some
// uniform size specified at construction.  A `bdlma::ConcurrentPool` object
// maintains an internal pool of free memory blocks, and dispenses one block
// for each `allocate` method invocation.  When a memory block is deallocated,
// it is returned for potential reuse.
//
// Whenever the pool of free memory blocks is depleted, the
// `bdlma::ConcurrentPool` replenishes the pool by allocating a contiguous
// "chunk" of memory, then splitting the chunk into multiple memory
// blocks.  A chunk and its constituent memory blocks can be depicted visually:
// ```
//    +-----+--- memory blocks of uniform size
//    |     |
//  ----- ----- ------------
// |     |     |     ...    |
//  =====^=====^============
//
//  \___________ __________/
//              V
//          a "chunk"
// ```
//
// When creating a `bdlma::ConcurrentPool`, clients must specify the specific
// block size managed and dispensed by the pool, and optionally the allocator
// used to supply memory to replenish the internal pool.  If not specified, the
// currently installed default allocator (see `bslma_default`) is used.
//
///Overloaded Global Operator `new`
///--------------------------------
// This component overloads the global `operator new` to allow convenient
// syntax for the construction of objects using a `bdlma::ConcurrentPool`.  The
// `new` operator supplied in this component takes a `bdlma::ConcurrentPool`
// argument indicating the source of the memory.  Consider the following use of
// standard placement `new` syntax (supplied by `bsl_new.h`) along with a
// `bdlma::ConcurrentPool` to allocate an object of type `T`.  Note that the
// size of `T` must be the same or smaller than the `blockSize` with which the
// pool is constructed:
// ```
// void f(bdlma::ConcurrentPool *pool)
// {
//     assert(pool->blockSize() >= sizeof(T));
//
//     T *t = new (pool->allocate()) T(...);
//
//     // ...
// }
// ```
// This usage style is not exception-safe.  If the constructor of `T` throws an
// exception, `pool->deallocate` is never called.
//
// Supplying an overloaded global `operator new`:
// ```
// ::operator new(bsl::size_t size, bdlma::ConcurrentPool& pool);
// ```
// allows for the following cleaner usage, which does not require the size
// calculation and guarantees that `pool->deallocate` *is* called in case of an
// exception:
// ```
// void f(bdlma::ConcurrentPool *pool)
// {
//     assert(pool->blockSize() >= sizeof(T));
//
//     T *t = new (*pool) T(...);
//
//     // ...
// ```
// Also note that the analogous version of operator `delete` should *not* be
// called directly.  Instead, this component provides a static template member
// function `deleteObject`, parameterized on `TYPE`:
// ```
//     pool->deleteObject(t);
// }
// ```
// The above `deleteObject` call is equivalent to performing the following:
// ```
// t->~TYPE();
// pool->deallocate(t);
// ```
// An overloaded operator `delete` is supplied solely to allow the compiler to
// arrange for it to be called in case of an exception.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Usage
/// - - - - - - - - - - -
// A `bdlma::ConcurrentPool` can be used by node-based containers (such as
// lists, trees, and hash tables that hold multiple elements of uniform size)
// for efficient memory allocation of new elements.  The following container
// class, `my_PooledArray`, stores templatized values "out-of-place" as nodes
// in a `vector` of pointers.  Since the size of each node is fixed and known
// *a priori*, the class uses a `bdlma::ConcurrentPool` to allocate memory for
// the nodes to improve memory allocation efficiency:
// ```
// // my_poolarray.h
//
// template <class T>
// class my_PooledArray {
//     // This class implements a container that stores 'double' values
//     // out-of-place.
//
//     // DATA
//     bsl::vector<T *>      d_array_p;  // array of pooled elements
//     bdlma::ConcurrentPool d_pool;     // memory manager for array elements
//
//   private:
//     // Not implemented:
//     my_PooledArray(const my_PooledArray&);
//
//   public:
//     // CREATORS
//     explicit my_PooledArray(bslma::Allocator *basicAllocator = 0);
//         // Create a pooled array that stores the parameterized values
//         // "out-of-place".  Optionally specify a 'basicAllocator' used to
//         // supply memory.  If 'basicAllocator' is 0, the currently
//         // installed default allocator is used.
//
//     ~my_PooledArray();
//         // Destroy this array and all elements held by it.
//
//     // MANIPULATORS
//     void append(const T &value);
//         // Append the specified 'value' to this array.
//
//     void removeAll();
//         // Remove all elements from this array.
//
//     // ACCESSORS
//     int length() const;
//         // Return the number of elements in this array.
//
//     const T& operator[](int index) const;
//         // Return a reference to the non-modifiable value at the specified
//         // 'index' in this array.  The behavior is undefined unless
//         // '0 <= index < length()'.
// };
// ```
// // CREATORS
// template <class T>
// my_PooledArray<T>::my_PooledArray(bslma::Allocator *basicAllocator)
// : d_array_p(basicAllocator)
// , d_pool(sizeof(T), basicAllocator)
// {
// }
// ```
// Since all memory is managed by `d_pool`, we do not have to explicitly invoke
// `deleteObject` to reclaim outstanding memory.  The destructor of the pool
// will automatically deallocate all array elements:
// ```
// template <class T>
// my_PooledArray<T>::~my_PooledArray()
// {
//     // Elements are automatically deallocated when 'd_pool' is destroyed.
// }
// ```
// // MANIPULATORS
// ```
// Note that the overloaded "placement" `new` is used to allocate new nodes:
// ```
// template <class T>
// void my_PooledArray<T>::append(const T& value)
// {
//     T *tmp = new (d_pool) T(value);
//     d_array_p.push_back(tmp);
// }
// ```
// In the `removeAll` method, all elements are deallocated by invoking the
// pool's `release` method.  This technique implies significant performance
// gain when the array contains many elements:
// template <class T>
// inline
// ```
// void my_PooledArray<T>::removeAll()
// {
//     d_array_p.clear();
//     d_pool.release();
// }
//
// // ACCESSORS
// template <class T>
// inline
// int my_PooledArray<T>::length() const
// {
//     return static_cast<int>(d_array_p.size());
// }
//
// template <class T>
// inline
// const T& my_PooledArray<T>::operator[](int index) const
// {
//     assert(0 <= index);
//     assert(index < length());
//
//     return *d_array_p[index];
// }
// ```

#include <bdlscm_version.h>

#include <bdlma_infrequentdeleteblocklist.h>
#include <bslma_allocator.h>
#include <bslma_deleterhelper.h>

#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>
#include <bslmt_platform.h>
#include <bslmt_threadutil.h>

#include <bsls_assert.h>
#include <bsls_atomic.h>
#include <bsls_atomicoperations.h>
#include <bsls_blockgrowth.h>
#include <bsls_compilerfeatures.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#include <bsl_cstddef.h>

namespace BloombergLP {
namespace bdlma {

                           // ====================
                           // class ConcurrentPool
                           // ====================

/// This class implements a memory pool that allocates and manages memory
/// blocks of some uniform size specified at construction.  This memory pool
/// maintains an internal pool of free memory blocks, and dispenses one block
/// for each `allocate` method invocation.  When a memory block is deallocated,
/// it is returned to the pool for potential reuse.
///
/// This class guarantees thread safety while allocating or releasing
/// memory (assuming released memory is no longer accessible outside of this
/// class).  Specifically, allocation and deallocating blocks from the pool is
/// thread-safe.  The underlying allocator used to replenish the pool's memory
/// is used under a lock, allowing for thread-safe use of a non-thread-safe
/// allocator *assuming* the allocator is not used concurrently outside of the
/// pool (including other instances of the pool).  The 'release' method has
/// additional restrictions for thread-safe usage.
class ConcurrentPool {

    // PRIVATE TYPES
    typedef bdlma::InfrequentDeleteBlockList             BlockList;
    typedef bsls::AtomicOperations::AtomicTypes::Uint64  AtomicUint64;
    typedef bsls::AtomicOperations::AtomicTypes::Pointer AtomicPtr;
    typedef bsls::AtomicOperations                       AtomicOp;
    typedef bsls::Types::size_type                       size_type;
    typedef bsls::Types::Uint64                          Uint64;

    /// This class provides a proctor to handle an exception during allocation
    /// in the `allocate` method.
    class AllocateProctor {
        // DATA
        ConcurrentPool *d_pool_p;      // pool being proctored
        Uint64          d_index;       // index in the allocation cache
        Uint64          d_tokenIndex;  // index to place 'd_token'
        void           *d_token;       // value to be placed

      public:
        // CREATORS

        /// Create a proctor for the specified `pool` to, if `release` is not
        /// invoked, assign appropriate values to allow continued operation
        /// after an allocation exception.
        AllocateProctor(ConcurrentPool *pool,
                        Uint64          index,
                        Uint64          tokenIndex,
                        void           *token);

        /// Destroy this proctor.  If `release` was not invoked, assign
        /// appropriate values to allow continued operation after an allocation
        /// exception.
        ~AllocateProctor();

        // MANIPULATORS

        /// Release this proctor.
        void release();
    };

    /// This `struct` implements a link data structure that stores the
    /// address of the next link, and is used to implement the linked lists
    /// of returned memory blocks.
    struct Link {
        void *d_next_p;
    };

    /// This `union` provides an atomic pointer with sufficient padding to
    /// occupy a cache line.
    union PaddedAtomicPtr {
        AtomicPtr d_ptr;
        char      d_pad[bslmt::Platform::e_CACHE_LINE_SIZE];
    };

    /// This `union` provides an atomic integer with sufficient padding to
    /// occupy a cache line.
    union PaddedAtomicUint64 {
        AtomicUint64 d_value;
        char         d_pad[bslmt::Platform::e_CACHE_LINE_SIZE];
    };

    /// This class provides a proctor to replace a value in the allocation
    /// cache.
    class ReplaceValueProctor {
        // DATA
        ConcurrentPool *d_pool_p;  // pool being guarded
        Uint64          d_index;   // index in the allocation cache
        void           *d_value;   // value to load into the allocation cache

      public:
        // CREATORS

        /// Create a proctor for the specified `pool` to, if `release` is not
        /// invoked, loads a specified `value` into the allocation cache at
        /// the specified `index` location.
        ReplaceValueProctor(ConcurrentPool *pool,
                            Uint64          index,
                            void           *value);

        /// Destroy this proctor.  If `release` was not invoked, load the
        /// stored value into the allocation cache at the stored index
        /// location.
        ~ReplaceValueProctor();

        // MANIPULATORS

        /// Release this proctor.
        void release();
    };

    // CONSTANTS
    static const int k_MAX_ALLOC_CACHE_SIZE = 256;  // maximum value for
                                                    // `d_allocCacheSize`

    static const int k_NUM_FREE_LISTS       = 4;    // number of free lists;
                                                    // must be a power of two

    static const Uint64 k_FREE_INDEX_MASK   = k_NUM_FREE_LISTS - 1;

    // DATA
    size_type           d_blockSize;          // size of allocated memory
                                              // returned to client

    size_type           d_internalBlockSize;  // adjusted block size to allow
                                              // for alignment and a free list

    size_type           d_chunkSize;          // blocks per chunk; must be a
                                              // power of two

    size_type           d_allocCacheSize;     // block addresses in cache; must
                                              // be a power of two and at least
                                              // 4 * d_chunkSize

    Uint64              d_allocIndexInChunkMask;
                                              // bitmask to obtain index in
                                              // chunk

    Uint64              d_allocIndexMask;     // bitmask to obtain index in
                                              // `d_allocCache`

    BlockList           d_blockList;          // memory manager; access
                                              // protected by owning the
                                              // "token"

    char                d_pad0[bslmt::Platform::e_CACHE_LINE_SIZE];
                                              // padding to prevent subsequent
                                              // data from being in the same
                                              // cache line as the prior data

    PaddedAtomicPtr     d_reuseCache;         // one element cache of a
                                              // reusable address

    AtomicPtr           d_allocCache[k_MAX_ALLOC_CACHE_SIZE];
                                              // memory addresses available for
                                              // allocation

    char                d_pad1[bslmt::Platform::e_CACHE_LINE_SIZE];
                                              // padding to prevent subsequent
                                              // data from being in the same
                                              // cache line as the prior data

    PaddedAtomicUint64  d_nextAllocIndex;     // next index for taking a value
                                              // from `d_allocCache`

    Uint64              d_reuseIndex;         // index into `d_freeLists` for
                                              // taking returned memory from
                                              // the available lists; access
                                              // protected by owning the
                                              // "token"

    void               *d_reuseList;          // residual values from a
                                              // `d_freeLists`; access
                                              // protected by owning the
                                              // "token"

    char                d_pad2[bslmt::Platform::e_CACHE_LINE_SIZE];
                                              // padding to prevent subsequent
                                              // data from being in the same
                                              // cache line as the prior data

    PaddedAtomicPtr     d_freeLists[k_NUM_FREE_LISTS];
                                              // linked lists of returned
                                              // memory

    PaddedAtomicUint64  d_numAvailable;       // number of available blocks in
                                              // the `d_freeLists` and
                                              // `d_reuseList`; lower bits used
                                              // as index into `d_freeLists`
                                              // for where to return memory

    bslmt::Mutex        d_mutex;              // protects `d_allocCache`
                                              // during `release` and
                                              // `reserveCapacity`

    // PRIVATE MANIPULATORS

    /// Return the address of a contiguous block of memory having the fixed
    /// block size specified at construction.  The behavior is undefined unless
    /// the invocation of this method is not concurrent with an invocation of
    /// `release()`.
    void *allocateWithoutReuseCache();

    /// Initialize this pool.  The behavior is undefined unless
    /// `initializeParameters()` has previously been invoked.
    void initialize();

    /// Initialize the parameters of this pool.  The behavior is undefined
    /// unless `d_blockSize` has already been initialized.
    void initializeParameters();

    /// Obtain exclusive usage of the allocator and return in the specified
    /// `index` and `address` the information necessary to unlock the
    /// allocator.
    void lockAllocator(Uint64 *index, void **address);

    /// Populate the allocation cache chunk, starting with the specified
    /// `index` location, values from the specified `memory` having blocks of
    /// specified `size`, then load into the specified `tokenIndex` location
    /// with the specified `token`.  Specifically, for
    /// `i = 1 .. d_chunkSize-1`, load `memory + i * size` into allocation
    /// cache location `index + i`.
    void populateAllocCache(Uint64                  index,
                            Uint64                  tokenIndex,
                            void                   *token,
                            void                   *memory,
                            bsls::Types::size_type  size);

    /// Release exclusive usage of the allocator and using the information
    /// specified in `index` and `address`.
    void unlockAllocator(Uint64 index, void *address);


    // NOT IMPLEMENTED
    ConcurrentPool(const ConcurrentPool&);
    ConcurrentPool& operator=(const ConcurrentPool&);

  public:
    // CREATORS

    /// Create a memory pool that returns blocks of contiguous memory of the
    /// specified `blockSize` (in bytes) for each `allocate` method
    /// invocation.  Optionally specify a `basicAllocator` used to supply
    /// memory.  If `basicAllocator` is 0, the currently installed default
    /// allocator is used.  The behavior is undefined unless `1 <= blockSize`.
    explicit ConcurrentPool(bsls::Types::size_type  blockSize,
                            bslma::Allocator       *basicAllocator = 0);

    /// Destroy this pool, releasing all associated memory back to the
    /// underlying allocator.
    ~ConcurrentPool();

    // MANIPULATORS

    /// Return the address of a contiguous block of memory having the fixed
    /// block size specified at construction.  The behavior is undefined unless
    /// the invocation of this method is not concurrent with an invocation of
    /// `release()`.
    void *allocate();

    /// Relinquish the memory block at the specified `address` back to this
    /// pool object for reuse.  The behavior is undefined unless `address`
    /// is non-zero, was allocated by this pool, and has not already been
    /// deallocated.
    void deallocate(void *address);

    /// Destroy the specified `object` based on its dynamic type and then
    /// use this pool to deallocate its memory footprint.  This method has
    /// no effect if `object` is 0.  The behavior is undefined unless
    /// `object`, when cast appropriately to `void *`, was allocated using
    /// this pool and has not already been deallocated.  Note that
    /// `dynamic_cast<void *>(object)` is applied if `TYPE` is polymorphic,
    /// and `static_cast<void *>(object)` is applied otherwise.
    template <class TYPE>
    void deleteObject(const TYPE *object);

    /// Destroy the specified `object` and then use this pool to deallocate
    /// its memory footprint.  This method has no effect if `object` is 0.
    /// The behavior is undefined unless `object` is **not** a secondary base
    /// class pointer (i.e., the address is (numerically) the same as when
    /// it was originally dispensed by this pool), was allocated using this
    /// pool, and has not already been deallocated.
    template <class TYPE>
    void deleteObjectRaw(const TYPE *object);

    /// Relinquish all memory currently allocated via this pool object and
    /// return to the underlying allocator memory that was allocated after
    /// construction of this pool object.  The behavior is undefined unless
    /// invocations of this method are not concurrent with uses of `allocate`,
    /// `deallocate`, and `deleteObject*`.
    void release();

    /// Reserve memory from this pool to satisfy memory requests for at
    /// least the specified `numBlocks` before the pool replenishes.  The
    /// behavior is undefined unless `0 <= numBlocks`.
    void reserveCapacity(int numBlocks);

    // ACCESSORS

    /// Return the size (in bytes) of the memory blocks allocated from this
    /// pool object.  Note that all blocks dispensed by this pool have the
    /// same size.
    bsls::Types::size_type blockSize() const;

                                  // Aspects

    /// Return the allocator used by this object to allocate memory.  Note
    /// that this allocator can not be used to deallocate memory
    /// allocated through this pool.
    bslma::Allocator *allocator() const;

    // DEPRECATED METHODS

#ifndef BDE_OMIT_INTERNAL_DEPRECATED  // BDE4.40
    /// Create a memory pool that returns blocks of contiguous memory of the
    /// specified `blockSize` (in bytes) for each `allocate` method
    /// invocation.  Optionally specify a `growthStrategy`, which is ignored.
    /// Optionally specify `maxBlocksPerChunk`, which is ignored.  Optionally
    /// specify a `basicAllocator` used to supply memory.  If `basicAllocator`
    /// is 0, the currently installed default allocator is used.  The behavior
    /// is undefined unless `1 <= blockSize`.
    ///
    /// @DEPRECATED: Use `ConcurrentPool(size_type, allocator)` instead.
    ConcurrentPool(bsls::Types::size_type       blockSize,
                   bsls::BlockGrowth::Strategy  growthStrategy,
                   bslma::Allocator            *basicAllocator = 0);
    ConcurrentPool(bsls::Types::size_type       blockSize,
                   bsls::BlockGrowth::Strategy  growthStrategy,
                   int                          maxBlocksPerChunk,
                   bslma::Allocator            *basicAllocator = 0);
#endif  // BDE_OMIT_INTERNAL_DEPRECATED -- BDE4.40

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

/// Return a block of memory of the specified `size` (in bytes) allocated
/// from the specified `pool`.  The behavior is undefined unless `size` is
/// the same or smaller than the `blockSize` with which `pool` was
/// constructed.  Note that an object may allocate additional memory
/// internally, requiring the allocator to be passed in as a constructor
/// argument:
///..
///  my_Type *newMyType(bdlma::ConcurrentPool *pool,
///                     bslma::Allocator      *basicAllocator)
///  {
///      return new (*pool) my_Type(..., basicAllocator);
///  }
///..
/// Also note that the analogous version of 'operator delete' should not be
/// called directly.  Instead, this component provides a static template
/// member function, 'deleteObject', parameterized by 'TYPE':
///..
///  void deleteMyType(my_Type *t, bdlma::ConcurrentPool *pool)
///  {
///      pool->deleteObject(t);
///  }
///..
/// 'deleteObject' performs the following:
///..
///  t->~my_Type();
///  pool->deallocate(t);
///..
void *operator new(bsl::size_t size, BloombergLP::bdlma::ConcurrentPool& pool);

/// Use the specified `pool` to deallocate the memory at the specified
/// `address`.  The behavior is undefined unless `address` was allocated
/// using `pool` and has not already been deallocated.  This operator is
/// supplied solely to allow the compiler to arrange for it to be called in
/// case of an exception.  Client code should not call it; use
/// `bdlma::ConcurrentPool::deleteObject()` instead.
inline
void operator delete(void *address, BloombergLP::bdlma::ConcurrentPool& pool);

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

namespace BloombergLP {
namespace bdlma {

                  // -------------------------------------
                  // class ConcurrentPool::AllocateProctor
                  // -------------------------------------

// CREATORS
inline
ConcurrentPool::AllocateProctor::AllocateProctor(ConcurrentPool *pool,
                                                 Uint64          index,
                                                 Uint64          tokenIndex,
                                                 void           *token)
: d_pool_p(pool)
, d_index(index)
, d_tokenIndex(tokenIndex)
, d_token(token)
{
}

inline
ConcurrentPool::AllocateProctor::~AllocateProctor()
{
    if (d_pool_p) {
        // use the pool's address to indicate an exception occurred
        d_pool_p->populateAllocCache(d_index,
                                     d_tokenIndex,
                                     d_token,
                                     static_cast<void *>(d_pool_p),
                                     0);
    }
}

// MANIPULATORS
inline
void ConcurrentPool::AllocateProctor::release()
{
    d_pool_p = 0;
}

                // ------------------------------------------
                // class ConcurrentPool::ReplaceValueProctor
                // ------------------------------------------

// CREATORS
inline
ConcurrentPool::ReplaceValueProctor::ReplaceValueProctor(ConcurrentPool *pool,
                                                         Uint64          index,
                                                         void           *value)
: d_pool_p(pool)
, d_index(index)
, d_value(value)
{
}

inline
ConcurrentPool::ReplaceValueProctor::~ReplaceValueProctor()
{
    if (d_pool_p) {
        bsls::AtomicOperations::setPtrRelease(&d_pool_p->d_allocCache[d_index],
                                              d_value);
    }
}

// MANIPULATORS
inline
void ConcurrentPool::ReplaceValueProctor::release()
{
    d_pool_p = 0;
}

                           // --------------------
                           // class ConcurrentPool
                           // --------------------

// PRIVATE MANIPULATORS
inline
void ConcurrentPool::unlockAllocator(Uint64 index, void *address)
{
    AtomicOp::swapPtrAcqRel(&d_allocCache[index], address);
}

// MANIPULATORS
inline
void *ConcurrentPool::allocate()
{
    // attempt reuse from reuse cache
    void *address = AtomicOp::swapPtrAcqRel(&d_reuseCache.d_ptr, 0);

    return (0 != address ? address : allocateWithoutReuseCache());
}

inline
void ConcurrentPool::deallocate(void *address)
{
    // attempt to place in reuse cache
    if (0 == AtomicOp::testAndSwapPtrAcqRel(&d_reuseCache.d_ptr, 0, address)) {
        return;                                                       // RETURN
    }

    // place into a free list
    Uint64 index = (  (AtomicOp::addUint64NvAcqRel(&d_numAvailable.d_value,
                                                   1) - 1)
                    & k_FREE_INDEX_MASK);

    AtomicPtr *list = &d_freeLists[index].d_ptr;

    void *oldList = AtomicOp::getPtrAcquire(list);
    void *prevOldList;
    do {
        prevOldList = oldList;
        static_cast<Link *>(address)->d_next_p = oldList;
        oldList = AtomicOp::testAndSwapPtrAcqRel(list, oldList, address);
    } while (oldList != prevOldList);
}

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
    // synchronize with `reserveCapacity` and other `release`
    bslmt::LockGuard<bslmt::Mutex> lockGuard(&d_mutex);

    // release memory
    d_blockList.release();

    initialize();
}

// ACCESSORS
inline
bsls::Types::size_type ConcurrentPool::blockSize() const
{
    return d_blockSize;
}

// Aspects

inline
bslma::Allocator *ConcurrentPool::allocator() const
{
    return d_blockList.allocator();
}

}  // close package namespace
}  // close enterprise namespace

// FREE OPERATORS
inline
void *operator new(bsl::size_t size, BloombergLP::bdlma::ConcurrentPool& pool)
{
    BSLS_ASSERT_SAFE(size <= pool.blockSize());

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
// Copyright 2026 Bloomberg Finance L.P.
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
