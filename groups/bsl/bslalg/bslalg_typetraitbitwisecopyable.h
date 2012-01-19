// bslalg_typetraitbitwisecopyable.h                                  -*-C++-*-
#ifndef INCLUDED_BSLALG_TYPETRAITBITWISECOPYABLE
#define INCLUDED_BSLALG_TYPETRAITBITWISECOPYABLE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a primitive type trait for bit-wise copyable classes.
//
//@CLASSES:
//  bslalg::TypeTraitBitwiseCopyable: bit-wise copyable trait
//
//@SEE_ALSO: bslmf_typetraits
//
//@AUTHOR: Herve Bronnimann (hbronnim)
//
//@DESCRIPTION: This component provides a single traits class,
// 'bslalg::TypeTraitBitwiseCopyable'.  An object of a 'TYPE' that has the
// bit-wise copyable trait can be copied either by invoking the copy
// constructor or by copying the footprint (i.e., the 'sizeof(TYPE)' bytes at
// the object address) using 'memcpy'.
//
// Note also that this type is a synonym for having a trivial destructor, as in
// almost every case a bitwise copyable class does not need to perform any
// operation on destruction.  It is possible to write types that are
// operationally bitwise copyable (i.e., copy constructor can be implemented in
// terms of 'memcpy') but have a non-trivial destructor, but most examples of
// such types are contrived.  In any case, this trait should not be attached to
// such types.
//
// This component is used by various 'bslalg' components for providing
// optimized primitives for types that have the bit-wise copyable trait.  The
// major benefit of this trait is not for a single object but for an array of
// such types, as a loop can be replaced by a single call to 'memcpy'.
//
///What constitutes bit-wise copyability?
///--------------------------------------
// TBD: A short guide on when to attach this trait to a class should follow.
//
///Usage
///-----
// TBD

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

namespace BloombergLP {

namespace bslalg {

                        // ==============================
                        // class TypeTraitBitwiseCopyable
                        // ==============================

struct TypeTraitBitwiseCopyable {
    // Objects of a type with this trait can be copied using 'memcpy'.  After
    // such a bitwise copy, both the original and the copy are valid.  Classes
    // with this trait are assumed to have trivial (no-op) destructors and are
    // assumed to be bitwise moveable (see the 'TypeTraitBitwiseMoveable'
    // trait).  Undefined behavior may result if this trait is assigned to a
    // type that allocates memory or other resources, uses virtual inheritance,
    // or places pointers to itself within other data structures.  Also,
    // objects of a type with this trait can be destroyed by a no-op, i.e., not
    // invoking the destructor, although it is safe to write zeros into the
    // memory footprint of the object.
};

}  // close package namespace

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
