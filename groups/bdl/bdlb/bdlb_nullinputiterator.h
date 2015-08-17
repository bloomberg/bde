// bdlb_nullinputiterator.h                                           -*-C++-*-
#ifndef INCLUDED_BDLB_NULLINPUTITERATOR
#define INCLUDED_BDLB_NULLINPUTITERATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a pure input iterator for an empty range.
//
//@CLASSES:
//  bdlb::NullInputIterator: parameterized null input iterator
//
//@SEE_ALSO: bdlb_nulloutputiterator
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION: This components provides at templated iterator type,
// 'bdlb::NullInputIterator', with the following attributes:
//..
//  o Exactly meets the requirements for an input iterator according to the
//    C++ Standard (C++98, Section 24.1.1 [lib.input.iterators]).
//  o For a given type, 'T', all objects of type 'bdlb::NullInputIterator<T>'
//    compare equal.  Thus, any pair of such iterators constitute an
//    empty range.
//  o Dereferencing or incrementing the iterator is undefined behavior, since
//    every iterator is logically at the end of its valid range.
//..
// This iterator type is typically used to test that a function or template
// class compiles when instantiated with a pure input iterator.
//
///Usage
///-----
// The following test function is designed to traverse an input iterator range
// and sum the elements.
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
// The following program uses 'sum' to compute the sum of elements in an
// array.  Then it uses 'sum' again, this time instantiated with
// 'bdlb::NullInputIterator'.  The result is zero because
// 'bdlb::NullInputIterator' is always at the end.  The point is to prove that
// 'sum' compiles when instantiated with pure input iterators.
//..
//  int main()
//  {
//      static const int myArray[6] = { 2, 3, 5, 7, 11, 0 };
//
//      // Compute the sum using random-access iterators (pointers).
//      int r1 = sum(&myArray[0], &myArray[5]);
//      assert(28 == r1);
//
//      // Now test that it compiles using pure input iterators:
//      typedef bdlb::NullInputIterator<unsigned> iterType;
//      unsigned r2 = sum(iterType(), iterType());
//      assert(0 == r2);
//
//      return 0;
//  }
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSL_ITERATOR
#include <bsl_iterator.h>
#endif


namespace BloombergLP {

namespace bdlb {
                        // =============================
                        // class NullInputIterator
                        // =============================

template <class TYPE>
class NullInputIterator :
    public bsl::iterator<bsl::input_iterator_tag, TYPE> {
    // Provide an input iterator that iterates over an empty sequence.
    // All null iterators compare equal.  Since the iteration sequence is
    // empty, incrementing or dereferencing this iterator yields undefined
    // behavior.

  public:
    // CREATORS
    NullInputIterator();
        // Creates a null input iterator.

    NullInputIterator(const NullInputIterator& original);
        // Construct a copy of 'original'.

    ~NullInputIterator();
        // Destroy this object.

    // MANIPULATORS
    NullInputIterator& operator=(const NullInputIterator& rhs);
        // Assignment operator.  Does nothing and returns '*this'.

    NullInputIterator& operator++();
    NullInputIterator& operator++(int);
        // Increment the iterator.  Since the only value for this iterator is
        // the "end" value, incrementing it always produces undefined
        // behavior.  A loop traversing a pair of null input iterators will
        // execute zero times, but may still contain an (unused) calls to
        // these operators.  Thus, these operators can be instantiated at
        // compile-time, even though they must never be called at run-time.

    // ACCESSORS
    TYPE* operator->() const;
    TYPE operator*() const;
        // Dereference the iterator.  Since the only value for this iterator
        // is the "end" value, dereferencing it always produces undefined
        // behavior.  A loop traversing a pair of null input iterators will
        // execute zero times, but may still contain an (unused) calls to
        // these operators.  Thus, these operators can be instantiated at
        // compile-time, even though they must never be called at run-time.
};

// FREE OPERATORS
template <class TYPE>
inline
bool operator==(const NullInputIterator<TYPE>& lhs,
                const NullInputIterator<TYPE>& rhs);
    // Return 'true'.

template <class TYPE>
inline
bool operator!=(const NullInputIterator<TYPE>& lhs,
                const NullInputIterator<TYPE>& rhs);
    // Return 'false'.

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

// CREATORS
template<class TYPE>
inline
NullInputIterator<TYPE>::NullInputIterator()
{
}

template<class TYPE>
inline
NullInputIterator<TYPE>::NullInputIterator(
                                                const NullInputIterator&)
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
    return *this;
}

template<class TYPE>
inline
NullInputIterator<TYPE>& NullInputIterator<TYPE>::operator++(int)
{
    return *this;
}

// ACCESSORS
template<class TYPE>
inline
TYPE *NullInputIterator<TYPE>::operator->() const
{
    return 0;
}

template<class TYPE>
inline
TYPE NullInputIterator<TYPE>::operator*() const
{
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
