// bdlma_factory.h                                                    -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLMA_FACTORY
#define INCLUDED_BDLMA_FACTORY

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a protocol for creator/deleter of parameterized objects.
//
//@CLASSES:
//   bdlma::Factory: protocol class for creator/deleter of 'TYPE' objects
//
//@SEE_ALSO: bdlma_deleter
//
//@DESCRIPTION: This component defines the base-level protocol for a
// thread-safe and type-safe creator and deleter of objects of parameterized
// type.  In particular, 'bdlma::Factory' extends the 'bdlma::Deleter' protocol
// with the addition of a 'createObject' method:
//..
//                         ( bdlma::Factory )
//                                 |       createObject
//                                 v
//                         ( bdlma::Deleter )
//                                         dtor
//                                         deleteObject
//..
// This class is extremely useful for transferring the ownership of objects
// between different entities.  The following usage example demonstrates this
// point.
//
///Usage
///-----
// Suppose that we would like to transfer an object between threads using
// 'bsl::shared_ptr'.  For the sake of discussion, the type of this object is
// 'my_Obj' and we will suppose that it is created using a concrete
// implementation of 'bdlma::Factory', say, 'my_Factory', the implementation of
// which is assumed:
//..
//  my_Factory  factory;
//  my_Obj     *object = factory.createObject();
//..
// Next we create a shared pointer passing to it 'object' and the 'factory'
// that was used to create 'object':
//..
//  bsl::shared_ptr<my_Obj> handle(object, &factory);
//..
// Now the 'handle' can be passed to another thread or enqueued efficiently.
// Once the reference count of 'handle' goes to 0, 'object' is automatically
// deleted via the 'deleteObject' method of 'factory', which in turn will
// invoke the destructor of 'object'.  Note that since the type of the factory
// used to both create the object under management and to instantiate 'handle'
// is 'bdlma::Factory<my_Obj>', any kind of creator/deleter that implements
// this protocol can be passed.  Also note, on the downside, that the lifetime
// of 'factory' must be longer than the lifetime of all associated object
// instances.

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLMA_DELETER
#include <bdlma_deleter.h>
#endif

namespace BloombergLP {
namespace bdlma {

                              // =============
                              // class Factory
                              // =============

template <class TYPE>
class Factory : public Deleter<TYPE> {
    // This 'class' provides a protocol (or pure interface) for a thread-safe
    // object creator and deleter.  It extends the 'Deleter' protocol with the
    // addition of a 'createObject' method.

  public:
    // CREATORS
    virtual ~Factory();
        // Destroy this object factory.

    virtual TYPE *createObject() = 0;
        // Create an instance of the specified 'TYPE' using default arguments.
        // Return the address of the newly-created object on success, and 0
        // otherwise.  The instance must be destroyed using the 'deleteObject'
        // method of this object factory.

    // MANIPULATORS
    virtual void deleteObject(TYPE *instance) = 0;
        // Destroy the specified 'instance' based on its static type and return
        // its memory footprint to the appropriate memory manager.  The
        // behavior is undefined unless 'instance' was obtained from a call to
        // 'createObject' on this object factory and has not yet been deleted.
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

// CREATORS
template <class TYPE>
Factory<TYPE>::~Factory()
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
