// bdema_managedallocator.h            -*-C++-*-
#ifndef INCLUDED_BDEMA_MANAGEDALLOCATOR
#define INCLUDED_BDEMA_MANAGEDALLOCATOR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a protocol for memory allocators that support 'release'.
//
//@DEPRECATED: Use 'bslma_managedallocator' instead.
//
//@CLASSES:
//   bdema_ManagedAllocator: protocol for allocators with 'release' capability
//
//@SEE_ALSO: 'bdema_strallocator'
//
//@AUTHOR: Shao-wei Hung (shung)
//
//@DESCRIPTION: This component extends the 'bdema_Allocator' protocol to
// allocators that support the ability to 'release' all memory currently
// allocated through the protocol back to the memory supplier of the derived
// concrete allocator object.
//..
//                    ( bdema_ManagedAllocator )
//                                 |           release
//                                 |
//                                 v
//                        ( bdema_Allocator )
//                                          allocate
//                                          deallocate
//..
///USAGE
///-----
// The 'bdema_ManagedAllocator' protocol class serves as a useful internal
// interface for documentation purpose and could be used as a parameter to
// low-level helper functions for some implementations.  We have yet to find a
// suitable real-world example and when one becomes available, it will be
// added.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLMA_MANAGEDALLOCATOR
#include <bslma_managedallocator.h>
#endif

namespace BloombergLP {

typedef bslma_ManagedAllocator bdema_ManagedAllocator;
    // Provide a protocol for allocators with the ability to 'release' all
    // memory currently allocated through the protocol back to the memory
    // supplier of the derived concrete allocator object.

}  // close namespace BloombergLP



#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
