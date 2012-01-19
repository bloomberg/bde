// bslalg_typetraithaspointersemantics.h                              -*-C++-*-
#ifndef INCLUDED_BSLALG_TYPETRAITHASPOINTERSEMANTICS
#define INCLUDED_BSLALG_TYPETRAITHASPOINTERSEMANTICS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a primitive type trait for classes with pointer semantics.
//
//@CLASSES:
//  bslalg::TypeTraitHasPointerSemantics: has pointer semantics
//
//@SEE_ALSO: bslmf_typetraits
//
//@AUTHOR: Herve Bronnimann (hbronnim)
//
//@DESCRIPTION: This component provides a single traits class,
// 'bslalg::TypeTraitHasPointerSemantics'.  A type with this trait is said to
// have "pointer semantics".  That is the type behaves as if it were a
// fundamental pointer type.  The type must define(at a minimum) 'operator*'
// and 'operator->'.
//
///Usage
///-----
// TBD

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

namespace BloombergLP {

namespace bslalg {

                 //====================================
                 // struct TypeTraitHasPointerSemantics
                 //====================================

struct TypeTraitHasPointerSemantics {
    // A type with this trait is said to have "pointer semantics".  That is the
    // type behaves as if it were a fundamental pointer type.  The type must
    // define(at a minimum) 'operator*' and 'operator->'.  Note that simply
    // providing the above operators does not imply that a type has pointer
    // semantics.  This trait is intended for "wrapper" types that behave as
    // pointers.  Some examples of such types include 'std::auto_ptr', and
    // 'bslma::ManagedPtr'.  This trait is generally used by objects that
    // invoke the wrapped type.
};

}  // close package namespace

}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
