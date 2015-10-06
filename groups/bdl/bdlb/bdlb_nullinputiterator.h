// bdlb_nullinputiterator.h                                           -*-C++-*-
#ifndef INCLUDED_BDLB_NULLINPUTITERATOR
#define INCLUDED_BDLB_NULLINPUTITERATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

//@PURPOSE: Provide a pure input iterator for an empty range.
//
//@CLASSES:
//  bdlb::NullInputIterator: input iterator template that discards the input
//
//@SEE_ALSO: bdlb_nulloutputiterator
//
//@DESCRIPTION: This components provides a class template defining an input
// iterator type, 'bdlb::NullInputIterator', with the following attributes:
//
//: o For a given type, 'T', all objects of type 'NullInputIterator<T>' compare
//:   equal.  Thus, any pair of such iterators constitute an empty range.
//:
//: o Dereferencing or incrementing the iterator is undefined behavior, since
//:   every iterator is logically at the end of its valid range.
//:
//: o Exactly meets the requirements for an input iterator according to the C++
//:   Standard (C++98, Section 24.1.1 [lib.input.iterators]).
//
// This iterator type is typically used to test that a function or template
// class compiles when instantiated with a pure input iterator.
//
///Usage
///-----
// In the following example we use a 'bdlb::NullInputIterator' to test that
// function compiles when instantiated with a pure input iterator.
//
// First, we define a function 'sum' that accepts two input iterators and
// returns sum of all elements in range specified by them.
//..
//  template <class IN_ITER>
//  typename bsl::iterator_traits<IN_ITER>::value_type
//  sum(IN_ITER first, IN_ITER last)
//  {
//      typename bsl::iterator_traits<IN_ITER>::value_type total = 0;
//      while (first != last) {
//          total += *first++;
//      }
//      return total;
//  }
//..
// Now, we define a function 'testSum' that first verifies that 'sum' correctly
// accumulates a sum, and then verifies, using 'bdlb::NullInputIterator', that
// 'sum' can be instantiated on an iterator that strictly matches the
// requirements of a null input iterator:
//..
//  int testSum()
//  {
//      static const int myArray[6] = { 2, 3, 5, 7, 11, 0 };
//
//      // Verify that 'sum' correctly computes the sum using random access
//      // iterators (pointers).
//      int r1 = sum(&myArray[0], &myArray[5]);
//      assert(28 == r1);
//
//      // Verify that 'sum' can be instantiated using a pure input iterator.
//      typedef bdlb::NullInputIterator<unsigned> iterType;
//      unsigned r2 = sum(iterType(), iterType());
//      assert(0 == r2);
//
//      return 0;
//  }
//..

#ifndef INCLUDED_BSL_ITERATOR
#include <bsl_iterator.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP {
namespace bdlb {

                        // =======================
                        // class NullInputIterator
                        // =======================

template <class TYPE>
class NullInputIterator : public bsl::iterator<bsl::input_iterator_tag, TYPE> {
    // Provide an input iterator that iterates over an empty sequence.  All
    // null iterators compare equal.  Since the iteration sequence is empty,
    // incrementing or de-referencing this iterator yields undefined behavior.

  public:
    // CREATORS
    NullInputIterator();
        // Construct a null input iterator.

    NullInputIterator(const NullInputIterator& original);
        // Construct a copy of the specified 'original' object.

    ~NullInputIterator();
        // Destroy this object.

    // MANIPULATORS
    NullInputIterator& operator=(const NullInputIterator& rhs);
        // Assign to this object the value of the specified 'rhs' and return a
        // reference to this modifiable object.

    NullInputIterator& operator++();
        // The behavior is undefined for this method.  Note that this method
        // signature matches the requirements of an input iterator, but a
        // 'NullInputIterator' always represents the end position in a range.


    NullInputIterator& operator++(int);
        // The behavior is undefined for this method.  Note that this method
        // signature matches the requirements of an input iterator, but a
        // 'NullInputIterator' always represents the end position in a range.

    // ACCESSORS
    TYPE* operator->() const;
        // The behavior is undefined for this method.  Note that this method
        // signature matches the requirements of an input iterator, but a
        // 'NullInputIterator' always represents the end position in a range.

    TYPE operator*() const;
        // The behavior is undefined for this method.  Note that this method
        // signature matches the requirements of an input iterator, but a
        // 'NullInputIterator' always represents the end position in a range.
};

// FREE OPERATORS
template <class TYPE>
inline
bool operator==(const NullInputIterator<TYPE>& lhs,
                const NullInputIterator<TYPE>& rhs);
    // Return 'true' since the specified 'lhs' iterator always has the same
    // value as the specified 'rhs' iterator.

template <class TYPE>
inline
bool operator!=(const NullInputIterator<TYPE>& lhs,
                const NullInputIterator<TYPE>& rhs);
    // Return 'false' since the specified 'lhs' iterator always has the same
    // value as the specified 'rhs' iterator.

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

// CREATORS
template<class TYPE>
inline
NullInputIterator<TYPE>::NullInputIterator()
{
}

template<class TYPE>
inline
NullInputIterator<TYPE>::NullInputIterator(const NullInputIterator&)
{
}

template<class TYPE>
inline
NullInputIterator<TYPE>::~NullInputIterator()
{
}

// MANIPULATORS
template<class TYPE>
inline
NullInputIterator<TYPE>&
NullInputIterator<TYPE>::operator=(const NullInputIterator&)
{
    return *this;
}

template<class TYPE>
inline
NullInputIterator<TYPE>& NullInputIterator<TYPE>::operator++()
{
    BSLS_ASSERT(false);
    return *this;
}

template<class TYPE>
inline
NullInputIterator<TYPE>& NullInputIterator<TYPE>::operator++(int)
{
    BSLS_ASSERT(false);
    return *this;
}

// ACCESSORS
template<class TYPE>
inline
TYPE *NullInputIterator<TYPE>::operator->() const
{
    BSLS_ASSERT(false);
    return 0;
}

template<class TYPE>
inline
TYPE NullInputIterator<TYPE>::operator*() const
{
    BSLS_ASSERT(false);
    return *this->operator->();
}

}  // close package namespace

// FREE OPERATORS
template <class TYPE>
inline
bool bdlb::operator==(const NullInputIterator<TYPE>&,
                      const NullInputIterator<TYPE>&)
{
    return true;
}

template <class TYPE>
inline
bool bdlb::operator!=(const NullInputIterator<TYPE>&,
                      const NullInputIterator<TYPE>&)
{
    return false;
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
