// bdlat_customizedtypefunctions.h                                    -*-C++-*-
#ifndef INCLUDED_BDLAT_CUSTOMIZEDTYPEFUNCTIONS
#define INCLUDED_BDLAT_CUSTOMIZEDTYPEFUNCTIONS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a namespace defining customized type functions.
//
//@CLASSES:
//  bdlat_CustomizedTypeFunctions: namespace for customized type functions
//
//@SEE_ALSO:
//
//@DESCRIPTION: The `bdlat_CustomizedTypeFunctions` `namespace` provided in
// this component defines parameterized functions that expose
// "customized type" behavior for "customized type" types.  See the
// package-level documentation for a brief description of "customized type"
// types.
//
// The functions in this namespace allow users to:
// * convert from base type to customized type (`convertFromBaseType`).
// * convert from customized type to base type (`convertToBaseType`).
//
// Also, the meta-function `IsCustomizedType` contains a compile-time constant
// `value` that is non-zero if the parameterized `TYPE` exposes
// "customized type" behavior through the `bdlat_CustomizedTypeFunctions`
// `namespace`.
//
// The `BaseType` meta-function contains a typedef `Type` that specifies the
// base type of the value for the parameterized "customized type" type.
//
// This component specializes all of these functions for types that have the
// `bdlat_TypeTraitBasicCustomizedType` trait.
//
// Types that do not have the `bdlat_TypeTraitBasicCustomizedType` trait can be
// plugged into the `bdlat` framework.  This is done by overloading the
// `bdlat_customizedType*` functions inside the namespace of the plugged in
// type.  Note that the placeholders `YOUR_TYPE` and `BASE_TYPE` are not
// template arguments.  The former should be replaced with the name of the type
// being plugged into the framework.  The latter is a type defined by the
// above-mentioned `BaseType` meta-function for `YOUR_TYPE`.
// ```
// // MANIPULATORS
//
// /// Convert from the specified `value` to the specified customized `object`.
// /// Return 0 if successful and non-zero otherwise.
// int bdlat_customizedTypeConvertFromBaseType(YOUR_TYPE        *object,
//                                             const BASE_TYPE&  value);
//
// // ACCESSORS
//
// /// Load into the specified `result` the value of the specified `object`.
// const BASE_TYPE& bdlat_customizedTypeConvertToBaseType(
//                                                    const YOUR_TYPE& object);
// ```
// Also, the `IsCustomizedType` meta-function must be specialized for the
// `YOUR_TYPE` type in the `bdlat_CustomizedTypeFunctions` namespace.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Usage
/// - - - - - - - - - - -
// Suppose we have a customized type called `Cusip`, holding an object of type
// `bsl::string` with a restriction that the length of the string cannot be
// longer than nine characters.  We can obtain the value of the string using
// the following code:
// ```
// mine::Cusip myCusip("281C82UE");
// bsl::string base = bdlat_CustomizedTypeFunctions::convertToBaseType(
//                                                                    myCusip);
//
// assert("281C82UE" == base);
// ```
// Attempting to assign a string longer than nine characters will not succeed:
// ```
// bsl::string invalidCusip = "1234567890";
//
// int retCode = bdlat_CustomizedTypeFunctions::convertFromBaseType(
//                                                               &myCusip,
//                                                               invalidCusip);
// assert(0 != retCode);
// ```
// For the purpose of this example, the class definition is as follows:
// ```
// #include <bdlat_customizedtypefunctions.h>
// #include <bdlb_string.h>
// #include <bsls_assert.h>
// #include <sstream>
// #include <string>
//
// namespace BloombergLP {
// namespace mine {
//
// /// Identification number for the US and Canada.  It is a 9-digit number
// /// consisting of 8 digits and a check digit.  The Bloomberg ID will be
// /// returned for Corp, Govt, Pfd if a CUSIP is not available.
// class Cusip {
//   private:
//     // PRIVATE DATA MEMBERS
//     bsl::string d_value;  // stored value
//
//     // FRIENDS
//     friend bool operator==(const Cusip& lhs, const Cusip& rhs);
//     friend bool operator!=(const Cusip& lhs, const Cusip& rhs);
//   public:
//     // TYPES
//     typedef bsl::string BaseType;
//
//     // CREATORS
//
//     /// Create an object of type `Cusip` having the default value.
//     /// Optionally specify a `basicAllocator` used to supply memory.  If
//     /// `basicAllocator` is 0, the currently installed default allocator is
//     /// used.
//     explicit Cusip(bslma::Allocator *basicAllocator = 0);
//
//     /// Create an object of type `Cusip` having the value of the specified
//     /// `original` object.  Optionally specify a `basicAllocator` used to
//     /// supply memory.  If `basicAllocator` is 0, the currently installed
//     /// default allocator is used.
//     Cusip(const Cusip& original, bslma::Allocator *basicAllocator = 0);
//
//     /// Create an object of type `Cusip` having the specified `value`.
//     /// Optionally specify a `basicAllocator` used to supply memory.  If
//     /// `basicAllocator` is 0, the currently installed default allocator is
//     /// used.
//     explicit Cusip(const bsl::string&  value,
//                    bslma::Allocator   *basicAllocator = 0);
//
//     /// Destroy this object.
//     ~Cusip();
//
//     // MANIPULATORS
//
//     /// Assign to this object the value of the specified `rhs` object.
//     Cusip& operator=(const Cusip& rhs);
//
//     /// Reset this object to the default value (i.e., its value upon default
//     /// construction).
//     void reset();
//
//     /// Convert from the specified `value` to this type.  Return 0 if
//     /// successful and non-zero otherwise.
//     int fromString(const bsl::string& value);
//
//     // ACCESSORS
//
//     /// Format this object to the specified output `stream` at the
//     /// optionally specified indentation `level` and return a reference to
//     /// the modifiable `stream`.  If `level` is specified, optionally
//     /// specify `spacesPerLevel`, the number of spaces per indentation level
//     /// for this and all of its nested objects.  Each line is indented by
//     /// the absolute value of `level * spacesPerLevel`.  If `level` is
//     /// negative, suppress indentation of the first line.  If
//     /// `spacesPerLevel` is negative, suppress line breaks and format the
//     /// entire output on one line.  If `stream` is initially invalid, this
//     /// operation has no effect.  Note that a trailing newline is provided
//     /// in multiline mode only.
//     bsl::ostream& print(bsl::ostream& stream,
//                         int           level = 0,
//                         int           spacesPerLevel = 4) const;
//
//     /// Convert this value to `bsl::string`.
//     const bsl::string& toString() const;
// };
//
// // FREE OPERATORS
//
// /// Return `true` if the specified `lhs` and `rhs` attribute objects have
// /// the same value, and `false` otherwise.  Two attribute objects have the
// /// same value if each respective attribute has the same value.
// bool operator==(const Cusip& lhs, const Cusip& rhs);
//
// /// Return `true` if the specified `lhs` and `rhs` attribute objects do not
// /// have the same value, and `false` otherwise.  Two attribute objects do
// /// not have the same value if one or more respective attributes differ in
// /// values.
// bool operator!=(const Cusip& lhs, const Cusip& rhs);
//
// /// Format the specified `rhs` to the specified output `stream` and return a
// /// reference to the modifiable `stream`.
// bsl::ostream& operator<<(bsl::ostream& stream, const Cusip& rhs);
// ```
// The class implementation is straightforward and is deferred to the end of
// this usage example.
//
// We can now make `Cusip` expose "customized type" behavior by implementing
// `bdlat_CustomizedTypeFunctions` for `Cusip`.  The first method (the longer
// one) overloads all the `bdlat_customizedType*` functions.  In the second
// method, we show how to bypass this by simply declaring the class
// `mine::Cusip` to have the `bdlat_TypeTraitBasicCustomizedType` trait.
//
///Longer Usage
/// - - - - - -
// First, we should forward declare all the functions that we will implement
// inside the `mine` namespace:
// ```
// // MANIPULATORS
//
// /// Convert from the specified `value` to the specified customized `object`.
// ///  Return 0 if successful and non-zero otherwise.
// int bdlat_customizedTypeConvertFromBaseType(Cusip              *object,
//                                             const bsl::string&  value);
//
// // ACCESSORS
//
// /// Load into the specified `result` the value of the specified `object`.
// const bsl::string& bdlat_customizedTypeConvertToBaseType(
//                                                        const Cusip& object);
// ```
// Next, we provide the definitions for each of these functions:
// ```
// // MANIPULATORS
// int bdlat_customizedTypeConvertFromBaseType(Cusip              *object,
//                                             const bsl::string&  value)
// {
//     return object->fromString(value);
// }
//
// // ACCESSORS
// const bsl::string& bdlat_customizedTypeConvertToBaseType(
//                                                         const Cusip& object)
// {
//     return object.toString();
// }
//
// }  // close namespace mine
// ```
// Finally, we need to specialize the `IsCustomizedType` meta-function in the
// `bdlat_CustomizedTypeFunctions` namespace for the `mine::Cusip` type.  This
// makes the `bdlat` infrastructure recognize `mine::Cusip` as a customized
// type abstraction:
// ```
// namespace bdlat_CustomizedTypeFunctions {
//
//     template <>
//     struct IsCustomizedType<mine::Cusip> : bsl::true_type {
//     };
//
//     template <>
//     struct BaseType<mine::Cusip> {
//         typedef bsl::string Type;
//     };
//
// }  // close namespace bdlat_CustomizedTypeFunctions
// }  // close namespace BloombergLP
// ```
// The `bdlat` infrastructure (and any component that uses this infrastructure)
// will now recognize `mine::Cusip` as a "customized" type.
//
///Shorter Usage
///- - - - - - -
// We can bypass all the code from the longer usage example by simply
// declaring `mine::Cusip` to have the `bdlat_TypeTraitBasicCustomizedType`
// trait as follows:
// ```
// namespace BloombergLP {
//
// // TRAITS
//
// BDLAT_DECL_CUSTOMIZEDTYPE_WITH_ALLOCATOR_TRAITS(mine::Cusip)
//
// }  // close namespace BloombergLP
// ```
// Again, the `bdlat` infrastructure (and any component that uses this
// infrastructure) will now recognize `mine::Cusip` as a "customized" type.
//
// For example, suppose we have the following XML data:
// ```
// <?xml version='1.0' encoding='UTF-8' ?>
// <Cusip>
//     <value>"281C82UE"</value>
// </Cusip>
// ```
// Using the `balxml_decoder` component, we can load this XML data into a
// `mine::Cusip` object:
// ```
// #include <balxml_decoder.h>
//
// void decodeMyCustomizedTypeFromXML(bsl::istream& inputData)
// {
//     Cusip object;
//
//     balxml::DecoderOptions options;
//     balxml::MiniReader     reader;
//     balxml::ErrorInfo      errInfo;
//
//     balxml::Decoder decoder(&options, &reader, &errInfo);
//     int result = decoder.decode(inputData, &object);
//
//     assert(0          == result);
//     assert("281C82UE" == object.toString());
// }
// ```
// Note that the `bdlat` framework can be used for functionality other than
// encoding/decoding into XML.  When `mine::Cusip` is plugged into the
// framework, then it will be automatically usable within the framework.  For
// example, the following snippets of code will convert a string from a stream
// and load it into a `Cusip` object:
// ```
// int readCusip(bsl::istream& stream, mine::Cusip *cusip)
// {
//     bsl::string value;
//     stream >> value;
//
//     return bdlat_CustomizedTypeFunctions::convertFromBaseType(cusip, value);
// }
// ```
// Now we have a generic function that takes an input stream and a `Cusip`
// object, and inputs its value.  We can use this generic function as follows:
// ```
// void usageExample()
// {
//     bsl::stringstream ss;
//     mine::Cusip object;
//
//     ss << "281C82UE\n1234567890\n";
//
//     assert(0          == readCusip(ss, &object));
//     assert("281C82UE" == object.toString());
//
//     assert(0          != readCusip(ss, &object));
// }
// ```
// This concludes the usage example.
//
// For completeness, we finish by providing the straightforward details of the
// implementation of the class `Cusip`:
// ```
// namespace BloombergLP {
// namespace mine {
//
// // CREATORS
//
// inline
// Cusip::Cusip(bslma::Allocator *basicAllocator)
// : d_value(basicAllocator)
// {
// }
//
// inline
// Cusip::Cusip(const Cusip& original, bslma::Allocator *basicAllocator)
// : d_value(original.d_value, basicAllocator)
// {
// }
//
// inline
// Cusip::Cusip(const bsl::string& value, bslma::Allocator *basicAllocator)
// : d_value(value, basicAllocator)
// {
// }
//
// inline
// Cusip::~Cusip()
// {
// }
//
// // MANIPULATORS
//
// inline
// Cusip& Cusip::operator=(const Cusip& rhs)
// {
//     d_value = rhs.d_value;
//     return *this;
// }
//
// inline
// void Cusip::reset()
// {
//     // bdlat_ValueTypeFunctions::reset(&d_value);
//     d_value.erase();
// }
//
// inline
// int Cusip::fromString(const bsl::string& value)
// {
//     enum { SUCCESS = 0, FAILURE = -1 };
//
//     globalFlag = 1;
//
//     if (9 < value.size()) {
//         return FAILURE;
//     }
//
//     d_value = value;
//
//     return SUCCESS;
// }
//
// // ACCESSORS
//
// inline
// bsl::ostream& Cusip::print(bsl::ostream& stream,
//                            int           level,
//                            int           spacesPerLevel) const
// {
//     return bdlb::PrintMethods::print(stream,
//                                     d_value,
//                                     level,
//                                     spacesPerLevel);
// }
//
// inline
// const bsl::string& Cusip::toString() const
// {
//     globalFlag = 2;
//
//     return d_value;
// }
//
// // FREE OPERATORS
//
// inline
// bool operator==(const Cusip& lhs, const Cusip& rhs)
// {
//     return lhs.d_value == rhs.d_value;
// }
//
// inline
// bool operator!=(const Cusip& lhs, const Cusip& rhs)
// {
//     return lhs.d_value != rhs.d_value;
// }
//
// inline
// bsl::ostream& operator<<(bsl::ostream& stream, const Cusip& rhs)
// {
//     return rhs.print(stream, 0, -1);
// }
//
// }  // close namespace mine
// }  // close namespace BloombergLP
// ```

#include <bdlscm_version.h>

#include <bdlat_bdeatoverrides.h>
#include <bdlat_typetraits.h>

#include <bdlt_date.h>
#include <bdlt_datetz.h>
#include <bdlt_datetime.h>
#include <bdlt_datetimetz.h>
#include <bdlt_time.h>
#include <bdlt_timetz.h>

#include <bslalg_hastrait.h>

#include <bslmf_assert.h>
#include <bslmf_integralconstant.h>

#include <bsls_platform.h>
#include <bsls_types.h>

#include <bsl_string.h>

namespace BloombergLP {

                  // =======================================
                  // namespace bdlat_CustomizedTypeFunctions
                  // =======================================

/// This `namespace` provides functions that expose "customized type" behavior
/// for "customized type" types.  See the component-level documentation for
/// more information.
namespace bdlat_CustomizedTypeFunctions {
    // META-FUNCTIONS

    /// This `struct` should be specialized for third-party types that need
    /// to expose "customized type" behavior.  See the component-level
    /// documentation for further information.
    template <class TYPE>
    struct IsCustomizedType
    : public bsl::integral_constant<
        bool,
        bslalg::HasTrait<TYPE, bdlat_TypeTraitBasicCustomizedType>::value> {
    };

    /// This meta-function should contain a typedef `Type` that specifies
    /// the default base type for the parameterized `TYPE`.
    template <class TYPE>
    struct BaseType {
        BSLMF_ASSERT(
          (bslalg::HasTrait<TYPE, bdlat_TypeTraitBasicCustomizedType>::value));

        typedef typename TYPE::BaseType Type;
    };

    // MANIPULATORS

    /// Convert from the specified `value` to the specified customized
    /// `object`.  Return 0 if successful and non-zero otherwise.
    template <class TYPE, class BASE_TYPE>
    int convertFromBaseType(TYPE *object, const BASE_TYPE& value);

    // ACCESSORS

    /// Load into the specified `result` the value of the specified `object`.
    template <class TYPE>
    const typename BaseType<TYPE>::Type& convertToBaseType(const TYPE& object);

}  // close namespace bdlat_CustomizedTypeFunctions

                            // ====================
                            // default declarations
                            // ====================

/// This namespace declaration adds the default implementations of the
/// "customized type" customization-point functions to
/// `bdlat_CustomizedTypeFunctions`.  These default implementations assume the
/// type of the acted-upon object has member functions to convert to and from
/// "simple" types.
namespace bdlat_CustomizedTypeFunctions {
    // MANIPULATORS
    template <class TYPE, class BASE_TYPE>
    int bdlat_customizedTypeConvertFromBaseType(TYPE             *object,
                                                const BASE_TYPE&  value);

    // ACCESSORS
    template <class TYPE>
    const typename BaseType<TYPE>::Type&
    bdlat_customizedTypeConvertToBaseType(const TYPE& object);

}  // close namespace bdlat_CustomizedTypeFunctions

                  // ========================================
                  // struct bdlat_CustomizedTypeFunctions_Imp
                  // ========================================

struct bdlat_CustomizedTypeFunctions_Imp {

    template <class TYPE>
    static int convertFromBaseType(TYPE *object, bool value);

    template <class TYPE>
    static int convertFromBaseType(TYPE *object, int value);

    template <class TYPE>
    static int convertFromBaseType(TYPE *object, char value);

    template <class TYPE>
    static int convertFromBaseType(TYPE *object, short value);

    template <class TYPE>
    static int convertFromBaseType(TYPE               *object,
                                   bsls::Types::Int64  value);

    template <class TYPE>
    static int convertFromBaseType(TYPE *object, unsigned int value);

    template <class TYPE>
    static int convertFromBaseType(TYPE *object, unsigned char value);

    template <class TYPE>
    static int convertFromBaseType(TYPE *object, unsigned short value);

    template <class TYPE>
    static int convertFromBaseType(TYPE                *object,
                                   bsls::Types::Uint64  value);

    template <class TYPE>
    static int convertFromBaseType(TYPE *object, float value);

    template <class TYPE>
    static int convertFromBaseType(TYPE *object, double value);

    template <class TYPE>
    static int convertFromBaseType(TYPE *object, const bdlt::Date& value);

    template <class TYPE>
    static int convertFromBaseType(TYPE *object, const bdlt::DateTz& value);

    template <class TYPE>
    static int convertFromBaseType(TYPE *object, const bdlt::Datetime& value);

    template <class TYPE>
    static int convertFromBaseType(TYPE                    *object,
                                   const bdlt::DatetimeTz&  value);

    template <class TYPE>
    static int convertFromBaseType(TYPE *object, const bdlt::Time& value);

    template <class TYPE>
    static int convertFromBaseType(TYPE *object, const bdlt::TimeTz& value);

    template <class TYPE>
    static int convertFromBaseType(TYPE *object, const bsl::string& value);

    template <class TYPE>
    static int convertFromBaseType(TYPE                     *object,
                                   const bsl::vector<char>&  value);

    template <class TYPE>
    static const bool& convertToBaseType(const TYPE& object, bool*);

    template <class TYPE>
    static const char& convertToBaseType(const TYPE& object, char*);

    template <class TYPE>
    static const short& convertToBaseType(const TYPE& object, short*);

    template <class TYPE>
    static const int& convertToBaseType(const TYPE& object, int*);

    template <class TYPE>
    static const bsls::Types::Int64& convertToBaseType(const TYPE& object,
                                                       bsls::Types::Int64*);

    template <class TYPE>
    static const unsigned char& convertToBaseType(const TYPE& object,
                                                  unsigned char*);

    template <class TYPE>
    static const unsigned short& convertToBaseType(const TYPE& object,
                                                   unsigned short*);

    template <class TYPE>
    static const unsigned int& convertToBaseType(const TYPE& object,
                                                 unsigned int*);

    template <class TYPE>
    static const bsls::Types::Uint64& convertToBaseType(const TYPE& object,
                                                        bsls::Types::Uint64*);

    template <class TYPE>
    static const float& convertToBaseType(const TYPE& object, float*);

    template <class TYPE>
    static const double& convertToBaseType(const TYPE& object, double*);

    template <class TYPE>
    static const bdlt::Date& convertToBaseType(const TYPE&  object,
                                               bdlt::Date  *);

    template <class TYPE>
    static const bdlt::DateTz& convertToBaseType(const TYPE& object,
                                                bdlt::DateTz*);

    template <class TYPE>
    static const bdlt::Datetime& convertToBaseType(const TYPE& object,
                                                  bdlt::Datetime*);

    template <class TYPE>
    static const bdlt::DatetimeTz& convertToBaseType(const TYPE& object,
                                                    bdlt::DatetimeTz*);

    template <class TYPE>
    static const bdlt::Time& convertToBaseType(const TYPE&  object,
                                               bdlt::Time  *);

    template <class TYPE>
    static const bdlt::TimeTz& convertToBaseType(const TYPE&  object,
                                                bdlt::TimeTz *);

    template <class TYPE>
    static const bsl::string& convertToBaseType(const TYPE&  object,
                                                bsl::string *);
    template <class TYPE>
    static const bsl::vector<char>& convertToBaseType(
                                                     const TYPE&        object,
                                                     bsl::vector<char> *);
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                   // ---------------------------------------
                   // namespace bdlat_CustomizedTypeFunctions
                   // ---------------------------------------

// MANIPULATORS
template <class TYPE, class BASE_TYPE>
inline
int bdlat_CustomizedTypeFunctions::convertFromBaseType(
                                                      TYPE             *object,
                                                      const BASE_TYPE&  value)
{
    return bdlat_customizedTypeConvertFromBaseType(object, value);
}

// ACCESSORS
template <class TYPE>
inline
const typename bdlat_CustomizedTypeFunctions::BaseType<TYPE>::Type&
bdlat_CustomizedTypeFunctions::convertToBaseType(const TYPE&  object)
{
    return bdlat_customizedTypeConvertToBaseType(object);
}

                            // -------------------
                            // default definitions
                            // -------------------

// MANIPULATORS
template <class TYPE, class BASE_TYPE>
inline
int bdlat_CustomizedTypeFunctions::bdlat_customizedTypeConvertFromBaseType(
                                                      TYPE             *object,
                                                      const BASE_TYPE&  value)
{
    return bdlat_CustomizedTypeFunctions_Imp::convertFromBaseType(object,
                                                                  value);
}

// ACCESSORS
template <class TYPE>
inline
const typename bdlat_CustomizedTypeFunctions::BaseType<TYPE>::Type&
bdlat_CustomizedTypeFunctions::bdlat_customizedTypeConvertToBaseType(
                                                           const TYPE&  object)
{
    typedef typename
    bdlat_CustomizedTypeFunctions::BaseType<TYPE>::Type BaseType;

    return bdlat_CustomizedTypeFunctions_Imp::convertToBaseType(object,
                                                                (BaseType*)0);
}

                  // ----------------------------------------
                  // struct bdlat_CustomizedTypeFunctions_Imp
                  // ----------------------------------------

template <class TYPE>
inline
int bdlat_CustomizedTypeFunctions_Imp::convertFromBaseType(TYPE *object,
                                                           bool  value)
{
    BSLMF_ASSERT((bslalg::HasTrait<TYPE,
                                  bdlat_TypeTraitBasicCustomizedType>::value));

    return object->fromBool(value);
}

template <class TYPE>
inline
int bdlat_CustomizedTypeFunctions_Imp::convertFromBaseType(TYPE *object,
                                                           int   value)
{
    BSLMF_ASSERT((bslalg::HasTrait<TYPE,
                                  bdlat_TypeTraitBasicCustomizedType>::value));

    return object->fromInt(value);
}

template <class TYPE>
inline
int bdlat_CustomizedTypeFunctions_Imp::convertFromBaseType(TYPE *object,
                                                           char  value)
{
    BSLMF_ASSERT((bslalg::HasTrait<TYPE,
                                  bdlat_TypeTraitBasicCustomizedType>::value));

    return object->fromChar(value);
}

template <class TYPE>
inline
int bdlat_CustomizedTypeFunctions_Imp::convertFromBaseType(TYPE  *object,
                                                           short  value)
{
    BSLMF_ASSERT((bslalg::HasTrait<TYPE,
                                  bdlat_TypeTraitBasicCustomizedType>::value));

    return object->fromShort(value);
}

template <class TYPE>
inline
int bdlat_CustomizedTypeFunctions_Imp::convertFromBaseType(
                                                    TYPE               *object,
                                                    bsls::Types::Int64  value)
{
    BSLMF_ASSERT((bslalg::HasTrait<TYPE,
                                  bdlat_TypeTraitBasicCustomizedType>::value));

    return object->fromInt64(value);
}

template <class TYPE>
inline
int bdlat_CustomizedTypeFunctions_Imp::convertFromBaseType(
                                                          TYPE         *object,
                                                          unsigned int  value)
{
    BSLMF_ASSERT((bslalg::HasTrait<TYPE,
                                  bdlat_TypeTraitBasicCustomizedType>::value));

    return object->fromUnsignedInt(value);
}

template <class TYPE>
inline
int bdlat_CustomizedTypeFunctions_Imp::convertFromBaseType(
                                                         TYPE          *object,
                                                         unsigned char  value)
{
    BSLMF_ASSERT((bslalg::HasTrait<TYPE,
                                  bdlat_TypeTraitBasicCustomizedType>::value));

    return object->fromUnsignedChar(value);
}

template <class TYPE>
inline
int bdlat_CustomizedTypeFunctions_Imp::convertFromBaseType(
                                                        TYPE           *object,
                                                        unsigned short  value)
{
    BSLMF_ASSERT((bslalg::HasTrait<TYPE,
                                  bdlat_TypeTraitBasicCustomizedType>::value));

    return object->fromUnsignedShort(value);
}

template <class TYPE>
inline
int bdlat_CustomizedTypeFunctions_Imp::convertFromBaseType(
                                                   TYPE                *object,
                                                   bsls::Types::Uint64  value)
{
    BSLMF_ASSERT((bslalg::HasTrait<TYPE,
                                  bdlat_TypeTraitBasicCustomizedType>::value));

    return object->fromUnsignedInt64(value);
}

template <class TYPE>
inline
int bdlat_CustomizedTypeFunctions_Imp::convertFromBaseType(TYPE  *object,
                                                           float  value)
{
    BSLMF_ASSERT((bslalg::HasTrait<TYPE,
                                  bdlat_TypeTraitBasicCustomizedType>::value));

    return object->fromFloat(value);
}

template <class TYPE>
inline
int bdlat_CustomizedTypeFunctions_Imp::convertFromBaseType(TYPE   *object,
                                                           double  value)
{
    BSLMF_ASSERT((bslalg::HasTrait<TYPE,
                                  bdlat_TypeTraitBasicCustomizedType>::value));

    return object->fromDouble(value);
}

template <class TYPE>
inline
int bdlat_CustomizedTypeFunctions_Imp::convertFromBaseType(
                                                     TYPE              *object,
                                                     const bdlt::Date&  value)
{
    BSLMF_ASSERT((bslalg::HasTrait<TYPE,
                                  bdlat_TypeTraitBasicCustomizedType>::value));

    return object->fromDate(value);
}

template <class TYPE>
inline
int bdlat_CustomizedTypeFunctions_Imp::convertFromBaseType(
                                                   TYPE                *object,
                                                   const bdlt::DateTz&  value)
{
    BSLMF_ASSERT((bslalg::HasTrait<TYPE,
                                  bdlat_TypeTraitBasicCustomizedType>::value));

    return object->fromDateTz(value);
}

template <class TYPE>
inline
int bdlat_CustomizedTypeFunctions_Imp::convertFromBaseType(
                                                 TYPE                  *object,
                                                 const bdlt::Datetime&  value)
{
    BSLMF_ASSERT((bslalg::HasTrait<TYPE,
                                  bdlat_TypeTraitBasicCustomizedType>::value));

    return object->fromDatetime(value);
}

template <class TYPE>
inline
int bdlat_CustomizedTypeFunctions_Imp::convertFromBaseType(
                                               TYPE                    *object,
                                               const bdlt::DatetimeTz&  value)
{
    BSLMF_ASSERT((bslalg::HasTrait<TYPE,
                                  bdlat_TypeTraitBasicCustomizedType>::value));

    return object->fromDatetimeTz(value);
}

template <class TYPE>
inline
int bdlat_CustomizedTypeFunctions_Imp::convertFromBaseType(
                                                     TYPE              *object,
                                                     const bdlt::Time&  value)
{
    BSLMF_ASSERT((bslalg::HasTrait<TYPE,
                                  bdlat_TypeTraitBasicCustomizedType>::value));

    return object->fromTime(value);
}

template <class TYPE>
inline
int bdlat_CustomizedTypeFunctions_Imp::convertFromBaseType(
                                                   TYPE                *object,
                                                   const bdlt::TimeTz&  value)
{
    BSLMF_ASSERT((bslalg::HasTrait<TYPE,
                                  bdlat_TypeTraitBasicCustomizedType>::value));

    return object->fromTimeTz(value);
}

template <class TYPE>
inline
int bdlat_CustomizedTypeFunctions_Imp::convertFromBaseType(
                                                    TYPE               *object,
                                                    const bsl::string&  value)
{
    BSLMF_ASSERT((bslalg::HasTrait<TYPE,
                                  bdlat_TypeTraitBasicCustomizedType>::value));

    return object->fromString(value);
}

template <class TYPE>
inline
int bdlat_CustomizedTypeFunctions_Imp::convertFromBaseType(
                                              TYPE                     *object,
                                              const bsl::vector<char>&  value)
{
    BSLMF_ASSERT((bslalg::HasTrait<TYPE,
                                  bdlat_TypeTraitBasicCustomizedType>::value));

    return object->fromVector(value);
}

template <class TYPE>
inline
const bool& bdlat_CustomizedTypeFunctions_Imp::convertToBaseType(
                                                           const TYPE&  object,
                                                           bool        *)
{
    BSLMF_ASSERT((bslalg::HasTrait<TYPE,
                                  bdlat_TypeTraitBasicCustomizedType>::value));

    return object.toBool();
}

template <class TYPE>
inline
const int& bdlat_CustomizedTypeFunctions_Imp::convertToBaseType(
                                                           const TYPE&  object,
                                                           int         *)
{
    BSLMF_ASSERT((bslalg::HasTrait<TYPE,
                                  bdlat_TypeTraitBasicCustomizedType>::value));

    return object.toInt();
}

template <class TYPE>
inline
const char& bdlat_CustomizedTypeFunctions_Imp::convertToBaseType(
                                                           const TYPE&  object,
                                                           char        *)
{
    BSLMF_ASSERT((bslalg::HasTrait<TYPE,
                                  bdlat_TypeTraitBasicCustomizedType>::value));

    return object.toChar();
}

template <class TYPE>
inline
const short& bdlat_CustomizedTypeFunctions_Imp::convertToBaseType(
                                                           const TYPE&  object,
                                                           short       *)
{
    BSLMF_ASSERT((bslalg::HasTrait<TYPE,
                                  bdlat_TypeTraitBasicCustomizedType>::value));

    return object.toShort();
}

template <class TYPE>
inline
const bsls::Types::Int64&
bdlat_CustomizedTypeFunctions_Imp::convertToBaseType(
                                                    const TYPE&         object,
                                                    bsls::Types::Int64 *)
{
    BSLMF_ASSERT((bslalg::HasTrait<TYPE,
                                  bdlat_TypeTraitBasicCustomizedType>::value));

    return object.toInt64();
}

template <class TYPE>
inline
const unsigned int& bdlat_CustomizedTypeFunctions_Imp::convertToBaseType(
                                                          const TYPE&   object,
                                                          unsigned int *)
{
    BSLMF_ASSERT((bslalg::HasTrait<TYPE,
                                  bdlat_TypeTraitBasicCustomizedType>::value));

    return object.toUnsignedInt();
}

template <class TYPE>
inline
const unsigned char& bdlat_CustomizedTypeFunctions_Imp::convertToBaseType(
                                                         const TYPE&    object,
                                                         unsigned char *)
{
    BSLMF_ASSERT((bslalg::HasTrait<TYPE,
                                  bdlat_TypeTraitBasicCustomizedType>::value));

    return object.toUnsignedChar();
}

template <class TYPE>
inline
const unsigned short& bdlat_CustomizedTypeFunctions_Imp::convertToBaseType(
                                                        const TYPE&     object,
                                                        unsigned short *)
{
    BSLMF_ASSERT((bslalg::HasTrait<TYPE,
                                  bdlat_TypeTraitBasicCustomizedType>::value));

    return object.toUnsignedShort();
}

template <class TYPE>
inline
const bsls::Types::Uint64&
bdlat_CustomizedTypeFunctions_Imp::convertToBaseType(
                                                   const TYPE&          object,
                                                   bsls::Types::Uint64 *)
{
    BSLMF_ASSERT((bslalg::HasTrait<TYPE,
                                  bdlat_TypeTraitBasicCustomizedType>::value));

    return object.toUnsignedInt64();
}

template <class TYPE>
inline
const float& bdlat_CustomizedTypeFunctions_Imp::convertToBaseType(
                                                           const TYPE&  object,
                                                           float       *)
{
    BSLMF_ASSERT((bslalg::HasTrait<TYPE,
                                  bdlat_TypeTraitBasicCustomizedType>::value));

    return object.toFloat();
}

template <class TYPE>
inline
const double& bdlat_CustomizedTypeFunctions_Imp::convertToBaseType(
                                                           const TYPE&  object,
                                                           double      *)
{
    BSLMF_ASSERT((bslalg::HasTrait<TYPE,
                                  bdlat_TypeTraitBasicCustomizedType>::value));

    return object.toDouble();
}

template <class TYPE>
inline
const bdlt::Date& bdlat_CustomizedTypeFunctions_Imp::convertToBaseType(
                                                           const TYPE&  object,
                                                           bdlt::Date  *)
{
    BSLMF_ASSERT((bslalg::HasTrait<TYPE,
                                  bdlat_TypeTraitBasicCustomizedType>::value));

    return object.toDate();
}

template <class TYPE>
inline
const bdlt::DateTz& bdlat_CustomizedTypeFunctions_Imp::convertToBaseType(
                                                          const TYPE&   object,
                                                          bdlt::DateTz *)
{
    BSLMF_ASSERT((bslalg::HasTrait<TYPE,
                                  bdlat_TypeTraitBasicCustomizedType>::value));

    return object.toDateTz();
}

template <class TYPE>
inline
const bdlt::Datetime& bdlat_CustomizedTypeFunctions_Imp::convertToBaseType(
                                                        const TYPE&     object,
                                                        bdlt::Datetime *)
{
    BSLMF_ASSERT((bslalg::HasTrait<TYPE,
                                  bdlat_TypeTraitBasicCustomizedType>::value));

    return object.toDatetime();
}

template <class TYPE>
inline
const bdlt::DatetimeTz& bdlat_CustomizedTypeFunctions_Imp::convertToBaseType(
                                                      const TYPE&       object,
                                                      bdlt::DatetimeTz *)
{
    BSLMF_ASSERT((bslalg::HasTrait<TYPE,
                                  bdlat_TypeTraitBasicCustomizedType>::value));

    return object.toDatetimeTz();
}

template <class TYPE>
inline
const bdlt::Time& bdlat_CustomizedTypeFunctions_Imp::convertToBaseType(
                                                           const TYPE&  object,
                                                           bdlt::Time  *)
{
    BSLMF_ASSERT((bslalg::HasTrait<TYPE,
                                  bdlat_TypeTraitBasicCustomizedType>::value));

    return object.toTime();
}

template <class TYPE>
inline
const bdlt::TimeTz& bdlat_CustomizedTypeFunctions_Imp::convertToBaseType(
                                                          const TYPE&   object,
                                                          bdlt::TimeTz *)
{
    BSLMF_ASSERT((bslalg::HasTrait<TYPE,
                                  bdlat_TypeTraitBasicCustomizedType>::value));

    return object.toTimeTz();
}

template <class TYPE>
inline
const bsl::string& bdlat_CustomizedTypeFunctions_Imp::convertToBaseType(
                                                           const TYPE&  object,
                                                           bsl::string *)
{
    BSLMF_ASSERT((bslalg::HasTrait<TYPE,
                                  bdlat_TypeTraitBasicCustomizedType>::value));

    return object.toString();
}

template <class TYPE>
inline
const bsl::vector<char>& bdlat_CustomizedTypeFunctions_Imp::convertToBaseType(
                                                     const TYPE&        object,
                                                     bsl::vector<char> *)
{
    BSLMF_ASSERT((bslalg::HasTrait<TYPE,
                                  bdlat_TypeTraitBasicCustomizedType>::value));

    return object.toVector();
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
