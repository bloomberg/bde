// bslalg_typetraitsgroupstlordered.h                                 -*-C++-*-
#ifndef INCLUDED_BSLALG_TYPETRAITSGROUPSTLORDERED
#define INCLUDED_BSLALG_TYPETRAITSGROUPSTLORDERED

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide facilities for grouping types with compile-time traits.
//
//@CLASSES:
//  bslalg::TypeTraitsGroupStlOrdered: for STL ordered containers
//
//@SEE_ALSO: bslmf_typetraits
//
//@AUTHOR: Herve Bronnimann (hbronnim)
//
//@DESCRIPTION: This component provides a facility for computing the trait
// class of STL ordered containers based on the traits of the underlying value
// type and allocator.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_HASTRAIT
#include <bslalg_hastrait.h>
#endif

#ifndef INCLUDED_BSLALG_PASSTHROUGHTRAIT
#include <bslalg_passthroughtrait.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITBITWISEMOVEABLE
#include <bslalg_typetraitbitwisemoveable.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITHASSTLITERATORS
#include <bslalg_typetraithasstliterators.h>
#endif

namespace BloombergLP {

namespace bslalg {

                  // ===============================
                  // class TypeTraitsGroupStlOrdered
                  // ===============================

template <typename T, typename COMP, typename ALLOCATOR>
struct TypeTraitsGroupStlOrdered :
    TypeTraitHasStlIterators,
    bslmf::If<HasTrait<COMP,TypeTraitBitwiseMoveable>::VALUE &&
              HasTrait<ALLOCATOR,TypeTraitBitwiseMoveable>::VALUE,
              TypeTraitBitwiseMoveable,
              TypeTraits_NotTrait<TypeTraitBitwiseMoveable>
             >::Type,
    PassthroughTraitBslmaAllocator<ALLOCATOR> {
    // Type traits for STL *ordered* containers of the parameterized type 'T'.
    // An ordered container is bitwise moveable if the both the comparison
    // functor and allocator are bitwise moveable.  It uses 'bslma' allocators
    // if 'ALLOCATOR' is convertible from 'bslma::Allocator*'.
};

}  // close package namespace

// ===========================================================================
//                           BACKWARD COMPATIBILITY
// ===========================================================================

#ifdef bslalg_TypeTraitsGroupStlOrdered
#undef bslalg_TypeTraitsGroupStlOrdered
#endif
#define bslalg_TypeTraitsGroupStlOrdered bslalg::TypeTraitsGroupStlOrdered
    // This alias is defined for backward compatibility.

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
