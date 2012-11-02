// bdeat_nullablevaluefunctions.h                  -*-C++-*-
#ifndef INCLUDED_BDEAT_NULLABLEVALUEFUNCTIONS
#define INCLUDED_BDEAT_NULLABLEVALUEFUNCTIONS

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a namespace defining nullable value functions.
//
//@CLASSES:
//  bdeat_NullableValueFunctions: namespace for nullable value functions
//
//@SEE_ALSO: bdeut_nullablevalue
//
//@AUTHOR: Shezan Baig (sbaig)
//
//@CONTACT: Rohan Bhindwale (rbhindwa)
//
// TBD: update this documentation to reflect the new overloaded functions
//
//@DESCRIPTION: The 'bdeat_NullableValueFunctions' 'namespace' provided in this
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
// behavior through the 'bdeat_NullableValueFunctions' 'namespace'.
//
// The 'ValueType' meta-function contains a typedef 'Type' that specifies the
// type of the value that can be stored in the parameterized "nullable" type.
//
// This component specializes all of these functions for
// 'bdeut_NullableValue<TYPE>'.
//
// Other "nullable" types may expose "nullable" behavior by overloading the
// 'bdeat_nullableValue*' functions inside the namespace of the plugged in
// type.  For example, suppose there is a type called 'mine::MyNullable'.  In
// order to plug this type into the 'bdeat' framework as a "nullable", the
// following functions must be declared and implemented in the 'mine'
// namespace:
//..
//  // MANIPULATORS
//  template <typename TYPE>
//  void bdeat_nullableValueMakeValue(TYPE *object);
//      // Assign to the specified "nullable" 'object' the default value for
//      // the contained type.
//
//  template <typename TYPE, typename MANIPULATOR>
//  int bdeat_nullableValueManipulateValue(TYPE         *object,
//                                         MANIPULATOR&  manipulator);
//      // Invoke the specified 'manipulator' on the address of the value
//      // stored in the specified "nullable" 'object'.  Return the value from
//      // the invocation of 'manipulator'.  The behavior is undefined unless
//      // 'object' does not contain a null value.
//
//  // ACCESSORS
//  template <typename TYPE, typename ACCESSOR>
//  int bdeat_nullableValueAccessValue(const TYPE& object,
//                                     ACCESSOR&   accessor);
//      // Invoke the specified 'accessor' on the non-modifiable value stored
//      // in the specified "nullable" 'object'.  Return the value from the
//      // invocation of 'accessor'.  The behavior is undefined unless 'object'
//      // does not contain a null value.
//
//  template <typename TYPE>
//  bool bdeat_nullableValueIsNull(const TYPE& object);
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
//  #include <bdeat_nullablevaluefunctions.h>
//  #include <bslmf_If.h>
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
//      template <typename TYPE>
//      int execute(const TYPE& value, IsNotNullableValueType)
//      {
//          enum { SUCCESS = 0 };
//
//          (*d_stream_p) << value;
//
//          return SUCCESS;
//      }
//
//      template <typename TYPE>
//      int execute(const TYPE& value, IsNullableValueType)
//      {
//          enum { SUCCESS = 0 };
//..
// For the nullable type implementation, first we need to check if the value is
// null.  If so, simply print 'NULL' and return 'SUCCESS':
//..
//          if (bdeat_NullableValueFunctions::isNull(value)) {
//              (*d_stream_p) << "NULL";
//
//              return SUCCESS;
//          }
//..
// Otherwise, since the value is not null, we will access the value using a
// reference to 'this' object as the parameterized accessor:
//..
//          return bdeat_NullableValueFunctions::accessValue(value, *this);
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
//      template <typename TYPE>
//      int operator()(const TYPE& value)
//      {
//          typedef typename
//          bslmf_If<
//              bdeat_NullableValueFunctions::IsNullableValueType<TYPE>::VALUE,
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
//      template <typename TYPE>
//      int execute(const TYPE& value, IsNotNullableValueType)
//      {
//          enum { SUCCESS = 0 };
//
//          (*d_stream_p) << value;
//
//          return SUCCESS;
//      }
//
//      template <typename TYPE>
//      int execute(const TYPE& value, IsNullableValueType)
//      {
//          enum { SUCCESS = 0 };
//
//          if (bdeat_NullableValueFunctions::isNull(value)) {
//              (*d_stream_p) << "NULL";
//
//              return SUCCESS;
//          }
//
//          return bdeat_NullableValueFunctions::accessValue(value, *this);
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
//          bslmf_If<
//              bdeat_NullableValueFunctions::IsNullableValue<TYPE>::VALUE,
//              IsNullableValueType,
//              IsNotNullableValueType>::Type Toggle;
//
//          return execute(value, Toggle());
//      }
//  };  // end 'class PrintValue'
//..
// The 'PrintValue' function class can be used for types that expose "nullable"
// behavior through the 'bdeat_NullableValueFunctions' 'namespace' (e.g.,
// 'bdeut_NullableValue') and any other type that has 'operator<<' defined for
// it.  For example:
//..
//  #include <iostream>
//  #include <bdeut_nullablevalue.h>
//
//  void usageExample()
//  {
//      PrintValue printValue(bsl::cout);
//
//      int intScalar = 123;
//
//      printValue(intScalar);  // expected output: '123'
//
//      bdeut_NullableValue<int> intNullable;
//
//      printValue(intNullable);  // expected output: 'NULL'
//
//      intNullable.makeValue(321);
//
//      printValue(intNullable);  // expected output: '321'
//  }
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLMF_METAINT
#include <bslmf_metaint.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BDEUT_NULLABLEVALUE
#include <bdeut_nullablevalue.h>
#endif

#ifndef INCLUDED_BDEUT_NULLABLEALLOCATEDVALUE
#include <bdeut_nullableallocatedvalue.h>
#endif

#ifndef INCLUDED_BSLMF_MATCHANYTYPE
#include <bslmf_matchanytype.h>
#endif


namespace BloombergLP {

                   // ======================================
                   // namespace bdeat_NullableValueFunctions
                   // ======================================

namespace bdeat_NullableValueFunctions {
    // This 'namespace' provides functions that expose "nullable" behavior for
    // "nullable" types.  See the component-level documentation for more
    // information.

    // META-FUNCTIONS
#ifndef BDE_OMIT_INTERNAL_DEPRECATED

    template <typename TYPE>
    bslmf_MetaInt<0> isNullableValueMetaFunction(const TYPE&);
        // This function can be overloaded to support partial specialization
        // (Sun5.2 compiler is unable to partially specialize the 'struct'
        // below).  Note that this function is has no definition and should not
        // be called at runtime.
        //
        // This function is *DEPRECATED*.  User's should specialize the
        // 'IsNullableValue' meta-function.

#endif // BDE_OMIT_INTERNAL_DEPRECATED
    template <typename TYPE>
    struct IsNullableValue {
        // This 'struct' should be specialized for third-party types that need
        // to expose "nullable" behavior.  See the component-level
        // documentation for further information.

        enum {
            VALUE = 0
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
                 || BSLMF_METAINT_TO_BOOL(isNullableValueMetaFunction(
                                                   bslmf_TypeRep<TYPE>::rep()))
#endif // BDE_OMIT_INTERNAL_DEPRECATED
        };
    };

    template <typename TYPE>
    struct ValueType;
        // This meta-function should contain a typedef 'Type' that specifies
        // the type of value stored in a nullable type of the parameterized
        // 'TYPE'.

    // MANIPULATORS
    template <typename TYPE>
    void makeValue(TYPE *object);
        // Assign to the specified "nullable" 'object' the default value for
        // the contained type.

    template <typename TYPE, typename MANIPULATOR>
    int manipulateValue(TYPE         *object,
                        MANIPULATOR&  manipulator);
        // Invoke the specified 'manipulator' on the address of the value
        // stored in the specified "nullable" 'object'.  Return the value from
        // the invocation of 'manipulator'.  The behavior is undefined unless
        // 'object' does not contain a null value.

    // ACCESSORS
    template <typename TYPE, typename ACCESSOR>
    int accessValue(const TYPE& object,
                    ACCESSOR&   accessor);
        // Invoke the specified 'accessor' on the non-modifiable value stored
        // in the specified "nullable" 'object'.  Return the value from the
        // invocation of 'accessor'.  The behavior is undefined unless 'object'
        // does not contain a null value.

    template <typename TYPE>
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
    template <typename TYPE>
    void bdeat_nullableValueMakeValue(TYPE *object);
    template <typename TYPE, typename MANIPULATOR>
    int bdeat_nullableValueManipulateValue(TYPE         *object,
                                           MANIPULATOR&  manipulator);

    // ACCESSORS
    template <typename TYPE, typename ACCESSOR>
    int bdeat_nullableValueAccessValue(const TYPE& object,
                                       ACCESSOR&   accessor);
    template <typename TYPE>
    bool bdeat_nullableValueIsNull(const TYPE& object);
#endif

}  // close namespace bdeat_NullableValueFunctions

                  // =======================================
                  // Specializations for bdeut_NullableValue
                  // =======================================

namespace bdeat_NullableValueFunctions {

    // META-FUNCTIONS
    template <typename TYPE>
    struct IsNullableValue<bdeut_NullableValue<TYPE> > {
        enum { VALUE = 1 };
    };

    template <typename TYPE>
    struct ValueType<bdeut_NullableValue<TYPE> > {
        typedef TYPE Type;
    };

    // MANIPULATORS
    template <typename TYPE>
    void bdeat_nullableValueMakeValue(bdeut_NullableValue<TYPE> *object);

    template <typename TYPE, typename MANIPULATOR>
    int bdeat_nullableValueManipulateValue(
                                       bdeut_NullableValue<TYPE> *object,
                                       MANIPULATOR&               manipulator);

    // ACCESSORS
    template <typename TYPE, typename ACCESSOR>
    int bdeat_nullableValueAccessValue(
                                    const bdeut_NullableValue<TYPE>& object,
                                    ACCESSOR&                        accessor);

    template <typename TYPE>
    bool bdeat_nullableValueIsNull(const bdeut_NullableValue<TYPE>& object);

}  // close namespace bdeat_NullableValueFunctions

              // ================================================
              // Specializations for bdeut_NullableAllocatedValue
              // ================================================

namespace bdeat_NullableValueFunctions {

    // META-FUNCTIONS
    template <typename TYPE>
    struct IsNullableValue<bdeut_NullableAllocatedValue<TYPE> > {
        enum { VALUE = 1 };
    };

    template <typename TYPE>
    struct ValueType<bdeut_NullableAllocatedValue<TYPE> > {
        typedef TYPE Type;
    };

    // MANIPULATORS
    template <typename TYPE>
    void bdeat_nullableValueMakeValue(
                                   bdeut_NullableAllocatedValue<TYPE> *object);
    template <typename TYPE, typename MANIPULATOR>
    int bdeat_nullableValueManipulateValue(
                                    bdeut_NullableAllocatedValue<TYPE> *object,
                                    MANIPULATOR&               manipulator);

    // ACCESSORS
    template <typename TYPE, typename ACCESSOR>
    int bdeat_nullableValueAccessValue(
                              const bdeut_NullableAllocatedValue<TYPE>& object,
                              ACCESSOR&                        accessor);
    template <typename TYPE>
    bool bdeat_nullableValueIsNull(
                             const bdeut_NullableAllocatedValue<TYPE>& object);

}  // close namespace bdeat_NullableValueFunctions

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                   // --------------------------------------
                   // namespace bdeat_NullableValueFunctions
                   // --------------------------------------

// MANIPULATORS

template <typename TYPE>
inline
void bdeat_NullableValueFunctions::makeValue(TYPE *object)
{
    bdeat_nullableValueMakeValue(object);
}

template <typename TYPE, typename MANIPULATOR>
inline
int bdeat_NullableValueFunctions::manipulateValue(TYPE         *object,
                                                  MANIPULATOR&  manipulator)
{
    return bdeat_nullableValueManipulateValue(object, manipulator);
}

// ACCESSORS

template <typename TYPE, typename ACCESSOR>
inline
int bdeat_NullableValueFunctions::accessValue(const TYPE& object,
                                              ACCESSOR&   accessor)
{
    return bdeat_nullableValueAccessValue(object, accessor);
}

template <typename TYPE>
inline
bool bdeat_NullableValueFunctions::isNull(const TYPE& object)
{
    return bdeat_nullableValueIsNull(object);
}

                  // ---------------------------------------
                  // Specializations for bdeut_NullableValue
                  // ---------------------------------------

// MANIPULATORS

template <typename TYPE>
inline
void bdeat_NullableValueFunctions::bdeat_nullableValueMakeValue(
                                             bdeut_NullableValue<TYPE> *object)
{
    object->makeValue();
}

template <typename TYPE, typename MANIPULATOR>
inline
int bdeat_NullableValueFunctions::bdeat_nullableValueManipulateValue(
                                        bdeut_NullableValue<TYPE> *object,
                                        MANIPULATOR&               manipulator)
{
    BSLS_ASSERT_SAFE(!object->isNull());

    return manipulator(&object->value());
}

// ACCESSORS

template <typename TYPE, typename ACCESSOR>
inline
int bdeat_NullableValueFunctions::bdeat_nullableValueAccessValue(
                                     const bdeut_NullableValue<TYPE>& object,
                                     ACCESSOR&                        accessor)
{
    BSLS_ASSERT_SAFE(!object.isNull());

    return accessor(object.value());
}

template <typename TYPE>
inline
bool bdeat_NullableValueFunctions::bdeat_nullableValueIsNull(
                                       const bdeut_NullableValue<TYPE>& object)
{
    return object.isNull();
}

              // ------------------------------------------------
              // Specializations for bdeut_NullableAllocatedValue
              // ------------------------------------------------

// MANIPULATORS

template <typename TYPE>
inline
void bdeat_NullableValueFunctions::bdeat_nullableValueMakeValue(
                                    bdeut_NullableAllocatedValue<TYPE> *object)
{
    object->makeValue();
}

template <typename TYPE, typename MANIPULATOR>
inline
int bdeat_NullableValueFunctions::bdeat_nullableValueManipulateValue(
                                    bdeut_NullableAllocatedValue<TYPE> *object,
                                    MANIPULATOR&               manipulator)
{
    BSLS_ASSERT_SAFE(!object->isNull());

    return manipulator(&object->value());
}

// ACCESSORS

template <typename TYPE, typename ACCESSOR>
inline
int bdeat_NullableValueFunctions::bdeat_nullableValueAccessValue(
                              const bdeut_NullableAllocatedValue<TYPE>& object,
                              ACCESSOR&                        accessor)
{
    BSLS_ASSERT_SAFE(!object.isNull());

    return accessor(object.value());
}

template <typename TYPE>
inline
bool bdeat_NullableValueFunctions::bdeat_nullableValueIsNull(
                              const bdeut_NullableAllocatedValue<TYPE>& object)
{
    return object.isNull();
}

}  // close namespace BloombergLP



#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
