// btlb_blobstreambuf.cpp                                             -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <btlb_blobstreambuf.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlb_blobstreambuf_cpp,"$Id$ $CSID$")

#include <btlb_blob.h>

#include <bsls_assert.h>

#include <bsl_algorithm.h>
#include <bsl_cstdio.h>
#include <bsl_cstring.h>
#include <bsl_string.h>

// Note: on Windows -> WinDef.h:#define min(a,b) ...
#if defined(BSLS_PLATFORM_CMP_MSVC) && defined(min)
#undef min
#endif

namespace BloombergLP {
namespace btlb {

                          // =====================
                          // class InBlobStreamBuf
                          // =====================

// PRIVATE MANIPULATORS
void InBlobStreamBuf::setGetPosition(bsl::size_t position)
{
    BSLS_ASSERT(position <= (unsigned)d_blob_p->length());
    if (d_blob_p->length() == 0) {
        setg(0, 0, 0);
        return;                                                       // RETURN
    }

    if (egptr() == eback()) {
        // Initialization.  Buffer now has a length but we did not have the
        // chance to actually initialize the streambuf pointers.

        BSLS_ASSERT(d_blob_p->numBuffers() != 0);
        setg(d_blob_p->buffer(0).data(), d_blob_p->buffer(0).data(),
             d_blob_p->buffer(0).data() +
                     bsl::min(d_blob_p->buffer(0).size(), d_blob_p->length()));
    }

    int maxBufPos = egptr() - eback() + d_previousBuffersLength;
    if (((unsigned)maxBufPos > position &&
            (unsigned)d_previousBuffersLength <= position)
      || ((unsigned)maxBufPos == position &&
             position == (unsigned)d_blob_p->length())) {
        // We are not crossing any buffer boundaries.

        BSLS_ASSERT(position >= (unsigned)d_previousBuffersLength);
        BSLS_ASSERT((position - d_previousBuffersLength) <=
                          (unsigned)d_blob_p->buffer(d_getBufferIndex).size());
        setg(eback(), eback() + position - d_previousBuffersLength, egptr());
        return;                                                       // RETURN
    }

    BSLS_ASSERT(position != (unsigned)d_previousBuffersLength);

    if (position > (unsigned)d_previousBuffersLength) {
        // We are moving forward.

        int left = position - (d_previousBuffersLength +
                               d_blob_p->buffer(d_getBufferIndex).size());
        do {
            d_previousBuffersLength +=
                                     d_blob_p->buffer(d_getBufferIndex).size();
            ++d_getBufferIndex;
            left -= d_blob_p->buffer(d_getBufferIndex).size();
        } while (left > 0);
    }
    else {
        // We are moving backwards

        int left = d_previousBuffersLength - position;
        do {
            --d_getBufferIndex;
            d_previousBuffersLength -=
                                     d_blob_p->buffer(d_getBufferIndex).size();
            left -= d_blob_p->buffer(d_getBufferIndex).size();
        } while (left > 0);
    }

    BSLS_ASSERT(position >= (unsigned)d_previousBuffersLength);

    char *base = d_blob_p->buffer(d_getBufferIndex).data();

    setg(base, base + position - d_previousBuffersLength,
         base + bsl::min(d_blob_p->buffer(d_getBufferIndex).size(),
                         d_blob_p->length() - d_previousBuffersLength));
}

// PRIVATE ACCESSORS
int InBlobStreamBuf::checkInvariant() const
{
    bsl::size_t numBuffers = d_blob_p->numBuffers();

    if (gptr()) {
        BSLS_ASSERT(eback());
        BSLS_ASSERT(egptr());
        BSLS_ASSERT(gptr() - eback() <= egptr()-eback());
        BSLS_ASSERT((unsigned)d_getBufferIndex < numBuffers);
        BSLS_ASSERT(egptr() - eback() <=
                                  d_blob_p->buffer(d_getBufferIndex).size());
        BSLS_ASSERT(d_previousBuffersLength + egptr() - eback() <=
                                                           d_blob_p->length());
    }
    else {
        BSLS_ASSERT(0 == eback());
        BSLS_ASSERT(0 == egptr());
        BSLS_ASSERT((unsigned)d_getBufferIndex <= numBuffers);
    }

    return 0;
}

// PROTECTED MANIPULATORS
InBlobStreamBuf::int_type
InBlobStreamBuf::overflow(InBlobStreamBuf::int_type)
{
    return traits_type::eof();
}

InBlobStreamBuf::int_type
InBlobStreamBuf::pbackfail(InBlobStreamBuf::int_type c)
{
    BSLS_ASSERT(checkInvariant() == 0);

    if (gptr() == eback()) {
        if (0 == d_getBufferIndex) {
             // No put-back position available.

            return traits_type::eof();                                // RETURN
        }
        else {
            d_getBufferIndex--;
            d_previousBuffersLength -=
                                     d_blob_p->buffer(d_getBufferIndex).size();

            char *gbuf = d_blob_p->buffer(d_getBufferIndex).data();

            int bufferLength = d_blob_p->buffer(d_getBufferIndex).size();
            setg(gbuf, gbuf + bufferLength, gbuf + bufferLength);
        }
    }

    gbump(-1);
    if (traits_type::eof() == c) {
        return ~traits_type::eof();                                   // RETURN
    }
    else {
        *gptr() = static_cast<char_type>(c);
        return c;                                                     // RETURN
    }
}

InBlobStreamBuf::pos_type
InBlobStreamBuf::seekpos(pos_type position, bsl::ios_base::openmode which)
{
    return seekoff(off_type(position), bsl::ios_base::beg, which);
}

InBlobStreamBuf::pos_type
InBlobStreamBuf::seekoff(off_type                offset,
                         bsl::ios_base::seekdir  fixedPosition,
                         bsl::ios_base::openmode which)
{
    BSLS_ASSERT(0 == checkInvariant());

    bool gseek = which & bsl::ios_base::in;
    bool pseek = which & bsl::ios_base::out;

    if (!gseek || pseek) {
        return pos_type(-1);                                          // RETURN
    }

    sync();
    bsl::size_t totalSize = d_blob_p->length();

    off_type newoff;
    switch (fixedPosition)
    {
        case bsl::ios_base::beg:
          newoff = 0;
          break;
        case bsl::ios_base::cur:
          newoff = d_previousBuffersLength + gptr() - eback();
          break;
        case bsl::ios_base::end:
          newoff = totalSize;
          break;
        default:
          return off_type(-1);                                        // RETURN
    }

    newoff += offset;
    if (newoff < 0 || totalSize < (unsigned)newoff) {
        return off_type(-1);                                          // RETURN
    }

    setGetPosition(static_cast<bsl::size_t>(newoff));

    return newoff;
}

bsl::streamsize InBlobStreamBuf::showmanyc()
{
    BSLS_ASSERT(0 == checkInvariant());

    return d_blob_p->length() - (d_previousBuffersLength + gptr() - eback());
}

int InBlobStreamBuf::sync()
{
    BSLS_ASSERT(0 == checkInvariant());
    return 0;
}

InBlobStreamBuf::int_type InBlobStreamBuf::underflow()
{
    BSLS_ASSERT(0 == checkInvariant());
    BSLS_ASSERT(egptr() == gptr());

    int totalSize = d_blob_p->length();
    int getPosition =  d_previousBuffersLength + gptr() - eback();

    if (getPosition >= totalSize) {
        BSLS_ASSERT(getPosition == totalSize);
        return EOF;                                                   // RETURN
    }

    bsl::size_t curOffset;
    if ((egptr() - eback()) == d_blob_p->buffer(d_getBufferIndex).size()) {
        // We're getting a new buffer.

        d_previousBuffersLength += d_blob_p->buffer(d_getBufferIndex).size();
        ++d_getBufferIndex;
        BSLS_ASSERT(d_getBufferIndex < d_blob_p->numBuffers());
        curOffset = 0; // start of the buffer.
    }
    else {
        // This is our offset in this buffer.  The point is that the length of
        // the underlying blob could have grown.

        curOffset = egptr() - eback();
    }

    char *gbuf = d_blob_p->buffer(d_getBufferIndex).data();
    bsl::size_t glen =  d_blob_p->buffer(d_getBufferIndex).size();

    // We need to figure out where to stop in that buffer.  If this is the last
    // buffer, we may have to stop before the end of the memory since it may
    // not be full at that time.

    bsl::size_t endOffset = bsl::min(totalSize - d_previousBuffersLength,
                                     (int)glen);

    BSLS_ASSERT(curOffset < endOffset);
    BSLS_ASSERT(endOffset <= glen);
    setg(gbuf, gbuf + curOffset, gbuf + endOffset);

    return traits_type::to_int_type(*gptr());
}

bsl::streamsize InBlobStreamBuf::xsgetn(char_type       *destination,
                                        bsl::streamsize  numChars)
{
    bsl::streamsize numLeft   = numChars;
    bsl::streamsize numCopied = 0;
    while (0 < numLeft) {
        bsl::streamsize remainingChars = egptr() - gptr();
        int canCopy = bsl::min(remainingChars, numLeft);

        bsl::memcpy(destination + numCopied, gptr(), canCopy);
        gbump(canCopy);
        numLeft -= canCopy;

        if (   0 < numLeft
            && gptr() == egptr()
            && traits_type::eof() == underflow()) {
            return numCopied + canCopy;                               // RETURN
        }
        numCopied += canCopy;
    }
    return numCopied;
}

bsl::streamsize InBlobStreamBuf::xsputn(const char_type *, bsl::streamsize)
{
    return 0;
}

// CREATORS
InBlobStreamBuf::InBlobStreamBuf(const btlb::Blob *blob)
: d_blob_p(blob)
, d_getBufferIndex(0)
, d_previousBuffersLength(0)
{
    setGetPosition(0);
}

InBlobStreamBuf::~InBlobStreamBuf()
{
    BSLS_ASSERT(0 == checkInvariant());
}

                          // ======================
                          // class OutBlobStreamBuf
                          // ======================

// PRIVATE MANIPULATORS
void OutBlobStreamBuf::setPutPosition(bsl::size_t position)
{
    BSLS_ASSERT(position <= (unsigned)d_blob_p->totalSize());
    if (d_blob_p->totalSize() == 0) {
        setp(0, 0);
        return;                                                       // RETURN
    }

    if (epptr() == pbase()) {
        // Initialization.  Buffer now has a length but we did not have the
        // chance to actually initialize the streambuf pointers.

        BSLS_ASSERT(d_blob_p->numBuffers() != 0);
        const btlb::BlobBuffer& buffer = d_blob_p->buffer(0);
        setp(buffer.data(), buffer.data() + buffer.size());
    }

    int maxBufPos = d_previousBuffersLength +
                    d_blob_p->buffer(d_putBufferIndex).size();
    if (((unsigned)maxBufPos > position &&
            (unsigned)d_previousBuffersLength <= position) ||
        ((unsigned)maxBufPos == position &&
             position == (unsigned)d_blob_p->totalSize())) {
        // We are not crossing any buffer boundaries.

        BSLS_ASSERT(position >= (unsigned)d_previousBuffersLength);
        BSLS_ASSERT((position - d_previousBuffersLength) <=
                          (unsigned)d_blob_p->buffer(d_putBufferIndex).size());
        const btlb::BlobBuffer& buffer = d_blob_p->buffer(d_putBufferIndex);
        setp(buffer.data(), buffer.data() + buffer.size());
        pbump(position - d_previousBuffersLength);
        return;                                                       // RETURN
    }

    BSLS_ASSERT(position != (unsigned)d_previousBuffersLength);

    if (position > (unsigned)d_previousBuffersLength) {
        // We are moving forward.

        int left = position - (d_previousBuffersLength +
                               d_blob_p->buffer(d_putBufferIndex).size());
        do {
            d_previousBuffersLength +=
                                     d_blob_p->buffer(d_putBufferIndex).size();
            ++d_putBufferIndex;
            left -= d_blob_p->buffer(d_putBufferIndex).size();
        } while (left > 0);
    }
    else {
        // We are moving backwards

        int left = d_previousBuffersLength - position;
        do {
            --d_putBufferIndex;
            d_previousBuffersLength -=
                                     d_blob_p->buffer(d_putBufferIndex).size();
            left -= d_blob_p->buffer(d_putBufferIndex).size();
        } while (left > 0);
    }

    // The only case where position > d_previousBuffersLength happens during a
    // first call to this method (from the constructor) for a non-empty blob
    // which does not start at the beginning of a buffer.

    BSLS_ASSERT(position >= (unsigned)d_previousBuffersLength);

    // The only case where (position - d_previousBuffersLength) ==
    //                                d_blob_p->buffer(d_putBufferIndex).size()
    // happens during a first call to this method (from the constructor) for a
    // non-empty blob which finishes on a buffer boundary.

    BSLS_ASSERT(position - d_previousBuffersLength <=
                          (unsigned)d_blob_p->buffer(d_putBufferIndex).size());

    char *base = d_blob_p->buffer(d_putBufferIndex).data();

    setp(base, base + d_blob_p->buffer(d_putBufferIndex).size());
    pbump(position - d_previousBuffersLength);
}

// PRIVATE ACCESSORS
int OutBlobStreamBuf::checkInvariant() const
{
    bsl::size_t numBuffers = d_blob_p->numBuffers();

    if (pptr()) {
        BSLS_ASSERT(pbase());
        BSLS_ASSERT(epptr());
        BSLS_ASSERT((unsigned)d_putBufferIndex < numBuffers);
        BSLS_ASSERT(epptr() - pbase() ==
                                    d_blob_p->buffer(d_putBufferIndex).size());
        BSLS_ASSERT(pptr() - pbase() <=
                                    d_blob_p->buffer(d_putBufferIndex).size());
        BSLS_ASSERT(d_previousBuffersLength + epptr() - pbase() <=
                                                        d_blob_p->totalSize());
    }
    else {
        BSLS_ASSERT(0 == pbase());
        BSLS_ASSERT(0 == epptr());
        BSLS_ASSERT((unsigned)d_putBufferIndex == numBuffers);
    }

    return 0;
}

// PROTECTED MANIPULATORS
OutBlobStreamBuf::int_type
OutBlobStreamBuf::overflow(OutBlobStreamBuf::int_type c)
{
    BSLS_ASSERT(0 == checkInvariant());

    if (EOF == c) {
        return traits_type::not_eof(c);                               // RETURN
    }

    if (pptr() == epptr()) {

        int currentPos;
        if (0 == d_blob_p->totalSize() && 0 == d_blob_p->length()) {
            currentPos = 0;
        }
        else {
            currentPos = d_previousBuffersLength +
                                     d_blob_p->buffer(d_putBufferIndex).size();
        }
        if (currentPos >= d_blob_p->totalSize()) {
            d_blob_p->setLength(currentPos + 1); // grow if necessary
        }

        setPutPosition(currentPos);
    }

    BSLS_ASSERT(pptr() != epptr());
    *pptr() = static_cast<char_type>(c);
    pbump(1);
    return c;
}

OutBlobStreamBuf::int_type
OutBlobStreamBuf::pbackfail(OutBlobStreamBuf::int_type)
{
    return traits_type::eof();
}

OutBlobStreamBuf::pos_type
OutBlobStreamBuf::seekpos(pos_type position, bsl::ios_base::openmode which)
{
    return seekoff(off_type(position), bsl::ios_base::beg, which);
}

OutBlobStreamBuf::pos_type
OutBlobStreamBuf::seekoff(off_type                offset,
                          bsl::ios_base::seekdir  fixedPosition,
                          bsl::ios_base::openmode which)
{
    BSLS_ASSERT(0 == checkInvariant());

    bool gseek = which & bsl::ios_base::in;
    bool pseek = which & bsl::ios_base::out;

    if (gseek || !pseek) {
        return pos_type(-1);                                          // RETURN
    }

    sync();
    bsl::size_t totalSize = d_blob_p->length();

    off_type newoff;
    switch (fixedPosition)
    {
        case bsl::ios_base::beg:
          newoff = 0;
          break;
        case bsl::ios_base::cur:
          newoff = d_previousBuffersLength + pptr() - pbase();
          break;
        case bsl::ios_base::end:
          newoff = totalSize;
          break;
        default:
          return off_type(-1);                                        // RETURN
    }

    newoff += offset;
    if (newoff < 0 || totalSize < (unsigned)newoff) {
        return off_type(-1);                                          // RETURN
    }

    setPutPosition(static_cast<bsl::size_t>(newoff));

    return newoff;
}

bsl::streamsize OutBlobStreamBuf::showmanyc()
{
    BSLS_ASSERT(0 == checkInvariant());

    return 0;
}

int OutBlobStreamBuf::sync()
{
    BSLS_ASSERT(0 == checkInvariant());

    int totalSize = d_blob_p->length();
    int putPosition = d_previousBuffersLength + pptr() - pbase();

    if (putPosition > totalSize) {
        d_blob_p->setLength(putPosition);
    }
    return 0;
}

OutBlobStreamBuf::int_type OutBlobStreamBuf::underflow()
{
    return traits_type::eof();
}

bsl::streamsize OutBlobStreamBuf::xsgetn(char_type *, bsl::streamsize)
{
    return 0;
}

bsl::streamsize OutBlobStreamBuf::xsputn(const char_type *source,
                                         bsl::streamsize  numChars)
{
    bsl::streamsize numLeft   = numChars;
    bsl::streamsize numCopied = 0;

    while (0 < numLeft) {
        bsl::streamsize remainingChars = epptr() - pptr();
        int canCopy = bsl::min(remainingChars, numLeft);

        bsl::memcpy(pptr(), source + numCopied, canCopy);
        pbump(canCopy);
        numCopied += canCopy;
        numLeft   -= canCopy;

        if (0 < numLeft) {
            if (traits_type::eof() ==
                         overflow(traits_type::to_int_type(source[numCopied])))
            {
                return numCopied;                                     // RETURN
            }
            else {
                numCopied += 1;
                numLeft   -= 1;
            }
        }
    }
    return numCopied;
}

// CREATORS
OutBlobStreamBuf::OutBlobStreamBuf(btlb::Blob *blob)
: d_blob_p(blob)
, d_putBufferIndex(0)
, d_previousBuffersLength(0)
{
    setPutPosition(d_blob_p->length());
}

OutBlobStreamBuf::~OutBlobStreamBuf()
{
    BSLS_ASSERT(0 == checkInvariant());
    sync();
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
