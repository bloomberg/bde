// bslma_allocator.cpp                                                -*-C++-*-
#include <bslma_allocator.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bslma_memoryresourceimpsupport.h>

#include <bsls_assert.h>
#include <bsls_bslexceptionutil.h>
#include <bsls_keyword.h>
#include <bsls_performancehint.h>

typedef BloombergLP::bsls::AlignmentUtil AlignUtil;

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
void *Allocator::do_allocate(std::size_t bytes, std::size_t align)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(0 == bytes)) {
        void *p = this->allocate(bytes);
        if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(0 == p)) {
            // Convert null pointer to singular pointer.
            p = bslma::MemoryResourceImpSupport::singularPointer();
        }
        return p;
    }

    int naturalAlign = bsls::AlignmentUtil::calculateAlignmentFromSize(bytes);

    if ((int) align <= naturalAlign) {
        return this->allocate(bytes);  // Forward to 'Allocator::allocate'
    }
    else if (align <= AlignUtil::BSLS_MAX_ALIGNMENT) {
        // Round 'bytes' up to a larger size that has a natural alignment of
        // 'align'.
        bytes = (bytes + align - 1) & ~(align - 1);
        return this->allocate(bytes);  // Forward to 'Allocator::allocate'
    }
    else {
        // Allocate extra memory to hold an aligned sub-block preceded by a
        // 'int' header representing the offset from the start of the allocated
        // block to the start of the aligned sub-block:
        // ```
        //   <------ allocated block --------->
        //   <- offset -->
        //  |    | header | returned block |   |
        // ```

        bytes += sizeof(int) + align - 1;
        char *block = static_cast<char *>(this->allocate(bytes));
        int offset = sizeof(int) +
            AlignUtil::calculateAlignmentOffset(block + sizeof(int),
                                                (int) align);
        void *ret  = block + offset;
        static_cast<int *>(ret)[-1] = offset;  // Store offset in header
        return ret;
    }
}

void Allocator::do_deallocate(void *p, std::size_t bytes, std::size_t align)
{
    (void) bytes;  // Used only for `BSLS_ASSERT`; not used in an OPT build.

    if (bslma::MemoryResourceImpSupport::singularPointer() == p) {
        // Special pointer indicates zero-byte `memory_resource` allocation.
        BSLS_ASSERT(0 == bytes);

        // Convert singular pointer to null pointer before forwarding to
        // 'Allocator::deallocate'.
        p = 0;
    }
    else if (align > AlignUtil::BSLS_MAX_ALIGNMENT) {
        int offset = static_cast<int *>(p)[-1];  // Read offset before 'p'
        char* block = static_cast<char *>(p) - offset; // Find enclosing block
        p = block;  // Forward enclosing block to 'Allocator::deallocate'.
    }

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
