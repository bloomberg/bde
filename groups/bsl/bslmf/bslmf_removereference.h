// bslmf_removereference.h                                            -*-C++-*-
#ifndef INCLUDED_BSLMF_REMOVEREFERENCE
#define INCLUDED_BSLMF_REMOVEREFERENCE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function for removing reference-ness from types.
//
//@CLASSES:
//    bslmf_RemoveReference: meta-function for stripping reference-ness
//
//@AUTHOR: Oleg Grunin (ogrunin)
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component defines a simple template structure used to
// strip reference-ness from its single template type parameter.  Types that
// are not reference types are unmodified.  The "de-referenced" type can be
// accessed via the 'Type' member defined in 'bslmf_RemoveReference'.
//
///Usage
///-----
// For example:
//..
//   struct MyType {};
//   typedef MyType& MyTypeRef;
//
//   bslmf_RemoveReference<int          >::Type x1;  // int
//   bslmf_RemoveReference<int&         >::Type x2;  // int
//   bslmf_RemoveReference<int volatile >::Type x3;  // volatile int
//   bslmf_RemoveReference<int volatile&>::Type x4;  // volatile int
//
//   bslmf_RemoveReference<MyType       >::Type x5;  // MyType
//   bslmf_RemoveReference<MyType&      >::Type x6;  // MyType
//   bslmf_RemoveReference<MyTypeRef    >::Type x7;  // MyType
//   bslmf_RemoveReference<MyType const >::Type x8;  // const MyType
//   bslmf_RemoveReference<MyType const&>::Type x9;  // const MyType
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

namespace BloombergLP {

                         // ============================
                         // struct bslmf_RemoveReference
                         // ============================

template <typename TYPE>
struct bslmf_RemoveReference {
    typedef TYPE Type;
};

template <typename TYPE>
struct bslmf_RemoveReference<TYPE&> {
    typedef TYPE Type;
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
