// bdema_autodestructor.h      -*-C++-*-
#ifndef INCLUDED_BDEMA_AUTODESTRUCTOR
#define INCLUDED_BDEMA_AUTODESTRUCTOR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Destroy a sequence of otherwise-unmanaged objects at destruction.
//
//@DEPRECATED: Use 'bslma_autodestructor' instead.
//
//@CLASSES:
//   bdema_AutoDestructor: proctor to manage a sequence of objects
//
//@AUTHOR: Shao-wei Hung (shung1)
//
//@SEE_ALSO: bdema_autodeallocator, bdema_autodeleter
//
//@DESCRIPTION: This component provides a proctor object to manage a contiguous
// (in-place) sequence of otherwise-unmanaged instances of a user-defined type.
// If not explicitly released, all objects managed by the proctor object are
// automatically destroyed by the proctor's destructor.
//
///Usage
///-----
// 'bdema_AutoDestructor' can be used to auto-destroy orphaned array elements
// when an exception occurs.  The following illustrates the insertion operation
// for a string array.  Assume that a string array initially contains the
// following five elements:
//..
//                 0     1     2     3     4
//               _____ _____ _____ _____ _____ __
//              | "A" | "B" | "C" | "D" | "E" |
//              `=====^=====^=====^=====^=====^==
//..
// To insert a string "F" at index position 2, the existing elements at index
// positions 2 - 4 are first shifted (we assume here and below that the array
// has sufficient capacity):
//..
//                 0     1     2     3     4     5
//               _____ _____ _____ _____ _____ _____ __
//              | "A" | "B" |xxxxx| "C" | "D" | "E" |
//              `=====^=====^=====^=====^=====^=====^==
//
//               Note: "xxxxx" denotes uninitialized memory
//..
// Next, a new string element must be created and initialized with the string
// "F".  If, during creation, an allocation fails and an exception is thrown,
// the array will be left in an invalid state because the element at index
// position 2 is invalid.  We can restore exception neutrality by setting the
// array length to 2 before attempting to create the string "F", but there is
// still a problem: the strings "C", "D" and "E" (at index positions 3 - 5) are
// "orphaned" and will never be destroyed -- a memory leak.  To prevent this
// potential memory leak, we can additionally create an instance of
// 'bdema_AutoDestructor' to manage (temporarily) the elements at index
// positions 3 - 5 prior to creating the new element:
//..
//                0     1     2     3     4     5
//              _____ _____ _____ _____ _____ _____ __
//             | "A" | "B" |xxxxx| "C" | "D" | "E" |
//             `=====^=====^=====^=====^=====^=====^==
//             my_StringArray    ^----------------- bdema_AutoDestructor
//             (length = 2)                         (length = 3)
//
//              Figure: Use of proctor for my_StringArray::insert
//..
// If an exception occurs, the array (now of length 2) is in a perfectly valid
// state, while the proctor is responsible for destroying the orphaned elements
// at index positions 3 - 5.  If no exception is thrown, the proctor's
// 'release()' method is called, releasing its control over the temporarily
// managed contents.
//
// For an in-place string array, an exception-neutral (but not alias-safe)
// 'insert' method can be implemented using the 'bdema_AutoDestructor' as
// follows:
//..
//  // Assume no aliasing.
//  void my_StrArray::insert(int dstIndex, const char *item)
//  {
//      assert(0 <= dstIndex);  assert(dstIndex <= d_length);
//
//      if (d_length >= d_size) {
//          increaseSize();
//      }
//      assert(d_size > d_length);
//
//      my_String *here = d_array_p + dstIndex;
//      memmove(here + 1, here, (d_length - dstIndex) * sizeof *d_array_p);
//
//      int origLen = d_length;
//      d_length = dstIndex;
//      bdema_AutoDestructor<my_String> autoDtor(&d_array_p[dstIndex + 1],
//                                               origLen - dstIndex);
//      new(&d_array_p[dstIndex]) my_String(item);
//      autoDtor.release();
//
//      d_length = origLen + 1;
//  }
//..
// Note that because we know that the implementation of 'my_String' does not
// contain any self-referential pointers, we can shift the elements in the
// array using 'memmove'.  For an array that supports arbitrary user-defined
// types, we must use the placement 'new' operator (and destructor) to copy the
// elements one by one.  For each placement 'new' operation, we must modify the
// number of elements managed by the proctor to correspond to the number of
// elements that would be orphaned.  Consider the implementation of the
// 'insert' method for a templatized array below.  The proctor's *origin* is
// set (at construction) to refer to the index position one (i.e., the number
// of elements to be inserted) past 'array[length]'.  Initially, the proctor
// manages no objects (i.e., its length is 0).
//..
//                 0     1     2     3     4     5     6
//               _____ _____ _____ _____ _____ _____ _____
//              | "A" | "B" | "C" | "D" | "E" |xxxxx|xxxxx|
//              `=====^=====^=====^=====^=====^=====^====='
//              my_Array                            ^----- bdema_AutoDestructor
//              (length = 5)                               (length = 0)
//
//              Figure: Use of proctor for my_Array::insert
//..
// As each of the elements at index positions beyond the insertion position is
// shifted up by one index position, the proctor (proctor's length) is
// *decremented*, thereby *extending* by one the sequence of elements it
// manages *below* its origin (note that when the proctor's length is non-
// positive, the element at the origin is *not* managed).  At the same time,
// the array's length is *decremented* to ensure that each array element is
// always being managed (during an allocation attempt) either by the proctor or
// the array itself, but not both.
//..
//                 0     1     2     3     4     5     6
//               _____ _____ _____ _____ _____ _____ _____
//              | "A" | "B" | "C" | "D" |xxxxx| "E" |xxxxx|
//              `=====^=====^=====^=====^=====^=====^====='
//              my_Array                            ^----- bdema_AutoDestructor
//              (length = 4)                               (length = -1)
//
//              Figure: Configuration after shifting up one element
//..
// The code for the (non-alias-safe) templatized 'insert' method is as follows:
//..
//  // Assume no aliasing.
//  template <class TYPE>
//  inline
//  void my_Array<TYPE>::insert(int dstIndex, const TYPE& item)
//  {
//      if (d_length >= d_size) {
//          this->increaseSize();
//      }
//
//      bdema_AutoDestructor<TYPE> autoDtor(&d_array_p[d_length + 1], 0);
//      int origLen = d_length;
//      for (int i = d_length - 1; i >= dstIndex; --i,--autoDtor,--d_length) {
//          new(&d_array_p[i + 1]) TYPE(d_array_p[i]); // copy to new index
//          d_array_p[i].~TYPE();                      // destroy original
//      }
//
//      new(&d_array_p[dstIndex]) TYPE(item);
//      autoDtor.release();
//      d_length = origLen + 1;
//  }
//..
// In the implementation of the 'remove' method for a templatized array below,
// the proctor's *origin* is set (at construction) to refer to the index
// position 'array[length]'.  The proctor's length is initialized so as to
// manage all of the elements from the one following the element to be removed
// up to (but not including) the *origin* (note that the length of the proctor
// will be negative in this example, since it manages elements below its
// origin).  The array's length is temporarily adjusted to the index position
// of the element being removed.
//..
//                 0     1     2     3     4     5
//               _____ _____ _____ _____ _____ _____
//              | "A" | "B" |xxxxx| "D" | "E" |xxxxx|
//              `=====^=====^=====^=====^=====^====='
//              my_Array                      ^----- bdema_AutoDestructor
//              (length = 2)                         (length = -2)
//
//              Figure: Use of proctor for my_Array::remove
//..
// As each of the elements at index positions beyond the removal position is
// shifted down by one index position, the proctor (proctor's length) is
// *incremented*, thereby *decreasing* by one the number of elements it manages
// below its origin.  At the same time, the array's length is *incremented* to
// ensure that each array element is always being managed by some object:
//..
//                 0     1     2     3     4     5
//               _____ _____ _____ _____ _____ _____
//              | "A" | "B" | "D" |xxxxx| "E" |xxxxx|
//              `=====^=====^=====^=====^=====^====='
//              my_Array                      ^----- bdema_AutoDestructor
//              (length = 3)                         (length = -1)
//
//              Figure: Configuration after shifting down one element
//..
// The code for the templatized 'remove' method is as follows:
//..
//  template <class TYPE>
//  inline
//  void my_Array<TYPE>::remove(int dstIndex)
//  {
//      d_array_p[dstIndex].~TYPE();
//      bdema_AutoDestructor<TYPE> autoDtor(&d_array_p[d_length],
//                                          dstIndex + 1 - d_length);
//      int origLen = d_length;
//      d_length = dstIndex;
//      for (int i = dstIndex + 1; i < origLen; ++i, ++autoDtor, ++d_length) {
//          new(&d_array_p[i - 1]) TYPE(d_array_p[i]); // copy to new index
//          d_array_p[i].~TYPE();                      // destroy original
//      }
//
//      autoDtor.release();
//      d_length = origLen - 1;
//  }
//..
// Note that in the two 'insert' examples above, we illustrate exception
// neutrality, but not alias safety.  The 'remove' method has no alias issue,
// and so is completely correct as written.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLMA_AUTODESTRUCTOR
#include <bslma_autodestructor.h>
#endif

#ifndef bdema_AutoDestructor
#define bdema_AutoDestructor   bslma_AutoDestructor
    // This class implements a proctor that automatically destroys its managed
    // objects at destruction unless its 'release' method is invoked.  Note
    // that the order in which the managed objects are destroyed is undefined.
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
