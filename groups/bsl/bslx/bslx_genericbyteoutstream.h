// bslx_genericbyteoutstream.h                                        -*-C++-*-
#ifndef INCLUDED_BSLX_GENERICBYTEOUTSTREAM
#define INCLUDED_BSLX_GENERICBYTEOUTSTREAM

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Externalization of fundamental types to a parameterized stream.
//
//@CLASSES:
//  bslx::GenericByteOutStream: parameterized output stream for fundamentals
//
//@SEE_ALSO: bslx_byteoutstreamformatter, bslx_genericbyteinstream
//
//@DESCRIPTION: This component implements a parameterized output stream
// class, 'bslx::GenericByteOutStream', that provides platform-independent
// output methods ("externalization") on values, and arrays of values, of
// fundamental types, and on 'bsl::string'.
//
// This component is intended to be used in conjunction with the
// 'bslx_genericbyteinstream' "unexternalization" component.  Each output
// method of 'bslx::GenericByteOutStream' writes either a value or a
// homogeneous array of values of a fundamental type, in a format that is
// readable by the corresponding 'bslx::GenericByteInStream' method.  In
// general, the user cannot rely on any other mechanism to read data written by
// 'bslx::GenericByteOutStream' unless that mechanism explicitly states its
// ability to do so.
//
// The supported types and required content are listed in the table below.  All
// of the fundamental types in the table may be output as scalar values or as
// homogeneous arrays.  'bsl::string' is output as an 'int' representing the
// string's length and a homogeneous 'char' array for the string's data.  Note
// that 'Int64' and 'Uint64' denote 'bsls::Types::Int64' and
// 'bsls::Types::Uint64', which in turn are 'typedef' names for the signed and
// unsigned 64-bit integer types, respectively, on the host platform.
//..
//      C++ TYPE          REQUIRED CONTENT OF ANY PLATFORM-NEUTRAL FORMAT
//      --------          -----------------------------------------------
//      Int64             least significant 64 bits (signed)
//      Uint64            least significant 64 bits (unsigned)
//      int               least significant 32 bits (signed)
//      unsigned int      least significant 32 bits (unsigned)
//      short             least significant 16 bits (signed)
//      unsigned short    least significant 16 bits (unsigned)
//      char              least significant  8 bits (platform-dependent)
//      signed char       least significant  8 bits (signed)
//      unsigned char     least significant  8 bits (unsigned)
//      double            IEEE standard 8-byte floating-point value
//      float             IEEE standard 4-byte floating-point value
//
//      bsl::string       BDE implementation of the STL string class
//..
// This component also supports compact streaming of integer types.  In
// particular, 64-bit integers can be streamed as 40-, 48-, 56-, or 64-bit
// values, and 32-bit integers can be streamed as 24- or 32-bit values, at the
// user's discretion.  In all cases, the least significant bytes of the
// fundamental integer type are written to the stream.  Note that, for signed
// types, this truncation may not preserve the sign of the streamed value; it
// is the user's responsibility to choose output methods appropriate to the
// data.
//
// Note that the values are stored in big-endian (i.e., network byte order)
// format.
//
// Note that output streams can be *invalidated* explicitly and queried for
// *validity*.  Writing to an initially invalid stream has no effect.  Whenever
// an output operation fails, the stream should be invalidated explicitly.
//
///Generic Byte-Format Generator
///-----------------------------
// The class 'bslx::GenericByteOutStream' is parameterized by a buffered stream
// class, 'STREAMBUF', which, given the declarations:
//..
//  char        c;
//  int         len;
//  const char *s;
//  STREAMBUF  *sb;
//..
// must make the following expressions syntactically valid, with the assert
// statements highlighting the expected return values:
//..
//  STREAMBUF::traits_type::int_type eof = STREAMBUF::traits_type::eof();
//  assert(eof != sb->sputc(c));
//  assert(len == sb->sputn(s, len));
//  assert(  0 == sb->pubsync());
//..
// Suitable choices for 'STREAMBUF' include any class that implements the
// 'bsl::basic_streambuf' protocol.
//
// The class 'bslx::ByteOutStreamFormatter' is a 'typedef' of
// 'bslx::GenericByteOutStream<bsl::streambuf>'.
//
///Versioning
///----------
// BDEX provides two concepts that support versioning the BDEX serialization
// format of a type: 'version' and 'versionSelector'.  A 'version' is a 1-based
// integer indicating one of the supported formats (e.g., format 1, format 2,
// etc.).  A 'versionSelector' is a value that is mapped to a 'version' for a
// type by the type's implementation of 'maxSupportedBdexVersion'.
//
// Selecting a value for a 'versionSelector' is required at two different
// points: (1) when implementing a new 'version' format within the
// 'bdexStreamIn' and 'bdexStreamOut' methods of a type, and (2) when
// implementing code that constructs a BDEX 'OutStream'.  In both cases, the
// value should be a *compile*-time-selected value.
//
// When a new 'version' format is implemented within the 'bdexStreamIn' and
// 'bdexStreamOut' methods of a type, a new mapping in
// 'maxSupportedBdexVersion' should be created to expose this new 'version'
// with a 'versionSelector'.  A simple - and the recommended - approach is to
// use a value having the pattern "YYYYMMDD", where "YYYYMMDD" corresponds to
// the "go-live" date of the corresponding 'version' format.
//
// When constructing an 'OutStream', a simple approach is to use the current
// date as a *compile*-time constant value.  In combination with the
// recommended selection of 'versionSelector' values for
// 'maxSupportedBdexVersion', this will result in consistent and predictable
// behavior while externalizing types.  Note that this recommendation is chosen
// for its simplicity: to ensure the largest possible audience for an
// externalized representation, clients can select the minimum date value that
// will result in the desired version of all types externalized with
// 'operator<<' being selected.
//
// See the 'bslx' package-level documentation for more detailed information
// about versioning.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Externalization
///- - - - - - - - - - - - - - - -
// A 'bslx::GenericByteOutStream' can be used to externalize values in a
// platform-neutral way.  Writing out fundamental C++ types and 'bsl::string'
// requires no additional work on the part of the client; the client can simply
// use the stream directly.  The following code serializes a few representative
// values using a 'bslx::GenericByteOutStream', compares the contents of this
// stream to the expected value, and then writes the contents of this stream's
// buffer to 'stdout'.
//
// First, we create a 'bslx::GenericByteOutStream' with an arbitrary value for
// its 'versionSelector' and externalize some values:
//..
//  bsl::stringbuf                             buffer;
//  bslx::GenericByteOutStream<bsl::stringbuf> outStream(&buffer, 20131127);
//  outStream.putInt32(1);
//  outStream.putInt32(2);
//  outStream.putInt8('c');
//  outStream.putString(bsl::string("hello"));
//..
// Then, we compare the contents of the buffer to the expected value:
//..
//  bsl::string  theChars = buffer.str();
//  assert(15 == theChars.size());
//  assert( 0 == bsl::memcmp(theChars.data(),
//                           "\x00\x00\x00\x01\x00\x00\x00\x02""c\x05""hello",
//                           15));
//..
// Finally, we print the buffer's contents to 'bsl::cout'.
//..
//  for (bsl::size_t i = 0; i < theChars.size(); ++i) {
//      if (bsl::isalnum(static_cast<unsigned char>(theChars[i]))) {
//          bsl::cout << "nextByte (char): " << theChars[i] << bsl::endl;
//      }
//      else {
//          bsl::cout << "nextByte (int): "
//                    << static_cast<int>(theChars[i])
//                    << bsl::endl;
//      }
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
// See the 'bslx_genericbyteinstream' component usage example for a more
// practical example of using 'bslx' streams.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLX_OUTSTREAMFUNCTIONS
#include <bslx_outstreamfunctions.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PERFORMANCEHINT
#include <bsls_performancehint.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_CSTDDEF
#include <bsl_cstddef.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {
namespace bslx {

                      // ==========================
                      // class GenericByteOutStream
                      // ==========================

template <class STREAMBUF>
class GenericByteOutStream {
    // This class provides output methods to externalize values, and C-style
    // arrays of values, of the fundamental integral and floating-point types,
    // as well as 'bsl::string' values.  In particular, each 'put' method of
    // this class is guaranteed to write stream data that can be read by the
    // corresponding 'get' method of 'bslx::GenericByteInStream'.  See the
    // 'bslx' package-level documentation for the definition of the BDEX
    // 'OutStream' protocol.

    // PRIVATE TYPES
    enum {
        // Enumerate the platform-independent sizes (in bytes) of data types in
        // wire format.  Note that the wire format size may differ from the
        // size in memory.

        k_SIZEOF_INT64   = 8,
        k_SIZEOF_INT56   = 7,
        k_SIZEOF_INT48   = 6,
        k_SIZEOF_INT40   = 5,
        k_SIZEOF_INT32   = 4,
        k_SIZEOF_INT24   = 3,
        k_SIZEOF_INT16   = 2,
        k_SIZEOF_INT8    = 1,
        k_SIZEOF_FLOAT64 = 8,
        k_SIZEOF_FLOAT32 = 4
    };

    // DATA
    STREAMBUF *d_streamBuf;        // held stream to write to

    int        d_versionSelector;  // 'versionSelector' to use with
                                   // 'operator<<' as per the 'bslx'
                                   // package-level documentation

    int        d_validFlag;        // stream validity flag; 'true' if stream is
                                   // in valid state, 'false' otherwise

    // NOT IMPLEMENTED
    GenericByteOutStream(const GenericByteOutStream&);
    GenericByteOutStream& operator=(const GenericByteOutStream&);

  private:
    // PRIVATE MANIPULATORS
    void validate();
        // Put this output stream into a valid state.  This function has no
        // effect if this stream is already valid.

  public:
    // CREATORS
    GenericByteOutStream(STREAMBUF *streamBuf, int versionSelector);
        // Create an output byte stream that writes its output to the specified
        // 'streamBuf' and uses the specified (*compile*-time-defined)
        // 'versionSelector' as needed (see {Versioning}).  Note that the
        // 'versionSelector' is expected to be formatted as "YYYYMMDD", a date
        // representation.

    ~GenericByteOutStream();
        // Destroy this object.

    // MANIPULATORS
    GenericByteOutStream& flush();
        // If this stream is valid, invoke the 'pubsync' method on the
        // underlying stream supplied at construction of this object;
        // otherwise, this function has no effect.

    void invalidate();
        // Put this output stream in an invalid state.  This function has no
        // effect if this stream is already invalid.

    GenericByteOutStream& putLength(int length);
        // If the specified 'length' is less than 128, write to the stream
        // supplied at construction the one-byte integer comprised of the
        // least-significant one byte of the 'length'; otherwise, write to the
        // stream the four-byte, two's complement integer (in network byte
        // order) comprised of the least-significant four bytes of the 'length'
        // (in host byte order) with the most-significant bit set.  Return a
        // reference to this stream.  If this stream is initially invalid, this
        // operation has no effect.  The behavior is undefined unless
        // '0 <= length'.

    GenericByteOutStream& putVersion(int version);
        // Write to the stream supplied at construction the one-byte, two's
        // complement unsigned integer comprised of the least-significant one
        // byte of the specified 'version', and return a reference to this
        // stream.  If this stream is initially invalid, this operation has no
        // effect.

                      // *** scalar integer values ***

    GenericByteOutStream& putInt64(bsls::Types::Int64 value);
        // Write to the stream supplied at construction the eight-byte, two's
        // complement integer (in network byte order) comprised of the
        // least-significant eight bytes of the specified 'value' (in host byte
        // order), and return a reference to this stream.  If this stream is
        // initially invalid, this operation has no effect.

    GenericByteOutStream& putUint64(bsls::Types::Uint64 value);
        // Write to the stream supplied at construction the eight-byte, two's
        // complement unsigned integer (in network byte order) comprised of the
        // least-significant eight bytes of the specified 'value' (in host byte
        // order), and return a reference to this stream.  If this stream is
        // initially invalid, this operation has no effect.

    GenericByteOutStream& putInt56(bsls::Types::Int64 value);
        // Write to the stream supplied at construction the seven-byte, two's
        // complement integer (in network byte order) comprised of the
        // least-significant seven bytes of the specified 'value' (in host byte
        // order), and return a reference to this stream.  If this stream is
        // initially invalid, this operation has no effect.

    GenericByteOutStream& putUint56(bsls::Types::Uint64 value);
        // Write to the stream supplied at construction the seven-byte, two's
        // complement unsigned integer (in network byte order) comprised of the
        // least-significant seven bytes of the specified 'value' (in host byte
        // order), and return a reference to this stream.  If this stream is
        // initially invalid, this operation has no effect.

    GenericByteOutStream& putInt48(bsls::Types::Int64 value);
        // Write to the stream supplied at construction the six-byte, two's
        // complement integer (in network byte order) comprised of the
        // least-significant six bytes of the specified 'value' (in host byte
        // order), and return a reference to this stream.  If this stream is
        // initially invalid, this operation has no effect.

    GenericByteOutStream& putUint48(bsls::Types::Uint64 value);
        // Write to the stream supplied at construction the six-byte, two's
        // complement unsigned integer (in network byte order) comprised of the
        // least-significant six bytes of the specified 'value' (in host byte
        // order), and return a reference to this stream.  If this stream is
        // initially invalid, this operation has no effect.

    GenericByteOutStream& putInt40(bsls::Types::Int64 value);
        // Write to the stream supplied at construction the five-byte, two's
        // complement integer (in network byte order) comprised of the
        // least-significant five bytes of the specified 'value' (in host byte
        // order), and return a reference to this stream.  If this stream is
        // initially invalid, this operation has no effect.

    GenericByteOutStream& putUint40(bsls::Types::Uint64 value);
        // Write to the stream supplied at construction the five-byte, two's
        // complement unsigned integer (in network byte order) comprised of the
        // least-significant five bytes of the specified 'value' (in host byte
        // order), and return a reference to this stream.  If this stream is
        // initially invalid, this operation has no effect.

    GenericByteOutStream& putInt32(int value);
        // Write to the stream supplied at construction the four-byte, two's
        // complement integer (in network byte order) comprised of the
        // least-significant four bytes of the specified 'value' (in host byte
        // order), and return a reference to this stream.  If this stream is
        // initially invalid, this operation has no effect.

    GenericByteOutStream& putUint32(unsigned int value);
        // Write to the stream supplied at construction the four-byte, two's
        // complement unsigned integer (in network byte order) comprised of the
        // least-significant four bytes of the specified 'value' (in host byte
        // order), and return a reference to this stream.  If this stream is
        // initially invalid, this operation has no effect.

    GenericByteOutStream& putInt24(int value);
        // Write to the stream supplied at construction the three-byte, two's
        // complement integer (in network byte order) comprised of the
        // least-significant three bytes of the specified 'value' (in host byte
        // order), and return a reference to this stream.  If this stream is
        // initially invalid, this operation has no effect.

    GenericByteOutStream& putUint24(unsigned int value);
        // Write to the stream supplied at construction the three-byte, two's
        // complement unsigned integer (in network byte order) comprised of the
        // least-significant three bytes of the specified 'value' (in host byte
        // order), and return a reference to this stream.  If this stream is
        // initially invalid, this operation has no effect.

    GenericByteOutStream& putInt16(int value);
        // Write to the stream supplied at construction the two-byte, two's
        // complement integer (in network byte order) comprised of the
        // least-significant two bytes of the specified 'value' (in host byte
        // order), and return a reference to this stream.  If this stream is
        // initially invalid, this operation has no effect.

    GenericByteOutStream& putUint16(unsigned int value);
        // Write to the stream supplied at construction the two-byte, two's
        // complement unsigned integer (in network byte order) comprised of the
        // least-significant two bytes of the specified 'value' (in host byte
        // order), and return a reference to this stream.  If this stream is
        // initially invalid, this operation has no effect.

    GenericByteOutStream& putInt8(int value);
        // Write to the stream supplied at construction the one-byte, two's
        // complement integer comprised of the least-significant one byte of
        // the specified 'value', and return a reference to this stream.  If
        // this stream is initially invalid, this operation has no effect.

    GenericByteOutStream& putUint8(unsigned int value);
        // Write to the stream supplied at construction the one-byte, two's
        // complement unsigned integer comprised of the least-significant one
        // byte of the specified 'value', and return a reference to this
        // stream.  If this stream is initially invalid, this operation has no
        // effect.

                      // *** scalar floating-point values ***

    GenericByteOutStream& putFloat64(double value);
        // Write to the stream supplied at construction the eight-byte IEEE
        // double-precision floating-point number (in network byte order)
        // comprised of the most-significant eight bytes of the specified
        // 'value' (in host byte order), and return a reference to this stream.
        // If this stream is initially invalid, this operation has no effect.
        // Note that for non-conforming platforms, this operation may be lossy.

    GenericByteOutStream& putFloat32(float value);
        // Write to the stream supplied at construction the four-byte IEEE
        // single-precision floating-point number (in network byte order)
        // comprised of the most-significant four bytes of the specified
        // 'value' (in host byte order), and return a reference to this stream.
        // If this stream is initially invalid, this operation has no effect.
        // Note that for non-conforming platforms, this operation may be lossy.

                      // *** string values ***

    GenericByteOutStream& putString(const bsl::string& value);
        // Write to the stream supplied at construction the length of the
        // specified 'value' (see 'putLength') and an array of one-byte, two's
        // complement unsigned integers comprised of the least-significant one
        // byte of each character in the 'value', and return a reference to
        // this stream.  If this stream is initially invalid, this operation
        // has no effect.

                      // *** arrays of integer values ***

    GenericByteOutStream& putArrayInt64(const bsls::Types::Int64 *values,
                                        int                       numValues);
        // Write to the stream supplied at construction the consecutive
        // eight-byte, two's complement integers (in network byte order)
        // comprised of the least-significant eight bytes of each of the
        // specified 'numValues' leading entries in the specified 'values' (in
        // host byte order), and return a reference to this stream.  If this
        // stream is initially invalid, this operation has no effect.  The
        // behavior is undefined unless '0 <= numValues' and 'values' has
        // sufficient contents.

    GenericByteOutStream& putArrayUint64(const bsls::Types::Uint64 *values,
                                         int                        numValues);
        // Write to the stream supplied at construction the consecutive
        // eight-byte, two's complement unsigned integers (in network byte
        // order) comprised of the least-significant eight bytes of each of the
        // specified 'numValues' leading entries in the specified 'values' (in
        // host byte order), and return a reference to this stream.  If this
        // stream is initially invalid, this operation has no effect.  The
        // behavior is undefined unless '0 <= numValues' and 'values' has
        // sufficient contents.

    GenericByteOutStream& putArrayInt56(const bsls::Types::Int64 *values,
                                        int                       numValues);
        // Write to the stream supplied at construction the consecutive
        // seven-byte, two's complement integers (in network byte order)
        // comprised of the least-significant seven bytes of each of the
        // specified 'numValues' leading entries in the specified 'values' (in
        // host byte order), and return a reference to this stream.  If this
        // stream is initially invalid, this operation has no effect.  The
        // behavior is undefined unless '0 <= numValues' and 'values' has
        // sufficient contents.

    GenericByteOutStream& putArrayUint56(const bsls::Types::Uint64 *values,
                                         int                        numValues);
        // Write to the stream supplied at construction the consecutive
        // seven-byte, two's complement unsigned integers (in network byte
        // order) comprised of the least-significant seven bytes of each of the
        // specified 'numValues' leading entries in the specified 'values' (in
        // host byte order), and return a reference to this stream.  If this
        // stream is initially invalid, this operation has no effect.  The
        // behavior is undefined unless '0 <= numValues' and 'values' has
        // sufficient contents.

    GenericByteOutStream& putArrayInt48(const bsls::Types::Int64 *values,
                                        int                       numValues);
        // Write to the stream supplied at construction the consecutive
        // six-byte, two's complement integers (in network byte order)
        // comprised of the least-significant six bytes of each of the
        // specified 'numValues' leading entries in the specified 'values' (in
        // host byte order), and return a reference to this stream.  If this
        // stream is initially invalid, this operation has no effect.  The
        // behavior is undefined unless '0 <= numValues' and 'values' has
        // sufficient contents.

    GenericByteOutStream& putArrayUint48(const bsls::Types::Uint64 *values,
                                         int                        numValues);
        // Write to the stream supplied at construction the consecutive
        // six-byte, two's complement unsigned integers (in network byte order)
        // comprised of the least-significant six bytes of each of the
        // specified 'numValues' leading entries in the specified 'values' (in
        // host byte order), and return a reference to this stream.  If this
        // stream is initially invalid, this operation has no effect.  The
        // behavior is undefined unless '0 <= numValues' and 'values' has
        // sufficient contents.

    GenericByteOutStream& putArrayInt40(const bsls::Types::Int64 *values,
                                        int                       numValues);
        // Write to the stream supplied at construction the consecutive
        // five-byte, two's complement integers (in network byte order)
        // comprised of the least-significant five bytes of each of the
        // specified 'numValues' leading entries in the specified 'values' (in
        // host byte order), and return a reference to this stream.  If this
        // stream is initially invalid, this operation has no effect.  The
        // behavior is undefined unless '0 <= numValues' and 'values' has
        // sufficient contents.

    GenericByteOutStream& putArrayUint40(const bsls::Types::Uint64 *values,
                                         int                        numValues);
        // Write to the stream supplied at construction the consecutive
        // five-byte, two's complement unsigned integers (in network byte
        // order) comprised of the least-significant five bytes of each of the
        // specified 'numValues' leading entries in the specified 'values' (in
        // host byte order), and return a reference to this stream.  If this
        // stream is initially invalid, this operation has no effect.  The
        // behavior is undefined unless '0 <= numValues' and 'values' has
        // sufficient contents.

    GenericByteOutStream& putArrayInt32(const int *values, int numValues);
        // Write to the stream supplied at construction the consecutive
        // four-byte, two's complement integers (in network byte order)
        // comprised of the least-significant four bytes of each of the
        // specified 'numValues' leading entries in the specified 'values' (in
        // host byte order), and return a reference to this stream.  If this
        // stream is initially invalid, this operation has no effect.  The
        // behavior is undefined unless '0 <= numValues' and 'values' has
        // sufficient contents.

    GenericByteOutStream& putArrayUint32(const unsigned int *values,
                                         int                 numValues);
        // Write to the stream supplied at construction the consecutive
        // four-byte, two's complement unsigned integers (in network byte
        // order) comprised of the least-significant four bytes of each of the
        // specified 'numValues' leading entries in the specified 'values' (in
        // host byte order), and return a reference to this stream.  If this
        // stream is initially invalid, this operation has no effect.  The
        // behavior is undefined unless '0 <= numValues' and 'values' has
        // sufficient contents.

    GenericByteOutStream& putArrayInt24(const int *values, int numValues);
        // Write to the stream supplied at construction the consecutive
        // three-byte, two's complement integers (in network byte order)
        // comprised of the least-significant three bytes of each of the
        // specified 'numValues' leading entries in the specified 'values' (in
        // host byte order), and return a reference to this stream.  If this
        // stream is initially invalid, this operation has no effect.  The
        // behavior is undefined unless '0 <= numValues' and 'values' has
        // sufficient contents.

    GenericByteOutStream& putArrayUint24(const unsigned int *values,
                                         int                 numValues);
        // Write to the stream supplied at construction the consecutive
        // three-byte, two's complement unsigned integers (in network byte
        // order) comprised of the least-significant three bytes of each of the
        // specified 'numValues' leading entries in the specified 'values' (in
        // host byte order), and return a reference to this stream.  If this
        // stream is initially invalid, this operation has no effect.  The
        // behavior is undefined unless '0 <= numValues' and 'values' has
        // sufficient contents.

    GenericByteOutStream& putArrayInt16(const short *values, int numValues);
        // Write to the stream supplied at construction the consecutive
        // two-byte, two's complement integers (in network byte order)
        // comprised of the least-significant two bytes of each of the
        // specified 'numValues' leading entries in the specified 'values' (in
        // host byte order), and return a reference to this stream.  If this
        // stream is initially invalid, this operation has no effect.  The
        // behavior is undefined unless '0 <= numValues' and 'values' has
        // sufficient contents.

    GenericByteOutStream& putArrayUint16(const unsigned short *values,
                                         int                   numValues);
        // Write to the stream supplied at construction the consecutive
        // two-byte, two's complement unsigned integers (in network byte order)
        // comprised of the least-significant two bytes of each of the
        // specified 'numValues' leading entries in the specified 'values' (in
        // host byte order), and return a reference to this stream.  If this
        // stream is initially invalid, this operation has no effect.  The
        // behavior is undefined unless '0 <= numValues' and 'values' has
        // sufficient contents.

    GenericByteOutStream& putArrayInt8(const char        *values,
                                       int                numValues);
    GenericByteOutStream& putArrayInt8(const signed char *values,
                                       int                numValues);
        // Write to the stream supplied at construction the consecutive
        // one-byte, two's complement integers comprised of the
        // least-significant one byte of each of the specified 'numValues'
        // leading entries in the specified 'values', and return a reference to
        // this stream.  If this stream is initially invalid, this operation
        // has no effect.  The behavior is undefined unless '0 <= numValues'
        // and 'values' has sufficient contents.

    GenericByteOutStream& putArrayUint8(const char          *values,
                                        int                  numValues);
    GenericByteOutStream& putArrayUint8(const unsigned char *values,
                                        int                  numValues);
        // Write to the stream supplied at construction the consecutive
        // one-byte, two's complement unsigned integers comprised of the
        // least-significant one byte of each of the specified 'numValues'
        // leading entries in the specified 'values', and return a reference to
        // this stream.  If this stream is initially invalid, this operation
        // has no effect.  The behavior is undefined unless '0 <= numValues'
        // and 'values' has sufficient contents.

                      // *** arrays of floating-point values ***

    GenericByteOutStream& putArrayFloat64(const double *values, int numValues);
        // Write to the stream supplied at construction the consecutive
        // eight-byte IEEE double-precision floating-point numbers (in network
        // byte order) comprised of the most-significant eight bytes of each of
        // the specified 'numValues' leading entries in the specified 'values'
        // (in host byte order), and return a reference to this stream.  If
        // this stream is initially invalid, this operation has no effect.  The
        // behavior is undefined unless '0 <= numValues' and 'values' has
        // sufficient contents.  Note that for non-conforming platforms, this
        // operation may be lossy.

    GenericByteOutStream& putArrayFloat32(const float *values, int numValues);
        // Write to the stream supplied at construction the consecutive
        // four-byte IEEE single-precision floating-point numbers (in network
        // byte order) comprised of the most-significant four bytes of each of
        // the specified 'numValues' leading entries in the specified 'values'
        // (in host byte order), and return a reference to this stream.  If
        // this stream is initially invalid, this operation has no effect.  The
        // behavior is undefined unless '0 <= numValues' and 'values' has
        // sufficient contents.  Note that for non-conforming platforms, this
        // operation may be lossy.

    // ACCESSORS
    operator const void *() const;
        // Return a non-zero value if this stream is valid, and 0 otherwise.
        // An invalid stream is a stream for which an output operation was
        // detected to have failed or 'invalidate' was called.

    int bdexVersionSelector() const;
        // Return the 'versionSelector' to be used with 'operator<<' for BDEX
        // streaming as per the 'bslx' package-level documentation.

    bool isValid() const;
        // Return 'true' if this stream is valid, and 'false' otherwise.  An
        // invalid stream is a stream for which an output operation was
        // detected to have failed or 'invalidate' was called.
};

// FREE OPERATORS
template <class STREAMBUF, class TYPE>
GenericByteOutStream<STREAMBUF>&
operator<<(GenericByteOutStream<STREAMBUF>& stream, const TYPE& value);
    // Write the specified 'value' to the specified output 'stream' following
    // the requirements of the BDEX protocol (see the 'bslx' package-level
    // documentation), and return a reference to 'stream'.  The behavior is
    // undefined unless 'TYPE' is BDEX-compliant.

// ============================================================================
//                          INLINE DEFINITIONS
// ============================================================================

                      // --------------------------
                      // class GenericByteOutStream
                      // --------------------------

// PRIVATE MANIPULATORS
template <class STREAMBUF>
inline
void GenericByteOutStream<STREAMBUF>::validate()
{
    d_validFlag = true;
}

// CREATORS
template <class STREAMBUF>
inline
GenericByteOutStream<STREAMBUF>::GenericByteOutStream(
                                                    STREAMBUF *streamBuf,
                                                    int        versionSelector)
: d_streamBuf(streamBuf)
, d_versionSelector(versionSelector)
, d_validFlag(true)
{
    BSLS_ASSERT_SAFE(streamBuf);
}

template <class STREAMBUF>
inline
GenericByteOutStream<STREAMBUF>::~GenericByteOutStream()
{
}

// MANIPULATORS
template <class STREAMBUF>
inline
GenericByteOutStream<STREAMBUF>& GenericByteOutStream<STREAMBUF>::flush()
{
    if (isValid()) {
        invalidate();
        if (0 == d_streamBuf->pubsync()) {
            validate();
        }
    }
    return *this;
}

template <class STREAMBUF>
inline
void GenericByteOutStream<STREAMBUF>::invalidate()
{
    d_validFlag = false;
}

template <class STREAMBUF>
inline
GenericByteOutStream<STREAMBUF>&
GenericByteOutStream<STREAMBUF>::putLength(int length)
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

template <class STREAMBUF>
inline
GenericByteOutStream<STREAMBUF>&
GenericByteOutStream<STREAMBUF>::putVersion(int version)
{
    return putUint8(version);
}

                      // *** scalar integer values ***

template <class STREAMBUF>
inline
GenericByteOutStream<STREAMBUF>&
GenericByteOutStream<STREAMBUF>::putInt64(bsls::Types::Int64 value)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    invalidate();

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    const char *rawBytes = reinterpret_cast<char *>(&value);
    char        bytes[k_SIZEOF_INT64];

    bytes[0] = rawBytes[7];
    bytes[1] = rawBytes[6];
    bytes[2] = rawBytes[5];
    bytes[3] = rawBytes[4];
    bytes[4] = rawBytes[3];
    bytes[5] = rawBytes[2];
    bytes[6] = rawBytes[1];
    bytes[7] = rawBytes[0];
#else
    const char *bytes =
              reinterpret_cast<char *>(&value) + sizeof value - k_SIZEOF_INT64;
#endif

    if (k_SIZEOF_INT64 == d_streamBuf->sputn(bytes, k_SIZEOF_INT64)) {
        validate();
    }

    return *this;
}

template <class STREAMBUF>
inline
GenericByteOutStream<STREAMBUF>&
GenericByteOutStream<STREAMBUF>::putUint64(bsls::Types::Uint64 value)
{
    return putInt64(static_cast<bsls::Types::Int64>(value));
}

template <class STREAMBUF>
inline
GenericByteOutStream<STREAMBUF>&
GenericByteOutStream<STREAMBUF>::putInt56(bsls::Types::Int64 value)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    invalidate();

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    const char *rawBytes = reinterpret_cast<char *>(&value);
    char        bytes[k_SIZEOF_INT56];

    bytes[0] = rawBytes[6];
    bytes[1] = rawBytes[5];
    bytes[2] = rawBytes[4];
    bytes[3] = rawBytes[3];
    bytes[4] = rawBytes[2];
    bytes[5] = rawBytes[1];
    bytes[6] = rawBytes[0];
#else
    const char *bytes =
              reinterpret_cast<char *>(&value) + sizeof value - k_SIZEOF_INT56;
#endif

    if (k_SIZEOF_INT56 == d_streamBuf->sputn(bytes, k_SIZEOF_INT56)) {
        validate();
    }

    return *this;
}

template <class STREAMBUF>
inline
GenericByteOutStream<STREAMBUF>&
GenericByteOutStream<STREAMBUF>::putUint56(bsls::Types::Uint64 value)
{
    return putInt56(static_cast<bsls::Types::Int64>(value));
}

template <class STREAMBUF>
inline
GenericByteOutStream<STREAMBUF>&
GenericByteOutStream<STREAMBUF>::putInt48(bsls::Types::Int64 value)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    invalidate();

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    const char *rawBytes = reinterpret_cast<char *>(&value);
    char        bytes[k_SIZEOF_INT48];

    bytes[0] = rawBytes[5];
    bytes[1] = rawBytes[4];
    bytes[2] = rawBytes[3];
    bytes[3] = rawBytes[2];
    bytes[4] = rawBytes[1];
    bytes[5] = rawBytes[0];
#else
    const char *bytes =
              reinterpret_cast<char *>(&value) + sizeof value - k_SIZEOF_INT48;
#endif

    if (k_SIZEOF_INT48 == d_streamBuf->sputn(bytes, k_SIZEOF_INT48)) {
        validate();
    }

    return *this;
}

template <class STREAMBUF>
inline
GenericByteOutStream<STREAMBUF>&
GenericByteOutStream<STREAMBUF>::putUint48(bsls::Types::Uint64 value)
{
    return putInt48(static_cast<bsls::Types::Int64>(value));
}

template <class STREAMBUF>
inline
GenericByteOutStream<STREAMBUF>&
GenericByteOutStream<STREAMBUF>::putInt40(bsls::Types::Int64 value)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    invalidate();

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    const char *rawBytes = reinterpret_cast<char *>(&value);
    char        bytes[k_SIZEOF_INT40];

    bytes[0] = rawBytes[4];
    bytes[1] = rawBytes[3];
    bytes[2] = rawBytes[2];
    bytes[3] = rawBytes[1];
    bytes[4] = rawBytes[0];
#else
    const char *bytes =
              reinterpret_cast<char *>(&value) + sizeof value - k_SIZEOF_INT40;
#endif

    if (k_SIZEOF_INT40 == d_streamBuf->sputn(bytes, k_SIZEOF_INT40)) {
        validate();
    }

    return *this;
}

template <class STREAMBUF>
inline
GenericByteOutStream<STREAMBUF>&
GenericByteOutStream<STREAMBUF>::putUint40(bsls::Types::Uint64 value)
{
    return putInt40(static_cast<bsls::Types::Int64>(value));
}

template <class STREAMBUF>
inline
GenericByteOutStream<STREAMBUF>&
GenericByteOutStream<STREAMBUF>::putInt32(int value)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    invalidate();

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    const char *rawBytes = reinterpret_cast<char *>(&value);
    char        bytes[k_SIZEOF_INT32];

    bytes[0] = rawBytes[3];
    bytes[1] = rawBytes[2];
    bytes[2] = rawBytes[1];
    bytes[3] = rawBytes[0];
#else
    const char *bytes =
              reinterpret_cast<char *>(&value) + sizeof value - k_SIZEOF_INT32;
#endif

    if (k_SIZEOF_INT32 == d_streamBuf->sputn(bytes, k_SIZEOF_INT32)) {
        validate();
    }

    return *this;
}

template <class STREAMBUF>
inline
GenericByteOutStream<STREAMBUF>&
GenericByteOutStream<STREAMBUF>::putUint32(unsigned int value)
{
    return putInt32(static_cast<int>(value));
}

template <class STREAMBUF>
inline
GenericByteOutStream<STREAMBUF>&
GenericByteOutStream<STREAMBUF>::putInt24(int value)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    invalidate();

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    const char *rawBytes = reinterpret_cast<char *>(&value);
    char        bytes[k_SIZEOF_INT24];

    bytes[0] = rawBytes[2];
    bytes[1] = rawBytes[1];
    bytes[2] = rawBytes[0];
#else
    const char *bytes =
              reinterpret_cast<char *>(&value) + sizeof value - k_SIZEOF_INT24;
#endif

    if (k_SIZEOF_INT24 == d_streamBuf->sputn(bytes, k_SIZEOF_INT24)) {
        validate();
    }

    return *this;
}

template <class STREAMBUF>
inline
GenericByteOutStream<STREAMBUF>&
GenericByteOutStream<STREAMBUF>::putUint24(unsigned int value)
{
    return putInt24(static_cast<int>(value));
}

template <class STREAMBUF>
inline
GenericByteOutStream<STREAMBUF>&
GenericByteOutStream<STREAMBUF>::putInt16(int value)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    invalidate();

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    const char *rawBytes = reinterpret_cast<char *>(&value);
    char        bytes[k_SIZEOF_INT16];

    bytes[0] = rawBytes[1];
    bytes[1] = rawBytes[0];
#else
    const char *bytes =
              reinterpret_cast<char *>(&value) + sizeof value - k_SIZEOF_INT16;
#endif

    if (k_SIZEOF_INT16 == d_streamBuf->sputn(bytes, k_SIZEOF_INT16)) {
        validate();
    }

    return *this;
}

template <class STREAMBUF>
inline
GenericByteOutStream<STREAMBUF>&
GenericByteOutStream<STREAMBUF>::putUint16(unsigned int value)
{
    return putInt16(static_cast<int>(value));
}

template <class STREAMBUF>
inline
GenericByteOutStream<STREAMBUF>&
GenericByteOutStream<STREAMBUF>::putInt8(int value)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    invalidate();

    if (STREAMBUF::traits_type::eof() !=
                                d_streamBuf->sputc(static_cast<char>(value))) {
        validate();
    }

    return *this;
}

template <class STREAMBUF>
inline
GenericByteOutStream<STREAMBUF>&
GenericByteOutStream<STREAMBUF>::putUint8(unsigned int value)
{
    return putInt8(static_cast<int>(value));
}

                      // *** scalar floating-point values ***

template <class STREAMBUF>
inline
GenericByteOutStream<STREAMBUF>&
GenericByteOutStream<STREAMBUF>::putFloat64(double value)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    invalidate();

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    const char *rawBytes = reinterpret_cast<char *>(&value);
    char        bytes[k_SIZEOF_FLOAT64];

    bytes[0] = rawBytes[sizeof value - 1];
    bytes[1] = rawBytes[sizeof value - 2];
    bytes[2] = rawBytes[sizeof value - 3];
    bytes[3] = rawBytes[sizeof value - 4];
    bytes[4] = rawBytes[sizeof value - 5];
    bytes[5] = rawBytes[sizeof value - 6];
    bytes[6] = rawBytes[sizeof value - 7];
    bytes[7] = rawBytes[sizeof value - 8];
#else
    const char *bytes = reinterpret_cast<char *>(&value);
#endif

    if (k_SIZEOF_FLOAT64 == d_streamBuf->sputn(bytes, k_SIZEOF_FLOAT64)) {
        validate();
    }

    return *this;
}

template <class STREAMBUF>
inline
GenericByteOutStream<STREAMBUF>&
GenericByteOutStream<STREAMBUF>::putFloat32(float value)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    invalidate();

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    const char *rawBytes = reinterpret_cast<char *>(&value);
    char        bytes[k_SIZEOF_FLOAT32];

    bytes[0] = rawBytes[sizeof value - 1];
    bytes[1] = rawBytes[sizeof value - 2];
    bytes[2] = rawBytes[sizeof value - 3];
    bytes[3] = rawBytes[sizeof value - 4];
#else
    const char *bytes = reinterpret_cast<char *>(&value);
#endif

    if (k_SIZEOF_FLOAT32 == d_streamBuf->sputn(bytes, k_SIZEOF_FLOAT32)) {
        validate();
    }

    return *this;
}

                      // *** string values ***

template <class STREAMBUF>
inline
GenericByteOutStream<STREAMBUF>&
GenericByteOutStream<STREAMBUF>::putString(const bsl::string& value)
{
    putLength(static_cast<int>(value.length()));
    return putArrayUint8(value.data(), static_cast<int>(value.length()));
}

                      // *** arrays of integer values ***

template <class STREAMBUF>
GenericByteOutStream<STREAMBUF>&
GenericByteOutStream<STREAMBUF>::putArrayInt64(
                                           const bsls::Types::Int64 *values,
                                           int                       numValues)
{
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid() || 0 == numValues)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const bsls::Types::Int64 *end = values + numValues;
    for (; values != end; ++values) {
        putInt64(*values);
    }

    return *this;
}

template <class STREAMBUF>
GenericByteOutStream<STREAMBUF>&
GenericByteOutStream<STREAMBUF>::putArrayUint64(
                                          const bsls::Types::Uint64 *values,
                                          int                        numValues)
{
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid() || 0 == numValues)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const bsls::Types::Uint64 *end = values + numValues;
    for (; values != end; ++values) {
        putUint64(*values);
    }

    return *this;
}

template <class STREAMBUF>
GenericByteOutStream<STREAMBUF>&
GenericByteOutStream<STREAMBUF>::putArrayInt56(
                                           const bsls::Types::Int64 *values,
                                           int                       numValues)
{
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid() || 0 == numValues)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const bsls::Types::Int64 *end = values + numValues;
    for (; values != end; ++values) {
        putInt56(*values);
    }

    return *this;
}

template <class STREAMBUF>
GenericByteOutStream<STREAMBUF>&
GenericByteOutStream<STREAMBUF>::putArrayUint56(
                                          const bsls::Types::Uint64 *values,
                                          int                        numValues)
{
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid() || 0 == numValues)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const bsls::Types::Uint64 *end = values + numValues;
    for (; values != end; ++values) {
        putUint56(*values);
    }

    return *this;
}

template <class STREAMBUF>
GenericByteOutStream<STREAMBUF>&
GenericByteOutStream<STREAMBUF>::putArrayInt48(
                                           const bsls::Types::Int64 *values,
                                           int                       numValues)
{
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid() || 0 == numValues)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const bsls::Types::Int64 *end = values + numValues;
    for (; values != end; ++values) {
        putInt48(*values);
    }

    return *this;
}

template <class STREAMBUF>
GenericByteOutStream<STREAMBUF>&
GenericByteOutStream<STREAMBUF>::putArrayUint48(
                                          const bsls::Types::Uint64 *values,
                                          int                        numValues)
{
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid() || 0 == numValues)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const bsls::Types::Uint64 *end = values + numValues;
    for (; values != end; ++values) {
        putUint48(*values);
    }

    return *this;
}

template <class STREAMBUF>
GenericByteOutStream<STREAMBUF>&
GenericByteOutStream<STREAMBUF>::putArrayInt40(
                                           const bsls::Types::Int64 *values,
                                           int                       numValues)
{
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid() || 0 == numValues)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const bsls::Types::Int64 *end = values + numValues;
    for (; values != end; ++values) {
        putInt40(*values);
    }

    return *this;
}

template <class STREAMBUF>
GenericByteOutStream<STREAMBUF>&
GenericByteOutStream<STREAMBUF>::putArrayUint40(
                                          const bsls::Types::Uint64 *values,
                                          int                        numValues)
{
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid() || 0 == numValues)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const bsls::Types::Uint64 *end = values + numValues;
    for (; values != end; ++values) {
        putUint40(*values);
    }

    return *this;
}

template <class STREAMBUF>
GenericByteOutStream<STREAMBUF>&
GenericByteOutStream<STREAMBUF>::putArrayInt32(const int *values,
                                               int        numValues)
{
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid() || 0 == numValues)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const int *end = values + numValues;
    for (; values != end; ++values) {
        putInt32(*values);
    }

    return *this;
}

template <class STREAMBUF>
GenericByteOutStream<STREAMBUF>&
GenericByteOutStream<STREAMBUF>::putArrayUint32(const unsigned int *values,
                                                int                 numValues)
{
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid() || 0 == numValues)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const unsigned int *end = values + numValues;
    for (; values != end; ++values) {
        putUint32(*values);
    }

    return *this;
}

template <class STREAMBUF>
GenericByteOutStream<STREAMBUF>&
GenericByteOutStream<STREAMBUF>::putArrayInt24(const int *values,
                                               int        numValues)
{
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid() || 0 == numValues)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const int *end = values + numValues;
    for (; values != end; ++values) {
        putInt24(*values);
    }

    return *this;
}

template <class STREAMBUF>
GenericByteOutStream<STREAMBUF>&
GenericByteOutStream<STREAMBUF>::putArrayUint24(const unsigned int *values,
                                                int                 numValues)
{
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid() || 0 == numValues)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const unsigned int *end = values + numValues;
    for (; values != end; ++values) {
        putUint24(*values);
    }

    return *this;
}

template <class STREAMBUF>
GenericByteOutStream<STREAMBUF>&
GenericByteOutStream<STREAMBUF>::putArrayInt16(const short *values,
                                               int          numValues)
{
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid() || 0 == numValues)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const short *end = values + numValues;
    for (; values != end; ++values) {
        putInt16(*values);
    }

    return *this;
}

template <class STREAMBUF>
GenericByteOutStream<STREAMBUF>&
GenericByteOutStream<STREAMBUF>::putArrayUint16(
                                               const unsigned short *values,
                                               int                   numValues)
{
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid() || 0 == numValues)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const unsigned short *end = values + numValues;
    for (; values != end; ++values) {
        putUint16(*values);
    }

    return *this;
}

template <class STREAMBUF>
inline
GenericByteOutStream<STREAMBUF>&
GenericByteOutStream<STREAMBUF>::putArrayInt8(const char *values,
                                              int         numValues)
{
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid() || 0 == numValues)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    invalidate();

    if (numValues == d_streamBuf->sputn(values, numValues)) {
        validate();
    }

    return *this;
}

template <class STREAMBUF>
inline
GenericByteOutStream<STREAMBUF>&
GenericByteOutStream<STREAMBUF>::putArrayInt8(const signed char *values,
                                              int                numValues)
{
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid() || 0 == numValues)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    invalidate();

    if (numValues == d_streamBuf->sputn(reinterpret_cast<const char *>(values),
                                        numValues)) {
        validate();
    }

    return *this;
}

template <class STREAMBUF>
inline
GenericByteOutStream<STREAMBUF>&
GenericByteOutStream<STREAMBUF>::putArrayUint8(const char *values,
                                               int         numValues)
{
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid() || 0 == numValues)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    invalidate();

    if (numValues == d_streamBuf->sputn(values, numValues)) {
        validate();
    }

    return *this;
}

template <class STREAMBUF>
inline
GenericByteOutStream<STREAMBUF>&
GenericByteOutStream<STREAMBUF>::putArrayUint8(const unsigned char *values,
                                               int                  numValues)
{
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid() || 0 == numValues)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    invalidate();

    if (numValues == d_streamBuf->sputn(reinterpret_cast<const char *>(values),
                                        numValues)) {
        validate();
    }

    return *this;
}

                      // *** arrays of floating-point values ***

template <class STREAMBUF>
GenericByteOutStream<STREAMBUF>&
GenericByteOutStream<STREAMBUF>::putArrayFloat64(const double *values,
                                                 int           numValues)
{
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid() || 0 == numValues)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const double *end = values + numValues;
    for (; values != end; ++values) {
        putFloat64(*values);
    }

    return *this;
}

template <class STREAMBUF>
GenericByteOutStream<STREAMBUF>&
GenericByteOutStream<STREAMBUF>::putArrayFloat32(const float *values,
                                                 int          numValues)
{
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid() || 0 == numValues)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const float *end = values + numValues;
    for (; values != end; ++values) {
        putFloat32(*values);
    }

    return *this;
}

// ACCESSORS
template <class STREAMBUF>
inline
GenericByteOutStream<STREAMBUF>::operator const void *() const
{
    return isValid() ? this : 0;
}

template <class STREAMBUF>
inline
int GenericByteOutStream<STREAMBUF>::bdexVersionSelector() const
{
    return d_versionSelector;
}

template <class STREAMBUF>
inline
bool GenericByteOutStream<STREAMBUF>::isValid() const
{
    return d_validFlag;
}

// FREE OPERATORS
template <class STREAMBUF, class TYPE>
inline
GenericByteOutStream<STREAMBUF>&
operator<<(GenericByteOutStream<STREAMBUF>& stream, const TYPE& value)
{
    return OutStreamFunctions::bdexStreamOut(stream, value);
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2014 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
