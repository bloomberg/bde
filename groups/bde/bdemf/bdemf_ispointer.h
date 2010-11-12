// bdemf_ispointer.h                -*-C++-*-
#ifndef INCLUDED_BDEMF_ISPOINTER
#define INCLUDED_BDEMF_ISPOINTER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for pointer types.
//
//@DEPRECATED: Use 'bslmf_ispointer' instead.
//
//@CLASSES:
//  bdemf_IsPointer: meta-function for determining pointer types
//
//@AUTHOR: Shawn Edwards (sedwards)
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component defines a simple template structure used to
// evaluate whether it's parameter is a pointer.  'bdemf_IsPointer' defines a
// member, 'VALUE', whose value is initailized (at compile-time) to 1 if the
// parameter is a pointer (to anyting) (ignoring any 'const' or 'volatile'
// qualification), and 0 if it is not.  For example:
//..
//   struct MyType {};
//   typedef MyType* PMT;
//
//   static const int a1 = bdemf_IsPointer<int *        >::VALUE; // a1 == 1
//   static const int a2 = bdemf_IsPointer<int *const   >::VALUE; // a2 == 1
//   static const int a3 = bdemf_IsPointer<int *volatile>::VALUE; // a3 == 1
//   static const int a4 = bdemf_IsPointer<int          >::VALUE; // a4 == 0
//   static const int a5 = bdemf_IsPointer<MyType       >::VALUE; // a5 == 0
//   static const int a6 = bdemf_IsPointer<MyType*      >::VALUE; // a6 == 1
//   static const int a7 = bdemf_IsPointer<PMT          >::VALUE; // a7 == 1
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLMF_ISPOINTER
#include <bslmf_ispointer.h>
#endif

#ifndef bdemf_IsPointer
#define bdemf_IsPointer   bslmf_IsPointer
    // This class implements a meta-function for checking if a type is
    // a pointer.
#endif

namespace BloombergLP {

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
