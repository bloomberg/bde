// bdlma_buffermanager.cpp                                            -*-C++-*-
#include <bdlma_buffermanager.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlma_buffermanager_cpp,"$Id$ $CSID$")

#include <bdlma_bufferimputil.h>

namespace BloombergLP {
namespace bdlma {

                        // -------------------
                        // class BufferManager
                        // -------------------

// PRIVATE MANIPULATORS
void BufferManager::init(bsls::Alignment::Strategy strategy)
{
    switch (strategy) {
      case bsls::Alignment::BSLS_MAXIMUM: {
        d_allocate_p    = &BufferImpUtil::allocateMaximallyAlignedFromBuffer;
        d_allocateRaw_p =
                         &BufferImpUtil::allocateMaximallyAlignedFromBufferRaw;
      } break;
      case bsls::Alignment::BSLS_NATURAL: {
        d_allocate_p    = &BufferImpUtil::allocateNaturallyAlignedFromBuffer;
        d_allocateRaw_p =
                         &BufferImpUtil::allocateNaturallyAlignedFromBufferRaw;
      } break;
      case bsls::Alignment::BSLS_BYTEALIGNED: {
        d_allocate_p    = &BufferImpUtil::allocateOneByteAlignedFromBuffer;
        d_allocateRaw_p = &BufferImpUtil::allocateOneByteAlignedFromBufferRaw;
      } break;
      default: {
        BSLS_ASSERT_OPT(0 && "Invalid alignment 'strategy' value.");
      } break;
    }
}

// MANIPULATORS
int BufferManager::expand(void *address, int size)
{
    BSLS_ASSERT(address);
    BSLS_ASSERT(0 < size);
    BSLS_ASSERT(d_buffer_p);
    BSLS_ASSERT(0 <= d_cursor);
    BSLS_ASSERT(d_cursor <= d_bufferSize);

    if (static_cast<char *>(address) + size == d_buffer_p + d_cursor) {
        const int newSize = size + d_bufferSize - d_cursor;
        d_cursor = d_bufferSize;

        return newSize;                                               // RETURN
    }

    return size;
}

int BufferManager::truncate(void *address, int originalSize, int newSize)
{
    BSLS_ASSERT(address);
    BSLS_ASSERT(0 <= newSize);
    BSLS_ASSERT(newSize <= originalSize);
    BSLS_ASSERT(d_buffer_p);
    BSLS_ASSERT(0 <= d_cursor);
    BSLS_ASSERT(d_cursor <= d_bufferSize);

    if (static_cast<char *>(address) + originalSize == d_buffer_p + d_cursor) {
        d_cursor -= originalSize - newSize;
        return newSize;                                               // RETURN
    }

    return originalSize;
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
