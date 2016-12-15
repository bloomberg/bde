// bslmf_iscopyconstructible.h                                        -*-C++-*-
#ifndef INCLUDED_BSLMF_ISCOPYCONSTRUCTIBLE
#define INCLUDED_BSLMF_ISCOPYCONSTRUCTIBLE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function to report if a type is copy constructible.
//
//@CLASSES:
//  bsl::is_copy_constructible: type-traits meta-function
//
//@SEE_ALSO: bslmf_integralconstant, bslmf_nestedtraitdeclaration
//
//@DESCRIPTION: This component defines a meta-function,
// 'bsl::is_copy_constructible', that may be used to query whether a type is
// copy constructible.
//
// 'bsl::is_copy_constructible' has the same syntax as the
// 'is_copy_constructible' template from the C++11 standard [meta.unary.prop].
// Indeed, in C++11 compilation environments, 'bsl::is_copy_constructible'
// simply forwards to the native implementation, which can determine the
// correct value for all types without requiring specialization; in C++03
// environments, 'bsl::is_copy_construcible' provides welcome backward
// compatibility but returns 'true' for all user-defined types and requires
// explicit specialization for types that are not copy constructible (e.g.,
// move-only types).
//
// NOTE: The 'bsl::is_copy_constructible' trait cannot be declared as a nested
// trait because the default value of the trait is 'true' and nested traits
// work properly only for traits that have default value 'false'.  In order to
// indicate that a certain type 'T' is not copy constructible, the following
// idiom should be used:
//..
//   namespace bsl {
//       template <>
//       struct is_copy_constructible<T> : false_type { };
//   }
//..
//

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_DETECTNESTEDTRAIT
#include <bslmf_detectnestedtrait.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

#ifndef INCLUDED_BSLMF_ISFUNCTION
#include <bslmf_isfunction.h>
#endif

#ifndef INCLUDED_BSLMF_ISREFERENCE
#include <bslmf_isreference.h>
#endif

#ifndef INCLUDED_BSLMF_ISVOLATILE
#include <bslmf_isvolatile.h>
#endif

#ifndef INCLUDED_BSLMF_ISTRIVIALLYCOPYABLE
#include <bslmf_istriviallycopyable.h>
#endif

#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER)

#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#ifndef INCLUDED_TYPE_TRAITS
# define BSLMF_INCLUDE_ONLY_NATIVE_TRAITS
# include <type_traits>
#endif

#endif // BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER

#if defined(BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER)                      \
 && (!defined(BSLS_PLATFORM_CMP_MSVC) || BSLS_PLATFORM_CMP_VERSION > 1700)
    // The VisualC++ 2010 type traits library does not implement this trait,
    // while the VC 2012 library does provide the trait, but gives the wrong
    // answer in several cases.
# define BSLS_ISCOPYCONSTRUCTIBLE_USE_NATIVE_TRAIT 1
#endif

namespace bsl {

                        // ============================
                        // struct is_copy_constructible
                        // ============================

template <class TYPE>
struct is_copy_constructible;
    // This 'struct' template implements a meta-function to determine whether
    // the (template parameter) 'TYPE' is copy constructible.  This 'struct'
    // derives from 'bsl::true_type' if the 'TYPE' is copy constructible, and
    // from 'bsl::false_type' otherwise.  This meta-function has the same
    // syntax as the 'is_copy_constructible' meta-function defined in the C++11
    // standard [meta.unary.prop]; on C++03 platforms, however, this
    // meta-function defaults to 'true_type' for all types that are not
    // explicitly declared to have the 'bslmf::IsNonCopyable' trait using the
    // 'BSLMF_NESTED_TRAIT_DECLARATION' macro.  To mark a type as non-copyable,
    // 'bslmf::IsNonCopyable' must be specialized (for that type) to inherit
    // from 'bsl::true_type'.

}  // close namespace bsl

// ============================================================================
//                          CLASS TEMPLATE DEFINITIONS
// ============================================================================

#if !defined(BSLS_ISCOPYCONSTRUCTIBLE_USE_NATIVE_TRAIT)
namespace BloombergLP {
namespace bslmf {

                       // ==============================
                       // struct IsCopyConstructible_Imp
                       // ==============================

template <class TYPE>
struct IsCopyConstructible_Imp
    : bsl::integral_constant<bool,
                             bsl::is_trivially_copyable<TYPE>::value
                          || bsl::is_reference<TYPE>::value
                          || !(bsl::is_volatile<TYPE>::value
                            || bsl::is_function<TYPE>::value)>
{
    // This 'struct' template implements a meta-function to determine whether
    // the (non-cv-qualified) (template parameter) 'TYPE' has a copy
    // constructor.
};

template <>
struct IsCopyConstructible_Imp<void> : bsl::false_type
{
    // This explicit specialization reports that 'void' does not have a copy
    // constructor, despite being a fundamental type.
};

template <>
struct IsCopyConstructible_Imp<volatile void> : bsl::false_type
{
    // This explicit specialization reports that 'volatile void' does not have
    // a copy constructor, despite being a fundamental type.
};

template <class TYPE>
struct IsCopyConstructible_Imp<TYPE *volatile> : bsl::true_type
{
    // This explicit specialization reports that volatile pointer objects have
    // a copy constructor, just like a fundamental type.
};

}  // close package namespace
}  // close enterprise namespace

namespace bsl {

                        // ====================================
                        // struct is_copy_constructible (C++03)
                        // ====================================

template <class TYPE>
struct is_copy_constructible
: BloombergLP::bslmf::IsCopyConstructible_Imp<TYPE>::type {
    // The primary template for this traits handles only non-const-qualified
    // types; partial specializations will handle some interesting cases,
    // including the remaining const-qualified types.
};

template <class TYPE>
struct is_copy_constructible<const TYPE> : is_copy_constructible<TYPE>::type
{
    // This partial specialization ensures that const-qualified types have the
    // same result as their element type.
};

template <class TYPE, size_t LEN>
struct is_copy_constructible<TYPE[LEN]> : false_type
{
    // This partial specialization ensures that array types have the result
    // 'false'.
};

template <class TYPE, size_t LEN>
struct is_copy_constructible<const TYPE[LEN]> : false_type
{
    // This partial specialization ensures that const-qualified array types
    // have result 'false'.
};

template <class TYPE, size_t LEN>
struct is_copy_constructible<volatile TYPE[LEN]> : false_type
{
    // This partial specialization ensures that volatile-qualified array types
    // have the result 'false'.
};

template <class TYPE, size_t LEN>
struct is_copy_constructible<const volatile TYPE[LEN]> : false_type
{
    // This partial specialization ensures that const-volatile-qualified array
    // types have the result false.
};

#if !defined(BSLS_PLATFORM_CMP_IBM)
// Last checked with the xlC 12.1 compiler.  The IBM xlC compiler has problems
// correctly handling arrays of unknown bound as template parameters.

template <class TYPE>
struct is_copy_constructible<TYPE[]> : false_type
{
    // This partial specialization ensures that array-of-unknown-bound types
    // have the result 'false'.
};

template <class TYPE>
struct is_copy_constructible<const TYPE[]> : false_type
{
    // This partial specialization ensures that const-qualified
    // array-of-unknown-bound types have the result 'false'.
};

template <class TYPE>
struct is_copy_constructible<volatile TYPE[]> : false_type
{
    // This partial specialization ensures that volatile-qualified
    // array-of-unknown-bound types have the result 'false'.
};

template <class TYPE>
struct is_copy_constructible<const volatile TYPE[]> : false_type
{
    // This partial specialization ensures that const-volatile-qualified
    // array-of-unknown-bound types have the result 'false'.
};

#endif  // defined(BSLS_PLATFORM_CMP_IBM)

}  // close namespace bsl
#else
namespace bsl {

                        // ====================================
                        // struct is_copy_constructible (C++11)
                        // ====================================

template <class TYPE>
struct is_copy_constructible
    : bsl::integral_constant<bool,
                             ::native_std::is_copy_constructible<TYPE>::value>
{
    // Defer entirely to the native trait on supported C++11 compilers.
};

}  // close namespace bsl
#endif  // !defined(BSLS_ISCOPYCONSTRUCTIBLE_USE_NATIVE_TRAIT)

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
