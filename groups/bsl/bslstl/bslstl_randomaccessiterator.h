// bslstl_randomaccessiterator.h                                      -*-C++-*-
#ifndef INCLUDED_BSLSTL_RANDOMACCESSITERATOR
#define INCLUDED_BSLSTL_RANDOMACCESSITERATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a template to create STL-compliant random access iterators.
//
//@CLASSES:
//  bslstl::RandomAccessIterator: random-access iterator template
//
//@SEE_ALSO: bslstl_iterator, bslstl_forwarditerator,
//           bslstl_bidirectionaliterator
//
//@DESCRIPTION: This component provides an iterator adaptor that, given an
// implementation class defining a core set of iterator functionality specified
// in the class level documentation, adapts it to provide an STL-compliant
// random access iterator interface.  'bslstl::RandomAccessIterator' meets the
// requirements of a random access iterator described in the C++11 standard
// [24.2.7] under the tag "[random.access.iterators]".  Include bsl_iterator.h
// to use this component.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Defining a Standard Compliant Random Access Iterator
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to create a standard compliant random access iterator for a
// container.
//
// First, we define an iterator, 'MyArrayIterator', that meets the requirements
// of the 'IMP_ITER' template parameter of 'RandomAccessIterator' class (see
// class level documentation), but does not meet the full set of requirements
// for a random access iterator as defined by the C++ standard.  Note that the
// following shows only the public interface required.  Private members and
// additional methods that may be needed to implement this class are elided in
// this example:
//..
//  template <class VALUE>
//  class MyArrayIterator {
//      // This class implements the minimal requirements to implement a random
//      // access iterator using 'bslstl::RandomAccessIterator'.
//
//    public:
//      // CREATORS
//      MyArrayIterator();
//          // Create a 'MyArrayIterator' object that does not refer to any
//          // value.
//
//      MyArrayIterator(const MyArrayIterator& original);
//          // Create a 'MyArrayIterator' object having the same value
//          // as the specified 'original' object.
//
//      ~MyArrayIterator();
//          // Destroy this object;
//
//      // MANIPULATORS
//      MyArrayIterator& operator=(const MyArrayIterator& rhs);
//          // Assign to this object the value of the specified 'rhs' object,
//          // and return a reference providing modifiable access to this
//          // object.
//
//      void operator++();
//          // Increment this object to refer to the next element in an array.
//
//      void operator--();
//          // Decrement this object to refer to the previous element in an
//          // array.
//
//      void operator+=(std::ptrdiff_t n);
//          // Move this object by the specified 'n' element in the array.
//
//      // ACCESSORS
//      VALUE& operator*() const;
//          // Return a reference providing modifiable access to the value (of
//          // the parameterized 'VALUE' type) of the element referred to by
//          // this object.
//  };
//
//  template <class VALUE>
//  bool operator==(const MyArrayIterator<VALUE>&,
//                  const MyArrayIterator<VALUE>&);
//  template <class VALUE>
//  bool operator<(const MyArrayIterator<VALUE>&,
//                 const MyArrayIterator<VALUE>&);
//  template <class VALUE>
//  std::ptrdiff_t operator-(const MyArrayIterator<VALUE>&,
//                           const MyArrayIterator<VALUE>&);
//..
// Notice that 'MyArrayIterator' does not implement a complete standard
// compliant random access iterator.  It is missing methods such as 'operator+'
// and 'operator[]'.
//
// Then, we define the interface for our container class template,
// 'MyFixedSizeArray'.  The implementation of the interface is elided for
// brevity:
//..
//  template <class VALUE, int SIZE>
//  class MyFixedSizeArray {
//      // This class implements a container that contains the parameterized
//      // 'SIZE' number of elements of the parameterized 'VALUE' type.
//
//      // DATA
//      VALUE d_array[SIZE];   // storage of the container
//
//    public:
//      // PUBLIC TYPES
//      typedef VALUE value_type;
//..
// Now, we use 'RandomAccessIterator' to create a standard compliant iterator
// for this container:
//..
//      typedef bslstl::RandomAccessIterator<VALUE,
//                                           MyArrayIterator<VALUE> > iterator;
//      typedef bslstl::RandomAccessIterator<const VALUE,
//                                           MyArrayIterator<VALUE> >
//                                                              const_iterator;
//..
// Notice that the implementation for 'const_iterator' is
// 'MyArrayIterator<VALUE>' and *not* 'MyArrayIterator<const VALUE>'.
//
// Next, we continue defining the rest of the class.
//..
//      // CREATORS
//      //! MyFixedSizeArray() = default;
//          // Create a 'MyFixedSizeArray' object having the parameterized
//          // 'SIZE' number of elements of the parameterized type 'VALUE'.
//
//      //! MyFixedSizeArray(const MyFixedSizeArray& original) = default;
//          // Create a 'MyFixedSizeArray' object having same number of
//          // elements as that of the specified 'original', the same value of
//          // each element as that of corresponding element in 'original'.
//
//      //! ~MyFixedSizeArray() = default;
//          // Destroy this object.
//
//      // MANIPULATORS
//      iterator begin();
//          // Return a random access iterator providing modifiable access to
//          // the first valid element of this object.
//
//      iterator end();
//          // Return a random access iterator providing modifiable access to
//          // the last valid element of this object.
//
//      VALUE& operator[](int position);
//          // Return a reference providing modifiable access to the element at
//          // the specified 'position'.
//
//      // ACCESSORS
//      const_iterator begin() const;
//          // Return a random access iterator providing non-modifiable access
//          // to the first valid element of this object.
//
//      const_iterator end() const;
//          // Return a random access iterator providing non-modifiable access
//          // to the last valid element of this object.
//
//      const VALUE& operator[](int position) const;
//          // Return a reference providing non-modifiable access to the
//          // specified 'i'th element in this object.
//  };
//..
// Then, we create a 'MyFixedSizeArray' and initialize its elements:
//..
//  MyFixedSizeArray<int, 5> fixedArray;
//  fixedArray[0] = 3;
//  fixedArray[1] = 2;
//  fixedArray[2] = 5;
//  fixedArray[3] = 4;
//  fixedArray[4] = 1;
//..
// Finally, to show that 'MyFixedSizeArray::iterator' can be used as a random
// access iterator, we invoke a function that takes random iterators as
// parameters, such as 'std::sort', on the 'begin' and 'end' iterators
// and verify the results:
//..
//  std::sort(fixedArray.begin(), fixedArray.end());
//
//  assert(fixedArray[0] == 1);
//  assert(fixedArray[1] == 2);
//  assert(fixedArray[2] == 3);
//  assert(fixedArray[3] == 4);
//  assert(fixedArray[4] == 5);
//..


// Prevent 'bslstl' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
#error "<bslstl_randomaccessiterator.h> header can't be included directly in \
BSL_OVERRIDES_STD mode"
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLSTL_BIDIRECTIONALITERATOR
#include <bslstl_bidirectionaliterator.h>
#endif

#ifndef INCLUDED_BSLSTL_ITERATOR
#include <bslstl_iterator.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECVQ
#include <bslmf_removecvq.h>
#endif

#ifndef INCLUDED_ITERATOR
#include <iterator>
#define INCLUDED_ITERATOR
#endif

#ifndef INCLUDED_CSTDDEF
#include <cstddef>  // 'ptrdiff_t'
#define INCLUDED_CSTDDEF
#endif

namespace BloombergLP {

namespace bslstl {

                        //===========================
                        // class RandomAccessIterator
                        //===========================

template <class T, class ITER_IMP, class TAG_TYPE =
                                               std::random_access_iterator_tag>
class RandomAccessIterator
    : public BidirectionalIterator<T,ITER_IMP, TAG_TYPE> {
    // Given an 'ITER_IMP' type that implements a minimal subset of an iterator
    // interface, this template generates a complete iterator that meets all of
    // the requirements of a "random-access iterator" in the C++ standard.  If
    // 'T' is const-qualified, then the resulting type is a const iterator.
    // 'T' shall not be a function or reference type.  'ITER_IMP' must provide
    // public operations so that, for objects 'i' and 'j' of type 'ITER_IMP'
    // and 'n' of an integral type, the following operations are supported:
    //..
    //     ITER_IMP i;                          default construction
    //     ITER_IMP j(i);                       copy construction
    //     i = j                                assignment
    //     ++i                                  increment to next element
    //     --i                                  decrement to previous element
    //     i += n                               increment by n elements
    //     j - i   // convertible to ptrdiff_t  distance from i to j
    //     i == j  // convertible to bool       equality comparison
    //     i < j   // convertible to bool       less-than comparison
    //     *i      // convertible to T&         element access (dereference)
    //..

    // PRIVATE TYPES
    typedef typename bsl::remove_cv<T>::type UnCvqT;   // value type without
                                                       // 'const' and
                                                       // 'volatile'
                                                       // qualifications
  public:
    // TYPES
    typedef T                                value_type;
    typedef std::ptrdiff_t                   difference_type;
    typedef T                               *pointer;
    typedef T&                               reference;
    typedef std::random_access_iterator_tag  iterator_category;

    // CREATORS
    RandomAccessIterator();
        // Construct the default value for this iterator type.  All default-
        // constructed 'RandomAccessIterator' objects represent
        // non-dereferenceable iterators into the same empty range.  They do
        // not have a singular value unless an object of the type specified by
        // the template parameter 'ITER_IMP' has a singular value after
        // value-initialization.

    RandomAccessIterator(const ITER_IMP& implementation);           // IMPLICIT
        // Construct a random access iterator having the specified
        // 'implementation' of the parameterized 'ITER_IMP' type.

    //! RandomAccessIterator(const RandomAccessIterator& original);
        // Create a 'RandomAccessIterator' having the same value as the
        // specified 'original' iterator.  Note that this method's definition
        // is compiler generated.

    RandomAccessIterator(
                  const RandomAccessIterator<UnCvqT,ITER_IMP,TAG_TYPE>& other);
        // Construct a random access iterator from another (compatible)
        // 'RandomAccessIterator' type, e.g., a mutable iterator of the same
        // type.  Note that since this is a template, it has a lower priority
        // in overload resolution than the other constructors.

    //! ~RandomAccessIterator();
        // Destroy this iterator.  Note that this method's definition is
        // compiler generated.

    // MANIPULATORS
    //! RandomAccessIterator& operator=(const RandomAccessIterator& rhs);
        // Copy the value of the specified 'rhs' to this iterator.  Return a
        // reference to this modifiable iterator.  Note that this method's
        // definition is compiler generated.

    RandomAccessIterator& operator++();
        // Increment to the next element.  Return a reference to this
        // modifiable iterator.  The behavior is undefined if, on entry, this
        // iterator has the past-the-end value for an iterator over the
        // underlying sequence.

    RandomAccessIterator& operator--();
        // Decrement to the previous element.  Return a reference to this
        // modifiable iterator.  The behavior is undefined if, on entry, this
        // iterator has the same value as an iterator to the start of the
        // underlying sequence.

    RandomAccessIterator& operator+=(difference_type offset);
        // Increment by the specified 'offset' number of elements.  Return a
        // reference to this modifiable iterator.  The behavior is undefined
        // unless the iterator, after incrementing by 'offset', is within the
        // bounds of the underlying sequence.

    RandomAccessIterator& operator-=(difference_type offset);
        // Decrement by the specified 'offset' number of elements.  Return a
        // reference to this modifiable iterator.  The behavior is undefined
        // unless the iterator, after decrementing by 'offset', is within the
        // bounds of the underlying sequence.

    // ACCESSORS
    T& operator[](difference_type index) const;
        // Return a reference to the element at the specified 'index' positions
        // past the current one.  The behavior is undefined unless the
        // referenced position lies within the underlying sequence.  Note that
        // 'index' may be negative.
};

// FREE OPERATORS
template <class T1, class T2, class ITER_IMP, class TAG_TYPE>
bool operator==(const RandomAccessIterator<T1,ITER_IMP,TAG_TYPE>& lhs,
                const RandomAccessIterator<T2,ITER_IMP,TAG_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' iterator has the same value as the
    // specified 'rhs' iterator, and 'false' otherwise.  Two iterators have the
    // same value if they refer to the same element, or both have the past-the-
    // end value for the underlying sequence.  The behavior is undefined unless
    // both iterators refer to the same underlying sequence.

template <class T1, class T2, class ITER_IMP, class TAG_TYPE>
bool operator!=(const RandomAccessIterator<T1,ITER_IMP,TAG_TYPE>& lhs,
                const RandomAccessIterator<T2,ITER_IMP,TAG_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' iterator does not have the same
    // value as the specified 'rhs' iterator, and 'false' otherwise.  Two
    // iterators do not have the same value if (1) they do not refer to the
    // same element and (2) both do not have the past-the-end iterator value
    // for the underlying sequence.  The behavior is undefined unless both
    // iterators refer to the same underlying sequence.

template <class T, class ITER_IMP, class TAG_TYPE>
RandomAccessIterator<T,ITER_IMP,TAG_TYPE>
operator++(RandomAccessIterator<T,ITER_IMP,TAG_TYPE>& iter, int);
    // Increment the specified 'iter' to next element.  Return the previous
    // value of 'iter'.  The behavior is undefined if, on entry, 'iter' has the
    // past-the-end value for an iterator of the underlying sequence.

template <class T, class ITER_IMP, class TAG_TYPE>
RandomAccessIterator<T,ITER_IMP,TAG_TYPE>
operator--(RandomAccessIterator<T,ITER_IMP,TAG_TYPE>& iter, int);
    // Decrement the specified 'iter' to previous element.  Return the previous
    // value of 'iter'.  The behavior is undefined if, on entry, 'iter' has the
    // same value as an iterator to the start of the underlying sequence.

template <class T, class ITER_IMP, class TAG_TYPE>
RandomAccessIterator<T,ITER_IMP,TAG_TYPE>
operator+(const RandomAccessIterator<T,ITER_IMP,TAG_TYPE>& lhs,
          std::ptrdiff_t                                   rhs);
    // Return an iterator to the element at the specified 'rhs' positions past
    // the specified 'lhs'.  The behavior is undefined unless 'lhs', after
    // incrementing by 'rhs', is within the bounds of the underlying sequence.

template <class T, class ITER_IMP, class TAG_TYPE>
RandomAccessIterator<T,ITER_IMP,TAG_TYPE>
operator+(std::ptrdiff_t                                   lhs,
          const RandomAccessIterator<T,ITER_IMP,TAG_TYPE>& rhs);
    // Return an iterator to the element at the specified 'lhs' positions past
    // the specified 'rhs'.  The behavior is undefined unless 'rhs', after
    // incrementing by 'lhs', is within the bounds of the underlying sequence.

template <class T, class ITER_IMP, class TAG_TYPE>
RandomAccessIterator<T,ITER_IMP,TAG_TYPE>
operator-(const RandomAccessIterator<T,ITER_IMP,TAG_TYPE>& lhs,
          std::ptrdiff_t                                   rhs);
    // Return an iterator to the element at the specified 'rhs' positions
    // before the specified 'lhs'.  The behavior is undefined unless 'lhs',
    // after decrementing by 'rhs', is within the bounds of the underlying
    // sequence.  Note that this function is logically equivalent to:
    //..
    //  iter + (-rhs)
    //..

template <class T1, class T2, class ITER_IMP, class TAG_TYPE>
std::ptrdiff_t
operator-(const RandomAccessIterator<T1,ITER_IMP,TAG_TYPE>& lhs,
          const RandomAccessIterator<T2,ITER_IMP,TAG_TYPE>& rhs);
    // Return the distance from the specified 'rhs' iterator to the specified
    // 'lhs' iterator.  The behavior is undefined unless 'lhs' and 'rhs' are
    // iterators into the same underlying sequence.  Note that the result might
    // be negative.

template <class T1, class T2, class ITER_IMP, class TAG_TYPE>
bool operator<(const RandomAccessIterator<T1,ITER_IMP,TAG_TYPE>& lhs,
               const RandomAccessIterator<T2,ITER_IMP,TAG_TYPE>& rhs);
    // Return 'true' if (1) the specified 'lhs' iterator refers to an element
    // before the specified 'rhs' iterator in the iteration sequence, or (2)
    // 'rhs' (and not 'lhs') has the past-the-end value for an iterator over
    // this sequence, and 'false' otherwise.  The behavior is undefined unless
    // 'lhs' and 'rhs' are iterators into the same underlying sequence.

template <class T1, class T2, class ITER_IMP, class TAG_TYPE>
bool operator>(const RandomAccessIterator<T1,ITER_IMP,TAG_TYPE>& lhs,
               const RandomAccessIterator<T2,ITER_IMP,TAG_TYPE>& rhs);
    // Return 'true' if (1) the specified 'lhs' iterator refers to an element
    // after the specified 'rhs' iterator in the iteration sequence, or (2)
    // 'lhs' (and not 'rhs') has the past-the-end value for an iterator over
    // this sequence, and 'false' otherwise.  The behavior is undefined unless
    // 'lhs' and 'rhs' are iterators into the same underlying sequence.

template <class T1, class T2, class ITER_IMP, class TAG_TYPE>
bool operator<=(const RandomAccessIterator<T1,ITER_IMP,TAG_TYPE>& lhs,
                const RandomAccessIterator<T2,ITER_IMP,TAG_TYPE>& rhs);
    // Return 'true' if (1) the specified 'lhs' iterator has the same value as
    // the specified 'rhs' iterator, or (2) 'lhs' refers to an element before
    // 'rhs' in the iteration sequence, or (3) 'rhs' has the past-the-end value
    // for an iterator over this sequence, and 'false' otherwise.  The behavior
    // is undefined unless 'lhs' and 'rhs' are iterators into the same
    // underlying sequence.

template <class T1, class T2, class ITER_IMP, class TAG_TYPE>
bool operator>=(const RandomAccessIterator<T1,ITER_IMP,TAG_TYPE>& lhs,
                const RandomAccessIterator<T2,ITER_IMP,TAG_TYPE>& rhs);
    // Return 'true' if (1) the specified 'lhs' iterator has the same value as
    // the specified 'rhs' iterator, or (2) 'lhs' has the past-the-end value
    // for an iterator over this sequence, or (3) 'lhs' refers to an element
    // after 'rhs' in the iteration sequence, and 'false' otherwise.  The
    // behavior is undefined unless 'lhs' and 'rhs' are iterators into the same
    // underlying sequence.

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                    //----------------------------------
                    // class RandomAccessIterator
                    //----------------------------------

// CREATORS
template <class T, class ITER_IMP, class TAG_TYPE>
inline
RandomAccessIterator<T,ITER_IMP,TAG_TYPE>::RandomAccessIterator()
{
}

template <class T, class ITER_IMP, class TAG_TYPE>
inline
RandomAccessIterator<T,ITER_IMP,TAG_TYPE>::
RandomAccessIterator(const ITER_IMP& implementation)
: BidirectionalIterator<T,ITER_IMP,TAG_TYPE>(implementation)
{
}

template <class T, class ITER_IMP, class TAG_TYPE>
inline
RandomAccessIterator<T,ITER_IMP,TAG_TYPE>::
RandomAccessIterator(
                   const RandomAccessIterator<UnCvqT,ITER_IMP,TAG_TYPE>& other)
: BidirectionalIterator<T,ITER_IMP,TAG_TYPE>(other.imp())
{
}

// MANIPULATORS
template <class T, class ITER_IMP, class TAG_TYPE>
inline
RandomAccessIterator<T,ITER_IMP,TAG_TYPE>&
RandomAccessIterator<T,ITER_IMP,TAG_TYPE>::operator++()
{
    ++this->imp();
    return *this;
}

template <class T, class ITER_IMP, class TAG_TYPE>
inline
RandomAccessIterator<T,ITER_IMP,TAG_TYPE>&
RandomAccessIterator<T,ITER_IMP,TAG_TYPE>::operator--()
{
    --this->imp();
    return *this;
}

template <class T, class ITER_IMP, class TAG_TYPE>
inline
RandomAccessIterator<T,ITER_IMP,TAG_TYPE>&
RandomAccessIterator<T,ITER_IMP,TAG_TYPE>::operator+=(difference_type offset)
{
    this->imp() += offset;
    return *this;
}

template <class T, class ITER_IMP, class TAG_TYPE>
inline
RandomAccessIterator<T,ITER_IMP,TAG_TYPE>&
RandomAccessIterator<T,ITER_IMP,TAG_TYPE>::operator-=(difference_type offset)
{
    this->imp() += -offset;
    return *this;
}

// ACCESSORS
template <class T, class ITER_IMP, class TAG_TYPE>
inline
T& RandomAccessIterator<T,ITER_IMP,TAG_TYPE>::operator[](
                                                   difference_type index) const
{
    RandomAccessIterator<T,ITER_IMP,TAG_TYPE> tmp(*this);
    tmp += index;
    return *tmp;
}

}  // close package namespace

// FREE OPERATORS
template <class T1, class T2, class ITER_IMP, class TAG_TYPE>
inline
bool bslstl::operator==(const RandomAccessIterator<T1,ITER_IMP,TAG_TYPE>& lhs,
                        const RandomAccessIterator<T2,ITER_IMP,TAG_TYPE>& rhs)
{
    return lhs.imp() == rhs.imp();
}

template <class T1, class T2, class ITER_IMP, class TAG_TYPE>
inline
bool bslstl::operator!=(const RandomAccessIterator<T1,ITER_IMP,TAG_TYPE>& lhs,
                        const RandomAccessIterator<T2,ITER_IMP,TAG_TYPE>& rhs)
{
    return !(lhs == rhs);
}

template <class T, class ITER_IMP, class TAG_TYPE>
inline
bslstl::RandomAccessIterator<T,ITER_IMP,TAG_TYPE>
bslstl::operator++(RandomAccessIterator<T,ITER_IMP,TAG_TYPE>& iter, int)
{
    RandomAccessIterator<T,ITER_IMP,TAG_TYPE> tmp(iter);
    ++iter;
    return tmp;
}

template <class T, class ITER_IMP, class TAG_TYPE>
inline
bslstl::RandomAccessIterator<T,ITER_IMP,TAG_TYPE>
bslstl::operator--(RandomAccessIterator<T,ITER_IMP,TAG_TYPE>& iter, int)
{
    RandomAccessIterator<T,ITER_IMP,TAG_TYPE> tmp(iter);
    --iter;
    return tmp;
}

template <class T, class ITER_IMP, class TAG_TYPE>
inline
bslstl::RandomAccessIterator<T,ITER_IMP,TAG_TYPE>
bslstl::operator+(const RandomAccessIterator<T,ITER_IMP,TAG_TYPE>& lhs,
                  std::ptrdiff_t                                   rhs)
{
    RandomAccessIterator<T,ITER_IMP,TAG_TYPE> result(lhs);
    result += rhs;
    return result;
}

template <class T, class ITER_IMP, class TAG_TYPE>
inline
bslstl::RandomAccessIterator<T,ITER_IMP,TAG_TYPE>
bslstl::operator+(std::ptrdiff_t                                   lhs,
                  const RandomAccessIterator<T,ITER_IMP,TAG_TYPE>& rhs)
{
    return rhs + lhs;
}

template <class T, class ITER_IMP, class TAG_TYPE>
inline
bslstl::RandomAccessIterator<T,ITER_IMP,TAG_TYPE>
bslstl::operator-(const RandomAccessIterator<T,ITER_IMP,TAG_TYPE>& lhs,
                  std::ptrdiff_t                                   rhs)
{
    return lhs + -rhs;
}

template <class T1, class T2, class ITER_IMP, class TAG_TYPE>
inline
std::ptrdiff_t bslstl::operator-(
                         const RandomAccessIterator<T1,ITER_IMP,TAG_TYPE>& lhs,
                         const RandomAccessIterator<T2,ITER_IMP,TAG_TYPE>& rhs)
{
    return lhs.imp() - rhs.imp();
}

template <class T1, class T2, class ITER_IMP, class TAG_TYPE>
inline
bool bslstl::operator<(const RandomAccessIterator<T1,ITER_IMP,TAG_TYPE>& lhs,
                       const RandomAccessIterator<T2,ITER_IMP,TAG_TYPE>& rhs)
{
    return lhs.imp() < rhs.imp();
}

template <class T1, class T2, class ITER_IMP, class TAG_TYPE>
inline
bool bslstl::operator>(const RandomAccessIterator<T1,ITER_IMP,TAG_TYPE>& lhs,
                       const RandomAccessIterator<T2,ITER_IMP,TAG_TYPE>& rhs)
{
    return rhs < lhs;
}

template <class T1, class T2, class ITER_IMP, class TAG_TYPE>
inline
bool bslstl::operator<=(const RandomAccessIterator<T1,ITER_IMP,TAG_TYPE>& lhs,
                        const RandomAccessIterator<T2,ITER_IMP,TAG_TYPE>& rhs)
{
    return !(rhs < lhs);
}

template <class T1, class T2, class ITER_IMP, class TAG_TYPE>
inline
bool bslstl::operator>=(const RandomAccessIterator<T1,ITER_IMP,TAG_TYPE>& lhs,
                        const RandomAccessIterator<T2,ITER_IMP,TAG_TYPE>& rhs)
{
    return !(lhs < rhs);
}


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
