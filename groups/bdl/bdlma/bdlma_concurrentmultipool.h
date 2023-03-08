// bdlma_concurrentmultipool.h                                        -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLMA_CONCURRENTMULTIPOOL
#define INCLUDED_BDLMA_CONCURRENTMULTIPOOL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a memory manager to manage pools of varying block sizes.
//
//@CLASSES:
//   bdlma::ConcurrentMultipool: memory manager that manages pools of blocks
//
//@SEE_ALSO: bdlma_concurrentpool, bdlmca_multipoolallocator
//
//@DESCRIPTION: This component implements a memory manager,
// 'bdlma::ConcurrentMultipool', that maintains a configurable number of
// 'bdlma::ConcurrentPool' objects, each dispensing memory blocks of a unique
// size.  The 'bdlma::ConcurrentPool' objects are placed in an array, starting
// at index 0, with each successive pool managing memory blocks of a size twice
// that of the previous pool.  Each multipool allocation (deallocation) request
// allocates memory from (returns memory to) the internal pool managing memory
// blocks of the smallest size not less than the requested size, or else from a
// separately managed list of memory blocks, if no internal pool managing
// memory block of sufficient size exists.  Both the 'release' method and the
// destructor of a 'bdlma::ConcurrentMultipool' release all memory currently
// allocated via the object.
//
// A 'bdlma::ConcurrentMultipool' can be depicted visually:
//..
//                    +-----+--- memory blocks of 8 bytes
//                    |     |
//   ========       ----- ----- ------------
//  |8 bytes |---->|     |     |     ...    |
//  >========<      =====^=====^============
//  |16 bytes|
//  >========<      \___________ __________/
//  |32 bytes|                  V
//  >========<              a "chunk"
//  |        |
//  |  ...   |
//  |        |
//   ========
//      |
//      +------- array of 'bdlma::ConcurrentPool'
//..
// Note that a "chunk" is a large, contiguous block of memory, internal to a
// 'bdlma::ConcurrentPool' maintained by the multipool, from which memory
// blocks of uniform size are dispensed to users.
//
///Thread Safety
///-------------
// 'bdlma::ConcurrentMultipool' is *fully thread-safe*, meaning any operation
// on the same object can be safely invoked from any thread.
//
///Configuration at Construction
///-----------------------------
// When creating a 'bdlma::ConcurrentMultipool', clients can optionally
// configure:
//
//: 1 NUMBER OF POOLS -- the number of internal pools (the block size managed
//:   by the first pool is eight bytes, with each successive pool managing
//:   block of a size twice that of the previous pool).
//:
//: 2 GROWTH STRATEGY -- geometrically growing chunk size starting from 1 (in
//:   terms of the number of memory blocks per chunk), or fixed chunk size,
//:   specified as either:
//:   o the unique growth strategy for all pools, or
//:   o (if the number of pools is specified) an array of growth strategies
//:     corresponding to each individual pool
//:   If the growth strategy is not specified, geometric growth is used for all
//:   pools.
//:
//: 3 MAX BLOCKS PER CHUNK -- the maximum number of memory blocks within a
//:   chunk, specified as either:
//:     o the unique maximum-blocks-per-chunk value for all of the pools, or
//:     o an array of maximum-blocks-per-chunk values corresponding to each
//:       individual pool.
//:   If the maximum blocks per chunk is not specified, an
//:   implementation-defined default value is used.  Note that the maximum
//:   blocks per chunk can be configured only if the number of pools is also
//:   configured.
//:
//: 4 BASIC ALLOCATOR -- the allocator used to supply memory (to replenish an
//:   internal pool, or directly if the maximum block size is exceeded).  If
//:   not specified, the currently installed default allocator (see
//:   'bslma_default') is used.
//
// A default-constructed multipool has a relatively small,
// implementation-defined number of pools 'N' with respective block sizes
// ranging from '2^3 = 8' to '2^(N+2)'.  By default, the initial chunk size,
// (i.e., the number of blocks of a given size allocated at once to replenish a
// pool's memory) is 1, and each pool's chunk size grows geometrically until it
// reaches an implementation-defined maximum, at which it is capped.  Finally,
// unless otherwise specified, all memory comes from the allocator that was the
// currently installed default allocator at the time the
// 'bdlma::ConcurrentMultipool' was created.
//
// Using the various pooling options described above, we can configure the
// number of pools maintained, whether replenishment should be adaptive (i.e.,
// geometric starting with 1) or fixed at a maximum chunk size, what that
// maximum chunk size should be (which need not be an integral power of 2), and
// the underlying allocator used to supply memory.  Note that both GROWTH
// STRATEGY and MAX BLOCKS PER CHUNK can be specified separately either as a
// single value applying to all of the maintained pools, or as an array of
// values, with the elements applying to each individually maintained pool.
//
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using a 'bdlma::ConcurrentMultipool' Directly
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// A 'bdlma::ConcurrentMultipool' can be used by containers that hold different
// types of elements, each of uniform size, for efficient memory allocation of
// new elements.  Suppose we have a factory class, 'my_MessageFactory', that
// creates messages based on user requests.  Each message is created with the
// most efficient memory storage possible - using predefined 8-byte, 16-byte
// and 32-byte buffers.  If the message size exceeds the three predefined
// values, a generic message is used.  For efficient memory allocation of
// messages, we use a 'bdlma::ConcurrentMultipool'.
//
// First, we define our message types as follows:
//..
//  class my_MessageFactory;
//
//  class my_Message {
//      // This class represents a general message interface that provides a
//      // 'getMessage' method for clients to retrieve the underlying message.
//
//    public:
//      // ACCESSORS
//      virtual const char *getMessage() = 0;
//          // Return the null-terminated message string.
//  };
//
//  class my_SmallMessage : public my_Message {
//      // This class represents an 8-byte message (including null terminator).
//
//      // DATA
//      char d_buffer[8];
//
//      // FRIEND
//      friend class my_MessageFactory;
//
//      // NOT IMPLEMENTED
//      my_SmallMessage(const my_SmallMessage&);
//      my_SmallMessage& operator=(const my_SmallMessage&);
//
//      // PRIVATE CREATORS
//      my_SmallMessage(const char *msg, int length)
//      {
//          assert(length <= 7);
//
//          bsl::memcpy(d_buffer, msg, length);
//          d_buffer[length] = '\0';
//      }
//
//      // PRIVATE ACCESSORS
//      virtual const char *getMessage()
//      {
//          return d_buffer;
//      }
//  };
//
//  class my_MediumMessage : public my_Message {
//      // This class represents a 16-byte message (including null
//      // terminator).
//
//      // DATA
//      char d_buffer[16];
//
//      // FRIEND
//      friend class my_MessageFactory;
//
//      // NOT IMPLEMENTED
//      my_MediumMessage(const my_MediumMessage&);
//      my_MediumMessage& operator=(const my_MediumMessage&);
//
//      // PRIVATE CREATORS
//      my_MediumMessage(const char *msg, int length)
//      {
//          assert(length <= 15);
//
//          bsl::memcpy(d_buffer, msg, length);
//          d_buffer[length] = '\0';
//      }
//
//      // PRIVATE ACCESSORS
//      virtual const char *getMessage()
//      {
//          return d_buffer;
//      }
//  };
//
//  class my_LargeMessage : public my_Message {
//      // This class represents a 32-byte message (including null
//      // terminator).
//
//      // DATA
//      char d_buffer[32];
//
//      // FRIEND
//      friend class my_MessageFactory;
//
//      // NOT IMPLEMENTED
//      my_LargeMessage(const my_LargeMessage&);
//      my_LargeMessage& operator=(const my_LargeMessage&);
//
//      // PRIVATE CREATORS
//      my_LargeMessage(const char *msg, int length)
//      {
//          assert(length <= 31);
//
//          bsl::memcpy(d_buffer, msg, length);
//          d_buffer[length] = '\0';
//      }
//
//      // PRIVATE ACCESSORS
//      virtual const char *getMessage()
//      {
//          return d_buffer;
//      }
//  };
//
//  class my_GenericMessage : public my_Message {
//      // This class represents a generic message.
//
//      // DATA
//      char *d_buffer;
//
//      // FRIEND
//      friend class my_MessageFactory;
//
//      // NOT IMPLEMENTED
//      my_GenericMessage(const my_GenericMessage&);
//      my_GenericMessage& operator=(const my_GenericMessage&);
//
//      // PRIVATE CREATORS
//      my_GenericMessage(char *msg) : d_buffer(msg)
//      {
//      }
//
//      // PRIVATE ACCESSORS
//      virtual const char *getMessage()
//      {
//          return d_buffer;
//      }
//  };
//..
// Then we define our factory class, 'my_MessageFactory', as follows:
//..
//  class my_MessageFactory {
//      // This class implements an efficient message factory that builds and
//      // returns messages.  The life-time of the messages created by this
//      // factory is the same as this factory.
//
//      // DATA
//      bdlma::ConcurrentMultipool d_multipool;  // multipool used to supply
//                                               // memory
//
//    private:
//      // Not implemented:
//      my_MessageFactory(const my_MessageFactory&);
//
//    public:
//      // CREATORS
//      my_MessageFactory(bslma::Allocator *basicAllocator = 0);
//          // Create a message factory.  Optionally specify a 'basicAllocator'
//          // used to supply memory.  If 'basicAllocator' is 0, the currently
//          // installed default allocator is used.
//
//      ~my_MessageFactory();
//          // Destroy this factory and reclaim all messages created by it.
//
//      // MANIPULATORS
//      my_Message *createMessage(const char *data);
//          // Create a message storing the specified 'data'.  The behavior is
//          // undefined unless 'data' is null-terminated.
//
//      void disposeAllMessages();
//          // Dispose of all created messages.
//
//      void disposeMessage(my_Message *message);
//          // Dispose of the specified 'message'.  The behavior is undefined
//          // unless 'message' was created by this factory.
//  };
//..
// The use of a multipool and the 'release' method enables the
// 'disposeAllMessages' method to quickly deallocate all memory blocks used to
// create messages:
//..
//  // MANIPULATORS
//  inline
//  void my_MessageFactory::disposeAllMessages()
//  {
//      d_multipool.release();
//  }
//..
// The multipool can also reuse deallocated memory.  Once a message is
// destroyed by the 'disposeMessage' method, memory allocated for that message
// is reclaimed by the multipool and can be used to create the next message
// having the same size:
//..
//  inline
//  void my_MessageFactory::disposeMessage(my_Message *message)
//  {
//      d_multipool.deleteObject(message);
//  }
//..
// A multipool optimizes the allocation of memory by using
// dynamically-allocated buffers (also known as chunks) to supply memory.  As
// each chunk can satisfy multiple memory block requests before requiring
// additional dynamic memory allocation, the number of dynamic allocation
// requests needed is greatly reduced.
//
// For the number of pools managed by the multipool, we chose to use the
// implementation-defined default value instead of calculating and specifying a
// value.  Note that if users want to specify the number of pools, the value
// can be calculated as the smallest 'N' such that the following relationship
// holds:
//..
//  N > log2(sizeof(Object Type)) - 2
//..
// Continuing on with the usage example:
//..
//  // CREATORS
//  my_MessageFactory::my_MessageFactory(bslma::Allocator *basicAllocator)
//  : d_multipool(basicAllocator)
//  {
//  }
//..
// Note that in the destructor, all outstanding messages are reclaimed
// automatically when 'd_multipool' is destroyed:
//..
//  my_MessageFactory::~my_MessageFactory()
//  {
//  }
//..
// A 'bdlma::ConcurrentMultipool' is ideal for allocating the different sized
// messages since repeated deallocations might be necessary (which renders a
// 'bdlma::SequentialPool' unsuitable) and the sizes of these types are all
// different:
//..
//  // MANIPULATORS
//  my_Message *my_MessageFactory::createMessage(const char *data)
//  {
//      enum { k_SMALL = 8, k_MEDIUM = 16, k_LARGE = 32 };
//
//      const int length = static_cast<int>(bsl::strlen(data));
//
//      if (length < k_SMALL) {
//          return new(d_multipool.allocate(sizeof(my_SmallMessage)))
//                                    my_SmallMessage(data, length);  // RETURN
//      }
//
//      if (length < k_MEDIUM) {
//          return new(d_multipool.allocate(sizeof(my_MediumMessage)))
//                                   my_MediumMessage(data, length);  // RETURN
//      }
//
//      if (length < k_LARGE) {
//          return new(d_multipool.allocate(sizeof(my_LargeMessage)))
//                                    my_LargeMessage(data, length);  // RETURN
//      }
//
//      char *buffer = (char *)d_multipool.allocate(length + 1);
//      bsl::memcpy(buffer, data, length + 1);
//
//      return new(d_multipool.allocate(sizeof(my_GenericMessage)))
//                                                   my_GenericMessage(buffer);
//  }
//..
//
///Example 2: Implementing an Allocator Using 'bdlma::ConcurrentMultipool'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 'bslma::Allocator' is used throughout the interfaces of BDE components.
// Suppose we would like to create a multipool allocator,
// 'my_MultipoolAllocator', that allocates memory from multiple
// 'bdlma::ConcurrentPool' objects in a similar fashion to
// 'bdlma::ConcurrentMultipool'.  This class can be used directly to implement
// such an allocator.
//
// Note that the documentation for this class is simplified for this usage
// example.  Please see 'bdlmca_multipoolallocator' for full documentation of a
// similar class.
//..
//  class my_MultipoolAllocator : public bslma::Allocator{
//      // This class implements the 'bslma::Allocator' protocol to provide an
//      // allocator that manages a set of memory pools, each dispensing memory
//      // blocks of a unique size, with each successive pool's block size
//      // being twice that of the previous one.
//
//      // DATA
//      bdlma::ConcurrentMultipool d_multiPool;  // memory manager for
//                                               // allocated memory blocks
//
//    public:
//      // CREATORS
//      my_MultipoolAllocator(bslma::Allocator *basicAllocator = 0);
//          // Create a multipool allocator.  Optionally specify a
//          // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
//          // 0, the currently installed default allocator is used.
//
//      // ...
//
//      virtual ~my_MultipoolAllocator();
//          // Destroy this multipool allocator.  All memory allocated from
//          // this memory pool is released.
//
//      // MANIPULATORS
//      virtual void *allocate(bsls::Types::size_type size);
//          // Return the address of a contiguous block of maximally-aligned
//          // memory of (at least) the specified 'size' (in bytes).  If 'size'
//          // is 0, no memory is allocated and 0 is returned.
//
//      virtual void deallocate(void *address);
//          // Relinquish the memory block at the specified 'address' back to
//          // this multipool allocator for reuse.  The behavior is undefined
//          // unless 'address' is non-zero, was allocated by this multipool
//          // allocator, and has not already been deallocated.
//  };
//
//  // CREATORS
//  inline
//  my_MultipoolAllocator::my_MultipoolAllocator(
//                                            bslma::Allocator *basicAllocator)
//  : d_multiPool(basicAllocator)
//  {
//  }
//
//  my_MultipoolAllocator::~my_MultipoolAllocator()
//  {
//  }
//
//  // MANIPULATORS
//  inline
//  void *my_MultipoolAllocator::allocate(bsls::Types::size_type size)
//  {
//      return d_multiPool.allocate(size);
//  }
//
//  inline
//  void my_MultipoolAllocator::deallocate(void *address)
//  {
//      d_multiPool.deallocate(address);
//  }
//..

#include <bdlscm_version.h>

#include <bdlma_concurrentallocatoradapter.h>
#include <bdlma_blocklist.h>

#include <bslma_allocator.h>
#include <bslma_deleterhelper.h>

#include <bslmt_mutex.h>

#include <bsls_alignmentutil.h>
#include <bsls_blockgrowth.h>
#include <bsls_types.h>

namespace BloombergLP {
namespace bdlma {

class ConcurrentPool;

                        // =========================
                        // class ConcurrentMultipool
                        // =========================

class ConcurrentMultipool {
    // This class implements a memory manager that maintains a configurable
    // number of 'bdlma::Pool' objects, each dispensing memory blocks of a
    // unique size.  The 'Pool' objects are placed in an array, with each
    // successive pool managing memory blocks of size twice that of the
    // previous pool.  Each multipool allocation (deallocation) request
    // allocates memory from (returns memory to) the internal pool having the
    // smallest block size not less than the requested size, or, if no pool
    // manages memory blocks of sufficient sized, from a separately managed
    // list of memory blocks.  Both the 'release' method and the destructor of
    // a 'bdema::Multipool' release all memory currently allocated via the
    // object.

    // PRIVATE TYPES
    struct Header {
        // This 'struct' provides header information for each allocated memory
        // block.  The header stores the index to the pool used for the memory
        // allocation.

        union {
            int                    d_poolIdx;  // pool used for this memory
                                               // block

            bsls::AlignmentUtil::MaxAlignedType
                                   d_dummy;    // force maximum alignment
        } d_header;
    };

    // DATA
    ConcurrentPool   *d_pools_p;       // array of memory pools, each
                                       // dispensing fixed-size memory blocks

    int               d_numPools;      // number of memory pools

    bsls::Types::size_type
                      d_maxBlockSize;  // largest memory block size; dispensed
                                       // by the 'd_numPools - 1'th pool;
                                       // always a power of 2

    bdlma::BlockList  d_blockList;     // memory manager for "large" memory
                                       // blocks.

    bslmt::Mutex      d_mutex;         // synchronize data access

    ConcurrentAllocatorAdapter
                      d_allocAdapter;  // thread-safe adapter

  private:
    // NOT IMPLEMENTED
    ConcurrentMultipool(const ConcurrentMultipool&);
    ConcurrentMultipool& operator=(const ConcurrentMultipool&);

   private:
    // PRIVATE MANIPULATORS
    void initialize(bsls::BlockGrowth::Strategy growthStrategy,
                    int                         maxBlocksPerChunk);
    void initialize(const bsls::BlockGrowth::Strategy *growthStrategyArray,
                    int                                maxBlocksPerChunk);
    void initialize(bsls::BlockGrowth::Strategy  growthStrategy,
                    const int                   *maxBlocksPerChunkArray);
    void initialize(const bsls::BlockGrowth::Strategy *growthStrategyArray,
                    const int                         *maxBlocksPerChunkArray);
        // Initialize this multipool with the specified 'growthStrategy[Array]'
        // and 'maxBlocksPerChunk[Array]'.  If an array is used, each
        // individual 'bdlma::Pool' maintained by this multipool is initialized
        // with the corresponding growth strategy or max blocks per chunk entry
        // within the array.

    // PRIVATE ACCESSORS
    int findPool(bsls::Types::size_type size) const;
        // Return the index of the memory pool in this multipool for an
        // allocation request of the specified 'size' (in bytes).  Note that
        // the index of the memory pool managing memory blocks having the
        // minimum block size is 0.

  public:
    // CREATORS
    explicit ConcurrentMultipool(bslma::Allocator *basicAllocator = 0);
    explicit ConcurrentMultipool(int               numPools,
                                 bslma::Allocator *basicAllocator = 0);
    explicit ConcurrentMultipool(
                              bsls::BlockGrowth::Strategy  growthStrategy,
                              bslma::Allocator            *basicAllocator = 0);
    ConcurrentMultipool(int                          numPools,
                        bsls::BlockGrowth::Strategy  growthStrategy,
                        bslma::Allocator            *basicAllocator = 0);
    ConcurrentMultipool(int                          numPools,
                        bsls::BlockGrowth::Strategy  growthStrategy,
                        int                          maxBlocksPerChunk,
                        bslma::Allocator            *basicAllocator = 0);
        // Create a multipool memory manager.  Optionally specify 'numPools',
        // indicating the number of internally created 'Pool' objects; the
        // block size of the first pool is 8 bytes, with the block size of each
        // additional pool successively doubling.  If 'numPools' is not
        // specified, an implementation-defined number of pools 'N' -- covering
        // memory blocks ranging in size from '2^3 = 8' to '2^(N+2)' -- are
        // created.  Optionally specify a 'growthStrategy' indicating whether
        // the number of blocks allocated at once for every internally created
        // 'Pool' should be either fixed or grow geometrically, starting with
        // 1.  If 'growthStrategy' is not specified, the allocation strategy
        // for each internally created 'Pool' object is geometric, starting
        // from 1.  If 'numPools' is specified, optionally specify a
        // 'maxBlocksPerChunk', indicating the maximum number of blocks to be
        // allocated at once when a pool must be replenished.  If
        // 'maxBlocksPerChunk' is not specified, an implementation-defined
        // value is used.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  Memory allocation (and deallocation) requests
        // will be satisfied using the internally maintained pool managing
        // memory blocks of the smallest size not less than the requested size,
        // or directly from the underlying allocator (supplied at
        // construction), if no internally pool managing memory block of
        // sufficient size exists.  The behavior is undefined unless
        // '1 <= numPools' and '1 <= maxBlocksPerChunk'.  Note that, on
        // platforms where '8 < bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT',
        // excess memory may be allocated for pools managing smaller blocks.
        // Also note that 'maxBlocksPerChunk' need not be an integral power of
        // 2; if geometric growth would exceed the maximum value, the chunk
        // size is capped at that value).

    ConcurrentMultipool(int                                numPools,
                        const bsls::BlockGrowth::Strategy *growthStrategyArray,
                        bslma::Allocator                  *basicAllocator = 0);
    ConcurrentMultipool(int                                numPools,
                        const bsls::BlockGrowth::Strategy *growthStrategyArray,
                        int                                maxBlocksPerChunk,
                        bslma::Allocator                  *basicAllocator = 0);
    ConcurrentMultipool(int                          numPools,
                        bsls::BlockGrowth::Strategy  growthStrategy,
                        const int                   *maxBlocksPerChunkArray,
                        bslma::Allocator            *basicAllocator = 0);
    ConcurrentMultipool(
                     int                                numPools,
                     const bsls::BlockGrowth::Strategy *growthStrategyArray,
                     const int                         *maxBlocksPerChunkArray,
                     bslma::Allocator                  *basicAllocator = 0);
        // Create a multipool memory manager having the specified 'numPools',
        // indicating the number of internally created 'Pool' objects; the
        // block size of the first pool is 8 bytes, with the block size of each
        // additional pool successively doubling.  Optionally specify a
        // 'growthStrategy' indicating whether the number of blocks allocated
        // at once for every internally created 'Pool' should be either fixed
        // or grow geometrically, starting with 1.  If 'growthStrategy' is not
        // specified, optionally specify 'growthStrategyArray', indicating the
        // strategies for each individual 'Pool' created by this object.  If
        // neither 'growthStrategy' nor 'growthStrategyArray' are specified,
        // the allocation strategy for each internally created 'Pool' object
        // will grow geometrically, starting from 1.  Optionally specify a
        // 'maxBlocksPerChunk', indicating the maximum number of blocks to be
        // allocated at once when a pool must be replenished.  If
        // 'maxBlocksPerChunk' is not specified, optionally specify
        // 'maxBlocksPerChunkArray', indicating the maximum number of blocks to
        // allocate at once for each individually created 'Pool' object.  If
        // neither 'maxBlocksPerChunk' nor 'maxBlocksPerChunkArray' are
        // specified, an implementation-defined value is used.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  Memory allocation (and deallocation) requests will be
        // satisfied using the internally maintained pool managing memory
        // blocks of the smallest size not less than the requested size, or
        // directly from the underlying allocator (supplied at construction),
        // if no internally pool managing memory block of sufficient size
        // exists.  The behavior is undefined unless '1 <= numPools',
        // 'growthStrategyArray' has at least 'numPools' strategies,
        // '1 <= maxBlocksPerChunk' and 'maxBlocksPerChunkArray' have at least
        // 'numPools' positive values.  Note that, on platforms where
        // '8 < bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT', excess memory may be
        // allocated for pools managing smaller blocks.  Also note that the
        // maximum need not be an integral power of 2; if geometric growth
        // would exceed a maximum value, the chunk size is capped at that
        // value).

    ~ConcurrentMultipool();
        // Destroy this multipool.  All memory allocated from this memory pool
        // is released.

    // MANIPULATORS
    void *allocate(bsls::Types::size_type size);
        // Return the address of a contiguous block of maximally-aligned memory
        // of (at least) the specified 'size' (in bytes).  If
        // 'size > maxPooledBlockSize()', the memory allocation is managed
        // directly by the underlying allocator, and will not be pooled, but
        // will be deallocated when the 'release' method is called, or when
        // this object is destroyed.  If 'size' is 0, no memory is allocated
        // and 0 is returned.

    void deallocate(void *address);
        // Relinquish the memory block at the specified 'address' back to this
        // multipool object for reuse.  The behavior is undefined unless
        // 'address' is non-zero, was allocated by this multipool object, and
        // has not already been deallocated.

    template <class TYPE>
    void deleteObject(const TYPE *object);
        // Destroy the specified 'object' based on its dynamic type and then
        // use this multipool object to deallocate its memory footprint.  This
        // method has no effect if 'object' is 0.  The behavior is undefined
        // unless 'object', when cast appropriately to 'void *', was allocated
        // using this multipool object and has not already been deallocated.
        // Note that 'dynamic_cast<void *>(object)' is applied if 'TYPE' is
        // polymorphic, and 'static_cast<void *>(object)' is applied otherwise.

    template <class TYPE>
    void deleteObjectRaw(const TYPE *object);
        // Destroy the specified 'object' and then use this multipool to
        // deallocate its memory footprint.  This method has no effect if
        // 'object' is 0.  The behavior is undefined unless 'object' is !not! a
        // secondary base class pointer (i.e., the address is (numerically) the
        // same as when it was originally dispensed by this multipool), was
        // allocated using this multipool, and has not already been
        // deallocated.

    void release();
        // Relinquish all memory currently allocated via this multipool object.

    void reserveCapacity(bsls::Types::size_type size, int numBlocks);
        // Reserve memory from this multipool to satisfy memory requests for at
        // least the specified 'numBlocks' having the specified 'size' (in
        // bytes) before the pool replenishes.  If 'size' is 0, this method has
        // no effect.  The behavior is undefined unless
        // 'size <= maxPooledBlockSize()' and '0 <= numBlocks'.

    // ACCESSORS
    int numPools() const;
        // Return the number of pools managed by this multipool object.

    bsls::Types::size_type maxPooledBlockSize() const;
        // Return the maximum size of memory blocks that are pooled by this
        // multipool object.  Note that the maximum value is defined as:
        //..
        //  2 ^ (numPools + 2)
        //..
        // where 'numPools' is either specified at construction, or an
        // implementation-defined value.


                                  // Aspects

    bslma::Allocator *allocator() const;
        // Return the allocator used by this object to allocate memory.  Note
        // that this allocator can not be used to deallocate memory
        // allocated through this pool.
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                        // -------------------------
                        // class ConcurrentMultipool
                        // -------------------------

// MANIPULATORS
template <class TYPE>
inline
void ConcurrentMultipool::deleteObject(const TYPE *object)
{
    bslma::DeleterHelper::deleteObject(object, this);
}

template <class TYPE>
inline
void ConcurrentMultipool::deleteObjectRaw(const TYPE *object)
{
    bslma::DeleterHelper::deleteObjectRaw(object, this);
}

// ACCESSORS
inline
int ConcurrentMultipool::numPools() const
{
    return d_numPools;
}

inline
bsls::Types::size_type ConcurrentMultipool::maxPooledBlockSize() const
{
    return d_maxBlockSize;
}

// Aspects

inline
bslma::Allocator *ConcurrentMultipool::allocator() const
{
    return d_blockList.allocator();
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
