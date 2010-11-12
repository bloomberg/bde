// bdex_byteoutstreamraw.h                                            -*-C++-*-
#ifndef INCLUDED_BDEX_BYTEOUTSTREAMRAW
#define INCLUDED_BDEX_BYTEOUTSTREAMRAW

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Enable externalization of fundamental types to byte representation.
//
//@CLASSES:
//   bdex_ByteOutStreamRaw: byte-array-based output stream class
//
//@AUTHOR: Shao-wei Hung
//
//@SEE_ALSO: bdex_byteinstreamraw, bdex_byteinstream
//
//@DESCRIPTION: This component conforms to the 'bdex_outstream' (documentation
// only) "protocol", implementing a byte-array-based output stream class that
// provides platform-independent output methods ("externalization") on values,
// and arrays of values, of fundamental types, and on 'bsl::string'.
//
///Usage Warning
///-------------
// This "raw" component writes to a user-supplied, unmanaged buffer, and must
// therefore be used with extreme caution, and even then only where the highest
// possible performance is needed *and* where the considerable risk of writing
// past the end of the raw buffer can be borne (and safely managed).
//
///Interoperability of Raw Streams
///-------------------------------
// Although the general design idea within 'bdex' is that in-streams and
// out-streams must be used in "matched pairs" (e.g., the output buffer of
// 'bdex_TestOutStream' can be read successfully only by 'bdex_TestInStream'),
// the "raw" streams ('bdex_ByteOutStreamRaw' and 'bdex_ByteInStreamRaw') and
// the "production" streams ('bdex_ByteOutStream' and 'bdex_ByteInStream') all
// use the same byte format and are designed to interoperate.
//
// The raw streams have in common that they do not manage or own the buffers
// that they use, but rather just copy pointers.  The ramifications of this
// design choice are rather different for input and output streams (see the
// specific Usage Warning section above), however, and therefore the user may
// wish to "mix and match" streams, depending upon that user's specific needs
// and risk tolerances.
//
// Specifically, this component may be used in conjunction with either the
// 'bdex_byteinstreamraw' or the 'bdex_byteinstream' "unexternalization"
// components, at the user's choice.  Each output method of
// 'bdex_ByteOutStreamRaw' writes a value or a homogeneous array of values to a
// user-supplied buffer.  That buffer may then be supplied at construction to
// either of the above-mentioned input streams; the values are formatted to be
// readable by the corresponding methods of those input streams.  In general,
// the user cannot rely on any other mechanism to read data written by
// 'bdex_ByteOutStreamRaw' unless that mechanism explicitly states its ability
// to do so.
//
///Supported Types
///---------------
// The supported types and required content are listed in the table below.
// All of the fundamental types in the table may be output as scalar values or
// as homogeneous arrays.  'bsl::string' is output as a logical scalar string.
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
// format.  Also note that a 'bdex_ByteOutStream' can neither be invalidated
// nor queried for validity.
//
// Note that output streams can be *invalidated* explicitly and queried for
// *validity*.  Writing to an initially invalid stream has no effect.  Whenever
// an output operation fails, the stream should be invalidated explicitly.
//
///EXTERNALIZATION AND VALUE SEMANTICS
///- - - - - - - - - - - - - - - - - -
// The concept of "externalization" in this component is intentionally
// specialized to support streaming the *values* of entities atomically.  In
// particular, streaming an array of a specific length is considered to be an
// atomic operation.  It is therefore an error, for example, to stream out an
// array of 'int' of length three and then stream in those three 'int' values
// as three scalar 'int' variables.  Similarly, it is an error to stream out an
// array of length L and stream back in an array of length other than L.
//
///Usage
///-----
// The 'bdex_ByteOutStreamRaw' implementation of the 'bdex_OutStream' protocol
// can be used to externalize values in a platform-neutral way.  This
// serialization type requires the client to provide an underlying buffer, in
// the form of a character array.
//
// Writing out fundamental C++ types requires no additional work on the part
// of the client; the client can simply use the stream directly.  The
// following code serializes some fundamental types using a
// 'bdex_ByteOutStreamRaw', and then visually investigates the contents of the
// stream:
//..
// int main(int argc, char **argv) {
//
//    // Create a stream and write out some values.
//    char bigBuffer[10000];
//    bdex_ByteOutStreamRaw outStream(bigBuffer, 10000);
//    bdex_OutStreamFunctions::streamOut(outStream, 1, 0);
//    bdex_OutStreamFunctions::streamOut(outStream, 2, 0);
//    bdex_OutStreamFunctions::streamOut(outStream, 'c', 0);
//    bdex_OutStreamFunctions::streamOut(outStream, bsl::string("hello"), 0);
//
//    // Verify the results on 'stdout'.
//    const char *theChars = outStream.data();
//    int length = outStream.length();
//    for(int i = 0; i < length; ++i) {
//        if(bsl::isalnum(static_cast<unsigned char>(theChars[i]))) {
//            cout << "nextByte (char): " << theChars[i] << endl;
//        }
//        else {
//            cout << "nextByte (int): " << (int)theChars[i] << endl;
//        }
//    }
//..
// Executing the above code results in the following output:
//..
// nextByte (int): 0
// nextByte (int): 0
// nextByte (int): 0
// nextByte (int): 1
// nextByte (int): 0
// nextByte (int): 0
// nextByte (int): 0
// nextByte (int): 2
// nextByte (char): c
// nextByte (int): 5
// nextByte (char): h
// nextByte (char): e
// nextByte (char): l
// nextByte (char): l
// nextByte (char): o
//..
// Note the numeric value 5 prepended to the string literal "hello".  This is
// because 'const char *' types are picked up by the template specialization
// for type bsl::string, which outputs the string length before emitting the
// characters.
//
// See the 'bdex_byteinstreamraw' component usage example for a more practical
// example of using 'bdex' streams with a value-semantic type.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEX_GENERICBYTEOUTSTREAM
#include <bdex_genericbyteoutstream.h>
#endif

#ifndef INCLUDED_BDEX_OUTSTREAMMETHODS
#include <bdex_outstreammethods.h>
#endif

#ifndef INCLUDED_BDESB_FIXEDMEMOUTPUT
#include <bdesb_fixedmemoutput.h>
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

                       // ===========================
                       // class bdex_ByteOutStreamRaw
                       // ===========================

class bdex_ByteOutStreamRaw {
    // This class provides output methods to externalize fundamental types
    // in byte representation and stores the accumulated result in network
    // byte order format.

    // PRIVATE TYPES
    enum {
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

    // IMPORTANT: d_streambuf and d_formatter must be initialized in the order
    // declared below.  Don't re-arrange these declarations!
    // DATA
    bdesb_FixedMemOutput                            d_streambuf; // streamed
                                                                 // values
    bdex_GenericByteOutStream<bdesb_FixedMemOutput> d_formatter; // formatter

    friend bsl::ostream& operator<<(bsl::ostream&                stream,
                                    const bdex_ByteOutStreamRaw& object);

  public:
    // CREATORS
    bdex_ByteOutStreamRaw(char *buffer, int length = 0x7fffffff);

    ~bdex_ByteOutStreamRaw();

    // MANIPULATORS
    void invalidate();
        // Put this output stream in an invalid state.  This function has no
        // effect if this stream is already invalid.  Note that this function
        // should be called whenever a write operation to this stream fails.

    void reset(char *buffer, int length = 0x7fffffff);

    void setCursor(int index);

    bdex_ByteOutStreamRaw& putLength(int length);
        // Format the specified 'length' as a 8-bit unsigned integer if
        // 'length' is less than 128, otherwise as a 32-bit integer to this
        // output stream with its most significant bit set and return a
        // reference to this modifiable stream.

    bdex_ByteOutStreamRaw& putString(const bsl::string& str);
        // Format as an array of 8-bit integers the specified 'str' to this
        // output stream and return a reference to this modifiable stream.

    bdex_ByteOutStreamRaw& putVersion(int version);
        // Format the specified 'version' as an 8-bit unsigned integer to this
        // output stream and return a reference to this modifiable stream.

// TBD #if !defined(BSL_LEGACY) || 1 == BSL_LEGACY

    template <typename TYPE>
    bdex_ByteOutStreamRaw& put(const TYPE& variable);
        // TBD

// TBD #endif

                        // *** scalar integer values ***

    bdex_ByteOutStreamRaw& putInt64(bsls_PlatformUtil::Int64 value);
        // Format as a signed integer the least significant 64 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    bdex_ByteOutStreamRaw& putUint64(bsls_PlatformUtil::Int64 value);
        // Format as an unsigned integer the least significant 64 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    bdex_ByteOutStreamRaw& putInt56(bsls_PlatformUtil::Int64 value);
        // Format as a signed integer the least significant 56 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    bdex_ByteOutStreamRaw& putUint56(bsls_PlatformUtil::Int64 value);
        // Format as an unsigned integer the least significant 56 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    bdex_ByteOutStreamRaw& putInt48(bsls_PlatformUtil::Int64 value);
        // Format as a signed integer the least significant 48 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    bdex_ByteOutStreamRaw& putUint48(bsls_PlatformUtil::Int64 value);
        // Format as an unsigned integer the least significant 48 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    bdex_ByteOutStreamRaw& putInt40(bsls_PlatformUtil::Int64 value);
        // Format as a signed integer the least significant 40 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    bdex_ByteOutStreamRaw& putUint40(bsls_PlatformUtil::Int64 value);
        // Format as an unsigned integer the least significant 40 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    bdex_ByteOutStreamRaw& putInt32(int value);
        // Format as a signed integer the least significant 32 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    bdex_ByteOutStreamRaw& putUint32(unsigned int value);
        // Format as an unsigned integer the least significant 32 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    bdex_ByteOutStreamRaw& putInt24(int value);
        // Format as a signed integer the least significant 24 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    bdex_ByteOutStreamRaw& putUint24(int value);
        // Format as an unsigned integer the least significant 24 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    bdex_ByteOutStreamRaw& putInt16(int value);
        // Format as a signed integer the least significant 16 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    bdex_ByteOutStreamRaw& putUint16(int value);
        // Format as an unsigned integer the least significant 16 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    bdex_ByteOutStreamRaw& putInt8(int value);
        // Format as a signed integer the least significant 8 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    bdex_ByteOutStreamRaw& putUint8(int value);
        // Format as an unsigned integer the least significant 8 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

                        // *** scalar floating-point values ***

    bdex_ByteOutStreamRaw& putFloat64(double value);
        // Format as an IEEE double-precision floating point number the most
        // significant 8 bytes in the specified 'value' to this output stream
        // and return a reference to this modifiable stream.  Note that, for
        // non-conforming platforms, the "most significant 8 bytes" might not
        // be contiguous bits, and that this operation may be lossy.

    bdex_ByteOutStreamRaw& putFloat32(float value);
        // Format as an IEEE single-precision floating point number the most
        // significant 4 bytes in the specified 'value' to this output stream
        // and return a reference to this modifiable stream.  Note that, for
        // non-conforming platforms, the "most significant 4 bytes" might not
        // be contiguous bits, and that this operation may be lossy.

                        // *** arrays of integer values ***

    bdex_ByteOutStreamRaw&
    putArrayInt64(const bsls_PlatformUtil::Int64 *array, int count);
        // Format as an atomic sequence of signed integers the least
        // significant 64 bits of each of the specified 'count' leading entries
        // in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless 0 <= 'count'.

    bdex_ByteOutStreamRaw&
    putArrayUint64(const bsls_PlatformUtil::Uint64 *array, int count);
        // Format as an atomic sequence of unsigned integers the least
        // significant 64 bits of each of the specified 'count' leading entries
        // in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless 0 <= 'count'.

    bdex_ByteOutStreamRaw&
    putArrayInt56(const bsls_PlatformUtil::Int64 *array, int count);
        // Format as an atomic sequence of signed integers the least
        // significant 56 bits of each of the specified 'count' leading entries
        // in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless 0 <= 'count'.

    bdex_ByteOutStreamRaw&
    putArrayUint56(const bsls_PlatformUtil::Uint64 *array, int count);
        // Format as an atomic sequence of unsigned integers the least
        // significant 56 bits of each of the specified 'count' leading entries
        // in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless 0 <= 'count'.

    bdex_ByteOutStreamRaw&
    putArrayInt48(const bsls_PlatformUtil::Int64 *array, int count);
        // Format as an atomic sequence of signed integers the least
        // significant 48 bits of each of the specified 'count' leading entries
        // in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless 0 <= 'count'.

    bdex_ByteOutStreamRaw&
    putArrayUint48(const bsls_PlatformUtil::Uint64 *array, int count);
        // Format as an atomic sequence of unsigned integers the least
        // significant 48 bits of each of the specified 'count' leading entries
        // in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless 0 <= 'count'.

    bdex_ByteOutStreamRaw&
    putArrayInt40(const bsls_PlatformUtil::Int64 *array, int count);
        // Format as an atomic sequence of signed integers the least
        // significant 40 bits of each of the specified 'count' leading entries
        // in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless 0 <= 'count'.

    bdex_ByteOutStreamRaw&
    putArrayUint40(const bsls_PlatformUtil::Uint64 *array, int count);
        // Format as an atomic sequence of unsigned integers the least
        // significant 40 bits of each of the specified 'count' leading entries
        // in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless 0 <= 'count'.

    bdex_ByteOutStreamRaw& putArrayInt32(const int *array, int count);
        // Format as an atomic sequence of signed integers the least
        // significant 32 bits of each of the specified 'count' leading entries
        // in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless 0 <= 'count'.

    bdex_ByteOutStreamRaw& putArrayUint32(const unsigned int *array,
                                          int                 count);
        // Format as an atomic sequence of unsigned integers the least
        // significant 32 bits of each of the specified 'count' leading entries
        // in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless 0 <= 'count'.

    bdex_ByteOutStreamRaw& putArrayInt24(const int *array, int count);
        // Format as an atomic sequence of signed integers the least
        // significant 24 bits of each of the specified 'count' leading entries
        // in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless 0 <= 'count'.

    bdex_ByteOutStreamRaw& putArrayUint24(const unsigned int *array,
                                          int                 count);
        // Format as an atomic sequence of unsigned integers the least
        // significant 24 bits of each of the specified 'count' leading entries
        // in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless 0 <= 'count'.

    bdex_ByteOutStreamRaw& putArrayInt16(const short *array, int count);
        // Format as an atomic sequence of signed integers the least
        // significant 16 bits of each of the specified 'count' leading entries
        // in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless 0 <= 'count'.

    bdex_ByteOutStreamRaw& putArrayUint16(const unsigned short *array,
                                          int                   count);
        // Format as an atomic sequence of unsigned integers the least
        // significant 16 bits of each of the specified 'count' leading entries
        // in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless 0 <= 'count'.

    bdex_ByteOutStreamRaw& putArrayInt8(const char *array, int count);
    bdex_ByteOutStreamRaw& putArrayInt8(const signed char *array, int count);
        // Format as an atomic sequence of signed integers the 8 bits of each
        // of the specified 'count' leading entries in the specified 'array' to
        // this output stream and return a reference to this modifiable stream.
        // The behavior is undefined unless 0 <= 'count'.

    bdex_ByteOutStreamRaw& putArrayUint8(const char *array, int count);
    bdex_ByteOutStreamRaw& putArrayUint8(const unsigned char *array,
                                         int                  count);
        // Format as an atomic sequence of unsigned integers the 8 bits of each
        // of the specified 'count' leading entries in the specified 'array' to
        // this output stream and return a reference to this modifiable stream.
        // The behavior is undefined unless '0 <= count'.

                        // *** arrays of floating-point values ***

    bdex_ByteOutStreamRaw& putArrayFloat64(const double *array, int count);
        // Format as an atomic sequence of IEEE double-precision floating point
        // numbers the most significant 8 bytes in each of the specified
        // 'count' leading entries in the specified 'array' to this output
        // stream and return a reference to this modifiable stream.  The
        // behavior is undefined unless 0 <= 'count'.  Note that, for
        // non-conforming platforms, the "most significant 8 bytes" might not
        // be contiguous bits, and that this operation may be lossy.

    bdex_ByteOutStreamRaw& putArrayFloat32(const float *array, int count);
        // Format as an atomic sequence of IEEE single-precision floating point
        // numbers the most significant 4 bytes in each of the specified
        // 'count' leading entries in the specified 'array' to this output
        // stream and return a reference to this modifiable stream.  The
        // behavior is undefined unless 0 <= 'count'.  Note that, for
        // non-conforming platforms, the "most significant 4 bytes" might not
        // be contiguous bits, and that this operation may be lossy.

    void removeAll();
        // Remove all content in this stream.

    void reserveCapacity(int newCapacity);
        // Set the internal buffer size to be at least the specified
        // 'newCapacity'.

    // ACCESSORS
    operator const void *() const;
        // Return a non-zero value if this stream is valid, and 0 otherwise.
        // An invalid stream is a stream for which an output operation was
        // detected to have failed.

    const char *data() const;
        // Return the address of the contiguous, non-modifiable internal memory
        // buffer of this stream.  The address will remain valid as long as
        // this array is not destroyed or modified (i.e., the current capacity
        // is not exceeded).  The behavior of accessing elements outside the
        // range '[ data() .. data() + (length() - 1) ]' is undefined.

    int length() const;
        // Return the number of bytes in this stream.
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&                stream,
                         const bdex_ByteOutStreamRaw& object);
    // Write the specified 'object' to the specified output 'stream' in some
    // reasonable (multi-line) format, and return a reference to 'stream'

// TBD #if !defined(BSL_LEGACY) || 1 == BSL_LEGACY

template <typename OBJECT>
inline
bdex_ByteOutStreamRaw& operator<<(bdex_ByteOutStreamRaw& stream,
                                  const OBJECT&          object);
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

// CREATORS
inline
bdex_ByteOutStreamRaw::bdex_ByteOutStreamRaw(char *buffer, int length)
: d_streambuf(buffer, length)
, d_formatter(&d_streambuf)
{
    // IMPORTANT: d_streambuf and d_formatter must be initialized in the
    // order shown here.  Don't re-arrange their declarations in the class
    // definition.
}

inline
bdex_ByteOutStreamRaw::~bdex_ByteOutStreamRaw()
{
}

// MANIPULATORS
inline
void bdex_ByteOutStreamRaw::invalidate()
{
    d_formatter.invalidate();
}

inline
void bdex_ByteOutStreamRaw::reset(char *buffer, int length)
{
    d_streambuf.pubsetbuf(buffer, length);
}

inline
void bdex_ByteOutStreamRaw::setCursor(int index)
{
    d_streambuf.pubseekpos(index);
}

inline
void bdex_ByteOutStreamRaw::removeAll()
{
    d_streambuf.pubseekpos(0);
}

inline
void bdex_ByteOutStreamRaw::reserveCapacity(int)
{
}

inline
bdex_ByteOutStreamRaw& bdex_ByteOutStreamRaw::putLength(int value)
{
    d_formatter.putLength(value);
    return *this;
}

inline
bdex_ByteOutStreamRaw& bdex_ByteOutStreamRaw::putString(const bsl::string& str)
{
    d_formatter.putString(str);
    return *this;
}

inline
bdex_ByteOutStreamRaw& bdex_ByteOutStreamRaw::putVersion(int value)
{
    d_formatter.putVersion(value);
    return *this;
}

inline
bdex_ByteOutStreamRaw&
bdex_ByteOutStreamRaw::putInt64(bsls_PlatformUtil::Int64 value)
{
    d_formatter.putInt64(value);
    return *this;
}

inline
bdex_ByteOutStreamRaw&
bdex_ByteOutStreamRaw::putUint64(bsls_PlatformUtil::Int64 value)
{
    d_formatter.putUint64(value);
    return *this;
}

inline
bdex_ByteOutStreamRaw&
bdex_ByteOutStreamRaw::putInt56(bsls_PlatformUtil::Int64 value)
{
    d_formatter.putInt56(value);
    return *this;
}

inline
bdex_ByteOutStreamRaw&
bdex_ByteOutStreamRaw::putUint56(bsls_PlatformUtil::Int64 value)
{
    d_formatter.putUint56(value);
    return *this;
}

inline
bdex_ByteOutStreamRaw&
bdex_ByteOutStreamRaw::putInt48(bsls_PlatformUtil::Int64 value)
{
    d_formatter.putInt48(value);
    return *this;
}

inline
bdex_ByteOutStreamRaw&
bdex_ByteOutStreamRaw::putUint48(bsls_PlatformUtil::Int64 value)
{
    d_formatter.putUint48(value);
    return *this;
}

inline
bdex_ByteOutStreamRaw&
bdex_ByteOutStreamRaw::putInt40(bsls_PlatformUtil::Int64 value)
{
    d_formatter.putInt40(value);
    return *this;
}

inline
bdex_ByteOutStreamRaw&
bdex_ByteOutStreamRaw::putUint40(bsls_PlatformUtil::Int64 value)
{
    d_formatter.putUint40(value);
    return *this;
}

inline
bdex_ByteOutStreamRaw&
bdex_ByteOutStreamRaw::putInt32(int value)
{
    d_formatter.putInt32(value);
    return *this;
}

inline
bdex_ByteOutStreamRaw&
bdex_ByteOutStreamRaw::putUint32(unsigned int value)
{
    d_formatter.putUint32(value);
    return *this;
}

inline
bdex_ByteOutStreamRaw&
bdex_ByteOutStreamRaw::putInt24(int value)
{
    d_formatter.putInt24(value);
    return *this;
}

inline
bdex_ByteOutStreamRaw&
bdex_ByteOutStreamRaw::putUint24(int value)
{
    d_formatter.putUint24(value);
    return *this;
}

inline
bdex_ByteOutStreamRaw&
bdex_ByteOutStreamRaw::putInt16(int value)
{
    d_formatter.putInt16(value);
    return *this;
}

inline
bdex_ByteOutStreamRaw&
bdex_ByteOutStreamRaw::putUint16(int value)
{
    d_formatter.putUint16(value);
    return *this;
}

inline
bdex_ByteOutStreamRaw&
bdex_ByteOutStreamRaw::putInt8(int value)
{
    d_formatter.putInt8(value);
    return *this;
}

inline
bdex_ByteOutStreamRaw&
bdex_ByteOutStreamRaw::putUint8(int value)
{
    d_formatter.putUint8(value);
    return *this;
}

inline
bdex_ByteOutStreamRaw&
bdex_ByteOutStreamRaw::putFloat64(double value)
{
    d_formatter.putFloat64(value);
    return *this;
}

inline
bdex_ByteOutStreamRaw&
bdex_ByteOutStreamRaw::putFloat32(float value)
{
    d_formatter.putFloat32(value);
    return *this;
}

inline
bdex_ByteOutStreamRaw&
bdex_ByteOutStreamRaw::putArrayInt64(const bsls_PlatformUtil::Int64 *array,
                                     int                             count)
{
    d_formatter.putArrayInt64(array, count);
    return *this;
}

inline
bdex_ByteOutStreamRaw&
bdex_ByteOutStreamRaw::putArrayUint64(const bsls_PlatformUtil::Uint64 *array,
                                      int                              count)
{
    d_formatter.putArrayUint64(array, count);
    return *this;
}

inline
bdex_ByteOutStreamRaw&
bdex_ByteOutStreamRaw::putArrayInt56(const bsls_PlatformUtil::Int64 *array,
                                     int                             count)
{
    d_formatter.putArrayInt56(array, count);
    return *this;
}

inline
bdex_ByteOutStreamRaw&
bdex_ByteOutStreamRaw::putArrayUint56(const bsls_PlatformUtil::Uint64 *array,
                                      int                              count)
{
    d_formatter.putArrayUint56(array, count);
    return *this;
}

inline
bdex_ByteOutStreamRaw&
bdex_ByteOutStreamRaw::putArrayInt48(const bsls_PlatformUtil::Int64 *array,
                                     int                             count)
{
    d_formatter.putArrayInt48(array, count);
    return *this;
}

inline
bdex_ByteOutStreamRaw&
bdex_ByteOutStreamRaw::putArrayUint48(const bsls_PlatformUtil::Uint64 *array,
                                      int                              count)
{
    d_formatter.putArrayUint48(array, count);
    return *this;
}

inline
bdex_ByteOutStreamRaw&
bdex_ByteOutStreamRaw::putArrayInt40(const bsls_PlatformUtil::Int64 *array,
                                     int                             count)
{
    d_formatter.putArrayInt40(array, count);
    return *this;
}

inline
bdex_ByteOutStreamRaw&
bdex_ByteOutStreamRaw::putArrayUint40(const bsls_PlatformUtil::Uint64 *array,
                                      int                              count)
{
    d_formatter.putArrayUint40(array, count);
    return *this;
}

inline
bdex_ByteOutStreamRaw&
bdex_ByteOutStreamRaw::putArrayInt32(const int *array, int count)
{
    d_formatter.putArrayInt32(array, count);
    return *this;
}

inline
bdex_ByteOutStreamRaw&
bdex_ByteOutStreamRaw::putArrayUint32(const unsigned int *array, int count)
{
    d_formatter.putArrayUint32(array, count);
    return *this;
}

inline
bdex_ByteOutStreamRaw&
bdex_ByteOutStreamRaw::putArrayInt24(const int *array, int count)
{
    d_formatter.putArrayInt24(array, count);
    return *this;
}

inline
bdex_ByteOutStreamRaw&
bdex_ByteOutStreamRaw::putArrayUint24(const unsigned int *array, int count)
{
    d_formatter.putArrayUint24(array, count);
    return *this;
}

inline
bdex_ByteOutStreamRaw&
bdex_ByteOutStreamRaw::putArrayInt16(const short *array, int count)
{
    d_formatter.putArrayInt16(array, count);
    return *this;
}

inline
bdex_ByteOutStreamRaw&
bdex_ByteOutStreamRaw::putArrayUint16(const unsigned short *array, int count)
{
    d_formatter.putArrayUint16(array, count);
    return *this;
}

inline
bdex_ByteOutStreamRaw&
bdex_ByteOutStreamRaw::putArrayInt8(const char *array, int count)
{
    d_formatter.putArrayInt8(array, count);
    return *this;
}

inline
bdex_ByteOutStreamRaw&
bdex_ByteOutStreamRaw::putArrayInt8(const signed char *array, int count)
{
    d_formatter.putArrayInt8(array, count);
    return *this;
}

inline
bdex_ByteOutStreamRaw&
bdex_ByteOutStreamRaw::putArrayUint8(const char *array, int count)
{
    d_formatter.putArrayUint8(array, count);
    return *this;
}

inline
bdex_ByteOutStreamRaw&
bdex_ByteOutStreamRaw::putArrayUint8(const unsigned char *array, int count)
{
    d_formatter.putArrayUint8(array, count);
    return *this;
}

inline
bdex_ByteOutStreamRaw&
bdex_ByteOutStreamRaw::putArrayFloat64(const double *array, int count)
{
    d_formatter.putArrayFloat64(array, count);
    return *this;
}

inline
bdex_ByteOutStreamRaw&
bdex_ByteOutStreamRaw::putArrayFloat32(const float *array, int count)
{
    d_formatter.putArrayFloat32(array, count);
    return *this;
}

// TBD #if !defined(BSL_LEGACY) || 1 == BSL_LEGACY

template <typename TYPE>
inline
bdex_ByteOutStreamRaw& bdex_ByteOutStreamRaw::put(const TYPE& variable)
{
    return bdex_OutStreamMethodsUtil::streamOutVersionAndObject(*this,
                                                                variable);
}

// TBD #endif

// ACCESSORS
inline
bdex_ByteOutStreamRaw::operator const void *() const
{
    return d_formatter ? this : 0;
}

inline
const char *bdex_ByteOutStreamRaw::data() const
{
    return (const char *)d_streambuf.data();
}

inline
int bdex_ByteOutStreamRaw::length() const
{
    return d_streambuf.length();
}

// FREE OPERATORS
// TBD #if !defined(BSL_LEGACY) || 1 == BSL_LEGACY

template <typename OBJECT>
inline
bdex_ByteOutStreamRaw& operator<<(bdex_ByteOutStreamRaw& stream,
                                  const OBJECT&          object)
{
    return stream.put(object);
}

// TBD #endif

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
