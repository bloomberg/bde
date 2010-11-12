// bdealg_typetraitbitwiseequalitycomparable.h    -*-C++-*-
#ifndef INCLUDED_BDEALG_TYPETRAITBITWISEEQUALITYCOMPARABLE
#define INCLUDED_BDEALG_TYPETRAITBITWISEEQUALITYCOMPARABLE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a primitive type trait for bit-wise eq.-comparable classes.
//
//@DEPRECATED: Use 'bslalg_typetraitbitwiseequalitycomparable' instead.
//
//@CLASSES:
//  bdealg_TypeTraitBitwiseEqualityComparable: bit-wise eq.-comparable trait
//
//@SEE_ALSO: bslmf_typetraits
//
//@AUTHOR: Herve Bronnimann (hbronnim)
//
//@DESCRIPTION:  This component provides a single traits class,
// 'bdealg_TypeTraitBitwiseEqualityComparable'.  Two instances of a 'TYPE' that
// has the bit-wise equality comparable trait can be compared either by
// invoking the equality operator or by comparing the footprint (i.e., the
// 'sizeof(TYPE)' bytes at the respective object addresses) using 'memcmp'.
//
// This component is used by various 'bdealg' components for providing
// optimized primitives for types that have the bit-wise equality comparable
// trait.  The major benefit of this trait is not for a single instance but for
// an array of such types, as a loop can be replaced by a single call to
// 'memcmp'.
//
///What constitutes bit-wise equality comparability?
///-------------------------------------------------
// TBD  A short guide on when to attach this trait to a class should follow.
//
///Usage
///-----
// TBD

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITBITWISEEQUALITYCOMPARABLE
#include <bslalg_typetraitbitwiseequalitycomparable.h>
#endif

namespace BloombergLP {

              //=================================================
              // struct bdealg_TypeTraitBitwiseEqualityComparable
              //=================================================

typedef bslalg_TypeTraitBitwiseEqualityComparable
                                     bdealg_TypeTraitBitwiseEqualityComparable;
    // Objects of a type with this trait can be compared for equality using
    // 'memcmp'.  If two instances compare equal, then 'memcmp' returns 0,
    // otherwise it returns non-zero.  Classes with this traits are assumed to
    // have no padding and to be bit-wise copyable (see the
    // 'bdealg_TypeTraitBitwiseCopyable' trait).  Undefined behavior may result
    // if this trait is assigned to a type that does not have an 'operator=='
    // or whose 'operator==' implementation may return different from comparing
    // the footprints with 'memcmp' .

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
