// bdealg_typetraitsgroupstlunordered.h   -*-C++-*-
#ifndef INCLUDED_BDEALG_TYPETRAITSGROUPSTLUNORDERED
#define INCLUDED_BDEALG_TYPETRAITSGROUPSTLUNORDERED

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide facilities for grouping types with compile-time traits.
//
//@DEPRECATED: Use 'bslalg_typetraitsgroupstlunordered' instead.
//
//@CLASSES:
//   bdealg_TypeTraitsGroupStlSequence: for STL sequence containers
//    bdealg_TypeTraitsGroupStlOrdered: for STL ordered containers
//  bdealg_TypeTraitsGroupStlUnordered: for STL unordered containers
//
//@SEE_ALSO: bslmf_typetraits
//
//@AUTHOR: Herve Bronnimann (hbronnim)
//
//@DESCRIPTION:  This component provides several traits classes, which compute
// the traits for POD types, and for STL containers based on properties of the
// underlying value type and allocator.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITSGROUPSTLUNORDERED
#include <bslalg_typetraitsgroupstlunordered.h>
#endif

namespace BloombergLP {

                  // ========================================
                  // class bdealg_TypeTraitsGroupStlUnordered
                  // ========================================

#ifndef bdealg_TypeTraitsGroupStlUnordered
#define bdealg_TypeTraitsGroupStlUnordered bslalg_TypeTraitsGroupStlUnordered
    // Type traits for STL *unordered* containers.  An unordered container is
    // bitwise moveable if the hash functor, equality functor, and allocator
    // are all bitwise moveable.  It uses 'bdema' allocators if 'ALLOCATOR' is
    // convertible from 'bdema_Allocator*'.
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
