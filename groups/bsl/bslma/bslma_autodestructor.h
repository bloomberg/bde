// bslma_autodestructor.h                                             -*-C++-*-
#ifndef INCLUDED_BSLMA_AUTODESTRUCTOR
#define INCLUDED_BSLMA_AUTODESTRUCTOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a range proctor to manage an array of objects.
//
//@CLASSES:
//  bslma::AutoDestructor: range proctor to manage an array of objects
//
//@SEE_ALSO: bslma_destructorguard, bslma_destructorproctor
//
//@DESCRIPTION: This component provides a range proctor class template,
// 'bslma::AutoDestructor', to manage an array of (otherwise-unmanaged) objects
// of parameterized 'TYPE' supplied at construction.  Unless explicitly
// released, the contiguous managed objects are destroyed automatically when
// the range proctor goes out of scope by calling each (managed) object's
// destructor.  Note that after a proctor object releases its managed objects,
// the same proctor can be reused to conditionally manage another contiguous
// sequence of objects by invoking the 'reset' method.
//
///Usage
///-----
// 'bslma::AutoDestructor' is normally used to achieve *exception* *safety* in
// an *exception* *neutral* way by automatically destroying
// (otherwise-unmanaged) orphaned objects for an "in-place" array should an
// exception occur.  The following example illustrates the insertion operation
// for a generic array.  Assume that the array initially contains the following
// five elements:
//..
//     0     1     2     3     4
//   _____ _____ _____ _____ _____ __
//  | "A" | "B" | "C" | "D" | "E" |
//  `=====^=====^=====^=====^=====^==
//..
// To insert an element "F" at index position 2, the existing elements at index
// positions 2, 3, and 4 (i.e., "C", "D", and "E") are first shifted right to
// create an empty spot at the specified insert destination (we assume here and
// below that the array has sufficient capacity).  The elements have to be
// shifted one by one by invoking the copy constructor (immediately followed by
// destroying the original elements).  However, should any of the copy
// construction operations throw, all allocated resources from every previous
// copy construction would be leaked.  Using the 'bslma::AutoDestructor'
// prevents the leak by invoking the destructor of each of the previously
// copied elements should the proctor go out of scope before the 'release'
// method of the proctor is called (such as when the function exits prematurely
// due to an exception):
//..
//     0     1     2     3     4     5     6
//   _____ _____ _____ _____ _____ _____ _____
//  | "A" | "B" | "C" | "D" | "E" |xxxxx|xxxxx|
//  `=====^=====^=====^=====^=====^=====^====='
//  my_Array                            ^----- bslma::AutoDestructor
//  (length = 5)                           (origin = 6, length = 0)
//
//   Note: "xxxxx" denotes uninitialized memory.
//..
// As each of the elements at index positions beyond the insertion position is
// shifted up by one index position, the proctor (i.e., the proctor's length)
// is *decremented*, thereby *extending* by one the sequence of elements it
// manages *below* its origin (note that when the proctor's length is non-
// positive, the element at the origin is *not* managed).  At the same time,
// the array's length is *decremented* to ensure that each array element is
// always being managed (during an allocation attempt) either by the proctor or
// the array itself, but not both:
//..
//     0     1     2     3     4     5     6
//   _____ _____ _____ _____ _____ _____ _____
//  | "A" | "B" | "C" | "D" |xxxxx| "E" |xxxxx|
//  `=====^=====^=====^=====^=====^=====^====='
//  my_Array                      ^----------- bslma::AutoDestructor
//  (length = 4)                          (origin = 6, length = -1)
//
//   Note: Configuration after shifting up one element.
//..
// When all elements are shifted, the 'bslma::AutoDestructor' will protect the
// entire range of shifted objects:
//..
//     0     1     2     3     4     5     6
//   _____ _____ _____ _____ _____ _____ _____
//  | "A" | "B" |xxxxx| "C" | "D" | "E" |xxxxx|
//  `=====^=====^=====^=====^=====^=====^====='
//  my_Array          ^----------------------- bslma::AutoDestructor
//  (length = 2)                           (origin = 6, length = -3)
//
//    Note: Configuration after shifting up three elements.
//..
// Next, a new copy of element "F" must be created.  If, during creation, an
// allocation fails and an exception is thrown, the array (now of length 2) is
// in a valid state, while the proctor is responsible for destroying the
// orphaned elements at index positions 3, 4, and 5.  If no exception is
// thrown, the proctor's 'release' method is called, releasing its control over
// the temporarily-managed contents:
//..
//  // my_array.h
//  // ...
//
//  template <class TYPE>
//  class my_Array {
//      // This class implements an "in-place" array of objects of
//      // parameterized 'TYPE' stored contiguously in memory.
//
//      // DATA
//      TYPE             *d_array_p;      // dynamically allocated array
//      int               d_length;       // logical length of this array
//      int               d_size;         // physical capacity of this array
//      bslma::Allocator *d_allocator_p;  // allocator (held, not owned)
//
//    public:
//      // CREATORS
//      my_Array(bslma::Allocator *basicAllocator = 0);
//          // Create a 'my_Array' object having an initial length and capacity
//          // of 0.  Optionally specify a 'basicAllocator' used to supply
//          // memory.  If 'basicAllocator' is 0, the currently installed
//          // default allocator is used.
//
//      my_Array(int initialCapacity, bslma::Allocator *basicAllocator = 0);
//          // Create a 'my_Array' object having an initial length of 0 and
//          // the specified 'initialCapacity'.  Optionally specify a
//          // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
//          // 0, the currently installed default allocator is used.
//
//      // ...
//
//      ~my_Array();
//          // Destroy this 'my_Array' object and all elements currently
//          // stored.
//
//      // MANIPULATORS
//      // ...
//
//      void insert(int dstIndex, const TYPE& object);
//          // Insert (a copy of) the specified 'object' of parameterized
//          // 'TYPE' at the specified 'dstIndex' position of this array.  All
//          // values with initial indices at or above 'dstIndex' are shifted
//          // up by one index position.  The behavior is undefined unless
//          // '0 <= dstIndex' and 'dstIndex' is less than the number of items
//          // in this array.
//
//      // ...
//  };
//..
// Note that the rest of the 'my_Array' interface (above) and implementation
// (below) is omitted as the portion shown is sufficient to demonstrate the use
// of 'bslma::AutoDestructor'.
//..
//  // CREATORS
//  template <class TYPE>
//  inline
//  my_Array<TYPE>::my_Array(bslma::Allocator *basicAllocator)
//  : d_array_p(0)
//  , d_length(0)
//  , d_size(0)
//  , d_allocator_p(bslma::Default::allocator(basicAllocator))
//  {
//  }
//
//  template <class TYPE>
//  my_Array<TYPE>::~my_Array()
//  {
//      for (int i = 0; i < d_length; ++i) {
//          d_array_p[i].~TYPE();
//      }
//      d_allocator_p->deallocate(d_array_p);
//  }
//..
// The elided implementation of the following 'insert' function (which shows
// code for the case above, i.e., there is sufficient capacity) is sufficient
// to illustrate the use of 'bslma::AutoDestructor':
//..
//  // MANIPULATORS
//  template <class TYPE>
//  void my_Array<TYPE>::insert(int dstIndex, const TYPE& object)
//  {
//      BSLS_ASSERT(0 <= dstIndex);
//      BSLS_ASSERT(dstIndex <= d_length);
//
//      if (d_size == d_length) {  // resize needed
//          // ...
//      }
//
//      const TYPE *tmp = &object;
//
//      if ((d_array_p + dstIndex <= &object)
//       && (&object < d_array_p + d_length)) {  // self-aliasing
//          tmp = &object + 1;
//      }
//
//      //**************************************************************
//      // Note the use of the auto destructor on 'd_array_p' (below). *
//      //**************************************************************
//
//      bslma::AutoDestructor<TYPE> autoDtor(&d_array_p[d_length + 1], 0);
//      int origLen = d_length;
//      for (int i = d_length - 1; i >= dstIndex; --i, --autoDtor,
//                                                                --d_length) {
//          new(&d_array_p[i + 1]) TYPE(d_array_p[i], d_allocator_p);
//                                                         // copy to new index
//          d_array_p[i].~TYPE();                          // destroy original
//      }
//
//      new(&d_array_p[dstIndex]) TYPE(*tmp, d_allocator_p);
//
//      //*****************************************************
//      // Note that the auto destructor is released (below). *
//      //*****************************************************
//
//      autoDtor.release();
//
//      d_length = origLen + 1;
//  }
//..
// Note that the 'insert' method assumes the copy constructor of 'TYPE' takes
// an allocator as a second argument.  In production code, a constructor proxy
// that checks the traits of 'TYPE' (to determine whether 'TYPE' indeed uses
// 'bslma::Allocator') should be used (see 'bslalg_constructorproxy').

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
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
                        // class AutoDestructor
                        // ====================

template <class TYPE>
class AutoDestructor {
    // This class implements a range proctor that, unless its 'release' method
    // has previously been invoked, automatically destroys the contiguous
    // sequence of managed objects upon its own destruction by invoking each
    // object's destructor.  If 'release' is invoked (see 'release'), the
    // proctor will not destroy any objects upon its own destruction (unless
    // 'reset' is invoked to assign the proctor another contiguous sequence of
    // objects for management).  Note that when the length of this object is
    // non-zero, it must refer to a non-null array of objects.

    // DATA
    TYPE *d_origin_p;  // reference location for the array of managed objects
    int   d_length;    // number of objects to manage (sign encodes direction)

    // NOT IMPLEMENTED
    AutoDestructor(const AutoDestructor&);
    AutoDestructor& operator=(const AutoDestructor&);

  private:
    // PRIVATE MANIPULATORS
    void destroy();
        // Destroy the contiguous sequence of objects managed by this range
        // proctor (if any) by invoking the destructor of each (managed)
        // object.  Note that the order in which the managed objects are
        // destroyed is undefined.  Also note that this method factors out the
        // destruction logic, which allows the destructor to be declared
        // 'inline' for the common case (the range proctor is released before
        // being destroyed).

  public:
    // CREATORS
    explicit AutoDestructor(TYPE *origin, int length = 0);
        // Create a range proctor to manage an array of objects at the
        // specified 'origin'.  Optionally specify 'length' to define its
        // range, which by default is empty (i.e., 'length = 0').  The sequence
        // of objects may extend in either direction from 'origin'.  A positive
        // 'length' represents the sequence of objects starting at 'origin' and
        // extending "up" to 'length' (*not* including the object at the index
        // position 'origin + length').  A negative 'length' represents the
        // sequence of objects starting at one position below 'origin' and
        // extending "down" to the absolute value of 'length' (including the
        // object at index position 'origin + length').  If 'length' is 0, then
        // this range proctor manages no objects.  The behavior is undefined
        // unless both 'origin' and 'length' are zero, or 'origin' is non-zero
        // and all objects (if any) within the managed range (as defined by
        // 'length') are valid.  Note that when 'length' is non-positive, the
        // object at the origin is *not* managed by this range proctor.  For
        // example, if 'origin' is at the index position 2, a 'length' of 2
        // signifies that the objects at positions 2 and 3 are managed, whereas
        // a 'length' of -2 signifies that the objects at positions 0 and 1 are
        // managed:
        //..
        //     length = -2                            length = 2
        //     |<----->|                              |<----->|
        //      ___ ___ ___ ___ ___            ___ ___ ___ ___ ___
        //     | 0 | 1 | 2 | 3 | 4 |          | 0 | 1 | 2 | 3 | 4 |
        //     `===^===^===^===^==='          `===^===^===^===^==='
        //             ^------------ origin           ^------------ origin
        //..

    ~AutoDestructor();
        // Destroy this range proctor along with the contiguous sequence of
        // objects it manages (if any) by invoking the destructor of each
        // (managed) object.  Note that the order in which the managed objects
        // are destroyed is undefined.

    // MANIPULATORS
    void operator++();
        // Increase by one the (signed) length of the sequence of objects
        // managed by this range proctor.  The behavior is undefined unless the
        // origin of the sequence of objects managed by this proctor is
        // non-zero.  The behavior is undefined unless the origin of this range
        // proctor is non-zero.  Note that if the length of this proctor is
        // currently negative, the number of managed objects will decrease by
        // one, whereas if the length is non-negative, the number of managed
        // objects will increase by one.

    void operator--();
        // Decrease by one the (signed) length of the sequence of objects
        // managed by this range proctor.  The behavior is undefined unless the
        // origin of the sequence of objects managed by this proctor is
        // non-zero.  The behavior is undefined unless the origin of this range
        // proctor is non-zero.  Note that if the length of this proctor is
        // currently positive, the number of managed objects will decrease by
        // one, whereas if the length is non-positive, the number of managed
        // objects will increase by one.

    void release();
        // Release from management the sequence of objects currently managed by
        // this range proctor by setting the length of the managed sequence to
        // 0.  All objects currently under management will become unmanaged
        // (i.e., when the proctor goes out of scope and it was not assigned
        // another sequence of objects to manage by invoking 'reset', no
        // objects will be destroyed).  If no objects are currently being
        // managed, this method has no effect.  Note that the origin is not
        // affected.

    void reset(TYPE *origin);
        // Set the specified 'origin' of the sequence of objects managed by
        // this range proctor.  This method does not destroy the
        // previously-managed objects or affect the length of the sequence
        // managed.  The behavior is undefined unless 'origin' is non-zero.
        // Note that 'reset' can be called without having previously called
        // 'release' provided that 'length' is set appropriately before this
        // proctor could be destroyed (e.g., via a thrown exception).

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
                        // class AutoDestructor
                        // --------------------

// PRIVATE MANIPULATORS
template <class TYPE>
void AutoDestructor<TYPE>::destroy()
{
    if (0 < d_length) {
        for (; d_length > 0; --d_length, ++d_origin_p) {
            d_origin_p->~TYPE();
        }
    }
    else {
        --d_origin_p;
        for (; d_length < 0; ++d_length, --d_origin_p) {
            d_origin_p->~TYPE();
        }
    }
}

// CREATORS
template <class TYPE>
inline
AutoDestructor<TYPE>::AutoDestructor(TYPE *origin, int length)
: d_origin_p(origin)
, d_length(length)
{
    BSLS_ASSERT_SAFE(origin || !length);
}

template <class TYPE>
inline
AutoDestructor<TYPE>::~AutoDestructor()
{
    BSLS_ASSERT_SAFE(d_origin_p || !d_length);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(d_length)) {
        destroy();
    }
}

// MANIPULATORS
template <class TYPE>
inline
void AutoDestructor<TYPE>::operator++()
{
    BSLS_ASSERT_SAFE(d_origin_p);

    ++d_length;
}

template <class TYPE>
inline
void AutoDestructor<TYPE>::operator--()
{
    BSLS_ASSERT_SAFE(d_origin_p);

    --d_length;
}

template <class TYPE>
inline
void AutoDestructor<TYPE>::release()
{
    d_length = 0;
}

template <class TYPE>
inline
void AutoDestructor<TYPE>::reset(TYPE *origin)
{
    BSLS_ASSERT_SAFE(origin);

    d_origin_p = origin;
}

template <class TYPE>
inline
void AutoDestructor<TYPE>::setLength(int length)
{
    BSLS_ASSERT_SAFE(d_origin_p);

    d_length = length;
}

// ACCESSORS
template <class TYPE>
inline
int AutoDestructor<TYPE>::length() const
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
