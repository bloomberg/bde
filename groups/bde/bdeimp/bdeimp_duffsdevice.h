// bdeimp_duffsdevice.h             -*-C++-*-
#ifndef INCLUDED_BDEIMP_DUFFSDEVICE
#define INCLUDED_BDEIMP_DUFFSDEVICE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")


//@PURPOSE: Provide Duff's Device implementation of common array operations.
//
//@DEPRECATED: Use 'bslalg_arrayprimitives' instead.
//
//@CLASSES:
//   bdeimp_DuffsDevice: efficient implementation of common array operations
//
//@AUTHOR: Jeffrey Mendelsohn (jmendels)
//
//@DESCRIPTION:
// This component provides "Duff's Device"-based implementations of common
// array operations such as initialization, assignment, and equality
// comparison.  Clients of 'bdeimp_DuffsDevice' can apply these templatized
// methods as needed.
//
///SYNOPSIS
///--------
// This components methods are templatized over the type 'T':
//..
// static int  areEqual(const T *lhsArray, const T *rhsArray, int numElements);
// static void assign(T *dstArray, const T *srcArray, int numElements);
// static void initialize(T *array, T value, int numElements);
//..
// The 'areEqual' method compares two arrays, the 'assign' method assigns the
// values of the 'srcArray' to the 'dstArray', and the 'initialize' method sets
// the values of the 'array' to the specified 'value'.  The 'raw' version
// can be used when it is known that 'numElements' is never zero and may
// perform slightly better.
//
// Note that the following functions are deprecated.
//..
//     static bool areEqualRaw(const T *lhsArray, const T *rhsArray,int numEl);
//     static void assignRaw(T *dstArray, const T *srcArray, int numElements);
//     static void initializeRaw(T *array, T value, int numElements);
//..
///Usage
///-----
// The following snippets of code illustrate how to use this utility.  First
// create an array 'a1' and initialize its values:
//..
//      int a1[32];
//      bdeimp_DuffsDevice<int>::initialize(a1, 7, 32);
//..
// Next create a second array 'a2' and assign it the values from 'a1':
//..
//      int a2[32];
//      bdeimp_DuffsDevice<int>::assign(a2, a1, 32);
//..
// Finally, verify the two arrays have the same value:
//..
//      assert(bdeimp_DuffsDevice<int>::areEqual(a1, a2, 32));
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

namespace BloombergLP {

                     // =====================================
                     // struct bdeimp_DuffsDevice<VALUE_TYPE>
                     // =====================================

template <typename VALUE_TYPE>
struct bdeimp_DuffsDevice {
    // This 'struct' provides a namespace for Duff's Device implementation of
    // common array operations.  The templatized type 'VALUE_TYPE' must support
    // 'operator=' and 'operator=='.  The methods are alias safe and exception
    // neutral.

    // CLASS METHODS
    static bool areEqual(const VALUE_TYPE *lhsArray,
                        const VALUE_TYPE *rhsArray,
                        int               numElements);
        // Return 'true' if the specified 'numElements' values of 'lhsArray'
        // are equivalent to those of the specified 'rhsArray', and 'false'
        // otherwise.  The behavior is undefined unless 0 <= numElements,
        // 'dstArray' has 'numElements' values, and 'srcArray' has
        // 'numElements' values.

    static bool areEqualRaw(const VALUE_TYPE *lhsArray,
                           const VALUE_TYPE *rhsArray,
                           int               numElements);
        // Return 'true' if the specified 'numElements' values of 'lhsArray'
        // are equivalent to those of the specified 'rhsArray', and 'false'
        // otherwise.  The behavior is undefined unless 0 < numElements,
        // 'dstArray' has 'numElements' values, and 'srcArray' has
        // 'numElements' values.
        //
        // DEPRECATED: use 'areEqual' instead to get the same performance
        // benefit.

    static void assign(VALUE_TYPE       *dstArray,
                       const VALUE_TYPE *srcArray,
                       int               numElements);
        // Assign to the specified 'dstArray' the specified 'numElements'
        // values from the specified 'srcArray.  The behavior is undefined
        // unless 0 <= numElements, 'dstArray' has sufficient capacity to
        // store 'numElements' at least values, and 'srcArray' has at least
        // 'numElements' values.

    static void assignRaw(VALUE_TYPE       *dstArray,
                          const VALUE_TYPE *srcArray,
                          int               numElements);
        // Assign to the specified 'dstArray' the specified 'numElements'
        // values from the specified 'srcArray.  The behavior is undefined
        // unless 0 < numElements, 'dstArray' has sufficient capacity to
        // store 'numElements' at least values, and 'srcArray' has at least
        // 'numElements' values.
        //
        // DEPRECATED: use 'assign' instead to get the same performance
        // benefit.

    static void initialize(VALUE_TYPE *array,
                           VALUE_TYPE  value,
                           int         numElements);
        // Initialize the specified 'numElements' of the specified 'array' to
        // the specified 'value'.  The behavior is undefined unless
        // 0 <= numElements and 'array' has sufficient capacity to store at
        // least 'numElements' values.

    static void initializeRaw(VALUE_TYPE *array,
                              VALUE_TYPE  value,
                              int         numElements);
        // Initialize the specified 'numElements' of the specified 'array' to
        // the specified 'value'.  The behavior is undefined unless
        // 0 < numElements and 'array' has sufficient capacity to store at
        // least 'numElements' values.
        //
        // DEPRECATED: use 'initialize' instead to get the same performance
        // benefit.
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                     // -------------------------------------
                     // struct bdeimp_DuffsDevice<VALUE_TYPE>
                     // -------------------------------------

// CLASS METHODS
template <typename VALUE_TYPE>
bool bdeimp_DuffsDevice<VALUE_TYPE>::areEqual(const VALUE_TYPE *lhsArray,
                                              const VALUE_TYPE *rhsArray,
                                              int               numElements)
{
    int rv = 1;    // will be coerced to bool on return

    switch (numElements % 8) {
      case 7: rv &= (*lhsArray == *rhsArray); ++lhsArray; ++rhsArray;
      case 6: rv &= (*lhsArray == *rhsArray); ++lhsArray; ++rhsArray;
      case 5: rv &= (*lhsArray == *rhsArray); ++lhsArray; ++rhsArray;
      case 4: rv &= (*lhsArray == *rhsArray); ++lhsArray; ++rhsArray;
      case 3: rv &= (*lhsArray == *rhsArray); ++lhsArray; ++rhsArray;
      case 2: rv &= (*lhsArray == *rhsArray); ++lhsArray; ++rhsArray;
      case 1: rv &= (*lhsArray == *rhsArray); ++lhsArray; ++rhsArray;
      default: ;
    }

    if (!rv) {
        return rv;                                                    // RETURN
    }

    int n = numElements / 8;
    while (n && rv) {
        rv &= (*lhsArray == *rhsArray); ++lhsArray; ++rhsArray;
        rv &= (*lhsArray == *rhsArray); ++lhsArray; ++rhsArray;
        rv &= (*lhsArray == *rhsArray); ++lhsArray; ++rhsArray;
        rv &= (*lhsArray == *rhsArray); ++lhsArray; ++rhsArray;
        rv &= (*lhsArray == *rhsArray); ++lhsArray; ++rhsArray;
        rv &= (*lhsArray == *rhsArray); ++lhsArray; ++rhsArray;
        rv &= (*lhsArray == *rhsArray); ++lhsArray; ++rhsArray;
        rv &= (*lhsArray == *rhsArray); ++lhsArray; ++rhsArray;
        --n;
    }

    return rv;
}

template <typename VALUE_TYPE>
inline
bool bdeimp_DuffsDevice<VALUE_TYPE>::areEqualRaw(const VALUE_TYPE *lhsArray,
                                                 const VALUE_TYPE *rhsArray,
                                                 int               numElements)
{
    return areEqual(lhsArray, rhsArray, numElements);
}

template <typename VALUE_TYPE>
void bdeimp_DuffsDevice<VALUE_TYPE>::assign(VALUE_TYPE       *dstArray,
                                            const VALUE_TYPE *srcArray,
                                            int               numElements)
{
    switch (numElements % 8) {
      case 7: *dstArray = *srcArray; ++dstArray; ++srcArray;
      case 6: *dstArray = *srcArray; ++dstArray; ++srcArray;
      case 5: *dstArray = *srcArray; ++dstArray; ++srcArray;
      case 4: *dstArray = *srcArray; ++dstArray; ++srcArray;
      case 3: *dstArray = *srcArray; ++dstArray; ++srcArray;
      case 2: *dstArray = *srcArray; ++dstArray; ++srcArray;
      case 1: *dstArray = *srcArray; ++dstArray; ++srcArray;
      default: ;
    }

    int n = numElements / 8;
    while (n) {
        *dstArray = *srcArray; ++dstArray; ++srcArray;
        *dstArray = *srcArray; ++dstArray; ++srcArray;
        *dstArray = *srcArray; ++dstArray; ++srcArray;
        *dstArray = *srcArray; ++dstArray; ++srcArray;
        *dstArray = *srcArray; ++dstArray; ++srcArray;
        *dstArray = *srcArray; ++dstArray; ++srcArray;
        *dstArray = *srcArray; ++dstArray; ++srcArray;
        *dstArray = *srcArray; ++dstArray; ++srcArray;
        --n;
    }
}

template <typename VALUE_TYPE>
inline
void bdeimp_DuffsDevice<VALUE_TYPE>::assignRaw(VALUE_TYPE       *dstArray,
                                               const VALUE_TYPE *srcArray,
                                               int               numElements)
{
    return assign(dstArray, srcArray, numElements);
}

template <typename VALUE_TYPE>
void bdeimp_DuffsDevice<VALUE_TYPE>::initialize(VALUE_TYPE *array,
                                                VALUE_TYPE  value,
                                                int         numElements)
{
    switch (numElements % 8) {
      case 7: *array = value; ++array;
      case 6: *array = value; ++array;
      case 5: *array = value; ++array;
      case 4: *array = value; ++array;
      case 3: *array = value; ++array;
      case 2: *array = value; ++array;
      case 1: *array = value; ++array;
      default: ;
    }

    int n = numElements / 8;
    while (n) {
        *array = value; ++array;
        *array = value; ++array;
        *array = value; ++array;
        *array = value; ++array;
        *array = value; ++array;
        *array = value; ++array;
        *array = value; ++array;
        *array = value; ++array;
        --n;
    }
}

template <typename VALUE_TYPE>
inline
void bdeimp_DuffsDevice<VALUE_TYPE>::initializeRaw(VALUE_TYPE *array,
                                                   VALUE_TYPE  value,
                                                   int         numElements)
{
    return initialize(array, value, numElements);
}

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
