// bslmf_isfundamental.h                                              -*-C++-*-
#ifndef INCLUDED_BSLMF_ISFUNDAMENTAL
#define INCLUDED_BSLMF_ISFUNDAMENTAL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for determining fundamental types.
//
//@CLASSES:
//  bsl::is_fundamental: standard meta-function for detecting fundamental types
//  bsl::is_fundamental_v: the result value of 'bsl::is_fundamental'
//  bslmf::IsFundamental: meta-function for detecting fundamental types
//
//@SEE_ALSO: bslmf_isenum, bslmf_ispointer
//
//@DESCRIPTION: This component defines two meta-functions,
// 'bsl::is_fundamental' and 'BloombergLP::bslmf::IsFundamental' and a template
// variable 'bsl::is_fundamental_v', that represents the result value of the
// 'bsl::is_fundamental' meta-function.  All these meta-functions may be used
// to query whether a type is a fundamental type.
//
// 'bsl::is_fundamental' meets the requirements of the 'is_fundamental'
// template defined in the C++11 standard [meta.unary.comp], while
// 'bslmf::Fundamental' was devised before 'is_fundamental' was standardized.
//
// The two meta-functions are functionally equivalent except on reference of
// fundamental types.  Lvalue-references to fundamental types are determined as
// fundamental types by 'bslmf::IsFundamental', but not by
// 'bsl::is_fundamental'.  Rvalue-references, on compilers that support them,
// are not deemed to be fundamental by either trait.  In expected use, the
// result for 'bsl::is_fundamental' is indicated by the class member 'value',
// while the result for 'bslmf::Fundamental' is indicated by the class member
// 'VALUE'.  In practice, both traits support both names of the result value,
// although the all-caps 'VALUE' form is deprecated.
//
// Note that 'bsl::is_fundamental' should be preferred over
// 'bslmf::Fundamental', and in general, should be used by new components.
//
// Also note that the template variable 'is_fundamental_v' is defined in the
// C++17 standard as an inline variable.  If the current compiler supports the
// inline variable C++17 compiler feature, 'bsl::is_fundamental_v' is defined
// as an 'inline constexpr bool' variable.  Otherwise, if the compiler supports
// the variable templates C++14 compiler feature, 'bsl::is_fundamental_v' is
// defined as a non-inline 'constexpr bool' variable.  See
// 'BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES' and
// 'BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES' macros in
// bsls_compilerfeatures component for details.
//
// The C++ fundamental types are described in the C++ standard
// [basic.fundamental], and consist of the following distinct types, and
// cv-qualified variations of these types:
//..
//  bool
//  char
//  signed char
//  unsigned char
//  wchar_t
//  char16_t
//  char32_t
//  short int                                    (also referred to as "short")
//  unsigned short int                  (also referred to as "unsigned short")
//  int
//  unsigned int
//  long int                                      (also referred to as "long")
//  unsigned long int                    (also referred to as "unsigned long")
//  long long int                            (also referred to as "long long")
//  unsigned long long int          (also referred to as "unsigned long long")
//  float
//  double
//  long double
//  void
//  nullptr_t
//..
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify Fundamental Types
///- - - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a set of types are fundamental types.
//
// Now, we instantiate the 'bsl::is_fundamental' template for several
// non-fundamental and fundamental types, and assert the 'value' static data
// member of each instantiation:
//..
//  assert(true  == bsl::is_fundamental<int>::value);
//  assert(false == bsl::is_fundamental<int&>::value);
//  assert(true  == bsl::is_fundamental<long long  >::value);
//  assert(false == bsl::is_fundamental<long long *>::value);
//..
// Note that if the current compiler supports the variable templates C++14
// feature then we can re-write the snippet of code above using the
// 'bsl::is_fundamental_v' variable as follows:
//..
//#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
//  assert(true  == bsl::is_fundamental_v<int>);
//  assert(false == bsl::is_fundamental_v<int&>);
//  assert(true  == bsl::is_fundamental_v<long long  >);
//  assert(false == bsl::is_fundamental_v<long long *>);
//#endif
//..

#include <bslscm_version.h>

#include <bslmf_integralconstant.h>
#include <bslmf_isarithmetic.h>
#include <bslmf_isvoid.h>
#include <bslmf_removecv.h>

#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_nullptr.h>

namespace BloombergLP {
namespace bslmf {

                         // ========================
                         // struct IsFundamental_Imp
                         // ========================

template <class t_TYPE>
struct IsFundamental_Imp : bsl::false_type {
    // This 'struct' template implements a meta-function to determine whether
    // the (template parameter) 't_TYPE' is a (non-cv-qualified) fundamental
    // type.  This generic default template derives from 'bsl::false_type'.
    // Template specializations for fundamental types are provided (below) that
    // derive from 'bsl::true_type'.
};

template <> struct IsFundamental_Imp<bool> : bsl::true_type {
    // This partial specialization of 'IsFundamental_Imp' derives from
    // 'bsl::true_type' for when the (template parameter) 't_TYPE' is 'bool'.
};

template <> struct IsFundamental_Imp<char> : bsl::true_type {
    // This partial specialization of 'IsFundamental_Imp' derives from
    // 'bsl::true_type' for when the (template parameter) 't_TYPE' is 'char'.
};

template <> struct IsFundamental_Imp<signed char> : bsl::true_type {
    // This partial specialization of 'IsFundamental_Imp' derives from
    // 'bsl::true_type' for when the (template parameter) 't_TYPE' is
    // 'signed char'.
};

template <> struct IsFundamental_Imp<unsigned char> : bsl::true_type {
    // This partial specialization of 'IsFundamental_Imp' derives from
    // 'bsl::true_type' for when the (template parameter) 't_TYPE' is
    // 'unsigned char'.
};

#if defined BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE
template <> struct IsFundamental_Imp<char8_t> : bsl::true_type {
    // This partial specialization of 'IsFundamental_Imp' derives from
    // 'bsl::true_type' for when the (template parameter) 'TYPE' is 'char8_t'.
};
#endif

template <> struct IsFundamental_Imp<wchar_t> : bsl::true_type {
    // This partial specialization of 'IsFundamental_Imp' derives from
    // 'bsl::true_type' for when the (template parameter) 't_TYPE' is
    // 'wchar_t'.
};

template <> struct IsFundamental_Imp<short> : bsl::true_type {
    // This partial specialization of 'IsFundamental_Imp' derives from
    // 'bsl::true_type' for when the (template parameter) 't_TYPE' is 'short'.
};

template <> struct IsFundamental_Imp<unsigned short> : bsl::true_type {
    // This partial specialization of 'IsFundamental_Imp' derives from
    // 'bsl::true_type' for when the (template parameter) 't_TYPE' is
    // 'unsigned short'.
};

template <> struct IsFundamental_Imp<int> : bsl::true_type {
    // This partial specialization of 'IsFundamental_Imp' derives from
    // 'bsl::true_type' for when the (template parameter) 't_TYPE' is 'int'.
};

template <> struct IsFundamental_Imp<unsigned int> : bsl::true_type {
    // This partial specialization of 'IsFundamental_Imp' derives from
    // 'bsl::true_type' for when the (template parameter) 't_TYPE' is
    // 'unsigned int'.
};

template <> struct IsFundamental_Imp<long> : bsl::true_type {
    // This partial specialization of 'IsFundamental_Imp' derives from
    // 'bsl::true_type' for when the (template parameter) 't_TYPE' is 'long'.
};

template <> struct IsFundamental_Imp<unsigned long> : bsl::true_type {
    // This partial specialization of 'IsFundamental_Imp' derives from
    // 'bsl::true_type' for when the (template parameter) 't_TYPE' is
    // 'unsigned long'.
};

template <> struct IsFundamental_Imp<long long> : bsl::true_type {
    // This partial specialization of 'IsFundamental_Imp' derives from
    // 'bsl::true_type' for when the (template parameter) 't_TYPE' is
    // 'long long'.
};

template <> struct IsFundamental_Imp<unsigned long long> : bsl::true_type {
    // This partial specialization of 'IsFundamental_Imp' derives from
    // 'bsl::true_type' for when the (template parameter) 't_TYPE' is
    // 'unsigned long long'
};

template <> struct IsFundamental_Imp<float> : bsl::true_type {
    // This partial specialization of 'IsFundamental_Imp' derives from
    // 'bsl::true_type' for when the (template parameter) 't_TYPE' is 'float'.
};

template <>
struct IsFundamental_Imp<double> : bsl::true_type {
    // This partial specialization of 'IsFundamental_Imp' derives from
    // 'bsl::true_type' for when the (template parameter) 't_TYPE' is 'double'.
};

template <> struct IsFundamental_Imp<long double> : bsl::true_type {
    // This partial specialization of 'IsFundamental_Imp' derives from
    // 'bsl::true_type' for when the (template parameter) 't_TYPE' is
    // 'long double'.
};

template <> struct IsFundamental_Imp<void> : bsl::true_type {
    // This partial specialization of 'IsFundamental_Imp' derives from
    // 'bsl::true_type' for when the (template parameter) 't_TYPE' is 'void'.
};

#if defined(BSLS_COMPILERFEATURES_SUPPORT_NULLPTR)
template <> struct IsFundamental_Imp<bsl::nullptr_t> : bsl::true_type {
    // This partial specialization of 'IsFundamental_Imp' derives from
    // 'bsl::true_type' for when the (template parameter) 't_TYPE' is 'void'.
};
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES)
template <> struct IsFundamental_Imp<char16_t> : bsl::true_type {
    // This partial specialization of 'IsFundamental_Imp' derives from
    // 'bsl::true_type' for when the (template parameter) 't_TYPE' is 'void'.
};

template <> struct IsFundamental_Imp<char32_t> : bsl::true_type {
    // This partial specialization of 'IsFundamental_Imp' derives from
    // 'bsl::true_type' for when the (template parameter) 't_TYPE' is 'void'.
};
#endif


                         // ====================
                         // struct IsFundamental
                         // ====================

template <class t_TYPE>
struct IsFundamental
: IsFundamental_Imp<typename bsl::remove_cv<t_TYPE>::type>::type {
    // This 'struct' template implements a meta-function for checking if a type
    // is fundamental, or a reference to a fundamental type.  The static
    // constant 'VALUE' member will be 1 if 't_TYPE' is fundamental and 0
    // otherwise.
};

template <class t_TYPE>
struct IsFundamental<t_TYPE&>
: IsFundamental_Imp<typename bsl::remove_cv<t_TYPE>::type>::type {
    // This specialization of 'IsFundamental' causes lvalue-references to be
    // treated as their underlying (non-reference) types.
};

}  // close package namespace
}  // close enterprise namespace

namespace bsl {

                         // =====================
                         // struct is_fundamental
                         // =====================

template <class t_TYPE>
struct is_fundamental
: integral_constant<bool,
                    is_arithmetic<t_TYPE>::value || is_void<t_TYPE>::value> {
    // This 'struct' template implements a meta-function for checking if a type
    // is fundamental as defined in the C++11 standard [basic.fundamental].
    // Note that this is subtly differemt from 'bslmf::IsFundamental', which
    // also returns 'true_type' for references to fundamental types.
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
template <class t_TYPE>
BSLS_KEYWORD_INLINE_VARIABLE constexpr bool is_fundamental_v =
                                                 is_fundamental<t_TYPE>::value;
    // This template variable represents the result value of the
    // 'bsl::is_fundamental' meta-function.
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_NULLPTR)
template <>
struct is_fundamental<bsl::nullptr_t> : bsl::true_type {
    // Explicit specialization to confirm that the type of 'nullptr' is
    // fundamental.
};

template <>
struct is_fundamental<const bsl::nullptr_t> : bsl::true_type {
    // Explicit specialization to confirm that the type of 'const nullptr' is
    // fundamental.
};

template <>
struct is_fundamental<volatile bsl::nullptr_t> : bsl::true_type {
    // Explicit specialization to confirm that the type of 'volatile nullptr'
    // is fundamental.
};

template <>
struct is_fundamental<const volatile bsl::nullptr_t> : bsl::true_type {
    // Explicit specialization to confirm that the type of
    // 'const volatile nullptr' is fundamental.
};

#endif

}  // close namespace bsl

#ifndef BDE_OPENSOURCE_PUBLICATION  // BACKWARD_COMPATIBILITY
// ============================================================================
//                           BACKWARD COMPATIBILITY
// ============================================================================

#ifdef bslmf_IsFundamental
#undef bslmf_IsFundamental
#endif
#define bslmf_IsFundamental bslmf::IsFundamental
    // This alias is defined for backward compatibility.
#endif  // BDE_OPENSOURCE_PUBLICATION -- BACKWARD_COMPATIBILITY

#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
