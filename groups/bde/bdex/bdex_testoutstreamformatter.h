// bdex_testoutstreamformatter.h                                      -*-C++-*-
#ifndef INCLUDED_BDEX_TESTOUTSTREAMFORMATTER
#define INCLUDED_BDEX_TESTOUTSTREAMFORMATTER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Enable externalization of fundamental types with identification.
//
//@CLASSES:
//   bdex_TestOutStream: streambuf-based output stream class
//
//@AUTHOR: Lea Fester
//
//@SEE_ALSO: bdex_testinstreamformatter
//
//@DESCRIPTION: This component conforms to the 'bdex_outstream' (documentation
// only) "protocol", implementing a byte-array-based output stream class that
// provides platform-independent output methods ("externalization") on values,
// and arrays of values, of fundamental types, and on 'bsl::string'.
//
// This component is intended to be used in conjunction with the
// 'bdex_testinstreamformatter' "unexternalization" component.  Each output
// method of 'bdex_TestOutStreamFormatter' writes a value or a homogeneous
// array of values to an internally managed buffer.  The values are formatted
// to be readable by the corresponding 'bdex_TestInStreamFormatter' method.  In
// general, the user cannot rely on any other mechanism to read data written by
// 'bdex_TestOutStreamFormatter' unless that mechanism explicitly states its
// ability to do so.
//
// The supported types and required content are listed in the table below.  All
// of the fundamental types in the table may be output as scalar values or as
// homogeneous arrays.  'bsl::string' is output as a logical scalar string.
// Note that 'Int64' and 'Uint64' denote 'bsls_PlatformUtil::Int64' and
// 'bsls_PlatformUtil::Uint64', which in turn are 'typedef' names for the
// signed and unsigned 64-bit integer types, respectively, on the host
// platform.
//..
//  C++ TYPE          REQUIRED CONTENT OF ANY PLATFORM NEUTRAL FORMAT
//  --------          -----------------------------------------------
//  Int64             least significant 64 bits (signed)
//  Uint64            least significant 64 bits (unsigned)
//  int               least significant 32 bits (signed)
//  unsigned int      least significant 32 bits (unsigned)
//  short             least significant 16 bits (signed)
//  unsigned short    least significant 16 bits (unsigned)
//  char              least significant  8 bits (platform dependent)
//  signed char       least significant  8 bits (signed)
//  unsigned char     least significant  8 bits (unsigned)
//  double            IEEE standard 8-byte floating-point value
//  float             IEEE standard 4-byte floating-point value
//
//  bsl::string       BDE STL implementation of the STL string class
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
// format.  Also note that a 'bdex_TestOutStreamFormatter' can neither be
// invalidated nor queried for validity.
//
// Note that output streams can be *invalidated* explicitly and queried for
// *validity*.  Writing to an initially invalid stream has no effect.  Whenever
// an output operation fails, the stream should be invalidated explicitly.
//
///Externalization and Value Semantics
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
// The 'bdex_TestOutStreamFormatter' implementation of the 'bdex_OutStream'
// protocol can be used to externalize values in a platform-neutral way.
// Writing out fundamental C++ types and 'bsl::string' requires no additional
// work on the part of the client; the client can simply use the stream
// directly.  The following code serializes a few representative values using a
// 'bdex_TestOutStreamFormatter', and then writes the contents of the 'bdex'
// stream's buffer to 'stdout'.
//..
//  int main(int argc, char **argv)
//  {
//      // Create a stream and write out some values.
//      bdex_TestOutStreamFormatter outStream;
//      bdex_OutStreamFunctions::streamOut(stream, 1, 0);
//      bdex_OutStreamFunctions::streamOut(stream, 2, 0);
//      bdex_OutStreamFunctions::streamOut(stream, 'c', 0);
//      bdex_OutStreamFunctions::streamOut(stream, bsl::string("hello"), 0);
//
//      // Verify the results on 'stdout'.
//      const char *theChars = outStream.data();
//      int length = outStream.length();
//      for (int i = 0; i < length; ++i) {
//          if (bsl::isalnum(static_cast<unsigned char>(theChars[i]))) {
//              cout << "nextByte (char): " << theChars[i] << endl;
//          }
//          else {
//              cout << "nextByte (int): " << (int)theChars[i] << endl;
//          }
//      }
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

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEX_BYTEOUTSTREAMFORMATTER
#include <bdex_byteoutstreamformatter.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_STREAMBUF
#include <bsl_streambuf.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSLFWD_BSLMA_ALLOCATOR
#include <bslfwd_bslma_allocator.h>
#endif

namespace BloombergLP {

class bdesb_MemOutStreamBuf;

                        // =================================
                        // class bdex_TestOutStreamFormatter
                        // =================================

class bdex_TestOutStreamFormatter {
    // This class implements output methods to externalize fundamental types
    // and their associated type identification data in byte representation.
    // It stores the accumulated result in network byte order format.

    // DATA
    bsl::streambuf *d_streamBuf;        // streambuf to write to
    bdex_ByteOutStreamFormatter
                    d_imp;              // the implementation
    int             d_makeNextInvalid;  // set to output invalid data for next
                                        // operation
    int             d_validFlag;        // indicate this stream's validity

    // NOT DEFINED
    bdex_TestOutStreamFormatter(const bdex_TestOutStreamFormatter&);
    bdex_TestOutStreamFormatter& operator=(const bdex_TestOutStreamFormatter&);

    bool operator==(const bdex_TestOutStreamFormatter&) const;
    bool operator!=(const bdex_TestOutStreamFormatter&) const;

  public:
    // CREATORS
    bdex_TestOutStreamFormatter(bsl::streambuf *streamBuf);
        // Create an empty output test stream that will write its output to the
        // specified 'streamBuf'.

    ~bdex_TestOutStreamFormatter();
        // Destroy this object.

    // MANIPULATORS
    void invalidate();
        // Put this output stream in an invalid state.  This function has no
        // effect if this stream is already invalid.  Note that this function
        // should be called whenever a write operation to this stream fails.

    void makeNextInvalid();
        // Make the next output operation format invalid data to this output
        // stream.  The invalid data can be detected by a corresponding
        // 'bdex_TestInStream' object.

    bdex_TestOutStreamFormatter& putLength(int length);
        // Format the specified 'length' as an 8-bit unsigned integer if
        // 'length' is less than 128, otherwise as a 32-bit integer to this
        // output stream with its most significant bit set and return a
        // reference to this modifiable stream.

    bdex_TestOutStreamFormatter& putString(const bsl::string& str);
        // Format as an array of 8-bit integers the specified 'str' to this
        // output stream and return a reference to this modifiable stream.

    bdex_TestOutStreamFormatter& putVersion(int version);
        // Format the specified 'version' as an 8-bit unsigned integer to this
        // output stream and return a reference to this modifiable stream.

                        // *** scalar integer values ***

    bdex_TestOutStreamFormatter& putInt64(bsls_PlatformUtil::Int64 value);
        // Format as a signed integer the least significant 64 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    bdex_TestOutStreamFormatter& putUint64(bsls_PlatformUtil::Int64 value);
        // Format as an unsigned integer the least significant 64 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    bdex_TestOutStreamFormatter& putInt56(bsls_PlatformUtil::Int64 value);
        // Format as a signed integer the least significant 56 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    bdex_TestOutStreamFormatter& putUint56(bsls_PlatformUtil::Int64 value);
        // Format as an unsigned integer the least significant 56 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    bdex_TestOutStreamFormatter& putInt48(bsls_PlatformUtil::Int64 value);
        // Format as a signed integer the least significant 48 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    bdex_TestOutStreamFormatter& putUint48(bsls_PlatformUtil::Int64 value);
        // Format as an unsigned integer the least significant 48 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    bdex_TestOutStreamFormatter& putInt40(bsls_PlatformUtil::Int64 value);
        // Format as a signed integer the least significant 40 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    bdex_TestOutStreamFormatter& putUint40(bsls_PlatformUtil::Int64 value);
        // Format as an unsigned integer the least significant 40 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    bdex_TestOutStreamFormatter& putInt32(int value);
        // Format as a signed integer the least significant 32 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    bdex_TestOutStreamFormatter& putUint32(unsigned int value);
        // Format as an unsigned integer the least significant 32 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    bdex_TestOutStreamFormatter& putInt24(int value);
        // Format as a signed integer the least significant 24 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    bdex_TestOutStreamFormatter& putUint24(int value);
        // Format as an unsigned integer the least significant 24 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    bdex_TestOutStreamFormatter& putInt16(int value);
        // Format as a signed integer the least significant 16 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    bdex_TestOutStreamFormatter& putUint16(int value);
        // Format as an unsigned integer the least significant 16 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    bdex_TestOutStreamFormatter& putInt8(int value);
        // Format as a signed integer the least significant 8 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    bdex_TestOutStreamFormatter& putUint8(int value);
        // Format as an unsigned integer the least significant 8 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

                        // *** scalar floating-point values ***

    bdex_TestOutStreamFormatter& putFloat64(double value);
        // Format as an IEEE double-precision floating point number the most
        // significant 8 bytes in the specified 'value' to this output stream
        // and return a reference to this modifiable stream.  Note that, for
        // non-conforming platforms, the "most significant 8 bytes" might not
        // be contiguous bits, and that this operation may be lossy.

    bdex_TestOutStreamFormatter& putFloat32(float value);
        // Format as an IEEE single-precision floating point number the most
        // significant 4 bytes in the specified 'value' to this output stream
        // and return a reference to this modifiable stream.  Note that, for
        // non-conforming platforms, the "most significant 4 bytes" might not
        // be contiguous bits, and that this operation may be lossy.

                        // *** arrays of integer values ***

    bdex_TestOutStreamFormatter&
    putArrayInt64(const bsls_PlatformUtil::Int64 *array, int count);
        // Format as an atomic sequence of signed integers the least
        // significant 64 bits of each of the specified 'count' leading entries
        // in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= count'.

    bdex_TestOutStreamFormatter&
    putArrayUint64(const bsls_PlatformUtil::Uint64 *array, int count);
        // Format as an atomic sequence of unsigned integers the least
        // significant 64 bits of each of the specified 'count' leading entries
        // in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= count'.

    bdex_TestOutStreamFormatter&
    putArrayInt56(const bsls_PlatformUtil::Int64 *array, int count);
        // Format as an atomic sequence of signed integers the least
        // significant 56 bits of each of the specified 'count' leading entries
        // in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= count'.

    bdex_TestOutStreamFormatter&
    putArrayUint56(const bsls_PlatformUtil::Uint64 *array, int count);
        // Format as an atomic sequence of unsigned integers the least
        // significant 56 bits of each of the specified 'count' leading entries
        // in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= count'.

    bdex_TestOutStreamFormatter&
    putArrayInt48(const bsls_PlatformUtil::Int64 *array, int count);
        // Format as an atomic sequence of signed integers the least
        // significant 48 bits of each of the specified 'count' leading entries
        // in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= count'.

    bdex_TestOutStreamFormatter&
    putArrayUint48(const bsls_PlatformUtil::Uint64 *array, int count);
        // Format as an atomic sequence of unsigned integers the least
        // significant 48 bits of each of the specified 'count' leading entries
        // in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= count'.

    bdex_TestOutStreamFormatter&
    putArrayInt40(const bsls_PlatformUtil::Int64 *array, int count);
        // Format as an atomic sequence of signed integers the least
        // significant 40 bits of each of the specified 'count' leading entries
        // in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= count'.

    bdex_TestOutStreamFormatter&
    putArrayUint40(const bsls_PlatformUtil::Uint64 *array, int count);
        // Format as an atomic sequence of unsigned integers the least
        // significant 40 bits of each of the specified 'count' leading entries
        // in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= count'.

    bdex_TestOutStreamFormatter& putArrayInt32(const int *array, int count);
        // Format as an atomic sequence of signed integers the least
        // significant 32 bits of each of the specified 'count' leading entries
        // in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= count'.

    bdex_TestOutStreamFormatter&
    putArrayUint32(const unsigned int *array, int count);
        // Format as an atomic sequence of unsigned integers the least
        // significant 32 bits of each of the specified 'count' leading entries
        // in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= count'.

    bdex_TestOutStreamFormatter& putArrayInt24(const int *array, int count);
        // Format as an atomic sequence of signed integers the least
        // significant 24 bits of each of the specified 'count' leading entries
        // in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= count'.

    bdex_TestOutStreamFormatter&
    putArrayUint24(const unsigned int *array, int count);
        // Format as an atomic sequence of unsigned integers the least
        // significant 24 bits of each of the specified 'count' leading entries
        // in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= count'.

    bdex_TestOutStreamFormatter& putArrayInt16(const short *array, int count);
        // Format as an atomic sequence of signed integers the least
        // significant 16 bits of each of the specified 'count' leading entries
        // in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= count'.

    bdex_TestOutStreamFormatter&
    putArrayUint16(const unsigned short *array, int count);
        // Format as an atomic sequence of unsigned integers the least
        // significant 16 bits of each of the specified 'count' leading entries
        // in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= count'.

    bdex_TestOutStreamFormatter& putArrayInt8(const char *array, int count);
    bdex_TestOutStreamFormatter&
    putArrayInt8(const signed char *array, int count);
        // Format as an atomic sequence of signed integers the 8 bits of each
        // of the specified 'count' leading entries in the specified 'array' to
        // this output stream and return a reference to this modifiable stream.
        // The behavior is undefined unless '0 <= count'.

    bdex_TestOutStreamFormatter& putArrayUint8(const char *array, int count);
    bdex_TestOutStreamFormatter&
    putArrayUint8(const unsigned char *array, int count);
        // Format as an atomic sequence of unsigned integers the 8 bits of each
        // of the specified 'count' leading entries in the specified 'array' to
        // this output stream and return a reference to this modifiable stream.
        // The behavior is undefined unless '0 <= count'.

                        // *** arrays of floating-point values ***

    bdex_TestOutStreamFormatter&
    putArrayFloat64(const double *array, int count);
        // Format as an atomic sequence of IEEE double-precision floating point
        // numbers the most significant 8 bytes in each of the specified
        // 'count' leading entries in the specified 'array' to this output
        // stream and return a reference to this modifiable stream.  The
        // behavior is undefined unless '0 <= count'.  Note that, for
        // non-conforming platforms, the "most significant 8 bytes" might not
        // be contiguous bits, and that this operation may be lossy.

    bdex_TestOutStreamFormatter&
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
// TBD #ifndef BDE_OMIT_DEPRECATED

template <typename OBJECT>
inline
bdex_TestOutStreamFormatter& operator<<(bdex_TestOutStreamFormatter& stream,
                                        const OBJECT&       object);
    // Write to the specified output 'stream' the most current version number
    // supported by 'OBJECT' and format the value of the specified 'object' to
    // 'stream' using the 'bdex' compliant 'OBJECT::bdexStreamOut' method in
    // the 'OBJECT::maxSupportedBdexVersion' format.  Return a reference to the
    // modifiable 'stream'.  Note that specializations for the supported
    // fundamental (scalar) types and for 'bsl::string' are also provided, in
    // which case this operator calls the appropriate 'put' method from this
    // component.

// TBD #endif  // BDE_OMIT_DEPRECATED

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

// MANIPULATORS
inline
void bdex_TestOutStreamFormatter::invalidate()
{
    d_validFlag = 0;
}

inline
bdex_TestOutStreamFormatter& bdex_TestOutStreamFormatter::putString(
                                                        const bsl::string& str)
{
    const int length = (int) str.length();
    putLength(length);
    return putArrayUint8(str.data(), length);
}

inline void bdex_TestOutStreamFormatter::makeNextInvalid()
{
    d_makeNextInvalid = 1;
}

// ACCESSORS
inline
bdex_TestOutStreamFormatter::operator const void *() const
{
    return d_validFlag ? this : 0;
}

// FREE OPERATORS
// TBD #ifndef BDE_OMIT_DEPRECATED

template <typename VALUE>
inline
bdex_TestOutStreamFormatter& operator<<(bdex_TestOutStreamFormatter& stream,
                                        const VALUE&                 value)
{
    return bdex_OutStreamFunctions::streamOutVersionAndObject(stream, value);
}

// TBD #endif  // BDE_OMIT_DEPRECATED

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
