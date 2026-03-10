// bsl_type_traits.h                                                  -*-C++-*-
#ifndef INCLUDED_BSL_TYPE_TRAITS
#define INCLUDED_BSL_TYPE_TRAITS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functionality of the corresponding C++ Standard header.
//
//@DESCRIPTION: Provide types, in the `bsl` namespace, equivalent to those
// defined in the corresponding C++ standard header.  Include the native
// compiler-provided standard header, and also directly include Bloomberg's
// implementation of the C++ standard type (if one exists).  Finally, place the
// included symbols from the `std` namespace (if any) into the `bsl` namespace.
//
// The following standard type traits are provided in namespace `bsl` by BDE in
// C++03 as well as C++11.
//```
// `add_const`
// `add_cv`
// `add_lvalue_reference`
// `add_pointer`
// `add_rvalue_reference`
// `add_volatile`
// `conditional`
// `decay`
// `enable_if`
// `false_type`
// `integral_constant`
// `is_arithmetic`
// `is_array`
// `is_class`
// `is_const`
// `is_convertible`
// `is_copy_constructible`
// `is_empty`
// `is_enum`
// `is_floating_point`
// `is_function`
// `is_fundamental`
// `is_integral`
// `is_lvalue_reference`
// `is_member_function_pointer`
// `is_member_object_pointer`
// `is_member_pointer`
// `is_nothrow_move_constructible`
// `is_pointer`
// `is_polymorphic`
// `is_reference`
// `is_rvalue_reference`
// `is_same`
// `is_trivially_copyable`
// `is_trivially_default_constructible`
// `is_void`
// `is_volatile`
// `remove_const`
// `remove_cv`
// `remove_extent`
// `remove_pointer`
// `remove_reference`
// `remove_volatile`
// `true_type`
//```
// The following standard type traits are provided in namespace `bsl` by BDE in
// C++03 as well as C++17.
//```
// `bool_constant`
// `conjunction`
// `disjunction`
// `invoke_result`
// `negation`
// `void_t`
//```
// The following standard type traits are provided in namespace `bsl` by BDE in
// C++11 as well as C++17.
//```
// `is_nothrow_swappable`
// `is_swappable`
//```
// The following standard type traits are provided in namespace `bsl` by BDE in
// C++03 as well as C++20.
//```
// `is_bounded_array`
// `is_unbounded_array`
// `remove_cvref`
// `type_identity`
// `unwrap_ref_decay`
// `unwrap_reference`
//```
// In addition, all the `bsl` traits with a `type` member define the
// corresponding `bsl::trait_t` alias template in C++11 to simplify use.
// Similarly the `bsl::trait_v` variable templates are provided in C++14.
// The alias templates and variable template are also provided for the traits
// supplied by the native Standard Library implementation in C++11/14, ahead
// of the published standard that provided them.
//
// Note that the traits imported from namespace `std` are organized first by
// the Standard that introduced them, and then by the subsection ordering
// within the Standard.  Traits that have been removed in later editions of
// the Standard are moved to the end of the file.

#include <bsla_deprecated.h>

// BDE implementation of standard traits
#include <bslmf_addconst.h>
#include <bslmf_addcv.h>
#include <bslmf_addlvaluereference.h>
#include <bslmf_addpointer.h>
#include <bslmf_addrvaluereference.h>
#include <bslmf_addvolatile.h>
#include <bslmf_conditional.h>
#include <bslmf_conjunction.h>
#include <bslmf_decay.h>
#include <bslmf_disjunction.h>
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
#include <bslmf_negation.h>
#include <bslmf_removeconst.h>
#include <bslmf_removecv.h>
#include <bslmf_removecvref.h>
#include <bslmf_removeextent.h>
#include <bslmf_removepointer.h>
#include <bslmf_removereference.h>
#include <bslmf_removevolatile.h>
#include <bslmf_typeidentity.h>
#include <bslmf_unwraprefdecay.h>
#include <bslmf_unwrapreference.h>
#include <bslmf_voidtype.h>

#include <bsls_alignmentutil.h>
#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>
#include <bsls_platform.h>

#include <cstddef>       // for `std::size_t`

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
# include <type_traits>

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
using std::is_trivially_constructible;
using std::is_trivially_copy_constructible;
using std::is_trivially_move_constructible;
using std::is_trivially_assignable;
using std::is_trivially_copy_assignable;
using std::is_trivially_move_assignable;
using std::is_trivially_destructible;
using std::is_nothrow_constructible;
using std::is_nothrow_default_constructible;
using std::is_nothrow_copy_constructible;
using std::is_nothrow_assignable;
using std::is_nothrow_copy_assignable;
using std::is_nothrow_move_assignable;
using std::is_nothrow_destructible;

using std::has_virtual_destructor;

    // 20.10.5, type property queries:
using std::alignment_of;
using std::rank;
using std::extent;

    // 20.10.6, type relations:
using std::is_base_of;

    // 20.10.7.3, sign modifications:
using std::make_signed;
using std::make_unsigned;

    // 20.10.7.4, array modifications:
using std::remove_all_extents;

    // 20.10.7.6, other transformations:

    // `std::aligned_storage` and `std::aligned_union` are deprecated in C++23.
    // Silence the warning that would result from including this header, but
    // mark the bsl aliases deprecated.
# if BSLS_COMPILERFEATURES_CPLUSPLUS > 202002L
#   if defined(BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC)
#     pragma GCC diagnostic push
#     pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#   endif
#   define BSL_TYPE_TRAITS_DEPRECATED_ALIGNED [[deprecated]]
# else
#   define BSL_TYPE_TRAITS_DEPRECATED_ALIGNED
# endif

template <std::size_t t_LEN,
          std::size_t t_ALIGNMENT =
              BloombergLP::bsls::AlignmentUtil
              ::defaultAlignmentOfAlignedStorage<t_LEN>()>
using aligned_storage BSL_TYPE_TRAITS_DEPRECATED_ALIGNED
    = std::aligned_storage<t_LEN, t_ALIGNMENT>;

template <std::size_t t_LEN,
          std::size_t t_ALIGNMENT =
              BloombergLP::bsls::AlignmentUtil
              ::defaultAlignmentOfAlignedStorage<t_LEN>()>
using aligned_storage_t BSL_TYPE_TRAITS_DEPRECATED_ALIGNED
    = typename std::aligned_storage<t_LEN, t_ALIGNMENT>::type;

template <std::size_t t_LEN, class... t_TYPES>
using aligned_union BSL_TYPE_TRAITS_DEPRECATED_ALIGNED
    = std::aligned_union<t_LEN, t_TYPES...>;

template <std::size_t t_LEN, class... t_TYPES>
using aligned_union_t BSL_TYPE_TRAITS_DEPRECATED_ALIGNED
    = typename std::aligned_union<t_LEN, t_TYPES...>::type;

# if BSLS_COMPILERFEATURES_CPLUSPLUS > 202002L
#   if defined(BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC)
#     pragma GCC diagnostic pop
#   endif
#   undef BSL_TYPE_TRAITS_DEPRECATED_ALIGNED
# endif

using std::common_type;
using std::underlying_type;

template <class... TYPES>
using common_type_t = typename std::common_type<TYPES...>::type;

template <class TYPE>
using make_signed_t = typename std::make_signed<TYPE>::type;

template <class TYPE>
using make_unsigned_t = typename std::make_unsigned<TYPE>::type;

template <class TYPE>
using remove_all_extents_t =
                           typename std::remove_all_extents<TYPE>::type;

template <class TYPE>
using underlying_type_t = typename std::underlying_type<TYPE>::type;

# ifdef BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY
using std::is_null_pointer;
using std::is_final;
# endif

# if defined BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
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

#   if defined(BSLS_PLATFORM_CMP_GNU)
    // Silence deprecation warning declaring this variable template in the case
    // that the native standard library has marked `is_pod` as deprecated.
#     pragma GCC diagnostic push
#     pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#   endif

template <class TYPE>
BSLA_DEPRECATED BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_pod_v = std::is_pod<TYPE>::value;

#   if defined(BSLS_PLATFORM_CMP_GNU)
#     pragma GCC diagnostic pop
#   endif

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
# endif // BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES

# ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
    // 23.15.4.3, type properties
using std::has_unique_object_representations;
using std::is_aggregate;

    // 23.15.6, type relations
using std::is_invocable;
using std::is_invocable_r;
using std::is_nothrow_invocable;
using std::is_nothrow_invocable_r;

    // 23.15.4.3, type properties
using std::is_swappable_with;
using std::is_nothrow_swappable_with;

template <class TYPE>
inline constexpr
bool has_unique_object_representations_v =
                           std::has_unique_object_representations<TYPE>::value;

template <class TYPE>
inline constexpr
bool is_aggregate_v = std::is_aggregate<TYPE>::value;

template <class TYPE, class ...Args>
inline constexpr
bool is_invocable_v = std::is_invocable<TYPE, Args...>::value;

template <class RET, class TYPE, class ...Args>
inline constexpr
bool is_invocable_r_v = std::is_invocable_r<RET, TYPE, Args...>::value;

template <class TYPE, class ...Args>
inline constexpr
bool is_nothrow_invocable_v = std::is_nothrow_invocable<TYPE, Args...>::value;

template <class RET, class TYPE, class ...Args>
inline constexpr
bool is_nothrow_invocable_r_v =
                        std::is_nothrow_invocable_r<RET, TYPE, Args...>::value;

template <class TYPE1, class TYPE2>
inline constexpr
bool is_swappable_with_v = std::is_swappable_with<TYPE1, TYPE2>::value;

template <class TYPE1, class TYPE2>
inline constexpr
bool is_nothrow_swappable_with_v =
                           std::is_nothrow_swappable_with<TYPE1, TYPE2>::value;
# endif // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

# ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
// 20.15.6, type relations
using std::is_nothrow_convertible;
using std::is_nothrow_convertible_v;

// 20.15.7.6, other transformations
using std::common_reference;
using std::common_reference_t;
using std::basic_common_reference;
# endif

# ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_IS_LAYOUT_COMPATIBLE
// 20.15.6, type relations
using std::is_layout_compatible;
using std::is_layout_compatible_v;
# endif

# ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_IS_POINTER_INTERCONVERTIBLE
// 20.15.6, type relations
using std::is_pointer_interconvertible_base_of;
using std::is_pointer_interconvertible_base_of_v;

// 20.15.9, member relationships
using std::is_pointer_interconvertible_with_class;
# endif

# ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_IS_CORRESPONDING_MEMBER
// 20.15.9, member relationships
using std::is_corresponding_member;
# endif

# ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
// 20.15.10, constant evaluation context
using std::is_constant_evaluated;
# endif

# if defined(BSLS_PLATFORM_CMP_GNU)
    // Silence deprecation warnings for declarations of alias templates and
    // variable templates where the native library implementation may have
    // marked the aliased trait as deprecated.
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif

/// A numbers of type traits have been deprecated and removed as the Standard
/// evolved.  We move those traits to the end of this header to simplify
/// auditing against the latest Standard document.
# if !defined(BSLS_LIBRARYFEATURES_HAS_CPP20_DEPRECATED_REMOVED)
    // This name is removed by C++20
using std::is_literal_type;
#   if defined BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
template <class TYPE>
BSLA_DEPRECATED BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_literal_type_v = std::is_literal_type<TYPE>::value;
#   endif

using std::result_of;
template <class TYPE>
using result_of_t = typename std::result_of<TYPE>::type;
# endif // BSLS_LIBRARYFEATURES_HAS_CPP20_DEPRECATED_REMOVED

# if defined(BSLS_PLATFORM_CMP_GNU)
#   pragma GCC diagnostic pop
# endif

}  // close package namespace

#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

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
