// bslalg_autoarraydestructor.h                                       -*-C++-*-
#ifndef INCLUDED_BSLALG_AUTOARRAYDESTRUCTOR
#define INCLUDED_BSLALG_AUTOARRAYDESTRUCTOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a proctor for destroying arrays.
//
//@CLASSES:
//  bslalg::AutoArrayDestructor: exception-neutrality proctor for arrays
//
//@SEE_ALSO: bslma_autodestructor
//
//@AUTHOR: Herve Bronnimann (hbronnim)
//
//@DESCRIPTION: This component provides a proctor object to manage a contiguous
// (in-place) sequence of otherwise-unmanaged instances of a user-defined type.
// If not explicitly released, all objects managed by the proctor object are
// automatically destroyed by the proctor's destructor, using the
// 'bslalg_arraydestructionprimitives'.
//
///Usage
///-----
// The usage example is nearly identical to that of 'bslma_autodestructor', so
// we will only quote and adapt a small portion of that usage example.  Namely,
// we will focus on an array that supports arbitrary user-defined types, and
// suppose that we want to implement insertion of an arbitrary number of
// elements at some (intermediate) position in the array, taking care that if
// an element copy constructor or assignment operator throws, the whole array
// is left in a valid (but unspecified) state.
//
// Consider the implementation of the 'insert' method for a templatized array
// below.  The proctor's *origin* is set (at construction) to refer to the
// 'numItems' position past 'array[length]'.  Initially, the proctor manages no
// objects (i.e., its end is the same as its beginning).
//..
//     0     1     2     3     4     5     6     7
//   _____ _____ _____ _____ _____ _____ _____ _____
//  | "A" | "B" | "C" | "D" | "E" |xxxxx|xxxxx|xxxxx|
//  `=====^=====^=====^=====^=====^=====^=====^====='
//  my_Array                                  ^----- AutoArrayDestructor
//  (length = 5)
//
//              Figure: Use of proctor for my_Array::insert
//..
// As each of the elements at index positions beyond the insertion position is
// shifted up by two index positions, the proctor's begin address is
// *decremented*.  At the same time, the array's length is *decremented* to
// ensure that each array element is always being managed (during an allocation
// attempt) either by the proctor or the array itself, but not both.
//..
//     0     1     2     3     4     5     6     7
//   _____ _____ _____ _____ _____ _____ _____ _____
//  | "A" | "B" | "C" | "D" |xxxxx|xxxxx| "E" |xxxxx|
//  `=====^=====^=====^=====^=====^=====^=====^====='
//  my_Array                            ^     ^ AutoArrayDestructor::end
//  (length = 4)                        `---- AutoArrayDestructor::begin
//
//              Figure: Configuration after shifting up one element
//..
// After the required number of elements have been shifted, the hole is filled
// (backwards) by copies of the element to be inserted.  The code for the
// templatized 'insert' method is as follows:
//..
//  // Assume no aliasing.
//  template <class TYPE> inline
//  void my_Array<TYPE>::insert(int dstIndex, const TYPE& item, int numItems)
//  {
//      if (d_length >= d_size) {
//          this->increaseSize(numItems);
//      }
//
//      int   origLen = d_length;
//      TYPE *src     = &d_array_p[d_length];
//      TYPE *src     = &d_array_p[d_length + numItems];
//      bslalg::AutoArrayDestructor<TYPE> autoDtor(dest, dest);
//
//      for (int i = d_length; i > dstIndex; --i, --d_length) {
//          dest = autoDtor.moveBegin(-1);  // decrement destination
//          new(dest) TYPE(--src);          // copy to new index
//          src->~TYPE();                   // destroy original
//      }
//      for (int i = numItems; i > 0; --i) {
//          dest = autoDtor.moveBegin(-1);  // decrement destination
//          new(dest) TYPE(item);           // copy new value into hole
//      }
//      autoDtor.release();
//      d_length = origLen + numItems;
//  }
//..
// Note that in the 'insert' example above, we illustrate exception neutrality,
// but not alias safety (i.e., in the case when 'item' is a reference into the
// portion of the array at 'dstIndex' or beyond).

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_ARRAYDESTRUCTIONPRIMITIVES
#include <bslalg_arraydestructionprimitives.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_CSTDDEF
#include <cstddef>  // size_t
#define INCLUDED_CSTDDEF
#endif

namespace BloombergLP {

namespace bslalg {

                        // =========================
                        // class AutoArrayDestructor
                        // =========================

template <class OBJECT_TYPE>
class AutoArrayDestructor {
    // This 'class' provides a specialized proctor object that, upon
    // destruction and unless the 'release' method has been called, destroys
    // the elements in a segment of an array of parameterized type
    // 'OBJECT_TYPE'.  The elements destroyed are delimited by the "guarded"
    // range '[ begin(), end() )'.

    // DATA
    OBJECT_TYPE *d_begin_p;  // address of first element in guarded range

    OBJECT_TYPE *d_end_p;    // first address beyond last element in guarded
                             // range

  private:
    // NOT IMPLEMENTED
    AutoArrayDestructor(const AutoArrayDestructor&);
    AutoArrayDestructor& operator=(const AutoArrayDestructor&);

  public:
    // TYPES
    typedef std::ptrdiff_t difference_type;

    // CREATORS
    AutoArrayDestructor(OBJECT_TYPE *begin, OBJECT_TYPE *end);
        // Create an array exception guard object for the sequence of elements
        // of the parameterized 'OBJECT_TYPE' delimited by the range specified
        // by '[ begin, end )'.  The behavior is undefined unless
        // 'begin <= end' and each element in the range '[ begin, end )' has
        // been initialized.

    ~AutoArrayDestructor();
        // Call the destructor on each of the elements of the parameterized
        // 'OBJECT_TYPE' delimited by the range '[ begin(), end() )' and
        // destroy this array exception guard.

    // MANIPULATORS
    OBJECT_TYPE *moveBegin(difference_type offset = -1);
        // Move the begin pointer by the specified 'offset', and return the new
        // begin pointer.

    OBJECT_TYPE *moveEnd(difference_type offset = 1);
        // Move the end pointer by the specified 'offset', and return the new
        // end pointer.

    void release();
        // Set the range of elements guarded by this object to be empty.  Note
        // that 'begin() == end()' following this operation, but the specific
        // value is unspecified.
};

// ===========================================================================
//                          INLINE FUNCTION DEFINITIONS
// ===========================================================================

                      // -------------------------
                      // class AutoArrayDestructor
                      // -------------------------

// CREATORS
template <class OBJECT_TYPE>
inline
AutoArrayDestructor<OBJECT_TYPE>::AutoArrayDestructor(OBJECT_TYPE *begin,
                                                      OBJECT_TYPE *end)
: d_begin_p(begin)
, d_end_p(end)
{
    BSLS_ASSERT_SAFE(!begin == !end);
    BSLS_ASSERT_SAFE(begin <= end);
}

template <class OBJECT_TYPE>
inline
AutoArrayDestructor<OBJECT_TYPE>::~AutoArrayDestructor()
{
    BSLS_ASSERT_SAFE(!d_begin_p == !d_end_p);
    BSLS_ASSERT_SAFE(d_begin_p <= d_end_p);

    ArrayDestructionPrimitives::destroy(d_begin_p, d_end_p);
}

// MANIPULATORS
template <class OBJECT_TYPE>
inline
OBJECT_TYPE *AutoArrayDestructor<OBJECT_TYPE>::moveBegin(
                                                        difference_type offset)
{
    BSLS_ASSERT_SAFE(d_begin_p || 0 == offset);
    BSLS_ASSERT_SAFE(d_end_p - d_begin_p >= offset);

    d_begin_p += offset;
    return d_begin_p;
}

template <class OBJECT_TYPE>
inline
OBJECT_TYPE *AutoArrayDestructor<OBJECT_TYPE>::moveEnd(difference_type offset)
{
    BSLS_ASSERT_SAFE(d_end_p || 0 == offset);
    BSLS_ASSERT_SAFE(d_end_p - d_begin_p >= -offset);

    d_end_p += offset;
    return d_end_p;
}

template <class OBJECT_TYPE>
inline
void AutoArrayDestructor<OBJECT_TYPE>::release()
{
    d_begin_p = d_end_p;
}

}  // close package namespace

}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
