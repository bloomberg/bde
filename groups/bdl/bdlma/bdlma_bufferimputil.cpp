// bdlma_bufferimputil.cpp                                            -*-C++-*-
#include <bdlma_bufferimputil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlma_bufferimputil_cpp,"$Id$ $CSID$")

#include <bsls_alignmentutil.h>
#include <bsls_assert.h>

namespace BloombergLP {
namespace bdlma {

                        // --------------------
                        // struct BufferImpUtil
                        // --------------------

// CLASS METHODS
void *BufferImpUtil::allocateFromBuffer(int                       *cursor,
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

    void *result = 0;

    switch (strategy) {
      case bsls::Alignment::BSLS_MAXIMUM: {
        result = BufferImpUtil::allocateMaximallyAlignedFromBuffer(cursor,
                                                                   buffer,
                                                                   bufferSize,
                                                                   size);
      } break;
      case bsls::Alignment::BSLS_NATURAL: {
        result = BufferImpUtil::allocateNaturallyAlignedFromBuffer(cursor,
                                                                   buffer,
                                                                   bufferSize,
                                                                   size);
      } break;
      case bsls::Alignment::BSLS_BYTEALIGNED: {
        result = BufferImpUtil::allocateOneByteAlignedFromBuffer(cursor,
                                                                 buffer,
                                                                 bufferSize,
                                                                 size);
      } break;
      default: {
        BSLS_ASSERT_OPT(0 && "Invalid alignment 'strategy' value.");
      } break;
    }

    return result;
}

void *BufferImpUtil::allocateMaximallyAlignedFromBuffer(int  *cursor,
                                                        char *buffer,
                                                        int   bufferSize,
                                                        int   size)
{
    BSLS_ASSERT(cursor);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= bufferSize);
    BSLS_ASSERT(0 < size);
    BSLS_ASSERT(0 <= *cursor);
    BSLS_ASSERT(*cursor <= bufferSize);

    const int offset = bsls::AlignmentUtil::calculateAlignmentOffset(
                                      buffer + *cursor,
                                      bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT);

    if (*cursor + offset + size > bufferSize) {
        return 0;                                                     // RETURN
    }

    void *result = &buffer[*cursor + offset];
    *cursor += offset + size;

    return result;
}

void *BufferImpUtil::allocateNaturallyAlignedFromBuffer(int  *cursor,
                                                        char *buffer,
                                                        int   bufferSize,
                                                        int   size)
{
    BSLS_ASSERT(cursor);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= bufferSize);
    BSLS_ASSERT(0 < size);
    BSLS_ASSERT(0 <= *cursor);
    BSLS_ASSERT(*cursor <= bufferSize);

    const int alignment = bsls::AlignmentUtil::calculateAlignmentFromSize(
                                                                         size);

    const int offset    = bsls::AlignmentUtil::calculateAlignmentOffset(
                                                              buffer + *cursor,
                                                              alignment);

    if (*cursor + offset + size > bufferSize) {
        return 0;                                                     // RETURN
    }

    void *result = &buffer[*cursor + offset];
    *cursor += offset + size;

    return result;
}

void *BufferImpUtil::allocateOneByteAlignedFromBuffer(int  *cursor,
                                                      char *buffer,
                                                      int   bufferSize,
                                                      int   size)
{
    BSLS_ASSERT(cursor);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= bufferSize);
    BSLS_ASSERT(0 < size);
    BSLS_ASSERT(0 <= *cursor);
    BSLS_ASSERT(*cursor <= bufferSize);

    if (*cursor + size > bufferSize) {
        return 0;                                                     // RETURN
    }

    void *result = &buffer[*cursor];
    *cursor += size;

    return result;
}

void *BufferImpUtil::allocateFromBufferRaw(int                       *cursor,
                                           char                      *buffer,
                                           int                        size,
                                           bsls::Alignment::Strategy  strategy)
{
    BSLS_ASSERT(cursor);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < size);
    BSLS_ASSERT(0 <= *cursor);

    void *result = 0;

    switch (strategy) {
      case bsls::Alignment::BSLS_MAXIMUM: {
        result = BufferImpUtil::allocateMaximallyAlignedFromBufferRaw(cursor,
                                                                      buffer,
                                                                      size);
      } break;
      case bsls::Alignment::BSLS_NATURAL: {
        result = BufferImpUtil::allocateNaturallyAlignedFromBufferRaw(cursor,
                                                                      buffer,
                                                                      size);
      } break;
      case bsls::Alignment::BSLS_BYTEALIGNED: {
        result = BufferImpUtil::allocateOneByteAlignedFromBufferRaw(cursor,
                                                                    buffer,
                                                                    size);
      } break;
      default: {
        BSLS_ASSERT_OPT(0 && "Invalid alignment 'strategy' value.");
      } break;
    }

    return result;
}

void *BufferImpUtil::allocateMaximallyAlignedFromBufferRaw(int  *cursor,
                                                           char *buffer,
                                                           int   size)
{
    BSLS_ASSERT(cursor);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < size);
    BSLS_ASSERT(0 <= *cursor);

    const int offset = bsls::AlignmentUtil::calculateAlignmentOffset(
                                      buffer + *cursor,
                                      bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT);

    void *result = &buffer[*cursor + offset];
    *cursor += offset + size;

    return result;
}

void *BufferImpUtil::allocateNaturallyAlignedFromBufferRaw(int  *cursor,
                                                           char *buffer,
                                                           int   size)
{
    BSLS_ASSERT(cursor);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < size);
    BSLS_ASSERT(0 <= *cursor);

    const int alignment = bsls::AlignmentUtil::calculateAlignmentFromSize(
                                                                         size);

    const int offset    = bsls::AlignmentUtil::calculateAlignmentOffset(
                                                              buffer + *cursor,
                                                              alignment);

    void *result = &buffer[*cursor + offset];
    *cursor += offset + size;

    return result;
}

void *BufferImpUtil::allocateOneByteAlignedFromBufferRaw(int  *cursor,
                                                         char *buffer,
                                                         int   size)
{
    BSLS_ASSERT(cursor);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < size);
    BSLS_ASSERT(0 <= *cursor);

    void *result = &buffer[*cursor];
    *cursor += size;

    return result;
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
