// bdlb_testinputiterator.h                                           -*-C++-*-
#ifndef INCLUDED_BDLB_TESTINPUTITERATOR
#define INCLUDED_BDLB_TESTINPUTITERATOR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a pure input iterator for an empty range.
//
//@DEPRECATED: Use 'bsltf_testinputiterator' instead.
//
//@CLASSES:
//  bdlb::TestInputIterator: empty input iterator template
//
//@DESCRIPTION: This components provides a mechanism,
// 'bdlb::TestInputIterator', that defines an input iterator with the following
// attributes:
//..
//  o For a given type, 'T', all objects of type 'TestInputIterator<T>'
//    compare equal.  Thus, any pair of such iterators constitute an
//    empty range.
//  o Dereferencing or incrementing the iterator is undefined behavior, since
//    every iterator is logically at the end of its valid range.
//  o Exactly meets the requirements for an input iterator according to the
//    C++ Standard (C++98, Section 24.1.1 [lib.input.iterators]).
//..
// This iterator type is typically used to check algorithms for compatibility
// with input iterators.  The goal is to make sure that their code is able to
// work even with the most restrictive input iterator.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Use of 'bdlb::TestInputIterator'
///- - - - - - - - - - - - - - - - - - - - - - - - -
// In the following example we use a 'bdlb::TestInputIterator' to test that an
// aggregation function compiles when instantiated with a pure input iterator.
//
// First, we define a function 'sum' that accepts two input iterators and
// returns the sum of all elements in range specified by them.
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
// accumulates a sum, and then verifies, using 'bdlb::TestInputIterator', that
// 'sum' can be instantiated on an iterator that strictly matches the
// requirements of an empty input iterator:
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
//      typedef bdlb::TestInputIterator<unsigned> iterType;
//      unsigned r2 = sum(iterType(), iterType());
//      assert(0 == r2);
//
//      return 0;
//  }
//..

#include <bdlscm_version.h>

#include <bsltf_inputiterator.h>

#include <bsls_compilerfeatures.h>

#include <bsl_iterator.h>

namespace BloombergLP {
namespace bdlb {

                        // =======================
                        // class TestInputIterator
                        // =======================

#if 201103L <= BSLS_COMPILERFEATURES_CPLUSPLUS

template <class TYPE>
using TestInputIterator = bsltf::InputIterator<TYPE>;

#else

template <class TYPE>
struct TestInputIterator : public bsltf::InputIterator<TYPE> {
    // CREATORS
    TestInputIterator();

    TestInputIterator(const TestInputIterator& original);
};

                        // -----------------------
                        // class TestInputIterator
                        // -----------------------

// CREATORS
template <class TYPE>
inline
TestInputIterator<TYPE>::TestInputIterator()
: bsltf::InputIterator<TYPE>()
{}

template <class TYPE>
inline
TestInputIterator<TYPE>::TestInputIterator(const TestInputIterator& original)
: bsltf::InputIterator<TYPE>(original)
{}

#endif

}  // close package namespace
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
