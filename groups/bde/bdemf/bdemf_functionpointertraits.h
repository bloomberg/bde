// bdemf_functionpointertraits.h                                      -*-C++-*-
#ifndef INCLUDED_BDEMF_FUNCTIONPOINTERTRAITS
#define INCLUDED_BDEMF_FUNCTIONPOINTERTRAITS

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function for determining function pointer traits.
//
//@DEPRECATED: Use 'bslmf_functionpointertraits' instead.
//
//@CLASSES:
//   bdemf_FunctionPointerTraits: meta-function for detecting function pointer
//                                traits
//       bdemf_IsFunctionPointer: meta-function to determine if a type is a
//                                function pointer
//
//@AUTHOR: Ilougino Rocha (irocha)
//
//@SEE_ALSO: bdemf_MemberFunctionPointerTraits
//
//@DESCRIPTION: This component provides meta-functions for determining whether
// a type is a pointer to either a free function or a class method (but not to
// a member function, see the component 'bdemf_memberfunctionpointertraits'
// component for that), and some information about this function type.  The
// meta-function 'bdemf_IsFunctionPointer' provides an enumerated 'VALUE' which
// can be either 1 or 0 depending on whether or not the template argument
// 'PROTOTYPE' is a pointer to a free function or class method.  In the
// affirmative, the class 'bdemf_FunctionPointerTraits' also provides
// information regarding the function type, such as its argument list type and
// its return type.
//
// Note that there is no reference to function traits, since whether 'FUNC' is
// a reference to function type can be very easily obtained using the
// meta-function call 'bdemf_IsFunctionPointer<FUNC*>'.
//
///Usage
///-----
// Define the following function types:
//..
//  typedef int  (*IntFunctionIntIntPtr)(int, int);
//  typedef void (*VoidFunc0)();
//..
// The following program should compile and run without errors:
//..
//  int main()
//  {
//      assert(0 == bdemf_IsFunctionPointer<int>::VALUE);
//      assert(0 == bdemf_IsFunctionPointer<int>::VALUE);
//
//      assert(1 == bdemf_IsFunctionPointer<IntFunctionIntIntPtr>::VALUE);
//      typedef bdemf_FunctionPointerTraits<IntFunctionIntIntPtr>::ResultType
//                                                                 ResultType1;
//      assert(1 == (bdemf_IsSame<ResultType1, int>::VALUE));
//
//      assert(1 == bdemf_IsFunctionPointer<VoidFunc0>::VALUE);
//      typedef bdemf_FunctionPointerTraits<VoidFunc0>::ResultType
//                                                                 ResultType0;
//      typedef bdemf_FunctionPointerTraits<VoidFunc0>::ArgumentList  ArgList0;
//      assert(1 == (bdemf_IsSame<ResultType0, void>::VALUE));
//      assert(1 == (bdemf_IsSame<ArgList0, bdemf_TypeList0>::VALUE));
//  }
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLMF_FUNCTIONPOINTERTRAITS
#include <bslmf_functionpointertraits.h>
#endif

#ifndef bdemf_FunctionPointerTraits
#define bdemf_FunctionPointerTraits  bslmf_FunctionPointerTraits
    // This class gives information about the specified 'PROTOTYPE'.  The
    // general definition gives no information, but specializations for
    // function pointers types define nested types 'ResultType',
    // 'ArgumentList', and 'Linkage'.
#endif

#ifndef bdemf_IsFunctionPointer
#define bdemf_IsFunctionPointer      bslmf_IsFunctionPointer
    // This template determines if the specified 'PROTOTYPE' is a free (i.e.,
    // non-member) function pointer.  VALUE is defined as 1 if the specified
    // 'PROTOTYPE' is a function pointer type, and a zero value otherwise.
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
