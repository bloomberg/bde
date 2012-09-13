// bdesb_memoutstreambuf.h                                            -*-C++-*-
#ifndef INCLUDED_BDESB_MEMOUTSTREAMBUF
#define INCLUDED_BDESB_MEMOUTSTREAMBUF

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide an output 'basic_streambuf' using managed memory.
//
//@CLASSES:
//   bdesb_MemOutStreamBuf: output stream buffer using memory from an allocator
//
//@AUTHOR: Lea Fester (lfester)
//
//@SEE_ALSO: bdesb_fixedmemoutstreambuf, bdesb_fixedmeminstreambuf
//
//@DESCRIPTION: This component implements the output portion of the
// 'bsl::basic_streambuf' protocol using a managed, allocator-supplied memory
// buffer.  Method names necessarily correspond to the protocol-specified
// method names.
//
// This component provides none of the input-related functionality of
// 'basic_streambuf' (see "Streaming Architecture", below), nor does it use
// locales in any way.
//
// Because the underlying buffer is always obtained from the client-specified
// allocator, the 'pubsetbuf' method in this component has no effect.
//
///Streaming Architecture
///----------------------
// Stream buffers are designed to decouple device handling from content
// formatting, providing the requisite device handling and possible buffering
// services, and leaving the formatting to the client stream.  The standard
// C++ IOStreams library further partitions streaming into input streaming and
// output streaming, separating responsibilities for each at both the stream
// layer and the stream buffer layer.  The BDE streaming library for 'bdex',
// including all of 'bdesb', follows this model.
//
///Usage
///-----
// This example demonstrates use of a stream buffer by a stream, in this case
// a stream with simple formatting requirements -- namely, capitalizing all
// lower-case ASCII character data that is output.  To simplify the example, we
// do not include the functions for streaming non-character data, e.g., numeric
// values:
//..
//  // my_capitalizingstream.h
//
//  class my_CapitalizingStream {
//      // This class capitalizes lower-case ASCII characters that are output.
//
//      bdesb_MemOutStreamBuf d_streamBuf;  // buffer to write to
//
//      friend
//      my_CapitalizingStream& operator<<(my_CapitalizingStream&  stream,
//                                        char                    data);
//      friend
//      my_CapitalizingStream& operator<<(my_CapitalizingStream&  stream,
//                                        const char             *data);
//
//    public:
//      // CREATORS
//      my_CapitalizingStream();
//          // Create a capitalizing stream.
//
//      ~my_CapitalizingStream();
//          // Destroy this capitalizing stream.
//
//      // ACCESSORS
//      const bdesb_MemOutStreamBuf& streamBuf() { return d_streamBuf; }
//          // Return the stream buffer used by this capitalizing stream.  Note
//          // that this function is for debugging only.
//  };
//
//  // FREE OPERATORS
//  my_CapitalizingStream& operator<<(my_CapitalizingStream&  stream,
//                                    char                    data);
//  my_CapitalizingStream& operator<<(my_CapitalizingStream&  stream,
//                                    const char             *data);
//      // Write the specified 'data' in capitalized form to the specified
//      // capitalizing 'stream', and return a reference to the modifiable
//      // 'stream'.
//
//  my_CapitalizingStream::my_CapitalizingStream()
//  {
//  }
//
//  my_CapitalizingStream::~my_CapitalizingStream()
//  {
//  }
//..
// As is typical, the streaming operators are made friends of the class.  We
// use the 'transform' algorithm to convert lower-case characters to uppercase:
//..
//  // my_capitalizingstream.cpp
//
//  #include <algorithm>
//
//  // FREE OPERATORS
//  my_CapitalizingStream& operator<<(my_CapitalizingStream& stream, char data)
//  {
//      stream.d_streamBuf.sputc(
//                             bsl::toupper(static_cast<unsigned char>(data)));
//      return stream;
//  }
//
//  my_CapitalizingStream& operator<<(my_CapitalizingStream&  stream,
//                                    const char             *data)
//  {
//      bsl::string tmp(data);
//      transform(tmp.begin(), tmp.end(), tmp.begin(), bsl::toupper);
//      stream.d_streamBuf.sputn(tmp.data(), tmp.length());
//      return stream;
//  }
//..
// Given the above two functions, we can now write 'main' as follows:
//..
//  // my_app.m.cpp
//
//  int main(int argc, char **argv)
//  {
//      my_CapitalizingStream cs;
//      cs << "Hello," << ' ' << "World." << '\0';
//
//      // Verify the results by writing to 'stdout'.
//      bsl::cout << cs.streamBuf().data() << bsl::endl;
//  }
//..
// Running the program above produces the following output on 'stdout':
//..
//  HELLO, WORLD.
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSL_CSTRING
#include <bsl_cstring.h>
#endif

#ifndef INCLUDED_BSL_CSTDLIB
#include <bsl_cstdlib.h>
#endif

#ifndef INCLUDED_BSL_IOS
#include <bsl_ios.h>
#endif

#ifndef INCLUDED_BSL_STREAMBUF
#include <bsl_streambuf.h>  // (char|int|pos|off|traits)_type
#endif

namespace BloombergLP {

                         // ===========================
                         // class bdesb_MemOutStreamBuf
                         // ===========================

class bdesb_MemOutStreamBuf : public bsl::streambuf {
    // This 'class' implements the output functionality of the
    // 'basic_streambuf' protocol, using a user-supplied or default 'bslma'
    // allocator to supply memory.

    // PRIVATE CONSTANTS
    enum {
        BDESB_INITIAL_BUFFER_SIZE = 256,  // default initial buffer size

        BDESB_GROWTH_FACTOR       =   2   // geometric growth factor to use
                                          // when resizing internal buffer

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , INITIAL_BUFFER_SIZE = BDESB_INITIAL_BUFFER_SIZE
      , GROWTH_FACTOR       = BDESB_GROWTH_FACTOR
#endif // BDE_OMIT_INTERNAL_DEPRECATED
    };

    // DATA
    bslma_Allocator *d_allocator_p;  // memory source for buffer memory
                                     // (held, not owned)

    // NOT IMPLEMENTED
    bdesb_MemOutStreamBuf(const bdesb_MemOutStreamBuf&);
    bdesb_MemOutStreamBuf& operator=(const bdesb_MemOutStreamBuf&);

  private:
    // PRIVATE MANIPULATORS
    void grow(int newLength);
        // Grow the size of the internal buffer to be at least large enough to
        // fit the specified 'newLength' characters.  The buffer size is grown
        // by the minimum power of 'BDESB_GROWTH_FACTOR' needed to accommodate
        // the new length, but with a final size not less than
        // 'BDESB_INITIAL_BUFFER_SIZE'.  This method has no effect if
        // 'newLength <= capacity()' holds before the call.

    // PRIVATE ACCESSORS
    int capacity() const;
        // Return the current buffer capacity.

  protected:
    // PROTECTED MANIPULATORS
    virtual int_type overflow(
                  int_type insertionChar = bsl::streambuf::traits_type::eof());
        // Append the optionally specified 'insertionChar' to this stream
        // buffer's character buffer and return 'insertionChar'.  If
        // 'insertionChar' is not specified, 'traits_type::eof()' is appended
        // instead.

    virtual pos_type seekoff(
                           off_type                offset,
                           bsl::ios_base::seekdir  fixedPosition,
                           bsl::ios_base::openmode which = bsl::ios_base::out);
        // Set the location at which the next output will be written to the
        // specified 'offset' from the location indicated by the specified
        // 'fixedPosition', unless the specified 'which' does not include the
        // flag 'bsl::ios_base::out'.  Return the location at which output will
        // be next written if 'which' includes the flag 'bsl::ios_base::out'
        // and 'char_traits<char>::pos_type(char_traits<char>::off_type(-1))'
        // otherwise.  'offset' may be negative.  If 'which' includes the flag
        // 'bsl::ios_base::out' then the behavior is undefined unless
        // '0 <= fixedPosition + offset < length()'.

    virtual pos_type seekpos(
                           pos_type                position,
                           bsl::ios_base::openmode which = bsl::ios_base::out);
        // Set the location at which the next output will be written to the
        // specified 'position', unless the specified 'which' does not include
        // the flag 'bsl::ios_base::out'.  Return 'position' if 'which'
        // includes the flag 'bsl::ios_base::out', and
        // 'char_traits<char>::pos_type(char_traits<char>::off_type(-1))'
        // otherwise.  If 'which' includes the flag 'bsl::ios_base::out' then
        // the behavior is undefined unless '0 <= position < length()'.

    virtual bsl::streamsize xsputn(const char_type *source,
                                   bsl::streamsize  numChars);
        // Copy the specified 'numChars' from the specified 'source' to
        // this stream buffer's character buffer, starting at this stream
        // buffer's current location.  Return the number of characters
        // successfully added.  The behavior is undefined unless
        // '0 <= numChars'.

  public:
    // CREATORS
    explicit
    bdesb_MemOutStreamBuf(bslma_Allocator *basicAllocator = 0);
        // Create an empty stream buffer.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    explicit
    bdesb_MemOutStreamBuf(int              numElements,
                          bslma_Allocator *basicAllocator = 0);
        // Create an empty stream buffer with sufficient initial capacity to
        // accommodate up to the specified 'numElements' characters without
        // subsequent reallocation.  If 'numElements <= 0', an implementation-
        // defined initial capacity is used.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    ~bdesb_MemOutStreamBuf();
        // Destroy this stream buffer.

    // MANIPULATORS
    void reserveCapacity(int numCharacters);
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
    const char *data() const;
        // Return the address of the non-modifiable character buffer managed by
        // this stream buffer.

    bsl::streamsize length() const;
        // Return the number of valid characters in this stream buffer.
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

// PRIVATE ACCESSORS
inline
int bdesb_MemOutStreamBuf::capacity() const
{
    return (int)(epptr() - pbase());
}

// CREATORS
inline
bdesb_MemOutStreamBuf::bdesb_MemOutStreamBuf(bslma_Allocator *basicAllocator)
: d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    setp(0, 0);
}

inline
bdesb_MemOutStreamBuf::bdesb_MemOutStreamBuf(int              numElements,
                                             bslma_Allocator *basicAllocator)
: d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    setp(0, 0);
    reserveCapacity(numElements <= 0
                    ? BDESB_INITIAL_BUFFER_SIZE
                    : numElements);
}

inline
bdesb_MemOutStreamBuf::~bdesb_MemOutStreamBuf()
{
    d_allocator_p->deallocate(pbase());
}

// MANIPULATORS
inline
void bdesb_MemOutStreamBuf::reset()
{
    d_allocator_p->deallocate(pbase());
    setp(0, 0);
}

// ACCESSORS
inline
const char *bdesb_MemOutStreamBuf::data() const
{
    return pbase();
}

inline
bsl::streamsize bdesb_MemOutStreamBuf::length() const
{
    return pptr() - pbase();
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
