// bslma_bufferallocator.cpp                                          -*-C++-*-
#ifndef BDE_OPENSOURCE_PUBLICATION // DEPRECATED

#include <bslma_bufferallocator.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bslmf_assert.h>
#include <bsls_alignment.h>
#include <bsls_alignmentutil.h>
#include <bsls_assert.h>
#include <bsls_performancehint.h>
#include <bsls_bslexceptionutil.h>

#include <climits>  // INT_MAX
#include <cstdio>

namespace BloombergLP {

static
void *allocateFromBufferImp(int                               *cursor,
                            char                              *buffer,
                            bslma::BufferAllocator::size_type  bufSize,
                            bslma::BufferAllocator::size_type  size,
                            int                                alignment)
    // Allocate a memory block of the specified 'size' from the specified
    // 'buffer' at the specified 'cursor' position, aligned at the specified
    // 'alignment' boundary.  Return the address of the allocated memory block
    // if 'buffer' contains enough available memory, and 'null' otherwise.  The
    // 'cursor' is set to the position of remaining free buffer space after the
    // allocation.  The behavior is undefined unless
    // '0 < alignment <= bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT' and alignment
    // is an integral power of 2.

{
    BSLS_ASSERT(cursor);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < alignment);
    BSLS_ASSERT(alignment <= bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT);
    BSLS_ASSERT(0 == (alignment & (alignment - 1))); // alignment is power of 2

    int offset = bsls::AlignmentUtil::calculateAlignmentOffset(
                                                              buffer + *cursor,
                                                              alignment);

    if (*cursor + offset + size > bufSize) { // insufficient space
        return static_cast<void *>(0);
    }

#if 0
    // Consider adding the following:

    if (*cursor + offset + size > 
                     static_cast<bslma::BufferAllocator::size_type>(INT_MAX)) {
        // not representable

        return static_cast<void *>(0);
    }
#endif

    void *result = &buffer[*cursor + offset];
    *cursor += static_cast<int>(offset + size);

    return result;
}

namespace bslma {

                        // ---------------------
                        // class BufferAllocator
                        // ---------------------

// CLASS METHODS
void *BufferAllocator::allocateFromBuffer(int               *cursor,
                                          char              *buffer,
                                          size_type          bufSize,
                                          size_type          size,
                                          AlignmentStrategy  strategy)
{
    BSLS_ASSERT(cursor);
    BSLS_ASSERT(buffer);

    return 0 >= size
           ? static_cast<void *>(0)
           : allocateFromBufferImp(
                             cursor,
                             buffer,
                             bufSize,
                             size,
                             strategy == NATURAL_ALIGNMENT
                             ? bsls::AlignmentUtil::calculateAlignmentFromSize(
                                                                          size)
                             : bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT);
}

void *BufferAllocator::allocateFromBuffer(int       *cursor,
                                          char      *buffer,
                                          size_type  bufSize,
                                          size_type  size,
                                          int        alignment)
{
    BSLS_ASSERT(cursor);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < alignment);
    BSLS_ASSERT(alignment <= bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT);
    BSLS_ASSERT(0 == (alignment & (alignment - 1))); // alignment is power of 2


    return 0 >= size
           ? static_cast<void *>(0)
           : allocateFromBufferImp(cursor,
                                   buffer,
                                   bufSize,
                                   size,
                                   alignment);
}

// CREATORS
BufferAllocator::~BufferAllocator()
{
}

// MANIPULATORS
void *BufferAllocator::allocate(size_type size)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(0 == size)) {
        return static_cast<void *>(0);
    }

    void *result;
    if (d_strategy == NATURAL_ALIGNMENT) {
        result = allocateFromBufferImp(
                           &d_cursor,
                           d_buffer_p,
                           d_bufferSize,
                           size,
                           bsls::AlignmentUtil::calculateAlignmentFromSize(
                                                                        size));
    } else { // default is MAXIMUM_ALIGNMENT
        result = allocateFromBufferImp(
                                      &d_cursor,
                                      d_buffer_p,
                                      d_bufferSize,
                                      size,
                                      bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT);
    }
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(result)) {
        return result;
    }
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(d_allocCallback)) {
        return (*d_allocCallback)(static_cast<int>(size));
    }

    // Throw 'std::bad_alloc' if cannot satisfy request.
    bsls::BslExceptionUtil::throwBadAlloc();

    return result;
}

// ACCESSORS
void BufferAllocator::print() const
{
    union {
        // Quell diagnostics that occur whenever a function ptr is cast to
        // 'void *' by going through this union.

        AllocCallback  d_acb;
        void          *d_p;
    } u;
    u.d_acb = d_allocCallback;

    std::printf("buffer address      = %p\n"
                "buffer size         = %d\n"
                "cursor position     = %d\n"
                "allocation function = %p\n"
                "alignment strategy  = %s\n",
                static_cast<void *>(d_buffer_p),
                d_bufferSize,
                d_cursor,
                u.d_p,
                BufferAllocator::MAXIMUM_ALIGNMENT == d_strategy
                ? "MAXIMUM_ALIGNMENT"
                : "NATURAL_ALIGNMENT");

    std::fflush(stdout);
}

}  // close package namespace

}  // close enterprise namespace

#endif // BDE_OPENSOURCE_PUBLICATION -- DEPRECATED

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
