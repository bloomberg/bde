// bslmf_isnothrowmoveconstructible.h                                 -*-C++-*-
#ifndef INCLUDED_BSLMF_ISNOTHROWMOVECONSTRUCTIBLE
#define INCLUDED_BSLMF_ISNOTHROWMOVECONSTRUCTIBLE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide meta-function for determining types with nothrow move ctor.
//
//@REVIEW_FOR_MASTER: component-level doc, usage example
//
//@CLASSES:
//  bslmf::is_nothrow_move_constructible: type-traits meta-function
//
//@SEE_ALSO: bslmf_integralconstant, bslmf_nestedtraitdeclaration
//
//@DESCRIPTION: TBD

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_DETECTNESTEDTRAIT
#include <bslmf_detectnestedtrait.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

#ifndef INCLUDED_BSLMF_ISENUM
#include <bslmf_isenum.h>
#endif

#ifndef INCLUDED_BSLMF_ISFUNDAMENTAL
#include <bslmf_isfundamental.h>
#endif

#ifndef INCLUDED_BSLMF_ISPOINTER
#include <bslmf_ispointer.h>
#endif

#ifndef INCLUDED_BSLMF_ISPOINTERTOMEMBER
#include <bslmf_ispointertomember.h>
#endif

#ifndef INCLUDED_BSLMF_ISREFERENCE
#include <bslmf_isreference.h>
#endif

#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
#endif

#ifdef BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER

#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#ifndef INCLUDED_TYPE_TRAITS
# define BSLMF_INCLUDE_ONLY_NATIVE_TRAITS
# include <type_traits>
#endif

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

#ifdef BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
template <class TYPE>
struct IsNothrowMoveConstructible_Imp
    : bsl::integral_constant<
        bool,
        ::native_std::is_nothrow_move_constructible<TYPE>::value
            || DetectNestedTrait<TYPE,
                                 bsl::is_nothrow_move_constructible>::value>
{
    // This 'struct' template implements a meta-function to determine whether
    // the (non-cv-qualified) (template parameter) 'TYPE' has a 'nothrow' move
    // constructor.
};
#else
template <class TYPE>
struct IsNothrowMoveConstructible_Imp
    : bsl::integral_constant<
           bool, bsl::is_reference<TYPE>::value
              || bsl::is_fundamental<TYPE>::value
              || IsEnum<TYPE>::value
              || bsl::is_pointer<TYPE>::value
              || IsPointerToMember<TYPE>::value
              || DetectNestedTrait<TYPE,
                                   bsl::is_nothrow_move_constructible>::value>
{
    // This 'struct' template implements a meta-function to determine whether
    // the (non-cv-qualified) (template parameter) 'TYPE' has a 'nothrow' move
    // constructor.
};
#endif

template <>
struct IsNothrowMoveConstructible_Imp<void> : bsl::false_type
{
    // This explicit specialization reports that 'void' does not have a
    // 'nothrow' move constructor, despite being a fundamental type.
};

}  // close package namespace
}  // close enterprise namespace

namespace bsl {

                   // ====================================
                   // struct is_nothrow_move_constructible
                   // ====================================

template <class TYPE>
struct is_nothrow_move_constructible
: BloombergLP::bslmf::IsNothrowMoveConstructible_Imp<TYPE>::type {
    // This 'struct' template implements a meta-function to determine whether
    // the (template parameter) 'TYPE' has a 'nothrow' move constructor.  This
    // 'struct' derives from 'bsl::true_type' if the 'TYPE' has a 'nothrow'
    // move constructor, and from 'bsl::false_type' otherwise.  This
    // meta-function has the same syntax as the 'is_nothrow_move_constructible'
    // meta-function defined in the C++11 standard [meta.unary.prop]; on C++03
    // platforms, however, this meta-function can automatically determine the
    // value for the following types only: reference types, fundamental types,
    // enumerated types, pointers to members, and types declared to have the
    // 'bsl::is_nothrow_move_constructible' trait using the
    // 'BSLMF_NESTED_TRAIT_DECLARATION' macro (the value for other types
    // defaults to 'false').  To support other 'nothrow' move constructible
    // types, this template must be specialized to inherit from
    // 'bsl::true_type' for them.
};

// TBD: check to make sure these are correct
template <class TYPE>
struct is_nothrow_move_constructible<const TYPE>
    :  is_nothrow_move_constructible<TYPE>::type {
    // This partial specialization ensures that const-qualified types have the
    // same result as their element type.
};

template <class TYPE>
struct is_nothrow_move_constructible<volatile TYPE>
    :  is_nothrow_move_constructible<TYPE>::type {
    // This partial specialization ensures that volatile-qualified types have
    // the same result as their element type.
};

template <class TYPE>
struct is_nothrow_move_constructible<const volatile TYPE>
    :  is_nothrow_move_constructible<TYPE>::type {
    // This partial specialization ensures that const-volatile-qualified types
    // have the same result as their element type.
};

template <class TYPE, size_t LEN>
struct is_nothrow_move_constructible<TYPE[LEN]>
    :  is_nothrow_move_constructible<TYPE>::type {
    // This partial specialization ensures that array types have the same
    // result as their element type.
};

template <class TYPE, size_t LEN>
struct is_nothrow_move_constructible<const TYPE[LEN]>
    :  is_nothrow_move_constructible<TYPE>::type {
    // This partial specialization ensures that const-qualified array types
    // have the same result as their element type.
};

template <class TYPE, size_t LEN>
struct is_nothrow_move_constructible<volatile TYPE[LEN]>
    :  is_nothrow_move_constructible<TYPE>::type {
    // This partial specialization ensures that volatile-qualified array types
    // have the same result as their element type.
};

template <class TYPE, size_t LEN>
struct is_nothrow_move_constructible<const volatile TYPE[LEN]>
    :  is_nothrow_move_constructible<TYPE>::type {
    // This partial specialization ensures that const-volatile-qualified array
    // types have the same result as their element type.
};

#if !defined(BSLS_PLATFORM_CMP_IBM)
// Last checked with the xlC 12.1 compiler.  The IBM xlC compiler has problems
// correctly handling arrays of unknown bound as template parameters.

template <class TYPE>
struct is_nothrow_move_constructible<TYPE[]>
    :  is_nothrow_move_constructible<TYPE>::type {
    // This partial specialization ensures that array-of-unknown-bound types
    // have the same result as their element type.
};

template <class TYPE>
struct is_nothrow_move_constructible<const TYPE[]>
    :  is_nothrow_move_constructible<TYPE>::type {
    // This partial specialization ensures that const-qualified
    // array-of-unknown-bound types have the same result as their element type.
};

template <class TYPE>
struct is_nothrow_move_constructible<volatile TYPE[]>
    :  is_nothrow_move_constructible<TYPE>::type {
    // This partial specialization ensures that volatile-qualified
    // array-of-unknown-bound types have the same result as their element type.
};

template <class TYPE>
struct is_nothrow_move_constructible<const volatile TYPE[]>
    :  is_nothrow_move_constructible<TYPE>::type {
    // This partial specialization ensures that const-volatile-qualified
    // array-of-unknown-bound types have the same result as their element type.
};
#endif

}  // close namespace bsl

#endif

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
