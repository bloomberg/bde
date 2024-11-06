// bslma_allocator.cpp                                                -*-C++-*-
#include <bslma_allocator.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bslma_memoryresourceimpsupport.h>

#include <bsls_assert.h>
#include <bsls_bslexceptionutil.h>
#include <bsls_keyword.h>

#include <stdint.h>  // 'uintptr_t' -- portable to all supported compilers

namespace BloombergLP {
namespace bslma {

                        // ---------------
                        // class Allocator
                        // ---------------

#ifndef BDE_OMIT_INTERNAL_DEPRECATED

// CLASS METHODS
void Allocator::throwBadAlloc()
{
    bsls::BslExceptionUtil::throwBadAlloc();
}

#endif  // BDE_OMIT_INTERNAL_DEPRECATED

// CREATORS
Allocator::~Allocator()
{
}

// PROTECTED MANIPULATORS
void *Allocator::do_allocate(std::size_t bytes, std::size_t)
{
    void *p = this->allocate(bytes);

    if (0 == bytes && 0 == p) {
        // `do_allocate` is not allowed to return null, so return special ptr.
        return bslma::MemoryResourceImpSupport::singularPointer();
    }

    return p;
}

void Allocator::do_deallocate(void *p, std::size_t bytes, std::size_t)
{
    (void) bytes;  // Not used in an OPT build

    if (bslma::MemoryResourceImpSupport::singularPointer() == p) {
        // Special pointer indicates zero-byte `memory_resource` allocation.
        BSLS_ASSERT(0 == bytes);
        p = 0;  // Change pointer to zero-byte expected value for `Allocator`.
    }

    // Forward to `Allocator` interface.
    this->deallocate(p);
}

// PROTECTED ACCESSORS
bool Allocator::do_is_equal(const bsl::memory_resource& other) const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return this == &other;
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
