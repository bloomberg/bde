// bslstl_array.h                                                     -*-C++-*-
#ifndef INCLUDED_BSLSTL_ARRAY
#define INCLUDED_BSLSTL_ARRAY

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an STL compliant array.
//
//@CLASSES:
//   bsl::array: an STL compliant array
//
//@SEE_ALSO: bslstl_vector
//
//@AUTHOR: Jordan Levin (jlevin)
//
//@DESCRIPTION: This component defines a single class template, 'bsl::array',
// implementing the standard container 'std::array', holding a non-resizable
// array of values of a template parameter type where the size is specified as
// the second template parameter.
//
// An instantiation of 'array' is a value-semantic type whose salient attibutes
// are its size and the sequence of values the array contains.  If 'array' is
// instantiated with a value type that is not value-semantic, then the array
// will not retain all of its value-semantic qualities.  In particular, if a
// value type cannot be tested for equality, then an 'array' containing objects
// of that type cannot be tested for equality.  Similarly, if an 'array' is
// instantiated with a type that does not have a copy-constructor, then the
// 'array' will not be copyable.
//
// An array meets the requirements of a container with random access iterators
// in the C++ standard [array].  The 'array' implemented here follows the C++11
// standard when compiled with a C++11 compiler and follows the C++03 standard
// otherwise.

// An array lacks certain requirements of a sequential container.  Array lacks
// insert, erase, emplace, and clear, as these functions would require
// modifying the size of the array.

// An array also meets the requirements of an aggregate.  This means that an
// array has: no user-declared constructors, no private or protected non-static
// data members, no base classes, and no virtual functions.  An array can be
// constructed using aggregate initialization.  Refer to the section
// [del.init.aggr] in the C++ standard for more detailed information.
//
///Operations
///----------
// This section describes the run-time complexity of operations on instances of
// 'array':
//..
//  Legend
//  ------
//  'V'                 - (template parameter) 'VALUE_TYPE' of the array
//  'S'                 - (template parameter) 'SIZE' of the array
//  'a', 'b'            - two distinct objects of type 'array<V, S>'
//  'k'                 - non-negative integer
//  'vt1', 'vt2', 'vt3' - objects of type 'VALUE_TYPE'
//
//  |-----------------------------------------+-------------------------------|
//  | Operation                               | Complexity                    |
//  |=========================================+===============================|
//  | array<V> a    (default construction)    | O[1]                          |
//  |-----------------------------------------+-------------------------------|
//  | array<V> a(b) (copy construction)       | O[S]                          |
//  |-----------------------------------------+-------------------------------|
//  | array<V> a = {{vt1, vt2, vt3}}          | O[S]                          |
//  | array<V> a = {vt1, vt2, vt3}            |                               |
//  |               (aggregate initialization)|                               |
//  |-----------------------------------------+-------------------------------|
//  | a.~array<V>() (destruction)             | O[S]                          |
//  |-----------------------------------------+-------------------------------|
//  | a.begin(), a.end(),                     | O[1]                          |
//  | a.cbegin(), a.cend(),                   |                               |
//  | a.rbegin(), a.rend(),                   |                               |
//  | a.crbegin(), a.crend()                  |                               |
//  |-----------------------------------------+-------------------------------|
//  | a.size()                                | O[1]                          |
//  |-----------------------------------------+-------------------------------|
//  | a.max_size()                            | O[1]                          |
//  |-----------------------------------------+-------------------------------|
//  | a.empty()                               | O[1]                          |
//  |-----------------------------------------+-------------------------------|
//  | a[k]                                    | O[1]                          |
//  |-----------------------------------------+-------------------------------|
//  | a.at(k)                                 | O[1]                          |
//  |-----------------------------------------+-------------------------------|
//  | a.front()                               | O[1]                          |
//  |-----------------------------------------+-------------------------------|
//  | a.back()                                | O[1]                          |
//  |-----------------------------------------+-------------------------------|
//  | a.swap(b), swap(a,b)                    | O[S]                          |
//  |-----------------------------------------+-------------------------------|
//  | a = b;           (copy assignment)      | O[S]                          |
//  |-----------------------------------------+-------------------------------|
//  | a == b, a != b                          | O[S]                          |
//  |-----------------------------------------+-------------------------------|
//  | a < b, a <= b, a > b, a >= b            | O[S]                          |
//  |-----------------------------------------+-------------------------------|
//..
//

#include <stddef.h>
#include <bsls_assert.h>
#include <bslscm_version.h>
#include <bslstl_iterator.h>
#include <bslstl_stdexceptutil.h>

namespace bsl {

                                // ===========
                                // class array
                                // ===========

template <class VALUE_TYPE, size_t SIZE>
class array {
    // This class template provides an STL-compliant 'array'.  Consult the
    // C++11 standard for the requirements of an array class.  This class is an
    // aggregate that allows for aggregate initialization.  This class provides
    // a standard container interface wrapped around a raw array.
  public:
// BDE_VERIFY pragma: push
// BDE_VERIFY pragma: -MN01
    // DATA
    VALUE_TYPE d_data[(0 == SIZE) ? 1 : SIZE];
// BDE_VERIFY pragma: pop

    // PUBLIC TYPES
    typedef VALUE_TYPE                             value_type;
    typedef VALUE_TYPE                            *pointer;
    typedef const VALUE_TYPE                      *const_pointer;
    typedef VALUE_TYPE&                            reference;
    typedef const VALUE_TYPE&                      const_reference;
    typedef size_t                                 size_type;
    typedef ptrdiff_t                              difference_type;
    typedef pointer                                iterator;
    typedef const_pointer                          const_iterator;
    typedef bsl::reverse_iterator<iterator>        reverse_iterator;
    typedef bsl::reverse_iterator<const_iterator>  const_reverse_iterator;

    // CREATORS
    //! array() = default;
    // Create an 'array' object.  Every element is default constructed if
    // 'VALUE_TYPE' is default constructable; otherwise, array is not default
    // constructable.
    //! array(const array& other) = default;
        // Create an 'array' object.  Every element is copy constructed from
        // the corrosponding element in the specified 'other' if 'VALUE_TYPE'
        // is copy constructable; otherwise, array is not copy constructable.
        //! ~array() = default;
        // Destroy this object.  Evert element is destroyed if 'VALUE_TYPE' is
        // destructible; otherwise, array is not destructible.

    // MANIPULATORS
    void fill(const VALUE_TYPE& value);
        // Set every elemment in this array to the specified 'value' using the
        // 'operator=' of 'value_type'.

    void swap(array& rhs);
        // Exchange each corrosponding element between this array and the
        // specified 'rhs' array by calling 'swap(a,b)' where 'swap' is found
        // by overload resolution including at least the namespaces 'std' and
        // the associated namespaces of 'VALUE_TYPE'.

    iterator begin();
        // Return an iterator providing modifiable access to the first element
        // in this array; return a past-the-end iterator if this array has size
        // 0.

    iterator end();
        // Return a past-the-end iterator providing modifiable access to this
        // array.

    reverse_iterator rbegin();
        // Return a reverse iterator providing modifiable access to the last
        // element in this array; return a past-the-end iterator if this array
        // has size 0.

    reverse_iterator rend();
        // Return a reverse iterator providing modifiable access to the first
        // element in this array; return a past-the-end iterator if this array
        // has size 0.

    reference operator[](size_type position);
        // Return a reference providing modifiable access to the element at the
        // specified 'position' in this array.  The behavior is undefined
        // unless 'position < size()'.

    reference at(size_type position);
        // Return a reference to the element at the specified 'position' in
        // this array.  Throw an 'out_of_range' exception if
        // 'position >= size()'.

    reference front();
        // Return a reference to the first element in this array.  The behavior
        // is undefined unless 'SIZE > 0'.

    reference back();
        // Return a reference to the last element in this array.  The Behavior
        // is undefined unless 'SIZE > 0'.

    pointer data();
        // Return the address of the first element of the underlying raw array.

    //! array& operator=(const array& other);
    // Sets every element in this array to the corrosponding element in the
    // specified 'other' if 'VALUE_TYPE' is copy assignable; otherwise, array
    // is not copy assignable.


    // BDE_VERIFY pragma: -FABC01
    // ACCESSORS
    const_iterator begin() const;
    const_iterator cbegin() const;
        // Return an iterator providing non-modifiable access to the first
        // element in this array; return a past-the-end iterator if this array
        // has size 0.

    const_iterator end() const;
    const_iterator cend() const;
        // Return a past-the-end iterator providing non-modifiable access to
        // this array.

    const_reverse_iterator rbegin() const;
    const_reverse_iterator crbegin() const;
        // Return a reverse iterator providing non-modifiable access to the
        // last element in this array; return a past-the-end iterator if this
        // array has size 0.

    const_reverse_iterator rend() const;
    const_reverse_iterator crend() const;
        // Return a reverse iterator providing non-modifiable access to the
        // first element in this array; return a past-the-end iterator if this
        // array has size 0.

    bool empty() const;
        // Return 'true' if the array has size 0, and 'false' otherwise.

    size_type size() const;
    size_type max_size() const;
        // Return the number of elements in this array.

    const_reference operator[](size_type position) const;
        // Return a reference providing non-modifiable access to the element at
        // the specified 'position' in this array.  The behavior is undefined
        // unless 'position < size()'.

    const_reference at(size_type position) const;
        // Return a reference providing non-modifiable access to the element at
        // the specified 'position' in this array.  Throw an 'out_of_range'
        // exception if 'position >= size()'.

    const_reference front() const;
        // Return a reference providing non-modifiable access to the first
        // element in this array.  The behavior is undefined unless 'SIZE > 0'.

    const_reference back() const;
        // Return a reference providing non-modifiable access to the last
        // element in this array.  Behavior is undefined unless 'SIZE > 0'.

    const_pointer data() const;
        // Return the address of the first element of the underlying raw array.
};

// FREE OPERATORS
template <class VALUE_TYPE, size_t SIZE1, size_t SIZE2>
bool operator==(const array<VALUE_TYPE, SIZE1>& lhs,
                const array<VALUE_TYPE, SIZE2>& rhs);
    // Return 'true' if the specified 'lhs' has the same value as the specified
    // 'rhs'; return false otherwise.  Two arrays have the same value if they
    // have the same 'size' and each element has the same value as the
    // corrosponding element in the other array.

template <class VALUE_TYPE, size_t SIZE1, size_t SIZE2>
bool operator!=(const array<VALUE_TYPE, SIZE1>& lhs,
                const array<VALUE_TYPE, SIZE2>& rhs);
    // Return 'true' if the specified 'lhs' does not have the same value as the
    // specified 'rhs'; return false otherwise.  Two arrays have the same value
    // if they have the same 'size' and each element has the same value as the
    // corrosponding element in the other array.

template <class VALUE_TYPE, size_t SIZE1, size_t SIZE2>
bool operator<(const array<VALUE_TYPE, SIZE1>& lhs,
               const array<VALUE_TYPE, SIZE2>& rhs);
    // Return 'true' if the specified 'lhs' is lexicographically less than the
    // specified 'rhs' by using the comparison operators of 'VALUE_TYPE' on
    // each element; return 'false' otherwise.

template <class VALUE_TYPE, size_t SIZE1, size_t SIZE2>
bool operator>(const array<VALUE_TYPE, SIZE1>& lhs,
               const array<VALUE_TYPE, SIZE2>& rhs);
    // Return 'true' if the specified 'lhs' is lexicographically greater than
    // the specified 'rhs' by using the comparison operators of 'VALUE_TYPE' on
    // each element; return 'false' otherwise.

template <class VALUE_TYPE, size_t SIZE1, size_t SIZE2>
bool operator<=(const array<VALUE_TYPE, SIZE1>& lhs,
                const array<VALUE_TYPE, SIZE2>& rhs);
    // Return 'true' if the specified 'lhs' is lexicographically less than the
    // specified 'rhs' by using the comparison operators of 'VALUE_TYPE' on
    // each element or if 'lhs' and 'rhs' are equal; return 'false' otherwise.

template <class VALUE_TYPE, size_t SIZE1, size_t SIZE2>
bool operator>=(const array<VALUE_TYPE, SIZE1>& lhs,
                const array<VALUE_TYPE, SIZE2>& rhs);
    // Return 'true' if the specified 'lhs' is lexicographically greater than
    // the specified 'rhs' by using the comparison operators of 'VALUE_TYPE' on
    // each element or if 'lhs' and 'rhs' are equal; return 'false' otherwise.

template <class VALUE_TYPE, size_t SIZE1, size_t SIZE2>
void swap(array<VALUE_TYPE, SIZE1>& lhs, array<VALUE_TYPE, SIZE2>& rhs);
    // Set every value in the specified 'lhs' to the value at that index in the
    // specified 'rhs' and vice versa.  'rhs' must have the exact same type as
    // 'lhs'

// ============================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ============================================================================

                                // -----------
                                // class array
                                // -----------

// MANIPULATORS
template <class VALUE_TYPE, size_t SIZE>
void array<VALUE_TYPE, SIZE>::fill(const VALUE_TYPE& value)
{
    for (size_t i = 0; i < SIZE; ++i) {
        d_data[i] = value;
    }
}

template <class VALUE_TYPE, size_t SIZE>
void array<VALUE_TYPE, SIZE>::swap(array<VALUE_TYPE, SIZE>& rhs)
{
    using std::swap;
    for (size_t i = 0; i < SIZE; ++i) {
        swap(d_data[i], rhs[i]);
    }
}

// ACCESSORS
template <class VALUE_TYPE, size_t SIZE>
typename array<VALUE_TYPE, SIZE>::iterator array<VALUE_TYPE, SIZE>::begin()
{
    return d_data;
}

template <class VALUE_TYPE, size_t SIZE>
typename array<VALUE_TYPE, SIZE>::const_iterator
array<VALUE_TYPE, SIZE>::begin() const
{
    return d_data;
}

template <class VALUE_TYPE, size_t SIZE>
typename array<VALUE_TYPE, SIZE>::iterator array<VALUE_TYPE, SIZE>::end()
{
    return d_data + SIZE;
}

template <class VALUE_TYPE, size_t SIZE>
typename array<VALUE_TYPE, SIZE>::const_iterator
array<VALUE_TYPE, SIZE>::end() const
{
    return d_data + SIZE;
}

template <class VALUE_TYPE, size_t SIZE>
typename array<VALUE_TYPE, SIZE>::reverse_iterator
array<VALUE_TYPE, SIZE>::rbegin()
{
    return array<VALUE_TYPE, SIZE>::reverse_iterator(d_data + SIZE);
}

template <class VALUE_TYPE, size_t SIZE>
typename array<VALUE_TYPE, SIZE>::const_reverse_iterator
array<VALUE_TYPE, SIZE>::rbegin() const
{
    return array<VALUE_TYPE, SIZE>::const_reverse_iterator(d_data + SIZE);
}

template <class VALUE_TYPE, size_t SIZE>
typename array<VALUE_TYPE, SIZE>::reverse_iterator
array<VALUE_TYPE, SIZE>::rend()
{
    return array<VALUE_TYPE, SIZE>::reverse_iterator(d_data);
}

template <class VALUE_TYPE, size_t SIZE>
typename array<VALUE_TYPE, SIZE>::const_reverse_iterator
array<VALUE_TYPE, SIZE>::rend() const
{
    return array<VALUE_TYPE, SIZE>::const_reverse_iterator(d_data);
}

template <class VALUE_TYPE, size_t SIZE>
typename array<VALUE_TYPE, SIZE>::const_iterator
array<VALUE_TYPE, SIZE>::cbegin() const
{
    return d_data;
}

template <class VALUE_TYPE, size_t SIZE>
typename array<VALUE_TYPE, SIZE>::const_iterator
array<VALUE_TYPE, SIZE>::cend() const
{
    return d_data + SIZE;
}

template <class VALUE_TYPE, size_t SIZE>
typename array<VALUE_TYPE, SIZE>::const_reverse_iterator
array<VALUE_TYPE, SIZE>::crbegin() const
{
    return array<VALUE_TYPE, SIZE>::const_reverse_iterator(d_data + SIZE);
}

template <class VALUE_TYPE, size_t SIZE>
typename array<VALUE_TYPE, SIZE>::const_reverse_iterator
array<VALUE_TYPE, SIZE>::crend() const
{
    return array<VALUE_TYPE, SIZE>::const_reverse_iterator(d_data);
}

template <class VALUE_TYPE, size_t SIZE>
bool array<VALUE_TYPE, SIZE>::empty() const
{
    return SIZE == 0;
}

template <class VALUE_TYPE, size_t SIZE>
size_t array<VALUE_TYPE, SIZE>::size() const
{
    return SIZE;
}

template <class VALUE_TYPE, size_t SIZE>
size_t array<VALUE_TYPE, SIZE>::max_size() const
{
    return SIZE;
}

template <class VALUE_TYPE, size_t SIZE>
typename array<VALUE_TYPE, SIZE>::reference
array<VALUE_TYPE, SIZE>::operator[](size_type position)
{
    BSLS_ASSERT(position >= 0 && position < SIZE);
    return d_data[position];
}

template <class VALUE_TYPE, size_t SIZE>
typename array<VALUE_TYPE, SIZE>::const_reference
array<VALUE_TYPE, SIZE>::operator[](size_type position) const
{
    BSLS_ASSERT(position >= 0 && position < SIZE);
    return d_data[position];
}

template <class VALUE_TYPE, size_t SIZE>
typename array<VALUE_TYPE, SIZE>::reference array<VALUE_TYPE, SIZE>::at(
                                                            size_type position)
{
    if (position >= SIZE) {
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange(
            "array<...>::at(position): invalid position");
    }
    return d_data[position];
}

template <class VALUE_TYPE, size_t SIZE>
typename array<VALUE_TYPE, SIZE>::const_reference array<VALUE_TYPE, SIZE>::at(
                                                      size_type position) const
{
    if (position >= SIZE) {
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange(
            "array<...>::at(position): invalid position");
    }
    return d_data[position];
}

template <class VALUE_TYPE, size_t SIZE>
typename array<VALUE_TYPE, SIZE>::reference array<VALUE_TYPE, SIZE>::front()
{
    BSLS_ASSERT(SIZE > 0);
    return d_data[0];
}

template <class VALUE_TYPE, size_t SIZE>
typename array<VALUE_TYPE, SIZE>::const_reference
array<VALUE_TYPE, SIZE>::front() const
{
    BSLS_ASSERT(SIZE > 0);
    return d_data[0];
}

template <class VALUE_TYPE, size_t SIZE>
typename array<VALUE_TYPE, SIZE>::reference array<VALUE_TYPE, SIZE>::back()
{
    BSLS_ASSERT(SIZE > 0);
    return d_data[SIZE - 1];
}

template <class VALUE_TYPE, size_t SIZE>
typename array<VALUE_TYPE, SIZE>::const_reference
array<VALUE_TYPE, SIZE>::back() const
{
    BSLS_ASSERT(SIZE > 0);
    return d_data[SIZE - 1];
}

template <class VALUE_TYPE, size_t SIZE>
typename array<VALUE_TYPE, SIZE>::iterator array<VALUE_TYPE, SIZE>::data()
{
    return d_data;
}

template <class VALUE_TYPE, size_t SIZE>
typename array<VALUE_TYPE, SIZE>::const_iterator
array<VALUE_TYPE, SIZE>::data() const
{
    return d_data;
}

// FREE OPERATORS
template <class VALUE_TYPE, size_t SIZE1, size_t SIZE2>
bool operator==(const array<VALUE_TYPE, SIZE1>& lhs,
                const array<VALUE_TYPE, SIZE2>& rhs)
{
    if (SIZE1 != SIZE2)
        return false;                                                 // RETURN
    for (size_t i = 0; i < SIZE1; i++) {
        if (lhs[i] != rhs[i])
            return false;                                             // RETURN
    }
    return true;                                                      // RETURN
}

template <class VALUE_TYPE, size_t SIZE1, size_t SIZE2>
bool operator!=(const array<VALUE_TYPE, SIZE1>& lhs,
                const array<VALUE_TYPE, SIZE2>& rhs)
{
    if (SIZE1 != SIZE2)
        return true;                                                  // RETURN
    return !(lhs == rhs);
}

template <class VALUE_TYPE, size_t SIZE1, size_t SIZE2>
bool operator<(const array<VALUE_TYPE, SIZE1>& lhs,
               const array<VALUE_TYPE, SIZE2>& rhs)
{
    typename array<VALUE_TYPE, SIZE1>::const_iterator first1 = lhs.begin();
    typename array<VALUE_TYPE, SIZE1>::const_iterator last1  = lhs.end();
    typename array<VALUE_TYPE, SIZE1>::const_iterator first2 = rhs.begin();
    typename array<VALUE_TYPE, SIZE1>::const_iterator last2  = rhs.end();
    while (first1 != last1) {
        if (first1 == last1 || *first2 < *first1)
            return false;                                             // RETURN
        if (*first1 < *first2)
            return true;                                              // RETURN
        ++first1;
        ++first2;
    }
    if (first2 == last2)
        return false;                                                 // RETURN
    return true;                                                      // RETURN
}

template <class VALUE_TYPE, size_t SIZE1, size_t SIZE2>
bool operator>(const array<VALUE_TYPE, SIZE1>& lhs,
               const array<VALUE_TYPE, SIZE2>& rhs)
{
    return rhs < lhs;
}

template <class VALUE_TYPE, size_t SIZE1, size_t SIZE2>
bool operator<=(const array<VALUE_TYPE, SIZE1>& lhs,
                const array<VALUE_TYPE, SIZE2>& rhs)
{
    return !(rhs < lhs);
}

template <class VALUE_TYPE, size_t SIZE1, size_t SIZE2>
bool operator>=(const array<VALUE_TYPE, SIZE1>& lhs,
                const array<VALUE_TYPE, SIZE2>& rhs)
{
    return !(lhs < rhs);
}

template <class VALUE_TYPE, size_t SIZE1, size_t SIZE2>
void swap(array<VALUE_TYPE, SIZE1>& lhs, array<VALUE_TYPE, SIZE2>& rhs)
{
    lhs.swap(rhs);
}

}  // close namespace bsl

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
