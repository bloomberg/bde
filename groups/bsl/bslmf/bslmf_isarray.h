// bslmf_isarray.h                                                    -*-C++-*-
#ifndef INCLUDED_BSLMF_ISARRAY
#define INCLUDED_BSLMF_ISARRAY

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for array types.
//
//@CLASSES:
//  bsl::is_array:           meta-function for detecting array types
//  bslmf::IsArray: non-standard meta-function for detecting array types
//  bsl::is_bounded_array:   meta-function for detecting bounded array types
//  bsl::is_unbounded_array: meta-function for detecting unbounded array types
//  bsl::is_array_v:           value of the 'is_array' meta-function
//  bsl::is_bounded_array_v:   value of the 'is_bounded_array' meta-function
//  bsl::is_unbounded_array_v: value of the 'is_unbounded_array' meta-function
//
//@DESCRIPTION: This component defines four meta-functions, -- 'bsl::is_array',
// 'bsl::is_bounded_array', 'bsl::is_unbounded_array', and
// 'BloombergLP::bslmf::IsArray' and three template variables, --
// 'bsl::is_array_v', 'bsl::is_bounded_array_v', and
// 'bsl::is_unbounded_array_v' --  that contain the result values of the
// corresponding meta-functions.  The 'is_array' meta-functions may be used to
// query whether a type is an array type, while the 'is_bounded_array'
// meta-functions may be used to query whether a type is an array of known
// bound, and the 'is_unbounded_array' meta-functions may be used to query
// whether a type is an array of unknown bound.
//
// 'bsl::is_array' meets the requirements of the 'is_array' template defined in
// the C++11 standard [meta.unary.cat], while 'bslmf::IsArray' was devised
// before 'is_array' was standardized.  The two meta-functions are functionally
// equivalent.  The major difference between them is that the result for
// 'bsl::is_array' is indicated by the class member 'value', while the result
// for 'bslmf::IsArray' is indicated by the class member 'VALUE'.
//
// Note that 'bsl::is_array' should be preferred over 'bslmf::IsArray', and in
// general, should be used by new components.
//
// 'bsl::is_bounded_array' and 'bsl::is_bounded_array' meet the requirements of
// the 'is_bounded_array' and 'is_unbounded_array' template defined in the
// C++20 standard [meta.unary.prop].
//
// Also note that the template variables 'is_array_v', 'is_bounded_array_v',
// and 'is_unbounded_array_v' are defined in the standard as inline variables.
// If the current compiler supports the inline variable C++17 compiler feature,
// these variables are defined as 'inline constexpr bool' variables.
// Otherwise, if the compiler supports the variable templates C++14 compiler
// feature, these variables are defined as non-inline 'constexpr bool'
// variables.  See 'BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES' and
// 'BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES' macros in the
// {'bsls_compilerfeatures'} component for details.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify Array Types
///- - - - - - - - - - - - - - -
// Suppose that we want to assert whether a particular type is an array type.
//
// First, we create three 'typedef's -- a non-array type, an array type with a
// known bound, and an array type and an array type with an unknown bound:
//..
//  typedef int MyNonArrayType;
//  typedef int MySizedArrayType[12];
//  typedef int MyUnsizedArrayType[];
//..
// Now, we instantiate each of the the meta-functions for each of the
// 'typedef's and assert the 'value' static data member of each instantiation:
//..
//  assert(false == bsl::is_array<MyType>::value);
//  assert(true  == bsl::is_array<MySizedArrayType>::value);
//  assert(true  == bsl::is_array<MyUnsizedArrayType>::value);
//
//  assert(false == bsl::is_bounded_array<MyType>::value);
//  assert(true  == bsl::is_bounded_array<MySizedArrayType>::value);
//  assert(false == bsl::is_bounded_array<MyUnsizedArrayType>::value);
//
//  assert(false == bsl::is_unbounded_array<MyType>::value);
//  assert(false == bsl::is_unbounded_array<MySizedArrayType>::value);
//  assert(true  == bsl::is_unbounded_array<MyUnsizedArrayType>::value);
//..
// Note that if the current compiler supports the variable templates C++14
// feature then we can re-write the snippet of code above using the
// 'bsl::is_array_v' variable as follows:
//..
//#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
//  assert(false == bsl::is_array_v<MyNonArrayType>);
//  assert(true  == bsl::is_array_v<MySizedArrayType>);
//  assert(true  == bsl::is_array_v<MyUnsizedArrayType>);
//
//  assert(false == bsl::is_bounded_array_v<MyNonArrayType>);
//  assert(true  == bsl::is_bounded_array_v<MySizedArrayType>);
//  assert(false == bsl::is_bounded_array_v<MyUnsizedArrayType>);
//
//  assert(false == bsl::is_unbounded_array_v<MyNonArrayType>);
//  assert(false == bsl::is_unbounded_array_v<MySizedArrayType>);
//  assert(true  == bsl::is_unbounded_array_v<MyUnsizedArrayType>);
//#endif
//..

#include <bslscm_version.h>

#include <bslmf_integralconstant.h>

#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>

#include <cstddef>       // 'std::size_t'

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <cstdlib>
#endif

namespace bsl {

                         // ===============
                         // struct is_array
                         // ===============

template <class t_TYPE>
struct is_array : false_type {
    // This 'struct' template implements the 'is_array' meta-function defined
    // in the C++11 standard [meta.unary.cat] to determine if the (template
    // parameter) 't_TYPE' is an array type.  This 'struct' derives from
    // 'bsl::true_type' if the 't_TYPE' is an array type, and 'bsl::false_type'
    // otherwise.
};

                  // ========================================
                  // struct is_array<t_TYPE [t_NUM_ELEMENTS]>
                  // ========================================

template <class t_TYPE, std::size_t t_NUM_ELEMENTS>
struct is_array<t_TYPE[t_NUM_ELEMENTS]> : true_type {
    // This specialization of 'is_array', for when the (template parameter)
    // 't_TYPE' is an array type of known bound, derives from 'bsl::true_type'.
};

                         // ==========================
                         // struct is_array<t_TYPE []>
                         // ==========================

template <class t_TYPE>
struct is_array<t_TYPE[]> : true_type {
    // This specialization of 'is_array', for when the (template parameter)
    // 't_TYPE' is an array type of unknown bound, derives from
    // 'bsl::true_type'.
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
template <class t_TYPE>
BSLS_KEYWORD_INLINE_VARIABLE constexpr bool is_array_v =
                                                       is_array<t_TYPE>::value;
    // This template variable represents the result value of the
    // 'bsl::is_array' meta-function.
#endif

                     // =======================
                     // struct is_bounded_array
                     // =======================

template <class t_TYPE>
struct is_bounded_array : false_type {
    // This 'struct' template implements the 'is_bounded_array' meta-function
    // defined in the C++20 standard [meta.unary.cat] to determine if the
    // (template parameter) 't_TYPE' is an array type of known bound.  This
    // 'struct' derives from 'bsl::true_type' if the 't_TYPE' is an array type
    // of known bound, and 'bsl::false_type' otherwise.
};

template <class t_TYPE, std::size_t t_NUM_ELEMENTS>
struct is_bounded_array<t_TYPE[t_NUM_ELEMENTS]> : true_type {
    // This specialization of 'is_bounded_array', for when the (template
    // parameter) 't_TYPE' is an array type of known bound, derives from
    // 'bsl::true_type'.
};

#ifdef BSLS_PLATFORM_CMP_IBM
template <class t_TYPE>
struct is_bounded_array<t_TYPE[]> : false_type {
    // IBM miscomputes the SFINAE condition for arrays of unknown bound, so we
    // provide an additional partial specialization for this platform.
};
#endif

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
template <class t_TYPE>
BSLS_KEYWORD_INLINE_VARIABLE constexpr bool is_bounded_array_v =
                                               is_bounded_array<t_TYPE>::value;
    // This template variable represents the result value of the
    // 'bsl::is_bounded_array' meta-function.
#endif

                    // =========================
                    // struct is_unbounded_array
                    // ==============--=========

template <class t_TYPE>
struct is_unbounded_array : false_type {
    // This 'struct' template implements the 'is_unbounded_array' meta-function
    // defined in the C++20 standard [meta.unary.cat] to determine if the
    // (template parameter) 't_TYPE' is an array type of unknown bound.  This
    // 'struct' derives from 'bsl::true_type' if the 't_TYPE' is an array type
    // of unknown bound, and 'bsl::false_type' otherwise.
};

template <class t_TYPE>
struct is_unbounded_array<t_TYPE[]> : true_type {
    // This specialization of 'is_unbounded_array', for when the (template
    // parameter) 't_TYPE' is an array type of unknown bound, derives from
    // 'bsl::true_type'.
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
template <class t_TYPE>
BSLS_KEYWORD_INLINE_VARIABLE constexpr bool is_unbounded_array_v =
                                             is_unbounded_array<t_TYPE>::value;
    // This template variable represents the result value of the
    // 'bsl::is_unbounded_array_v' meta-function.
#endif

}  // close namespace bsl

namespace BloombergLP {
namespace bslmf {

                         // ==============
                         // struct IsArray
                         // ==============

template <class t_TYPE>
struct IsArray : bsl::is_array<t_TYPE>::type {
    // This 'struct' template implements a meta-function to determine if the
    // (template parameter) 't_TYPE' is an array type.  This 'struct' derives
    // from 'bsl::true_type' if the 't_TYPE' is an array type, and
    // 'bsl::false_type' otherwise.
    //
    // Note that although this 'struct' is functionally equivalent to
    // 'bsl::is_array', the use of 'bsl::is_array' should be preferred.
};

}  // close package namespace
}  // close enterprise namespace

#ifndef BDE_OPENSOURCE_PUBLICATION  // BACKWARD_COMPATIBILITY
// ============================================================================
//                           BACKWARD COMPATIBILITY
// ============================================================================

#ifdef bslmf_IsArray
#undef bslmf_IsArray
#endif
#define bslmf_IsArray bslmf::IsArray
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
