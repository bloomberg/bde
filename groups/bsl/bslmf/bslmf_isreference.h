// bslmf_isreference.h                                                -*-C++-*-
#ifndef INCLUDED_BSLMF_ISREFERENCE
#define INCLUDED_BSLMF_ISREFERENCE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function to test reference types.
//
//@CLASSES:
//  bsl::is_reference: standard meta-function for testing reference types
//  bsl::is_reference_v: the result value of the standard meta-function
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component defines a meta-function, 'bsl::is_reference' and
// a template variable 'bsl::is_reference_v', that represents the result value
// of the 'bsl::is_class' meta-function, that may be used to query whether a
// type is an (lvalue or rvalue) reference type.
//
// 'bsl::is_reference' meets the requirements of the 'is_reference' template
// defined in the C++11 standard [meta.unary.comp].
//
// Note that the template variable 'is_reference_v' is defined in the C++17
// standard as an inline variable.  If the current compiler supports the inline
// variable C++17 compiler feature, 'bsl::is_reference_v' is defined as an
// 'inline constexpr bool' variable.  Otherwise, if the compiler supports the
// variable templates C++14 compiler feature, 'bsl::is_reference_v' is defined
// as a non-'const' 'constexpr bool' variable.  See
// 'BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES' and
// 'BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES' macros in
// bsls_compilerfeatures component for details.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify Reference Types
///- - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a set of types are (lvalue or rvalue)
// reference types.
//
// Now, we instantiate the 'bsl::is_reference' template for a non-reference
// type, an lvalue reference type, and an rvalue reference type, and assert the
// 'value' static data member of each instantiation:
//..
//  assert(false == bsl::is_reference<int>::value);
//  assert(true  == bsl::is_reference<int&>::value);
//#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
//  assert(true  == bsl::is_reference<int&&>::value);
//#endif
//..
// Note that rvalue reference is a feature introduced in the C++11 standand,
// and may not be supported by all compilers.
//
// Also note that if the current compiler supports the variable templates C++14
// feature then we can re-write the snippet of code above using the
// 'bsl::is_reference_v' variable as follows:
//..
//#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
//  assert(false == bsl::is_reference_v<int>);
//  assert(true  == bsl::is_reference_v<int&>);
//  assert(true  == bsl::is_reference_v<int&&>);
//#endif
//..

#include <bslscm_version.h>

#include <bslmf_integralconstant.h>

#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bslmf_islvaluereference.h>
#include <bslmf_isrvaluereference.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

namespace bsl {

                         // ===================
                         // struct is_reference
                         // ===================

// IMPLEMENTATION NOTE:  We originally implemented this trait in terms of
// 'is_lvalue_reference' and 'is_rvalue_reference' but ran into an issue on
// Sun where the compiler was reporting that the maximum depth of template
// expansion was being exceeded.

template <class t_TYPE>
struct is_reference : false_type {
    // This 'struct' template implements the 'is_reference' meta-function
    // defined in the C++11 standard [meta.unary.comp] to determine if the
    // (template parameter) 't_TYPE' is a (lvalue or rvalue) reference type.
    //
    // This 'struct' derives from 'bsl::false_type' with specializations to
    // follow for lvalue and rvalue references.
};

template <class t_TYPE>
struct is_reference<t_TYPE&> : true_type {
    // This 'struct' template implements the 'is_reference' meta-function
    // defined in the C++11 standard [meta.unary.comp] to determine if the
    // (template parameter) 't_TYPE' is a (lvalue or rvalue) reference type.
    //
    // This specialization for lvalue references derives from 'bsl::true_type'.
};

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template <class t_TYPE>
struct is_reference<t_TYPE&&> : true_type {
    // This 'struct' template implements the 'is_reference' meta-function
    // defined in the C++11 standard [meta.unary.comp] to determine if the
    // (template parameter) 't_TYPE' is a (lvalue or rvalue) reference type.
    //
    // This specialization for rvalue references derives from 'bsl::true_type.
};
#endif

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
template <class t_TYPE>
BSLS_KEYWORD_INLINE_VARIABLE constexpr bool is_reference_v =
                                                   is_reference<t_TYPE>::value;
    // This template variable represents the result value of the
    // 'bsl::is_reference' meta-function.
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
