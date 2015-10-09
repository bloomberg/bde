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
// allocate another buffer (see "Overflow Buffer" below).  The only difference
// between this component and 'bdlsb_overflowmemoutput' is that the class
// 'bdlsb::OverflowMemOutput' *does* derive from a 'bsl::streambuf'.  Method
// names necessarily correspond to the protocol-specified method names.  Refer
// to the C++ Standard, Sect.  27.5.2, for a full specification of the
// 'bsl::basic_streambuf' interface.  This component provides none of the
// input-related functionality of 'basic_streambuf' (see Streaming
// Architecture, below), nor does it use locales in any way.
//
///Overflow Buffer
///---------------
// This output streambuf will use the initial buffer (supplied at construction)
// as its output buffer.  If an overflow of the initial buffer were to occur,
// an additional buffer (the overflow buffer) will be allocated.  If this
// overflow buffer ever becomes full, it will be automatically grown.  The
// overflow buffer grows geometrically (to twice the current overflow buffer
// size) whenever the amount of data written exceeds the amount of space
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
// services, and leaving the formatting to the client stream.  The standard C++
// IOStreams library further partitions streaming into input streaming and
// output streaming, separating responsibilities for each at both the stream
// layer and the stream buffer layer.  The BDE streaming library for 'bdex',
// including all of 'bdesb', follows this model.
//
///Usage
///-----
// This example demonstrates use of a stream buffer by a stream, in this case a
// stream with simple formatting requirements -- namely, capitalizing all
// character data that passes through its management.  (To simplify the
// example, we do not include the functions for streaming non-character data.)
//
// The stream uses a user-supplied 'char'-array-based stream buffer, which is
// inherently a fixed-size buffer.
//..
//  // my_capitalizingstream.h
//
//  class my_CapitalizingStream {
//      // This class capitalizes character data....
//
//      // PRIVATE TYPES
//      enum { k_STREAMBUF_CAPACITY = 10 };
//
//      // DATA
//      char                           *d_buffer;       // initial buffer
//                                                      // (owned)
//
//      bdlsb::OverflowMemOutStreamBuf *d_streamBuf;    // stream buffer
//                                                      // (owned)
//
//      bslma::Allocator               *d_allocator_p;  // memory allocator
//                                                      // (held, not owned)
//
//      // FRIENDS
//      friend
//      my_CapitalizingStream& operator<<(my_CapitalizingStream&  stream,
//                                        const bsl::string&      data);
//      friend
//      my_CapitalizingStream& operator<<(my_CapitalizingStream&  stream,
//                                        const char             *data);
//      friend
//      my_CapitalizingStream& operator<<(my_CapitalizingStream&  stream,
//                                        char                    data);
//    public:
//      // TRAITS
//      BSLALG_DECLARE_NESTED_TRAITS(my_CapitalizingStream,
//                                   bslalg::TypeTraitUsesBslmaAllocator);
//
//      // CREATORS
//      my_CapitalizingStream(bslma::Allocator *allocator = 0);
//          // Create a stream that capitalizes everything.
//
//      ~my_CapitalizingStream();
//          // Destroy this object.
//
//      // ACCESSORS
//      const bdlsb::OverflowMemOutStreamBuf *streamBuf();
//          // Return the stream buffer used by this stream.  Note that this
//          // function is for debugging only.
//  };
//
//  // FREE OPERATORS
//  my_CapitalizingStream& operator<<(my_CapitalizingStream&  stream,
//                                    const bsl::string&      data);
//  my_CapitalizingStream& operator<<(my_CapitalizingStream&  stream,
//                                    const char             *data);
//  my_CapitalizingStream& operator<<(my_CapitalizingStream&  stream,
//                                    char                    data);
//      // Write the specified 'data' in capitalized form to the
//      // specified 'stream'.
//..
// As is typical, the streaming operators are made friends of the class.  We
// use the 'transform' algorithm to convert all string characters to upper-
// case.
//..
//  // my_capitalizingstream.cpp
//
//  // CREATORS
//  my_CapitalizingStream::my_CapitalizingStream(
//                                            bslma::Allocator *basicAllocator)
//  : d_allocator_p(bslma::Default::allocator(basicAllocator))
//  {
//      d_buffer = reinterpret_cast<char*>(
//                              d_allocator_p->allocate(k_STREAMBUF_CAPACITY));
//
//      d_streamBuf = new(*d_allocator_p) bdlsb::OverflowMemOutStreamBuf(
//                                                        d_buffer,
//                                                        k_STREAMBUF_CAPACITY,
//                                                        d_allocator_p);
//  }
//
//  my_CapitalizingStream::~my_CapitalizingStream()
//  {
//      d_allocator_p->deleteObjectRaw(d_streamBuf);
//      d_allocator_p->deleteObjectRaw(d_buffer);
//  }
//
//  // ACCESSORS
//  const bdlsb::OverflowMemOutStreamBuf *my_CapitalizingStream::streamBuf()
//  {
//      return d_streamBuf;
//  }
//
//  // FREE OPERATORS
//  my_CapitalizingStream& operator<<(my_CapitalizingStream& stream,
//                                    const bsl::string&     data)
//  {
//      bsl::string tmp(data);
//      bsl::transform(tmp.begin(),
//                     tmp.end(),
//                     tmp.begin(),
//                     (int(*)(int))bsl::toupper);
//      stream.d_streamBuf->sputn(tmp.data(), tmp.length());
//      return stream;
//  }
//
//  my_CapitalizingStream& operator<<(my_CapitalizingStream&  stream,
//                                    const char             *data)
//  {
//      bsl::string tmp(data);
//      bsl::transform(tmp.begin(),
//                     tmp.end(),
//                     tmp.begin(),
//                     (int(*)(int))bsl::toupper);
//      stream.d_streamBuf->sputn(tmp.data(), tmp.length());
//      return stream;
//  }
//
//  my_CapitalizingStream& operator<<(my_CapitalizingStream& stream,
//                                    char                   data)
//  {
//       stream.d_streamBuf->sputc(bsl::toupper(data));
//       stream.d_streamBuf->pubsync();
//       return stream;
//  }
//..
// Given the above two functions, we can now write 'main', as follows:
//..
//  // my_app.m.cpp
//
//  bslma::TestAllocator allocator;
//
//  {
//      my_CapitalizingStream cs(&allocator);
//      cs << "Hello" << ' ' << "world." << '\0';
//
//      assert(10 == cs.streamBuf()->dataLengthInInitialBuffer());
//      assert(0 == strncmp("HELLO WORLD", cs.streamBuf()->initialBuffer(),
//                          cs.streamBuf()->dataLengthInInitialBuffer()));
//      assert(3 == cs.streamBuf()->dataLengthInOverflowBuffer());
//      assert(0 == strncmp("D.", cs.streamBuf()->overflowBuffer(),
//                          cs.streamBuf()->dataLengthInOverflowBuffer()));
//  }
//
//  ASSERT(0 <  allocator.numAllocations());
//  ASSERT(0 == allocator.numBytesInUse());
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
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
    // parameter to 'bslx::GenericOutStream' as well as
    // 'bslx::StreamBufOutStream'.

    // PRIVATE TYPES
    typedef bsl::ios_base ios_base;

    // DATA
    // The order of the data members is determined by our usage pattern for
    // cache efficiency.  Do not reorder them.

    int   d_dataLength;            // total data length

    char *d_initialBuffer_p;       // user-supplied buffer (held, not owned)

    int   d_initialBufferSize;     // size of 'd_initialBuffer_p' buffer

    bool  d_inOverflowBufferFlag;  // true if 'pptr' points into the overflow
                                   // buffer

    char *d_overflowBuffer_p;      // overflow buffer (owned)

    int   d_overflowBufferSize;    // size of 'd_overflowBuffer_p' buffer

    bslma::Allocator
         *d_allocator_p;           // memory allocator (held, not owned)

    // NOT IMPLEMENTED
    OverflowMemOutStreamBuf(const OverflowMemOutStreamBuf&);
    OverflowMemOutStreamBuf& operator=(const OverflowMemOutStreamBuf&);

  private:
    // PRIVATE MANIPULATORS
    void grow(int numBytes);
        // Replace the overflow buffer with another buffer larger by at least
        // the specified 'numBytes', by growing geometrically by a factor of
        // two, and preserve the bytes that are in use by the overflow buffer,
        // as determined by the 'd_dataLength'.  Note that 'pptr()' is not
        // updated at this time, and may be pointing to deallocated memory when
        // this returns.

    void privateSync();
        // Set 'd_dataLength' to the amount of valid data that is considered
        // written to this stream by calculating the offset from 'pptr()' to
        // the beginning of the buffer.  If 'pptr()' points into the overflow
        // buffer, the size of the initial buffer is added on.

  protected:
    // PROTECTED VIRTUAL FUNCTIONS

           // *** 27.5.2.4.2 buffer management and positioning ***

    virtual bsl::streambuf *setbuf(char *buffer, bsl::streamsize size);
        // Return 0 unconditionally.

    virtual pos_type seekoff(
        off_type                offset,
        bsl::ios_base::seekdir  fixedPosition,
        bsl::ios_base::openmode mode = bsl::ios_base::in | bsl::ios_base::out);
        // Set the location from which the next I/O operation indicated by the
        // specified 'which' mode will occur to the specified 'offset' position
        // from the location indicated by the specified 'fixedPosition'.
        // Return the new position on success, and 'pos_type(-1)' otherwise.
        // 'offset' may be negative.  Note that this method will fail if
        // 'mode & bsl::ios_base::in' is not 0.

    virtual pos_type seekpos(
        pos_type                position,
        bsl::ios_base::openmode mode = bsl::ios_base::in | bsl::ios_base::out);
        // Set the location from which the next I/O operation indicated by the
        // specified 'which' mode will occur to the specified 'position'.
        // Return 'position' on success, and 'pos_type(-1)' otherwise.  Note
        // that this method will fail if 'mode & bsl::ios_base::in' is not 0.

    virtual int sync();
        // Set 'd_dataLength' to the amount of valid data that is considered
        // written to this stream by calculating the offset from 'pptr()' to
        // the beginning of the buffer.  If the 'pptr()' points into the
        // overflow buffer, the size of the initial buffer is added on.

                        // *** 27.5.2.4.3 get area ***

    virtual bsl::streamsize showmanyc();
        // Return 0 unconditionally.

    virtual bsl::streamsize xsgetn(char_type       *destination,
                                   bsl::streamsize  numChars);
        // Return 'traits_type::eof()' unconditionally.

    virtual int_type underflow();
        // Return 'traits_type::eof()' unconditionally.

                        // *** 27.5.2.4.4 putback ***

    virtual int_type pbackfail(
                              int_type c = bsl::streambuf::traits_type::eof());
        // Return 'traits_type::eof()' unconditionally.

                        // *** 27.5.2.4.5 put area ***

    virtual bsl::streamsize xsputn(const char_type *source,
                                   bsl::streamsize  numChars);
        // Put the specified 'numChars' from the specified 'source' to the
        // stream.  Return the number of characters successfully put.  The
        // behavior is undefined unless '0 <= numChars'.

    virtual int_type overflow(int_type c = bsl::streambuf::traits_type::eof());
        // If c is not 'EOF', adjust the underlying buffer so that the next put
        // position is valid, put the specified 'character' at this position,
        // and increment the put position.  Return 'c' on success, and
        // 'traits_type::not_eof(c)' if 'c' is 'EOF'.

  public:
    // CREATORS
    OverflowMemOutStreamBuf(char             *buffer,
                                  int               length,
                                  bslma::Allocator *basicAllocator = 0);
        // Create an empty stream buffer that uses the specified 'buffer' as an
        // initial output buffer of the specified 'length' (in bytes).
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently-installed default allocator is
        // used.  Note that this stream buffer does not assume ownership of
        // 'buffer'.

    virtual ~OverflowMemOutStreamBuf();
        // Destroy this stream buffer.

    // ACCESSORS
    int dataLength() const;
        // Return the number of bytes written and synced in this object.  Note
        // that if 'pptr()' is currently pointing into the overflow buffer the
        // data length will be greater than the size of the initial buffer.

    int dataLengthInOverflowBuffer() const;
        // Return the length of the data in the overflow buffer, i.e., 0 if
        // there is no overflow buffer, or 'dataLength() - initialBufferSize()'
        // if there is one.

    int dataLengthInInitialBuffer() const;
        // Return the length of data in the initial buffer, i.e.,
        // 'dataLength()' if there is no overflow buffer, or
        // 'initialBufferSize()' if there is one.

    const char *initialBuffer() const;
        // Return a pointer to the non-modifiable buffer specified at
        // construction.

    int initialBufferSize() const;
        // Return the size of the buffer specified at construction.

    const char *overflowBuffer() const;
        // Return a pointer to the non-modifiable overflow buffer if there is
        // one, or 0 otherwise.

    int overflowBufferSize() const;
        // Return the size of the overflow buffer, or 0 if there is no overflow
        // buffer.
};

typedef OverflowMemOutStreamBuf OverflowMemOutStreambuf;
    // DEPRECATED: Use 'OverflowMemOutStreamBuf' instead.

// ============================================================================
//                            INLINE DEFINITIONS
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
int OverflowMemOutStreamBuf::dataLength() const
{
    return d_dataLength;
}

inline
int OverflowMemOutStreamBuf::dataLengthInOverflowBuffer() const
{
    return d_inOverflowBufferFlag ? d_dataLength - d_initialBufferSize : 0;
}

inline
int OverflowMemOutStreamBuf::dataLengthInInitialBuffer() const
{
    return d_inOverflowBufferFlag ? d_initialBufferSize : d_dataLength;
}

inline
const char *OverflowMemOutStreamBuf::initialBuffer() const
{
    return d_initialBuffer_p;
}

inline
int OverflowMemOutStreamBuf::initialBufferSize() const
{
    return d_initialBufferSize;
}

inline
const char *OverflowMemOutStreamBuf::overflowBuffer() const
{
    return d_overflowBuffer_p;
}

inline
int OverflowMemOutStreamBuf::overflowBufferSize() const
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
