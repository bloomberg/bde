// bslmf_ismemberfunctionpointer.h                                    -*-C++-*-
#ifndef INCLUDED_BSLMF_ISMEMBERFUNCTIONPOINTER
#define INCLUDED_BSLMF_ISMEMBERFUNCTIONPOINTER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for member function pointer types.
//
//@CLASSES:
//  bsl::is_member_function_pointer: standard meta-function
//  bsl::is_member_function_pointer_v: the result value of the meta-function
//
//@SEE_ALSO: bslmf_memberfunctionpointertraits, bslmf_ismemberobjectpointer
//
//@DESCRIPTION: This component defines a meta-function,
// 'bsl::is_member_function_pointer' and a template variable
// 'bsl::is_member_function_pointer_v', that represents the result value of the
// 'bsl::is_member_function_pointer' meta-function, that may be used to query
// whether a type is a pointer to non-static member function type.
//
// 'bsl::is_member_function_pointer' meets the requirements of the
// 'is_member_function_pointer' template defined in the C++11 standard
// [meta.unary.cat].
//
// Note that, in order to support the widest range of compilers, this
// component depends on bslmf_memberfunctionpointertraits, which does not
// support member functions with C-style (varargs) elipses.  Thus,
// 'bsl::is_member_function_pointer' will return a false negative if invoked
// on a pointer to such a member.
//
// Note that the template variable 'is_member_function_pointer_v' is defined in
// the C++17 standard as an inline variable.  If the current compiler supports
// the inline variable C++17 compiler feature,
// 'bsl::is_member_function_pointer_v' is defined as an 'inline constexpr bool'
// variable.  Otherwise, if the compiler supports the variable templates C++14
// compiler feature, 'bsl::is_member_function_pointer_v' is defined as a
// non-inline 'constexpr bool' variable.  See
// 'BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES' and
// 'BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES' macros in
// bsls_compilerfeatures component for details.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify Member Function Pointer Types
///- - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a set of types are pointers to
// non-static member function types.
//
// First, we create a user-defined type 'MyStruct':
//..
//  struct MyStruct
//  {
//  };
//..
// Now, we create two 'typedef's -- a function pointer type and a member
// function pointer type:
//..
//  typedef int (MyStruct::*MyStructMethodPtr) ();
//  typedef int (*MyFunctionPtr) ();
//..
// Finally, we instantiate the 'bsl::is_member_function_pointer' template for
// each of the 'typedef's and assert the 'value' static data member of each
// instantiation:
//..
//  assert(false == bsl::is_member_function_pointer<MyFunctionPtr    >::value);
//  assert(true  == bsl::is_member_function_pointer<MyStructMethodPtr>::value);
//..
// Note that if the current compiler supports the variable templates C++14
// feature then we can re-write the snippet of code above using the
// 'bsl::is_member_function_pointer_v' variable as follows:
//..
//#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
//  assert(false == bsl::is_member_function_pointer_v<MyFunctionPtr    >);
//  assert(true  == bsl::is_member_function_pointer_v<MyStructMethodPtr>);
//#endif
//..

#include <bslscm_version.h>

#include <bslmf_integralconstant.h>
#include <bslmf_isfunction.h>

#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_platform.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bslmf_memberfunctionpointertraits.h>
#endif


#if !defined(BSLS_PLATFORM_CMP_MSVC)   \
 ||  BSLS_PLATFORM_CMP_VERSION >  1900 \
 ||((BSLS_PLATFORM_CMP_VERSION == 1900) && defined(BSLS_PLATFORM_CPU_64_BIT))

namespace bsl {
                       // =================================
                       // struct is_member_function_pointer
                       // =================================

template <class t_TYPE>
struct is_member_function_pointer : false_type {
    // This 'struct' template implements the 'is_member_function_pointer'
    // meta-function defined in the C++11 standard [meta.unary.cat] to
    // determine if the (template parameter) 't_TYPE' is a pointer to
    // non-static member function type.  This 'struct' derives from
    // 'bsl::true_type' if the 't_TYPE' is a pointer to non-static member
    // function type, and from 'bsl::false_type' otherwise.  Additional
    // specializations are provided below to give the correct answer in all
    // cases.
};

template <class t_TYPE, class t_CLASS>
struct is_member_function_pointer<t_TYPE t_CLASS::*>
: bsl::is_function<t_TYPE>::type {
    // This partial specialization for all non-cv-qualified pointer-to-member
    // types derives from 'true_type' if the specified (template parameter)
    // 'type' is a function type, and from 'false_type' otherwise.
};

template <class t_TYPE, class t_CLASS>
struct is_member_function_pointer<t_TYPE t_CLASS::*const>
: bsl::is_function<t_TYPE>::type {
    // This partial specialization for all 'const'-qualified pointer-to-member
    // types derives from 'true_type' if the specified (template parameter)
    // 'type' is a function type, and from 'false_type' otherwise.
};

template <class t_TYPE, class t_CLASS>
struct is_member_function_pointer<t_TYPE t_CLASS::*volatile>
: bsl::is_function<t_TYPE>::type {
    // This partial specialization for all 'volatile'-qualified
    // pointer-to-member types derives from 'true_type' if the specified
    // (template parameter) 'type' is a function type, and from 'false_type'
    // otherwise.
};

template <class t_TYPE, class t_CLASS>
struct is_member_function_pointer<t_TYPE t_CLASS::*const volatile>
: bsl::is_function<t_TYPE>::type {
    // This partial specialization for all 'const volatile'-qualified
    // pointer-to-member types derives from 'true_type'.
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
template <class t_TYPE>
BSLS_KEYWORD_INLINE_VARIABLE constexpr bool is_member_function_pointer_v =
                                     is_member_function_pointer<t_TYPE>::value;
    // This variable template represents the result value of the
    // 'bsl::is_member_function_pointer' meta-function.
#endif

}  // close namespace bsl

#else  // MSVC 2013 workaround

namespace bsl {

                       // =================================
                       // struct is_member_function_pointer
                       // =================================

template <class t_TYPE>
struct is_member_function_pointer : false_type {
    // This 'struct' template implements the 'is_member_function_pointer'
    // meta-function defined in the C++11 standard [meta.unary.cat] to
    // determine if the (template parameter) 't_TYPE' is a pointer to
    // non-static member function type.  This 'struct' derives from
    // 'bsl::true_type' if the 't_TYPE' is a pointer to non-static member
    // function type, and from 'bsl::false_type' otherwise.  Additional
    // specializations are provided below to give the correct answer in all
    // cases.
};

template <class t_TYPE>
struct is_member_function_pointer<const t_TYPE>
: is_member_function_pointer<t_TYPE>::type {
};

template <class t_TYPE>
struct is_member_function_pointer<volatile t_TYPE>
: is_member_function_pointer<t_TYPE>::type {
};

template <class t_TYPE>
struct is_member_function_pointer<const volatile t_TYPE>
: is_member_function_pointer<t_TYPE>::type {
};

template <class RESULT, class HOST, class... ARGS>
struct is_member_function_pointer<RESULT (HOST::*)(ARGS...)>
    : true_type {
    // This partial specialization for non-cv-qualified pointer-to-member types
    // derives from 'true_type' if the specified (template parameter) 'type' is
    // a function type.
};

template <class RESULT, class HOST, class... ARGS>
struct is_member_function_pointer<RESULT (HOST::*)(ARGS...) const>
    : true_type {
    // This partial specialization for non-cv-qualified pointer-to-member types
    // derives from 'true_type' if the specified (template parameter) 'type' is
    // a function type.
};

template <class RESULT, class HOST, class... ARGS>
struct is_member_function_pointer<RESULT (HOST::*)(ARGS...) volatile>
    : true_type {
    // This partial specialization for non-cv-qualified pointer-to-member types
    // derives from 'true_type' if the specified (template parameter) 'type' is
    // a function type.
};

template <class RESULT, class HOST, class... ARGS>
struct is_member_function_pointer<RESULT (HOST::*)(ARGS...) const volatile>
    : true_type {
    // This partial specialization for non-cv-qualified pointer-to-member types
    // derives from 'true_type' if the specified (template parameter) 'type' is
    // a function type.
};

template <class RESULT, class HOST, class... ARGS>
struct is_member_function_pointer<RESULT (HOST::*)(ARGS...,...)>
    : true_type {
    // This partial specialization for non-cv-qualified pointer-to-member types
    // derives from 'true_type' if the specified (template parameter) 'type' is
    // a function type.
};

template <class RESULT, class HOST, class... ARGS>
struct is_member_function_pointer<RESULT (HOST::*)(ARGS...,...) const>
    : true_type {
    // This partial specialization for non-cv-qualified pointer-to-member types
    // derives from 'true_type' if the specified (template parameter) 'type' is
    // a function type.
};

template <class RESULT, class HOST, class... ARGS>
struct is_member_function_pointer<RESULT (HOST::*)(ARGS...,...) volatile>
    : true_type {
    // This partial specialization for non-cv-qualified pointer-to-member types
    // derives from 'true_type' if the specified (template parameter) 'type' is
    // a function type.
};

template <class RESULT, class HOST, class... ARGS>
struct is_member_function_pointer<RESULT (HOST::*)(ARGS...,...) const volatile>
    : true_type {
    // This partial specialization for non-cv-qualified pointer-to-member types
    // derives from 'true_type' if the specified (template parameter) 'type' is
    // a function type.
};

#if defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
// Only MSVC 2015 32-bit builds get here
template <class RESULT, class HOST, class... ARGS>
struct is_member_function_pointer<RESULT (HOST::*)(ARGS...) &>
    : true_type {
    // This partial specialization for non-cv-qualified pointer-to-member types
    // derives from 'true_type' if the specified (template parameter) 'type' is
    // a function type.
};

template <class RESULT, class HOST, class... ARGS>
struct is_member_function_pointer<RESULT (HOST::*)(ARGS...) const &>
    : true_type {
    // This partial specialization for non-cv-qualified pointer-to-member types
    // derives from 'true_type' if the specified (template parameter) 'type' is
    // a function type.
};

template <class RESULT, class HOST, class... ARGS>
struct is_member_function_pointer<RESULT (HOST::*)(ARGS...) volatile &>
    : true_type {
    // This partial specialization for non-cv-qualified pointer-to-member types
    // derives from 'true_type' if the specified (template parameter) 'type' is
    // a function type.
};

template <class RESULT, class HOST, class... ARGS>
struct is_member_function_pointer<RESULT (HOST::*)(ARGS...) const volatile &>
    : true_type {
    // This partial specialization for non-cv-qualified pointer-to-member types
    // derives from 'true_type' if the specified (template parameter) 'type' is
    // a function type.
};

template <class RESULT, class HOST, class... ARGS>
struct is_member_function_pointer<RESULT (HOST::*)(ARGS...,...) &>
    : true_type {
    // This partial specialization for non-cv-qualified pointer-to-member types
    // derives from 'true_type' if the specified (template parameter) 'type' is
    // a function type.
};

template <class RESULT, class HOST, class... ARGS>
struct is_member_function_pointer<RESULT (HOST::*)(ARGS...,...) const &>
    : true_type {
    // This partial specialization for non-cv-qualified pointer-to-member types
    // derives from 'true_type' if the specified (template parameter) 'type' is
    // a function type.
};

template <class RESULT, class HOST, class... ARGS>
struct is_member_function_pointer<RESULT (HOST::*)(ARGS...,...) volatile &>
    : true_type {
    // This partial specialization for non-cv-qualified pointer-to-member types
    // derives from 'true_type' if the specified (template parameter) 'type' is
    // a function type.
};

template <class RESULT, class HOST, class... ARGS>
struct is_member_function_pointer<
                                RESULT (HOST::*)(ARGS...,...) const volatile &>
    : true_type {
    // This partial specialization for non-cv-qualified pointer-to-member types
    // derives from 'true_type' if the specified (template parameter) 'type' is
    // a function type.
};

template <class RESULT, class HOST, class... ARGS>
struct is_member_function_pointer<RESULT (HOST::*)(ARGS...) &&>
    : true_type {
    // This partial specialization for non-cv-qualified pointer-to-member types
    // derives from 'true_type' if the specified (template parameter) 'type' is
    // a function type.
};

template <class RESULT, class HOST, class... ARGS>
struct is_member_function_pointer<RESULT (HOST::*)(ARGS...) const &&>
    : true_type {
    // This partial specialization for non-cv-qualified pointer-to-member types
    // derives from 'true_type' if the specified (template parameter) 'type' is
    // a function type.
};

template <class RESULT, class HOST, class... ARGS>
struct is_member_function_pointer<RESULT (HOST::*)(ARGS...) volatile &&>
    : true_type {
    // This partial specialization for non-cv-qualified pointer-to-member types
    // derives from 'true_type' if the specified (template parameter) 'type' is
    // a function type.
};

template <class RESULT, class HOST, class... ARGS>
struct is_member_function_pointer<RESULT (HOST::*)(ARGS...) const volatile &&>
    : true_type {
    // This partial specialization for non-cv-qualified pointer-to-member types
    // derives from 'true_type' if the specified (template parameter) 'type' is
    // a function type.
};

template <class RESULT, class HOST, class... ARGS>
struct is_member_function_pointer<RESULT (HOST::*)(ARGS...,...) &&>
    : true_type {
    // This partial specialization for non-cv-qualified pointer-to-member types
    // derives from 'true_type' if the specified (template parameter) 'type' is
    // a function type.
};

template <class RESULT, class HOST, class... ARGS>
struct is_member_function_pointer<RESULT (HOST::*)(ARGS...,...) const &&>
    : true_type {
    // This partial specialization for non-cv-qualified pointer-to-member types
    // derives from 'true_type' if the specified (template parameter) 'type' is
    // a function type.
};

template <class RESULT, class HOST, class... ARGS>
struct is_member_function_pointer<RESULT (HOST::*)(ARGS...,...) volatile &&>
    : true_type {
    // This partial specialization for non-cv-qualified pointer-to-member types
    // derives from 'true_type' if the specified (template parameter) 'type' is
    // a function type.
};

template <class RESULT, class HOST, class... ARGS>
struct is_member_function_pointer<
                               RESULT (HOST::*)(ARGS...,...) const volatile &&>
    : true_type {
    // This partial specialization for non-cv-qualified pointer-to-member types
    // derives from 'true_type' if the specified (template parameter) 'type' is
    // a function type.
};
#endif

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
template <class t_TYPE>
BSLS_KEYWORD_INLINE_VARIABLE constexpr bool is_member_function_pointer_v =
                                     is_member_function_pointer<t_TYPE>::value;
    // This variable template represents the result value of the
    // 'bsl::is_member_function_pointer' meta-function.
#endif

}  // close namespace bsl

#endif  // MSVC 2013 workaround

#endif

// ----------------------------------------------------------------------------
// Copyright 2013-2018 Bloomberg Finance L.P.
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
