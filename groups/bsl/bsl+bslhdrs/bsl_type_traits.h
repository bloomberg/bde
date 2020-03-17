// bsl_type_traits.h                                                  -*-C++-*-
#ifndef INCLUDED_BSL_TYPE_TRAITS
#define INCLUDED_BSL_TYPE_TRAITS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functionality of the corresponding C++ Standard header.
//
//@SEE_ALSO: package bos+stdhdrs in the bos package group
//
//@DESCRIPTION: Provide types, in the 'bsl' namespace, equivalent to those
// defined in the corresponding C++ standard header.  Include the native
// compiler-provided standard header, and also directly include Bloomberg's
// implementation of the C++ standard type (if one exists).  Finally, place the
// included symbols from the 'std' namespace (if any) into the 'bsl' namespace.

#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>
#include <bsls_nativestd.h>
#include <bsls_platform.h>

#if __cplusplus < 201103L \
    && (defined(BSLS_PLATFORM_CMP_SUN) || defined(BSLS_PLATFORM_CMP_IBM))
#   error This file requires compiler and library support for \
          the ISO C++ 2011 standard.
#endif

#include <cstddef>       // for 'std::size_t'

#include <type_traits>

// PLATFORM SUPPORT MACROS
// -----------------------
// The following macros diagnose whether the current platform supports the full
// set of C++11 type traits.  Although this should be detected by directly
// testing the version of the native standard library (which may deviate from
// the default tool-chain) we are using the compiler version as a proxy for the
// library, which is a good match on the platforms available to us for testing.
// Library-specific detection and support may follow as subsequent patch
// requests.

// Our default implementation assumes that a full set of C++11-conformant
// traits are not available; some standard library implementations omit them
// silently, with no indicator macros.

// BSL_TYPE_TRAITS_HAS_IS_TRIVIALLY_TRAITS
    // C++11 renamed a variety of 'has_trivial_*' traits to 'is_trivially_*'
    // to provide a more consistent naming convention for the standard traits.
    // Several standard library implementations took a while to catch up, after
    // initially promoting their TR1 traits implementation into 'std'.  This
    // macro tracks whether a given library version provides these traits.

// BSL_TYPE_TRAITS_HAS_ALIGNED_UNION
    // The 'aligned_union' type was added, removed, and then restored, over the
    // evolution of C++11.  This macro tracks whether a given library version
    // provides the type.

// We then detect specific platforms that may have some elements of support,
// and define the support macro for just those cases.  We do not redefine the
// support macros if they are already defined, so that it is possible to define
// them on the compiler command line to 0 or 1 regardless of platform.

#if defined(BSLS_PLATFORM_CMP_MSVC)
#  ifndef   BSL_TYPE_TRAITS_HAS_IS_TRIVIALLY_TRAITS
#    define BSL_TYPE_TRAITS_HAS_IS_TRIVIALLY_TRAITS 1
#  endif
#  ifndef   BSL_TYPE_TRAITS_HAS_ALIGNED_UNION
#    define BSL_TYPE_TRAITS_HAS_ALIGNED_UNION       1
#  endif
#endif

#if defined(BSLS_PLATFORM_CMP_GNU)
#  if BSLS_PLATFORM_CMP_VERSION >= 50000
#    ifndef   BSL_TYPE_TRAITS_HAS_IS_TRIVIALLY_TRAITS
#      define BSL_TYPE_TRAITS_HAS_IS_TRIVIALLY_TRAITS 1
#    endif
#    ifndef   BSL_TYPE_TRAITS_HAS_ALIGNED_UNION
#      define BSL_TYPE_TRAITS_HAS_ALIGNED_UNION       1
#    endif
#  endif
#endif

#if defined(BSLS_PLATFORM_CMP_CLANG)
#  if defined(__APPLE_CC__) && __APPLE_CC__ >= 6000
#    ifndef   BSL_TYPE_TRAITS_HAS_IS_TRIVIALLY_TRAITS
#      define BSL_TYPE_TRAITS_HAS_IS_TRIVIALLY_TRAITS 1
#    endif
#    if defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES)
#      ifndef   BSL_TYPE_TRAITS_HAS_ALIGNED_UNION
#        define BSL_TYPE_TRAITS_HAS_ALIGNED_UNION     1
#      endif
#    endif
#  endif
#endif

namespace bsl {
    // Import selected symbols into bsl namespace

    // 20.10.4.1, primary type categories:
using native_std::is_union;

    // 20.10.4.2, composite type categories:
using native_std::is_object;
using native_std::is_scalar;
using native_std::is_compound;

    // 20.10.4.3, type properties:
using native_std::is_trivial;
using native_std::is_standard_layout;
using native_std::is_pod;
using native_std::is_literal_type;
using native_std::is_abstract;
using native_std::is_signed;
using native_std::is_unsigned;
using native_std::is_constructible;
using native_std::is_default_constructible;
using native_std::is_move_constructible;
using native_std::is_assignable;
using native_std::is_copy_assignable;
using native_std::is_move_assignable;
using native_std::is_destructible;
#if BSL_TYPE_TRAITS_HAS_IS_TRIVIALLY_TRAITS
using native_std::is_trivially_constructible;
using native_std::is_trivially_copy_constructible;
using native_std::is_trivially_move_constructible;
using native_std::is_trivially_assignable;
using native_std::is_trivially_copy_assignable;
using native_std::is_trivially_move_assignable;
using native_std::is_trivially_destructible;
#endif
using native_std::is_nothrow_constructible;
using native_std::is_nothrow_default_constructible;
using native_std::is_nothrow_copy_constructible;
using native_std::is_nothrow_assignable;
using native_std::is_nothrow_copy_assignable;
using native_std::is_nothrow_move_assignable;
#if !defined(BSLS_PLATFORM_CMP_GNU) || BSLS_PLATFORM_CMP_VERSION >= 40800
using native_std::is_nothrow_destructible;
#endif
using native_std::has_virtual_destructor;

    // 20.10.5, type property queries:
using native_std::alignment_of;
using native_std::rank;
using native_std::extent;

    // 20.10.6, type relations:
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
using native_std::is_base_of;
#endif

    // 20.10.7.3, sign modifications:
using native_std::make_signed;
using native_std::make_unsigned;

    // 20.10.7.4, array modifications:
using native_std::remove_all_extents;

    // 20.10.7.6, other transformations:
using native_std::aligned_storage;
#if BSL_TYPE_TRAITS_HAS_ALIGNED_UNION
using native_std::aligned_union;
#endif
using native_std::common_type;
using native_std::underlying_type;
using native_std::result_of;

#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
template <std::size_t LEN, std::size_t ALIGN>
using aligned_storage_t =
                        typename native_std::aligned_storage<LEN, ALIGN>::type;
    // 'aligned_storage_t' is an alias to the return type of the
    // 'native_std::aligned_storage' meta-function.

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES
template <std::size_t LEN, class... TYPES>
using aligned_union_t =
                       typename native_std::aligned_union<LEN, TYPES...>::type;
    // 'aligned_union_t' is an alias to the return type of the
    // 'native_std::aligned_union' meta-function.

template <class... TYPES>
using common_type_t = typename native_std::common_type<TYPES...>::type;
    // 'common_type_t' is an alias to the return type of the
    // 'native_std::common_type' meta-function.
#endif

template <class TYPE>
using make_signed_t = typename native_std::make_signed<TYPE>::type;
    // 'make_signed_t' is an alias to the return type of the
    // 'native_std::make_signed' meta-function.

template <class TYPE>
using make_unsigned_t = typename native_std::make_unsigned<TYPE>::type;
    // 'make_unsigned_t' is an alias to the return type of the
    // 'native_std::make_unsigned' meta-function.

template <class TYPE>
using remove_all_extents_t =
                           typename native_std::remove_all_extents<TYPE>::type;
    // 'remove_all_extents_t' is an alias to the return type of the
    // 'native_std::remove_all_extents' meta-function.

template <class TYPE>
using result_of_t = typename native_std::result_of<TYPE>::type;
    // ' result_of_t' is an alias to the return type of the
    // 'native_std::result_of' meta-function.

template <class TYPE>
using underlying_type_t = typename native_std::underlying_type<TYPE>::type;
    // 'underlying_type_t' is an alias to the return type of the
    // 'native_std::underlying_type' meta-function.
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY
using native_std::is_null_pointer;
using native_std::is_final;
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
    // 20.10.8, logical operator traits:
using native_std::conjunction;
using native_std::disjunction;
using native_std::negation;

// void_t is supplied by bslmf_voidtype.h
#endif

#if defined BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool has_virtual_destructor_v =
                               native_std::has_virtual_destructor<TYPE>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_null_pointer_v = native_std::is_null_pointer<TYPE>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_union_v = native_std::is_union<TYPE>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_scalar_v = native_std::is_scalar<TYPE>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_object_v = native_std::is_object<TYPE>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_compound_v = native_std::is_compound<TYPE>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_trivial_v = native_std::is_trivial<TYPE>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_standard_layout_v =
                                   native_std::is_standard_layout<TYPE>::value;
template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_pod_v = native_std::is_pod<TYPE>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_literal_type_v = native_std::is_literal_type<TYPE>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_abstract_v = native_std::is_abstract<TYPE>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_final_v = native_std::is_final<TYPE>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_signed_v = native_std::is_signed<TYPE>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_unsigned_v = native_std::is_unsigned<TYPE>::value;

template <class TYPE, class ...Args>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_constructible_v =
                            native_std::is_constructible<TYPE, Args...>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_default_constructible_v =
                             native_std::is_default_constructible<TYPE>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_move_constructible_v =
                                native_std::is_move_constructible<TYPE>::value;

template <class TYPE1, class TYPE2>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_assignable_v =
                                native_std::is_assignable<TYPE1, TYPE2>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_copy_assignable_v =
                                   native_std::is_copy_assignable<TYPE>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_move_assignable_v =
                                   native_std::is_move_assignable<TYPE>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_destructible_v = native_std::is_destructible<TYPE>::value;

template <class TYPE, class ...Args>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_trivially_constructible_v =
                  native_std::is_trivially_constructible<TYPE, Args...>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_trivially_copy_constructible_v =
                      native_std::is_trivially_copy_constructible<TYPE>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_trivially_move_constructible_v =
                      native_std::is_trivially_move_constructible<TYPE>::value;

template <class TYPE1, class TYPE2>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_trivially_assignable_v =
                      native_std::is_trivially_assignable<TYPE1, TYPE2>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_trivially_copy_assignable_v =
                         native_std::is_trivially_copy_assignable<TYPE>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_trivially_move_assignable_v =
                         native_std::is_trivially_move_assignable<TYPE>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_trivially_destructible_v =
                            native_std::is_trivially_destructible<TYPE>::value;

template <class TYPE, class ...Args>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_nothrow_constructible_v =
                    native_std::is_nothrow_constructible<TYPE, Args...>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_nothrow_default_constructible_v =
                     native_std::is_nothrow_default_constructible<TYPE>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_nothrow_copy_constructible_v =
                        native_std::is_nothrow_copy_constructible<TYPE>::value;

template <class TYPE1, class TYPE2>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_nothrow_assignable_v =
                        native_std::is_nothrow_assignable<TYPE1, TYPE2>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_nothrow_copy_assignable_v =
                           native_std::is_nothrow_copy_assignable<TYPE>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_nothrow_move_assignable_v =
                           native_std::is_nothrow_move_assignable<TYPE>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_nothrow_destructible_v =
                              native_std::is_nothrow_destructible<TYPE>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr std::size_t alignment_of_v = native_std::alignment_of<TYPE>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr std::size_t rank_v = native_std::rank<TYPE>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr std::size_t extent_v = native_std::extent<TYPE>::value;

template <class TYPE1, class TYPE2>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_base_of_v = native_std::is_base_of<TYPE1, TYPE2>::value;
#endif

#if 0
    // These traits are provided by BDE, and have additional members for
    // Bloomberg legacy code still using the pre-standard interface.

using native_std::add_const;
using native_std::add_cv;
using native_std::add_lvalue_reference;
using native_std::add_pointer;
using native_std::add_rvalue_reference;
using native_std::add_volatile;
using native_std::conditional;
using native_std::decay;
using native_std::enable_if;
using native_std::false_type;
using native_std::integral_constant;
using native_std::invoke_result;
using native_std::is_arithmetic;
using native_std::is_array;
using native_std::is_class;
using native_std::is_const;
using native_std::is_convertible;
using native_std::is_copy_constructible;
using native_std::is_empty;
using native_std::is_enum;
using native_std::is_floating_point;
using native_std::is_function;
using native_std::is_fundamental;
using native_std::is_integral;
using native_std::is_lvalue_reference;
using native_std::is_member_function_pointer;
using native_std::is_member_object_pointer;
using native_std::is_member_pointer;
using native_std::is_nothrow_move_constructible;
using native_std::is_pointer;
using native_std::is_polymorphic;
using native_std::is_reference;
using native_std::is_rvalue_reference;
using native_std::is_same;
using native_std::is_trivially_copyable;
using native_std::is_trivially_default_constructible;
using native_std::is_void;
using native_std::is_volatile;
using native_std::remove_const;
using native_std::remove_cv;
using native_std::remove_extent;
using native_std::remove_pointer;
using native_std::remove_reference;
using native_std::remove_volatile;
using native_std::true_type;
using native_std::void_t;
#endif
}  // close package namespace

// Include Bloomberg's implementation, unless compilation is configured to
// override native types in the 'std' namespace with Bloomberg's
// implementation, in which case the implementation file will be included by
// the Bloomberg supplied standard header file.

// standard traits
#ifndef BSL_OVERRIDES_STD
#include <bslmf_addconst.h>
#include <bslmf_addcv.h>
#include <bslmf_addlvaluereference.h>
#include <bslmf_addpointer.h>
#include <bslmf_addreference.h>
#include <bslmf_addrvaluereference.h>
#include <bslmf_addvolatile.h>
#include <bslmf_conditional.h>
#include <bslmf_decay.h>
#include <bslmf_enableif.h>
#include <bslmf_integralconstant.h>
#include <bslmf_invokeresult.h>
#include <bslmf_isarithmetic.h>
#include <bslmf_isarray.h>
#include <bslmf_isclass.h>
#include <bslmf_isconst.h>
#include <bslmf_isconvertible.h>
#include <bslmf_iscopyconstructible.h>
#include <bslmf_isempty.h>
#include <bslmf_isenum.h>
#include <bslmf_isfloatingpoint.h>
#include <bslmf_isfunction.h>
#include <bslmf_isfundamental.h>
#include <bslmf_isintegral.h>
#include <bslmf_islvaluereference.h>
#include <bslmf_ismemberfunctionpointer.h>
#include <bslmf_ismemberobjectpointer.h>
#include <bslmf_ismemberpointer.h>
#include <bslmf_isnothrowmoveconstructible.h>
#include <bslmf_ispointer.h>
#include <bslmf_ispolymorphic.h>
#include <bslmf_isreference.h>
#include <bslmf_isrvaluereference.h>
#include <bslmf_issame.h>
#include <bslmf_istriviallycopyable.h>
#include <bslmf_istriviallydefaultconstructible.h>
#include <bslmf_isvoid.h>
#include <bslmf_isvolatile.h>
#include <bslmf_removeconst.h>
#include <bslmf_removecv.h>
#include <bslmf_removeextent.h>
#include <bslmf_removepointer.h>
#include <bslmf_removereference.h>
#include <bslmf_removevolatile.h>
#include <bslmf_voidtype.h>
#endif  // BSL_OVERRIDES_STD

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
