// bslalg_typetraitbitwiseequalitycomparable.h                        -*-C++-*-
#ifndef INCLUDED_BSLALG_TYPETRAITBITWISEEQUALITYCOMPARABLE
#define INCLUDED_BSLALG_TYPETRAITBITWISEEQUALITYCOMPARABLE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a primitive type trait for bit-wise eq.-comparable classes.
//
//@CLASSES:
//  bslalg::TypeTraitBitwiseEqualityComparable: bit-wise eq.-comparable trait
//
//@SEE_ALSO: bslmf_typetraits
//
//@AUTHOR: Herve Bronnimann (hbronnim)
//
//@DESCRIPTION: This component provides a single traits class,
// 'bslalg::TypeTraitBitwiseEqualityComparable'.  Two objects of a 'TYPE' that
// has the bit-wise equality comparable trait can be compared either by
// invoking the equality operator or by comparing the footprint (i.e., the
// 'sizeof(TYPE)' bytes at the respective object addresses) using 'memcmp'.
//
// This component is used by various 'bslalg' components for providing
// optimized primitives for types that have the bit-wise equality comparable
// trait.  The major benefit of this trait is not for a single object but for
// an array of such types, as a loop can be replaced by a single call to
// 'memcmp'.
//
///What constitutes bit-wise equality comparability?
///-------------------------------------------------
// TBD: A short guide on when to attach this trait to a class should follow.
//
///Usage
///-----
// TBD

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

namespace BloombergLP {

namespace bslalg {

              //==========================================
              // struct TypeTraitBitwiseEqualityComparable
              //==========================================

struct TypeTraitBitwiseEqualityComparable {
    // Objects of a type with this trait can be compared for equality using
    // 'memcmp'.  If two objects compare equal, then 'memcmp' returns 0,
    // otherwise it returns non-zero.  Classes with this traits are assumed to
    // have no padding and to be bit-wise copyable (see the
    // 'TypeTraitBitwiseCopyable' trait).  Undefined behavior may result if
    // this trait is assigned to a type that does not have an 'operator==' or
    // whose 'operator==' implementation may return different from comparing
    // the footprints with 'memcmp' .
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
