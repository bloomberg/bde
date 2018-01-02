// bslx_byteoutstream.h                                               -*-C++-*-
#ifndef INCLUDED_BSLX_BYTEOUTSTREAM
#define INCLUDED_BSLX_BYTEOUTSTREAM

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a stream class for externalization of fundamental types.
//
//@CLASSES:
//  bslx::ByteOutStream: byte-array-based output stream for fundamental types
//
//@SEE_ALSO: bslx_byteinstream
//
//@DESCRIPTION: This component implements a byte-array-based output stream
// class, 'bslx::ByteOutStream', that provides platform-independent output
// methods ("externalization") on values, and arrays of values, of fundamental
// types, and on 'bsl::string'.
//
// This component is intended to be used in conjunction with the
// 'bslx_byteinstream' "unexternalization" component.  Each output method of
// 'bslx::ByteOutStream' writes a value or a homogeneous array of values to an
// internally managed buffer.  The values are formatted to be readable by the
// corresponding 'bslx::ByteInStream' method.  In general, the user cannot rely
// on any other mechanism to read data written by 'bslx::ByteOutStream' unless
// that mechanism explicitly states its ability to do so.
//
// The supported types and required content are listed in the 'bslx'
// package-level documentation under "Supported Types".
//
// Note that the values are stored in big-endian (i.e., network byte order)
// format.
//
// Note that output streams can be *invalidated* explicitly and queried for
// *validity*.  Writing to an initially invalid stream has no effect.  Whenever
// an output operation fails, the stream should be invalidated explicitly.
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
/// - - - - - - - - - - - - - - - -
// A 'bslx::ByteOutStream' can be used to externalize values in a
// platform-neutral way.  Writing out fundamental C++ types and 'bsl::string'
// requires no additional work on the part of the client; the client can simply
// use the stream directly.  The following code serializes a few representative
// values using a 'bslx::ByteOutStream', compares the contents of this stream
// to the expected value, and then writes the contents of this stream's buffer
// to 'stdout'.
//
// First, we create a 'bslx::ByteOutStream' with an arbitrary value for its
// 'versionSelector' and externalize some values:
//..
//  bslx::ByteOutStream outStream(20131127);
//  outStream.putInt32(1);
//  outStream.putInt32(2);
//  outStream.putInt8('c');
//  outStream.putString(bsl::string("hello"));
//..
// Then, we compare the contents of the stream to the expected value:
//..
//  const char  *theChars = outStream.data();
//  bsl::size_t  length   = outStream.length();
//  assert(15 == length);
//  assert( 0 == bsl::memcmp(theChars,
//                           "\x00\x00\x00\x01\x00\x00\x00\x02""c\x05""hello",
//                           length));
//..
// Finally, we print the stream's contents to 'bsl::cout'.
//..
//  for (bsl::size_t i = 0; i < length; ++i) {
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
// See the 'bslx_byteinstream' component usage example for a more practical
// example of using 'bslx' streams.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLX_MARSHALLINGUTIL
#include <bslx_marshallingutil.h>
#endif

#ifndef INCLUDED_BSLX_OUTSTREAMFUNCTIONS
#include <bslx_outstreamfunctions.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PERFORMANCEHINT
#include <bsls_performancehint.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_CSTDDEF
#include <bsl_cstddef.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {
namespace bslx {

                         // ===================
                         // class ByteOutStream
                         // ===================

class ByteOutStream {
    // This class provides output methods to externalize values, and C-style
    // arrays of values, of the fundamental integral and floating-point types,
    // as well as 'bsl::string' values.  In particular, each 'put' method of
    // this class is guaranteed to write stream data that can be read by the
    // corresponding 'get' method of 'bslx::ByteInStream'.  See the 'bslx'
    // package-level documentation for the definition of the BDEX 'OutStream'
    // protocol.

    // DATA
    bsl::vector<char> d_buffer;     // byte buffer to write to

    int               d_versionSelector;
                                    // 'versionSelector' to use with
                                    // 'operator<<' as per the 'bslx'
                                    // package-level documentation

    int               d_validFlag;  // stream validity flag; 'true' if stream
                                    // is in valid state, 'false' otherwise

    // FRIENDS
    friend bsl::ostream& operator<<(bsl::ostream&, const ByteOutStream&);

    // NOT IMPLEMENTED
    ByteOutStream(const ByteOutStream&);
    ByteOutStream& operator=(const ByteOutStream&);

  private:
    // PRIVATE MANIPULATORS
    void validate();
        // Put this output stream into a valid state.  This function has no
        // effect if this stream is already valid.

  public:
    // CREATORS
    explicit ByteOutStream(int               versionSelector,
                           bslma::Allocator *basicAllocator = 0);
        // Create an empty output byte stream that will use the specified
        // (*compile*-time-defined) 'versionSelector' as needed (see
        // {Versioning}).  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  Note that the 'versionSelector' is expected to
        // be formatted as "YYYYMMDD", a date representation.

    ByteOutStream(int               versionSelector,
                  bsl::size_t       initialCapacity,
                  bslma::Allocator *basicAllocator = 0);
        // Create an empty output byte stream having an initial buffer capacity
        // of at least the specified 'initialCapacity' (in bytes) and that will
        // use the specified (*compile*-time-defined) 'versionSelector' as
        // needed (see {Versioning}).  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.  Note that the
        // 'versionSelector' is expected to be formatted as "YYYYMMDD", a date
        // representation.

    ~ByteOutStream();
        // Destroy this object.

    // MANIPULATORS
    void invalidate();
        // Put this output stream in an invalid state.  This function has no
        // effect if this stream is already invalid.

    ByteOutStream& putLength(int length);
        // If the specified 'length' is less than 128, write to this stream the
        // one-byte integer comprised of the least-significant one byte of the
        // 'length'; otherwise, write to this stream the four-byte, two's
        // complement integer (in network byte order) comprised of the
        // least-significant four bytes of the 'length' (in host byte order)
        // with the most-significant bit set.  Return a reference to this
        // stream.  If this stream is initially invalid, this operation has no
        // effect.  The behavior is undefined unless '0 <= length'.

    ByteOutStream& putVersion(int version);
        // Write to this stream the one-byte, two's complement unsigned integer
        // comprised of the least-significant one byte of the specified
        // 'version', and return a reference to this stream.  If this stream is
        // initially invalid, this operation has no effect.

    void reserveCapacity(bsl::size_t newCapacity);
        // Set the internal buffer size of this stream to be at least the
        // specified 'newCapacity' (in bytes).

    void reset();
        // Remove all content in this stream and validate this stream if it is
        // currently invalid.

                      // *** scalar integer values ***

    ByteOutStream& putInt64(bsls::Types::Int64 value);
        // Write to this stream the eight-byte, two's complement integer (in
        // network byte order) comprised of the least-significant eight bytes
        // of the specified 'value' (in host byte order), and return a
        // reference to this stream.  If this stream is initially invalid, this
        // operation has no effect.

    ByteOutStream& putUint64(bsls::Types::Uint64 value);
        // Write to this stream the eight-byte, two's complement unsigned
        // integer (in network byte order) comprised of the least-significant
        // eight bytes of the specified 'value' (in host byte order), and
        // return a reference to this stream.  If this stream is initially
        // invalid, this operation has no effect.

    ByteOutStream& putInt56(bsls::Types::Int64 value);
        // Write to this stream the seven-byte, two's complement integer (in
        // network byte order) comprised of the least-significant seven bytes
        // of the specified 'value' (in host byte order), and return a
        // reference to this stream.  If this stream is initially invalid, this
        // operation has no effect.

    ByteOutStream& putUint56(bsls::Types::Uint64 value);
        // Write to this stream the seven-byte, two's complement unsigned
        // integer (in network byte order) comprised of the least-significant
        // seven bytes of the specified 'value' (in host byte order), and
        // return a reference to this stream.  If this stream is initially
        // invalid, this operation has no effect.

    ByteOutStream& putInt48(bsls::Types::Int64 value);
        // Write to this stream the six-byte, two's complement integer (in
        // network byte order) comprised of the least-significant six bytes of
        // the specified 'value' (in host byte order), and return a reference
        // to this stream.  If this stream is initially invalid, this operation
        // has no effect.

    ByteOutStream& putUint48(bsls::Types::Uint64 value);
        // Write to this stream the six-byte, two's complement unsigned integer
        // (in network byte order) comprised of the least-significant six bytes
        // of the specified 'value' (in host byte order), and return a
        // reference to this stream.  If this stream is initially invalid, this
        // operation has no effect.

    ByteOutStream& putInt40(bsls::Types::Int64 value);
        // Write to this stream the five-byte, two's complement integer (in
        // network byte order) comprised of the least-significant five bytes of
        // the specified 'value' (in host byte order), and return a reference
        // to this stream.  If this stream is initially invalid, this operation
        // has no effect.

    ByteOutStream& putUint40(bsls::Types::Uint64 value);
        // Write to this stream the five-byte, two's complement unsigned
        // integer (in network byte order) comprised of the least-significant
        // five bytes of the specified 'value' (in host byte order), and return
        // a reference to this stream.  If this stream is initially invalid,
        // this operation has no effect.

    ByteOutStream& putInt32(int value);
        // Write to this stream the four-byte, two's complement integer (in
        // network byte order) comprised of the least-significant four bytes of
        // the specified 'value' (in host byte order), and return a reference
        // to this stream.  If this stream is initially invalid, this operation
        // has no effect.

    ByteOutStream& putUint32(unsigned int value);
        // Write to this stream the four-byte, two's complement unsigned
        // integer (in network byte order) comprised of the least-significant
        // four bytes of the specified 'value' (in host byte order), and return
        // a reference to this stream.  If this stream is initially invalid,
        // this operation has no effect.

    ByteOutStream& putInt24(int value);
        // Write to this stream the three-byte, two's complement integer (in
        // network byte order) comprised of the least-significant three bytes
        // of the specified 'value' (in host byte order), and return a
        // reference to this stream.  If this stream is initially invalid, this
        // operation has no effect.

    ByteOutStream& putUint24(unsigned int value);
        // Write to this stream the three-byte, two's complement unsigned
        // integer (in network byte order) comprised of the least-significant
        // three bytes of the specified 'value' (in host byte order), and
        // return a reference to this stream.  If this stream is initially
        // invalid, this operation has no effect.

    ByteOutStream& putInt16(int value);
        // Write to this stream the two-byte, two's complement integer (in
        // network byte order) comprised of the least-significant two bytes of
        // the specified 'value' (in host byte order), and return a reference
        // to this stream.  If this stream is initially invalid, this operation
        // has no effect.

    ByteOutStream& putUint16(unsigned int value);
        // Write to this stream the two-byte, two's complement unsigned integer
        // (in network byte order) comprised of the least-significant two bytes
        // of the specified 'value' (in host byte order), and return a
        // reference to this stream.  If this stream is initially invalid, this
        // operation has no effect.

    ByteOutStream& putInt8(int value);
        // Write to this stream the one-byte, two's complement integer
        // comprised of the least-significant one byte of the specified
        // 'value', and return a reference to this stream.  If this stream is
        // initially invalid, this operation has no effect.

    ByteOutStream& putUint8(unsigned int value);
        // Write to this stream the one-byte, two's complement unsigned integer
        // comprised of the least-significant one byte of the specified
        // 'value', and return a reference to this stream.  If this stream is
        // initially invalid, this operation has no effect.

                      // *** scalar floating-point values ***

    ByteOutStream& putFloat64(double value);
        // Write to this stream the eight-byte IEEE double-precision
        // floating-point number (in network byte order) comprised of the
        // most-significant eight bytes of the specified 'value' (in host byte
        // order), and return a reference to this stream.  If this stream is
        // initially invalid, this operation has no effect.  Note that for
        // non-conforming platforms, this operation may be lossy.

    ByteOutStream& putFloat32(float value);
        // Write to this stream the four-byte IEEE single-precision
        // floating-point number (in network byte order) comprised of the
        // most-significant four bytes of the specified 'value' (in host byte
        // order), and return a reference to this stream.  If this stream is
        // initially invalid, this operation has no effect.  Note that for
        // non-conforming platforms, this operation may be lossy.

                      // *** string values ***

    ByteOutStream& putString(const bsl::string& value);
        // Write to this stream the length of the specified 'value' (see
        // 'putLength') and an array of one-byte, two's complement unsigned
        // integers comprised of the least-significant one byte of each
        // character in the 'value', and return a reference to this stream.  If
        // this stream is initially invalid, this operation has no effect.

                      // *** arrays of integer values ***

    ByteOutStream& putArrayInt64(const bsls::Types::Int64 *values,
                                 int                       numValues);
        // Write to this stream the consecutive eight-byte, two's complement
        // integers (in network byte order) comprised of the least-significant
        // eight bytes of each of the specified 'numValues' leading entries in
        // the specified 'values' (in host byte order), and return a reference
        // to this stream.  If this stream is initially invalid, this operation
        // has no effect.  The behavior is undefined unless '0 <= numValues'
        // and 'values' has sufficient contents.

    ByteOutStream& putArrayUint64(const bsls::Types::Uint64 *values,
                                  int                        numValues);
        // Write to this stream the consecutive eight-byte, two's complement
        // unsigned integers (in network byte order) comprised of the
        // least-significant eight bytes of each of the specified 'numValues'
        // leading entries in the specified 'values' (in host byte order), and
        // return a reference to this stream.  If this stream is initially
        // invalid, this operation has no effect.  The behavior is undefined
        // unless '0 <= numValues' and 'values' has sufficient contents.

    ByteOutStream& putArrayInt56(const bsls::Types::Int64 *values,
                                 int                       numValues);
        // Write to this stream the consecutive seven-byte, two's complement
        // integers (in network byte order) comprised of the least-significant
        // seven bytes of each of the specified 'numValues' leading entries in
        // the specified 'values' (in host byte order), and return a reference
        // to this stream.  If this stream is initially invalid, this operation
        // has no effect.  The behavior is undefined unless '0 <= numValues'
        // and 'values' has sufficient contents.

    ByteOutStream& putArrayUint56(const bsls::Types::Uint64 *values,
                                  int                        numValues);
        // Write to this stream the consecutive seven-byte, two's complement
        // unsigned integers (in network byte order) comprised of the
        // least-significant seven bytes of each of the specified 'numValues'
        // leading entries in the specified 'values' (in host byte order), and
        // return a reference to this stream.  If this stream is initially
        // invalid, this operation has no effect.  The behavior is undefined
        // unless '0 <= numValues' and 'values' has sufficient contents.

    ByteOutStream& putArrayInt48(const bsls::Types::Int64 *values,
                                 int                       numValues);
        // Write to this stream the consecutive six-byte, two's complement
        // integers (in network byte order) comprised of the least-significant
        // six bytes of each of the specified 'numValues' leading entries in
        // the specified 'values' (in host byte order), and return a reference
        // to this stream.  If this stream is initially invalid, this operation
        // has no effect.  The behavior is undefined unless '0 <= numValues'
        // and 'values' has sufficient contents.

    ByteOutStream& putArrayUint48(const bsls::Types::Uint64 *values,
                                  int                        numValues);
        // Write to this stream the consecutive six-byte, two's complement
        // unsigned integers (in network byte order) comprised of the
        // least-significant six bytes of each of the specified 'numValues'
        // leading entries in the specified 'values' (in host byte order), and
        // return a reference to this stream.  If this stream is initially
        // invalid, this operation has no effect.  The behavior is undefined
        // unless '0 <= numValues' and 'values' has sufficient contents.

    ByteOutStream& putArrayInt40(const bsls::Types::Int64 *values,
                                 int                       numValues);
        // Write to this stream the consecutive five-byte, two's complement
        // integers (in network byte order) comprised of the least-significant
        // five bytes of each of the specified 'numValues' leading entries in
        // the specified 'values' (in host byte order), and return a reference
        // to this stream.  If this stream is initially invalid, this operation
        // has no effect.  The behavior is undefined unless '0 <= numValues'
        // and 'values' has sufficient contents.

    ByteOutStream& putArrayUint40(const bsls::Types::Uint64 *values,
                                  int                        numValues);
        // Write to this stream the consecutive five-byte, two's complement
        // unsigned integers (in network byte order) comprised of the
        // least-significant five bytes of each of the specified 'numValues'
        // leading entries in the specified 'values' (in host byte order), and
        // return a reference to this stream.  If this stream is initially
        // invalid, this operation has no effect.  The behavior is undefined
        // unless '0 <= numValues' and 'values' has sufficient contents.

    ByteOutStream& putArrayInt32(const int *values, int numValues);
        // Write to this stream the consecutive four-byte, two's complement
        // integers (in network byte order) comprised of the least-significant
        // four bytes of each of the specified 'numValues' leading entries in
        // the specified 'values' (in host byte order), and return a reference
        // to this stream.  If this stream is initially invalid, this operation
        // has no effect.  The behavior is undefined unless '0 <= numValues'
        // and 'values' has sufficient contents.

    ByteOutStream& putArrayUint32(const unsigned int *values, int numValues);
        // Write to this stream the consecutive four-byte, two's complement
        // unsigned integers (in network byte order) comprised of the
        // least-significant four bytes of each of the specified 'numValues'
        // leading entries in the specified 'values' (in host byte order), and
        // return a reference to this stream.  If this stream is initially
        // invalid, this operation has no effect.  The behavior is undefined
        // unless '0 <= numValues' and 'values' has sufficient contents.

    ByteOutStream& putArrayInt24(const int *values, int numValues);
        // Write to this stream the consecutive three-byte, two's complement
        // integers (in network byte order) comprised of the least-significant
        // three bytes of each of the specified 'numValues' leading entries in
        // the specified 'values' (in host byte order), and return a reference
        // to this stream.  If this stream is initially invalid, this operation
        // has no effect.  The behavior is undefined unless '0 <= numValues'
        // and 'values' has sufficient contents.

    ByteOutStream& putArrayUint24(const unsigned int *values, int numValues);
        // Write to this stream the consecutive three-byte, two's complement
        // unsigned integers (in network byte order) comprised of the
        // least-significant three bytes of each of the specified 'numValues'
        // leading entries in the specified 'values' (in host byte order), and
        // return a reference to this stream.  If this stream is initially
        // invalid, this operation has no effect.  The behavior is undefined
        // unless '0 <= numValues' and 'values' has sufficient contents.

    ByteOutStream& putArrayInt16(const short *values, int numValues);
        // Write to this stream the consecutive two-byte, two's complement
        // integers (in network byte order) comprised of the least-significant
        // two bytes of each of the specified 'numValues' leading entries in
        // the specified 'values' (in host byte order), and return a reference
        // to this stream.  If this stream is initially invalid, this operation
        // has no effect.  The behavior is undefined unless '0 <= numValues'
        // and 'values' has sufficient contents.

    ByteOutStream& putArrayUint16(const unsigned short *values, int numValues);
        // Write to this stream the consecutive two-byte, two's complement
        // unsigned integers (in network byte order) comprised of the
        // least-significant two bytes of each of the specified 'numValues'
        // leading entries in the specified 'values' (in host byte order), and
        // return a reference to this stream.  If this stream is initially
        // invalid, this operation has no effect.  The behavior is undefined
        // unless '0 <= numValues' and 'values' has sufficient contents.

    ByteOutStream& putArrayInt8(const char        *values, int numValues);
    ByteOutStream& putArrayInt8(const signed char *values, int numValues);
        // Write to this stream the consecutive one-byte, two's complement
        // integers comprised of the least-significant one byte of each of the
        // specified 'numValues' leading entries in the specified 'values', and
        // return a reference to this stream.  If this stream is initially
        // invalid, this operation has no effect.  The behavior is undefined
        // unless '0 <= numValues' and 'values' has sufficient contents.

    ByteOutStream& putArrayUint8(const char          *values, int numValues);
    ByteOutStream& putArrayUint8(const unsigned char *values, int numValues);
        // Write to this stream the consecutive one-byte, two's complement
        // unsigned integers comprised of the least-significant one byte of
        // each of the specified 'numValues' leading entries in the specified
        // 'values', and return a reference to this stream.  If this stream is
        // initially invalid, this operation has no effect.  The behavior is
        // undefined unless '0 <= numValues' and 'values' has sufficient
        // contents.

                      // *** arrays of floating-point values ***

    ByteOutStream& putArrayFloat64(const double *values, int numValues);
        // Write to this stream the consecutive eight-byte IEEE
        // double-precision floating-point numbers (in network byte order)
        // comprised of the most-significant eight bytes of each of the
        // specified 'numValues' leading entries in the specified 'values' (in
        // host byte order), and return a reference to this stream.  If this
        // stream is initially invalid, this operation has no effect.  The
        // behavior is undefined unless '0 <= numValues' and 'values' has
        // sufficient contents.  Note that for non-conforming platforms, this
        // operation may be lossy.

    ByteOutStream& putArrayFloat32(const float *values, int numValues);
        // Write to this stream the consecutive four-byte IEEE single-precision
        // floating-point numbers (in network byte order) comprised of the
        // most-significant four bytes of each of the specified 'numValues'
        // leading entries in the specified 'values' (in host byte order), and
        // return a reference to this stream.  If this stream is initially
        // invalid, this operation has no effect.  The behavior is undefined
        // unless '0 <= numValues' and 'values' has sufficient contents.  Note
        // that for non-conforming platforms, this operation may be lossy.

    // ACCESSORS
    operator const void *() const;
        // Return a non-zero value if this stream is valid, and 0 otherwise.
        // An invalid stream is a stream for which an output operation was
        // detected to have failed or 'invalidate' was called.

    int bdexVersionSelector() const;
        // Return the 'versionSelector' to be used with 'operator<<' for BDEX
        // streaming as per the 'bslx' package-level documentation.

    const char *data() const;
        // Return the address of the contiguous, non-modifiable internal memory
        // buffer of this stream.  The address will remain valid as long as
        // this stream is not destroyed or modified.  The behavior of accessing
        // elements outside the range '[ data() .. data() + (length() - 1) ]'
        // is undefined.

    bool isValid() const;
        // Return 'true' if this stream is valid, and 'false' otherwise.  An
        // invalid stream is a stream for which an output operation was
        // detected to have failed or 'invalidate' was called.

    bsl::size_t length() const;
        // Return the number of bytes in this stream.
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&        stream,
                         const ByteOutStream& object);
    // Write the specified 'object' to the specified output 'stream' in some
    // reasonable (multi-line) format, and return a reference to 'stream'.

template <class TYPE>
ByteOutStream& operator<<(ByteOutStream& stream, const TYPE& value);
    // Write the specified 'value' to the specified output 'stream' following
    // the requirements of the BDEX protocol (see the 'bslx' package-level
    // documentation), and return a reference to 'stream'.  The behavior is
    // undefined unless 'TYPE' is BDEX-compliant.

// ============================================================================
//                          INLINE DEFINITIONS
// ============================================================================

                         // -------------------
                         // class ByteOutStream
                         // -------------------

// PRIVATE MANIPULATORS
inline
void ByteOutStream::validate()
{
    d_validFlag = true;
}

// CREATORS
inline
ByteOutStream::ByteOutStream(int               versionSelector,
                             bslma::Allocator *basicAllocator)
: d_buffer(basicAllocator)
, d_versionSelector(versionSelector)
, d_validFlag(true)
{
}

inline
ByteOutStream::ByteOutStream(int               versionSelector,
                             bsl::size_t       initialCapacity,
                             bslma::Allocator *basicAllocator)
: d_buffer(basicAllocator)
, d_versionSelector(versionSelector)
, d_validFlag(true)
{
    d_buffer.reserve(initialCapacity);
}

inline
ByteOutStream::~ByteOutStream()
{
}

// MANIPULATORS
inline
void ByteOutStream::invalidate()
{
    d_validFlag = false;
}

inline
ByteOutStream& ByteOutStream::putLength(int length)
{
    BSLS_ASSERT_SAFE(0 <= length);

    if (length > 127) {
        putInt32(length | (1 << 31));
    } else {
        putInt8(length);
    }
    return *this;
}

inline
ByteOutStream& ByteOutStream::putVersion(int version)
{
    return putUint8(version);
}

inline
void ByteOutStream::reserveCapacity(bsl::size_t newCapacity)
{
    d_buffer.reserve(newCapacity);
}

inline
void ByteOutStream::reset()
{
    d_buffer.clear();
    validate();
}

                      // *** scalar integer values ***

inline
ByteOutStream& ByteOutStream::putInt64(bsls::Types::Int64 value)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    // Resize the buffer to have sufficient capacity with care to ensure this
    // stream is invalidated if an exception is thrown.

    const bsl::size_t n = d_buffer.size();
    invalidate();
    d_buffer.resize(n + MarshallingUtil::k_SIZEOF_INT64);
    validate();

    // Write to the buffer the specified 'value'.

    MarshallingUtil::putInt64(d_buffer.data() + n, value);

    return *this;
}

inline
ByteOutStream& ByteOutStream::putUint64(bsls::Types::Uint64 value)
{
    return putInt64(static_cast<bsls::Types::Int64>(value));
}

inline
ByteOutStream& ByteOutStream::putInt56(bsls::Types::Int64 value)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    // Resize the buffer to have sufficient capacity with care to ensure this
    // stream is invalidated if an exception is thrown.

    const bsl::size_t n = d_buffer.size();
    invalidate();
    d_buffer.resize(n + MarshallingUtil::k_SIZEOF_INT56);
    validate();

    // Write to the buffer the specified 'value'.

    MarshallingUtil::putInt56(d_buffer.data() + n, value);

    return *this;
}

inline
ByteOutStream& ByteOutStream::putUint56(bsls::Types::Uint64 value)
{
    return putInt56(static_cast<bsls::Types::Int64>(value));
}

inline
ByteOutStream& ByteOutStream::putInt48(bsls::Types::Int64 value)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    // Resize the buffer to have sufficient capacity with care to ensure this
    // stream is invalidated if an exception is thrown.

    const bsl::size_t n = d_buffer.size();
    invalidate();
    d_buffer.resize(n + MarshallingUtil::k_SIZEOF_INT48);
    validate();

    // Write to the buffer the specified 'value'.

    MarshallingUtil::putInt48(d_buffer.data() + n, value);

    return *this;
}

inline
ByteOutStream& ByteOutStream::putUint48(bsls::Types::Uint64 value)
{
    return putInt48(static_cast<bsls::Types::Int64>(value));
}

inline
ByteOutStream& ByteOutStream::putInt40(bsls::Types::Int64 value)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    // Resize the buffer to have sufficient capacity with care to ensure this
    // stream is invalidated if an exception is thrown.

    const bsl::size_t n = d_buffer.size();
    invalidate();
    d_buffer.resize(n + MarshallingUtil::k_SIZEOF_INT40);
    validate();

    // Write to the buffer the specified 'value'.

    MarshallingUtil::putInt40(d_buffer.data() + n, value);

    return *this;
}

inline
ByteOutStream& ByteOutStream::putUint40(bsls::Types::Uint64 value)
{
    return putInt40(static_cast<bsls::Types::Int64>(value));
}

inline
ByteOutStream& ByteOutStream::putInt32(int value)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    // Resize the buffer to have sufficient capacity with care to ensure this
    // stream is invalidated if an exception is thrown.

    const bsl::size_t n = d_buffer.size();
    invalidate();
    d_buffer.resize(n + MarshallingUtil::k_SIZEOF_INT32);
    validate();

    // Write to the buffer the specified 'value'.

    MarshallingUtil::putInt32(d_buffer.data() + n, value);

    return *this;
}

inline
ByteOutStream& ByteOutStream::putUint32(unsigned int value)
{
    return putInt32(static_cast<int>(value));
}

inline
ByteOutStream& ByteOutStream::putInt24(int value)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    // Resize the buffer to have sufficient capacity with care to ensure this
    // stream is invalidated if an exception is thrown.

    const bsl::size_t n = d_buffer.size();
    invalidate();
    d_buffer.resize(n + MarshallingUtil::k_SIZEOF_INT24);
    validate();

    // Write to the buffer the specified 'value'.

    MarshallingUtil::putInt24(d_buffer.data() + n, value);

    return *this;
}

inline
ByteOutStream& ByteOutStream::putUint24(unsigned int value)
{
    return putInt24(static_cast<int>(value));
}

inline
ByteOutStream& ByteOutStream::putInt16(int value)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    // Resize the buffer to have sufficient capacity with care to ensure this
    // stream is invalidated if an exception is thrown.

    const bsl::size_t n = d_buffer.size();
    invalidate();
    d_buffer.resize(n + MarshallingUtil::k_SIZEOF_INT16);
    validate();

    // Write to the buffer the specified 'value'.

    MarshallingUtil::putInt16(d_buffer.data() + n, value);

    return *this;
}

inline
ByteOutStream& ByteOutStream::putUint16(unsigned int value)
{
    return putInt16(static_cast<int>(value));
}

inline
ByteOutStream& ByteOutStream::putInt8(int value)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    // Resize the buffer to have sufficient capacity with care to ensure this
    // stream is invalidated if an exception is thrown.

    const bsl::size_t n = d_buffer.size();
    invalidate();
    d_buffer.resize(n + MarshallingUtil::k_SIZEOF_INT8);
    validate();

    // Write to the buffer the specified 'value'.

    MarshallingUtil::putInt8(d_buffer.data() + n, value);

    return *this;
}

inline
ByteOutStream& ByteOutStream::putUint8(unsigned int value)
{
    return putInt8(static_cast<int>(value));
}

                      // *** scalar floating-point values ***

inline
ByteOutStream& ByteOutStream::putFloat64(double value)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    // Resize the buffer to have sufficient capacity with care to ensure this
    // stream is invalidated if an exception is thrown.

    const bsl::size_t n = d_buffer.size();
    invalidate();
    d_buffer.resize(n + MarshallingUtil::k_SIZEOF_FLOAT64);
    validate();

    // Write to the buffer the specified 'value'.

    MarshallingUtil::putFloat64(d_buffer.data() + n, value);

    return *this;
}

inline
ByteOutStream& ByteOutStream::putFloat32(float value)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    // Resize the buffer to have sufficient capacity with care to ensure this
    // stream is invalidated if an exception is thrown.

    const bsl::size_t n = d_buffer.size();
    invalidate();
    d_buffer.resize(n + MarshallingUtil::k_SIZEOF_FLOAT32);
    validate();

    // Write to the buffer the specified 'value'.

    MarshallingUtil::putFloat32(d_buffer.data() + n, value);

    return *this;
}

                      // *** arrays of integer values ***

inline
ByteOutStream& ByteOutStream::putArrayInt64(
                                           const bsls::Types::Int64 *values,
                                           int                       numValues)
{
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid() || 0 == numValues)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    // Resize the buffer to have sufficient capacity with care to ensure this
    // stream is invalidated if an exception is thrown.

    const bsl::size_t n = d_buffer.size();
    invalidate();
    d_buffer.resize(n + numValues * MarshallingUtil::k_SIZEOF_INT64);
    validate();

    // Write to the buffer the specified 'value'.

    MarshallingUtil::putArrayInt64(d_buffer.data() + n, values, numValues);

    return *this;
}

inline
ByteOutStream& ByteOutStream::putArrayUint64(
                                          const bsls::Types::Uint64 *values,
                                          int                        numValues)
{
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid() || 0 == numValues)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    // Resize the buffer to have sufficient capacity with care to ensure this
    // stream is invalidated if an exception is thrown.

    const bsl::size_t n = d_buffer.size();
    invalidate();
    d_buffer.resize(n + numValues * MarshallingUtil::k_SIZEOF_INT64);
    validate();

    // Write to the buffer the specified 'value'.

    MarshallingUtil::putArrayInt64(d_buffer.data() + n, values, numValues);

    return *this;
}

inline
ByteOutStream& ByteOutStream::putArrayInt56(
                                           const bsls::Types::Int64 *values,
                                           int                       numValues)
{
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid() || 0 == numValues)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    // Resize the buffer to have sufficient capacity with care to ensure this
    // stream is invalidated if an exception is thrown.

    const bsl::size_t n = d_buffer.size();
    invalidate();
    d_buffer.resize(n + numValues * MarshallingUtil::k_SIZEOF_INT56);
    validate();

    // Write to the buffer the specified 'value'.

    MarshallingUtil::putArrayInt56(d_buffer.data() + n, values, numValues);

    return *this;
}

inline
ByteOutStream& ByteOutStream::putArrayUint56(
                                          const bsls::Types::Uint64 *values,
                                          int                        numValues)
{
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid() || 0 == numValues)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    // Resize the buffer to have sufficient capacity with care to ensure this
    // stream is invalidated if an exception is thrown.

    const bsl::size_t n = d_buffer.size();
    invalidate();
    d_buffer.resize(n + numValues * MarshallingUtil::k_SIZEOF_INT56);
    validate();

    // Write to the buffer the specified 'value'.

    MarshallingUtil::putArrayInt56(d_buffer.data() + n, values, numValues);

    return *this;
}

inline
ByteOutStream& ByteOutStream::putArrayInt48(
                                           const bsls::Types::Int64 *values,
                                           int                       numValues)
{
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid() || 0 == numValues)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    // Resize the buffer to have sufficient capacity with care to ensure this
    // stream is invalidated if an exception is thrown.

    const bsl::size_t n = d_buffer.size();
    invalidate();
    d_buffer.resize(n + numValues * MarshallingUtil::k_SIZEOF_INT48);
    validate();

    // Write to the buffer the specified 'value'.

    MarshallingUtil::putArrayInt48(d_buffer.data() + n, values, numValues);

    return *this;
}

inline
ByteOutStream& ByteOutStream::putArrayUint48(
                                          const bsls::Types::Uint64 *values,
                                          int                        numValues)
{
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid() || 0 == numValues)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    // Resize the buffer to have sufficient capacity with care to ensure this
    // stream is invalidated if an exception is thrown.

    const bsl::size_t n = d_buffer.size();
    invalidate();
    d_buffer.resize(n + numValues * MarshallingUtil::k_SIZEOF_INT48);
    validate();

    // Write to the buffer the specified 'value'.

    MarshallingUtil::putArrayInt48(d_buffer.data() + n, values, numValues);

    return *this;
}

inline
ByteOutStream& ByteOutStream::putArrayInt40(
                                           const bsls::Types::Int64 *values,
                                           int                       numValues)
{
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid() || 0 == numValues)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    // Resize the buffer to have sufficient capacity with care to ensure this
    // stream is invalidated if an exception is thrown.

    const bsl::size_t n = d_buffer.size();
    invalidate();
    d_buffer.resize(n + numValues * MarshallingUtil::k_SIZEOF_INT40);
    validate();

    // Write to the buffer the specified 'value'.

    MarshallingUtil::putArrayInt40(d_buffer.data() + n, values, numValues);

    return *this;
}

inline
ByteOutStream& ByteOutStream::putArrayUint40(
                                          const bsls::Types::Uint64 *values,
                                          int                        numValues)
{
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid() || 0 == numValues)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    // Resize the buffer to have sufficient capacity with care to ensure this
    // stream is invalidated if an exception is thrown.

    const bsl::size_t n = d_buffer.size();
    invalidate();
    d_buffer.resize(n + numValues * MarshallingUtil::k_SIZEOF_INT40);
    validate();

    // Write to the buffer the specified 'value'.

    MarshallingUtil::putArrayInt40(d_buffer.data() + n, values, numValues);

    return *this;
}

inline
ByteOutStream& ByteOutStream::putArrayInt32(const int *values, int numValues)
{
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid() || 0 == numValues)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    // Resize the buffer to have sufficient capacity with care to ensure this
    // stream is invalidated if an exception is thrown.

    const bsl::size_t n = d_buffer.size();
    invalidate();
    d_buffer.resize(n + numValues * MarshallingUtil::k_SIZEOF_INT32);
    validate();

    // Write to the buffer the specified 'value'.

    MarshallingUtil::putArrayInt32(d_buffer.data() + n, values, numValues);

    return *this;
}

inline
ByteOutStream& ByteOutStream::putArrayUint32(const unsigned int *values,
                                             int                 numValues)
{
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid() || 0 == numValues)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    // Resize the buffer to have sufficient capacity with care to ensure this
    // stream is invalidated if an exception is thrown.

    const bsl::size_t n = d_buffer.size();
    invalidate();
    d_buffer.resize(n + numValues * MarshallingUtil::k_SIZEOF_INT32);
    validate();

    // Write to the buffer the specified 'value'.

    MarshallingUtil::putArrayInt32(d_buffer.data() + n, values, numValues);

    return *this;
}

inline
ByteOutStream& ByteOutStream::putArrayInt24(const int *values, int numValues)
{
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid() || 0 == numValues)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    // Resize the buffer to have sufficient capacity with care to ensure this
    // stream is invalidated if an exception is thrown.

    const bsl::size_t n = d_buffer.size();
    invalidate();
    d_buffer.resize(n + numValues * MarshallingUtil::k_SIZEOF_INT24);
    validate();

    // Write to the buffer the specified 'value'.

    MarshallingUtil::putArrayInt24(d_buffer.data() + n, values, numValues);

    return *this;
}

inline
ByteOutStream& ByteOutStream::putArrayUint24(const unsigned int *values,
                                             int                 numValues)
{
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid() || 0 == numValues)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    // Resize the buffer to have sufficient capacity with care to ensure this
    // stream is invalidated if an exception is thrown.

    const bsl::size_t n = d_buffer.size();
    invalidate();
    d_buffer.resize(n + numValues * MarshallingUtil::k_SIZEOF_INT24);
    validate();

    // Write to the buffer the specified 'value'.

    MarshallingUtil::putArrayInt24(d_buffer.data() + n, values, numValues);

    return *this;
}

inline
ByteOutStream& ByteOutStream::putArrayInt16(const short *values, int numValues)
{
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid() || 0 == numValues)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    // Resize the buffer to have sufficient capacity with care to ensure this
    // stream is invalidated if an exception is thrown.

    const bsl::size_t n = d_buffer.size();
    invalidate();
    d_buffer.resize(n + numValues * MarshallingUtil::k_SIZEOF_INT16);
    validate();

    // Write to the buffer the specified 'value'.

    MarshallingUtil::putArrayInt16(d_buffer.data() + n, values, numValues);

    return *this;
}

inline
ByteOutStream& ByteOutStream::putArrayUint16(const unsigned short *values,
                                             int                   numValues)
{
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid() || 0 == numValues)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    // Resize the buffer to have sufficient capacity with care to ensure this
    // stream is invalidated if an exception is thrown.

    const bsl::size_t n = d_buffer.size();
    invalidate();
    d_buffer.resize(n + numValues * MarshallingUtil::k_SIZEOF_INT16);
    validate();

    // Write to the buffer the specified 'value'.

    MarshallingUtil::putArrayInt16(d_buffer.data() + n, values, numValues);

    return *this;
}

inline
ByteOutStream& ByteOutStream::putArrayInt8(const char *values, int numValues)
{
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid() || 0 == numValues)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    // Resize the buffer to have sufficient capacity with care to ensure this
    // stream is invalidated if an exception is thrown.

    const bsl::size_t n = d_buffer.size();
    invalidate();
    d_buffer.resize(n + numValues * MarshallingUtil::k_SIZEOF_INT8);
    validate();

    // Write to the buffer the specified 'value'.

    MarshallingUtil::putArrayInt8(d_buffer.data() + n, values, numValues);

    return *this;
}

inline
ByteOutStream& ByteOutStream::putArrayInt8(const signed char *values,
                                           int                numValues)
{
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid() || 0 == numValues)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    // Resize the buffer to have sufficient capacity with care to ensure this
    // stream is invalidated if an exception is thrown.

    const bsl::size_t n = d_buffer.size();
    invalidate();
    d_buffer.resize(n + numValues * MarshallingUtil::k_SIZEOF_INT8);
    validate();

    // Write to the buffer the specified 'value'.

    MarshallingUtil::putArrayInt8(d_buffer.data() + n, values, numValues);

    return *this;
}

inline
ByteOutStream& ByteOutStream::putArrayUint8(const char *values, int numValues)
{
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid() || 0 == numValues)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    // Resize the buffer to have sufficient capacity with care to ensure this
    // stream is invalidated if an exception is thrown.

    const bsl::size_t n = d_buffer.size();
    invalidate();
    d_buffer.resize(n + numValues * MarshallingUtil::k_SIZEOF_INT8);
    validate();

    // Write to the buffer the specified 'value'.

    MarshallingUtil::putArrayInt8(d_buffer.data() + n, values, numValues);

    return *this;
}

inline
ByteOutStream& ByteOutStream::putArrayUint8(const unsigned char *values,
                                            int                  numValues)
{
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid() || 0 == numValues)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    // Resize the buffer to have sufficient capacity with care to ensure this
    // stream is invalidated if an exception is thrown.

    const bsl::size_t n = d_buffer.size();
    invalidate();
    d_buffer.resize(n + numValues * MarshallingUtil::k_SIZEOF_INT8);
    validate();

    // Write to the buffer the specified 'value'.

    MarshallingUtil::putArrayInt8(d_buffer.data() + n, values, numValues);

    return *this;
}

                      // *** arrays of floating-point values ***

inline
ByteOutStream& ByteOutStream::putArrayFloat64(const double *values,
                                              int           numValues)
{
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid() || 0 == numValues)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    // Resize the buffer to have sufficient capacity with care to ensure this
    // stream is invalidated if an exception is thrown.

    const bsl::size_t n = d_buffer.size();
    invalidate();
    d_buffer.resize(n + numValues * MarshallingUtil::k_SIZEOF_FLOAT64);
    validate();

    // Write to the buffer the specified 'value'.

    MarshallingUtil::putArrayFloat64(d_buffer.data() + n, values, numValues);

    return *this;
}

inline
ByteOutStream& ByteOutStream::putArrayFloat32(const float *values,
                                              int          numValues)
{
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid() || 0 == numValues)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    // Resize the buffer to have sufficient capacity with care to ensure this
    // stream is invalidated if an exception is thrown.

    const bsl::size_t n = d_buffer.size();
    invalidate();
    d_buffer.resize(n + numValues * MarshallingUtil::k_SIZEOF_FLOAT32);
    validate();

    // Write to the buffer the specified 'value'.

    MarshallingUtil::putArrayFloat32(d_buffer.data() + n, values, numValues);

    return *this;
}

// ACCESSORS
inline
ByteOutStream::operator const void *() const
{
    return isValid() ? this : 0;
}

inline
int ByteOutStream::bdexVersionSelector() const
{
    return d_versionSelector;
}

inline
const char *ByteOutStream::data() const
{
    return d_buffer.begin();
}

inline
bool ByteOutStream::isValid() const
{
    return d_validFlag;
}

inline
bsl::size_t ByteOutStream::length() const
{
    return d_buffer.size();
}

// FREE OPERATORS
template <class TYPE>
inline
ByteOutStream& operator<<(ByteOutStream& stream, const TYPE& value)
{
    return OutStreamFunctions::bdexStreamOut(stream, value);
}

}  // close package namespace
}  // close enterprise namespace

// TRAITS
namespace BloombergLP {
namespace bslma {

template <>
struct UsesBslmaAllocator<bslx::ByteOutStream> : bsl::true_type {};

}  // close namespace bslma
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2014 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
