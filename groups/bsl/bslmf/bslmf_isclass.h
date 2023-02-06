// bslmf_isclass.h                                                    -*-C++-*-
#ifndef INCLUDED_BSLMF_ISCLASS
#define INCLUDED_BSLMF_ISCLASS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for determining class types.
//
//@CLASSES:
//  bsl::is_class: standard meta-function for determining class types
//  bsl::is_class_v: the result value of the 'bsl::is_class' meta-function
//  bslmf::IsClass: meta-function for determining class types
//
//@DESCRIPTION: This component defines two meta-functions, 'bsl::is_class' and
// 'BloombergLP::bslmf::IsClass' and a template variable 'bsl::is_class_v',
// that represents the result value of the 'bsl::is_class' meta-function.  All
// these meta-functions may be used to query whether a type is a 'class',
// 'struct', or 'union', optionally qualified with 'const' or 'volatile'.
//
// 'bsl::is_class' meets the requirements of the 'is_class' template defined in
// the C++11 standard [meta.unary.cat], while 'bslmf::IsClass' was devised
// before 'is_class' was standardized.
//
// The two meta-functions are functionally equivalent.  The major difference
// between them is that the result for 'bsl::is_class' is indicated by the
// class member 'value', while the result for 'bslmf::IsClass' is indicated by
// the class member 'VALUE'.
//
// Note that 'bsl::is_class' should be preferred over 'bslmf::IsClass', and in
// general, should be used by new components.
//
// Also note that the template variable 'is_class_v' is defined in the C++17
// standard as an inline variable.  If the current compiler supports the inline
// variable C++17 compiler feature, 'bsl::is_class_v' is defined as an
// 'inline constexpr bool' variable.  Otherwise, if the compiler supports the
// variable templates C++14 compiler feature, 'bsl::is_class_v' is defined as a
// non-inline 'constexpr bool' variable.  See
// 'BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES' and
// 'BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES' macros in
// bsls_compilerfeatures component for details.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify Class Types
///- - - - - - - - - - - - - - -
// Suppose that we want to assert whether a set of types are class types.
//
// First, we create a class type 'MyClass':
//..
//  class MyClass
//  {
//  };
//..
// Now, we instantiate the 'bsl::is_class' template for both a non-class type
// and the defined type 'MyClass', and assert the 'value' static data member of
// each instantiation:
//..
//  assert(false == bsl::is_class<int>::value);
//  assert(true  == bsl::is_class<MyClass>::value);
//..
// Note that if the current compiler supports the variable the templates C++14
// feature then we can re-write the snippet of code above using the
// 'bsl::is_class_v' variable as follows:
//..
//#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
//  assert(false == bsl::is_class_v<int>);
//  assert(true  == bsl::is_class_v<MyClass>);
//#endif
//..

#include <bslscm_version.h>

#include <bslmf_integralconstant.h>
#include <bslmf_voidtype.h>

#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bslmf_removecv.h>
#include <cstdlib>
#endif

namespace BloombergLP {
namespace bslmf {

                             // ==================
                             // struct IsClass_Imp
                             // ==================

template <class t_TYPE, class = void>
struct IsClass_Imp : bsl::false_type {
    // This 'struct' template provides a meta-function to determine whether the
    // (template parameter) 't_TYPE' is a class type.
};

template <class t_TYPE>
struct IsClass_Imp<t_TYPE, BSLMF_VOIDTYPE(int t_TYPE::*)> : bsl::true_type {
    // This 'struct' template provides a meta-function to determine whether the
    // (template parameter) 't_TYPE' is a class type.
};

}  // close package namespace
}  // close enterprise namespace

namespace bsl {

                             // ===============
                             // struct is_class
                             // ===============

template <class t_TYPE>
struct is_class : BloombergLP::bslmf::IsClass_Imp<t_TYPE>::type {
    // This 'struct' template implements the 'is_class' meta-function defined
    // in the C++11 standard [meta.unary.cat] to determine if the (template
    // parameter) 't_TYPE' is a class.  Note that for implementations without
    // native library support, this component will mis-diagnose 'union' types
    // as classes.  This would be correct according to the core language
    // definition of a class type, but is an error according to the standard
    // library definition of this type trait.
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
template <class t_TYPE>
BSLS_KEYWORD_INLINE_VARIABLE constexpr bool is_class_v =
                                                       is_class<t_TYPE>::value;
    // This template variable represents the result value of the
    // 'bsl::is_class' meta-function.
#endif

}  // close namespace bsl

namespace BloombergLP {
namespace bslmf {
                            // ==============
                            // struct IsClass
                            // ==============

template <class t_TYPE>
struct IsClass : bsl::is_class<t_TYPE>::type {
    // This meta-function derives from 'bsl::true_type' if the (template
    // parameter) 't_TYPE' is a class type, or a reference to a class type, and
    // from 'bsl::false_type' otherwise.
    //
    // Note that although this 'struct' is functionally identical to
    // 'bsl::is_class', the use of 'bsl::is_class' should be preferred.
};

}  // close package namespace
}  // close enterprise namespace

#ifndef BDE_OPENSOURCE_PUBLICATION  // BACKWARD_COMPATIBILITY
// ============================================================================
//                           BACKWARD COMPATIBILITY
// ============================================================================

#ifdef bslmf_IsClass
#undef bslmf_IsClass
#endif
#define bslmf_IsClass bslmf::IsClass
    // This alias is defined for backward compatibility.
#endif  // BDE_OPENSOURCE_PUBLICATION -- BACKWARD_COMPATIBILITY

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
