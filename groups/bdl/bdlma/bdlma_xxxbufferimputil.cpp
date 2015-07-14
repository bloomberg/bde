// bdlma_xxxbufferimputil.cpp                                            -*-C++-*-
#include <bdlma_xxxbufferimputil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlma_xxxbufferimputil_cpp,"$Id$ $CSID$")

#include <bsls_alignmentutil.h>
#include <bsls_assert.h>

namespace BloombergLP {

namespace bdlma {
                        // --------------------------
                        // struct BufferImpUtil
                        // --------------------------

// CLASS METHODS
void *BufferImpUtil::allocateFromBuffer(
                                         int                       *cursor,
                                         char                      *buffer,
                                         int                        bufferSize,
                                         int                        size,
                                         bsls::Alignment::Strategy  strategy)
{
    BSLS_ASSERT(cursor);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= bufferSize);
    BSLS_ASSERT(0 < size);
    BSLS_ASSERT(0 <= *cursor);
    BSLS_ASSERT(*cursor <= bufferSize);

    const int alignment = strategy == bsls::Alignment::BSLS_NATURAL
                        ? bsls::AlignmentUtil::calculateAlignmentFromSize(size)
                        : bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT;

    const int offset = bsls::AlignmentUtil::calculateAlignmentOffset(
                                                              buffer + *cursor,
                                                              alignment);

    if (*cursor + offset + size > bufferSize) {
        return 0;
    }

    void *result = &buffer[*cursor + offset];
    *cursor += offset + size;

    return result;
}

void *BufferImpUtil::allocateFromBufferRaw(
                                           int                       *cursor,
                                           char                      *buffer,
                                           int                        size,
                                           bsls::Alignment::Strategy  strategy)
{
    BSLS_ASSERT(cursor);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < size);

    const int alignment = strategy == bsls::Alignment::BSLS_NATURAL
                        ? bsls::AlignmentUtil::calculateAlignmentFromSize(size)
                        : bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT;

    const int offset = bsls::AlignmentUtil::calculateAlignmentOffset(
                                                              buffer + *cursor,
                                                              alignment);

    void *result = &buffer[*cursor + offset];
    *cursor += offset + size;

    return result;
}
}  // close package namespace

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
