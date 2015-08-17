// bdlb_nulloutputiterator.h                                          -*-C++-*-
#ifndef INCLUDED_BDLB_NULLOUTPUTITERATOR
#define INCLUDED_BDLB_NULLOUTPUTITERATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a bit-bucket for functions that take output iterators.
//
//@CLASSES:
//  bdlb::NullOutputIterator: null output iterator template
//
//@SEE_ALSO: bdlb_nullinputiterator
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION: This component provides a templated iterator type,
// 'bdlb::NullOutputIterator', with the following attributes:
//..
//  - Meets exactly the requirements for an output iterator according to the
//    C++ Standard (C++98, Section 24.1.2 [lib.output.iterators]).
//  - Dereferencing an iterator and assigning tot he result is a no-op.
//  - Incrementing an iterator is a no-op.
//..
// This iterator type is typically used to call functions purely for their
// side-effects, discarding the normal output.  It is also useful for testing
// whether a template function will compile when presented with a pure output
// iterator.
//
///Usage
///-----
// The following test function is designed to traverse an input iterator range
// and sum the elements.  A running total is written to the output iterator
// and the final sum is returned.
//..
//  template <class IN_ITER, class OUT_ITER>
//  typename bsl::iterator_traits<OUT_ITER>::value_type
//  runningSum(IN_ITER first, IN_ITER last, OUT_ITER output)
//  {
//      typename bsl::iterator_traits<OUT_ITER>::value_type total = 0;
//      while (first != last) {
//          total += *first++;
//          *output++ = total;
//      }
//      return total;
//  }
//..
// The following program uses 'bdlb::NullOutputIterator' to compute the
// sum of a elements in an array while discarding the running total.
//..
//  int main()
//  {
//      static const int myArray[5] = { 2, 3, 5, 7, 11 };
//      int sum = runningSum(myArray, myArray + 5,
//                           bdlb::NullOutputIterator<int>());
//      assert(28 == sum);
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
                        // ==============================
                        // class NullOutputIterator
                        // ==============================

template <class TYPE>
class NullOutputIterator :
    public bsl::iterator<bsl::output_iterator_tag, TYPE> {
    // Provide an output iterator that throws elements into the bit bucket.
    // Dereferencing this iterator and assigning to the result is a no-op.

  public:
    // TYPES
    class AssignmentProxy {
        // Provide an object that can appear on the left side of an assignment
        // from 'TYPE'.  The operation AssignmentProxy() = TYPE() is valid and
        // has no effect.  (i.e., the result goes to the bit bucket.)
      public:
        void operator=(const TYPE&) { }
            // Does nothing.
    };

    // CREATORS
    NullOutputIterator();
        // Creates a null output iterator.

    NullOutputIterator(const NullOutputIterator& original);
        // Construct a copy of 'original'.

    ~NullOutputIterator();
        // Destroy this object.

    // MANIPULATORS
    NullOutputIterator& operator=(const NullOutputIterator& rhs);
        // Assignment operator.  Does nothing and returns '*this'.

    AssignmentProxy operator*();
        // Return an object which can appear on the left-hand side of
        // an assignment from 'TYPE'.  The assignment itself is a no-op.  This
        // function is non-const in accordance with the input iterator
        // requirements, even though '*this' is not modified.

    NullOutputIterator& operator++();
        // Does nothing and returns '*this'.  This function is non-const in
        // accordance with the input iterator requirements, even though
        // '*this' is not modified.

    NullOutputIterator& operator++(int);
        // Does nothing and returns '*this'.  This function is non-const in
        // accordance with the input iterator requirements, even though
        // '*this' is not modified.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

// CREATORS
template <class TYPE>
inline
NullOutputIterator<TYPE>::NullOutputIterator()
{
}

template <class TYPE>
inline
NullOutputIterator<TYPE>::
                      NullOutputIterator(const NullOutputIterator&)
{
}

template <class TYPE>
inline
NullOutputIterator<TYPE>::~NullOutputIterator()
{
}

// MANIPULATORS
template <class TYPE>
inline
NullOutputIterator<TYPE>&
NullOutputIterator<TYPE>::operator=(const NullOutputIterator&)
{
    return *this;
}

template <class TYPE>
inline
typename NullOutputIterator<TYPE>::AssignmentProxy
NullOutputIterator<TYPE>::operator*()
{
    return AssignmentProxy();
}

template <class TYPE>
inline
NullOutputIterator<TYPE>& NullOutputIterator<TYPE>::operator++()
{
    return *this;
}

template <class TYPE>
inline
NullOutputIterator<TYPE>& NullOutputIterator<TYPE>::operator++(int)
{
    return *this;
}
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
