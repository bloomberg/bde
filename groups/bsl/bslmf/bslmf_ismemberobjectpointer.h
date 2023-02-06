// bslmf_ismemberobjectpointer.h                                      -*-C++-*-
#ifndef INCLUDED_BSLMF_ISMEMBEROBJECTPOINTER
#define INCLUDED_BSLMF_ISMEMBEROBJECTPOINTER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for member object pointer types.
//
//@CLASSES:
//  bsl::is_member_object_pointer: standard meta-function
//  bsl::is_member_object_pointer_v: the result value of the meta-function
//
//@SEE_ALSO: bslmf_integralconstant, bslmf_ismemberfunctionpointer
//
//@DESCRIPTION: This component defines a meta-function,
// 'bsl::is_member_object_pointer' and a template variable
// 'bsl::is_member_object_pointer_v', that represents the result value of the
// 'bsl::is_member_object_pointer' meta-function, that may be used to query
// whether a type is a pointer to non-static member object type.
//
// 'bsl::is_member_object_pointer' meets the requirements of the
// 'is_member_object_pointer' template defined in the C++11 standard
// [meta.unary.cat].
//
// Note that the template variable 'is_member_object_pointer_v' is defined in
// the C++17 standard as an inline variable.  If the current compiler supports
// the inline variable C++17 compiler feature,
// 'bsl::is_member_object_pointer_v' is defined as an 'inline constexpr bool'
// variable.  Otherwise, if the compiler supports the variable templates C++14
// compiler feature, 'bsl::is_member_object_pointer_v' is defined as a
// non-inline 'constexpr bool' variable.  See
// 'BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES' and
// 'BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES' macros in
// bsls_compilerfeatures component for details.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify Member Object Pointer Types
///- - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a set of types are pointers to member
// object types.
//
// First, we create a user-defined type 'MyStruct':
//..
//  struct MyStruct
//  {
//  };
//..
// Now, we create a 'typedef' for a member object pointer type:
//..
//  typedef int MyStruct::* DataMemPtr;
//..
// Finally, we instantiate the 'bsl::is_member_object_pointer' template for a
// non-member data type and the 'MyStructDataPtr' type, and assert the 'value'
// static data member of each instantiation:
//..
//  assert(false == bsl::is_member_object_pointer<int*>::value);
//  assert(true  == bsl::is_member_object_pointer<DataMemPtr>::value);
//..
// Note that if the current compiler supports the variable templates C++14
// feature then we can re-write the snippet of code above using the
// 'bsl::is_member_object_pointer_v' variable as follows:
//..
//#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
//  assert(false == bsl::is_member_object_pointer_v<int*>);
//  assert(true  == bsl::is_member_object_pointer_v<DataMemPtr>);
//#endif
//..

#include <bslscm_version.h>

#include <bslmf_integralconstant.h>
#include <bslmf_isconst.h>
#include <bslmf_isfunction.h>

#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_platform.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bslmf_ismemberfunctionpointer.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#if !defined(BSLS_PLATFORM_CMP_IBM)
namespace bsl {

                       // ===============================
                       // struct is_member_object_pointer
                       // ===============================

template <class t_TYPE>
struct is_member_object_pointer : false_type {
    // This 'struct' template implements the 'is_member_object_pointer'
    // meta-function defined in the C++11 standard [meta.unary.cat] to
    // determine if the (template parameter) 't_TYPE' is a pointer to
    // non-static data member type.  This 'struct' derives from
    // 'bsl::true_type' if the 't_TYPE' is a pointer to non-static data member
    // type, and from 'bsl::false_type' otherwise.
};

#ifdef BSLS_PLATFORM_CMP_MSVC
# pragma warning(push)
# pragma warning(disable: 4180)  // cv-qualifier has no effect on function type
#endif

template <class t_TYPE, class t_CLASS>
struct is_member_object_pointer<t_TYPE t_CLASS::*>
: is_const<const t_TYPE>::type {
    // Partial specialization relies on the principle that only object types
    // can be 'const'-qualified.  Reference-types and function-types do not
    // retain the 'const' qualifier when added in this manner, and there are
    // no 'void' class members.
};

#ifdef BSLS_PLATFORM_CMP_MSVC
# pragma warning(pop)
#endif

template <class t_TYPE, class t_CLASS>
struct is_member_object_pointer<t_TYPE t_CLASS::*const>
: is_const<const t_TYPE>::type {
    // Partial specialization relies on the principle that only object types
    // can be 'const'-qualified.  Reference-types and function-types do not
    // retain the 'const' qualifier when added in this manner, and there are
    // no 'void' class members.
};

template <class t_TYPE, class t_CLASS>
struct is_member_object_pointer<t_TYPE t_CLASS::*volatile>
: is_const<const t_TYPE>::type {
    // Partial specialization relies on the principle that only object types
    // can be 'const'-qualified.  Reference-types and function-types do not
    // retain the 'const' qualifier when added in this manner, and there are
    // no 'void' class members.
};

template <class t_TYPE, class t_CLASS>
struct is_member_object_pointer<t_TYPE t_CLASS::*const volatile>
: is_const<const t_TYPE>::type {
    // Partial specialization relies on the principle that only object types
    // can be 'const'-qualified.  Reference-types and function-types do not
    // retain the 'const' qualifier when added in this manner, and there are
    // no 'void' class members.
};

}  // close namespace bsl
#else
// The IBM xlC compiler produces parses an error when trying to add 'const' to
// a function type, so leans on the original BDE implementation of this trait.
// Note that this implementation fails on all other compilers for member
// functions with a C-style elipsis, erroneously reporting such functions as
// data members.  However, xlC appears to have sufficient compenstating bugs
// that this implementation gives the correct result in such cases too.

namespace BloombergLP {
namespace bslmf {

                       // ================================
                       // struct IsPointerToMemberData_Imp
                       // ================================

template <class t_TYPE>
struct IsPointerToMemberData_Imp : bsl::false_type {
    // This 'struct' template provides a meta-function to determine whether the
    // (template parameter) 't_TYPE' is a pointer to non-static data member
    // type.  This generic default template derives from 'bsl::false_type'.  A
    // template specialization is provided (below) that derives from
    // 'bsl::true_type'.
};

template <class t_TYPE, class t_CLASS>
struct IsPointerToMemberData_Imp<t_TYPE t_CLASS::*>
: bsl::integral_constant<bool, !bsl::is_function<t_TYPE>::value> {
    // This partial specialization of 'IsPointerToMemberData_Imp' derives from
    // 'bsl::true_type' for when the (template parameter) 't_TYPE' is a pointer
    // to non-static data member type.
};

}  // close package namespace
}  // close enterprise namespace

namespace bsl {

                       // ===============================
                       // struct is_member_object_pointer
                       // ===============================

template <class t_TYPE>
struct is_member_object_pointer
: BloombergLP::bslmf::IsPointerToMemberData_Imp<t_TYPE>::type {
    // This 'struct' template implements the 'is_member_object_pointer'
    // meta-function defined in the C++11 standard [meta.unary.cat] to
    // determine if the (template parameter) 't_TYPE' is a pointer to
    // non-static data member type.  This 'struct' derives from
    // 'bsl::true_type' if the 't_TYPE' is a pointer to non-static data member
    // type, and from 'bsl::false_type' otherwise.
};

template <class t_TYPE>
struct is_member_object_pointer<const t_TYPE>
: BloombergLP::bslmf::IsPointerToMemberData_Imp<t_TYPE>::type {
    // Partial specialization to handle 'const'-qualified pointer-to-member
    // objects.
};

template <class t_TYPE>
struct is_member_object_pointer<volatile t_TYPE>
: BloombergLP::bslmf::IsPointerToMemberData_Imp<t_TYPE>::type {
    // Partial specialization to handle 'volatile'-qualified pointer-to-member
    // objects.
};

template <class t_TYPE>
struct is_member_object_pointer<const volatile t_TYPE>
: BloombergLP::bslmf::IsPointerToMemberData_Imp<t_TYPE>::type {
    // Partial specialization to handle 'const volatile'-qualified
    // pointer-to-member objects.
};

}  // close namespace bsl
#endif

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
namespace bsl {

template <class t_TYPE>
BSLS_KEYWORD_INLINE_VARIABLE constexpr bool is_member_object_pointer_v =
                                       is_member_object_pointer<t_TYPE>::value;
    // This template variable represents the result value of the
    // 'bsl::is_member_object_pointer' meta-function.
}  // close namespace bsl
#endif

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
