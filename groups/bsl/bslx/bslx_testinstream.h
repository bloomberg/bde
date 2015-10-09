// bslx_testinstream.h                                                -*-C++-*-
#ifndef INCLUDED_BSLX_TESTINSTREAM
#define INCLUDED_BSLX_TESTINSTREAM

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Enable unexternalization of fundamental types with identification.
//
//@CLASSES:
//  bslx::TestInStream: byte-array-based input stream class
//
//@MACROS:
//  BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN: macro to begin testing exceptions
//  BSLX_TESTINSTREAM_EXCEPTION_TEST_END: macro to end testing exceptions
//
//@SEE_ALSO: bslx_testoutstream, bslx_byteinstream
//
//@DESCRIPTION: This component implements a byte-array-based input stream
// class, 'bslx::TestInStream', that provides platform-independent input
// methods ("unexternalization") on values, and arrays of values, of
// fundamental types, and on 'bsl::string'.  'bslx::TestInStream' also
// verifies, for these types, that the type of data requested from the stream
// matches what was written to the stream.  'bslx::TestInStream' is meant for
// testing only.
//
// The 'bslx::TestInStream' type reads from a user-supplied buffer directly,
// with no data copying or assumption of ownership.  The user must therefore
// make sure that the lifetime and visibility of the buffer is sufficient to
// satisfy the needs of the input stream.
//
// This component is intended to be used in conjunction with the
// 'bslx_testoutstream' externalization component.  Each input method of
// 'bslx::TestInStream' reads either a value or a homogeneous array of values
// of a fundamental type, in a format that was written by the corresponding
// 'bslx::TestOutStream' method.  In general, the user of this component cannot
// rely on being able to read data that was written by any mechanism other than
// 'bslx::TestOutStream'.
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
///Input Limit
///-----------
// If exceptions are enabled at compile time, the test input stream can be
// configured to throw an exception after a specified number of input requests
// is exceeded.  If the input limit is less than zero (default), then the
// stream never throws an exception.  Note that a non-negative input limit is
// decremented after each input attempt, and throws only when the current input
// limit transitions from 0 to -1; no additional exceptions will be thrown
// until the input limit is again reset to a non-negative value.
//
// The input limit is set using the 'setInputLimit' manipulator.
//
///Exception Test Macros
///---------------------
// This component also provides a pair of macros:
//
//: o 'BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(testInStream)'
//: o 'BSLX_TESTINSTREAM_EXCEPTION_TEST_END'
//
// These macros can be used for testing exception-safety of classes and their
// methods when BDEX streaming is involved.  A reference to an object of type
// 'bslx::TestInStream' must be supplied as an argument to the '*_BEGIN' macro.
// Note that if exception-handling is disabled (i.e., if
// '-DBDE_BUILD_TARGET_EXC' was *not* supplied at compile time), then the
// macros simply print the following:
//..
//  BSLX EXCEPTION TEST -- (NOT ENABLED) --
//..
// When exception-handling is enabled (i.e., if '-DBDE_BUILD_TARGET_EXC' was
// supplied at compile time), the '*_BEGIN' macro will set the input limit of
// the supplied instream to 0, 'try' the code being tested, 'catch' any
// 'TestInstreamException's that are thrown, and keep increasing the input
// limit until the code being tested completes successfully.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Unexternalization Test
///- - - - - - - - - - - - - - - - - - - -
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
//      const bsl::string& firstName() const;
//          // Return the first name of this person.
//
//      const bsl::string& lastName() const;
//          // Return the last name of this person.
//
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
//  bsl::ostream& operator<<(bsl::ostream& stream, const MyPerson& person);
//      // Write the specified 'person' value to the specified output 'stream'
//      // in some reasonable format, and return a reference to 'stream'.
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
//                  return stream;
//              }
//              stream.getString(d_lastName);
//              if (!stream) {
//                  d_lastName = "stream error";  // *might* be corrupted;
//                                                //  value for testing
//                  return stream;
//              }
//              stream.getInt32(d_age);
//              if (!stream) {
//                  d_age = 999;     // *might* be corrupted; value for testing
//                  return stream;
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
//..
// Then, we can exercise the new 'MyPerson' value-semantic class by
// externalizing and reconstituting an object.  First, create a 'MyPerson'
// 'janeSmith' and a 'bslx::TestOutStream' 'outStream':
//..
//  MyPerson janeSmith("Jane", "Smith", 42);
//  bslx::TestOutStream outStream(20131127);
//  const int VERSION = 1;
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
// Then, create a 'bslx::TestInStream' 'inStream' initialized with the buffer
// from the 'bslx::TestOutStream' object 'outStream' and unexternalize this
// data into 'janeCopy':
//..
//  bslx::TestInStream inStream(outStream.data(), outStream.length());
//  int version;
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

#ifndef INCLUDED_BSLX_TESTINSTREAMEXCEPTION
#include <bslx_testinstreamexception.h>
#endif

#ifndef INCLUDED_BSLX_TYPECODE
#include <bslx_typecode.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
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
                         // class TestInStream
                         // ==================

class TestInStream {
    // This class provides input methods to unexternalize values, and C-style
    // arrays of values, of fundamental types from their byte representations.
    // Each input method also verifies the input value type.  By default, if
    // invalid data is detected, error messages are displayed on 'stdout'; this
    // error reporting may be disabled via the 'setQuiet' method.  Note that
    // attempting to read beyond the end of a stream will automatically
    // invalidate the stream.  See the 'bslx' package-level documentation for
    // the definition of the BDEX 'InStream' protocol.

    // DATA
    const char  *d_buffer;      // bytes to be unexternalized

    bsl::size_t  d_numBytes;    // number of bytes in 'd_buffer'

    bool         d_validFlag;   // stream validity flag; 'true' if stream is in
                                // valid state, 'false' otherwise

    int          d_quietFlag;   // flag for "quiet" mode

    int          d_inputLimit;  // number of input op's before exception

    bsl::size_t  d_cursor;      // index of the next byte to be extracted from
                                // this stream

    // FRIENDS
    friend bsl::ostream& operator<<(bsl::ostream&, const TestInStream&);

    // NOT IMPLEMENTED
    TestInStream(const TestInStream&);
    TestInStream& operator=(const TestInStream&);

  private:
    // PRIVATE MANIPULATORS
    void checkArray(TypeCode::Enum code,
                    int            elementSize,
                    int            numElements);
        // Verify the validity of the type code and array length, and the
        // sufficiency of data at the current cursor position in the external
        // memory buffer.  Extract the type code at the cursor position from
        // the buffer.  If the type code does not correspond to the specified
        // 'code', then mark this stream as invalid, and if the quiet flag is
        // zero print an error message.  Otherwise, advance the cursor by the
        // size of the type code and extract the array length.  If the length
        // does not correspond to the specified 'numElements', then mark this
        // stream as invalid, and if the quiet flag is zero print an error
        // message.  Otherwise, advance the cursor by the size of the array
        // length, and verify that the buffer contains sufficient bytes for
        // 'numElements' of the specified 'elementSize'.  If there are too few
        // bytes in the buffer, then mark this stream as invalid.  If this
        // stream is invalid on entry, this function has no effect.  The
        // behavior is undefined unless '0 < elementSize' and
        // '0 <= numElements'.  Note that error messages are not printed for
        // insufficient data in the buffer.

    void checkTypeCodeAndAvailableLength(TypeCode::Enum code,
                                         bsl::size_t    numExpectedBytes);
        // Verify the validity of the type code and the sufficiency of data at
        // the current cursor position in the external memory buffer.  Extract
        // the type code at the cursor position from the buffer.  If the type
        // code does not correspond to the specified 'code', then mark this
        // stream as invalid and, if the quiet flag is zero, print an error
        // message.  Otherwise, advance the cursor position by the size of the
        // type code, and verify that the buffer contains sufficient bytes for
        // the specified 'numExpectedBytes'.  If there are too few bytes, then
        // this stream is marked as invalid.  If this stream is invalid on
        // entry, this function has no effect.  The behavior is undefined
        // unless '0 < numExpectedBytes'.  Also note that error messages are
        // not printed for insufficient data in the buffer.

    void throwExceptionIfInputLimitExhausted(const TypeCode::Enum& code);
        // Decrement the internal input limit of this test stream.  If the
        // input limit becomes negative and exception-handling is enabled
        // (i.e., '-DBDE_BUILD_TARGET_EXC' was supplied at compile time), then
        // throw a 'TestInStreamException' object initialized with the
        // specified type 'code'.  If exception-handling is not enabled, this
        // method has no effect.

  public:
    // CREATORS
    explicit TestInStream();
        // Create an empty test input stream.  Note that the constructed object
        // is useless until a buffer is set with the 'reset' method.

    TestInStream(const char *buffer, bsl::size_t numBytes);
        // Create a test input stream containing the specified initial
        // 'numBytes' from the specified 'buffer'.  The behavior is undefined
        // unless '0 == numBytes' if '0 == buffer'.

    explicit TestInStream(const bslstl::StringRef& srcData);
        // Create a test input stream containing the specified 'srcData'.

    ~TestInStream();
        // Destroy this test input stream.

    // MANIPULATORS
    TestInStream& getLength(int& variable);
        // If required, throw a 'TestInStreamException' (see
        // 'throwExceptionIfInputLimitExhausted'); otherwise, consume the 8-bit
        // unsigned integer type code, verify the type of the next value in
        // this stream, consume that 8-bit unsigned integer or 32-bit signed
        // integer value representing a length (see the 'bslx' package-level
        // documentation) into the specified 'variable' if its type is
        // appropriate, update the cursor location, and return a reference to
        // this stream.  Consume an 8-bit unsigned integer if the most
        // significant bit of this byte is 0, otherwise consume a 32-bit signed
        // integer and set the most significant bit to zero in the resultant
        // 'variable'.  If the type is incorrect, then this stream is marked
        // invalid and the value of 'variable' is unchanged.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid value, this stream is marked
        // invalid and the value of 'variable' is undefined.

    TestInStream& getVersion(int& variable);
        // If required, throw a 'TestInStreamException' (see
        // 'throwExceptionIfInputLimitExhausted'); otherwise, consume the 8-bit
        // unsigned integer type code, verify the type of the next value in
        // this stream, consume that 8-bit unsigned integer value representing
        // a version (see the 'bslx' package-level documentation) into the
        // specified 'variable' if its type is appropriate, update the cursor
        // location, and return a reference to this stream.  If the type is
        // incorrect, then this stream is marked invalid and the value of
        // 'variable' is unchanged.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.

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

    void seek(bsl::size_t offset);
        // Set the index of the next byte to be extracted from this stream to
        // the specified 'offset' from the beginning of the stream, and
        // validate this stream if it is currently invalid.  The behavior is
        // undefined unless 'offset <= length()'.

    void setInputLimit(int limit);
        // Set the number of input operations allowed on this stream to the
        // specified 'limit' before an exception is thrown.  If 'limit' is less
        // than 0, no exception is to be thrown.  By default, no exception is
        // scheduled.

    void setQuiet(bool flagValue);
        // Set the quiet mode for this test stream to the specified (boolean)
        // 'flagValue'.  If 'flagValue' is 'true', then quiet mode is turned ON
        // and no error messages will be written to standard output.  If
        // 'flagValue' is 'false', then quiet mode is turned OFF.  Note that
        // quiet mode is turned OFF by default.

                      // *** scalar integer values ***

    TestInStream& getInt64(bsls::Types::Int64& variable);
        // If required, throw a 'TestInStreamException' (see
        // 'throwExceptionIfInputLimitExhausted'); otherwise, consume the 8-bit
        // unsigned integer type code, verify the type of the next value in
        // this stream, consume that 64-bit signed integer value into the
        // specified 'variable' if its type is appropriate, update the cursor
        // location, and return a reference to this stream.  If the type is
        // incorrect, then this stream is marked invalid and the value of
        // 'variable' is unchanged.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.

    TestInStream& getUint64(bsls::Types::Uint64& variable);
        // If required, throw a 'TestInStreamException' (see
        // 'throwExceptionIfInputLimitExhausted'); otherwise, consume the 8-bit
        // unsigned integer type code, verify the type of the next value in
        // this stream, consume that 64-bit unsigned integer value into the
        // specified 'variable' if its type is appropriate, update the cursor
        // location, and return a reference to this stream.  If the type is
        // incorrect, then this stream is marked invalid and the value of
        // 'variable' is unchanged.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.

    TestInStream& getInt56(bsls::Types::Int64& variable);
        // If required, throw a 'TestInStreamException' (see
        // 'throwExceptionIfInputLimitExhausted'); otherwise, consume the 8-bit
        // unsigned integer type code, verify the type of the next value in
        // this stream, consume that 56-bit signed integer value into the
        // specified 'variable' if its type is appropriate, update the cursor
        // location, and return a reference to this stream.  If the type is
        // incorrect, then this stream is marked invalid and the value of
        // 'variable' is unchanged.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.

    TestInStream& getUint56(bsls::Types::Uint64& variable);
        // If required, throw a 'TestInStreamException' (see
        // 'throwExceptionIfInputLimitExhausted'); otherwise, consume the 8-bit
        // unsigned integer type code, verify the type of the next value in
        // this stream, consume that 56-bit unsigned integer value into the
        // specified 'variable' if its type is appropriate, update the cursor
        // location, and return a reference to this stream.  If the type is
        // incorrect, then this stream is marked invalid and the value of
        // 'variable' is unchanged.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.

    TestInStream& getInt48(bsls::Types::Int64& variable);
        // If required, throw a 'TestInStreamException' (see
        // 'throwExceptionIfInputLimitExhausted'); otherwise, consume the 8-bit
        // unsigned integer type code, verify the type of the next value in
        // this stream, consume that 48-bit signed integer value into the
        // specified 'variable' if its type is appropriate, update the cursor
        // location, and return a reference to this stream.  If the type is
        // incorrect, then this stream is marked invalid and the value of
        // 'variable' is unchanged.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.

    TestInStream& getUint48(bsls::Types::Uint64& variable);
        // If required, throw a 'TestInStreamException' (see
        // 'throwExceptionIfInputLimitExhausted'); otherwise, consume the 8-bit
        // unsigned integer type code, verify the type of the next value in
        // this stream, consume that 48-bit unsigned integer value into the
        // specified 'variable' if its type is appropriate, update the cursor
        // location, and return a reference to this stream.  If the type is
        // incorrect, then this stream is marked invalid and the value of
        // 'variable' is unchanged.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.

    TestInStream& getInt40(bsls::Types::Int64& variable);
        // If required, throw a 'TestInStreamException' (see
        // 'throwExceptionIfInputLimitExhausted'); otherwise, consume the 8-bit
        // unsigned integer type code, verify the type of the next value in
        // this stream, consume that 40-bit signed integer value into the
        // specified 'variable' if its type is appropriate, update the cursor
        // location, and return a reference to this stream.  If the type is
        // incorrect, then this stream is marked invalid and the value of
        // 'variable' is unchanged.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.

    TestInStream& getUint40(bsls::Types::Uint64& variable);
        // If required, throw a 'TestInStreamException' (see
        // 'throwExceptionIfInputLimitExhausted'); otherwise, consume the 8-bit
        // unsigned integer type code, verify the type of the next value in
        // this stream, consume that 40-bit unsigned integer value into the
        // specified 'variable' if its type is appropriate, update the cursor
        // location, and return a reference to this stream.  If the type is
        // incorrect, then this stream is marked invalid and the value of
        // 'variable' is unchanged.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.

    TestInStream& getInt32(int& variable);
        // If required, throw a 'TestInStreamException' (see
        // 'throwExceptionIfInputLimitExhausted'); otherwise, consume the 8-bit
        // unsigned integer type code, verify the type of the next value in
        // this stream, consume that 32-bit signed integer value into the
        // specified 'variable' if its type is appropriate, update the cursor
        // location, and return a reference to this stream.  If the type is
        // incorrect, then this stream is marked invalid and the value of
        // 'variable' is unchanged.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.

    TestInStream& getUint32(unsigned int& variable);
        // If required, throw a 'TestInStreamException' (see
        // 'throwExceptionIfInputLimitExhausted'); otherwise, consume the 8-bit
        // unsigned integer type code, verify the type of the next value in
        // this stream, consume that 32-bit unsigned integer value into the
        // specified 'variable' if its type is appropriate, update the cursor
        // location, and return a reference to this stream.  If the type is
        // incorrect, then this stream is marked invalid and the value of
        // 'variable' is unchanged.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.

    TestInStream& getInt24(int& variable);
        // If required, throw a 'TestInStreamException' (see
        // 'throwExceptionIfInputLimitExhausted'); otherwise, consume the 8-bit
        // unsigned integer type code, verify the type of the next value in
        // this stream, consume that 24-bit signed integer value into the
        // specified 'variable' if its type is appropriate, update the cursor
        // location, and return a reference to this stream.  If the type is
        // incorrect, then this stream is marked invalid and the value of
        // 'variable' is unchanged.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.

    TestInStream& getUint24(unsigned int& variable);
        // If required, throw a 'TestInStreamException' (see
        // 'throwExceptionIfInputLimitExhausted'); otherwise, consume the 8-bit
        // unsigned integer type code, verify the type of the next value in
        // this stream, consume that 24-bit unsigned integer value into the
        // specified 'variable' if its type is appropriate, update the cursor
        // location, and return a reference to this stream.  If the type is
        // incorrect, then this stream is marked invalid and the value of
        // 'variable' is unchanged.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.

    TestInStream& getInt16(short& variable);
        // If required, throw a 'TestInStreamException' (see
        // 'throwExceptionIfInputLimitExhausted'); otherwise, consume the 8-bit
        // unsigned integer type code, verify the type of the next value in
        // this stream, consume that 16-bit signed integer value into the
        // specified 'variable' if its type is appropriate, update the cursor
        // location, and return a reference to this stream.  If the type is
        // incorrect, then this stream is marked invalid and the value of
        // 'variable' is unchanged.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.

    TestInStream& getUint16(unsigned short& variable);
        // If required, throw a 'TestInStreamException' (see
        // 'throwExceptionIfInputLimitExhausted'); otherwise, consume the 8-bit
        // unsigned integer type code, verify the type of the next value in
        // this stream, consume that 16-bit unsigned integer value into the
        // specified 'variable' if its type is appropriate, update the cursor
        // location, and return a reference to this stream.  If the type is
        // incorrect, then this stream is marked invalid and the value of
        // 'variable' is unchanged.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.

    TestInStream& getInt8(char&        variable);
    TestInStream& getInt8(signed char& variable);
        // If required, throw a 'TestInStreamException' (see
        // 'throwExceptionIfInputLimitExhausted'); otherwise, consume the 8-bit
        // unsigned integer type code, verify the type of the next value in
        // this stream, consume that 8-bit signed integer value into the
        // specified 'variable' if its type is appropriate, update the cursor
        // location, and return a reference to this stream.  If the type is
        // incorrect, then this stream is marked invalid and the value of
        // 'variable' is unchanged.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.

    TestInStream& getUint8(char&          variable);
    TestInStream& getUint8(unsigned char& variable);
        // If required, throw a 'TestInStreamException' (see
        // 'throwExceptionIfInputLimitExhausted'); otherwise, consume the 8-bit
        // unsigned integer type code, verify the type of the next value in
        // this stream, consume that 8-bit unsigned integer value into the
        // specified 'variable' if its type is appropriate, update the cursor
        // location, and return a reference to this stream.  If the type is
        // incorrect, then this stream is marked invalid and the value of
        // 'variable' is unchanged.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.

                      // *** scalar floating-point values ***

    TestInStream& getFloat64(double& variable);
        // If required, throw a 'TestInStreamException' (see
        // 'throwExceptionIfInputLimitExhausted'); otherwise, consume the 8-bit
        // unsigned integer type code, verify the type of the next value in
        // this stream, consume that IEEE double-precision (8-byte)
        // floating-point value into the specified 'variable' if its type is
        // appropriate, update the cursor location, and return a reference to
        // this stream.  If the type is incorrect, then this stream is marked
        // invalid and the value of 'variable' is unchanged.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid value, this stream is marked
        // invalid and the value of 'variable' is undefined.  Note that for
        // non-conforming platforms, this operation may be lossy.

    TestInStream& getFloat32(float& variable);
        // If required, throw a 'TestInStreamException' (see
        // 'throwExceptionIfInputLimitExhausted'); otherwise, consume the 8-bit
        // unsigned integer type code, verify the type of the next value in
        // this stream, consume that IEEE single-precision (4-byte)
        // floating-point value into the specified 'variable' if its type is
        // appropriate, update the cursor location, and return a reference to
        // this stream.  If the type is incorrect, then this stream is marked
        // invalid and the value of 'variable' is unchanged.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid value, this stream is marked
        // invalid and the value of 'variable' is undefined.  Note that for
        // non-conforming platforms, this operation may be lossy.

                      // *** string values ***

    TestInStream& getString(bsl::string& variable);
        // If required, throw a 'TestInStreamException' (see
        // 'throwExceptionIfInputLimitExhausted'); otherwise, consume a string
        // from this input stream, assign that value to the specified
        // 'variable', update the cursor location, and return a reference to
        // this stream.  If this stream is initially invalid, this operation
        // has no effect.  If this function otherwise fails to extract a valid
        // value, this stream is marked invalid and the value of 'variable' is
        // undefined.  The string must be prefaced by a non-negative integer
        // indicating the number of characters composing the string.  The
        // behavior is undefined unless the length indicator is non-negative.

                      // *** arrays of integer values ***

    TestInStream& getArrayInt64(bsls::Types::Int64 *variables,
                                int                 numVariables);
        // If required, throw a 'TestInStreamException' (see
        // 'throwExceptionIfInputLimitExhausted'); otherwise, consume the 8-bit
        // unsigned integer type code, verify the type of the next value in
        // this stream, consume that 64-bit signed integer array value into the
        // specified 'variables' of the specified 'numVariables' if its type
        // and length are appropriate, update the cursor location, and return a
        // reference to this stream.  If the type is incorrect, then this
        // stream is marked invalid and the value of 'variables' is unchanged.
        // If this stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value, this
        // stream is marked invalid and the value of 'variables' is undefined.
        // The behavior is undefined unless '0 <= numVariables' and 'variables'
        // has sufficient capacity.

    TestInStream& getArrayUint64(bsls::Types::Uint64 *variables,
                                 int                  numVariables);
        // If required, throw a 'TestInStreamException' (see
        // 'throwExceptionIfInputLimitExhausted'); otherwise, consume the 8-bit
        // unsigned integer type code, verify the type of the next value in
        // this stream, consume that 64-bit unsigned integer array value into
        // the specified 'variables' of the specified 'numVariables' if its
        // type and length are appropriate, update the cursor location, and
        // return a reference to this stream.  If the type is incorrect, then
        // this stream is marked invalid and the value of 'variables' is
        // unchanged.  If this stream is initially invalid, this operation has
        // no effect.  If this function otherwise fails to extract a valid
        // value, this stream is marked invalid and the value of 'variables' is
        // undefined.  The behavior is undefined unless '0 <= numVariables' and
        // 'variables' has sufficient capacity.

    TestInStream& getArrayInt56(bsls::Types::Int64 *variables,
                                int                 numVariables);
        // If required, throw a 'TestInStreamException' (see
        // 'throwExceptionIfInputLimitExhausted'); otherwise, consume the 8-bit
        // unsigned integer type code, verify the type of the next value in
        // this stream, consume that 56-bit signed integer array value into the
        // specified 'variables' of the specified 'numVariables' if its type
        // and length are appropriate, update the cursor location, and return a
        // reference to this stream.  If the type is incorrect, then this
        // stream is marked invalid and the value of 'variables' is unchanged.
        // If this stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value, this
        // stream is marked invalid and the value of 'variables' is undefined.
        // The behavior is undefined unless '0 <= numVariables' and 'variables'
        // has sufficient capacity.

    TestInStream& getArrayUint56(bsls::Types::Uint64 *variables,
                                 int                  numVariables);
        // If required, throw a 'TestInStreamException' (see
        // 'throwExceptionIfInputLimitExhausted'); otherwise, consume the 8-bit
        // unsigned integer type code, verify the type of the next value in
        // this stream, consume that 56-bit unsigned integer array value into
        // the specified 'variables' of the specified 'numVariables' if its
        // type and length are appropriate, update the cursor location, and
        // return a reference to this stream.  If the type is incorrect, then
        // this stream is marked invalid and the value of 'variables' is
        // unchanged.  If this stream is initially invalid, this operation has
        // no effect.  If this function otherwise fails to extract a valid
        // value, this stream is marked invalid and the value of 'variables' is
        // undefined.  The behavior is undefined unless '0 <= numVariables' and
        // 'variables' has sufficient capacity.

    TestInStream& getArrayInt48(bsls::Types::Int64 *variables,
                                int                 numVariables);
        // If required, throw a 'TestInStreamException' (see
        // 'throwExceptionIfInputLimitExhausted'); otherwise, consume the 8-bit
        // unsigned integer type code, verify the type of the next value in
        // this stream, consume that 48-bit signed integer array value into the
        // specified 'variables' of the specified 'numVariables' if its type
        // and length are appropriate, update the cursor location, and return a
        // reference to this stream.  If the type is incorrect, then this
        // stream is marked invalid and the value of 'variables' is unchanged.
        // If this stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value, this
        // stream is marked invalid and the value of 'variables' is undefined.
        // The behavior is undefined unless '0 <= numVariables' and 'variables'
        // has sufficient capacity.

    TestInStream& getArrayUint48(bsls::Types::Uint64 *variables,
                                 int                  numVariables);
        // If required, throw a 'TestInStreamException' (see
        // 'throwExceptionIfInputLimitExhausted'); otherwise, consume the 8-bit
        // unsigned integer type code, verify the type of the next value in
        // this stream, consume that 48-bit unsigned integer array value into
        // the specified 'variables' of the specified 'numVariables' if its
        // type and length are appropriate, update the cursor location, and
        // return a reference to this stream.  If the type is incorrect, then
        // this stream is marked invalid and the value of 'variables' is
        // unchanged.  If this stream is initially invalid, this operation has
        // no effect.  If this function otherwise fails to extract a valid
        // value, this stream is marked invalid and the value of 'variables' is
        // undefined.  The behavior is undefined unless '0 <= numVariables' and
        // 'variables' has sufficient capacity.

    TestInStream& getArrayInt40(bsls::Types::Int64 *variables,
                                int                 numVariables);
        // If required, throw a 'TestInStreamException' (see
        // 'throwExceptionIfInputLimitExhausted'); otherwise, consume the 8-bit
        // unsigned integer type code, verify the type of the next value in
        // this stream, consume that 40-bit signed integer array value into the
        // specified 'variables' of the specified 'numVariables' if its type
        // and length are appropriate, update the cursor location, and return a
        // reference to this stream.  If the type is incorrect, then this
        // stream is marked invalid and the value of 'variables' is unchanged.
        // If this stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value, this
        // stream is marked invalid and the value of 'variables' is undefined.
        // The behavior is undefined unless '0 <= numVariables' and 'variables'
        // has sufficient capacity.

    TestInStream& getArrayUint40(bsls::Types::Uint64 *variables,
                                 int                  numVariables);
        // If required, throw a 'TestInStreamException' (see
        // 'throwExceptionIfInputLimitExhausted'); otherwise, consume the 8-bit
        // unsigned integer type code, verify the type of the next value in
        // this stream, consume that 40-bit unsigned integer array value into
        // the specified 'variables' of the specified 'numVariables' if its
        // type and length are appropriate, update the cursor location, and
        // return a reference to this stream.  If the type is incorrect, then
        // this stream is marked invalid and the value of 'variables' is
        // unchanged.  If this stream is initially invalid, this operation has
        // no effect.  If this function otherwise fails to extract a valid
        // value, this stream is marked invalid and the value of 'variables' is
        // undefined.  The behavior is undefined unless '0 <= numVariables' and
        // 'variables' has sufficient capacity.

    TestInStream& getArrayInt32(int *variables, int numVariables);
        // If required, throw a 'TestInStreamException' (see
        // 'throwExceptionIfInputLimitExhausted'); otherwise, consume the 8-bit
        // unsigned integer type code, verify the type of the next value in
        // this stream, consume that 32-bit signed integer array value into the
        // specified 'variables' of the specified 'numVariables' if its type
        // and length are appropriate, update the cursor location, and return a
        // reference to this stream.  If the type is incorrect, then this
        // stream is marked invalid and the value of 'variables' is unchanged.
        // If this stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value, this
        // stream is marked invalid and the value of 'variables' is undefined.
        // The behavior is undefined unless '0 <= numVariables' and 'variables'
        // has sufficient capacity.

    TestInStream& getArrayUint32(unsigned int *variables, int numVariables);
        // If required, throw a 'TestInStreamException' (see
        // 'throwExceptionIfInputLimitExhausted'); otherwise, consume the 8-bit
        // unsigned integer type code, verify the type of the next value in
        // this stream, consume that 32-bit unsigned integer array value into
        // the specified 'variables' of the specified 'numVariables' if its
        // type and length are appropriate, update the cursor location, and
        // return a reference to this stream.  If the type is incorrect, then
        // this stream is marked invalid and the value of 'variables' is
        // unchanged.  If this stream is initially invalid, this operation has
        // no effect.  If this function otherwise fails to extract a valid
        // value, this stream is marked invalid and the value of 'variables' is
        // undefined.  The behavior is undefined unless '0 <= numVariables' and
        // 'variables' has sufficient capacity.

    TestInStream& getArrayInt24(int *variables, int numVariables);
        // If required, throw a 'TestInStreamException' (see
        // 'throwExceptionIfInputLimitExhausted'); otherwise, consume the 8-bit
        // unsigned integer type code, verify the type of the next value in
        // this stream, consume that 24-bit signed integer array value into the
        // specified 'variables' of the specified 'numVariables' if its type
        // and length are appropriate, update the cursor location, and return a
        // reference to this stream.  If the type is incorrect, then this
        // stream is marked invalid and the value of 'variables' is unchanged.
        // If this stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value, this
        // stream is marked invalid and the value of 'variables' is undefined.
        // The behavior is undefined unless '0 <= numVariables' and 'variables'
        // has sufficient capacity.

    TestInStream& getArrayUint24(unsigned int *variables, int numVariables);
        // If required, throw a 'TestInStreamException' (see
        // 'throwExceptionIfInputLimitExhausted'); otherwise, consume the 8-bit
        // unsigned integer type code, verify the type of the next value in
        // this stream, consume that 24-bit unsigned integer array value into
        // the specified 'variables' of the specified 'numVariables' if its
        // type and length are appropriate, update the cursor location, and
        // return a reference to this stream.  If the type is incorrect, then
        // this stream is marked invalid and the value of 'variables' is
        // unchanged.  If this stream is initially invalid, this operation has
        // no effect.  If this function otherwise fails to extract a valid
        // value, this stream is marked invalid and the value of 'variables' is
        // undefined.  The behavior is undefined unless '0 <= numVariables' and
        // 'variables' has sufficient capacity.

    TestInStream& getArrayInt16(short *variables, int numVariables);
        // If required, throw a 'TestInStreamException' (see
        // 'throwExceptionIfInputLimitExhausted'); otherwise, consume the 8-bit
        // unsigned integer type code, verify the type of the next value in
        // this stream, consume that 16-bit signed integer array value into the
        // specified 'variables' of the specified 'numVariables' if its type
        // and length are appropriate, update the cursor location, and return a
        // reference to this stream.  If the type is incorrect, then this
        // stream is marked invalid and the value of 'variables' is unchanged.
        // If this stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value, this
        // stream is marked invalid and the value of 'variables' is undefined.
        // The behavior is undefined unless '0 <= numVariables' and 'variables'
        // has sufficient capacity.

    TestInStream& getArrayUint16(unsigned short *variables, int numVariables);
        // If required, throw a 'TestInStreamException' (see
        // 'throwExceptionIfInputLimitExhausted'); otherwise, consume the 8-bit
        // unsigned integer type code, verify the type of the next value in
        // this stream, consume that 16-bit unsigned integer array value into
        // the specified 'variables' of the specified 'numVariables' if its
        // type and length are appropriate, update the cursor location, and
        // return a reference to this stream.  If the type is incorrect, then
        // this stream is marked invalid and the value of 'variables' is
        // unchanged.  If this stream is initially invalid, this operation has
        // no effect.  If this function otherwise fails to extract a valid
        // value, this stream is marked invalid and the value of 'variables' is
        // undefined.  The behavior is undefined unless '0 <= numVariables' and
        // 'variables' has sufficient capacity.

    TestInStream& getArrayInt8(char        *variables, int numVariables);
    TestInStream& getArrayInt8(signed char *variables, int numVariables);
        // If required, throw a 'TestInStreamException' (see
        // 'throwExceptionIfInputLimitExhausted'); otherwise, consume the 8-bit
        // unsigned integer type code, verify the type of the next value in
        // this stream, consume that 8-bit signed integer array value into the
        // specified 'variables' of the specified 'numVariables' if its type
        // and length are appropriate, update the cursor location, and return a
        // reference to this stream.  If the type is incorrect, then this
        // stream is marked invalid and the value of 'variables' is unchanged.
        // If this stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value, this
        // stream is marked invalid and the value of 'variables' is undefined.
        // The behavior is undefined unless '0 <= numVariables' and 'variables'
        // has sufficient capacity.

    TestInStream& getArrayUint8(char          *variables, int numVariables);
    TestInStream& getArrayUint8(unsigned char *variables, int numVariables);
        // If required, throw a 'TestInStreamException' (see
        // 'throwExceptionIfInputLimitExhausted'); otherwise, consume the 8-bit
        // unsigned integer type code, verify the type of the next value in
        // this stream, consume that 8-bit unsigned integer array value into
        // the specified 'variables' of the specified 'numVariables' if its
        // type and length are appropriate, update the cursor location, and
        // return a reference to this stream.  If the type is incorrect, then
        // this stream is marked invalid and the value of 'variables' is
        // unchanged.  If this stream is initially invalid, this operation has
        // no effect.  If this function otherwise fails to extract a valid
        // value, this stream is marked invalid and the value of 'variables' is
        // undefined.  The behavior is undefined unless '0 <= numVariables' and
        // 'variables' has sufficient capacity.

                      // *** arrays of floating-point values ***

    TestInStream& getArrayFloat64(double *variables, int numVariables);
        // If required, throw a 'TestInStreamException' (see
        // 'throwExceptionIfInputLimitExhausted'); otherwise, consume the 8-bit
        // unsigned integer type code, verify the type of the next value in
        // this stream, consume that IEEE double-precision (8-byte)
        // floating-point array value into the specified 'variables' of the
        // specified 'numVariables' if its type and length are appropriate,
        // update the cursor location, and return a reference to this stream.
        // If the type is incorrect, then this stream is marked invalid and the
        // value of 'variables' is unchanged.  If this stream is initially
        // invalid, this operation has no effect.  If this function otherwise
        // fails to extract a valid value, this stream is marked invalid and
        // the value of 'variables' is undefined.  The behavior is undefined
        // unless '0 <= numVariables' and 'variables' has sufficient capacity.
        // Note that for non-conforming platforms, this operation may be lossy.

    TestInStream& getArrayFloat32(float *variables, int numVariables);
        // If required, throw a 'TestInStreamException' (see
        // 'throwExceptionIfInputLimitExhausted'); otherwise, consume the 8-bit
        // unsigned integer type code, verify the type of the next value in
        // this stream, consume that IEEE single-precision (4-byte)
        // floating-point array value into the specified 'variables' of the
        // specified 'numVariables' if its type and length are appropriate,
        // update the cursor location, and return a reference to this stream.
        // If the type is incorrect, then this stream is marked invalid and the
        // value of 'variables' is unchanged.  If this stream is initially
        // invalid, this operation has no effect.  If this function otherwise
        // fails to extract a valid value, this stream is marked invalid and
        // the value of 'variables' is undefined.  The behavior is undefined
        // unless '0 <= numVariables' and 'variables' has sufficient capacity.
        // Note that for non-conforming platforms, this operation may be lossy.

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

    int inputLimit() const;
        // Return the current number of input requests left before an exception
        // is thrown.  A negative value indicates that no exception is
        // scheduled.

    bool isEmpty() const;
        // Return 'true' if this stream is empty, and 'false' otherwise.  Note
        // that this function enables higher-level types to verify that, after
        // successfully reading all expected data, no data remains.

    bool isQuiet() const;
        // Return 'true' if this stream's quiet mode is ON, and 'false'
        // otherwise.

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
bsl::ostream& operator<<(bsl::ostream& stream, const TestInStream& object);
    // Write the specified 'object' to the specified output 'stream' in some
    // reasonable (multi-line) format, and return a reference to 'stream'.

template <class TYPE>
TestInStream& operator>>(TestInStream& stream, TYPE& value);
    // Read the specified 'value' from the specified input 'stream' following
    // the requirements of the BDEX protocol (see the 'bslx' package-level
    // documentation), and return a reference to 'stream'.  The behavior is
    // undefined unless 'TYPE' is BDEX-compliant.

}  // close package namespace

               // ============================================
               // macro BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN
               // ============================================

#ifdef BDE_BUILD_TARGET_EXC

namespace bslx {

class TestInStream_ProxyBase {
    // This class provides a common base class for the parameterized
    // 'TestInStream_Proxy' class (below).  Note that the 'virtual'
    // 'setInputLimit' method, although a "setter", *must* be declared 'const'.

  public:
    virtual ~TestInStream_ProxyBase()
    {
    }

    // ACCESSORS
    virtual void setInputLimit(int limit) const = 0;
};

template <class BSLX_STREAM_TYPE>
class TestInStream_Proxy: public TestInStream_ProxyBase {
    // This class provides a proxy to the test stream that is supplied to the
    // 'BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN' macro.  This proxy may be
    // instantiated with 'TestInStream', or with a type that supports the same
    // interface as 'TestInStream'.

    // DATA
    BSLX_STREAM_TYPE *d_stream_p;  // stream used in '*_BEGIN' and
                                   // '*_END' macros (held, not owned)

  public:
    // CREATORS
    TestInStream_Proxy(BSLX_STREAM_TYPE *stream)
    : d_stream_p(stream)
    {
    }

    ~TestInStream_Proxy()
    {
    }

    // ACCESSORS
    virtual void setInputLimit(int limit) const
    {
        d_stream_p->setInputLimit(limit);
    }
};

template <class BSLX_STREAM_TYPE>
inline
TestInStream_Proxy<BSLX_STREAM_TYPE>
TestInStream_getProxy(BSLX_STREAM_TYPE *stream)
    // Return, by value, a test stream proxy for the specified parameterized
    // 'stream'.
{
    return TestInStream_Proxy<BSLX_STREAM_TYPE>(stream);
}

}  // close package namespace

#ifndef BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN
#define BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(BSLX_TESTINSTREAM)             \
{                                                                             \
    const bslx::TestInStream_ProxyBase& testInStream =                        \
                       bslx::TestInStream_getProxy(&BSLX_TESTINSTREAM);       \
    {                                                                         \
        static int firstTime = 1;                                             \
        if (veryVerbose && firstTime) bsl::cout <<                            \
            "### BSLX EXCEPTION TEST -- (ENABLED) --" << '\n';                \
        firstTime = 0;                                                        \
    }                                                                         \
    if (veryVeryVerbose) bsl::cout <<                                         \
        "### Begin BSLX exception test." << '\n';                             \
    int bslxExceptionCounter = 0;                                             \
    static int bslxExceptionLimit = 100;                                      \
    testInStream.setInputLimit(bslxExceptionCounter);                         \
    do {                                                                      \
        try {
#endif  // BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN

#else // !defined(BDE_BUILD_TARGET_EXC)

#ifndef BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN
#define BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(testInStream)                  \
{                                                                             \
    static int firstTime = 1;                                                 \
    if (verbose && firstTime) {                                               \
        bsl::cout << "### BSLX EXCEPTION TEST -- (NOT ENABLED) --" << '\n';   \
        firstTime = 0;                                                        \
    }                                                                         \
}
#endif  // BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN

#endif  // BDE_BUILD_TARGET_EXC

               // ==========================================
               // macro BSLX_TESTINSTREAM_EXCEPTION_TEST_END
               // ==========================================

#ifdef BDE_BUILD_TARGET_EXC

#ifndef BSLX_TESTINSTREAM_EXCEPTION_TEST_END
#define BSLX_TESTINSTREAM_EXCEPTION_TEST_END                                  \
        } catch (bslx::TestInStreamException& e) {                            \
            if ((veryVerbose && bslxExceptionLimit) || veryVeryVerbose)       \
            {                                                                 \
                --bslxExceptionLimit;                                         \
                bsl::cout << "(" << bslxExceptionCounter << ')';              \
                if (veryVeryVerbose) {                                        \
                    bsl::cout << " BSLX EXCEPTION: "                          \
                              << "input limit = "                             \
                              << bslxExceptionCounter                         \
                              << ", "                                         \
                              << "last data type = "                          \
                              << e.dataType();                                \
                }                                                             \
                else if (0 == bslxExceptionLimit) {                           \
                    bsl::cout << " [ Note: 'bslxExceptionLimit' reached. ]";  \
                }                                                             \
                bsl::cout << '\n';                                            \
            }                                                                 \
            testInStream.setInputLimit(++bslxExceptionCounter);               \
            continue;                                                         \
        }                                                                     \
        testInStream.setInputLimit(-1);                                       \
        break;                                                                \
    } while (1);                                                              \
    if (veryVeryVerbose) {                                                    \
        bsl::cout << "### End BSLX exception test." << '\n';                  \
    }                                                                         \
}
#endif  // BSLX_TESTINSTREAM_EXCEPTION_TEST_END

#else // !defined(BDE_BUILD_TARGET_EXC)

#ifndef BSLX_TESTINSTREAM_EXCEPTION_TEST_END
#define BSLX_TESTINSTREAM_EXCEPTION_TEST_END
#endif

#endif  // BDE_BUILD_TARGET_EXC

namespace bslx {

// ============================================================================
//                          INLINE DEFINITIONS
// ============================================================================

                         // ------------------
                         // class TestInStream
                         // ------------------

// PRIVATE MANIPULATORS
inline
void TestInStream::throwExceptionIfInputLimitExhausted(
                                                    const TypeCode::Enum& code)
{
#ifdef BDE_BUILD_TARGET_EXC
    if (0 <= d_inputLimit) {
        --d_inputLimit;
        if (0 > d_inputLimit) {
            throw TestInStreamException(code);
        }
    }
#endif
}

// MANIPULATORS
inline
void TestInStream::invalidate()
{
    d_validFlag = false;
}

inline
void TestInStream::reset()
{
    d_validFlag = true;
    d_cursor    = 0;
}

inline
void TestInStream::reset(const char *buffer, bsl::size_t numBytes)
{
    BSLS_ASSERT_SAFE(buffer || 0 == numBytes);

    d_buffer    = buffer;
    d_numBytes  = numBytes;
    d_validFlag = true;
    d_cursor    = 0;
}

inline
void TestInStream::reset(const bslstl::StringRef& srcData)
{
    d_buffer    = srcData.data();
    d_numBytes  = srcData.length();
    d_validFlag = true;
    d_cursor    = 0;
}

inline
void TestInStream::seek(bsl::size_t offset)
{
    BSLS_ASSERT_SAFE(offset <= length());

    d_cursor    = offset;
    d_validFlag = 1;
}

inline
void TestInStream::setInputLimit(int limit)
{
    d_inputLimit = limit;
}

inline
void TestInStream::setQuiet(bool flagValue)
{
    d_quietFlag = flagValue;
}

// ACCESSORS
inline
TestInStream::operator const void *() const
{
    return isValid() ? this : 0;
}

inline
bsl::size_t TestInStream::cursor() const
{
    return d_cursor;
}

inline
const char *TestInStream::data() const
{
    return d_numBytes ? d_buffer : 0;
}

inline
int TestInStream::inputLimit() const
{
    return d_inputLimit;
}

inline
bool TestInStream::isEmpty() const
{
    return cursor() >= length();
}

inline
bool TestInStream::isQuiet() const
{
    return d_quietFlag;
}

inline
bool TestInStream::isValid() const
{
    return d_validFlag;
}

inline
bsl::size_t TestInStream::length() const
{
    return d_numBytes;
}

template <class TYPE>
inline
TestInStream& operator>>(TestInStream& stream, TYPE& value)
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
