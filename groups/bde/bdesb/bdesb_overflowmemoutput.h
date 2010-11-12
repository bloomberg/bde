// bdesb_overflowmemoutput.h -*-C++-*-
#ifndef INCLUDED_BDESB_OVERFLOWMEMOUTPUT
#define INCLUDED_BDESB_OVERFLOWMEMOUTPUT

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide an overflowable output 'streambuf' using a client buffer.
//
//@CLASSES:
// bdesb_OverflowMemOutput: overflowable output 'streambuf' using client memory
//
//@AUTHOR: Guillaume Morin (gmorin1), Robert Day (rday7)
//
//@SEE_ALSO: bdesb_fixedmemoutput, bdesb_overflowmemoutstreambuf
//
//@DESCRIPTION: This component implements the output portion of the
// 'bsl::basic_streambuf' protocol using a user-supplied memory buffer and a
// dynamically allocated overflow buffer which is used when the client-supplied
// buffer runs out.  As with 'bdesb_overflowmemoutstreambuf', users supply the
// character buffer at construction.  Unlike 'bdesb_overflowmemoutstreambuf',
// they can no longer reinitialize the stream buffer with a different character
// buffer by calling the 'pubsetbuf' method; instead, if that buffer runs out,
// the 'bdesb_OverflowMemOutput' will allocate another buffer (see "Overflow
// Buffer" below).  The only difference between this component and
// 'bdesb_overflowmemoutstreambuf' is that the class 'bdesb_OverflowMemOutput'
// does *not* derive from 'bsl::streambuf' and does not support locales.
// Nevertheless, method names correspond to the protocol-specified method
// names; refer/to the C++ Standard, Sect. 27.5.2, for a full specification of
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
// services, and leaving the formatting to the client stream.  The standard
// C++ IOStreams library further partitions streaming into input streaming and
// output streaming, separating responsibilities for each at both the stream
// layer and the stream buffer layer.  The BDE streaming library for 'bdex',
// including all of 'bdesb', follows this model.
//
///Usage
///-----
// See the 'bdesb_overflowmemoutstreambuf' component for an identical usage
// example, where every occurrence of 'bdesb_OverflowMemOutStreamBuf' can be
// substituted for 'bdesb_OverflowMemOutput'.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
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

                       // =============================
                       // class bdesb_OverflowMemOutput
                       // =============================

class bdesb_OverflowMemOutput {
    // This class, like 'bdesb_OverflowMemOutStreamBuf', implements the output
    // functionality of the 'bsl::basic_streambuf' interface, using a
    // client-supplied buffer and client-supplied allocator if additional
    // memory is needed.  It has an interface similar to
    // 'bdesb_OverflowMemOutStreambuf' but does *not* inherit from
    // 'bsl::streambuf'.  Thus, it is suitable for use as template parameter to
    // 'bdex_GenericByteOutStream' (but not to 'bdex_ByteOutStream' or
    // 'bdex_ByteOutStreamFormatter').  Note that this class is not designed to
    // be derived from.

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

    bslma_Allocator
         *d_allocator_p;           // memory allocator (held, not owned)

    // NOT IMPLEMENTED
    bdesb_OverflowMemOutput(const bdesb_OverflowMemOutput&);
    bdesb_OverflowMemOutput& operator=(const bdesb_OverflowMemOutput&);

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
    bdesb_OverflowMemOutput(char            *buffer,
                            int              length,
                            bslma_Allocator *basicAllocator = 0);
        // Create an empty stream buffer that uses the specified 'buffer' as an
        // initial output buffer of the specified 'length' (in bytes).
        // Optionally specify 'basicAllocator' for used to supply memory.   If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  Note that this stream buffer does not assume ownership of
        // 'buffer'.

    ~bdesb_OverflowMemOutput();
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

    bsl::streampos pubseekpos(bsl::streampos     position,
                              ios_base::openmode mode = bsl::ios_base::in
                                                      | bsl::ios_base::out);
        // Set the location from which the next I/O operation indicated by the
        // specified 'mode' will occur to the specified 'position'.
        // Return 'position' on success, and 'bsl::streampos(-1)' otherwise.
        // Note that this method will fail if 'mode & bsl::ios_base::in' is not
        // 0.

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
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

                       // -----------------------------
                       // class bdesb_OverflowMemOutput
                       // -----------------------------

// CREATORS
inline
bdesb_OverflowMemOutput::~bdesb_OverflowMemOutput()
{
    d_allocator_p->deallocate(d_overflowBuffer_p);
}

// MANIPULATORS
inline
bsl::streambuf *bdesb_OverflowMemOutput::pubsetbuf(char *, bsl::streamsize)
{
    return 0;
}

inline
bsl::streampos bdesb_OverflowMemOutput::pubseekpos(
                                              bsl::streampos          position,
                                              bsl::ios_base::openmode which)
{
    return pubseekoff(bsl::streamoff(position), bsl::ios_base::beg, which);
}

inline
int bdesb_OverflowMemOutput::pubsync()
{
    return 0;
}

inline
bsl::streamsize bdesb_OverflowMemOutput::in_avail()
{
    return 0;
}

inline
int bdesb_OverflowMemOutput::snextc()
{
    return bsl::streambuf::traits_type::eof();
}

inline
int bdesb_OverflowMemOutput::sbump()
{
    return bsl::streambuf::traits_type::eof();
}

inline
int bdesb_OverflowMemOutput::sgetc()
{
    return bsl::streambuf::traits_type::eof();
}

inline
bsl::streamsize bdesb_OverflowMemOutput::sgetn(char *, bsl::streamsize)
{
    return bsl::streambuf::traits_type::eof();
}

inline
int bdesb_OverflowMemOutput::sputbackc(char)
{
    return bsl::streambuf::traits_type::eof();
}

inline
int bdesb_OverflowMemOutput::sungetc()
{
    return bsl::streambuf::traits_type::eof();
}

// ACCESSORS
inline
int bdesb_OverflowMemOutput::dataLength() const
{
    return d_dataLength;
}

inline
int bdesb_OverflowMemOutput::dataLengthInOverflowBuffer() const
{
    return d_inOverflowBufferFlag ? d_dataLength - d_initialBufferSize : 0;
}

inline
int bdesb_OverflowMemOutput::dataLengthInInitialBuffer() const
{
    return d_inOverflowBufferFlag ? d_initialBufferSize : d_dataLength;
}

inline
const char *bdesb_OverflowMemOutput::overflowBuffer() const
{
    return d_overflowBuffer_p;
}

inline
int bdesb_OverflowMemOutput::overflowBufferSize() const
{
    return d_overflowBufferSize;
}

inline
const char *bdesb_OverflowMemOutput::initialBuffer() const
{
    return d_initialBuffer_p;
}

inline
int bdesb_OverflowMemOutput::initialBufferSize() const
{
    return d_initialBufferSize;
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
