// bdlsb_fixedmeminstreambuf.h                                        -*-C++-*-
#ifndef INCLUDED_BDLSB_FIXEDMEMINSTREAMBUF
#define INCLUDED_BDLSB_FIXEDMEMINSTREAMBUF

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an input 'basic_streambuf' using a client buffer.
//
//@CLASSES:
//   bdlsb::FixedMemInStreamBuf: input stream buffer using client memory
//
//@SEE_ALSO: bdlsb_fixedmemoutstreambuf, bdlsb_memoutstreambuf
//
//@DESCRIPTION: This component defines a class, 'bdlsb::FixedMemInStreamBuf',
// that implements the input portion of the 'bsl::basic_streambuf' protocol
// using a client-supplied memory buffer.  Method names necessarily correspond
// to the protocol-specified method names.  Clients supply the character buffer
// at stream buffer construction, and can later reinitialize the stream buffer
// with a different character buffer by calling the 'pubsetbuf' method.
//
// This component provides none of the output-related functionality of
// 'basic_streambuf' (see Streaming Architecture, below), nor does it use
// locales in any way.
//
///Streaming Architecture
///----------------------
// Stream buffers are designed to decouple device handling from content
// formatting, providing the requisite device handling and possible buffering
// services, and leaving the formatting to the client stream.  The standard
// C++ IOStreams library further partitions streaming into input streaming and
// output streaming, separating responsibilities for each at both the stream
// layer and the stream buffer layer.  The BDE streaming library for 'bdex',
// including all of 'bdlsb', follows this model.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Use of the 'bdlsb::FixedMemInStreamBuf'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 'bdlsb::FixedMemInStreamBuf' can be used in situations when you already
// have an array of bytes in memory and you'd like to wrap it in an input
// stream to extract data in a formatted manner.  A
// 'bdlsb::FixedMemInStreamBuf' object refers to an externally managed buffer
// that is supplied either at construction, or using the 'pubsetbuf' method of
// the 'bsl::streambuf' base-class.
//
// First, we create an array of characters to provide data that needs to be
// parsed, and construct 'bdlsb::FixedMemInStreamBuf' on that array:
//..
//  {
//      const char *inputText = "1 1 2 3 5 8 13 21";
//      bdlsb::FixedMemInStreamBuf buffer(inputText, strlen(inputText));
//..
// Notice that 'bdlsb::FixedMemInStreamBuf' can be used with buffers referring
// to stack memory or to heap memory.
//
// Then, we use 'buffer' to construct a 'bsl::istream':
//..
//      bsl::istream stream(&buffer);
//..
// Finally, we can input the data from the stream in a formatted manner:
//..
//      int value;
//      while (stream >> value) {
//          cout << "Value is: " << value << endl;
//      }
//  }
//..
//
///Example 2: Scanning Input Data
/// - - - - - - - - - - - - - - -
// This example illustrates scanning of the input stream buffer for particular
// pattern ( digits, in our case ) and then using stream to read out found
// number.
//
// First, we create an array of characters to provide data that needs to be
// parsed, and construct 'bdlsb::FixedMemInStreamBuf' on that array:
//..
//  {
//      const char *inputText = "The answer is: 42.";
//      bdlsb::FixedMemInStreamBuf buffer(inputText, strlen(inputText));
//..
// Then, we use 'buffer' to construct a 'bsl::istream' that will be used later
// to read found number:
//..
//      bsl::istream stream(&buffer);
//..
// Next, we scan input buffer one character at a time searching for the first
// digit:
//..
//      char ch;
//      do {
//          ch = buffer.sbumpc();
//
//          if ( (ch >= '0') && (ch <= '9') ) {
//..
// Now, when the digit character is found, we return the first digit into the
// input stream buffer for subsequent read:
//..
//              buffer.sputbackc(ch);
//              int n;
//..
// Finally, we read out the whole number:
//..
//              stream >> n;
//              assert( 42 == n );
//              cout << "The answer is " << n << " indeed..." << endl;
//              break;
//          }
//      } while ( ch != EOF );
//  }
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSL_IOS
#include <bsl_ios.h>       // for 'bsl::streamsize'
#endif

#ifndef INCLUDED_BSL_STREAMBUF
#include <bsl_streambuf.h>
#endif

#ifndef INCLUDED_BSL_CSTRING
#include <bsl_cstring.h>
#endif

#if defined(BSLS_PLATFORM_CMP_MSVC) && defined(min)
    // Note: on Windows -> WinDef.h:#define min(a,b) ...
#undef min
#endif

namespace BloombergLP {
namespace bdlsb {

                       // =========================
                       // class FixedMemInStreamBuf
                       // =========================

class FixedMemInStreamBuf : public bsl::streambuf {
    // This class implements the input functionality of the 'basic_streambuf'
    // protocol, using client-supplied 'char *' memory.

    // DATA
    char            *d_buffer_p;      // buffer (held, not owned)
    bsl::size_t      d_bufferSize;    // length of buffer

    // NOT IMPLEMENTED
    FixedMemInStreamBuf(const FixedMemInStreamBuf&);
    FixedMemInStreamBuf& operator=(const FixedMemInStreamBuf&);

  protected:
    // PROTECTED MANIPULATORS
    virtual pos_type seekoff(
                            off_type                offset,
                            bsl::ios_base::seekdir  way,
                            bsl::ios_base::openmode which = bsl::ios_base::in);
        // Set the position indicator to the relative specified 'offset' from
        // the base position indicated by the specified 'way' and return the
        // resulting absolute position on success or pos_type(-1) on failure.
        // Optionally specify 'which' area of the stream buffer.  The seek
        // operation will fail if 'which' does not include the flag
        // 'bsl::ios_base::in' or if the resulting absolute position is less
        // than zero or greater than the value returned by 'length'.

    virtual pos_type seekpos(
                            pos_type                position,
                            bsl::ios_base::openmode which = bsl::ios_base::in);
        // Set the position indicator to the specified 'position' and return
        // the resulting absolute position on success or pos_type(-1) on
        // failure.  Optionally specify 'which' area of the stream buffer.  The
        // 'seekpos' operation will fail if 'which' does not include the flag
        // 'bsl::ios_base::in' or if position is less then zero or greater
        // than the value returned by 'length'.

    virtual FixedMemInStreamBuf *setbuf(char  *buffer, bsl::streamsize length);
    FixedMemInStreamBuf *setbuf(const char *buffer, bsl::streamsize length);
        // Reinitialize this stream buffer to use the specified character
        // 'buffer' having the specified 'length'.  Return the pointer
        // providing modifiable access the this stream buffer.  The behavior is
        // undefined unless 'buffer != 0 && length > 0' or 'length == 0'.  Upon
        // re-initialization for use of the new buffer, neither the content nor
        // the next input position indicator are preserved.  Note that 'buffer'
        // is held but not owned.

    virtual bsl::streamsize showmanyc();
        // Return the number of characters currently available for reading
        // from this stream buffer, or -1 if there are none.

    virtual bsl::streamsize xsgetn(char_type       *destination,
                                   bsl::streamsize  length);
        // Read the specified 'length' number of characters into the specified
        // 'destination'.  Return the number of characters successfully read.
        // The behavior is undefined unless '0 <= length'.

  public:
    // CREATORS
    FixedMemInStreamBuf(const char *buffer, bsl::size_t length);
        // Create a 'FixedMemInStreamBuf' that provides access to the character
        // sequence in the specified 'buffer' of the specified 'length'.  The
        // behavior is undefined unless 'buffer != 0 && length > 0' or
        // 'length == 0'.

    ~FixedMemInStreamBuf();
        // Destroy this stream buffer.

    // MANIPULATORS
    FixedMemInStreamBuf *pubsetbuf(char            *buffer,
                                   bsl::streamsize  length);
    FixedMemInStreamBuf *pubsetbuf(const char      *buffer,
                                   bsl::streamsize  length);
        // Reinitialize this stream buffer to use the specified character
        // 'buffer' having the specified 'length'.  Return the address of this
        // modifiable stream buffer.  The behavior is undefined unless
        // 'buffer != 0 && length > 0' or 'length == 0'.  Upon reinitialization
        // for use of the new buffer, neither the content nor the next input
        // position indicator is preserved.  Note that 'buffer' is held but not
        // owned.

    // ACCESSORS
    const char *data() const;
        // Return the address of the non-modifiable character buffer held by
        // this stream buffer.

    bsl::size_t length() const;
        // Return the number of characters from the current input position to
        // the end of the stream buffer.  The function returns the same value
        // as 'seekoff(0, bsl::ios_base::beg)'.  The length is modified by a
        // call to 'seekpos', 'seekoff' or by reading characters from the
        // buffer.
};

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                       // -------------------------
                       // class FixedMemInStreamBuf
                       // -------------------------

// PROTECTED MANIPULATORS
inline
FixedMemInStreamBuf::pos_type
FixedMemInStreamBuf::seekpos(pos_type                position,
                             bsl::ios_base::openmode which)
{
    return seekoff(static_cast<off_type>(position), bsl::ios_base::beg, which);
}


inline
FixedMemInStreamBuf *FixedMemInStreamBuf::setbuf(char            *buffer,
                                                 bsl::streamsize  length)

{
    BSLS_ASSERT_SAFE(buffer || 0 == length);
    BSLS_ASSERT_SAFE(0 <= length);

    // Reset pointers and length.
    d_buffer_p    = buffer;
    d_bufferSize  = static_cast<bsl::size_t>(length);
    setg(d_buffer_p, d_buffer_p, d_buffer_p + d_bufferSize);
    return this;
}

inline
FixedMemInStreamBuf *FixedMemInStreamBuf::setbuf(const char      *buffer,
                                                 bsl::streamsize  length)
{
    BSLS_ASSERT_SAFE(buffer || 0 == length);
    BSLS_ASSERT_SAFE(0 <= length);

    return setbuf(const_cast<char *>(buffer), length);
}

inline
bsl::streamsize FixedMemInStreamBuf::showmanyc()
{
    bsl::streamsize numChars = egptr() - gptr();
    if (0 == numChars) {
        return -1;                                                    // RETURN
    }
    return numChars;
}

inline
bsl::streamsize FixedMemInStreamBuf::xsgetn(char_type       *destination,
                                            bsl::streamsize  length)
{
    BSLS_ASSERT_SAFE(destination);
    BSLS_ASSERT_SAFE(0 <= length);

    bsl::streamsize charsLeft = egptr() - gptr();

    bsl::streamsize canCopy = charsLeft < length ? charsLeft : length;

    bsl::memcpy(destination, gptr(), canCopy);
    gbump(static_cast<int>(canCopy));
    return canCopy;
}

// CREATORS
inline
FixedMemInStreamBuf::FixedMemInStreamBuf(const char  *buffer,
                                         bsl::size_t  length)
: d_buffer_p(const_cast<char *>(buffer))
, d_bufferSize(length)
{
    BSLS_ASSERT_SAFE(buffer || 0 == length);

    setg(d_buffer_p, d_buffer_p, d_buffer_p + d_bufferSize);
}

inline
FixedMemInStreamBuf::~FixedMemInStreamBuf()
{
}

// MANIPULATORS
inline
FixedMemInStreamBuf *FixedMemInStreamBuf::pubsetbuf(const char     *buffer,
                                                    bsl::streamsize length)
{
    BSLS_ASSERT_SAFE(buffer || 0 == length);
    BSLS_ASSERT_SAFE(0 <= length);

    return setbuf(buffer, length);
}

inline
FixedMemInStreamBuf *FixedMemInStreamBuf::pubsetbuf(char           *buffer,
                                                    bsl::streamsize length)
{
    BSLS_ASSERT_SAFE(buffer || 0 == length);
    BSLS_ASSERT_SAFE(0 <= length);

    return setbuf(buffer, length);
}

// ACCESSORS
inline
const char *FixedMemInStreamBuf::data() const
{
    return d_buffer_p;
}

inline
bsl::size_t FixedMemInStreamBuf::length() const
{
    return egptr() - gptr();
}

}  // close package namespace
}  // close enterprise namespace

#endif

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
