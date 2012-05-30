// bdema_strpool.h      -*-C++-*-
#ifndef INCLUDED_BDEMA_STRPOOL
#define INCLUDED_BDEMA_STRPOOL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide fast allocation and management for unaligned string memory.
//
//@CLASSES:
//   bdema_StrPool: fast memory manager for unaligned character string memory
//
//@AUTHOR: Shao-wei Hung (shung1)
//
//@DESCRIPTION: This component implements a fast memory pool that manages
// *unaligned* memory of varying sizes.  Memory allocated from an instance of
// 'bdema_StrPool' is intended to store objects of types (e.g., 'char') that do
// not require alignment.  The pool internally requests relatively large memory
// blocks, and distributes memory piecemeal from each memory block on demand.
// The 'release' method releases all memory currently allocated from a pool, as
// does the destructor.  Note, however, that no facility is provided for
// deallocating individually allocated blocks of memory.
//
///Usage
///-----
// 'bdema_StrPool' can be used to perform fast memory allocation and
// deallocation for an array of 'char's.  The following 'my_CstrArray' uses
// 'bdema_StrPool' to allocate memory for its member string elements:
//..
//  // my_cstrarray.h
//
//  class my_CstrArray {
//      char            **d_array_p;     // dynamically allocated array
//      int               d_size;        // physical capacity of this array
//      int               d_length;      // logical length of this array
//      bdema_StrPool     d_strPool;     // manages and supplies string memory
//      bslma_Allocator  *d_allocator_p; // holds (but doesn't own) allocator
//
//    private:  // not implemented.
//      my_CstrArray(const my_CstrArray& original);
//
//    private:
//      void increaseSize();
//
//    public:
//      my_CstrArray(bslma_Allocator *basicAllocator = 0);
//      ~my_CstrArray();
//
//      my_CstrArray& operator=(const my_CstrArray& rhs);
//      void append(const char *item);
//      const char *operator[](int ind) const { return d_array_p[ind];}
//      int length() const { return d_length; }
//  };
//
//  // ...
//
//  // my_cstrarray.cpp
//  #include <bdema_default.h>
//
//  enum {
//      MY_INITIAL_SIZE = 1, // initial physical capacity (number of elements)
//      MY_GROW_FACTOR = 2   // multiplicative factor by which to grow 'd_size'
//  };
//
//  static inline
//  int nextSize(int size)
//      // Return the specified 'size' multiplied by 'MY_GROW_FACTOR'.
//  {
//      return size * MY_GROW_FACTOR;
//  }
//
//  static inline
//  void reallocate(char            ***array,
//                  int               *size,
//                  int                newSize,
//                  int                length,
//                  bslma_Allocator   *basicAllocator)
//      // Reallocate memory in the specified 'array' using the specified
//      // 'basicAllocator' and update the specified size to the specified
//      // 'newSize'.  The specified 'length' number of leading elements are
//      // preserved.  If 'new' should throw an exception, this function has no
//      // effect.  The behavior is undefined unless 1 <= newSize, 0 <= length,
//      // and newSize <= length.
//  {
//      assert(array);
//      assert(*array);
//      assert(size);
//      assert(1 <= newSize);
//      assert(0 <= length);
//      assert(basicAllocator);
//      assert(length <= *size);    // sanity check
//      assert(length <= newSize);  // ensure class invariant
//
//      char **tmp = *array;
//
//      *array = (char **) basicAllocator->allocate(newSize * sizeof **array);
//      // COMMIT
//      memcpy(*array, tmp, length * sizeof **array);
//      *size = newSize;
//      basicAllocator->deallocate(tmp);
//  }
//
//  void my_CstrArray::increaseSize()
//  {
//      reallocate(&d_array_p, &d_size, nextSize(d_size), d_length,
//                 d_allocator_p);
//  }
//
//  my_CstrArray::my_CstrArray(bslma_Allocator *basicAllocator)
//  : d_size(MY_INITIAL_SIZE)
//  , d_length(0)
//  , d_allocator_p(bslma_Default::allocator(basicAllocator))
//  , d_strPool(basicAllocator)
//  {
//      assert(d_allocator_p)
//      d_array_p =
//          (char **) d_allocator_p->allocate(d_size * sizeof *d_array_p);
//  }
//
//  my_CstrArray::~my_CstrArray()
//  {
//      assert(1 <= d_size);
//      assert(0 <= d_length);
//      assert(d_allocator_p);
//      assert(d_length <= d_size);
//      d_allocator_p->deallocate(d_array_p);
//  }
//
//  my_CstrArray& my_CstrArray::operator=(const my_CstrArray& rhs)
//  {
//      if (&rhs != this) {
//          d_strPool.release();
//          d_length = 0;
//          if (rhs.d_size > d_size) {
//              reallocate(&d_array_p, &d_size, rhs.d_size, d_length,
//                         d_allocator_p);
//          }
//          for (d_length = 0; d_length < rhs.d_length; ++d_length) {
//              int size = strlen(rhs.d_array_p[d_length]) + 1;
//              char *elem = (char *) d_strPool.allocate(size);
//              memcpy(elem, rhs.d_array_p[d_length], size);
//              d_array_p[d_length] = elem;
//          }
//      }
//      return *this;
//  }
//
//  void my_CstrArray::append(const char *item)
//  {
//      if (d_length >= d_size) {
//          this->increaseSize();
//      }
//      int sSize = strlen(item) + 1;
//      char *elem = (char *) d_strPool.allocate(sSize);
//      memcpy(elem, item, sSize * sizeof *item);
//      d_array_p[d_length++] = elem;
//  }
//..
// Note that 'operator=' efficiently deallocates memory for any initial string
// elements by calling the pool's 'release' method.  Similarly, the destructor
// explicitly deallocates only memory for the array of pointers.  Memory for
// the string elements is automatically deallocated when the pool goes out of
// scope.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEMA_INFREQUENTDELETEBLOCKLIST
#include <bdema_infrequentdeleteblocklist.h>
#endif

#ifndef INCLUDED_BSLFWD_BSLMA_ALLOCATOR
#include <bslfwd_bslma_allocator.h>
#endif


namespace BloombergLP {

                        // ===================
                        // class bdema_StrPool
                        // ===================

class bdema_StrPool {
    // This class implements a fast memory pool that manages *unaligned* memory
    // of varying sizes.  This pool internally requests relatively large memory
    // blocks, and distributes memory piecemeal from each memory block on
    // demand.  The 'release' method releases all memory currently allocated
    // from this pool, as does the destructor.  Note, however, that no facility
    // is provided for deallocating individually allocated blocks of memory.

    // DATA
    int               d_blockSize;  // size of current memory block

    char             *d_block_p;    // address of current free memory block

    int               d_cursor;     // offset to address of next available byte

    bdema_InfrequentDeleteBlockList
                      d_blockList;  // supplies managed memory blocks

    // NOT IMPLEMENTED
    bdema_StrPool(const bdema_StrPool&);
    bdema_StrPool& operator=(const bdema_StrPool&);

  private:
    // PRIVATE MANIPULATORS
    void *allocateImp(int numBytes);
        // Request a new memory block of at least the specified 'numBytes'
        // size and allocate the initial 'numBytes' from this block.  Return
        // the address of the allocated memory.  Note that the internal cursor
        // is adjusted appropriately assuming that the cursor is 'numBytes'
        // more than it should be due to the optimistic commit in 'allocate'.

  public:
    // CREATORS
    bdema_StrPool(bslma_Allocator *basicAllocator = 0);
        // Create a string memory pool.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    ~bdema_StrPool();
        // Destroy this object and release all associated memory.

    // MANIPULATORS
    void *allocate(int numBytes);
        // Allocate the specified 'numBytes' of memory and return its address.

    template <class TYPE>
    void deleteObject(const TYPE *object);
        // Destroy the specified 'object' based on its dynamic type.

    template <class TYPE>
    void deleteObjectRaw(const TYPE *object);
        // Destroy the specified 'object'.

    void release();
        // Release all memory currently allocated through this object.

    void reserveCapacity(int numBytes);
        // Reserve sufficient memory from this pool to satisfy memory requests
        // for at least the specified 'numBytes' before the pool replenishes.
        // The behavior is undefined unless '0 <= numBytes'.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // -------------------
                        // class bdema_StrPool
                        // -------------------

// MANIPULATORS
inline
void *bdema_StrPool::allocate(int numBytes)
{
    if (0 == numBytes) {
        return 0;
    }

    // Cache current cursor.  We cannot compute 'p = d_block_p + d_cursor'
    // here, or it will cause a Purify "uninitialize memory read" error if
    // 'p' exceeds the end of the current available block.

    int cursor = d_cursor;
    d_cursor += numBytes;  // optimistic commit

    return d_cursor <= d_blockSize ? d_block_p + cursor
                                   : allocateImp(numBytes);
}

template <class TYPE>
inline
void bdema_StrPool::deleteObjectRaw(const TYPE *object)
{
    if (0 != object) {
#ifndef BSLS_PLATFORM__CMP_SUN
        object->~TYPE();
#else
        const_cast<TYPE *>(object)->~TYPE();
#endif
    }
}

template <class TYPE>
inline
void bdema_StrPool::deleteObject(const TYPE *object)
{
    deleteObjectRaw(object);
}

inline
void bdema_StrPool::release()
{
    d_blockList.release();
    d_cursor = d_blockSize;  // Force next request to require a new block.
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
