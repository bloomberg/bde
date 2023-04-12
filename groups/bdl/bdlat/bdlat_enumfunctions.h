// bdlat_enumfunctions.h                                              -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLAT_ENUMFUNCTIONS
#define INCLUDED_BDLAT_ENUMFUNCTIONS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a namespace defining enumeration functions.
//
//@CLASSES:
//  bdlat_EnumFunctions: namespace for calling enumeration functions
//
//@SEE_ALSO:
//
//@DESCRIPTION: The 'bdlat_EnumFunctions' 'namespace' provided in this
// component defines parameterized functions that expose "enumeration" behavior
// for "enumeration" types.  See the package-level documentation for a full
// description of "enumeration" types.  The functions in this namespace allow
// users to:
//..
//      o load an enumeration value from an integer value ('fromInt').
//      o load an enumeration value from a string value ('fromString').
//      o load an integer value from an enumeration value ('toInt').
//      o load a string value from an enumeration value ('toString').
//      o set an enumeration to its fallback value ('makeFallback').
//      o check whether an enumeration supports a fallback value
//        ('hasFallback').
//      o test whether an enumeration is equal to the fallback value
//        ('isFallback').
//..
// The meta-functions 'IsEnumeration' and 'HasFallbackEnumerator' indicate
// whether a type supports the above functions.  If the compile-time constant
// 'IsEnumeration<TYPE>::VALUE' is declared to be nonzero, the type must
// support the first four functions listed above.  If the compile-time constant
// 'HasFallbackEnumerator<TYPE>::VALUE' is declared to be nonzero, the type
// must also support the last three functions listed above.
//
// This component provides default implementations of the above listed
// functions and meta-functions for types that have the
// 'bdlat_TypeTraitBasicEnumeration' trait.  For other types, one may
// specialize the meta-functions and provide implementations for the functions
// by declaring overloads of the corresponding customization points.  An
// example of this is provided in the 'Usage' section of this document.
//
///Usage
///-----
// The following section illustrates intended use of this component.
//
// Suppose you have a C++ 'enum' type called 'ImageType' whose enumerators
// represent supported formats for image files:
//..
// #include <bdlat_enumfunctions.h>
// #include <bdlb_string.h>
// #include <bsl_iostream.h>
// #include <bsl_sstream.h>
// #include <bsl_string.h>
//
// namespace BloombergLP {
//
// namespace mine {
//
// enum ImageType {
//     JPG     = 0,
//     PNG     = 1,
//     GIF     = 2,
//     UNKNOWN = 100
// };
//..
// We can now make 'ImageType' expose "enumeration" behavior by implementing
// all the necessary 'bdlat_enum*' functions for 'ImageType' inside the 'mine'
// namespace (*not* by attempting to declare specializations or overloads in
// the 'bdlat_EnumFunctions' namespace).  First we should forward declare all
// the functions that we will implement inside the 'mine' namespace:
//..
// // MANIPULATORS
//
// int bdlat_enumFromInt(ImageType* result, int number);
//     // Load into the specified 'result' the enumerator matching the
//     // specified 'number'.  Return 0 on success, and a non-zero value
//     // with no effect on 'result' if 'number' does not match any
//     // enumerator.
//
// int bdlat_enumFromString(ImageType  *result,
//                          const char *string,
//                          int         stringLength);
//     // Load into the specified 'result' the enumerator matching the
//     // specified 'string' of the specified 'stringLength'.  Return 0 on
//     // success, and a non-zero value with no effect on 'result' if
//     // 'string' and 'stringLength' do not match any enumerator.
//
// int bdlat_enumMakeFallback(ImageType *result);
//     // Load into the specified 'result' the fallback enumerator value and
//     // return 0 to indicate success.
//
// // ACCESSORS
//
// void bdlat_enumToInt(int *result, const ImageType& value);
//     // Load into the specified 'result' the integer representation of the
//     // enumerator value held by the specified 'value'.
//
// void bdlat_enumToString(bsl::string *result, const ImageType& value);
//     // Load into the specified 'result' the string representation of the
//     // enumerator value held by the specified 'value'.
//
// bool bdlat_enumHasFallback(const ImageType&);
//     // Return 'true' to indicate that this type supports a fallback
//     // enumerator.
//
// bool bdlat_enumIsFallback(const ImageType& value);
//     // Return 'true' if the specified 'value' equals the fallback
//     // enumerator, and 'false' otherwise.
//
//  }  // close namespace mine
//..
// Next, we provide the definitions for each of these functions:
//..
//  // MANIPULATORS
//
//  inline
//  int mine::bdlat_enumFromInt(ImageType *result, int number)
//  {
//      enum { SUCCESS = 0, NOT_FOUND = -1 };
//
//      switch (number) {
//        case JPG: {
//          *result = JPG;
//          return SUCCESS;
//        }
//        case PNG: {
//          *result = PNG;
//          return SUCCESS;
//        }
//        case GIF: {
//          *result = GIF;
//          return SUCCESS;
//        }
//        case UNKNOWN: {
//          *result = UNKNOWN;
//          return SUCCESS;
//        }
//        default: {
//          return NOT_FOUND;
//        }
//      }
//  }
//
//  inline
//  int mine::bdlat_enumFromString(ImageType  *result,
//                                 const char *string,
//                                 int         stringLength)
//  {
//      enum { SUCCESS = 0, NOT_FOUND = -1 };
//
//      if (bdlb::String::areEqualCaseless("jpg",
//                                         string,
//                                         stringLength)) {
//          *result = JPG;
//          return SUCCESS;
//      }
//
//      if (bdlb::String::areEqualCaseless("png",
//                                         string,
//                                         stringLength)) {
//          *result = PNG;
//          return SUCCESS;
//      }
//
//      if (bdlb::String::areEqualCaseless("gif",
//                                         string,
//                                         stringLength)) {
//          *result = GIF;
//          return SUCCESS;
//      }
//
//      if (bdlb::String::areEqualCaseless("unknown",
//                                         string,
//                                         stringLength)) {
//          *result = UNKNOWN;
//          return SUCCESS;
//      }
//
//      return NOT_FOUND;
//  }
//
//  inline
//  int mine::bdlat_enumMakeFallback(ImageType *result)
//  {
//      *result = UNKNOWN;
//      return 0;
//  }
//
//  // ACCESSORS
//
//  inline
//  void mine::bdlat_enumToInt(int *result, const ImageType& value)
//  {
//      *result = static_cast<int>(value);
//  }
//
//  inline
//  void mine::bdlat_enumToString(bsl::string *result, const ImageType& value)
//  {
//      switch (value) {
//        case JPG: {
//          *result = "JPG";
//        } break;
//        case PNG: {
//          *result = "PNG";
//        } break;
//        case GIF: {
//          *result = "GIF";
//        } break;
//        case UNKNOWN: {
//          *result = "UNKNOWN";
//        } break;
//        default: {
//          *result = "INVALID";
//        } break;
//      }
//  }
//
//  inline
//  bool mine::bdlat_enumHasFallback(const ImageType&)
//  {
//      return true;
//  }
//
//  inline
//  bool mine::bdlat_enumIsFallback(const ImageType& value)
//  {
//      return value == UNKNOWN;
//  }
//..
// Finally, we need to specialize the 'IsEnumeration' and
// 'HasFallbackEnumerator' meta-functions in the 'bdlat_EnumFunctions'
// namespace for the 'mine::ImageType' type.  This makes the 'bdlat'
// infrastructure recognize 'ImageType' as an enumeration abstraction with a
// fallback enumerator:
//..
//  namespace bdlat_EnumFunctions {
//  template <>
//  struct IsEnumeration<mine::ImageType> {
//      enum { VALUE = 1 };
//  };
//  template <>
//  struct HasFallbackEnumerator<mine::ImageType> {
//      enum { VALUE = 1 };
//  };
//  }  // close namespace 'bdlat_EnumFunctions'
//  }  // close namespace 'BloombergLP'
//..
// The 'bdlat' infrastructure (and any component that uses this infrastructure)
// will now recognize 'ImageType' as an "enumeration" type with a fallback
// enumerator.  For example, suppose we have the following XML data:
//..
//  <?xml version='1.0' encoding='UTF-8' ?>
//  <ImageType>PNG</ImageType>
//..
// Using the 'balxml_decoder' component, we can load this XML data into a
// 'ImageType' object:
//..
//  #include <balxml_decoder.h>
//
//  void decodeImageTypeFromXML(bsl::istream& inputData)
//  {
//      using namespace BloombergLP;
//
//      ImageType object = 0;
//
//      balxml::DecoderOptions options;
//      balxml::MiniReader     reader;
//      balxml::ErrorInfo      errInfo;
//
//      balxml::Decoder decoder(&options, &reader, &errInfo);
//      int result = decoder.decode(inputData, &object);
//
//      assert(0   == result);
//      assert(PNG == object);
//  }
//..
// Note that the 'bdlat' framework can be used for functionality other than
// encoding/decoding into XML.  When 'mine::ImageType' is plugged into the
// framework, then it will be automatically usable within the framework.  For
// example, consider the following generic functions that read a string from a
// stream and decode its value into a 'bdlat' "enumeration" object:
//..
//  template <class TYPE>
//  int readEnum(bsl::istream& stream, TYPE *object)
//  {
//      bsl::string value;
//      stream >> value;
//
//      return bdlat_EnumFunctions::fromString(object,
//                                             value.c_str(),
//                                             value.length())) {
//  }
//
//  template <class TYPE>
//  int readEnumOrFallback(bsl::istream& stream, TYPE *object)
//  {
//      const int rc = readEnum(stream, object);
//      return (0 == rc) ? rc : bdlat_EnumFunctions::makeFallback(object);
//  }
//..
// We can use these generic functions with 'mine::ImageType' as follows:
//..
//  void usageExample()
//  {
//      using namespace BloombergLP;
//
//      bsl::stringstream ss;
//      mine::ImageType   object;
//
//      ss << "JPG\nWEBP\nWEBP\n";
//
//      assert(0             == readEnum(ss, &object));
//      assert(mine::JPG     == object);
//
//      assert(0             != readEnum(ss, &object));
//      assert(mine::JPG     == object);
//
//      assert(0             == readEnumOrFallback(ss, &object));
//      assert(mine::UNKNOWN == object);
//  }
//..

#include <bdlscm_version.h>

#include <bdlat_bdeatoverrides.h>
#include <bdlat_typetraits.h>

#include <bslalg_hastrait.h>

#include <bslmf_assert.h>
#include <bslmf_integralconstant.h>
#include <bslmf_matchanytype.h>
#include <bslmf_metaint.h>

#include <bsls_assert.h>
#include <bsls_platform.h>

#include <bsl_string.h>

namespace BloombergLP {

                      // =============================
                      // namespace bdlat_EnumFunctions
                      // =============================

namespace bdlat_EnumFunctions {
    // This 'namespace' provides functions that expose "enumeration" behavior
    // for "enumeration" types.  See the component-level documentation for more
    // information.

    // META-FUNCTIONS
    template <class TYPE>
    struct IsEnumeration {
        // This 'struct' should be specialized for third-party types that need
        // to expose "enumeration" behavior.  See the component-level
        // documentation for further information.

        enum {
            VALUE = bslalg::HasTrait<TYPE,
                                     bdlat_TypeTraitBasicEnumeration>::VALUE
        };
    };

    template <class TYPE>
    struct HasFallbackEnumerator {
        // This 'struct' should be specialized for third-party types that need
        // to declare the fact that they have a fallback enumerator value.
        // Clients that specialize this struct must ensure that if
        // 'HasFallbackEnumerator<TYPE>::VALUE' is true, then
        // 'IsEnumeration<TYPE>::VALUE' is also true; otherwise, the behavior
        // is undefined.

        enum {
            VALUE =
                bslalg::HasTrait<TYPE,
                                 bdlat_TypeTraitHasFallbackEnumerator>::VALUE
        };
    };

    // MANIPULATORS
    template <class TYPE>
    int fromInt(TYPE *result, int number);
        // Load into the specified 'result' the enumerator matching the
        // specified 'number'.  Return 0 on success, and a non-zero value with
        // no effect on 'result' if 'number' does not match any enumerator.

    template <class TYPE>
    int fromString(TYPE *result, const char *string, int stringLength);
        // Load into the specified 'result' the enumerator matching the
        // specified 'string' of the specified 'stringLength'.  Return 0 on
        // success, and a non-zero value with no effect on 'result' if 'string'
        // and 'stringLength' do not match any enumerator.

    template <class TYPE>
    int makeFallback(TYPE *result);
        // Load into the specified 'result' the fallback enumerator value.
        // Return 0 on success, and a non-zero value with no effect on 'result'
        // if it does not have a fallback enumerator.

    // ACCESSORS
    template <class TYPE>
    bool hasFallback(const TYPE& value);
        // Return 'true' if the specified 'value' supports a fallback
        // enumerator, and 'false' otherwise.

    template <class TYPE>
    bool isFallback(const TYPE& value);
        // Return 'true' if the specified 'value' is equal to a fallback
        // enumerator, and 'false' otherwise.

    template <class TYPE>
    void toInt(int *result, const TYPE& value);
        // Load into the specified 'result' the integer representation of the
        // enumerator value held by the specified 'value'.

    template <class TYPE>
    void toString(bsl::string *result, const TYPE& value);
        // Load into the specified 'result' the string representation of the
        // enumerator value held by the specified 'value'.

}  // close namespace bdlat_EnumFunctions

                    // ===================================
                    // struct bdlat_EnumFunctions_ImplUtil
                    // ===================================

struct bdlat_EnumFunctions_ImplUtil {
    ///Implementation Notes
    ///--------------------
    // The below functions use tag dispatch to provide an implementation of the
    // fallback-related operations that either delegate to the associated
    // customization points or immediately return a non-zero value to indicate
    // failure, depending on whether the type does or does not satisfy the
    // 'HasFallbackEnumerator' trait, respectively.  The purpose of doing this
    // is so that the fallback-related operations can be used on any type, even
    // enumeration types that do not have fallback enumerators.

    // CLASS METHODS
    template <class TYPE>
    static int makeFallback(TYPE *result, bsl::true_type);
    template <class TYPE>
    static int makeFallback(TYPE *result, bsl::false_type);

    template <class TYPE>
    static bool hasFallback(const TYPE& value, bsl::true_type);
    template <class TYPE>
    static bool hasFallback(const TYPE& value, bsl::false_type);

    template <class TYPE>
    static bool isFallback(const TYPE& value, bsl::true_type);
    template <class TYPE>
    static bool isFallback(const TYPE& value, bsl::false_type);
};

                            // ====================
                            // default declarations
                            // ====================

namespace bdlat_EnumFunctions {
    // This namespace declaration adds the default implementations of the
    // "enumeration" customization-point functions to 'bdlat_EnumFunctions'.
    // These default implementations assume the type of the acted-upon object
    // is a basic-enumeration type.  For more information about
    // basic-enumeration types, see {'bdlat_typetraits'}.
    //
    // In order to use a type as a 'bdlat' enumeration type that is *not* a
    // basic-enumeration type, you must implement the below customization
    // points for that type as overloads in the namespace that the type belongs
    // to.  Overloading the 'bdlat_enumMakeFallback', 'bdlat_enumHasFallback',
    // and 'bdlat_enumIsFallback' functions is required only if
    // 'HasFallbackEnumerator' is 'true' for your type or class of types.  In
    // that case, the three functions' behaviors must be consistent with each
    // other, which means that the following axioms must hold for a non-const
    // lvalue 'x':
    //: 1 Whenever 'bdlat_enumHasFallback(x)' is 'false',
    //:   'bdlat_enumMakeFallback(&x)' would fail by leaving 'x' unchanged and
    //:   returning a nonzero value, and 'bdlat_enumIsFallback(x)' is 'false';
    //: 2 Whenever 'bdlat_enumHasFallback(x)' is 'true',
    //:   'bdlat_enumMakeFallback(&x)' would succeed by returning 0 and a
    //:   call to 'bdlat_enumIsFallback(x)' immediately afterward would return
    //:   'true'.

    // MANIPULATORS
    template <class TYPE>
    int bdlat_enumFromInt(TYPE *result, int number);

    template <class TYPE>
    int bdlat_enumFromString(TYPE       *result,
                             const char *string,
                             int         stringLength);

    template <class TYPE>
    int bdlat_enumMakeFallback(TYPE *result);
        // Load into the specified 'result' the fallback enumerator value.
        // Return 0 on success, and a non-zero value with no effect on 'result'
        // if it does not have a fallback enumerator.  The behavior is
        // undefined if this default implementation of 'bdlat_enumMakeFallback'
        // is instantiated with a template parameter 'TYPE' such that
        // 'bdlat_HasFallbackEnumerator<TYPE>' is 'false'.  Note that this is a
        // customization point function and should not be called directly by
        // user code.  Use 'bdlat_EnumFunctions::makeFallback' instead.

    // ACCESSORS
    template <class TYPE>
    void bdlat_enumToInt(int *result, const TYPE& value);

    template <class TYPE>
    void bdlat_enumToString(bsl::string *result, const TYPE& value);

    template <class TYPE>
    bool bdlat_enumHasFallback(const TYPE& value);
        // Return 'true' if the specified 'value' supports a fallback
        // enumerator, and 'false' otherwise.  The behavior is undefined if
        // this default implementation of 'bdlat_enumHasFallback' is
        // instantiated with a template parameter 'TYPE' such that
        // 'bdlat_HasFallbackEnumerator<TYPE>' is 'false'.  Note that this is a
        // customization point function and should not be called directly by
        // user code.  Use 'bdlat_EnumFunctions::hasFallback' instead.

    template <class TYPE>
    bool bdlat_enumIsFallback(const TYPE& value);
        // Return 'true' if the specified 'value' is equal to a fallback
        // enumerator, and 'false' otherwise.  The behavior is undefined if
        // this default implementation of 'bdlat_enumIsFallback' is
        // instantiated with a template parameter 'TYPE' such that
        // 'bdlat_HasFallbackEnumerator<TYPE>' is 'false'.  Note that this is a
        // customization point function and should not be called directly by
        // user code.  Use 'bdlat_EnumFunctions::isFallback' instead.

}  // close namespace bdlat_EnumFunctions

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                      // -----------------------------
                      // namespace bdlat_EnumFunctions
                      // -----------------------------

// MANIPULATORS
template <class TYPE>
inline
int bdlat_EnumFunctions::fromInt(TYPE *result, int number)
{
    return bdlat_enumFromInt(result, number);
}

template <class TYPE>
inline
int bdlat_EnumFunctions::fromString(TYPE       *result,
                                    const char *string,
                                    int         stringLength)
{
    return bdlat_enumFromString(result, string, stringLength);
}

template <class TYPE>
inline
int bdlat_EnumFunctions::makeFallback(TYPE *result)
{
    bsl::integral_constant<bool, HasFallbackEnumerator<TYPE>::VALUE> tag;
    return bdlat_EnumFunctions_ImplUtil::makeFallback(result, tag);
}

// ACCESSORS
template <class TYPE>
inline
bool bdlat_EnumFunctions::hasFallback(const TYPE& value)
{
    bsl::integral_constant<bool, HasFallbackEnumerator<TYPE>::VALUE> tag;
    return bdlat_EnumFunctions_ImplUtil::hasFallback(value, tag);
}

template <class TYPE>
inline
bool bdlat_EnumFunctions::isFallback(const TYPE& value)
{
    bsl::integral_constant<bool, HasFallbackEnumerator<TYPE>::VALUE> tag;
    return bdlat_EnumFunctions_ImplUtil::isFallback(value, tag);
}

template <class TYPE>
inline
void bdlat_EnumFunctions::toInt(int *result, const TYPE& value)
{
    bdlat_enumToInt(result, value);
}


template <class TYPE>
inline
void bdlat_EnumFunctions::toString(bsl::string *result, const TYPE& value)
{
    bdlat_enumToString(result, value);
}

                    // -----------------------------------
                    // struct bdlat_EnumFunctions_ImplUtil
                    // -----------------------------------

// CLASS METHODS
template <class TYPE>
int bdlat_EnumFunctions_ImplUtil::makeFallback(TYPE *result, bsl::true_type)
{
#if !defined(BSLS_PLATFORM_CMP_SUN)
    BSLMF_ASSERT(bdlat_EnumFunctions::IsEnumeration<TYPE>::VALUE);
#endif
    using bdlat_EnumFunctions::bdlat_enumMakeFallback;
    return bdlat_enumMakeFallback(result);
}

template <class TYPE>
int bdlat_EnumFunctions_ImplUtil::makeFallback(TYPE *result, bsl::false_type)
{
    static_cast<void>(result);
    return -1;
}

template <class TYPE>
bool bdlat_EnumFunctions_ImplUtil::hasFallback(const TYPE& value,
                                               bsl::true_type)
{
#if !defined(BSLS_PLATFORM_CMP_SUN)
    BSLMF_ASSERT(bdlat_EnumFunctions::IsEnumeration<TYPE>::VALUE);
#endif
    using bdlat_EnumFunctions::bdlat_enumHasFallback;
    return bdlat_enumHasFallback(value);
}

template <class TYPE>
bool bdlat_EnumFunctions_ImplUtil::hasFallback(const TYPE& value,
                                               bsl::false_type)
{
    static_cast<void>(value);
    return false;
}

template <class TYPE>
bool bdlat_EnumFunctions_ImplUtil::isFallback(const TYPE& value,
                                              bsl::true_type)
{
#if !defined(BSLS_PLATFORM_CMP_SUN)
    BSLMF_ASSERT(bdlat_EnumFunctions::IsEnumeration<TYPE>::VALUE);
#endif
    using bdlat_EnumFunctions::bdlat_enumIsFallback;
    return bdlat_enumIsFallback(value);
}

template <class TYPE>
bool bdlat_EnumFunctions_ImplUtil::isFallback(const TYPE& value,
                                              bsl::false_type)
{
    static_cast<void>(value);
    return false;
}
                            // -------------------
                            // default definitions
                            // -------------------

// MANIPULATORS
template <class TYPE>
inline
int bdlat_EnumFunctions::bdlat_enumFromInt(TYPE *result, int number)
{
    BSLMF_ASSERT(bdlat_IsBasicEnumeration<TYPE>::value);

    typedef typename bdlat_BasicEnumerationWrapper<TYPE>::Wrapper Wrapper;
    return Wrapper::fromInt(result, number);
}

template <class TYPE>
inline
int bdlat_EnumFunctions::bdlat_enumFromString(TYPE       *result,
                                              const char *string,
                                              int         stringLength)
{
    BSLMF_ASSERT(
             (bslalg::HasTrait<TYPE, bdlat_TypeTraitBasicEnumeration>::VALUE));

    typedef typename bdlat_BasicEnumerationWrapper<TYPE>::Wrapper Wrapper;
    return Wrapper::fromString(result, string, stringLength);
}

template <class TYPE>
inline
int bdlat_EnumFunctions::bdlat_enumMakeFallback(TYPE *result)
{
#if !defined(BSLS_PLATFORM_CMP_SUN)
    BSLMF_ASSERT(
             (bslalg::HasTrait<TYPE, bdlat_TypeTraitBasicEnumeration>::VALUE));
    BSLMF_ASSERT(
        (bslalg::HasTrait<TYPE, bdlat_TypeTraitHasFallbackEnumerator>::VALUE));
#endif

    typedef typename bdlat_BasicEnumerationWrapper<TYPE>::Wrapper Wrapper;
    return Wrapper::makeFallback(result);
}

// ACCESSORS
template <class TYPE>
inline
void bdlat_EnumFunctions::bdlat_enumToInt(int *result, const TYPE& value)
{
    BSLMF_ASSERT(
             (bslalg::HasTrait<TYPE, bdlat_TypeTraitBasicEnumeration>::VALUE));

    *result = static_cast<int>(value);
}

template <class TYPE>
inline
void bdlat_EnumFunctions::bdlat_enumToString(bsl::string *result,
                                             const TYPE&  value)
{
    BSLMF_ASSERT(
             (bslalg::HasTrait<TYPE, bdlat_TypeTraitBasicEnumeration>::VALUE));

    typedef typename bdlat_BasicEnumerationWrapper<TYPE>::Wrapper Wrapper;
    *result = Wrapper::toString(value);
}

template <class TYPE>
inline
bool bdlat_EnumFunctions::bdlat_enumHasFallback(const TYPE& value)
{
#if !defined(BSLS_PLATFORM_CMP_SUN)
    BSLMF_ASSERT(
             (bslalg::HasTrait<TYPE, bdlat_TypeTraitBasicEnumeration>::VALUE));
    BSLMF_ASSERT(
        (bslalg::HasTrait<TYPE, bdlat_TypeTraitHasFallbackEnumerator>::VALUE));
#endif

    typedef typename bdlat_BasicEnumerationWrapper<TYPE>::Wrapper Wrapper;
    return Wrapper::hasFallback(value);
}

template <class TYPE>
inline
bool bdlat_EnumFunctions::bdlat_enumIsFallback(const TYPE& value)
{
#if !defined(BSLS_PLATFORM_CMP_SUN)
    BSLMF_ASSERT(
             (bslalg::HasTrait<TYPE, bdlat_TypeTraitBasicEnumeration>::VALUE));
    BSLMF_ASSERT(
        (bslalg::HasTrait<TYPE, bdlat_TypeTraitHasFallbackEnumerator>::VALUE));
#endif

    typedef typename bdlat_BasicEnumerationWrapper<TYPE>::Wrapper Wrapper;
    return Wrapper::isFallback(value);
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
