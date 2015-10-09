// bdlma_sequentialpool.h                                             -*-C++-*-
#ifndef INCLUDED_BDLMA_SEQUENTIALPOOL
#define INCLUDED_BDLMA_SEQUENTIALPOOL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide sequential memory using dynamically-allocated buffers.
//
//@CLASSES:
//   bdlma::SequentialPool: memory pool using dynamically-allocated buffers
//
//@SEE_ALSO: bdlma_infrequentdeleteblocklist, bdlma_sequentialallocator
//
//@DESCRIPTION: This component provides a fast sequential memory pool,
// 'bdlma::SequentialPool', that dispenses heterogeneous memory blocks (of
// varying, user-specified sizes) from a dynamically-allocated internal buffer.
// If an allocation request exceeds the remaining free memory space in the
// internal buffer, the pool either replenishes its buffer with new memory to
// satisfy the request, or returns a separate memory block, depending on
// whether the request size exceeds an optionally-specified maximum buffer
// size.  The 'release' method releases all memory allocated through the pool,
// as does the destructor.  Note that individually allocated memory blocks
// cannot be separately deallocated.
//
// A 'bdlma::SequentialPool' is typically used when fast allocation and
// deallocation is needed, but the user does not know in advance the maximum
// amount of memory needed.
//
///Optional 'initialSize' Parameter
///--------------------------------
// An optional 'initialSize' parameter can be supplied at construction to
// specify the initial size of the internal buffer.  If 'initialSize' is not
// supplied, an implementation-defined value is used for the initial internal
// size of the buffer.
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
// follow maximum alignment, natural alignment, or 1-byte alignment.  If
// 'alignmentStrategy' is not specified, natural alignment is used.  See
// 'bsls_alignment' for more details.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using 'bdlma::SequentialPool' for Efficient Allocations
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we define a container class, 'my_IntDoubleArray', that holds both
// 'int' and 'double' values.  The class can be implemented using two parallel
// arrays: one storing the type information, and the other storing pointers to
// the 'int' and 'double' values.  For efficient memory allocation, we can use
// a 'bdlma::SequentialPool' for memory allocation:
//..
//  // my_intdoublearray.h
//
//  class my_IntDoubleArray {
//      // This class implements an efficient container for an array that
//      // stores both 'int' and 'double' values.
//
//      // DATA
//      char  *d_typeArray_p;   // array indicating the type of corresponding
//                              // values stored in 'd_valueArray_p'
//
//      void **d_valueArray_p;  // array of pointers to the values stored
//
//      int    d_length;        // number of values stored
//
//      int    d_capacity;      // physical capacity of the type and value
//                              // arrays
//
//      bdlma::SequentialPool
//             d_pool;          // sequential memory pool used to supply memory
//
//    private:
//      // PRIVATE MANIPULATORS
//      void increaseSize();
//          // Increase the capacity of the internal arrays used to store
//          // elements added to this array by at least one element.
//
//      // Not implemented:
//      my_IntDoubleArray(const my_IntDoubleArray&);
//
//    public:
//      // TYPES
//      enum Type { k_MY_INT, k_MY_DOUBLE };
//
//      // CREATORS
//      explicit my_IntDoubleArray(bslma::Allocator *basicAllocator = 0);
//          // Create an 'int'-'double' array.  Optionally specify a
//          // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
//          // 0, the currently installed default allocator is used.
//
//      ~my_IntDoubleArray();
//          // Destroy this array and all elements held by it.
//
//      // ...
//
//      // MANIPULATORS
//      void appendInt(int value);
//          // Append the specified 'int' 'value' to this array.
//
//      void appendDouble(double value);
//          // Append the specified 'double' 'value' to this array.
//
//      void removeAll();
//          // Remove all elements from this array.
//
//      // ...
//  };
//..
// The use of a sequential pool and the 'release' method allows the 'removeAll'
// method to quickly deallocate memory of all elements:
//..
//  // MANIPULATORS
//  inline
//  void my_IntDoubleArray::removeAll()
//  {
//      d_pool.release();
//      d_length = 0;
//  }
//..
// The sequential pool optimizes the allocation of memory by using
// dynamically-allocated buffers to supply memory.  This greatly reduces the
// amount of dynamic allocation needed:
//..
//  // my_intdoublearray.cpp
//
//  enum { k_INITIAL_SIZE = 1 };
//
//  // PRIVATE MANIPULATORS
//  void my_IntDoubleArray::increaseSize()
//  {
//      // Implementation elided.
//      // ...
//  }
//
//  // CREATORS
//  my_IntDoubleArray::my_IntDoubleArray(bslma::Allocator *basicAllocator)
//  : d_length(0)
//  , d_capacity(k_INITIAL_SIZE)
//  , d_pool(basicAllocator)
//  {
//      d_typeArray_p  = static_cast<char *>(
//                       d_pool.allocate(d_capacity * sizeof *d_typeArray_p));
//      d_valueArray_p = static_cast<void **>(
//                       d_pool.allocate(d_capacity * sizeof *d_valueArray_p));
//  }
//..
// Note that in the destructor, all outstanding memory blocks are deallocated
// automatically when 'd_pool' is destroyed:
//..
//  my_IntDoubleArray::~my_IntDoubleArray()
//  {
//      assert(0 <= d_length);
//      assert(0 <= d_capacity);
//      assert(d_length <= d_capacity);
//  }
//
//  // MANIPULATORS
//  void my_IntDoubleArray::appendInt(int value)
//  {
//      if (d_length >= d_capacity) {
//          increaseSize();
//      }
//
//      int *item = static_cast<int *>(d_pool.allocate(sizeof *item));
//      *item = value;
//
//      d_typeArray_p[d_length]  = static_cast<char>(k_MY_INT);
//      d_valueArray_p[d_length] = item;
//
//      ++d_length;
//  }
//
//  void my_IntDoubleArray::appendDouble(double value)
//  {
//      if (d_length >= d_capacity) {
//          increaseSize();
//      }
//
//      double *item = static_cast<double *>(d_pool.allocate(sizeof *item));
//      *item = value;
//
//      d_typeArray_p[d_length]  = static_cast<char>(k_MY_DOUBLE);
//      d_valueArray_p[d_length] = item;
//
//      ++d_length;
//  }
//..
//
///Example 2: Implementing an Allocator Using 'bdlma::SequentialPool'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 'bslma::Allocator' is used throughout the interfaces of BDE components.
// Suppose we would like to create a fast allocator, 'my_FastAllocator', that
// allocates memory from a buffer in a similar fashion to
// 'bdlma::SequentialPool'.  'bdlma::SequentialPool' can be used directly to
// implement such an allocator.
//
// Note that the documentation for this class is simplified for this usage
// example.  Please see 'bdlma_sequentialallocator' for full documentation of a
// similar class.
//..
//  class my_SequentialAllocator : public bslma::Allocator {
//      // This class implements the 'bslma::Allocator' protocol to provide a
//      // fast allocator of heterogeneous blocks of memory (of varying,
//      // user-specified sizes) from dynamically-allocated internal buffers.
//
//      // DATA
//      bdlma::SequentialPool d_pool;  // memory manager for allocated memory
//                                     // blocks
//
//    public:
//      // CREATORS
//      explicit my_SequentialAllocator(bslma::Allocator *basicAllocator = 0);
//          // Create an allocator for allocating memory blocks from
//          // dynamically-allocated internal buffers.  Optionally specify a
//          // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
//          // 0, the currently installed default allocator is used.
//
//      ~my_SequentialAllocator();
//          // Destroy this allocator.  All memory allocated from this
//          // allocator is released.
//
//      // MANIPULATORS
//      virtual void *allocate(size_type size);
//          // Return the address of a contiguous block of memory of the
//          // specified 'size' (in bytes).
//
//      virtual void deallocate(void *address);
//          // This method has no effect on the memory block at the specified
//          // 'address' as all memory allocated by this allocator is managed.
//          // The behavior is undefined unless 'address' was allocated by this
//          // allocator, and has not already been deallocated.
//  };
//
//  // CREATORS
//  inline
//  my_SequentialAllocator::my_SequentialAllocator(
//                                            bslma::Allocator *basicAllocator)
//  : d_pool(basicAllocator)
//  {
//  }
//
//  inline
//  my_SequentialAllocator::~my_SequentialAllocator()
//  {
//      d_pool.release();
//  }
//
//  // MANIPULATORS
//  inline
//  void *my_SequentialAllocator::allocate(size_type size)
//  {
//      return d_pool.allocate(size);
//  }
//
//  inline
//  void my_SequentialAllocator::deallocate(void *)
//  {
//  }
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLMA_BUFFERMANAGER
#include <bdlma_buffermanager.h>
#endif

#ifndef INCLUDED_BDLMA_INFREQUENTDELETEBLOCKLIST
#include <bdlma_infrequentdeleteblocklist.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
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

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_CSTDDEF
#include <bsl_cstddef.h>
#endif

namespace BloombergLP {
namespace bdlma {

                           // ====================
                           // class SequentialPool
                           // ====================

class SequentialPool {
    // This class implements a fast memory pool that efficiently dispenses
    // heterogeneous blocks of memory (of varying, user-specified sizes) from a
    // sequence of dynamically-allocated internal buffers.  Memory for the
    // internal buffers is supplied by an (optional) allocator supplied at
    // construction; if no allocator is supplied, the currently installed
    // default allocator is used.  If an allocation exceeds the remaining free
    // memory space in the current buffer, the pool replenishes its internal
    // buffer with new memory to satisfy the request.  This class is
    // *exception* *neutral*: If memory cannot be allocated, the behavior is
    // defined by the (optional) allocator specified at construction.

    // DATA
    BufferManager       d_buffer;          // memory manager for current buffer

    bsls::BlockGrowth::Strategy
                        d_growthStrategy;  // growth strategy for block list

    int                 d_initialSize;     // initial internal buffer size

    int                 d_maxBufferSize;   // maximum internal buffer size

    InfrequentDeleteBlockList
                        d_blockList;       // memory manager used to supply
                                           // dynamically-allocated memory
                                           // blocks

  private:
    // NOT IMPLEMENTED
    SequentialPool(const SequentialPool&);
    SequentialPool& operator=(const SequentialPool&);

  private:
    // PRIVATE ACCESSORS
    int calculateNextBufferSize(int size) const;
        // Return the next buffer size (in bytes) that is sufficiently large to
        // satisfy a memory allocation request of the specified 'size' (in
        // bytes), or the maximum buffer size if the buffer can no longer grow.

  public:
    // CREATORS
    explicit
    SequentialPool(bslma::Allocator *basicAllocator = 0);
    explicit
    SequentialPool(bsls::BlockGrowth::Strategy  growthStrategy,
                   bslma::Allocator            *basicAllocator = 0);
    explicit
    SequentialPool(bsls::Alignment::Strategy  alignmentStrategy,
                   bslma::Allocator          *basicAllocator = 0);
    SequentialPool(bsls::BlockGrowth::Strategy  growthStrategy,
                   bsls::Alignment::Strategy    alignmentStrategy,
                   bslma::Allocator            *basicAllocator = 0);
        // Create a sequential pool for allocating memory blocks from a
        // sequence of dynamically-allocated buffers.  Optionally specify a
        // 'basicAllocator' used to supply memory for the dynamically-allocated
        // buffers.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  Optionally specify a 'growthStrategy' used to
        // control buffer growth.  If no 'growthStrategy' is specified,
        // geometric growth is used.  Optionally specify an 'alignmentStrategy'
        // used to control alignment of allocated memory blocks.  If no
        // 'alignmentStrategy' is specified, natural alignment is used.  An
        // implementation-defined value is used as the initial size of the
        // internal buffer.  Note that no limit is imposed on the size of the
        // internal buffers when geometric growth is used.  Also note that when
        // constant growth is used, the size of the internal buffers will
        // always be the same as the implementation-defined value.

    explicit
    SequentialPool(int initialSize, bslma::Allocator *basicAllocator = 0);
    SequentialPool(int                          initialSize,
                   bsls::BlockGrowth::Strategy  growthStrategy,
                   bslma::Allocator            *basicAllocator = 0);
    SequentialPool(int                        initialSize,
                   bsls::Alignment::Strategy  alignmentStrategy,
                   bslma::Allocator          *basicAllocator = 0);
    SequentialPool(int                          initialSize,
                   bsls::BlockGrowth::Strategy  growthStrategy,
                   bsls::Alignment::Strategy    alignmentStrategy,
                   bslma::Allocator            *basicAllocator = 0);
        // Create a sequential pool for allocating memory blocks from a
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
        // construction of a sequential pool will incur a memory allocation.
        // The behavior is undefined unless '0 < initialSize'.  Note that no
        // limit is imposed on the size of the internal buffers when geometric
        // growth is used.  Also note that when constant growth is used, the
        // size of the internal buffers will always be the same as
        // 'initialSize'.

    SequentialPool(int               initialSize,
                   int               maxBufferSize,
                   bslma::Allocator *basicAllocator = 0);
    SequentialPool(int                          initialSize,
                   int                          maxBufferSize,
                   bsls::BlockGrowth::Strategy  growthStrategy,
                   bslma::Allocator            *basicAllocator = 0);
    SequentialPool(int                        initialSize,
                   int                        maxBufferSize,
                   bsls::Alignment::Strategy  alignmentStrategy,
                   bslma::Allocator          *basicAllocator = 0);
    SequentialPool(int                          initialSize,
                   int                          maxBufferSize,
                   bsls::BlockGrowth::Strategy  growthStrategy,
                   bsls::Alignment::Strategy    alignmentStrategy,
                   bslma::Allocator            *basicAllocator = 0);
        // Create a sequential pool for allocating memory blocks from a
        // sequence of dynamically-allocated buffers, of which the initial
        // buffer has the specified 'initialSize' (in bytes), and the internal
        // buffer growth is limited to the specified 'maxBufferSize'.
        // Optionally specify a 'basicAllocator' used to supply memory for the
        // dynamically-allocated buffers.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.  Optionally specify a
        // 'growthStrategy' used to control buffer growth.  If no
        // 'growthStrategy' is specified, geometric growth is used.  Optionally
        // specify an 'alignmentStrategy' used to control alignment of
        // allocated memory blocks.  If no 'alignmentStrategy' is specified,
        // natural alignment is used.  The behavior is undefined unless
        // '0 < initialSize' and 'initialSize <= maxBufferSize'.  Note that
        // when constant growth is used, the size of the internal buffers will
        // always be the same as 'initialSize'.

    ~SequentialPool();
        // Destroy this sequential pool.  All memory allocated by this pool is
        // released.

    // MANIPULATORS
    void *allocate(bsls::Types::size_type size);
        // Return the address of a contiguous block of memory of the specified
        // 'size' (in bytes) according to the alignment strategy specified at
        // construction.  If the allocation request exceeds the remaining free
        // memory space in the current internal buffer, use the allocator
        // supplied at construction to allocate a new internal buffer, then
        // allocate memory from the new buffer.  The behavior is undefined
        // unless '0 < size'.

    void *allocateAndExpand(bsls::Types::size_type *size);
        // Return the address of a contiguous block of memory of at least the
        // specified '*size' (in bytes), and load the actual amount of memory
        // allocated in '*size'.  If the allocation request exceeds the
        // remaining free memory space in the current internal buffer, use the
        // allocator supplied at construction to allocate a new internal
        // buffer, then allocate memory from the new buffer.  The behavior is
        // undefined unless '0 < *size'.

    template <class TYPE>
    void deleteObjectRaw(const TYPE *object);
        // Destroy the specified 'object'.  Note that memory associated with
        // 'object' is not deallocated because there is no 'deallocate' method
        // in 'SequentialPool'.

    template <class TYPE>
    void deleteObject(const TYPE *object);
        // Destroy the specified 'object'.  Note that this method has the same
        // effect as the 'deleteObjectRaw' method (since no deallocation is
        // involved), and exists for consistency across pools.

    void release();
        // Release all memory allocated through this pool.  The pool is reset
        // to its default-constructed state, retaining the alignment and growth
        // strategies, and the initial and maximum buffer sizes in effect
        // following construction.

    void reserveCapacity(int numBytes);
        // Reserve sufficient memory to satisfy allocation requests for at
        // least the specified 'numBytes' without replenishment (i.e., without
        // dynamic allocation).  This method ignores 'maxBufferSize' even if it
        // is supplied at construction.  The behavior is undefined unless
        // '0 < numBytes'.  Note that, due to alignment effects, it is possible
        // that not all 'numBytes' of memory will be used for allocation before
        // triggering dynamic allocation.

    int truncate(void *address, int originalSize, int newSize);
        // Reduce the amount of memory allocated at the specified 'address' of
        // the specified 'originalSize' (in bytes) to the specified 'newSize'.
        // Return 'newSize' after truncating, or 'originalSize' if the memory
        // block at 'address' cannot be truncated.  This method can only
        // 'truncate' the memory block returned by the most recent 'allocate'
        // request from this memory pool, and otherwise has no effect.  The
        // behavior is undefined unless the memory at 'address' was originally
        // allocated by this memory pool, the size of the memory block at
        // 'address' is 'originalSize', 'newSize <= originalSize',
        // '0 <= newSize', and 'release' was not called after allocating the
        // memory block at 'address'.
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
// overloaded the array version of operator 'new':
//..
//   void *operator new[](bsl::size_t                         size,
//                        BloombergLP::bdlma::SequentialPool& pool);
//..
// The user of the pool class would have expected to be able to use operator
// 'new' as follows:
//..
//   new (*pool) my_Type[...];
//..
// The problem is that this expression returns an array that cannot be safely
// deallocated.  On the one hand, there is no syntax in C++ to invoke an
// overloaded 'operator delete'; on the other hand, the pointer returned by
// operator 'new' cannot be passed to the 'deallocate' method directly because
// the pointer is different from the one returned by the 'allocate' method.
// The compiler offsets the value of this pointer by a header, which is used to
// maintain the number of objects in the array (so that the 'operator delete'
// can destroy the right number of objects).

// FREE OPERATORS
void *operator new(bsl::size_t size, BloombergLP::bdlma::SequentialPool& pool);
    // Return a block of memory of the specified 'size' (in bytes) allocated
    // from the specified 'pool'.  Note that an object may allocate additional
    // memory internally, requiring the allocator to be passed in as a
    // constructor argument:
    //..
    //  my_Type *newMyType(bdlma::SequentialPool *pool,
    //                     bslma::Allocator      *basicAllocator)
    //  {
    //      return new (*pool) my_Type(..., basicAllocator);
    //  }
    //..
    // Also note that the analogous version of operator 'delete' should not be
    // called directly.  Instead, this component provides a static template
    // member function, 'deleteObject', parameterized by 'TYPE' that performs
    // the following:
    //..
    //  void deleteMyType(bdlma::SequentialPool *pool, my_Type *t)
    //  {
    //      t->~my_Type();
    //  }
    //..

void operator delete(void *address, BloombergLP::bdlma::SequentialPool& pool);
    // Use the specified 'pool' to deallocate the memory at the specified
    // 'address'.  The behavior is undefined unless 'address' was allocated
    // using 'pool' and has not already been deallocated.  This operator is
    // supplied solely to allow the compiler to arrange for it to be called in
    // case of an exception.

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

namespace BloombergLP {
namespace bdlma {

                           // --------------------
                           // class SequentialPool
                           // --------------------

// CREATORS
inline
SequentialPool::~SequentialPool()
{
    d_blockList.release();
}

// MANIPULATORS
template <class TYPE>
inline
void SequentialPool::deleteObjectRaw(const TYPE *object)
{
    if (0 != object) {
#ifndef BSLS_PLATFORM_CMP_SUN
        object->~TYPE();
#else
        const_cast<TYPE *>(object)->~TYPE();
#endif
    }
}

template <class TYPE>
inline
void SequentialPool::deleteObject(const TYPE *object)
{
    deleteObjectRaw(object);
}

inline
void SequentialPool::release()
{
    // 'BufferManager::release' keeps the buffer and just resets the internal
    // cursor, so 'reset' is used instead.

    d_buffer.reset();

    d_blockList.release();
}

inline
int SequentialPool::truncate(void *address, int originalSize, int newSize)
{
    BSLS_ASSERT_SAFE(address);
    BSLS_ASSERT_SAFE(0 <= newSize);
    BSLS_ASSERT_SAFE(newSize <= originalSize);

    return d_buffer.truncate(address, originalSize, newSize);
}

}  // close package namespace
}  // close enterprise namespace

// FREE OPERATORS
inline
void *operator new(bsl::size_t size, BloombergLP::bdlma::SequentialPool& pool)
{
    return pool.allocate(size);
}

inline
void operator delete(void *, BloombergLP::bdlma::SequentialPool&)
{
    // NOTE: there is no deallocation from this allocation mechanism.
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
