// bdema_strallocator.h      -*-C++-*-
#ifndef INCLUDED_BDEMA_STRALLOCATOR
#define INCLUDED_BDEMA_STRALLOCATOR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide fast allocation for character string memory.
//
//@CLASSES:
//   bdema_StrAllocator: fast memory allocator for character string memory
//
//@AUTHOR: Shao-wei Hung (shung1)
//
//@DESCRIPTION: This component provides an allocator, 'bdema_StrAllocator',
// that implements the 'bslma_ManagedAllocator' protocol and allocates
// *unaligned* memory of varying sizes.  Memory allocated from
// 'bdema_StrAllocator' is intended to store objects of types such as 'char'
// (and array-of-'char') that do not require alignment.  The 'release' method
// releases all memory currently allocated from a 'bdema_StrAllocator' object,
// as does the destructor.  Note, however, that the 'deallocate' method has no
// effect; individually allocated blocks of memory cannot individually be
// returned to the allocator.
//..
//     ,------------------.
//    ( bdema_StrAllocator )
//     `------------------'
//              |        ctor/dtor
//              |        reserveCapacity
//              V
//   ,----------------------.
//  ( bslma_ManagedAllocator )
//   `----------------------'
//              |         release
//              V
//     ,----------------.
//    (  bslma_Allocator )
//     `----------------'
//                      allocate
//                      deallocate
//..
//
///Usage
///-----
// 'bdema_StrAllocator' is commonly used to supply memory for character
// strings.  The following 'my_StrArray' object implements an array of
// 'my_String' objects.  'my_StrArray' conditionally creates an internal
// 'bdema_StrAllocator' based on the 'allocationHint' flag specified at
// construction, and then passes this string allocator to each 'my_String'
// object to supply memory for its internal character array:
//..
//  // my_string.h
//
//  class my_String {
//      // This class is a simple implementation of a string type.
//
//      // DATA
//      char            *d_string_p;
//      int              d_length;
//      int              d_size;
//      bslma_Allocator *d_allocator_p;
//
//      // NOT IMPLEMENTED
//      my_String(const my_String&);
//
//    public:
//      my_String(const char *string, bslma_Allocator *basicAllocator = 0);
//      my_String(const my_String&  original,
//                bslma_Allocator  *basicAllocator = 0);
//      ~my_String();
//
//      int length() const            { return d_length;   }
//      operator const char *() const { return d_string_p; }
//  };
//
//  // ...
//
//  // my_string.cpp
//  #include <bslma_default.h>
//
//  my_String::my_String(const char *string, bslma_Allocator *basicAllocator)
//  : d_length(strlen(string))
//  , d_allocator_p(basicAllocator)
//  {
//      assert(string);
//      assert(d_allocator_p);
//      d_size = d_length + 1;
//      d_string_p = (char *) d_allocator_p->allocate(d_size);
//      memcpy(d_string_p, string, d_size);
//  }
//
//  my_String::my_String(const my_String& original,
//                       bslma_Allocator *basicAllocator)
//  : d_length(original.d_length)
//  , d_size(original.d_length + 1)
//  , d_allocator_p(bslma_Default::allocator(basicAllocator))
//  {
//      assert(d_allocator_p);
//      d_string_p = (char *) d_allocator_p->allocate(d_size);
//      memcpy(d_string_p, original.d_string_p, d_size);
//  }
//
//  my_String::~my_String()
//  {
//      assert(d_string_p);
//      d_allocator_p->deallocate(d_string_p);
//  }
//
//  // ...
//
//  // my_strarray.h
//
//  class my_StrArray {
//      my_String        *d_array_p;        // dynamically allocated array
//      int               d_size;           // physical capacity of this array
//      int               d_length;         // logical length of this array
//      bslma_Allocator  *d_allocator_p;    // supply non-string memory
//      bslma_Allocator  *d_strAllocator_p; // supply memory for strings
//
//    private: // not implemented.
//      my_StrArray(const my_StrArray& original);
//
//    private:
//      void increaseSize();
//
//    public:
//      enum Hint { NO_HINT, INFREQUENT_DELETE_HINT };
//      my_StrArray(Hint             allocationHint = NO_HINT,
//                  bslma_Allocator *basicAllocator = 0);
//      ~my_StrArray();
//
//      my_StrArray& operator=(const my_StrArray& rhs);
//      void append(const char *item);
//      const my_String& operator[](int ind) const { return d_array_p[ind]; }
//      int length() const                         { return d_length;       }
//  };
//
//  // ...
//
//  // my_strarray.cpp
//  #include <bslma_default.h>
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
//  void reallocate(my_String       **array,
//                  int              *size,
//                  int               newSize,
//                  int               length,
//                  bslma_Allocator  *basicAllocator)
//      // Reallocate memory in the specified 'array' using the specified
//      // 'basicAllocator' and update the specified size to the specified
//      // 'newSize'.  The specified 'length' number of leading elements are
//      // preserved.  If 'new' should throw an exception, this function has no
//      // effect.  The behavior is undefined unless 1 <= newSize, 0 <= length,
//      // and newSize <= length.
//  {
//      assert(array);
//      assert(size);
//      assert(1 <= newSize);
//      assert(0 <= length);
//      assert(basicAllocator);
//      assert(length <= *size);    // sanity check
//      assert(length <= newSize);  // ensure class invariant
//
//      my_String *tmp = *array;
//      *array =
//          (my_String *) basicAllocator->allocate(newSize * sizeof **array);
//      // COMMIT
//      memcpy(*array, tmp, length * sizeof **array);
//      *size = newSize;
//
//      basicAllocator->deallocate(tmp);
//  }
//
//  void my_StrArray::increaseSize()
//  {
//      reallocate(&d_array_p, &d_size, nextSize(d_size),
//                 d_length, d_allocator_p);
//  }
//
//  my_StrArray::my_StrArray(my_StrArray::Hint  allocationHint,
//                           bslma_Allocator   *basicAllocator)
//  : d_size(MY_INITIAL_SIZE)
//  , d_length(0)
//  , d_allocator_p(bslma_Default::allocator(basicAllocator))
//  {
//      assert(d_allocator_p);
//
//      if (INFREQUENT_DELETE_HINT == allocationHint) {
//          d_strAllocator_p =
//              new(d_allocator_p->allocate(sizeof(bdema_StrAllocator)))
//                  bdema_StrAllocator(d_allocator_p);
//      }
//      else {
//          d_strAllocator_p = d_allocator_p;
//      }
//
//      d_array_p =
//          (my_String *) d_allocator_p->allocate(d_size * sizeof *d_array_p);
//  }
//
//  my_StrArray::~my_StrArray()
//  {
//      assert(1 <= d_size);
//      assert(0 <= d_length);
//      assert(d_length <= d_size);
//      assert(d_allocator_p);
//      assert(d_strAllocator_p);
//
//      // If the allocator for string elements is different from the basic
//      // allocator, it is a specialized string allocator.  Memory for all
//      // string elements will be deallocated automatically when the string
//      // allocator is destroyed.
//      if (d_strAllocator_p == d_allocator_p) {
//          for (int i = 0; i < d_length; ++i) {
//              d_array_p[i].~my_String();
//          }
//      }
//      else {
//          d_strAllocator_p->~bslma_Allocator();
//          d_allocator_p->deallocate(d_strAllocator_p);
//      }
//      d_allocator_p->deallocate(d_array_p);
//  }
//
//  my_StrArray& my_StrArray::operator=(const my_StrArray& rhs)
//  {
//      if (&rhs != this) {
//          if (d_strAllocator_p == d_allocator_p) {
//              // Strings using basic allocator.  Destroy each string.
//              for (int i = 0; i < d_length; ++i) {
//                  d_array_p[i].~my_String();
//              }
//          }
//          else {
//              // Strings using string allocator.  Release all string memory.
//              ((bslma_ManagedAllocator *) d_strAllocator_p)->release();
//          }
//
//          d_length = 0;
//
//          if (rhs.d_length > d_size) {
//              reallocate(&d_array_p, &d_size, rhs.d_length, d_length,
//                         d_allocator_p);
//          }
//
//          for (d_length = 0; d_length < rhs.d_length; ++d_length) {
//              new(d_array_p + d_length)
//                  my_String(rhs.d_array_p[d_length], d_strAllocator_p);
//          }
//      }
//      return *this;
//  }
//
//  void my_StrArray::append(const char *item)
//  {
//      if (d_length >= d_size) {
//          this->increaseSize();
//      }
//      new(d_array_p + d_length) my_String(item, d_strAllocator_p);
//      ++d_length;
//  }
//..
// Note that in 'operator=' for 'my_StrArray', the 'd_allocator_p' member is
// compared to the 'd_strAllocator_p' member to determine whether a string
// allocator was created at construction.  If a string allocator was created
// and used, its 'release' method is called to deallocate character string
// memory for all 'my_String' elements.  Otherwise, the destructor for each
// 'my_String' object called.  Similarly, the destructor of 'my_StrArray'
// determines whether a string allocator was created and used, and if so,
// deallocate the character string memory by invoking the destructor of the
// string allocator.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLMA_MANAGEDALLOCATOR
#include <bslma_managedallocator.h>
#endif

#ifndef INCLUDED_BDEMA_STRPOOL
#include <bdema_strpool.h>
#endif

namespace BloombergLP {

class bslma_Allocator;

                        // ========================
                        // class bdema_StrAllocator
                        // ========================

class bdema_StrAllocator : public bslma_ManagedAllocator {
    // This class implements a fast memory allocator that allocates *unaligned*
    // memory of varying sizes.  The allocator uses its 'bdema_StrPool' member
    // to fulfill memory requests.  The 'release' method releases all memory
    // managed by this allocator, as does the destructor.  Note, however, that
    // the 'deallocate' method has no effect.

    // DATA
    bdema_StrPool d_strPool;

    // NOT IMPLEMENTED
    bdema_StrAllocator(const bdema_StrAllocator&);
    bdema_StrAllocator& operator=(const bdema_StrAllocator&);

  public:
    bdema_StrAllocator(bslma_Allocator *basicAllocator = 0);
        // Create a string memory allocator using the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    ~bdema_StrAllocator();
        // Destroy this object and release all associated memory.

    void *allocate(size_type numBytes);
        // Allocate the specified 'numBytes' of memory and return its address.

    void deallocate(void *address);
        // This method has no effect.

    void release();
        // Release memory currently allocated through this allocator.

    void reserveCapacity(size_type numBytes);
        // Reserve memory to satisfy memory requests for at least the specified
        // 'numBytes' before an allocation.  The behavior is undefined unless
        // '0 <= numBytes'.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ------------------------
                        // class bdema_StrAllocator
                        // ------------------------

// CREATORS
inline
bdema_StrAllocator::bdema_StrAllocator(bslma_Allocator *basicAllocator)
: d_strPool(basicAllocator)
{
}

// MANIPULATORS
inline
void *bdema_StrAllocator::allocate(size_type numBytes)
{
    return d_strPool.allocate(numBytes);
}

inline
void bdema_StrAllocator::deallocate(void *)
{
}

inline
void bdema_StrAllocator::release()
{
    d_strPool.release();
}

inline
void bdema_StrAllocator::reserveCapacity(size_type numBytes)
{
    d_strPool.reserveCapacity(numBytes);
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
