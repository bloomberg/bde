// bdema_buffermanager.h                                              -*-C++-*-
#ifndef INCLUDED_BDEMA_BUFFERMANAGER
#define INCLUDED_BDEMA_BUFFERMANAGER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a memory manager that manages an external buffer.
//
//@CLASSES:
//  bdema_BufferManager: memory manager that manages an external buffer
//
//@SEE_ALSO: bdema_bufferimputil, bdema_bufferedsequentialallocator
//
//@AUTHOR: Arthur Chiu (achiu21)
//
//@DESCRIPTION: This component provides a memory manager,
// 'bdema_BufferManager', that dispenses heterogenous memory blocks (of
// varying, user-specified sizes) from an external buffer.  The memory manager
// has a similar interface to a sequential pool in that the two methods
// 'allocate' and 'release' are provided.
//
// In addition to the 'allocate' method, a less safe, but faster variation,
// 'allocateRaw', is provided to support memory allocation: If there is
// insufficient memory remaining in the buffer to satisfy an allocation
// request, 'allocate' will return 0 while 'allocateRaw' will result in
// undefined behavior.
//
// The behavior of 'allocate' and 'allocateRaw' illustrates the main difference
// between this memory manager and a sequential pool.  Once the external buffer
// runs out of memory, the buffer manager does not self-replenish, whereas a
// sequential pool will do so.
//
// The 'release' method resets the memory manager such that the memory within
// the entire external buffer will be made available for subsequent
// allocations.  Note that individually allocated memory blocks cannot be
// separately deallocated.
//
// 'bdema_BufferManager' is typically used for fast and efficient memory
// allocation, when the user knows in advance the maximum amount of memory
// needed.
//
///Usage
///-----
// Suppose we need to detect whether there are 'n' duplicates within an array
// of integers.  Furthermore, suppose speed is a concern and we need the
// fastest possible implementation.  A natural solution will be to use a hash
// table.  To further optimize the speed, we can use a custom memory manager,
// such as 'bdema_BufferManager', to speed up memory allocations.
//
// First, let's define the structure of a node inside the custom hash table
// structure:
//..
//  struct my_Node {
//      // This struct represents a node within a hash table.
//
//      // DATA
//      int      d_value;   // integer value this node holds
//      int      d_count;   // number of occurrences of this integer value
//      my_Node *d_next_p;  // pointer to the next node
//
//      // CREATORS
//      my_Node(int value);
//          // Create a node with the specified 'value'.
//  };
//
//  // CREATORS
//  my_Node::my_Node(int value)
//  : d_value(value)
//  , d_count(1)
//  , d_next_p(0)
//  {
//  }
//..
// Note that the above 'my_Node' structure is 12 bytes when compiled under
// 32-bit mode, and 16 bytes when compiled under 64-bit mode.  This difference
// affects the amount of memory saved under different alignment strategies (see
// 'bsls_alignment' for more details on alignment strategies).
//
// We can then define the structure of our specialized hash table used for
// integer counting:
//..
//  class my_IntegerCountingHashTable {
//      // This class represents a hash table that is used to keep track of the
//      // number of occurrences of various integers.  Note that this is a
//      // highly specialized class that uses a 'bdema_BufferManager' with
//      // sufficient memory for memory allocations.
//
//      // DATA
//      my_Node             **d_nodeArray;  // an array of 'my_Node' pointers
//      int                   d_size;       // size of the node array
//      bdema_BufferManager  *d_buffer;     // memory manager (held, not owned)
//
//    public:
//      // CLASS METHODS
//      static int calculateBufferSize(int tableLength, int numNodes);
//          // Return the memory required by a 'my_IntegerCountingHashTable'
//          // that has the specified 'tableLength' and 'numNodes'.
//
//      // CREATORS
//      my_IntegerCountingHashTable(int size, bdema_BufferManager *buffer);
//          // Create a hash table of the specified 'size', using the specified
//          // 'buffer' to supply memory.  The behavior is undefined unless
//          // '0 < size', 'buffer' is non-zero, and 'buffer' has sufficient
//          // memory to support all memory allocations required.
//
//      // ...
//
//      // MANIPULATORS
//      int insert(int value);
//          // Insert the specified 'value' with a count of 1 into this hash
//          // table if 'value' does not currently exist in the hash table, or
//          // increment the count for 'value' if it already exists.  Return
//          // the number of occurrences of 'value'.
//
//      // ...
//  };
//..
// The implementation of the rest of 'my_IntegerCountingHashTable' is elided as
// the class method 'calculateBufferSize', constructor and the 'insert' method
// alone are sufficient to illustrate the use of 'bdema_BufferManager':
//..
//  // CLASS METHODS
//  int my_IntegerCountingHashTable::calculateBufferSize(int tableSize,
//                                                       int numNodes)
//  {
//      return tableSize * sizeof(my_Node *) + numNodes * sizeof(my_Node)
//                                    + bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT;
//  }
//..
// Note that, in case the allocated buffer is not aligned, the size calculation
// includes a "fudge" factor equivalent to the maximum alignment requirement of
// the platform.
//..
//  // CREATORS
//  my_IntegerCountingHashTable::my_IntegerCountingHashTable(
//                                                 int                  size,
//                                                 bdema_BufferManager *buffer)
//  : d_size(size)
//  , d_buffer(buffer)
//  {
//      // 'd_buffer' must have sufficient memory to satisfy the allocation
//      // request (specified by the constructor's contract).
//
//      d_nodeArray = static_cast<my_Node **>(
//                             d_buffer->allocate(d_size * sizeof(my_Node *)));
//
//      bsl::memset(d_nodeArray, 0, d_size * sizeof(my_Node *));
//  }
//
//  // MANIPULATORS
//  int my_IntegerCountingHashTable::insert(int value)
//  {
//      // Naive hash function using only mod.
//
//      const int hashValue = value % d_size;
//      my_Node **tmp       = &d_nodeArray[hashValue];
//
//      while (*tmp) {
//          if ((*tmp)->d_value != value) {
//              tmp = &((*tmp)->d_next_p);
//          }
//          else {
//              return ++((*tmp)->d_count);
//          }
//      }
//
//      // 'allocate' does not trigger dynamic memory allocation, therefore
//      // we don't have to worry about exceptions and can use placement 'new'
//      // directly with 'allocate'.  'd_buffer' must have sufficient memory to
//      // satisfy the allocation request (specified by the constructor's
//      // contract).
//
//      *tmp = new(d_buffer->allocate(sizeof(my_Node))) my_Node(value);
//
//      return 1;
//  }
//..
// Note that 'bdema_BufferManager' is used to allocate memory blocks of
// heterogenous sizes.  In the constructor, memory is allocated for the node
// array.  In 'insert', memory is allocated for the nodes.
//
// Finally, in the following 'detectNOccurrences' function, we can use the hash
// table class to detect whether any integer values at least occurred 'n' times
// within a specified array:
//..
//  bool detectNOccurrences(int n, const int *array, int length)
//      // Return 'true' if any integer value in the specified 'array' having
//      // the specified 'length' appears at least the specified 'n' times, and
//      // 'false' otherwise.
//  {
//      const int MAX_SIZE = my_IntegerCountingHashTable::
//                                         calculateBufferSize(length, length);
//..
//
// We then allocate an external buffer to be used by 'bdema_BufferManager'.
// Normally, this buffer will be created on the program stack if we know the
// length in advance (for example, if we specify in the contract of this
// function we only handle arrays having a length of up to 10,000 integers).
// To make this function more general, we decide to allocate the memory
// dynamically.  This approach is still much more efficient than using the
// default allocator, as we need only a single dynamic allocation, versus
// dynamic allocations for every single node:
//..
//      bslma_Allocator *alloc = bslma_Default::defaultAllocator();
//      char *buffer = static_cast<char *>(alloc->allocate(MAX_SIZE));
//..
// We can use a 'bslma_DeallocatorGuard' to automatically deallocate the buffer
// when the function ends:
//..
//      bslma_DeallocatorGuard<bslma_Allocator> guard(buffer, alloc);
//
//      bdema_BufferManager bufferManager(buffer, MAX_SIZE);
//      my_IntegerCountingHashTable table(length, &bufferManager);
//
//      while (--length >= 0) {
//          if (n == table.insert(array[length])) {
//              return true;                                          // RETURN
//          }
//      }
//
//      return false;
//  }
//..
// Note that the calculation of 'MAX_SIZE' assumes natural alignment.  If
// maximum alignment is used instead, a larger buffer is needed since each node
// object will then be aligned maximally, which then takes up 16 bytes each
// instead of 12 bytes on a 32-bit architecture.  On a 64-bit architecture,
// there will be no savings using natural alignment since the size of a node
// will be 16 bytes regardless.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEMA_BUFFERIMPUTIL
#include <bdema_bufferimputil.h>
#endif

#ifndef INCLUDED_BSLS_ALIGNMENT
#include <bsls_alignment.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>
#endif

namespace BloombergLP {

                        // =========================
                        // class bdema_BufferManager
                        // =========================

class bdema_BufferManager {
    // This class implements a memory manager that dispenses heterogeneous
    // blocks of memory (of varying, user-specified sizes) from an external
    // buffer whose address and size are optionally supplied at construction.
    // If an allocation request exceeds the remaining free memory space in the
    // external buffer, the allocation request returns 0 if 'allocate' is used,
    // or results in undefined behavior if 'allocateRaw' is used.  Note that in
    // no event will the memory manager attempt to deallocate the external
    // buffer.

    // DATA
    char *d_buffer_p;           // external buffer (held, not owned)

    int   d_bufferSize;         // size of external buffer (in bytes)

    int   d_cursor;             // offset to next available byte in buffer

    bsls_Alignment::Strategy
          d_alignmentStrategy;  // memory block alignment strategy


    // NOT IMPLEMENTED
    bdema_BufferManager(const bdema_BufferManager&);
    bdema_BufferManager& operator=(const bdema_BufferManager&);

  public:
    // CREATORS
    explicit bdema_BufferManager(bsls_Alignment::Strategy strategy =
                                                 bsls_Alignment::BSLS_NATURAL);
        // Create a memory manager for allocating memory blocks.  Optionally
        // specify an alignment 'strategy' used to align allocated memory
        // blocks.  If 'strategy' is not specified, natural alignment is used.
        // A default constructed memory manager is unable to allocate any
        // memory until an external buffer is provided by calling the
        // 'replaceBuffer' method.

    bdema_BufferManager(char                     *buffer,
                        int                       bufferSize,
                        bsls_Alignment::Strategy  strategy
                                               = bsls_Alignment::BSLS_NATURAL);
        // Create a memory manager for allocating memory blocks from the
        // specified external 'buffer' having the specified 'bufferSize' (in
        // bytes).  Optionally specify an alignment 'strategy' used to align
        // allocated memory blocks.  If 'strategy' is not specified, natural
        // alignment is used.  The behavior is undefined unless
        // '0 < bufferSize' and 'buffer' has at least 'bufferSize' bytes.

#if defined(BDE_BUILD_TARGET_SAFE) || defined(BDE_BUILD_TARGET_SAFE_2)
     ~bdema_BufferManager();
        // Destroy this memory manager.  Note that this trivial destructor is,
        // in some build modes, generated by the compiler.
#endif

    // MANIPULATORS
    void *allocate(bsls_PlatformUtil::size_type size);
        // Return the address of a contiguous block of memory of the specified
        // 'size' (in bytes) on success, according to the alignment strategy
        // specified at construction, and 0 if the allocation request
        // exceeds the remaining free memory space in the external buffer.  The
        // behavior is undefined unless '0 < size' and this memory manager is
        // currently managing a buffer.

    void *allocateRaw(int size);
        // Return the address of a contiguous block of memory of the specified
        // 'size' (in bytes).  The behavior is undefined unless the allocation
        // request does not exceed the remaining free memory space in the
        // external buffer, '0 < size', and this memory manager is currently
        // managing a buffer.

    template <class TYPE>
    void deleteObjectRaw(const TYPE *object);
        // Destroy the specified 'object'.  Note that the memory is not
        // deallocated because there is no 'deallocate' method in
        // 'bdema_BufferManager'.

    template <class TYPE>
    void deleteObject(const TYPE *object);
        // Destroy the specified 'object'.  Note that this method has the same
        // effect as the 'deleteObjectRaw' method (since no deallocation is
        // involved), and exists for consistency with a pool interface.

    int expand(void *address, int size);
        // Increase the amount of memory allocated at the specified 'address'
        // from the original 'size' (in bytes) to also include the maximum
        // amount remaining in the buffer.  Return the amount of memory
        // available at 'address' after expanding, or 'size' if 'address'
        // cannot be expanded.  This method can only 'expand' the memory block
        // returned by the most recent 'allocate' or 'allocateRaw' request from
        // this buffer manager, and otherwise has no effect.  The behavior is
        // undefined unless memory at 'address' was originally allocated by
        // this buffer manager, size of memory at 'address' is 'size', and
        // 'release' is not called after allocating memory at 'address'.

    char *replaceBuffer(char *newBuffer, int newBufferSize);
        // Replace the buffer currently managed by this manager with the
        // specified 'newBuffer' of the specified 'newBufferSize' (in bytes),
        // and return the address having the previously held buffer, or 0 if
        // this manager currently manages no buffer.  The replaced buffer (if
        // any) is removed from the management of this memory manager with no
        // effect on the outstanding allocated memory blocks.  The behavior is
        // undefined unless '0 < newBufferSize' and 'newBuffer' has at least
        // 'newBufferSize' bytes.

    void release();
        // Release all memory currently allocated through this memory manager.
        // After this call, the external buffer managed by this manager is
        // retained.  Subsequent allocations will allocate memory from the
        // beginning of the external buffer.

    void reset();
        // Reset this buffer manager to its default constructed state.  The
        // currently managed buffer (if any) is removed from the management of
        // this memory manager with no effect on the outstanding allocated
        // memory blocks.

    int truncate(void *address, int originalSize, int newSize);
        // Reduce the amount of memory allocated at the specified 'address'
        // of the specified 'originalSize' to the specified 'newSize'.  Return
        // 'newSize' after truncating, or 'originalSize' if 'address' cannot
        // be truncated.  This method can only 'truncate' the memory block
        // returned by the most recent 'allocate' or 'allocateRaw' request from
        // this buffer manager, and otherwise has no effect.  The behavior is
        // undefined unless the memory at 'address' was originally allocated
        // by this buffer manager, the size of the memory block at 'address' is
        // 'originalSize', 'newSize <= originalSize', '0 <= newSize', and
        // 'release' was not called after allocating memory at 'address'.

    // ACCESSORS
    char *buffer() const;
        // Return the address of the currently managed modifiable buffer, or 0
        // if this manager currently manages no buffer.

    int bufferSize() const;
        // Return the size of the currently managed modified buffer, or 0 if
        // this manager currently manages no buffer.

    bool hasSufficientCapacity(int size) const;
        // Return 'true' if there is sufficient memory space in the buffer to
        // allocate a contiguous memory block of the specified 'size' after
        // taking the alignment strategy into consideration.  The behavior
        // is undefined unless '0 < size', and this memory manager is currently
        // managing a buffer.
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        // ------------------
                        // class bdema_Buffer
                        // ------------------

// CREATORS
inline
bdema_BufferManager::bdema_BufferManager(bsls_Alignment::Strategy strategy)
: d_buffer_p(0)
, d_bufferSize(0)
, d_cursor(0)
, d_alignmentStrategy(strategy)
{
}

inline
bdema_BufferManager::bdema_BufferManager(char                     *buffer,
                                         int                       bufferSize,
                                         bsls_Alignment::Strategy  strategy)
: d_buffer_p(buffer)
, d_bufferSize(bufferSize)
, d_cursor(0)
, d_alignmentStrategy(strategy)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(0 < bufferSize);
}

#if defined(BDE_BUILD_TARGET_SAFE) || defined(BDE_BUILD_TARGET_SAFE_2)

inline
bdema_BufferManager::~bdema_BufferManager()
{
    BSLS_ASSERT_SAFE(0 <= d_cursor);
    BSLS_ASSERT_SAFE(d_cursor <= d_bufferSize);
    BSLS_ASSERT_SAFE((0 != d_buffer_p && 0 <  d_bufferSize)
                  || (0 == d_buffer_p && 0 == d_bufferSize));
}

#endif

// MANIPULATORS
inline
void *bdema_BufferManager::allocate(bsls_PlatformUtil::size_type size)
{
    BSLS_ASSERT_SAFE(0 < size);
    BSLS_ASSERT_SAFE(d_buffer_p);
    BSLS_ASSERT_SAFE(0 <= d_cursor);
    BSLS_ASSERT_SAFE(d_cursor <= d_bufferSize);

    return bdema_BufferImpUtil::allocateFromBuffer(&d_cursor,
                                                   d_buffer_p,
                                                   d_bufferSize,
                                                   static_cast<int>(size),
                                                   d_alignmentStrategy);
}

inline
void *bdema_BufferManager::allocateRaw(int size)
{
    BSLS_ASSERT_SAFE(0 < size);
    BSLS_ASSERT_SAFE(d_buffer_p);
    BSLS_ASSERT_SAFE(0 <= d_cursor);
    BSLS_ASSERT_SAFE(d_cursor <= d_bufferSize);

    return bdema_BufferImpUtil::allocateFromBufferRaw(&d_cursor,
                                                      d_buffer_p,
                                                      size,
                                                      d_alignmentStrategy);
}

template <class TYPE>
inline
void bdema_BufferManager::deleteObjectRaw(const TYPE *object)
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
void bdema_BufferManager::deleteObject(const TYPE *object)
{
    deleteObjectRaw(object);
}

inline
char *bdema_BufferManager::replaceBuffer(char *newBuffer, int newBufferSize)
{
    BSLS_ASSERT_SAFE(newBuffer);
    BSLS_ASSERT_SAFE(0 < newBufferSize);

    char *oldBuffer = d_buffer_p;
    d_buffer_p      = newBuffer;
    d_bufferSize    = newBufferSize;
    d_cursor        = 0;

    return oldBuffer;
}

inline
void bdema_BufferManager::release()
{
    d_cursor = 0;
}

inline
void bdema_BufferManager::reset()
{
    d_buffer_p   = 0;
    d_bufferSize = 0;
    d_cursor     = 0;
}

// ACCESSORS
inline
char *bdema_BufferManager::buffer() const
{
    return d_buffer_p;
}

inline
int bdema_BufferManager::bufferSize() const
{
    return d_bufferSize;
}

inline
bool bdema_BufferManager::hasSufficientCapacity(int size) const
{
    BSLS_ASSERT_SAFE(0 < size);
    BSLS_ASSERT_SAFE(d_buffer_p);
    BSLS_ASSERT_SAFE(0 <= d_cursor);
    BSLS_ASSERT_SAFE(d_cursor <= d_bufferSize);

    int cursor = d_cursor;
    return 0 != bdema_BufferImpUtil::allocateFromBuffer(&cursor,
                                                        d_buffer_p,
                                                        d_bufferSize,
                                                        size,
                                                        d_alignmentStrategy);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
