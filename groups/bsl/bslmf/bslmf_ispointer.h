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
//@AUTHOR: Shawn Edwards (sedwards)
//
//@DESCRIPTION: This component defines a simple template structure used to
// evaluate whether it's parameter is a pointer.  'bslmf::IsPointer' defines a
// member, 'VALUE', whose value is initialized (at compile-time) to 1 if the
// parameter is a pointer (to anything) (ignoring any 'const' or 'volatile'
// qualification), and 0 if it is not.  For example:
//..
//  struct MyType {};
//  typedef MyType* PMT;
//
//  static const int a1 = bslmf::IsPointer<int *        >::VALUE; // a1 == 1
//  static const int a2 = bslmf::IsPointer<int *const   >::VALUE; // a2 == 1
//  static const int a3 = bslmf::IsPointer<int *volatile>::VALUE; // a3 == 1
//  static const int a4 = bslmf::IsPointer<int          >::VALUE; // a4 == 0
//  static const int a5 = bslmf::IsPointer<MyType       >::VALUE; // a5 == 0
//  static const int a6 = bslmf::IsPointer<MyType*      >::VALUE; // a6 == 1
//  static const int a7 = bslmf::IsPointer<PMT          >::VALUE; // a7 == 1
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_METAINT
#include <bslmf_metaint.h>
#endif

namespace BloombergLP {

namespace bslmf {

                         // ================
                         // struct IsPointer
                         // ================

template <typename T>
struct IsPointer : MetaInt<0>
{
    // This class implements a meta-function for checking if a type is a
    // pointer.
};

// Specializations for pointer types
template <typename T>
struct IsPointer<T*> : MetaInt<1> { };

template <typename T>
struct IsPointer<T* const> : MetaInt<1> { };

template <typename T>
struct IsPointer<T* volatile> : MetaInt<1> { };

template <typename T>
struct IsPointer<T* const volatile> : MetaInt<1> { };

}  // close package namespace

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

}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
