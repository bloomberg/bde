// bsl_type_traits.h                                                  -*-C++-*-
#ifndef INCLUDED_BSL_TYPE_TRAITS
#define INCLUDED_BSL_TYPE_TRAITS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functionality of the corresponding C++ Standard header.
//
//@DESCRIPTION: Provide types, in the 'bsl' namespace, equivalent to those
// defined in the corresponding C++ standard header.  Include the native
// compiler-provided standard header, and also directly include Bloomberg's
// implementation of the C++ standard type (if one exists).  Finally, place the
// included symbols from the 'std' namespace (if any) into the 'bsl' namespace.

#include <bsla_deprecated.h>

#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>
#include <bsls_platform.h>

#include <cstddef>       // for 'std::size_t'

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsls_nativestd.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#if BSLS_COMPILERFEATURES_CPLUSPLUS >= 201103L
#  ifndef BSL_USE_NATIVE_STD_IMPLEMENTATION
#    define BSL_USE_NATIVE_STD_IMPLEMENTATION 1
#  endif
#endif

#if defined(BSL_USE_NATIVE_STD_IMPLEMENTATION)

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
using std::is_union;

    // 20.10.4.2, composite type categories:
using std::is_object;
using std::is_scalar;
using std::is_compound;

    // 20.10.4.3, type properties:
using std::is_trivial;
using std::is_standard_layout;
using std::is_pod;
#if ! defined(BSLS_LIBRARYFEATURES_HAS_CPP20_DEPRECATED_REMOVED)
    // This name is removed by C++20
using std::is_literal_type;
#endif
using std::is_abstract;
using std::is_signed;
using std::is_unsigned;
using std::is_constructible;
using std::is_default_constructible;
using std::is_move_constructible;
using std::is_assignable;
using std::is_copy_assignable;
using std::is_move_assignable;
using std::is_destructible;
#if BSL_TYPE_TRAITS_HAS_IS_TRIVIALLY_TRAITS
using std::is_trivially_constructible;
using std::is_trivially_copy_constructible;
using std::is_trivially_move_constructible;
using std::is_trivially_assignable;
using std::is_trivially_copy_assignable;
using std::is_trivially_move_assignable;
using std::is_trivially_destructible;
#endif
using std::is_nothrow_constructible;
using std::is_nothrow_default_constructible;
using std::is_nothrow_copy_constructible;
using std::is_nothrow_assignable;
using std::is_nothrow_copy_assignable;
using std::is_nothrow_move_assignable;
#if !defined(BSLS_PLATFORM_CMP_GNU) || BSLS_PLATFORM_CMP_VERSION >= 40800
using std::is_nothrow_destructible;
#endif
using std::has_virtual_destructor;

    // 20.10.5, type property queries:
using std::alignment_of;
using std::rank;
using std::extent;

    // 20.10.6, type relations:
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
using std::is_base_of;
#endif

    // 20.10.7.3, sign modifications:
using std::make_signed;
using std::make_unsigned;

    // 20.10.7.4, array modifications:
using std::remove_all_extents;

    // 20.10.7.6, other transformations:
using std::aligned_storage;
#if BSL_TYPE_TRAITS_HAS_ALIGNED_UNION
using std::aligned_union;
#endif
using std::common_type;
using std::underlying_type;
#if ! defined(BSLS_LIBRARYFEATURES_HAS_CPP20_DEPRECATED_REMOVED)
    // This name is removed by C++20
using std::result_of;
#endif

#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
template <std::size_t LEN, std::size_t ALIGN>
using aligned_storage_t =
                        typename std::aligned_storage<LEN, ALIGN>::type;
    // 'aligned_storage_t' is an alias to the return type of the
    // 'std::aligned_storage' meta-function.

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES
template <std::size_t LEN, class... TYPES>
using aligned_union_t =
                       typename std::aligned_union<LEN, TYPES...>::type;
    // 'aligned_union_t' is an alias to the return type of the
    // 'std::aligned_union' meta-function.

template <class... TYPES>
using common_type_t = typename std::common_type<TYPES...>::type;
    // 'common_type_t' is an alias to the return type of the
    // 'std::common_type' meta-function.
#endif

template <class TYPE>
using make_signed_t = typename std::make_signed<TYPE>::type;
    // 'make_signed_t' is an alias to the return type of the
    // 'std::make_signed' meta-function.

template <class TYPE>
using make_unsigned_t = typename std::make_unsigned<TYPE>::type;
    // 'make_unsigned_t' is an alias to the return type of the
    // 'std::make_unsigned' meta-function.

template <class TYPE>
using remove_all_extents_t =
                           typename std::remove_all_extents<TYPE>::type;
    // 'remove_all_extents_t' is an alias to the return type of the
    // 'std::remove_all_extents' meta-function.

#if ! defined(BSLS_LIBRARYFEATURES_HAS_CPP20_DEPRECATED_REMOVED)
    // This name is removed by C++20

template <class TYPE>
using result_of_t = typename std::result_of<TYPE>::type;
    // ' result_of_t' is an alias to the return type of the 'std::result_of'
    // meta-function.
#endif

template <class TYPE>
using underlying_type_t = typename std::underlying_type<TYPE>::type;
    // 'underlying_type_t' is an alias to the return type of the
    // 'std::underlying_type' meta-function.
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY
using std::is_null_pointer;
using std::is_final;
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
    // 20.10.8, logical operator traits:
using std::conjunction;
using std::disjunction;
using std::negation;

    // 23.15.4.3, type properties
using std::has_unique_object_representations;
using std::is_aggregate;

    // 23.15.6, type relations
using std::is_invocable;
using std::is_invocable_r;
using std::is_nothrow_invocable;
using std::is_nothrow_invocable_r;

    // 23.15.4.3, type properties
using std::is_swappable;
using std::is_swappable_with;
using std::is_nothrow_swappable;
using std::is_nothrow_swappable_with;

// void_t is supplied by bslmf_voidtype.h, as is invoke_result
// (bslmf_invokeresult.h), bool_constant (bslmf_integralconstant.h), and
// type_identity (bslmf_typeidentity.h)
#endif

#if defined BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool has_virtual_destructor_v =
                               std::has_virtual_destructor<TYPE>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_null_pointer_v = std::is_null_pointer<TYPE>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_union_v = std::is_union<TYPE>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_scalar_v = std::is_scalar<TYPE>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_object_v = std::is_object<TYPE>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_compound_v = std::is_compound<TYPE>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_trivial_v = std::is_trivial<TYPE>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_standard_layout_v = std::is_standard_layout<TYPE>::value;

// ----------------------------------------------------------------------------

#if defined(BSLS_PLATFORM_CMP_GNU) && BSLS_PLATFORM_CMP_VERSION >= 110000 &&  \
    BSLS_COMPILERFEATURES_CPLUSPLUS >= 202002L
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

template <class TYPE>
#if BSLS_COMPILERFEATURES_CPLUSPLUS >= 202002L
BSLA_DEPRECATED  // Warn of using 'bsl::is_pod_v' even though we suppress
                 // warnings of using 'std::is_pod' in this implementation.
#endif // C++20
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_pod_v = std::is_pod<TYPE>::value;

#if defined(BSLS_PLATFORM_CMP_GNU) && BSLS_PLATFORM_CMP_VERSION >= 110000 &&  \
    BSLS_COMPILERFEATURES_CPLUSPLUS >= 202002L
#pragma GCC diagnostic pop
#endif

// ----------------------------------------------------------------------------

#if defined(BSLS_PLATFORM_CMP_GNU) && BSLS_PLATFORM_CMP_VERSION >= 110000 &&  \
    BSLS_COMPILERFEATURES_CPLUSPLUS >= 201703L
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#if ! defined(BSLS_LIBRARYFEATURES_HAS_CPP20_DEPRECATED_REMOVED)
template <class TYPE>
BSLA_DEPRECATED  // Warn of using 'bsl::is_literal_type' even though we
                 // suppress warnings of using 'std::is_pod' in this
                 // implementation.
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_literal_type_v = std::is_literal_type<TYPE>::value;
#else
    // Removed in C++20
#endif // Introduced in C++17 already deprecated; removed in C++20.

#if defined(BSLS_PLATFORM_CMP_GNU) && BSLS_PLATFORM_CMP_VERSION >= 110000 &&  \
    BSLS_COMPILERFEATURES_CPLUSPLUS >= 201703L
#pragma GCC diagnostic pop
#endif

// ----------------------------------------------------------------------------

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_abstract_v = std::is_abstract<TYPE>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_final_v = std::is_final<TYPE>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_signed_v = std::is_signed<TYPE>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_unsigned_v = std::is_unsigned<TYPE>::value;

template <class TYPE, class ...Args>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_constructible_v =
                            std::is_constructible<TYPE, Args...>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_default_constructible_v =
                             std::is_default_constructible<TYPE>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_move_constructible_v =
                                std::is_move_constructible<TYPE>::value;

template <class TYPE1, class TYPE2>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_assignable_v =
                                std::is_assignable<TYPE1, TYPE2>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_copy_assignable_v =
                                   std::is_copy_assignable<TYPE>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_move_assignable_v =
                                   std::is_move_assignable<TYPE>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_destructible_v = std::is_destructible<TYPE>::value;

template <class TYPE, class ...Args>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_trivially_constructible_v =
                  std::is_trivially_constructible<TYPE, Args...>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_trivially_copy_constructible_v =
                      std::is_trivially_copy_constructible<TYPE>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_trivially_move_constructible_v =
                      std::is_trivially_move_constructible<TYPE>::value;

template <class TYPE1, class TYPE2>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_trivially_assignable_v =
                      std::is_trivially_assignable<TYPE1, TYPE2>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_trivially_copy_assignable_v =
                         std::is_trivially_copy_assignable<TYPE>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_trivially_move_assignable_v =
                         std::is_trivially_move_assignable<TYPE>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_trivially_destructible_v =
                            std::is_trivially_destructible<TYPE>::value;

template <class TYPE, class ...Args>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_nothrow_constructible_v =
                    std::is_nothrow_constructible<TYPE, Args...>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_nothrow_default_constructible_v =
                     std::is_nothrow_default_constructible<TYPE>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_nothrow_copy_constructible_v =
                        std::is_nothrow_copy_constructible<TYPE>::value;

template <class TYPE1, class TYPE2>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_nothrow_assignable_v =
                        std::is_nothrow_assignable<TYPE1, TYPE2>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_nothrow_copy_assignable_v =
                           std::is_nothrow_copy_assignable<TYPE>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_nothrow_move_assignable_v =
                           std::is_nothrow_move_assignable<TYPE>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_nothrow_destructible_v =
                              std::is_nothrow_destructible<TYPE>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr std::size_t alignment_of_v = std::alignment_of<TYPE>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr std::size_t rank_v = std::rank<TYPE>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr std::size_t extent_v = std::extent<TYPE>::value;

template <class TYPE1, class TYPE2>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_base_of_v = std::is_base_of<TYPE1, TYPE2>::value;

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
template <class ...Bools>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool conjunction_v = std::conjunction<Bools...>::value;

template <class ...Bools>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool disjunction_v = std::disjunction<Bools...>::value;

template <class BOOLEAN_VALUE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool negation_v = std::negation<BOOLEAN_VALUE>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr std::size_t has_unique_object_representations_v =
                    std::has_unique_object_representations<TYPE>::value;

template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr std::size_t is_aggregate_v = std::is_aggregate<TYPE>::value;

template <class TYPE, class ...Args>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr std::size_t is_invocable_v =
                                std::is_invocable<TYPE, Args...>::value;

template <class RET, class TYPE, class ...Args>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr std::size_t is_invocable_r_v =
                         std::is_invocable_r<RET, TYPE, Args...>::value;

template <class TYPE, class ...Args>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr std::size_t is_nothrow_invocable_v =
                        std::is_nothrow_invocable<TYPE, Args...>::value;

template <class RET, class TYPE, class ...Args>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr std::size_t is_nothrow_invocable_r_v =
                 std::is_nothrow_invocable_r<RET, TYPE, Args...>::value;

template <class TYPE1, class TYPE2>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr std::size_t is_swappable_with_v =
                            std::is_swappable_with<TYPE1, TYPE2>::value;

template <class TYPE1, class TYPE2>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr std::size_t is_nothrow_swappable_with_v =
                    std::is_nothrow_swappable_with<TYPE1, TYPE2>::value;
#endif
#endif

#if 0
    // These traits are provided by BDE, and have additional members for
    // Bloomberg legacy code still using the pre-standard interface.

using std::add_const;
using std::add_cv;
using std::add_lvalue_reference;
using std::add_pointer;
using std::add_rvalue_reference;
using std::add_volatile;
using std::conditional;
using std::decay;
using std::enable_if;
using std::false_type;
using std::integral_constant;
using std::invoke_result;
using std::is_arithmetic;
using std::is_array;
using std::is_class;
using std::is_const;
using std::is_convertible;
using std::is_copy_constructible;
using std::is_empty;
using std::is_enum;
using std::is_floating_point;
using std::is_function;
using std::is_fundamental;
using std::is_integral;
using std::is_lvalue_reference;
using std::is_member_function_pointer;
using std::is_member_object_pointer;
using std::is_member_pointer;
using std::is_nothrow_move_constructible;
using std::is_pointer;
using std::is_polymorphic;
using std::is_reference;
using std::is_rvalue_reference;
using std::is_same;
using std::is_trivially_copyable;
using std::is_trivially_default_constructible;
using std::is_void;
using std::is_volatile;
using std::remove_const;
using std::remove_cv;
using std::remove_extent;
using std::remove_pointer;
using std::remove_reference;
using std::remove_volatile;
using std::true_type;
using std::void_t;
#endif
}  // close package namespace

#endif  // BSL_USE_NATIVE_STD_IMPLEMENTATION

// Include Bloomberg's implementation, unless compilation is configured to
// override native types in the 'std' namespace with Bloomberg's
// implementation, in which case the implementation file will be included by
// the Bloomberg supplied standard header file.

// standard traits
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
#include <bslmf_isnothrowswappable.h>
#include <bslmf_ispointer.h>
#include <bslmf_ispolymorphic.h>
#include <bslmf_isreference.h>
#include <bslmf_isrvaluereference.h>
#include <bslmf_issame.h>
#include <bslmf_isswappable.h>
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
#include <bslmf_typeidentity.h>
#include <bslmf_voidtype.h>

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
