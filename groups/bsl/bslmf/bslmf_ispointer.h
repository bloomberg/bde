// bslmf_ispointer.h                                                  -*-C++-*-
#ifndef INCLUDED_BSLMF_ISPOINTER
#define INCLUDED_BSLMF_ISPOINTER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for pointer types.
//
//@CLASSES:
//  bslmf::IsPointer: meta-function for determining pointer types
//
//@DESCRIPTION: This component defines a simple template structure used to
// evaluate whether it's parameter is a pointer.  'bslmf::IsPointer' defines a
// member, 'value', whose value is initialized (at compile-time) to 1 if the
// parameter is a pointer (to anything) (ignoring any 'const' or 'volatile'
// qualification), and 0 if it is not.  For example:
//..
//  struct MyType {};
//  typedef MyType* PMT;
//
//  static const int a1 = bslmf::IsPointer<int *        >::value; // a1 == 1
//  static const int a2 = bslmf::IsPointer<int *const   >::value; // a2 == 1
//  static const int a3 = bslmf::IsPointer<int *volatile>::value; // a3 == 1
//  static const int a4 = bslmf::IsPointer<int          >::value; // a4 == 0
//  static const int a5 = bslmf::IsPointer<MyType       >::value; // a5 == 0
//  static const int a6 = bslmf::IsPointer<MyType*      >::value; // a6 == 1
//  static const int a7 = bslmf::IsPointer<PMT          >::value; // a7 == 1
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

#ifndef INCLUDED_BSLMF_METAINT
#include <bslmf_metaint.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECV
#include <bslmf_removecv.h>
#endif

namespace BloombergLP {
namespace bslmf {

template <typename TYPE>
struct IsPointer_Imp : bsl::false_type
{};

template <typename TYPE>
struct IsPointer_Imp<TYPE *> : bsl::true_type
{};

}
}

namespace bsl {

template <typename TYPE>
struct is_pointer
    : BloombergLP::bslmf::IsPointer_Imp<typename remove_cv<TYPE>::type>::type
{};

}

namespace BloombergLP {
namespace bslmf {

                         // ================
                         // struct IsPointer
                         // ================

template <typename T>
struct IsPointer : bsl::is_pointer<T>::type
{
    // This class implements a meta-function for checking if a type is a
    // pointer.
};

}  // close package namespace
}  // close enterprise namespace

#ifndef BDE_OMIT_TRANSITIONAL  // BACKWARD_COMPATIBILITY
// ===========================================================================
//                           BACKWARD COMPATIBILITY
// ===========================================================================

#ifdef bslmf_IsPointer
#undef bslmf_IsPointer
#endif
#define bslmf_IsPointer bslmf::IsPointer
    // This alias is defined for backward compatibility.
#endif  // BDE_OMIT_TRANSITIONAL -- BACKWARD_COMPATIBILITY

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
