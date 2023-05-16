// bslmf_isbitwisecopyable.h                                          -*-C++-*-
#ifndef INCLUDED_BSLMF_ISBITWISECOPYABLE
#define INCLUDED_BSLMF_ISBITWISECOPYABLE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function for determining bitwise copyable types.
//
//@CLASSES:
//  IsBitwiseCopyable: type-traits meta-function
//  IsBitwiseCopyable_v: the result value of the meta-function
//
//@SEE_ALSO: bslmf_integralconstant, bslmf_nestedtraitdeclaration
//
//@DESCRIPTION: This component defines a meta-function,
// 'bslmf::IsBitwiseCopyable' and a template variable
// 'bslmf::IsBitwiseCopyable_v', that represents the result value of the
// 'bslmf::IsBitwiseCopyable' meta-function, that may be used to query
// whether a type is deemed by the author to be trivially copyable, but does
// *NOT* have the property 'bsl::is_trivially_copyable'.
//
// 'bslmf::IsBitwiseCopyable' has the same syntax as the
// 'is_trivially_copyable' template from the C++11 standard [meta.unary.prop],
// but is used exclusively to identify types deemed by the author to be
// trivially copyable, but for which 'std::is_trivially_copyable' is not 'true'
// by default.  Typically such types might have a destructor, or non-trivial
// creators, so that copying them via 'memcpy' may be theoretically undefined
// behavior and cause problems on some future compilers.
//..
//  Type Category        Is Trivially Copyable
//  -------------        ---------------------
//  reference types      false
//  fundamental types    true
//  enumerated types     true
//  pointers             true
//  pointers to members  true
//..
// No types are 'IsBitwiseCopyable' by default -- the only way a type acquires
// this trait is by explicit specialization or by the
// 'BSLMF_NESTED_TRAIT_DECLARATION' macro.
//
// It is illegal to declare a type to have the 'IsBitwiseCopyable' if it also
// has the 'bsl::is_trivially_copyable' trait.
//
// Note that the template variable 'IsBitwiseCopyable_v' is defined in the
// C++17 standard as an inline variable.  If the current compiler supports the
// inline variable C++17 compiler feature, 'bslmf::IsBitwiseCopyable_v' is
// defined as an 'inline constexpr bool' variable.  Otherwise, if the compiler
// supports the variable templates C++14 compiler feature,
// 'bslmf::IsBitwiseCopyable_v' is defined as a non-inline 'constexpr bool'
// variable.  See 'BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES' and
// 'BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES' macros in
// bsls_compilerfeatures component for details.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: 
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
//..

#include <bslscm_version.h>

#include <bslmf_detectnestedtrait.h>
#include <bslmf_integralconstant.h>
#include <bslmf_istriviallycopyable.h>

#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_platform.h>

#include <stddef.h>

#ifdef BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
# include <type_traits>
#endif // BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER

namespace BloombergLP {
namespace bslmf {

                           // ========================
                           // struct IsBitwiseCopyable
                           // ========================

template <class t_TYPE>
struct IsBitwiseCopyable;

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
template <class t_TYPE>
BSLS_KEYWORD_INLINE_VARIABLE constexpr bool IsBitwiseCopyable_v =
                                              IsBitwiseCopyable<t_TYPE>::value;
    // This template variable represents the result value of the
    // 'bsl::is_trivially_copyable' meta-function.
#endif

template <class t_TYPE>
struct IsBitwiseCopyable :
          bsl::integral_constant<
                         bool,
                         DetectNestedTrait<t_TYPE, IsBitwiseCopyable>::value
                        || bsl::is_trivially_copyable<t_TYPE>::value>::type {};

template <class t_TYPE>
struct IsBitwiseCopyable<t_TYPE&> : bsl::false_type {
    // This partial specialization optimizes away a number of nested template
    // instantiations to prove that reference types are never trivially
    // copyable.
};

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template <class t_TYPE>
struct IsBitwiseCopyable<t_TYPE&&> : bsl::false_type {
    // This partial specialization optimizes away a number of nested template
    // instantiations to prove that reference types are never trivially
    // copyable.
};
#endif

#if defined(BSLS_PLATFORM_CMP_SUN) && BSLS_PLATFORM_CMP_VERSION < 0x5130
template <class NON_CV_TYPE, class = void>
struct IsBitwiseCopyable_Solaris
     : bsl::false_type {
    // The Solaris CC compiler (prior to CC 12.4) will match certain types,
    // such as abominable function types, as matching a 'cv'-qualified type in
    // partial specialization, even when that type is not 'cv'-qualified.  The
    // idiom of implementing a partial specialization for 'cv'-qualified traits
    // in terms of the primary template then becomes infinitely recursive for
    // those special cases, so we provide a shim implementation class to handle
    // the delegation.  This primary template always derives from 'false_type',
    // and will be matched for function types, reference types, and 'void',
    // none of which are trivially copyable.  The partial specialization below
    // handles recursion back to the primary trait for all other types.
};

template <class NON_CV_TYPE>
struct IsBitwiseCopyable_Solaris<NON_CV_TYPE, BSLMF_VOIDTYPE(NON_CV_TYPE[])>
                                     : IsBitwiseCopyable<NON_CV_TYPE>::type {};

template <class t_TYPE>
struct IsBitwiseCopyable<const t_TYPE> : IsBitwiseCopyable_Solaris<t_TYPE> {};

template <class t_TYPE>
struct IsBitwiseCopyable<volatile t_TYPE> :
                                          IsBitwiseCopyable_Solaris<t_TYPE> {};

template <class t_TYPE>
struct IsBitwiseCopyable<const volatile t_TYPE> :
                                          IsBitwiseCopyable_Solaris<t_TYPE> {};
#else

template <class t_TYPE>
struct IsBitwiseCopyable<const t_TYPE> : IsBitwiseCopyable<t_TYPE> {};

template <class t_TYPE>
struct IsBitwiseCopyable<volatile t_TYPE> : IsBitwiseCopyable<t_TYPE> {};

template <class t_TYPE>
struct IsBitwiseCopyable<const volatile t_TYPE> : IsBitwiseCopyable<t_TYPE> {};

#endif

template <class t_TYPE, int t_LEN>
struct IsBitwiseCopyable<t_TYPE[t_LEN]> : IsBitwiseCopyable<t_TYPE> {};

template <class t_TYPE, int t_LEN>
struct IsBitwiseCopyable<const t_TYPE[t_LEN]> : IsBitwiseCopyable<t_TYPE> {};

template <class t_TYPE, int t_LEN>
struct IsBitwiseCopyable<volatile t_TYPE[t_LEN]> :
                                                  IsBitwiseCopyable<t_TYPE> {};

template <class t_TYPE, int t_LEN>
struct IsBitwiseCopyable<const volatile t_TYPE[t_LEN]> :
                                                  IsBitwiseCopyable<t_TYPE> {};

#if !defined(BSLS_PLATFORM_CMP_IBM)
// Last checked with the xlC 12.1 compiler.  The IBM xlC compiler has problems
// correctly handling arrays of unknown bound as template parameters.

template <class t_TYPE>
struct IsBitwiseCopyable<t_TYPE[]> : IsBitwiseCopyable<t_TYPE>::type {};
template <class t_TYPE>
struct IsBitwiseCopyable<const t_TYPE[]> : IsBitwiseCopyable<t_TYPE>::type {};

template <class t_TYPE>
struct IsBitwiseCopyable<volatile t_TYPE[]> :
                                            IsBitwiseCopyable<t_TYPE>::type {};
template <class t_TYPE>
struct IsBitwiseCopyable<const volatile t_TYPE[]>
                                          : IsBitwiseCopyable<t_TYPE>::type {};
    // These partial specializations ensures that array-of-unknown-bound types
    // have the same result as their element type.

#endif  // !ibm

}  // close namespace bslmf
}  // close enterprise namespace

#endif // ! defined(INCLUDED_BSLMF_ISBITWISECOPYABLE)

// ----------------------------------------------------------------------------
// Copyright 2023 Bloomberg Finance L.P.
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
