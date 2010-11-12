// bdealg_typetraitnil.h   -*-C++-*-
#ifndef INCLUDED_BDEALG_TYPETRAITNIL
#define INCLUDED_BDEALG_TYPETRAITNIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a primitive type trait for bit-wise copyable classes.
//
//@DEPRECATED: Use 'bslalg_typetraitnil' instead.
//
//@CLASSES:
//  bdealg_TypeTraitBitwiseCopyable: bit-wise copyable trait
//
//@SEE_ALSO: bslmf_typetraits
//
//@AUTHOR: Herve Bronnimann (hbronnim)
//
//@DESCRIPTION:  This component provides a single traits class,
// 'bdealg_TypeTraitNil'.  This trait is assigned by default to any class that
// does not have any other trait.
//
///Usage
///-----
// TBD

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITNIL
#include <bslalg_typetraitnil.h>
#endif

namespace BloombergLP {

                            //====================
                            // bdealg_TypeTraitNil
                            //====================

typedef bslalg_TypeTraitNil bdealg_TypeTraitNil;
    // The nil trait.  Types with no other traits have the nil trait.

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
