// bslmf_ismemberpointer.h                                            -*-C++-*-
#ifndef INCLUDED_BSLMF_ISMEMBERPOINTER
#define INCLUDED_BSLMF_ISMEMBERPOINTER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for non-static member pointer types.
//
//@CLASSES:
//  bsl::is_member_pointer: standard meta-function for member pointer types
//  bsl::is_member_pointer_v: the result value of the standard meta-function
//
//@SEE_ALSO: bslmf_ismemberfunctionpointer, bslmf_ismemberobjectpointer
//
//@DESCRIPTION: This component defines a meta-function,
// 'bsl::is_member_pointer' and a template variable 'bsl::is_member_pointer_v',
// that represents the result value of the 'bsl::is_member_pointer'
// meta-function, that may be used to query whether a type is a pointer to
// non-static member type.
//
// 'bsl::is_member_pointer' meets the requirements of the 'is_member_pointer'
// template defined in the C++11 standard [meta.unary.comp].
//
// Note that the template variable 'is_member_pointer_v' is defined in the
// C++17 standard as an inline variable.  If the current compiler supports the
// inline variable C++17 compiler feature, 'bsl::is_member_pointer_v' is
// defined as an 'inline constexpr bool' variable.  Otherwise, if the compiler
// supports the variable templates C++14 compiler feature,
// 'bsl::is_member_pointer_v' is defined as a non-inline 'constexpr bool'
// variable.  See 'BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES' and
// 'BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES' macros in
// bsls_compilerfeatures component for details.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify Member Pointer Types
/// - - - - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a set of types are member pointer
// types.
//
// First, we create a user-defined type 'MyStruct':
//..
//  struct MyStruct
//  {
//  };
//..
// Now, we create three 'typedef's -- a member object pointer type, a member
// function pointer type and a general function pointer type:
//..
//  typedef int MyStruct::* DataMemPtr;
//  typedef int (MyStruct::*MyStructMethodPtr) ();
//  typedef int (*MyFunctionPtr) ();
//..
// Finally, we instantiate the 'bsl::is_member_pointer' template for various
// types and assert the 'value' static data member of each instantiation:
//..
//  assert(false == bsl::is_member_pointer<int*>::value);
//  assert(false == bsl::is_member_pointer<MyFunctionPtr>::value);
//  assert(true  == bsl::is_member_pointer<DataMemPtr>::value);
//  assert(true  == bsl::is_member_pointer<MyStructMethodPtr>::value);
//..
// Note that if the current compiler supports the variable templates C++14
// feature then we can re-write the snippet of code above using the
// 'bsl::is_member_pointer_v' variable as follows:
//..
//#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
//  assert(false == bsl::is_member_pointer_v<int*>);
//  assert(false == bsl::is_member_pointer_v<MyFunctionPtr>);
//  assert(true  == bsl::is_member_pointer_v<DataMemPtr>);
//  assert(true  == bsl::is_member_pointer_v<MyStructMethodPtr>);
//#endif
//..

#include <bslscm_version.h>

#include <bslmf_integralconstant.h>

#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_platform.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bslmf_ismemberfunctionpointer.h>
#include <bslmf_ismemberobjectpointer.h>
#endif

namespace bsl {

                         // ========================
                         // struct is_member_pointer
                         // ========================

template <class t_TYPE>
struct is_member_pointer : false_type {
    // This 'struct' template implements the 'is_member_pointer' meta-function
    // defined in the C++11 standard [meta.unary.comp] to determine if the
    // (template parameter) 't_TYPE' is a member pointer type.  This 'struct'
    // derives from 'bsl::true_type' if the 't_TYPE' is a member pointer type,
    // and from 'bsl::false_type' otherwise.  Additional specializations are
    // provided below to give the correct answer in all cases.
};

template <class t_TARGET_TYPE, class t_HOST_TYPE>
struct is_member_pointer<t_TARGET_TYPE t_HOST_TYPE::*> : true_type {
    // This partial specialization provides the 'true_type' result for a
    // (cv-unqualified) pointer-to-member type.  Note that additional partial
    // specializations are required to handle the cv-qualified cases.
};

template <class t_TARGET_TYPE, class t_HOST_TYPE>
struct is_member_pointer<t_TARGET_TYPE t_HOST_TYPE::*const> : true_type {
    // This partial specialization provides the 'true_type' result for a
    // 'const'-qualified pointer-to-member type.
};

template <class t_TARGET_TYPE, class t_HOST_TYPE>
struct is_member_pointer<t_TARGET_TYPE t_HOST_TYPE::*volatile> : true_type {
    // This partial specialization provides the 'true_type' result for a
    // 'volatile'-qualified pointer-to-member type.
};

template <class t_TARGET_TYPE, class t_HOST_TYPE>
struct is_member_pointer<t_TARGET_TYPE t_HOST_TYPE::*const volatile>
: true_type {
    // This partial specialization provides the 'true_type' result for a
    // 'const volatile'-qualified pointer-to-member type.
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
template <class t_TYPE>
BSLS_KEYWORD_INLINE_VARIABLE constexpr bool is_member_pointer_v =
                                              is_member_pointer<t_TYPE>::value;
    // This template variable represents the result value of the
    // 'bsl::is_member_pointer' meta-function.
#endif

#if defined(BSLS_PLATFORM_CMP_MSVC)    \
 &&   BSLS_PLATFORM_CMP_VERSION < 1910 \
 &&!((BSLS_PLATFORM_CMP_VERSION == 1900) && defined(BSLS_PLATFORM_CPU_64_BIT))
// MSVC 2013 (and earlier) has two bugs that affect this component.  First,
// it does not match partial specializations for cv-qualified pointer-to-member
// types, requiring a general stripping of cv-qualifiers for all types to get
// the right result.  Secondly, pointer-to-cv-qualified-member-function fails
// to match the partial specialization at all, so fall back on trying to match
// a member function with exactly the right signature using variadic templates,
// which apparently does work.  Note that this bug also appears to affect the
// MSVC 2015 compiler, but only for 32-bit builds.

template <class t_TYPE>
struct is_member_pointer<const t_TYPE> : is_member_pointer<t_TYPE>::type {
    // The 'const'-qualified (template parameter) 't_TYPE' is a member pointer
    // if the corresponding unqualified 't_TYPE' is a member pointer.
};

template <class t_TYPE>
struct is_member_pointer<volatile t_TYPE> : is_member_pointer<t_TYPE>::type {
    // The 'volatile'-qualified (template parameter) 't_TYPE' is a member
    // pointer if the corresponding unqualified 't_TYPE' is a member pointer.
};

template <class t_TYPE>
struct is_member_pointer<const volatile t_TYPE>
: is_member_pointer<t_TYPE>::type {
    // The 'const volatile'-qualified (template parameter) 't_TYPE' is a member
    // pointer if the corresponding unqualified 't_TYPE' is a member pointer.
};

template <class RESULT, class HOST, class... ARGS>
struct is_member_pointer<RESULT (HOST::*)(ARGS...)> : true_type {
    // This partial specialization for non-cv-qualified pointer-to-member types
    // derives from 'true_type' if the specified (template parameter) 'type' is
    // a function type.
};

template <class RESULT, class HOST, class... ARGS>
struct is_member_pointer<RESULT (HOST::*)(ARGS...) const> : true_type {
    // This partial specialization for non-cv-qualified pointer-to-member types
    // derives from 'true_type' if the specified (template parameter) 'type' is
    // a function type.
};

template <class RESULT, class HOST, class... ARGS>
struct is_member_pointer<RESULT (HOST::*)(ARGS...) volatile> : true_type {
    // This partial specialization for non-cv-qualified pointer-to-member types
    // derives from 'true_type' if the specified (template parameter) 'type' is
    // a function type.
};

template <class RESULT, class HOST, class... ARGS>
struct is_member_pointer<RESULT (HOST::*)(ARGS...) const volatile>
    : true_type {
    // This partial specialization for non-cv-qualified pointer-to-member types
    // derives from 'true_type' if the specified (template parameter) 'type' is
    // a function type.
};

template <class RESULT, class HOST, class... ARGS>
struct is_member_pointer<RESULT (HOST::*)(ARGS...,...)> : true_type {
    // This partial specialization for non-cv-qualified pointer-to-member types
    // derives from 'true_type' if the specified (template parameter) 'type' is
    // a function type.
};

template <class RESULT, class HOST, class... ARGS>
struct is_member_pointer<RESULT (HOST::*)(ARGS...,...) const> : true_type {
    // This partial specialization for non-cv-qualified pointer-to-member types
    // derives from 'true_type' if the specified (template parameter) 'type' is
    // a function type.
};

template <class RESULT, class HOST, class... ARGS>
struct is_member_pointer<RESULT (HOST::*)(ARGS...,...) volatile> : true_type {
    // This partial specialization for non-cv-qualified pointer-to-member types
    // derives from 'true_type' if the specified (template parameter) 'type' is
    // a function type.
};

template <class RESULT, class HOST, class... ARGS>
struct is_member_pointer<RESULT (HOST::*)(ARGS...,...) const volatile>
    : true_type {
    // This partial specialization for non-cv-qualified pointer-to-member types
    // derives from 'true_type' if the specified (template parameter) 'type' is
    // a function type.
};

#if defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
// Only MSVC 2015 32-bit builds get here
template <class RESULT, class HOST, class... ARGS>
struct is_member_pointer<RESULT (HOST::*)(ARGS...) &>
    : true_type {
    // This partial specialization for non-cv-qualified pointer-to-member types
    // derives from 'true_type' if the specified (template parameter) 'type' is
    // a function type.
};

template <class RESULT, class HOST, class... ARGS>
struct is_member_pointer<RESULT (HOST::*)(ARGS...) const &>
    : true_type {
    // This partial specialization for non-cv-qualified pointer-to-member types
    // derives from 'true_type' if the specified (template parameter) 'type' is
    // a function type.
};

template <class RESULT, class HOST, class... ARGS>
struct is_member_pointer<RESULT (HOST::*)(ARGS...) volatile &>
    : true_type {
    // This partial specialization for non-cv-qualified pointer-to-member types
    // derives from 'true_type' if the specified (template parameter) 'type' is
    // a function type.
};

template <class RESULT, class HOST, class... ARGS>
struct is_member_pointer<RESULT (HOST::*)(ARGS...) const volatile &>
    : true_type {
    // This partial specialization for non-cv-qualified pointer-to-member types
    // derives from 'true_type' if the specified (template parameter) 'type' is
    // a function type.
};

template <class RESULT, class HOST, class... ARGS>
struct is_member_pointer<RESULT (HOST::*)(ARGS...,...) &>
    : true_type {
    // This partial specialization for non-cv-qualified pointer-to-member types
    // derives from 'true_type' if the specified (template parameter) 'type' is
    // a function type.
};

template <class RESULT, class HOST, class... ARGS>
struct is_member_pointer<RESULT (HOST::*)(ARGS...,...) const &>
    : true_type {
    // This partial specialization for non-cv-qualified pointer-to-member types
    // derives from 'true_type' if the specified (template parameter) 'type' is
    // a function type.
};

template <class RESULT, class HOST, class... ARGS>
struct is_member_pointer<RESULT (HOST::*)(ARGS...,...) volatile &>
    : true_type {
    // This partial specialization for non-cv-qualified pointer-to-member types
    // derives from 'true_type' if the specified (template parameter) 'type' is
    // a function type.
};

template <class RESULT, class HOST, class... ARGS>
struct is_member_pointer<RESULT (HOST::*)(ARGS...,...) const volatile &>
    : true_type {
    // This partial specialization for non-cv-qualified pointer-to-member types
    // derives from 'true_type' if the specified (template parameter) 'type' is
    // a function type.
};

template <class RESULT, class HOST, class... ARGS>
struct is_member_pointer<RESULT (HOST::*)(ARGS...) &&>
    : true_type {
    // This partial specialization for non-cv-qualified pointer-to-member types
    // derives from 'true_type' if the specified (template parameter) 'type' is
    // a function type.
};

template <class RESULT, class HOST, class... ARGS>
struct is_member_pointer<RESULT (HOST::*)(ARGS...) const &&>
    : true_type {
    // This partial specialization for non-cv-qualified pointer-to-member types
    // derives from 'true_type' if the specified (template parameter) 'type' is
    // a function type.
};

template <class RESULT, class HOST, class... ARGS>
struct is_member_pointer<RESULT (HOST::*)(ARGS...) volatile &&>
    : true_type {
    // This partial specialization for non-cv-qualified pointer-to-member types
    // derives from 'true_type' if the specified (template parameter) 'type' is
    // a function type.
};

template <class RESULT, class HOST, class... ARGS>
struct is_member_pointer<RESULT (HOST::*)(ARGS...) const volatile &&>
    : true_type {
    // This partial specialization for non-cv-qualified pointer-to-member types
    // derives from 'true_type' if the specified (template parameter) 'type' is
    // a function type.
};

template <class RESULT, class HOST, class... ARGS>
struct is_member_pointer<RESULT (HOST::*)(ARGS...,...) &&>
    : true_type {
    // This partial specialization for non-cv-qualified pointer-to-member types
    // derives from 'true_type' if the specified (template parameter) 'type' is
    // a function type.
};

template <class RESULT, class HOST, class... ARGS>
struct is_member_pointer<RESULT (HOST::*)(ARGS...,...) const &&>
    : true_type {
    // This partial specialization for non-cv-qualified pointer-to-member types
    // derives from 'true_type' if the specified (template parameter) 'type' is
    // a function type.
};

template <class RESULT, class HOST, class... ARGS>
struct is_member_pointer<RESULT (HOST::*)(ARGS...,...) volatile &&>
    : true_type {
    // This partial specialization for non-cv-qualified pointer-to-member types
    // derives from 'true_type' if the specified (template parameter) 'type' is
    // a function type.
};

template <class RESULT, class HOST, class... ARGS>
struct is_member_pointer<RESULT (HOST::*)(ARGS...,...) const volatile &&>
    : true_type {
    // This partial specialization for non-cv-qualified pointer-to-member types
    // derives from 'true_type' if the specified (template parameter) 'type' is
    // a function type.
};
#endif  // Reference-qualifier support for MSVC 2015

#endif  // MSVC workarounds

}  // close namespace bsl

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
