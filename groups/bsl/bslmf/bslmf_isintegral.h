// bslmf_isintegral.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLMF_ISINTEGRAL
#define INCLUDED_BSLMF_ISINTEGRAL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for integral types.
//
//@CLASSES:
//  bsl::is_integral: standard meta-function for determining integral types
//  bsl::is_integral_v: the result value of 'bsl::is_integral'
//
//@SEE_ALSO: bslmf_integralconstant
//
//@DESCRIPTION: This component defines a meta-function, 'bsl::is_integral' and
// a template variable 'bsl::is_integral_v', that represents the result value
// of the 'bsl::is_integral' meta-function, that may be used to query whether a
// type is an integral type as defined in section 3.9.1.7 of the C++11 standard
// [basic.fundamental] (excluding those types that were introduced in C++11).
//
// 'bsl::is_integral' meets the requirements of the 'is_integral' template
// defined in the C++11 standard [meta.unary.cat] except that it may not
// correctly evaluate types introduced in C++11.
//
// Note that the template variable 'is_integral_v' is defined in the C++17
// standard as an inline variable.  If the current compiler supports the inline
// variable C++17 compiler feature, 'bsl::is_integral_v' is defined as an
// 'inline constexpr bool' variable.  Otherwise, if the compiler supports the
// variable templates C++14 compiler feature, 'bsl::is_integral_v' is defined
// as a non-inline 'constexpr bool' variable.  See
// 'BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES' and
// 'BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES' macros in
// bsls_compilerfeatures component for details.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify Integral Types
/// - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a particular type is an integral
// type.
//
// First, we create two 'typedef's -- an integral type and a non-integral type:
//..
//  typedef void MyType;
//  typedef int  MyIntegralType;
//..
// Now, we instantiate the 'bsl::is_integral' template for each of the
// 'typedef's and assert the 'value' static data member of each instantiation:
//..
//  assert(false == bsl::is_integral<MyType>::value);
//  assert(true  == bsl::is_integral<MyIntegralType>::value);
//..
// Note that if the current compiler supports the variable templates C++14
// feature then we can re-write the snippet of code above using the
// 'bsl::is_integral_v' variable as follows:
//..
//#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
//  assert(false == bsl::is_integral_v<MyType>);
//  assert(true  == bsl::is_integral_v<MyIntegralType>);
//#endif
//..

#include <bslscm_version.h>

#include <bslmf_integralconstant.h>

#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bslmf_removecv.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

namespace bsl {

                         // ==================
                         // struct is_integral
                         // ==================

template <class t_TYPE>
struct is_integral : bsl::false_type {
    // This 'struct' template implements the 'is_integral' meta-function
    // defined in the C++11 standard [meta.unary.cat] to determine if the
    // (template parameter) 't_TYPE' is an integral type.  This 'struct'
    // derives from 'bsl::true_type' if the 't_TYPE' is an integral type, and
    // 'bsl::false_type' otherwise.
};

template <class t_TYPE>
struct is_integral<const t_TYPE> : is_integral<t_TYPE>::type {
    // This partial specialization of 'is_integral', for when the (template
    // parameter) 't_TYPE' is 'const'-qualified delegates to the
    // non-cv-qualified primary template.
};

template <class t_TYPE>
struct is_integral<volatile t_TYPE> : is_integral<t_TYPE>::type {
    // This partial specialization of 'is_integral', for when the (template
    // parameter) 't_TYPE' is 'volatile'-qualified delegates to the
    // non-cv-qualified primary template.
};

template <class t_TYPE>
struct is_integral<const volatile t_TYPE> : is_integral<t_TYPE>::type {
    // This partial specialization of 'is_integral', for when the (template
    // parameter) 't_TYPE' is 'const volatile'-qualified delegates to the
    // non-cv-qualified primary template.
};

template <>
struct is_integral<bool> : bsl::true_type {
    // This explicit specialization of 'is_integral', for when the (template
    // parameter) 't_TYPE' is 'bool', derives from 'bsl::true_type'.
};

template <>
struct is_integral<char> : bsl::true_type {
    // This explicit specialization of 'is_integral', for when the (template
    // parameter) 't_TYPE' is 'char', derives from 'bsl::true_type'.
};

template <>
struct is_integral<wchar_t> : bsl::true_type {
    // This explicit specialization of 'is_integral', for when the (template
    // parameter) 't_TYPE' is 'wchar_t', derives from 'bsl::true_type'.
};

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE)
template <>
struct is_integral<char8_t> : bsl::true_type {
     // This explicit specialization of 'is_integral', for when the (template
     // parameter) 'TYPE' is 'wchar_t', derives from 'bsl::true_type'.
};
#endif

#if defined BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES
template <>
struct is_integral<char16_t> : bsl::true_type {
    // This explicit specialization of 'is_integral', for when the (template
    // parameter) 't_TYPE' is 'wchar_t', derives from 'bsl::true_type'.
};

template <>
struct is_integral<char32_t> : bsl::true_type {
    // This explicit specialization of 'is_integral', for when the (template
    // parameter) 't_TYPE' is 'wchar_t', derives from 'bsl::true_type'.
};
#endif // BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES

template <>
struct is_integral<signed char> : bsl::true_type {
    // This explicit specialization of 'is_integral', for when the (template
    // parameter) 't_TYPE' is 'signed char', derives from 'bsl::true_type'.
};

template <>
struct is_integral<unsigned char> : bsl::true_type {
    // This explicit specialization of 'is_integral', for when the (template
    // parameter) 't_TYPE' is 'unsigned char', derives from 'bsl::true_type'.
};

template <>
struct is_integral<short int> : bsl::true_type {
    // This explicit specialization of 'is_integral', for when the (template
    // parameter) 't_TYPE' is 'short', derives from 'bsl::true_type'.
};

template <>
struct is_integral<unsigned short int> : bsl::true_type {
    // This explicit specialization of 'is_integral', for when the (template
    // parameter) 't_TYPE' is 'unsigned short', derives from 'bsl::true_type'.
};

template <>
struct is_integral<int> : bsl::true_type {
    // This explicit specialization of 'is_integral', for when the (template
    // parameter) 't_TYPE' is 'int', derives from 'bsl::true_type'.
};

template <>
struct is_integral<unsigned int> : bsl::true_type {
    // This explicit specialization of 'is_integral', for when the (template
    // parameter) 't_TYPE' is 'unsigned int', derives from 'bsl::true_type'.
};

template <>
struct is_integral<long int> : bsl::true_type {
    // This explicit specialization of 'is_integral', for when the (template
    // parameter) 't_TYPE' is 'long int', derives from 'bsl::true_type'.
};

template <>
struct is_integral<unsigned long int> : bsl::true_type {
    // This explicit specialization of 'is_integral', for when the (template
    // parameter) 't_TYPE' is 'unsigned long int', derives from
    // 'bsl::true_type'.
};

template <>
struct is_integral<long long int> : bsl::true_type {
    // This explicit specialization of 'is_integral', for when the (template
    // parameter) 't_TYPE' is 'long long int', derives from 'bsl::true_type'.
};

template <>
struct is_integral<unsigned long long int> : bsl::true_type {
    // This explicit specialization of 'is_integral', for when the (template
    // parameter) 't_TYPE' is 'unsigned long long int', derives from
    // 'bsl::true_type'.
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
template <class t_TYPE>
BSLS_KEYWORD_INLINE_VARIABLE constexpr bool is_integral_v =
                                                    is_integral<t_TYPE>::value;
    // This template variable represents the result value of the
    // 'bsl::is_integral' meta-function.
#endif

}  // close namespace bsl

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
