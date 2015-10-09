// bslx_testoutstream.h                                               -*-C++-*-
#ifndef INCLUDED_BSLX_TESTOUTSTREAM
#define INCLUDED_BSLX_TESTOUTSTREAM

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Enable externalization of fundamental types with identification.
//
//@CLASSES:
//  bslx::TestOutStream: byte-array-based output stream for fundamental types
//
//@SEE_ALSO: bslx_testinstream, bslx_byteoutstream
//
//@DESCRIPTION: This component implements a byte-array-based output stream
// class, 'bslx::TestOutStream', that provides platform-independent output
// methods ("externalization") on values, and arrays of values, of fundamental
// types, and on 'bsl::string'.  This component also externalizes information
// to the stream that can be used by the reader of the stream to verify, for
// these types, that the type of data requested from the input stream matches
// what was written by this output stream.  This component is meant for testing
// only.
//
// This component is intended to be used in conjunction with the
// 'bslx_testinstream' "unexternalization" component.  Each output method of
// 'bslx::TestOutStream' writes a value or a homogeneous array of values to an
// internally managed buffer.  The values are formatted to be readable by the
// corresponding 'bslx::TestInStream' method.  In general, the user cannot rely
// on any other mechanism to read data written by 'bslx::TestOutStream' unless
// that mechanism explicitly states its ability to do so.
//
// The supported types and required content are listed in the 'bslx'
// package-level documentation under "Supported Types".
//
// Note that the values are stored in big-endian format (i.e., network byte
// order).
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
// A 'bslx::TestOutStream' can be used to externalize values in a
// platform-neutral way.  Writing out fundamental C++ types and 'bsl::string'
// requires no additional work on the part of the client; the client can simply
// use the stream directly.  The following code serializes a few representative
// values using a 'bslx::TestOutStream', compares the contents of this stream
// to the expected value, and then writes the contents of this stream's buffer
// to 'stdout'.
//
// First, we create a 'bslx::TestOutStream' with an arbitrary value for its
// 'versionSelector' and externalize some values:
//..
//  bslx::TestOutStream outStream(20131127);
//  outStream.putInt32(1);
//  outStream.putInt32(2);
//  outStream.putInt8('c');
//  outStream.putString(bsl::string("hello"));
//..
// Then, we compare the contents of the stream to the expected value:
//..
//  const char  *theChars = outStream.data();
//  bsl::size_t  length   = outStream.length();
//  assert(24 == length);
//  assert( 0 == bsl::memcmp(theChars,
//                           "\xE6\x00\x00\x00\x01\xE6\x00\x00\x00\x02\xE0"
//                                      "c\xE0\x05\xE1\x00\x00\x00\x05""hello",
//                           length));
//..
// Finally, we print the stream's contents to 'bsl::cout'.
//..
//  for (bsl::size_t i = 0; i < length; ++i) {
//      if(bsl::isalnum(static_cast<unsigned char>(theChars[i]))) {
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
//  nextByte (int): -26
//  nextByte (int): 0
//  nextByte (int): 0
//  nextByte (int): 0
//  nextByte (int): 1
//  nextByte (int): -26
//  nextByte (int): 0
//  nextByte (int): 0
//  nextByte (int): 0
//  nextByte (int): 2
//  nextByte (int): -32
//  nextByte (char): c
//  nextByte (int): -32
//  nextByte (int): 5
//  nextByte (int): -31
//  nextByte (char): h
//  nextByte (char): e
//  nextByte (char): l
//  nextByte (char): l
//  nextByte (char): o
//..
// Note the negative numeric values indicate the "type" of the data that
// follows (see 'bslx_typecode').
//
// See the 'bslx_testinstream' component usage example for a more practical
// example of using this test output stream.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLX_BYTEOUTSTREAM
#include <bslx_byteoutstream.h>
#endif

#ifndef INCLUDED_BSLX_OUTSTREAMFUNCTIONS
#include <bslx_outstreamfunctions.h>
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

namespace BloombergLP {

namespace bslma { class Allocator; }

namespace bslx {

                         // ===================
                         // class TestOutStream
                         // ===================

class TestOutStream {
    // This class implements output methods to externalize fundamental types
    // and their associated type identification data.  It stores the
    // accumulated result in network byte order.  See the 'bslx' package-level
    // documentation for the definition of the BDEX 'OutStream' protocol.

    // DATA
    ByteOutStream d_imp;                  // byte out stream implementation

    bool          d_makeNextInvalidFlag;  // if 'true', next "put" operation
                                          // outputs the invalid data indicator
                                          // and resets this flag to 'false'

    // FRIENDS
    friend bsl::ostream& operator<<(bsl::ostream&, const TestOutStream&);

    // NOT IMPLEMENTED
    TestOutStream(const TestOutStream&);
    TestOutStream& operator=(const TestOutStream&);

  public:
    // CREATORS
    explicit TestOutStream(int               versionSelector,
                           bslma::Allocator *basicAllocator = 0);
        // Create an empty output byte stream that will use the specified
        // (*compile*-time-defined) 'versionSelector' as needed (see
        // {Versioning}).  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  Note that the 'versionSelector' is expected to
        // be formatted as "YYYYMMDD", a date representation.

    TestOutStream(int               versionSelector,
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

    ~TestOutStream();
        // Destroy this object.

    // MANIPULATORS
    void invalidate();
        // Put this output stream in an invalid state.  This function has no
        // effect if this stream is already invalid.

    void makeNextInvalid();
        // Make the next output operation externalize the invalid data
        // indicator, as opposed to the actual type indicator, to this output
        // stream; the data associated with the next output operation is still
        // externalized.  Note that the invalid data indicator can be detected
        // by a corresponding 'TestInStream' object.

    TestOutStream& putLength(int length);
        // If the specified 'length' is less than 128, write to this stream the
        // one-byte type indicator for a one-byte integer and the one-byte
        // integer comprised of the least-significant one byte of the 'length';
        // otherwise, write to this stream the one-byte type indicator for a
        // four-byte integer and the four-byte, two's complement integer (in
        // network byte order) comprised of the least-significant four bytes of
        // the 'length' (in host byte order) with the most-significant bit set.
        // Return a reference to this stream.  If this stream is initially
        // invalid, this operation has no effect.  If the next output operation
        // has been set to be marked invalid (see 'makeNextInvalid'), reset
        // this marking and emit the invalid indicator instead of the type
        // indicator.  The behavior is undefined unless '0 <= length'.

    TestOutStream& putVersion(int version);
        // Write to this stream the one-byte type indicator for a one-byte
        // unsigned integer and the one-byte, two's complement unsigned integer
        // comprised of the least-significant one byte of the specified
        // 'version', and return a reference to this stream.  If this stream is
        // initially invalid, this operation has no effect.  If the next output
        // operation has been set to be marked invalid (see 'makeNextInvalid'),
        // reset this marking and emit the invalid indicator instead of the
        // type indicator.

    void reserveCapacity(bsl::size_t newCapacity);
        // Set the internal buffer size of this stream to be at least the
        // specified 'newCapacity' (in bytes).

    void reset();
        // Remove all content in this stream and validate this stream if it is
        // currently invalid.

                      // *** scalar integer values ***

    TestOutStream& putInt64(bsls::Types::Int64 value);
        // Write to this stream the one-byte type indicator for an eight-byte
        // integer and the eight-byte, two's complement integer (in network
        // byte order) comprised of the least-significant eight bytes of the
        // specified 'value' (in host byte order), and return a reference to
        // this stream.  If this stream is initially invalid, this operation
        // has no effect.  If the next output operation has been set to be
        // marked invalid (see 'makeNextInvalid'), reset this marking and emit
        // the invalid indicator instead of the type indicator.

    TestOutStream& putUint64(bsls::Types::Uint64 value);
        // Write to this stream the one-byte type indicator for an eight-byte
        // unsigned integer and the eight-byte, two's complement unsigned
        // integer (in network byte order) comprised of the least-significant
        // eight bytes of the specified 'value' (in host byte order), and
        // return a reference to this stream.  If this stream is initially
        // invalid, this operation has no effect.  If the next output operation
        // has been set to be marked invalid (see 'makeNextInvalid'), reset
        // this marking and emit the invalid indicator instead of the type
        // indicator.

    TestOutStream& putInt56(bsls::Types::Int64 value);
        // Write to this stream the one-byte type indicator for a seven-byte
        // integer and the seven-byte, two's complement integer (in network
        // byte order) comprised of the least-significant seven bytes of the
        // specified 'value' (in host byte order), and return a reference to
        // this stream.  If this stream is initially invalid, this operation
        // has no effect.  If the next output operation has been set to be
        // marked invalid (see 'makeNextInvalid'), reset this marking and emit
        // the invalid indicator instead of the type indicator.

    TestOutStream& putUint56(bsls::Types::Uint64 value);
        // Write to this stream the one-byte type indicator for a seven-byte
        // unsigned integer and the seven-byte, two's complement unsigned
        // integer (in network byte order) comprised of the least-significant
        // seven bytes of the specified 'value' (in host byte order), and
        // return a reference to this stream.  If this stream is initially
        // invalid, this operation has no effect.  If the next output operation
        // has been set to be marked invalid (see 'makeNextInvalid'), reset
        // this marking and emit the invalid indicator instead of the type
        // indicator.

    TestOutStream& putInt48(bsls::Types::Int64 value);
        // Write to this stream the one-byte type indicator for a six-byte
        // integer and the six-byte, two's complement integer (in network byte
        // order) comprised of the least-significant six bytes of the specified
        // 'value' (in host byte order), and return a reference to this stream.
        // If this stream is initially invalid, this operation has no effect.
        // If the next output operation has been set to be marked invalid (see
        // 'makeNextInvalid'), reset this marking and emit the invalid
        // indicator instead of the type indicator.

    TestOutStream& putUint48(bsls::Types::Uint64 value);
        // Write to this stream the one-byte type indicator for a six-byte
        // unsigned integer and the six-byte, two's complement unsigned integer
        // (in network byte order) comprised of the least-significant six bytes
        // of the specified 'value' (in host byte order), and return a
        // reference to this stream.  If this stream is initially invalid, this
        // operation has no effect.  If the next output operation has been set
        // to be marked invalid (see 'makeNextInvalid'), reset this marking and
        // emit the invalid indicator instead of the type indicator.

    TestOutStream& putInt40(bsls::Types::Int64 value);
        // Write to this stream the one-byte type indicator for a five-byte
        // integer and the five-byte, two's complement integer (in network byte
        // order) comprised of the least-significant five bytes of the
        // specified 'value' (in host byte order), and return a reference to
        // this stream.  If this stream is initially invalid, this operation
        // has no effect.  If the next output operation has been set to be
        // marked invalid (see 'makeNextInvalid'), reset this marking and emit
        // the invalid indicator instead of the type indicator.

    TestOutStream& putUint40(bsls::Types::Uint64 value);
        // Write to this stream the one-byte type indicator for a five-byte
        // unsigned integer and the five-byte, two's complement unsigned
        // integer (in network byte order) comprised of the least-significant
        // five bytes of the specified 'value' (in host byte order), and return
        // a reference to this stream.  If this stream is initially invalid,
        // this operation has no effect.  If the next output operation has been
        // set to be marked invalid (see 'makeNextInvalid'), reset this marking
        // and emit the invalid indicator instead of the type indicator.

    TestOutStream& putInt32(int value);
        // Write to this stream the one-byte type indicator for a four-byte
        // integer and the four-byte, two's complement integer (in network byte
        // order) comprised of the least-significant four bytes of the
        // specified 'value' (in host byte order), and return a reference to
        // this stream.  If this stream is initially invalid, this operation
        // has no effect.  If the next output operation has been set to be
        // marked invalid (see 'makeNextInvalid'), reset this marking and emit
        // the invalid indicator instead of the type indicator.

    TestOutStream& putUint32(unsigned int value);
        // Write to this stream the one-byte type indicator for a four-byte
        // unsigned integer and the four-byte, two's complement unsigned
        // integer (in network byte order) comprised of the least-significant
        // four bytes of the specified 'value' (in host byte order), and return
        // a reference to this stream.  If this stream is initially invalid,
        // this operation has no effect.  If the next output operation has been
        // set to be marked invalid (see 'makeNextInvalid'), reset this marking
        // and emit the invalid indicator instead of the type indicator.

    TestOutStream& putInt24(int value);
        // Write to this stream the one-byte type indicator for a three-byte
        // integer and the three-byte, two's complement integer (in network
        // byte order) comprised of the least-significant three bytes of the
        // specified 'value' (in host byte order), and return a reference to
        // this stream.  If this stream is initially invalid, this operation
        // has no effect.  If the next output operation has been set to be
        // marked invalid (see 'makeNextInvalid'), reset this marking and emit
        // the invalid indicator instead of the type indicator.

    TestOutStream& putUint24(unsigned int value);
        // Write to this stream the one-byte type indicator for a three-byte
        // unsigned integer and the three-byte, two's complement unsigned
        // integer (in network byte order) comprised of the least-significant
        // three bytes of the specified 'value' (in host byte order), and
        // return a reference to this stream.  If this stream is initially
        // invalid, this operation has no effect.  If the next output operation
        // has been set to be marked invalid (see 'makeNextInvalid'), reset
        // this marking and emit the invalid indicator instead of the type
        // indicator.

    TestOutStream& putInt16(int value);
        // Write to this stream the one-byte type indicator for a two-byte
        // integer and the two-byte, two's complement integer (in network byte
        // order) comprised of the least-significant two bytes of the specified
        // 'value' (in host byte order), and return a reference to this stream.
        // If this stream is initially invalid, this operation has no effect.
        // If the next output operation has been set to be marked invalid (see
        // 'makeNextInvalid'), reset this marking and emit the invalid
        // indicator instead of the type indicator.

    TestOutStream& putUint16(unsigned int value);
        // Write to this stream the one-byte type indicator for a two-byte
        // unsigned integer and the two-byte, two's complement unsigned integer
        // (in network byte order) comprised of the least-significant two bytes
        // of the specified 'value' (in host byte order), and return a
        // reference to this stream.  If this stream is initially invalid, this
        // operation has no effect.  If the next output operation has been set
        // to be marked invalid (see 'makeNextInvalid'), reset this marking and
        // emit the invalid indicator instead of the type indicator.

    TestOutStream& putInt8(int value);
        // Write to this stream the one-byte type indicator for a one-byte
        // integer and the one-byte, two's complement integer comprised of the
        // least-significant one byte of the specified 'value', and return a
        // reference to this stream.  If this stream is initially invalid, this
        // operation has no effect.  If the next output operation has been set
        // to be marked invalid (see 'makeNextInvalid'), reset this marking and
        // emit the invalid indicator instead of the type indicator.

    TestOutStream& putUint8(unsigned int value);
        // Write to this stream the one-byte type indicator for a one-byte
        // unsigned integer and the one-byte, two's complement unsigned integer
        // comprised of the least-significant one byte of the specified
        // 'value', and return a reference to this stream.  If this stream is
        // initially invalid, this operation has no effect.  If the next output
        // operation has been set to be marked invalid (see 'makeNextInvalid'),
        // reset this marking and emit the invalid indicator instead of the
        // type indicator.

                      // *** scalar floating-point values ***

    TestOutStream& putFloat64(double value);
        // Write to this stream the one-byte type indicator for an eight-byte
        // double-precision floating-point number and the eight-byte IEEE
        // double-precision floating-point number (in network byte order)
        // comprised of the most-significant eight bytes of the specified
        // 'value' (in host byte order), and return a reference to this stream.
        // If this stream is initially invalid, this operation has no effect.
        // If the next output operation has been set to be marked invalid (see
        // 'makeNextInvalid'), reset this marking and emit the invalid
        // indicator instead of the type indicator.  Note that for
        // non-conforming platforms, this operation may be lossy.

    TestOutStream& putFloat32(float value);
        // Write to this stream the one-byte type indicator for a four-byte
        // single-precision floating-point number and the four-byte IEEE
        // single-precision floating-point number (in network byte order)
        // comprised of the most-significant four bytes of the specified
        // 'value' (in host byte order), and return a reference to this stream.
        // If this stream is initially invalid, this operation has no effect.
        // If the next output operation has been set to be marked invalid (see
        // 'makeNextInvalid'), reset this marking and emit the invalid
        // indicator instead of the type indicator.  Note that for
        // non-conforming platforms, this operation may be lossy.

                      // *** string values ***

    TestOutStream& putString(const bsl::string& value);
        // Write to this stream the one-byte type indicator for a length (see
        // 'putLength'), the length of the specified 'value' (see 'putLength'),
        // the one-byte type indicator for an array of one-byte unsigned
        // integers, and an array of one-byte, two's complement unsigned
        // integers comprised of the least-significant one byte of each
        // character in the 'value', and return a reference to this stream.  If
        // this stream is initially invalid, this operation has no effect.  If
        // the next output operation has been set to be marked invalid (see
        // 'makeNextInvalid'), reset this marking and emit the invalid
        // indicator instead of the type indicator.

                      // *** arrays of integer values ***

    TestOutStream& putArrayInt64(const bsls::Types::Int64 *values,
                                 int                       numValues);
        // Write to this stream the one-byte type indicator for an eight-byte
        // integer, the four-byte, two's complement integer (in network byte
        // order) comprised of the least-significant four bytes of the
        // specified 'numValues' (in host byte order), and the consecutive
        // eight-byte, two's complement integers (in network byte order)
        // comprised of the least-significant eight bytes of each of the
        // 'numValues' leading entries in the specified 'values' (in host byte
        // order), and return a reference to this stream.  If this stream is
        // initially invalid, this operation has no effect.  If the next output
        // operation has been set to be marked invalid (see 'makeNextInvalid'),
        // reset this marking and emit the invalid indicator instead of the
        // type indicator.  The behavior is undefined unless '0 <= numValues'
        // and 'values' has sufficient contents.

    TestOutStream& putArrayUint64(const bsls::Types::Uint64 *values,
                                  int                        numValues);
        // Write to this stream the one-byte type indicator for an eight-byte
        // unsigned integer, the four-byte, two's complement integer (in
        // network byte order) comprised of the least-significant four bytes of
        // the specified 'numValues' (in host byte order), and the consecutive
        // eight-byte, two's complement unsigned integers (in network byte
        // order) comprised of the least-significant eight bytes of each of the
        // 'numValues' leading entries in the specified 'values' (in host byte
        // order), and return a reference to this stream.  If this stream is
        // initially invalid, this operation has no effect.  If the next output
        // operation has been set to be marked invalid (see 'makeNextInvalid'),
        // reset this marking and emit the invalid indicator instead of the
        // type indicator.  The behavior is undefined unless '0 <= numValues'
        // and 'values' has sufficient contents.

    TestOutStream& putArrayInt56(const bsls::Types::Int64 *values,
                                 int                       numValues);
        // Write to this stream the one-byte type indicator for a seven-byte
        // integer, the four-byte, two's complement integer (in network byte
        // order) comprised of the least-significant four bytes of the
        // specified 'numValues' (in host byte order), and the consecutive
        // seven-byte, two's complement integers (in network byte order)
        // comprised of the least-significant seven bytes of each of the
        // 'numValues' leading entries in the specified 'values' (in host byte
        // order), and return a reference to this stream.  If this stream is
        // initially invalid, this operation has no effect.  If the next output
        // operation has been set to be marked invalid (see 'makeNextInvalid'),
        // reset this marking and emit the invalid indicator instead of the
        // type indicator.  The behavior is undefined unless '0 <= numValues'
        // and 'values' has sufficient contents.

    TestOutStream& putArrayUint56(const bsls::Types::Uint64 *values,
                                  int                        numValues);
        // Write to this stream the one-byte type indicator for a seven-byte
        // unsigned integer, the four-byte, two's complement integer (in
        // network byte order) comprised of the least-significant four bytes of
        // the specified 'numValues' (in host byte order), and the consecutive
        // seven-byte, two's complement unsigned integers (in network byte
        // order) comprised of the least-significant seven bytes of each of the
        // 'numValues' leading entries in the specified 'values' (in host byte
        // order), and return a reference to this stream.  If this stream is
        // initially invalid, this operation has no effect.  If the next output
        // operation has been set to be marked invalid (see 'makeNextInvalid'),
        // reset this marking and emit the invalid indicator instead of the
        // type indicator.  The behavior is undefined unless '0 <= numValues'
        // and 'values' has sufficient contents.

    TestOutStream& putArrayInt48(const bsls::Types::Int64 *values,
                                 int                       numValues);
        // Write to this stream the one-byte type indicator for a six-byte
        // integer, the four-byte, two's complement integer (in network byte
        // order) comprised of the least-significant four bytes of the
        // specified 'numValues' (in host byte order), and the consecutive
        // six-byte, two's complement integers (in network byte order)
        // comprised of the least-significant six bytes of each of the
        // 'numValues' leading entries in the specified 'values' (in host byte
        // order), and return a reference to this stream.  If this stream is
        // initially invalid, this operation has no effect.  If the next output
        // operation has been set to be marked invalid (see 'makeNextInvalid'),
        // reset this marking and emit the invalid indicator instead of the
        // type indicator.  The behavior is undefined unless '0 <= numValues'
        // and 'values' has sufficient contents.

    TestOutStream& putArrayUint48(const bsls::Types::Uint64 *values,
                                  int                        numValues);
        // Write to this stream the one-byte type indicator for a six-byte
        // unsigned integer, the four-byte, two's complement integer (in
        // network byte order) comprised of the least-significant four bytes of
        // the specified 'numValues' (in host byte order), and the consecutive
        // six-byte, two's complement unsigned integers (in network byte order)
        // comprised of the least-significant six bytes of each of the
        // 'numValues' leading entries in the specified 'values' (in host byte
        // order), and return a reference to this stream.  If this stream is
        // initially invalid, this operation has no effect.  If the next output
        // operation has been set to be marked invalid (see 'makeNextInvalid'),
        // reset this marking and emit the invalid indicator instead of the
        // type indicator.  The behavior is undefined unless '0 <= numValues'
        // and 'values' has sufficient contents.

    TestOutStream& putArrayInt40(const bsls::Types::Int64 *values,
                                 int                       numValues);
        // Write to this stream the one-byte type indicator for a five-byte
        // integer, the four-byte, two's complement integer (in network byte
        // order) comprised of the least-significant four bytes of the
        // specified 'numValues' (in host byte order), and the consecutive
        // five-byte, two's complement integers (in network byte order)
        // comprised of the least-significant five bytes of each of the
        // 'numValues' leading entries in the specified 'values' (in host byte
        // order), and return a reference to this stream.  If this stream is
        // initially invalid, this operation has no effect.  If the next output
        // operation has been set to be marked invalid (see 'makeNextInvalid'),
        // reset this marking and emit the invalid indicator instead of the
        // type indicator.  The behavior is undefined unless '0 <= numValues'
        // and 'values' has sufficient contents.

    TestOutStream& putArrayUint40(const bsls::Types::Uint64 *values,
                                  int                        numValues);
        // Write to this stream the one-byte type indicator for a five-byte
        // unsigned integer, the four-byte, two's complement integer (in
        // network byte order) comprised of the least-significant four bytes of
        // the specified 'numValues' (in host byte order), and the consecutive
        // five-byte, two's complement unsigned integers (in network byte
        // order) comprised of the least-significant five bytes of each of the
        // 'numValues' leading entries in the specified 'values' (in host byte
        // order), and return a reference to this stream.  If this stream is
        // initially invalid, this operation has no effect.  If the next output
        // operation has been set to be marked invalid (see 'makeNextInvalid'),
        // reset this marking and emit the invalid indicator instead of the
        // type indicator.  The behavior is undefined unless '0 <= numValues'
        // and 'values' has sufficient contents.

    TestOutStream& putArrayInt32(const int *values, int numValues);
        // Write to this stream the one-byte type indicator for a four-byte
        // integer, the four-byte, two's complement integer (in network byte
        // order) comprised of the least-significant four bytes of the
        // specified 'numValues' (in host byte order), and the consecutive
        // four-byte, two's complement integers (in network byte order)
        // comprised of the least-significant four bytes of each of the
        // 'numValues' leading entries in the specified 'values' (in host byte
        // order), and return a reference to this stream.  If this stream is
        // initially invalid, this operation has no effect.  If the next output
        // operation has been set to be marked invalid (see 'makeNextInvalid'),
        // reset this marking and emit the invalid indicator instead of the
        // type indicator.  The behavior is undefined unless '0 <= numValues'
        // and 'values' has sufficient contents.

    TestOutStream& putArrayUint32(const unsigned int *values, int numValues);
        // Write to this stream the one-byte type indicator for a four-byte
        // unsigned integer, the four-byte, two's complement integer (in
        // network byte order) comprised of the least-significant four bytes of
        // the specified 'numValues' (in host byte order), and the consecutive
        // four-byte, two's complement unsigned integers (in network byte
        // order) comprised of the least-significant four bytes of each of the
        // 'numValues' leading entries in the specified 'values' (in host byte
        // order), and return a reference to this stream.  If this stream is
        // initially invalid, this operation has no effect.  If the next output
        // operation has been set to be marked invalid (see 'makeNextInvalid'),
        // reset this marking and emit the invalid indicator instead of the
        // type indicator.  The behavior is undefined unless '0 <= numValues'
        // and 'values' has sufficient contents.

    TestOutStream& putArrayInt24(const int *values, int numValues);
        // Write to this stream the one-byte type indicator for a three-byte
        // integer, the four-byte, two's complement integer (in network byte
        // order) comprised of the least-significant four bytes of the
        // specified 'numValues' (in host byte order), and the consecutive
        // three-byte, two's complement integers (in network byte order)
        // comprised of the least-significant three bytes of each of the
        // 'numValues' leading entries in the specified 'values' (in host byte
        // order), and return a reference to this stream.  If this stream is
        // initially invalid, this operation has no effect.  If the next output
        // operation has been set to be marked invalid (see 'makeNextInvalid'),
        // reset this marking and emit the invalid indicator instead of the
        // type indicator.  The behavior is undefined unless '0 <= numValues'
        // and 'values' has sufficient contents.

    TestOutStream& putArrayUint24(const unsigned int *values, int numValues);
        // Write to this stream the one-byte type indicator for a three-byte
        // unsigned integer, the four-byte, two's complement integer (in
        // network byte order) comprised of the least-significant four bytes of
        // the specified 'numValues' (in host byte order), and the consecutive
        // three-byte, two's complement unsigned integers (in network byte
        // order) comprised of the least-significant three bytes of each of the
        // 'numValues' leading entries in the specified 'values' (in host byte
        // order), and return a reference to this stream.  If this stream is
        // initially invalid, this operation has no effect.  If the next output
        // operation has been set to be marked invalid (see 'makeNextInvalid'),
        // reset this marking and emit the invalid indicator instead of the
        // type indicator.  The behavior is undefined unless '0 <= numValues'
        // and 'values' has sufficient contents.

    TestOutStream& putArrayInt16(const short *values, int numValues);
        // Write to this stream the one-byte type indicator for a two-byte
        // integer, the four-byte, two's complement integer (in network byte
        // order) comprised of the least-significant four bytes of the
        // specified 'numValues' (in host byte order), and the consecutive
        // two-byte, two's complement integers (in network byte order)
        // comprised of the least-significant two bytes of each of the
        // 'numValues' leading entries in the specified 'values' (in host byte
        // order), and return a reference to this stream.  If this stream is
        // initially invalid, this operation has no effect.  If the next output
        // operation has been set to be marked invalid (see 'makeNextInvalid'),
        // reset this marking and emit the invalid indicator instead of the
        // type indicator.  The behavior is undefined unless '0 <= numValues'
        // and 'values' has sufficient contents.

    TestOutStream& putArrayUint16(const unsigned short *values, int numValues);
        // Write to this stream the one-byte type indicator for a two-byte
        // unsigned integer, the four-byte, two's complement integer (in
        // network byte order) comprised of the least-significant four bytes of
        // the specified 'numValues' (in host byte order), and the consecutive
        // two-byte, two's complement unsigned integers (in network byte order)
        // comprised of the least-significant two bytes of each of the
        // 'numValues' leading entries in the specified 'values' (in host byte
        // order), and return a reference to this stream.  If this stream is
        // initially invalid, this operation has no effect.  If the next output
        // operation has been set to be marked invalid (see 'makeNextInvalid'),
        // reset this marking and emit the invalid indicator instead of the
        // type indicator.  The behavior is undefined unless '0 <= numValues'
        // and 'values' has sufficient contents.

    TestOutStream& putArrayInt8(const char        *values, int numValues);
    TestOutStream& putArrayInt8(const signed char *values, int numValues);
        // Write to this stream the one-byte type indicator for a one-byte
        // integer, the four-byte, two's complement integer (in network byte
        // order) comprised of the least-significant four bytes of the
        // specified 'numValues' (in host byte order), and the consecutive
        // one-byte, two's complement integers comprised of the
        // least-significant one byte of each of the 'numValues' leading
        // entries in the specified 'values', and return a reference to this
        // stream.  If this stream is initially invalid, this operation has no
        // effect.  If the next output operation has been set to be marked
        // invalid (see 'makeNextInvalid'), reset this marking and emit the
        // invalid indicator instead of the type indicator.  The behavior is
        // undefined unless '0 <= numValues' and 'values' has sufficient
        // contents.

    TestOutStream& putArrayUint8(const char          *values, int numValues);
    TestOutStream& putArrayUint8(const unsigned char *values, int numValues);
        // Write to this stream the one-byte type indicator for a one-byte
        // unsigned integer, the four-byte, two's complement integer (in
        // network byte order) comprised of the least-significant four bytes of
        // the specified 'numValues' (in host byte order), and the consecutive
        // one-byte, two's complement unsigned integers comprised of the
        // least-significant one byte of each of the 'numValues' leading
        // entries in the specified 'values', and return a reference to this
        // stream.  If this stream is initially invalid, this operation has no
        // effect.  If the next output operation has been set to be marked
        // invalid (see 'makeNextInvalid'), reset this marking and emit the
        // invalid indicator instead of the type indicator.  The behavior is
        // undefined unless '0 <= numValues' and 'values' has sufficient
        // contents.

                      // *** arrays of floating-point values ***

    TestOutStream& putArrayFloat64(const double *values, int numValues);
        // Write to this stream the one-byte type indicator for an eight-byte
        // double-precision floating-point number, the four-byte, two's
        // complement integer (in network byte order) comprised of the
        // least-significant four bytes of the specified 'numValues' (in host
        // byte order), and the consecutive eight-byte IEEE double-precision
        // floating-point numbers (in network byte order) comprised of the
        // most-significant eight bytes of each of the 'numValues' leading
        // entries in the specified 'values' (in host byte order), and return a
        // reference to this stream.  If this stream is initially invalid, this
        // operation has no effect.  If the next output operation has been set
        // to be marked invalid (see 'makeNextInvalid'), reset this marking and
        // emit the invalid indicator instead of the type indicator.  The
        // behavior is undefined unless '0 <= numValues' and 'values' has
        // sufficient contents.  Note that for non-conforming platforms, this
        // operation may be lossy.

    TestOutStream& putArrayFloat32(const float *values, int numValues);
        // Write to this stream the one-byte type indicator for a four-byte
        // single-precision floating-point number, the four-byte, two's
        // complement integer (in network byte order) comprised of the
        // least-significant four bytes of the specified 'numValues' (in host
        // byte order), and the consecutive four-byte IEEE single-precision
        // floating-point numbers (in network byte order) comprised of the
        // most-significant four bytes of each of the 'numValues' leading
        // entries in the specified 'values' (in host byte order), and return a
        // reference to this stream.  If this stream is initially invalid, this
        // operation has no effect.  If the next output operation has been set
        // to be marked invalid (see 'makeNextInvalid'), reset this marking and
        // emit the invalid indicator instead of the type indicator.  The
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

    const char *data() const;
        // Return the address of the contiguous, non-modifiable internal memory
        // buffer of this stream.  The address will remain valid as long as
        // this array is not destroyed or modified (i.e., the current capacity
        // is not exceeded).  The behavior of accessing elements outside the
        // range '[ data() .. data() + (length() - 1) ]' is undefined.

    bool isValid() const;
        // Return 'true' if this stream is valid, and 'false' otherwise.  An
        // invalid stream is a stream for which an output operation was
        // detected to have failed or 'invalidate' was called.

    bsl::size_t length() const;
        // Return the number of bytes in this stream.
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&        stream,
                         const TestOutStream& object);
    // Write the specified 'object' to the specified output 'stream' in some
    // reasonable (multi-line) format, and return a reference to 'stream'.

template <class TYPE>
TestOutStream& operator<<(TestOutStream& stream, const TYPE& value);
    // Write the specified 'value' to the specified output 'stream' following
    // the requirements of the BDEX protocol (see the 'bslx' package-level
    // documentation), and return a reference to 'stream'.  The behavior is
    // undefined unless 'TYPE' is BDEX-compliant.

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                         // -------------------
                         // class TestOutStream
                         // -------------------

// MANIPULATORS
inline
void TestOutStream::invalidate()
{
    d_imp.invalidate();
}

inline
void TestOutStream::makeNextInvalid()
{
    d_makeNextInvalidFlag = true;
}

inline
void TestOutStream::reserveCapacity(bsl::size_t newCapacity)
{
    d_imp.reserveCapacity(newCapacity);
}

inline
void TestOutStream::reset()
{
    d_imp.reset();
}

                      // *** string values ***

inline
TestOutStream& TestOutStream::putString(const bsl::string& value)
{
    putLength(static_cast<int>(value.length()));
    return putArrayUint8(value.data(), static_cast<int>(value.length()));
}

// ACCESSORS
inline
TestOutStream::operator const void *() const
{
    return d_imp;
}

inline
int TestOutStream::bdexVersionSelector() const
{
    return d_imp.bdexVersionSelector();
}

inline
const char *TestOutStream::data() const
{
    return d_imp.data();
}

inline
bool TestOutStream::isValid() const
{
    return d_imp.isValid();
}

inline
bsl::size_t TestOutStream::length() const
{
    return d_imp.length();
}

// FREE OPERATORS
template <class TYPE>
inline
TestOutStream& operator<<(TestOutStream& stream, const TYPE& value)
{
    return OutStreamFunctions::bdexStreamOut(stream, value);
}

}  // close package namespace
}  // close enterprise namespace

// TRAITS
namespace BloombergLP {
namespace bslma {

template <>
struct UsesBslmaAllocator<bslx::TestOutStream> : bsl::true_type {};

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
