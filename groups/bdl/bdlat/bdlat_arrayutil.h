// bdlat_arrayutil.h                                                  -*-C++-*-
#ifndef INCLUDED_BDLAT_ARRAYUTIL
#define INCLUDED_BDLAT_ARRAYUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide utilities for operating on 'bdlat' "array" types.
//
//@CLASSES:
//  bdlat::ArrayUtil: namespace for utility functions on "array" types
//
//@SEE_ALSO: bdlat_arrayfunctions, bdlat_typecategory
//
//@DESCRIPTION: This component provides a utility 'struct', 'bdlat::ArrayUtil',
// which serves as a namespace for a collection of function templates providing
// derived operations for "array" types.  See {'bdlat_arrayfunctions'}
// for the set of requirements of "array" types in the 'bdlat' framework.  See
// {'bdlat_typecategory'} for more general information about this framework.
//
///Primitive and Derived Functions of Arrays
///-----------------------------------------
// In order to be "plugged in" to the 'bdlat' framework as an "array", a type
// must meet a set of requirements including providing certain function
// overloads (customization points) and specifying certain type traits, as
// specified by the {'bdlat_arrayfunctions'} component.  We call the required
// function overloads the "primitive" operations of "array" types.  This
// component provides "derived" operations, which are operations that are
// exclusively defined in terms of primitive operations, and as such can be
// used with any "array" type.
//
///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Accessing an Array Element And Its Category
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we would like to define a function that detects whether an element
// of an array is itself an array, in order to more generally detect nested
// arrays.
//
// First, we need to define an accessor functor per
// {'bdlat_typecategory'|'ACCESSOR' Functors} that will be used to detect
// whether an array element is itself an array:
//..
//  class MyArrayDetector {
//      // DATA
//      bool d_didVisitArray;
//
//    public:
//      // CREATORS
//      MyArrayDetector()
//      : d_didVisitArray(false)
//      {
//      }
//
//      // MANIPULATORS
//      template <class TYPE>
//      int operator()(const TYPE& object, bdlat_TypeCategory::Array)
//      {
//          d_didVisitArray = true;
//          return 0;
//      }
//
//      template <class TYPE, class OTHER_CATEGORY>
//      int operator()(const TYPE&, OTHER_CATEGORY)
//      {
//          d_didVisitArray = false;
//          return 0;
//      }
//
//      // ACCESSORS
//      bool didVisitArray()
//      {
//          return d_didVisitArray;
//      }
//  };
//..
// Then, we can define a utility 'struct', 'MyArrayUtil', that provides a
// function for detecting whether or not an array has an element that is itself
// an array:
//..
//  struct MyArrayUtil {
//
//      // CLASS METHODS
//      template <class TYPE>
//      static int isElementAnArray(bool        *isArray,
//                                  const TYPE&  array,
//                                  int          index)
//          // Load the value 'true' to the specified 'isArray' if the element
//          // at the specified 'index' of the specified 'array' has the
//          // "array" type category, and load the value 'false' otherwise.
//          // Return 0 on success, and a non-zero value otherwise.  If a
//          // non-zero value is returned, the value loaded to 'isArray' is
//          // unspecified.  The behavior is undefined unless the specified
//          // 'array' has the "array" type category, '0 <= index', and
//          // 'index < bdlat_ArrayFunctions::size(array)'.
//      {
//          BSLS_ASSERT(bdlat_TypeCategoryFunctions::select(array) ==
//                      bdlat_TypeCategory::e_ARRAY_CATEGORY);
//          BSLS_ASSERT(0 <= index);
//          BSLS_ASSERT(static_cast<bsl::size_t>(index) <
//                      bdlat_ArrayFunctions::size(array));
//
//          MyArrayDetector detector;
//          int rc = bdlat::ArrayUtil::accessElementByCategory(array,
//                                                             detector,
//                                                             index);
//          if (0 != rc) {
//              return -1;                                            // RETURN
//          }
//
//          *isArray = detector.didVisitArray();
//          return 0;
//      }
//  };
//..
// Finally, we can use this utility to detect whether elements of array types
// are themselves arrays:
//..
//  void example()
//  {
//      bsl::vector<int> vectorA;
//      vectorA.push_back(42);
//
//      bool isArray = false;
//      int rc = MyArrayUtil::isElementAnArray(&isArray, vectorA, 0);
//
//      assert(0 == rc);
//      assert(! isArray);
//
//      bsl::vector<bsl::vector<int> > vectorB;
//      vectorB.push_back(bsl::vector<int>());
//
//      rc = MyArrayUtil::isElementAnArray(&isArray, vectorB, 0);
//
//      assert(0 == rc);
//      assert(isArray);
//  }
//..

#include <bdlscm_version.h>

#include <bdlat_arrayfunctions.h>
#include <bdlat_typecategory.h>

#include <bslmf_assert.h>

#include <bsls_assert.h>
#include <bsls_platform.h>

#include <bsl_cstddef.h>

namespace BloombergLP {
namespace bdlat {

                              // ================
                              // struct ArrayUtil
                              // ================

struct ArrayUtil {
    // This 'struct' provides a namespace for suite of function templates
    // providing non-primitive operations on "array" types.

  private:
    // PRIVATE TYPES
    template <class ACCESSOR>
    class AccessByCategoryAdapter;
        // This private class provides a function-object type that adapts a
        // (categorized) accessor functor to an uncategorized accessor functor.
        // For the definition of an accessor functor, see
        // {'bdlat_typecategory'|'ACCESSOR' Functors}.  An uncategorized
        // accessor functor is one that does not take a second, 'category',
        // argument, such as a functor that may be passed to
        // 'bdlat_ArrayFunctions::accessElement', for example.

    template <class MANIPULATOR>
    class ManipulateByCategoryAdapter;
        // This private class provides a function-object type that adapts a
        // (categorized) manipulator functor to an uncategorized manipulator
        // functor.  For the definition of a manipulator functor, see
        // {'bdlat_typecategory'|'MANIPULATOR' Functors}.  An uncategorized
        // manipulator functor is one that does not take a second, 'category',
        // argument, such as a functor that may be passed to
        // 'bdlat_ArrayFunctions::manipulateElement', for example.

  public:
    // CLASS METHODS
    template <class TYPE, class ACCESSOR>
    static int accessElementByCategory(const TYPE& array,
                                       ACCESSOR&   accessor,
                                       int         index);
        // Invoke the specified 'accessor' on the non-modifiable element at the
        // specified 'index' of the specified 'array' and on a prvalue of the
        // category tag type for the dynamic category of the element.  See
        // {'bdlat_typecategory'|Category Tags and Enumerators} for
        // documentation about category tags.  Return the value from the
        // invocation of 'accessor'.  The accessor must be an accessor functor.
        // See {'bdlat_typecategory'|'ACCESSOR' Functors} for the requirements
        // on 'accessor'.  The behavior is undefined unless
        // '0 <= index < bdlat_ArrayFunctions::size(array)'.

    template <class TYPE, class MANIPULATOR>
    static int manipulateElementByCategory(TYPE         *array,
                                           MANIPULATOR&  manipulator,
                                           int           index);
        // Invoke the specified 'manipulator' on the address of the element at
        // the specified 'index' of the specified 'array' and on a prvalue of
        // the category tag type for the dynamic category of the element.  See
        // {'bdlat_typecategory'|Category Tags and Enumerators} for
        // documentation about category tags.  Return the value from the
        // invocation of 'manipulator'.  The 'manipulator' must be a
        // manipulator functor.  See
        // {'bdlat_typecategory'|'MANIPULATOR' Functors} for the requirements
        // on 'manipulator'.  The behavior is undefined unless
        // '0 <= index < bdlat_ArrayFunctions::size(array)'.
};

                  // ========================================
                  // class ArrayUtil::AccessByCategoryAdapter
                  // ========================================

template <class ACCESSOR>
class ArrayUtil::AccessByCategoryAdapter {
    // See the class-level documentation of 'ArrayUtil' for the description of
    // this component-private class template.

    // DATA
    ACCESSOR *d_accessor_p;
        // The 'accessor' attribute of this object

  public:
    // CREATORS
    explicit AccessByCategoryAdapter(ACCESSOR *accessor);
        // Create an 'AccessByCategoryAdapter' object having the specified
        // 'accessor' attribute value.

    // ACCESSORS
    template <class VALUE_TYPE>
    int operator()(const VALUE_TYPE& value) const;
        // Invoke the 'accessor' of this object with the specified 'value' and
        // a prvalue of the category tag type for its dynamic category.  Return
        // the value from the invocation of the 'accessor'.
};

                // ============================================
                // class ArrayUtil::ManipulateByCategoryAdapter
                // ============================================

template <class MANIPULATOR>
class ArrayUtil::ManipulateByCategoryAdapter {
    // See the class-level documentation of 'ArrayUtil' for the description of
    // this component-private class template.

    // DATA
    MANIPULATOR *d_manipulator_p;
        // The 'manipulator' attribute of this object.

  public:
    // CREATORS
    explicit ManipulateByCategoryAdapter(MANIPULATOR *manipulator);
        // Create a 'ManipulateByCategory' object having the specified
        // 'manipulator' attribute value.

    // ACCESSORS
    template <class VALUE_TYPE>
    int operator()(VALUE_TYPE *value) const;
        // Invoke the 'manipulator' of this object with the specified 'value'
        // and a prvalue of the category tag type for its dynamic category.
        // Return the value from the invocation of the 'manipulator'.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                              // ----------------
                              // struct ArrayUtil
                              // ----------------

// CLASS METHODS
template <class TYPE, class ACCESSOR>
inline
int ArrayUtil::accessElementByCategory(const TYPE& array,
                                       ACCESSOR&   accessor,
                                       int         index)
{
#if !defined(BSLS_PLATFORM_CMP_SUN)
    BSLMF_ASSERT((bdlat_ArrayFunctions::IsArray<TYPE>::VALUE));
#endif
    BSLS_ASSERT(bdlat_TypeCategoryFunctions::select(array) ==
                bdlat_TypeCategory::e_ARRAY_CATEGORY);
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(static_cast<bsl::size_t>(index) <
                bdlat_ArrayFunctions::size(array));

    const AccessByCategoryAdapter<ACCESSOR> adapter(&accessor);
    return bdlat_ArrayFunctions::accessElement(array, adapter, index);
}

template <class TYPE, class MANIPULATOR>
inline
int ArrayUtil::manipulateElementByCategory(TYPE         *array,
                                           MANIPULATOR&  manipulator,
                                           int           index)
{
#if !defined(BSLS_PLATFORM_CMP_SUN)
    BSLMF_ASSERT((bdlat_ArrayFunctions::IsArray<TYPE>::VALUE));
#endif
    BSLS_ASSERT(array);
    BSLS_ASSERT(bdlat_TypeCategoryFunctions::select(*array) ==
                bdlat_TypeCategory::e_ARRAY_CATEGORY);
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(static_cast<bsl::size_t>(index) <
                bdlat_ArrayFunctions::size(*array));

    const ManipulateByCategoryAdapter<MANIPULATOR> adapter(&manipulator);
    return bdlat_ArrayFunctions::manipulateElement(array, adapter, index);
}

                  // ----------------------------------------
                  // class ArrayUtil::AccessByCategoryAdapter
                  // ----------------------------------------

// CREATORS
template <class ACCESSOR>
inline
ArrayUtil::AccessByCategoryAdapter<ACCESSOR>::AccessByCategoryAdapter(
                                                            ACCESSOR *accessor)
: d_accessor_p(accessor)
{
}

// ACCESSORS
template <class ACCESSOR>
template <class VALUE_TYPE>
inline
int ArrayUtil::AccessByCategoryAdapter<ACCESSOR>::operator()(
                                                 const VALUE_TYPE& value) const
{
    return bdlat_TypeCategoryUtil::accessByCategory(value, *d_accessor_p);
}

                // --------------------------------------------
                // class ArrayUtil::ManipulateByCategoryAdapter
                // --------------------------------------------

// CREATORS
template <class MANIPULATOR>
inline
ArrayUtil::ManipulateByCategoryAdapter<
    MANIPULATOR>::ManipulateByCategoryAdapter(MANIPULATOR *manipulator)
: d_manipulator_p(manipulator)
{
}

template <class MANIPULATOR>
template <class VALUE_TYPE>
inline
int ArrayUtil::ManipulateByCategoryAdapter<MANIPULATOR>::operator()(
                                                       VALUE_TYPE *value) const
{
    return bdlat_TypeCategoryUtil::manipulateByCategory(value,
                                                        *d_manipulator_p);
}

}  // close package namespace
}  // close enterprise namespace

#endif  // INCLUDED_BDLAT_ARRAYUTIL

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
