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

bsls::AtomicOperations::AtomicTypes::Pointer
                                    Default::s_requestedDefaultAllocator = {0};
bsls::AtomicOperations::AtomicTypes::Pointer Default::s_defaultAllocator = {0};

                        // *** global allocator ***

bsls::AtomicOperations::AtomicTypes::Pointer Default::s_globalAllocator = {0};

// CLASS METHODS

                        // *** default allocator ***

Allocator *Default::determineAndReturnDefaultAllocator()
{
    // Make sure to cast from the base protocol to void in order to match the
    // reverse cast below.

    void *const fallback = static_cast<Allocator *>(
                                             &NewDeleteAllocator::singleton());

    // If nothing is requested use the 'fallback' allocator.

    void *requested = bsls::AtomicOperations::testAndSwapPtr(
                                                  &s_requestedDefaultAllocator,
                                                  0,
                                                  fallback);
    if (!requested) {
        // When the previous value of 's_requestedDefaultAllocator' was '0' we
        // were the one to assign it the 'fallback' value, so use that.
        // Otherwise we use whatever was returned in 'requested'.

        requested = fallback;
    }

    // If nothing is installed install the 'requested' allocator.

    void *installed = bsls::AtomicOperations::testAndSwapPtr(
                                                           &s_defaultAllocator,
                                                           0,
                                                           requested);
    if (!installed) {
        // When the previous value of 's_defaultAllocator' was '0' we were the
        // one to assign it the 'requested' value, so use that.  Otherwise we
        // use whatever was returned in 'installed'.

        installed = requested;
    }

    // The reverse cast: go from void to the base protocol.

    return static_cast<Allocator *>(installed);
}

int Default::setDefaultAllocator(Allocator *basicAllocator)
{
    BSLS_ASSERT(0 != basicAllocator);

    // Use read-modify-write operation to update the value and acquire writes
    // to 's_defaultAllocator'.  We'll check its value below to check for
    // success.  We prevent reordering the read before the write here.

    void *const previouslyRequested = bsls::AtomicOperations::swapPtr(
                                                  &s_requestedDefaultAllocator,
                                                  basicAllocator);

    // If nothing was previously requested we've successfully placed a request.

    if (previouslyRequested == 0) {
        return 0;                                                     // RETURN
    }

    // We check for the installed default allocator being '0' where this value
    // is recent as of our read-modify-write above.  If nothing has been
    // installed yet we conclude success.  When the default allocator is first
    // requested our write to 's_requestedDefaultAllocator' will be honored.
    // This could only fail when the request for the default allocator runs
    // concurrently to this code, but this would be out of contract and we just
    // take our best guess.

    if (!bsls::AtomicOperations::getPtr(&s_defaultAllocator)) {
        return 0;                                                     // RETURN
    }

    return -1;     // some allocator in use already
}

void Default::setDefaultAllocatorRaw(Allocator *basicAllocator)
{
    BSLS_ASSERT(0 != basicAllocator);

    bsls::AtomicOperations::setPtr(&s_defaultAllocator, basicAllocator);
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

