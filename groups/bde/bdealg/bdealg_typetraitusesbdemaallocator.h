// bdealg_typetraitusesbdemaallocator.h   -*-C++-*-
#ifndef INCLUDED_BDEALG_TYPETRAITUSESBDEMAALLOCATOR
#define INCLUDED_BDEALG_TYPETRAITUSESBDEMAALLOCATOR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a primitive trait for types that use 'bdema' allocators.
//
//@DEPRECATED: Use 'bslalg_typetraitusesbslmaallocator' instead.
//
//@CLASSES:
//   bdealg_TypeTraitUsesBdemaAllocator: uses 'bdema' allocators
//
//@SEE_ALSO: bslmf_typetraits, bdealg_constructorproxy
//
//@AUTHOR: Herve Bronnimann (hbronnim)
//
//@DESCRIPTION:  This component provides a single traits class,
// 'bdealg_TypeTraitUsesBdemaAllocator'.  A type 'T' with this trait uses an
// allocator derived from 'bdema_Allocator' to allocate memory.  Such a type
// *must* have a "copy" constructor with prototype 'T(const T&,
// bdema_Allocator*)'.  It usually also has a "default" constructor with
// prototype 'T(bdema_Allocator*)', and it may also have additional
// constructors with various numbers of arguments, that take an optional
// 'bdema_Allocator*' last argument.
//
// This component is used by virtually all 'bdealg' components for providing
// primitives that ensure that a 'bdema' allocator is always passed through
// properly to a class creator if that class has the 'bdema' allocator trait.
//
///What does it mean, "uses 'bdema' allocators"?
///---------------------------------------------
// TBD  Some nice guide on 'bdema' allocator protocol, its use at Bloomberg,
// why it is important to properly respect this protocol, etc.
//
///Usage
///-----
// TBD

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITUSESBSLMAALLOCATOR
#include <bslalg_typetraitusesbslmaallocator.h>
#endif

namespace BloombergLP {

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
