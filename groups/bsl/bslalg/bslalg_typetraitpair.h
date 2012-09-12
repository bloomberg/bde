// bslalg_typetraitpair.h                                             -*-C++-*-
#ifndef INCLUDED_BSLALG_TYPETRAITPAIR
#define INCLUDED_BSLALG_TYPETRAITPAIR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide facilities for identifying 'std::pair'-like classes.
//
//@INTERNAL_DEPRECATED: Do not use.
//
//@CLASSES:
//  bslalg::TypeTraitPair: for 'std::pair'-like classes
//
//@SEE_ALSO: bslalg_typetraits
//
//@AUTHOR: Herve Bronnimann (hbronnim)
//
//@DESCRIPTION: This component provides a single traits class,
// 'bslalg::TypeTraitPair'.  A 'TYPE' that has this trait fulfills the
// following requirements, where 'mX' is a modifiable object and 'X' a
// non-modifiable object of 'TYPE':
//..
//  Valid expression     Type
//  ----------------     ----
//  TYPE::first_type
//  TYPE::second_type
//
//  mX.first             first_type
//  mX.second            second
//  X.first              const first_type
//  X.second             const second_type
//..
// Note that 'first' and 'second' are *not* member functions, but data members.

#ifdef BDE_OSS_TEST
#error "bslalg_typetraitpair is deprecated"
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

namespace BloombergLP {

namespace bslalg {

                        // ====================
                        // struct TypeTraitPair
                        // ====================

struct TypeTraitPair {
    // A type, 'T', with this trait has two data members, 'first' and 'second'
    // of types 'T::first_type' and 'T::second_type', respectively.
    // Metafunctions can be used to probe and combine the traits of the
    // individual pair members.  For example, the pair is bitwise moveable only
    // if both 'first_type' and 'second_type' have the
    // 'TypeTraitBitwiseMoveable' trait.  User-defined types will rarely need
    // this trait.
};

}  // close package namespace

#ifndef BDE_OMIT_TRANSITIONAL  // BACKWARD_COMPATIBILITY
// ===========================================================================
//                           BACKWARD COMPATIBILITY
// ===========================================================================

typedef bslalg::TypeTraitPair bslalg_TypeTraitPair;
    // This alias is defined for backward compatibility.
#endif  // BDE_OMIT_TRANSITIONAL -- BACKWARD COMPATIBILITY

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
