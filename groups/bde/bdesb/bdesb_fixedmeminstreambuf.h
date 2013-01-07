// bdesb_fixedmeminstreambuf.h                                        -*-C++-*-
#ifndef INCLUDED_BDESB_FIXEDMEMINSTREAMBUF
#define INCLUDED_BDESB_FIXEDMEMINSTREAMBUF

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide an input 'basic_streambuf' using a client buffer.
//
//@CLASSES:
//   bdesb_FixedMemInStreamBuf: input stream buffer using client memory
//
//@AUTHOR: Lea Fester (lfester)
//
//@SEE_ALSO: bdesb_fixedmemoutstreambuf, bdesb_memoutstreambuf
//
//@DESCRIPTION: This component implements the input portion of the
// 'bsl::basic_streambuf' protocol using a client-supplied memory buffer.
// Method names necessarily correspond to the protocol-specified method names.
// Clients supply the character buffer at stream buffer construction, and can
// later reinitialize the stream buffer with a different character buffer by
// calling the 'pubsetbuf' method.
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
// including all of 'bdesb', follows this model.
//
///Usage Example
///-------------
// This example demonstrates use of a stream buffer by a stream, in this case
// a stream that breaks up space-delimited strings into individual words.  It
// thus gives the clients the ability to read one (English) word at a time,
// using a natural streaming interface.
//
// This example is deliberately simple, and does not check for multiple
// spaces between words, nor other possible error inputs.
//..
// // my_wordstreamer.h
//
// class my_WordStreamer {
//     // This class tokenizes white-space delimited input into distinct
//     // words.
//
//    enum { LINE_SIZE_REQUEST = 512 };
//
//    basic_streambuf<char, char_traits<char> > *d_streamBuf;
//                                              // buffer to read from
//    char                      *d_currentLine; // local cache to store a line
//    int                        d_lineLength;  // length of current line
//    int                        d_cursor;      // character position
//
//    friend my_WordStreamer& operator>>(my_WordStreamer& stream,
//                                       bsl::string&     word);
//    friend my_WordStreamer& operator>>(my_WordStreamer&  stream,
//                                       char             *word);
//
//  public:
//    // CREATORS
//    my_WordStreamer(basic_streambuf<char, char_traits<char> > *streamBuf);
//        // Create a WordStreamer....
//
//    ~my_WordStreamer();
//        // Destroy this object.
// };
//..
// The client passes to the stream on construction a
// 'basic_streambuf<char, char_traits<char> > *', one that in this examples
// wraps an underlying character buffer.  The buffer might, for example, have
// been obtained by a messaging-middleware callback, and be the output of some
// process that is communicating from across the network.
//..
// // my_wordstreamer.cpp
//
// // CREATORS
// my_WordStreamer::my_WordStreamer(
//                       basic_streambuf<char, char_traits<char> > *streamBuf)
//  : d_streamBuf(streamBuf)
//  , d_cursor(0)
//  , d_currentLine(new char[LINE_SIZE_REQUEST])
// {
//     d_lineLength = streamBuf->sgetn(d_currentLine, LINE_SIZE_REQUEST);
// }
//
// my_WordStreamer::~my_WordStreamer()
// {
//    delete [] d_currentLine;
// }
//..
// The following operator breaks off a single word and reads it into an
// 'bsl::string'.
//..
// my_WordStreamer& operator>>(my_WordStreamer& stream, bsl::string& word)
// {
//     if (stream.d_cursor < stream.d_lineLength) {
//         // See if we can find another word
//         for (int i = stream.d_cursor; i < stream.d_lineLength; ++i) {
//             if (' ' == stream.d_currentLine[i]) {
//                 word.assign(&(stream.d_currentLine[stream.d_cursor]),
//                              i - stream.d_cursor);
//                 stream.d_cursor = i + 1;
//                 return stream;
//             }
//         }
//     }
//..
// Note that if a 'bdesb_FixedMemInStreamBuf' is the underlying character
// source for the 'WordStreamer', once it has read past the number of
// characters contained by the stream buffer, no replenishment is possible.
// However, if the initial 'LINE_SIZE_REQUEST' is smaller than the number
// of characters stored by the buffer, repeated requests can succeed (until
// the character store is exhausted); furthermore, when using other stream
// buffers, replenishment may be possible.
//..
//     // No word in existing buffer.  See if we can replenish/add.
//     word.assign(&(stream.d_currentLine[stream.d_cursor]),
//                stream.d_lineLength - stream.d_cursor); // copy over existing
//     while(1) {
//         stream.d_lineLength = stream.d_streamBuf->sgetn(
//                             stream.d_currentLine, stream.LINE_SIZE_REQUEST);
//         stream.d_cursor = 0;
//         if (0 == stream.d_lineLength) {
//             word.clear();
//             return stream;
//         }
//
//         for(int i = 0; i < stream.d_lineLength; ++i) {
//
//             // Found the word.  Copy & exit.
//             if (' ' == stream.d_currentLine[i]) {
//                 word.append(stream.d_currentLine, i - 1);
//                 stream.d_cursor = i + 1;
//                 return stream;
//             }
//             // Else copy and, after forcing an underflow, continue.
//             word.append(stream.d_currentLine, stream.d_lineLength);
//         }
//     }
// }
//..
// The following 'main' is a simple exercise demonstrating 'my_WordStreamer'
// use.
//..
// // my_app.m.cpp
//
// int main(int argc, char **argv) {
//
//     char *text = "If concrete objects reside in such a library ";
//     bdesb_FixedMemInStreamBuf sb(text, strlen(text));
//     my_WordStreamer ws(&sb);
//
//     do {
//         ws >> nextWord;
//         cout << "Next word is: " << nextWord << endl;
//     } while (nextWord[0] != 0);
// }
//..
// The resulting output is:
//..
// Next word is: If
// Next word is: concrete
// Next word is: objects
// Next word is: reside
// Next word is: in
// Next word is: such
// Next word is: a
// Next word is: library
// Next word is:
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSL_ALGORITHM
#include <bsl_algorithm.h>  // for 'bsl::min'
#endif

#ifndef INCLUDED_BSL_IOS
#include <bsl_ios.h>       // for 'bsl::streamsize'
#endif

#ifndef INCLUDED_BSL_STREAMBUF
#include <bsl_streambuf.h>
#endif

#ifndef INCLUDED_BSL_CSTDLIB
#include <bsl_cstdlib.h>
#endif

#ifndef INCLUDED_BSL_CSTRING
#include <bsl_cstring.h>
#endif

#if defined(BSLS_PLATFORM_CMP_MSVC) && defined(min)
    // Note: on Windows -> WinDef.h:#define min(a,b) ...
#undef min
#endif

namespace BloombergLP {

                       // ===============================
                       // class bdesb_FixedMemInStreamBuf
                       // ===============================

class bdesb_FixedMemInStreamBuf : public bsl::streambuf {
    // This class implements the input functionality of the 'basic_streambuf'
    // protocol, using client-supplied, 'char *' memory.

    // DATA
    char *d_buffer_p;  // buffer (held, not owned)
    int   d_length;    // length of buffer

    // NOT IMPLEMENTED
    bdesb_FixedMemInStreamBuf(const bdesb_FixedMemInStreamBuf&);
    bdesb_FixedMemInStreamBuf& operator=(const bdesb_FixedMemInStreamBuf&);

    // Disable the "alternative interface" from base class.
    // NOT IMPLEMENTED
    char_type *_M_eback() const;
    char_type *_M_gptr()  const;
    char_type *_M_egptr() const;
    void _M_gbump(int);
    void _M_setg(char_type *, char_type *, char_type *);

    // Disable the output-related functionality from base class.
    // NOT IMPLEMENTED
    int pubsync();
    int_type sputc(char_type);
    bsl::streamsize sputn(const char_type *, bsl::streamsize);
    bsl::streamsize _M_sputnc(char_type, bsl::streamsize);
    bsl::locale pubimbue(const bsl::locale&);
    bsl::locale getloc() const;

  protected:
    // PROTECTED MANIPULATORS
    virtual int_type pbackfail(int_type character);
        // Ensure that this method is not invoked as a consequence of a
        // 'sputbackc' call.  Return without doing anything on success, and
        // assert on failure.

    virtual pos_type seekoff(
                            off_type                offset,
                            bsl::ios_base::seekdir  fixedPosition,
                            bsl::ios_base::openmode which = bsl::ios_base::in);
        // Set the location from which the next input will be read to the
        // specified 'offset' from the location indicated by the specified
        // 'fixedPosition', unless the specified 'which' does not include the
        // flag 'bsl::ios_base::in'.  Return the location from which input will
        // be next read if 'which' includes the flag 'bsl::ios_base::in', and
        // 'char_traits<char>::pos_type(char_traits<char>::off_type(-1))'
        // otherwise.  'offset' may be negative.  If 'which' includes the flag
        // 'bsl::ios_base::in' then the behavior is undefined unless
        // 0 <= 'fixedPosition' + 'offset' < length().

    virtual pos_type seekpos(
                            pos_type                offset,
                            bsl::ios_base::openmode which = bsl::ios_base::in);
        // Set the location from which the next input will be read to the
        // specified 'position', unless the specified 'which' does not include
        // the flag 'bsl::ios_base::in'.  Return 'position' if 'which' includes
        // the flag 'bsl::ios_base::in', and
        // 'char_traits<char>::pos_type(char_traits<char>::off_type(-1))'
        // otherwise.  If 'which' includes the flag 'bsl::ios_base::in' then
        // the behavior is undefined unless 0 <= 'position' < length().

    bdesb_FixedMemInStreamBuf *setbuf(const char      *buffer,
                                      bsl::streamsize  length);
    bdesb_FixedMemInStreamBuf *setbuf(char            *buffer,
                                      bsl::streamsize  length);
        // Reinitialize this stream buffer to use the specified character
        // 'buffer' having the specified 'length'.  Return the address of this
        // modifiable stream buffer.  Upon reinitialization for use of the new
        // buffer, neither the content nor the next input location is
        // preserved.  Note that 'buffer' is held but not owned.

    virtual bsl::streamsize showmanyc();
        // Return the number of characters currently available for reading
        // from this stream buffer, or -1 if there are none.

    virtual bsl::streamsize xsgetn(char_type       *destination,
                                   bsl::streamsize  numChars);
        // Read the specified 'numChars' to the specified 'destination'.
        // Return the number of characters successfully read.  The behavior is
        // undefined unless 0 <= 'numChars'.

  public:
    // CREATORS
    bdesb_FixedMemInStreamBuf(const char *buffer, bsl::streamsize length);
        // Create a 'bdesb_FixedMemInStreamBuf' using the specified 'buffer'
        // of the specified 'length'.  The behavior is undefined unless
        // '0 < length'.

    ~bdesb_FixedMemInStreamBuf();
        // Destroy this stream buffer.

    // MANIPULATORS
    bdesb_FixedMemInStreamBuf *pubsetbuf(char            *buffer,
                                         bsl::streamsize  length);
    virtual bdesb_FixedMemInStreamBuf *pubsetbuf(const char      *buffer,
                                                 bsl::streamsize  length);
        // Reinitialize this stream buffer to use the specified character
        // 'buffer' having the specified 'length'.  Return the address of this
        // modifiable stream buffer.  Upon reinitialization for use of the new
        // buffer, neither the content nor the next input location is
        // preserved.  Note that 'buffer' is held but not owned.

    // ACCESSORS
    const char *data() const;
        // Return the address of the non-modifiable character buffer held by
        // this stream buffer.

    bsl::streamsize length() const;
        // Return the number of characters available in this stream buffer.
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                       // -------------------------------
                       // class bdesb_FixedMemInStreamBuf
                       // -------------------------------

// PROTECTED MANIPULATORS
inline
bdesb_FixedMemInStreamBuf *bdesb_FixedMemInStreamBuf::setbuf(
                                                       char            *buffer,
                                                       bsl::streamsize  length)

{
    BSLS_ASSERT_SAFE(buffer || 0 == length);
    BSLS_ASSERT_SAFE(0 <= length);

    // Reset pointers and length.
    d_buffer_p = buffer;
    d_length   = (int)length;
    setg(d_buffer_p, d_buffer_p, d_buffer_p + d_length);
    return this;
}

inline
bdesb_FixedMemInStreamBuf *bdesb_FixedMemInStreamBuf::setbuf(
                                                       const char      *buffer,
                                                       bsl::streamsize  length)
{
    BSLS_ASSERT_SAFE(buffer || 0 == length);
    BSLS_ASSERT_SAFE(0 <= length);

    return setbuf(const_cast<char *>(buffer), length);
}

inline
bsl::streamsize bdesb_FixedMemInStreamBuf::showmanyc()
{
    int numChars = (int)(egptr() - gptr());
    if (0 == numChars) {
        return -1;
    }
    return numChars;
}

inline
bsl::streamsize bdesb_FixedMemInStreamBuf::xsgetn(char_type       *destination,
                                                  bsl::streamsize  numChars)
{
    BSLS_ASSERT_SAFE(destination);
    BSLS_ASSERT_SAFE(0 <= numChars);

    bsl::streamsize remainingChars = egptr() - gptr();
    int canCopy = (int)bsl::min(remainingChars, numChars);
    bsl::memcpy(destination, gptr(), canCopy);
    gbump(canCopy);
    return canCopy;
}

// CREATORS
inline
bdesb_FixedMemInStreamBuf::bdesb_FixedMemInStreamBuf(const char      *buffer,
                                                     bsl::streamsize  length)
: d_buffer_p(const_cast<char *>(buffer))
, d_length((int)length)
{
    BSLS_ASSERT_SAFE(buffer || 0 == length);
    BSLS_ASSERT_SAFE(0 <= length);

    setg(d_buffer_p, d_buffer_p, d_buffer_p + length);
}

inline
bdesb_FixedMemInStreamBuf::~bdesb_FixedMemInStreamBuf()
{
}

// MANIPULATORS
inline
bdesb_FixedMemInStreamBuf *bdesb_FixedMemInStreamBuf::pubsetbuf(
                                                       const char     *buffer,
                                                       bsl::streamsize length)
{
    BSLS_ASSERT_SAFE(buffer || 0 == length);
    BSLS_ASSERT_SAFE(0 <= length);

    return setbuf(buffer, length);
}

inline
bdesb_FixedMemInStreamBuf *bdesb_FixedMemInStreamBuf::pubsetbuf(
                                                      char           *buffer,
                                                      bsl::streamsize length)
{
    BSLS_ASSERT_SAFE(buffer || 0 == length);
    BSLS_ASSERT_SAFE(0 <= length);

    return static_cast<bdesb_FixedMemInStreamBuf *>(
                             this->bsl::streambuf::pubsetbuf(buffer, length));
}

// ACCESSORS
inline
const char *bdesb_FixedMemInStreamBuf::data() const
{
    return d_buffer_p;
}

inline
bsl::streamsize bdesb_FixedMemInStreamBuf::length() const
{
    return egptr() - gptr();
}

}  // end namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
