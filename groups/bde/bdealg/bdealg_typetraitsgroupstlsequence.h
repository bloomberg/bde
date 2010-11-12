// bdealg_typetraitsgroupstlsequence.h   -*-C++-*-
#ifndef INCLUDED_BDEALG_TYPETRAITSGROUPSTLSEQUENCE
#define INCLUDED_BDEALG_TYPETRAITSGROUPSTLSEQUENCE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide facilities for grouping types with compile-time traits.
//
//@DEPRECATED: Use 'bslalg_typetraitsgroupstlsequence' instead.
//
//@CLASSES:
//   bdealg_TypeTraitsGroupStlSequence: for STL sequence containers
//
//@SEE_ALSO: bslmf_typetraits
//
//@AUTHOR: Herve Bronnimann (hbronnim)
//
//@DESCRIPTION:  This component provides a facility for computing the trait
// class of STL sequence containers based on the traits of the underlying value
// type and allocator.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITSGROUPSTLSEQUENCE
#include <bslalg_typetraitsgroupstlsequence.h>
#endif

namespace BloombergLP {

                  // =======================================
                  // class bdealg_TypeTraitsGroupStlSequence
                  // =======================================

#ifndef bdealg_TypeTraitsGroupStlSequence
#define bdealg_TypeTraitsGroupStlSequence bslalg_TypeTraitsGroupStlSequence
    // Type traits for STL *sequence* containers of the parameterized type 'T'.
    // A sequence container is bitwise moveable if the allocator is bitwise
    // moveable.  It uses 'bdema' allocators if the parameterized 'ALLOCATOR'
    // is convertible from 'bdema_Allocator*'.
#endif

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
