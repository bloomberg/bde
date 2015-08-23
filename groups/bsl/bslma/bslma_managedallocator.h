// bslma_managedallocator.h                                           -*-C++-*-
#ifndef INCLUDED_BSLMA_MANAGEDALLOCATOR
#define INCLUDED_BSLMA_MANAGEDALLOCATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a protocol for memory allocators that support 'release'.
//
//@INTERNAL_DEPRECATED: Use 'bdlma_managedallocator' instead.
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

#ifdef BDE_OPENSOURCE_PUBLICATION // DEPRECATED
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

#ifndef BDE_OPENSOURCE_PUBLICATION  // BACKWARD_COMPATIBILITY
// ===========================================================================
//                           BACKWARD COMPATIBILITY
// ===========================================================================

typedef bslma::ManagedAllocator bslma_ManagedAllocator;
    // This alias is defined for backward compatibility.
#endif  // BDE_OPENSOURCE_PUBLICATION -- BACKWARD_COMPATIBILITY

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
