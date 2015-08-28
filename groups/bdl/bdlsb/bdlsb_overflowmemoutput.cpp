// bdlsb_overflowmemoutput.cpp                                        -*-C++-*-
#include <bdlsb_overflowmemoutput.h>

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
void OverflowMemOutput::grow(int n)
{
    BSLS_ASSERT(0 <= n);

    int newSize;
    newSize = 0 == d_overflowBufferSize ? d_initialBufferSize
                                        : d_overflowBufferSize;
    do {
        newSize *= 2;
    } while ((newSize - d_overflowBufferSize) < n);

    char *newBuffer =
                    reinterpret_cast<char *>(d_allocator_p->allocate(newSize));

    bsl::memcpy(newBuffer, d_overflowBuffer_p, d_overflowBufferSize);
    d_allocator_p->deallocate(d_overflowBuffer_p);

    d_overflowBuffer_p = newBuffer;
    d_overflowBufferSize = newSize;
}

// CREATORS
OverflowMemOutput::OverflowMemOutput(
                                              char             *buffer,
                                              int               size,
                                              bslma::Allocator *basicAllocator)
: d_dataLength(0)
, d_put_p(buffer)
, d_initialBuffer_p(buffer)
, d_initialBufferSize(size)
, d_inOverflowBufferFlag(false)
, d_overflowBuffer_p(0)
, d_overflowBufferSize(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < size);
}

// MANIPULATORS
bsl::locale OverflowMemOutput::pubimbue(const bsl::locale& loc)
{
    return loc;
}

bsl::streampos OverflowMemOutput::pubseekoff(bsl::streamoff     offset,
                                                   ios_base::seekdir  way,
                                                   ios_base::openmode which)
{
    bool gseek = which & bsl::ios_base::in;
    bool pseek = which & bsl::ios_base::out;

    if (!pseek || gseek) {
        return bsl::streampos(-1);                                    // RETURN
    }

    int totalSize = d_initialBufferSize + d_overflowBufferSize;
    bsl::streamoff newoff;
    switch (way) {
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
        return bsl::streampos(-1);                                    // RETURN
      }
    }

    newoff += offset;

    if (0 > newoff) {
        return -1;                                                    // RETURN
    }

    BSLS_ASSERT(0 <= newoff);

    if (newoff <= d_initialBufferSize) {
        // Move destination belong to initial buffer.

        d_inOverflowBufferFlag = false;
        d_put_p = d_initialBuffer_p + newoff;
    }
    else {
        // Move destination belongs to overflow buffer.

        d_inOverflowBufferFlag = true;

        if (totalSize >= (int) newoff) {
            d_put_p = d_overflowBuffer_p + newoff - d_initialBufferSize;
        }
        else {
            grow(static_cast<int>(newoff - totalSize));
            BSLS_ASSERT(newoff <= d_initialBufferSize + d_overflowBufferSize);
            d_put_p = d_overflowBuffer_p + newoff - d_initialBufferSize;
        }
    }
    d_dataLength = static_cast<int>(newoff);
    return newoff;
}

int OverflowMemOutput::sputc(char c)
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
    d_dataLength += 1;
    return c;
}

bsl::streamsize OverflowMemOutput::sputn(const char      *source,
                                               bsl::streamsize  numChars)
{
    BSLS_ASSERT(source);
    BSLS_ASSERT(0 <= numChars);

    int numBytesForLastCopy = numChars;
    if (d_inOverflowBufferFlag) {
        if ((d_put_p - d_overflowBuffer_p + numChars) >
                                                      d_overflowBufferSize) {
            grow(numChars);  // this is potentially more than necessary
            d_put_p = d_overflowBuffer_p + d_dataLength - d_initialBufferSize;
        }
    }
    else if (d_dataLength + numChars > d_initialBufferSize) {
        if (d_dataLength + numChars >
                                  d_initialBufferSize + d_overflowBufferSize) {
            grow(numChars);
        }
        d_inOverflowBufferFlag = true;

        int firstCopyBytes = d_initialBufferSize -
                                                 (d_put_p - d_initialBuffer_p);
        BSLS_ASSERT(firstCopyBytes >= 0);
        BSLS_ASSERT(firstCopyBytes < numChars);

        bsl::memcpy(d_put_p, source, firstCopyBytes);
        source += firstCopyBytes;
        numBytesForLastCopy -= firstCopyBytes;
        d_put_p = d_overflowBuffer_p;
    }

    bsl::memcpy(d_put_p, source, numBytesForLastCopy);
    d_put_p += numBytesForLastCopy;
    d_dataLength += numChars;

    return numChars;
}

// ACCESSORS
bsl::locale OverflowMemOutput::getloc() const
{
    return bsl::locale();
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
