// bdlsb_overflowmemoutput.cpp                                        -*-C++-*-
#include <bdlsb_overflowmemoutput.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlsb_overflowmemoutput_cpp,"$Id$ $CSID$")

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

#include <bsl_cstring.h>

namespace BloombergLP {
namespace bdlsb {

                          // -----------------------
                          // class OverflowMemOutput
                          // -----------------------

// PRIVATE MANIPULATORS
void OverflowMemOutput::grow(bsl::size_t numBytes,
                             bool        copyOrigin)
{
    bsl::size_t newSize = d_overflowBufferSize ? d_overflowBufferSize*2
                                               : d_initialBufferSize;

    while (newSize < (d_overflowBufferSize + numBytes)) {
        newSize <<= 1;
    }

    char *newBuffer =
                    reinterpret_cast<char *>(d_allocator_p->allocate(newSize));

    // copyOrigin flag is a caller hint to skip content copy, because caller
    // will completely overwrite the original content.
    if (copyOrigin) {
        bsl::memcpy(newBuffer, d_overflowBuffer_p, d_overflowBufferSize);
    }
    d_allocator_p->deallocate(d_overflowBuffer_p);

    d_overflowBuffer_p = newBuffer;
    d_overflowBufferSize = newSize;
}

// CREATORS
OverflowMemOutput::OverflowMemOutput(char             *buffer,
                                     bsl::size_t       length,
                                     bslma::Allocator *basicAllocator)
: d_dataLength(0)
, d_put_p(buffer)
, d_initialBuffer_p(buffer)
, d_initialBufferSize(length)
, d_inOverflowBufferFlag(false)
, d_overflowBuffer_p(0)
, d_overflowBufferSize(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < length);
}

// MANIPULATORS
OverflowMemOutput::pos_type
OverflowMemOutput::pubseekoff(off_type                offset,
                              bsl::ios_base::seekdir  way,
                              bsl::ios_base::openmode which)
{
    if (!(which & bsl::ios_base::out)) {
        return pos_type(-1);                                          // RETURN
    }

    bsl::size_t totalSize = d_initialBufferSize + d_overflowBufferSize;
    off_type    newOffset;

    switch (way) {
      case bsl::ios_base::beg: {
        newOffset = 0;
      } break;
      case bsl::ios_base::cur: {
        newOffset = d_dataLength;
      } break;
      case bsl::ios_base::end: {
        newOffset = totalSize;
      } break;
      default: {
        return pos_type(-1);                                          // RETURN
      }
    }

    newOffset += offset;

    if (0 > newOffset) {
        return pos_type(-1);                                          // RETURN
    }

    bsl::size_t finalOffset = static_cast<bsl::size_t>(newOffset);

    if (finalOffset <= d_initialBufferSize) {
        // Final absolute position is in the initial buffer.
        d_inOverflowBufferFlag = false;
        d_put_p = d_initialBuffer_p + finalOffset;
    } else {
        // Final absolute position is in the overflow buffer.
        d_inOverflowBufferFlag = true;

        if (totalSize >= finalOffset) {
            d_put_p = d_overflowBuffer_p + finalOffset - d_initialBufferSize;
        }
        else {
            grow(finalOffset - totalSize);
            BSLS_ASSERT(finalOffset <=
                                   d_initialBufferSize + d_overflowBufferSize);
            d_put_p = d_overflowBuffer_p + finalOffset - d_initialBufferSize;
        }
    }
    d_dataLength = finalOffset;
    return newOffset;
}

OverflowMemOutput::int_type OverflowMemOutput::sputc(char c)
{
    if (d_put_p == (d_initialBuffer_p + d_initialBufferSize)) {
        if (0 == d_overflowBufferSize) {
            grow(1);
        }
        d_put_p = d_overflowBuffer_p;
        d_inOverflowBufferFlag = true;
    }
    else if (d_put_p == (d_overflowBuffer_p  + d_overflowBufferSize)) {
        grow(1);
        d_put_p = d_overflowBuffer_p + d_dataLength - d_initialBufferSize;
        d_inOverflowBufferFlag = true;
    }

    *(d_put_p++) = c;
    ++d_dataLength;
    return traits_type::to_int_type(c);
}

bsl::streamsize OverflowMemOutput::sputn(const char      *source,
                                         bsl::streamsize  length)
{
    BSLS_ASSERT(source || 0 == length);
    BSLS_ASSERT(0 <= length);

    if (length == 0) {
        return length;                                                // RETURN
    }

    bsl::size_t numBytesForLastCopy = static_cast<bsl::size_t>(length);

    bsl::streamsize numBytesNeeded = length + d_dataLength
                                     - d_initialBufferSize
                                     - d_overflowBufferSize;
    if (d_inOverflowBufferFlag) {
        if (numBytesNeeded > 0) {
            grow(numBytesNeeded);
            d_put_p = d_overflowBuffer_p + d_dataLength - d_initialBufferSize;
        }
    } else {
        if (d_dataLength + length > d_initialBufferSize) {
            if (numBytesNeeded > 0) {
                // This is the case where we will definitely overwrite the
                // content of the existing overflow buffer, thus skipping copy
                // in the 'grow'
                grow(numBytesNeeded, false);
            }

            bsl::size_t firstCopyBytes = d_initialBufferSize - d_dataLength;

            bsl::memcpy(d_put_p, source, firstCopyBytes);
            source += firstCopyBytes;
            numBytesForLastCopy -= firstCopyBytes;
            d_put_p = d_overflowBuffer_p;
            d_inOverflowBufferFlag = true;
        }
    }

    bsl::memcpy(d_put_p, source, numBytesForLastCopy);
    d_put_p += numBytesForLastCopy;
    d_dataLength += length;

    return length;
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
