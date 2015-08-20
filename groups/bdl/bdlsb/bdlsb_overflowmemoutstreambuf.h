// bdlsb_overflowmemoutstreambuf.h                                    -*-C++-*-
#ifndef INCLUDED_BDLSB_OVERFLOWMEMOUTSTREAMBUF
#define INCLUDED_BDLSB_OVERFLOWMEMOUTSTREAMBUF

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an overflowable output 'streambuf' using a client buffer.
//
//@CLASSES:
//  bdlsb::OverflowMemOutStreamBuf: overflowable output 'bsl::streambuf'
//
//@SEE_ALSO: bdlsb_overflowmemoutput
//
//@DESCRIPTION: This component implements the output portion of the
// 'bsl::basic_streambuf' protocol using a user-supplied initial buffer and a
// dynamically allocated overflow buffer.  As with 'bdlsb_fixedmemoutput',
// users supply the character buffer at construction.  Unlike
// 'bdlsb_fixedmemoutput', they can no longer reinitialize the stream buffer
// with a different character buffer by calling the 'pubsetbuf' method;
// instead, if that buffer runs out, the 'bdlsb::OverflowMemOutput' will
// allocate another buffer (see "Overflow Buffer" below).   The only difference
// between this component and 'bdlsb_overflowmemoutput' is that the class
// 'bdlsb::OverflowMemOutStreamBuf' is derived from a 'bsl::streambuf'.  Method
// names necessarily correspond to the protocol-specified method names.  Refer
// to the C++ Standard, Section 27.5.2, for a full specification of the
// 'bsl::basic_streambuf' interface.  This component provides none of the
// input-related functionality of 'basic_streambuf' (see Streaming
// Architecture, below), nor does it use locales in any way.
//
///Overflow Buffer
///---------------
// This output stream buffer will use the initial buffer (supplied at
// construction) as its output buffer.  If an overflow of the initial buffer
// were to occur, an additional buffer (the overflow buffer) will be allocated.
// If this overflow buffer ever becomes full, it will be automatically grown.
// The overflow buffer grows geometrically (to twice the current overflow
// buffer size) whenever the amount of data written exceeds the amount of space
// available.  On growth, the old overflow buffer is copied over to the newly
// allocated overflow buffer, and then deallocated, thus after any write/seek
// forward one cannot assume that the overflow buffer is still the same memory.
// Data in the overflow buffer beyond the reach of the current write position
// is not guaranteed to be preserved after a growth operation.
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
/// Example 1: Basic Use of 'bdlsb::OverflowMemOutStreamBuf'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates how a stream buffer can be used for testing of a
// certain stream.  bdlsb::OverflowMemOutStreamBuf provides a way to inspect
// the data that has been processed by the stream.  In this case we will create
// a stream with simple formatting requirements - namely, capitalizing all
// lower-case ASCII character data that is output.  To simplify the example, we
// do not include the functions for streaming non-character data, e.g., numeric
// values.
//
// First, we define a stream class, that will use our stream buffer:
//..
//  class CapitalizingStream {
//      // This class capitalizes lower-case ASCII characters that are output.
//
//      // PRIVATE TYPES
//      enum { STREAMBUF_CAPACITY = 10 };
//
//      // DATA
//      char                           *d_buffer;      // initial buffer
//                                                     // (owned)
//
//      bdlsb::OverflowMemOutStreamBuf *d_streamBuf;   // stream buffer (owned)
//      bslma::Allocator               *d_allocator_p; // memory allocator
//                                                     // (held, not owned)
//
//      // FRIENDS
//      friend CapitalizingStream& operator<<(CapitalizingStream& stream,
//                                            const bsl::string&  data);
//      friend CapitalizingStream& operator<<(CapitalizingStream&  stream,
//                                            const char          *data);
//      friend CapitalizingStream& operator<<(CapitalizingStream& stream,
//                                            char                data);
//    public:
//      // TRAITS
//      BSLALG_DECLARE_NESTED_TRAITS(CapitalizingStream,
//                                   bslalg::TypeTraitUsesBslmaAllocator);
//
//      // CREATORS
//      CapitalizingStream(bslma::Allocator *basicAllocator = 0);
//          // Create a capitalizing stream.  Optionally specify
//          // 'basicAllocator' to provide user-defined allocator.
//
//      ~CapitalizingStream();
//          // Destroy this capitalizing stream.
//
//      // ACCESSORS
//      const bdlsb::OverflowMemOutStreamBuf *streamBuf() {return d_streamBuf;}
//          // Return the stream buffer used by this stream.  Note that this
//          // function is for debugging only.
//  };
//
//  // FREE OPERATORS
//  CapitalizingStream& operator<<(CapitalizingStream& stream,
//                                 const bsl::string&  data);
//  CapitalizingStream& operator<<(CapitalizingStream&  stream,
//                                 const char          *data);
//  CapitalizingStream& operator<<(CapitalizingStream& stream,
//                                 char                data);
//      // Write the specified 'data' in capitalized form to the specified
//      // 'stream'.
//
//  CapitalizingStream::CapitalizingStream(bslma::Allocator *basicAllocator)
//  : d_allocator_p(bslma::Default::allocator(basicAllocator))
//  {
//      d_buffer = reinterpret_cast<char*>(
//                                d_allocator_p->allocate(STREAMBUF_CAPACITY));
//
//      d_streamBuf = new(*d_allocator_p) bdlsb::OverflowMemOutStreamBuf(
//                                                          d_buffer,
//                                                          STREAMBUF_CAPACITY,
//                                                          d_allocator_p);
//  }
//
//  CapitalizingStream::~CapitalizingStream()
//  {
//      d_allocator_p->deleteObjectRaw(d_streamBuf);
//      d_allocator_p->deleteObjectRaw(d_buffer);
//  }
//
//..
// As is typical, the streaming operators are made friends of the class.  We
// use the 'transform' algorithm to convert lower-case characters to
// upper-case.
//..
//  // FREE OPERATORS
//  CapitalizingStream& operator<<(CapitalizingStream& stream,
//                                 const bsl::string&  data)
//  {
//      bsl::string tmp(data);
//      bsl::transform(tmp.begin(),
//                     tmp.end(),
//                     tmp.begin(),
//                     static_cast<int(*)(int)>(bsl::toupper));
//      stream.d_streamBuf->sputn(tmp.data(), tmp.length());
//      return stream;
//  }
//
//  CapitalizingStream& operator<<(CapitalizingStream&  stream,
//                                 const char          *data)
//  {
//      bsl::string tmp(data);
//      bsl::transform(tmp.begin(),
//                     tmp.end(),
//                     tmp.begin(),
//                     static_cast<int(*)(int)>(bsl::toupper));
//      stream.d_streamBuf->sputn(tmp.data(), tmp.length());
//      return stream;
//  }
//
//  CapitalizingStream& operator<<(CapitalizingStream& stream,
//                                 char                data)
//  {
//       stream.d_streamBuf->sputc(static_cast<char>(bsl::toupper(data)));
//       return stream;
//  }
//..
// Now, we create an object of our stream and write some words to it:
//..
//  bslma::TestAllocator             ta("test", false);
//  UsageExample::CapitalizingStream cs(&ta);
//  cs << "Hello" << ' ' << "world." << '\0';
//..
// Finally, we are able to verify that the streamed data has been capitalized:
//..
//  if (verbose) {
//      // Visually verify that the streamed data has been capitalized.
//      bsl::string iBuf(cs.streamBuf()->initialBuffer(),
//                       cs.streamBuf()->dataLengthInInitialBuffer());
//      bsl::string oBuf(cs.streamBuf()->overflowBuffer(),
//                       cs.streamBuf()->dataLengthInOverflowBuffer());
//      cout << iBuf << oBuf << endl;
//  }
//
//  assert(10 == cs.streamBuf()->dataLengthInInitialBuffer());
//  assert(0 == strncmp("HELLO WORL",
//                      cs.streamBuf()->initialBuffer(),
//                      cs.streamBuf()->dataLengthInInitialBuffer()));
//  assert(3 == cs.streamBuf()->dataLengthInOverflowBuffer());
//  assert(0 == strncmp("D.",
//                      cs.streamBuf()->overflowBuffer(),
//                      cs.streamBuf()->dataLengthInOverflowBuffer()));
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
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

#ifndef INCLUDED_BSL_STREAMBUF
#include <bsl_streambuf.h>
#endif

namespace BloombergLP {
namespace bdlsb {

                   // =============================
                   // class OverflowMemOutStreamBuf
                   // =============================

class OverflowMemOutStreamBuf : public bsl::streambuf {
    // This class implements the output functionality of the
    // 'bsl::basic_streambuf' protocol, using client-supplied memory and
    // client-supplied allocator if additional memory is needed.  It does
    // derive from 'bsl::streambuf', thus it is suitable for use as template
    // parameter to 'bslx::GenericByteOutStream' as well as
    // 'bslx::ByteOutStream'.

    // PRIVATE TYPES
    typedef bsl::ios_base ios_base;

    // DATA
    // The order of the data members is determined by our usage pattern for
    // cache efficiency.  Do not reorder them.
    //
    // 'd_dataLength' is used for caching current stream buffer workload.
    // Characters can be written to stream buffer by base class method 'sputc',
    // so we need to update this variable before it's value usage. As it can be
    // changed even in accessors methods, it has 'mutable' qualifier.

    mutable bsl::size_t  d_dataLength;            // total data length
    char                *d_initialBuffer_p;       // user-supplied buffer
                                                  // (held, not owned)

    bsl::size_t          d_initialBufferSize;     // size of initial buffer
    bool                 d_inOverflowBufferFlag;  // true if 'pptr' points into
                                                  // the overflow buffer

    char                *d_overflowBuffer_p;      // overflow buffer (owned)
    bsl::size_t          d_overflowBufferSize;    // size of overflow buffer
    bslma::Allocator    *d_allocator_p;           // memory allocator (held,
                                                  // not owned)

    // NOT IMPLEMENTED
    OverflowMemOutStreamBuf(const OverflowMemOutStreamBuf&);
    OverflowMemOutStreamBuf& operator=(const OverflowMemOutStreamBuf&);

  private:
    // PRIVATE MANIPULATORS
    void grow(bsl::size_t numBytes);
        // Replace the overflow buffer with another buffer larger by at least
        // the specified 'numBytes', by growing geometrically by a factor of
        // two, and preserve the bytes that are in use by the overflow buffer,
        // as determined by the 'd_dataLength'.  Note that 'pptr()' is not
        // updated at this time, and may be pointing to deallocated memory when
        // this returns.

    void privateSync() const;
        // Set 'd_dataLength' to the amount of valid data that is considered
        // written to this stream by calculating the offset from 'pptr()' to
        // the beginning of the buffer.  If 'pptr()' points into the overflow
        // buffer, the size of the initial buffer is added on.

  protected:
    // PROTECTED VIRTUAL FUNCTIONS

           // *** 27.5.2.4.2 buffer management and positioning ***
    virtual pos_type seekoff(
       off_type                offset,
       bsl::ios_base::seekdir  fixedPosition,
       bsl::ios_base::openmode which = bsl::ios_base::in | bsl::ios_base::out);
        // Set the location from which the next I/O operation will occur to the
        // specified 'offset' position from the location indicated by the
        // specified 'fixedPosition'.  Optionally specify 'which' mode to
        // determine the position on which of the controlled sequences shall be
        // modified: the input sequence, the output sequence, or both.  Return
        // the new position on success, and 'pos_type(-1)' otherwise.  'offset'
        // may be negative.  Note that this method will fail if 'mode &
        // bsl::ios_base::in' is not 0.

    virtual pos_type seekpos(
       pos_type                position,
       bsl::ios_base::openmode which = bsl::ios_base::in | bsl::ios_base::out);
        // Set the location from which the next I/O operation will occur to the
        // specified 'position'.  Optionally specify 'which' mode to determine
        // the position on which of the controlled sequences shall be modified:
        // the input sequence, the output sequence, or both.  Return 'position'
        // on success, and 'pos_type(-1)' otherwise.  Note that this method
        // will fail if 'mode & bsl::ios_base::in' is not 0.

    virtual int sync();
        // Set 'd_dataLength' to the amount of valid data that is considered
        // written to this stream by calculating the offset from 'pptr()' to
        // the beginning of the buffer.  If the 'pptr()' points into the
        // overflow buffer, the size of the initial buffer is added on.
        // Return 0 unconditionally.

    virtual bsl::streamsize xsputn(const char_type *source,
                                   bsl::streamsize  numChars);
        // Put the specified 'numChars' from the specified 'source' to the
        // stream.  Return the number of characters successfully put.  The
        // behavior is undefined unless '0 <= numChars'.

    virtual int_type overflow(int_type c = bsl::streambuf::traits_type::eof());
        // If c is not 'EOF', adjust the underlying buffer so that the next put
        // position is valid, put the specified 'c' at this position, and
        // increment the put position.  Return 'c' on success, and
        // 'traits_type::not_eof(c)' if 'c' is 'EOF'.

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(OverflowMemOutStreamBuf,
                                   bslma::UsesBslmaAllocator);

    // CREATORS
    OverflowMemOutStreamBuf(char             *buffer,
                            bsl::size_t       size,
                            bslma::Allocator *basicAllocator = 0);
        // Create an empty stream buffer that uses the specified 'buffer' as an
        // initial output buffer of the specified 'size' (in bytes).
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently-installed default allocator is
        // used.  Note that this stream buffer does not assume ownership of
        // 'buffer'.

    virtual ~OverflowMemOutStreamBuf();
        // Destroy this stream buffer.

    // ACCESSORS
    bsl::size_t dataLength() const;
        // Return the number of bytes written and synced in this object.  Note
        // that if 'pptr()' is currently pointing into the overflow buffer the
        // data length will be greater than the size of the initial buffer.
        //
    bsl::size_t dataLengthInInitialBuffer() const;
        // Return the length of data in the initial buffer, i.e.,
        // 'dataLength()' if there is no overflow buffer, or
        // 'initialBufferSize()' if there is one.
        //
    bsl::size_t dataLengthInOverflowBuffer() const;
        // Return the length of the data in the overflow buffer, i.e., 0 if
        // there is no overflow buffer, or 'dataLength() - initialBufferSize()'
        // if there is one.

    const char *initialBuffer() const;
        // Return a pointer to the non-modifiable buffer specified at
        // construction.

    bsl::size_t initialBufferSize() const;
        // Return the size of the buffer specified at construction.

    const char *overflowBuffer() const;
        // Return a pointer to the non-modifiable overflow buffer if there is
        // one, or 0 otherwise.

    bsl::size_t overflowBufferSize() const;
        // Return the size of the overflow buffer, or 0 if there is no overflow
        // buffer.
};

// ============================================================================
//                          INLINE DEFINITIONS
// ============================================================================

                    // -----------------------------
                    // class OverflowMemOutStreamBuf
                    // -----------------------------

// PROTECTED VIRTUAL FUNCTIONS
inline
OverflowMemOutStreamBuf::pos_type
OverflowMemOutStreamBuf::seekpos(pos_type                position,
                                 bsl::ios_base::openmode which)
{
    return seekoff(off_type(position), bsl::ios_base::beg, which);
}

inline
int OverflowMemOutStreamBuf::sync()
{
    privateSync();
    return 0;
}

// CREATORS
inline
OverflowMemOutStreamBuf::~OverflowMemOutStreamBuf()
{
    d_allocator_p->deallocate(d_overflowBuffer_p);
}

// ACCESSORS
inline
bsl::size_t OverflowMemOutStreamBuf::dataLength() const
{
    privateSync();
    return d_dataLength;
}

inline
bsl::size_t OverflowMemOutStreamBuf::dataLengthInInitialBuffer() const
{
    privateSync();
    return d_inOverflowBufferFlag ? d_initialBufferSize : d_dataLength;
}

inline
bsl::size_t OverflowMemOutStreamBuf::dataLengthInOverflowBuffer() const
{
    privateSync();
    return d_inOverflowBufferFlag ? d_dataLength - d_initialBufferSize : 0;
}

inline
const char *OverflowMemOutStreamBuf::initialBuffer() const
{
    return d_initialBuffer_p;
}

inline
bsl::size_t OverflowMemOutStreamBuf::initialBufferSize() const
{
    return d_initialBufferSize;
}

inline
const char *OverflowMemOutStreamBuf::overflowBuffer() const
{
    return d_overflowBuffer_p;
}

inline
bsl::size_t OverflowMemOutStreamBuf::overflowBufferSize() const
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
