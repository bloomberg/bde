// bdlat_nullablevaluefunctions.h                                     -*-C++-*-
#ifndef INCLUDED_BDLAT_NULLABLEVALUEFUNCTIONS
#define INCLUDED_BDLAT_NULLABLEVALUEFUNCTIONS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a namespace defining nullable value functions.
//
//@CLASSES:
//  bdlat_NullableValueFunctions: namespace for "nullable" value functions
//
//@SEE_ALSO: bdlb_nullablevalue
//
//@DESCRIPTION: The `bdlat_NullableValueFunctions` `namespace` provided in this
// component defines parameterized functions that expose "nullable" behavior
// for "nullable" types.  See the `bdlat` package-level documentation for a
// brief description of "nullable" types.
//
// The functions in this namespace allow users to:
// * make the nullable object contain a value (`makeValue`).
// * manipulate the value contained in a nullable object using a parameterized
//   manipulator functor (`manipulateValue`).
// * access the value contained in a nullable object using a parameterized
//   accessor functor (`accessValue`).
// * check whether the nullable object is null or not (`isNull`).
//
// A type becomes part of the `bdlat` "nullable" framework by creating, in the
// namespace where the type is defined, specializations of the following four
// (free) function templates:
//
// A type becomes part of the `bdlat` "nullable" framework by creating, in the
// namespace where the type is defined, overloads of the following two (free)
// functions and two (free) function templates.  Note that the placeholder
// `YOUR_TYPE` is not a template argument and should be replaced with the name
// of the type being plugged into the framework.
// ```
// // MANIPULATORS
//
// /// Assign to the specified "nullable" `object` the default value for the
// /// contained type (i.e., `ValueType()`).
// void bdlat_nullableValueMakeValue(YOUR_TYPE *object);
//
// /// Invoke the specified `manipulator` on the address of the value stored in
// /// the specified "nullable" `object`.  The supplied `manipulator` must be a
// /// callable type that can be called as if it had the following signature:
// /// ```
// /// int manipulator(VALUE_TYPE *value);
// /// ```
// /// Return the value from the invocation of `manipulator`.  The behavior is
// /// undefined if `object` contains a null value.
// template <class MANIPULATOR>
// int bdlat_nullableValueManipulateValue(YOUR_TYPE    *object,
//                                        MANIPULATOR&  manipulator);
//
// // ACCESSORS
//
// /// Invoke the specified `accessor` on a `const`-reference to the value
// /// stored in the specified "nullable" `object`.  The supplied `accessor`
// /// must be a callable type that can be called as if it had the following
// /// signature:
// /// ```
// /// int accessor(const VALUE_TYPE& value);
// /// ```
// /// Return the value from the invocation of `accessor`.  The behavior is
// /// undefined if `object` contains a null value.
// template <class ACCESSOR>
// int bdlat_nullableValueAccessValue(const YOUR_TYPE& object,
//                                    ACCESSOR&        accessor);
//
// /// Return `true` if the specified "nullable" `object` contains a null
// /// value, and `false` otherwise.
// bool bdlat_nullableValueIsNull(const YOUR_TYPE& object);
// ```
// The "nullable" type must also define two meta-functions in the
// `bdlat_NullableValueFunctions` namespace:
//
// * the meta-function `IsNullableValue` contains a compile-time constant
//   `value` that is non-zero if the parameterized `TYPE` exposes "nullable"
//   behavior.
// * the `ValueType` meta-function contains a `typedef` `Type` that specifies
//   the type of the value that can be stored in the parameterized "nullable"
//   type.
//
// Note that `bdlb::NullableValue<TYPE>` is already part of the `bldat`
// infrastructure for "nullable" types because this component also provides
// overloads of the required functions and meta-function specializations.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Defining a "Nullable" Type
///- - - - - - - - - - - - - - - - - - -
// Suppose you had a type whose value could be in a "null" state.
//
// ```
// namespace BloombergLP {
// namespace mine {
//
// struct MyNullableValue {
//
//     // DATA
//     bool d_isNull;
//     int  d_value;
//
//     // CREATORS
//     MyNullableValue()
//     {
//         d_isNull = true;
//     }
// };
//
// }  // close namespace mine
// }  // close enterprise namespace
// ```
// We can now make `mine::MyNullableValue` expose "nullable" behavior by
// implementing the necessary `bdlta_NullableValueFunctions` for
// `MyNullableValue` inside the `mine` namespace and defining the required
// meta-functions withing the `bdlat_NullableValueFunctions` namespace.
//
// First, we should forward declare all the functions that we will implement
// inside the `mine` namespace:
// ```
// namespace BloombergLP {
// namespace mine {
//
// // MANIPULATORS
//
// /// Assign to the specified "nullable" `object` the default value for the
// /// contained type (i.e., `ValueType()`).
// void bdlat_nullableValueMakeValue(MyNullableValue *object);
//
// /// Invoke the specified `manipulator` on the address of the value stored in
// /// the specified "nullable" `object`.  Return the value from the invocation
// /// of `manipulator`.  The behavior is undefined if `object` contains a null
// /// value.
// template <class MANIPULATOR>
// int bdlat_nullableValueManipulateValue(MyNullableValue *object,
//                                        MANIPULATOR&     manipulator);
//
// // ACCESSORS
//
// /// Invoke the specified `accessor` on a `const`-reference to the value
// /// stored in the specified "nullable" `object`.  Return the value from the
// /// invocation of `accessor`.  The behavior is undefined if `object`
// /// contains a null value.
// template <class ACCESSOR>
// int bdlat_nullableValueAccessValue(const MyNullableValue& object,
//                                    ACCESSOR&              accessor);
//
// /// Return `true` if the specified "nullable" `object` contains a null
// /// value, and `false` otherwise.
// bool bdlat_nullableValueIsNull(const MyNullableValue& object);
//
// }  // close namespace mine
// }  // close enterprise namespace
// ```
// Then, we will implement these functions.  Recall that the two (non-template)
// functions should be defined in some `.cpp` file, unless you choose to make
// them `inline` functions.
// ```
// namespace BloombergLP {
//
// // MANIPULATORS
// void mine::bdlat_nullableValueMakeValue(MyNullableValue *object)
// {
//     assert(object);
//
//     object->d_isNull = false;
//     object->d_value  = 0;
// }
//
// template <class MANIPULATOR>
// int mine::bdlat_nullableValueManipulateValue(MyNullableValue *object,
//                                              MANIPULATOR&     manipulator)
// {
//     assert(object);
//     assert(!object->d_isNull);
//
//     return manipulator(&object->d_value);
// }
//
// // ACCESSORS
// template <class ACCESSOR>
// int mine::bdlat_nullableValueAccessValue(const MyNullableValue& object,
//                                          ACCESSOR&              accessor)
// {
//     assert(!object.d_isNull);
//
//     return accessor(object.d_value);
// }
//
// bool mine::bdlat_nullableValueIsNull(const MyNullableValue& object)
// {
//     return object.d_isNull;
// }
//
// }  // close enterprise namespace
// ```
// Finally, we specialize the `IsNullableValue` and `ValueType` meta-functions
// in the `bdlat_NullableValueFunctions` namespace for the
// `mine::MyNullableValue` type:
// ```
// namespace BloombergLP {
// namespace bdlat_NullableValueFunctions {
//
// // TRAITS
// template <>
// struct IsNullableValue<mine::MyNullableValue> : bsl::true_type {
// };
//
// template <>
// struct ValueType<mine::MyNullableValue> {
//     typedef int Type;
// };
//
// }  // close namespace bdlat_NullableValueFunctions
// }  // close enterprise namespace
// ```
// This completes the `bdlat` infrastructure for `mine::MyNullableValue` and
// allows the generic software to recognize the type as a nullable abstraction.
//
///Example 2: Using the Infrastructure Via General Methods
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The `bdlat` "nullable" framework provides a set of fundamental operations
// common to any "nullable" type.  We can build upon these operations to make
// our own utilities, or use them on our own types that are plugged into the
// framework, like `mine::MyNullableValue`, which we created in {Example 1}.
// For example, we can use the (fundamental) operations in the
// `bdlat_NullableValueFunctions` namespace to operate on
// `mine::NullableValue`, even though they have no knowledge of that type in
// particular:
//
// Two of those operations are rather basic.  One simply informs whether or not
// an object is in the null state (the `isNull` method).  Another sets an
// object to a default, non-null state (the `makeValue` method).
// ```
// void usageMakeObject()
// {
//     BSLMF_ASSERT(bdlat_NullableValueFunctions::
//                  IsNullableValue<mine::MyNullableValue>::value);
//
//     mine::MyNullableValue object;
//     assert( bdlat_NullableValueFunctions::isNull(object));
//
//     bdlat_NullableValueFunctions::makeValue(&object);
//     assert(!bdlat_NullableValueFunctions::isNull(object));
// }
// ```
// The other two generic methods accomplish their actions via user-supplied
// functors.
//
// Let us define a generic functor that gives us access to the underlying value
// of the "nullable" type, if it's not null:
// ```
// template <class VALUE_TYPE>
// class GetValueAccessor {
//
//     // DATA
//     VALUE_TYPE *d_value_p;
//
//   public:
//     // CREATORS
//     explicit GetValueAccessor(VALUE_TYPE *value)
//     : d_value_p(value)
//     {
//     }
//
//     // MANIPULATORS
//
//     /// Assign the value of the specified `containedValue` to the object
//     /// addressed by `d_value_p`.
//     int operator()(const VALUE_TYPE& containedValue)
//     {
//         *d_value_p = containedValue;
//         return 0;
//     }
// };
// ```
// Notice that the above class makes no assumptions about the value being
// accessed other than it can be copied (in the constructor) and assigned (in
// the operator).
//
// This functor can be used to fetch the value of our nullable object:
// ```
// void usageGetValue()
// {
//     mine::MyNullableValue object;
//
//     bdlat_NullableValueFunctions::makeValue(&object);
//     assert(!bdlat_NullableValueFunctions::isNull(object));
//
//     int value;
//     GetValueAccessor<int> accessor(&value);
//
//     int rc = bdlat_NullableValueFunctions::accessValue(object, accessor);
//     assert(0 == rc);
//     assert(0 == value);
// }
// ```
// Notice that we did not invoke `accessValue` until `object` had been set to a
// non-null state.  Doing otherwise would have led to undefined behavior.
//
// Finally, let's define a functor to set the state of a nullable object:
// ```
// template <class VALUE_TYPE>
// class SetValueManipulator {
//
//     // DATA
//     VALUE_TYPE d_value;
//
//   public:
//     // CREATORS
//     explicit SetValueManipulator(const VALUE_TYPE& value)
//     : d_value(value)
//     {
//     }
//
//     // ACCESSOR
//     int operator()(VALUE_TYPE *value) const
//     {
//         *value = d_value;
//         return 0;
//     }
// };
// ```
// As with the previous functor, this functor has no knowledge of the nullable
// type to which it will be applied.  The only assumption here is that the
// value (type) of our nullable type can be copy constructed and copy assigned.
//
// Let us use this functor to modify one of our nullable objects:
// ```
// void usageSetValue()
// {
//     mine::MyNullableValue object;
//
//     bdlat_NullableValueFunctions::makeValue(&object);
//     assert(!bdlat_NullableValueFunctions::isNull(object));
//
//     SetValueManipulator<int> manipulator(42);
//     int rcm = bdlat_NullableValueFunctions::manipulateValue(&object,
//                                                             manipulator);
//     assert(0 == rcm);
//
//     // Confirm that the object was set to the expected state.
//
//     int value;
//     GetValueAccessor<int> accessor(&value);
//
//     int rca = bdlat_NullableValueFunctions::accessValue(object, accessor);
//     assert( 0 == rca);
//     assert(42 == value);
// }
// ```
//
///Example 3: Defining Utility Functions
///- - - - - - - - - - - - - - - - - - -
// Creating functor objects for each operation can be tedious and error prone;
// consequently, those types are often executed via utility functions.
//
// Suppose we want to create utilities for getting and setting the value
// associated with an arbitrary "nullable" type.
//
// These functors make minimal assumptions of `VALUE_TYPE`, merely that it is
// copy constructable and copy assignable.
//
// ```
// struct NullableValueUtil {
//
//     // CLASS METHODS
//
//     /// Load to the specified `value` the value of the specified nullable
//     /// value `object`.  This function template requires that the specified
//     /// `NULLABLE_VALUE_TYPE` is a `bdlat` "nullable" type.  The behavior is
//     /// undefined unless `object` is in a non-null state (i.e.,
//     /// `false == bdlat_NullableValueFunctions::isNull(object))`.
//     template <class NULLABLE_VALUE_TYPE>
//     static int getValue(
//         typename bdlat_NullableValueFunctions
//                             ::ValueType<NULLABLE_VALUE_TYPE>::Type *value,
//         const NULLABLE_VALUE_TYPE&                                  object)
//     {
//         BSLMF_ASSERT(bdlat_NullableValueFunctions
//                             ::IsNullableValue<NULLABLE_VALUE_TYPE>::value);
//
//         BSLS_ASSERT(!bdlat_NullableValueFunctions::isNull(object));
//
//         typedef typename bdlat_NullableValueFunctions
//                           ::ValueType<NULLABLE_VALUE_TYPE>::Type ValueType;
//
//         GetValueAccessor<ValueType> valueAccessor(value);
//         return bdlat_NullableValueFunctions::accessValue(object,
//                                                          valueAccessor);
//     }
//
//     /// Set the value of the specified `object` to the specified `value`.
//     /// This function template requires that the specified
//     /// `NULLABLE_VALUE_TYPE` is a `bdlat` "nullable" type.  The behavior is
//     /// undefined unless `object` is in a non-null state (i.e.,
//     /// `false == bdlat_NullableValueFunctions::isNull(object))`.  Note that
//     /// a "nullable" object can be put into a non-null state by the
//     /// `bdlat_NullableValueFunctions::makeValue` function overload for the
//     /// `NULLABLE_VALUE_TYPE`.
//     template <class NULLABLE_VALUE_TYPE>
//     static int setValue(NULLABLE_VALUE_TYPE                        *object,
//                         const typename bdlat_NullableValueFunctions
//                            ::ValueType<NULLABLE_VALUE_TYPE>::Type&  value)
//     {
//         BSLMF_ASSERT(bdlat_NullableValueFunctions
//                             ::IsNullableValue<NULLABLE_VALUE_TYPE>::value);
//
//         BSLS_ASSERT(object);
//         BSLS_ASSERT(!bdlat_NullableValueFunctions::isNull(*object));
//
//         typedef typename bdlat_NullableValueFunctions
//                           ::ValueType<NULLABLE_VALUE_TYPE>::Type ValueType;
//
//         SetValueManipulator<ValueType> manipulator(value);
//         return bdlat_NullableValueFunctions::manipulateValue(object,
//                                                              manipulator);
//     }
// };
// ```
// Now, we can use these functors to write generic utility functions for
// getting and setting the value types of arbitrary "nullable" classes.
// ```
// void myUsageScenario()
// {
//     mine::MyNullableValue object;
//     assert(bdlat_NullableValueFunctions::isNull(object));
//
//     bdlat_NullableValueFunctions::makeValue(&object);
//     assert(!bdlat_NullableValueFunctions::isNull(object));
//
//     typedef
//     bdlat_NullableValueFunctions::ValueType<mine::MyNullableValue>::Type
//                                                              MyValueType;
//
//     int rcs = NullableValueUtil::setValue(&object, MyValueType(42));
//     assert(0 == rcs);
//
//     MyValueType value;
//     int rcg = NullableValueUtil::getValue(&value, object);
//     assert( 0 == rcg);
//     assert(42 == value);
// }
// ```
//
///Example 4: Achieving Type Independence
/// - - - - - - - - - - - - - - - - - - -
// Finally, suppose we have another type such as `your::YourNullableType`,
// shown below:
// ```
// namespace BloombergLP {
// namespace your {
//
// class YourNullableValue {
//
//     // DATA
//     bool        d_isNull;
//     bsl::string d_value;
//
//   public:
//     // CREATORS
//     YourNullableValue()
//     : d_isNull(true)
//     , d_value()
//     {
//     }
//
//     // MANIPULATORS
//     void makeValue()
//     {
//         d_isNull = false;
//         d_value.clear();
//     }
//
//     void makeNull()
//     {
//         d_isNull = true;
//         d_value.clear();
//     }
//
//     bsl::string& value()
//     {
//         assert(!d_isNull);
//
//         return d_value;
//     }
//
//     // ACCESSORS
//     const bsl::string& value() const
//     {
//         assert(!d_isNull);
//
//         return d_value;
//     }
//     bool isNull() const
//     {
//         return d_isNull;
//     }
// };
//
// }  // close namespace your
// }  // close enterprise namespace
// ```
// Notice that while there are many similarities to `mine::MyNullableValue`
// there are clearly differences:
// * The value type is `bsl::string`, not `int`.
// * Attributes are accessed via accessor methods, not public data members.
//
// Nevertheless, since `your::YourNullableValue` also provides the functions
// and types expected by the `bdlat` infrastructure (not shown) we can
// successfully use `your::YourNullableValue` value instead of
// `mine::MyNullableValue` in the previous usage scenario, with no other
// changes:
// ```
// void yourUsageScenario()
// {
//     your::YourNullableValue object;  // YOUR NULLABLE TYPE
//     assert(bdlat_NullableValueFunctions::isNull(object));
//
//     bdlat_NullableValueFunctions::makeValue(&object);
//     assert(!bdlat_NullableValueFunctions::isNull(object));
//
//     typedef
//     bdlat_NullableValueFunctions::ValueType<your::YourNullableValue>::Type
//                                                              YourValueType;
//
//     int rcs = NullableValueUtil::setValue(&object, YourValueType("NB"));
//     assert(0 == rcs);
//
//     YourValueType value;
//     int rcg = NullableValueUtil::getValue(&value, object);
//     assert(  0  == rcg);
//     assert("NB" == value);
// }
// ```
// Notice that syntax and order of `bdlat_NullableValueFunction` functions
// calls have not been changed.  The only difference is that the contained
// type has changed from `int` to `bsl::string`.
//
// Finally, instead of defining a new "nullable" type, we could substitute the
// existing type template `bdlb::NullableValue`.  Note that this component
// provides specializations of the `bdlat_nullableValueFunctions` for that
// type.  Since the accessor and manipulator functions we created earlier are
// type neutral, we can simply drop `bdlb::NullableValue<float>` into our
// familiar scenario:
// ```
// void anotherUsageScenario()
// {
//     bdlb::NullableValue<float> object;  // BDE NULLABLE TYPE
//     assert(bdlat_NullableValueFunctions::isNull(object));
//
//     bdlat_NullableValueFunctions::makeValue(&object);
//     assert(!bdlat_NullableValueFunctions::isNull(object));
//
//     typedef
//     bdlat_NullableValueFunctions::ValueType<bdlb::NullableValue<float> >
//                                                 ::Type AnotherValueType;
//
//     int rcs = NullableValueUtil::setValue(&object, AnotherValueType(2.0));
//     assert(0 == rcs);
//
//     AnotherValueType value;
//     int rcg = NullableValueUtil::getValue(&value, object);
//     assert(0   == rcg);
//     assert(2.0 == value);
// }
// ```

#include <bdlscm_version.h>

#include <bslmf_integralconstant.h>

#include <bsls_assert.h>
#include <bsls_review.h>

#include <bdlat_bdeatoverrides.h>

#include <bdlb_nullablevalue.h>
#include <bdlb_nullableallocatedvalue.h>

#include <bslmf_matchanytype.h>

namespace BloombergLP {

                   // ======================================
                   // namespace bdlat_NullableValueFunctions
                   // ======================================

/// This `namespace` provides functions that expose "nullable" behavior for
/// "nullable value" types.  See the component-level documentation for more
/// information.
namespace bdlat_NullableValueFunctions {
    // META-FUNCTIONS

    /// This `struct` should be specialized for third-party types that need
    /// to expose "nullable" behavior.  See the component-level
    /// documentation for further information.
    template <class TYPE>
    struct IsNullableValue : bsl::false_type {
    };

    /// This meta-function should contain a typedef `Type` that specifies
    /// the type of value stored in a nullable type of the parameterized
    /// `TYPE`.
    template <class TYPE>
    struct ValueType;

    // MANIPULATORS

    /// Assign to the specified "nullable" `object` the default value for
    /// the contained type.
    template <class TYPE>
    void makeValue(TYPE *object);

    /// Invoke the specified `manipulator` on the address of the value stored
    /// in the specified "nullable" `object`.  The supplied `manipulator` must
    /// be a callable type that can be called as if it had the following
    /// signature:
    /// ```
    /// int manipulator(VALUE_TYPE *value);
    /// ```
    /// Return the value from the invocation of `manipulator`.  The behavior is
    /// undefined unless `object` does not contain a null value.
    template <class TYPE, class MANIPULATOR>
    int manipulateValue(TYPE *object, MANIPULATOR& manipulator);

    // ACCESSORS

    /// Invoke the specified `accessor` on the non-modifiable value stored in
    /// the specified "nullable" `object`.  The supplied `accessor` must be a
    /// callable type that can be called as if it had the following signature:
    /// ```
    /// int accessor(const VALUE_TYPE& value);
    /// ```
    /// Return the value from the invocation of `accessor`.  The behavior is
    /// undefined unless `object` does not contain a null value.
    template <class TYPE, class ACCESSOR>
    int accessValue(const TYPE& object, ACCESSOR& accessor);

    /// Return `true` if the specified "nullable" `object` contains a null
    /// value, and `false` otherwise.
    template <class TYPE>
    bool isNull(const TYPE& object);

}  // close namespace bdlat_NullableValueFunctions

                      // ================================
                      // bdlb::NullableValue declarations
                      // ================================

/// This namespace declaration adds the implementation of the "nullable value"
/// traits for `bdlb::NullableValue` to `bdlat_NullableValueFunctions`.  Note
/// that `bdlb::NullableValue` is the first of two canonical "nullable value"
/// types.
namespace bdlat_NullableValueFunctions {
    // META-FUNCTIONS
    template <class TYPE>
    struct IsNullableValue<bdlb::NullableValue<TYPE> > : public bsl::true_type
    {};

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

                  // =========================================
                  // bdlb::NullableAllocatedValue declarations
                  // =========================================

/// This namespace declaration adds the implementation of the "nullable value"
/// traits for `bdlb::NullableAllocatedValue` to
/// `bdlat_NullableValueFunctions`.  Note that `bdlb::NullableAllocatedValue`
/// is the second of two canonical "nullable value" types.
namespace bdlat_NullableValueFunctions {
    // META-FUNCTIONS
    template <class TYPE>
    struct IsNullableValue<bdlb::NullableAllocatedValue<TYPE> >
    : public bsl::true_type {
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

                      // -------------------------------
                      // bdlb::NullableValue definitions
                      // -------------------------------

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
    BSLS_ASSERT(!object->isNull());

    return manipulator(&object->value());
}

// ACCESSORS
template <class TYPE, class ACCESSOR>
inline
int bdlat_NullableValueFunctions::bdlat_nullableValueAccessValue(
                                     const bdlb::NullableValue<TYPE>& object,
                                     ACCESSOR&                        accessor)
{
    BSLS_ASSERT(!object.isNull());

    return accessor(object.value());
}

template <class TYPE>
inline
bool bdlat_NullableValueFunctions::bdlat_nullableValueIsNull(
                                       const bdlb::NullableValue<TYPE>& object)
{
    return object.isNull();
}

                  // ----------------------------------------
                  // bdlb::NullableAllocatedValue definitions
                  // ----------------------------------------

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
    BSLS_ASSERT(!object->isNull());

    return manipulator(&object->value());
}

// ACCESSORS
template <class TYPE, class ACCESSOR>
inline
int bdlat_NullableValueFunctions::bdlat_nullableValueAccessValue(
                            const bdlb::NullableAllocatedValue<TYPE>& object,
                            ACCESSOR&                                 accessor)
{
    BSLS_ASSERT(!object.isNull());

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
