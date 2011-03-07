// bdex_byteoutstream.h                                               -*-C++-*-
#ifndef INCLUDED_BDEX_BYTEOUTSTREAM
#define INCLUDED_BDEX_BYTEOUTSTREAM

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Enable externalization of fundamental types to byte representation.
//
//@CLASSES:
//   bdex_ByteOutStream: byte-array-based output stream class
//
//@AUTHOR: Shao-wei Hung
//
//@SEE_ALSO: bdex_byteinstream, bdex_byteinstreamraw
//
//@DESCRIPTION: This component conforms to the 'bdex_outstream' (documentation
// only) "protocol", implementing a byte-array-based output stream class that
// provides platform-independent output methods ("externalization") on values,
// and arrays of values, of fundamental types, and on 'bsl::string'.
//
// This component is intended to be used in conjunction with the
// 'bdex_byteinstream' "unexternalization" component.  Each output method of
// 'bdex_ByteOutStream' writes a value or a homogeneous array of values to an
// internally managed buffer.  The values are formatted to be readable by the
// corresponding 'bdex_ByteInStream' method.  'bdex_ByteInStreamRaw' may also
// be used to read buffers written by this stream; see the
// 'bdex_byteinstreamraw' component documentation for more information about
// "raw" stream interoperability.  In general, the user cannot rely on any
// other mechanism to read data written by 'bdex_ByteOutStream' unless that
// mechanism explicitly states its ability to do so.
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
///EXTERNALIZATION AND VALUE SEMANTICS
///-----------------------------------
// The concept of "externalization" in this component is intentionally
// specialized to support streaming the *values* of entities atomically.  In
// particular, streaming an array of a specific length is considered to be an
// atomic operation.  It is therefore an error, for example, to stream out an
// array of 'int' of length three and then stream in those three 'int' values
// as three scalar 'int' variables.  Similarly, it is an error to stream out an
// array of length *L* and stream back in an array of length other than *L*.
//
///Usage
///-----
// The 'bdex_ByteOutStream' implementation of the 'bdex_OutStream' protocol can
// be used to externalize values in a platform-neutral way.  Writing out
// fundamental C++ types and 'bsl::string' requires no additional work on the
// part of the client; the client can simply use the stream directly.  The
// following code serializes a few representative values using a
// 'bdex_ByteOutStream', and then writes the contents of the 'bdex' stream's
// buffer to 'stdout'.
//..
// int main(int argc, char **argv)
// {
//
//     // Create a stream and write out some values.
//     bdex_ByteOutStream outStream;
//     bdex_OutStreamFunctions::streamOut(stream, 1, 0);
//     bdex_OutStreamFunctions::streamOut(stream, 2, 0);
//     bdex_OutStreamFunctions::streamOut(stream, 'c', 0);
//     bdex_OutStreamFunctions::streamOut(stream, bsl::string("hello"), 0);
//
//     // Verify the results on 'stdout'.
//     const char *theChars = outStream.data();
//     int length = outStream.length();
//     for(int i = 0; i < length; ++i) {
//         if(bsl::isalnum(static_cast<unsigned char>(theChars[i]))) {
//             cout << "nextByte (char): " << theChars[i] << endl;
//         }
//         else {
//             cout << "nextByte (int): " << (int)theChars[i] << endl;
//         }
//     }
//     return 0;
// }
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
// for type 'bsl::string', which outputs the string length before emitting the
// characters.
//
// See the 'bdex_byteinstream' component usage example for a more practical
// example of using 'bdex' streams with a value-semantic type.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEX_BYTEOUTSTREAMFORMATTER
#include <bdex_byteoutstreamformatter.h>
#endif

#ifndef INCLUDED_BDESB_MEMOUTSTREAMBUF
#include <bdesb_memoutstreambuf.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
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

class bslma_Allocator;

                        // ========================
                        // class bdex_ByteOutStream
                        // ========================

class bdex_ByteOutStream {
    // This class provides output methods to externalize values, and C-style
    // arrays of values, of the fundamental integral and floating-point types,
    // as well as 'bsl::string' values, in a documented format.

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

    // IMPORTANT: d_streambuf and d_formatter must be initialized in the order
    // declared below.  Don't re-arrange these declarations!

    // DATA
    bdesb_MemOutStreamBuf       d_streambuf;     // externalized values
    bdex_ByteOutStreamFormatter d_formatter;     // formatter

    friend bsl::ostream& operator<<(bsl::ostream&, const bdex_ByteOutStream&);

  public:
    // CREATORS
    bdex_ByteOutStream(bslma_Allocator *basicAllocator = 0);
        // Create an empty output byte stream.  Optionally specify the
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used to supply memory.

    bdex_ByteOutStream(int              initialCapacity,
                       bslma_Allocator *basicAllocator = 0);
        // Create an empty output byte stream having an initial buffer capacity
        // of at least the specified 'initialCapacity'.  Optionally specify the
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used to supply memory.

    ~bdex_ByteOutStream();
        // Destroy this output byte stream.

    // MANIPULATORS
    void invalidate();
        // Put this output stream in an invalid state.  This function has no
        // effect if this stream is already invalid.  Note that this function
        // should be called whenever a write operation to this stream fails.

    bdex_ByteOutStream& putLength(int length);
        // Format the specified non-negative 'length' to this output stream and
        // return a reference to this modifiable stream.
        //
        // If 'length' is less than 128, then write the least significant byte
        // of 'length'.  Otherwise, write the least significant four bytes of
        // 'length' in network byte order, but with the most significant output
        // bit set to 1.

    bdex_ByteOutStream& putVersion(int version);
        // Format the specified non-negative 'version' to this output stream
        // and return a reference to this modifiable stream.
        //
        // Write the least significant byte of 'version'.

// TBD #if !defined(BSL_LEGACY) || 1 == BSL_LEGACY

    template <typename TYPE>
    bdex_ByteOutStream& put(const TYPE& variable);
        // Format the specified 'value' to this output stream and return a
        // reference to this modifiable stream.  If the user has provided
        // specializations of the 'maxSupportedBdexVersion' and 'bdexStreamOut'
        // methods for 'TYPE' then those are called.
        // If not, the 'bdex_OutStreamFunctions::streamOutVersionAndObject'
        // is called.

// TBD #endif

                        // *** scalar integer values ***

    bdex_ByteOutStream& putInt64(bsls_PlatformUtil::Int64 value);
        // Format the least significant 64 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.
        //
        // Write the least significant eight bytes of 'value' in network byte
        // order.

    bdex_ByteOutStream& putUint64(bsls_PlatformUtil::Int64 value);
        // Format the least significant 64 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.
        //
        // Write the least significant eight bytes of 'value' in network byte
        // order.

    bdex_ByteOutStream& putInt56(bsls_PlatformUtil::Int64 value);
        // Format the least significant 56 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.
        //
        // Write the least significant seven bytes of 'value' in network byte
        // order.

    bdex_ByteOutStream& putUint56(bsls_PlatformUtil::Int64 value);
        // Format the least significant 56 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.
        //
        // Write the least significant seven bytes of 'value' in network byte
        // order.

    bdex_ByteOutStream& putInt48(bsls_PlatformUtil::Int64 value);
        // Format the least significant 48 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.
        //
        // Write the least significant six bytes of 'value' in network byte
        // order.

    bdex_ByteOutStream& putUint48(bsls_PlatformUtil::Int64 value);
        // Format the least significant 48 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.
        //
        // Write the least significant six bytes of 'value' in network byte
        // order.

    bdex_ByteOutStream& putInt40(bsls_PlatformUtil::Int64 value);
        // Format the least significant 40 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.
        //
        // Write the least significant five bytes of 'value' in network byte
        // order.

    bdex_ByteOutStream& putUint40(bsls_PlatformUtil::Int64 value);
        // Format the least significant 40 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.
        //
        // Write the least significant five bytes of 'value' in network byte
        // order.

    bdex_ByteOutStream& putInt32(int value);
        // Format the least significant 32 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.
        //
        // Write the least significant four bytes of 'value' in network byte
        // order.

    bdex_ByteOutStream& putUint32(unsigned int value);
        // Format the least significant 32 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.
        //
        // Write the least significant four bytes of 'value' in network byte
        // order.

    bdex_ByteOutStream& putInt24(int value);
        // Format the least significant 24 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.
        //
        // Write the least significant three bytes of 'value' in network byte
        // order.

    bdex_ByteOutStream& putUint24(int value);
        // Format the least significant 24 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.
        //
        // Write the least significant three bytes of 'value' in network byte
        // order.

    bdex_ByteOutStream& putInt16(int value);
        // Format the least significant 16 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.
        //
        // Write the least significant two bytes of 'value' in network byte
        // order.

    bdex_ByteOutStream& putUint16(int value);
        // Format the least significant 16 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.
        //
        // Write the least significant two bytes of 'value' in network byte
        // order.

    bdex_ByteOutStream& putInt8(int value);
        // Format the least significant 8 bits of the specified 'value' to this
        // output stream and return a reference to this modifiable stream.
        //
        // Write the least significant byte of 'value'.

    bdex_ByteOutStream& putUint8(int value);
        // Format the least significant 8 bits of the specified 'value' to this
        // output stream and return a reference to this modifiable stream.
        //
        // Write the least significant byte of 'value'.

                        // *** scalar floating-point values ***

    bdex_ByteOutStream& putFloat64(double value);
        // Format the most significant 64 bits in the specified 'value' to this
        // output stream and return a reference to this modifiable stream.
        // Note that, for non-IEEE-conforming platforms, the "most significant
        // 64 bits" of 'value' might not be contiguous, and that this operation
        // may be lossy.
        //
        // Format 'value' as an IEEE double-precision floating point number and
        // write those eight bytes in network byte order.

    bdex_ByteOutStream& putFloat32(float value);
        // Format the most significant 32 bits in the specified 'value' to this
        // output stream and return a reference to this modifiable stream.
        // Note that, for non-IEEE-conforming platforms, the "most significant
        // 32 bits" of 'value' might not be contiguous, and that this operation
        // may be lossy.
        //
        // Format 'value' as an IEEE single-precision floating point number and
        // write those four bytes in network byte order.

                        // *** string values ***

    bdex_ByteOutStream& putString(const bsl::string& value);
        // Format the specified 'value' to this output stream and return
        // a reference to this modifiable stream.
        //
        // Write 'value.length()' using the 'putLength' format, then
        // write each character of 'value' as a one-byte integer.

                        // *** arrays of integer values ***

    bdex_ByteOutStream& putArrayInt64(const bsls_PlatformUtil::Int64 *array,
                                      int                             count);
        // Format as an atomic sequence the least significant 64 bits of each
        // of the specified 'count' leading entries in the specified 'array' to
        // this output stream and return a reference to this modifiable stream.
        // The behavior is undefined unless '0 <= count'.
        //
        // For each element of 'array', write the least significant eight bytes
        // in network byte order.

    bdex_ByteOutStream& putArrayUint64(const bsls_PlatformUtil::Uint64 *array,
                                       int                              count);
        // Format as an atomic sequence the least significant 64 bits of each
        // of the specified 'count' leading entries in the specified 'array' to
        // this output stream and return a reference to this modifiable stream.
        // The behavior is undefined unless '0 <= count'.
        //
        // For each element of 'array', write the least significant eight bytes
        // in network byte order.

    bdex_ByteOutStream& putArrayInt56(const bsls_PlatformUtil::Int64 *array,
                                      int                             count);
        // Format as an atomic sequence the least significant 56 bits of each
        // of the specified 'count' leading entries in the specified 'array' to
        // this output stream and return a reference to this modifiable stream.
        // The behavior is undefined unless '0 <= count'.
        //
        // For each element of 'array', write the least significant seven bytes
        // in network byte order.

    bdex_ByteOutStream& putArrayUint56(const bsls_PlatformUtil::Uint64 *array,
                                       int                              count);
        // Format as an atomic sequence the least significant 56 bits of each
        // of the specified 'count' leading entries in the specified 'array' to
        // this output stream and return a reference to this modifiable stream.
        // The behavior is undefined unless '0 <= count'.
        //
        // For each element of 'array', write the least significant seven bytes
        // in network byte order.

    bdex_ByteOutStream& putArrayInt48(const bsls_PlatformUtil::Int64 *array,
                                      int                             count);
        // Format as an atomic sequence the least significant 48 bits of each
        // of the specified 'count' leading entries in the specified 'array' to
        // this output stream and return a reference to this modifiable stream.
        // The behavior is undefined unless '0 <= count'.
        //
        // For each element of 'array', write the least significant six bytes
        // in network byte order.

    bdex_ByteOutStream& putArrayUint48(const bsls_PlatformUtil::Uint64 *array,
                                       int                              count);
        // Format as an atomic sequence the least significant 48 bits of each
        // of the specified 'count' leading entries in the specified 'array' to
        // this output stream and return a reference to this modifiable stream.
        // The behavior is undefined unless '0 <= count'.
        //
        // For each element of 'array', write the least significant six bytes
        // in network byte order.

    bdex_ByteOutStream& putArrayInt40(const bsls_PlatformUtil::Int64 *array,
                                      int                             count);
        // Format as an atomic sequence the least significant 40 bits of each
        // of the specified 'count' leading entries in the specified 'array' to
        // this output stream and return a reference to this modifiable stream.
        // The behavior is undefined unless '0 <= count'.
        //
        // For each element of 'array', write the least significant five bytes
        // in network byte order.

    bdex_ByteOutStream& putArrayUint40(const bsls_PlatformUtil::Uint64 *array,
                                       int                              count);
        // Format as an atomic sequence the least significant 40 bits of each
        // of the specified 'count' leading entries in the specified 'array' to
        // this output stream and return a reference to this modifiable stream.
        // The behavior is undefined unless '0 <= count'.
        //
        // For each element of 'array', write the least significant five bytes
        // in network byte order.

    bdex_ByteOutStream& putArrayInt32(const int *array, int count);
        // Format as an atomic sequence the least significant 32 bits of each
        // of the specified 'count' leading entries in the specified 'array' to
        // this output stream and return a reference to this modifiable stream.
        // The behavior is undefined unless '0 <= count'.
        //
        // For each element of 'array', write the least significant four bytes
        // in network byte order.

    bdex_ByteOutStream& putArrayUint32(const unsigned int *array, int count);
        // Format as an atomic sequence the least significant 32 bits of each
        // of the specified 'count' leading entries in the specified 'array' to
        // this output stream and return a reference to this modifiable stream.
        // The behavior is undefined unless '0 <= count'.
        //
        // For each element of 'array', write the least significant four bytes
        // in network byte order.

    bdex_ByteOutStream& putArrayInt24(const int *array, int count);
        // Format as an atomic sequence the least significant 24 bits of each
        // of the specified 'count' leading entries in the specified 'array' to
        // this output stream and return a reference to this modifiable stream.
        // The behavior is undefined unless '0 <= count'.
        //
        // For each element of 'array', write the least significant three bytes
        // in network byte order.

    bdex_ByteOutStream& putArrayUint24(const unsigned int *array,
                                       int                 count);
        // Format as an atomic sequence the least significant 24 bits of each
        // of the specified 'count' leading entries in the specified 'array' to
        // this output stream and return a reference to this modifiable stream.
        // The behavior is undefined unless '0 <= count'.
        //
        // For each element of 'array', write the least significant three bytes
        // in network byte order.

    bdex_ByteOutStream& putArrayInt16(const short *array, int count);
        // Format as an atomic sequence the least significant 16 bits of each
        // of the specified 'count' leading entries in the specified 'array' to
        // this output stream and return a reference to this modifiable stream.
        // The behavior is undefined unless '0 <= count'.
        //
        // For each element of 'array', write the least significant two bytes
        // in network byte order.

    bdex_ByteOutStream& putArrayUint16(const unsigned short *array,
                                       int                   count);
        // Format as an atomic sequence the least significant 16 bits of each
        // of the specified 'count' leading entries in the specified 'array' to
        // this output stream and return a reference to this modifiable stream.
        // The behavior is undefined unless '0 <= count'.
        //
        // For each element of 'array', write the least significant two bytes
        // in network byte order.

    bdex_ByteOutStream& putArrayInt8(const char *array, int count);
    bdex_ByteOutStream& putArrayInt8(const signed char *array, int count);
        // Format as an atomic sequence the least significant 8 bits of each of
        // the specified 'count' leading entries in the specified 'array' to
        // this output stream and return a reference to this modifiable stream.
        // The behavior is undefined unless '0 <= count'.
        //
        // Write each byte of 'array' in order.

    bdex_ByteOutStream& putArrayUint8(const char *array, int count);
    bdex_ByteOutStream& putArrayUint8(const unsigned char *array, int count);
        // Format as an atomic sequence the least significant 8 bits of each of
        // the specified 'count' leading entries in the specified 'array' to
        // this output stream and return a reference to this modifiable stream.
        // The behavior is undefined unless '0 <= count'.
        //
        // Write each byte of 'array' in order.

                        // *** arrays of floating-point values ***

    bdex_ByteOutStream& putArrayFloat64(const double *array, int count);
        // Format as an atomic sequence the most significant 64 bits in each of
        // the specified 'count' leading entries in the specified 'array' to
        // this output stream and return a reference to this modifiable stream.
        // The behavior is undefined unless '0 <= count'.  Note that, for
        // non-IEEE-conforming platforms, the "most significant 64 bits" might
        // not be contiguous, and that this operation may be lossy.
        //
        // For each element of 'array', format the value as an IEEE
        // double-precision floating point number and write those eight bytes
        // in network byte order.

    bdex_ByteOutStream& putArrayFloat32(const float *array, int count);
        // Format as an atomic sequence the most significant 32 bits in each of
        // the specified 'count' leading entries in the specified 'array' to
        // this output stream and return a reference to this modifiable stream.
        // The behavior is undefined unless '0 <= count'.  Note that, for
        // non-IEEE-conforming platforms, the "most significant 32 bits" might
        // not be contiguous, and that this operation may be lossy.
        //
        // For each element of 'array', format the value as an IEEE
        // single-precision floating point number and write those four bytes in
        // network byte order.

    void removeAll();
        // Remove all content in this stream.

    void reserveCapacity(int newCapacity);
        // Set the internal buffer size of this stream to be at least the
        // specified 'newCapacity'.

    // ACCESSORS
    operator const void *() const;
        // Return a non-zero value if this stream is valid, and 0 otherwise.
        // An invalid stream is a stream for which an output operation was
        // detected to have failed.

    const char *data() const;
        // Return the address of the contiguous, non-modifiable internal memory
        // buffer of this stream.  The address will remain valid as long as
        // this stream is not destroyed or modified.  The behavior of accessing
        // elements outside the range '[ data() .. data() + (length() - 1) ]'
        // is undefined.

    int length() const;
        // Return the number of bytes in this stream.
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&             stream,
                         const bdex_ByteOutStream& object);
    // Write the specified 'object' to the specified output 'stream' in some
    // reasonable (multi-line) format, and return a reference to 'stream'.

// TBD #if !defined(BSL_LEGACY) || 1 == BSL_LEGACY

template <typename OBJECT>
inline
bdex_ByteOutStream& operator<<(bdex_ByteOutStream& stream,
                               const OBJECT&       object);
    // Write to the specified output 'stream', using the 'putVersion' method of
    // this component, the most current version number supported by 'OBJECT'
    // and format the value of the specified 'object' to 'stream' using the
    // 'bdex' compliant 'OBJECT::bdexStreamOut' method in the
    // 'OBJECT::maxSupportedBdexVersion' format.  Return a reference to the
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
bdex_ByteOutStream::bdex_ByteOutStream(bslma_Allocator *basicAllocator)
: d_streambuf(basicAllocator)
, d_formatter(&d_streambuf)
{
    // IMPORTANT: d_streambuf and d_formatter must be initialized in the order
    // shown here.  Don't re-arrange their declarations in the class
    // definition.
}

inline
bdex_ByteOutStream::bdex_ByteOutStream(int              initialCapacity,
                                       bslma_Allocator *basicAllocator)
: d_streambuf(initialCapacity, basicAllocator)
, d_formatter(&d_streambuf)
{
    // IMPORTANT: d_streambuf and d_formatter must be initialized in the order
    // shown here.  Don't re-arrange their declarations in the class
    // definition.

    BSLS_ASSERT_SAFE(0 <= initialCapacity);
}

inline
bdex_ByteOutStream::~bdex_ByteOutStream()
{
}

// MANIPULATORS
inline
void bdex_ByteOutStream::invalidate()
{
    d_formatter.invalidate();
}

inline
void bdex_ByteOutStream::removeAll()
{
    d_streambuf.pubseekpos(0);
}

inline
void bdex_ByteOutStream::reserveCapacity(int newCapacity)
{
    BSLS_ASSERT_SAFE(0 <= newCapacity);

    d_streambuf.reserveCapacity(newCapacity);
}

inline
bdex_ByteOutStream& bdex_ByteOutStream::putLength(int length)
{
    BSLS_ASSERT_SAFE(0 <= length);

    d_formatter.putLength(length);
    return *this;
}

inline
bdex_ByteOutStream& bdex_ByteOutStream::putString(const bsl::string& value)
{
    d_formatter.putString(value);
    return *this;
}

inline
bdex_ByteOutStream& bdex_ByteOutStream::putVersion(int version)
{
    d_formatter.putVersion(version);
    return *this;
}

inline
bdex_ByteOutStream&
bdex_ByteOutStream::putInt64(bsls_PlatformUtil::Int64 value)
{
    d_formatter.putInt64(value);
    return *this;
}

inline
bdex_ByteOutStream&
bdex_ByteOutStream::putUint64(bsls_PlatformUtil::Int64 value)
{
    d_formatter.putUint64(value);
    return *this;
}

inline
bdex_ByteOutStream&
bdex_ByteOutStream::putInt56(bsls_PlatformUtil::Int64 value)
{
    d_formatter.putInt56(value);
    return *this;
}

inline
bdex_ByteOutStream&
bdex_ByteOutStream::putUint56(bsls_PlatformUtil::Int64 value)
{
    d_formatter.putUint56(value);
    return *this;
}

inline
bdex_ByteOutStream&
bdex_ByteOutStream::putInt48(bsls_PlatformUtil::Int64 value)
{
    d_formatter.putInt48(value);
    return *this;
}

inline
bdex_ByteOutStream&
bdex_ByteOutStream::putUint48(bsls_PlatformUtil::Int64 value)
{
    d_formatter.putUint48(value);
    return *this;
}

inline
bdex_ByteOutStream&
bdex_ByteOutStream::putInt40(bsls_PlatformUtil::Int64 value)
{
    d_formatter.putInt40(value);
    return *this;
}

inline
bdex_ByteOutStream&
bdex_ByteOutStream::putUint40(bsls_PlatformUtil::Int64 value)
{
    d_formatter.putUint40(value);
    return *this;
}

inline
bdex_ByteOutStream&
bdex_ByteOutStream::putInt32(int value)
{
    d_formatter.putInt32(value);
    return *this;
}

inline
bdex_ByteOutStream&
bdex_ByteOutStream::putUint32(unsigned int value)
{
    d_formatter.putUint32(value);
    return *this;
}

inline
bdex_ByteOutStream&
bdex_ByteOutStream::putInt24(int value)
{
    d_formatter.putInt24(value);
    return *this;
}

inline
bdex_ByteOutStream&
bdex_ByteOutStream::putUint24(int value)
{
    d_formatter.putUint24(value);
    return *this;
}

inline
bdex_ByteOutStream&
bdex_ByteOutStream::putInt16(int value)
{
    d_formatter.putInt16(value);
    return *this;
}

inline
bdex_ByteOutStream&
bdex_ByteOutStream::putUint16(int value)
{
    d_formatter.putUint16(value);
    return *this;
}

inline
bdex_ByteOutStream&
bdex_ByteOutStream::putInt8(int value)
{
    d_formatter.putInt8(value);
    return *this;
}

inline
bdex_ByteOutStream&
bdex_ByteOutStream::putUint8(int value)
{
    d_formatter.putUint8(value);
    return *this;
}

inline
bdex_ByteOutStream&
bdex_ByteOutStream::putFloat64(double value)
{
    d_formatter.putFloat64(value);
    return *this;
}

inline
bdex_ByteOutStream&
bdex_ByteOutStream::putFloat32(float value)
{
    d_formatter.putFloat32(value);
    return *this;
}

inline
bdex_ByteOutStream&
bdex_ByteOutStream::putArrayInt64(const bsls_PlatformUtil::Int64 *array,
                                  int                             count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    d_formatter.putArrayInt64(array, count);
    return *this;
}

inline
bdex_ByteOutStream&
bdex_ByteOutStream::putArrayUint64(const bsls_PlatformUtil::Uint64 *array,
                                   int                              count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    d_formatter.putArrayUint64(array, count);
    return *this;
}

inline
bdex_ByteOutStream&
bdex_ByteOutStream::putArrayInt56(const bsls_PlatformUtil::Int64 *array,
                                  int                             count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    d_formatter.putArrayInt56(array, count);
    return *this;
}

inline
bdex_ByteOutStream&
bdex_ByteOutStream::putArrayUint56(const bsls_PlatformUtil::Uint64 *array,
                                   int                              count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    d_formatter.putArrayUint56(array, count);
    return *this;
}

inline
bdex_ByteOutStream&
bdex_ByteOutStream::putArrayInt48(const bsls_PlatformUtil::Int64 *array,
                                  int                             count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    d_formatter.putArrayInt48(array, count);
    return *this;
}

inline
bdex_ByteOutStream&
bdex_ByteOutStream::putArrayUint48(const bsls_PlatformUtil::Uint64 *array,
                                   int                              count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    d_formatter.putArrayUint48(array, count);
    return *this;
}

inline
bdex_ByteOutStream&
bdex_ByteOutStream::putArrayInt40(const bsls_PlatformUtil::Int64 *array,
                                  int                             count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    d_formatter.putArrayInt40(array, count);
    return *this;
}

inline
bdex_ByteOutStream&
bdex_ByteOutStream::putArrayUint40(const bsls_PlatformUtil::Uint64 *array,
                                   int                              count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    d_formatter.putArrayUint40(array, count);
    return *this;
}

inline
bdex_ByteOutStream&
bdex_ByteOutStream::putArrayInt32(const int *array, int count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    d_formatter.putArrayInt32(array, count);
    return *this;
}

inline
bdex_ByteOutStream&
bdex_ByteOutStream::putArrayUint32(const unsigned int *array, int count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    d_formatter.putArrayUint32(array, count);
    return *this;
}

inline
bdex_ByteOutStream&
bdex_ByteOutStream::putArrayInt24(const int *array, int count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    d_formatter.putArrayInt24(array, count);
    return *this;
}

inline
bdex_ByteOutStream&
bdex_ByteOutStream::putArrayUint24(const unsigned int *array, int count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    d_formatter.putArrayUint24(array, count);
    return *this;
}

inline
bdex_ByteOutStream&
bdex_ByteOutStream::putArrayInt16(const short *array, int count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    d_formatter.putArrayInt16(array, count);
    return *this;
}

inline
bdex_ByteOutStream&
bdex_ByteOutStream::putArrayUint16(const unsigned short *array, int count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    d_formatter.putArrayUint16(array, count);
    return *this;
}

inline
bdex_ByteOutStream&
bdex_ByteOutStream::putArrayInt8(const char *array, int count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    d_formatter.putArrayInt8(array, count);
    return *this;
}

inline
bdex_ByteOutStream&
bdex_ByteOutStream::putArrayInt8(const signed char *array, int count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    d_formatter.putArrayInt8(array, count);
    return *this;
}

inline
bdex_ByteOutStream&
bdex_ByteOutStream::putArrayUint8(const char *array, int count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    d_formatter.putArrayUint8(array, count);
    return *this;
}

inline
bdex_ByteOutStream&
bdex_ByteOutStream::putArrayUint8(const unsigned char *array, int count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    d_formatter.putArrayUint8(array, count);
    return *this;
}

inline
bdex_ByteOutStream&
bdex_ByteOutStream::putArrayFloat64(const double *array, int count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    d_formatter.putArrayFloat64(array, count);
    return *this;
}

inline
bdex_ByteOutStream&
bdex_ByteOutStream::putArrayFloat32(const float *array, int count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    d_formatter.putArrayFloat32(array, count);
    return *this;
}

// TBD #if !defined(BSL_LEGACY) || 1 == BSL_LEGACY

template <typename TYPE>
inline
bdex_ByteOutStream& bdex_ByteOutStream::put(const TYPE& variable)
{
    return bdex_OutStreamMethodsUtil::streamOutVersionAndObject(*this,
                                                                variable);
}

// TBD #endif

// ACCESSORS
inline
bdex_ByteOutStream::operator const void *() const
{
    return d_formatter ? this : 0;
}

inline
const char *bdex_ByteOutStream::data() const
{
    return (const char *)d_streambuf.data();
}

inline
int bdex_ByteOutStream::length() const
{
    return (int)d_streambuf.length();
}

// FREE OPERATORS

// TBD #if !defined(BSL_LEGACY) || 1 == BSL_LEGACY

template <typename OBJECT>
inline
bdex_ByteOutStream& operator<<(bdex_ByteOutStream& stream,
                               const OBJECT&       object)
{
    return stream.put(object);
}

// TBD #endif

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
