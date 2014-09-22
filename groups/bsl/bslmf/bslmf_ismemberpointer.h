// bslmf_ismemberpointer.h                                            -*-C++-*-
#ifndef INCLUDED_BSLMF_ISMEMBERPOINTER
#define INCLUDED_BSLMF_ISMEMBERPOINTER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for non-static member pointer types.
//
//@CLASSES:
//  bsl::is_member_pointer: standard meta-function for member pointer types
//
//@SEE_ALSO: bslmf_ismemberfunctionpointer, bslmf_ismemberobjectpointer
//
//@DESCRIPTION: This component defines a meta-function,
// 'bsl::is_member_pointer', that may be used to query whether a type is a
// pointer to non-static member type.
//
// 'bsl::is_member_pointer' meets the requirements of the 'is_member_pointer'
// template defined in the C++11 standard [meta.unary.comp].
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

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

#ifndef INCLUDED_BSLMF_ISMEMBERFUNCTIONPOINTER
#include <bslmf_ismemberfunctionpointer.h>
#endif

#ifndef INCLUDED_BSLMF_ISMEMBEROBJECTPOINTER
#include <bslmf_ismemberobjectpointer.h>
#endif

namespace bsl {

                         // ========================
                         // struct is_member_pointer
                         // ========================

template <class TYPE>
struct is_member_pointer
    : integral_constant<bool,
                        is_member_object_pointer<TYPE>::value
                        || bsl::is_member_function_pointer<TYPE>::value> {
    // This 'struct' template implements the 'is_member_pointer' meta-function
    // defined in the C++11 standard [meta.unary.comp] to determine if the
    // (template parameter) 'TYPE' is a member pointer type.  This 'struct'
    // derives from 'bsl::true_type' if the 'TYPE' is a member pointer type,
    // and from 'bsl::false_type' otherwise.
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
