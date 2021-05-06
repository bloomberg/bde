// bdlde_utf8checkinginstreambufwrapper.cpp                           -*-C++-*-

#include <bdlde_utf8checkinginstreambufwrapper.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdls_fdstreambuf_cpp,"$Id$ $CSID$")

///IMPLEMENTATION NOTES
///--------------------
// This code was ported from STLport to create 'bdls_fdstreambuf' in 2009,
// which was then evolved into 'bdlde_utf8checkinginstreambufwrapper' in 2020.

#include <bdlde_utf8util.h>

#include <bslma_default.h>
#include <bsls_assert.h>

#include <bsl_algorithm.h>
#include <bsl_cstring.h>
#include <bsl_limits.h>
#include <bsl_locale.h>
#include <bsl_streambuf.h>

namespace BloombergLP {
namespace bdlde {

                     // ------------------------------------
                     // class Utf8CheckingInStreamBufWrapper
                     // ------------------------------------

// PRIVATE MANIPULATOR
inline
bsl::streambuf::pos_type Utf8CheckingInStreamBufWrapper::setSeekFailure(
                                                  bsl::ios_base::openmode mode)
{
    if (d_seekable) {
        BSLS_ASSERT_OPT(d_heldStreamBuf_p);

        // Reset the held 'streambuf' to the start of the file.

        (void) d_heldStreamBuf_p->pubseekoff(0, bsl::ios_base::beg, mode);
    }

    d_offset = 0;
    d_errorStatus = k_SEEK_FAIL;
    d_bufEndStatus = 0;
    d_putBackMode = false;
    setg(0, 0, 0);

    return pos_type(-1);
}

// PROTECTED MANIPULATORS

                            // implementation functions

bsl::streambuf::int_type
Utf8CheckingInStreamBufWrapper::overflow(int_type)
    // This method is normally associated with output and is stubbed out in
    // this input-only implemntation.  Here it unconditionally returns
    // 'eof'.  The argument is ignored.
{
    return traits_type::eof();
}

bsl::streamsize Utf8CheckingInStreamBufWrapper::showmanyc()
{
    if (!isValid()) {
        return -1;                                                    // RETURN
    }

    return (d_putBackMode ? d_savedEgptr_p - d_savedEback_p : 0) +
                                                            (egptr() - gptr());
}

bsl::streambuf::int_type Utf8CheckingInStreamBufWrapper::underflow()
{
    if (!isValid()) {
        return traits_type::eof();                                    // RETURN
    }

    if (d_putBackMode) {
        BSLS_ASSERT(gptr() == egptr());

        setg(d_savedEback_p, d_savedEback_p, d_savedEgptr_p);
        d_putBackMode = false;

        if (gptr() < egptr()) {
            return traits_type::to_int_type(*gptr());                 // RETURN
        }
    }

    BSLS_ASSERT(!d_putBackMode);
    BSLS_ASSERT(gptr() == egptr());

    if (0 != d_bufEndStatus) {
        d_errorStatus = d_bufEndStatus;
        return traits_type::eof();                                    // RETURN
    }

    int sts = 0;
    IntPtr bytesRead = Utf8Util::readIfValid(&sts,
                                             d_buf_p,
                                             k_BUF_SIZE,
                                             d_heldStreamBuf_p);
    if (0 == bytesRead) {
        d_errorStatus = d_bufEndStatus = sts;
        return traits_type::eof();                                    // RETURN
    }
    if (sts < 0) {
        d_bufEndStatus = sts;
    }

    BSLS_ASSERT(0 < bytesRead);
    BSLS_ASSERT(bytesRead <= k_BUF_SIZE);

    d_offset += gptr() - eback();
    setg(d_buf_p, d_buf_p, d_buf_p + bytesRead);
    return traits_type::to_int_type(*d_buf_p);
}

             // functions forwarded to by corresponding public functions

void Utf8CheckingInStreamBufWrapper::imbue(const bsl::locale& locale)
{
    if (d_heldStreamBuf_p) {
        d_heldStreamBuf_p->pubimbue(locale);
    }
}

bsl::streambuf::int_type
Utf8CheckingInStreamBufWrapper::pbackfail(int_type c)
{
    // If we are at the beginning of the 'd_buf_p', or if there is no current
    // buffer, the object will have to enter putback mode.
    //
    // If we are in putback mode or entering putback mode and 'c == eof', fail
    // with no effect.
    //
    // If we are at the end of the putback buffer, fail with no effect.

    if (!isValid()) {
        return traits_type::eof();                                    // RETURN
    }

    const bool isEof = traits_type::eq_int_type(c, traits_type::eof());

    if (eback() < gptr()) {
        if (!isEof) {
            gptr()[-1] = traits_type::to_char_type(c);
        }
        else if (d_putBackMode) {
            // We don't know what character to put into the putback buffer.
            // Fail.

            return traits_type::eof();                                // RETURN
        }

        gbump(-1);

        return traits_type::to_int_type(*gptr());                     // RETURN
    }

    if (d_putBackMode || isEof) {
        // We've reached the back end of the putback buffer or we have no
        // character to put into the putback buffer.  Fail.

        return traits_type::eof();                                    // RETURN
    }

    // switch to putback mode

    BSLS_ASSERT(eback() == gptr());

    d_savedEback_p = eback();
    d_savedEgptr_p = egptr();

    char * const end = d_pBackBuf + k_PBACK_BUF_SIZE;
    setg(d_pBackBuf, end - 1, end);
    d_putBackMode = true;

    *gptr() = traits_type::to_char_type(c);

    return traits_type::to_int_type(*gptr());
}

bsl::streambuf::pos_type Utf8CheckingInStreamBufWrapper::seekoff(
                                                off_type                offset,
                                                bsl::ios_base::seekdir  whence,
                                                bsl::ios_base::openmode mode)
{
    BSLS_ASSERT(mode & bsl::ios_base::in);

    if (bsl::ios_base::end == whence || !d_heldStreamBuf_p) {
        d_offset = 0;
        d_errorStatus = k_SEEK_FAIL;
        d_bufEndStatus = 0;
        d_putBackMode = false;
        setg(0, 0, 0);

        return pos_type(-1);                                          // RETURN
    }

    const pos_type curPos = pos_type(d_offset) +
                    pos_type(d_putBackMode ? gptr() - egptr()   // non-positive
                                           : gptr() - eback()); // non-negative

    pos_type dest;

    switch (whence) {
      case bsl::ios_base::beg: {
        dest = pos_type(offset);

        // The only seek that's allowed from a failed seek state is an absolute
        // seek to 0.

        if (k_SEEK_FAIL == d_errorStatus && 0 == dest) {
            if (d_seekable) {
                d_errorStatus = 0;
            }
            else {
                return setSeekFailure(mode);                          // RETURN
            }
        }
        else if (curPos < dest) {
            return setSeekFailure(mode);                              // RETURN
        }
      } break;
      case bsl::ios_base::cur: {
        dest = curPos + offset;

        // If the state is 'k_SEEK_FAIL', we don't have a valid location, so a
        // relative seek is meaningless.

        if (k_SEEK_FAIL == d_errorStatus || 0 < offset) {
            return setSeekFailure(mode);                              // RETURN
        }
      } break;
      default: {
        BSLS_ASSERT_INVOKE_NORETURN("invalid 'whence' in 'seek'");
      } break;
    }

    if (dest < 0) {
        return setSeekFailure(mode);                                  // RETURN
    }

    const off_type relOffset = dest - curPos;

    if (0 == relOffset) {
        return dest;                                                  // RETURN
    }

    BSLS_ASSERT(relOffset < 0);

    d_errorStatus = 0;

    // See if we can just seek within the buffer without seeking on
    // 'd_heldStreamBuf_p'.

    if (gptr() && !d_putBackMode) {
        if (bsl::numeric_limits<int>::min() <= relOffset &&
                                               eback() <= gptr() + relOffset) {
            gbump(static_cast<int>(relOffset));

            return dest;                                              // RETURN
        }
    }

    if (!d_seekable) {
        return setSeekFailure(mode);                                  // RETURN
    }

    d_offset = d_heldStreamBuf_p->pubseekoff(dest,
                                             bsl::ios_base::beg,
                                             mode);
    if (dest != d_offset) {
        return setSeekFailure(mode);                                  // RETURN
    }

    d_putBackMode = false;
    setg(0, 0, 0);
    d_bufEndStatus = 0;

    return d_offset;
}

bsl::streambuf::pos_type
Utf8CheckingInStreamBufWrapper::seekpos(pos_type                offset,
                                        bsl::ios_base::openmode mode)
{
    return seekoff(offset, bsl::ios_base::beg, mode);
}

bsl::streamsize Utf8CheckingInStreamBufWrapper::xsgetn(
                                                     char            *buffer,
                                                     bsl::streamsize  numBytes)
{
    BSLS_ASSERT(buffer);

    if (!isValid()) {
        return 0;                                                     // RETURN
    }

    BSLS_ASSERT(0 <= numBytes);

    const char * const start = buffer;
    const char * const end   = buffer + numBytes;

    BSLS_ASSERT(start <= end);

    IntPtr outBufLen;
    while (0 < (outBufLen = static_cast<int_type>(end - buffer))) {
        const IntPtr streamBufLen = egptr() - gptr();

        if (0 < streamBufLen) {
            const int_type chunk = static_cast<int_type>(bsl::min(
                                                     outBufLen, streamBufLen));
            BSLS_ASSERT(0 < chunk);

            traits_type::copy(buffer, gptr(), chunk);
            buffer += chunk;
            gbump(chunk);
        }
        else if (traits_type::eof() == underflow()) {
            break;
        }
    }

    // If less than 'numBytes' were read, we will have had a failed 'underflow'
    // which will have updated 'd_errorStatus' from 'd_bufEndStatus'.

    return buffer - start;
}

bsl::streamsize Utf8CheckingInStreamBufWrapper::xsputn(const char      *,
                                                       bsl::streamsize  )
{
    return 0;
}

// PUBLIC CLASS METHOD
const char *Utf8CheckingInStreamBufWrapper::toAscii(int errorStatus)
{
    if (0 == errorStatus) {
        return "NO_ERROR";                                            // RETURN
    }
    else if (k_SEEK_FAIL == errorStatus) {
        return "SEEK_FAIL";                                           // RETURN
    }

    return Utf8Util::toAscii(errorStatus);
}

// CREATORS
Utf8CheckingInStreamBufWrapper::Utf8CheckingInStreamBufWrapper()
: bsl::streambuf()
, d_heldStreamBuf_p(0)
, d_errorStatus(0)
, d_bufEndStatus(0)
, d_savedEback_p(0)
, d_savedEgptr_p(0)
, d_offset(0)
, d_seekable(false)
, d_putBackMode(false)
, d_allocator_p(bslma::Default::allocator())
{
    d_buf_p = static_cast<char_type *>(d_allocator_p->allocate(k_BUF_SIZE));

    setg(0, 0, 0);
    setp(0, 0);
}

Utf8CheckingInStreamBufWrapper::Utf8CheckingInStreamBufWrapper(
                                              bslma::Allocator *basicAllocator)
: bsl::streambuf()
, d_heldStreamBuf_p(0)
, d_errorStatus(0)
, d_bufEndStatus(0)
, d_savedEback_p(0)
, d_savedEgptr_p(0)
, d_offset(0)
, d_seekable(false)
, d_putBackMode(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    d_buf_p = static_cast<char_type *>(d_allocator_p->allocate(k_BUF_SIZE));

    setg(0, 0, 0);
    setp(0, 0);
}

Utf8CheckingInStreamBufWrapper::Utf8CheckingInStreamBufWrapper(
                                              bsl::streambuf   *streamBuf,
                                              bslma::Allocator *basicAllocator)
: bsl::streambuf()
, d_heldStreamBuf_p(streamBuf)
, d_errorStatus(0)
, d_bufEndStatus(0)
, d_savedEback_p(0)
, d_savedEgptr_p(0)
, d_offset(streamBuf ? streamBuf->pubseekoff(0, bsl::ios_base::cur)
                     : pos_type(-1))
, d_seekable(0 <= d_offset)
, d_putBackMode(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    d_buf_p = static_cast<char_type *>(d_allocator_p->allocate(k_BUF_SIZE));

    if (d_offset < 0) {
        d_offset = 0;
    }

    setg(0, 0, 0);
    setp(0, 0);
}

Utf8CheckingInStreamBufWrapper::~Utf8CheckingInStreamBufWrapper()
{
    d_allocator_p->deallocate(d_buf_p);
}

// PUBLIC MANIPULATOR
void Utf8CheckingInStreamBufWrapper::reset(bsl::streambuf *streamBuf)
{
    d_heldStreamBuf_p = streamBuf;
    d_errorStatus = 0;
    d_bufEndStatus = 0;
    d_savedEback_p = 0;
    d_savedEgptr_p = 0;
    d_offset = streamBuf ? streamBuf->pubseekoff(0, bsl::ios_base::cur)
                         : pos_type(-1);
    d_seekable = 0 <= d_offset;
    d_putBackMode = false;

    if (d_offset < 0) {
        d_offset = 0;
    }

    setg(0, 0, 0);
}

}  // close package namespace
}  // close enterprise namespace

//-----------------------------------------------------------------------------
// Adapted to 'bdlde::Utf8CheckingInStreamBufWrapper' from 'bdls::FdStreamBuf',
// 2020
//
// Adapted to bde from STLport, 2009
//     'bdls::FdStreamBuf' from 'bsl::filebuf'
//     'bdls::FdStreamBuf_FileHandler' from 'bsl::_Filebuf_base'
//
// Copyright (c) 1996,1997,1999
// Silicon Graphics Computer Systems, Inc.
//
// Copyright (c) 1999
// Boris Fomitchev
//
// This material is provided "as is", with absolutely no warranty expressed
// or implied.  Any use is at your own risk.
//
// Permission to use or copy this software for any purpose is hereby granted
// without fee, provided the above notices are retained on all copies.
// Permission to modify the code and to distribute modified code is granted,
// provided the above notices are retained, and a notice that the code was
// modified is included with the above copyright notice.
//-----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
