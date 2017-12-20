// bdlsb_memoutstreambuf.h                                            -*-C++-*-
#ifndef INCLUDED_BDLSB_MEMOUTSTREAMBUF
#define INCLUDED_BDLSB_MEMOUTSTREAMBUF

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an output 'basic_streambuf' using managed memory.
//
//@CLASSES:
//   bdlsb::MemOutStreamBuf: output stream buffer using memory allocator
//
//@SEE_ALSO: bdlsb_fixedmemoutstreambuf, bdlsb_fixedmeminstreambuf
//
//@DESCRIPTION: This component provides a mechanism, 'bdlsb::MemOutStreamBuf',
// that implements the output portion of the 'bsl::basic_streambuf' protocol
// using a managed, allocator-supplied memory buffer.  Method names necessarily
// correspond to those specified by the protocol.
//
// This component provides none of the input-related functionality of
// 'basic_streambuf' (see "Streaming Architecture", below), nor does it use
// locales in any way.
//
// Because the underlying buffer is always obtained from the client-specified
// allocator, the 'pubsetbuf' method in this component has no effect.
//
// Note that this component has an unspecified minimum allocation size, and
// therefore users trying to limit themselves to a fixed buffer should use
// bdlsb_fixedmemoutstreambuf.
//
///Streaming Architecture
///----------------------
// Stream buffers are designed to decouple device handling from content
// formatting, providing the requisite device handling and possible buffering
// services, and leaving the formatting to the client stream.  The standard C++
// IOStreams library further partitions streaming into input streaming and
// output streaming, separating responsibilities for each at both the stream
// layer and the stream buffer layer.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
/// Example 1: Basic Use of 'bdlsb::MemOutStreamBuf'
///- - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates using a 'bdlsb::MemOutStreamBuf' in order to test
// a user defined stream type, 'CapitalizingStream'.  In this example, we'll
// define a simple example stream type 'CapitalizingStream' that capitalizing
// lower-case ASCII data written to the stream.  In order to test this
// 'CapitalizingStream' type, we'll create an instance, and supply it a
// 'bdlsb::MemOutStreamBuf' object as its stream buffer; after we write some
// character data to the 'CapitalizingStream' we'll inspect the buffer of the
// 'bdlsb::MemOutStreamBuf' and verify its contents match our expected output.
// Note that to simplify the example, we do not include the functions for
// streaming non-character data, e.g., numeric values.
//
// First, we define our example stream class, 'CapitalizingStream' (which we
// will later test using 'bdlsb::MemOutStreamBuf):
//..
//  class CapitalizingStream {
//      // This class capitalizes lower-case ASCII characters that are output.
//
//      // DATA
//      bsl::streambuf  *d_streamBuffer_p;   // pointer to a stream buffer
//
//      // FRIENDS
//      friend CapitalizingStream& operator<<(CapitalizingStream&  stream,
//                                            const char          *data);
//    public:
//      // CREATORS
//      explicit CapitalizingStream(bsl::streambuf *streamBuffer);
//          // Create a capitalizing stream using the specified 'streamBuffer'
//          // as underlying stream buffer to the stream.
//  };
//
//  // FREE OPERATORS
//  CapitalizingStream& operator<<(CapitalizingStream&  stream,
//                                 const char          *data);
//      // Write the specified 'data' in capitalized form to the specified
//      // 'stream'.
//
//  CapitalizingStream::CapitalizingStream(bsl::streambuf *streamBuffer)
//  : d_streamBuffer_p(streamBuffer)
//  {
//  }
//..
// As is typical, the streaming operators are made friends of the class.
//
// Note that we cannot directly use 'bsl::toupper' to capitalize each
// individual character, because 'bsl::toupper' operates on 'int' instead of
// 'char'.  Instead, we call a function 'ucharToUpper' that works in terms of
// 'unsigned char'.  some care must be made to avoid undefined and
// implementation-specific behavior during the conversions to and from 'int'.
// Therefore we wrap 'bsl::toupper' in an interface that works in terms of
// 'unsigned char':
//..
//  static unsigned char ucharToUpper(unsigned char input)
//      // Return the upper-case equivalent to the specified 'input' character.
//  {
//      return static_cast<unsigned char>(bsl::toupper(input));
//  }
//..
// Finally, we use the 'transform' algorithm to convert lower-case characters
// to upper-case.
//..
//  // FREE OPERATORS
//  CapitalizingStream& operator<<(CapitalizingStream&  stream,
//                                 const char          *data)
//  {
//      bsl::string tmp(data);
//      bsl::transform(tmp.begin(),
//                     tmp.end(),
//                     tmp.begin(),
//                     ucharToUpper);
//      stream.d_streamBuffer_p->sputn(tmp.data(), tmp.length());
//      return stream;
//  }
//..
// Now, we create an instance of 'bdlsb::MemOutStreamBuf' that will serve as
// underlying stream buffer for our 'CapitalingStream':
//..
//  bdlsb::MemOutStreamBuf streamBuffer;
//..
// Now, we test our 'CapitalingStream' by supplying the created instance of
// 'bdlsb::MemOutStreamBuf' and using it to inspect the output of the stream:
//..
//  CapitalizingStream  testStream(&streamBuffer);
//  testStream << "Hello world.";
//..
// Finally, we verify that the streamed data has been capitalized and placed
// into dynamically allocated buffer:
//..
//  assert(12 == streamBuffer.length());
//  assert(0  == bsl::strncmp("HELLO WORLD.",
//                            streamBuffer.data(),
//                            streamBuffer.length()));
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

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_CSTDDEF
#include <bsl_cstddef.h>
#endif

#ifndef INCLUDED_BSL_CSTDLIB
#include <bsl_cstdlib.h>
#endif

#ifndef INCLUDED_BSL_CSTRING
#include <bsl_cstring.h>
#endif

#ifndef INCLUDED_BSL_IOS
#include <bsl_ios.h>
#endif

#ifndef INCLUDED_BSL_STREAMBUF
#include <bsl_streambuf.h>  // (char|int|pos|off|traits)_type
#endif

namespace BloombergLP {
namespace bdlsb {

                         // =====================
                         // class MemOutStreamBuf
                         // =====================

class MemOutStreamBuf : public bsl::streambuf {
    // This 'class' implements the output functionality of the
    // 'basic_streambuf' protocol, using a user-supplied or default 'bslma'
    // allocator to supply memory.

    // PRIVATE CONSTANTS
    enum {
        k_INITIAL_BUFFER_SIZE = 256,  // default initial buffer size

        k_GROWTH_FACTOR       =   2   // geometric growth factor to use when
                                      // resizing internal buffer

    };

    // DATA
    bslma::Allocator *d_allocator_p;  // memory source for buffer memory
                                      // (held, not owned)

  private:
    // NOT IMPLEMENTED
    MemOutStreamBuf(const MemOutStreamBuf&); // = delete;
    MemOutStreamBuf& operator=(const MemOutStreamBuf&); // = delete;

  private:
    // PRIVATE MANIPULATORS
    void grow(bsl::size_t newLength);
        // Grow the size of the internal buffer to be at least large enough to
        // fit the specified 'newLength' characters.  The buffer size is grown
        // by the minimum power of 'k_GROWTH_FACTOR' needed to accommodate the
        // new length, but with a final size not less than
        // 'k_INITIAL_BUFFER_SIZE'.  This method has no effect if 'newLength <=
        // capacity()' holds before the call.

  protected:
    // PROTECTED MANIPULATORS
    virtual int_type overflow(
                  int_type insertionChar = bsl::streambuf::traits_type::eof());
        // Append the optionally specified 'insertionChar' to this stream
        // buffer's character buffer and return 'insertionChar'.  If
        // 'insertionChar' is not specified, 'traits_type::eof()' is appended
        // instead.

    virtual pos_type seekoff(off_type                offset,
                             bsl::ios_base::seekdir  way,
                             bsl::ios_base::openmode which = bsl::ios_base::in
                                                         | bsl::ios_base::out);
        // Set the position indicator to the relative specified 'offset' from
        // the base position indicated by the specified 'way' and return the
        // resulting absolute position on success or pos_type(-1) on failure.
        // Optionally specify 'which' area of the stream buffer.  The seek
        // operation will fail if 'which' does not include the flag
        // 'bsl::ios_base::out' or if the resulting absolute position is less
        // than zero or greater then 'length()'.

    virtual pos_type seekpos(pos_type                position,
                             bsl::ios_base::openmode which = bsl::ios_base::in
                                                         | bsl::ios_base::out);
        // Set the position indicator to the specified 'position' and return
        // the resulting absolute position on success or pos_type(-1) on
        // failure.  Optionally specify 'which' area of the stream buffer.  The
        // 'seekpos' operation will fail if 'which' does not include the flag
        // 'bsl::ios_base::out' or if 'position' is less then zero or greater
        // than 'length()'.

    virtual bsl::streamsize xsputn(const char_type *source,
                                   bsl::streamsize  numChars);
        // Write the specified 'numChars' characters from the specified
        // 'source' to the stream buffer.  Return the number of characters
        // successfully written.  The behavior is undefined unless '(source &&
        // 0 < numChars) || 0 == numChars'.

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(MemOutStreamBuf,
                                   bslma::UsesBslmaAllocator);

    // CREATORS
    explicit
    MemOutStreamBuf(bslma::Allocator *basicAllocator = 0);
        // Create an empty stream buffer.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    explicit
    MemOutStreamBuf(bsl::size_t       numElements,
                    bslma::Allocator *basicAllocator = 0);
        // Create an empty stream buffer with sufficient initial capacity to
        // accommodate up to the specified 'numElements' characters without
        // subsequent reallocation.  If 'numElements == 0', an implementation-
        // defined initial capacity is used.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    ~MemOutStreamBuf();
        // Destroy this stream buffer.

    // MANIPULATORS
    void reserveCapacity(bsl::size_t numCharacters);
        // Reserve sufficient internal capacity to store at least the specified
        // 'numCharacters' characters without reallocation.  Note that if the
        // storage size specified is less than the number of characters already
        // in the buffer, this method has no effect.

    void reset();
        // Destroy the contents of this stream buffer, return all allocated
        // memory to the allocator, and reset the buffer to the default
        // constructed state.  Note that 'length() == 0' holds following a call
        // to this method.

    // ACCESSORS
    bsl::size_t capacity() const;
        // Return the current capacity of the buffer managed by this stream
        // buffer.

    const char *data() const;
        // Return the address of the non-modifiable character buffer managed by
        // this stream buffer.

    bsl::size_t length() const;
        // Return the number of valid characters in this stream buffer.
};

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                         // ---------------------
                         // class MemOutStreamBuf
                         // ---------------------

// CREATORS
inline
MemOutStreamBuf::MemOutStreamBuf(bslma::Allocator *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    setp(0, 0);
}

inline
MemOutStreamBuf::MemOutStreamBuf(bsl::size_t       numElements,
                                 bslma::Allocator *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    setp(0, 0);
    reserveCapacity(numElements == 0
                    ? static_cast<bsl::streamsize>(k_INITIAL_BUFFER_SIZE)
                    : numElements);
}

inline
MemOutStreamBuf::~MemOutStreamBuf()
{
    d_allocator_p->deallocate(pbase());
}

// MANIPULATORS
inline
void MemOutStreamBuf::reset()
{
    d_allocator_p->deallocate(pbase());
    setp(0, 0);
}

// ACCESSORS
inline
bsl::size_t MemOutStreamBuf::capacity() const
{
    return epptr() - pbase();
}

inline
const char *MemOutStreamBuf::data() const
{
    return pbase();
}

inline
bsl::size_t MemOutStreamBuf::length() const
{
    return pptr() - pbase();
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
