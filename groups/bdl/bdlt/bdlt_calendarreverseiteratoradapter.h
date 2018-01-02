// bdlt_calendarreverseiteratoradapter.h                              -*-C++-*-
#ifndef INCLUDED_BDLT_CALENDARREVERSEITERATORADAPTER
#define INCLUDED_BDLT_CALENDARREVERSEITERATORADAPTER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide reverse iterator adapter for calendar iterators.
//
//@CLASSES:
//  bdlt::CalendarReverseIteratorAdapter: calendar reverse iterator adapter
//
//@SEE_ALSO: bdlt_calendar, bdlt_packedcalendar
//
//@DESCRIPTION: This component provides a template,
// 'bdlt::CalendarReverseIteratorAdapter', that can be used to adapt a calendar
// iterator to be a reverse iterator (see 'bdlt_calendar' and
// 'bdlt_packedcalendar').  Calendar iterators cannot return a reference to an
// underlying element of the calendar and hence cannot be used with
// 'bsl::reverse_iterator'.  The reverse iterator adapter defined in this
// component provides a subset of the 'bsl::reverse_iterator' interface that
// can be used with the calendar iterators defined in 'bdlt'.  Specifically,
// the types 'value_type', 'difference_type', 'pointer', and 'reference' are
// defined but 'iterator_category' is not defined (since this is not a
// fully-compliant iterator).  Furthermore, the methods appropriate for
// random-access iterators are not included (e.g., 'operator+=').
//
///Limitation
///----------
// 'bdlt::CalendarReverseIteratorAdapter' is *not* a fully-compliant
// implementation of 'std::reverse_iterator' according to the C++ standard.  It
// is an implementation of the minimum functionality needed to support the
// public iterators in the 'bdlt_calendar' and 'bdlt_packedcalendar'
// components.  Within that limitation, it is a subset implementation of
// 'bsl::reverse_iterator'.  Specifically, 'iterator_category' is not defined
// for this adapter and the methods of a 'bsl::reverse_iterator' relevant only
// to random-access compliant 'ITERATOR' types are omitted.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Use of 'bdlt::CalendarReverseIteratorAdapter'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we will use the 'bdlt::CalendarReverseIteratorAdapter' to
// traverse an iterable container type.  Specifically, we will create an array
// of 'struct' values, implement a bidirectional iterator class that is a
// forward iterator for this array, and then use
// 'bdlt::CalendarReverseIteratorAdapter' to provide a reverse iterator that
// will be used to traverse the array.
//
// First, we define a bidirectional iterator class:
//..
//  template <class TYPE>
//  class Iterator {
//      // This 'class' basically behaves as a pointer to the (template
//      // parameter) 'TYPE' with 4 types defined to allow the use of
//      // 'bdlt::CalendarReverseIteratorAdapter' with this 'class'.  Note that
//      // this 'class' supports only a subset of the functionality that a
//      // pointer would, but this subset covers all the functionality that a
//      // 'bdlt::CalendarReverseIteratorAdapter' needs.
//
//      // DATA
//      TYPE *d_ptr;  // pointer to the element referred to by this iterator
//
//      // FRIENDS
//      template <class OTHER>
//      friend bool operator==(const Iterator<OTHER>&, const Iterator<OTHER>&);
//
//    public:
//      // PUBLIC TYPES
//      typedef TYPE   value_type;
//      typedef int    difference_type;
//      typedef TYPE  *pointer;
//      typedef TYPE&  reference;
//
//      // CREATORS
//      Iterator()
//          // Create an 'Iterator' object having the default value.
//      : d_ptr(0)
//      {
//      }
//
//      Iterator(TYPE *value)                                       // IMPLICIT
//          // Create an 'Iterator' object from the specified 'value'.
//      : d_ptr(value)
//      {
//      }
//
//      // Iterator(const Iterator&) = default;
//
//      // ~Iterator() = default;
//
//      // MANIPULATORS
//      // Iterator& operator=(const Iterator&) = default;
//
//      Iterator& operator++()
//          // Increment to the next element in the iteration sequence, and
//          // return a reference providing modifiable access to this iterator.
//          // The behavior is undefined if, on entry, this iterator has the
//          // past-the-end value for an iterator over the underlying sequence.
//      {
//          ++d_ptr;
//          return *this;
//      }
//
//      Iterator& operator--()
//          // Decrement to the previous element in the iteration sequence, and
//          // return a reference providing modifiable access to this iterator.
//          // The behavior is undefined if, on entry, this iterator has the
//          // same value as an iterator at the start of the underlying
//          // sequence.
//      {
//          --d_ptr;
//          return *this;
//      }
//
//      // ACCESSORS
//      reference operator*() const
//          // Return a reference to the element referred to by this iterator.
//          // The behavior is undefined unless this iterator is within the
//          // bounds of the underlying sequence.
//      {
//          return *d_ptr;
//      }
//
//      pointer operator->() const
//          // Return a pointer to the element referred to by this iterator.
//          // The behavior is undefined unless this iterator is within the
//          // bounds of the underlying sequence.
//      {
//          return d_ptr;
//      }
//
//      Iterator operator+(bsl::ptrdiff_t offset) const
//          // Return an iterator referencing the location at the specified
//          // 'offset' from the element referenced by this iterator.  The
//          // behavior is undefined unless the resultant iterator is within
//          // the bounds of the underlying sequence.
//      {
//          return Iterator(d_ptr + offset);
//      }
//  };
//
//  // FREE OPERATORS
//  template <class TYPE>
//  inline
//  bool operator==(const Iterator<TYPE>& lhs,  const Iterator<TYPE>& rhs)
//      // Return 'true' if the specified 'lhs' iterator has the same value as
//      // the specified 'rhs' iterator, and 'false' otherwise.  Two iterators
//      // have the same value if they refer to the same element, or both have
//      // the past-the-end value for am iterator over the underlying iteration
//      // sequence.  The behavior is undefined unless 'lhs' and 'rhs' refer to
//      // the same underlying sequence.
//  {
//      return lhs.d_ptr == rhs.d_ptr;
//  }
//
//  template <class TYPE>
//  inline
//  bool operator!=(const Iterator<TYPE>& lhs, const Iterator<TYPE>& rhs)
//      // Return 'true' if the specified 'lhs' iterator does not have the same
//      // value as the specified 'rhs' iterator, and 'false' otherwise.  Two
//      // iterators do not have the same value if (1) they do not refer to the
//      // same element and (2) both do not have the past-the-end value for an
//      // iterator over the underlying iteration sequence.  The behavior is
//      // undefined unless 'lhs' and 'rhs' refer to the same underlying
//      // sequence.
//  {
//      return !(lhs == rhs);
//  }
//..
// Then, we define 'struct' 'S', the type that will be referred to by the
// 'Iterator' type:
//..
//  struct S {
//      char d_c;
//      int  d_i;
//  };
//..
// The 'struct' 'S' has two data members.  By creating an array of distinct 'S'
// values, the state of an iterator referring to an element of this array can
// be easily verified by inspecting these two members.
//
// Next, we define four (distinct) 'S' values:
//..
//  const S s0 = { 'A', 3 };
//  const S s1 = { 'B', 5 };
//  const S s2 = { 'C', 7 };
//  const S s3 = { 'D', 9 };
//..
// Then, we define 's', an array of 'S' values:
//..
//  S s[] = { s0, s1, s2, s3 };
//  enum { NUM_S = sizeof s / sizeof *s };
//..
// Next, we define an iterator, 'sfBegin', referring to the first element of
// 's' and an iterator, 'sfEnd', having the past-the-end value for an iterator
// over 's':
//..
//  Iterator<S> sfBegin(s + 0), sfEnd(s + NUM_S);
//..
// Then, for convenience we declare our reverse iterator type that will be used
// to traverse 's' in the reverse direction:
//..
//  typedef bdlt::CalendarReverseIteratorAdapter<Iterator<S> > Reverse;
//..
// Next, we declare begin and end reverse iterators to our range of 'S' values:
//..
//  const Reverse rBegin(sfEnd), rEnd(sfBegin);
//..
// Now, we traverse our range in the reverse direction, from 'rBegin' to
// 'rEnd', streaming out the contents of the 'S' values as we go:
//..
//  bsl::ostringstream stream;
//  for (Reverse it = rBegin; rEnd != it; ++it) {
//      stream << (rBegin == it ? "" : ", ")
//             << "{ "
//             << it->d_c
//             << ", "
//             << it->d_i
//             << " }";
//  }
//  stream << bsl::flush;
//..
// Finally, we verify the contents of the range output:
//..
//  assert(stream.str() == "{ D, 9 }, { C, 7 }, { B, 5 }, { A, 3 }");
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

namespace BloombergLP {
namespace bdlt {

                    // ====================================
                    // class CalendarReverseIteratorAdapter
                    // ====================================

template <class ITERATOR>
class CalendarReverseIteratorAdapter {
    // This reverse iterator adapter  provides a subset of the
    // 'bsl::reverse_iterator' interface that can be used with the calendar
    // iterators defined in 'bdlt'.  Specifically, the types 'value_type',
    // 'difference_type', 'pointer', and 'reference' are defined but
    // 'iterator_category' is not defined (since this is not a fully-compliant
    // iterator).  Furthermore, the methods appropriate for random-access
    // iterators are not included (e.g., 'operator+=').

    // DATA
    ITERATOR d_forwardIter;  // bidirectional iterator referring to current
                             // position

  public:
    // PUBLIC TYPES
    typedef typename ITERATOR::value_type      value_type;
    typedef typename ITERATOR::difference_type difference_type;
    typedef typename ITERATOR::pointer         pointer;
    typedef typename ITERATOR::reference       reference;

    // CREATORS
    CalendarReverseIteratorAdapter();
        // Create a reverse iterator having the default value.  The
        // default-constructed reverse iterator does not have a singular value
        // unless an object of the type specified by the template parameter
        // 'ITERATOR' has a singular value after default construction.

    explicit
    CalendarReverseIteratorAdapter(const ITERATOR& value);
        // Create a reverse iterator referring to the element that precedes, in
        // the forward sequence (or that follows, in the backward sequence) the
        // element referred to by the specified 'value'.

//! CalendarReverseIteratorAdapter(
//!                  const CalendarReverseIteratorAdapter& original) = default;
        // Create a 'CalendarReverseIteratorAdapter' object having the value of
        // the specified 'original' object.

//! ~CalendarReverseIteratorAdapter() = default;
        // Destroy this 'CalendarReverseIteratorAdapter' object.

    // MANIPULATORS
    CalendarReverseIteratorAdapter& operator=(
                                    const CalendarReverseIteratorAdapter& rhs);
        // Assign the value of the specified 'rhs' to this object, and return a
        // reference providing modifiable access to this object.

    CalendarReverseIteratorAdapter& operator=(const ITERATOR& rhs);
        // Set the value of this object to refer to the element that precedes,
        // in the forward sequence (or that follows, in the backward sequence)
        // the element referred to by the specified 'rhs', and return a
        // reference providing modifiable access to this object.

    CalendarReverseIteratorAdapter& operator++();
        // Modify this reverse iterator to refer to the next element in the
        // reverse iteration sequence, and return a reference providing
        // modifiable access to this reverse iterator.  The behavior is
        // undefined unless, on entry, this reverse iterator does not have the
        // past-the-end value for a reverse iterator over the underlying
        // sequence.

    CalendarReverseIteratorAdapter& operator--();
        // Modify this reverse iterator to refer to the previous element in the
        // reverse iteration sequence, and return a reference providing
        // modifiable access to this reverse iterator.  The behavior is
        // undefined unless, on entry, this reverse iterator does not have the
        // same value as a reverse iterator at the start of the underlying
        // sequence.

    // ACCESSORS
    reference operator*() const;
        // Return a reference to the element referred to by this reverse
        // iterator.  The behavior is undefined unless this iterator is within
        // the bounds of the underlying sequence.

    pointer operator->() const;
        // Return a pointer to the element referred to by this reverse
        // iterator.  The behavior is undefined unless this iterator is within
        // the bounds of the underlying sequence.

    ITERATOR forwardIterator() const;
        // Return the forward iterator referring to the element in the forward
        // sequence after the element referred to by this reverse iterator.
};

// FREE OPERATORS
template <class ITERATOR>
bool operator==(const CalendarReverseIteratorAdapter<ITERATOR>& lhs,
                const CalendarReverseIteratorAdapter<ITERATOR>& rhs);
    // Return 'true' if the specified 'lhs' reverse iterator has the same value
    // as the specified 'rhs' reverse iterator, and 'false' otherwise.  Two
    // reverse iterators have the same value if they refer to the same element,
    // or both have the past-the-end value for a reverse iterator over the
    // underlying reverse iteration sequence.  The behavior is undefined unless
    // 'lhs' and 'rhs' refer to the same underlying sequence.

template <class ITERATOR>
bool operator!=(const CalendarReverseIteratorAdapter<ITERATOR>& lhs,
                const CalendarReverseIteratorAdapter<ITERATOR>& rhs);
    // Return 'true' if the specified 'lhs' reverse iterator does not have the
    // same value as the specified 'rhs' reverse iterator, and 'false'
    // otherwise.  Two reverse iterators do not have the same value if (1) they
    // do not refer to the same element and (2) both do not have the
    // past-the-end value for a reverse iterator over the underlying reverse
    // iteration sequence.  The behavior is undefined unless 'lhs' and 'rhs'
    // refer to the same underlying sequence.

template <class ITERATOR>
CalendarReverseIteratorAdapter<ITERATOR> operator++(
                      CalendarReverseIteratorAdapter<ITERATOR>& iterator, int);
    // Modify the specified 'iterator' to refer to the next element in the
    // reverse iteration sequence, and return a reverse iterator having the
    // pre-increment value of 'iterator'.  The behavior is undefined unless, on
    // entry, 'iterator' does not have the past-the-end value for a reverse
    // iterator over the underlying sequence.

template <class ITERATOR>
CalendarReverseIteratorAdapter<ITERATOR> operator--(
                      CalendarReverseIteratorAdapter<ITERATOR>& iterator, int);
    // Modify the specified 'iterator' to refer to the previous element in the
    // reverse iteration sequence, and return a reverse iterator having the
    // pre-decrement value of 'iterator'.  The behavior is undefined unless, on
    // entry, 'iterator' does not have the same value as a reverse iterator to
    // the start of the underlying sequence.

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                    // ------------------------------------
                    // class CalendarReverseIteratorAdapter
                    // ------------------------------------

// CREATORS
template <class ITERATOR>
inline
CalendarReverseIteratorAdapter<ITERATOR>::CalendarReverseIteratorAdapter()
: d_forwardIter()
{
}

template <class ITERATOR>
inline
CalendarReverseIteratorAdapter<ITERATOR>::CalendarReverseIteratorAdapter(
                                                         const ITERATOR& value)
: d_forwardIter(value)
{
}

// MANIPULATORS
template <class ITERATOR>
inline
CalendarReverseIteratorAdapter<ITERATOR>&
CalendarReverseIteratorAdapter<ITERATOR>::operator=(
                                     const CalendarReverseIteratorAdapter& rhs)
{
    d_forwardIter = rhs.d_forwardIter;
    return *this;
}

template <class ITERATOR>
inline
CalendarReverseIteratorAdapter<ITERATOR>&
CalendarReverseIteratorAdapter<ITERATOR>::operator=(const ITERATOR& rhs)
{
    d_forwardIter = rhs;
    return *this;
}

template <class ITERATOR>
inline
CalendarReverseIteratorAdapter<ITERATOR>&
CalendarReverseIteratorAdapter<ITERATOR>::operator++()
{
    --d_forwardIter;
    return *this;
}

template <class ITERATOR>
inline
CalendarReverseIteratorAdapter<ITERATOR>&
CalendarReverseIteratorAdapter<ITERATOR>::operator--()
{
    ++d_forwardIter;
    return *this;
}

// ACCESSORS
template <class ITERATOR>
inline
typename CalendarReverseIteratorAdapter<ITERATOR>::reference
CalendarReverseIteratorAdapter<ITERATOR>::operator*() const
{
    ITERATOR tmp = d_forwardIter;
    return *--tmp;
}

template <class ITERATOR>
inline
typename CalendarReverseIteratorAdapter<ITERATOR>::pointer
CalendarReverseIteratorAdapter<ITERATOR>::operator->() const
{
    ITERATOR tmp = d_forwardIter;
    return &*--tmp;
}

template <class ITERATOR>
inline
ITERATOR CalendarReverseIteratorAdapter<ITERATOR>::forwardIterator() const
{
    return d_forwardIter;
}

}  // close package namespace

// FREE OPERATORS
template <class ITERATOR>
inline
bool bdlt::operator==(const CalendarReverseIteratorAdapter<ITERATOR>& lhs,
                      const CalendarReverseIteratorAdapter<ITERATOR>& rhs)
{
    return lhs.forwardIterator() == rhs.forwardIterator();
}

template <class ITERATOR>
inline
bool bdlt::operator!=(const CalendarReverseIteratorAdapter<ITERATOR>& lhs,
                      const CalendarReverseIteratorAdapter<ITERATOR>& rhs)
{
    return lhs.forwardIterator() != rhs.forwardIterator();
}

template <class ITERATOR>
inline
bdlt::CalendarReverseIteratorAdapter<ITERATOR>
      bdlt::operator++(CalendarReverseIteratorAdapter<ITERATOR>& iterator, int)
{
    CalendarReverseIteratorAdapter<ITERATOR> tmp = iterator;
    ++iterator;
    return tmp;
}

template <class ITERATOR>
inline
bdlt::CalendarReverseIteratorAdapter<ITERATOR>
      bdlt::operator--(CalendarReverseIteratorAdapter<ITERATOR>& iterator, int)
{
    CalendarReverseIteratorAdapter<ITERATOR> tmp = iterator;
    --iterator;
    return tmp;
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
