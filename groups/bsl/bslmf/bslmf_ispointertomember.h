// bslmf_ispointertomember.h                                          -*-C++-*-
#ifndef INCLUDED_BSLMF_ISPOINTERTOMEMBER
#define INCLUDED_BSLMF_ISPOINTERTOMEMBER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for pointer-to-member types.
//
//@CLASSES:
//          bslmf_IsPointerToMember: check for pointer-to-member types
//      bslmf_IsPointerToMemberData: check for pointer-to-member data types
//  bslmf_IsPointerToMemberFunction: check for pointer-to-member function types
//
//@AUTHOR: Clay Wilson (cwilson9)
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component defines a simple template structure used to
// evaluate whether it's parameter is a pointer to member, optionally qualified
// with 'const' or 'volatile'.  'bslmf_IsPointerToMember' defines a 'VALUE'
// member that is initialized (at compile-time) to 1 if the parameter is a
// pointer to member (or a reference to such a type), and to 0 otherwise.  The
// meta-functions 'bslmf_IsPointerToMemberData' and
// 'bslmf_IsPointerToMemberFunction' are also provided to test specifically for
// pointers to (non-'static') data members and pointers to (non-'static')
// function members, respectively.  Note that, consistent with TR1, these
// metafunctions evaluate to 0 (false) for reference types
// (i.e., reference-to-pointer-to-member types).
//
///Usage
///-----
// For example:
//..
//   struct MyStruct {};
//   enum   MyEnum {};
//   class  MyClass {};
//
//   typedef int (MyClass::* PMFdRi)(double, int&);
//   typedef int (MyClass::* PMFCe)(MyEnum) const;
//
//   assert(0 == bslmf_IsPointerToMember<int             *>::VALUE);
//   assert(0 == bslmf_IsPointerToMember<    MyStruct    *>::VALUE);
//   assert(1 == bslmf_IsPointerToMember<int MyStruct::*  >::VALUE);
//   assert(0 == bslmf_IsPointerToMember<int MyStruct::*& >::VALUE);
//   assert(0 == bslmf_IsPointerToMember<int MyStruct::* *>::VALUE);
//
//   assert(1 == bslmf_IsPointerToMemberData<int MyStruct::*>::VALUE);
//   assert(0 == bslmf_IsPointerToMemberData<PMFdRi         >::VALUE);
//
//   assert(1 == bslmf_IsPointerToMember<PMFdRi >::VALUE);
//   assert(0 == bslmf_IsPointerToMember<PMFdRi&>::VALUE);
//   assert(1 == bslmf_IsPointerToMember<PMFCe  >::VALUE);
//   assert(0 == bslmf_IsPointerToMember<PMFCe& >::VALUE);
//
//   assert(1 == bslmf_IsPointerToMemberFunction<PMFdRi        >::VALUE);
//   assert(0 == bslmf_IsPointerToMemberFunction<int MyClass::*>::VALUE);
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_METAINT
#include <bslmf_metaint.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECVQ
#include <bslmf_removecvq.h>
#endif

namespace BloombergLP {

                      // ======================================
                      // struct bslmf_IsPointerToMemberFunction
                      // ======================================

template <typename PROTOTYPE>
struct bslmf_IsPointerToMemberFunction_Imp : bslmf_MetaInt<0> {
};

template <typename RETURN, typename CLASS>
struct bslmf_IsPointerToMemberFunction_Imp<RETURN (CLASS::*)()>
: bslmf_MetaInt<1> {
};

template <typename RETURN, typename CLASS, typename ARG1>
struct bslmf_IsPointerToMemberFunction_Imp<RETURN (CLASS::*)(ARG1)>
: bslmf_MetaInt<1> {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2>
struct bslmf_IsPointerToMemberFunction_Imp<RETURN (CLASS::*)(ARG1, ARG2)>
: bslmf_MetaInt<1> {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3>
struct bslmf_IsPointerToMemberFunction_Imp<RETURN (CLASS::*)(ARG1, ARG2, ARG3)>
: bslmf_MetaInt<1> {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4>
struct bslmf_IsPointerToMemberFunction_Imp<
                                     RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4)>
: bslmf_MetaInt<1> {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4,
                                           typename ARG5>
struct bslmf_IsPointerToMemberFunction_Imp<
                               RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5)>
: bslmf_MetaInt<1> {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4,
                                           typename ARG5, typename ARG6>
struct bslmf_IsPointerToMemberFunction_Imp<
                         RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6)>
: bslmf_MetaInt<1> {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4,
                                           typename ARG5, typename ARG6,
                                           typename ARG7>
struct bslmf_IsPointerToMemberFunction_Imp<
                   RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7)>
: bslmf_MetaInt<1> {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4,
                                           typename ARG5, typename ARG6,
                                           typename ARG7, typename ARG8>
struct bslmf_IsPointerToMemberFunction_Imp<
             RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8)>
: bslmf_MetaInt<1> {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4,
                                           typename ARG5, typename ARG6,
                                           typename ARG7, typename ARG8,
                                           typename ARG9>
struct bslmf_IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8, ARG9)>
: bslmf_MetaInt<1> {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4,
                                           typename ARG5, typename ARG6,
                                           typename ARG7, typename ARG8,
                                           typename ARG9, typename ARG10>
struct bslmf_IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                         ARG6, ARG7, ARG8, ARG9, ARG10)> : bslmf_MetaInt<1> {
};

// Match pointer to const member function:
template <typename RETURN, typename CLASS>
struct bslmf_IsPointerToMemberFunction_Imp<RETURN (CLASS::*)() const>
: bslmf_MetaInt<1> {
};

template <typename RETURN, typename CLASS, typename ARG1>
struct bslmf_IsPointerToMemberFunction_Imp<RETURN (CLASS::*)(ARG1) const>
: bslmf_MetaInt<1> {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2>
struct bslmf_IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2) const>
: bslmf_MetaInt<1> {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3>
struct bslmf_IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3) const>
: bslmf_MetaInt<1> {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4>
struct bslmf_IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4) const>
: bslmf_MetaInt<1> {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4,
                                           typename ARG5>
struct bslmf_IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5) const>
: bslmf_MetaInt<1> {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4,
                                           typename ARG5, typename ARG6>
struct bslmf_IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6) const>
: bslmf_MetaInt<1> {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4,
                                           typename ARG5, typename ARG6,
                                           typename ARG7>
struct bslmf_IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                         ARG6, ARG7) const>
: bslmf_MetaInt<1> {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4,
                                           typename ARG5, typename ARG6,
                                           typename ARG7, typename ARG8>
struct bslmf_IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                         ARG6, ARG7, ARG8) const>
: bslmf_MetaInt<1> {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4,
                                           typename ARG5, typename ARG6,
                                           typename ARG7, typename ARG8,
                                           typename ARG9>
struct bslmf_IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                         ARG6, ARG7, ARG8, ARG9) const>
: bslmf_MetaInt<1> {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4,
                                           typename ARG5, typename ARG6,
                                           typename ARG7, typename ARG8,
                                           typename ARG9, typename ARG10>
struct bslmf_IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                         ARG6, ARG7, ARG8, ARG9, ARG10) const>
: bslmf_MetaInt<1> {
};

// Match pointer to volatile member function:
template <typename RETURN, typename CLASS>
struct bslmf_IsPointerToMemberFunction_Imp<RETURN (CLASS::*)() volatile>
: bslmf_MetaInt<1> {
};

template <typename RETURN, typename CLASS, typename ARG1>
struct bslmf_IsPointerToMemberFunction_Imp<RETURN (CLASS::*)(ARG1) volatile>
: bslmf_MetaInt<1> {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2>
struct bslmf_IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2) volatile> : bslmf_MetaInt<1> {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3>
struct bslmf_IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3) volatile> : bslmf_MetaInt<1> {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4>
struct bslmf_IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4) volatile>
: bslmf_MetaInt<1> {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4,
                                           typename ARG5>
struct bslmf_IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5) volatile>
: bslmf_MetaInt<1> {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4,
                                           typename ARG5, typename ARG6>
struct bslmf_IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6) volatile>
: bslmf_MetaInt<1> {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4,
                                           typename ARG5, typename ARG6,
                                           typename ARG7>
struct bslmf_IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                         ARG6, ARG7) volatile>
: bslmf_MetaInt<1> {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4,
                                           typename ARG5, typename ARG6,
                                           typename ARG7, typename ARG8>
struct bslmf_IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                         ARG6, ARG7, ARG8) volatile>
: bslmf_MetaInt<1> {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4,
                                           typename ARG5, typename ARG6,
                                           typename ARG7, typename ARG8,
                                           typename ARG9>
struct bslmf_IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                         ARG6, ARG7, ARG8, ARG9) volatile>
: bslmf_MetaInt<1> {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4,
                                           typename ARG5, typename ARG6,
                                           typename ARG7, typename ARG8,
                                           typename ARG9, typename ARG10>
struct bslmf_IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                         ARG6, ARG7, ARG8, ARG9, ARG10) volatile>
: bslmf_MetaInt<1> {
};

// Match pointer to const volatile member function:
template <typename RETURN, typename CLASS>
struct bslmf_IsPointerToMemberFunction_Imp<RETURN (CLASS::*)() const volatile> 
: bslmf_MetaInt<1> {
};

template <typename RETURN, typename CLASS, typename ARG1>
struct bslmf_IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1) const volatile>
: bslmf_MetaInt<1> {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2>
struct bslmf_IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2) const volatile>
: bslmf_MetaInt<1> {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3>
struct bslmf_IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3) const volatile>
: bslmf_MetaInt<1> {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4>
struct bslmf_IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4) const volatile>
: bslmf_MetaInt<1> {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4,
                                           typename ARG5>
struct bslmf_IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5) const volatile>
: bslmf_MetaInt<1>{
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4,
                                           typename ARG5, typename ARG6>
struct bslmf_IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6) const volatile>
: bslmf_MetaInt<1> {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4,
                                           typename ARG5, typename ARG6,
                                           typename ARG7>
struct bslmf_IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                         ARG6, ARG7) const volatile>
: bslmf_MetaInt<1> {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4,
                                           typename ARG5, typename ARG6,
                                           typename ARG7, typename ARG8>
struct bslmf_IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                         ARG6, ARG7, ARG8) const volatile>
: bslmf_MetaInt<1> {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4,
                                           typename ARG5, typename ARG6,
                                           typename ARG7, typename ARG8,
                                           typename ARG9>
struct bslmf_IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                         ARG6, ARG7, ARG8, ARG9) const volatile>
: bslmf_MetaInt<1> {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4,
                                           typename ARG5, typename ARG6,
                                           typename ARG7, typename ARG8,
                                           typename ARG9, typename ARG10>
struct bslmf_IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                         ARG6, ARG7, ARG8, ARG9, ARG10) const volatile>
: bslmf_MetaInt<1> {
};

template <typename TYPE>
struct bslmf_IsPointerToMemberFunction
: bslmf_IsPointerToMemberFunction_Imp<
                                  typename bslmf_RemoveCvq<TYPE>::Type>::Type {
    // Metafunction to test if 'TYPE' is a pointer to a member function.
    // Note that the result is false if 'TYPE' is a reference.
};

template <typename TYPE>
struct bslmf_IsPointerToMemberFunction<TYPE&> : bslmf_MetaInt<0> {
    // Specialization of 'bslmf_IsPointerToMemberFunction' for reference
    // types.  Result is always false.
};

                      // ==================================
                      // struct bslmf_IsPointerToMemberData
                      // ==================================

template <typename TYPE>
struct bslmf_IsPointerToMemberData_Imp : bslmf_MetaInt<0> {
};

template <typename TYPE, typename CLASS>
struct bslmf_IsPointerToMemberData_Imp<TYPE CLASS::*> : bslmf_MetaInt<1> {
};

template <typename TYPE>
struct bslmf_IsPointerToMemberData
: bslmf_MetaInt<!bslmf_IsPointerToMemberFunction<TYPE>::VALUE &&
                 bslmf_IsPointerToMemberData_Imp<typename
                                         bslmf_RemoveCvq<TYPE>::Type>::VALUE> {
    // Metafunction to test if 'TYPE' is a pointer to a member object.
    // Note that the result is false if 'TYPE' is a reference.
};

template <typename TYPE>
struct bslmf_IsPointerToMemberData<TYPE&> : bslmf_MetaInt<0> {
    // Specialization of 'bslmf_IsPointerToMemberData' for reference
    // types.  Result is always false.
};

                      // ==============================
                      // struct bslmf_IsPointerToMember
                      // ==============================

template <typename TYPE>
struct bslmf_IsPointerToMember
: bslmf_MetaInt<bslmf_IsPointerToMemberData<TYPE>::VALUE ||
                bslmf_IsPointerToMemberFunction<TYPE>::VALUE> {
    // Metafunction to test if 'TYPE' is a pointer to member (function or
    // object).  The result is false if 'TYPE' is a reference.
};

template <typename TYPE>
struct bslmf_IsPointerToMember<TYPE&> : bslmf_MetaInt<0> {
    // Specialization of 'bslmf_IsPointerToMember' for reference
    // types.  Result is always false.
};

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
