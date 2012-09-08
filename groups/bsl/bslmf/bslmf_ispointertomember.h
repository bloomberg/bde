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
//  bslmf::IsPointerToMember: check for pointer-to-member types
//  bslmf::IsPointerToMemberData: check for pointer-to-member data types
//  bslmf::IsPointerToMemberFunction: check for pointer-to-member function types
//
//@AUTHOR: Clay Wilson (cwilson9)
//
//@DESCRIPTION: This component defines a simple template structure used to
// evaluate whether it's parameter is a pointer to member, optionally qualified
// with 'const' or 'volatile'.  'bslmf::IsPointerToMember' defines a 'VALUE'
// member that is initialized (at compile-time) to 1 if the parameter is a
// pointer to member (or a reference to such a type), and to 0 otherwise.  The
// meta-functions 'bslmf::IsPointerToMemberData' and
// 'bslmf::IsPointerToMemberFunction' are also provided to test specifically
// for pointers to (non-'static') data members and pointers to (non-'static')
// function members, respectively.  Note that, consistent with TR1, these
// metafunctions evaluate to 0 (false) for reference types (i.e.,
// reference-to-pointer-to-member types).
//
///Usage
///-----
// For example:
//..
//  struct MyStruct {};
//  enum   MyEnum {};
//  class  MyClass {};
//
//  typedef int (MyClass::* PMFdRi)(double, int&);
//  typedef int (MyClass::* PMFCe)(MyEnum) const;
//
//  assert(0 == bslmf::IsPointerToMember<int             *>::VALUE);
//  assert(0 == bslmf::IsPointerToMember<    MyStruct    *>::VALUE);
//  assert(1 == bslmf::IsPointerToMember<int MyStruct::*  >::VALUE);
//  assert(0 == bslmf::IsPointerToMember<int MyStruct::*& >::VALUE);
//  assert(0 == bslmf::IsPointerToMember<int MyStruct::* *>::VALUE);
//
//  assert(1 == bslmf::IsPointerToMemberData<int MyStruct::*>::VALUE);
//  assert(0 == bslmf::IsPointerToMemberData<PMFdRi         >::VALUE);
//
//  assert(1 == bslmf::IsPointerToMember<PMFdRi >::VALUE);
//  assert(0 == bslmf::IsPointerToMember<PMFdRi&>::VALUE);
//  assert(1 == bslmf::IsPointerToMember<PMFCe  >::VALUE);
//  assert(0 == bslmf::IsPointerToMember<PMFCe& >::VALUE);
//
//  assert(1 == bslmf::IsPointerToMemberFunction<PMFdRi        >::VALUE);
//  assert(0 == bslmf::IsPointerToMemberFunction<int MyClass::*>::VALUE);
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_METAINT
#include <bslmf_metaint.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGERCONSTANT
#include <bslmf_integerconstant.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECVQ
#include <bslmf_removecvq.h>
#endif

#ifndef INCLUDED_BSLMF_ISREFERENCE
#include <bslmf_isreference.h>
#endif

                      // ================================
                      // struct IsPointerToMemberFunction
                      // ================================

namespace BloombergLP {
namespace bslmf {

template <typename PROTOTYPE>
struct IsPointerToMemberFunction_Imp : bsl::false_type {
};

template <typename RETURN, typename CLASS>
struct IsPointerToMemberFunction_Imp<RETURN (CLASS::*)()> : bsl::true_type {
};

template <typename RETURN, typename CLASS, typename ARG1>
struct IsPointerToMemberFunction_Imp<RETURN (CLASS::*)(ARG1)> : bsl::true_type {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2>
struct IsPointerToMemberFunction_Imp<RETURN (CLASS::*)(ARG1, ARG2)>
: bsl::true_type {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3>
struct IsPointerToMemberFunction_Imp<RETURN (CLASS::*)(ARG1, ARG2, ARG3)>
: bsl::true_type {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4>
struct IsPointerToMemberFunction_Imp<RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4)>
: bsl::true_type {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4,
                                           typename ARG5>
struct IsPointerToMemberFunction_Imp<
                               RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5)>
: bsl::true_type {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4,
                                           typename ARG5, typename ARG6>
struct IsPointerToMemberFunction_Imp<
                         RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6)>
: bsl::true_type {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4,
                                           typename ARG5, typename ARG6,
                                           typename ARG7>
struct IsPointerToMemberFunction_Imp<
                   RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7)>
: bsl::true_type {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4,
                                           typename ARG5, typename ARG6,
                                           typename ARG7, typename ARG8>
struct IsPointerToMemberFunction_Imp<
             RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8)>
: bsl::true_type {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4,
                                           typename ARG5, typename ARG6,
                                           typename ARG7, typename ARG8,
                                           typename ARG9>
struct IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8, ARG9)>
: bsl::true_type {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4,
                                           typename ARG5, typename ARG6,
                                           typename ARG7, typename ARG8,
                                           typename ARG9, typename ARG10>
struct IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                         ARG6, ARG7, ARG8, ARG9, ARG10)> : bsl::true_type {
};

// Match pointer to const member function:
template <typename RETURN, typename CLASS>
struct IsPointerToMemberFunction_Imp<RETURN (CLASS::*)() const>
: bsl::true_type {
};

template <typename RETURN, typename CLASS, typename ARG1>
struct IsPointerToMemberFunction_Imp<RETURN (CLASS::*)(ARG1) const>
: bsl::true_type {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2>
struct IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2) const>
: bsl::true_type {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3>
struct IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3) const>
: bsl::true_type {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4>
struct IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4) const>
: bsl::true_type {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4,
                                           typename ARG5>
struct IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5) const>
: bsl::true_type {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4,
                                           typename ARG5, typename ARG6>
struct IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6) const>
: bsl::true_type {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4,
                                           typename ARG5, typename ARG6,
                                           typename ARG7>
struct IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                         ARG6, ARG7) const>
: bsl::true_type {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4,
                                           typename ARG5, typename ARG6,
                                           typename ARG7, typename ARG8>
struct IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                         ARG6, ARG7, ARG8) const>
: bsl::true_type {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4,
                                           typename ARG5, typename ARG6,
                                           typename ARG7, typename ARG8,
                                           typename ARG9>
struct IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                         ARG6, ARG7, ARG8, ARG9) const>
: bsl::true_type {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4,
                                           typename ARG5, typename ARG6,
                                           typename ARG7, typename ARG8,
                                           typename ARG9, typename ARG10>
struct IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                         ARG6, ARG7, ARG8, ARG9, ARG10) const>
: bsl::true_type {
};

// Match pointer to volatile member function:
template <typename RETURN, typename CLASS>
struct IsPointerToMemberFunction_Imp<RETURN (CLASS::*)() volatile>
: bsl::true_type {
};

template <typename RETURN, typename CLASS, typename ARG1>
struct IsPointerToMemberFunction_Imp<RETURN (CLASS::*)(ARG1) volatile>
: bsl::true_type {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2>
struct IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2) volatile> : bsl::true_type {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3>
struct IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3) volatile> : bsl::true_type {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4>
struct IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4) volatile>
: bsl::true_type {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4,
                                           typename ARG5>
struct IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5) volatile>
: bsl::true_type {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4,
                                           typename ARG5, typename ARG6>
struct IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6) volatile>
: bsl::true_type {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4,
                                           typename ARG5, typename ARG6,
                                           typename ARG7>
struct IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                         ARG6, ARG7) volatile>
: bsl::true_type {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4,
                                           typename ARG5, typename ARG6,
                                           typename ARG7, typename ARG8>
struct IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                         ARG6, ARG7, ARG8) volatile>
: bsl::true_type {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4,
                                           typename ARG5, typename ARG6,
                                           typename ARG7, typename ARG8,
                                           typename ARG9>
struct IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                         ARG6, ARG7, ARG8, ARG9) volatile>
: bsl::true_type {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4,
                                           typename ARG5, typename ARG6,
                                           typename ARG7, typename ARG8,
                                           typename ARG9, typename ARG10>
struct IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                         ARG6, ARG7, ARG8, ARG9, ARG10) volatile>
: bsl::true_type {
};

// Match pointer to const volatile member function:
template <typename RETURN, typename CLASS>
struct IsPointerToMemberFunction_Imp<RETURN (CLASS::*)() const volatile>
: bsl::true_type {
};

template <typename RETURN, typename CLASS, typename ARG1>
struct IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1) const volatile>
: bsl::true_type {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2>
struct IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2) const volatile>
: bsl::true_type {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3>
struct IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3) const volatile>
: bsl::true_type {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4>
struct IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4) const volatile>
: bsl::true_type {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4,
                                           typename ARG5>
struct IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5) const volatile>
: bsl::true_type{
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4,
                                           typename ARG5, typename ARG6>
struct IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6) const volatile>
: bsl::true_type {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4,
                                           typename ARG5, typename ARG6,
                                           typename ARG7>
struct IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                         ARG6, ARG7) const volatile>
: bsl::true_type {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4,
                                           typename ARG5, typename ARG6,
                                           typename ARG7, typename ARG8>
struct IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                         ARG6, ARG7, ARG8) const volatile>
: bsl::true_type {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4,
                                           typename ARG5, typename ARG6,
                                           typename ARG7, typename ARG8,
                                           typename ARG9>
struct IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                         ARG6, ARG7, ARG8, ARG9) const volatile>
: bsl::true_type {
};

template <typename RETURN, typename CLASS, typename ARG1, typename ARG2,
                                           typename ARG3, typename ARG4,
                                           typename ARG5, typename ARG6,
                                           typename ARG7, typename ARG8,
                                           typename ARG9, typename ARG10>
struct IsPointerToMemberFunction_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                         ARG6, ARG7, ARG8, ARG9, ARG10) const volatile>
: bsl::true_type {
};

}
}

namespace bsl {

template <typename TYPE>
struct is_member_function_pointer
    : integer_constant<bool,
                       BloombergLP::bslmf::IsPointerToMemberFunction_Imp<
                           typename remove_cv<TYPE>::type>::value
                       && !is_reference<TYPE>::value>
{};

}

namespace BloombergLP {
namespace bslmf {

template <typename TYPE>
struct IsPointerToMemberFunction
    : MetaInt<bsl::is_member_function_pointer<TYPE>::value>
{
    // Metafunction to test if 'TYPE' is a pointer to a member function.  Note
    // that the result is false if 'TYPE' is a reference.
};

}
}

                      // ============================
                      // struct IsPointerToMemberData
                      // ============================

namespace BloombergLP {
namespace bslmf {

template <typename TYPE>
struct IsPointerToMemberData_Imp : bsl::false_type {
};

template <typename TYPE, typename CLASS>
struct IsPointerToMemberData_Imp<TYPE CLASS::*> : bsl::true_type {
};

}
}

namespace bsl {

template <typename TYPE>
struct is_member_object_pointer
    : integer_constant<bool,
                       BloombergLP::bslmf::IsPointerToMemberData_Imp<
                           typename remove_cv<TYPE>::type>::value
                       && !is_member_function_pointer<TYPE>::value
                       && !is_reference<TYPE>::value>
{};

}

namespace BloombergLP {
namespace bslmf {

template <typename TYPE>
struct IsPointerToMemberData
    : MetaInt<bsl::is_member_object_pointer<TYPE>::value>
{
    // Metafunction to test if 'TYPE' is a pointer to a member object.  Note
    // that the result is false if 'TYPE' is a reference.
};

}
}

                      // ========================
                      // struct IsPointerToMember
                      // ========================

namespace bsl {

template <typename TYPE>
struct is_member_pointer
    : integer_constant<bool,
                       is_member_object_pointer<TYPE>::value
                       || bsl::is_member_function_pointer<TYPE>::value>
{};

}

namespace BloombergLP {
namespace bslmf {

template <typename TYPE>
struct IsPointerToMember
    : MetaInt<bsl::is_member_pointer<TYPE>::value>
{
    // Metafunction to test if 'TYPE' is a pointer to member (function or
    // object).  The result is false if 'TYPE' is a reference.
};

}  // close package namespace
}  // close enterprise namespace

// ===========================================================================
//                           BACKWARD COMPATIBILITY
// ===========================================================================

#ifdef bslmf_IsPointerToMember
#undef bslmf_IsPointerToMember
#endif
#define bslmf_IsPointerToMember bslmf::IsPointerToMember
    // This alias is defined for backward compatibility.

#ifdef bslmf_IsPointerToMemberFunction
#undef bslmf_IsPointerToMemberFunction
#endif
#define bslmf_IsPointerToMemberFunction bslmf::IsPointerToMemberFunction
    // This alias is defined for backward compatibility.

#ifdef bslmf_IsPointerToMemberData
#undef bslmf_IsPointerToMemberData
#endif
#define bslmf_IsPointerToMemberData bslmf::IsPointerToMemberData
    // This alias is defined for backward compatibility.

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
