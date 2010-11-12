// bdemf_memberfunctionpointertraits.h                                -*-C++-*-
#ifndef INCLUDED_BDEMF_MEMBERFUNCTIONPOINTERTRAITS
#define INCLUDED_BDEMF_MEMBERFUNCTIONPOINTERTRAITS

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide meta-functions to detect member function pointer traits.
//
//@DEPRECATED: Use 'bslmf_memberfunctionpointertraits' instead.
//
//@CLASSES:
// bdemf_MemberFunctionPointerTraits: Meta-function for detecting member
//                                    function  pointer traits
//     bdemf_IsMemberFunctionPointer: Meta-function to determine if a type is
//                                    a member function pointer
//
//@AUTHOR: Ilougino Rocha (irocha)
//
//@SEE_ALSO: bdemf_FunctionPointerTraits
//
//@DESCRIPTION: This component provides meta-functions for determining the
// traits of a member function pointer.  Two meta-functions are provided:
// 'bdemf_IsMemberFunctionPointer', and 'bdemf_MemberFunctionPointerTraits'.
// 'bdemf_IsMemberFunctionPointer' tests if a given type is member function
// pointer.  'bdemf_MemberFunctionPointerTraits' determines the traits of a
// member function type, including the type of the object that it is a member
// of, its result type, and the type of its list of arguments.
//
///Usage
///-----
// Define the following function types:
//..
//  typedef void (*VoidFunc0)();
//..
// and the following 'struct' with the following members:
//..
//  struct MyTestClass {
//      static void VoidFunc0() {}
//      int VoidFunc1(int) { return 0; }
//      int VoidFunc2(int, int) { return 1; }
//  };
//..
// In order to deduce the types of 'VoidFunc0' and 'VoidFunc1', we will use the
// C++ template system to get two auxilliary functions:
//..
//  template <class TYPE>
//  void checkNotMemberFunctionPointer(TYPE object)
//  {
//      ASSERT(0 == bdemf_IsMemberFunctionPointer<TYPE>::VALUE);
//  }
//
//  template <class RET, class ARGS, class TYPE>
//  void checkMemberFunctionPointer(TYPE object)
//  {
//      ASSERT(1 == bdemf_IsMemberFunctionPointer<TYPE>::VALUE);
//      typedef typename bdemf_MemberFunctionPointerTraits<TYPE>::ResultType
//          ResultType;
//      typedef typename bdemf_MemberFunctionPointerTraits<TYPE>::ArgumentList
//          ArgumentList;
//      ASSERT(1 == (bdemf_IsSame<ResultType, RET>::VALUE));
//      ASSERT(1 == (bdemf_IsSame<ArgumentList, ARGS>::VALUE));
//  }
//..
// The following program should compile and run without errors:
//..
//  void usageExample()
//  {
//      ASSERT(0 == bdemf_IsMemberFunctionPointer<int>::VALUE);
//      ASSERT(0 == bdemf_IsMemberFunctionPointer<int>::VALUE);
//
//      checkNotMemberFunctionPointer( &MyTestClass::VoidFunc0);
//      checkMemberFunctionPointer<int, bdemf_TypeList1<int> >(
//                                                    &MyTestClass::VoidFunc1);
//      checkMemberFunctionPointer<int, bdemf_TypeList2<int, int> >(
//                                                    &MyTestClass::VoidFunc2);
//  }
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLMF_MEMBERFUNCTIONPOINTERTRAITS
#include <bslmf_memberfunctionpointertraits.h>
#endif

#ifndef bdemf_MemberFunctionPointerTraits
#define bdemf_MemberFunctionPointerTraits   bslmf_MemberFunctionPointerTraits
#endif

#ifndef bdemf_IsMemberFunctionPointer
#define bdemf_IsMemberFunctionPointer       bslmf_IsMemberFunctionPointer
    // This template determines if the specified 'PROTOTYPE' is a member
    // function pointer.  VALUE is defined as 1 if the specified 'PROTOTYPE'
    // is a member function, and a zero value otherwise.
#endif

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
