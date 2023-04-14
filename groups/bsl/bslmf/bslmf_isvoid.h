// bslmf_isvoid.h                                                     -*-C++-*-
#ifndef INCLUDED_BSLMF_ISVOID
#define INCLUDED_BSLMF_ISVOID

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for 'void' types.
//
//@CLASSES:
//  bsl::is_void: standard meta-function for determining 'void' types
//  bsl::is_void_v: the result value of the 'bsl::is_void' meta-function
//  bslmf::IsVoid: meta-function for determining 'void' types
//
//@SEE_ALSO: bslmf_integralconstant
//
//@DESCRIPTION: This component defines two meta-functions, 'bsl::is_void' and
// 'BloombergLP::bslmf::IsVoid' and a template variable 'bsl::is_void_v', that
// represents the result value of 'bsl::is_void' meta-function.  All these
// meta-functions may be used to query whether a type is the (possibly
// cv-qualified) 'void' type.
//
// 'bsl::is_void' meets the requirements of the 'is_void' template defined in
// the C++11 standard [meta.unary.cat], while 'bslmf::IsVoid' was devised
// before 'is_void' was standardized.
//
// The two meta-functions are functionally equivalent.  The major difference
// between them is that the result for 'bsl::is_void' is indicated by the
// class member 'value', while the result for 'bslmf::IsVoid' is indicated
// by the class member 'VALUE'.
//
// Note that 'bsl::is_void' should be preferred over 'bslmf::IsVoid', and in
// general, should be used by new components.
//
// Also note that the template variable 'is_void_v' is defined in the C++17
// standard as an inline variable.  If the current compiler supports the inline
// variable C++17 compiler feature, 'bsl::is_void_v' is defined as an
// 'inline constexpr bool' variable.  Otherwise, if the compiler supports the
// variable templates C++14 compiler feature, 'bsl::is_void_v' is defined as a
// non-inline 'constexpr bool' variable.  See
// 'BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES' and
// 'BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES' macros in
// bsls_compilerfeatures component for details.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Determine Whether a Type is the 'void' Type
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a particular type is the 'void' type.
//
// First, we create two 'typedef's -- the 'void' type and another type:
//..
//  typedef int  MyType;
//  typedef void MyVoidType;
//..
// Now, we instantiate the 'bsl::is_void' template for each of the 'typedef's
// and assert the 'value' static data member of each instantiation:
//..
//  assert(false == bsl::is_void<MyType>::value);
//  assert(true  == bsl::is_void<MyVoidType>::value);
//..
// Note that if the current compiler supports the variable templates C++14
// feature, then we can re-write the snippet of code above as follows:
//..
//#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
//  assert(false == bsl::is_void_v<MyType>);
//  assert(true  == bsl::is_void_v<MyVoidType>);
//#endif
//..

#include <bslscm_version.h>

#include <bslmf_integralconstant.h>

#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bslmf_metaint.h>
#include <bslmf_removecv.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES


namespace bsl {

                         // ==============
                         // struct is_void
                         // ==============

template <class t_TYPE>
struct is_void : false_type {
    // This 'struct' template implements the 'is_void' meta-function defined in
    // the C++11 standard [meta.unary.cat] to determine if the (template
    // parameter) 't_TYPE' is the (possibly cv-qualified) 'void' type.  This
    // 'struct' derives from 'bsl::true_type' if 't_TYPE' is the 'void' type,
    // and 'bsl::false_type' otherwise.
};

template <>
struct is_void<void> : true_type {
    // This partial specialization of 'is_void' derives from 'bsl::true_type'
    // for when the (template parameter) 't_TYPE' is 'void'.
};

template <>
struct is_void<const void> : true_type {
    // This partial specialization of 'is_void' derives from 'bsl::true_type'
    // for when the (template parameter) 't_TYPE' is 'const void'.
};

template <>
struct is_void<volatile void> : true_type {
    // This partial specialization of 'is_void' derives from 'bsl::true_type'
    // for when the (template parameter) 't_TYPE' is 'volatile void'.
};

template <>
struct is_void<const volatile void> : bsl::true_type {
    // This partial specialization of 'is_void' derives from 'bsl::true_type'
    // for when the (template parameter) 't_TYPE' is 'const volatile void'.
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
template <class t_TYPE>
BSLS_KEYWORD_INLINE_VARIABLE constexpr bool is_void_v = is_void<t_TYPE>::value;
    // This template variable represents the result value of the 'bsl::is_void'
    // meta-function.
#endif

}  // close namespace bsl

namespace BloombergLP {
namespace bslmf {

                        // ===================
                        // struct bslmf_IsVoid
                        // ===================

template <class t_TYPE>
struct IsVoid : bsl::is_void<t_TYPE>::type {
    // This 'struct' template implements a meta-function to determine if the
    // (template parameter) 't_TYPE' is the (possibly cv-qualified) 'void'
    // type.  This 'struct' derives from 'bslmf::MetaInt<1>' if 't_TYPE' is the
    // 'void' type, and 'bslmf::MetaInt<0>' otherwise.
    //
    // Note that although this 'struct' is functionally equivalent to
    // 'bsl::is_void', and the use of 'bsl::is_void' should be preferred.
};

}  // close package namespace
}  // close enterprise namespace

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
