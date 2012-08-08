// bcesb_blobstreambuf.cpp                                            -*-C++-*-
#include <bcesb_blobstreambuf.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcesb_blobstreambuf_cpp,"$Id$ $CSID$")

#include <bcema_blob.h>

#include <bsls_assert.h>

#include <bsl_cstdio.h>  // EOF
#include <bsl_cstring.h>
#include <bsl_string.h>

// Note: on Windows -> WinDef.h:#define min(a,b) ...
#if defined(BSLS_PLATFORM__CMP_MSVC) && defined(min)
#undef min
#endif

namespace BloombergLP {
                        // ===========================
                        // class bcesb_InBlobStreamBuf
                        // ===========================

// PRIVATE MANIPULATORS
void bcesb_InBlobStreamBuf::setGetPosition(bsl::size_t position)
{
    BSLS_ASSERT(position <= (unsigned)d_blob_p->length());
    if (d_blob_p->length() == 0) {
        setg(0, 0, 0);
        return;
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
        return;
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
int bcesb_InBlobStreamBuf::checkInvariant() const
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
bcesb_InBlobStreamBuf::int_type
bcesb_InBlobStreamBuf::overflow(bcesb_InBlobStreamBuf::int_type)
{
    return traits_type::eof();
}

bcesb_InBlobStreamBuf::int_type
bcesb_InBlobStreamBuf::pbackfail(bcesb_InBlobStreamBuf::int_type c)
{
    BSLS_ASSERT(checkInvariant() == 0);

    if (gptr() == eback()) {
        if (0 == d_getBufferIndex) {
            return traits_type::eof(); // No put-back position available
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
        return ~traits_type::eof();
    }
    else {
        *gptr() = c;
        return c;
    }
}

bcesb_InBlobStreamBuf::pos_type
bcesb_InBlobStreamBuf::seekpos(pos_type                position,
                               bsl::ios_base::openmode which)
{
    return seekoff(off_type(position), bsl::ios_base::beg, which);
}

bcesb_InBlobStreamBuf::pos_type
bcesb_InBlobStreamBuf::seekoff(off_type                offset,
                               bsl::ios_base::seekdir  fixedPosition,
                               bsl::ios_base::openmode which)
{
    BSLS_ASSERT(0 == checkInvariant());

    bool gseek = which & bsl::ios_base::in;
    bool pseek = which & bsl::ios_base::out;

    if (!gseek || pseek) {
        return pos_type(-1);
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
          return off_type(-1);
    }

    newoff += offset;
    if (newoff < 0 || totalSize < (unsigned)newoff) {
        return off_type(-1);
    }

    setGetPosition(newoff);

    return newoff;
}

bsl::streamsize bcesb_InBlobStreamBuf::showmanyc()
{
    BSLS_ASSERT(0 == checkInvariant());

    return d_blob_p->length() - (d_previousBuffersLength + gptr() - eback());
}

int bcesb_InBlobStreamBuf::sync()
{
    BSLS_ASSERT(0 == checkInvariant());
    return 0;
}

bcesb_InBlobStreamBuf::int_type bcesb_InBlobStreamBuf::underflow()
{
    BSLS_ASSERT(0 == checkInvariant());
    BSLS_ASSERT(egptr() == gptr());

    int totalSize = d_blob_p->length();
    int getPosition =  d_previousBuffersLength + gptr() - eback();

    if (getPosition >= totalSize) {
        BSLS_ASSERT(getPosition == totalSize);
        return EOF;
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
        // This is our offset in this buffer.  The point is that the length
        // of the underlying blob could have grown.

        curOffset = egptr() - eback();
    }

    char *gbuf = d_blob_p->buffer(d_getBufferIndex).data();
    bsl::size_t glen =  d_blob_p->buffer(d_getBufferIndex).size();

    // We need to figure out where to stop in that buffer.  If this is
    // the last buffer, we may have to stop before the end of the memory
    // since it may not be full at that time.

    bsl::size_t endOffset = bsl::min(totalSize - d_previousBuffersLength,
                                     (int)glen);

    BSLS_ASSERT(curOffset < endOffset);
    BSLS_ASSERT(endOffset <= glen);
    setg(gbuf, gbuf + curOffset, gbuf + endOffset);

    return traits_type::to_int_type(*gptr());
}

bsl::streamsize bcesb_InBlobStreamBuf::xsgetn(char_type       *destination,
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
            return numCopied + canCopy;
        }
        numCopied += canCopy;
    }
    return numCopied;
}

bsl::streamsize bcesb_InBlobStreamBuf::xsputn(const char_type *,
                                              bsl::streamsize)
{
    return 0;
}

// CREATORS
bcesb_InBlobStreamBuf::bcesb_InBlobStreamBuf(const bcema_Blob *blob)
: d_blob_p(blob)
, d_getBufferIndex(0)
, d_previousBuffersLength(0)
{
    setGetPosition(0);
}

bcesb_InBlobStreamBuf::~bcesb_InBlobStreamBuf()
{
    BSLS_ASSERT(0 == checkInvariant());
}

                        // ============================
                        // class bcesb_OutBlobStreamBuf
                        // ============================

// PRIVATE MANIPULATORS
void bcesb_OutBlobStreamBuf::setPutPosition(bsl::size_t position)
{
    BSLS_ASSERT(position <= (unsigned)d_blob_p->totalSize());
    if (d_blob_p->totalSize() == 0) {
        setp(0, 0);
        return;
    }

    if (epptr() == pbase()) {
        // Initialization.  Buffer now has a length but we did not have the
        // chance to actually initialize the streambuf pointers.

        BSLS_ASSERT(d_blob_p->numBuffers() != 0);
        const bcema_BlobBuffer& buffer = d_blob_p->buffer(0);
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
        const bcema_BlobBuffer& buffer = d_blob_p->buffer(d_putBufferIndex);
        setp(buffer.data(), buffer.data() + buffer.size());
        pbump(position - d_previousBuffersLength);
        return;
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

    // The only case where position > d_previousBuffersLength happens
    // during a first call to this method (from the constructor) for
    // a non-empty blob which does not start at the beginning of a buffer.

    BSLS_ASSERT(position >= (unsigned)d_previousBuffersLength);

    // The only case where
    // (position - d_previousBuffersLength) ==
    //                                d_blob_p->buffer(d_putBufferIndex).size()
    // happens during a first call to this method (from the constructor) for
    // a non-empty blob which finishes on a buffer boundary.

    BSLS_ASSERT(position - d_previousBuffersLength <=
                          (unsigned)d_blob_p->buffer(d_putBufferIndex).size());

    char *base = d_blob_p->buffer(d_putBufferIndex).data();

    setp(base, base + d_blob_p->buffer(d_putBufferIndex).size());
    pbump(position - d_previousBuffersLength);
}

// PRIVATE ACCESSORS
int bcesb_OutBlobStreamBuf::checkInvariant() const
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
bcesb_OutBlobStreamBuf::int_type
bcesb_OutBlobStreamBuf::overflow(bcesb_OutBlobStreamBuf::int_type c)
{
    BSLS_ASSERT(0 == checkInvariant());

    if (EOF == c) {
        return traits_type::not_eof(c);
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
    *pptr() = c;
    pbump(1);
    return c;
}

bcesb_OutBlobStreamBuf::int_type
bcesb_OutBlobStreamBuf::pbackfail(bcesb_OutBlobStreamBuf::int_type)
{
    return traits_type::eof();
}

bcesb_OutBlobStreamBuf::pos_type
bcesb_OutBlobStreamBuf::seekpos(pos_type                position,
                                bsl::ios_base::openmode which)
{
    return seekoff(off_type(position), bsl::ios_base::beg, which);
}

bcesb_OutBlobStreamBuf::pos_type
bcesb_OutBlobStreamBuf::seekoff(off_type                offset,
                                bsl::ios_base::seekdir  fixedPosition,
                                bsl::ios_base::openmode which)
{
    BSLS_ASSERT(0 == checkInvariant());

    bool gseek = which & bsl::ios_base::in;
    bool pseek = which & bsl::ios_base::out;

    if (gseek || !pseek) {
        return pos_type(-1);
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
          return off_type(-1);
    }

    newoff += offset;
    if (newoff < 0 || totalSize < (unsigned)newoff) {
        return off_type(-1);
    }

    setPutPosition(newoff);

    return newoff;
}

bsl::streamsize bcesb_OutBlobStreamBuf::showmanyc()
{
    BSLS_ASSERT(0 == checkInvariant());

    return 0;
}

int bcesb_OutBlobStreamBuf::sync()
{
    BSLS_ASSERT(0 == checkInvariant());

    int totalSize = d_blob_p->length();
    int putPosition = d_previousBuffersLength + pptr() - pbase();

    if (putPosition > totalSize) {
        d_blob_p->setLength(putPosition);
    }
    return 0;
}

bcesb_OutBlobStreamBuf::int_type bcesb_OutBlobStreamBuf::underflow()
{
    return traits_type::eof();
}

bsl::streamsize bcesb_OutBlobStreamBuf::xsgetn(char_type       *,
                                               bsl::streamsize  )
{
    return 0;
}

bsl::streamsize bcesb_OutBlobStreamBuf::xsputn(const char_type *source,
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
                return numCopied;
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
bcesb_OutBlobStreamBuf::bcesb_OutBlobStreamBuf(bcema_Blob *blob)
: d_blob_p(blob)
, d_putBufferIndex(0)
, d_previousBuffersLength(0)
{
    setPutPosition(d_blob_p->length());
}

bcesb_OutBlobStreamBuf::~bcesb_OutBlobStreamBuf()
{
    BSLS_ASSERT(0 == checkInvariant());
    sync();
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
