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
//@AUTHOR: Clay Wilson (cwilson9)
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
///Example 1: Verify Member Function Object Types
/// - - - - - - - - - - - - - - - - - - - - - - -
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
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
