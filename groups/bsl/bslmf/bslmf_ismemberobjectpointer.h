// bslmf_ismemberobjectpointer.h                                      -*-C++-*-
#ifndef INCLUDED_BSLMF_ISMEMBEROBJECTPOINTER
#define INCLUDED_BSLMF_ISMEMBEROBJECTPOINTER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for member object pointer types.
//
//@CLASSES:
//  bsl::is_member_object_pointer: standard meta-function
//
//@SEE_ALSO: bslmf_integralconstant, bslmf_ismemberfunctionpointer
//
//@DESCRIPTION: This component defines a meta-function,
// 'bsl::is_member_object_pointer', that may be used to query whether a type is
// a pointer to non-static member object type.
//
// 'bsl::is_member_object_pointer' meets the requirements of the
// 'is_member_object_pointer' template defined in the C++11 standard
// [meta.unary.cat].
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

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

#ifndef INCLUDED_BSLMF_ISMEMBERFUNCTIONPOINTER
#include <bslmf_ismemberfunctionpointer.h>
#endif

#ifndef INCLUDED_BSLMF_ISREFERENCE
#include <bslmf_isreference.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECV
#include <bslmf_removecv.h>
#endif

namespace BloombergLP {
namespace bslmf {

                       // ================================
                       // struct IsPointerToMemberData_Imp
                       // ================================

template <class TYPE>
struct IsPointerToMemberData_Imp : bsl::false_type {
    // This 'struct' template provides a meta-function to determine whether the
    // (template parameter) 'TYPE' is a pointer to non-static data member type.
    // This generic default template derives from 'bsl::false_type'.  A
    // template specialization is provided (below) that derives from
    // 'bsl::true_type'.
};

template <class TYPE, class CLASS>
struct IsPointerToMemberData_Imp<TYPE CLASS::*> : bsl::true_type {
     // This partial specialization of 'IsPointerToMemberData_Imp' derives from
     // 'bsl::true_type' for when the (template parameter) 'TYPE' is a pointer
     // to non-static data member type.
};

}  // close package namespace
}  // close enterprise namespace

namespace bsl {

                       // ===============================
                       // struct is_member_object_pointer
                       // ===============================

template <class TYPE>
struct is_member_object_pointer
    : integral_constant<bool,
                       BloombergLP::bslmf::IsPointerToMemberData_Imp<
                           typename remove_cv<TYPE>::type>::value
                       && !is_member_function_pointer<TYPE>::value
                       && !is_reference<TYPE>::value> {
    // This 'struct' template implements the 'is_member_object_pointer'
    // meta-function defined in the C++11 standard [meta.unary.cat] to
    // determine if the (template parameter) 'TYPE' is a pointer to non-static
    // data member type.  This 'struct' derives from 'bsl::true_type' if the
    // 'TYPE' is a pointer to non-static data member type, and from
    // 'bsl::false_type' otherwise.
};

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
