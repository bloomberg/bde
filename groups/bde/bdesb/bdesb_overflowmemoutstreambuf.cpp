// bdesb_overflowmemoutstreambuf.cpp                                  -*-C++-*-
#include <bdesb_overflowmemoutstreambuf.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

#include <bsl_cstdio.h>
#include <bsl_cstring.h>
#include <bsl_locale.h>

namespace BloombergLP {

                    // -----------------------------------
                    // class bdesb_OverflowMemOutStreamBuf
                    // -----------------------------------

// PRIVATE MANIPULATORS
void bdesb_OverflowMemOutStreamBuf::grow(int n)
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

void bdesb_OverflowMemOutStreamBuf::privateSync()
{
    if (d_inOverflowBufferFlag) {
        d_dataLength = pptr() - pbase() + d_initialBufferSize;
    }
    else {
        d_dataLength = pptr() - pbase();
    }
}

// PROTECTED MANIPULATORS
bdesb_OverflowMemOutStreamBuf::int_type
bdesb_OverflowMemOutStreamBuf::overflow(
                                     bdesb_OverflowMemOutStreamBuf::int_type c)
{
    if (EOF == c) {
        return traits_type::not_eof(c);
    }

    privateSync();

    if (0 == d_overflowBufferSize || d_inOverflowBufferFlag) {
        grow(1);
    }

    setp(d_overflowBuffer_p,
         d_overflowBuffer_p + d_overflowBufferSize);
    pbump(d_dataLength - d_initialBufferSize);
    d_inOverflowBufferFlag = true;

    BSLS_ASSERT(pptr() != epptr());

    *pptr() = c;
    pbump(1);
    ++d_dataLength;
    return c;
}

bdesb_OverflowMemOutStreamBuf::int_type
bdesb_OverflowMemOutStreamBuf::pbackfail(
                                       bdesb_OverflowMemOutStreamBuf::int_type)
{
    return traits_type::eof();
}

bdesb_OverflowMemOutStreamBuf::pos_type
bdesb_OverflowMemOutStreamBuf::seekoff(off_type                offset,
                                       bsl::ios_base::seekdir  fixedPosition,
                                       bsl::ios_base::openmode which)
{
    privateSync();
    if (d_inOverflowBufferFlag) {
        BSLS_ASSERT(pptr() - pbase() == (d_dataLength - d_initialBufferSize));
    }
    else {
        BSLS_ASSERT(pptr() - pbase() == d_dataLength);
    }

    bool gseek = which & bsl::ios_base::in;
    bool pseek = which & bsl::ios_base::out;

    if (!pseek || gseek) {
        return bsl::streambuf::pos_type(-1);
    }

    int totalSize = d_initialBufferSize + d_overflowBufferSize;
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
        return bsl::streambuf::pos_type(-1);
      }
    }

    newoff += offset;

    if (0 > newoff) {
        return -1;
    }

    BSLS_ASSERT(0 <= newoff);

    if (newoff <= d_initialBufferSize) {
        // Move destination is in initial buffer.

        if (!d_inOverflowBufferFlag)  {
            pbump(newoff - d_dataLength);
        }
        else {
            d_inOverflowBufferFlag = false;
            setp(d_initialBuffer_p, d_initialBuffer_p + d_initialBufferSize);
            pbump(newoff);
        }
    }
    else {
        // Move destination is in overflow buffer.

        if (totalSize < (int) newoff) {
            grow(newoff - totalSize);
            d_inOverflowBufferFlag = true;
            setp(d_overflowBuffer_p,
                 d_overflowBuffer_p + d_overflowBufferSize);
            pbump(newoff - d_initialBufferSize);
        }
        else {
            if (d_inOverflowBufferFlag) {
                pbump(newoff - d_dataLength);
            }
            else {
                d_inOverflowBufferFlag = true;
                setp(d_overflowBuffer_p,
                     d_overflowBuffer_p + d_overflowBufferSize);
                pbump(newoff - d_initialBufferSize);
            }
        }
    }

    d_dataLength = newoff;
    return newoff;
}

bsl::streambuf *bdesb_OverflowMemOutStreamBuf::setbuf(char *, bsl::streamsize)
{
    // this function is not supported

    return 0;
}

bsl::streamsize bdesb_OverflowMemOutStreamBuf::showmanyc()
{
    return 0;
}

bdesb_OverflowMemOutStreamBuf::int_type
bdesb_OverflowMemOutStreamBuf::underflow()
{
    return traits_type::eof();
}

bsl::streamsize
bdesb_OverflowMemOutStreamBuf::xsgetn(char_type *, bsl::streamsize)
{
    return traits_type::eof();
}

bsl::streamsize
bdesb_OverflowMemOutStreamBuf::xsputn(const char_type *source,
                                      bsl::streamsize  numChars)
{
    BSLS_ASSERT(source);
    BSLS_ASSERT(0 <= numChars);

    privateSync();
    int numBytesForLastCopy = numChars;

    int newDataLength = d_dataLength + numChars;

    if (d_inOverflowBufferFlag) {
        if (newDataLength > d_initialBufferSize + d_overflowBufferSize) {
            grow(numChars); // this is potentially more than necessary
            setp(d_overflowBuffer_p,
                 d_overflowBuffer_p + d_overflowBufferSize);
            pbump(d_dataLength - d_initialBufferSize);
        }
    }
    else if (newDataLength > d_initialBufferSize) {

        if (newDataLength > d_initialBufferSize + d_overflowBufferSize) {
            grow(numChars);
        }

        int firstCopyBytes = d_initialBufferSize - d_dataLength;

        BSLS_ASSERT(firstCopyBytes >= 0);
        BSLS_ASSERT(firstCopyBytes < numChars);

        bsl::memcpy(pptr(), source, firstCopyBytes);
        source += firstCopyBytes;
        numBytesForLastCopy -= firstCopyBytes;
        d_inOverflowBufferFlag = true;
        setp(d_overflowBuffer_p,
             d_overflowBuffer_p + d_overflowBufferSize);
    }

    d_dataLength += numChars;
    bsl::memcpy(pptr(), source, numBytesForLastCopy);
    pbump(numBytesForLastCopy);
    return numChars;
}

// CREATORS
bdesb_OverflowMemOutStreamBuf::bdesb_OverflowMemOutStreamBuf(
                                               char            *buffer,
                                               int              size,
                                               bslma_Allocator *basicAllocator)
: d_dataLength(0)
, d_initialBuffer_p(buffer)
, d_initialBufferSize(size)
, d_inOverflowBufferFlag(false)
, d_overflowBuffer_p(0)
, d_overflowBufferSize(0)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < size);

    setp(d_initialBuffer_p, d_initialBuffer_p + d_initialBufferSize);
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
