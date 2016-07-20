// bslma_autorawdeleter.h                                             -*-C++-*-
#ifndef INCLUDED_BSLMA_AUTORAWDELETER
#define INCLUDED_BSLMA_AUTORAWDELETER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a range proctor to manage a sequence objects.
//
//@CLASSES:
//  bslma::AutoRawDeleter: range proctor to manage a sequence of objects
//
//@SEE_ALSO: bslma_rawdeleterproctor, bslma_rawdeleterguard
//
//@DESCRIPTION: This component provides a range proctor class template,
// 'bslma::AutoRawDeleter', to manage a sequence of (otherwise-unmanaged)
// objects of parameterized 'TYPE' supplied at construction.  If not explicitly
// released, the sequence of managed objects are deleted automatically when the
// range proctor goes out of scope by iterating over each object, first calling
// the (managed) object's destructor, and then freeing its memory footprint by
// invoking the 'deallocate' method of an allocator (or pool) of parameterized
// 'ALLOCATOR' type also supplied at construction.  Note that after a range
// proctor releases its sequence of managed objects, the same proctor can be
// reused to conditionally manage another sequence of objects (allocated from
// the same allocator or pool that was supplied at construction) by invoking
// the 'reset' method.
//
///"Raw" Warning
///-------------
// Note that this component should be used only if we are sure that the
// supplied pointer is !not! of a type that is a secondary base class -- i.e.,
// the (managed) object's address is (numerically) the same as when it was
// originally dispensed by 'ALLOCATOR'.
//
///Requirement
///-----------
// The parameterized 'ALLOCATOR' type of the 'bslma::AutoRawDeleter' class
// template must provide a (possibly 'virtual') method:
//..
//  void deallocate(void *address);
//..
// to deallocate memory at the specified 'address' (originally supplied by the
// 'ALLOCATOR' object).
//
///Usage
///-----
// The 'bslma::AutoRawDeleter' proctor object can be used to preserve exception
// neutrality during manipulation of out-of-place arrays of user-defined-type
// objects.  The following illustrates the insertion operation for an
// "out-of-place" string array.  Assume that a string array initially contains
// the addresses of the following string objects as its elements:
//..
//     0     1     2     3     4
//   _____ _____ _____ _____ _____
//  |  o  |  o  |  o  |  o  |  o  |
//  `==|==^==|==^==|==^==|==^==|=='
//    _V_   _V_   _V_   _V_   _V_
//   |"A"| |"B"| |"C"| |"D"| |"E"|
//   `===' `===' `===' `===' `==='
//..
// To insert two string objects with values "F" and "G" at index position 2,
// the array is first reallocated if it is not big enough, and then the
// existing elements at index positions 2, 3, and 4 are shifted:
//..
//     0     1     2     3     4     5     6
//   _____ _____ _____ _____ _____ _____ _____
//  |  o  |  o  |xxxxx|xxxxx|  o  |  o  |  o  |
//  `==|==^==|==^=====^=====^==|==^==|==^==|=='
//    _V_   _V_               _V_   _V_   _V_
//   |"A"| |"B"|             |"C"| |"D"| |"E"|
//   `===' `==='             `===' `===' `==='
//
//  Note: "xxxxx" denotes undefined value
//..
// Next, two new string objects must be created and initialized with string
// values "F" and "G", respectively.  If, during creation, an allocation fails
// and an exception is thrown, the array will be left in an invalid state
// because the addresses contained at index positions 2 and 3 may be duplicates
// of those at index positions 4 and 5, or, if a resize occurred, invalid
// altogether.  We can restore exception neutrality by setting the array's
// length to 2 before attempting to create the string objects, but there is
// still a problem: the string objects "C", "D", and "E" (at index positions
// 3, 4, and 5) are "orphaned" and will never be deleted -- a memory leak.  To
// prevent this potential memory leak, we can additionally create a
// 'bslma::AutoRawDeleter' object to manage (temporarily) the elements at index
// positions 4, 5, and 6 prior to creating the new objects:
//..
//      0     1     2     3     4     5     6
//    _____ _____ _____ _____ _____ _____ _____
//   |  o  |  o  |xxxxx|xxxxx|  o  |  o  |  o  |
//   `==|==^==|==^=====^=====^==|==^==|==^==|=='
//     _V_   _V_               _V_   _V_   _V_
//    |"A"| |"B"|             |"C"| |"D"| |"E"|
//    `===' `==='             `===' `===' `==='
//    my_StrArray2           ^-----------------bslma::AutoRawDeleter
//    (length = 2)                             (length = 3)
//
//  Figure: Use of proctor for my_StrArray2::insert
//..
// If an exception occurs, the array (now of length 2) is in a perfectly valid
// state, while the second proctor is responsible for deleting the orphaned
// elements at index positions 4, 5, and 6.  If no exception is thrown, the
// elements at index positions 2 and 3 are set to new strings "F" and "G", the
// length of the first proctor is set to 7 and the second proctor's 'release'
// method is called, releasing its control over the temporarily managed
// elements.
//
// The following example illustrates the use of 'bslma::AutoRawDeleter' in
// conjunction with 'bslma::DeallocatorProctor' to manage temporarily a
// templatized, "out-of-place" array of parameterized 'TYPE' objects during the
// array's insertion operation.
//
// First we define a 'myArray' class that stores an array of parameterized
// 'TYPE' objects:
//..
//  template <class TYPE>
//  class myArray {
//      // This class is a container that stores an array of objects of
//      // parameterized 'TYPE'.
//
//      // DATA
//      TYPE              **d_array_p;       // dynamically allocated array of
//                                           // character sequence
//
//      int                 d_length;        // logical length of this array
//
//      int                 d_size;          // physical capacity of this array
//
//      bslma::Allocator   *d_allocator_p;   // allocator (held, not owned)
//
//    public:
//      // CREATORS
//      myArray(bslma::Allocator *basicAllocator = 0);
//          // Create a 'myArray' object.  Optionally specify a
//          // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
//          // 0, the currently installed default allocator is used.
//
//      ~myArray();
//          // Destroy this 'myArray' object and all elements currently stored.
//
//      // MANIPULATORS
//      void insert(int dstIndex, const myArray& srcArray);
//          // Insert into this array at the specified 'dstIndex', the
//          // character sequences in the specified 'srcArray'.  All values
//          // with initial indices at or above 'dstIndex' are shifted up by
//          // 'srcArray.length()' index positions.  The behavior is undefined
//          // unless '0 <= dstIndex' and 'dstIndex <= length()'.
//
//      // ...
//
//      // ACCESSORS
//      int length() const;
//          // Return the logical length of this array.
//
//      // ...
//  };
//..
// Note that a 'bslma::DeallocatorProctor' is used to manage a block of memory
// allocated before invoking the constructor of 'TYPE'.  If the constructor of
// 'TYPE' throws, the (managed) memory is automatically deallocated by
// 'bslma::DeallocatorProctor's destructor:
//..
//  template <class TYPE>
//  void myArray<TYPE>::insert(int dstIndex, const myArray<TYPE>& srcArray)
//  {
//      int srcLength  = srcArray.d_length;
//      int newLength  = d_length + srcLength;
//      int numShifted = d_length - dstIndex;
//
//      if (newLength > d_size) {  // need to resize
//          // ...
//      }
//
//      // First shift the elements to the back of the array.
//      memmove(d_array_p + dstIndex + srcLength,
//              d_array_p + dstIndex,
//              numShifted * sizeof *d_array_p);
//
//      // Shorten 'd_length' and use 'bslma::AutoDeleter' to proctor tail
//      // elements.
//      d_length = dstIndex;
//
//      //*************************************************************
//      // Note the use of auto raw deleter on tail elements (below). *
//      //*************************************************************
//
//      bslma::AutoRawDeleter<TYPE, bslma::Allocator>
//                                tailDeleter(d_array_p + dstIndex + srcLength,
//                                            d_allocator_p,
//                                            numShifted);
//..
// Now, if any allocation, either allocating memory for new elements or the
// constructor of the new element throws, the elements that had been moved to
// the end of the array will be deleted automatically by the
// 'bslma::AutoRawDeleter'.
//..
//      // Used to temporarily proctor each new element's memory.
//      bslma::DeallocatorProctor<bslma::Allocator>
//                                        elementDeallocator(0, d_allocator_p);
//
//      if (this != &srcArray) {  // no self-alias
//
//          // Copy the objects one by one.
//          for (int i = 0; i < srcLength; ++i, ++d_length) {
//              d_array_p[dstIndex + i] =
//                        (TYPE *) d_allocator_p->allocate(sizeof **d_array_p);
//
//              elementDeallocator.reset(d_array_p[dstIndex + i]);
//              new(d_array_p[dstIndex + i]) TYPE(*srcArray.d_array_p[i],
//                                                d_allocator_p);
//              elementDeallocator.release();
//          }
//      }
//      else {  // self-alias
//          // ...
//      }
//
//      //*********************************************
//      // Note that the proctor is released (below). *
//      //*********************************************
//
//      tailDeleter.release();
//      d_length = newLength;
//  }
//..
// Note that portions of the implementation are elided as it adds unnecessary
// complications to the usage example.  The shown portion is sufficient to
// illustrate the use of 'bslma_autorawdeleter'.
//
// The above method copies the source elements (visually) from left to right.
// Another (functionally equivalent) implementation copies the source elements
// from right to left, and makes use of the 'operator--()' of the
// 'bslma::AutoRawDeleter' interface:
//..
//  template <class TYPE>
//  void myArray<TYPE>::insert(int dstIndex, const myStrArray<TYPE>& srcArray)
//  {
//      int srcLength  = srcArray.d_length;
//      int newLength  = d_length + srcLength;
//      int numShifted = d_length - dstIndex;
//
//      if (newLength > d_size) {  // need to resize
//          // ...
//      }
//
//      // First shift the elements to the back of the array.
//      memmove(d_array_p + dstIndex + srcLength,
//              d_array_p + dstIndex,
//              numShifted * sizeof *d_array_p);
//
//      // Shorten 'd_length' and use 'bslma::AutoDeallocator' to proctor the
//      // memory shifted.
//      d_length = dst_Index;
//
//      //********************************************
//      //* Note the use of auto raw deleter on tail *
//      //* memory with negative length (below).     *
//      //********************************************
//
//      bslma::AutoRawDeleter<TYPE, bslma::Allocator> tailDeleter(newLength,
//                                                              d_allocator_p,
//                                                              -numShifted);
//..
// Since we have decided to copy the source elements from right to left, we
// set the origin of the 'bslma::AutoRawDeleter' to the end of the array, and
// decrement the (signed) length on each copy to extend the proctor range by
// 1.
//..
//      // Used to temporarily proctor each new element's memory.
//      bslma::DeallocatorProctor<bslma::Allocator>
//                                        elementDeallocator(0, d_allocator_p);
//
//      if (this != &srcArray) {  // no self-alias
//
//          // Copy the character sequences from the 'srcArray'.  Note that the
//          // 'tailDeleter' has to be decremented to cover the newly
//          // created object.
//
//          for (int i = srcLength - 1; i >= 0; --i, --tailDeleter) {
//              d_array_p[dstIndex + i] =
//                        (TYPE *) d_allocator_p->allocate(sizeof **d_array_p);
//              elementDeallocator.reset(d_array_p[dstIndex + i]);
//              new(d_array_p[dstIndex + i]) TYPE(*srcArray.d_array_p[i],
//                                                d_allocator_p);
//              elementDeallocator.release();
//          }
//      }
//      else {  // self-alias
//          // ...
//      }
//
//      //*********************************************
//      // Note that the proctor is released (below). *
//      //*********************************************
//
//      tailDeleter.release();
//      d_length = newLength;
//  }
//..
// Note that though the two implementations are functionally equivalent, they
// are logically different.  First of all, the second implementation will be
// slightly slower because it is accessing memory backwards when compared to
// the normal forward sequential access.  Secondly, in case of an exception,
// the first implementation will retain all the elements copied prior to the
// exception, whereas the second implementation will remove them.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_DELETERHELPER
#include <bslma_deleterhelper.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PERFORMANCEHINT
#include <bsls_performancehint.h>
#endif

namespace BloombergLP {

namespace bslma {

                        // ====================
                        // class AutoRawDeleter
                        // ====================

template <class TYPE, class ALLOCATOR>
class AutoRawDeleter {
    // This class implements a range proctor that, unless its 'release' method
    // has previously been invoked, automatically deletes the contiguous
    // sequence of managed objects upon destruction by iterating on each
    // (managed) object, first invoking the object's destructor, and then free
    // memory by invoking the 'deallocate' method of an allocator (or pool) of
    // parameterized 'ALLOCATOR' type supplied to it at construction.  The
    // sequence of managed objects of parameterized 'TYPE' must have been
    // created using memory provided by this allocator (or pool), which must
    // remain valid throughout the lifetime of this range proctor.  Note that
    // when the length of this object is non-zero, it must refer to a non-null
    // array of objects.

    // DATA
    TYPE      **d_origin_p;     // reference location for the sequence of
                                // managed objects

    int         d_length;       // number of objects managed (sign encodes
                                // direction)

    ALLOCATOR  *d_allocator_p;  // allocator or pool (held, not owned)

    // NOT IMPLEMENTED
    AutoRawDeleter(const AutoRawDeleter&);
    AutoRawDeleter& operator=(const AutoRawDeleter&);

  private:
    // PRIVATE MANIPULATORS
    void rawDelete();
        // Delete the contiguous sequence of objects managed by this auto raw
        // deleter (if any) by iterating over each (managed) object, first
        // invoking the object's destructor, and then freeing memory by
        // invoking the 'deallocate' method of the allocator (or pool) that was
        // supplied with the sequence of (managed) objects at construction.
        // Note that the order in which the managed objects are deleted is
        // undefined.  Also note that this method factors out the destruction
        // logic, which allows the destructor to be declared 'inline' for the
        // common case (the range proctor released before being destroyed).

  public:
    // CREATORS
    AutoRawDeleter(TYPE      **origin,
                   ALLOCATOR  *allocator,
                   int         length = 0);
        // Create an auto raw deleter to manage an array of objects at the
        // specified 'origin', and that uses the specified 'allocator' to
        // delete the sequence of objects managed by this range proctor (if not
        // released -- see 'release') upon destruction.  Optionally specify
        // 'length' to define its range, which by default is empty (i.e.,
        // 'length = 0').  The sequence of objects may extend in either
        // direction from 'origin'.  A positive 'length' represents the
        // sequence of objects starting at 'origin' and extending "up" to
        // 'length' (*not* including the object at the index position
        // 'origin + length').  A negative 'length' represents the sequence of
        // objects starting at one position below 'origin' and extending "down"
        // to the absolute value of 'length' (including the object at index
        // position 'origin + length').  If 'length' is 0, then this range
        // proctor manages no objects.  If 'origin' is non-zero, all objects
        // within the proctored range (if any) must be constructed using memory
        // supplied by 'allocator'.  The behavior is undefined unless
        // 'allocator' is non-zero, and, if 'origin' is 0, 'length is also 0.
        // Note that when 'length' is non-positive, the object at the origin is
        // *not* managed by this range proctor.  For example, if 'origin' is at
        // the index position 2, a 'length' of 2 signifies that the objects at
        // positions 2 and 3 are managed, whereas a 'length' of -2 signifies
        // that the objects at positions 0 and 1 are managed:
        //..
        //     length = -2                            length = 2
        //     |<----->|                              |<----->|
        //      ___ ___ ___ ___ ___            ___ ___ ___ ___ ___
        //     | 0 | 1 | 2 | 3 | 4 |          | 0 | 1 | 2 | 3 | 4 |
        //     `===^===^===^===^==='          `===^===^===^===^==='
        //             ^------------ origin           ^------------ origin
        //..

    ~AutoRawDeleter();
        // Destroy this range proctor and delete the contiguous sequence of
        // objects it manages (if any) by iterating over each (managed) object,
        // first invoking the object's destructor, and then freeing memory by
        // invoking the 'deallocate' method of the allocator (or pool) that was
        // supplied with the sequence of (managed) objects at construction.
        // Note that the order in which the managed objects are deleted is
        // undefined.

    // MANIPULATORS
    void operator++();
        // Increase by one the (signed) length of the sequence of objects
        // managed by this range proctor.  The behavior is undefined unless the
        // origin of the sequence of objects managed by this proctor is
        // non-zero.  The behavior is undefined unless the origin or this range
        // proctor is non-zero.  Note that if the length of this proctor is
        // currently negative, the number of managed objects will decrease by
        // one, whereas if the length is non-negative, the number of managed
        // objects will increase by one.

    void operator--();
        // Decrease by one the (signed) length of the sequence of objects
        // managed by this range proctor.  The behavior is undefined unless the
        // origin of the sequence of objects managed by this proctor is
        // non-zero.  The behavior is undefined unless the origin or this range
        // proctor is non-zero.  Note that if the length of this proctor is
        // currently positive, the number of managed objects will decrease by
        // one, whereas if the length is non-positive, the number of managed
        // objects will increase by one.

    void release() ;
        // Release from management the sequence of objects currently managed by
        // this range proctor by setting the length of the managed sequence to
        // 0.  All objects currently under management will become unmanaged
        // (i.e., when the proctor goes out of scope and it was not assigned
        // another sequence of objects to manage by invoking 'reset', no
        // objects will be deleted).  If no objects are currently being
        // managed, this method has no effect.  Note that the origin is not
        // affected.

    void reset(TYPE **origin);
        // Set the specified 'origin' as the origin of the sequence of objects
        // to be managed by this range proctor.  The behavior is undefined
        // unless 'origin' is non-zero.  Note that the length of the sequence
        // of objects managed by this proctor is not affected, and 'setLength'
        // should be invoked if the managed range is different from the
        // previously managed sequence of objects.  Also note that this method
        // releases any previously-managed objects from management (without
        // deleting them), and so may be called with or without having called
        // 'release' when reusing this object.

    void setLength(int length);
        // Set the (signed) length of the sequence of objects managed by this
        // range proctor to the specified 'length'.  The behavior is undefined
        // unless the origin of this range proctor is non-zero.

    // ACCESSORS
    int length() const;
        // Return the (signed) length of the sequence of objects managed by
        // this range proctor.
};

// ============================================================================
//                          INLINE DEFINITIONS
// ============================================================================

                        // --------------------
                        // class AutoRawDeleter
                        // --------------------

// PRIVATE MANIPULATORS
template <class TYPE, class ALLOCATOR>
void AutoRawDeleter<TYPE, ALLOCATOR>::rawDelete()
{
    if (d_length > 0) {
        for (; d_length > 0; --d_length, ++d_origin_p) {
            DeleterHelper::deleteObjectRaw(*d_origin_p,
                                           d_allocator_p);
        }
    }
    else {
        --d_origin_p;
        for (; d_length < 0; ++d_length, --d_origin_p) {
            DeleterHelper::deleteObjectRaw(*d_origin_p,
                                           d_allocator_p);
        }
    }
}

// CREATORS
template <class TYPE, class ALLOCATOR>
inline
AutoRawDeleter<TYPE, ALLOCATOR>::
AutoRawDeleter(TYPE **origin, ALLOCATOR *allocator, int length)
: d_origin_p(origin)
, d_length(length)
, d_allocator_p(allocator)
{
    BSLS_ASSERT_SAFE(allocator);
    BSLS_ASSERT_SAFE(origin || !length);
}

template <class TYPE, class ALLOCATOR>
inline
AutoRawDeleter<TYPE, ALLOCATOR>::~AutoRawDeleter()
{
    BSLS_ASSERT_SAFE(d_origin_p || !d_length);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(d_length)) {
        rawDelete();
    }
}

// MANIPULATORS
template <class TYPE, class ALLOCATOR>
inline
void AutoRawDeleter<TYPE, ALLOCATOR>::operator++()
{
    BSLS_ASSERT_SAFE(d_origin_p);

    ++d_length;
}

template <class TYPE, class ALLOCATOR>
inline
void AutoRawDeleter<TYPE, ALLOCATOR>::operator--()
{
    BSLS_ASSERT_SAFE(d_origin_p);

    --d_length;
}

template <class TYPE, class ALLOCATOR>
inline
void AutoRawDeleter<TYPE, ALLOCATOR>::release()
{
    d_length = 0;
}

template <class TYPE, class ALLOCATOR>
inline
void AutoRawDeleter<TYPE, ALLOCATOR>::reset(TYPE **origin)
{
    BSLS_ASSERT_SAFE(origin);

    d_origin_p = origin;
}

template <class TYPE, class ALLOCATOR>
inline
void AutoRawDeleter<TYPE, ALLOCATOR>::setLength(int length)
{
    BSLS_ASSERT_SAFE(d_origin_p);

    d_length = length;
}

// ACCESSORS
template <class TYPE, class ALLOCATOR>
inline
int AutoRawDeleter<TYPE, ALLOCATOR>::length() const
{
    return d_length;
}

}  // close package namespace


}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
