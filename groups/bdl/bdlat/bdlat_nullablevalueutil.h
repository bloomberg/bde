// bdlat_nullablevalueutil.h                                          -*-C++-*-
#ifndef INCLUDED_BDLAT_NULLABLEVALUEUTIL
#define INCLUDED_BDLAT_NULLABLEVALUEUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide utilities for operating on 'bdlat' "nullable value" types.
//
//@CLASSES:
//  bdlat::NullableValueUtil: namespace for utility functions on nullables
//
//@SEE_ALSO: bdlat_nullablevaluefunctions, bdlat_typecategory
//
//@DESCRIPTION: This component provides a utility 'struct',
// 'bdlat::NullableValueUtil', which serves as a namespace for a collection of
// function templates providing derived operations for "nullable value" types.
// See {'bdlat_nullablevaluefunctions'} for the set of requirements of
// "nullable value" types in the 'bdlat' framework.  See {'bdlat_typecategory'}
// for more general information about this framework.
//
///Primitive and Derived Functions of Nullable Values
///--------------------------------------------------
// In order to be "plugged in" to the 'bdlat' framework as a "nullable value",
// a type must meet a set of requirements including providing certain function
// overloads (customization points) and specifying certain type traits, as
// specified by the {'bdlat_nullablevaluefunctions'} component.  We call the
// required function overloads the "primitive" operations of "nullable value"
// types.  This component provides "derived" operations, which are operations
// that are exclusively defined in terms of primitive operations, and as such
// can be used with any "nullable value" type.
//
///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Accessing the Held Value And Its Category
/// - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we would like to define a function that detects whether the value
// held by a nullable value is an array.
//
// First, we need to define an accessor functor per
// {'bdlat_typecategory'|'ACCESSOR' Functors} that will be used to detect
// whether the held value is an array:
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
// Then, we can define a utility 'struct', 'MyNullableValueUtil', that provides
// a function for detecting whether or not the held value of a nullable value
// is an array:
//..
//  struct MyNullableValueUtil {
//
//      // CLASS METHODS
//      template <class TYPE>
//      static int isValueAnArray(bool *isArray, const TYPE& object)
//          // Load the value 'true' to the specified 'isArray' if the value
//          // stored in the specified 'object' has the "array" type category,
//          // and load the value 'false' otherwise.  Return 0 on success,
//          // and a non-zero value otherwise.  If a non-zero value is
//          // returned, the value loaded to 'isArray' is unspecified.  The
//          // behavior is undefined if 'object' contains a null value.
//      {
//          BSLS_ASSERT(bdlat_TypeCategoryFunctions::select(object) ==
//                      bdlat_TypeCategory::e_NULLABLE_VALUE_CATEGORY);
//          BSLS_ASSERT(!bdlat_NullableValueFunctions::isNull(object));
//
//          MyArrayDetector detector;
//          int rc = bdlat::NullableValueUtil::accessValueByCategory(object,
//                                                                   detector);
//          if (0 != rc) {
//              return -1;                                            // RETURN
//          }
//
//          *isArray = detector.didVisitArray();
//          return 0;
//      }
//  };
//..
// Finally, we can use this utility to detect whether nullable values are
// arrays:
//..
//  void example()
//  {
//      bdlb::NullableValue<int> valueA(42);
//
//      bool isArray = false;
//      int rc = MyNullableValueUtil::isValueAnArray(&isArray, valueA);
//
//      assert(0 == rc);
//      assert(! isArray);
//
//      bdlb::NullableValue<bsl::vector<int> > valueB;
//      valueB.makeValue(bsl::vector<int>());
//
//      rc = MyNullableValueUtil::isValueAnArray(&isArray, valueB);
//
//      assert(0 == rc);
//      assert(isArray);
//  }
//..

#include <bdlscm_version.h>

#include <bdlat_nullablevaluefunctions.h>
#include <bdlat_typecategory.h>

#include <bslmf_assert.h>

#include <bsls_assert.h>
#include <bsls_platform.h>

namespace BloombergLP {
namespace bdlat {

                          // ========================
                          // struct NullableValueUtil
                          // ========================

struct NullableValueUtil {
    // This 'struct' provides a namespace for a suite of function templates
    // providing non-primitive operations on "nullable value" types.

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
        // 'bdlat_NullableValueFunctions::accessValue', for example.

    template <class MANIPULATOR>
    class ManipulateByCategoryAdapter;
        // This private class provides a function-object type that adapts a
        // (categorized) manipulator functor to an uncategorized manipulator
        // functor.  For the definition of a manipulator functor, see
        // {'bdlat_typecategory'|'MANIPULATOR' Functors}.  An uncategorized
        // manipulator functor is one that does not take a second, 'category',
        // argument, such as a functor that may be passed to
        // 'bdlat_NullableValueFunctions::manipulateValue', for example.

  public:
    // CLASS METHODS
    template <class TYPE, class ACCESSOR>
    static int accessValueByCategory(const TYPE& object, ACCESSOR& accessor);
        // Invoke the specified 'accessor' on the non-modifiable value
        // stored in the specified "nullable" 'object' and on a prvalue of
        // the category tag type for the dynamic category of the value.  See
        // {'bdlat_typecategory'|Category Tags and Enumerators} for
        // documentation about category tags.  Return the value from the
        // invocation of 'accessor'.  The 'accessor' must be an accessor
        // functor.  See {'bdlat_typecategory'|'ACCESSOR' Functors} for the
        // requirements on 'accessor'.  The behavior is undefined if 'object'
        // contains a null value.

    template <class TYPE, class MANIPULATOR>
    static int manipulateValueByCategory(TYPE         *object,
                                         MANIPULATOR&  manipulator);
        // Invoke the specified 'manipulator' on the address of the value
        // stored in the specified "nullable" 'object' and on a prvalue of
        // the category tag type for the dynamic category of the value.  See
        // {'bdlat_typecategory'|Category Tags and Enumerators} for
        // documentation about category tags.  Return the value from the
        // invocation of 'manipulator'.  The 'manipulator' must be a
        // manipulator functor.  See
        // {'bdlat_typecategory'|'MANIPULATOR' Functors}
        // for the requirements on 'manipulator'.  The behavior is undefined if
        // 'object' contains a null value.
};

              // ================================================
              // class NullableValueUtil::AccessByCategoryAdapter
              // ================================================

template <class ACCESSOR>
class NullableValueUtil::AccessByCategoryAdapter {
    // See the class-level documentation of 'NullableValueUtil' for the
    // description of this component-private class template.

    // DATA
    ACCESSOR *d_accessor_p;
        // The 'accessor' attribute of this object.

  public:
    // CREATORS
    explicit AccessByCategoryAdapter(ACCESSOR *accessor);
        // Create an 'AccessByCategoryAdapter' object having the specified
        // 'accessor' attribute.

    // ACCESSORS
    template <class VALUE_TYPE>
    int operator()(const VALUE_TYPE& value) const;
        // Invoke the 'accessor' of this object with the specified 'value' and
        // a prvalue of the category tag type for its dynamic category.  Return
        // the value from the invocation of 'accessor'.
};

            // ====================================================
            // class NullableValueUtil::ManipulateByCategoryAdapter
            // ====================================================

template <class MANIPULATOR>
class NullableValueUtil::ManipulateByCategoryAdapter {
    // See the class-level documentation of 'NullableValueUtil' for the
    // description of this component-private class template.

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

                          // -----------------------
                          // class NullableValueUtil
                          // -----------------------

// CLASS METHODS
template <class TYPE, class ACCESSOR>
inline
int NullableValueUtil::accessValueByCategory(const TYPE& object,
                                             ACCESSOR&   accessor)
{
#if !defined(BSLS_PLATFORM_CMP_SUN)
    BSLMF_ASSERT((bdlat_NullableValueFunctions::IsNullableValue<TYPE>::VALUE));
#endif
    BSLS_ASSERT(bdlat_TypeCategoryFunctions::select(object) ==
                bdlat_TypeCategory::e_NULLABLE_VALUE_CATEGORY);
    BSLS_ASSERT(!bdlat_NullableValueFunctions::isNull(object));

    const NullableValueUtil::AccessByCategoryAdapter<ACCESSOR> adapter(
                                                                    &accessor);
    return bdlat_NullableValueFunctions::accessValue(object, adapter);
}

template <class TYPE, class MANIPULATOR>
inline
int NullableValueUtil::manipulateValueByCategory(TYPE         *object,
                                                 MANIPULATOR&  manipulator)
{
#if !defined(BSLS_PLATFORM_CMP_SUN)
    BSLMF_ASSERT((bdlat_NullableValueFunctions::IsNullableValue<TYPE>::VALUE));
#endif
    BSLS_ASSERT(object);
    BSLS_ASSERT(bdlat_TypeCategoryFunctions::select(*object) ==
                bdlat_TypeCategory::e_NULLABLE_VALUE_CATEGORY);
    BSLS_ASSERT(!bdlat_NullableValueFunctions::isNull(*object));

    const NullableValueUtil::ManipulateByCategoryAdapter<MANIPULATOR> adapter(
                                                                 &manipulator);
    return bdlat_NullableValueFunctions::manipulateValue(object, adapter);
}

              // ------------------------------------------------
              // class NullableValueUtil::AccessByCategoryAdapter
              // ------------------------------------------------

// CREATORS
template <class ACCESSOR>
inline
NullableValueUtil::AccessByCategoryAdapter<ACCESSOR>::AccessByCategoryAdapter(
                                                            ACCESSOR *accessor)
: d_accessor_p(accessor)
{
}

// ACCESSORS
template <class ACCESSOR>
template <class VALUE_TYPE>
inline
int NullableValueUtil::AccessByCategoryAdapter<ACCESSOR>::operator()(
                                                 const VALUE_TYPE& value) const
{
    return bdlat_TypeCategoryUtil::accessByCategory(value, *d_accessor_p);
}

            // ----------------------------------------------------
            // class NullableValueUtil::ManipulateByCategoryAdapter
            // ----------------------------------------------------

// CREATORS
template <class MANIPULATOR>
inline
NullableValueUtil::ManipulateByCategoryAdapter<
    MANIPULATOR>::ManipulateByCategoryAdapter(MANIPULATOR *manipulator)
: d_manipulator_p(manipulator)
{
}

// ACCESSORS
template <class MANIPULATOR>
template <class VALUE_TYPE>
inline
int NullableValueUtil::ManipulateByCategoryAdapter<MANIPULATOR>::operator()(
                                                       VALUE_TYPE *value) const
{
    return bdlat_TypeCategoryUtil::manipulateByCategory(value,
                                                        *d_manipulator_p);
}

}  // close package namespace
}  // close enterprise namespace

#endif  // INCLUDED_BDLAT_NULLABLEVALUEUTIL

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
