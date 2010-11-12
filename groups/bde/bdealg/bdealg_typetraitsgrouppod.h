// bdealg_typetraitsgrouppod.h   -*-C++-*-
#ifndef INCLUDED_BDEALG_TYPETRAITSGROUPPOD
#define INCLUDED_BDEALG_TYPETRAITSGROUPPOD

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide pre-grouped compile-time traits for POD classes.
//
//@DEPRECATED: Use 'bslalg_typetraitsgrouppod' instead.
//
//@CLASSES:
//   bdealg_TypeTraitsGroupPod: for POD types
//
//@SEE_ALSO: bslmf_typetraits
//
//@AUTHOR: Herve Bronnimann (hbronnim)
//
//@DESCRIPTION:  This component provides an aggregate traits class, suitable to
// attach to POD types.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITSGROUPPOD
#include <bslalg_typetraitsgrouppod.h>
#endif

namespace BloombergLP {

                      // ===============================
                      // class bdealg_TypeTraitsGroupPod
                      // ===============================

#ifndef bdealg_TypeTraitsGroupPod
#define bdealg_TypeTraitsGroupPod bslalg_TypeTraitsGroupPod
    // This type groups together all of the traits of a POD (Plain Old Data)
    // type.  Authors of new POD types (C-style 'structs') can assign traits
    // to the new type using this group rather than assigning each individual
    // trait separately.
#endif

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
