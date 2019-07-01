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
#include <bslmf_istriviallycopyable.h>
#include <bslmf_voidtype.h>

#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_platform.h>

#include <stddef.h>

#ifdef BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
# include <bsls_nativestd.h>

# define BSLMF_INCLUDE_ONLY_NATIVE_TRAITS
# include <type_traits>
#endif // BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER

namespace bsl {

template <class TYPE>
struct is_nothrow_move_constructible;

}  // close namespace bsl

namespace BloombergLP {
namespace bslmf {

                   // =====================================
                   // struct IsNothrowMoveConstructible_Imp
                   // =====================================

#if defined(BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER)
template <class TYPE, class = void>
struct IsNothrowMoveConstructible_Impl
    : bsl::integral_constant<
                    bool,
                    ::native_std::is_nothrow_move_constructible<TYPE>::value> {
    // This 'struct' template implements a metafunction to determine whether
    // the (non-cv-qualified) (template parameter) 'TYPE' has a no-throw move
    // constructor.  Note that the partial specializations below will provide
    // the determination for class types.
};

template <class TYPE>
struct IsNothrowMoveConstructible_Impl<TYPE,
                                       typename VoidType<int TYPE::*>::type>
    : bsl::integral_constant<
       bool,
       ::native_std::is_nothrow_move_constructible<TYPE>::value
       || bsl::is_trivially_copyable<TYPE>::value
       || DetectNestedTrait<TYPE, bsl::is_nothrow_move_constructible>::value> {
    // This 'struct' template implements a metafunction to determine whether
    // the (non-cv-qualified) (template parameter) 'TYPE' has a no-throw move
    // constructor.  To maintain consistency between the C++03 and C++11
    // implementations of this trait, types that use the BDE trait association
    // techniques are also detected as no-throw move constructible, even if the
    // 'noexcept' operator would draw a different conclusion.

    BSLMF_ASSERT(sizeof(TYPE) != 0);    // Diagnose incomplete types
};

template <class TYPE>
struct IsNothrowMoveConstructible_Impl<const TYPE,
                                       typename VoidType<int TYPE::*>::type>
    : bsl::integral_constant<
                 bool,
                 ::native_std::is_nothrow_move_constructible<const TYPE>::value
                 || bsl::is_trivially_copyable<TYPE>::value> {
    BSLMF_ASSERT(sizeof(TYPE) != 0);    // Diagnose incomplete types
};
template <class TYPE>
struct IsNothrowMoveConstructible_Impl<volatile TYPE,
                                       typename VoidType<int TYPE::*>::type>
    : bsl::integral_constant<
           bool,
           ::native_std::is_nothrow_move_constructible<volatile TYPE>::value> {
    BSLMF_ASSERT(sizeof(TYPE) != 0);    // Diagnose incomplete types
};
template <class TYPE>
struct IsNothrowMoveConstructible_Impl<const volatile TYPE,
                                       typename VoidType<int TYPE::*>::type>
    : bsl::integral_constant<bool,
                             ::native_std::is_nothrow_move_constructible<
                                                 const volatile TYPE>::value> {
    BSLMF_ASSERT(sizeof(TYPE) != 0);    // Diagnose incomplete types
};
    // Partial specializations to avoid detecting cv-qualified class types with
    // nested trait declarations as having no-throw constructors that take
    // cv-qualified rvalues unless those constructors are actually detected as
    // non-throwing.  Note that volatile-qualified scalar types will detect as
    // no-throw move constructible through the primary template.  In addition,
    // we flag types that specialize 'bsl::is_trivially_copyable' as no-throw
    // move constructible to maintain consistency between the C++03 and C++11
    // implementations of this trait.

#else
template <class TYPE, class = void>
struct IsNothrowMoveConstructible_Impl
    : bsl::is_trivially_copyable<TYPE>::type {
    // This 'struct' template implements a metafunction to determine whether
    // the (non-cv-qualified) (template parameter) 'TYPE' has a no-throw move
    // constructor.  For C++03, the set of types known to be no-throw move
    // constructible are all trivial types.  Note that the partial
    // specializations below will provide the determination for class types,
    // and this primary template is equivalent to querying whether 'TYPE' is a
    // scalar type.
};

template <class TYPE>
struct IsNothrowMoveConstructible_Impl<TYPE,
                                       typename VoidType<int TYPE::*>::type>
    : bsl::integral_constant<
          bool, bsl::is_trivially_copyable<TYPE>::value
             || DetectNestedTrait<TYPE,
                                  bsl::is_nothrow_move_constructible>::value> {
    // This 'struct' template implements a metafunction to determine whether
    // the (non-cv-qualified) (template parameter) 'TYPE' has a no-throw move
    // constructor.  To maintain consistency between the C++03 and C++11
    // implementations of this trait, types that use the BDE trait association
    // techniques are also detected as no-throw move constructible, even if the
    // 'noexcept' operator would draw a different conclusion.

    BSLMF_ASSERT(sizeof(TYPE) != 0);    // Diagnose incomplete types
};

template <class TYPE>
struct IsNothrowMoveConstructible_Impl<const TYPE,
                                       typename VoidType<int TYPE::*>::type>
    : bsl::is_trivially_copyable<TYPE>::type {
    BSLMF_ASSERT(sizeof(TYPE) != 0);    // Diagnose incomplete types
};
template <class TYPE>
struct IsNothrowMoveConstructible_Impl<volatile TYPE,
                                       typename VoidType<int TYPE::*>::type>
    : bsl::false_type {
    BSLMF_ASSERT(sizeof(TYPE) != 0);    // Diagnose incomplete types
};
template <class TYPE>
struct IsNothrowMoveConstructible_Impl<const volatile TYPE,
                                       typename VoidType<int TYPE::*>::type>
    : bsl::false_type {
    BSLMF_ASSERT(sizeof(TYPE) != 0);    // Diagnose incomplete types
};
    // Partial specializations to avoid detecting cv-qualified class types with
    // nested trait declarations as having no-throw constructors that take
    // cv-qualified rvalues unless those constructors are actually detected.
    // Note that volatile-qualified scalar types will detect as no-throw move
    // constructible through the primary template.  As a practical matter, we
    // assume that types flagged as trivially copyable have a no-throw copy
    // constructor, and so are no-throw move constructible too.


# if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template <class TYPE>
struct IsNothrowMoveConstructible_Impl<TYPE&&> : bsl::true_type {};
# endif
template <class TYPE>
struct IsNothrowMoveConstructible_Impl<TYPE&> : bsl::true_type {};
    // Partial specializations to indicate that reference binding from an
    // identical reference can never throw.

template <class TYPE, size_t LEN>
struct IsNothrowMoveConstructible_Impl<TYPE[LEN]>
    :  bsl::false_type {};
template <class TYPE, size_t LEN>
struct IsNothrowMoveConstructible_Impl<const TYPE[LEN]>
    :  bsl::false_type {};
template <class TYPE, size_t LEN>
struct IsNothrowMoveConstructible_Impl<volatile TYPE[LEN]>
    :  bsl::false_type {};
template <class TYPE, size_t LEN>
struct IsNothrowMoveConstructible_Impl<const volatile TYPE[LEN]>
    :  bsl::false_type {};
    // These partial specializations ensures that array types are not detected
    // as movable.  Note that there is no need to specialize for arrays of
    // unknown bound, and the primary template for this trait delegated to
    // 'bsl::is_trivially_copyable', which always produces 'false_type' for
    // arrays of unknown bound.

#endif
}  // close package namespace
}  // close enterprise namespace

namespace bsl {

                   // ====================================
                   // struct is_nothrow_move_constructible
                   // ====================================

template <class TYPE>
struct is_nothrow_move_constructible
    : BloombergLP::bslmf::IsNothrowMoveConstructible_Impl<TYPE>::type {
    // This 'struct' template implements a metafunction to determine whether
    // the (template parameter) 'TYPE' has a no-throw move constructor.  This
    // 'struct' derives from 'bsl::true_type' if the 'TYPE' has a no-throw move
    // constructor, and from 'bsl::false_type' otherwise.  This metafunction
    // has the same syntax as the 'is_nothrow_move_constructible' metafunction
    // defined in the C++11 standard [meta.unary.prop]; on C++03 platforms,
    // however, this metafunction can automatically determine the value for
    // trivially copyable types (including scalar types), for reference types,
    // and for class types associating with the
    // 'bsl::is_nothrow_move_constructible' trait using the
    // 'BSLMF_NESTED_TRAIT_DECLARATION' macro.  To support other no-throw move
    // constructible types, this template should be specialized to inherit from
    // 'bsl::true_type'.  Note that cv-qualified user defined types are rarely
    // no-throw move constructible unless they are also trivially copyable, so
    // there are no cv-qualified partial specializations of this trait.
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_nothrow_move_constructible_v
                                  = is_nothrow_move_constructible<TYPE>::value;
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
