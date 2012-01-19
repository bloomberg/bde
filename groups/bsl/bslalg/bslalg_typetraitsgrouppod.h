// bslalg_typetraitsgrouppod.h                                        -*-C++-*-
#ifndef INCLUDED_BSLALG_TYPETRAITSGROUPPOD
#define INCLUDED_BSLALG_TYPETRAITSGROUPPOD

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide pre-grouped compile-time traits for POD classes.
//
//@CLASSES:
//  bslalg::TypeTraitsGroupPod: for POD types
//
//@SEE_ALSO: bslmf_typetraits
//
//@AUTHOR: Herve Bronnimann (hbronnim)
//
//@DESCRIPTION: This component provides an aggregate traits class, suitable to
// attach to POD types.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITBITWISECOPYABLE
#include <bslalg_typetraitbitwisecopyable.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITBITWISEMOVEABLE
#include <bslalg_typetraitbitwisemoveable.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITHASTRIVIALDEFAULTCONSTRUCTOR
#include <bslalg_typetraithastrivialdefaultconstructor.h>
#endif

namespace BloombergLP {

namespace bslalg {

                      // ========================
                      // class TypeTraitsGroupPod
                      // ========================

struct TypeTraitsGroupPod : TypeTraitBitwiseCopyable,
                                   TypeTraitBitwiseMoveable,
                                   TypeTraitHasTrivialDefaultConstructor
{
    // This type groups together all of the traits of a POD (Plain Old Data)
    // type.  Authors of new POD types (C-style 'structs') can assign traits to
    // the new type using this group rather than assigning each individual
    // trait separately.
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
