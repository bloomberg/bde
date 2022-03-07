// bslmf_memberpointertraits.h                                        -*-C++-*-
#ifndef INCLUDED_BSLMF_POINTERTOMEMBERTRAITS
#define INCLUDED_BSLMF_POINTERTOMEMBERTRAITS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide meta-function to detect pointer to member traits.
//
//@CLASSES:
//  bslmf::MemberPointerTraits: meta-function to get member pointer traits
//
//@SEE_ALSO: bslmf_memberfunctionpointertraits
//
//@DESCRIPTION: This component provides a meta-function,
// 'bslmf::MemberPointerTraits', that determines traits of a
// pointer-to-member type, including the type of the object that it is a member
// of, and the type of the member it addresses.
//
///Usage
///-----
// Define the following 'struct' with the following members:
//..
//  struct MyTestClass {
//      int func1(int) { return 0; }
//      int d_int;
//  };
//..
// In order to deduce the types of 'func1' and 'd_int', we will use
// 'bslmf::MemberPointerTraits'.
//..
//  template <class MEMBER, class CLASS, class TYPE>
//  void checkMemberPointer(TYPE pointer)
//  {
//      (void) pointer;
//      typedef typename bslmf::MemberPointerTraits<TYPE>::MemberType
//          MemberType;
//      typedef typename bslmf::MemberPointerTraits<TYPE>::ClassType
//          ClassType;
//      assert(1 == (bsl::is_same<MemberType, MEMBER>::value));
//      assert(1 == (bsl::is_same<ClassType, CLASS>::value));
//  }
//..
// The following program should compile and run without errors:
//..
//  void usageExample()
//  {
//      checkMemberPointer<int(int), MyTestClass>(&MyTestClass::func1);
//      checkMemberPointer<int, MyTestClass>(&MyTestClass::d_int);
//  }
//..

#include <bslscm_version.h>

#include <bslmf_removecv.h>

namespace BloombergLP {
namespace bslmf {

template <class TYPE>
struct MemberPointerTraits_Imp;

                        // ===========================
                        // class MemberPointerTraits
                        // ===========================

template <class TYPE>
struct MemberPointerTraits
: public MemberPointerTraits_Imp<typename bsl::remove_cv<TYPE>::type> {
    // This utility 'struct' template provides the following nested typedefs:
    //: 'ClassType':  The type of the class for which the specified 'TYPE' is a
    //:               pointer to member object.
    //: 'MemberType': The type of the member object of the class for which the
    //:               specified 'TYPE' is a pointer to member object.
    // These typedefs will only be defined if 'TYPE' is a
    // pointer-to-member-object type.  The primary (unspecialized)
    // MemberPointerTraits_Imp template is defined and empty.
};

                      // ===============================
                      // class MemberPointerTraits_Imp
                      // ===============================

template <class TYPE>
struct MemberPointerTraits_Imp
{
    // Empty
};

template <class MEMBER_TYPE, class CLASS_TYPE>
struct MemberPointerTraits_Imp<MEMBER_TYPE CLASS_TYPE::*> {
    // TYPES
    typedef CLASS_TYPE ClassType;
        // 'ClassType' is an alias to the type of the class for which the
        // specified 'TYPE' is a pointer to member object.

    typedef MEMBER_TYPE MemberType;
        // 'MemberType' is an alias to the type of object to which the member
        // object points.
};

}  // close package namespace
}  // close enterprise namespace

#endif  // INCLUDED_BSLMF_POINTERTOMEMBERTRAITS

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
