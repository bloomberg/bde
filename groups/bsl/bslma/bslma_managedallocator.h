// bslma_managedallocator.h                                           -*-C++-*-
#ifndef INCLUDED_BSLMA_MANAGEDALLOCATOR
#define INCLUDED_BSLMA_MANAGEDALLOCATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a protocol for memory allocators that support 'release'.
//
//@CLASSES:
//   bslma_ManagedAllocator: protocol for allocators with 'release' capability
//
//@SEE_ALSO: bslma_allocator
//
//@AUTHOR: Shao-wei Hung (shung)
//
//@DESCRIPTION: This component extends the base-level protocol (pure abstract
// interface) class, 'bslma_allocator', providing the ability to 'release' all
// memory currently allocated through the protocol back to the memory supplier
// of the derived concrete allocator object.
//..
//   ,----------------------.
//  ( bslma_ManagedAllocator )
//   `----------------------'
//               |       release
//               V
//       ,---------------.
//      ( bslma_Allocator )
//       `---------------'
//                       allocate
//                       deallocate
//..
///Usage
///-----
// The 'bslma_ManagedAllocator' protocol class serves as a useful internal
// interface for documentation purpose and could be used as a parameter to
// low-level helper functions for some implementations.  We have yet to find a
// suitable real-world example and when one becomes available, it will be
// added.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

namespace BloombergLP {

                        // ============================
                        // class bslma_ManagedAllocator
                        // ============================

class bslma_ManagedAllocator : public bslma_Allocator {
    // Provide a protocol for allocators with the ability to 'release' all
    // memory currently allocated through the protocol back to the memory
    // supplier of the derived concrete allocator object.

  public:
    // MANIPULATORS
    virtual void release() = 0;
        // Release all memory currently allocated through this allocator.
};

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
