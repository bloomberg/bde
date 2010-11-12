// bdealg_typetraitbitwisemoveable.h   -*-C++-*-
#ifndef INCLUDED_BDEALG_TYPETRAITBITWISEMOVEABLE
#define INCLUDED_BDEALG_TYPETRAITBITWISEMOVEABLE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a primitive type trait for bit-wise moveable classes.
//
//@DEPRECATED: Use 'bslalg_typetraitbitwisemoveable' instead.
//
//@CLASSES:
//  bdealg_TypeTraitBitwiseMoveable: bit-wise moveable trait
//
//@SEE_ALSO: bslmf_typetraits
//
//@AUTHOR: Herve Bronnimann (hbronnim)
//
//@DESCRIPTION:  This component provides a single traits class,
// 'bdealg_TypeTraitBitwiseMoveable'.  An instance of a 'TYPE' that has
// the bit-wise moveable trait can be copied either by invoking the move
// constructor or by moving the footprint (i.e., the 'sizeof(TYPE)' bytes at
// the object address) using 'memcpy'.
//
// This component is used by various 'bdealg' components for providing
// optimized primitives for types that have the bit-wise moveable trait.  The
// major benefit of this trait is not for a single instance but for an array of
// such types, as a loop can be replaced by a single call to 'memcpy'.
//
///What constitutes bit-wise movability?
///--------------------------------------
// TBD  A short guide on when to attach this trait to a class should follow.
//
///Usage
///-----
// TBD

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITBITWISEMOVEABLE
#include <bslalg_typetraitbitwisemoveable.h>
#endif

namespace BloombergLP {

                        // =====================================
                        // class bdealg_TypeTraitBitwiseMoveable
                        // =====================================

typedef bslalg_TypeTraitBitwiseMoveable bdealg_TypeTraitBitwiseMoveable;
    // Objects of a type with this trait can be "moved" from one memory
    // location to another using 'memmove' or 'memcpy'.  Although the result
    // of such a bitwise copy is two copies of the same object, this trait
    // only guarantees that one of the copies can be destroyed.  The other
    // copy must be considered invalid and its destructor must not be called.
    // Most types, even those that contain pointers, are bitwise moveable.
    // Undefined behavior may result if this trait is assigned to a types that
    // contains pointers to its own internals, use virtual inheritance, or
    // places pointers to itself within other data structures.

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
