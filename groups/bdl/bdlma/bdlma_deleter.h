// bdlma_deleter.h                                                    -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLMA_DELETER
#define INCLUDED_BDLMA_DELETER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a protocol for deleting objects of parameterized type.
//
//@CLASSES:
//   bdlma::Deleter: protocol class for deleting objects
//
//@SEE_ALSO: bslma_allocator
//
//@DESCRIPTION: This component defines the base-level protocol,
// 'bdlma::Deleter', for a thread-safe and type-safe deleter of objects of
// arbitrary type.  This class is extremely useful for transferring the
// ownership of objects between different entities.  The following usage
// example demonstrates this point.
//
///Usage
///-----
// Suppose that we would like to transfer ownership of an object between
// threads using 'bsl::shared_ptr'.  For the sake of discussion, the type of
// this object is 'my_Obj', we will suppose that it is created using a given
// 'basicAllocator', and that a concrete implementation of 'bdlma::Deleter',
// say 'my_Deleter', is to be used.  Note that we assume that 'my_Obj' does not
// require an allocator for any of its members:
//..
//  bslma::NewDeleteAllocator basicAllocator;
//  my_Obj *object = new(basicAllocator) my_Obj;
//..
// Next, create a concrete deleter for 'object' using the same allocator as was
// used to allocate its footprint:
//..
//  my_Deleter deleter(&basicAllocator);
//..
// Finally, create a shared pointer passing to it 'object' and the address of
// 'deleter':
//..
//  bsl::shared_ptr<my_Obj> handle(object, &deleter, &basicAllocator);
//..
// Now the 'handle' can be passed to another thread or enqueued efficiently.
// When the reference count of 'handle' goes to 0, 'object' is automatically
// deleted via the 'deleteObject' method of 'deleter', which in turn will
// invoke the destructor of 'object'.  Note that since the type of the deleter
// used to instantiate 'handle' is 'bdlma::Deleter<my_Obj>', any kind of
// deleter that implements this protocol can be passed.  Also note, on the
// downside, that the lifetime of 'deleter' must be longer than the lifetime of
// all associated instances.

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

namespace BloombergLP {
namespace bdlma {

                              // =============
                              // class Deleter
                              // =============

template <class TYPE>
class Deleter {
    // Provide a protocol (or pure interface) for a thread-safe object deleter.

  public:
    // CREATORS
    virtual ~Deleter();
        // Destroy this object deleter.

    // MANIPULATORS
    virtual void deleteObject(TYPE *instance) = 0;
        // Destroy the specified 'instance' based on its static type and return
        // its memory footprint to the appropriate memory manager.  Note that a
        // particular implementation is allowed to destroy this deleter itself.
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

// CREATORS
template <class TYPE>
Deleter<TYPE>::~Deleter()
{
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
