// bdlma_buffermanager.cpp                                            -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

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
