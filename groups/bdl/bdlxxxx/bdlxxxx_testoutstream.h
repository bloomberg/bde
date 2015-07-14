// bdlxxxx_testoutstream.h                                               -*-C++-*-
#ifndef INCLUDED_BDLXXXX_TESTOUTSTREAM
#define INCLUDED_BDLXXXX_TESTOUTSTREAM

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Enable externalization of fundamental types with identification.
//
//@CLASSES:
//   bdlxxxx::TestOutStream: byte-array-based output stream class
//
//@AUTHOR: Shao-wei Hung
//
//@SEE_ALSO: bdlxxxx_testinstream
//
//@DESCRIPTION: This component conforms to the 'bdlxxxx_outstream' (documentation
// only) "protocol", implementing a byte-array-based output stream class that
// provides platform-independent output methods ("externalization") on values,
// and arrays of values, of fundamental types, and on 'bsl::string'.
//
// This component is intended to be used in conjunction with the
// 'bdlxxxx_testinstream' "unexternalization" component.  Each output method of
// 'bdlxxxx::TestOutStream' writes a value or a homogeneous array of values to an
// internally managed buffer.  The values are formatted to be readable by the
// corresponding 'bdlxxxx::TestInStream' method.  In general, the user cannot rely
// on any other mechanism to read data written by 'bdlxxxx::TestOutStream' unless
// that mechanism explicitly states its ability to do so.
//
// The supported types and required content are listed in the table below.  All
// of the fundamental types in the table may be output as scalar values or as
// homogeneous arrays.  'bsl::string' is output as a logical scalar string.
// Note that 'Int64' and 'Uint64' denote 'bsls::Types::Int64' and
// 'bsls::Types::Uint64', which in turn are 'typedef' names for the
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
// format.  Also note that output streams can be *invalidated* explicitly and
// queried for *validity*.  Writing to an initially invalid stream has no
// effect.  Whenever an output operation fails, the stream should be
// invalidated explicitly.
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

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLXXXX_TESTOUTSTREAMFORMATTER
#include <bdlxxxx_testoutstreamformatter.h>
#endif

#ifndef INCLUDED_BDLSB_MEMOUTSTREAMBUF
#include <bdlsb_memoutstreambuf.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {

namespace bdlxxxx {
                        // ========================
                        // class TestOutStream
                        // ========================

class TestOutStream {
    // This class implements output methods to externalize fundamental types
    // and their associated type identification data in byte representation.
    // It stores the accumulated result in network byte order format.

    // IMPORTANT: d_streambuf and d_formatter must be initialized in the order
    // declared below.  Don't re-arrange these declarations!

    // DATA
    bdlsb::MemOutStreamBuf       d_streambuf;  // externalized values
    TestOutStreamFormatter d_formatter;  // formatter

    // FRIENDS
    friend bsl::ostream& operator<<(bsl::ostream&, const TestOutStream&);

    // NOT DEFINED
    TestOutStream(const TestOutStream&);
    TestOutStream& operator=(const TestOutStream&);

    bool operator==(const TestOutStream&) const;
    bool operator!=(const TestOutStream&) const;

  public:
    // CREATORS
    TestOutStream(bslma::Allocator *basicAllocator = 0);
        // Create an empty output byte stream.  Optionally specify the
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    ~TestOutStream();
        // Destroy this 'TestOutStream'.

    // MANIPULATORS
    void invalidate();
        // Put this output stream in an invalid state.  This function has no
        // effect if this stream is already invalid.  Note that this function
        // should be called whenever a write operation to this stream fails.

    void makeNextInvalid();
        // Make the next output operation format invalid data to this output
        // stream.  The invalid data can be detected by a corresponding
        // 'TestInStream' object.

    TestOutStream& putLength(int length);
        // Format the specified 'length' as a 8-bit unsigned integer if
        // 'length' is less than 128, otherwise as a 32-bit integer to this
        // output stream with its most significant bit set and return a
        // reference to this modifiable stream.

    TestOutStream& putString(const bsl::string& str);
        // Format as an array of 8-bit integers the specified 'str' to this
        // output stream and return a reference to this modifiable stream.

    TestOutStream& putVersion(int version);
        // Format the specified 'version' as an 8-bit unsigned integer to this
        // output stream and return a reference to this modifiable stream.

    template <typename TYPE>
    TestOutStream& put(const TYPE& variable);
        // TBD

                        // *** scalar integer values ***

    TestOutStream& putInt64(bsls::Types::Int64 value);
        // Format as a signed integer the least significant 64 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    TestOutStream& putUint64(bsls::Types::Int64 value);
        // Format as an unsigned integer the least significant 64 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    TestOutStream& putInt56(bsls::Types::Int64 value);
        // Format as a signed integer the least significant 56 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    TestOutStream& putUint56(bsls::Types::Int64 value);
        // Format as an unsigned integer the least significant 56 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    TestOutStream& putInt48(bsls::Types::Int64 value);
        // Format as a signed integer the least significant 48 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    TestOutStream& putUint48(bsls::Types::Int64 value);
        // Format as an unsigned integer the least significant 48 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    TestOutStream& putInt40(bsls::Types::Int64 value);
        // Format as a signed integer the least significant 40 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    TestOutStream& putUint40(bsls::Types::Int64 value);
        // Format as an unsigned integer the least significant 40 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    TestOutStream& putInt32(int value);
        // Format as a signed integer the least significant 32 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    TestOutStream& putUint32(unsigned int value);
        // Format as an unsigned integer the least significant 32 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    TestOutStream& putInt24(int value);
        // Format as a signed integer the least significant 24 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    TestOutStream& putUint24(int value);
        // Format as an unsigned integer the least significant 24 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    TestOutStream& putInt16(int value);
        // Format as a signed integer the least significant 16 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    TestOutStream& putUint16(int value);
        // Format as an unsigned integer the least significant 16 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    TestOutStream& putInt8(int value);
        // Format as a signed integer the least significant 8 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    TestOutStream& putUint8(int value);
        // Format as an unsigned integer the least significant 8 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

                        // *** scalar floating-point values ***

    TestOutStream& putFloat64(double value);
        // Format as an IEEE double-precision floating point number the most
        // significant 8 bytes in the specified 'value' to this output stream
        // and return a reference to this modifiable stream.  Note that, for
        // non-conforming platforms, the "most significant 8 bytes" might not
        // be contiguous bits, and that this operation may be lossy.

    TestOutStream& putFloat32(float value);
        // Format as an IEEE single-precision floating point number the most
        // significant 4 bytes in the specified 'value' to this output stream
        // and return a reference to this modifiable stream.  Note that, for
        // non-conforming platforms, the "most significant 4 bytes" might not
        // be contiguous bits, and that this operation may be lossy.

                        // *** arrays of integer values ***

    TestOutStream& putArrayInt64(const bsls::Types::Int64 *array,
                                      int                       length);
        // Format as an atomic sequence of signed integers the least
        // significant 64 bits of each of the specified 'length' leading
        // entries in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= length'.

    TestOutStream& putArrayUint64(const bsls::Types::Uint64 *array,
                                       int                        length);
        // Format as an atomic sequence of unsigned integers the least
        // significant 64 bits of each of the specified 'length' leading
        // entries in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= length'.

    TestOutStream& putArrayInt56(const bsls::Types::Int64 *array,
                                      int                       length);
        // Format as an atomic sequence of signed integers the least
        // significant 56 bits of each of the specified 'length' leading
        // entries in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= length'.

    TestOutStream& putArrayUint56(const bsls::Types::Uint64 *array,
                                       int                        length);
        // Format as an atomic sequence of unsigned integers the least
        // significant 56 bits of each of the specified 'length' leading
        // entries in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= length'.

    TestOutStream& putArrayInt48(const bsls::Types::Int64 *array,
                                      int                       length);
        // Format as an atomic sequence of signed integers the least
        // significant 48 bits of each of the specified 'length' leading
        // entries in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= length'.

    TestOutStream& putArrayUint48(const bsls::Types::Uint64 *array,
                                       int                        length);
        // Format as an atomic sequence of unsigned integers the least
        // significant 48 bits of each of the specified 'length' leading
        // entries in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= length'.

    TestOutStream& putArrayInt40(const bsls::Types::Int64 *array,
                                      int                       length);
        // Format as an atomic sequence of signed integers the least
        // significant 40 bits of each of the specified 'length' leading
        // entries in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= length'.

    TestOutStream& putArrayUint40(const bsls::Types::Uint64 *array,
                                       int                        length);
        // Format as an atomic sequence of unsigned integers the least
        // significant 40 bits of each of the specified 'length' leading
        // entries in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= length'.

    TestOutStream& putArrayInt32(const int *array, int length);
        // Format as an atomic sequence of signed integers the least
        // significant 32 bits of each of the specified 'length' leading
        // entries in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= length'.

    TestOutStream& putArrayUint32(const unsigned int *array, int length);
        // Format as an atomic sequence of unsigned integers the least
        // significant 32 bits of each of the specified 'length' leading
        // entries in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= length'.

    TestOutStream& putArrayInt24(const int *array, int length);
        // Format as an atomic sequence of signed integers the least
        // significant 24 bits of each of the specified 'length' leading
        // entries in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= length'.

    TestOutStream& putArrayUint24(const unsigned int *array, int length);
        // Format as an atomic sequence of unsigned integers the least
        // significant 24 bits of each of the specified 'length' leading
        // entries in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= length'.

    TestOutStream& putArrayInt16(const short *array, int length);
        // Format as an atomic sequence of signed integers the least
        // significant 16 bits of each of the specified 'length' leading
        // entries in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= length'.

    TestOutStream& putArrayUint16(const unsigned short *array,
                                           int                   length);
        // Format as an atomic sequence of unsigned integers the least
        // significant 16 bits of each of the specified 'length' leading
        // entries in the specified 'array' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= length'.

    TestOutStream& putArrayInt8(const char *array, int length);
    TestOutStream& putArrayInt8(const signed char *array, int length);
        // Format as an atomic sequence of signed integers the 8 bits of each
        // of the specified 'length' leading entries in the specified 'array'
        // to this output stream and return a reference to this modifiable
        // stream.  The behavior is undefined unless '0 <= length'.

    TestOutStream& putArrayUint8(const char *array, int length);
    TestOutStream& putArrayUint8(const unsigned char *array, int length);
        // Format as an atomic sequence of unsigned integers the 8 bits of each
        // of the specified 'length' leading entries in the specified 'array'
        // to this output stream and return a reference to this modifiable
        // stream.  The behavior is undefined unless '0 <= length'.

                        // *** arrays of floating-point values ***

    TestOutStream& putArrayFloat64(const double *array, int length);
        // Format as an atomic sequence of IEEE double-precision floating point
        // numbers the most significant 8 bytes in each of the specified
        // 'length' leading entries in the specified 'array' to this output
        // stream and return a reference to this modifiable stream.  The
        // behavior is undefined unless '0 <= length'.  Note that, for
        // non-conforming platforms, the "most significant 8 bytes" might not
        // be contiguous bits, and that this operation may be lossy.

    TestOutStream& putArrayFloat32(const float *array, int length);
        // Format as an atomic sequence of IEEE single-precision floating point
        // numbers the most significant 4 bytes in each of the specified
        // 'length' leading entries in the specified 'array' to this output
        // stream and return a reference to this modifiable stream.  The
        // behavior is undefined unless '0 <= length'.  Note that, for
        // non-conforming platforms, the "most significant 4 bytes" might not
        // be contiguous bits, and that this operation may be lossy.

    void removeAll();
        // Remove all content in this stream.

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
bsl::ostream& operator<<(bsl::ostream&             stream,
                         const TestOutStream& obj);
    // Write the specified 'obj' to the specified output 'stream' in some
    // reasonable (multi-line) format, and return a reference to 'stream'

template <typename VALUE>
inline
TestOutStream& operator<<(TestOutStream& stream, const VALUE& value);
    // TBD doc

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        // ------------------------
                        // class TestOutStream
                        // ------------------------

// MANIPULATORS
inline
void TestOutStream::invalidate()
{
    d_formatter.invalidate();
}

inline
TestOutStream& TestOutStream::putString(const bsl::string& str)
{
    d_formatter.putString(str);
    return *this;
}

template <typename TYPE>
inline
TestOutStream& TestOutStream::put(const TYPE& variable)
{
    return OutStreamMethodsUtil::streamOutVersionAndObject(*this,
                                                                variable);
}

inline void TestOutStream::removeAll()
{
    d_streambuf.pubseekpos(0);
}

inline void TestOutStream::makeNextInvalid()
{
    d_formatter.makeNextInvalid();
}
}  // close package namespace

// ACCESSORS
inline
bdlxxxx::TestOutStream::operator const void *() const
{
    return d_formatter ? this : 0;
}

namespace bdlxxxx {
inline
const char *TestOutStream::data() const
{
    return (const char *)d_streambuf.data();
}

inline
int TestOutStream::length() const
{
    return (int)d_streambuf.length();
}
}  // close package namespace

// FREE OPERATORS
template <typename VALUE>
inline
bdlxxxx::TestOutStream& bdlxxxx::operator<<(TestOutStream& stream, const VALUE& value)
{
    return stream.put(value);
}

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
