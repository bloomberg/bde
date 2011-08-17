// bdex_genericbyteoutstream.h                                        -*-C++-*-
#ifndef INCLUDED_BDEX_GENERICBYTEOUTSTREAM
#define INCLUDED_BDEX_GENERICBYTEOUTSTREAM

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Format fundamental types for streaming to a parameterized stream.
//
//@CLASSES:
//   bdex_GenericByteOutStream: write to a parameterized stream
//
//@AUTHOR: Herve Bronnimann (hbronnimann)
//
//@SEE_ALSO: bdex_byteoutstreamformatter, bdex_genericbyteinstream
//
//@DESCRIPTION: This component conforms to the 'bdex_outstream' (documentation
// only) "protocol", implementing a byte-array-based output stream class that
// provides platform-independent output methods ("externalization") on values,
// and arrays of values, of fundamental types, and on 'bsl::string'.
//
// This component is intended to be used in conjunction with the
// 'bdex_genericbyteinstream' "unexternalization" component.  Each output
// method of 'bdex_GenericByteOutStream' writes a value or a homogeneous array
// of values to an internally held stream.  The values are formatted to be
// readable by the corresponding 'bdex_GenericByteInStream' method.  In
// general, the user cannot rely on any other mechanism to read data written by
// 'bdex_GenericByteOutStream' unless that mechanism explicitly states its
// ability to do so.  Note that the 'bdex_ByteInStream' is a kind of
// 'bdex_GenericByteInStream', so it is perfectly acceptable to read with a
// 'bdex_ByteInStream' values that have been written with a
// 'bdex_GenericByteOutStream', and vice versa.
//
// Unlike 'bdex_ByteOutStreamFormatter' which assumes the underlying stream to
// be implement the 'bsl::basic_streambuf' protocol, this component is generic
// in that the underlying stream is provided as a template parameter whose
// syntactic requirements are identical to a 'bsl::basic_streambuf' but which
// is not required to derive from it.  The main reason to prefer this component
// over a 'bdex_byteoutstreamformatter' is performance, as the parameterized
// stream can be chosen to avoid all the overhead of virtual function calls
// inside the 'bsl::basic_streambuf' hierarchy.
//
// The supported types and required content are listed in the table below.  All
// of the fundamental types in the table may be output as scalar values or as
// homogeneous arrays.  'bsl::string' is output as a logical scalar string.
// Note that 'Int64' and 'Uint64' denote 'bsls_PlatformUtil::Int64' and
// 'bsls_PlatformUtil::Uint64', which in turn are 'typedef' names for the
// signed and unsigned 64-bit integer types, respectively, on the host
// platform.
//..
//      C++ TYPE          REQUIRED CONTENT OF ANY PLATFORM NEUTRAL FORMAT
//      --------          -----------------------------------------------
//      Int64             least significant 64 bits (signed)
//      Uint64            least significant 64 bits (unsigned)
//      int               least significant 32 bits (signed)
//      unsigned int      least significant 32 bits (unsigned)
//      short             least significant 16 bits (signed)
//      unsigned short    least significant 16 bits (unsigned)
//      char              least significant  8 bits (platform dependent)
//      signed char       least significant  8 bits (signed)
//      unsigned char     least significant  8 bits (unsigned)
//      double            IEEE standard 8-byte floating-point value
//      float             IEEE standard 4-byte floating-point value
//
//      bsl::string       BDE STL implementation of the STL string class
//..
// This component also supports compact streaming of integer types.  In
// particular, 64-bit integers can be streamed as 40, 48, 56, or 64-bit values,
// and 32-bit integers can be streamed as 24 or 32-bit values, at the user's
// discretion.  In all cases, the least significant bytes of the fundamental
// integer type are written to the stream.  Note that, for signed types, this
// truncation may not preserve the sign of the streamed value; it is the user's
// responsibility to choose output methods appropriate to the data.
//
// Note that the values are stored in 'Big Endian' (i.e., network byte order)
// format.
//
// Note that output streams can be *invalidated* explicitly and queried for
// *validity*.  Writing to an initially invalid stream has no effect.  Whenever
// an output operation fails, the stream should be invalidated explicitly.
//
///Generic Byte-format Formatter
///-----------------------------
// The class 'bdex_GenericByteOutStream' is parameterized by a buffered stream
// class 'StreamBuf' which, given the declarations:
//..
//  char c;
//  int  len;
//  const char *s;
//  StreamBuf  *sb;
//..
// must make the following expressions syntactically valid, with the assert
// statement highlighting the expected return values:
//..
//  StreamBuf::traits_type::int_type eof = StreamBuf::traits_type::eof();
//  assert(eof != sb->sputc(c));
//  assert(len == sb->sputn(s, len));
//  assert(  0 == sb->pubsync());
//..
// Suitable choices for 'StreamBuf' include any class that implements the
// 'bsl::basic_streambuf' protocol, but also 'bdesb_FixedMemOutput' which does
// not inherit from 'bsl::streambuf' and which, therefore, does not have the
// initialization and virtual function overhead of 'bsl::streambuf'.
//
// The class 'bdex_ByteOutStreamFormatter' is not parameterized and is resolved
// to be the same type as 'bdex_GenericByteOutStream<bsl::streambuf>'.
//
///Usage
///-----
// See the 'bdex_byteoutstreamformatter' component usage example for an a use
// with a 'bsl::streambuf' template parameter.  The same example is valid using
// a 'bdesb_FixedMemOutput' template parameter instead, which is more
// efficient.  We give that example below.
//
// The 'bdex_GenericByteOutStream' implementation of the 'bdex_OutStream'
// protocol can be used to externalize values in a platform-neutral way.
// Writing out fundamental C++ types and 'bsl::string' requires no additional
// work on the part of the client; the client can simply use the stream
// directly.  The following code serializes a few representative values using a
// 'bdex_GenericByteOutStream', and then writes the contents of the 'bdex'
// stream buffer to 'stdout'.
//..
//  int main(int argc, char **argv)
//  {
//
//   // Specify the buffer to write to
//   char buffer[4096];
//   bdesb_FixedMemOutput sb(buffer, sizeof buffer);
//
//   // Create a stream and write out some values.
//   bdex_GenericByteOutStream<bdesb_FixedMemOutput> outStream(&sb);
//   bdex_OutStreamFunctions::streamOut(outStream, 1, 0);
//   bdex_OutStreamFunctions::streamOut(outStream, 2, 0);
//   bdex_OutStreamFunctions::streamOut(outStream, 'c', 0);
//   bdex_OutStreamFunctions::streamOut(outStream, bsl::string("hello"), 0);
//
//   // Verify the results on 'stdout'.
//   const int length = 15;
//   for (int i = 0; i < length; ++i) {
//       if (bsl::isalnum(static_cast<unsigned char>(buffer[i]))) {
//           cout << "nextByte (char): " << buffer[i] << endl;
//       }
//       else {
//           cout << "nextByte (int): " << (int)buffer[i] << endl;
//       }
//   }
//      return 0;
//  }
//..
// Executing the above code results in the following output:
//..
//  nextByte (int): 0
//  nextByte (int): 0
//  nextByte (int): 0
//  nextByte (int): 1
//  nextByte (int): 0
//  nextByte (int): 0
//  nextByte (int): 0
//  nextByte (int): 2
//  nextByte (char): c
//  nextByte (int): 5
//  nextByte (char): h
//  nextByte (char): e
//  nextByte (char): l
//  nextByte (char): l
//  nextByte (char): o
//..
// Note the numeric value 5 prepended to the string literal "hello".  This is
// because 'const char *' types are picked up by the template specialization
// for type 'bsl::string', which outputs the string length before emitting the
// characters.
//
// See also the 'bdex_byteinstream' component usage example for a more
// practical example of using 'bdex' streams with a value-semantic type.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEX_OUTSTREAMMETHODS
#include <bdex_outstreammethods.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_BYTEORDER
#include <bsls_byteorder.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {

                        // ===============================
                        // class bdex_GenericByteOutStream
                        // ===============================

template <class StreamBuf>
class bdex_GenericByteOutStream {
    // This class, parameterized by a stream buffer, provides output methods to
    // externalize fundamental types in byte representation and stores the
    // accumulated result in network byte order format.  The 'StreamBuf'
    // parameter needs to support 'putc', 'putn' and 'pubsync' methods.  Note
    // that 'bsl::streambuf' is suitable for use as the 'StreamBuf' parameter
    // type, in which case use of this parameterized class may be replaced by
    // the non-parameterized 'bdex_GenericOutStream' class.

    // PRIVATE TYPES
    enum {
        // CPU-independent size of data types in wire format.  (Might not be
        // the same as size of data types in memory.)
        BDEX_SIZEOF_INT64   = 8,
        BDEX_SIZEOF_INT56   = 7,
        BDEX_SIZEOF_INT48   = 6,
        BDEX_SIZEOF_INT40   = 5,
        BDEX_SIZEOF_INT32   = 4,
        BDEX_SIZEOF_INT24   = 3,
        BDEX_SIZEOF_INT16   = 2,
        BDEX_SIZEOF_INT8    = 1,
        BDEX_SIZEOF_FLOAT64 = 8,
        BDEX_SIZEOF_FLOAT32 = 4
    };

    // DATA
    StreamBuf *d_streamBuf;  // held, not owned
    bool       d_valid;      // indicates stream validity

    // NOT DEFINED
    bdex_GenericByteOutStream(const bdex_GenericByteOutStream&);
    bdex_GenericByteOutStream& operator=(const bdex_GenericByteOutStream&);

    bool operator==(const bdex_GenericByteOutStream&) const;
    bool operator!=(const bdex_GenericByteOutStream&) const;

    template<class OTHER_BUF>
    bool operator==(const bdex_GenericByteOutStream<OTHER_BUF>&) const;
    template<class OTHER_BUF>
    bool operator!=(const bdex_GenericByteOutStream<OTHER_BUF>&) const;

  public:
    // CREATORS
    bdex_GenericByteOutStream(StreamBuf *streamBuffer);
        // Create an output stream formatter that writes its output to the
        // specified 'streamBuffer'.  Behavior is undefined unless
        // 0 != 'streamBuffer'.

    //! ~bdex_GenericByteOutStream();
        // Destroy this output stream.  Note that this method's definition is
        // compiler generated.

    // MANIPULATORS
    bdex_GenericByteOutStream& flush();
        // Invoke the 'pubsync' method on the underlying stream passed at
        // construction to this formatter, if this stream is still valid.

    void invalidate();
        // Put this output stream in an invalid state.  This function has no
        // effect if this stream is already invalid.  Note that this function
        // should be called whenever a write operation to this stream fails.

    bdex_GenericByteOutStream& putLength(int length);
        // Format the specified 'length' as an 8-bit unsigned integer if
        // 'length' is less than 128, otherwise as a 32-bit integer to this
        // output stream with its most significant bit set and return a
        // reference to this modifiable stream.

    bdex_GenericByteOutStream& putString(const bsl::string& str);
        // Format as an array of 8-bit integers the specified 'str' to this
        // output stream and return a reference to this modifiable stream.

    bdex_GenericByteOutStream& putVersion(int version);
        // Format the specified 'version' as an 8-bit unsigned integer to this
        // output stream and return a reference to this modifiable stream.

    ///Putting Scalar Integer Values
    ///- - - - - - - - - - - - - - -
    bdex_GenericByteOutStream& putInt64(bsls_PlatformUtil::Int64 value);
        // Format as a signed integer the least significant 64 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    bdex_GenericByteOutStream& putUint64(bsls_PlatformUtil::Int64 value);
        // Format as an unsigned integer the least significant 64 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    bdex_GenericByteOutStream& putInt56(bsls_PlatformUtil::Int64 value);
        // Format as a signed integer the least significant 56 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    bdex_GenericByteOutStream& putUint56(bsls_PlatformUtil::Int64 value);
        // Format as an unsigned integer the least significant 56 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    bdex_GenericByteOutStream& putInt48(bsls_PlatformUtil::Int64 value);
        // Format as a signed integer the least significant 48 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    bdex_GenericByteOutStream& putUint48(bsls_PlatformUtil::Int64 value);
        // Format as an unsigned integer the least significant 48 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    bdex_GenericByteOutStream& putInt40(bsls_PlatformUtil::Int64 value);
        // Format as a signed integer the least significant 40 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    bdex_GenericByteOutStream& putUint40(bsls_PlatformUtil::Int64 value);
        // Format as an unsigned integer the least significant 40 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    bdex_GenericByteOutStream& putInt32(int value);
        // Format as a signed integer the least significant 32 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    bdex_GenericByteOutStream& putUint32(unsigned int value);
        // Format as an unsigned integer the least significant 32 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    bdex_GenericByteOutStream& putInt24(int value);
        // Format as a signed integer the least significant 24 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    bdex_GenericByteOutStream& putUint24(int value);
        // Format as an unsigned integer the least significant 24 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    bdex_GenericByteOutStream& putInt16(int value);
        // Format as a signed integer the least significant 16 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    bdex_GenericByteOutStream& putUint16(int value);
        // Format as an unsigned integer the least significant 16 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    bdex_GenericByteOutStream& putInt8(int value);
        // Format as a signed integer the least significant 8 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    bdex_GenericByteOutStream& putUint8(int value);
        // Format as an unsigned integer the least significant 8 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    ///Putting Scalar Floating-Point Values
    /// - - - - - - - - - - - - - - - - - -
    bdex_GenericByteOutStream& putFloat64(double value);
        // Format as an IEEE double-precision floating point number the most
        // significant 8 bytes in the specified 'value' to this output stream
        // and return a reference to this modifiable stream.  Note that, for
        // non-conforming platforms, the "most significant 8 bytes" might not
        // be contiguous bits, and that this operation may be lossy.

    bdex_GenericByteOutStream& putFloat32(float value);
        // Format as an IEEE single-precision floating point number the most
        // significant 4 bytes in the specified 'value' to this output stream
        // and return a reference to this modifiable stream.  Note that, for
        // non-conforming platforms, the "most significant 4 bytes" might not
        // be contiguous bits, and that this operation may be lossy.

    ///Putting Arrays of Integral Values
    ///- - - - - - - - - - - - - - - - -
    bdex_GenericByteOutStream&
    putArrayInt64(const bsls_PlatformUtil::Int64 *array, int count);
        // Format as an atomic sequence of signed integers the least
        // significant 64 bits of each of the specified 'count' leading entries
        // in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= count'.

    bdex_GenericByteOutStream&
    putArrayUint64(const bsls_PlatformUtil::Uint64 *array, int count);
        // Format as an atomic sequence of unsigned integers the least
        // significant 64 bits of each of the specified 'count' leading entries
        // in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= count'.

    bdex_GenericByteOutStream&
    putArrayInt56(const bsls_PlatformUtil::Int64 *array, int count);
        // Format as an atomic sequence of signed integers the least
        // significant 56 bits of each of the specified 'count' leading entries
        // in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= count'.

    bdex_GenericByteOutStream&
    putArrayUint56(const bsls_PlatformUtil::Uint64 *array, int count);
        // Format as an atomic sequence of unsigned integers the least
        // significant 56 bits of each of the specified 'count' leading entries
        // in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= count'.

    bdex_GenericByteOutStream&
    putArrayInt48(const bsls_PlatformUtil::Int64 *array, int count);
        // Format as an atomic sequence of signed integers the least
        // significant 48 bits of each of the specified 'count' leading entries
        // in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= count'.

    bdex_GenericByteOutStream&
    putArrayUint48(const bsls_PlatformUtil::Uint64 *array, int count);
        // Format as an atomic sequence of unsigned integers the least
        // significant 48 bits of each of the specified 'count' leading entries
        // in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= count'.

    bdex_GenericByteOutStream&
    putArrayInt40(const bsls_PlatformUtil::Int64 *array, int count);
        // Format as an atomic sequence of signed integers the least
        // significant 40 bits of each of the specified 'count' leading entries
        // in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= count'.

    bdex_GenericByteOutStream&
    putArrayUint40(const bsls_PlatformUtil::Uint64 *array, int count);
        // Format as an atomic sequence of unsigned integers the least
        // significant 40 bits of each of the specified 'count' leading entries
        // in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= count'.

    bdex_GenericByteOutStream&
    putArrayInt32(const int *array, int count);
        // Format as an atomic sequence of signed integers the least
        // significant 32 bits of each of the specified 'count' leading entries
        // in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= count'.

    bdex_GenericByteOutStream&
    putArrayUint32(const unsigned int *array, int count);
        // Format as an atomic sequence of unsigned integers the least
        // significant 32 bits of each of the specified 'count' leading entries
        // in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= count'.

    bdex_GenericByteOutStream&
    putArrayInt24(const int *array, int count);
        // Format as an atomic sequence of signed integers the least
        // significant 24 bits of each of the specified 'count' leading entries
        // in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= count'.

    bdex_GenericByteOutStream&
    putArrayUint24(const unsigned int *array, int  count);
        // Format as an atomic sequence of unsigned integers the least
        // significant 24 bits of each of the specified 'count' leading entries
        // in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= count'.

    bdex_GenericByteOutStream&
    putArrayInt16(const short *array, int count);
        // Format as an atomic sequence of signed integers the least
        // significant 16 bits of each of the specified 'count' leading entries
        // in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= count'.

    bdex_GenericByteOutStream&
    putArrayUint16(const unsigned short *array, int count);
        // Format as an atomic sequence of unsigned integers the least
        // significant 16 bits of each of the specified 'count' leading entries
        // in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= count'.

    bdex_GenericByteOutStream&
    putArrayInt8(const char *array, int count);
    bdex_GenericByteOutStream&
    putArrayInt8(const signed char *array, int  count);
        // Format as an atomic sequence of signed integers the 8 bits of each
        // of the specified 'count' leading entries in the specified 'array' to
        // this output stream and return a reference to this modifiable stream.
        // The behavior is undefined unless '0 <= count'.

    bdex_GenericByteOutStream&
    putArrayUint8(const char *array, int count);
    bdex_GenericByteOutStream&
    putArrayUint8(const unsigned char *array, int count);
        // Format as an atomic sequence of unsigned integers the 8 bits of each
        // of the specified 'count' leading entries in the specified 'array' to
        // this output stream and return a reference to this modifiable stream.
        // The behavior is undefined unless '0 <= count'.

    ///Putting Arrays of Floating-Point Values
    ///- - - - - - - - - - - - - - - - - - - -
    bdex_GenericByteOutStream&
    putArrayFloat64(const double *array, int count);
        // Format as an atomic sequence of IEEE double-precision floating point
        // numbers the most significant 8 bytes in each of the specified
        // 'count' leading entries in the specified 'array' to this output
        // stream and return a reference to this modifiable stream.  The
        // behavior is undefined unless '0 <= count'.  Note that, for
        // non-conforming platforms, the "most significant 8 bytes" might not
        // be contiguous bits, and that this operation may be lossy.

    bdex_GenericByteOutStream&
    putArrayFloat32(const float *array, int count);
        // Format as an atomic sequence of IEEE single-precision floating point
        // numbers the most significant 4 bytes in each of the specified
        // 'count' leading entries in the specified 'array' to this output
        // stream and return a reference to this modifiable stream.  The
        // behavior is undefined unless '0 <= count'.  Note that, for
        // non-conforming platforms, the "most significant 4 bytes" might not
        // be contiguous bits, and that this operation may be lossy.

    // ACCESSORS
    operator const void *() const;
        // Return a non-zero value if this stream is valid, and 0 otherwise.
        // An invalid stream is a stream for which an output operation was
        // detected to have failed.
};

// FREE OPERATORS
template <class StreamBuf>
inline
bdex_GenericByteOutStream<StreamBuf>&
  operator<<(bdex_GenericByteOutStream<StreamBuf>& stream,
             bdex_GenericByteOutStream<StreamBuf>&
                        (*pf)(bdex_GenericByteOutStream<StreamBuf>&));
    // Provide support for all manipulator functions on this stream, e.g.,
    // 'bdexFlush'.

// TBD #if !defined(BSL_LEGACY) || 1 == BSL_LEGACY

template <class StreamBuf, typename OBJECT>
inline
bdex_GenericByteOutStream<StreamBuf>&
  operator<<(bdex_GenericByteOutStream<StreamBuf>& stream,
             const OBJECT&                         object);
    // Write to the specified output 'stream' the most current version number
    // supported by 'OBJECT' and format the value of the specified 'object' to
    // 'stream' using the 'bdex' compliant 'OBJECT::bdexStreamOut' method in
    // the 'OBJECT::maxSupportedBdexVersion' format.  Return a reference to the
    // modifiable 'stream'.  Note that specializations for the supported
    // fundamental (scalar) types and for 'bsl::string' are also provided, in
    // which case this operator calls the appropriate 'put' method from this
    // component.

// TBD #endif

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        // -------------------------------
                        // class bdex_GenericByteOutStream
                        // -------------------------------

// CREATORS
template <class StreamBuf>
inline
bdex_GenericByteOutStream<StreamBuf>::
bdex_GenericByteOutStream(StreamBuf *streamBuffer)
: d_streamBuf(streamBuffer)
, d_valid(true)
{
    BSLS_ASSERT_SAFE(streamBuffer);
}

// MANIPULATORS
template <class StreamBuf>
inline
bdex_GenericByteOutStream<StreamBuf>&
bdex_GenericByteOutStream<StreamBuf>::flush()
{
    d_valid = d_valid ? 0 == d_streamBuf->pubsync() : false;
    return *this;
}

template <class StreamBuf>
inline
void bdex_GenericByteOutStream<StreamBuf>::invalidate()
{
    d_valid = false;
}

template <class StreamBuf>
inline
bdex_GenericByteOutStream<StreamBuf>&
bdex_GenericByteOutStream<StreamBuf>::putLength(int length)
{
    BSLS_ASSERT_SAFE(0 <= length);

    if (length > 127) {
        putInt32(length | (1 << 31));
    }
    else {
        putInt8(length);
    }
    return *this;
}

template <class StreamBuf>
inline
bdex_GenericByteOutStream<StreamBuf>&
bdex_GenericByteOutStream<StreamBuf>::putString(const bsl::string& str)
{
    const int length = (int)str.length();
    putLength(length);
    return putArrayUint8(str.data(), length);
}

template <class StreamBuf>
inline
bdex_GenericByteOutStream<StreamBuf>&
bdex_GenericByteOutStream<StreamBuf>::putVersion(int version)
{
    const unsigned char tmp = (unsigned char)version;
    putUint8(tmp);
    return *this;
}

///Putting Scalar Integral Values
/// - - - - - - - - - - - - - - -

template <class StreamBuf>
inline
bdex_GenericByteOutStream<StreamBuf>&
bdex_GenericByteOutStream<StreamBuf>::putInt64(bsls_PlatformUtil::Int64 value)
{
#if BSLS_PLATFORMUTIL__IS_LITTLE_ENDIAN
    value = BSLS_BYTEORDER_HOST_U64_TO_BE(value);
#endif
    d_valid = d_valid ? 8 == d_streamBuf->sputn((char *)&value, 8) : false;
    return *this;
}

template <class StreamBuf>
inline
bdex_GenericByteOutStream<StreamBuf>&
bdex_GenericByteOutStream<StreamBuf>::putUint64(bsls_PlatformUtil::Int64 value)
{
    return putInt64(value);
}

template <class StreamBuf>
inline
bdex_GenericByteOutStream<StreamBuf>&
bdex_GenericByteOutStream<StreamBuf>::putInt56(bsls_PlatformUtil::Int64 value)
{
#if BSLS_PLATFORMUTIL__IS_LITTLE_ENDIAN
    char data[7];
    const char *byte = (const char *)&value;

    data[0] = byte[6];
    data[1] = byte[5];
    data[2] = byte[4];
    data[3] = byte[3];
    data[4] = byte[2];
    data[5] = byte[1];
    data[6] = byte[0];
    d_valid = d_valid ? 7 == d_streamBuf->sputn(data, 7) : false;
#else
    d_valid = d_valid ? 7 == d_streamBuf->sputn(&((char *)&value)[1], 7)
                      : false;
#endif
    return *this;
}

template <class StreamBuf>
inline
bdex_GenericByteOutStream<StreamBuf>&
bdex_GenericByteOutStream<StreamBuf>::putUint56(bsls_PlatformUtil::Int64 value)
{
    return putInt56(value);
}

template <class StreamBuf>
inline
bdex_GenericByteOutStream<StreamBuf>&
bdex_GenericByteOutStream<StreamBuf>::putInt48(bsls_PlatformUtil::Int64 value)
{
#if BSLS_PLATFORMUTIL__IS_LITTLE_ENDIAN
    char data[6];
    const char *byte = (const char *)&value;

    data[0] = byte[5];
    data[1] = byte[4];
    data[2] = byte[3];
    data[3] = byte[2];
    data[4] = byte[1];
    data[5] = byte[0];
    d_valid = d_valid ? 6 == d_streamBuf->sputn(data, 6) : false;
#else
    d_valid = d_valid ? 6 == d_streamBuf->sputn(&((char *)&value)[2], 6)
                      : false;
#endif
    return *this;
}

template <class StreamBuf>
inline
bdex_GenericByteOutStream<StreamBuf>&
bdex_GenericByteOutStream<StreamBuf>::putUint48(bsls_PlatformUtil::Int64 value)
{
    return putInt48(value);
}

template <class StreamBuf>
inline
bdex_GenericByteOutStream<StreamBuf>&
bdex_GenericByteOutStream<StreamBuf>::putInt40(bsls_PlatformUtil::Int64 value)
{
#if BSLS_PLATFORMUTIL__IS_LITTLE_ENDIAN
    char data[5];
    const char *byte = (const char *)&value;

    data[0] = byte[4];
    data[1] = byte[3];
    data[2] = byte[2];
    data[3] = byte[1];
    data[4] = byte[0];
    d_valid = d_valid ? 5 == d_streamBuf->sputn(data, 5) : false;
#else
    d_valid = d_valid ? 5 == d_streamBuf->sputn(&((char *)&value)[3], 5)
                      : false;
#endif
    return *this;
}

template <class StreamBuf>
inline
bdex_GenericByteOutStream<StreamBuf>&
bdex_GenericByteOutStream<StreamBuf>::putUint40(bsls_PlatformUtil::Int64 value)
{
    return putInt40(value);
}

template <class StreamBuf>
inline
bdex_GenericByteOutStream<StreamBuf>&
bdex_GenericByteOutStream<StreamBuf>::putInt32(int value)
{
#if BSLS_PLATFORMUTIL__IS_LITTLE_ENDIAN
    value = BSLS_BYTEORDER_HOST_U32_TO_BE(value);
#endif
    d_valid = d_valid ? 4 == d_streamBuf->sputn((char *)&value, 4) : false;
    return *this;
}

template <class StreamBuf>
inline
bdex_GenericByteOutStream<StreamBuf>&
bdex_GenericByteOutStream<StreamBuf>::putUint32(unsigned int value)
{
    return putInt32(value);
}

template <class StreamBuf>
inline
bdex_GenericByteOutStream<StreamBuf>&
bdex_GenericByteOutStream<StreamBuf>::putInt24(int value)
{
#if BSLS_PLATFORMUTIL__IS_LITTLE_ENDIAN
    char data[3];
    const char *byte = (const char *)&value;

    data[0] = byte[2];
    data[1] = byte[1];
    data[2] = byte[0];
    d_valid = d_valid ? 3 == d_streamBuf->sputn(data, 3) : false;
#else
    d_valid = d_valid ? 3 == d_streamBuf->sputn(&((char *)&value)[1], 3)
                      : false;
#endif
    return *this;
}

template <class StreamBuf>
inline
bdex_GenericByteOutStream<StreamBuf>&
bdex_GenericByteOutStream<StreamBuf>::putUint24(int value)
{
    return putInt24(value);
}

template <class StreamBuf>
inline
bdex_GenericByteOutStream<StreamBuf>&
bdex_GenericByteOutStream<StreamBuf>::putInt16(int value)
{
#if BSLS_PLATFORMUTIL__IS_LITTLE_ENDIAN
    char data[2];
    const char *byte = (const char *)&value;

    data[0] = byte[1];
    data[1] = byte[0];
    d_valid = d_valid ? 2 == d_streamBuf->sputn(data, 2) : false;
#else
    d_valid = d_valid ? 2 == d_streamBuf->sputn(&((char *)&value)[2], 2)
                      : false;
#endif
    return *this;
}

template <class StreamBuf>
inline
bdex_GenericByteOutStream<StreamBuf>&
bdex_GenericByteOutStream<StreamBuf>::putUint16(int value)
{
    return putInt16(value);
}

template <class StreamBuf>
inline
bdex_GenericByteOutStream<StreamBuf>&
bdex_GenericByteOutStream<StreamBuf>::putInt8(int value)
{
#if BSLS_PLATFORMUTIL__IS_LITTLE_ENDIAN
    d_valid = d_valid
     ? StreamBuf::traits_type::eof() != d_streamBuf->sputc(((char *)&value)[0])
     : false;
#else
    d_valid = d_valid
     ? StreamBuf::traits_type::eof() != d_streamBuf->sputc(((char *)&value)[3])
     : false;
#endif
    return *this;
}

template <class StreamBuf>
inline
bdex_GenericByteOutStream<StreamBuf>&
bdex_GenericByteOutStream<StreamBuf>::putUint8(int value)
{
    return putInt8(value);
}

///Putting Scalar Floating-Point Values
/// - - - - - - - - - - - - - - - - - -

template <class StreamBuf>
inline
bdex_GenericByteOutStream<StreamBuf>&
bdex_GenericByteOutStream<StreamBuf>::putFloat64(double value)
{
#if BSLS_PLATFORMUTIL__IS_LITTLE_ENDIAN
    typedef union {
        double                    d_variable;
        bsls_PlatformUtil::Uint64 d_longlong;
    } T;
    T u = { value };

    u.d_longlong = BSLS_BYTEORDER_HOST_U64_TO_BE(u.d_longlong);
    d_valid = d_valid ? 8 == d_streamBuf->sputn((char *)&u.d_longlong, 8)
                      : false;
#else
    d_valid = d_valid ? 8 == d_streamBuf->sputn((char *)&value, 8) : false;
#endif
    return *this;
}

template <class StreamBuf>
inline
bdex_GenericByteOutStream<StreamBuf>&
bdex_GenericByteOutStream<StreamBuf>::putFloat32(float value)
{
#if BSLS_PLATFORMUTIL__IS_LITTLE_ENDIAN
    typedef union {
        float d_variable;
        int   d_int;
    } T;
    T u = { value };

    u.d_int = BSLS_BYTEORDER_HOST_U32_TO_BE(u.d_int);
    d_valid = d_valid ? 4 == d_streamBuf->sputn((char *)&u.d_int, 4) : false;
#else
    d_valid = d_valid ? 4 == d_streamBuf->sputn((char *)&value, 4) : false;
#endif
    return *this;
}

///Putting Arrays of Integral Values
///- - - - - - - - - - - - - - - - -

template <class StreamBuf>
#if BSLS_PLATFORMUTIL__IS_BIG_ENDIAN
inline
#endif
bdex_GenericByteOutStream<StreamBuf>&
bdex_GenericByteOutStream<StreamBuf>::putArrayInt64(
                                         const bsls_PlatformUtil::Int64 *array,
                                         int                             count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

#if BSLS_PLATFORMUTIL__IS_LITTLE_ENDIAN
    const bsls_PlatformUtil::Int64 *endArray = array + count;
    for (; array < endArray; ++array) {
        putInt64(*array);
    }
#else
    const int len = (int)(sizeof(bsls_PlatformUtil::Int64) * count);
    d_valid = d_valid ? len == d_streamBuf->sputn((const char *)array, len)
                      : false;
#endif
    return *this;
}

template <class StreamBuf>
#if BSLS_PLATFORMUTIL__IS_BIG_ENDIAN
inline
#endif
bdex_GenericByteOutStream<StreamBuf>&
bdex_GenericByteOutStream<StreamBuf>::putArrayUint64(
                                        const bsls_PlatformUtil::Uint64 *array,
                                        int                              count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

#if BSLS_PLATFORMUTIL__IS_LITTLE_ENDIAN
    const bsls_PlatformUtil::Uint64 *endArray = array + count;
    for (; array < endArray; ++array) {
        putUint64(*array);
    }
#else
    const int len = (int)(sizeof(bsls_PlatformUtil::Int64) * count);
    d_valid = d_valid ? len == d_streamBuf->sputn((const char *)array, len)
                      : false;
#endif
    return *this;
}

template <class StreamBuf>
bdex_GenericByteOutStream<StreamBuf>&
bdex_GenericByteOutStream<StreamBuf>::putArrayInt56(
                                         const bsls_PlatformUtil::Int64 *array,
                                         int                             count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    const bsls_PlatformUtil::Int64 *endArray = array + count;
    for (; array < endArray; ++array) {
        putInt56(*array);
    }
    return *this;
}

template <class StreamBuf>
bdex_GenericByteOutStream<StreamBuf>&
bdex_GenericByteOutStream<StreamBuf>::putArrayUint56(
                                        const bsls_PlatformUtil::Uint64 *array,
                                        int                              count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    const bsls_PlatformUtil::Uint64 *endArray = array + count;
    for (; array < endArray; ++array) {
        putUint56(*array);
    }
    return *this;
}

template <class StreamBuf>
bdex_GenericByteOutStream<StreamBuf>&
bdex_GenericByteOutStream<StreamBuf>::putArrayInt48(
                                         const bsls_PlatformUtil::Int64 *array,
                                         int                             count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    const bsls_PlatformUtil::Int64 *endArray = array + count;
    for (; array < endArray; ++array) {
        putInt48(*array);
    }
    return *this;
}

template <class StreamBuf>
bdex_GenericByteOutStream<StreamBuf>&
bdex_GenericByteOutStream<StreamBuf>::putArrayUint48(
                                        const bsls_PlatformUtil::Uint64 *array,
                                        int                              count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    const bsls_PlatformUtil::Uint64 *endArray = array + count;
    for (; array < endArray; ++array) {
        putUint48(*array);
    }
    return *this;
}

template <class StreamBuf>
bdex_GenericByteOutStream<StreamBuf>&
bdex_GenericByteOutStream<StreamBuf>::putArrayInt40(
                                         const bsls_PlatformUtil::Int64 *array,
                                         int                             count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    const bsls_PlatformUtil::Int64 *endArray = array + count;
    for (; array < endArray; ++array) {
        putInt40(*array);
    }
    return *this;
}

template <class StreamBuf>
bdex_GenericByteOutStream<StreamBuf>&
bdex_GenericByteOutStream<StreamBuf>::putArrayUint40(
                                        const bsls_PlatformUtil::Uint64 *array,
                                        int                              count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    const bsls_PlatformUtil::Uint64 *endArray = array + count;
    for (; array < endArray; ++array) {
        putUint40(*array);
    }
    return *this;
}

template <class StreamBuf>
#if BSLS_PLATFORMUTIL__IS_BIG_ENDIAN
inline
#endif
bdex_GenericByteOutStream<StreamBuf>&
bdex_GenericByteOutStream<StreamBuf>::putArrayInt32(const int *array,
                                                    int        count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

#if BSLS_PLATFORMUTIL__IS_LITTLE_ENDIAN
    const int *endArray = array + count;
    for (; array < endArray; ++array) {
        putInt32(*array);
    }
#else
    const int len = (int)(sizeof(int) * count);
    d_valid = d_valid ? len == d_streamBuf->sputn((const char *)array, len)
                      : false;
#endif
    return *this;
}

template <class StreamBuf>
#if BSLS_PLATFORMUTIL__IS_BIG_ENDIAN
inline
#endif
bdex_GenericByteOutStream<StreamBuf>&
bdex_GenericByteOutStream<StreamBuf>::putArrayUint32(const unsigned int *array,
                                                     int                 count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

#if BSLS_PLATFORMUTIL__IS_LITTLE_ENDIAN
    const unsigned int *endArray = array + count;
    for (; array < endArray; ++array) {
        putUint32(*array);
    }
#else
    const int len = (int)(sizeof(int) * count);
    d_valid = d_valid ? len == d_streamBuf->sputn((const char *)array, len)
                      : false;
#endif
    return *this;
}

template <class StreamBuf>
bdex_GenericByteOutStream<StreamBuf>&
bdex_GenericByteOutStream<StreamBuf>::putArrayInt24(const int *array,
                                                    int        count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    const int *endArray = array + count;
    for (; array < endArray; ++array) {
        putInt24(*array);
    }
    return *this;
}

template <class StreamBuf>
bdex_GenericByteOutStream<StreamBuf>&
bdex_GenericByteOutStream<StreamBuf>::putArrayUint24(const unsigned int *array,
                                                     int                 count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    const unsigned int *endArray = array + count;
    for (; array < endArray; ++array) {
        putUint24(*array);
    }
    return *this;
}

template <class StreamBuf>
#if BSLS_PLATFORMUTIL__IS_BIG_ENDIAN
inline
#endif
bdex_GenericByteOutStream<StreamBuf>&
bdex_GenericByteOutStream<StreamBuf>::putArrayInt16(const short *array,
                                                    int          count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

#if BSLS_PLATFORMUTIL__IS_LITTLE_ENDIAN
    const short *endArray = array + count;
    for (; array < endArray; ++array) {
        putInt16(*array);
    }
#else
    const int len = (int)(sizeof(short) * count);
    d_valid = d_valid ? len == d_streamBuf->sputn((const char *)array, len)
                      : false;
#endif
    return *this;
}

template <class StreamBuf>
#if BSLS_PLATFORMUTIL__IS_BIG_ENDIAN
inline
#endif
bdex_GenericByteOutStream<StreamBuf>&
bdex_GenericByteOutStream<StreamBuf>::putArrayUint16(
                                                   const unsigned short *array,
                                                   int                   count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

#if BSLS_PLATFORMUTIL__IS_LITTLE_ENDIAN
    const unsigned short *endArray = array + count;
    for (; array < endArray; ++array) {
        putUint16(*array);
    }
#else
    const int len = (int)(sizeof(short) * count);
    d_valid = d_valid ? len == d_streamBuf->sputn((const char *)array, len)
                      : false;
#endif
    return *this;
}

template <class StreamBuf>
inline
bdex_GenericByteOutStream<StreamBuf>&
bdex_GenericByteOutStream<StreamBuf>::putArrayInt8(const char *array,
                                                   int         count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    d_valid = d_valid ? count == d_streamBuf->sputn(array, count) : false;
    return *this;
}

template <class StreamBuf>
inline
bdex_GenericByteOutStream<StreamBuf>&
bdex_GenericByteOutStream<StreamBuf>::putArrayInt8(const signed char *array,
                                                   int                count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    putArrayInt8((const char *)array, count);
    return *this;
}

template <class StreamBuf>
inline
bdex_GenericByteOutStream<StreamBuf>&
bdex_GenericByteOutStream<StreamBuf>::putArrayUint8(const char *array,
                                                    int         count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    putArrayInt8((const char *)array, count);
    return *this;
}

template <class StreamBuf>
bdex_GenericByteOutStream<StreamBuf>&
bdex_GenericByteOutStream<StreamBuf>::putArrayUint8(const unsigned char *array,
                                                    int                  count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    const unsigned char *endArray = array + count;
    for (; array < endArray; ++array) {
        putUint8(*array);
    }
    return *this;
}

///Putting Arrays of Floating-Point Values
///- - - - - - - - - - - - - - - - - - - -

template <class StreamBuf>
#if BSLS_PLATFORMUTIL__IS_BIG_ENDIAN
inline
#endif
bdex_GenericByteOutStream<StreamBuf>&
bdex_GenericByteOutStream<StreamBuf>::putArrayFloat64(const double *array,
                                                      int           count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

#if BSLS_PLATFORMUTIL__IS_LITTLE_ENDIAN
    const double *endArray = array + count;
    for (; array < endArray; ++array) {
        putFloat64(*array);
    }
#else
    const int len = (int)(sizeof(double) * count);
    d_valid = d_valid ? len == d_streamBuf->sputn((const char *)array, len)
                      : false;
#endif
    return *this;
}

template <class StreamBuf>
#if BSLS_PLATFORMUTIL__IS_BIG_ENDIAN
inline
#endif
bdex_GenericByteOutStream<StreamBuf>&
bdex_GenericByteOutStream<StreamBuf>::putArrayFloat32(const float *array,
                                                      int          count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

#if BSLS_PLATFORMUTIL__IS_LITTLE_ENDIAN
    const float *endArray = array + count;
    for (; array < endArray; ++array) {
        putFloat32(*array);
    }
#else
    const int len = (int)(sizeof(float) * count);
    d_valid = d_valid ? len == d_streamBuf->sputn((const char *)array, len)
                      : false;
#endif
    return *this;
}

// ACCESSORS
template <class StreamBuf>
inline
bdex_GenericByteOutStream<StreamBuf>::operator const void *() const
{
    return d_valid ? this : 0;
}

// FREE OPERATORS
template <class StreamBuf>
inline
bdex_GenericByteOutStream<StreamBuf>&
  operator<<(bdex_GenericByteOutStream<StreamBuf>& stream,
             bdex_GenericByteOutStream<StreamBuf>&
                         (*pf)(bdex_GenericByteOutStream<StreamBuf>&))
{
    return (*pf)(stream);
}

// TBD #if !defined(BSL_LEGACY) || 1 == BSL_LEGACY

template <class StreamBuf, typename VALUE>
inline
bdex_GenericByteOutStream<StreamBuf>&
  operator<<(bdex_GenericByteOutStream<StreamBuf>& stream, const VALUE& value)
{
    return bdex_OutStreamMethodsUtil::streamOutVersionAndObject(stream, value);
}

// TBD #endif

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
