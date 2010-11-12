// bdema_autodeallocator.h      -*-C++-*-
#ifndef INCLUDED_BDEMA_AUTODEALLOCATOR
#define INCLUDED_BDEMA_AUTODEALLOCATOR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Deallocate a block of otherwise-unmanaged memory at destruction.
//
//@DEPRECATED: Use 'bslma_deallocatorproctor' instead.
//
//@CLASSES:
//   bdema_AutoDeallocator: proctor to manage a block of memory.
//
//@AUTHOR: Shao-wei Hung (shung1)
//
//@SEE_ALSO: bslma_autodestructor, bslma_autorawdeleter
//
//@DESCRIPTION: This component provides a proctor object to manage a block of
// otherwise-unmanaged memory.  If not explicitly released, the managed memory
// is automatically deallocated by the proctor's destructor.  Note that after a
// proctor object releases its managed memory, it can be reused to proctor
// another block of memory (allocated from the same 'ALLOCATOR' object that was
// specified at construction) by invoking the 'reset' method.
//
///REQUIREMENT
///-----------
// The object of the templatized type 'ALLOCATOR' must provide a method
//..
//       void deallocate(void *address);
//..
// to deallocate the memory at the specified 'address' (originally supplied by
// the 'ALLOCATOR' object).
//
///USAGE
///-----
// The 'bdema_AutoDeallocator' proctor object is often used to preserve
// exception neutrality in an operation that requires multiple memory
// allocations.  Since each memory allocation may potentially throw an
// exception, an instance of this proctor can be used to (temporarily) manage
// newly allocated memory while attempting to allocate additional memory.  If
// an exception is thrown during a subsequent memory allocation, the proctor's
// destructor deallocates its managed memory thus preventing a memory leak.  An
// example of this scenario can be found in the 'reallocate' function for a
// simple heterogeneous list object, 'my_List'.  The 'my_List' object contains
// two arrays of equal length: a 'void *' array that contains the addresses of
// the list items, and a corresponding 'char' array that indicates each list
// item's type at the respective index position.  When the list is to be
// extended, the function 'reallocate' is called to grow both arrays:
//..
//  // my_List.h
//
//  class my_List {
//      // This class implements a simple list object that supports 3 types of
//      // elements: 1) 'char', 2) 'int' and 3) 'double'.
//
//      char            *d_typeArray_p;  // array of element types
//      void           **d_list_p;       // array of addresses of list elements
//      int              d_length;       // logical length of this list
//      int              d_size;         // physical size of this list
//      bdema_Allocator *d_allocator_p;  // holds (but doesn't own) allocator
//
//    private:
//      void increaseSize();
//
//    public :
//      enum Type { CHAR, INT, DOUBLE }; // type constants
//
//      my_List(bdema_Allocator *basicAllocator);
//      ~my_List();
//      void append(char value);
//      void append(int value);
//      void append(double value);
//      const char *theChar(int index) const { return (char *)d_list_p[index];}
//      const int *theInt(int index) const   { return (int *) d_list_p[index];}
//      const double *theDouble(int index) const
//                                       { return (double *) d_list_p[index]; }
//      const Type type(int index) const { return (Type) d_typeArray_p[index];}
//      int length() const               { return d_length;                   }
//      // ...
//  };
//
//  // ...
//
//  // my_List.cpp
//
//  // ...
//
//  static inline
//  void reallocate(void ***list, char **typeArray, int *size,
//                  int newSize, int length, bdema_Allocator *basicAllocator)
//      // Reallocate memory in the specified 'list' and 'typeArray' using the
//      // specified 'basicAllocator' and update the specified size to the
//      // specified 'newSize'.  The specified 'length' number of leading
//      // elements are preserved in 'list' and 'typeArray'.  If 'allocate'
//      // should throw an exception, this function has no effect.  The
//      // behavior is undefined unless 1 <= newSize, 0 <= length, and
//      // newSize <= length.
//  {
//      assert(list);
//      assert(*list);
//      assert(typeArray);
//      assert(*typeArray);
//      assert(size);
//      assert(1 <= newSize);
//      assert(0 <= length);
//      assert(length <= *size);    // sanity check
//      assert(length <= newSize);  // ensure class invariant
//      assert(basicAllocator);
//
//      void **newList =
//          (void **) basicAllocator->allocate(newSize * sizeof *newList);
//      bdema_AutoDeallocator<bdema_Allocator> autoDealloc(newList,
//                                                         basicAllocator);
//      char *newTypeArray =
//          (char *) basicAllocator->allocate(newSize * sizeof *newTypeArray);
//      autoDealloc.release();
//
//      memcpy(newList, *list, length * sizeof **list);
//      memcpy(newTypeArray, *typeArray, length * sizeof **typeArray);
//      basicAllocator->deallocate(*list);
//      basicAllocator->deallocate(*typeArray);
//      *list = newList;
//      *typeArray = newTypeArray;
//      *size = newSize;
//  }
//..
// In the 'reallocate' function above, after allocating memory for the new
// array of list items ('newList'), the 'bdema_AutoDeallocator' proctor object
// is used to manage this memory while memory is allocated for the new array of
// types ('newTypeArray').  If an exception is thrown during this second
// allocation, the proctor's destructor automatically deallocates 'newList',
// preserving exception neutrality and preventing a memory leak.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLMA_DEALLOCATORPROCTOR
#include <bslma_deallocatorproctor.h>
#endif

#ifndef bdema_AutoDeallocator
#define bdema_AutoDeallocator   bslma_DeallocatorProctor
    // This class implements a proctor that automatically deallocates its
    // managed block of memory at destruction unless its 'release' method is
    // invoked.
#endif

namespace BloombergLP {

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
