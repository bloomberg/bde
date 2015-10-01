// bdlsb_overflowmemoutput.h                                          -*-C++-*-
#ifndef INCLUDED_BDLSB_OVERFLOWMEMOUTPUT
#define INCLUDED_BDLSB_OVERFLOWMEMOUTPUT

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an overflowable output 'streambuf' using a client buffer.
//
//@CLASSES:
// bdlsb::OverflowMemOutput: overflowable output stream buffer
//
//@SEE_ALSO: bdlsb_fixedmemoutput, bdlsb_overflowmemoutstreambuf
//
//@DESCRIPTION: This component provides a mechanism,
// 'bdlsb::OverflowMemOutput', that implements the output portion of the
// 'bsl::basic_streambuf' protocol using a user-supplied memory buffer and a
// managed, allocator-supplied overflow buffer that is created when the
// client-supplied buffer runs out.  Method names necessarily correspond to the
// protocol-specified method names.  As with 'bdlsb_overflowmemoutstreambuf',
// clients supply the character buffer at construction.  Unlike
// 'bdlsb_fixedmemoutput', they can no longer reinitialize the stream buffer
// with a different character buffer by calling the 'pubsetbuf' method;
// instead, if that buffer runs out, the 'bdlsb::OverflowMemOutput' will
// allocate another buffer (see "Overflow Buffer" below).  The only difference
// between this component and 'bdlsb_overflowmemoutstreambuf' is that the class
// 'bdlsb::OverflowMemOutput' does *not* derive from 'bsl::streambuf' and does
// not support locales.  This is advantageous for performance reasons, as the
// overhead of the initialization and virtual function calls of a
// 'bsl::streambuf' can be undesirable.  The 'bdlsb::OverflowMemOutput' is
// designed to be used by generic template code that must be instantiated on a
// type that matches the interface of 'bsl::streambuf', but does not require an
// actual 'bsl::streambuf', in particular 'bslx_genericoutstream'.
//
///Overflow Buffer
///---------------
// This output stream buffer uses the initial buffer (supplied at construction)
// as its output buffer.  If an overflow of the initial buffer were to occur,
// an additional buffer (the overflow buffer) will be allocated.  If this
// overflow buffer ever becomes full, it will be automatically grown.  The
// overflow buffer grows geometrically (to twice the current overflow buffer
// size) whenever the amount of data written exceeds the amount of space
// available.  On growth, the old overflow buffer is copied over to the newly
// allocated overflow buffer, and then deallocated, thus after any write one
// cannot assume that the overflow buffer is still the same memory.  Data in
// the overflow buffer beyond the reach of the current write position is not
// guaranteed to be preserved after a growth operation.
//
///Streaming Architecture
///----------------------
// Stream buffers are designed to decouple device handling from content
// formatting, providing the requisite device handling and possible buffering
// services, and leaving the formatting to the client stream.  The standard C++
// IOStreams library further partitions streaming into input streaming and
// output streaming, separating responsibilities for each at both the stream
// layer and the stream buffer layer.  The BDE streaming library for 'blsx',
// including all of 'bdlsb', follows this model.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Use of 'bdlsb::OverflowMemOutput'
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates instantiating a template,
// 'bslx::GenericOutStream', on a 'bdlsb::OverflowMemOutput' object and using
// the 'bslx::GenericOutStream' object to stream out some data.
//
// First, we create a stream buffer, 'streamBuf', and supply it stack allocated
// memory as its initial buffer:
//..
//  enum { k_STREAMBUF_CAPACITY = 8 };
//
//  char                     buffer[k_STREAMBUF_CAPACITY];
//  bdlsb::OverflowMemOutput streamBuf(buffer, k_STREAMBUF_CAPACITY);
//..
// Then, we create an instance of 'bslx::GenericOutStream' using 'streamBuf',
// with an arbitrary value for its 'versionSelector', and serialize some data:
//..
//  bslx::GenericOutStream<bdlsb::OverflowMemOutput> outStream(&streamBuf,
//                                                             20150707);
//  int MAGIC = 0x1812;
//  outStream.putInt32(MAGIC);
//  outStream.putInt32(MAGIC+1);
//..
// Next, we verify that the data was correctly serialized and completely filled
// initial buffer supplied at the stream buffer construction:
//..
//  assert(outStream.isValid());
//  assert(8 == streamBuf.dataLength());
//  assert(0 == bsl::memcmp(streamBuf.initialBuffer(),
//                          "\x00\x00\x18\x12\x00\x00\x18\x13",
//                          8));
//  assert(0 == bsl::memcmp(buffer, "\x00\x00\x18\x12\x00\x00\x18\x13", 8));
//  assert(0 == streamBuf.overflowBuffer());
//  assert(0 == streamBuf.overflowBufferSize());
//..
// Then, we serialize some more data to trigger allocation of the internal
// overflow buffer:
//..
//  outStream.putString(bsl::string("test"));
//..
// Finally, we verify that the additional data was serialized correctly and
// landed into dynamically allocated overflow buffer:
//..
//  assert(outStream.isValid());
//  assert(13 == streamBuf.dataLength());
//  assert(0  != streamBuf.overflowBuffer());
//  assert(5  == streamBuf.dataLengthInOverflowBuffer());
//  assert(0  == bsl::memcmp(streamBuf.overflowBuffer(), "\x04test", 5));
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

#ifndef INCLUDED_BSL_CSTDDEF
#include <bsl_cstddef.h>
#endif

#ifndef INCLUDED_BSL_IOS
#include <bsl_ios.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {
namespace bdlsb {

                       // =======================
                       // class OverflowMemOutput
                       // =======================

class OverflowMemOutput {
    // This class, like 'bdlsb::OverflowMemOutStreamBuf', implements the output
    // functionality of the 'bsl::basic_streambuf' interface, using a
    // client-supplied buffer and allocator-supplied overflow buffer if
    // additional memory is needed.  It has an interface similar to
    // 'bdlsb::OverflowMemOutStreamBuf' but does *not* inherit from
    // 'bsl::streambuf'.  Thus, it is suitable for use as template parameter to
    // 'bslx::GenericByteOutStream' (but not to 'bslx::ByteOutStream' or
    // 'bslx::ByteOutStreamFormatter').  Note that this class is not designed
    // to be derived from.

    // DATA

    // The order of the data members is determined by our usage pattern for
    // cache efficiency.  Do not reorder them.

    bsl::size_t       d_dataLength;            // total data length

    char             *d_put_p;                 // current put pointer

    char             *d_initialBuffer_p;       // user-supplied buffer (held,
                                               // not owned)

    bsl::size_t       d_initialBufferSize;     // size of 'd_initialBuffer_p'
                                               // buffer

    bool              d_inOverflowBufferFlag;  // true if 'd_put_p' points into
                                               // the overflow buffer

    char             *d_overflowBuffer_p;      // overflow buffer (owned)

    bsl::size_t       d_overflowBufferSize;    // size of 'd_overflowBuffer_p'
                                               // buffer

    bslma::Allocator *d_allocator_p;           // memory allocator (held, not
                                               // owned)

    // NOT IMPLEMENTED
    OverflowMemOutput(const OverflowMemOutput&);
    OverflowMemOutput& operator=(const OverflowMemOutput&);

  private:
    // PRIVATE MANIPULATORS
    void grow(bsl::size_t numBytes, bool copyOrigin = true);
        // Replace the overflow buffer with another buffer, larger then the
        // current buffer by at least the specified 'numBytes', by growing
        // geometrically by a factor of two.  Optionally specify a 'copyOrigin'
        // indicating whether the content of the overflow buffer should be
        // copied into new location.  Note that 'd_put_p' is not updated, and
        // may be pointing to deallocated memory when the method returns.

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(OverflowMemOutput,
                                   bslma::UsesBslmaAllocator);

    // TYPES
    typedef char                    char_type;
    typedef bsl::char_traits<char>  traits_type;
    typedef traits_type::int_type   int_type;
    typedef traits_type::pos_type   pos_type;
    typedef traits_type::off_type   off_type;

    // CREATORS
    OverflowMemOutput(char             *buffer,
                      bsl::size_t       length,
                      bslma::Allocator *basicAllocator = 0);
        // Create an 'OverflowMemOutput' using the specified 'buffer' of the
        // specified 'length' as the initial output buffer.  Optionally specify
        // a 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  The behavior is
        // undefined unless 'buffer != 0 && length > 0'.  Note that this stream
        // buffer does not assume ownership of 'buffer'.

    ~OverflowMemOutput();
        // Destroy this stream buffer.

    // MANIPULATORS
                             // *** 27.5.2.2.2 buffer and positioning ***

    pos_type pubseekoff(off_type                offset,
                        bsl::ios_base::seekdir  way,
                        bsl::ios_base::openmode which = bsl::ios_base::out);
        // Set the position indicator to the relative specified 'offset' from
        // the base position indicated by the specified 'way' and return the
        // resulting absolute position on success or pos_type(-1) on failure.
        // Optionally specify 'which' area of the stream buffer.  The seek
        // operation will fail if 'which' does not include the flag
        // 'bsl::ios_base::out' or if the resulting absolute position is less
        // than zero.

    pos_type pubseekpos(pos_type                position,
                        bsl::ios_base::openmode which = bsl::ios_base::out);
        // Set the position indicator to the specified 'position' and return
        // the resulting absolute position on success or pos_type(-1) on
        // failure.  Optionally specify 'which' area of the stream buffer.  The
        // 'seekpos' operation will fail if 'which' does not include the flag
        // 'bsl::ios_base::out' or if 'position' is less then zero.

    OverflowMemOutput *pubsetbuf(char *buffer, bsl::streamsize length);
        // Reinitialize this stream buffer to use the specified character
        // 'buffer' having the specified 'length'.  Return a pointer providing
        // modifiable access to this stream buffer.  This stream buffer does
        // not support reinitialization of the internal character buffer.

    int pubsync();
        // Synchronize this stream buffer with associated character sequence.
        // Operation has no effect.  Return '0' unconditionally.

                             // *** 27.5.2.2.5 put area ***

    int_type sputc(char c);
        // Write the specified character 'c' at the current write position and
        // advance write position of this buffer.  Return 'c', or
        // 'traits_type::eof()' if the end of the write buffer is reached.

    bsl::streamsize sputn(const char *source, bsl::streamsize length);
        // Write the specified 'length' characters from the specified 'source'
        // to this buffer.  Return the number of characters written, which is
        // either 'length' or the distance from the current write position to
        // the end of the write buffer, whichever is smaller, and move the
        // write cursor position by this amount.  The behaviour is undefined
        // unless 'source != 0 || length > 0'.

    // ACCESSORS
    bsl::size_t dataLength() const;
        // Return the number of bytes that have been written to this object.

    bsl::size_t dataLengthInInitialBuffer() const;
        // Return the length of data in the initial buffer, i.e.,
        // 'dataLength()' if there is no overflow buffer, or
        // 'initialBufferSize()' if there is one.

    bsl::size_t dataLengthInOverflowBuffer() const;
        // Return the length of the data in the overflow buffer, i.e., 0 if
        // there is no overflow buffer, or 'dataLength() - initialBufferSize()'
        // if there is one.

    const char *initialBuffer() const;
        // Return a pointer providing non-modifiable access to the character
        // buffer held by this stream buffer (specified at construction).

    bsl::size_t initialBufferSize() const;
        // Return the size of the initial buffer held by this stream buffer.

    const char *overflowBuffer() const;
        // Return a pointer providing non-modifiable access to the overflow
        // buffer if there is one, or 0 otherwise.

    bsl::size_t overflowBufferSize() const;
        // Return the size of the overflow buffer, or 0 if there is no overflow
        // buffer.
};

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                       // -----------------------
                       // class OverflowMemOutput
                       // -----------------------

// CREATORS
inline
OverflowMemOutput::~OverflowMemOutput()
{
    d_allocator_p->deallocate(d_overflowBuffer_p);
}

// MANIPULATORS
inline
OverflowMemOutput::pos_type
OverflowMemOutput::pubseekpos(pos_type position, bsl::ios_base::openmode which)
{
    return pubseekoff(off_type(position), bsl::ios_base::beg, which);
}

inline
OverflowMemOutput *OverflowMemOutput::pubsetbuf(char *, bsl::streamsize)
{
    return this;
}

inline
int OverflowMemOutput::pubsync()
{
    return 0;
}

// ACCESSORS
inline
bsl::size_t OverflowMemOutput::dataLength() const
{
    return d_dataLength;
}

inline
bsl::size_t OverflowMemOutput::dataLengthInInitialBuffer() const
{
    return d_inOverflowBufferFlag ? d_initialBufferSize : d_dataLength;
}

inline
bsl::size_t OverflowMemOutput::dataLengthInOverflowBuffer() const
{
    return d_inOverflowBufferFlag ? d_dataLength - d_initialBufferSize : 0;
}

inline
const char *OverflowMemOutput::initialBuffer() const
{
    return d_initialBuffer_p;
}

inline
bsl::size_t OverflowMemOutput::initialBufferSize() const
{
    return d_initialBufferSize;
}

inline
const char *OverflowMemOutput::overflowBuffer() const
{
    return d_overflowBuffer_p;
}

inline
bsl::size_t OverflowMemOutput::overflowBufferSize() const
{
    return d_overflowBufferSize;
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
