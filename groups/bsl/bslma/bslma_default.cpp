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
        return 0;  // success
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
// Copyright (C) 2012 Bloomberg L.P.
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
