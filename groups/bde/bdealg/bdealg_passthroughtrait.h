// bdealg_passthroughtrait.h   -*-C++-*-
#ifndef INCLUDED_BDEALG_PASSTHROUGHTRAIT
#define INCLUDED_BDEALG_PASSTHROUGHTRAIT

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide facilities for computing compile-time traits.
//
//@DEPRECATED: Use 'bslalg_passthroughtrait' instead.
//
//@CLASSES:
//                bdealg_PassthroughTrait: pass-through trait mechanism
//  bdealg_PassthroughTraitBdemaAllocator: pass-through for 'bdema' allocators
//
//@SEE_ALSO: bslmf_typetraits
//
//@AUTHOR: Pablo Halpern (phalpern), Herve Bronnimann (hbronnim)
//
//@DESCRIPTION:  This component provides a meta-function,
// 'bdealg_PassthroughTrait', for constructing a trait class that has a
// parameterized 'TRAIT' if a parameterized 'TYPE' has the 'TRAIT'.
//
// A similar meta-function, 'bdealg_PassthroughTraitBdemaAllocator', allows to
// compute the 'bdealg_TypeTraitUsesBdemaAllocator' trait for classes that have
// a parameterized 'ALLOCATOR' member.  This is most useful for STL containers.
//
///Usage
///-----
// TBD

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLALG_PASSTHROUGHTRAIT
#include <bslalg_passthroughtrait.h>
#endif

namespace BloombergLP {
                       // ==============================
                       // struct bdealg_PassthroughTrait
                       // ==============================

#ifndef bdealg_PassthroughTrait
#define bdealg_PassthroughTrait bslalg_PassthroughTrait
    // If 'T' has 'TRAIT', then evaluate to 'TRAIT', else evaluate to a unique
    // class that is not 'TRAIT'.  Users of this meta-function do not need to
    // expand the result with '::Type' (though they may).
#endif

               // ============================================
               // struct bdealg_PassthroughTraitBdemaAllocator
               // ============================================

#ifndef bdealg_PassthroughTraitBdemaAllocator
#define bdealg_PassthroughTraitBdemaAllocator \
                                          bslalg_PassthroughTraitBdemaAllocator
#endif

// BACKWARDS COMPATIBILITY
#ifndef bdealg_Passthrough_BdemaAllocator
#define bdealg_Passthrough_BdemaAllocator \
                                          bdealg_PassthroughTraitBdemaAllocator
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
