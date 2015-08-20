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
                                 bsl::ios_base::seekdir  fixedPosition,
                                 bsl::ios_base::openmode which)
{
    privateSync();

    bsl::size_t length = static_cast<bsl::size_t>(pptr() - pbase());
    if (d_inOverflowBufferFlag) {
        BSLS_ASSERT(length == (d_dataLength - d_initialBufferSize));
    }
    else {
        BSLS_ASSERT(length == d_dataLength);
    }

    bool gseek = which & bsl::ios_base::in;
    bool pseek = which & bsl::ios_base::out;

    if (!pseek || gseek) {
        return bsl::streambuf::pos_type(-1);                          // RETURN
    }

    bsl::size_t totalSize = d_initialBufferSize + d_overflowBufferSize;

    bsl::streambuf::off_type newoff;
    switch (fixedPosition) {
      case bsl::ios_base::beg: {
        newoff = 0;
      } break;
      case bsl::ios_base::cur: {
        newoff = d_dataLength;
      } break;
      case bsl::ios_base::end: {
        newoff = totalSize;
      } break;
      default: {
        return bsl::streambuf::pos_type(-1);                          // RETURN
      }
    }

    newoff += offset;

    if (0 > newoff) {
        return -1;                                                    // RETURN
    }


    if (static_cast<bsl::size_t>(newoff) <= d_initialBufferSize) {
        // Move destination is in initial buffer.

        if (!d_inOverflowBufferFlag)  {
            pbump(static_cast<int>(newoff - d_dataLength));
        }
        else {
            d_inOverflowBufferFlag = false;
            setp(d_initialBuffer_p, d_initialBuffer_p + d_initialBufferSize);
            pbump(static_cast<int>(newoff));
        }
    }
    else {
        // Move destination is in overflow buffer.

        if (totalSize < static_cast<bsl::size_t>(newoff)) {
            grow(static_cast<int>(newoff) - totalSize);
            d_inOverflowBufferFlag = true;
            setp(d_overflowBuffer_p,
                 d_overflowBuffer_p + d_overflowBufferSize);
            pbump(static_cast<int>(newoff - d_initialBufferSize));
        }
        else {
            if (d_inOverflowBufferFlag) {
                pbump(static_cast<int>(newoff - d_dataLength));
            }
            else {
                d_inOverflowBufferFlag = true;
                setp(d_overflowBuffer_p,
                     d_overflowBuffer_p + d_overflowBufferSize);
                pbump(static_cast<int>(newoff - d_initialBufferSize));
            }
        }
    }

    d_dataLength = static_cast<bsl::size_t>(newoff);
    return newoff;
}

bsl::streamsize
OverflowMemOutStreamBuf::xsputn(const char_type *source,
                                bsl::streamsize  numChars)
{
    BSLS_ASSERT(source);
    BSLS_ASSERT(0 <= numChars);

    privateSync();

    bsl::streamsize numBytesForLastCopy = numChars;

    bsl::size_t newDataLength = d_dataLength + numChars;

    if (d_inOverflowBufferFlag) {
        if (newDataLength > d_initialBufferSize + d_overflowBufferSize) {
            grow(numChars); // this is potentially more than necessary
            setp(d_overflowBuffer_p,
                 d_overflowBuffer_p + d_overflowBufferSize);
            pbump(static_cast<int>(d_dataLength - d_initialBufferSize));
        }
    }
    else if (newDataLength > d_initialBufferSize) {

        if (newDataLength > d_initialBufferSize + d_overflowBufferSize) {
            grow(numChars);
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
