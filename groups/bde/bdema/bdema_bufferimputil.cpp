// bdema_bufferimputil.cpp                                            -*-C++-*-
#include <bdema_bufferimputil.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdema_bufferimputil_cpp,"$Id$ $CSID$")

#include <bsls_alignmentutil.h>
#include <bsls_assert.h>

namespace BloombergLP {

                        // --------------------------
                        // struct bdema_BufferImpUtil
                        // --------------------------

// CLASS METHODS
void *bdema_BufferImpUtil::allocateFromBuffer(
                                          int                      *cursor,
                                          char                     *buffer,
                                          int                       bufferSize,
                                          int                       size,
                                          bsls_Alignment::Strategy  strategy)
{
    BSLS_ASSERT(cursor);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= bufferSize);
    BSLS_ASSERT(0 < size);
    BSLS_ASSERT(0 <= *cursor);
    BSLS_ASSERT(*cursor <= bufferSize);

    const int alignment = strategy == bsls_Alignment::BSLS_NATURAL
                        ? bsls_AlignmentUtil::calculateAlignmentFromSize(size)
                        : bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT;

    const int offset = bsls_AlignmentUtil::calculateAlignmentOffset(
                                                              buffer + *cursor,
                                                              alignment);

    if (*cursor + offset + size > bufferSize) {
        return 0;
    }

    void *result = &buffer[*cursor + offset];
    *cursor += offset + size;

    return result;
}

void *bdema_BufferImpUtil::allocateFromBufferRaw(
                                            int                      *cursor,
                                            char                     *buffer,
                                            int                       size,
                                            bsls_Alignment::Strategy  strategy)
{
    BSLS_ASSERT(cursor);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < size);

    const int alignment = strategy == bsls_Alignment::BSLS_NATURAL
                        ? bsls_AlignmentUtil::calculateAlignmentFromSize(size)
                        : bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT;

    const int offset = bsls_AlignmentUtil::calculateAlignmentOffset(
                                                              buffer + *cursor,
                                                              alignment);

    void *result = &buffer[*cursor + offset];
    *cursor += offset + size;

    return result;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
