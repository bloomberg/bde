// bslmf_isnothrowswappable.h                                         -*-C++-*-
#ifndef INCLUDED_BSLMF_ISNOTHROWSWAPPABLE
#define INCLUDED_BSLMF_ISNOTHROWSWAPPABLE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide metafunction to identify nothrow swappable types.
//
//@CLASSES:
//  bsl::is_nothrow_swappable:   type-traits metafunction
//  bsl::is_nothrow_swappable_v: the metafunction's result value
//
//@SEE_ALSO: bslmf_integralconstant
//
//@DESCRIPTION: This component defines a metafunction,
// 'bsl::is_nothrow_swappable', and a variable template
// 'bsl::is_nothrow_swappable_v' that represents the result value of the
// 'bsl::is_nothrow_swappable' metafunction, which may be used to query whether
// 'swap(x,y);' is well-formed.  Note that this is only implemented for C++11
// and above.
//
// 'bsl::is_nothrow_swappable' meets the requirements of the
// 'is_nothrow_swappable' template defined in the C++17 standard.
//
// Note that the template variable 'is_nothrow_swappable_v' is defined in the
// C++17 standard as an inline variable.  If the current compiler supports the
// inline variable C++17 compiler feature, 'bsl::is_nothrow_swappable_v' is
// defined as an 'inline constexpr bool' variable.  Otherwise, if the compiler
// supports the variable templates C++14 compiler feature,
// 'bsl::is_nothrow_swappable_v' is defined as a non-inline 'constexpr bool'
// variable.  See 'BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES' and
// 'BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES' macros in the
// bsls_compilerfeatures component for details.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify Class Types
///- - - - - - - - - - - - - - -
// Suppose that we want to assert whether a particular type is nothrow
// swappable.
//
// First, we create two 'struct's -- one nothrow swappable and one not.
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
// Now, we instantiate the 'bsl::is_nothrow_swappable' template for each of the
// 'struct's and assert the 'value' static data member of each instantiation:
//..
//  assert(true  == bsl::is_nothrow_swappable<MyType1>::value);
//  assert(false == bsl::is_nothrow_swappable<MyType2>::value);
//..
// Note that if the current compiler supports the variable templates C++14
// feature then we can re-write the snippet of code above using the
// 'bsl::is_nothrow_swappable_v' variable as follows:
//..
//#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
//  assert(true  == bsl::is_nothrow_swappable_v<MyType1>);
//  assert(false == bsl::is_nothrow_swappable_v<MyType2>);
//#endif
//..

#include <bslscm_version.h>

#include <bslmf_assert.h>
#include <bslmf_enableif.h>
#include <bslmf_if.h>
#include <bslmf_integralconstant.h>
#include <bslmf_isswappable.h>
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
namespace bslmf_is_nothrow_swappable_impl_ns {
// This test requires that 'using std::swap' be executed, hence the use of a
// named namespace to prevent that statement from "leaking" outside of the
// 'bslmf_isnothrowswappable.h' header.

template <class TYPE, class = void>
struct IsNoThrowSwappable_Impl;
    // Forward declaration

}  // close namespace bslmf_is_nothrow_swappable_impl_ns
}  // close package namespace
}  // close enterprise namespace
#endif

                        // ===========================
                        // struct is_nothrow_swappable
                        // ===========================

namespace bsl {

// C++17 alias definition
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY)

using std::is_nothrow_swappable;
using std::is_nothrow_swappable_v;

// Windows non-C++17 definition
#elif defined(BSLS_PLATFORM_CMP_MSVC)
// For MSVC before 2019 and all MSVC versions building C++14 (C++14 is the
// lowest supported by MSVC), a combination of MSVC's name lookup and sfinae
// peculiarities means we cannot reproduce this behaviour.  Under these
// unfortunate circumstances, it is therefore safest to assume nothing is
// nothrow swappable.

#define BSLMF_ISNOTHROWSWAPPABLE_ALWAYS_FALSE 1

template <class TYPE>
struct is_nothrow_swappable : bsl::false_type {};

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_nothrow_swappable_v = is_nothrow_swappable<TYPE>::value;
    // This template variable represents the result value of the
    // 'bsl::is_nothrow_swappable' metafunction.

// C++11 implementation
#elif defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR) \
   && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)  \
   && defined(BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE)
// We are compiling C++11 or C++14 on a conformant compiler, so we can
// replicate the is_swappable trait.

template <class TYPE>
struct is_nothrow_swappable
: BloombergLP::bslmf::If<
      BloombergLP::bslmf::bslmf_is_nothrow_swappable_impl_ns::
          IsNoThrowSwappable_Impl<TYPE>::isNoexcept(),
      bsl::true_type,
      bsl::false_type>::Type {
    // This 'struct' template implements a metafunction to determine whether
    // the (template parameter) 'TYPE' is nothrow swappable.  This 'struct'
    // derives from 'bsl::true_type' if the 'TYPE' is nothrow swappable, and
    // from 'bsl::false_type' otherwise.  This metafunction has the same syntax
    // as the 'is_nothrow_swappable' metafunction defined in the C++17 standard
    // [meta.unary.prop]; on C++03 platforms this metafunction is not
    // implemented; on C++11 platforms, it is implemented where it is possible
    // to do so given compiler support, with the notable exception of MSVC.
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_nothrow_swappable_v =
                           is_nothrow_swappable<TYPE>::value;
    // This template variable represents the result value of the
    // 'bsl::is_nothrow_swappable' metafunction.
#endif // BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES

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
namespace bslmf_is_nothrow_swappable_impl_ns {
// This test requires that 'using std::swap' be executed, hence the use of a
// named namespace to prevent that statement from "leaking" outside of the
// 'bslmf_isnothrowswappable.h' header.

using std::swap;

template <class TYPE, class BDE_OTHER_TYPE>
struct IsNoThrowSwappable_Impl {
    // This 'struct' template implements a 'constexpr' 'evaluate' function to
    // determine whether the (template parameter) 'TYPE' is nothrow swappable.
    // The default implementation is for non-swappable types, which are, by
    // definition, not nothrow swappable.  Note that the partial
    // specializations below will provide the determination for swappable
    // types.

    // CLASS METHODS
    static constexpr bool isNoexcept();
        // 'constexpr' method to determine whether the (template) parameter
        // 'TYPE' is nothrow swappable.
};

template <class TYPE>
struct IsNoThrowSwappable_Impl<
    TYPE,
    typename bsl::enable_if<bsl::is_swappable<TYPE>::value, void>::type> {
    // This 'struct' template implements a 'constexpr' 'evaluate' to determine
    // whether the (template parameter) 'TYPE' is nothrow swappable.  Note that
    // this partial specialization provides the determination for swappable
    // types.

    // CLASS METHODS
    static constexpr bool isNoexcept();
        // 'constexpr' method to determine whether the (template) parameter
        // 'TYPE' is nothrow swappable.
};


template <class TYPE, class BDE_OTHER_TYPE>
constexpr bool
IsNoThrowSwappable_Impl<TYPE, BDE_OTHER_TYPE>
::isNoexcept()
{
    return false;
}

template <class TYPE>
constexpr bool
IsNoThrowSwappable_Impl<
    TYPE,
    typename bsl::enable_if<bsl::is_swappable<TYPE>::value, void>::type>
::isNoexcept()
{
    return noexcept(swap(BloombergLP::bslmf::Util::declval<TYPE&>(),
                         BloombergLP::bslmf::Util::declval<TYPE&>()));
};

}  // close namespace bslmf_is_nothrow_swappable_impl_ns
}  // close package namespace
}  // close enterprise namespace
#endif

#endif // INCLUDED_BSLMF_ISNOTHROWSWAPPABLE

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
