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
// bdlsb::OverflowMemOutput: overflowable output 'streambuf' using client
// memory
//
//@AUTHOR: Guillaume Morin (gmorin1), Robert Day (rday7)
//
//@SEE_ALSO: bdlsb_fixedmemoutput, bdlsb_overflowmemoutstreambuf
//
//@DESCRIPTION: This component implements the output portion of the
// 'bsl::basic_streambuf' protocol using a user-supplied memory buffer and a
// dynamically allocated overflow buffer which is used when the client-supplied
// buffer runs out.  As with 'bdlsb_overflowmemoutstreambuf', users supply the
// character buffer at construction.  Unlike 'bdlsb_overflowmemoutstreambuf',
// they can no longer reinitialize the stream buffer with a different character
// buffer by calling the 'pubsetbuf' method; instead, if that buffer runs out,
// the 'bdlsb::OverflowMemOutput' will allocate another buffer (see "Overflow
// Buffer" below).  The only difference between this component and
// 'bdlsb_overflowmemoutstreambuf' is that the class 'bdlsb::OverflowMemOutput'
// does *not* derive from 'bsl::streambuf' and does not support locales.
// Nevertheless, method names correspond to the protocol-specified method
// names; refer/to the C++ Standard, Sect.  27.5.2, for a full specification of
// the 'bsl::basic_streambuf' interface.
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
// layer and the stream buffer layer.  The BDE streaming library for 'bdex',
// including all of 'bdesb', follows this model.
//
///Usage
///-----
// This example demonstrates use of a stream buffer by a stream, in this case a
// stream with simple formatting requirements -- namely, capitalizing all
// character data that passes through its management.  (To simplify the
// example, we do not include the functions for streaming non-character data.)
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
//      char                     *d_buffer;      // initial buffer (owned)
//      bdlsb::OverflowMemOutput *d_streamBuf;   // stream buffer (owned)
//      bslma::Allocator         *d_allocator_p; // allocator (held, not owned)
//
//      // FRIENDS
//      friend my_CapitalizingStream& operator<<(my_CapitalizingStream& stream,
//                                               const bsl::string&     data);
//      friend my_CapitalizingStream& operator<<(my_CapitalizingStream& stream,
//                                               const char            *data);
//      friend my_CapitalizingStream& operator<<(my_CapitalizingStream& stream,
//                                               char                   data);
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
//      const bdlsb::OverflowMemOutput *streamBuf() {
//          return d_streamBuf;
//      }
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
//      // specified 'stream'....
//..
// As is typical, the streaming operators are made friends of the class.  We
// use the 'transform' algorithm to convert all string characters to upper-
// case.
//..
//  // my_capitalizingstream.cpp
//  #include <bsl_algorithm.h>
//
//  my_CapitalizingStream::my_CapitalizingStream(
//                                            bslma::Allocator *basicAllocator)
//  : d_allocator_p(bslma::Default::allocator(basicAllocator))
//  {
//      d_buffer = reinterpret_cast<char*>(
//                              d_allocator_p->allocate(k_STREAMBUF_CAPACITY));
//
//      d_streamBuf = new(*d_allocator_p) bdlsb::OverflowMemOutput(
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
//  bslma::TestAllocator allocator;
//
//  {
//      my_CapitalizingStream cs(&allocator);
//      cs << "Hello" << ' ' << "world." << '\0';
//
//      assert(10 == cs.streamBuf()->dataLengthInInitialBuffer());
//      assert(0 == strncmp("HELLO WORL", cs.streamBuf()->initialBuffer(),
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

                         // =======================
                         // class OverflowMemOutput
                         // =======================

class OverflowMemOutput {
    // This class, like 'OverflowMemOutStreamBuf', implements the output
    // functionality of the 'bsl::basic_streambuf' interface, using a
    // client-supplied buffer and client-supplied allocator if additional
    // memory is needed.  It has an interface similar to
    // 'OverflowMemOutStreamBuf' but does *not* inherit from 'bsl::streambuf'.
    // Thus, it is suitable for use as template parameter to
    // 'bslx::GenericOutStream' (but not to 'bslx::StreamBufOutStream').  Note
    // that this class is not designed to be derived from.

    // PRIVATE TYPES
    typedef bsl::ios_base ios_base;

    // DATA
    // The order of the data members is determined by our usage pattern for
    // cache efficiency.  Do not reorder them.

    int   d_dataLength;            // total data length

    char *d_put_p;                 // current put pointer

    char *d_initialBuffer_p;       // user-supplied buffer (held, not owned)

    int   d_initialBufferSize;     // size of 'd_initialBuffer_p' buffer

    bool  d_inOverflowBufferFlag;  // true if 'pptr' points into the overflow
                                   // buffer

    char *d_overflowBuffer_p;      // overflow buffer (owned)

    int   d_overflowBufferSize;    // size of 'd_overflowBuffer_p' buffer

    bslma::Allocator
         *d_allocator_p;           // memory allocator (held, not owned)

    // NOT IMPLEMENTED
    OverflowMemOutput(const OverflowMemOutput&);
    OverflowMemOutput& operator=(const OverflowMemOutput&);

  private:
    // PRIVATE MANIPULATORS
    void grow(int numBytes);
        // Replace the overflow buffer with another buffer larger by at least
        // the specified 'numBytes', by growing geometrically by a factor of
        // two, and preserve the bytes that are in use by the overflow buffer,
        // as determined by the 'd_dataLength'.  Note that 'd_put_p' is not
        // updated at this time, and may be pointing to deallocated memory when
        // this returns.

  public:
    // TYPES
    typedef bsl::streambuf::traits_type traits_type;

    // CREATORS
    OverflowMemOutput(char             *buffer,
                            int               length,
                            bslma::Allocator *basicAllocator = 0);
        // Create an empty stream buffer that uses the specified 'buffer' as an
        // initial output buffer of the specified 'length' (in bytes).
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  Note that this stream buffer does not assume ownership of
        // 'buffer'.

    ~OverflowMemOutput();
        // Destroy this stream buffer.

    // MANIPULATORS
                             // *** 27.5.2.2.1 locales ***

    bsl::locale pubimbue(const bsl::locale& loc);
        // Return loc unconditionally.

                             // *** 27.5.2.2.2 buffer and positioning ***

    bsl::streambuf *pubsetbuf(char *buffer, bsl::streamsize size);
        // Return '0' unconditionally.

    bsl::streampos pubseekoff(
                      bsl::streamoff     off,
                      ios_base::seekdir  way,
                      ios_base::openmode which = ios_base::in | ios_base::out);
        // Set the location from which the next I/O operation indicated by the
        // specified 'which' mode will occur to the specified 'offset' position
        // from the location indicated by the specified 'fixedPosition'.
        // Return the new position on success, and 'bsl::streampos(-1)'
        // otherwise.  Note that 'offset' may be negative.  Also note that this
        // method will fail if 'mode & bsl::ios_base::in' is not 0.

    bsl::streampos pubseekpos(
             bsl::streampos     position,
             ios_base::openmode mode = bsl::ios_base::in | bsl::ios_base::out);
        // Set the location from which the next I/O operation indicated by the
        // specified 'mode' will occur to the specified 'position'.  Return
        // 'position' on success, and 'bsl::streampos(-1)' otherwise.  Note
        // that this method will fail if 'mode & bsl::ios_base::in' is not 0.

    int pubsync();
        // Synchronize this stream buffer.

                             // *** 27.5.2.2.4 get area ***

    bsl::streamsize in_avail();
        // Return '0' unconditionally.

    int snextc();
        // Return 'bsl::streambuf::traits_type::eof()' unconditionally.

    int sbump();
        // Return 'bsl::streambuf::traits_type::eof()' unconditionally.

    int sgetc();
        // Return 'bsl::streambuf::traits_type::eof()' unconditionally.

    bsl::streamsize sgetn(char *s, bsl::streamsize n);
        // Return 'bsl::streambuf::traits_type::eof()' unconditionally.

    int sputbackc(char c);
        // Return 'bsl::streambuf::traits_type::eof()' unconditionally.

    int sungetc();
        // Return 'bsl::streambuf::traits_type::eof()' unconditionally.

                             // *** 27.5.2.2.5 put area ***

    int sputc(char character);
        // Store the specified 'character' at the current put position and
        // increase the put pointer of this stream buffer.

    bsl::streamsize sputn(const char *s, bsl::streamsize n);
        // Put the specified 'numChars' from the specified 'source' to the
        // stream.  Return the number of characters successfully put.  The
        // behavior is undefined unless '0 <= numChars'.

    // ACCESSORS
    int dataLength() const;
        // Return the number of bytes that was written to this object.

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
        // Return the size of the initial buffer.

    const char *overflowBuffer() const;
        // Return a pointer to the non-modifiable overflow buffer if there is
        // one, or 0 otherwise.

    int overflowBufferSize() const;
        // Return the size of the overflow buffer, or 0 if there is no overflow
        // buffer.

                             // *** 27.5.2.2.1 locales ***

    bsl::locale getloc() const;
        // return 'bsl::locale()' unconditionally.
};

// ============================================================================
//                             INLINE DEFINITIONS
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
bsl::streambuf *OverflowMemOutput::pubsetbuf(char *, bsl::streamsize)
{
    return 0;
}

inline
bsl::streampos OverflowMemOutput::pubseekpos(bsl::streampos          position,
                                             bsl::ios_base::openmode which)
{
    return pubseekoff(bsl::streamoff(position), bsl::ios_base::beg, which);
}

inline
int OverflowMemOutput::pubsync()
{
    return 0;
}

inline
bsl::streamsize OverflowMemOutput::in_avail()
{
    return 0;
}

inline
int OverflowMemOutput::snextc()
{
    return bsl::streambuf::traits_type::eof();
}

inline
int OverflowMemOutput::sbump()
{
    return bsl::streambuf::traits_type::eof();
}

inline
int OverflowMemOutput::sgetc()
{
    return bsl::streambuf::traits_type::eof();
}

inline
bsl::streamsize OverflowMemOutput::sgetn(char *, bsl::streamsize)
{
    return bsl::streambuf::traits_type::eof();
}

inline
int OverflowMemOutput::sputbackc(char)
{
    return bsl::streambuf::traits_type::eof();
}

inline
int OverflowMemOutput::sungetc()
{
    return bsl::streambuf::traits_type::eof();
}

// ACCESSORS
inline
int OverflowMemOutput::dataLength() const
{
    return d_dataLength;
}

inline
int OverflowMemOutput::dataLengthInOverflowBuffer() const
{
    return d_inOverflowBufferFlag ? d_dataLength - d_initialBufferSize : 0;
}

inline
int OverflowMemOutput::dataLengthInInitialBuffer() const
{
    return d_inOverflowBufferFlag ? d_initialBufferSize : d_dataLength;
}

inline
const char *OverflowMemOutput::overflowBuffer() const
{
    return d_overflowBuffer_p;
}

inline
int OverflowMemOutput::overflowBufferSize() const
{
    return d_overflowBufferSize;
}

inline
const char *OverflowMemOutput::initialBuffer() const
{
    return d_initialBuffer_p;
}

inline
int OverflowMemOutput::initialBufferSize() const
{
    return d_initialBufferSize;
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
