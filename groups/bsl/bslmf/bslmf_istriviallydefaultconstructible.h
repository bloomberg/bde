// bslmf_istriviallydefaultconstructible.h                            -*-C++-*-
#ifndef INCLUDED_BSLMF_ISTRIVIALLYDEFAULTCONSTRUCTIBLE
#define INCLUDED_BSLMF_ISTRIVIALLYDEFAULTCONSTRUCTIBLE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for trivially default-constructible.
//
//@CLASSES:
//  bsl::is_trivially_default_constructible: trait meta-function
//  bsl::is_trivially_default_constructible_v: the result value
//
//@SEE_ALSO: bslmf_integerconstant, bslmf_nestedtraitdeclaration
//
//@DESCRIPTION: This component defines a meta-function,
// 'bsl::is_trivially_default_constructible' and a template variable
// 'bsl::is_trivially_default_constructible_v', that represents the result
// value of the 'bsl::is_trivially_default_constructible' meta-function, that
// may be used to query whether a type has a trivial default constructor as
// defined in section 12.1.5 of the C++11 standard [class.ctor].
//
// 'bsl::is_trivially_default_constructible' has the same syntax as the
// 'is_trivially_default_constructible' template from the C++11 standard
// [meta.unary.prop].  However, unlike the template defined in the C++11
// standard, which can determine the correct value for all types without
// requiring specialization, 'bsl::is_trivially_default_constructible' can, by
// default, determine the value for the following type categories only:
//..
//  Type Category        Has Trivial Default Constructor
//  -------------------  -------------------------------
//  reference types      false
//  fundamental types    true
//  enums                true
//  pointers             true
//  pointers to members  true
//..
// For all other types, 'bsl::is_trivially_default_constructible' returns
// 'false', unless the type is explicitly specified to be trivially
// default-constructible, which can be done in two ways:
//
//: 1 Define a template specialization for
//:   'bsl::is_trivially_default_constructible' having the type as the template
//:   parameter that inherits directly from 'bsl::true_type'.
//:
//: 2 Use the 'BSLMF_NESTED_TRAIT_DECLARATION' macro to define
//:   'bsl::is_trivially_default_constructible' as the trait in the class
//:   definition of the type.
//
// Note that the template variable 'is_trivially_default_constructible_v' is
// defined in the C++17 standard as an inline variable.  If the current
// compiler supports the inline variable C++17 compiler feature,
// 'bsl::is_trivially_default_constructible_v' is defined as an
// 'inline constexpr bool' variable.  Otherwise, if the compiler supports the
// variable templates C++14 compiler feature,
// 'bsl::is_trivially_default_constructible_v' is defined as a non-inline
// 'constexpr bool' variable.  See
// 'BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES' and
// 'BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES' macros in
// bsls_compilerfeatures component for details.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify Whether Types are Trivially Default-Constructible
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a type is trivially
// default-constructible.
//
// First, we define a set of types to evaluate:
//..
//  typedef int MyFundamentalType;
//  typedef int& MyFundamentalTypeReference;
//
//  class MyTriviallyDefaultConstructibleType {
//  };
//
//  struct MyNonTriviallyDefaultConstructibleType {
//
//      int d_data;
//
//      MyNonTriviallyDefaultConstructibleType()
//      : d_data(1)
//      {
//      }
//  };
//..
// Then, since user-defined types cannot be automatically evaluated by
// 'is_trivially_default_constructible', we define a template specialization to
// specify that 'MyTriviallyDefaultConstructibleType' is trivially
// default-constructible:
//..
//  namespace bsl {
//
//  template <>
//  struct is_trivially_default_constructible<
//                      MyTriviallyDefaultConstructibleType> : bsl::true_type {
//      // This template specialization for
//      // 'is_trivially_default_constructible' indicates that
//      // 'MyTriviallyDefaultConstructibleType' is a trivially
//      // default-constructible type.
//  };
//
//  }  // close namespace bsl
//..
// Now, we verify whether each type is trivially default-constructible using
// 'bsl::is_trivially_default_constructible':
//..
//  void showUsage()
//  {
//      assert(true  == bsl::is_trivially_default_constructible<
//                                                  MyFundamentalType>::value);
//      assert(false == bsl::is_trivially_default_constructible<
//                                         MyFundamentalTypeReference>::value);
//      assert(true  == bsl::is_trivially_default_constructible<
//                                MyTriviallyDefaultConstructibleType>::value);
//      assert(false == bsl::is_trivially_default_constructible<
//                             MyNonTriviallyDefaultConstructibleType>::value);
//..
// Note that if the current compiler supports the variable templates C++14
// feature, then we can re-write the snippet of code above as follows:
//..
//#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
//      assert(true  == bsl::is_trivially_default_constructible_v<
//                                                         MyFundamentalType>);
//      assert(false == bsl::is_trivially_default_constructible_v<
//                                                MyFundamentalTypeReference>);
//      assert(true  == bsl::is_trivially_default_constructible_v<
//                                       MyTriviallyDefaultConstructibleType>);
//      assert(false == bsl::is_trivially_default_constructible_v<
//                                    MyNonTriviallyDefaultConstructibleType>);
//#endif
//  }
//..

#include <bslscm_version.h>

#include <bslmf_detectnestedtrait.h>
#include <bslmf_integralconstant.h>
#include <bslmf_isconst.h>
#include <bslmf_voidtype.h>

#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_platform.h>

#include <stddef.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bslmf_isenum.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

namespace bsl {

template <class TYPE>
struct is_trivially_default_constructible;

}  // close namespace bsl

namespace BloombergLP {
namespace bslmf {


#if defined(BSLS_PLATFORM_CMP_IBM)
                    // =============================================
                    // struct IsTriviallyDefaultConstructible_Scalar
                    // =============================================

template <class TYPE, class = void>
struct IsTriviallyDefaultConstructible_Scalar : bsl::false_type {};
template <class TYPE>
struct IsTriviallyDefaultConstructible_Scalar<TYPE, BSLMF_VOIDTYPE(TYPE[])>
    : bsl::true_type {
    // This implementation-detail trait determines whether 'TYPE' is a scalar
    // type (an arithmetic type, enumeration, pointer, or pointer-to-member).
    // This implementation takes advantage of a previous layer of filtering
    // handling all class-types, and only object types being valid as both
    // array elements, and return values from functions.  Note that the test
    // for valid function return type only adds array types to the filter, and
    // we separately specialize for arrays already, so the 'BSLMF_VOIDTYPE'
    // filter is simplified to checking for valid array elements.
};

                    // ==========================================
                    // struct IsTriviallyDefaultConstructible_Imp
                    // ==========================================

template <class TYPE, class = void>
struct IsTriviallyDefaultConstructible_Imp
    : IsTriviallyDefaultConstructible_Scalar<TYPE>::type {};

template <class TYPE>
struct IsTriviallyDefaultConstructible_Imp<TYPE, BSLMF_VOIDTYPE(int TYPE::*)>
    : DetectNestedTrait<TYPE, bsl::is_trivially_default_constructible>::type {
};
#else
                    // ==========================================
                    // struct IsTriviallyDefaultConstructible_Imp
                    // ==========================================

# if defined(BSLS_PLATFORM_CMP_MSVC)
#   pragma warning(push)
#   pragma warning(disable: 4180)  // cv-qualifier has no effect on function type
# elif defined(BSLS_PLATFORM_CMP_SUN)
#   pragma error_messages (off, functypequal)
# endif

template <class TYPE, class = void>
struct IsTriviallyDefaultConstructible_Imp
    : bsl::is_const<const TYPE>::type {}; // false for functions and references

template <class TYPE>
struct IsTriviallyDefaultConstructible_Imp<TYPE, BSLMF_VOIDTYPE(int TYPE::*)>
    : DetectNestedTrait<TYPE, bsl::is_trivially_default_constructible>::type {
};

# if defined(BSLS_PLATFORM_CMP_MSVC)
#   pragma warning(pop)
# elif defined(BSLS_PLATFORM_CMP_SUN)
#   pragma error_messages (default, functypequal)
# endif

#endif  // BSLS_PLATFORM_CMP_IBM

}  // close package namespace
}  // close enterprise namespace

namespace bsl {

                    // =========================================
                    // struct is_trivially_default_constructible
                    // =========================================

template <class TYPE>
struct is_trivially_default_constructible
    : BloombergLP::bslmf::IsTriviallyDefaultConstructible_Imp<TYPE>::type {
    // This 'struct' template implements a meta-function to determine whether
    // the (template parameter) 'TYPE' is trivially default-constructible.
    // This 'struct' derives from 'bsl::true_type' if the 'TYPE' is trivially
    // default-constructible, and from 'bsl::false_type' otherwise.  This
    // meta-function has the same syntax as the
    // 'is_trivially_default_constructible' meta-function defined in the C++11
    // standard [meta.unary.prop]; however, this meta-function can
    // automatically determine the value for the following types only:
    // reference types, fundamental types, enums, pointers to members, and
    // types declared to have the 'bsl::is_trivially_default_constructible'
    // trait using the 'BSLMF_NESTED_TRAIT_DECLARATION' macro (and the value
    // for other types defaults to 'false').  To support other trivially
    // default-constructible types, this template must be specialized to
    // inherit from 'bsl::true_type' for them.
};

template <class TYPE>
struct is_trivially_default_constructible<const TYPE>
    :  is_trivially_default_constructible<TYPE>::type {
    // This partial specialization ensures that const-qualified types have the
    // same result as their element type.
};

template <class TYPE>
struct is_trivially_default_constructible<volatile TYPE>
    :  is_trivially_default_constructible<TYPE>::type {
    // This partial specialization ensures that volatile-qualified types have
    // the same result as their element type.
};

template <class TYPE>
struct is_trivially_default_constructible<const volatile TYPE>
    :  is_trivially_default_constructible<TYPE>::type {
    // This partial specialization ensures that const-volatile-qualified types
    // have the same result as their element type.
};

template <class TYPE, size_t LEN>
struct is_trivially_default_constructible<TYPE[LEN]>
    :  is_trivially_default_constructible<TYPE>::type {
    // This partial specialization ensures that array types have the same
    // result as their element type.
};

template <class TYPE, size_t LEN>
struct is_trivially_default_constructible<const TYPE[LEN]>
    :  is_trivially_default_constructible<TYPE>::type {
    // This partial specialization ensures that const-qualified array types
    // have the same result as their element type.
};

template <class TYPE, size_t LEN>
struct is_trivially_default_constructible<volatile TYPE[LEN]>
    :  is_trivially_default_constructible<TYPE>::type {
    // This partial specialization ensures that volatile-qualified array types
    // have the same result as their element type.
};

template <class TYPE, size_t LEN>
struct is_trivially_default_constructible<const volatile TYPE[LEN]>
    :  is_trivially_default_constructible<TYPE>::type {
    // This partial specialization ensures that const-volatile-qualified array
    // types have the same result as their element type.
};

template <class TYPE>
struct is_trivially_default_constructible<TYPE[]>
    :  is_trivially_default_constructible<TYPE>::type {
    // This partial specialization ensures that array-of-unknown-bound types
    // have the same result as their element type.
};

template <class TYPE>
struct is_trivially_default_constructible<const TYPE[]>
    :  is_trivially_default_constructible<TYPE>::type {
    // This partial specialization ensures that const-qualified
    // array-of-unknown-bound types have the same result as their element type.
};

template <class TYPE>
struct is_trivially_default_constructible<volatile TYPE[]>
    :  is_trivially_default_constructible<TYPE>::type {
    // This partial specialization ensures that volatile-qualified
    // array-of-unknown-bound types have the same result as their element type.
};

template <class TYPE>
struct is_trivially_default_constructible<const volatile TYPE[]>
    :  is_trivially_default_constructible<TYPE>::type {
    // This partial specialization ensures that const-volatile-qualified
    // array-of-unknown-bound types have the same result as their element type.
};

template <class TYPE>
struct is_trivially_default_constructible<TYPE&> : false_type {
    // This partial specialization ensures that references are never detected
    // as trivially default constructible.  Note that this specialization is
    // required for the Sun CC compiler (last tested with version 12.3) but
    // provides a useful short-circuit on other platforms, so is retained
    // unconditionally.  There is no motivation to detect support and add a
    // similar overload for rvalue references.
};

template <>
struct is_trivially_default_constructible<void> : false_type {
    // This explicit specialization ensures that 'void' is never detected as
    // trivially default constructible.  Note that this specialization is
    // required for the Sun CC compiler (last tested with version 12.3) but
    // provides a useful short-circuit on other platforms, so is retained
    // unconditionally.
};

#if defined(BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES)
template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_trivially_default_constructible_v
                             = is_trivially_default_constructible<TYPE>::value;
    // This template variable represents the result value of the
    // 'bsl::is_trivially_default_constructible' meta-function.
#endif

}  // close namespace bsl

#endif

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
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
