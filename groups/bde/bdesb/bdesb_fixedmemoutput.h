// bdesb_fixedmemoutput.h                                             -*-C++-*-
#ifndef INCLUDED_BDESB_FIXEDMEMOUTPUT
#define INCLUDED_BDESB_FIXEDMEMOUTPUT

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a basic output stream buffer using a client buffer.
//
//@CLASSES:
// bdesb_FixedMemOutput: basic output stream buffer using client memory
//
//@AUTHOR: Herve Bronnimann (hbronnimann)
//
//@SEE_ALSO: bdesb_fixedmemoutstreambuf
//
//@DESCRIPTION: This component implements the output portion of the
// 'bsl::basic_streambuf' protocol using a client-supplied memory buffer.
// Method names correspond to the protocol-specified method names.
// Clients supply the character buffer at stream buffer construction, and can
// later reinitialize the stream buffer with a different character buffer by
// calling the 'pubsetbuf' method.  The only difference between this component
// and 'bdesb_fixedmemoutstreambuf' is that the class 'bdesb_FixedMemOutput'
// does *not* derive from a 'bsl::streambuf' and does not support locales.
// This is advantageous for performance reasons, as the overhead of the
// initialization and virtual function calls of a 'bsl::streambuf' can be
// undesirable.  It is especially designed for streaming a very small amount of
// information into a fixed-length buffer using a 'bdex_GenericByteOutStream'
// when the output is guaranteed not to exceed the length of the buffer.
//
///Usage
///-----
// See the 'bdesb_fixedmemoutstreambuf' component for an identical usage
// example, where every occurrence of 'bdesb_FixedMemOutStreamBuf' can be
// substituted for 'bdesb_FixedMemOutput'.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
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

#ifndef INCLUDED_BSL_STREAMBUF
#include <bsl_streambuf.h>
#endif


namespace BloombergLP {

                        // ====================
                        // bdesb_FixedMemOutput
                        // ====================

class bdesb_FixedMemOutput {
    // This class, like 'bdesb_FixedMemOutStreamBuf', implements the output
    // functionality of the 'basic_streambuf' interface, using client-supplied
    // 'char *' memory.  It has an identical interface to
    // 'bdesb_FixedMemOutStreamBuf' but does *not* inherit from
    // 'bsl::streambuf'.  Thus, it is suitable for use as template parameter to
    // 'bdex_GenericByteOutStream' (but not to 'bdex_ByteOutStream' or
    // 'bdex_ByteOutStreamFormatter').  Note that this class is not designed to
    // be derived from.

  public:
    // TYPES
    typedef char                             char_type;
    typedef bsl::char_traits<char>::int_type int_type;
    typedef bsl::char_traits<char>::pos_type pos_type;
    typedef bsl::char_traits<char>::off_type off_type;
    typedef bsl::char_traits<char>           traits_type;

  private:
    // PRIVATE CLASS METHODS
    static int_type eof() { return traits_type::eof(); }

    // PRIVATE DATA MEMBERS
    char            *d_buffer_p;  // output buffer
    bsl::streamsize  d_capacity;  // length of output buffer
    pos_type         d_pos;       // output cursor

    // PRIVATE MANIPULATORS

    // NOTE: MSVC 13.10.3077 breaks if 'traits_type' is used in the default arg
    int_type overflow(int_type c = bsl::char_traits<char>::eof());
        // Return 'c' to indicate success, except when
        // 'traits::eq_int_type(c,traits::eof())' returns true, in which case
        // return 'traits::not_eof(c)'.  (From note 278, p. 634 of the C++
        // standard ISO/IEC 14882:2003(E).)

    // NOT IMPLEMENTED
    bdesb_FixedMemOutput(const bdesb_FixedMemOutput&);
    bdesb_FixedMemOutput& operator=(const bdesb_FixedMemOutput&);

  public:
    // CREATORS
    bdesb_FixedMemOutput(char *buffer, bsl::streamsize length);
        // Create an empty stream buffer that uses the specified character
        // 'buffer' of the specified 'length'.  The behavior is undefined
        // unless 'buffer' is not zero and 0 < 'length'.  Note that 'buffer' is
        // held but not owned.

    //! ~bdesb_FixedMemOutput();
        // Destroy this stream buffer.  Note that this method's definition is
        // compiler generated.

    // MANIPULATORS
    char *data();
        // Return the address of the modifiable character buffer held by this
        // stream buffer.

                             // *** 27.5.2.2.1 locales: ***

    bsl::locale pubimbue(const bsl::locale& loc);

                             // *** 27.5.2.2.2 buffer and positioning: ***

    bdesb_FixedMemOutput *pubsetbuf(char            *buffer,
                                    bsl::streamsize  length);
        // Reset the internal buffer of this stream to the specified 'buffer'
        // of the specified 'length'.  Note that the next write operation will
        // start at the beginning of 'buffer'.

    pos_type pubseekoff(off_type                offset,
                        bsl::ios_base::seekdir  fixedPosition,
                        bsl::ios_base::openmode which =
                            bsl::ios_base::in | bsl::ios_base::out);
        // Move the current write cursor position by the specified 'offset'.

    pos_type pubseekpos(pos_type                position,
                        bsl::ios_base::openmode which =
                            bsl::ios_base::in | bsl::ios_base::out);
        // Move the current write cursor position to the specified 'position'.

    int pubsync();
        // No-ops.  Note that locales are not supported by this component and
        // also that the stream is always kept in sync (no buffered output).

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
        // Return the size in bytes of the buffer held by this stream
        // buffer.

    const char *data() const;
        // Return the address of the non-modifiable character
        // buffer held by
        // this stream buffer.

    bsl::streamsize length() const;
        // Return the number of characters from the beginning of the buffer to
        // the current write position.  This function returns the same value as
        // 'seekoff(0, bsl::ios_base::end)'.  The length is modified by a call
        // to 'seekpos' or 'seekoff' and reset to zero by a call to
        // 'pubsetbuf'.

                             // *** 27.5.2.2.1 locales: ***

    bsl::locale getloc() const;
        // Return the current default locale.  Note that locales are not
        // supported by this component.

#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY

    bsl::streamsize bufSize() const;
        // Return the number of characters in the buffer held by this stream
        // buffer.  See 'length', below, for the span of bytes actually
        // written.
        //
        // DEPRECATED:  Use the 'capacity' method instead.

#endif

};

// PRIVATE MANIPULATORS
inline
bdesb_FixedMemOutput::int_type bdesb_FixedMemOutput::overflow(int_type c)
{
    return traits_type::eq_int_type(c,traits_type::eof())
               ? traits_type::not_eof(c) : c;
}

// CREATORS
inline
bdesb_FixedMemOutput::bdesb_FixedMemOutput(char            *buffer,
                                           bsl::streamsize  length)
: d_buffer_p(buffer)
, d_capacity(length)
, d_pos(0)
{
}

// MANIPULATORS
inline
char *bdesb_FixedMemOutput::data()
{
    return d_buffer_p;
}

inline
bdesb_FixedMemOutput *bdesb_FixedMemOutput::pubsetbuf(char            *buffer,
                                                      bsl::streamsize  length)
{
    d_buffer_p = buffer;
    d_capacity = length;
    d_pos      = 0;

    return this;
}

inline
bsl::locale bdesb_FixedMemOutput::pubimbue(const bsl::locale&)
{
    return bsl::locale();
}

inline
int bdesb_FixedMemOutput::pubsync()
{
    // Nothing to do, the buffer is always up to date.

    return 0;
}

inline
bdesb_FixedMemOutput::int_type bdesb_FixedMemOutput::sputc(char c)
{
    if (d_pos >= d_capacity) {
        return traits_type::eof();
    }
    d_buffer_p[d_pos] = c;
    d_pos += 1;
    return traits_type::to_int_type(c);
}

inline
bsl::streamsize bdesb_FixedMemOutput::sputn(const char      *s,
                                            bsl::streamsize  length)
{
    pos_type current = d_pos;
    d_pos += length;
    if (d_pos > d_capacity) {
        d_pos  = d_capacity;
        length = d_capacity - current;
    }
    bsl::memcpy(d_buffer_p + current, s, length);
    return length;
}

// ACCESSORS
inline
bsl::streamsize bdesb_FixedMemOutput::capacity() const
{
    return d_capacity;
}

inline
const char *bdesb_FixedMemOutput::data() const
{
    return d_buffer_p;
}

inline
bsl::locale bdesb_FixedMemOutput::getloc() const
{
    return bsl::locale();
}

inline
bsl::streamsize bdesb_FixedMemOutput::length() const
{
    return bsl::streamsize(d_pos);
}

#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY

inline
bsl::streamsize bdesb_FixedMemOutput::bufSize() const
{
    return d_capacity;
}

#endif

}  // closing namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
