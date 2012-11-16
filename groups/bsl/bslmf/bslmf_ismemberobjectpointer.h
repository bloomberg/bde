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
//@DESCRIPTION: This component defines a meta-function,
// 'bsl::is_member_object_pointer', which may be used to query whether a type
// is a pointer to non-static member object.
//
// 'bsl::is_member_object_pointer' meets the requirements of the
// 'is_member_object_pointer' template defined in the C++11 standard
// [meta.unary.cat].
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify Member Function Pointer Types
///- - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a particular type is a class type.
//
// First, we create a user-defined type 'MyStruct':
//..
//  struct MyStruct
//  {
//      // DATA
//      int memData;  // a member data
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
//  assert(false == bsl::is_member_object_pointer<int>::value);
//  assert(true  == bsl::is_member_object_pointer<DataMemPtr>::value);
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECV
#include <bslmf_removecv.h>
#endif

#ifndef INCLUDED_BSLMF_ISREFERENCE
#include <bslmf_isreference.h>
#endif

#ifndef INCLUDED_BSLMF_ISMEMBERFUNCTIONPOINTER
#include <bslmf_ismemberfunctionpointer.h>
#endif

namespace BloombergLP {
namespace bslmf {

                       // ================================
                       // struct IsPointerToMemberData_Imp
                       // ================================

template <typename TYPE>
struct IsPointerToMemberData_Imp : bsl::false_type {
    // This 'struct' template provides a meta-function to determine whether the
    // (template parameter) 'TYPE' is a pointer type to non-static data member.
    // This generic default template derives from 'bsl::false_type'.  A
    // template specialization is provided (below) that derives from
    // 'bsl::true_type'.
};

template <typename TYPE, typename CLASS>
struct IsPointerToMemberData_Imp<TYPE CLASS::*> : bsl::true_type
{
     // This partial specialization of 'IsPointerToMemberData_Imp' derives from
     // 'bsl::true_type' for when the (template parameter) 'TYPE' is a pointer
     // type to non-static data member.
};

}  // close package namespace
}  // close enterprise namespace

namespace bsl {

                       // ===============================
                       // struct is_member_object_pointer
                       // ===============================

template <typename TYPE>
struct is_member_object_pointer
    : integral_constant<bool,
                       BloombergLP::bslmf::IsPointerToMemberData_Imp<
                           typename remove_cv<TYPE>::type>::value
                       && !is_member_function_pointer<TYPE>::value
                       && !is_reference<TYPE>::value>
{
    // This 'struct' template implements the 'is_member_object_pointer'
    // meta-function defined in the C++11 standard [meta.unary.cat] to
    // determine if the (template parameter) 'TYPE' is a pointer type to
    // non-static data member.  This 'struct' derives from 'bsl::true_type' if
    // the 'TYPE' is a pointer type to non-static data member, and
    // 'bsl::false_type' otherwise.
};

}  // close namespace bsl

#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2012 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
