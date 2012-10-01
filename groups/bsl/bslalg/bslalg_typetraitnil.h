// bslalg_typetraitnil.h                                              -*-C++-*-
#ifndef INCLUDED_BSLALG_TYPETRAITNIL
#define INCLUDED_BSLALG_TYPETRAITNIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a primitive trait for classes that have no other traits.
//
//@CLASSES:
//  bslalg::TypeTraitBitwiseCopyable: bit-wise copyable trait
//
//@SEE_ALSO: bslmf_typetraits
//
//@AUTHOR: Herve Bronnimann (hbronnim)
//
//@DESCRIPTION: This component provides a single traits class,
// 'bslalg::TypeTraitNil'.  This trait is assigned by default to any class that
// does not have any other trait.
//
///Usage
///-----
// TBD

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_NIL
#include <bslmf_nil.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

namespace BloombergLP {

namespace bslalg {

                            //=============
                            // TypeTraitNil
                            //=============

struct TypeTraitNil
{
    // Nil trait -- every type has this trait.

    template <class TYPE>
    struct NestedTraitDeclaration
    {
        // This metafunction returns 'true_type' for any class that is queried
        // for the nil trait.
    };

    template <class TYPE>
    struct Metafunction : bsl::true_type { };
};

}  // close package namespace

#ifndef BDE_OMIT_TRANSITIONAL  // BACKWARD_COMPATIBILITY
// ===========================================================================
//                           BACKWARD COMPATIBILITY
// ===========================================================================

#ifdef bslalg_TypeTraitNil
#undef bslalg_TypeTraitNil
#endif
#define bslalg_TypeTraitNil bslalg::TypeTraitNil
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
