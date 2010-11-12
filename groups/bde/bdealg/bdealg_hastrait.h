// bdealg_hastrait.h   -*-C++-*-
#ifndef INCLUDED_BDEALG_HASTRAIT
#define INCLUDED_BDEALG_HASTRAIT

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide facilities for checking compile-time trait.
//
//@DEPRECATED: Use 'bslalg_hastrait' instead.
//
//@CLASSES:
//  bdealg_HasTrait: trait detection mechanism
//
//@SEE_ALSO: bslmf_typetraits
//
//@AUTHOR: Pablo Halpern (phalpern), Herve Bronnimann (hbronnim)
//
//@DESCRIPTION:  This component provides a meta-function, 'bdealg_HasTrait',
// for macros used to assign traits to user-defined class.  Traits are used to
// enable certain optimizations or discriminations at compile-time.  For
// instance, a class having the trait 'bdealg_TypeTraitBitwiseMoveable' may
// allow resizing an array of instances by simply calling 'bsl::memcpy' instead
// of invoking a copy-constructor on every instance.  The usage example shows
// how to use the 'bdealg_TypeTraitUsesBdemaAllocator' to propagate allocators
// to nested instances that may require them.
//
// This component should be used in conjunction with other components from
// the package 'bdealg'.  See the package-level documentation for an overview.
// The most useful classes and macros defined in this component are:
//..
//  bdealg_HasTrait<TYPE, TRAIT>              This meta-function computes
//                                            whether the parameterized 'TYPE'
//                                            possesses the parameterized
//                                            'TRAIT'.
//..
//
///Usage
///-----

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLALG_HASTRAIT
#include <bslalg_hastrait.h>
#endif

#ifndef bdealg_HasTrait
#define bdealg_HasTrait bslalg_HasTrait
    // This meta-function evaluates to 'bslmf_MetaInt<1>' if the parameterized
    // type 'TYPE' has the parameterized 'TRAIT', and to 'bslmf_MetaInt<0>'
    // otherwise.  Note that 'TYPE' "has" the 'TRAIT' if
    // 'bdealg_TypeTraits<TYPE>' directly includes 'TRAIT' or else includes a
    // trait that implies 'TRAIT'.
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
