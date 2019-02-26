// bslmf_isfunction.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLMF_ISFUNCTION
#define INCLUDED_BSLMF_ISFUNCTION

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for determining function types.
//
//@CLASSES:
//  bsl::is_function: standard meta-function for determining function types
//  bsl::is_function_v: the result value of 'bsl::is_function'
//
//@SEE_ALSO: bslmf_integralconstant
//
//@AUTHOR:
//
//@DESCRIPTION: This component defines a meta-function, 'bsl::is_function' and
// a template variable 'bsl::is_function_v', that represents the result value
// of the 'bsl::is_function' meta-function, that may be used to query whether a
// template parameter type is a function type.
//
// 'bsl::is_function' meets the requirements of the 'is_function' template
// defined in the C++11 standard [meta.unary.cat].
//
// Note that the template variable 'is_function_v' is defined in the C++17
// standard as an inline variable.  If the current compiler supports the inline
// variable C++17 compiler feature, 'bsl::is_function_v' is defined as an
// 'inline constexpr bool' variable.  Otherwise, if the compiler supports the
// variable templates C++14 compiler feature, 'bsl::is_function_v' is defined
// as a non-const 'constexpr bool' variable.  See
// 'BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES' and
// 'BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES' macros in
// bsls_compilerfeatures component for details.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify Function Types
/// - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a set of types are function types.
//
// Now, we instantiate the 'bsl::is_function' template for both a non-function
// type and a function type, and assert the 'value' static data member of each
// instantiation:
//..
//  assert(false == bsl::is_function<int>::value);
//  assert(true  == bsl::is_function<int (int)>::value);
//..
// Note that if the current compiler supports the variable templates C++14
// feature, then we can re-write the snippet of code above using the
// 'bsl::is_function_v' variable as follows:
//..
//#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
//  assert(false == bsl::is_function_v<int>);
//  assert(true  == bsl::is_function_v<int (int)>);
//#endif
//..

#include <bslscm_version.h>

#include <bslmf_integralconstant.h>
#include <bslmf_isconst.h>
#include <bslmf_voidtype.h>

#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_platform.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bslmf_addpointer.h>
#include <bslmf_functionpointertraits.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#if !defined(BSLS_PLATFORM_CMP_IBM) && !defined(BSLS_PLATFORM_CMP_SUN)
// This is the simplest implementation, for conforming compilers.  It relies
// only on the property that function types and reference types are the only
// type categories that cannot be explicitly cv-qualified.

namespace bsl {

# ifdef BSLS_PLATFORM_CMP_MSVC
#   pragma warning(push)
#   pragma warning(disable: 4180) // cv-qualifier does not change function type
# endif

                        // ====================
                        // struct 'is_function'
                        // ====================

template <class TYPE>
struct is_function
    :  bsl::integral_constant<bool, !is_const<const TYPE>::value> {
    // This 'struct' template implements the 'is_function' meta-function
    // defined in the C++11 standard [meta.unary.cat] to determine if the
    // (template parameter) 'TYPE' is a function type.  This 'struct' derives
    // from 'bsl::true_type' if the 'TYPE' is a function type, and from
    // 'bsl::false_type' otherwise.  This implementation relies on the fact
    // that neither function types nor reference types can be cv-qualified so
    // that 'is_const<const TYPE>' will actually yield 'false'.
};

# ifdef BSLS_PLATFORM_CMP_MSVC
#   pragma warning(pop)
# endif

template <class TYPE>
struct is_function<TYPE &> : false_type {
    // Reference types are, self-evidently, never function types.  The idiom
    // for detecting function types in this component is that a function is a
    // type that is the same as the const-qualified version of that same type.
    // As references also have this property, we must filter out references
    // with this partial specialization.
};

# if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template <class TYPE>
struct is_function<TYPE &&> : false_type {
    // Reference types are, self-evidently, never function types.  The idiom
    // for detecting function types in this component is that a function is a
    // type that is the same as the const-qualified version of that same type.
    // As references also have this property, we must filter out references
    // with this partial specialization.
};
# endif

# ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_function_v = is_function<TYPE>::value;
    // This template variable represents the result value of the
    // 'bsl::is_function' meta-function.
# endif
}  // close namespace bsl
#else  // BSLS_PLATFORM_CMP_IBM / BSLS_PLATFORM_CMP_SUN
// The IBM xlC compiler produces a "hard" error (not eligible for SFINAE) when
// trying to apply cv-qualifiers to a template parameter (or typedef) that is a
// function type.  Hence, a more oblique approach is needed to detect all
// function types on this platform.  This implementation relies on the fact
// that you cannot form an array of function types.

// While Sun CC can use the preferred implementation above, it also spams a lot
// of warnings about spurious cv-qualifiers.  This alternative approach should
// avoid that concern, and is almost as simple.  Measuring the resulting test
// driver size and compile time suggests that the implementations are
// interchangable, so prefer the one with fewer warnings.

namespace BloombergLP {
namespace bslmf {

template <class TYPE, class = void>
struct IsFunction_Imp;
    // Forward declaration of implementation detail class that will perform
    // most of the metaprogramming to determine whether the template parameter
    // 'TYPE' is a function type.  The second template parameter is unused,
    // other than providing a context for SFINAE-able type expressions.

} // close package namespace
} // close enterprise namespace

namespace bsl {
                        // ====================
                        // struct 'is_function'
                        // ====================

template <class TYPE>
struct is_function : BloombergLP::bslmf::IsFunction_Imp<TYPE>::type {
    // This 'struct' template implements the 'is_function' meta-function
    // defined in the C++11 standard [meta.unary.cat] to determine if the
    // (template parameter) 'TYPE' is a function type.  This 'struct' derives
    // from 'bsl::true_type' if the 'TYPE' is a function type, and from
    // 'bsl::false_type' otherwise.
};

template <class TYPE>           struct is_function<TYPE[ ]> : false_type {};
template <class TYPE, size_t N> struct is_function<TYPE[N]> : false_type {};
    // Array types are never functions, but pass the filter in the 'Imp' trait
    // above on both compilers relying on this workaround, regardless of what
    // the stnadard says.

# if defined(BSLS_PLATFORM_CMP_SUN)
// The Sun implementation below relies on function types not being valid array
// elements.  We must specialize for any other types that are not valid array
// elements.

template <class TYPE>
struct is_function<TYPE&> : false_type {};
    // Reference types are never functions, nor can they be array elements, so
    // a partial specialization is required to give the correct result.  The
    // Sun CC compiler does not support rvalue-references, so there is no need
    // to conditionally add an extra specialization to handle those.

template <> struct is_function<void>                : false_type {};
template <> struct is_function<const void>          : false_type {};
template <> struct is_function<volatile void>       : false_type {};
template <> struct is_function<const volatile void> : false_type {};
    // 'void' is not a function type, nor can it be array element, so explicit
    // specializations are required to give the correct result.
# endif
}  // close namespace bsl

// ============================================================================
//                          CLASS TEMPLATE DEFINTIONS
// ============================================================================

namespace BloombergLP {
namespace bslmf {

                    // ====================================
                    // struct 'IsFunction_FilterClassTypes'
                    // ====================================

template <class TYPE, class = void>
struct IsFunction_FilterClassTypes : bsl::true_type {
    // This 'struct' template implements a meta-function to determine if the
    // (template parameter) 'TYPE' is not a class (or union) type.  This
    // 'struct' derives from 'bsl::false_type' if the 'TYPE' is a class type,
    // and from 'bsl::true_type' otherwise.  Note that due to other filters
    // applied before querying this implementation-detail trait, a 'true_type'
    // result implies that 'TYPE' must be a function type.
};

template <class TYPE>
struct IsFunction_FilterClassTypes<TYPE, BSLMF_VOIDTYPE(int TYPE::*)>
    : bsl::false_type {
    // This partial specialization provide the 'false_type' result for all
    // (potentially incomplete or abstract) class types.  All other object
    // types, will be filterered at earlier stages of decuction.
};

                        // =======================
                        // struct 'IsFunction_Imp'
                        // =======================

template <class TYPE, class>
struct IsFunction_Imp : IsFunction_FilterClassTypes<TYPE>::type {
    // The template paramter 'TYPE' shall be a function type, or an incomplete
    // or abstract class type.  We filter array, referenence, cv-'void', and
    // complete non-abstract object types at the higher level trait; that just
    // leaves incomplete and abstract cv-qualified class types to distinguish
    // from functions, in a warning-free manner.
};

# if defined(BSLS_PLATFORM_CMP_IBM)
template <class TYPE>
struct IsFunction_Imp<TYPE, BSLMF_VOIDTYPE(TYPE())> : bsl::false_type {
    // This partial specialization provide the 'false_type' result for all
    // (non-abstract) object types, reference types, and cv-'void'.
};

#else // Sun CC
// Sun CC permits template parameters of function types that return functions,
// so we need a different workaround for this platform.

template <class TYPE>
struct IsFunction_Imp<TYPE, BSLMF_VOIDTYPE(TYPE[sizeof(TYPE)])>
    : bsl::false_type {
    // This partial specialization provide the 'false_type' result for all
    // complete (non-abstract) object types.  The 'sizeof' is required to
    // remove function types (and potentially incomplete class types) from
    // this filter, as Sun CC will otherwise accept arrays of function types as
    // a valid template type parameter.
};
# endif // Sun CC

} // close package namespace
} // close enterprise namespace
#endif  // BSLS_PLATFORM_CMP_IBM / BSLS_PLATFORM_CMP_SUN

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
