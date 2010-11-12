// bdeat_arrayiterators.h                  -*-C++-*-
#ifndef INCLUDED_BDEAT_ARRAYITERATORS
#define INCLUDED_BDEAT_ARRAYITERATORS

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide iterator support for bdeat_ArrayFunction-conformant types.
//
//@CLASSES:
//   bdeat_ArrayIterators::BackInsertIterator
//
//@SEE_ALSO: bdeat_arrayfunctions
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION: This component provides a namespace 'bdeat_ArrayIterators'
// that contains definitions for the 'BackInsertIterator' class template and
// the 'backInserter' convenience function.  Additional iterator types may be
// added in the future.
//
// 'BackInsertIterator<ARRAY_TYPE>' is an iterator type which, when used in an
// expression like "*i++ = v", appends the value 'v' to the end of the
// 'ARRAY_TYPE' object used to construct the iterator, 'i'.  It meets the
// requirements of an STL output iterator and it can be instantiated for any
// type that meets the requirements described in 'bdeat_arrayfunctions'.
// 'BackInsertIterator' is similar to the standard 'bsl::back_insert_iterator'
// class template, which works for STL sequence containers.
//
// The 'backInserter' function template takes a parameter of type
// pointer-to-'ARRAY_TYPE', where 'ARRAY_TYPE' is a type that conforms to the
// interface described in the 'bdeat_arrayfunctions' component, and returns an
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
//  #include <bdeat_arrayiterators.h>
//..
// The main use of the facilities in this component is for creating generic
// algorithms.  The following generic function appends a few integers to the
// end of an object of type 'ARRAY' that adheres to the 'bdeat_ArrayFunctions'
// interface.  It starts by creating a 'BackInsertIterator':
//..
//  template <typename ARRAY>
//  void appendSome(ARRAY *arrayObj)
//  {
//      bdeat_ArrayIterators::BackInsertIterator<ARRAY> it(arrayObj);
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
//                bdeat_ArrayIterators::backInserter(arrayObj));
//  }
//..
// In our main program, we need to construct an array that adheres to the
// 'bdeat_arrayfunctions' interface:
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

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEAT_ARRAYFUNCTIONS
#include <bdeat_arrayfunctions.h>
#endif

#ifndef INCLUDED_BDEAT_VALUETYPEFUNCTIONS
#include <bdeat_valuetypefunctions.h>
#endif

#ifndef INCLUDED_BSL_ITERATOR
#include <bsl_iterator.h>
#endif


namespace BloombergLP {

                        // ==============================
                        // namespace bdeat_ArrayIterators
                        // ==============================

namespace bdeat_ArrayIterators {

                        // ========================
                        // class BackInsertIterator
                        // ========================

template <typename TYPE>
class BackInsertIterator
        : public bsl::iterator<
                        bsl::output_iterator_tag,
                        typename bdeat_ArrayFunctions::ElementType<TYPE>::Type,
                        void, void, void> {
    // TBD doc

  public:
    // TYPES
    typedef typename bdeat_ArrayFunctions::ElementType<TYPE>::Type value_type;

  private:
    // Random-access iterator for any type that meets the requirements of
    // a bdeat array types.
    TYPE* d_array;

    template <typename ELEM_TYPE>
    struct ValueSetter {
        // Manipulator to set the value of a newly-inserted element.
        const ELEM_TYPE *d_value;

      public:
        ValueSetter(const ELEM_TYPE* value) : d_value(value) { }
        int operator()(value_type* element) {
            bdeat_ValueTypeFunctions::assign(element, *d_value);
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

    template <typename ELEM_TYPE>
    BackInsertIterator& operator=(const ELEM_TYPE& obj);
        // Append the specified 'obj' to the end of the array manipulated
        // by this iterator and return this iterator.

    BackInsertIterator& operator*();
        // Do nothing and return a reference to this modifiable iterator.
        // This function is used in generic algorithms that use the
        // expression '*i++ = v' or '*++i = v'.

    BackInsertIterator& operator++();
        // Do nothing and return a reference to this modifiable iterator.
        // This function is used in generic algorithms that use the
        // expression '*++i = v'

    BackInsertIterator operator++(int);
        // Do nothing and return a copy of this iterator.  This function
        // is used in generic algorithms that use the expression
        // '*i++ = v'
};

template <typename TYPE>
BackInsertIterator<TYPE> backInserter(TYPE *array);
     // Return a 'BackInsertIterator' (of the appropriate type) to
     // manipulate the specified 'array'.  Specializations of this
     // function might return a different back-inserter type.

template <typename TYPE, typename ALLOC>
typename bsl::back_insert_iterator<bsl::vector<TYPE, ALLOC> >
backInserter(bsl::vector<TYPE, ALLOC> *array);
    // Specialization of 'backInserter' for 'bsl::vector'.  Return
    // 'bsl::back_insert_iterator instead of 'BackInsertIterator'.

} // close namespace bdeat_ArrayIterators

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                   // ------------------------------
                   // namespace bdeat_ArrayIterators
                   // ------------------------------

                       // ------------------------
                       // class BackInsertIterator
                       // ------------------------

// CREATORS
template <typename TYPE>
inline
bdeat_ArrayIterators::BackInsertIterator<TYPE>::BackInsertIterator(TYPE* array)
    : d_array(array)
{
}

// MANIPULATORS
template <typename TYPE>
inline
bdeat_ArrayIterators::BackInsertIterator<TYPE>&
bdeat_ArrayIterators::BackInsertIterator<TYPE>::operator=(
    const BackInsertIterator& rhs)
{
    d_array = rhs.d_array;
    return *this;
}

template <typename TYPE>
template <typename ELEM_TYPE>
inline
bdeat_ArrayIterators::BackInsertIterator<TYPE>&
bdeat_ArrayIterators::BackInsertIterator<TYPE>::operator=(const ELEM_TYPE& obj)
{
    int length = bdeat_ArrayFunctions::size(*d_array);
    bdeat_ArrayFunctions::resize(d_array, length + 1);
    ValueSetter<ELEM_TYPE> setter(&obj);
    bdeat_ArrayFunctions::manipulateElement(d_array, setter, length);

    return *this;
}

template <typename TYPE>
inline
bdeat_ArrayIterators::BackInsertIterator<TYPE>&
bdeat_ArrayIterators::BackInsertIterator<TYPE>::operator*()
{
    return *this;
}

template <typename TYPE>
inline
bdeat_ArrayIterators::BackInsertIterator<TYPE>&
bdeat_ArrayIterators::BackInsertIterator<TYPE>::operator++()
{
    return *this;
}

template <typename TYPE>
inline
bdeat_ArrayIterators::BackInsertIterator<TYPE>
bdeat_ArrayIterators::BackInsertIterator<TYPE>::operator++(int)
{
    return *this;
}

                       // -------------------------
                       // namespace-level functions
                       // -------------------------

template <typename TYPE>
inline
bdeat_ArrayIterators::BackInsertIterator<TYPE>
bdeat_ArrayIterators::backInserter(TYPE *array)
{
    return BackInsertIterator<TYPE>(array);
}

template <typename TYPE, typename ALLOC>
inline
typename bsl::back_insert_iterator<bsl::vector<TYPE, ALLOC> >
bdeat_ArrayIterators::backInserter(bsl::vector<TYPE, ALLOC> *array)
{
    return bsl::back_insert_iterator<bsl::vector<TYPE, ALLOC> >(*array);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
