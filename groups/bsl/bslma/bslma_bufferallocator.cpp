// bslma_bufferallocator.cpp                                          -*-C++-*-
#include <bslma_bufferallocator.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_alignment.h>
#include <bsls_alignmentutil.h>
#include <bsls_assert.h>
#include <bsls_performancehint.h>

#include <cstdio>

namespace BloombergLP {

static
void *allocateFromBufferImp(int  *cursor,
                            char *buffer,
                            int   bufSize,
                            int   size,
                            int   alignment)
    // Allocate a memory block of the specified 'size' from the specified
    // 'buffer' at the specified 'cursor' position, aligned at the specified
    // 'alignment' boundary.  Return the address of the allocated memory
    // block if 'buffer' contains enough available memory, and 'null'
    // otherwise.  The 'cursor' is set to the position of remaining free
    // buffer space after the allocation.  The behavior is undefined unless
    // '0 <= bufSize', '0 < size',
    // '0 < alignment <= bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT' and alignment
    // is an integral power of 2.

{
    BSLS_ASSERT(cursor);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= bufSize);
    BSLS_ASSERT(0 < size);
    BSLS_ASSERT(0 < alignment);
    BSLS_ASSERT(alignment <= bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT);

    int offset = bsls_AlignmentUtil::calculateAlignmentOffset(buffer + *cursor,
                                                              alignment);

    if (*cursor + offset + size > bufSize) { // insufficient space
        return (void *) 0;
    }

    void *result = &buffer[*cursor + offset];
    *cursor += offset + size;

    return result;
}

void *bslma_BufferAllocator::allocateFromBuffer(int               *cursor,
                                                char              *buffer,
                                                size_type          bufSize,
                                                size_type          size,
                                                AlignmentStrategy  strategy)
{
    BSLS_ASSERT(cursor);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= bufSize);
    BSLS_ASSERT(0 <= size);

    return (0 >= size)
           ? (void *) 0
           : allocateFromBufferImp(cursor, buffer, bufSize, size,
                         (strategy == NATURAL_ALIGNMENT)
                         ? bsls_AlignmentUtil::calculateAlignmentFromSize(size)
                         : bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT);
}

void *bslma_BufferAllocator::allocateFromBuffer(int       *cursor,
                                                char      *buffer,
                                                size_type  bufSize,
                                                size_type  size,
                                                int        alignment)
{
    BSLS_ASSERT(cursor);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= bufSize);
    BSLS_ASSERT(0 <= size);
    BSLS_ASSERT(0 < alignment);
    BSLS_ASSERT(alignment <= bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT);

    return (0 >= size)
           ? (void *) 0
           : allocateFromBufferImp(cursor, buffer, bufSize, size, alignment);
}

bslma_BufferAllocator::~bslma_BufferAllocator()
{
}

void *bslma_BufferAllocator::allocate(size_type size)
{
    BSLS_ASSERT(0 <= size);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(0 == size)) {
        return (void *)0;
    }

    void *result;
    if (d_strategy == NATURAL_ALIGNMENT) {
        result = allocateFromBufferImp(
                         &d_cursor,
                         d_buffer_p,
                         d_bufferSize,
                         size,
                         bsls_AlignmentUtil::calculateAlignmentFromSize(size));
    } else { // default is MAXIMUM_ALIGNMENT
        result = allocateFromBufferImp(&d_cursor,
                                       d_buffer_p,
                                       d_bufferSize,
                                       size,
                                       bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT);
    }
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(result)) {
        return result;
    }
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(d_allocCallback)) {
        return (*d_allocCallback)(size);
    }

    // Throw 'std::bad_alloc' if cannot satisfy request.
    bslma_Allocator::throwBadAlloc();
    return result;
}

void bslma_BufferAllocator::print() const
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
                (void *) d_buffer_p,
                d_bufferSize,
                d_cursor,
                u.d_p,
                (bslma_BufferAllocator::MAXIMUM_ALIGNMENT == d_strategy ?
                 "MAXIMUM_ALIGNMENT" : "NATURAL_ALIGNMENT"));

    std::fflush(stdout);
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
