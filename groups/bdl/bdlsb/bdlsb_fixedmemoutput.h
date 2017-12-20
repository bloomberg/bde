// bdlsb_fixedmemoutput.h                                             -*-C++-*-
#ifndef INCLUDED_BDLSB_FIXEDMEMOUTPUT
#define INCLUDED_BDLSB_FIXEDMEMOUTPUT

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a basic output stream buffer using a client buffer.
//
//@CLASSES:
//  bdlsb::FixedMemOutput: basic output stream buffer using client memory
//
//@SEE_ALSO: bdlsb_fixedmemoutstreambuf
//
//@DESCRIPTION: This component implements the output portion of the
// 'bsl::basic_streambuf' protocol using a client-supplied memory buffer.
// Method names correspond to the protocol-specified method names.  Clients
// supply the character buffer at stream buffer construction, and can later
// reinitialize the stream buffer with a different character buffer by calling
// the 'pubsetbuf' method.  The only difference between this component and
// 'bdlsb_fixedmemoutstreambuf' is that the class 'bdlsb::FixedMemOutput' does
// *not* derive from a 'bsl::streambuf' and does not support locales.  This is
// advantageous for performance reasons, as the overhead of the initialization
// and virtual function calls of a 'bsl::streambuf' can be undesirable.  The
// 'bdlsb::FixedMemOutput' is designed to be used by generic template code that
// must be instantiated on a type that matches the interface of
// 'bsl::streambuf', but does not require an actual 'bsl::streambuf', in
// particular 'bslx_genericoutstream'.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Use of 'bdlsb::FixedMemOutput'
///- - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates instantiating a template, bslx::GenericOutStream',
// on a 'bdlsb::FixedMemOutput' object and using the 'bslx::GenericOutStream'
// object to stream out some data.
//
// First, we create an object of our stream buffer:
//..
//  enum { k_STREAMBUF_CAPACITY = 30 };
//
//  char                  buffer[k_STREAMBUF_CAPACITY];
//  bdlsb::FixedMemOutput streamBuf(buffer, k_STREAMBUF_CAPACITY);
//..
// Then, we create an instance of 'bslx::GenericOutStream' using 'streamBuf',
// with an arbitrary value for its 'versionSelector', and externalize some
// values:
//..
//  bslx::GenericOutStream<bdlsb::FixedMemOutput> outStream(&streamBuf,
//                                                          20150707);
//  outStream.putInt32(1);
//  outStream.putInt32(2);
//  outStream.putInt8('c');
//  outStream.putString(bsl::string("hello"));
//..
// Finally, we compare the contents of the buffer to the expected value:
//..
//  assert(15 == streamBuf.length());
//  assert( 0 == bsl::memcmp(streamBuf.data(),
//                           "\x00\x00\x00\x01\x00\x00\x00\x02""c\x05""hello",
//                           15));
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

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_ALGORITHM
#include <bsl_algorithm.h>
#endif

#ifndef INCLUDED_BSL_CSTDLIB
#include <bsl_cstdlib.h>
#endif

#ifndef INCLUDED_BSL_CSTRING
#include <bsl_cstring.h>
#endif

#ifndef INCLUDED_BSL_IOS
#include <bsl_ios.h>  // 'bsl::streamsize'
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_LOCALE
#include <bsl_locale.h>
#endif

#ifndef INCLUDED_BSL_STREAMBUF
#include <bsl_streambuf.h>
#endif

namespace BloombergLP {
namespace bdlsb {

                              // ==============
                              // FixedMemOutput
                              // ==============

class FixedMemOutput {
    // This class, like 'bdlsb::FixedMemOutStreamBuf', implements the output
    // functionality of the 'basic_streambuf' interface, using client-supplied
    // 'char *' memory.  It has an identical interface to
    // 'bdlsb::FixedMemOutStreamBuf' but does *not* inherit from
    // 'bsl::streambuf'.  Thus, it is suitable for use as template parameter to
    // 'bslx::GenericOutStream' (but not to 'bslx::StreambufOutStream').  Note
    // that this class is not designed to be derived from.

  public:
    // TYPES
    typedef char                             char_type;
    typedef bsl::char_traits<char>::int_type int_type;
    typedef bsl::char_traits<char>::pos_type pos_type;
    typedef bsl::char_traits<char>::off_type off_type;
    typedef bsl::char_traits<char>           traits_type;

  private:
    // PRIVATE TYPE
    typedef bsls::Types::IntPtr              IntPtr;

    // PRIVATE DATA MEMBERS
    char            *d_buffer_p;  // output buffer
    bsl::streamsize  d_capacity;  // length of output buffer
    pos_type         d_pos;       // output cursor

    // NOT IMPLEMENTED
    FixedMemOutput(const FixedMemOutput&);
    FixedMemOutput& operator=(const FixedMemOutput&);

  public:
    // CREATORS
    FixedMemOutput(char *buffer, bsl::streamsize length);
        // Create an empty stream buffer that uses the specified character
        // 'buffer' of the specified 'length'.  The behavior is undefined
        // unless 'length == 0' or 'length > 0 && buffer != 0'.
        // Note that 'buffer' is held but not owned.

    //! ~FixedMemOutput();
        // Destroy this stream buffer.  Note that this method's definition is
        // compiler generated.

    // MANIPULATORS
    char *data();
        // Return a pointer providing modifiable access to the character buffer
        // held by this stream buffer (supplied at construction).

                             // *** 27.5.2.2.1 locales: ***

    bsl::locale pubimbue(const bsl::locale& loc);
        // Associate the specified locale 'loc' to this stream buffer.
        // Operation has no effect, because locales are not supported by this
        // component.  Return default constructed bsl::locale object.

                             // *** 27.5.2.2.2 buffer and positioning: ***

    FixedMemOutput *pubsetbuf(char            *buffer,
                              bsl::streamsize  length);
        // Reset the internal buffer of this stream to the specified 'buffer'
        // of the specified 'length'.  Note that the next write operation will
        // start at the beginning of 'buffer'.

    pos_type pubseekoff(off_type                offset,
                        bsl::ios_base::seekdir  fixedPosition,
                        bsl::ios_base::openmode which =
                            bsl::ios_base::in | bsl::ios_base::out);
        // Set the position indicator to the relative specified 'offset' from
        // the base position indicated by the specified 'fixedPosition' and
        // return the resulting absolute position on success or pos_type(-1)
        // on failure.  Optionally specify 'which' area of the stream buffer.
        // The seek operation will fail if 'which' does not include the flag
        // 'bsl::ios_base::out' or if the resulting absolute position is less
        // than zero or greater than the value returned by 'length'.

    pos_type pubseekpos(pos_type                position,
                        bsl::ios_base::openmode which =
                            bsl::ios_base::in | bsl::ios_base::out);
        // Set the position indicator to the specified 'position' and return
        // the resulting absolute position on success or pos_type(-1) on
        // failure.  Optionally specify 'which' area of the stream buffer.  The
        // 'seekpos' operation will fail if 'which' does not include the flag
        // 'bsl::ios_base::out' or if position is less then zero or greater
        // than the value returned by 'length'.

    int pubsync();
        // Synchronizes the controlled character sequence (the buffers) with
        // the associated character sequence.  Operation has no effect, because
        // the stream is always kept in sync (no buffered output).  Return 0.

                             // *** 27.5.2.2.5 Put area: ***

    int_type sputc(char c);
        // Write the specified character 'c' to this buffer.  Return 'c', or
        // 'traits_type::eof()' if the end of the write buffer is reached.

    bsl::streamsize sputn(const char *s, bsl::streamsize length);
        // Write the specified 'length' characters at the specified address 's'
        // to this buffer.  Return the number of characters written, which is
        // either 'length' or the distance from the current write position to
        // the end of the write buffer, whichever is smaller, and move the
        // write cursor position by this amount.

    // ACCESSORS
    bsl::streamsize capacity() const;
        // Return the size in bytes of the buffer held by this stream buffer.

    const char *data() const;
        // Return a pointer providing non-modifiable access to the character
        // buffer held by this stream buffer (supplied at construction).

    bsl::streamsize length() const;
        // Return the number of characters from the beginning of the buffer to
        // the current write position.

                             // *** 27.5.2.2.1 locales: ***

    bsl::locale getloc() const;
        // Return the current default locale.  Operation has no effect, because
        // locales are not supported by this component.  Return default
        // constructed bsl::locale object.

};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                              // --------------
                              // FixedMemOutput
                              // --------------

// CREATORS
inline
FixedMemOutput::FixedMemOutput(char            *buffer,
                               bsl::streamsize  length)
: d_buffer_p(buffer)
, d_capacity(length)
, d_pos(0)
{
    BSLS_ASSERT_SAFE(buffer || 0 == length);
    BSLS_ASSERT_SAFE(0 <= length);
}

// MANIPULATORS
inline
char *FixedMemOutput::data()
{
    return d_buffer_p;
}

inline
bsl::locale FixedMemOutput::pubimbue(const bsl::locale&)
{
    return bsl::locale();
}

inline
FixedMemOutput *FixedMemOutput::pubsetbuf(char            *buffer,
                                          bsl::streamsize  length)
{
    BSLS_ASSERT_SAFE(buffer || 0 == length);
    BSLS_ASSERT_SAFE(0 <= length);

    d_buffer_p = buffer;
    d_capacity = length;
    d_pos      = 0;

    return this;
}

inline
int FixedMemOutput::pubsync()
{
    // Nothing to do, the buffer is always up to date.
    return 0;
}

inline
FixedMemOutput::int_type FixedMemOutput::sputc(char c)
{
    if (d_pos >= d_capacity) {
        return traits_type::eof();                                    // RETURN
    }
    d_buffer_p[static_cast<IntPtr>(d_pos)] = c;
    d_pos += 1;
    return traits_type::to_int_type(c);
}

inline
bsl::streamsize FixedMemOutput::sputn(const char      *s,
                                      bsl::streamsize  length)
{
    BSLS_ASSERT_SAFE(s);
    BSLS_ASSERT_SAFE(0 <= length);

    pos_type current = d_pos;
    d_pos += length;
    if (d_pos > d_capacity) {
        d_pos  = d_capacity;
        length = static_cast<bsl::streamsize>(d_capacity - current);
    }
    bsl::memcpy(d_buffer_p + static_cast<IntPtr>(current), s, length);
    return length;
}

// ACCESSORS
inline
bsl::streamsize FixedMemOutput::capacity() const
{
    return d_capacity;
}

inline
const char *FixedMemOutput::data() const
{
    return d_buffer_p;
}

inline
bsl::locale FixedMemOutput::getloc() const
{
    return bsl::locale();
}

inline
bsl::streamsize FixedMemOutput::length() const
{
    return bsl::streamsize(d_pos);
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
