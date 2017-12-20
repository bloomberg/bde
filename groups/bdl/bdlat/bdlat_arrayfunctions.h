// bdlat_arrayfunctions.h                                             -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLAT_ARRAYFUNCTIONS
#define INCLUDED_BDLAT_ARRAYFUNCTIONS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a namespace defining array functions.
//
//@CLASSES:
//  bdlat_ArrayFunctions: namespace for calling array functions
//
//@SEE_ALSO:
//
// TBD: update this documentation to reflect the new overloaded functions
//
//@DESCRIPTION: The 'bdlat_ArrayFunctions' 'namespace' provided in this
// component defines parameterized functions that expose "array" behavior for
// "array" types.  See the package-level documentation for a full description
// of "array" types.  The functions in this namespace allow users to:
//..
//      o obtain the number of elements in an array ('size').
//      o set the number of elements in an array ('resize').
//      o manipulate an element in an array using a parameterized manipulator
//        ('manipulateElement').
//      o access an element in an array using a parameterized accessor
//        ('accessElement').
//..
// Also, the meta-function 'IsArray' contains a compile-time constant 'VALUE'
// that is non-zero if the parameterized 'TYPE' exposes "array" behavior
// through the 'bdlat_ArrayFunctions' 'namespace'.
//
// The 'ElementType' meta-function contains a typedef 'Type' that specifies the
// type of element stored in the parameterized "array" type.
//
// This component specializes all of these functions for 'bsl::vector<TYPE>'.
//
// Custom types can be plugged into the 'bdlat' framework.  This is done by
// overloading the 'bdlat_array*' functions inside the namespace of the plugged
// in type.  For example, suppose there is a type called 'mine::MyArray'.  In
// order to plug this type into the 'bdlat' framework as an "array", the
// following functions must be declared and implemented in the 'mine'
// namespace:
//..
//  // MANIPULATORS
//  template <typename TYPE, typename MANIPULATOR>
//  int bdlat_arrayManipulateElement(TYPE         *array,
//                                   MANIPULATOR&  manipulator,
//                                   int           index);
//      // Invoke the specified 'manipulator' on the address of the element at
//      // the specified 'index' of the specified 'array'.  Return the value
//      // from the invocation of 'manipulator'.  The behavior is undefined
//      // unless '0 <= index' and 'index < size(*array)'.
//
//  template <typename TYPE>
//  void resize(TYPE *array, int newSize);
//      // Set the size of the specified modifiable 'array' to the specified
//      // 'newSize'.  If newSize > size(array), then newSize - size(array)
//      // elements with default values are appended to 'array'.  If newSize <
//      // size(array), then the size(array) - newSize elements at the end of
//      // 'array' are destroyed.  The behavior is undefined unless
//      // '0 <= newSize'.
//
//  // ACCESSORS
//  template <typename TYPE, typename ACCESSOR>
//  int bdlat_arrayAccessElement(const TYPE& array,
//                               ACCESSOR&   accessor,
//                               int         index);
//      // Invoke the specified 'accessor' on the non-modifiable element at the
//      // specified 'index' of the specified 'array'.  Return the value from
//      // the invocation of 'accessor'.  The behavior is undefined unless
//      // '0 <= index' and 'index < size(array)'.
//
//  template <typename TYPE>
//  bsl::size_t bdlat_arraySize(const TYPE& array);
//      // Return the number of elements in the specified 'array'.
//..
// Also, the 'IsArray' meta-function must be specialized for the
// 'mine::MyArray' type in the 'bdlat_ArrayFunctions' namespace.
//
// An example of plugging in a user-defined sequence type into the 'bdlat'
// framework is shown in the 'Usage' section of the 'bdlat_SequenceFunctions'
// component.
//
///Usage
///-----
// The following snippets of code illustrate the usage of this component.
// Suppose you wanted to create a function that prints values to a specified
// output stream.  The function should also be able to expand arrays and print
// each element in the array.  We will use a stateful function object for this
// example.  First, define a 'PrintValue' function class:
//..
//  #include <bdlat_arrayfunctions.h>
//  #include <bslmf::If.h>
//
//  #include <ostream>
//
//  using namespace BloombergLP;
//
//  class PrintValue {
//      // This function will print values to the specified output stream.
//
//      // PRIVATE DATA MEMBERS
//      bsl::ostream *d_stream_p;
//..
// Define two tags, 'IsNotArrayType' and 'IsArrayType'.  These two tags will be
// used to toggle between two implementations of the 'PrintValue' function:
//..
//      // PRIVATE TYPES
//      struct IsNotArrayType { };
//      struct IsArrayType    { };
//..
// Provide two implementations of the 'PrintValue' function, one for non-array
// types and another for array types:
//..
//      // PRIVATE OPERATIONS
//      template <typename TYPE>
//      int execute(const TYPE& value, IsNotArrayType)
//      {
//          enum { SUCCESS = 0 };
//
//          (*d_stream_p) << value << ' ';
//
//          return SUCCESS;
//      }
//
//      template <typename TYPE>
//      int execute(const TYPE& value, IsArrayType)
//      {
//          enum { SUCCESS = 0, FAILURE = -1 };
//..
// For the array type implementation, first we need to obtain the number of
// elements in the array so that we can loop over them:
//..
//          int numElements = bdlat_ArrayFunctions::size(value);
//..
// Next, access each element in a loop using a reference to 'this' object as
// the parameterized accessor:
//..
//          for (int index = 0; index < numElements; ++index) {
//              if (0 != bdlat_ArrayFunctions::accessElement(value,
//                                                           *this,
//                                                           index)) {
//                  return FAILURE;
//              }
//          }
//..
// Since '*this' is used as the parameterized accessor, the 'accessElement'
// function will invoke the 'operator()' method (defined below), passing to it
// a non-modifiable reference to the element at the specified 'index'.
//
// If all elements are accessed successfully, simply return 'SUCCESS':
//..
//          return SUCCESS;
//      }
//..
// Add a constructor that takes a pointer to the output stream:
//..
//    public:
//      // CREATORS
//      PrintValue(bsl::ostream *stream)
//      : d_stream_p(stream)
//      {
//      }
//..
// Add a parameterized function call operator that toggles between array types
// and non-array types and close the definition of the 'PrintValue' function
// class:
//..
//      // OPERATIONS
//      template <typename TYPE>
//      int operator()(const TYPE& value)
//      {
//          typedef typename
//          bslmf::If<bdlat_ArrayFunctions::IsArray<TYPE>::VALUE,
//                   IsArrayType,
//                   IsNotArrayType>::Type Toggle;
//
//          return execute(value, Toggle());
//      }
//  };  // end 'class PrintValue'
//..
// The entire 'PrintValue' function class is provided below, uninterrupted, for
// clarity:
//..
//  class PrintValue {
//      // This function will print values to the specified output stream.
//
//      // PRIVATE DATA MEMBERS
//      bsl::ostream *d_stream_p;
//
//      // PRIVATE TYPES
//      struct IsNotArrayType { };
//      struct IsArrayType    { };
//
//      // PRIVATE OPERATIONS
//      template <typename TYPE>
//      int execute(const TYPE& value, IsNotArrayType)
//      {
//          enum { SUCCESS = 0 };
//
//          (*d_stream_p) << value << ' ';
//
//          return SUCCESS;
//      }
//
//      template <typename TYPE>
//      int execute(const TYPE& value, IsArrayType)
//      {
//          enum { SUCCESS = 0, FAILURE = -1 };
//
//          int numElements = bdlat_ArrayFunctions::size(value);
//
//          for (int index = 0; index < numElements; ++index) {
//              if (0 != bdlat_ArrayFunctions::accessElement(value,
//                                                           *this,
//                                                           index)) {
//                  return FAILURE;
//              }
//          }
//
//          return SUCCESS;
//      }
//
//    public:
//      // CREATORS
//      PrintValue(bsl::ostream *stream)
//      : d_stream_p(stream)
//      {
//      }
//
//      // OPERATIONS
//      template <typename TYPE>
//      int operator()(const TYPE& value)
//      {
//          typedef typename
//          bslmf::If<bdlat_ArrayFunctions::IsArray<TYPE>::VALUE,
//                   IsArrayType,
//                   IsNotArrayType>::Type Toggle;
//
//          return execute(value, Toggle());
//      }
//  };  // end 'class PrintValue'
//..
// The 'PrintValue' function class can be used for types that expose "array"
// behavior through the 'bdlat_ArrayFunctions' 'namespace' (e.g.,
// 'bsl::vector') and any other type that has 'operator<<' defined for it.  For
// example:
//..
//  #include <iostream>
//  #include <vector>
//
//  void usageExample()
//  {
//      PrintValue printValue(bsl::cout);
//
//      int intScalar = 123;
//
//      printValue(intScalar);  // expected output: '123 '
//
//      bsl::vector<int> intArray;
//      intArray.push_back(345);
//      intArray.push_back(456);
//      intArray.push_back(567);
//
//      printValue(intArray);  // expected output: '345 456 567 '
//  }
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLAT_BDEATOVERRIDES
#include <bdlat_bdeatoverrides.h>
#endif

#ifndef INCLUDED_BSLMF_METAINT
#include <bslmf_metaint.h>
#endif

#ifndef INCLUDED_BSL_CSTDDEF
#include <bsl_cstddef.h>
#endif

#ifndef INCLUDED_BSL_CSTDLIB
#include <bsl_cstdlib.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

                       // ==============================
                       // namespace bdlat_ArrayFunctions
                       // ==============================

namespace bdlat_ArrayFunctions {
    // This 'namespace' provides functions that expose "array" behavior for
    // "array" types.  Specializations are provided for 'bsl::vector<TYPE>'.
    // See the component-level documentation for more information.

    // META-FUNCTIONS
    template <class TYPE>
    struct ElementType;
        // This meta-function should contain a typedef 'Type' that specifies
        // the type of element stored in an array of the parameterized 'TYPE'.


    template <class TYPE>
    struct IsArray {
        // This 'struct' should be specialized for third-party types that are
        // need to expose "array" behavior.  See the component-level
        // documentation for further information.

        enum {
//ARB:VALUE
            VALUE = 0
        };

        typedef bslmf::MetaInt<VALUE> Type;
    };

    // MANIPULATORS
    template <class TYPE, class MANIPULATOR>
    int manipulateElement(TYPE         *array,
                          MANIPULATOR&  manipulator,
                          int           index);
        // Invoke the specified 'manipulator' on the address of the element at
        // the specified 'index' of the specified 'array'.  Return the value
        // from the invocation of 'manipulator'.  The behavior is undefined
        // unless '0 <= index' and 'index < size(*array)'.

    template <class TYPE>
    void resize(TYPE *array, int newSize);
        // Set the size of the specified modifiable 'array' to the specified
        // 'newSize'.  If 'newSize > size(array)', then 'newSize - size(array)'
        // elements with default values are appended to 'array'.  If
        // 'newSize < size(array)', then the 'size(array) - newSize' elements
        // at the end of 'array' are destroyed.  The behavior is undefined
        // unless '0 <= newSize'.

    // ACCESSORS
    template <class TYPE, class ACCESSOR>
    int accessElement(const TYPE& array,
                      ACCESSOR&   accessor,
                      int         index);
        // Invoke the specified 'accessor' on the non-modifiable element at the
        // specified 'index' of the specified 'array'.  Return the value from
        // the invocation of 'accessor'.  The behavior is undefined unless
        // '0 <= index' and 'index < size(array)'.

    template <class TYPE>
    bsl::size_t size(const TYPE& array);
        // Return the number of elements in the specified 'array'.



    // OVERLOADABLE FUNCTIONS

    // The following functions should be overloaded for other types (in their
    // respective namespaces).  The following functions are the default
    // implementations (for 'bas_codegen.pl'-generated types).  Do *not* call
    // these functions directly.  Use the functions above instead.
    //
    // Also note that the following functions are commented out.  They are just
    // included here for reference, and so that users can copy-paste the
    // required functions easily.

#if 0
    // MANIPULATORS
    template <typename TYPE, typename MANIPULATOR>
    int bdlat_arrayManipulateElement(TYPE         *array,
                                     MANIPULATOR&  manipulator,
                                     int           index);
    template <typename TYPE>
    void bdlat_arrayResize(TYPE *array, int newSize);

    // ACCESSORS
    template <typename TYPE, typename ACCESSOR>
    int bdlat_arrayAccessElement(const TYPE& array,
                                 ACCESSOR&   accessor,
                                 int         index);
    template <typename TYPE>
    bsl::size_t bdlat_arraySize(const TYPE& array);
        // Return the number of elements in the specified 'array'.
#endif

}  // close namespace bdlat_ArrayFunctions

                        // ===========================
                        // bsl::vector specializations
                        // ===========================

namespace bdlat_ArrayFunctions {

    // META-FUNCTIONS
    template <class TYPE, class ALLOC>
    struct IsArray<bsl::vector<TYPE, ALLOC> > : bslmf::MetaInt<1> {
    };

    template <class TYPE, class ALLOC>
    struct ElementType<bsl::vector<TYPE, ALLOC> > {
        typedef TYPE Type;
    };

    // MANIPULATORS
    template <class TYPE, class ALLOC, class MANIPULATOR>
    int bdlat_arrayManipulateElement(bsl::vector<TYPE, ALLOC> *array,
                                     MANIPULATOR&              manipulator,
                                     int                       index);

    template <class TYPE, class ALLOC>
    void bdlat_arrayResize(bsl::vector<TYPE, ALLOC> *array, int newSize);

    // ACCESSORS
    template <class TYPE, class ALLOC, class ACCESSOR>
    int bdlat_arrayAccessElement(const bsl::vector<TYPE, ALLOC>& array,
                                 ACCESSOR&                       accessor,
                                 int                             index);

    template <class TYPE, class ALLOC>
    bsl::size_t bdlat_arraySize(const bsl::vector<TYPE, ALLOC>& array);

}  // close namespace bdlat_ArrayFunctions

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                       // -------------------------
                       // namespace-level functions
                       // -------------------------

// MANIPULATORS

template <class TYPE, class MANIPULATOR>
inline
int bdlat_ArrayFunctions::manipulateElement(TYPE         *array,
                                            MANIPULATOR&  manipulator,
                                            int           index)
{
    return bdlat_arrayManipulateElement(array, manipulator, index);
}

template <class TYPE>
inline
void bdlat_ArrayFunctions::resize(TYPE *array, int newSize)
{
    bdlat_arrayResize(array, newSize);
}

// ACCESSORS

template <class TYPE, class ACCESSOR>
inline
int bdlat_ArrayFunctions::accessElement(const TYPE& array,
                                        ACCESSOR&   accessor,
                                        int         index)
{
    return bdlat_arrayAccessElement(array, accessor, index);
}

template <class TYPE>
inline
bsl::size_t bdlat_ArrayFunctions::size(const TYPE& array)
{
    return bdlat_arraySize(array);
}


                        // ---------------------------
                        // bsl::vector specializations
                        // ---------------------------

// MANIPULATORS

template <class TYPE, class ALLOC, class MANIPULATOR>
inline
int bdlat_ArrayFunctions::bdlat_arrayManipulateElement(
                                         bsl::vector<TYPE, ALLOC> *array,
                                         MANIPULATOR&              manipulator,
                                         int                       index)
{
    TYPE& element = (*array)[index];
    return manipulator(&element);
}

template <class TYPE, class ALLOC>
inline
void bdlat_ArrayFunctions::bdlat_arrayResize(bsl::vector<TYPE, ALLOC> *array,
                                             int                       newSize)
{
    array->resize(newSize);
}

// ACCESSORS

template <class TYPE, class ALLOC, class ACCESSOR>
inline
int bdlat_ArrayFunctions::bdlat_arrayAccessElement(
                                      const bsl::vector<TYPE, ALLOC>& array,
                                      ACCESSOR&                       accessor,
                                      int                             index)
{
    return accessor(array[index]);
}

template <class TYPE, class ALLOC>
inline
bsl::size_t bdlat_ArrayFunctions::bdlat_arraySize(
                                         const bsl::vector<TYPE, ALLOC>& array)
{
    return array.size();
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
