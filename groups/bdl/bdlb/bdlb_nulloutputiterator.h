// bdlb_nulloutputiterator.h                                          -*-C++-*-
#ifndef INCLUDED_BDLB_NULLOUTPUTITERATOR
#define INCLUDED_BDLB_NULLOUTPUTITERATOR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an output iterator type that discards output.
//
//@CLASSES:
//  bdlb::NullOutputIterator: output iterator template that discards the output
//  bdlb::NullOutputIterator::AssignmentProxy: proxy for assignment
//
//@SEE_ALSO: bdlb_nullinputiterator
//
//@DESCRIPTION: This component provides a mechanism,
// `bdlb::NullOutputIterator`, that defines an output iterator with the
// following attributes:
// * Meets exactly the requirements for an output iterator according to the
//   C++ Standard (C++98, Section 24.1.2 [lib.output.iterators]).
// * De-referencing an iterator and assigning to the returned value has no
//   effect.
// * Incrementing an iterator has no effect.
//
// This iterator type is typically used to call functions purely for their
// side-effects, discarding the normal output.  It is also useful for testing
// whether a template function will compile when presented with a pure output
// iterator.  This component also provides a template
// `bdlb::NullOutputIterator::AssignmentProxy`, that is used as the return type
// of `bdlb::NullOutputIterator::operator*`.  The `AssignmentProxy` provides an
// `operator=` that does nothing, so that the result of the iterator's
// `operator*` can be assigned to even if the value type of the
// `bdlb::NullOutputIterator` does not provide a default constructor:
// ```
// class ValueType {
//     // ... data members ...
//
//   public:
//     ValueType(int value) { ... implementation elided ... }
//
//     // ... rest of class definition elided ...
//
// };
//
// ValueType v(42);
// bdlb::NullOutputIterator<ValueType> i;
//
// // With a non-proxy return type for 'operator*' it would be difficult to
// // provide a value for the lefthand side of this expression:
//
// *i = v;
// ```
//
///Usage
///-----
// This section illustrates intended use of this component.
//
/// Example 1: Basic Use of `bdlb::NullOutputIterator`
///- - - - - - - - - - - - - - - - - - - - - - - - - -
// In the following example we use a `bdlb::NullOutputIterator` to enable us to
// call a function to capture its return code, while ignoring the output
// provided through an iterator.
//
// First, we define a function `runningSum` that returns output both through an
// output iterator and through a return status code:
// ```
// template <class IN_ITER, class OUT_ITER>
// typename bsl::iterator_traits<OUT_ITER>::value_type
// runningSum(IN_ITER first, IN_ITER last, OUT_ITER output)
// {
//     typename bsl::iterator_traits<OUT_ITER>::value_type total = 0;
//     while (first != last) {
//         total += *first++;
//         *output++ = total;
//     }
//     return total;
// }
// ```
// Now, we define a function `average` that captures the total sum returned by
// `runningSum` and uses a `bdlb::NullOutputIterator` to facilitate calling the
// function, and ignoring the output it provides through its output iterator
// parameter:
// ```
// int average(int values[], int numValues)
// {
//     // ... input validation elided ...
//     return runningSum(values, values + numValues,
//                       bdlb::NullOutputIterator<int>()) / numValues;
// }
// ```
// Finally, we invoke function `average` on user array and validate result.
// ```
// void usageExample()
// {
//     const int myArray[5] = { 3, 4, 5, 7, 11 };
//
//     int averageValue = average(myArray, 5);
//     assert( averageValue == 6 );
// }
// ```

#include <bdlscm_version.h>

#include <bsls_libraryfeatures.h>
#include <bsl_iterator.h>

namespace BloombergLP {
namespace bdlb {

                   // =======================================
                   // class NullOutputIteratorAssignmentProxy
                   // =======================================

/// Provide an object that can appear on the left side of an assignment
/// from `TYPE`.  The operation AssignmentProxy() = TYPE() is valid and has
/// no effect.
template <class  TYPE>
class NullOutputIteratorAssignmentProxy {
  public:
    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs`. The operator
    /// has no effect.
    void operator=(const TYPE& rhs);
};

                        // ========================
                        // class NullOutputIterator
                        // ========================

/// Provide an output iterator that ignores the output that is provided.
/// De-referencing an iterator and assigning to the returned value has no
/// effect.
template <class TYPE>
class NullOutputIterator
#if defined(BSLS_LIBRARYFEATURES_STDCPP_LIBCSTD)
// Sun CC workaround: iterators must be derived from `std::iterator` to work
// with the native std library algorithms.  However, `std::iterator` is
// deprecated in C++17, so do not rely on derivation unless required, to avoid
// deprecation warnings on modern compilers.
                         : public bsl::iterator<bsl::output_iterator_tag, TYPE>
#endif
{

  public:
    // TYPES

    /// `AssignmentProxy` is an alias for an object type returned by
    /// de-referencing operator.
    typedef NullOutputIteratorAssignmentProxy<TYPE> AssignmentProxy;

    typedef bsl::output_iterator_tag  iterator_category;
    typedef TYPE                      value_type;
    typedef void                      difference_type;
    typedef void                      pointer;
    typedef void                      reference;

  public:
    // CREATORS

    /// Create a `NullOutputIterator` object.
    NullOutputIterator();

    /// Create a `NullOutputIterator` object having the value of the
    /// specified `original`.
    NullOutputIterator(const NullOutputIterator& original);

    /// Destroy this object.
    ~NullOutputIterator();

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` iterator, and
    /// return a reference providing modifiable access to this object.
    NullOutputIterator& operator=(const NullOutputIterator& rhs);

    /// Return an object that can appear on the left-hand side of an
    /// assignment from `TYPE`.  The assignment to the returned object has
    /// no effect.
    AssignmentProxy operator*();

    /// Set this object to point to the next writable element, and return a
    /// reference providing modifiable access to this object.  This
    /// operation has no effect.
    NullOutputIterator& operator++();

    /// Set this object to point to the next writable element, and return a
    /// reference providing modifiable access to this object.  This
    /// operation has no effect.
    NullOutputIterator& operator++(int);
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
