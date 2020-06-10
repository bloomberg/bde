// bdlma_smallsequentialpool.h                                        -*-C++-*-
#ifndef INCLUDED_BDLMA_SMALLSEQUENTIALPOOL
#define INCLUDED_BDLMA_SMALLSEQUENTIALPOOL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide sequential memory using dynamically-allocated buffers.
//
//@CLASSES:
//   bdlma::SmallSequentialPool: memory pool of dynamically-allocated buffers
//
//@SEE_ALSO: bdlma_sequentialallocator, bdlma_infrequentdeleteblocklist,
//           bdlma_sequentialpool, bdlma_sequentialallocator
//
//@DESCRIPTION: This component provides a fast sequential memory pool,
// 'bdlma::SmallSequentialPool', that dispenses heterogeneous memory blocks (of
// varying, user-specified sizes) from a dynamically-allocated internal buffer.
// A 'bdlma::SmallSequentialPool' is typically used when fast allocation and
// deallocation is needed, but the user does not know in advance the maximum
// amount of memory needed.  The class rates the appellation "small" because
// its footprint is significantly less than that of 'bdlma::SequentialPool', a
// similar class that uses a different policy for its 'rewind' method.  See
// {Comparison: 'bdlma::SequentialPool' and 'bdlma::SmallSequentialPool'}.
//
// If an allocation request exceeds the remaining free memory space in the
// internal buffer, the pool either replenishes its internal buffer with new
// memory to satisfy the request, or returns a separate memory block, depending
// on whether the request size exceeds an optionally-specified threshold.
// Allocations that exceed that threshold are deemed "large" blocks.  See
// {Optional 'maxBufferSize' Parameter} and {Optional 'growthStrategy'
// Parameter} for details of specifying allocation such thresholds.
//
// The 'release' method releases all memory allocated through the pool, as does
// the destructor.  Note that individually allocated memory blocks cannot be
// separately deallocated.
//
// The 'rewind' method releases all memory allocated through the pool *except*
// for the last non-"large" block, if any.  Using 'rewind' instead of
// 'release' can save on reallocations when a pool object is being reused.
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
// supplied at construction to specify the maximum buffer size by geometric
// growth.  The pool meet requests in excess of 'maxBufferSize' by allocating
// (and returning) a "large" block of the requested size.  The behavior is
// undefined unless 'intialSize <= maxBufferSize'.  Note that 'reserveCapacity'
// always ensures that the requested number of bytes is available (allocating a
// new internal buffer if necessary) regardless of whether the size of the
// request exceeds 'maxBufferSize'.
//
///Optional 'growthStrategy' Parameter
///-----------------------------------
// An optional 'growthStrategy' parameter can be supplied at construction to
// specify the growth rate of the dynamically-allocated buffers.  See
// {'bsls_blockgrowth'} for more details.  The buffers can grow either
// geometrically or remain constant in size.  If 'growthStrategy' is not
// specified, geometric growth is used.  If the constant size growth strategy
// is chosen, the pool always allocates internal buffers of the specified
// 'initialSize' (see {Optional 'initialSize' Parameter}) or an
// implementation-defined value.  Each pool request in excess of the constant
// buffer size is satisfied by allocating (and returning) a "large" block.
//
///Optional 'alignmentStrategy' Parameter
///--------------------------------------
// An optional 'alignmentStrategy' parameter can be supplied at construction to
// specify the memory alignment strategy.  Allocated memory blocks can either
// follow maximum alignment, natural alignment, or 1-byte alignment.  If
// 'alignmentStrategy' is not specified, natural alignment is used.  See
// {'bsls_alignment'} for more details.
//
///Comparison: 'bdlma::SequentialPool' and 'bdlma::SmallSequentialPool'
///--------------------------------------------------------------------
// The two classes, 'bdlma::SequentialPool' and 'bdlma::SmallSequentialPool',
// offer identical interfaces and nearly identical semantics.  The sole
// difference is that 'bslma::SequentialPool' class retains across calls to the
// 'rewind' method all memory blocks allocated under the growth strategy
// specified on construction whereas the much-smaller-footprint
// 'bdlma::SmallSequentialPool' class retains only the last memory block
// allocated under the growth strategy and releases the rest.  Note that the
// last block allocated is as large or larger than all previously allocated
// blocks.  Both classes release all allocated blocks that exceeded the growth
// strategy (i.e., "large" blocks).
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using 'bdlma::SmallSequentialPool' for Efficient Allocations
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Some applications require the allocation of many small blocks of memory to
// hold small amounts of data, pointers, etc.  Allocating small blocks from a
// global allocator can cause problems.  For example, a thread lock may be
// required before access to the global allocator.  Also, many small
// allocations may lead to fragmentation of the global allocator.  A memory
// pool can be used to replace many small allocations (from the global
// allocator) to a smaller number of larger allocations.
//
// Suppose we define a container class, 'my_IntDoubleArray', that holds both
// 'int' and 'double' values.  The class can be implemented using two parallel
// arrays: one storing the type information, and the other storing pointers to
// the 'int' and 'double' values.  For efficient memory allocation, we can use
// a 'bdlma::SmallSequentialPool' for memory allocation:
//
// First, we define 'my_IntDoubleArray' class:
//..
//  // my_intdoublearray.h
//
//  class my_IntDoubleArray {
//      // This class implements an efficient container for an array that
//      // stores both 'int' and 'double' values.
//
//      // PRIVATE TYPES
//      enum  Type { k_MY_INT, k_MY_DOUBLE };
//
//      union Data {
//                   int    d_intValue;
//                   double d_doubleValue;
//                 };
//
//      // DATA
//      char  *d_typeArray_p;   // array indicating the type of corresponding
//                              // values stored in 'd_dataArray_p'
//
//      Data **d_dataArray_p;   // array of pointers to the values stored
//
//      int    d_length;        // number of values stored
//
//      bdlma::SmallSequentialPool
//             d_pool;          // sequential memory pool used to supply memory
//
//    private:
//      // PRIVATE MANIPULATORS
//      void incrementCapacity();
//          // Increase the capacity of the internal arrays used to store
//          // elements by one element.
//
//      // NOT IMPLEMENTED
//      my_IntDoubleArray(const my_IntDoubleArray&) BSLS_KEYWORD_DELETED;
//
//    public:
//
//      // CREATORS
//      explicit my_IntDoubleArray(bslma::Allocator *basicAllocator = 0);
//          // Create an 'int'-'double' array.  Optionally specify a
//          // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
//          // 0, the currently installed default allocator is used.
//
//      // ...
//
//      ~my_IntDoubleArray();
//          // Destroy this array and all elements held by it.
//
//      // MANIPULATORS
//      void appendDouble(double value);
//          // Append the specified 'double' 'value' to this array.
//
//      void appendInt(int value);
//          // Append the specified 'int' 'value' to this array.
//
//      void removeAll();
//          // Remove all elements from this array.
//
//      // ...
//
//      // ACCESSORS
//      int numElements() const;
//          // Return the number of elements in this array.
//
//      // ...
//  };
//..
// Then, we define the (straightforward) inline methods of the class.
//..
//  // CREATORS
//  inline
//  my_IntDoubleArray::~my_IntDoubleArray()
//  {
//      assert(0 <= d_length);
//  }
//
//  // MANIPULATORS
//  inline
//  void my_IntDoubleArray::removeAll()
//  {
//      d_pool.rewind();
//      d_length = 0;
//  }
//
//  // ...
//
//  // ACCESSORS
//  inline
//  int my_IntDoubleArray::numElements() const
//  {
//      return d_length;
//  }
//
//  // ...
//..
// Next, we define the non-inlined methods of the class.  Note that our
// implementation is *not* sparing the number of requests for memory from the
// pool.  It makes many requests for small-ish blocks of memory.  Notably:
//: o A new "type" and "data" array is allocated for each appended value, and
//: o each data item is stored in a separately allocated node.
//..
//  // my_intdoublearray.cpp
//
//  // PRIVATE MANIPULATORS
//  void my_IntDoubleArray::incrementCapacity()
//  {
//      int    newLength    = d_length + 1;
//      char  *tmpTypeArray = static_cast<char *>(
//                       d_pool.allocate(newLength * sizeof *d_typeArray_p));
//      Data **tmpDataArray  = static_cast<Data **>(
//                       d_pool.allocate(newLength * sizeof *d_dataArray_p));
//
//      bsl::memcpy(tmpTypeArray,
//                  d_typeArray_p,
//                  d_length * sizeof *d_typeArray_p);
//      bsl::memcpy(tmpDataArray,
//                  d_dataArray_p,
//                  d_length * sizeof *d_dataArray_p);
//
//      d_typeArray_p = tmpTypeArray;
//      d_dataArray_p = tmpDataArray;
//
//      // Note that 'd_length' is updates in the calling functions.
//  }
//..
// Notice that we do nothing above clean up the old arrays because:
//: 1 The array types have trivial destructors (that need not be called).
//: 2 The memory will be recovered when the pool is destroyed by the destructor
//:   of 'my_IntDoubleArray'.  Note that the pool does *not* even provide a
//:   'deallocate' method.
//..
//  // CREATORS
//  my_IntDoubleArray::my_IntDoubleArray(bslma::Allocator *basicAllocator)
//  : d_length(0)
//  , d_pool(basicAllocator)
//  {
//  }
//
//  // MANIPULATORS
//  void my_IntDoubleArray::appendDouble(double value)
//  {
//      incrementCapacity();
//
//      Data *item = static_cast<Data *>(d_pool.allocate(sizeof *item));
//
//      item->d_doubleValue = value;
//
//      d_typeArray_p[d_length] = static_cast<char>(k_MY_DOUBLE);
//      d_dataArray_p[d_length] = item;
//
//      ++d_length;
//  }
//
//  void my_IntDoubleArray::appendInt(int value)
//  {
//      incrementCapacity();
//
//      Data *item = static_cast<Data *>(d_pool.allocate(sizeof *item));
//
//      item->d_intValue = value;
//
//      d_typeArray_p[d_length] = static_cast<char>(k_MY_INT);
//      d_dataArray_p[d_length] = item;
//
//      ++d_length;
//  }
//..
// Now, we can use the 'my_IntDoubleArray' class.  For purposes of
// illustration, we use the class in concert with a 'bslma::TestAllocator' so
// we can observe the interactions between our object and the global allocator.
//..
//  void useMyIntDoubleArray()
//      // Demonstrate some of the characteristic behaviors of the
//      // 'my_IntDoubleArray' class.
//  {
//      bslma::TestAllocator sa("supplied");
//
//      {
//          my_IntDoubleArray obj(&sa);
//          assert(0 == obj.numElements());
//          assert(0 == sa.numBlocksInUse());
//
//          obj.appendInt(42);
//          assert(1 == obj.numElements());
//          assert(1 == sa.numBlocksInUse());
//..
// Then, we observe that appending a value triggers the allocation of a block
// from the allocator.
//..
//          obj.appendDouble(355.0/113.0);
//          assert(2 == obj.numElements());
//          assert(1 == sa.numBlocksInUse());
//
//          obj.appendDouble(666);
//          assert(3 == obj.numElements());
//          assert(1 == sa.numBlocksInUse());
//..
// Next, we observe that appending additional elements does not require the
// allocation of additional blocks from the allocator.  The pool services the
// many subsequent data needs (new internal arrays and data nodes) out of the
// first block from the allocator.
//
// Then, we remove all of the elements from the array and observe that the pool
// retained that block for future use.  Note that our implementation of
// 'removeAll' invokes the 'rewind' (not the 'release') method of the pool.
//..
//          obj.removeAll();
//          assert(0 == obj.numElements());
//          assert(1 == sa.numBlocksInUse());
//..
// Next, we observe that appending additional several elements does not require
// calls to the allocator.  Again, the pool handles these memory needs using
// the original block.
//..
//          obj.appendInt   (1  );
//          obj.appendDouble(2.0);
//          obj.appendInt   (3  );
//          obj.appendDouble(4.0);
//
//          assert(4 == obj.numElements());
//          assert(1 == sa.numBlocksInUse());
//      }
//
//      assert(0 == sa.numBlocksInUse());
//  }
//..
// Finally, we observe that destruction of the pool, a step in destroying the
// array object, deallocates the block that was allocated in the first append
// operation.
//
///Example 2: Implementing an Allocator Using 'bdlma::SmallSequentialPool'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Although 'bslma::SmallSequentialPool' provides an 'allocate' method, it
// cannot be used as an allocator (mechanism) unless it is wrapped by a class
// that provides the full 'bslma::Allocator' protocol.  The class below,
// 'my_SmallSequentialAllocator' shows how that its done.  Note that the class
// below is a simplified version of that defined in
// {'bslma_smallsequentialallocator'}.
//
// First, we define the 'my_SmallSequentialAllocator' class, taking care to
// inherit from the protocol class:
//..
//  class my_SmallSequentialAllocator : public bslma::Allocator {
//      // This class implements the 'bslma::Allocator' protocol to provide a
//      // fast allocator of heterogeneous blocks of memory (of varying,
//      // user-specified sizes) from dynamically-allocated internal buffers.
//
//      // DATA
//      bdlma::SmallSequentialPool d_pool;  // manager for allocated memory
//                                          // blocks
//
//    public:
//      // CREATORS
//      explicit my_SmallSequentialAllocator(
//                                       bslma::Allocator *basicAllocator = 0);
//          // Create an allocator for allocating memory blocks from
//          // dynamically-allocated internal buffers.  Optionally specify a
//          // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
//          // 0, the currently installed default allocator is used.
//
//      ~my_SmallSequentialAllocator();
//          // Destroy this allocator.  All memory allocated from this
//          // allocator is released.
//
//      // MANIPULATORS
//      void *allocate(size_type size) BSLS_KEYWORD_OVERRIDE;
//          // Return the address of a contiguous block of memory of the
//          // specified 'size' (in bytes).
//
//      void deallocate(void *address) BSLS_KEYWORD_OVERRIDE;
//          // This method has no effect on the memory block at the specified
//          // 'address' as all memory allocated by this allocator is managed.
//          // The behavior is undefined unless 'address' was allocated by this
//          // allocator, and has not already been deallocated.
//  };
//..
// Then, we define the methods of the class.  All are small 'inline' wrappers
// that forward to the pool data member.
//..
//  // CREATORS
//  inline
//  my_SmallSequentialAllocator::my_SmallSequentialAllocator(
//                                            bslma::Allocator *basicAllocator)
//  : d_pool(basicAllocator)
//  {
//  }
//
//  inline
//  my_SmallSequentialAllocator::~my_SmallSequentialAllocator()
//  {
//      // Memory is released by the pool's destructor.
//  }
//
//  // MANIPULATORS
//  inline
//  void *my_SmallSequentialAllocator::allocate(size_type size)
//  {
//      return d_pool.allocate(size);
//  }
//
//  inline
//  void my_SmallSequentialAllocator::deallocate(void *)
//  {
//      // A no-op: The pool does not provide a 'deallocate' method.
//  }
//..
// Now, we can use our allocator class and confirm that its behavior is
// consistent with that of 'bslma::SmallSequentialPool'.  As in {Example 1}, we
// use a 'bslma::TestAllocator' to allow us to observe the interactions with
// the global allocator.
//..
//  void useMySmallSequentialAllocator()
//      // Demonstrate some of the characteristic behaviors of the
//      // 'my_SmallSequentialAllocator' class.
//  {
//      bslma::TestAllocator        ta("backup");
//
//      {
//          my_SmallSequentialAllocator ssa(&ta);
//
//          {
//              assert(0 == ta.numBlocksInUse());
//
//..
// Next, we observe that created a 'bsl::string' object that uses a
// 'my_SmallSequentialAllocator' object for memory and of length that exceeds
// the small string optimization (an implementation detail of 'bsl::string)
// triggers the allocation of a single member block from the "backup"
// allocator.
//..
//              bsl::string greeting("Hello word!  How are you today?",
//                                   &ssa);
//              assert(bsl::string().capacity() < greeting.size());
//              assert(1 == ta.numBlocksInUse());
//..
// Then, we observe that creating a second such string (using the same
// allocator object) does not require an additional allocation from the backup
// allocator.  The pool in our allocator object is able to satisfy this request
// from its original allocation.
//..
//              bsl::string response("I am fine.  Thank you for asking.",
//                                   &ssa);
//              assert(bsl::string().capacity() < response.size());
//              assert(1 == ta.numBlocksInUse());
//          }
//..
// Next, we observe that destroying these strings does return the allocated
// memory to the backup allocator.  Recall that the 'deallocate' method of our
// allocator object is a no-op.
//..
//          assert(1 == ta.numBlocksInUse());
//      }
//..
// Finally, we observe the memory being returned when our allocator object (and
// the pool it contains) is destroyed.
//..
//      assert(0 == ta.numBlocksInUse());
//  }
//..

#include <bdlscm_version.h>

#include <bdlma_buffermanager.h>
#include <bdlma_infrequentdeleteblocklist.h>

#include <bslma_allocator.h>

#include <bsls_alignment.h>
#include <bsls_assert.h>
#include <bsls_blockgrowth.h>
#include <bsls_performancehint.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#include <bsl_cstddef.h>  // 'bsl::size_t'

namespace BloombergLP {
namespace bdlma {

                           // =========================
                           // class SmallSequentialPool
                           // =========================

class SmallSequentialPool {
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

    bsl::size_t         d_initialSize;     // initial internal buffer size

    bsl::size_t         d_maxBufferSize;   // maximum internal buffer size

    InfrequentDeleteBlockList
                        d_blockList;       // memory manager used to supply
                                           // dynamically-allocated memory
                                           // blocks

    InfrequentDeleteBlockList
                        d_largeBlockList;  // memory manager used to supply
                                           // dynamically-allocated memory
                                           // blocks exceeding growth strategy
  private:
    // NOT IMPLEMENTED
    SmallSequentialPool(const SmallSequentialPool&);             // = delete
    SmallSequentialPool& operator=(const SmallSequentialPool&);  // = delete

  private:
    // PRIVATE MANIPULATORS
    void *allocateNonFastPath(bsl::size_t size);
        // If the specified 'size' is not 0, use the allocator supplied at
        // construction to allocate a new internal buffer and return the
        // address of a contiguous block of memory of 'size' (in bytes) from
        // this new buffer, according to the alignment strategy specified at
        // construction.  If 'size' is 0, no memory is allocated and 0 is
        // returned.

    // PRIVATE ACCESSORS
    bsl::size_t calculateNextBufferSize(bsl::size_t size) const;
        // Return the next buffer size (in bytes) that is sufficiently large to
        // satisfy a memory allocation request of the specified 'size' (in
        // bytes), or the maximum buffer size if the buffer can no longer grow.

  public:
    // CREATORS
    explicit
    SmallSequentialPool(bslma::Allocator            *basicAllocator = 0);
    explicit
    SmallSequentialPool(bsls::BlockGrowth::Strategy  growthStrategy,
                        bslma::Allocator            *basicAllocator = 0);
    explicit
    SmallSequentialPool(bsls::Alignment::Strategy    alignmentStrategy,
                        bslma::Allocator            *basicAllocator = 0);
    SmallSequentialPool(bsls::BlockGrowth::Strategy  growthStrategy,
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
    SmallSequentialPool(int                          initialSize,
                        bslma::Allocator            *basicAllocator = 0);
    SmallSequentialPool(bsl::size_t                  initialSize,
                        bsls::BlockGrowth::Strategy  growthStrategy,
                        bslma::Allocator            *basicAllocator = 0);
    SmallSequentialPool(bsl::size_t                  initialSize,
                        bsls::Alignment::Strategy    alignmentStrategy,
                        bslma::Allocator            *basicAllocator = 0);
    SmallSequentialPool(bsl::size_t                  initialSize,
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

    SmallSequentialPool(bsl::size_t                  initialSize,
                        bsl::size_t                  maxBufferSize,
                        bslma::Allocator            *basicAllocator = 0);
    SmallSequentialPool(bsl::size_t                  initialSize,
                        bsl::size_t                  maxBufferSize,
                        bsls::BlockGrowth::Strategy  growthStrategy,
                        bslma::Allocator            *basicAllocator = 0);
    SmallSequentialPool(bsl::size_t                  initialSize,
                        bsl::size_t                  maxBufferSize,
                        bsls::Alignment::Strategy    alignmentStrategy,
                        bslma::Allocator            *basicAllocator = 0);
    SmallSequentialPool(bsl::size_t                  initialSize,
                        bsl::size_t                  maxBufferSize,
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

    SmallSequentialPool(bsl::size_t                  initialSize,
                        bsl::size_t                  maxBufferSize,
                        bsls::BlockGrowth::Strategy  growthStrategy,
                        bsls::Alignment::Strategy    alignmentStrategy,
                        bool                         allocateInitialBuffer,
                        bslma::Allocator            *basicAllocator = 0);
        // Create a sequential pool for allocating memory blocks from a
        // sequence of dynamically-allocated buffers, of which the initial
        // buffer has the specified 'initialSize' (in bytes), the internal
        // buffer growth is limited to the specified 'maxBufferSize', the
        // specified 'growthStrategy' is used to control buffer growth, and the
        // specified 'alignmentStrategy' is used to control alignment of
        // allocated memory blocks.  Allocate the initial buffer only if the
        // specified 'allocateInitialBuffer' is 'true'.  Optionally specify a
        // 'basicAllocator' used to supply memory for the dynamically-allocated
        // buffers.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The behavior is undefined unless
        // '0 < initialSize' and 'initialSize <= maxBufferSize'.  Note that
        // when constant growth is used, the size of the internal buffers will
        // always be the same as 'initialSize'.

    ~SmallSequentialPool();
        // Destroy this sequential pool.  All memory allocated by this pool is
        // released.

    // MANIPULATORS
    void *allocate(bsl::size_t size);
        // Return the address of a contiguous block of memory of the specified
        // 'size' (in bytes) according to the alignment strategy specified at
        // construction.  If 'size' is 0, no memory is allocated and 0 is
        // returned.  If the allocation request exceeds the remaining free
        // memory space in the current internal buffer, use the allocator
        // supplied at construction to allocate a new internal buffer, then
        // allocate memory from the new buffer.

    void *allocateAndExpand(bsl::size_t *size);
        // Return the address of a contiguous block of memory of at least the
        // specified '*size' (in bytes), and load the actual amount of memory
        // allocated in '*size'.  If '*size' is 0, return 0 with no effect.  If
        // the allocation request exceeds the remaining free memory space in
        // the current internal buffer, use the allocator supplied at
        // construction to allocate a new internal buffer, then allocate memory
        // from the new buffer.

    template <class TYPE>
    void deleteObject(const TYPE *object);
        // Destroy the specified 'object'.  Note that this method has the same
        // effect as the 'deleteObjectRaw' method (since no deallocation is
        // involved), and exists for consistency across pools.

    template <class TYPE>
    void deleteObjectRaw(const TYPE *object);
        // Destroy the specified 'object'.  Note that memory associated with
        // 'object' is not deallocated because there is no 'deallocate' method
        // in 'SmallSequentialPool'.

    void release();
        // Release all memory allocated through this pool.  The pool is reset
        // to its default-constructed state, retaining the alignment and growth
        // strategies, and the initial and maximum buffer sizes in effect
        // following construction.  The effect of using a pointer after this
        // call that was obtained before this call from this object is
        // undefined.

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
        // request from this memory pool, and otherwise has no effect.  The
        // behavior is undefined unless the memory at 'address' was originally
        // allocated by this memory pool, the size of the memory block at
        // 'address' is 'originalSize', 'newSize <= originalSize', and
        // 'release' was not called after allocating the memory block at
        // 'address'.
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
//                        BloombergLP::bdlma::SmallSequentialPool& pool);
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
void *operator new(bsl::size_t                              size,
                   BloombergLP::bdlma::SmallSequentialPool& pool);
    // Return a block of memory of the specified 'size' (in bytes) allocated
    // from the specified 'pool'.  Note that an object may allocate additional
    // memory internally, requiring the allocator to be passed in as a
    // constructor argument:
    //..
    //  my_Type *newMyType(bdlma::SmallSequentialPool *pool,
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
    //  void deleteMyType(bdlma::SmallSequentialPool *pool, my_Type *t)
    //  {
    //      t->~my_Type();
    //  }
    //..

void operator delete(void                                     *address,
                     BloombergLP::bdlma::SmallSequentialPool&  pool);
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

                           // -------------------------
                           // class SmallSequentialPool
                           // -------------------------

// CREATORS
inline
SmallSequentialPool::~SmallSequentialPool()
{
    d_blockList.release();
    d_largeBlockList.release();
}

// MANIPULATORS
inline
void *SmallSequentialPool::allocate(bsl::size_t size)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(d_buffer.buffer())) {
        void *result = d_buffer.allocate(size);
        if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(result)) {
            return result;                                            // RETURN
        }
    }

    return allocateNonFastPath(size);
}

inline
void *SmallSequentialPool::allocateAndExpand(bsl::size_t *size)
{
    BSLS_ASSERT(size);

    void *result = allocate(*size);

    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(result)) {
        *size = d_buffer.expand(result, *size);
    }

    return result;
}

template <class TYPE>
inline
void SmallSequentialPool::deleteObject(const TYPE *object)
{
    deleteObjectRaw(object);
}

template <class TYPE>
inline
void SmallSequentialPool::deleteObjectRaw(const TYPE *object)
{
    if (0 != object) {
#ifndef BSLS_PLATFORM_CMP_SUN
        object->~TYPE();
#else
        const_cast<TYPE *>(object)->~TYPE();
#endif
    }
}

inline
void SmallSequentialPool::release()
{
    // 'BufferManager::release' keeps the buffer and just resets the internal
    // cursor, so 'reset' is used instead.

    d_buffer.reset();

    d_blockList.release();
    d_largeBlockList.release();
}

inline
void SmallSequentialPool::rewind()
{
    d_buffer.release();  // Reset the internal cursor in the current block.
    d_blockList.releaseAllButLastBlock();
    d_largeBlockList.release();
}

inline
bsl::size_t SmallSequentialPool::truncate(void        *address,
                                          bsl::size_t  originalSize,
                                          bsl::size_t  newSize)
{
    BSLS_ASSERT_SAFE(address);
    BSLS_ASSERT_SAFE(newSize <= originalSize);

    return d_buffer.truncate(address, originalSize, newSize);
}

}  // close package namespace
}  // close enterprise namespace

// FREE OPERATORS
inline
void *operator new(bsl::size_t                              size,
                   BloombergLP::bdlma::SmallSequentialPool& pool)
{
    return pool.allocate(size);
}

inline
void operator delete(void                                     *,
                     BloombergLP::bdlma::SmallSequentialPool&  )
{
    // NOTE: there is no deallocation from this allocation mechanism.
}

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
