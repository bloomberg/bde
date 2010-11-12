// bdemf_anytype.h                  -*-C++-*-
#ifndef INCLUDED_BDEMF_ANYTYPE
#define INCLUDED_BDEMF_ANYTYPE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a type that is convertible from any type.
//
//@DEPRECATED: Use 'bslmf_anytype' instead.
//
//@CLASSES:
//   bdemf_AnyType: generic type to which any type can be converted
//   bdemf_TypeRep: meta-function for providing a reference to 'TYPE'
//
//@SEE_ALSO:
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION:
//
///Usage
///-----
//

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLMF_ANYTYPE
#include <bslmf_anytype.h>
#endif

#ifndef bdemf_AnyType
#define bdemf_AnyType  bslmf_AnyType
    // Any type can be converted into this type.
#endif

#ifndef bdemf_TypeRep
#define bdemf_TypeRep  bslmf_TypeRep
    // Generate a reference to 'TYPE' for use in meta-functions.
#endif

namespace BloombergLP {

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
