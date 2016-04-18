// bdlsb_overflowmemoutstreambuf.cpp                                  -*-C++-*-
#include <bdlsb_overflowmemoutstreambuf.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlsb_overflowmemoutstreambuf_cpp,"$Id$ $CSID$")

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

#include <bsl_cstdio.h>
#include <bsl_cstring.h>
#include <bsl_locale.h>

namespace BloombergLP {
namespace bdlsb {

                    // -----------------------------
                    // class OverflowMemOutStreamBuf
                    // -----------------------------

// PRIVATE MANIPULATORS
void OverflowMemOutStreamBuf::grow(bsl::size_t numBytes)
{
    bsl::size_t newSize =  d_overflowBufferSize ? d_overflowBufferSize
                                                : d_initialBufferSize;

    while ((newSize - d_overflowBufferSize) < numBytes) {
        newSize *= 2;
    }

    char *newBuffer =
                    reinterpret_cast<char *>(d_allocator_p->allocate(newSize));

    bsl::memcpy(newBuffer, d_overflowBuffer_p, d_overflowBufferSize);
    d_allocator_p->deallocate(d_overflowBuffer_p);

    d_overflowBuffer_p = newBuffer;
    d_overflowBufferSize = newSize;
}

void OverflowMemOutStreamBuf::privateSync() const
{
    if (d_inOverflowBufferFlag) {
        d_dataLength = pptr() - pbase() + d_initialBufferSize;
    }
    else {
        d_dataLength = pptr() - pbase();
    }
}


// PROTECTED MANIPULATORS
OverflowMemOutStreamBuf::int_type
OverflowMemOutStreamBuf::overflow(OverflowMemOutStreamBuf::int_type c)
{
    if (EOF == c) {
        return traits_type::not_eof(c);                               // RETURN
    }

    privateSync();

    if (0 == d_overflowBufferSize || d_inOverflowBufferFlag) {
        grow(1);
    }

    setp(d_overflowBuffer_p,
         d_overflowBuffer_p + d_overflowBufferSize);
    pbump(static_cast<int>(d_dataLength - d_initialBufferSize));
    d_inOverflowBufferFlag = true;

    BSLS_ASSERT(pptr() != epptr());

    *pptr() = static_cast<char_type>(c);
    pbump(1);
    ++d_dataLength;
    return c;
}

OverflowMemOutStreamBuf::pos_type
OverflowMemOutStreamBuf::seekoff(off_type                offset,
                                 bsl::ios_base::seekdir  way,
                                 bsl::ios_base::openmode which)
{
    if (!(which & bsl::ios_base::out)) {
        return pos_type(-1);                                          // RETURN
    }

    privateSync();

    bsl::size_t length = static_cast<bsl::size_t>(pptr() - pbase());
    (void)length;

    BSLS_ASSERT(length ==
            d_inOverflowBufferFlag ? (d_dataLength - d_initialBufferSize)
                                   : d_dataLength);

    bsl::size_t totalCapacity = d_initialBufferSize + d_overflowBufferSize;

    off_type newOffset;

    switch (way) {
      case bsl::ios_base::beg: {
        newOffset = 0;
      } break;
      case bsl::ios_base::cur: {
        newOffset = d_dataLength;
      } break;
      case bsl::ios_base::end: {
        newOffset = totalCapacity;
      } break;
      default: {
        return pos_type(-1);                                          // RETURN
      }
    }

    newOffset += offset;

    if (0 > newOffset) {
        return pos_type(-1);                                          // RETURN
    }

    if (static_cast<bsl::size_t>(newOffset) <= d_initialBufferSize) {
        // Final absolute position is in initial buffer.
        if (!d_inOverflowBufferFlag)  {
            pbump(static_cast<int>(newOffset - d_dataLength));
        }
        else {
            d_inOverflowBufferFlag = false;
            setp(d_initialBuffer_p, d_initialBuffer_p + d_initialBufferSize);
            pbump(static_cast<int>(newOffset));
        }
    }
    else {
        // Final absolute position is in overflow buffer.
        if (totalCapacity < static_cast<bsl::size_t>(newOffset)) {
            grow(static_cast<bsl::size_t>(newOffset) - totalCapacity);
            d_inOverflowBufferFlag = true;
            setp(d_overflowBuffer_p,
                 d_overflowBuffer_p + d_overflowBufferSize);
            pbump(static_cast<int>(newOffset - d_initialBufferSize));
        }
        else {
            if (d_inOverflowBufferFlag) {
                pbump(static_cast<int>(newOffset - d_dataLength));
            }
            else {
                d_inOverflowBufferFlag = true;
                setp(d_overflowBuffer_p,
                     d_overflowBuffer_p + d_overflowBufferSize);
                pbump(static_cast<int>(newOffset - d_initialBufferSize));
            }
        }
    }

    d_dataLength = static_cast<bsl::size_t>(newOffset);
    return newOffset;
}

bsl::streamsize
OverflowMemOutStreamBuf::xsputn(const char_type *source,
                                bsl::streamsize  numChars)
{
    BSLS_ASSERT(( source && 0 < numChars) || 0 == numChars);

    if (0 == numChars) {
        return numChars;                                              // RETURN
    }

    privateSync();

    bsl::size_t numBytesForLastCopy = static_cast<bsl::size_t>(numChars);

    bsl::streamsize numBytesNeeded = numChars + d_dataLength
                                   - d_initialBufferSize
                                   - d_overflowBufferSize;

    if (d_inOverflowBufferFlag) {
        if (numBytesNeeded > 0) {
            grow(numBytesNeeded);
            setp(d_overflowBuffer_p,
                 d_overflowBuffer_p + d_overflowBufferSize);
            pbump(static_cast<int>(d_dataLength - d_initialBufferSize));
        }
    } else {
        if (d_dataLength + numChars > d_initialBufferSize) {
            if (numBytesNeeded > 0) {
                grow(numBytesNeeded);
            }

            bsl::size_t firstCopyBytes = d_initialBufferSize - d_dataLength;

            BSLS_ASSERT(firstCopyBytes < static_cast<bsl::size_t>(numChars));

            bsl::memcpy(pptr(), source, firstCopyBytes);
            source += firstCopyBytes;
            numBytesForLastCopy -= firstCopyBytes;
            d_inOverflowBufferFlag = true;
            setp(d_overflowBuffer_p,
                    d_overflowBuffer_p + d_overflowBufferSize);
        }
    }

    d_dataLength += numChars;
    bsl::memcpy(pptr(), source, numBytesForLastCopy);
    pbump(static_cast<int>(numBytesForLastCopy));
    return numChars;
}

// CREATORS
OverflowMemOutStreamBuf::OverflowMemOutStreamBuf(
                                              char             *buffer,
                                              bsl::size_t       size,
                                              bslma::Allocator *basicAllocator)
: d_dataLength(0)
, d_initialBuffer_p(buffer)
, d_initialBufferSize(size)
, d_inOverflowBufferFlag(false)
, d_overflowBuffer_p(0)
, d_overflowBufferSize(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < size);

    setp(d_initialBuffer_p, d_initialBuffer_p + d_initialBufferSize);
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
