// bslmf_isswappable.h                                                -*-C++-*-
#ifndef INCLUDED_BSLMF_ISSWAPPABLE
#define INCLUDED_BSLMF_ISSWAPPABLE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide metafunction to identify swappable types.
//
//@CLASSES:
//  bsl::is_swappable:   type-traits metafunction
//  bsl::is_swappable_v: the metafunction's result value
//
//@SEE_ALSO: bslmf_integralconstant
//
//@DESCRIPTION: This component defines a metafunction, 'bsl::is_swappable', and
// a variable template 'bsl::is_swappable_v' that represents the result value
// of the 'bsl::is_swappable' metafunction, which may be used to query whether
// 'swap(x,y);' is well-formed.  Note that this is only implemented for C++11
// and above.
//
// 'bsl::is_swappable' meets the requirements of the 'is_swappable' template
// defined in the C++17 standard.
//
// Note that the template variable 'is_swappable_v' is defined in the C++17
// standard as an inline variable.  If the current compiler supports the inline
// variable C++17 compiler feature, 'bsl::is_swappable_v' is defined as an
// 'inline constexpr bool' variable.  Otherwise, if the compiler supports the
// variable templates C++14 compiler feature, 'bsl::is_swappable_v' is defined
// as a non-inline 'constexpr bool' variable.  See
// 'BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES' and
// 'BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES' macros in the
// bsls_compilerfeatures component for details.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify Class Types
///- - - - - - - - - - - - - - -
// Suppose that we want to assert whether a particular type is swappable.
//
// First, we create two 'struct's -- one swappable and one not.
//..
//  struct MyType1 {
//      // trivial so swappable
//  };
//
//  struct MyType2 {
//      // private assignement, so not swappable
//    private:
//      // NOT IMPLEMENTED
//      MyType2& operator=(const MyType2&);
//          // Assignment operator made private to prevent swappability.
//  };
//..
// Now, we instantiate the 'bsl::is_swappable' template for each of the
// 'struct's and assert the 'value' static data member of each instantiation:
//..
//  assert(true  == bsl::is_swappable<MyType1>::value);
//  assert(false == bsl::is_swappable<MyType2>::value);
//..
// Note that if the current compiler supports the variable templates C++14
// feature then we can re-write the snippet of code above using the
// 'bsl::is_swappable_v' variable as follows:
//..
//#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
//  assert(true  == bsl::is_swappable_v<MyType1>);
//  assert(false == bsl::is_swappable_v<MyType2>);
//#endif
//..

#include <bslscm_version.h>

#include <bslmf_assert.h>
#include <bslmf_detectnestedtrait.h>
#include <bslmf_enableif.h>
#include <bslmf_integralconstant.h>
#include <bslmf_isconst.h>
#include <bslmf_isfundamental.h>
#include <bslmf_istriviallycopyable.h>
#include <bslmf_isvolatile.h>
#include <bslmf_util.h>
#include <bslmf_voidtype.h>

#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_platform.h>

#include <stddef.h>

#ifdef BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
# include <type_traits>
#endif // BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsls_nativestd.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

// Forward declaration for C++11 and C++14 non-MSVC
#if !defined(BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY) &&              \
    !defined(BSLS_PLATFORM_CMP_MSVC)                          &&              \
     defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR)         &&              \
     defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)          &&              \
     defined(BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE)
namespace BloombergLP {
namespace bslmf {
namespace bslmf_is_swappable_impl_ns {
// This test requires that 'using std::swap' be executed, hence the use of a
// named namespace to prevent that statement from "leaking" outside of the
// 'bslmf_isswappable.h' header.

template <class TYPE, class = void>
struct IsSwappable_Impl;
    // Forward declaration

}  // close namespace bslmf_is_swappable_impl_ns
}  // close package namespace
}  // close enterprise namespace
#endif

                            // ===================
                            // struct is_swappable
                            // ===================

namespace bsl {

// C++17 alias definition
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY)

using std::is_swappable;
using std::is_swappable_v;

// Windows non-C++17 definition
#elif defined(BSLS_PLATFORM_CMP_MSVC)
// For MSVC before 2019 and all MSVC versions building C++14 (C++14 is the
// lowest supported by MSVC), a combination of MSVC's name lookup and sfinae
// peculiarities means we cannot reproduce this behaviour.  Under these
// unfortunate circumstances, it is therefore safest to assume nothing is
// swappable.

template <class TYPE>
struct is_swappable : bsl::false_type {};

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_swappable_v = is_swappable<TYPE>::value;
    // This template variable represents the result value of the
    // 'bsl::is_swappable' metafunction.

// C++11 implementation
#elif defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR) \
   && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)  \
   && defined(BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE)
// We are compiling C++11 or C++14 on a conformant compiler, so we can
// replicate the is_swappable trait.

template <class TYPE>
struct is_swappable
    : BloombergLP::bslmf::bslmf_is_swappable_impl_ns::IsSwappable_Impl<TYPE> {
    // This 'struct' template implements a metafunction to determine whether
    // the (template parameter) 'TYPE' is swappable.  This 'struct' derives
    // from 'bsl::true_type' if the 'TYPE' is swappable, and from
    // 'bsl::false_type' otherwise.  This metafunction has the same syntax as
    // the 'is_swappable' metafunction defined in the C++17 standard
    // [meta.unary.prop]; on C++03 platforms this metafunction is not
    // implemented; on C++11 platforms, it is implemented where it is possible
    // to do so given compiler support, with the notable exception of MSVC.
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_swappable_v = is_swappable<TYPE>::value;
    // This template variable represents the result value of the
    // 'bsl::is_swappable' metafunction.
#endif

#endif

}  // close namespace bsl

// ============================================================================
//                              TEMPLATE IMPLEMENTATIONS
// ============================================================================

// Template implementation only for C++11 and C++14 non-MSVC
#if !defined(BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY) &&              \
    !defined(BSLS_PLATFORM_CMP_MSVC)                          &&              \
     defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR)         &&              \
     defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)          &&              \
     defined(BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE)
namespace BloombergLP {
namespace bslmf {
namespace bslmf_is_swappable_impl_ns
{
// This test requires that 'using std::swap' be executed, hence the use of a
// named namespace to prevent that statement from "leaking" outside of the
// 'bslmf_isswappable.h' header.

using std::swap;

template <class TYPE, class>
struct IsSwappable_Impl
: bsl::false_type {
    // This 'struct' template implements a metafunction to determine whether
    // the (template parameter) 'TYPE' is swappable.  Note that the partial
    // specializations below will provide the determination for class types.
};

template <class TYPE>
struct IsSwappable_Impl<TYPE,
                        BSLMF_VOIDTYPE(decltype(
                            swap(BloombergLP::bslmf::Util::declval<TYPE&>(),
                                 BloombergLP::bslmf::Util::declval<TYPE&>())))>
: bsl::true_type {
    // Partial specialization to determine whether the (template parameter)
    // 'TYPE' is swappable as defined in the C++17 standard.
};

}  // close namespace bslmf_is_swappable_impl_ns
}  // close package namespace
}  // close enterprise namespace
#endif

#endif // INCLUDED_BSLMF_ISSWAPPABLE

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
