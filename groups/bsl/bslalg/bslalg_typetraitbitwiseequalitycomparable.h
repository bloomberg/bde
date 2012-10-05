// bslalg_typetraitbitwiseequalitycomparable.h                        -*-C++-*-
#ifndef INCLUDED_BSLALG_TYPETRAITBITWISEEQUALITYCOMPARABLE
#define INCLUDED_BSLALG_TYPETRAITBITWISEEQUALITYCOMPARABLE

//@PURPOSE: Provide a type trait indicating bitwise equality comparability.
//
//@CLASSES:
//  bslalg::TypeTraitBitwiseEqualityComparable: type trait metafunction
//
//@AUTHOR: Alexei Zakharov <azakharov7)
//
//@DESCRIPTION:

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_ISBITWISEEQUALITYCOMPARABLE
#include <bslmf_isbitwiseequalitycomparable.h>
#endif

namespace BloombergLP {

namespace bslalg {

              // =========================================
              // struct TypeTraitBitwiseEqualityComparable
              // =========================================

struct TypeTraitBitwiseEqualityComparable {
    // Objects of a type with this trait can be compared for equality using
    // 'memcmp'.  If two objects compare equal, then 'memcmp' returns 0,
    // otherwise it returns non-zero.  Classes with this traits are assumed to
    // have no padding and to be bit-wise copyable (see the
    // 'TypeTraitBitwiseCopyable' trait).  Undefined behavior may result if
    // this trait is assigned to a type that does not have an 'operator==' or
    // whose 'operator==' implementation may return different from comparing
    // the footprints with 'memcmp' .

    template <class TYPE>
    struct NestedTraitDeclaration :
        bslmf::NestedTraitDeclaration<TYPE, bslmf::IsBitwiseEqualityComparable>
    {
        // This class template ties the
        // 'bslalg::TypeTaitBitwiseEqualityComparable' trait tag to the
        // 'bslmf::IsBitwiseEqualityCompareble' trait metafunction.
    };

    template <class TYPE>
    struct Metafunction : bslmf::IsBitwiseEqualityComparable<TYPE>::type { };
};

}  // close package namespace

#ifndef BDE_OMIT_TRANSITIONAL  // BACKWARD_COMPATIBILITY
// ===========================================================================
//                           BACKWARD COMPATIBILITY
// ===========================================================================

typedef bslalg::TypeTraitBitwiseEqualityComparable bslalg_TypeTraitBitwiseEqualityComparable;
    // This alias is defined for backward compatibility.
#endif  // BDE_OMIT_TRANSITIONAL -- BACKWARD_COMPATIBILITY

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
