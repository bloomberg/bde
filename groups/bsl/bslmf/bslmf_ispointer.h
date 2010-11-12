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
//  bslmf_IsPointer: meta-function for determining pointer types
//
//@AUTHOR: Shawn Edwards (sedwards)
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component defines a simple template structure used to
// evaluate whether it's parameter is a pointer.  'bslmf_IsPointer' defines a
// member, 'VALUE', whose value is initialized (at compile-time) to 1 if the
// parameter is a pointer (to anything) (ignoring any 'const' or 'volatile'
// qualification), and 0 if it is not.  For example:
//..
//   struct MyType {};
//   typedef MyType* PMT;
//
//   static const int a1 = bslmf_IsPointer<int *        >::VALUE; // a1 == 1
//   static const int a2 = bslmf_IsPointer<int *const   >::VALUE; // a2 == 1
//   static const int a3 = bslmf_IsPointer<int *volatile>::VALUE; // a3 == 1
//   static const int a4 = bslmf_IsPointer<int          >::VALUE; // a4 == 0
//   static const int a5 = bslmf_IsPointer<MyType       >::VALUE; // a5 == 0
//   static const int a6 = bslmf_IsPointer<MyType*      >::VALUE; // a6 == 1
//   static const int a7 = bslmf_IsPointer<PMT          >::VALUE; // a7 == 1
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_METAINT
#include <bslmf_metaint.h>
#endif

namespace BloombergLP {

                         // ======================
                         // struct bslmf_IsPointer
                         // ======================

template <typename T>
struct bslmf_IsPointer : bslmf_MetaInt<0>
{
    // This class implements a meta-function for checking if a type is a
    // pointer.
};

// Specializations for pointer types
template <typename T>
struct bslmf_IsPointer<T*> : bslmf_MetaInt<1> { };

template <typename T>
struct bslmf_IsPointer<T* const> : bslmf_MetaInt<1> { };

template <typename T>
struct bslmf_IsPointer<T* volatile> : bslmf_MetaInt<1> { };

template <typename T>
struct bslmf_IsPointer<T* const volatile> : bslmf_MetaInt<1> { };

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
