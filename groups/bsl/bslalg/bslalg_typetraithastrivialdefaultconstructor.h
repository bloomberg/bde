// bslalg_typetraithastrivialdefaultconstructor.h                     -*-C++-*-
#ifndef INCLUDED_BSLALG_TYPETRAITHASTRIVIALDEFAULTCONSTRUCTOR
#define INCLUDED_BSLALG_TYPETRAITHASTRIVIALDEFAULTCONSTRUCTOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a primitive type trait for class with trivial default ctor.
//
//@CLASSES:
//  bslalg::TypeTraitHasTrivialDefaultConstructor: has trivial default ctor
//
//@SEE_ALSO: bslmf_typetraits
//
//@AUTHOR: Herve Bronnimann (hbronnim)
//
//@DESCRIPTION: This component provides a single traits class.
// 'bslalg::TypeTraitHasTrivialDefaultConstructor'.  An object of a 'TYPE' that
// has the trivial default constructor trait can be initialized either by
// invoking the default constructor or by filling the footprint (i.e., the
// 'sizeof(TYPE)' bytes at the object address) with 0 using 'memset'.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_HASTRIVIALDEFAULTCONSTRUCTOR
#include <bslmf_hastrivialdefaultconstructor.h>
#endif

namespace BloombergLP {

namespace bslalg {

            //=============================================
            // struct TypeTraitHasTrivialDefaultConstructor
            //=============================================

struct TypeTraitHasTrivialDefaultConstructor {
    // Objects of a type with this trait can be default-initialized by simply
    // writing zeros into the memory footprint of the object.  Although it is
    // possible for such a type not to be bitwise copyable, undefined behavior
    // may result if this trait is assigned to such a type.  (See the
    // description of 'TypeTraitBitwiseCopyable'.)

    template <class TYPE>
    struct NestedTraitDeclaration :
        bslmf::NestedTraitDeclaration<TYPE, bslmf::HasTrivialDefaultConstructor>
    {
        // This class template ties the 'bslalg::TypeTaitBitwiseEqualityComparable'
        // trait tag to the 'bslmf::IsBitwiseEqualityCompareble' trait metafunction.
    };

    template <class TYPE>
    struct Metafunction : bslmf::HasTrivialDefaultConstructor<TYPE>::type { };
};

}  // close package namespace

// ===========================================================================
//                           BACKWARD COMPATIBILITY
// ===========================================================================

typedef bslalg::TypeTraitHasTrivialDefaultConstructor bslalg_TypeTraitHasTrivialDefaultConstructor;
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
