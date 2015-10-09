// bdlma_buffermanager.h                                              -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLMA_BUFFERMANAGER
#define INCLUDED_BDLMA_BUFFERMANAGER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a memory manager that manages an external buffer.
//
//@CLASSES:
//  bdlma::BufferManager: memory manager that manages an external buffer
//
//@SEE_ALSO: bdlma_bufferimputil, bdlma_bufferedsequentialallocator
//
//@DESCRIPTION: This component provides a memory manager ("buffer manager"),
// 'bdlma::BufferManager', that dispenses heterogeneous memory blocks (of
// varying, user-specified sizes) from an external buffer.  A 'BufferManager'
// has a similar interface to a sequential pool in that the two methods
// 'allocate' and 'release' are provided.
//
// In addition to the 'allocate' method, a less safe but faster variation,
// 'allocateRaw', is provided to support memory allocation: If there is
// insufficient memory remaining in the buffer to satisfy an allocation
// request, 'allocate' will return 0 while 'allocateRaw' will result in
// undefined behavior.
//
// The behavior of 'allocate' and 'allocateRaw' illustrates the main difference
// between this buffer manager and a sequential pool.  Once the external buffer
// runs out of memory, the buffer manager does not self-replenish, whereas a
// sequential pool will do so.
//
// The 'release' method resets the buffer manager such that the memory within
// the entire external buffer will be made available for subsequent
// allocations.  Note that individually allocated memory blocks cannot be
// separately deallocated.
//
// 'bdlma::BufferManager' is typically used for fast and efficient memory
// allocation, when the user knows in advance the maximum amount of memory
// needed.
//
///Usage
///-----
// Suppose that we need to detect whether there are at least 'n' duplicates
// within an array of integers.  Furthermore, suppose that speed is a concern
// and we need the fastest possible implementation.  A natural solution will be
// to use a hash table.  To further optimize for speed, we can use a custom
// memory manager, such as 'bdlma::BufferManager', to speed up memory
// allocations.
//
// First, let's define the structure of a node inside our custom hash table
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
//      my_Node(int value, my_Node *next);
//          // Create a node having the specified 'value' that refers to the
//          // specified 'next' node.
//  };
//
//  // CREATORS
//  my_Node::my_Node(int value, my_Node *next)
//  : d_value(value)
//  , d_count(1)
//  , d_next_p(next)
//  {
//  }
//..
// Note that 'sizeof(my_Node) == 12' when compiled in 32-bit mode, and
// 'sizeof(my_Node) == 16' when compiled in 64-bit mode.  This difference
// affects the amount of memory used under different alignment strategies (see
// 'bsls_alignment' for more details on alignment strategies).
//
// We can then define the structure of our specialized hash table used for
// integer counting:
//..
//  class my_IntegerCountingHashTable {
//      // This class represents a hash table that is used to keep track of the
//      // number of occurrences of various integers.  Note that this is a
//      // highly specialized class that uses a 'bdlma::BufferManager' with
//      // sufficient memory for memory allocations.
//
//      // DATA
//      my_Node              **d_nodeArray;  // array of 'my_Node' pointers
//
//      int                    d_size;       // size of the node array
//
//      bdlma::BufferManager  *d_buffer;     // buffer manager (held, not
//                                           // owned)
//
//    public:
//      // CLASS METHODS
//      static int calculateBufferSize(int tableLength, int numNodes);
//          // Return the memory required by a 'my_IntegerCountingHashTable'
//          // that has the specified 'tableLength' and 'numNodes'.
//
//      // CREATORS
//      my_IntegerCountingHashTable(int size, bdlma::BufferManager *buffer);
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
//          // table if 'value' does not currently exist in the hash table, and
//          // increment the count for 'value' otherwise.  Return the number of
//          // occurrences of 'value' in this hash table.
//
//      // ...
//  };
//..
// The implementation of the rest of 'my_IntegerCountingHashTable' is elided as
// the class method 'calculateBufferSize', constructor, and the 'insert' method
// alone are sufficient to illustrate the use of 'bdlma::BufferManager':
//..
//  // CLASS METHODS
//  int my_IntegerCountingHashTable::calculateBufferSize(int tableLength,
//                                                       int numNodes)
//  {
//      return static_cast<int>(tableLength * sizeof(my_Node *)
//                            + numNodes * sizeof(my_Node)
//                            + bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT);
//  }
//..
// Note that, in case the allocated buffer is not aligned, the size calculation
// includes a "fudge" factor equivalent to the maximum alignment requirement of
// the platform.
//..
//  // CREATORS
//  my_IntegerCountingHashTable::my_IntegerCountingHashTable(
//                                                int                   size,
//                                                bdlma::BufferManager *buffer)
//  : d_size(size)
//  , d_buffer(buffer)
//  {
//      // 'd_buffer' must have sufficient memory to satisfy the allocation
//      // request (as specified by the constructor's contract).
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
//              return ++((*tmp)->d_count);                           // RETURN
//          }
//      }
//
//      // 'allocate' does not trigger dynamic memory allocation.  Therefore,
//      // we don't have to worry about exceptions and can use placement 'new'
//      // directly with 'allocate'.  'd_buffer' must have sufficient memory to
//      // satisfy the allocation request (as specified by the constructor's
//      // contract).
//
//      *tmp = new(d_buffer->allocate(sizeof(my_Node))) my_Node(value, *tmp);
//
//      return 1;
//  }
//..
// Note that 'bdlma::BufferManager' is used to allocate memory blocks of
// heterogeneous sizes.  In the constructor, memory is allocated for the node
// array.  In 'insert', memory is allocated for the nodes.
//
// Finally, in the following 'detectNOccurrences' function, we can use the hash
// table class to detect whether any integer value occurs at least 'n' times
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
// We then allocate an external buffer to be used by 'bdlma::BufferManager'.
// Normally, this buffer will be created on the program stack if we know the
// length in advance (for example, if we specify in the contract of this
// function that we only handle arrays having a length of up to 10,000
// integers).  However, to make this function more general, we decide to
// allocate the memory dynamically.  This approach is still much more efficient
// than using the default allocator, say, to allocate memory for individual
// nodes within 'insert', since we need only a single dynamic allocation,
// versus separate dynamic allocations for every single node:
//..
//      bslma::Allocator *allocator = bslma::Default::defaultAllocator();
//      char *buffer = static_cast<char *>(allocator->allocate(MAX_SIZE));
//..
// We use a 'bslma::DeallocatorGuard' to automatically deallocate the buffer
// when the function ends:
//..
//      bslma::DeallocatorGuard<bslma::Allocator> guard(buffer, allocator);
//
//      bdlma::BufferManager bufferManager(buffer, MAX_SIZE);
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
// object will then be maximally aligned, which takes up 16 bytes each instead
// of 12 bytes on a 32-bit architecture.  On a 64-bit architecture, there will
// be no savings using natural alignment since the size of a node will be 16
// bytes regardless.

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
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

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

namespace BloombergLP {
namespace bdlma {

                           // ===================
                           // class BufferManager
                           // ===================

class BufferManager {
    // This class implements a buffer manager that dispenses heterogeneous
    // blocks of memory (of varying, user-specified sizes) from an external
    // buffer whose address and size are optionally supplied at construction.
    // If an allocation request exceeds the remaining free memory space in the
    // external buffer, the allocation request returns 0 if 'allocate' is used,
    // or results in undefined behavior if 'allocateRaw' is used.  Note that in
    // no event will the buffer manager attempt to deallocate the external
    // buffer.

    // DATA
    char   *d_buffer_p;    // external buffer (held, not owned)

    int     d_bufferSize;  // size (in bytes) of external buffer

    int     d_cursor;      // offset to next available byte in buffer

    void *(*d_allocate_p)(int *, char *, int, int);
                           // address of non-raw 'Alignment::Strategy'-specific
                           // method from 'bdlma::BufferImpUtil'

    void *(*d_allocateRaw_p)(int *, char *, int);
                           // address of raw 'Alignment::Strategy'-specific
                           // method from 'bdlma::BufferImpUtil'

  private:
    // NOT IMPLEMENTED
    BufferManager(const BufferManager&);
    BufferManager& operator=(const BufferManager&);

  private:
    // PRIVATE MANIPULATORS
    void init(bsls::Alignment::Strategy strategy);
        // Initialize this object to use the specified alignment 'strategy'.

  public:
    // CREATORS
    explicit
    BufferManager(
           bsls::Alignment::Strategy strategy = bsls::Alignment::BSLS_NATURAL);
        // Create a buffer manager for allocating memory blocks.  Optionally
        // specify an alignment 'strategy' used to align allocated memory
        // blocks.  If 'strategy' is not specified, natural alignment is used.
        // A default constructed buffer manager is unable to allocate any
        // memory until an external buffer is provided by calling the
        // 'replaceBuffer' method.

    BufferManager(
          char                      *buffer,
          int                        bufferSize,
          bsls::Alignment::Strategy  strategy = bsls::Alignment::BSLS_NATURAL);
        // Create a buffer manager for allocating memory blocks from the
        // specified external 'buffer' having the specified 'bufferSize' (in
        // bytes).  Optionally specify an alignment 'strategy' used to align
        // allocated memory blocks.  If 'strategy' is not specified, natural
        // alignment is used.  The behavior is undefined unless
        // '0 < bufferSize' and 'buffer' has at least 'bufferSize' bytes.

    ~BufferManager();
        // Destroy this buffer manager.

    // MANIPULATORS
    void *allocate(bsls::Types::size_type size);
        // Return the address of a contiguous block of memory of the specified
        // 'size' (in bytes) on success, according to the alignment strategy
        // specified at construction, and 0 if the allocation request exceeds
        // the remaining free memory space in the external buffer.  The
        // behavior is undefined unless '0 < size' and this object is currently
        // managing a buffer.

    void *allocateRaw(int size);
        // Return the address of a contiguous block of memory of the specified
        // 'size' (in bytes) according to the alignment strategy specified at
        // construction.  The behavior is undefined unless the allocation
        // request does not exceed the remaining free memory space in the
        // external buffer, '0 < size', and this object is currently managing a
        // buffer.

    template <class TYPE>
    void deleteObjectRaw(const TYPE *object);
        // Destroy the specified 'object'.  Note that memory associated with
        // 'object' is not deallocated because there is no 'deallocate' method
        // in 'BufferManager'.

    template <class TYPE>
    void deleteObject(const TYPE *object);
        // Destroy the specified 'object'.  Note that this method has the same
        // effect as the 'deleteObjectRaw' method (since no deallocation is
        // involved), and exists for consistency with a pool interface.

    int expand(void *address, int size);
        // Increase the amount of memory allocated at the specified 'address'
        // from the original 'size' (in bytes) to also include the maximum
        // amount remaining in the buffer.  Return the amount of memory
        // available at 'address' after expanding, or 'size' if the memory at
        // 'address' cannot be expanded.  This method can only 'expand' the
        // memory block returned by the most recent 'allocate' or 'allocateRaw'
        // request from this buffer manager, and otherwise has no effect.  The
        // behavior is undefined unless the memory at 'address' was originally
        // allocated by this buffer manager, the size of the memory at
        // 'address' is 'size', and 'release' was not called after allocating
        // the memory at 'address'.

    char *replaceBuffer(char *newBuffer, int newBufferSize);
        // Replace the buffer currently managed by this object with the
        // specified 'newBuffer' of the specified 'newBufferSize' (in bytes);
        // return the address of the previously held buffer, or 0 if this
        // object currently manages no buffer.  The replaced buffer (if any) is
        // removed from the management of this object with no effect on the
        // outstanding allocated memory blocks.  Subsequent allocations will
        // allocate memory from the beginning of the new external buffer.  The
        // behavior is undefined unless '0 < newBufferSize' and 'newBuffer' has
        // at least 'newBufferSize' bytes.

    void release();
        // Release all memory currently allocated through this buffer manager.
        // After this call, the external buffer managed by this object is
        // retained.  Subsequent allocations will allocate memory from the
        // beginning of the external buffer (if any).

    void reset();
        // Reset this buffer manager to its default constructed state, except
        // retain the alignment strategy in effect at the time of construction.
        // The currently managed buffer (if any) is removed from the management
        // of this object with no effect on the outstanding allocated memory
        // blocks.

    int truncate(void *address, int originalSize, int newSize);
        // Reduce the amount of memory allocated at the specified 'address' of
        // the specified 'originalSize' (in bytes) to the specified 'newSize'
        // (in bytes).  Return 'newSize' after truncating, or 'originalSize' if
        // the memory at 'address' cannot be truncated.  This method can only
        // 'truncate' the memory block returned by the most recent 'allocate'
        // or 'allocateRaw' request from this object, and otherwise has no
        // effect.  The behavior is undefined unless the memory at 'address'
        // was originally allocated by this buffer manager, the size of the
        // memory at 'address' is 'originalSize', 'newSize <= originalSize',
        // '0 <= newSize', and 'release' was not called after allocating the
        // memory at 'address'.

    // ACCESSORS
    char *buffer() const;
        // Return an address providing modifiable access to the buffer
        // currently managed by this object, or 0 if this object currently
        // manages no buffer.

    int bufferSize() const;
        // Return the size (in bytes) of the buffer currently managed by this
        // object, or 0 if this object currently manages no buffer.

    bool hasSufficientCapacity(int size) const;
        // Return 'true' if there is sufficient memory space in the buffer to
        // allocate a contiguous memory block of the specified 'size' (in
        // bytes) after taking the alignment strategy into consideration, and
        // 'false' otherwise.  The behavior is undefined unless '0 < size', and
        // this object is currently managing a buffer.
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                               // ------------
                               // class Buffer
                               // ------------

// CREATORS
inline
BufferManager::BufferManager(bsls::Alignment::Strategy strategy)
: d_buffer_p(0)
, d_bufferSize(0)
, d_cursor(0)
{
    init(strategy);
}

inline
BufferManager::BufferManager(char                      *buffer,
                             int                        bufferSize,
                             bsls::Alignment::Strategy  strategy)
: d_buffer_p(buffer)
, d_bufferSize(bufferSize)
, d_cursor(0)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(0 < bufferSize);

    init(strategy);
}

inline
BufferManager::~BufferManager()
{
    BSLS_ASSERT_SAFE(0 <= d_cursor);
    BSLS_ASSERT_SAFE(d_cursor <= d_bufferSize);
    BSLS_ASSERT_SAFE((0 != d_buffer_p && 0 <  d_bufferSize)
                  || (0 == d_buffer_p && 0 == d_bufferSize));
}

// MANIPULATORS
inline
void *BufferManager::allocate(bsls::Types::size_type size)
{
    BSLS_ASSERT_SAFE(0 < size);
    BSLS_ASSERT_SAFE(d_buffer_p);
    BSLS_ASSERT_SAFE(0 <= d_cursor);
    BSLS_ASSERT_SAFE(d_cursor <= d_bufferSize);

    return (*d_allocate_p)(&d_cursor,
                           d_buffer_p,
                           d_bufferSize,
                           static_cast<int>(size));
}

inline
void *BufferManager::allocateRaw(int size)
{
    BSLS_ASSERT_SAFE(0 < size);
    BSLS_ASSERT_SAFE(d_buffer_p);
    BSLS_ASSERT_SAFE(0 <= d_cursor);
    BSLS_ASSERT_SAFE(d_cursor <= d_bufferSize);

    return (*d_allocateRaw_p)(&d_cursor, d_buffer_p, size);
}

template <class TYPE>
inline
void BufferManager::deleteObjectRaw(const TYPE *object)
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
void BufferManager::deleteObject(const TYPE *object)
{
    deleteObjectRaw(object);
}

inline
char *BufferManager::replaceBuffer(char *newBuffer, int newBufferSize)
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
void BufferManager::release()
{
    d_cursor = 0;
}

inline
void BufferManager::reset()
{
    d_buffer_p   = 0;
    d_bufferSize = 0;
    d_cursor     = 0;
}

// ACCESSORS
inline
char *BufferManager::buffer() const
{
    return d_buffer_p;
}

inline
int BufferManager::bufferSize() const
{
    return d_bufferSize;
}

inline
bool BufferManager::hasSufficientCapacity(int size) const
{
    BSLS_ASSERT_SAFE(0 < size);
    BSLS_ASSERT_SAFE(d_buffer_p);
    BSLS_ASSERT_SAFE(0 <= d_cursor);
    BSLS_ASSERT_SAFE(d_cursor <= d_bufferSize);

    int cursorTmp = d_cursor;
    return 0 != (*d_allocate_p)(&cursorTmp, d_buffer_p, d_bufferSize, size);
}

}  // close package namespace
}  // close enterprise namespace

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
