// bdlat_nullablevaluefunctions.h                                     -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLAT_NULLABLEVALUEFUNCTIONS
#define INCLUDED_BDLAT_NULLABLEVALUEFUNCTIONS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a namespace defining nullable value functions.
//
//@CLASSES:
//  bdlat_NullableValueFunctions: namespace for nullable value functions
//
//@SEE_ALSO: bdlb_nullablevalue
//
// TBD: update this documentation to reflect the new overloaded functions
//
//@DESCRIPTION: The 'bdlat_NullableValueFunctions' 'namespace' provided in this
// component defines parameterized functions that expose "nullable" behavior
// for "nullable" types.  See the package-level documentation for a full
// description of "nullable" types.  The functions in this namespace allow
// users to:
//..
//      o make the nullable object contain a value ('makeValue').
//      o manipulate the value contained in a nullable object using a
//        parameterized manipulator ('manipulateValue').
//      o access the value contained in a nullable object using a parameterized
//        accessor ('accessValue').
//      o check whether the nullable object is null or not ('isNull').
//..
// Also, the meta-function 'IsNullableValue' contains a compile-time constant
// 'VALUE' that is non-zero if the parameterized 'TYPE' exposes "nullable"
// behavior through the 'bdlat_NullableValueFunctions' 'namespace'.
//
// The 'ValueType' meta-function contains a typedef 'Type' that specifies the
// type of the value that can be stored in the parameterized "nullable" type.
//
// This component specializes all of these functions for
// 'bdlb::NullableValue<TYPE>'.
//
// Other "nullable" types may expose "nullable" behavior by overloading the
// 'bdlat_nullableValue*' functions inside the namespace of the plugged in
// type.  For example, suppose there is a type called 'mine::MyNullable'.  In
// order to plug this type into the 'bdlat' framework as a "nullable", the
// following functions must be declared and implemented in the 'mine'
// namespace:
//..
//  // MANIPULATORS
//  template <class TYPE>
//  void bdlat_nullableValueMakeValue(TYPE *object);
//      // Assign to the specified "nullable" 'object' the default value for
//      // the contained type.
//
//  template <class TYPE, class MANIPULATOR>
//  int bdlat_nullableValueManipulateValue(TYPE         *object,
//                                         MANIPULATOR&  manipulator);
//      // Invoke the specified 'manipulator' on the address of the value
//      // stored in the specified "nullable" 'object'.  Return the value from
//      // the invocation of 'manipulator'.  The behavior is undefined unless
//      // 'object' does not contain a null value.
//
//  // ACCESSORS
//  template <class TYPE, class ACCESSOR>
//  int bdlat_nullableValueAccessValue(const TYPE& object,
//                                     ACCESSOR&   accessor);
//      // Invoke the specified 'accessor' on the non-modifiable value stored
//      // in the specified "nullable" 'object'.  Return the value from the
//      // invocation of 'accessor'.  The behavior is undefined unless 'object'
//      // does not contain a null value.
//
//  template <class TYPE>
//  bool bdlat_nullableValueIsNull(const TYPE& object);
//      // Return 'true' if the specified "nullable" 'object' contains a null
//      // value, and 'false' otherwise.
//..
//
///Usage
///-----
// The following snippets of code illustrate the usage of this component.
// Suppose you wanted to create a function that prints values to a specified
// output stream.  The function should also be able to handle nullable values.
// If the value is null, it should print 'NULL' otherwise it should print the
// value.  We will use a stateful function object for this example.  First,
// define a 'PrintValue' function class:
//..
//  #include <bdlat_nullablevaluefunctions.h>
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
// Define two tags, 'IsNotNullableValueType' and 'IsNullableValueType'.  These
// two tags will be used to toggle between two implementations of the
// 'PrintValue' function:
//..
//      // PRIVATE TYPES
//      struct IsNotNullableValueType { };
//      struct IsNullableValueType    { };
//..
// Provide two implementations of the 'PrintValue' function, one for
// non-nullable types and another for nullable types:
//..
//      // PRIVATE OPERATIONS
//      template <class TYPE>
//      int execute(const TYPE& value, IsNotNullableValueType)
//      {
//          enum { SUCCESS = 0 };
//
//          (*d_stream_p) << value;
//
//          return SUCCESS;
//      }
//
//      template <class TYPE>
//      int execute(const TYPE& value, IsNullableValueType)
//      {
//          enum { SUCCESS = 0 };
//..
// For the nullable type implementation, first we need to check if the value is
// null.  If so, simply print 'NULL' and return 'SUCCESS':
//..
//          if (bdlat_NullableValueFunctions::isNull(value)) {
//              (*d_stream_p) << "NULL";
//
//              return SUCCESS;
//          }
//..
// Otherwise, since the value is not null, we will access the value using a
// reference to 'this' object as the parameterized accessor:
//..
//          return bdlat_NullableValueFunctions::accessValue(value, *this);
//      }
//..
// Since '*this' is used as the parameterized accessor, the 'accessValue'
// function will invoke the 'operator()' method (defined below), passing to it
// a non-modifiable reference to the actual value.
//
// Add a constructor that takes a pointer to the output stream:
//..
//    public:
//      // CREATORS
//      PrintValue(bsl::ostream *stream)
//      : d_stream_p(stream)
//      {
//      }
//..
// Add a parameterized function call operator that toggles between nullable
// types and non-nullable types and close the definition of the 'PrintValue'
// function class:
//..
//      // OPERATIONS
//      template <class TYPE>
//      int operator()(const TYPE& value)
//      {
//          typedef typename
//          bslmf::If<
//              bdlat_NullableValueFunctions::IsNullableValueType<TYPE>::VALUE,
//              IsNullableValueType,
//              IsNotNullableValueType>::Type Toggle;
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
//      struct IsNotNullableValueType { };
//      struct IsNullableValueType    { };
//
//      // PRIVATE OPERATIONS
//      template <class TYPE>
//      int execute(const TYPE& value, IsNotNullableValueType)
//      {
//          enum { SUCCESS = 0 };
//
//          (*d_stream_p) << value;
//
//          return SUCCESS;
//      }
//
//      template <class TYPE>
//      int execute(const TYPE& value, IsNullableValueType)
//      {
//          enum { SUCCESS = 0 };
//
//          if (bdlat_NullableValueFunctions::isNull(value)) {
//              (*d_stream_p) << "NULL";
//
//              return SUCCESS;
//          }
//
//          return bdlat_NullableValueFunctions::accessValue(value, *this);
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
//      template <class TYPE>
//      int operator()(const TYPE& value)
//      {
//          typedef typename
//          bslmf::If<
//              bdlat_NullableValueFunctions::IsNullableValue<TYPE>::VALUE,
//              IsNullableValueType,
//              IsNotNullableValueType>::Type Toggle;
//
//          return execute(value, Toggle());
//      }
//  };  // end 'class PrintValue'
//..
// The 'PrintValue' function class can be used for types that expose "nullable"
// behavior through the 'bdlat_NullableValueFunctions' 'namespace' (e.g.,
// 'bdlb::NullableValue') and any other type that has 'operator<<' defined for
// it.  For example:
//..
//  #include <iostream>
//  #include <bdlb_nullablevalue.h>
//
//  void usageExample()
//  {
//      PrintValue printValue(bsl::cout);
//
//      int intScalar = 123;
//
//      printValue(intScalar);  // expected output: '123'
//
//      bdlb::NullableValue<int> intNullable;
//
//      printValue(intNullable);  // expected output: 'NULL'
//
//      intNullable.makeValue(321);
//
//      printValue(intNullable);  // expected output: '321'
//  }
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_METAINT
#include <bslmf_metaint.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BDLAT_BDEATOVERRIDES
#include <bdlat_bdeatoverrides.h>
#endif

#ifndef INCLUDED_BDLB_NULLABLEVALUE
#include <bdlb_nullablevalue.h>
#endif

#ifndef INCLUDED_BDLB_NULLABLEALLOCATEDVALUE
#include <bdlb_nullableallocatedvalue.h>
#endif

#ifndef INCLUDED_BSLMF_MATCHANYTYPE
#include <bslmf_matchanytype.h>
#endif


namespace BloombergLP {

                   // ======================================
                   // namespace bdlat_NullableValueFunctions
                   // ======================================

namespace bdlat_NullableValueFunctions {
    // This 'namespace' provides functions that expose "nullable" behavior for
    // "nullable" types.  See the component-level documentation for more
    // information.

    // META-FUNCTIONS
    template <class TYPE>
    struct IsNullableValue {
        // This 'struct' should be specialized for third-party types that need
        // to expose "nullable" behavior.  See the component-level
        // documentation for further information.

        enum {
//ARB:VALUE
            VALUE = 0
        };
    };

    template <class TYPE>
    struct ValueType;
        // This meta-function should contain a typedef 'Type' that specifies
        // the type of value stored in a nullable type of the parameterized
        // 'TYPE'.

    // MANIPULATORS
    template <class TYPE>
    void makeValue(TYPE *object);
        // Assign to the specified "nullable" 'object' the default value for
        // the contained type.

    template <class TYPE, class MANIPULATOR>
    int manipulateValue(TYPE         *object,
                        MANIPULATOR&  manipulator);
        // Invoke the specified 'manipulator' on the address of the value
        // stored in the specified "nullable" 'object'.  Return the value from
        // the invocation of 'manipulator'.  The behavior is undefined unless
        // 'object' does not contain a null value.

    // ACCESSORS
    template <class TYPE, class ACCESSOR>
    int accessValue(const TYPE& object,
                    ACCESSOR&   accessor);
        // Invoke the specified 'accessor' on the non-modifiable value stored
        // in the specified "nullable" 'object'.  Return the value from the
        // invocation of 'accessor'.  The behavior is undefined unless 'object'
        // does not contain a null value.

    template <class TYPE>
    bool isNull(const TYPE& object);
        // Return 'true' if the specified "nullable" 'object' contains a null
        // value, and 'false' otherwise.


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
    template <class TYPE>
    void bdlat_nullableValueMakeValue(TYPE *object);
    template <class TYPE, class MANIPULATOR>
    int bdlat_nullableValueManipulateValue(TYPE         *object,
                                           MANIPULATOR&  manipulator);

    // ACCESSORS
    template <class TYPE, class ACCESSOR>
    int bdlat_nullableValueAccessValue(const TYPE& object,
                                       ACCESSOR&   accessor);
    template <class TYPE>
    bool bdlat_nullableValueIsNull(const TYPE& object);
#endif

}  // close namespace bdlat_NullableValueFunctions

                  // =======================================
                  // Specializations for bdlb::NullableValue
                  // =======================================

namespace bdlat_NullableValueFunctions {

    // META-FUNCTIONS
    template <class TYPE>
    struct IsNullableValue<bdlb::NullableValue<TYPE> > {
        enum {
//ARB:VALUE
            VALUE = 1
        };
    };

    template <class TYPE>
    struct ValueType<bdlb::NullableValue<TYPE> > {
        typedef TYPE Type;
    };

    // MANIPULATORS
    template <class TYPE>
    void bdlat_nullableValueMakeValue(bdlb::NullableValue<TYPE> *object);

    template <class TYPE, class MANIPULATOR>
    int bdlat_nullableValueManipulateValue(
                                       bdlb::NullableValue<TYPE> *object,
                                       MANIPULATOR&               manipulator);

    // ACCESSORS
    template <class TYPE, class ACCESSOR>
    int bdlat_nullableValueAccessValue(
                                    const bdlb::NullableValue<TYPE>& object,
                                    ACCESSOR&                        accessor);

    template <class TYPE>
    bool bdlat_nullableValueIsNull(const bdlb::NullableValue<TYPE>& object);

}  // close namespace bdlat_NullableValueFunctions

              // ================================================
              // Specializations for bdlb::NullableAllocatedValue
              // ================================================

namespace bdlat_NullableValueFunctions {

    // META-FUNCTIONS
    template <class TYPE>
    struct IsNullableValue<bdlb::NullableAllocatedValue<TYPE> > {
        enum {
//ARB:VALUE
            VALUE = 1
        };
    };

    template <class TYPE>
    struct ValueType<bdlb::NullableAllocatedValue<TYPE> > {
        typedef TYPE Type;
    };

    // MANIPULATORS
    template <class TYPE>
    void bdlat_nullableValueMakeValue(
                                   bdlb::NullableAllocatedValue<TYPE> *object);
    template <class TYPE, class MANIPULATOR>
    int bdlat_nullableValueManipulateValue(
                              bdlb::NullableAllocatedValue<TYPE> *object,
                              MANIPULATOR&                        manipulator);

    // ACCESSORS
    template <class TYPE, class ACCESSOR>
    int bdlat_nullableValueAccessValue(
                           const bdlb::NullableAllocatedValue<TYPE>& object,
                           ACCESSOR&                                 accessor);
    template <class TYPE>
    bool bdlat_nullableValueIsNull(
                             const bdlb::NullableAllocatedValue<TYPE>& object);

}  // close namespace bdlat_NullableValueFunctions

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                   // --------------------------------------
                   // namespace bdlat_NullableValueFunctions
                   // --------------------------------------

// MANIPULATORS

template <class TYPE>
inline
void bdlat_NullableValueFunctions::makeValue(TYPE *object)
{
    bdlat_nullableValueMakeValue(object);
}

template <class TYPE, class MANIPULATOR>
inline
int bdlat_NullableValueFunctions::manipulateValue(TYPE         *object,
                                                  MANIPULATOR&  manipulator)
{
    return bdlat_nullableValueManipulateValue(object, manipulator);
}

// ACCESSORS

template <class TYPE, class ACCESSOR>
inline
int bdlat_NullableValueFunctions::accessValue(const TYPE& object,
                                              ACCESSOR&   accessor)
{
    return bdlat_nullableValueAccessValue(object, accessor);
}

template <class TYPE>
inline
bool bdlat_NullableValueFunctions::isNull(const TYPE& object)
{
    return bdlat_nullableValueIsNull(object);
}

                  // ---------------------------------------
                  // Specializations for bdlb::NullableValue
                  // ---------------------------------------

// MANIPULATORS

template <class TYPE>
inline
void bdlat_NullableValueFunctions::bdlat_nullableValueMakeValue(
                                             bdlb::NullableValue<TYPE> *object)
{
    object->makeValue();
}

template <class TYPE, class MANIPULATOR>
inline
int bdlat_NullableValueFunctions::bdlat_nullableValueManipulateValue(
                                        bdlb::NullableValue<TYPE> *object,
                                        MANIPULATOR&               manipulator)
{
    BSLS_ASSERT_SAFE(!object->isNull());

    return manipulator(&object->value());
}

// ACCESSORS

template <class TYPE, class ACCESSOR>
inline
int bdlat_NullableValueFunctions::bdlat_nullableValueAccessValue(
                                     const bdlb::NullableValue<TYPE>& object,
                                     ACCESSOR&                        accessor)
{
    BSLS_ASSERT_SAFE(!object.isNull());

    return accessor(object.value());
}

template <class TYPE>
inline
bool bdlat_NullableValueFunctions::bdlat_nullableValueIsNull(
                                       const bdlb::NullableValue<TYPE>& object)
{
    return object.isNull();
}

              // ------------------------------------------------
              // Specializations for bdlb::NullableAllocatedValue
              // ------------------------------------------------

// MANIPULATORS

template <class TYPE>
inline
void bdlat_NullableValueFunctions::bdlat_nullableValueMakeValue(
                                    bdlb::NullableAllocatedValue<TYPE> *object)
{
    object->makeValue();
}

template <class TYPE, class MANIPULATOR>
inline
int bdlat_NullableValueFunctions::bdlat_nullableValueManipulateValue(
                               bdlb::NullableAllocatedValue<TYPE> *object,
                               MANIPULATOR&                        manipulator)
{
    BSLS_ASSERT_SAFE(!object->isNull());

    return manipulator(&object->value());
}

// ACCESSORS

template <class TYPE, class ACCESSOR>
inline
int bdlat_NullableValueFunctions::bdlat_nullableValueAccessValue(
                            const bdlb::NullableAllocatedValue<TYPE>& object,
                            ACCESSOR&                                 accessor)
{
    BSLS_ASSERT_SAFE(!object.isNull());

    return accessor(object.value());
}

template <class TYPE>
inline
bool bdlat_NullableValueFunctions::bdlat_nullableValueIsNull(
                              const bdlb::NullableAllocatedValue<TYPE>& object)
{
    return object.isNull();
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
