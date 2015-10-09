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
