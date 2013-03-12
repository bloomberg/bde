// bslma_managedallocator.h                                           -*-C++-*-
#ifndef INCLUDED_BSLMA_MANAGEDALLOCATOR
#define INCLUDED_BSLMA_MANAGEDALLOCATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a protocol for memory allocators that support 'release'.
//
//@INTERNAL_DEPRECATED: use 'bdema_managedallocator' instead.
//
//@CLASSES:
//  bslma::ManagedAllocator: protocol for allocators with 'release' capability
//
//@SEE_ALSO: bslma_allocator
//
//@AUTHOR: Shao-wei Hung (shung)
//
//@DESCRIPTION: This component extends the base-level protocol (pure abstract
// interface) class, 'bslma::Allocator', providing the ability to 'release'
// all memory currently allocated through the protocol back to the memory
// supplier of the derived concrete allocator object.
//..
//   ,-----------------------.
//  ( bslma::ManagedAllocator )
//   `-----------------------'
//               |       release
//               V
//       ,----------------.
//      ( bslma::Allocator )
//       `----------------'
//                       allocate
//                       deallocate
//..
///Usage
///-----
// The 'bslma::ManagedAllocator' protocol class serves as a useful internal
// interface for documentation purpose and could be used as a parameter to
// low-level helper functions for some implementations.  We have yet to find a
// suitable real-world example and when one becomes available, it will be
// added.

#ifdef BDE_OMIT_TRANSITIONAL // DEPRECATED
#error "bslma_managedallocator is deprecated"
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

namespace BloombergLP {

namespace bslma {

                        // ======================
                        // class ManagedAllocator
                        // ======================

class ManagedAllocator : public Allocator {
    // Provide a protocol for allocators with the ability to 'release' all
    // memory currently allocated through the protocol back to the memory
    // supplier of the derived concrete allocator object.

  public:
    // MANIPULATORS
    virtual void release() = 0;
        // Release all memory currently allocated through this allocator.
};

}  // close package namespace

#ifndef BDE_OMIT_TRANSITIONAL  // BACKWARD_COMPATIBILITY
// ===========================================================================
//                           BACKWARD COMPATIBILITY
// ===========================================================================

typedef bslma::ManagedAllocator bslma_ManagedAllocator;
    // This alias is defined for backward compatibility.
#endif  // BDE_OMIT_TRANSITIONAL -- BACKWARD_COMPATIBILITY

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2013 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
