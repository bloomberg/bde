// bdlma_aligningallocator.cpp                                        -*-C++-*-
#include <bdlma_aligningallocator.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlma_aligningallocator_cpp, "$Id$ $CSID$")

#include <bdlma_bufferedsequentialallocator.h>     // for testing only

#include <bslma_default.h>
#include <bslmf_assert.h>

#include <bsls_alignmentutil.h>
#include <bsls_assert.h>
#include <bsls_bslexceptionutil.h>

namespace BloombergLP {
namespace bdlma {

namespace {

struct AllocationHeader {
    // A simple 'struct' to be placed before allocated user memory.

    void *d_allocation_p;
};

// If 'AllocationHeader' is somehow too big then we would have to allocate even
// more to fit the header in front of the returned memory blocks.  Verify it is
// a power of 2 to simplify the rounding that needs to be done in
// 'AligningAllocator::allocate'.
BSLMF_ASSERT(sizeof(AllocationHeader) <=
             bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT);
BSLMF_ASSERT(0 == (sizeof(AllocationHeader) & (sizeof(AllocationHeader) - 1)));

}  // close unnamed namespace

                          // -----------------------
                          // class AligningAllocator
                          // -----------------------

// CREATORS
AligningAllocator::AligningAllocator(bsls::Types::size_type  alignment,
                                     bslma::Allocator       *basicAllocator)
: d_mask(alignment - 1)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(0 < alignment);
    BSLS_ASSERT(0 == (alignment & (alignment - 1)));    // power of 2
}

// MANIPULATORS
void *AligningAllocator::allocate(bsls::Types::size_type size)
{
    void *ret;

    if (d_mask > bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT) {
        if (size) {
            bsls::Types::size_type needed = size + d_mask + 1;

            // We need to round up so that the requested block gets the
            // alignment of 'AllocationHeader'.
            needed = (needed + (sizeof(AllocationHeader) - 1)) &
                     ~(sizeof(AllocationHeader) - 1);

            void *underlying = d_allocator_p->allocate(needed);

            if (!underlying) {
                bsls::BslExceptionUtil::throwBadAlloc();
            }

            bsls::Types::size_type offset =
                bsls::AlignmentUtil::calculateAlignmentOffset(
                    underlying, static_cast<int>(d_mask + 1));
            if (offset < sizeof(AllocationHeader)) {
                // The requested block should be aligned to a multiple of
                // 'sizeof(AllocationHeader)', so the only possible smaller
                // 'offset' is 0.
                BSLS_ASSERT_SAFE(0 == offset);

                // Since we got the alignment we are trying to provide, we have
                // to offset by that alignment to have space for the header and
                // maintain the alignment.
                offset = d_mask + 1;
            }

            ret = reinterpret_cast<void *>(
                reinterpret_cast<unsigned char *>(underlying) + offset);
            AllocationHeader *header = reinterpret_cast<AllocationHeader *>(
                reinterpret_cast<unsigned char *>(ret) -
                sizeof(AllocationHeader));
            header->d_allocation_p = underlying;
        }
        else {
            ret = 0;
        }
    }
    else {
        // we can rely on the underlying allocator to align for us
        size = (size + d_mask) & ~d_mask;

        ret = d_allocator_p->allocate(size);
    }

    // assert 'ret' is aligned
    BSLS_ASSERT(0 == (reinterpret_cast<bsls::Types::size_type>(ret) & d_mask));

    return ret;
}

void AligningAllocator::deallocate(void *address)
{
    if (d_mask > bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT) {
        if (address) {
            AllocationHeader *header = reinterpret_cast<AllocationHeader *>(
                reinterpret_cast<unsigned char *>(address) -
                sizeof(AllocationHeader));
            d_allocator_p->deallocate(header->d_allocation_p);
        }
    }
    else {
        d_allocator_p->deallocate(address);
    }
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
