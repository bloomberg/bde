// bdlma_countingallocator.cpp                                        -*-C++-*-
#include <bdlma_countingallocator.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlma_countingallocator_cpp,"$Id$ $CSID$")

#include <bslma_default.h>

#include <bsls_alignmentutil.h>
#include <bsls_assert.h>
#include <bsls_performancehint.h>

#include <bsl_ostream.h>

namespace BloombergLP {
namespace bdlma {
namespace {

// LOCAL CONSTANTS

// Define the number of bytes by which the address returned to the user is
// *offset* from the actual address of the allocated memory block.

static const bslma::Allocator::size_type OFFSET =
                                       bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT;

}  // close unnamed namespace

                         // -----------------------
                         // class CountingAllocator
                         // -----------------------

// CREATORS
CountingAllocator::CountingAllocator(Allocator *basicAllocator)
: d_name_p(0)
, d_numBytesInUse(0)
, d_numBytesTotal(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(0 == name());
    BSLS_ASSERT(0 == numBytesInUse());
    BSLS_ASSERT(0 == numBytesTotal());
    BSLS_ASSERT(d_allocator_p);
}

CountingAllocator::CountingAllocator(const char *name,
                                     Allocator  *basicAllocator)
: d_name_p(name)
, d_numBytesInUse(0)
, d_numBytesTotal(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(0 != this->name());
    BSLS_ASSERT(0 == numBytesInUse());
    BSLS_ASSERT(0 == numBytesTotal());
    BSLS_ASSERT(d_allocator_p);
}

CountingAllocator::~CountingAllocator()
{
    BSLS_ASSERT(0               <= numBytesInUse());
    BSLS_ASSERT(0               <= numBytesTotal());
    BSLS_ASSERT(numBytesInUse() <= numBytesTotal());
    BSLS_ASSERT(d_allocator_p);
}

// MANIPULATORS
void *CountingAllocator::allocate(size_type size)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(0 == size)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return 0;                                                     // RETURN
    }

    // Round up 'size' for maximal alignment and add sufficient space to record
    // 'size' in the allocated block.

    const size_type totalSize =
                 bsls::AlignmentUtil::roundUpToMaximalAlignment(size) + OFFSET;

    void *address = d_allocator_p->allocate(totalSize);

    d_numBytesInUse.addRelaxed(static_cast<bsls::Types::Int64>(size));
    d_numBytesTotal.addRelaxed(static_cast<bsls::Types::Int64>(size));

    *static_cast<size_type *>(address) = size;

    return static_cast<char *>(address) + OFFSET;
}

void CountingAllocator::deallocate(void *address)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(0 == address)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return;                                                       // RETURN
    }

    address = static_cast<char *>(address) - OFFSET;

    const size_type recordedSize = *static_cast<size_type *>(address);

    d_numBytesInUse.addRelaxed(-static_cast<bsls::Types::Int64>(recordedSize));

    d_allocator_p->deallocate(address);
}

// ACCESSORS
bsl::ostream& CountingAllocator::print(bsl::ostream& stream) const
{
    stream << "----------------------------------------\n"
           << "        Counting Allocator State\n"
           << "----------------------------------------\n";

    if (d_name_p) {
        stream << "Allocator name: " << name() << "\n";
    }

    stream << "Bytes in use:   " << numBytesInUse() << "\n"
           << "Bytes in total: " << numBytesTotal() << "\n";

    return stream;
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
