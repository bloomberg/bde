// bdealg_selecttrait.h   -*-C++-*-
#ifndef INCLUDED_BDEALG_SELECTTRAIT
#define INCLUDED_BDEALG_SELECTTRAIT

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide facilities for selecting compile-time trait.
//
//@DEPRECATED: Use 'bslalg_selecttrait' instead.
//
//@CLASSES:
//  bdealg_SelectTrait: trait detection mechanism
//
//@SEE_ALSO: bslmf_typetraits
//
//@AUTHOR: Pablo Halpern (phalpern), Herve Bronnimann (hbronnim)
//
//@DESCRIPTION:  This component provides a meta-function, 'bdealg_SelectTrait',
// for selecting the most appropriate trait from a list of candidate traits
// for parameterized 'TYPE'.
//
///Usage
///-----

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLALG_SELECTTRAIT
#include <bslalg_selecttrait.h>
#endif

#ifndef bdealg_SelectTrait
#define bdealg_SelectTrait bslalg_SelectTrait
    // Select one trait out of several that the parameterized type 'T' may
    // possess.  If 'T' has the parameterized trait 'TRAIT1', then the nested
    // 'Type' is 'TRAIT1', else if 'T' has an optionally parameterized
    // 'TRAIT2', then 'Type' is 'TRAIT2', etc.  Also computes an integral
    // selection constant and meta-value.  If 'T' has none of the parameterized
    // 'TRAIT*', then the nested 'Type' is 'bdealg_TypeTraitNil'.
#endif

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
