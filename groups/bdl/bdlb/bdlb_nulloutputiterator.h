// bdlb_nulloutputiterator.h                                          -*-C++-*-
#ifndef INCLUDED_BDLB_NULLOUTPUTITERATOR
#define INCLUDED_BDLB_NULLOUTPUTITERATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an output iterator type that discards output.
//
//@CLASSES:
//  bdlb::NullOutputIterator: output iterator template that discards the output
//  bdlb::NullOutputIteratorAssignmentProxy: proxy for assignment
//
//@SEE_ALSO: bdlb_nullinputiterator
//
//@DESCRIPTION: This component provides a class template defining an output
// iterator type, 'bdlb::NullOutputIterator', with the following attributes:
//: o Meets exactly the requirements for an output iterator according to the
//:   C++ Standard (C++98, Section 24.1.2 [lib.output.iterators]).
//: o De-referencing an iterator and assigning to the returned value has no
//:   effect.
//: o Incrementing an iterator has no effect.
//
// This iterator type is typically used to call functions purely for their
// side-effects, discarding the normal output.  It is also useful for testing
// whether a template function will compile when presented with a pure output
// iterator.  This component also provides a template
// 'bdlb::NullOutputIteratorAssignmentProxy' allowing assignment from any type.
//
///Usage
///-----
// In the following example we use a 'bdlb::NullOutputIterator' to enable us to
// call a function to capture its return code, while ignoring the output
// provided through an iterator.
//
// First, we define a function 'runningSum' that returns output both through an
// output iterator and through a return status code:
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
// Now, we define a function 'average' that captures the total sum returned by
// 'runningSum' and uses a 'bdlb::NullOutputIterator' to facilitate calling the
// function, and ignoring the output it provides through its output iterator
// parameter:
//..
//  int average(int values[], int numValues)
//  {
//      int sum = runningSum(values, values + numValues,
//                           bdlb::NullOutputIterator<int>()) / numValues;
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

                   // =======================================
                   // class NullOutputIteratorAssignmentProxy
                   // =======================================

template <class  TYPE>
class NullOutputIteratorAssignmentProxy {
    // Provide an object that can appear on the left side of an assignment
    // from 'TYPE'.  The operation AssignmentProxy() = TYPE() is valid and has
    // no effect.
  public:
    // MANIPULATORS
    void operator=(const TYPE& rhs);
        // Assign to this object the value of the specified 'rhs'. The operator
        // has not effect.
};

                        // ========================
                        // class NullOutputIterator
                        // ========================

template <class TYPE>
class NullOutputIterator :
                         public bsl::iterator<bsl::output_iterator_tag, TYPE> {
    // Provide an output iterator that ignores the output that is provided.
    // De-referencing an iterator and assigning to the returned value has no
    // effect.

  public:
    // TYPES
    typedef NullOutputIteratorAssignmentProxy<TYPE> AssignmentProxy;
        // 'AssignmentProxy' is an alias for an object type returned by
        // de-referencing operator.

  public:
    // CREATORS
    NullOutputIterator();
        // Create a 'NullOutputIterator' object.

    NullOutputIterator(const NullOutputIterator& original);
        // Create a 'NullOutputIterator' object having the value of the
        // specified 'original'.

    ~NullOutputIterator();
        // Destroy this object.

    // MANIPULATORS
    NullOutputIterator& operator=(const NullOutputIterator& rhs);
        // Assign to this object the value of the specified 'rhs' iterator, and
        // return a reference providing modifiable access to this object.

    AssignmentProxy operator*();
        // Return an object that can appear on the left-hand side of an
        // assignment from 'TYPE'.  The assignment to the returned object has
        // no effect.

    NullOutputIterator& operator++();
        // Set this object to point to the next writable element, and return a
        // reference providing modifiable access to this object.  This
        // operation has no effect.

    NullOutputIterator& operator++(int);
        // Set this object to point to the next writable element, and return a
        // reference providing modifiable access to this object.  This
        // operation has no effect.
};

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                   // ---------------------------------------
                   // class NullOutputIteratorAssignmentProxy
                   // ---------------------------------------

// MANIPULATORS
template <class TYPE>
inline
void
NullOutputIteratorAssignmentProxy<TYPE>::operator=(const TYPE&)
{
}
                         // ------------------------
                         // class NullOutputIterator
                         // ------------------------

// CREATORS
template <class TYPE>
inline
NullOutputIterator<TYPE>::NullOutputIterator()
{
}

template <class TYPE>
inline
NullOutputIterator<TYPE>::NullOutputIterator(const NullOutputIterator&)
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
