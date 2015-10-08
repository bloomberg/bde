// bslx_byteinstream.h                                                -*-C++-*-
#ifndef INCLUDED_BSLX_BYTEINSTREAM
#define INCLUDED_BSLX_BYTEINSTREAM

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a stream class for unexternalization of fundamental types.
//
//@CLASSES:
//  bslx::ByteInStream: byte-array-based input stream for fundamental types
//
//@SEE_ALSO: bslx_byteoutstream
//
//@DESCRIPTION: This component implements a byte-array-based input stream
// class, 'bslx::ByteInStream', that provides platform-independent input
// methods ("unexternalization") on values, and arrays of values, of
// fundamental types, and on 'bsl::string'.
//
// The 'bslx::ByteInStream' type reads from a user-supplied buffer directly,
// with no data copying or assumption of ownership.  The user must therefore
// make sure that the lifetime and visibility of the buffer is sufficient to
// satisfy the needs of the input stream.
//
// This component is intended to be used in conjunction with the
// 'bslx_byteoutstream' "externalization" component.  Each input method of
// 'bslx::ByteInStream' reads either a value or a homogeneous array of values
// of a fundamental type, in a format that was written by the corresponding
// 'bslx::ByteOutStream' method.  In general, the user of this component cannot
// rely on being able to read data that was written by any mechanism other than
// 'bslx::ByteOutStream'.
//
// The supported types and required content are listed in the 'bslx'
// package-level documentation under "Supported Types".
//
// Note that input streams can be *invalidated* explicitly and queried for
// *validity* and *emptiness*.  Reading from an initially invalid stream has no
// effect.  Attempting to read beyond the end of a stream will automatically
// invalidate the stream.  Whenever an inconsistent value is detected, the
// stream should be invalidated explicitly.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Unexternalization
/// - - - - - - - - - - - - - - - - -
// Suppose we wish to implement a (deliberately simple) 'MyPerson' class as a
// value-semantic object that supports BDEX externalization and
// unexternalization.  In addition to whatever data and methods that we choose
// to put into our design, we must supply three methods having specific names
// and signatures in order to comply with the BDEX protocol: a class method
// 'maxSupportedBdexVersion', an accessor (i.e., a 'const' method)
// 'bdexStreamOut', and a manipulator (i.e., a non-'const' method)
// 'bdexStreamIn'.  This example shows how to implement those three methods.
//
// In this example we will not worry overly about "good design" of the
// 'MyPerson' component, and we will declare but not implement illustrative
// methods and free operators, except for the three required BDEX methods,
// which are implemented in full.  In particular, we will not make explicit use
// of 'bslma' allocators; a more complete design would do so:
//
// First, we implement 'MyPerson':
//..
//  class MyPerson {
//      bsl::string d_firstName;
//      bsl::string d_lastName;
//      int         d_age;
//
//      friend bool operator==(const MyPerson&, const MyPerson&);
//
//    public:
//      // CLASS METHODS
//      static int maxSupportedBdexVersion(int versionSelector);
//          // Return the maximum valid BDEX format version, as indicated by
//          // the specified 'versionSelector', to be passed to the
//          // 'bdexStreamOut' method.  Note that it is highly recommended that
//          // 'versionSelector' be formatted as "YYYYMMDD", a date
//          // representation.  Also note that 'versionSelector' should be a
//          // *compile*-time-chosen value that selects a format version
//          // supported by both externalizer and unexternalizer.  See the
//          // 'bslx' package-level documentation for more information on BDEX
//          // streaming of value-semantic types and containers.
//
//      // CREATORS
//      MyPerson();
//          // Create a default person.
//
//      MyPerson(const char *firstName, const char *lastName, int age);
//          // Create a person having the specified 'firstName', 'lastName',
//          // and 'age'.
//
//      MyPerson(const MyPerson& original);
//          // Create a person having the value of the specified 'original'
//          // person.
//
//      ~MyPerson();
//          // Destroy this object.
//
//      // MANIPULATORS
//      MyPerson& operator=(const MyPerson& rhs);
//          // Assign to this person the value of the specified 'rhs' person,
//          // and return a reference to this person.
//
//      template <class STREAM>
//      STREAM& bdexStreamIn(STREAM& stream, int version);
//          // Assign to this object the value read from the specified input
//          // 'stream' using the specified 'version' format, and return a
//          // reference to 'stream'.  If 'stream' is initially invalid, this
//          // operation has no effect.  If 'version' is not supported, this
//          // object is unaltered and 'stream' is invalidated, but otherwise
//          // unmodified.  If 'version' is supported but 'stream' becomes
//          // invalid during this operation, this object has an undefined, but
//          // valid, state.  Note that no version is read from 'stream'.  See
//          // the 'bslx' package-level documentation for more information on
//          // BDEX streaming of value-semantic types and containers.
//
//      //...
//
//      // ACCESSORS
//      int age() const;
//          // Return the age of this person.
//
//      template <class STREAM>
//      STREAM& bdexStreamOut(STREAM& stream, int version) const;
//          // Write the value of this object, using the specified 'version'
//          // format, to the specified output 'stream', and return a reference
//          // to 'stream'.  If 'stream' is initially invalid, this operation
//          // has no effect.  If 'version' is not supported, 'stream' is
//          // invalidated, but otherwise unmodified.  Note that 'version' is
//          // not written to 'stream'.  See the 'bslx' package-level
//          // documentation for more information on BDEX streaming of
//          // value-semantic types and containers.
//
//      const bsl::string& firstName() const;
//          // Return the first name of this person.
//
//      const bsl::string& lastName() const;
//          // Return the last name of this person.
//
//      //...
//
//  };
//
//  // FREE OPERATORS
//  bool operator==(const MyPerson& lhs, const MyPerson& rhs);
//      // Return 'true' if the specified 'lhs' and 'rhs' person objects have
//      // the same value, and 'false' otherwise.  Two person objects have the
//      // same value if they have the same first name, last name, and age.
//
//  bool operator!=(const MyPerson& lhs, const MyPerson& rhs);
//      // Return 'true' if the specified 'lhs' and 'rhs' person objects do not
//      // have the same value, and 'false' otherwise.  Two person objects
//      // differ in value if they differ in first name, last name, or age.
//
//  // ========================================================================
//  //                  INLINE FUNCTION DEFINITIONS
//  // ========================================================================
//
//  // CLASS METHODS
//  inline
//  int MyPerson::maxSupportedBdexVersion(int /* versionSelector */) {
//      return 1;
//  }
//
//  // CREATORS
//  inline
//  MyPerson::MyPerson()
//  : d_firstName("")
//  , d_lastName("")
//  , d_age(0)
//  {
//  }
//
//  inline
//  MyPerson::MyPerson(const char *firstName, const char *lastName, int age)
//  : d_firstName(firstName)
//  , d_lastName(lastName)
//  , d_age(age)
//  {
//  }
//
//  inline
//  MyPerson::~MyPerson()
//  {
//  }
//
//  template <class STREAM>
//  STREAM& MyPerson::bdexStreamIn(STREAM& stream, int version)
//  {
//      if (stream) {
//          switch (version) {  // switch on the 'bslx' version
//            case 1: {
//              stream.getString(d_firstName);
//              if (!stream) {
//                  d_firstName = "stream error";  // *might* be corrupted;
//                                                 //  value for testing
//                  return stream;                                    // RETURN
//              }
//              stream.getString(d_lastName);
//              if (!stream) {
//                  d_lastName = "stream error";  // *might* be corrupted;
//                                                //  value for testing
//                  return stream;                                    // RETURN
//              }
//              stream.getInt32(d_age);
//              if (!stream) {
//                  d_age = 999;     // *might* be corrupted; value for testing
//                  return stream;                                    // RETURN
//              }
//            } break;
//            default: {
//              stream.invalidate();
//            }
//          }
//      }
//      return stream;
//  }
//
//  // ACCESSORS
//  inline
//  int MyPerson::age() const
//  {
//      return d_age;
//  }
//
//  template <class STREAM>
//  STREAM& MyPerson::bdexStreamOut(STREAM& stream, int version) const
//  {
//      switch (version) {
//        case 1: {
//          stream.putString(d_firstName);
//          stream.putString(d_lastName);
//          stream.putInt32(d_age);
//        } break;
//        default: {
//          stream.invalidate();
//        } break;
//      }
//      return stream;
//  }
//
//  inline
//  const bsl::string& MyPerson::firstName() const
//  {
//      return d_firstName;
//  }
//
//  inline
//  const bsl::string& MyPerson::lastName() const
//  {
//      return d_lastName;
//  }
//
//  // FREE OPERATORS
//  inline
//  bool operator==(const MyPerson& lhs, const MyPerson& rhs)
//  {
//      return lhs.d_firstName == rhs.d_firstName &&
//             lhs.d_lastName  == rhs.d_lastName  &&
//             lhs.d_age       == rhs.d_age;
//  }
//
//  inline
//  bool operator!=(const MyPerson& lhs, const MyPerson& rhs)
//  {
//      return !(lhs == rhs);
//  }
//..
// Then, we can exercise the new 'MyPerson' value-semantic class by
// externalizing and reconstituting an object.  First, create a 'MyPerson'
// 'janeSmith' and a 'bslx::ByteOutStream' 'outStream':
//..
//  MyPerson            janeSmith("Jane", "Smith", 42);
//  bslx::ByteOutStream outStream(20131127);
//  const int           VERSION = 1;
//  outStream.putVersion(VERSION);
//  janeSmith.bdexStreamOut(outStream, VERSION);
//  assert(outStream.isValid());
//..
// Next, create a 'MyPerson' 'janeCopy' initialized to the default value, and
// assert that 'janeCopy' is different from 'janeSmith':
//..
//  MyPerson janeCopy;
//  assert(janeCopy != janeSmith);
//..
// Then, create a 'bslx::ByteInStream' 'inStream' initialized with the buffer
// from the 'bslx::ByteOutStream' object 'outStream' and unexternalize this
// data into 'janeCopy':
//..
//  bslx::ByteInStream inStream(outStream.data(), outStream.length());
//  int                version;
//  inStream.getVersion(version);
//  janeCopy.bdexStreamIn(inStream, version);
//  assert(inStream.isValid());
//..
// Finally, 'assert' the obtained values are as expected and display the
// results to 'bsl::stdout':
//..
//  assert(version  == VERSION);
//  assert(janeCopy == janeSmith);
//
//  if (janeCopy == janeSmith) {
//      bsl::cout << "Successfully serialized and de-serialized Jane Smith:"
//                << "\n\tFirstName: " << janeCopy.firstName()
//                << "\n\tLastName : " << janeCopy.lastName()
//                << "\n\tAge      : " << janeCopy.age() << bsl::endl;
//  }
//  else {
//      bsl::cout << "Serialization unsuccessful.  'janeCopy' holds:"
//                << "\n\tFirstName: " << janeCopy.firstName()
//                << "\n\tLastName : " << janeCopy.lastName()
//                << "\n\tAge      : " << janeCopy.age() << bsl::endl;
//  }
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLX_INSTREAMFUNCTIONS
#include <bslx_instreamfunctions.h>
#endif

#ifndef INCLUDED_BSLX_MARSHALLINGUTIL
#include <bslx_marshallingutil.h>
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

                         // ==================
                         // class ByteInStream
                         // ==================

class ByteInStream {
    // This class provides input methods to unexternalize values, and C-style
    // arrays of values, of the fundamental integral and floating-point types,
    // as well as 'bsl::string' values, using a byte format documented in the
    // 'bslx_byteoutstream' component.  In particular, each 'get' method of
    // this class is guaranteed to read stream data written by the
    // corresponding 'put' method of 'bslx::ByteOutStream'.  Note that
    // attempting to read beyond the end of a stream will automatically
    // invalidate the stream.  See the 'bslx' package-level documentation for
    // the definition of the BDEX 'InStream' protocol.

    // DATA
    const char  *d_buffer;     // bytes to be unexternalized

    bsl::size_t  d_numBytes;   // number of bytes in 'd_buffer'

    bool         d_validFlag;  // stream validity flag; 'true' if stream is in
                               // valid state, 'false' otherwise

    bsl::size_t  d_cursor;     // index of the next byte to be extracted from
                               // this stream

    // FRIENDS
    friend bsl::ostream& operator<<(bsl::ostream&       stream,
                                    const ByteInStream& object);

  private:
    // NOT IMPLEMENTED
    ByteInStream(const ByteInStream&);
    ByteInStream& operator=(const ByteInStream&);

  public:
    // CREATORS
    explicit ByteInStream();
        // Create an empty input byte stream.  Note that the constructed object
        // is useless until a buffer is set with the 'reset' method.

    ByteInStream(const char *buffer, bsl::size_t numBytes);
        // Create an input byte stream containing the specified initial
        // 'numBytes' from the specified 'buffer'.  The behavior is undefined
        // unless '0 == numBytes' if '0 == buffer'.

    explicit ByteInStream(const bslstl::StringRef& srcData);
        // Create an input byte stream containing the specified 'srcData'.

    ~ByteInStream();
        // Destroy this object.

    // MANIPULATORS
    ByteInStream& getLength(int& length);
        // If the most-significant bit of the one byte of this stream at the
        // current cursor location is set, assign to the specified 'length' the
        // four-byte, two's complement integer (in host byte order) comprised
        // of the four bytes of this stream at the current cursor location (in
        // network byte order) with the most-significant bit unset; otherwise,
        // assign to 'length' the one-byte, two's complement integer comprised
        // of the one byte of this stream at the current cursor location.
        // Update the cursor location and return a reference to this stream.
        // If this stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value, this
        // stream is marked invalid and the value of 'length' is undefined.
        // Note that the value will be zero-extended.

    ByteInStream& getVersion(int& version);
        // Assign to the specified 'version' the one-byte, two's complement
        // unsigned integer comprised of the one byte of this stream at the
        // current cursor location, update the cursor location, and return a
        // reference to this stream.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'version' is undefined.  Note that the value will be
        // zero-extended.

    void invalidate();
        // Put this input stream in an invalid state.  This function has no
        // effect if this stream is already invalid.  Note that this function
        // should be called whenever a value extracted from this stream is
        // determined to be invalid, inconsistent, or otherwise incorrect.

    void reset();
        // Set the index of the next byte to be extracted from this stream to 0
        // (i.e., the beginning of the stream) and validate this stream if it
        // is currently invalid.

    void reset(const char *buffer, bsl::size_t numBytes);
        // Reset this stream to extract from the specified 'buffer' containing
        // the specified 'numBytes', set the index of the next byte to be
        // extracted to 0 (i.e., the beginning of the stream), and validate
        // this stream if it is currently invalid.  The behavior is undefined
        // unless '0 == numBytes' if '0 == buffer'.

    void reset(const bslstl::StringRef& srcData);
        // Reset this stream to extract from the specified 'srcData', set the
        // index of the next byte to be extracted to 0 (i.e., the beginning of
        // the stream), and validate this stream if it is currently invalid.

                      // *** scalar integer values ***

    ByteInStream& getInt64(bsls::Types::Int64& variable);
        // Assign to the specified 'variable' the eight-byte, two's complement
        // integer (in host byte order) comprised of the eight bytes of this
        // stream at the current cursor location (in network byte order),
        // update the cursor location, and return a reference to this stream.
        // If this stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value, this
        // stream is marked invalid and the value of 'variable' is undefined.
        // Note that the value will be sign-extended.

    ByteInStream& getUint64(bsls::Types::Uint64& variable);
        // Assign to the specified 'variable' the eight-byte, two's complement
        // unsigned integer (in host byte order) comprised of the eight bytes
        // of this stream at the current cursor location (in network byte
        // order), update the cursor location, and return a reference to this
        // stream.  If this stream is initially invalid, this operation has no
        // effect.  If this function otherwise fails to extract a valid value,
        // this stream is marked invalid and the value of 'variable' is
        // undefined.  Note that the value will be zero-extended.

    ByteInStream& getInt56(bsls::Types::Int64& variable);
        // Assign to the specified 'variable' the seven-byte, two's complement
        // integer (in host byte order) comprised of the seven bytes of this
        // stream at the current cursor location (in network byte order),
        // update the cursor location, and return a reference to this stream.
        // If this stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value, this
        // stream is marked invalid and the value of 'variable' is undefined.
        // Note that the value will be sign-extended.

    ByteInStream& getUint56(bsls::Types::Uint64& variable);
        // Assign to the specified 'variable' the seven-byte, two's complement
        // unsigned integer (in host byte order) comprised of the seven bytes
        // of this stream at the current cursor location (in network byte
        // order), update the cursor location, and return a reference to this
        // stream.  If this stream is initially invalid, this operation has no
        // effect.  If this function otherwise fails to extract a valid value,
        // this stream is marked invalid and the value of 'variable' is
        // undefined.  Note that the value will be zero-extended.

    ByteInStream& getInt48(bsls::Types::Int64& variable);
        // Assign to the specified 'variable' the six-byte, two's complement
        // integer (in host byte order) comprised of the six bytes of this
        // stream at the current cursor location (in network byte order),
        // update the cursor location, and return a reference to this stream.
        // If this stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value, this
        // stream is marked invalid and the value of 'variable' is undefined.
        // Note that the value will be sign-extended.

    ByteInStream& getUint48(bsls::Types::Uint64& variable);
        // Assign to the specified 'variable' the six-byte, two's complement
        // unsigned integer (in host byte order) comprised of the six bytes of
        // this stream at the current cursor location (in network byte order),
        // update the cursor location, and return a reference to this stream.
        // If this stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value, this
        // stream is marked invalid and the value of 'variable' is undefined.
        // Note that the value will be zero-extended.

    ByteInStream& getInt40(bsls::Types::Int64& variable);
        // Assign to the specified 'variable' the five-byte, two's complement
        // integer (in host byte order) comprised of the five bytes of this
        // stream at the current cursor location (in network byte order),
        // update the cursor location, and return a reference to this stream.
        // If this stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value, this
        // stream is marked invalid and the value of 'variable' is undefined.
        // Note that the value will be sign-extended.

    ByteInStream& getUint40(bsls::Types::Uint64& variable);
        // Assign to the specified 'variable' the five-byte, two's complement
        // unsigned integer (in host byte order) comprised of the five bytes of
        // this stream at the current cursor location (in network byte order),
        // update the cursor location, and return a reference to this stream.
        // If this stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value, this
        // stream is marked invalid and the value of 'variable' is undefined.
        // Note that the value will be zero-extended.

    ByteInStream& getInt32(int& variable);
        // Assign to the specified 'variable' the four-byte, two's complement
        // integer (in host byte order) comprised of the four bytes of this
        // stream at the current cursor location (in network byte order),
        // update the cursor location, and return a reference to this stream.
        // If this stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value, this
        // stream is marked invalid and the value of 'variable' is undefined.
        // Note that the value will be sign-extended.

    ByteInStream& getUint32(unsigned int& variable);
        // Assign to the specified 'variable' the four-byte, two's complement
        // unsigned integer (in host byte order) comprised of the four bytes of
        // this stream at the current cursor location (in network byte order),
        // update the cursor location, and return a reference to this stream.
        // If this stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value, this
        // stream is marked invalid and the value of 'variable' is undefined.
        // Note that the value will be zero-extended.

    ByteInStream& getInt24(int& variable);
        // Assign to the specified 'variable' the three-byte, two's complement
        // integer (in host byte order) comprised of the three bytes of this
        // stream at the current cursor location (in network byte order),
        // update the cursor location, and return a reference to this stream.
        // If this stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value, this
        // stream is marked invalid and the value of 'variable' is undefined.
        // Note that the value will be sign-extended.

    ByteInStream& getUint24(unsigned int& variable);
        // Assign to the specified 'variable' the three-byte, two's complement
        // unsigned integer (in host byte order) comprised of the three bytes
        // of this stream at the current cursor location (in network byte
        // order), update the cursor location, and return a reference to this
        // stream.  If this stream is initially invalid, this operation has no
        // effect.  If this function otherwise fails to extract a valid value,
        // this stream is marked invalid and the value of 'variable' is
        // undefined.  Note that the value will be zero-extended.

    ByteInStream& getInt16(short& variable);
        // Assign to the specified 'variable' the two-byte, two's complement
        // integer (in host byte order) comprised of the two bytes of this
        // stream at the current cursor location (in network byte order),
        // update the cursor location, and return a reference to this stream.
        // If this stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value, this
        // stream is marked invalid and the value of 'variable' is undefined.
        // Note that the value will be sign-extended.

    ByteInStream& getUint16(unsigned short& variable);
        // Assign to the specified 'variable' the two-byte, two's complement
        // unsigned integer (in host byte order) comprised of the two bytes of
        // this stream at the current cursor location (in network byte order),
        // update the cursor location, and return a reference to this stream.
        // If this stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value, this
        // stream is marked invalid and the value of 'variable' is undefined.
        // Note that the value will be zero-extended.

    ByteInStream& getInt8(char&        variable);
    ByteInStream& getInt8(signed char& variable);
        // Assign to the specified 'variable' the one-byte, two's complement
        // integer comprised of the one byte of this stream at the current
        // cursor location, update the cursor location, and return a reference
        // to this stream.  If this stream is initially invalid, this operation
        // has no effect.  If this function otherwise fails to extract a valid
        // value, this stream is marked invalid and the value of 'variable' is
        // undefined.  Note that the value will be sign-extended.

    ByteInStream& getUint8(char&          variable);
    ByteInStream& getUint8(unsigned char& variable);
        // Assign to the specified 'variable' the one-byte, two's complement
        // unsigned integer comprised of the one byte of this stream at the
        // current cursor location, update the cursor location, and return a
        // reference to this stream.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.  Note that the value will be
        // zero-extended.

                      // *** scalar floating-point values ***

    ByteInStream& getFloat64(double& variable);
        // Assign to the specified 'variable' the eight-byte IEEE
        // double-precision floating-point number (in host byte order)
        // comprised of the eight bytes of this stream at the current cursor
        // location (in network byte order), update the cursor location, and
        // return a reference to this stream.  If this stream is initially
        // invalid, this operation has no effect.  If this function otherwise
        // fails to extract a valid value, this stream is marked invalid and
        // the value of 'variable' is undefined.

    ByteInStream& getFloat32(float& variable);
        // Assign to the specified 'variable' the four-byte IEEE
        // single-precision floating-point number (in host byte order)
        // comprised of the four bytes of this stream at the current cursor
        // location (in network byte order), update the cursor location, and
        // return a reference to this stream.  If this stream is initially
        // invalid, this operation has no effect.  If this function otherwise
        // fails to extract a valid value, this stream is marked invalid and
        // the value of 'variable' is undefined.

                      // *** string values ***

    ByteInStream& getString(bsl::string& variable);
        // Assign to the specified 'variable' the string comprised of the
        // length of the string (see 'getLength') and the string data (see
        // 'getUint8'), update the cursor location, and return a reference to
        // this stream.  If this stream is initially invalid, this operation
        // has no effect.  If this function otherwise fails to extract a valid
        // value, this stream is marked invalid and the value of 'variable' is
        // undefined.

                      // *** arrays of integer values ***

    ByteInStream& getArrayInt64(bsls::Types::Int64 *variables,
                                int                 numVariables);
        // Assign to the specified 'variables' the consecutive eight-byte,
        // two's complement integers (in host byte order) comprised of each of
        // the specified 'numVariables' eight-byte sequences of this stream at
        // the current cursor location (in network byte order), update the
        // cursor location, and return a reference to this stream.  If this
        // stream is initially invalid, this operation has no effect.  If this
        // function otherwise fails to extract a valid value, this stream is
        // marked invalid and the value of 'variables' is undefined.  The
        // behavior is undefined unless '0 <= numVariables' and 'variables' has
        // sufficient capacity.  Note that each of the values will be
        // sign-extended.

    ByteInStream& getArrayUint64(bsls::Types::Uint64 *variables,
                                 int                  numVariables);
        // Assign to the specified 'variables' the consecutive eight-byte,
        // two's complement unsigned integers (in host byte order) comprised of
        // each of the specified 'numVariables' eight-byte sequences of this
        // stream at the current cursor location (in network byte order),
        // update the cursor location, and return a reference to this stream.
        // If this stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value, this
        // stream is marked invalid and the value of 'variables' is undefined.
        // The behavior is undefined unless '0 <= numVariables' and 'variables'
        // has sufficient capacity.  Note that each of the values will be
        // zero-extended.

    ByteInStream& getArrayInt56(bsls::Types::Int64 *variables,
                                int                 numVariables);
        // Assign to the specified 'variables' the consecutive seven-byte,
        // two's complement integers (in host byte order) comprised of each of
        // the specified 'numVariables' seven-byte sequences of this stream at
        // the current cursor location (in network byte order), update the
        // cursor location, and return a reference to this stream.  If this
        // stream is initially invalid, this operation has no effect.  If this
        // function otherwise fails to extract a valid value, this stream is
        // marked invalid and the value of 'variables' is undefined.  The
        // behavior is undefined unless '0 <= numVariables' and 'variables' has
        // sufficient capacity.  Note that each of the values will be
        // sign-extended.

    ByteInStream& getArrayUint56(bsls::Types::Uint64 *variables,
                                 int                  numVariables);
        // Assign to the specified 'variables' the consecutive seven-byte,
        // two's complement unsigned integers (in host byte order) comprised of
        // each of the specified 'numVariables' seven-byte sequences of this
        // stream at the current cursor location (in network byte order),
        // update the cursor location, and return a reference to this stream.
        // If this stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value, this
        // stream is marked invalid and the value of 'variables' is undefined.
        // The behavior is undefined unless '0 <= numVariables' and 'variables'
        // has sufficient capacity.  Note that each of the values will be
        // zero-extended.

    ByteInStream& getArrayInt48(bsls::Types::Int64 *variables,
                                int                 numVariables);
        // Assign to the specified 'variables' the consecutive six-byte, two's
        // complement integers (in host byte order) comprised of each of the
        // specified 'numVariables' six-byte sequences of this stream at the
        // current cursor location (in network byte order), update the cursor
        // location, and return a reference to this stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid value, this stream is marked
        // invalid and the value of 'variables' is undefined.  The behavior is
        // undefined unless '0 <= numVariables' and 'variables' has sufficient
        // capacity.  Note that each of the values will be sign-extended.

    ByteInStream& getArrayUint48(bsls::Types::Uint64 *variables,
                                 int                  numVariables);
        // Assign to the specified 'variables' the consecutive six-byte, two's
        // complement unsigned integers (in host byte order) comprised of each
        // of the specified 'numVariables' six-byte sequences of this stream at
        // the current cursor location (in network byte order), update the
        // cursor location, and return a reference to this stream.  If this
        // stream is initially invalid, this operation has no effect.  If this
        // function otherwise fails to extract a valid value, this stream is
        // marked invalid and the value of 'variables' is undefined.  The
        // behavior is undefined unless '0 <= numVariables' and 'variables' has
        // sufficient capacity.  Note that each of the values will be
        // zero-extended.

    ByteInStream& getArrayInt40(bsls::Types::Int64 *variables,
                                int                 numVariables);
        // Assign to the specified 'variables' the consecutive five-byte, two's
        // complement integers (in host byte order) comprised of each of the
        // specified 'numVariables' five-byte sequences of this stream at the
        // current cursor location (in network byte order), update the cursor
        // location, and return a reference to this stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid value, this stream is marked
        // invalid and the value of 'variables' is undefined.  The behavior is
        // undefined unless '0 <= numVariables' and 'variables' has sufficient
        // capacity.  Note that each of the values will be sign-extended.

    ByteInStream& getArrayUint40(bsls::Types::Uint64 *variables,
                                 int                  numVariables);
        // Assign to the specified 'variables' the consecutive five-byte, two's
        // complement unsigned integers (in host byte order) comprised of each
        // of the specified 'numVariables' five-byte sequences of this stream
        // at the current cursor location (in network byte order), update the
        // cursor location, and return a reference to this stream.  If this
        // stream is initially invalid, this operation has no effect.  If this
        // function otherwise fails to extract a valid value, this stream is
        // marked invalid and the value of 'variables' is undefined.  The
        // behavior is undefined unless '0 <= numVariables' and 'variables' has
        // sufficient capacity.  Note that each of the values will be
        // zero-extended.

    ByteInStream& getArrayInt32(int *variables, int numVariables);
        // Assign to the specified 'variables' the consecutive four-byte, two's
        // complement integers (in host byte order) comprised of each of the
        // specified 'numVariables' four-byte sequences of this stream at the
        // current cursor location (in network byte order), update the cursor
        // location, and return a reference to this stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid value, this stream is marked
        // invalid and the value of 'variables' is undefined.  The behavior is
        // undefined unless '0 <= numVariables' and 'variables' has sufficient
        // capacity.  Note that each of the values will be sign-extended.

    ByteInStream& getArrayUint32(unsigned int *variables, int numVariables);
        // Assign to the specified 'variables' the consecutive four-byte, two's
        // complement unsigned integers (in host byte order) comprised of each
        // of the specified 'numVariables' four-byte sequences of this stream
        // at the current cursor location (in network byte order), update the
        // cursor location, and return a reference to this stream.  If this
        // stream is initially invalid, this operation has no effect.  If this
        // function otherwise fails to extract a valid value, this stream is
        // marked invalid and the value of 'variables' is undefined.  The
        // behavior is undefined unless '0 <= numVariables' and 'variables' has
        // sufficient capacity.  Note that each of the values will be
        // zero-extended.

    ByteInStream& getArrayInt24(int *variables, int numVariables);
        // Assign to the specified 'variables' the consecutive three-byte,
        // two's complement integers (in host byte order) comprised of each of
        // the specified 'numVariables' three-byte sequences of this stream at
        // the current cursor location (in network byte order), update the
        // cursor location, and return a reference to this stream.  If this
        // stream is initially invalid, this operation has no effect.  If this
        // function otherwise fails to extract a valid value, this stream is
        // marked invalid and the value of 'variables' is undefined.  The
        // behavior is undefined unless '0 <= numValues' and 'variables' has
        // sufficient capacity.  Note that each of the values will be
        // sign-extended.

    ByteInStream& getArrayUint24(unsigned int *variables, int numVariables);
        // Assign to the specified 'variables' the consecutive three-byte,
        // two's complement unsigned integers (in host byte order) comprised of
        // each of the specified 'numVariables' three-byte sequences of this
        // stream at the current cursor location (in network byte order),
        // update the cursor location, and return a reference to this stream.
        // If this stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value, this
        // stream is marked invalid and the value of 'variables' is undefined.
        // The behavior is undefined unless '0 <= numVariables' and 'variables'
        // has sufficient capacity.  Note that each of the values will be
        // zero-extended.

    ByteInStream& getArrayInt16(short *variables, int numVariables);
        // Assign to the specified 'variables' the consecutive two-byte, two's
        // complement integers (in host byte order) comprised of each of the
        // specified 'numVariables' two-byte sequences of this stream at the
        // current cursor location (in network byte order), update the cursor
        // location, and return a reference to this stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid value, this stream is marked
        // invalid and the value of 'variables' is undefined.  The behavior is
        // undefined unless '0 <= numVariables' and 'variables' has sufficient
        // capacity.  Note that each of the values will be sign-extended.

    ByteInStream& getArrayUint16(unsigned short *variables, int numVariables);
        // Assign to the specified 'variables' the consecutive two-byte, two's
        // complement unsigned integers (in host byte order) comprised of each
        // of the specified 'numVariables' two-byte sequences of this stream at
        // the current cursor location (in network byte order), update the
        // cursor location, and return a reference to this stream.  If this
        // stream is initially invalid, this operation has no effect.  If this
        // function otherwise fails to extract a valid value, this stream is
        // marked invalid and the value of 'variables' is undefined.  The
        // behavior is undefined unless '0 <= numVariables' and 'variables' has
        // sufficient capacity.  Note that each of the values will be
        // zero-extended.

    ByteInStream& getArrayInt8(char *variables,        int numVariables);
    ByteInStream& getArrayInt8(signed char *variables, int numVariables);
        // Assign to the specified 'variables' the consecutive one-byte, two's
        // complement integers comprised of each of the specified
        // 'numVariables' one-byte sequences of this stream at the current
        // cursor location, update the cursor location, and return a reference
        // to this stream.  If this stream is initially invalid, this operation
        // has no effect.  If this function otherwise fails to extract a valid
        // value, this stream is marked invalid and the value of 'variables' is
        // undefined.  The behavior is undefined unless '0 <= numVariables' and
        // 'variables' has sufficient capacity.  Note that each of the values
        // will be sign-extended.

    ByteInStream& getArrayUint8(char *variables,          int numVariables);
    ByteInStream& getArrayUint8(unsigned char *variables, int numVariables);
        // Assign to the specified 'variables' the consecutive one-byte, two's
        // complement unsigned integers comprised of each of the specified
        // 'numVariables' one-byte sequences of this stream at the current
        // cursor location, update the cursor location, and return a reference
        // to this stream.  If this stream is initially invalid, this operation
        // has no effect.  If this function otherwise fails to extract a valid
        // value, this stream is marked invalid and the value of 'variables' is
        // undefined.  The behavior is undefined unless '0 <= numVariables' and
        // 'variables' has sufficient capacity.  Note that each of the values
        // will be zero-extended.

                      // *** arrays of floating-point values ***

    ByteInStream& getArrayFloat64(double *variables, int numVariables);
        // Assign to the specified 'variables' the consecutive eight-byte IEEE
        // double-precision floating-point numbers (in host byte order)
        // comprised of each of the specified 'numVariables' eight-byte
        // sequences of this stream at the current cursor location (in network
        // byte order), update the cursor location, and return a reference to
        // this stream.  If this stream is initially invalid, this operation
        // has no effect.  If this function otherwise fails to extract a valid
        // value, this stream is marked invalid and the value of 'variables' is
        // undefined.  The behavior is undefined unless '0 <= numVariables' and
        // 'variables' has sufficient capacity.

    ByteInStream& getArrayFloat32(float *variables, int numVariables);
        // Assign to the specified 'variables' the consecutive four-byte IEEE
        // single-precision floating-point numbers (in host byte order)
        // comprised of each of the specified 'numVariables' four-byte
        // sequences of this stream at the current cursor location (in network
        // byte order), update the cursor location, and return a reference to
        // this stream.  If this stream is initially invalid, this operation
        // has no effect.  If this function otherwise fails to extract a valid
        // value, this stream is marked invalid and the value of 'variables' is
        // undefined.  The behavior is undefined unless '0 <= numVariables' and
        // 'variables' has sufficient capacity.

    // ACCESSORS
    operator const void *() const;
        // Return a non-zero value if this stream is valid, and 0 otherwise.
        // An invalid stream is a stream for which an input operation was
        // detected to have failed.

    bsl::size_t cursor() const;
        // Return the index of the next byte to be extracted from this stream.

    const char *data() const;
        // Return the address of the contiguous, non-modifiable external memory
        // buffer of this stream.  The behavior of accessing elements outside
        // the range '[ data() .. data() + (length() - 1) ]' is undefined.

    bool isEmpty() const;
        // Return 'true' if this stream is empty, and 'false' otherwise.  Note
        // that this function enables higher-level types to verify that, after
        // successfully reading all expected data, no data remains.

    bool isValid() const;
        // Return 'true' if this stream is valid, and 'false' otherwise.  An
        // invalid stream is a stream in which insufficient or invalid data was
        // detected during an extraction operation.  Note that an empty stream
        // will be valid unless an extraction attempt or explicit invalidation
        // causes it to be otherwise.

    bsl::size_t length() const;
        // Return the total number of bytes stored in the external memory
        // buffer.
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&       stream,
                         const ByteInStream& object);
    // Write the specified 'object' to the specified output 'stream' in some
    // reasonable (multi-line) format, and return a reference to 'stream'.

template <class TYPE>
ByteInStream& operator>>(ByteInStream& stream, TYPE& value);
    // Read the specified 'value' from the specified input 'stream' following
    // the requirements of the BDEX protocol (see the 'bslx' package-level
    // documentation), and return a reference to  'stream'.  The behavior is
    // undefined unless 'TYPE' is BDEX-compliant.

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                         // ------------------
                         // class ByteInStream
                         // ------------------

// CREATORS
inline
ByteInStream::ByteInStream()
: d_buffer(0)
, d_numBytes(0)
, d_validFlag(true)
, d_cursor(0)
{
}

inline
ByteInStream::ByteInStream(const char *buffer, bsl::size_t numBytes)
: d_buffer(buffer)
, d_numBytes(numBytes)
, d_validFlag(true)
, d_cursor(0)
{
    BSLS_ASSERT_SAFE(buffer || 0 == numBytes);
}

inline
ByteInStream::ByteInStream(const bslstl::StringRef& srcData)
: d_buffer(srcData.data())
, d_numBytes(static_cast<int>(srcData.length()))
, d_validFlag(true)
, d_cursor(0)
{
}

inline
ByteInStream::~ByteInStream()
{
}

// MANIPULATORS
inline
ByteInStream& ByteInStream::getLength(int& length)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    if (cursor() + MarshallingUtil::k_SIZEOF_INT8 <= ByteInStream::length()) {
        if (127 < static_cast<unsigned char>(d_buffer[cursor()])) {
            // If 'length > 127', 'length' is stored as 4 bytes with top bit
            // set.

            getInt32(length);
            length &= 0x7fffffff;  // Clear top bit.
        }
        else {
            // If 'length <= 127', 'length' is stored as one byte.

            char tmp;
            MarshallingUtil::getInt8(&tmp, data() + cursor());
            d_cursor += MarshallingUtil::k_SIZEOF_INT8;
            length = tmp;
        }
    }
    else {
        invalidate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getVersion(int& version)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    unsigned char tmp;
    getUint8(tmp);
    version = tmp;

    return *this;
}

inline
void ByteInStream::invalidate()
{
    d_validFlag = false;
}

inline
void ByteInStream::reset()
{
    d_validFlag = true;
    d_cursor    = 0;
}

inline
void ByteInStream::reset(const char *buffer, bsl::size_t numBytes)
{
    BSLS_ASSERT_SAFE(buffer || 0 == numBytes);

    d_buffer    = buffer;
    d_numBytes  = numBytes;
    d_validFlag = true;
    d_cursor    = 0;
}

inline
void ByteInStream::reset(const bslstl::StringRef& srcData)
{
    d_buffer    = srcData.data();
    d_numBytes  = srcData.length();
    d_validFlag = true;
    d_cursor    = 0;
}

                      // *** scalar integer values ***

inline
ByteInStream& ByteInStream::getInt64(bsls::Types::Int64& variable)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    if (cursor() + MarshallingUtil::k_SIZEOF_INT64 <= length()) {
        MarshallingUtil::getInt64(&variable, d_buffer + cursor());
        d_cursor += MarshallingUtil::k_SIZEOF_INT64;
    }
    else {
        invalidate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getUint64(bsls::Types::Uint64& variable)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    if (cursor() + MarshallingUtil::k_SIZEOF_INT64 <= length()) {
        MarshallingUtil::getUint64(&variable, d_buffer + cursor());
        d_cursor += MarshallingUtil::k_SIZEOF_INT64;
    }
    else {
        invalidate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getInt56(bsls::Types::Int64& variable)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    if (cursor() + MarshallingUtil::k_SIZEOF_INT56 <= length()) {
        MarshallingUtil::getInt56(&variable, d_buffer + cursor());
        d_cursor += MarshallingUtil::k_SIZEOF_INT56;
    }
    else {
        invalidate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getUint56(bsls::Types::Uint64& variable)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    if (cursor() + MarshallingUtil::k_SIZEOF_INT56 <= length()) {
        MarshallingUtil::getUint56(&variable, d_buffer + cursor());
        d_cursor += MarshallingUtil::k_SIZEOF_INT56;
    }
    else {
        invalidate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getInt48(bsls::Types::Int64& variable)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    if (cursor() + MarshallingUtil::k_SIZEOF_INT48 <= length()) {
        MarshallingUtil::getInt48(&variable, d_buffer + cursor());
        d_cursor += MarshallingUtil::k_SIZEOF_INT48;
    }
    else {
        invalidate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getUint48(bsls::Types::Uint64& variable)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    if (cursor() + MarshallingUtil::k_SIZEOF_INT48 <= length()) {
        MarshallingUtil::getUint48(&variable, d_buffer + cursor());
        d_cursor += MarshallingUtil::k_SIZEOF_INT48;
    }
    else {
        invalidate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getInt40(bsls::Types::Int64& variable)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    if (cursor() + MarshallingUtil::k_SIZEOF_INT40 <= length()) {
        MarshallingUtil::getInt40(&variable, d_buffer + cursor());
        d_cursor += MarshallingUtil::k_SIZEOF_INT40;
    }
    else {
        invalidate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getUint40(bsls::Types::Uint64& variable)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    if (cursor() + MarshallingUtil::k_SIZEOF_INT40 <= length()) {
        MarshallingUtil::getUint40(&variable, d_buffer + cursor());
        d_cursor += MarshallingUtil::k_SIZEOF_INT40;
    }
    else {
        invalidate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getInt32(int& variable)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    if (cursor() + MarshallingUtil::k_SIZEOF_INT32 <= length()) {
        MarshallingUtil::getInt32(&variable, d_buffer + cursor());
        d_cursor += MarshallingUtil::k_SIZEOF_INT32;
    }
    else {
        invalidate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getUint32(unsigned int& variable)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    if (cursor() + MarshallingUtil::k_SIZEOF_INT32 <= length()) {
        MarshallingUtil::getUint32(&variable, d_buffer + cursor());
        d_cursor += MarshallingUtil::k_SIZEOF_INT32;
    }
    else {
        invalidate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getInt24(int& variable)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    if (cursor() + MarshallingUtil::k_SIZEOF_INT24 <= length()) {
        MarshallingUtil::getInt24(&variable, d_buffer + cursor());
        d_cursor += MarshallingUtil::k_SIZEOF_INT24;
    }
    else {
        invalidate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getUint24(unsigned int& variable)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    if (cursor() + MarshallingUtil::k_SIZEOF_INT24 <= length()) {
        MarshallingUtil::getUint24(&variable, d_buffer + cursor());
        d_cursor += MarshallingUtil::k_SIZEOF_INT24;
    }
    else {
        invalidate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getInt16(short& variable)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    if (cursor() + MarshallingUtil::k_SIZEOF_INT16 <= length()) {
        MarshallingUtil::getInt16(&variable, d_buffer + cursor());
        d_cursor += MarshallingUtil::k_SIZEOF_INT16;
    }
    else {
        invalidate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getUint16(unsigned short& variable)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    if (cursor() + MarshallingUtil::k_SIZEOF_INT16 <= length()) {
        MarshallingUtil::getUint16(&variable, d_buffer + cursor());
        d_cursor += MarshallingUtil::k_SIZEOF_INT16;
    }
    else {
        invalidate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getInt8(char& variable)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    if (cursor() + MarshallingUtil::k_SIZEOF_INT8 <= length()) {
        MarshallingUtil::getInt8(&variable, data() + cursor());
        d_cursor += MarshallingUtil::k_SIZEOF_INT8;
    }
    else {
        invalidate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getInt8(signed char& variable)
{
    return getInt8(reinterpret_cast<char&>(variable));
}

inline
ByteInStream& ByteInStream::getUint8(char& variable)
{
    return getInt8(variable);
}

inline
ByteInStream& ByteInStream::getUint8(unsigned char& variable)
{
    return getInt8(reinterpret_cast<char&>(variable));
}

                      // *** scalar floating-point values ***

inline
ByteInStream& ByteInStream::getFloat64(double& variable)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    if (cursor() + MarshallingUtil::k_SIZEOF_FLOAT64 <= length()) {
        MarshallingUtil::getFloat64(&variable, d_buffer + cursor());
        d_cursor += MarshallingUtil::k_SIZEOF_FLOAT64;
    }
    else {
        invalidate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getFloat32(float& variable)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    if (cursor() + MarshallingUtil::k_SIZEOF_FLOAT32 <= length()) {
        MarshallingUtil::getFloat32(&variable, d_buffer + cursor());
        d_cursor += MarshallingUtil::k_SIZEOF_FLOAT32;
    }
    else {
        invalidate();
    }

    return *this;
}

                      // *** string values ***

inline
ByteInStream& ByteInStream::getString(bsl::string& variable)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    int length;
    getLength(length);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    // 'length' could be corrupt or invalid, so we limit the initial 'resize'
    // to something that can accommodate the preponderance of strings that will
    // arise in practice.  The remaining portion of a string longer than 16M is
    // read in via a second pass.

    enum { k_INITIAL_ALLOCATION_SIZE = 16 * 1024 * 1024 };

    const int initialLength = length < k_INITIAL_ALLOCATION_SIZE
                              ? length
                              : k_INITIAL_ALLOCATION_SIZE;

    variable.resize(initialLength);

    if (0 == length) {
        return *this;                                                 // RETURN
    }

    getArrayUint8(&variable.front(), initialLength);
    if (isValid() && length > initialLength) {
        variable.resize(length);
        getArrayUint8(&variable[initialLength], length - initialLength);
    }

    return *this;
}

                      // *** arrays of integer values ***

inline
ByteInStream& ByteInStream::getArrayInt64(bsls::Types::Int64 *variables,
                                          int                 numVariables)
{
    BSLS_ASSERT_SAFE(variables);
    BSLS_ASSERT_SAFE(0 <= numVariables);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const int len = MarshallingUtil::k_SIZEOF_INT64 * numVariables;
    if (cursor() + len  <= length()) {
        MarshallingUtil::getArrayInt64(variables,
                                       d_buffer + cursor(),
                                       numVariables);
        d_cursor += len;
    }
    else {
        invalidate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getArrayUint64(bsls::Types::Uint64 *variables,
                                           int                  numVariables)
{
    BSLS_ASSERT_SAFE(variables);
    BSLS_ASSERT_SAFE(0 <= numVariables);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const int len = MarshallingUtil::k_SIZEOF_INT64 * numVariables;
    if (cursor() + len  <= length()) {
        MarshallingUtil::getArrayUint64(variables,
                                        d_buffer + cursor(),
                                        numVariables);
        d_cursor += len;
    }
    else {
        invalidate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getArrayInt56(bsls::Types::Int64 *variables,
                                          int                 numVariables)
{
    BSLS_ASSERT_SAFE(variables);
    BSLS_ASSERT_SAFE(0 <= numVariables);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const int len = MarshallingUtil::k_SIZEOF_INT56 * numVariables;
    if (cursor() + len  <= length()) {
        MarshallingUtil::getArrayInt56(variables,
                                       d_buffer + cursor(),
                                       numVariables);
        d_cursor += len;
    }
    else {
        invalidate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getArrayUint56(bsls::Types::Uint64 *variables,
                                           int                  numVariables)
{
    BSLS_ASSERT_SAFE(variables);
    BSLS_ASSERT_SAFE(0 <= numVariables);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const int len = MarshallingUtil::k_SIZEOF_INT56 * numVariables;
    if (cursor() + len  <= length()) {
        MarshallingUtil::getArrayUint56(variables,
                                        d_buffer + cursor(),
                                        numVariables);
        d_cursor += len;
    }
    else {
        invalidate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getArrayInt48(bsls::Types::Int64 *variables,
                                          int                 numVariables)
{
    BSLS_ASSERT_SAFE(variables);
    BSLS_ASSERT_SAFE(0 <= numVariables);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const int len = MarshallingUtil::k_SIZEOF_INT48 * numVariables;
    if (cursor() + len  <= length()) {
        MarshallingUtil::getArrayInt48(variables,
                                       d_buffer + cursor(),
                                       numVariables);
        d_cursor += len;
    }
    else {
        invalidate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getArrayUint48(bsls::Types::Uint64 *variables,
                                           int                  numVariables)
{
    BSLS_ASSERT_SAFE(variables);
    BSLS_ASSERT_SAFE(0 <= numVariables);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const int len = MarshallingUtil::k_SIZEOF_INT48 * numVariables;
    if (cursor() + len  <= length()) {
        MarshallingUtil::getArrayUint48(variables,
                                        d_buffer + cursor(),
                                        numVariables);
        d_cursor += len;
    }
    else {
        invalidate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getArrayInt40(bsls::Types::Int64 *variables,
                                          int                 numVariables)
{
    BSLS_ASSERT_SAFE(variables);
    BSLS_ASSERT_SAFE(0 <= numVariables);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const int len = MarshallingUtil::k_SIZEOF_INT40 * numVariables;
    if (cursor() + len  <= length()) {
        MarshallingUtil::getArrayInt40(variables,
                                       d_buffer + cursor(),
                                       numVariables);
        d_cursor += len;
    }
    else {
        invalidate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getArrayUint40(bsls::Types::Uint64 *variables,
                                           int                  numVariables)
{
    BSLS_ASSERT_SAFE(variables);
    BSLS_ASSERT_SAFE(0 <= numVariables);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const int len = MarshallingUtil::k_SIZEOF_INT40 * numVariables;
    if (cursor() + len  <= length()) {
        MarshallingUtil::getArrayUint40(variables,
                                        d_buffer + cursor(),
                                        numVariables);
        d_cursor += len;
    }
    else {
        invalidate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getArrayInt32(int *variables, int numVariables)
{
    BSLS_ASSERT_SAFE(variables);
    BSLS_ASSERT_SAFE(0 <= numVariables);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const int len = MarshallingUtil::k_SIZEOF_INT32 * numVariables;
    if (cursor() + len  <= length()) {
        MarshallingUtil::getArrayInt32(variables,
                                       d_buffer + cursor(),
                                       numVariables);
        d_cursor += len;
    }
    else {
        invalidate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getArrayUint32(unsigned int *variables,
                                           int           numVariables)
{
    BSLS_ASSERT_SAFE(variables);
    BSLS_ASSERT_SAFE(0 <= numVariables);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const int len = MarshallingUtil::k_SIZEOF_INT32 * numVariables;
    if (cursor() + len  <= length()) {
        MarshallingUtil::getArrayUint32(variables,
                                        d_buffer + cursor(),
                                        numVariables);
        d_cursor += len;
    }
    else {
        invalidate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getArrayInt24(int *variables, int numVariables)
{
    BSLS_ASSERT_SAFE(variables);
    BSLS_ASSERT_SAFE(0 <= numVariables);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const int len = MarshallingUtil::k_SIZEOF_INT24 * numVariables;
    if (cursor() + len  <= length()) {
        MarshallingUtil::getArrayInt24(variables,
                                       d_buffer + cursor(),
                                       numVariables);
        d_cursor += len;
    }
    else {
        invalidate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getArrayUint24(unsigned int *variables,
                                           int           numVariables)
{
    BSLS_ASSERT_SAFE(variables);
    BSLS_ASSERT_SAFE(0 <= numVariables);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const int len = MarshallingUtil::k_SIZEOF_INT24 * numVariables;
    if (cursor() + len  <= length()) {
        MarshallingUtil::getArrayUint24(variables,
                                        d_buffer + cursor(),
                                        numVariables);
        d_cursor += len;
    }
    else {
        invalidate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getArrayInt16(short *variables, int numVariables)
{
    BSLS_ASSERT_SAFE(variables);
    BSLS_ASSERT_SAFE(0 <= numVariables);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const int len = MarshallingUtil::k_SIZEOF_INT16 * numVariables;
    if (cursor() + len  <= length()) {
        MarshallingUtil::getArrayInt16(variables,
                                       d_buffer + cursor(),
                                       numVariables);
        d_cursor += len;
    }
    else {
        invalidate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getArrayUint16(unsigned short *variables,
                                           int             numVariables)
{
    BSLS_ASSERT_SAFE(variables);
    BSLS_ASSERT_SAFE(0 <= numVariables);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const int len = MarshallingUtil::k_SIZEOF_INT16 * numVariables;
    if (cursor() + len  <= length()) {
        MarshallingUtil::getArrayUint16(variables,
                                        d_buffer + cursor(),
                                        numVariables);
        d_cursor += len;
    }
    else {
        invalidate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getArrayInt8(char *variables, int numVariables)
{
    BSLS_ASSERT_SAFE(variables);
    BSLS_ASSERT_SAFE(0 <= numVariables);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const int len = MarshallingUtil::k_SIZEOF_INT8 * numVariables;
    if (cursor() + len  <= length()) {
        MarshallingUtil::getArrayInt8(variables,
                                      d_buffer + cursor(),
                                      numVariables);
        d_cursor += len;
    }
    else {
        invalidate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getArrayInt8(signed char *variables,
                                         int          numVariables)
{
    BSLS_ASSERT_SAFE(variables);
    BSLS_ASSERT_SAFE(0 <= numVariables);

    return getArrayInt8(reinterpret_cast<char *>(variables), numVariables);
}

inline
ByteInStream& ByteInStream::getArrayUint8(char *variables, int numVariables)
{
    BSLS_ASSERT_SAFE(variables);
    BSLS_ASSERT_SAFE(0 <= numVariables);

    return getArrayInt8(variables, numVariables);
}

inline
ByteInStream& ByteInStream::getArrayUint8(unsigned char *variables,
                                          int            numVariables)
{
    BSLS_ASSERT_SAFE(variables);
    BSLS_ASSERT_SAFE(0 <= numVariables);

    return getArrayInt8(reinterpret_cast<char *>(variables), numVariables);
}

                      // *** arrays of floating-point values ***

inline
ByteInStream& ByteInStream::getArrayFloat64(double *variables,
                                            int     numVariables)
{
    BSLS_ASSERT_SAFE(variables);
    BSLS_ASSERT_SAFE(0 <= numVariables);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const int len = MarshallingUtil::k_SIZEOF_FLOAT64 * numVariables;
    if (cursor() + len  <= length()) {
        MarshallingUtil::getArrayFloat64(variables,
                                         d_buffer + cursor(),
                                         numVariables);
        d_cursor += len;
    }
    else {
        invalidate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getArrayFloat32(float *variables, int numVariables)
{
    BSLS_ASSERT_SAFE(variables);
    BSLS_ASSERT_SAFE(0 <= numVariables);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const int len = MarshallingUtil::k_SIZEOF_FLOAT32 * numVariables;
    if (cursor() + len  <= length()) {
        MarshallingUtil::getArrayFloat32(variables,
                                         d_buffer + cursor(),
                                         numVariables);
        d_cursor += len;
    }
    else {
        invalidate();
    }

    return *this;
}

// ACCESSORS
inline
ByteInStream::operator const void *() const
{
    return isValid() ? this : 0;
}

inline
bsl::size_t ByteInStream::cursor() const
{
    return d_cursor;
}

inline
const char *ByteInStream::data() const
{
    return d_numBytes ? d_buffer : 0;
}

inline
bool ByteInStream::isEmpty() const
{
    return cursor() == length();
}

inline
bool ByteInStream::isValid() const
{
    return d_validFlag;
}

inline
bsl::size_t ByteInStream::length() const
{
    return d_numBytes;
}

template <class TYPE>
inline
ByteInStream& operator>>(ByteInStream& stream, TYPE& value)
{
    return InStreamFunctions::bdexStreamIn(stream, value);
}

}  // close package namespace
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
