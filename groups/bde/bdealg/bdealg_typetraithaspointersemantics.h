// bdealg_typetraithaspointersemantics.h   -*-C++-*-
#ifndef INCLUDED_BDEALG_TYPETRAITHASPOINTERSEMANTICS
#define INCLUDED_BDEALG_TYPETRAITHASPOINTERSEMANTICS

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a primitive type trait for classes with pointer semantics.
//
//@DEPRECATED: Use 'bslalg_typetraithaspointersemantics' instead.
//
//@CLASSES:
//  bdealg_TypeTraitHasPointerSemantics: has pointer semantics
//
//@SEE_ALSO: bslmf_typetraits
//
//@AUTHOR: Herve Bronnimann (hbronnim)
//
//@DESCRIPTION:  This component provides a single traits class,
// 'bdealg_TypeTraitHasPointerSemantics'.  A type with this trait is said to
// have "pointer semantics".  That is the type behaves as if it were a
// fundamental pointer type.  The type must define(at a minimum) 'operator*'
// and 'operator->'.
//
///Usage
///-----
// TBD

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITHASPOINTERSEMANTICS
#include <bslalg_typetraithaspointersemantics.h>
#endif

namespace BloombergLP {

                 //===========================================
                 // struct bdealg_TypeTraitHasPointerSemantics
                 //===========================================

typedef bslalg_TypeTraitHasPointerSemantics
                                           bdealg_TypeTraitHasPointerSemantics;
    // A type with this trait is said to have "pointer semantics".  That is the
    // type behaves as if it were a fundamental pointer type.  The type must
    // define(at a minimum) 'operator*' and 'operator->'.  Note that simply
    // providing the above operators does not imply that a type has pointer
    // semantics.  This trait is intended for "wrapper" types that behave as
    // pointers.  Some examples of such types include 'bsl::auto_ptr', and
    // 'bdema_ManagedPtr'.  This trait is generally used by objects that invoke
    // the wrapped type.

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
