// bslma_default.cpp                                                  -*-C++-*-
#include <bslma_default.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bslma_allocator.h>            // for testing only
#include <bsls_assert.h>

namespace BloombergLP {

namespace bslma {

class Allocator;

                               // --------------
                               // struct Default
                               // --------------

// STATIC DATA MEMBERS

                        // *** default allocator ***

bsls::AtomicOperations::AtomicTypes::Pointer Default::s_allocator = {0};
bsls::AtomicOperations::AtomicTypes::Int     Default::s_locked    = {0};

                        // *** global allocator ***

bsls::AtomicOperations::AtomicTypes::Pointer Default::s_globalAllocator = {0};

// CLASS METHODS

                        // *** default allocator ***

int Default::setDefaultAllocator(Allocator *basicAllocator)
{
    BSLS_ASSERT(0 != basicAllocator);

    if (!bsls::AtomicOperations::getIntRelaxed(&s_locked)) {
        bsls::AtomicOperations::setPtrRelease(&s_allocator, basicAllocator);
        return 0;  // success                                         // RETURN
    }

    return -1;     // locked -- 'set' fails
}

void Default::setDefaultAllocatorRaw(Allocator *basicAllocator)
{
    BSLS_ASSERT(0 != basicAllocator);

    bsls::AtomicOperations::setPtrRelease(&s_allocator, basicAllocator);
}

                        // *** global allocator ***

Allocator *Default::setGlobalAllocator(Allocator *basicAllocator)
{
    Allocator *previous =
        (Allocator *) bsls::AtomicOperations::swapPtrAcqRel(&s_globalAllocator,
                                                            basicAllocator);

    return previous ? previous
                    : &NewDeleteAllocator::singleton();
}

}  // close package namespace

}  // close enterprise namespace

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
