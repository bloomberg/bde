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
//  bslmf::RemoveReference: meta-function for stripping reference-ness
//
//@AUTHOR: Oleg Grunin (ogrunin)
//
//@DESCRIPTION: This component defines a simple template structure used to
// strip reference-ness from its single template type parameter.  Types that
// are not reference types are unmodified.  The "de-referenced" type can be
// accessed via the 'Type' member defined in 'bslmf::RemoveReference'.
//
///Usage
///-----
// For example:
//..
//  struct MyType {};
//  typedef MyType& MyTypeRef;
//
//  bslmf::RemoveReference<int          >::Type x1;  // int
//  bslmf::RemoveReference<int&         >::Type x2;  // int
//  bslmf::RemoveReference<int volatile >::Type x3;  // volatile int
//  bslmf::RemoveReference<int volatile&>::Type x4;  // volatile int
//
//  bslmf::RemoveReference<MyType       >::Type x5;  // MyType
//  bslmf::RemoveReference<MyType&      >::Type x6;  // MyType
//  bslmf::RemoveReference<MyTypeRef    >::Type x7;  // MyType
//  bslmf::RemoveReference<MyType const >::Type x8;  // const MyType
//  bslmf::RemoveReference<MyType const&>::Type x9;  // const MyType
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

namespace BloombergLP {

namespace bslmf {

                         // ======================
                         // struct RemoveReference
                         // ======================

template <typename TYPE>
struct RemoveReference {
    typedef TYPE Type;
};

template <typename TYPE>
struct RemoveReference<TYPE&> {
    typedef TYPE Type;
};

}  // close package namespace

#ifndef BDE_OMIT_TRANSITIONAL  // BACKWARD_COMPATIBILITY
// ===========================================================================
//                           BACKWARD COMPATIBILITY
// ===========================================================================

#ifdef bslmf_RemoveReference
#undef bslmf_RemoveReference
#endif
#define bslmf_RemoveReference bslmf::RemoveReference
    // This alias is defined for backward compatibility.
#endif  // BDE_OMIT_TRANSITIONAL -- BACKWARD COMPATIBILITY

}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
