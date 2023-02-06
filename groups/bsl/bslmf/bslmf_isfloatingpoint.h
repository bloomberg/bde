// bslmf_isfloatingpoint.h                                            -*-C++-*-
#ifndef INCLUDED_BSLMF_ISFLOATINGPOINT
#define INCLUDED_BSLMF_ISFLOATINGPOINT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for floating-point types.
//
//@CLASSES:
//  bsl::is_floating_point: meta-function for determining floating-point types
//  bsl::is_floating_point_v: the result value of 'bsl::is_floating_point'
//
//@SEE_ALSO: bslmf_integralconstant
//
//@DESCRIPTION: This component defines a meta-function,
// 'bsl::is_floating_point' and a template variable 'bsl::is_floating_point_v',
// that represents the result value of the 'bsl::is_floating_point'
// meta-function, that may be used to query whether a type is a (possibly
// cv-qualified) floating-point type as defined in section 3.9.1.8 of the C++11
// standard [basic.fundamental].
//
// 'bsl::is_floating_point' meets the requirements of the 'is_floating_point'
// template defined in the C++11 standard [meta.unary.cat].
//
// Note that the template variable 'is_floating_point_v' is defined in the
// C++17 standard as an inline variable.  If the current compiler supports the
// inline variable C++17 compiler feature, 'bsl::is_floating_point_v' is
// defined as an 'inline constexpr bool' variable.  Otherwise, if the compiler
// supports the variable templates C++14 compiler feature,
// 'bsl::is_floating_point_v' is defined as a non-inline 'constexpr bool'
// variable.  See 'BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES' and
// 'BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES' macros in
// bsls_compilerfeatures component for details.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify Floating-Point Types
/// - - - - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a particular type is a floating-point
// type.
//
// First, we create two 'typedef's -- a floating-point type and a
// non-floating-point type:
//..
//  typedef void  MyType;
//  typedef float MyFloatingPointType;
//..
// Now, we instantiate the 'bsl::is_floating_point' template for each of the
// 'typedef's and assert the 'value' static data member of each instantiation:
//..
//  assert(false == bsl::is_floating_point<MyType>::value);
//  assert(true  == bsl::is_floating_point<MyFloatingPointType>::value);
//..
// Note that if the current compiler supports the variable templates C++14
// feature, then we can re-write the snippet of code above using the
// 'bsl::is_floating_point_v<T> as follows:
//..
//#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
//  assert(false == bsl::is_floating_point_v<MyType>);
//  assert(true  == bsl::is_floating_point_v<MyFloatingPointType>);
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

                         // ========================
                         // struct is_floating_point
                         // ========================

template <class t_TYPE>
struct is_floating_point : bsl::false_type {
    // This 'struct' template implements the 'is_floating_point' meta-function
    // defined in the C++11 standard [meta.unary.cat] to determine if the
    // (template parameter) 't_TYPE' is a floating-point type.  This 'struct'
    // derives from 'bsl::true_type' if the 't_TYPE' is a floating-point type,
    // and 'bsl::false_type' otherwise.
};

template <>
struct is_floating_point<float> : bsl::true_type {
    // This explicit specialization of 'is_floating_point', for when the
    // (template parameter) 't_TYPE' is 'float', derives from 'bsl::true_type'.
};

template <>
struct is_floating_point<double> : bsl::true_type {
    // This explicit specialization of 'is_floating_point', for when the
    // (template parameter) 't_TYPE' is 'double', derives from
    // 'bsl::true_type'.
};

template <>
struct is_floating_point<long double> : bsl::true_type {
    // This explicit specialization of 'is_floating_point', for when the
    // (template parameter) 't_TYPE' is 'long double', derives from
    // 'bsl::true_type'.
};

template <class t_TYPE>
struct is_floating_point<const t_TYPE> : is_floating_point<t_TYPE>::type {
    // This partial specialization of 'is_floating_point', for when the
    // (template parameter) 't_TYPE' is 'const'-qualified delegates to the
    // non-cv-qualified primary template.
};

template <class t_TYPE>
struct is_floating_point<volatile t_TYPE> : is_floating_point<t_TYPE>::type {
    // This partial specialization of 'is_floating_point', for when the
    // (template parameter) 't_TYPE' is 'volatile'-qualified delegates to the
    // non-cv-qualified primary template.
};

template <class t_TYPE>
struct is_floating_point<const volatile t_TYPE>
: is_floating_point<t_TYPE>::type {
    // This partial specialization of 'is_floating_point', for when the
    // (template parameter) 't_TYPE' is 'const volatile'-qualified delegates to
    // the non-cv-qualified primary template.
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
template <class t_TYPE>
BSLS_KEYWORD_INLINE_VARIABLE constexpr bool is_floating_point_v =
                                              is_floating_point<t_TYPE>::value;
    // This template variable represents the result value of the
    // 'bsl::is_floating_point' meta-function.
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
