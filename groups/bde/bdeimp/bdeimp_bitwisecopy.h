// bdeimp_bitwisecopy.h                                               -*-C++-*-
#ifndef INCLUDED_BDEIMP_BITWISECOPY
#define INCLUDED_BDEIMP_BITWISECOPY

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide high-performance bitwise copy of objects.
//
//@CLASSES:
//   bdeimp_BitwiseCopy: namespace for bitwise copy function
//
//@AUTHOR: Paul Staniforth (pstaniforth)
//
//@DESCRIPTION:
// This component provides templatized bitwise copy and swap functions for
// objects with a known size at compile time.  The copy function can perform
// better than 'memcpy' on almost every architecture and is very unlikely to
// perform worse.  The performance will be identical to that of the assignment
// operator on a 'POD' object with the same size and alignment as the type
// being copied.  The swap function is similarly better than multiple calls
// to 'memcpy' on almost every architecture.
//
// All bitwise copy operations require care with objects that allocate
// resources such as memory.  Memory leaks or memory freed twice could occur
// with improper use.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1
///- - - - -
// Consider the case where the pointer 'srcAddr' points to a struct of type
// 'MyObject'.  The pointer 'dstAddr' points to properly aligned, but not
// necessarily initialized memory.  This case is analogous to the 'memcpy'
// call:
//..
//  memcpy(destPointer, sourcePointer, sizeof(MyObject));
//..
// Declare the 'MyObject' 'struct' and initialize objects and pointers.
//..
//    struct MyObject {
//        char d_char;
//        int d_int;
//    };
//
//    MyObject srcObj = { 'a', 10 };          assert('a' == srcObj.d_char);
//                                            assert( 10 == srcObj.d_int);
//    MyObject dstObj = { 'm', -1 };          assert('m' == dstObj.d_char);
//                                            assert( -1 == dstObj.d_int);
//
//    const MyObject *srcAddr = &srcObj;
//    MyObject       *dstAddr = &dstObj;
//..
// 'srcObj' initially contains ('a', 10) and 'dstObj' contains ('m', -1).
// Make a copy of the struct at 'srcAddr' to the new location 'dstAddr'.
// The previous contents at 'dstAddr' are overwritten.  The contents
// at 'srcAddr' are unchanged.
//..
//    bdeimp_BitwiseCopy<MyObject>::copy(dstAddr, srcAddr);
//                                            assert('a' == srcObj.d_char);
//                                            assert( 10 == srcObj.d_int);
//                                            assert('a' == dstObj.d_char);
//                                            assert( 10 == dstObj.d_int);
//..
// Reset 'dstObj' to ('m', -1).  Swap 'srcObj' and 'dstObj' so that 'srcObj'
// contains ('m', -1) and 'dstObj' contains ('a', 10).  Then swap them back.
//..
//    dstObj.d_char = 'm';
//    dstObj.d_int  = -1;
//
//    bdeimp_BitwiseCopy<MyObject>::swap(&srcObj, &dstObj);
//                                            assert('m' == srcObj.d_char);
//                                            assert( -1 == srcObj.d_int);
//                                            assert('a' == dstObj.d_char);
//                                            assert( 10 == dstObj.d_int);
//    bdeimp_BitwiseCopy<MyObject>::swap(&srcObj, &dstObj);
//                                            assert('a' == srcObj.d_char);
//                                            assert( 10 == srcObj.d_int);
//                                            assert('m' == dstObj.d_char);
//                                            assert( -1 == dstObj.d_int);
//..
//
///Example 2
///- - - - -
// The following detailed example uses the bitwise-swap
// function.  We define a basic templatized 'Array' class as our example
// container.  This container class may be parameterized by any class that is
// bitwise moveable.  (See the component 'bslmf_IsBitwiseCopyable'.)  In
// addition to the constructor and destructor, the container class has an index
// operator and a swap function.  The swap function is implemented using the
// 'bdeimp_BitwiseCopy' component.
//..
//  // my_array.h
//  template <class TYPE>
//  class my_Array {
//     // A fixed size templatized array.
//
//     TYPE *d_array_p;      // dynamically allocated array (d_length elements)
//     int d_length;         // length of this array (in elements)
//
//   public:
//     // CREATORS
//     my_Array(int length);
//         // Create an array of the specified 'length' initialized with the
//         // default value of the parameterized 'TYPE' object.
//     ...
//     // MANIPULATORS
//     void swap(int index1, int index2);
//         // Swap the two elements at the specified 'index1' and 'index2'.
//
//     TYPE& operator[](int index);
//         // Return a reference to the element at the specified 'index'.
//     ...
//
//  };
//
//  // my_array.cpp
//  template <class TYPE>
//  inline
//  void my_Array<TYPE>::swap(int index1, int index2)
//  {
//     bdeimp_BitwiseCopy<TYPE>::swap(d_array_p + index1, d_array_p + index2);
//  }
//..
// Elsewhere, we define an 'Example' scalar object, which we will use as the
// element type for 'my_Array<TYPE>':
//..
//  struct Example {
//     // This is the example 'struct' used as the template parameter
//     // for the 'my_Array'.
//     int d_data;
//
//     // CREATORS
//     Example(int value = 0) : d_data(value) {};
//
//   private:
//     // Copy constructor and assignment operator are not implemented.
//     Example(const Example&);
//     Example& operator=(const Example&);
//  };
//..
// Create an array containing 10 default objects of type 'Example'.
//..
//          my_Array<Example> array(10);
//..
// Note that, for this example, the 'Example' assignment operator is
// intentionally not implemented.  As an expedient for this example only, we
// therefore modify our 'my_Array' by assigning directly to the public 'int'
// data member of 'Example', using 'my_Array's 'operator[]' to access the
// element.  In general, public data is a bad idea; we are illustrating here
// the 'swap' method.
//..
//          int i;
//          for (int i = 0; i < 10; ++i) array[i].d_data = 33 - 2 * i;
//
//          bsl::cout << "before swap: ";
//          for (i = 0; i < 10; ++i) bsl::cout << array[i].d_data << ' ';
//          bsl::cout << '\n';
//..
// Initial Values in the array:
//..
//          33 31 29 27 25 23 21 19 17 15
//..
// Perform a swap implemented using bitwise copy
//..
//          array.swap(0, 2);
//
//          bsl::cout << "after swap:  ";
//          for (i = 0; i < 10; ++i) bsl::cout << array[i].d_data << ' ';
//          bsl::cout << '\n';
//..
// The array contains the following after elements 0 and 1 are swapped.
//..
//          29 31 33 27 25 23 21 19 17 15
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_OBJECTBUFFER
#include <bsls_objectbuffer.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

namespace BloombergLP {

                         // =========================
                         // struct bdeimp_BitwiseCopy
                         // =========================

template <class TYPE>
struct bdeimp_BitwiseCopy {
    // This 'struct' provides a namespace for the function used to implement a
    // bitwise copy.

  public:
    // CLASS METHODS
    static
    void copy(TYPE *dstAddr, const TYPE *srcAddr);
        // Copy the bit pattern from the object of parameterized 'TYPE' at the
        // specified 'srcAddr' to the the specified 'dstAddr'.  The behavior is
        // undefined unless both 'srcAddr' and 'dstAddr' are aligned properly
        // for a 'TYPE' object and do not overlap.

    static
    void swap(TYPE *addr1, TYPE *addr2);
        // Swap the bit patterns of the objects of parameterized 'TYPE' at the
        // specified addresses.  The behavior is undefined unless both 'addr1'
        // and 'addr2' are aligned properly for their 'TYPE' and do not
        // overlap.
};

// ============================================================================
//                      TEMPLATE FUNCTION DEFINITIONS
// ============================================================================

                         // -------------------------
                         // struct bdeimp_BitwiseCopy
                         // -------------------------

// CLASS METHODS
template <class TYPE>
inline
void bdeimp_BitwiseCopy<TYPE>::copy(TYPE *dstAddr, const TYPE *srcAddr)
{
    BSLS_ASSERT_SAFE(dstAddr);  BSLS_ASSERT_SAFE(srcAddr);

    // This function performs a bitwise copy of the specified object taking
    // advantage of the compile time knowledge of the object's alignment and
    // size.  The object pointers are cast to pointers to objects having the
    // same size and compatible alignment, but lacking an assignment operator.
    // The compiler thus provides and invokes an assignment operator, producing
    // a compiler-optimized bitwise copy.

    typedef bsls_ObjectBuffer<TYPE> BitType;

    // The compiler will use the most efficient bit-wise copy instructions
    // available for assigning the memory at srcAddr to the memory at dstAddr.
    *reinterpret_cast<BitType *>(dstAddr) =
                                   *reinterpret_cast<const BitType *>(srcAddr);
}

template <class TYPE>
inline
void bdeimp_BitwiseCopy<TYPE>::swap(TYPE *addr1, TYPE *addr2)
{
    BSLS_ASSERT_SAFE(addr1);  BSLS_ASSERT_SAFE(addr2);

    // Object big enough to hold a 'TYPE', properly aligned.
    bsls_ObjectBuffer<TYPE> temp;

    bdeimp_BitwiseCopy<TYPE>::copy(&temp.object(), addr1);
    bdeimp_BitwiseCopy<TYPE>::copy(addr1, addr2);
    bdeimp_BitwiseCopy<TYPE>::copy(addr2, &temp.object());
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
