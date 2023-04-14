// bslmf_ispointer.h                                                  -*-C++-*-
#ifndef INCLUDED_BSLMF_ISPOINTER
#define INCLUDED_BSLMF_ISPOINTER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for pointer types.
//
//@CLASSES:
//  bsl::is_pointer: standard meta-function for determining pointer types
//  bsl::is_pointer_v: the result value of the 'bsl::is_pointer' meta-function
//  bsl::IsPointer: meta-function for determining pointer types
//
//@SEE_ALSO: bslmf_integralconstant
//
//@DESCRIPTION: This component defines two meta-functions, 'bsl::is_pointer'
// and 'BloombergLP::bslmf::IsPointer' and a template variable
// 'bsl::is_pointer_v', that represents the result value of the
// 'bsl::is_pointer' meta-function.  All these meta-functions may be used to
// query whether or not a type is a pointer type.
//
// 'bsl::is_pointer' meets the requirements of the 'is_pointer' template
// defined in the C++11 standard [meta.unary.cat], while 'bslmf::IsPointer' was
// devised before 'is_pointer' was standardized.
//
// The two meta-functions are functionally equivalent.  The major difference
// between them is that the result for 'bsl::is_pointer' is indicated by the
// class member 'value', while the result for 'bslmf::IsPointer' is indicated
// by the class member 'VALUE'.
//
// Note that 'bsl::is_pointer' should be preferred over 'bslmf::IsPointer', and
// in general, should be used by new components.
//
// Also note that the template variable 'is_pointer_v' is defined in the C++17
// standard as an inline variable.  If the current compiler supports the inline
// variable C++17 compiler feature, 'bsl::is_pointer_v' is defined as an
// 'inline constexpr bool' variable.  Otherwise, if the compiler supports the
// variable templates C++14 compiler feature, 'bsl::is_pointer_v' is
// defined as a non-inline 'constexpr bool' variable.  See
// 'BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES' and
// 'BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES' macros in
// bsls_compilerfeatures component for details.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify Pointer Types
///- - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a particular type is a pointer type.
//
// First, we create two 'typedef's -- a pointer type and a non-pointer type:
//..
//  typedef int  MyType;
//  typedef int *MyPtrType;
//..
// Now, we instantiate the 'bsl::is_pointer' template for each of the
// 'typedef's and assert the 'value' static data member of each instantiation:
//..
//  assert(false == bsl::is_pointer<MyType>::value);
//  assert(true  == bsl::is_pointer<MyPtrType>::value);
//..
// Note that if the current compiler supports the variable templates C++14
// feature then we can re-write the snippet of code above using the
// 'bsl::is_pointer_v' variable as follows:
//..
//#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
//  assert(false == bsl::is_pointer_v<MyType>);
//  assert(true  == bsl::is_pointer_v<MyPtrType>);
//#endif
//..

#include <bslscm_version.h>

#include <bslmf_integralconstant.h>

#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_platform.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bslmf_removecv.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

namespace bsl {

                         // =================
                         // struct is_pointer
                         // =================

template <class t_TYPE>
struct is_pointer : bsl::false_type {
    // This 'struct' template implements the 'is_pointer' meta-function defined
    // in the C++11 standard [meta.unary.cat] to determine if the (template
    // parameter) 't_TYPE' is a pointer.  This 'struct' derives from
    // 'bsl::true_type' if the 't_TYPE' is a pointer type (but not a
    // pointer-to-non-static-member type), and 'bsl::false_type' otherwise.
};

template <class t_TYPE>
struct is_pointer<t_TYPE *> : bsl::true_type {
    // This partial specialization of 'is_pointer' derives from
    // 'bsl::true_type' for when the (template parameter) 't_TYPE' is a
    // (cv-unqalified) pointer type.
};

#if defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION < 1900
// Older Microsoft compilers do not recognize cv-qualifiers on function pointer
// types as matching a 't_TYPE *const' partial specialization, but can
// correctly strip the cv-qualifier if we take a second template instantiation
// on a more general 't_TYPE const' parameter.

template <class t_TYPE>
struct is_pointer<t_TYPE const> : is_pointer<t_TYPE>::type {
    // This partial specialization of 'is_pointer' derives from
    // 'bsl::true_type' for when the (template parameter) 't_TYPE' is a 'const'
    // qualified pointer type.
};

template <class t_TYPE>
struct is_pointer<t_TYPE volatile> : is_pointer<t_TYPE>::type {
    // This partial specialization of 'is_pointer' derives from
    // 'bsl::true_type' for when the (template parameter) 't_TYPE' is a
    // 'volatile' qualified pointer type.
};

template <class t_TYPE>
struct is_pointer<t_TYPE const volatile> : is_pointer<t_TYPE>::type {
    // This partial specialization of 'is_pointer' derives from
    // 'bsl::true_type' for when the (template parameter) 't_TYPE' is a 'const
    // volatile' qualified pointer type.
};
#else
// Preferred implementation avoids a second dispatch for arbitrary cv-qualified
// types.

template <class t_TYPE>
struct is_pointer<t_TYPE *const> : bsl::true_type {
    // This partial specialization of 'is_pointer' derives from
    // 'bsl::true_type' for when the (template parameter) 't_TYPE' is a 'const'
    // qualified pointer type.
};

template <class t_TYPE>
struct is_pointer<t_TYPE *volatile> : bsl::true_type {
    // This partial specialization of 'is_pointer' derives from
    // 'bsl::true_type' for when the (template parameter) 't_TYPE' is a
    // 'volatile' qualified pointer type.
};

template <class t_TYPE>
struct is_pointer<t_TYPE *const volatile> : bsl::true_type {
    // This partial specialization of 'is_pointer' derives from
    // 'bsl::true_type' for when the (template parameter) 't_TYPE' is a 'const
    // volatile' qualified pointer type.
};
#endif

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
template <class t_TYPE>
BSLS_KEYWORD_INLINE_VARIABLE constexpr bool is_pointer_v =
                                                     is_pointer<t_TYPE>::value;
    // This template variable represents the result value of the
    // 'bsl::is_pointer' meta-function.
#endif

}  // close namespace bsl

namespace BloombergLP {
namespace bslmf {

                         // ================
                         // struct IsPointer
                         // ================

template <class t_TYPE>
struct IsPointer : bsl::is_pointer<t_TYPE>::type {
    // This 'struct' template implements a meta-function to determine if the
    // (template parameter) 't_TYPE' is a pointer type.  This 'struct' derives
    // from 'bsl::true_type' if the 't_TYPE' is a pointer type (but not a
    // pointer to non-static member), and 'bsl::false_type' otherwise.
    //
    // Note that this 'struct' is functionally equivalent to 'bsl::is_pointer',
    // and the use of 'bsl::is_pointer' should be preferred.
};

}  // close package namespace
}  // close enterprise namespace

#ifndef BDE_OPENSOURCE_PUBLICATION  // BACKWARD_COMPATIBILITY
// ============================================================================
//                           BACKWARD-COMPATIBILITY
// ============================================================================

#ifdef bslmf_IsPointer
#undef bslmf_IsPointer
#endif

#define bslmf_IsPointer bslmf::IsPointer
    // This alias is defined for backward-compatibility.
#endif  // BDE_OPENSOURCE_PUBLICATION -- BACKWARD_COMPATIBILITY

#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
