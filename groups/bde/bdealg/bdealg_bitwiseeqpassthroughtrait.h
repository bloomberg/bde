// bdealg_bitwiseeqpassthroughtrait.h   -*-C++-*-
#ifndef INCLUDED_BDEALG_BITWISEEQPASSTHROUGHTRAIT
#define INCLUDED_BDEALG_BITWISEEQPASSTHROUGHTRAIT

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide facilities for computing compile-time traits.
//
//@DEPRECATED: Use 'bslalg_bitwiseeqpassthroughtrait' instead.
//
//@CLASSES:
//  bdealg_BitwiseEqPassthroughTrait: pass-through trait mechanism
//
//@SEE_ALSO: bslmf_typetraits
//
//@AUTHOR: Herve Bronnimann (hbronnim)
//
//@DESCRIPTION:  This component provides a meta-function,
// 'bdealg_BitwiseEqPassthroughTrait', that allows to compute the
// 'bdealg_TypeTraitBitwiseEqualityComparable' trait for classes knowing the
// types of its members.
//
///Usage
///-----
// TBD

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLALG_BITWISEEQPASSTHROUGHTRAIT
#include <bslalg_bitwiseeqpassthroughtrait.h>
#endif

namespace BloombergLP {


                   // ======================================
                   // struct bdealg_BitwiseEqPasstroughTrait
                   // ======================================

#define bdealg_BitwiseEqPassthroughTrait bslalg_BitwiseEqPassthroughTrait
    // If the parameterized 'A1' up to 'A9' types have the
    // 'bdealg_TypeTraitBitwiseEqualityComparable' trait, and if a structure
    // packed with these types (excluding the non-specified types) has the same
    // size as the sum of the sizes of each type (i.e., there is no packing),
    // then evaluate to 'bdealg_TypeTraitBitwiseEqualityComparable', else
    // evaluate to a unique class that is not this trait.  Users of this
    // meta-function do not need to expand the result with '::Type' (though
    // they may).

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
