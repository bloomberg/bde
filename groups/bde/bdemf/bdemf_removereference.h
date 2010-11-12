// bdemf_removereference.h                -*-C++-*-
#ifndef INCLUDED_BDEMF_REMOVEREFERENCE
#define INCLUDED_BDEMF_REMOVEREFERENCE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function for removing reference-ness from types.
//
//@DEPRECATED: Use 'bslmf_removereference' instead.
//
//@CLASSES:
//    bdemf_RemoveReference: meta-function for stripping reference-ness
//
//@AUTHOR: Oleg Grunin (ogrunin)
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component defines a simple template structure used to
// strip reference-ness from its single template type parameter.  Types that
// are not reference types are unmodified.  The "de-referenced" type can be
// accessed via the 'Type' member defined in 'bdemf_RemoveReference'.
//
///Usage
///-----
// For example:
//..
//   struct MyType {};
//   typedef MyType& MyTypeRef;
//
//   bdemf_RemoveReference<int          >::Type x1;  // int
//   bdemf_RemoveReference<int&         >::Type x2;  // int
//   bdemf_RemoveReference<int volatile >::Type x3;  // volatile int
//   bdemf_RemoveReference<int volatile&>::Type x4;  // volatile int
//
//   bdemf_RemoveReference<MyType       >::Type x5;  // MyType
//   bdemf_RemoveReference<MyType&      >::Type x6;  // MyType
//   bdemf_RemoveReference<MyTypeRef    >::Type x7;  // MyType
//   bdemf_RemoveReference<MyType const >::Type x8;  // const MyType
//   bdemf_RemoveReference<MyType const&>::Type x9;  // const MyType
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVEREFERENCE
#include <bslmf_removereference.h>
#endif

#ifndef bdemf_RemoveReference
#define bdemf_RemoveReference   bslmf_RemoveReference
#endif

namespace BloombergLP {

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
