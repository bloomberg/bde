// bdlat_arrayiterators.h                                             -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLAT_ARRAYITERATORS
#define INCLUDED_BDLAT_ARRAYITERATORS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide iterator support for bdlat_ArrayFunction-conformant types.
//
//@CLASSES:
//   bdlat_ArrayIterators::BackInsertIterator: class for appending to arrays
//
//@SEE_ALSO: bdlat_arrayfunctions
//
//@DESCRIPTION: This component provides a namespace 'bdlat_ArrayIterators' that
// contains definitions for the 'bdlat_ArrayIterators::BackInsertIterator'
// class template and the 'backInserter' convenience function.  Additional
// iterator types may be added in the future.
//
// 'BackInsertIterator<ARRAY_TYPE>' is an iterator type which, when used in an
// expression like "*i++ = v", appends the value 'v' to the end of the
// 'ARRAY_TYPE' object used to construct the iterator, 'i'.  It meets the
// requirements of an STL output iterator and it can be instantiated for any
// type that meets the requirements described in 'bdlat_arrayfunctions'.
// 'BackInsertIterator' is similar to the standard 'bsl::back_insert_iterator'
// class template, which works for STL sequence containers.
//
// The 'backInserter' function template takes a parameter of type
// pointer-to-'ARRAY_TYPE', where 'ARRAY_TYPE' is a type that conforms to the
// interface described in the 'bdlat_arrayfunctions' component, and returns an
// object of type 'BackInsertIterator<ARRAY_TYPE>'.  It is a convenience
// function for creating a 'BackInsertIterator' without declaring its exact
// type.  The 'backInserter' function is similar to the standard
// 'bsl::back_inserter' template function, which works for STL sequence
// containers.  In fact, 'backInserter' is specialized for 'bsl::vector' so
// that it returns an 'bsl::back_insert_iterator', just like
// 'bsl::back_inserter' does.
//
///Thread Safety
///-------------
// A 'BackInsertIterator' contains a pointer to an array object and multiple
// 'backInsertIterator' objects may point to the same array object.  It is
// safe to access or modify two 'BackInsertIterator' objects simultaneously,
// each from a separate thread, if they each refer to a different array
// object.  It is safe to access a single 'BackInsertIterator' object
// simultaneously from two or more separate threads, provided no other thread
// is simultaneously modifying the iterator or its referenced array.  It is
// not safe to access or modify a 'BackInsertIterator' object in one thread
// while another thread modifies the same iterator, its referenced array
// object, or another iterator referring to the same array.
//
///Usage
///-----
// To use the facilities in this component, you must of course include the
// header file:
//..
//  #include <bdlat_arrayiterators.h>
//..
// The main use of the facilities in this component is for creating generic
// algorithms.  The following generic function appends a few integers to the
// end of an object of type 'ARRAY' that adheres to the 'bdlat_ArrayFunctions'
// interface.  It starts by creating a 'BackInsertIterator':
//..
//  template <typename ARRAY>
//  void appendSome(ARRAY *arrayObj)
//  {
//      bdlat_ArrayIterators::BackInsertIterator<ARRAY> it(arrayObj);
//..
// Now, using the "*i++ = v" idiom, append the numbers 5 and 4 to the array
// object:
//..
//      *it++ = 5;
//      *it++ = 4;
//..
// Alternatively, one can use the iterator in a standard algorithm.  For
// example, the following code appends the numbers 3, 2, and 1 to the array
// object:
//..
//      const int VALUES[] = { 3, 2, 1 };
//      const int NUM_VALUES = sizeof(VALUES) / sizeof(VALUES[0]);
//      bsl::copy(VALUES, VALUES + NUM_VALUES, it);
//  }
//..
// An alternative implementation of 'appendSome' would use 'backInserter' to
// create an iterator without declaring its exact type.  Note that, in this
// case, we do not create a variable 'it', but simply pass the iterator to a
// standard algorithm:
//..
//  template <typename ARRAY>
//  void appendSome2(ARRAY *arrayObj)
//  {
//      const int VALUES[] = { 5, 4, 3, 2, 1 };
//      const int NUM_VALUES = sizeof(VALUES) / sizeof(VALUES[0]);
//      bsl::copy(VALUES, VALUES + NUM_VALUES,
//                bdlat_ArrayIterators::backInserter(arrayObj));
//  }
//..
// In our main program, we need to construct an array that adheres to the
// 'bdlat_arrayfunctions' interface:
//..
//  #include <vector>
//
//  int main()
//  {
//      typedef bsl::vector<int> my_IntArrayType;
//..
// The result of calling 'appendSome' is that the elements 5, 4, 3, 2 and 1
// are appended to the array:
//..
//      my_IntArrayType array1;
//      appendSome(&array1);
//      assert(5 == array1[0]);
//      assert(4 == array1[1]);
//      assert(3 == array1[2]);
//      assert(2 == array1[3]);
//      assert(1 == array1[4]);
//..
// The result of calling 'appendSome2' is the same:
//..
//      my_IntArrayType array2;
//      appendSome2(&array2);
//      assert(array2 == array1);
//
//      return 0;
//  }
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLAT_ARRAYFUNCTIONS
#include <bdlat_arrayfunctions.h>
#endif

#ifndef INCLUDED_BDLAT_BDEATOVERRIDES
#include <bdlat_bdeatoverrides.h>
#endif

#ifndef INCLUDED_BDLAT_VALUETYPEFUNCTIONS
#include <bdlat_valuetypefunctions.h>
#endif

#ifndef INCLUDED_BSL_ITERATOR
#include <bsl_iterator.h>
#endif


namespace BloombergLP {

                        // ==============================
                        // namespace bdlat_ArrayIterators
                        // ==============================

namespace bdlat_ArrayIterators {

                          // ========================
                          // class BackInsertIterator
                          // ========================

template <class TYPE>
class BackInsertIterator
        : public bsl::iterator<
                        bsl::output_iterator_tag,
                        typename bdlat_ArrayFunctions::ElementType<TYPE>::Type,
                        void, void, void> {
    // TBD doc

  public:
    // TYPES
    typedef typename bdlat_ArrayFunctions::ElementType<TYPE>::Type value_type;

  private:
    // Random-access iterator for any type that meets the requirements of a
    // 'bdlat' array type.
    TYPE* d_array;

    template <class ELEM_TYPE>
    struct ValueSetter {
        // Manipulator to set the value of a newly-inserted element.
        const ELEM_TYPE *d_value;

      public:
        ValueSetter(const ELEM_TYPE* value) : d_value(value) { }
        int operator()(value_type* element) {
            bdlat_ValueTypeFunctions::assign(element, *d_value);
            return 0;
        }
    };

  public:
    // CREATORS
    BackInsertIterator(TYPE* array);
        // Construct a back-insertion iterator to manipulate the specified
        // 'array'.

#ifdef DOXYGEN // Compiler-generated functions:
    BackInsertIterator(const BackInsertIterator& other);
        // Construct a copy of the specified 'other'.

    ~BackInsertIterator();
        // Destroy this iterator
#endif

    // MANIPULATORS
    BackInsertIterator& operator=(const BackInsertIterator& rhs);
        // Assign this iterator the value of the specified 'rhs'.

    template <class ELEM_TYPE>
    BackInsertIterator& operator=(const ELEM_TYPE& obj);
        // Append the specified 'obj' to the end of the array manipulated by
        // this iterator and return this iterator.

    BackInsertIterator& operator*();
        // Do nothing and return a reference to this modifiable iterator.  This
        // function is used in generic algorithms that use the expression
        // '*i++ = v' or '*++i = v'.

    BackInsertIterator& operator++();
        // Do nothing and return a reference to this modifiable iterator.  This
        // function is used in generic algorithms that use the expression
        // '*++i = v'

    BackInsertIterator operator++(int);
        // Do nothing and return a copy of this iterator.  This function is
        // used in generic algorithms that use the expression '*i++ = v'
};

template <class TYPE>
BackInsertIterator<TYPE> backInserter(TYPE *array);
    // Return a 'BackInsertIterator' (of the appropriate type) to manipulate
    // the specified 'array'.  Specializations of this function might return a
    // different back-inserter type.

template <class TYPE, class ALLOC>
typename bsl::back_insert_iterator<bsl::vector<TYPE, ALLOC> >
backInserter(bsl::vector<TYPE, ALLOC> *array);
    // Specialization of 'backInserter' for 'bsl::vector'.  Return
    // 'bsl::back_insert_iterator instead of 'BackInsertIterator'.

}  // close namespace bdlat_ArrayIterators

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                   // ------------------------------
                   // namespace bdlat_ArrayIterators
                   // ------------------------------

                          // ------------------------
                          // class BackInsertIterator
                          // ------------------------

// CREATORS
template <class TYPE>
inline
bdlat_ArrayIterators::BackInsertIterator<TYPE>::BackInsertIterator(TYPE* array)
    : d_array(array)
{
}

// MANIPULATORS
template <class TYPE>
inline
bdlat_ArrayIterators::BackInsertIterator<TYPE>&
bdlat_ArrayIterators::BackInsertIterator<TYPE>::operator=(
    const BackInsertIterator& rhs)
{
    d_array = rhs.d_array;
    return *this;
}

template <class TYPE>
template <class ELEM_TYPE>
inline
bdlat_ArrayIterators::BackInsertIterator<TYPE>&
bdlat_ArrayIterators::BackInsertIterator<TYPE>::operator=(const ELEM_TYPE& obj)
{
    const int length = static_cast<int>(bdlat_ArrayFunctions::size(*d_array));
    bdlat_ArrayFunctions::resize(d_array, length + 1);
    ValueSetter<ELEM_TYPE> setter(&obj);
    bdlat_ArrayFunctions::manipulateElement(d_array, setter, length);

    return *this;
}

template <class TYPE>
inline
bdlat_ArrayIterators::BackInsertIterator<TYPE>&
bdlat_ArrayIterators::BackInsertIterator<TYPE>::operator*()
{
    return *this;
}

template <class TYPE>
inline
bdlat_ArrayIterators::BackInsertIterator<TYPE>&
bdlat_ArrayIterators::BackInsertIterator<TYPE>::operator++()
{
    return *this;
}

template <class TYPE>
inline
bdlat_ArrayIterators::BackInsertIterator<TYPE>
bdlat_ArrayIterators::BackInsertIterator<TYPE>::operator++(int)
{
    return *this;
}

                       // -------------------------
                       // namespace-level functions
                       // -------------------------

template <class TYPE>
inline
bdlat_ArrayIterators::BackInsertIterator<TYPE>
bdlat_ArrayIterators::backInserter(TYPE *array)
{
    return BackInsertIterator<TYPE>(array);
}

template <class TYPE, class ALLOC>
inline
typename bsl::back_insert_iterator<bsl::vector<TYPE, ALLOC> >
bdlat_ArrayIterators::backInserter(bsl::vector<TYPE, ALLOC> *array)
{
    return bsl::back_insert_iterator<bsl::vector<TYPE, ALLOC> >(*array);
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
