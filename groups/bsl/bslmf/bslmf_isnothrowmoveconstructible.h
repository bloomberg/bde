// bslmf_isnothrowmoveconstructible.h                                 -*-C++-*-
#ifndef INCLUDED_BSLMF_ISNOTHROWMOVECONSTRUCTIBLE
#define INCLUDED_BSLMF_ISNOTHROWMOVECONSTRUCTIBLE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide metafunction to identify no-throw move-constructible types.
//
//@CLASSES:
//  bsl::is_nothrow_move_constructible: type-traits metafunction
//  bsl::is_nothrow_move_constructible_v: the metafunction's result value
//
//@SEE_ALSO: bslmf_integralconstant, bslmf_nestedtraitdeclaration,
//           bslmf_movableref
//
//@DESCRIPTION: This component defines a metafunction,
// 'bsl::is_nothrow_move_constructible', and a variable template
// 'bsl::is_nothrow_move_constructible_v' that represents the result value of
// the 'bsl::is_nothrow_move_constructible' metafunction, which may be used to
// query whether a type has a constructor that can be called with a single
// rvalue that is known to not throw exceptions.  Note that a C++11 compiler
// will automatically infer this trait for class types with a move constructor
// that is marked as 'noexcept'.
//
// 'bsl::is_nothrow_move_constructible' meets the requirements of the
// 'is_nothrow_move_constructible' template defined in the C++11 standard.
//
// Note that the template variable 'is_nothrow_move_constructible_v' is defined
// in the C++17 standard as an inline variable.  If the current compiler
// supports the inline variable C++17 compiler feature,
// 'bsl::is_nothrow_move_constructible_v' is defined as an 'inline constexpr
// bool' variable.  Otherwise, if the compiler supports the variable templates
// C++14 compiler feature, 'bsl::is_nothrow_move_constructible_v' is defined as
// a non-inline 'constexpr bool' variable.  See
// 'BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES' and
// 'BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES' macros in the
// bsls_compilerfeatures component for details.

#include <bslscm_version.h>

#include <bslmf_assert.h>
#include <bslmf_detectnestedtrait.h>
#include <bslmf_integralconstant.h>
#include <bslmf_isarray.h>
#include <bslmf_isconst.h>
#include <bslmf_istriviallycopyable.h>
#include <bslmf_isvolatile.h>
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

namespace bsl {

template <class t_TYPE>
struct is_nothrow_move_constructible;

}  // close namespace bsl

///Implementation Notes
///--------------------
// In C++20 certain array types which decay to their element type, such as
// 'const void*' and 'bool', report as copy constructible through
// 'std::is_nothrow_move_constructible'.  In fact, initialization that attempts
// such move constructions does not result in a copy of the orginal array but
// instead initializes only the first element of the new array -- and that is
// set to the address of the original array, not the original's first element.
//
// This behavior has been observed in 'gcc-11' targeting C++20 and should occur
// with any compiler correctly supporting C++20 aggregate initialization with
// parenthesis (identified by the feature test macro
// '__cpp_aggregate_paren_init').
//
// An LWG issue (https://wg21.link/lwg####) has been filed to correct this
// behavior and continue to return 'false' for all array types.
//
// The implementation below preemptively implements the expected resolution of
// that issue on all platforms.

#define STD_IS_NOTHROW_MOVE_CONSTRUCTIBLE_VALUE(t_TYPE)                       \
    (bsl::is_array<t_TYPE>::value                                             \
         ? false                                                              \
         : ::std::is_nothrow_move_constructible<t_TYPE>::value)

namespace BloombergLP {
namespace bslmf {

                   // =====================================
                   // struct IsNothrowMoveConstructible_Imp
                   // =====================================

#if defined(BSLS_PLATFORM_CMP_SUN) &&                                         \
    (BSLS_PLATFORM_CMP_VERSION == 0x5150) &&                                  \
    (BSLS_COMPILERFEATURES_CPLUSPLUS == 199711L)
// Solaris 12.6 compiler in C++03 mode treats as ambiguous partial
// specializations with `t_TYPE` and with cv-qualified `t_TYPE`.
#define BSLMF_ISNOTHROWMOVECONSTRUCTIBLE_VOIDTYPE(t_TYPE)                     \
    typename bsl::enable_if<!bsl::is_const<t_TYPE>::value &&                  \
                                !bsl::is_volatile<t_TYPE>::value,             \
                            BSLMF_VOIDTYPE(int t_TYPE::*)>::type
#else
#define BSLMF_ISNOTHROWMOVECONSTRUCTIBLE_VOIDTYPE(t_TYPE)                     \
    BSLMF_VOIDTYPE(int t_TYPE::*)
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER)
template <class t_TYPE, class = void>
struct IsNothrowMoveConstructible_Impl
: bsl::integral_constant<bool,
                         STD_IS_NOTHROW_MOVE_CONSTRUCTIBLE_VALUE(t_TYPE)> {
    // This 'struct' template implements a metafunction to determine whether
    // the (non-cv-qualified) (template parameter) 't_TYPE' has a no-throw move
    // constructor.  Note that the partial specializations below will provide
    // the determination for class types.
};

template <class t_TYPE>
struct IsNothrowMoveConstructible_Impl<
    t_TYPE,
    BSLMF_ISNOTHROWMOVECONSTRUCTIBLE_VOIDTYPE(t_TYPE)>
: bsl::integral_constant<
      bool,
      STD_IS_NOTHROW_MOVE_CONSTRUCTIBLE_VALUE(t_TYPE) ||
          bsl::is_trivially_copyable<t_TYPE>::value ||
          DetectNestedTrait<t_TYPE,
                            bsl::is_nothrow_move_constructible>::value> {
    // This 'struct' template implements a metafunction to determine whether
    // the (non-cv-qualified) (template parameter) 't_TYPE' has a no-throw move
    // constructor.  To maintain consistency between the C++03 and C++11
    // implementations of this trait, types that use the BDE trait association
    // techniques are also detected as no-throw move constructible, even if the
    // 'noexcept' operator would draw a different conclusion.

    enum { k_CHECK_COMPLETE = sizeof(t_TYPE) };  // Diagnose incomplete types
};

template <class t_TYPE>
struct IsNothrowMoveConstructible_Impl<
    const t_TYPE,
    BSLMF_ISNOTHROWMOVECONSTRUCTIBLE_VOIDTYPE(t_TYPE)>
: bsl::integral_constant<
      bool,
      STD_IS_NOTHROW_MOVE_CONSTRUCTIBLE_VALUE(const t_TYPE) ||
          bsl::is_trivially_copyable<t_TYPE>::value> {
    enum { k_CHECK_COMPLETE = sizeof(t_TYPE) };  // Diagnose incomplete types
};

template <class t_TYPE>
struct IsNothrowMoveConstructible_Impl<
    volatile t_TYPE,
    BSLMF_ISNOTHROWMOVECONSTRUCTIBLE_VOIDTYPE(t_TYPE)>
: bsl::integral_constant<
      bool,
      STD_IS_NOTHROW_MOVE_CONSTRUCTIBLE_VALUE(volatile t_TYPE)> {
    enum { k_CHECK_COMPLETE = sizeof(t_TYPE) };  // Diagnose incomplete types
};

template <class t_TYPE>
struct IsNothrowMoveConstructible_Impl<
    const volatile t_TYPE,
    BSLMF_ISNOTHROWMOVECONSTRUCTIBLE_VOIDTYPE(t_TYPE)>
: bsl::integral_constant<
      bool,
      STD_IS_NOTHROW_MOVE_CONSTRUCTIBLE_VALUE(const volatile t_TYPE)> {
    enum { k_CHECK_COMPLETE = sizeof(t_TYPE) };   // Diagnose incomplete types
};
    // Partial specializations to avoid detecting cv-qualified class types with
    // nested trait declarations as having no-throw constructors that take
    // cv-qualified rvalues unless those constructors are actually detected as
    // non-throwing.  Note that volatile-qualified scalar types will detect as
    // no-throw move constructible through the primary template.  In addition,
    // we flag types that specialize 'bsl::is_trivially_copyable' as no-throw
    // move constructible to maintain consistency between the C++03 and C++11
    // implementations of this trait.

#undef STD_IS_NOTHROW_MOVE_CONSTRUCTIBLE_VALUE
#else
template <class t_TYPE, class = void>
struct IsNothrowMoveConstructible_Impl
: bsl::is_trivially_copyable<t_TYPE>::type {
    // This 'struct' template implements a metafunction to determine whether
    // the (non-cv-qualified) (template parameter) 't_TYPE' has a no-throw move
    // constructor.  For C++03, the set of types known to be no-throw move
    // constructible are all trivial types.  Note that the partial
    // specializations below will provide the determination for class types,
    // and this primary template is equivalent to querying whether 't_TYPE' is
    // a scalar type.
};

template <class t_TYPE>
struct IsNothrowMoveConstructible_Impl<
    t_TYPE,
    BSLMF_ISNOTHROWMOVECONSTRUCTIBLE_VOIDTYPE(t_TYPE)>
: bsl::integral_constant<
      bool,
      bsl::is_trivially_copyable<t_TYPE>::value ||
          DetectNestedTrait<t_TYPE,
                            bsl::is_nothrow_move_constructible>::value> {
    // This 'struct' template implements a metafunction to determine whether
    // the (non-cv-qualified) (template parameter) 't_TYPE' has a no-throw move
    // constructor.  To maintain consistency between the C++03 and C++11
    // implementations of this trait, types that use the BDE trait association
    // techniques are also detected as no-throw move constructible, even if the
    // 'noexcept' operator would draw a different conclusion.

    enum { k_CHECK_COMPLETE = sizeof(t_TYPE) };  // Diagnose incomplete types
};

template <class t_TYPE>
struct IsNothrowMoveConstructible_Impl<
    const t_TYPE,
    BSLMF_ISNOTHROWMOVECONSTRUCTIBLE_VOIDTYPE(t_TYPE)>
: bsl::is_trivially_copyable<t_TYPE>::type {
    enum { k_CHECK_COMPLETE = sizeof(t_TYPE) };  // Diagnose incomplete types
};

template <class t_TYPE>
struct IsNothrowMoveConstructible_Impl<
    volatile t_TYPE,
    BSLMF_ISNOTHROWMOVECONSTRUCTIBLE_VOIDTYPE(t_TYPE)> : bsl::false_type {
    enum { k_CHECK_COMPLETE = sizeof(t_TYPE) };  // Diagnose incomplete types
};

template <class t_TYPE>
struct IsNothrowMoveConstructible_Impl<
    const volatile t_TYPE,
    BSLMF_ISNOTHROWMOVECONSTRUCTIBLE_VOIDTYPE(t_TYPE)> : bsl::false_type {
    enum { k_CHECK_COMPLETE = sizeof(t_TYPE) };   // Diagnose incomplete types
};
    // Partial specializations to avoid detecting cv-qualified class types with
    // nested trait declarations as having no-throw constructors that take
    // cv-qualified rvalues unless those constructors are actually detected.
    // Note that volatile-qualified scalar types will detect as no-throw move
    // constructible through the primary template.  As a practical matter, we
    // assume that types flagged as trivially copyable have a no-throw copy
    // constructor, and so are no-throw move constructible too.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template <class t_TYPE>
struct IsNothrowMoveConstructible_Impl<t_TYPE&&> : bsl::true_type {
};
#endif
template <class t_TYPE>
struct IsNothrowMoveConstructible_Impl<t_TYPE&> : bsl::true_type {
};
    // Partial specializations to indicate that reference binding from an
    // identical reference can never throw.

template <class t_TYPE>
struct IsNothrowMoveConstructible_Impl<t_TYPE[]> : bsl::false_type {
};
template <class t_TYPE>
struct IsNothrowMoveConstructible_Impl<const t_TYPE[]> : bsl::false_type {
};
template <class t_TYPE>
struct IsNothrowMoveConstructible_Impl<volatile t_TYPE[]> : bsl::false_type {
};
template <class t_TYPE>
struct IsNothrowMoveConstructible_Impl<const volatile t_TYPE[]>
: bsl::false_type {
};
template <class t_TYPE, size_t t_LEN>
struct IsNothrowMoveConstructible_Impl<t_TYPE[t_LEN]> : bsl::false_type {
};
template <class t_TYPE, size_t t_LEN>
struct IsNothrowMoveConstructible_Impl<const t_TYPE[t_LEN]> : bsl::false_type {
};
template <class t_TYPE, size_t t_LEN>
struct IsNothrowMoveConstructible_Impl<volatile t_TYPE[t_LEN]>
: bsl::false_type {
};
template <class t_TYPE, size_t t_LEN>
struct IsNothrowMoveConstructible_Impl<const volatile t_TYPE[t_LEN]>
: bsl::false_type {
};
    // These partial specializations ensure that array types are not detected
    // as move constructible.  Note that while array data members of classes
    // will correctly move each element through move-construction (when
    // initializing the owning class), the standard defines the type trait in
    // terms of a variable declaration, where the move construction syntax is
    // invalid.

#endif
}  // close package namespace
}  // close enterprise namespace

namespace bsl {

                   // ====================================
                   // struct is_nothrow_move_constructible
                   // ====================================

template <class t_TYPE>
struct is_nothrow_move_constructible
: BloombergLP::bslmf::IsNothrowMoveConstructible_Impl<t_TYPE>::type {
    // This 'struct' template implements a metafunction to determine whether
    // the (template parameter) 't_TYPE' has a no-throw move constructor.  This
    // 'struct' derives from 'bsl::true_type' if the 't_TYPE' has a no-throw
    // move constructor, and from 'bsl::false_type' otherwise.  This
    // metafunction has the same syntax as the 'is_nothrow_move_constructible'
    // metafunction defined in the C++11 standard [meta.unary.prop]; on C++03
    // platforms, however, this metafunction can automatically determine the
    // value for trivially copyable types (including scalar types), for
    // reference types, and for class types associating with the
    // 'bsl::is_nothrow_move_constructible' trait using the
    // 'BSLMF_NESTED_TRAIT_DECLARATION' macro.  To support other no-throw move
    // constructible types, this template should be specialized to inherit from
    // 'bsl::true_type'.  Note that cv-qualified user defined types are rarely
    // no-throw move constructible unless they are also trivially copyable, so
    // there are no cv-qualified partial specializations of this trait.
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
template <class t_TYPE>
BSLS_KEYWORD_INLINE_VARIABLE constexpr bool is_nothrow_move_constructible_v =
                                  is_nothrow_move_constructible<t_TYPE>::value;
    // This template variable represents the result value of the
    // 'bsl::is_nothrow_move_constructible' metafunction.
#endif

}  // close namespace bsl

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
