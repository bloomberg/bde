// bslma_managedptr_factorydeleter.h                                  -*-C++-*-
#ifndef INCLUDED_BSLMA_MANAGEDPTR_FACTORYDELETER
#define INCLUDED_BSLMA_MANAGEDPTR_FACTORYDELETER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$ $CSID$")

//@PURPOSE: Provide a factory-based deleter for the managed pointer class.
//
//@CLASSES:
//  bslma::ManagedPtr_FactoryDeleter: deduced deleter used by bslma::ManagedPtr
//
//@SEE_ALSO: bslma_managedptr
//
//@DESCRIPTION: This component provides a utility 'struct',
// 'bslma::ManagedPtr_FactoryDeleter', used as an implementation detail by
// 'bslma::ManagedPtr' to produce a type-erasing deleter function that destroys
// an object of (template parameter) 'OBJECT_TYPE', using a factory of
// (template parameter) 'FACTORY' type that provides a 'deleteObject' method.

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

template <class OBJECT_TYPE, class FACTORY>
struct ManagedPtr_FactoryDeleter {
    // This utility provides a general deleter for factories that provide a
    // 'deleteObject' operation (e.g., 'bslma::Allocator', 'bdlma::Pool').

    // CLASS METHODS
    static void deleter(void *object, void *factory);
        // Delete the specified 'object' by invoking 'deleteObject' on the
        // specified 'factory', casting 'object' to 'OBJECT_TYPE *' and
        // 'factory' to 'FACTORY *'.  The behavior is undefined unless
        // 'factory' points to an object of type 'FACTORY', and 'object' points
        // to a complete object of type 'OBJECT_TYPE' or 'OBJECT_TYPE' has a
        // virtual destructor and 'object' points to an object whose dynamic
        // type derives from 'OBJECT_TYPE.
};

// ============================================================================
//                          INLINE DEFINITIONS
// ============================================================================

                    // --------------------------------
                    // struct ManagedPtr_FactoryDeleter
                    // --------------------------------

// CLASS METHODS
template <class OBJECT_TYPE, class FACTORY>
inline
void ManagedPtr_FactoryDeleter<OBJECT_TYPE, FACTORY>::deleter(void *object,
                                                              void *factory)
{
    BSLS_ASSERT_SAFE(0 != object);
    BSLS_ASSERT_SAFE(0 != factory);

    static_cast<FACTORY *>(factory)->deleteObject(
                                           static_cast<OBJECT_TYPE *>(object));
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
