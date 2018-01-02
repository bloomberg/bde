// bdlma_aligningallocator.cpp                                        -*-C++-*-
#include <bdlma_aligningallocator.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlma_aligningallocator_cpp,"$Id$ $CSID$")

#include <bslma_default.h>
#include <bslmf_assert.h>
#include <bsls_alignmentutil.h>
#include <bsls_assert.h>

#include <bsl_algorithm.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

namespace BloombergLP {
namespace bdlma {

// CREATORS
AligningAllocator::AligningAllocator(bsls::Types::size_type  alignment,
                                     bslma::Allocator       *allocator)
: d_mask(bsl::min<bsls::Types::size_type>(alignment,
                                  bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT) - 1)
, d_heldAllocator_p(bslma::Default::allocator(allocator))
{
    BSLS_ASSERT(0 < alignment);
    BSLS_ASSERT(0 == (alignment & (alignment - 1)));    // power of 2
}

// MANIPULATORS
void *AligningAllocator::allocate(bsls::Types::size_type size)
{
    size = (size + d_mask) & ~d_mask;

    void *ret = d_heldAllocator_p->allocate(size);

    // assert 'ret' is aligned
    BSLS_ASSERT(0 == (reinterpret_cast<bsls::Types::size_type>(ret) & d_mask));

    return ret;
}

void AligningAllocator::deallocate(void *address)
{
    d_heldAllocator_p->deallocate(address);
}

}  // close package namespace
}  // close enterprise namespace

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
