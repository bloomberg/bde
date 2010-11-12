// bdema_autodeleter.h      -*-C++-*-
#ifndef INCLUDED_BDEMA_AUTODELETER
#define INCLUDED_BDEMA_AUTODELETER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Delete unmanaged out-of-place sequence of objects at destruction.
//
//@DEPRECATED: Use 'bslma_autorawdeleter' instead.
//
//@CLASSES:
//   bdema_AutoDeleter: proctor to manage an out-of-place sequence of objects
//
//@AUTHOR: Shao-wei Hung (shung1)
//
//@SEE_ALSO: bslma_deallocatorproctor, bslma_autodestructor
//
//@DESCRIPTION: This component provides a proctor object to manage an
// *out-of-place* (see definition below) sequence of otherwise-unmanaged
// instances of a user-defined type.  If not explicitly released, all objects
// managed by the proctor object are automatically *deleted* at destruction.
// By *delete* we mean that each 'TYPE' object's destructor is called
// explicitly and then its *footprint* is deallocated to the held 'ALLOCATOR'
// object.  Note that it is explicitly permitted to have any or all elements
// (i.e., pointers) in the managed sequence be null, in which case the "delete"
// operation is not performed.  Note also that the order in which the managed
// objects are deleted is undefined.
//
///Requirement
///-----------
// The object of the templatized type 'ALLOCATOR' must provide a method
//..
//                      void deallocate(void *address);
//..
// to deallocate the memory at the specified 'address' (originally supplied by
// the 'ALLOCATOR' object).
//
///Out-Of-Place versus In-Place
///----------------------------
// An out-of-place array holds a contiguous sequence of addresses of its
// contained elements.  The following is an illustration of the memory
// organization of an out-of-place string array:
//..
//                          0     1     2     3     4
//                        _____ _____ _____ _____ _____
//                       |  o  |  o  |  o  |  o  |  o  |
//                       `==|==^==|==^==|==^==|==^==|=='
//                         _V_   _V_   _V_   _V_   _V_
//                        |"A"| |"B"| |"C"| |"D"| |"E"|
//                        `===' `===' `===' `===' `==='
//..
// In contrast, an in-place array *embeds* the instances of the elements it
// contains in the array itself:
//..
//                          0     1     2     3     4
//                        _____ _____ _____ _____ _____
//                       | "A" | "B" | "C" | "D" | "E" |
//                       `=====^=====^=====^=====^====='
//..
///Usage
///-----
// The 'bdema_AutoDeleter' proctor object can be used to preserve exception
// neutrality during manipulation of out-of-place arrays of user-defined-type
// objects.  The following illustrates the insertion operation for an
// out-of-place string array.  Assume that a string array initially contains
// the addresses of the following string objects as its elements:
//..
//                          0     1     2     3     4
//                        _____ _____ _____ _____ _____
//                       |  o  |  o  |  o  |  o  |  o  |
//                       `==|==^==|==^==|==^==|==^==|=='
//                         _V_   _V_   _V_   _V_   _V_
//                        |"A"| |"B"| |"C"| |"D"| |"E"|
//                        `===' `===' `===' `===' `==='
//..
// To insert two string objects with values "F" and "G" at index position 2,
// the array is first reallocated if it is not big enough, and then the
// existing elements at index positions 2 - 4 are shifted:
//..
//                  0     1     2     3     4     5     6
//                _____ _____ _____ _____ _____ _____ _____
//               |  o  |  o  |xxxxx|xxxxx|  o  |  o  |  o  |
//               `==|==^==|==^=====^=====^==|==^==|==^==|=='
//                 _V_   _V_               _V_   _V_   _V_
//                |"A"| |"B"|             |"C"| |"D"| |"E"|
//                `===' `==='             `===' `===' `==='
//
//               Note: "xxxxx" denotes undefined value
//..
// Next, two new string objects must be created and initialized with string
// values "F" and "G", respectively.  If, during creation, an allocation fails
// and an exception is thrown, the array will be left in an invalid state
// because the addresses contained at index positions 2 and 3 may be duplicates
// of those at index positions 4 and 5, or, if a resize occurred, invalid
// altogether.  We can restore exception neutrality by setting the array's
// length to 2 before attempting to create the string objects, but there is
// still a problem: the string objects "C", "D" and "E" (at index positions
// 3 - 5) are "orphaned" and will never be deleted -- a memory leak.  To
// prevent this potential memory leak, we can additionally create an instance
// of 'bdema_AutoDeleter' to manage (temporarily) the elements at index
// positions 3 - 5 prior to creating the new objects:
//..
//                  0     1     2     3     4     5     6
//                _____ _____ _____ _____ _____ _____ _____
//               |  o  |  o  |xxxxx|xxxxx|  o  |  o  |  o  |
//               `==|==^==|==^=====^=====^==|==^==|==^==|=='
//                 _V_   _V_               _V_   _V_   _V_
//                |"A"| |"B"|             |"C"| |"D"| |"E"|
//                `===' `==='             `===' `===' `==='
//                my_StrArray2           ^----------------- bdema_AutoDeleter
//               (length = 2)                               (length = 3)
//
//              Figure: Use of proctor for my_StrArray2::insert
//..
// If an exception occurs, the array (now of length 2) is in a perfectly valid
// state, while the proctor is responsible for deleting the orphaned elements
// at index positions 3 - 5.  If no exception is thrown, the length is set to 7
// and the proctor's 'release()' method is called, releasing its control over
// the temporarily managed contents.
//
// The following code illustrates the use of 'bdema_AutoDeleter' in conjunction
// with 'my_AutoDeallocator' to manage temporarily a templatized, out-of-place
// array of user-defined-type objects during the array's insertion operation.
// Note that a 'my_AutoDeallocator' proctor object is used to manage a block of
// memory.  Unless explicitly released, the memory is automatically deallocated
// by 'my_AutoDeallocator's destructor:
//..
//  template <class TYPE>
//  inline
//  void my_Array2<TYPE>::insert(int dstIndex, const my_Array2<TYPE>& srcArray)
//  {
//      int srcLength = srcArray.d_length;
//      int newLength = d_length + srcLength;
//      int numShifted = d_length - dstIndex;
//
//      if (newLength > d_size) {  // Need to resize.
//          int newSize = calculateSufficientSize(newLength, d_size);
//          TYPE **tmp = d_array_p;
//          d_array_p =
//              (TYPE **) d_allocator_p->allocate(newSize * sizeof *d_array_p);
//          d_size = newSize;
//
//          memcpy(d_array_p, tmp, dstIndex * sizeof *d_array_p);
//          memcpy(d_array_p + dstIndex + srcLength,
//                 tmp + dstIndex, numShifted * sizeof *d_array_p);
//          d_allocator_p->deallocate(tmp);
//      }
//      else {
//          memmove(d_array_p + dstIndex + srcLength,
//                  d_array_p + dstIndex, numShifted * sizeof *d_array_p);
//      }
//
//      // Shorten 'd_length' and use auto deleter to proctor tail elements.
//      d_length = dstIndex;
//      bdema_AutoDeleter<TYPE, bslma_Allocator>
//                       tailDeleter(d_array_p + dstIndex + srcLength,
//                                   numShifted, d_allocator_p);
//
//      // Used to temporarily proctor each new element's memory.
//      my_AutoDeallocator<bslma_Allocator>
//                       elementDeallocator(0, d_allocator_p);
//
//      if (this == &srcArray) { // self-alias
//          // Copy elements up to (but not including) insertion position
//          for (int i = 0; i < dstIndex; ++i, ++d_length) {
//              d_array_p[dstIndex + i] =
//                  (TYPE *) d_allocator_p->allocate(sizeof **d_array_p);
//
//              elementDeallocator.reset(d_array_p[dstIndex + i]);
//              new(d_array_p[dstIndex + i])
//                  TYPE(*srcArray.d_array_p[i], d_allocator_p);
//          }
//          elementDeallocator.release();
//          // Copy elements at and beyond insertion position
//          for (int j = dstIndex; j < srcLength; ++j, ++d_length) {
//              d_array_p[dstIndex + j] =
//                  (TYPE *) d_allocator_p->allocate(sizeof **d_array_p);
//
//              elementDeallocator.reset(d_array_p[dstIndex + j]);
//              new(d_array_p[dstIndex + j])
//                  TYPE(*srcArray.d_array_p[j + srcLength], d_allocator_p);
//          }
//          elementDeallocator.release();
//      } else {
//          for (int i = 0; i < srcLength; ++i, ++d_length) {
//              d_array_p[dstIndex + i] =
//                  (TYPE *) d_allocator_p->allocate(sizeof **d_array_p);
//
//              elementDeallocator.reset(d_array_p[dstIndex + i]);
//              new(d_array_p[dstIndex + i])
//                  TYPE(*srcArray.d_array_p[i], d_allocator_p);
//          }
//          elementDeallocator.release();
//      }
//
//      tailDeleter.release();
//      d_length = newLength;
//  }
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

namespace BloombergLP {

                        // =======================
                        // class bdema_AutoDeleter
                        // =======================

template <class TYPE, class ALLOCATOR>
class bdema_AutoDeleter {
    // This class implements a proctor that, unless its 'release' method is
    // invoked, automatically *deletes* its managed objects at destruction.
    // By *delete* we mean that each 'TYPE' object's destructor is called
    // explicitly and then its *footprint* is deallocated to the held
    // 'ALLOCATOR' object.  Note that it is explicitly permitted to have any
    // or all elements (i.e., pointers) in the managed sequence be null, in
    // which case the 'delete' operation is not performed.  Note also that the
    // order in which the managed objects are deleted is undefined.

    // DATA
    TYPE     **d_origin_p;// reference address for sequence of managed objects
    int        d_length;  // number of managed objects (sign encodes direction)
    ALLOCATOR *d_allocator_p; // holds (but doesn't own) the allocator or pool

    // NOT IMPLEMENTED
    bdema_AutoDeleter(const bdema_AutoDeleter<TYPE, ALLOCATOR>&);
    bdema_AutoDeleter<TYPE, ALLOCATOR>& operator=(
                      const bdema_AutoDeleter<TYPE, ALLOCATOR>&);

  public:
    // CREATORS
    bdema_AutoDeleter(TYPE **origin, int length, ALLOCATOR *originalAllocator);
        // Create a proctor object to manage an out-of-place sequence of
        // objects whose range is defined by the specified 'origin' and
        // 'length'.  Each pointer to 'TYPE' in the managed sequence must
        // either be 0 or else must reference memory allocated from the
        // specified 'originalAllocator'.  The sequence of objects may extend
        // in either direction from 'origin'.  A positive 'length' represents
        // the sequence of objects starting at 'origin' and extending "up" to
        // 'length' (*not* including the object at the index position
        // 'origin' + 'length').  A negative 'length' represents the sequence
        // of objects starting at one index position below 'origin' and
        // extending "down" to the absolute value of 'length' (including the
        // object at index position 'origin' + 'length').  Note that when
        // 'length' is non-positive, the object at 'origin' is *not* managed by
        // this proctor.  For example, if 'origin' is at the index position 2,
        // a  'length' of 2 signifies that the objects at index positions 2 and
        // 3 are managed, whereas a 'length' of -2 signifies that the objects
        // at index positions 0 and 1 are managed:
        //..
        //     length = -2                            length = 2
        //     |<----->|                              |<----->|
        //      ___ ___ ___ ___ ___            ___ ___ ___ ___ ___
        //     | 0 | 1 | 2 | 3 | 4 |          | 0 | 1 | 2 | 3 | 4 |
        //     `===^===^===^===^==='          `===^===^===^===^==='
        //             ^----------- origin            ^----------- origin
        //..
        // The behavior is undefined unless each non-null pointer managed by
        // this proctor addresses a valid object.

    ~bdema_AutoDeleter();
        // Delete all managed objects.  Note that the order in which the
        // managed objects are deleted is undefined.

    // MANIPULATORS
    void release();
        // Set this proctor's length field to 0, thereby releasing from
        // management all objects currently managed by this proctor.

    bdema_AutoDeleter<TYPE, ALLOCATOR>& operator++();
        // Increase by one the (signed) length of the sequence of objects
        // managed by this proctor.  Note that if the length of this proctor is
        // currently negative, the number of managed objects will decrease by
        // one, whereas if the length is non-negative, the number of managed
        // objects will increase by one.

    bdema_AutoDeleter<TYPE, ALLOCATOR>& operator--();
        // Decrease by one the (signed) length of the sequence of objects
        // managed by this proctor.  Note that if the length of this proctor is
        // currently positive, the number of managed objects will decrease by
        // one, whereas if the length is non-positive, the number of managed
        // objects will increase by one.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

// CREATORS
template <class TYPE, class ALLOCATOR>
inline
bdema_AutoDeleter<TYPE, ALLOCATOR>::bdema_AutoDeleter(
                                           TYPE     **origin,
                                           int        length,
                                           ALLOCATOR *originalAllocator)
: d_origin_p(origin)
, d_length(length)
, d_allocator_p(originalAllocator)
{
}

template <class TYPE, class ALLOCATOR>
bdema_AutoDeleter<TYPE, ALLOCATOR>::~bdema_AutoDeleter()
{
    if (d_length) {          // expected case is '0 == d_length'
        if (0 < d_length) {  // positive length
            for (int i = 0; i < d_length; ++i) {
                if (TYPE *object_p = d_origin_p[i]) {
                    object_p->~TYPE();
                    d_allocator_p->deallocate(object_p);
                }
            }
        }
        else {               // negative length
            for (int i = -1; i >= d_length; --i) {
                if (TYPE *object_p = d_origin_p[i]) {
                    object_p->~TYPE();
                    d_allocator_p->deallocate(object_p);
                }
            }
        }
    }
}

// MANIPULATORS
template <class TYPE, class ALLOCATOR>
inline
void bdema_AutoDeleter<TYPE, ALLOCATOR>::release()
{
    d_length = 0;
}

template <class TYPE, class ALLOCATOR>
inline
bdema_AutoDeleter<TYPE, ALLOCATOR>&
bdema_AutoDeleter<TYPE, ALLOCATOR>::operator++()
{
    ++d_length;
    return *this;
}

template <class TYPE, class ALLOCATOR>
inline
bdema_AutoDeleter<TYPE, ALLOCATOR>&
bdema_AutoDeleter<TYPE, ALLOCATOR>::operator--()
{
    --d_length;
    return *this;
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
