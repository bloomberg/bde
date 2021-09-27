// bsltf_inputiterator.h                                              -*-C++-*-
#ifndef INCLUDED_BSLTF_INPUTITERATOR
#define INCLUDED_BSLTF_INPUTITERATOR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a pure input iterator capable of traversing a range.
//
//@CLASSES:
//  bsltf::InputIterator: empty input iterator template
//
//@DESCRIPTION: This components provides a value-semantic 'class',
// 'bsltf::InputIterator', that defines an input iterator that supports the
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
///Example 1: Basic Use of 'bsltf::InputIterator':
/// - - - - - - - - - - - - - - - - - - - - - - -
// In the following example we use a 'bsltf::InputIterator' to test that an
// aggregation function compiles and works when instantiated with a pure input
// iterator.
//
// First, we define a function 'sum' that accepts two input iterators and
// returns the sum of all elements in range specified by them:
//..
//  template <class IN_ITER>
//  double sum(IN_ITER first, IN_ITER last)
//      // Return the sum of the 'double's in the specified range
//      // '[ first, last )'.
//  {
//      double total = 0;
//      while (first != last) {
//          total += *first++;
//      }
//      return total;
//  }
//..
// Then, in 'main', we define an array of 'double's and define 'InputIterators'
// pointing to the beginning and ending of it, initializing the iterators with
// pointers:
//..
//  static double myArray[] = { 2.5, 3, 5, 7, 11.5, 5 };
//  enum { k_MY_ARRAY_LEN = sizeof myArray / sizeof *myArray };
//
//  typedef bsltf::InputIterator<const double> Iter;
//
//  Iter begin(myArray + 0), end(myArray + k_MY_ARRAY_LEN);
//..
// Next, we call 'sum' with the two iterators, and observe that its yields the
// expected result, and because it compiles, we know that 'sum' did not attempt
// any operations on the iterators other than those defined for the most basic
// input iterator:
//..
//  const double x = sum(begin, end);
//  assert(34.0 == x);
//..
// Then, we illustrate that we can just make 'begin' and 'end' iterators from
// the array directly with the 'begin' and 'end' class methods of the
// 'InputIteratorUtil' class.
//..
//  typedef bsltf::InputIteratorUtil Util;
//
//  const double y = sum(Util::begin(myArray), Util::end(myArray));
//  assert(34.0 == y);
//..
// Now, we make an 'std::vector' containing the elements of 'myArray':
//..
//  const std::vector<double> v(myArray + 0, myArray + k_MY_ARRAY_LEN);
//..
// Finally, we call 'sum' using, again, the 'begin' and 'end' class methods to
// create iterators for it directly from our 'vector':
//..
//  const double z = sum(Util::begin(v), Util::end(v));
//  assert(34.0 == z);
//..

#include <bslscm_version.h>

#include <bsls_assert.h>
#include <bsls_libraryfeatures.h>

#include <cstddef>
#include <iterator>

namespace BloombergLP {
namespace bsltf {

                            // ===================
                            // class InputIterator
                            // ===================

#if defined(BSLS_LIBRARYFEATURES_STDCPP_LIBCSTD)
// Sun CC workaround: iterators must be derived from 'std::iterator' to work
// with the native std library algorithms.  However, 'std::iterator' is
// deprecated in C++17, so do not rely on derivation unless required, to avoid
// deprecation warnings on modern compilers.

template <class TYPE>
class InputIterator : public std::iterator<std::input_iterator_tag,
                                           TYPE,
                                           std::ptrdiff_t,
                                           TYPE *,
                                           TYPE> {
#else
template <class TYPE>
class InputIterator {
#endif
    // Provide an input iterator that iterates that can iterate over a
    // contiguous range of object while supporting no operations other than
    // those defined for 'LegacyInputIterator'.

  public:
    // PUBLIC TYPES
    typedef std::input_iterator_tag  iterator_category;
    typedef TYPE                     value_type;
    typedef std::ptrdiff_t           difference_type;
    typedef TYPE                    *pointer;
    typedef TYPE&                    reference;

  private:
    // DATA
    TYPE *d_value_p;

    // FRIENDS
    template <class TYPE2>
    friend bool operator==(const InputIterator<TYPE2>&,
                           const InputIterator<TYPE2>&);
    template <class TYPE2>
    friend bool operator!=(const InputIterator<TYPE2>&,
                           const InputIterator<TYPE2>&);

  public:
    // CREATORS
    InputIterator();
        // Construct an empty input iterator.

    explicit InputIterator(TYPE *ptr);
        // Construct an input iterator based on the specified 'ptr'.

    // InputIterator(const InputIterator& original) = default;
        // Construct a copy of the specified 'original' object.

    // ~InputIterator() = default;
        // Destroy this object.

    // MANIPULATORS
    // InputIterator& operator=(const InputIterator& rhs) = default;
        // Assign to this object the value of the specified 'rhs' and return a
        // reference to this modifiable object.

    InputIterator& operator++();
        // Increment this iterator to refer to the next contiguous 'TYPE'
        // object, and return a reference to after the increment.  The behavior
        // is undefined if this iterator is empty.

    InputIterator operator++(int);
        // Copy this iterator, increment, and return by value the copy that was
        // made prior to the increment.  The behavior is undefined if this
        // iterator is empty.

    // ACCESSORS
    pointer operator->() const;
        // Return a pointer to the 'TYPE' object referred to by this iterator.
        // The behavior is undefined if this iterator is empty.

    reference operator*() const;
        // Return a const reference to the 'TYPE' object referred to by this
        // iterator.  The behavior is undefined if this iterator is empty.
};

// FREE OPERATORS
template <class TYPE>
inline
bool operator==(const InputIterator<TYPE>& lhs,
                const InputIterator<TYPE>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' refer to the same 'TYPE'
    // object and 'false' othersise.

template <class TYPE>
inline
bool operator!=(const InputIterator<TYPE>& lhs,
                const InputIterator<TYPE>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' do not refer to the same
    // 'TYPE' object and 'false' othersise.

                          // =======================
                          // class InputIteratorUtil
                          // =======================

struct InputIteratorUtil {
    // This namespace 'struct' allows the easy creation of 'begin' and 'end'
    // iterators from a variety of contiguous range types.  Either the argument
    // must be an array, or a container that supports the following operations:
    //: o 'size()'
    //:
    //: o 'operator[](size_t)' (if 0 != size())
    //
    // and the type 'CONTIGUOUS_CONTAINER::value_type'.

    // CLASS METHODS
    template <class CONTIGUOUS_CONTAINER>
    static InputIterator<typename CONTIGUOUS_CONTAINER::value_type> begin(
                                              CONTIGUOUS_CONTAINER& container);
    template <class CONTIGUOUS_CONTAINER>
    static InputIterator<const typename CONTIGUOUS_CONTAINER::value_type>
                                  begin(const CONTIGUOUS_CONTAINER& container);
        // Return an 'InputIterator' referring to the first element of the
        // specified 'container' or a null iterator if 'container' is empty.

    template <class TYPE, std::size_t LEN>
    static InputIterator<TYPE> begin(TYPE (&array)[LEN]);
    template <class TYPE, std::size_t LEN>
    static InputIterator<const TYPE> begin(const TYPE (&array)[LEN]);
        // Return an 'InputIterator' referring to the first element of the
        // specified 'array'.

    template <class CONTIGUOUS_CONTAINER>
    static InputIterator<typename CONTIGUOUS_CONTAINER::value_type> end(
                                              CONTIGUOUS_CONTAINER& container);
    template <class CONTIGUOUS_CONTAINER>
    static InputIterator<const typename CONTIGUOUS_CONTAINER::value_type> end(
                                        const CONTIGUOUS_CONTAINER& container);
        // Return an 'InputIterator' referring to after the last element of the
        // specified 'container' or a null iterator if 'container' is empty.

    template <class TYPE, std::size_t LEN>
    static InputIterator<TYPE> end(TYPE (&array)[LEN]);
    template <class TYPE, std::size_t LEN>
    static InputIterator<const TYPE> end(const TYPE (&array)[LEN]);
        // Return an 'InputIterator' referring to after the last element of the
        // specified 'array'.
};

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                              // -------------------
                              // class InputIterator
                              // -------------------

// CREATORS
template<class TYPE>
inline
InputIterator<TYPE>::InputIterator()
: d_value_p(0)
{
}

template<class TYPE>
inline
InputIterator<TYPE>::InputIterator(TYPE *ptr)
: d_value_p(ptr)
{
}

// MANIPULATORS
template<class TYPE>
inline
InputIterator<TYPE>& InputIterator<TYPE>::operator++()
{
    BSLS_ASSERT_SAFE(d_value_p);

    ++d_value_p;

    return *this;
}

template<class TYPE>
inline
InputIterator<TYPE> InputIterator<TYPE>::operator++(int)
{
    BSLS_ASSERT_SAFE(d_value_p);

    TYPE *prev = d_value_p++;
    return InputIterator(prev);
}

// ACCESSORS
template<class TYPE>
inline
typename InputIterator<TYPE>::pointer InputIterator<TYPE>::operator->() const
{
    BSLS_ASSERT_SAFE(d_value_p);

    return d_value_p;
}

template<class TYPE>
inline
typename InputIterator<TYPE>::reference InputIterator<TYPE>::operator*() const
{
    BSLS_ASSERT_SAFE(d_value_p);

    return *d_value_p;
}

}  // close package namespace

// FREE OPERATORS
template <class TYPE>
inline
bool bsltf::operator==(const bsltf::InputIterator<TYPE>& lhs,
                       const bsltf::InputIterator<TYPE>& rhs)
{
    return lhs.d_value_p == rhs.d_value_p;
}

template <class TYPE>
inline
bool bsltf::operator!=(const bsltf::InputIterator<TYPE>& lhs,
                       const bsltf::InputIterator<TYPE>& rhs)
{
    return lhs.d_value_p != rhs.d_value_p;
}

namespace bsltf {

                          // -----------------------
                          // class InputIteratorUtil
                          // -----------------------

// CLASS METHODS
template <class CONTIGUOUS_CONTAINER>
inline
InputIterator<typename CONTIGUOUS_CONTAINER::value_type>
                      InputIteratorUtil::begin(CONTIGUOUS_CONTAINER& container)
{
    return InputIterator<typename CONTIGUOUS_CONTAINER::value_type>(
                                    0 == container.size() ? 0 : &container[0]);
}

template <class CONTIGUOUS_CONTAINER>
InputIterator<const typename CONTIGUOUS_CONTAINER::value_type>
                InputIteratorUtil::begin(const CONTIGUOUS_CONTAINER& container)
{
    return InputIterator<const typename CONTIGUOUS_CONTAINER::value_type>(
                                    0 == container.size() ? 0 : &container[0]);
}

template <class TYPE, std::size_t LEN>
InputIterator<TYPE> InputIteratorUtil::begin(TYPE (&array)[LEN])
{
    return InputIterator<TYPE>(array + 0);
}

template <class TYPE, std::size_t LEN>
InputIterator<const TYPE> InputIteratorUtil::begin(const TYPE (&array)[LEN])
{
    return InputIterator<const TYPE>(array + 0);
}

template <class CONTIGUOUS_CONTAINER>
InputIterator<typename CONTIGUOUS_CONTAINER::value_type>
                        InputIteratorUtil::end(CONTIGUOUS_CONTAINER& container)
{
    return InputIterator<typename CONTIGUOUS_CONTAINER::value_type>(
                 0 == container.size() ? 0 : &container[0] + container.size());
}

template <class CONTIGUOUS_CONTAINER>
InputIterator<const typename CONTIGUOUS_CONTAINER::value_type>
                  InputIteratorUtil::end(const CONTIGUOUS_CONTAINER& container)
{
    return InputIterator<const typename CONTIGUOUS_CONTAINER::value_type>(
                 0 == container.size() ? 0 : &container[0] + container.size());
}

template <class TYPE, std::size_t LEN>
InputIterator<TYPE> InputIteratorUtil::end(TYPE (&array)[LEN])
{
    return InputIterator<TYPE>(array + LEN);
}

template <class TYPE, std::size_t LEN>
InputIterator<const TYPE> InputIteratorUtil::end(const TYPE (&array)[LEN])
{
    return InputIterator<const TYPE>(array + LEN);
}

}  // close package namespace
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
