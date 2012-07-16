// bslstl_traitsgroupstlunorderedcontainer.h                          -*-C++-*-
#ifndef INCLUDED_BSLSTL_TRAITSGROUPSTLUNORDEREDCONTAINER
#define INCLUDED_BSLSTL_TRAITSGROUPSTLUNORDEREDCONTAINER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide facilities for grouping types with compile-time traits.
//
//@CLASSES:
//   bslstl::TraitsGroupStlUnorderedContainer: for STL unordered containers
//
//@SEE_ALSO: bslalg_typetraits
//
//@AUTHOR: Herve Bronnimann (hbronnim)
//
//@DESCRIPTION: This component provides several traits classes, which compute
// the traits for POD types, and for STL containers based on properties of the
// underlying value type and allocator.

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

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

namespace BloombergLP {
namespace bslstl {

                  // ======================================
                  // class TraitsGroupStlUnorderedContainer
                  // ======================================

template <typename T, typename HASH, typename EQ, typename ALLOCATOR>
struct TraitsGroupStlUnorderedContainer :
    bslalg::TypeTraitHasStlIterators,
    bslmf::If<
          bslalg::HasTrait<HASH, bslalg::TypeTraitBitwiseMoveable>::VALUE &&
          bslalg::HasTrait<EQ, bslalg::TypeTraitBitwiseMoveable>::VALUE &&
          bslalg::HasTrait<ALLOCATOR, bslalg::TypeTraitBitwiseMoveable>::VALUE,
          bslalg::TypeTraitBitwiseMoveable,
          bslalg::TypeTraits_NotTrait<bslalg::TypeTraitBitwiseMoveable>
    >::Type,
    bslalg::PassthroughTraitBslmaAllocator<ALLOCATOR> {
    // Type traits for STL *unordered* containers.  An unordered container is
    // bitwise moveable if the hash functor, equality functor, and allocator
    // are all bitwise moveable.  It uses 'bslma' allocators if 'ALLOCATOR' is
    // convertible from 'bslma::Allocator*'.
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
