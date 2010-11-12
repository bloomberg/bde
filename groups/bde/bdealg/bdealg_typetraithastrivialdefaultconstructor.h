// bdealg_typetraithastrivialdefaultconstructor.h   -*-C++-*-
#ifndef INCLUDED_BDEALG_TYPETRAITHASTRIVIALDEFAULTCONSTRUCTOR
#define INCLUDED_BDEALG_TYPETRAITHASTRIVIALDEFAULTCONSTRUCTOR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a primitive type trait for class with trivial default ctor.
//
//@DEPRECATED: Use 'bslalg_typetraithastrivialdefaultconstructor' instead.
//
//@CLASSES:
//  bdealg_TypeTraitHasTrivialDefaultConstructor: has trivial default ctor
//
//@SEE_ALSO: bslmf_typetraits
//
//@AUTHOR: Herve Bronnimann (hbronnim)
//
//@DESCRIPTION:  This component provides a single traits class,
// 'bdealg_TypeTraitHasTrivialDefaultConstructor'.  An instance of a 'TYPE'
// that has the trivial default constructor trait can be initialized either by
// invoking the default constructor or by filling the footprint (i.e., the
// 'sizeof(TYPE)' bytes at the object address) with 0 using 'memset'.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITHASTRIVIALDEFAULTCONSTRUCTOR
#include <bslalg_typetraithastrivialdefaultconstructor.h>
#endif

namespace BloombergLP {

            //====================================================
            // struct bdealg_TypeTraitHasTrivialDefaultConstructor
            //====================================================

typedef bslalg_TypeTraitHasTrivialDefaultConstructor
                                  bdealg_TypeTraitHasTrivialDefaultConstructor;
    // Objects of a type with this trait can be default-initialized by simply
    // writing zeros into the memory footprint of the object.  Although it is
    // possible for such a type not to be bitwise copyable, undefined behavior
    // may result if this trait is assigned to such a type.  (See the
    // description of 'bdealg_TypeTraitBitwiseCopyable'.)

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
