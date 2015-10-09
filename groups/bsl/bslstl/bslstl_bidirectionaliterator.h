// bslstl_bidirectionaliterator.h                                     -*-C++-*-
#ifndef INCLUDED_BSLSTL_BIDIRECTIONALITERATOR
#define INCLUDED_BSLSTL_BIDIRECTIONALITERATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a template to create STL-compliant bidirectional iterators.
//
//@CLASSES:
//  bslstl::BidirectionalIterator: bidirectional iterator template
//
//@SEE_ALSO: bslstl_iterator, bslstl_forwarditerator,
//           bslstl_randomaccessiterator
//
//@DESCRIPTION: This component provides an iterator adaptor that, given an
// implementation class defining a core set of iterator functionality specified
// in the class level documentation, adapts it to provide an STL-compliant
// bidirectional iterator interface.  'bslstl::BidirectionalIterator' meets the
// requirements of a bidirectional iterator described in the C++11 standard
// [24.2.7] under the tag "[bidirectional.iterators]".  Include bsl_iterator.h
// to use this component.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Defining a Standard Compliant Bidirectional Iterator
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to create a standard compliant bidirectional access iterator
// for a container.
//
// First, we define an iterator, 'MyArrayIterator', that meets the requirements
// of the 'IMP_ITER' template parameter of 'BidirectionalIterator' class (see
// class level documentation), but does not meet the full set of requirements
// for a bidirectional iterator as defined by the C++ standard.  Note that the
// following shows only the public interface required.  Private members and
// additional methods that may be needed to implement this class are elided in
// this example:
//..
//  template <class VALUE>
//  class MyArrayIterator {
//      // This class implements the minimal requirements to implement a
//      // bidirectional iterator using 'bslstl::BidirectionalIterator'.
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
//..
// Notice that 'MyArrayIterator' does not implement a complete standard
// compliant bidirectional iterator.  It is missing methods such as 'operator+'
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
// Now, we use 'BidirectionalIterator' to create a standard compliant iterator
// for this container:
//..
//      typedef bslstl::BidirectionalIterator<VALUE,
//                                           MyArrayIterator<VALUE> > iterator;
//      typedef bslstl::BidirectionalIterator<const VALUE,
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
//          // Return a bidirectional iterator providing modifiable access to
//          // the first valid element of this object.
//
//      iterator end();
//          // Return a bidirectional iterator providing modifiable access to
//          // the last valid element of this object.
//
//      VALUE& operator[](int position);
//          // Return a reference providing modifiable access to the element at
//          // the specified 'position'.
//
//      // ACCESSORS
//      const_iterator begin() const;
//          // Return a bidirectional iterator providing non-modifiable access
//          // to the first valid element of this object.
//
//      const_iterator end() const;
//          // Return a bidirectional iterator providing non-modifiable access
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
//  fixedArray[0] = 1;
//  fixedArray[1] = 2;
//  fixedArray[2] = 3;
//  fixedArray[3] = 4;
//  fixedArray[4] = 5;
//..
// Finally, to show that 'MyFixedSizeArray::iterator' can be used as a
// bidirectional iterator, we invoke a function that takes bidirectional
// iterators as parameters, such as 'std::reverse', on the 'begin' and 'end'
// iterators and verify the results:
//..
//  std::reverse(fixedArray.begin(), fixedArray.end());
//
//  assert(fixedArray[0] == 5);
//  assert(fixedArray[1] == 4);
//  assert(fixedArray[2] == 3);
//  assert(fixedArray[3] == 2);
//  assert(fixedArray[4] == 1);
//..

// Prevent 'bslstl' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
#error "<bslstl_bidirectionaliterator.h> header can't be included directly in \
in BSL_OVERRIDES_STD mode"
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLSTL_FORWARDITERATOR
#include <bslstl_forwarditerator.h>
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

                        //============================
                        // class BidirectionalIterator
                        //============================

template <class T, class ITER_IMP, class TAG_TYPE =
                                               std::bidirectional_iterator_tag>
class BidirectionalIterator
    : public ForwardIterator<T, ITER_IMP, TAG_TYPE> {
    // Given an 'ITER_IMP' type that implements a minimal subset of an iterator
    // interface, this template generates a complete iterator that meets all of
    // the requirements of a "bidirectional iterator" in the C++ standard.  If
    // 'T' is 'const'-qualified, then the resulting type is a constant
    // iterator.  'T' shall not be a function or reference type.  'ITER_IMP'
    // must provide public operations so that, for objects 'i' and 'j' of type
    // 'ITER_IMP', the following operations are supported:
    //..
    //     ITER_IMP i;                          default construction
    //     ITER_IMP j(i);                       copy construction
    //     i = j                                assignment
    //     ++i                                  increment to next element
    //     --i                                  decrement to previous element
    //     i == j     // convertible to bool    equality comparison
    //     *i         // convertible to T&      element access (dereference)
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
    typedef std::bidirectional_iterator_tag  iterator_category;

    // CREATORS
    BidirectionalIterator();
        // Construct the default value for this iterator type.  All default-
        // constructed 'BidirectionalIterator' objects represent
        // non-dereferenceable iterators into the same empty range.  They do
        // not have a singular value unless an object of the type specified by
        // the template parameter 'ITER_IMP' has a singular value after
        // value-initialization.

    BidirectionalIterator(const ITER_IMP& implementation);          // IMPLICIT
        // Construct a bidirectional iterator having the specified
        // 'implementation' of the parameterized 'ITER_IMP' type.

    //! BidirectionalIterator(const BidirectionalIterator& original);
        // Construct a bidirectional iterator having the same value as the
        // 'original' iterator.  Note that this method's definition is compiler
        // generated.

    BidirectionalIterator(
                 const BidirectionalIterator<UnCvqT,ITER_IMP,TAG_TYPE>& other);
        // Construct a bidirectional iterator from another (compatible)
        // 'BidirectionalIterator' type, e.g., a mutable iterator of the same
        // type.  Note that as this is a template, it has a lower priority in
        // overload resolution than the other constructors.

    //! ~BidirectionalIterator();
        // Destroy this iterator.  Note that this method's definition is
        // compiler generated.

    // MANIPULATORS
    //! BidirectionalIterator& operator=(const BidirectionalIterator& rhs);
        // Copy the value of the specified 'rhs' to this iterator.  Return a
        // reference to this modifiable object.  Note that this method's
        // definition is compiler generated.

    BidirectionalIterator& operator++();
        // Increment to the next element.  Return a reference to this
        // modifiable iterator.  The behavior is undefined if, on entry, this
        // iterator has the past-the-end value for an iterator over the
        // underlying sequence.

    BidirectionalIterator& operator--();
        // Decrement to the previous element.  Return a reference to this
        // modifiable iterator.  The behavior is undefined if, on entry, this
        // iterator has the same value as an iterator the refers to the start
        // of the underlying sequence.
};

// FREE OPERATORS
template <class T1, class T2, class ITER_IMP, class TAG_TYPE>
bool operator==(const BidirectionalIterator<T1,ITER_IMP,TAG_TYPE>& lhs,
                const BidirectionalIterator<T2,ITER_IMP,TAG_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' iterator has the same value as the
    // specified 'rhs' iterator, and 'false' otherwise.  Two iterators have the
    // same value if they refer to the same element, or both have the past-the-
    // end value for the underlying sequence.  The behavior is undefined unless
    // both iterators refer to the same underlying sequence.

template <class T1, class T2, class ITER_IMP, class TAG_TYPE>
bool operator!=(const BidirectionalIterator<T1,ITER_IMP,TAG_TYPE>& lhs,
                const BidirectionalIterator<T2,ITER_IMP,TAG_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' iterator does not have the same
    // value as the specified 'rhs' iterator, and 'false' otherwise.  Two
    // iterators do not have the same value if (1) they do not refer to the
    // same element and (2) both do not have the past-the-end iterator value
    // for the underlying sequence.  The behavior is undefined unless both
    // iterators refer to the same underlying sequence.

template <class T, class ITER_IMP, class TAG_TYPE>
BidirectionalIterator<T,ITER_IMP,TAG_TYPE>
operator++(BidirectionalIterator<T,ITER_IMP,TAG_TYPE>& iter, int);
    // Increment the specified 'iter' to the next element.  Return the previous
    // value of 'iter'.  The behavior is undefined if, on entry, 'iter' has the
    // past-the-end value for an iterator of the underlying sequence.

template <class T, class ITER_IMP, class TAG_TYPE>
BidirectionalIterator<T,ITER_IMP,TAG_TYPE>
operator--(BidirectionalIterator<T,ITER_IMP,TAG_TYPE>& iter, int);
    // Decrement the specified 'iter' to the previous element.  Return the
    // previous value of 'iter'.  The behavior is undefined if, on entry,
    // 'iter' has the same value as an iterator to the start of the underlying
    // sequence.

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                //-----------------------------------
                // class BidirectionalIterator
                //-----------------------------------

// CREATORS
template <class T, class ITER_IMP, class TAG_TYPE>
inline
BidirectionalIterator<T,ITER_IMP,TAG_TYPE>::BidirectionalIterator()
: ForwardIterator<T,ITER_IMP,TAG_TYPE>()
{
}

template <class T, class ITER_IMP, class TAG_TYPE>
inline
BidirectionalIterator<T,ITER_IMP,TAG_TYPE>::
BidirectionalIterator(const ITER_IMP& implementation)
: ForwardIterator<T,ITER_IMP,TAG_TYPE>(implementation)
{
}

template <class T, class ITER_IMP, class TAG_TYPE>
inline
BidirectionalIterator<T,ITER_IMP,TAG_TYPE>::BidirectionalIterator(
                  const BidirectionalIterator<UnCvqT,ITER_IMP,TAG_TYPE>& other)
: ForwardIterator<T,ITER_IMP,TAG_TYPE>(other.imp())
{
}

// MANIPULATORS
template <class T, class ITER_IMP, class TAG_TYPE>
inline
BidirectionalIterator<T,ITER_IMP,TAG_TYPE>&
BidirectionalIterator<T,ITER_IMP,TAG_TYPE>::operator++()
{
    ++this->imp();
    return *this;
}

template <class T, class ITER_IMP, class TAG_TYPE>
inline
BidirectionalIterator<T,ITER_IMP,TAG_TYPE>&
BidirectionalIterator<T,ITER_IMP,TAG_TYPE>::operator--()
{
    --this->imp();
    return *this;
}

}  // close package namespace

// FREE OPERATORS
template <class T1, class T2, class ITER_IMP, class TAG_TYPE>
inline
bool bslstl::operator==(const BidirectionalIterator<T1,ITER_IMP,TAG_TYPE>& lhs,
                        const BidirectionalIterator<T2,ITER_IMP,TAG_TYPE>& rhs)
{
    return lhs.imp() == rhs.imp();
}

template <class T1, class T2, class ITER_IMP, class TAG_TYPE>
inline
bool bslstl::operator!=(const BidirectionalIterator<T1,ITER_IMP,TAG_TYPE>& lhs,
                        const BidirectionalIterator<T2,ITER_IMP,TAG_TYPE>& rhs)
{
    return !(lhs == rhs);
}

template <class T, class ITER_IMP, class TAG_TYPE>
inline
bslstl::BidirectionalIterator<T,ITER_IMP,TAG_TYPE>
bslstl::operator++(BidirectionalIterator<T,ITER_IMP,TAG_TYPE>& iter, int)
{
    BidirectionalIterator<T,ITER_IMP,TAG_TYPE> tmp(iter);
    ++iter;
    return tmp;
}

template <class T, class ITER_IMP, class TAG_TYPE>
inline
bslstl::BidirectionalIterator<T,ITER_IMP,TAG_TYPE>
bslstl::operator--(BidirectionalIterator<T,ITER_IMP,TAG_TYPE>& iter, int)
{
    BidirectionalIterator<T,ITER_IMP,TAG_TYPE> tmp(iter);
    --iter;
    return tmp;
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
