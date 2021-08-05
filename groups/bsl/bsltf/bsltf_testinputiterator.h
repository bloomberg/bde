// bsltf_testinputiterator.h                                          -*-C++-*-
#ifndef INCLUDED_BSLTF_TESTINPUTITERATOR
#define INCLUDED_BSLTF_TESTINPUTITERATOR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a pure input iterator capable of traversing a range.
//
//@CLASSES:
//  bsltf::TestInputIterator: empty input iterator template
//
//@DESCRIPTION: This components provides a mechanism,
// 'bsltf::TestInputIterator', that defines an input iterator that supports the
// following operations:
//
//: o Obj& operator++()
//:
//: o Obj operator++(int)
//:
//: o pointer operator->() const
//:
//: o reference& operator*() const
//
// The iterator is initializable with either a pointer into a range, or a
// non-pointer iterator over a contiguous range.
//
// This iterator type is typically used to check algorithms for compatibility
// with input iterators.  The goal is to make sure that their code is able to
// compile and work even with the most restrictive input iterator.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Use of 'bsltf::TestInputIterator':
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// In the following example we use a 'bsltf::TestInputIterator' to test that an
// aggregation function compiles and works when instantiated with a pure input
// iterator.
//
// First, we define a function 'sum' that accepts two input iterators and
// returns the sum of all elements in range specified by them:
//..
//  template <class IN_ITER>
//  double sum(IN_ITER first, IN_ITER last)
//  {
//      double total = 0;
//      while (first != last) {
//          total += *first++;
//      }
//      return total;
//  }
//..
// Then, in 'main', we define an array of 'double's and define
// 'TestInputIterators' pointing to the beginning and ending of it,
// initializing the iterators with pointers:
//..
//  static double myArray[] = { 2.5, 3, 5, 7, 11.5, 5 };
//  enum { k_MY_ARRAY_LEN = sizeof myArray / sizeof *myArray };
//
//  typedef bsltf::TestInputIterator<double> IterType;
//
//  IterType begin(myArray + 0), end(myArray + k_MY_ARRAY_LEN);
//..
// Next, we call 'sum' with the two iterators, and observe that its yields the
// expected result, and because it compiles, we know that 'sum' did not attempt
// any operations on the iterators other than those defined for the most basic
// input iterator:
//..
//  const double x = sum(begin, end);
//  assert(34.0 == x);
//..
// Then, we make a vector containing the elements of 'myArray':
//..
//  const std::vector<double> v(myArray + 0, myArray + k_MY_ARRAY_LEN);
//..
// Next, we illustrate that we can create iterators by initializing them with
// vector iterators rather than pointers, even on STL implementations where
// 'vector::iterator' is not a simple pointer type:
//..
//  IterType vBegin(v.begin()), vEnd(v.end());
//..
// Now we call 'sum' on our new pair of iterators and observe that it, once
// again, gets the correct result:
//..
//  const double y = sum(vBegin, vEnd);
//  assert(34.0 == y);
//..

#include <bslscm_version.h>

#include <bsls_libraryfeatures.h>

#include <cstddef>
#include <iterator>

namespace BloombergLP {
namespace bsltf {

                          // =======================
                          // class TestInputIterator
                          // =======================

#if defined(BSLS_LIBRARYFEATURES_STDCPP_LIBCSTD)
// Sun CC workaround: iterators must be derived from 'std::iterator' to work
// with the native std library algorithms.  However, 'std::iterator' is
// deprecated in C++17, so do not rely on derivation unless required, to avoid
// deprecation warnings on modern compilers.

template <class TYPE>
class TestInputIterator : public std::iterator<std::input_iterator_tag,
                                               const TYPE,
                                               std::ptrdiff_t,
                                               const TYPE *,
                                               const TYPE> {
#else
template <class TYPE>
class TestInputIterator {
#endif
    // Provide an input iterator that iterates that can iterate over a
    // contiguous range of object while supporting no operations other than
    // those defined for 'LegacyInputIterator'.

  public:
    // PUBLIC TYPES
    typedef std::input_iterator_tag  iterator_category;
    typedef const TYPE               value_type;
    typedef std::ptrdiff_t           difference_type;
    typedef const TYPE              *pointer;
    typedef const TYPE&              reference;

  private:
    // DATA
    const TYPE *d_value_p;

    // FRIENDS
    template <class TYPE2>
    friend bool operator==(const TestInputIterator<TYPE2>&,
                           const TestInputIterator<TYPE2>&);
    template <class TYPE2>
    friend bool operator!=(const TestInputIterator<TYPE2>&,
                           const TestInputIterator<TYPE2>&);

  public:
    // CREATORS
    TestInputIterator();
        // Construct an empty input iterator.

    explicit
    TestInputIterator(TYPE *ptr);
        // Construct an input iterator based on the specified 'ptr'.

    template <class CONTIGUOUS_ITERATOR>
    explicit
    TestInputIterator(CONTIGUOUS_ITERATOR iter);
        // Construct an input iterator based on the specified iterator 'iter'.
        // The behavior is undefined unless 'CONTIGUOUS_ITERATOR' is an
        // iterator into a range of contiguous, adjacent objects of type
        // 'value_type' and is not a pointer type.

    // TestInputIterator(const TestInputIterator& original) = default;
        // Construct a copy of the specified 'original' object.

    // ~TestInputIterator() = default;
        // Destroy this object.

    // MANIPULATORS
    // TestInputIterator& operator=(const TestInputIterator& rhs) = default;
        // Assign to this object the value of the specified 'rhs' and return a
        // reference to this modifiable object.

    TestInputIterator& operator++();
        // Increment this iterator to refer to the next contiguous 'TYPE'
        // object, and return a reference to after the increment.

    TestInputIterator operator++(int);
        // Copy this iterator, increment, and return by value the copy that
        // was made prior to the increment.

    // ACCESSORS
    pointer operator->() const;
        // Return a pointer to the 'TYPE' object referred to by this iterator.

    reference operator*() const;
        // Return a const reference to the 'TYPE' object referred to by this
        // iterator.
};

// FREE OPERATORS
template <class TYPE>
inline
bool operator==(const TestInputIterator<TYPE>& lhs,
                const TestInputIterator<TYPE>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' refer to the same 'TYPE'
    // object and 'false' othersise.

template <class TYPE>
inline
bool operator!=(const TestInputIterator<TYPE>& lhs,
                const TestInputIterator<TYPE>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' do not refer to the same
    // 'TYPE' object and 'false' othersise.

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                          // -----------------------
                          // class TestInputIterator
                          // -----------------------

// CREATORS
template<class TYPE>
inline
TestInputIterator<TYPE>::TestInputIterator()
: d_value_p(0)
{
}

template<class TYPE>
inline
TestInputIterator<TYPE>::TestInputIterator(TYPE *ptr)
: d_value_p(ptr)
{
}

template<class TYPE>
template <class CONTIGUOUS_ITERATOR>
inline
TestInputIterator<TYPE>::TestInputIterator(CONTIGUOUS_ITERATOR iter)
: d_value_p(&*iter)
{
}

// MANIPULATORS
template<class TYPE>
inline
TestInputIterator<TYPE>& TestInputIterator<TYPE>::operator++()
{
    ++d_value_p;

    return *this;
}

template<class TYPE>
inline
TestInputIterator<TYPE> TestInputIterator<TYPE>::operator++(int)
{
    const TYPE *prev = d_value_p++;
    return TestInputIterator(prev);
}

// ACCESSORS
template<class TYPE>
inline
typename TestInputIterator<TYPE>::pointer
                                    TestInputIterator<TYPE>::operator->() const
{
    return d_value_p;
}

template<class TYPE>
inline
typename TestInputIterator<TYPE>::reference
                                     TestInputIterator<TYPE>::operator*() const
{
    return *d_value_p;
}

}  // close package namespace

// FREE OPERATORS
template <class TYPE>
inline
bool bsltf::operator==(const bsltf::TestInputIterator<TYPE>& lhs,
                       const bsltf::TestInputIterator<TYPE>& rhs)
{
    return lhs.d_value_p == rhs.d_value_p;
}

template <class TYPE>
inline
bool bsltf::operator!=(const bsltf::TestInputIterator<TYPE>& lhs,
                       const bsltf::TestInputIterator<TYPE>& rhs)
{
    return lhs.d_value_p != rhs.d_value_p;
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2021 Bloomberg Finance L.P.
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
