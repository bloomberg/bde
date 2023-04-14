// bslmf_islvaluereference.h                                          -*-C++-*-
#ifndef INCLUDED_BSLMF_ISLVALUEREFERENCE
#define INCLUDED_BSLMF_ISLVALUEREFERENCE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for lvalue reference types.
//
//@CLASSES:
//  bsl::is_lvalue_reference: standard meta-function for lvalue reference types
//  bsl::is_lvalue_reference_v: the result value of the meta-function
//
//@SEE_ALSO: bslmf_integralconstant
//
//@DESCRIPTION: This component defines a meta-function,
// 'bsl::is_lvalue_reference' and a template variable
// 'bsl::is_lvalue_reference_v', that represents the result value of the
// 'bsl::is_lvalue_reference' meta-function, that may be used to query whether
// a type is an lvalue reference type.
//
// 'bsl::is_lvalue_reference' meets the requirements of the
// 'is_lvalue_reference' template defined in the C++11 standard
// [meta.unary.cat].
//
// Note that the template variable 'is_lvalue_reference_v' is defined in the
// C++17 standard as an inline variable.  If the current compiler supports the
// inline variable C++17 compiler feature, 'bsl::is_lvalue_reference_v' is
// defined as an 'inline constexpr bool' variable.  Otherwise, if the compiler
// supports the variable templates C++14 compiler feature,
// 'bsl::is_lvalue_reference_v' is defined as a non-inline 'constexpr bool'
// variable.  See 'BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES' and
// 'BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES' macros in
// bsls_compilerfeatures component for details.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify Lvalue Reference Types
/// - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a set of types are lvalue reference
// types.
//
// Now, we instantiate the 'bsl::is_lvalue_reference' template for both a
// non-reference type and an lvalue reference type, and assert the 'value'
// static data member of each instantiation:
//..
//  assert(false == bsl::is_lvalue_reference<int>::value);
//  assert(true  == bsl::is_lvalue_reference<int&>::value);
//..
// Note that if the current compiler supports the variable templates C++14
// feature then we can re-write the snippet of code above using the
// 'bsl::is_lvalue_reference_v' variable as follows:
//..
//#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
//  assert(false == bsl::is_lvalue_reference_v<int>);
//  assert(true  == bsl::is_lvalue_reference_v<int&>);
//#endif
//..

#include <bslscm_version.h>

#include <bslmf_integralconstant.h>

#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>

namespace bsl {

                          // ==========================
                          // struct is_lvalue_reference
                          // ==========================

template <class t_TYPE>
struct is_lvalue_reference : false_type {
    // This 'struct' template provides a meta-function to determine whether the
    // (template parameter) 't_TYPE' is a (possibly cv-qualified) lvalue
    // reference type.  This generic default template derives from
    // 'bsl::false_type'.  A template specialization is provided (below) that
    // derives from 'bsl::true_type'.
};

template <class t_TYPE>
struct is_lvalue_reference<t_TYPE&> : true_type {
    // This partial specialization of 'is_lvalue_reference' derives from
    // 'bsl::true_type' for when the (template parameter) 't_TYPE' is an lvalue
    // reference type.
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
template <class t_TYPE>
BSLS_KEYWORD_INLINE_VARIABLE constexpr bool is_lvalue_reference_v =
                                            is_lvalue_reference<t_TYPE>::value;
    // This template variable represents the result value of the
    // 'bsl::is_lvalue_reference' meta-function.
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
