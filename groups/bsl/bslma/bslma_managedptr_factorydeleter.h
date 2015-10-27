// bslma_managedptr_factorydeleter.h                                  -*-C++-*-
#ifndef INCLUDED_BSLMA_MANAGEDPTR_FACTORYDELETER
#define INCLUDED_BSLMA_MANAGEDPTR_FACTORYDELETER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$ $CSID$")

//@PURPOSE: Provide a basic deleter for the managed pointer class.
//
//@CLASSES:
//  bslma::ManagedPtr_FactoryDeleter: deduced deleter used by bslma::ManagedPtr
//
//@SEE_ALSO: bslma_managedptr
//
//@DESCRIPTION: This component provides a class used as an implementation
// detail by 'bslma::ManagedPtr' to produce a type-erasing deleter function
// that will destroy an object of a parameterized type, using a factory of
// another parameterized type that provides a 'deleteObject' method.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP {
namespace bslma {

                   // ================================
                   // struct ManagedPtr_FactoryDeleter
                   // ================================

template <class ELEMENT_TYPE, class FACTORY>
struct ManagedPtr_FactoryDeleter {
    // This utility provides a general deleter for objects that provide a
    // 'deleteObject' operation (e.g., 'bslma::Allocator', 'bdlma_Pool').

    // CLASS METHODS
    static void deleter(void *object, void *factory);
        // Deleter function that deletes the specified 'object' by invoking
        // 'deleteObject' on the specified 'factory', casting 'object' to
        // 'ELEMENT_TYPE *' and 'factory' to 'FACTORY *'.  Note that the
        // behavior is undefined if '0 == object', or if 'factory' does not
        // point to an an object of type 'FACTORY', or 'object' does not point
        // to a complete object of type 'ELEMENT_TYPE' unless 'ELEMENT_TYPE'
        // has a virtual destructor and 'object' points to an object whose
        // dynamic type derives from 'ELEMENT_TYPE.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                    // --------------------------------
                    // struct ManagedPtr_FactoryDeleter
                    // --------------------------------

template <class ELEMENT_TYPE, class FACTORY>
inline
void ManagedPtr_FactoryDeleter<ELEMENT_TYPE, FACTORY>::deleter(void *object,
                                                               void *factory)
{
    BSLS_ASSERT_SAFE(0 != object);
    BSLS_ASSERT_SAFE(0 != factory);

    static_cast<FACTORY *>(factory)->deleteObject(
                                          static_cast<ELEMENT_TYPE *>(object));
}

}  // close package namespace
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
