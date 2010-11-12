// bdealg_typetraitsgroupstlordered.h   -*-C++-*-
#ifndef INCLUDED_BDEALG_TYPETRAITSGROUPSTLORDERED
#define INCLUDED_BDEALG_TYPETRAITSGROUPSTLORDERED

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide facilities for grouping types with compile-time traits.
//
//@DEPRECATED: Use 'bslalg_typetraitsgroupstlordered' instead.
//
//@CLASSES:
//    bdealg_TypeTraitsGroupStlOrdered: for STL ordered containers
//
//@SEE_ALSO: bslmf_typetraits
//
//@AUTHOR: Herve Bronnimann (hbronnim)
//
//@DESCRIPTION:  This component provides a facility for computing the trait
// class of STL ordered containers based on the traits of the underlying value
// type and allocator.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITSGROUPSTLORDERED
#include <bslalg_typetraitsgroupstlordered.h>
#endif

namespace BloombergLP {

                  // ======================================
                  // class bdealg_TypeTraitsGroupStlOrdered
                  // ======================================

#ifndef bdealg_TypeTraitsGroupStlOrdered
#define bdealg_TypeTraitsGroupStlOrdered bslalg_TypeTraitsGroupStlOrdered
    // Type traits for STL *ordered* containers of the parameterized type 'T'.
    // An ordered container is bitwise moveable if the both the comparison
    // functor and allocator are bitwise moveable.  It uses 'bdema' allocators
    // if 'ALLOCATOR' is convertible from 'bdema_Allocator*'.
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
