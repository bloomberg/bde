// bslx_genericinstream.h                                             -*-C++-*-
#ifndef INCLUDED_BSLX_GENERICINSTREAM
#define INCLUDED_BSLX_GENERICINSTREAM

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Unexternalization of fundamental types from a parameterized stream.
//
//@CLASSES:
//  bslx::GenericInStream: parameterized input stream for fundamental types
//
//@SEE_ALSO: bslx_streambufinstream, bslx_genericoutstream
//
//@DESCRIPTION: This component implements a parameterized input stream
// class, 'bslx::GenericInStream', that provides platform-independent input
// methods ("unexternalization") on values, and arrays of values, of
// fundamental types, and on 'bsl::string'.
//
// The 'bslx::GenericInStream' type reads from a compliant user-supplied buffer
// (see {Generic Byte-Format Parser}) directly, with no data copying or
// assumption of ownership.  The user must therefore make sure that the
// lifetime and visibility of the buffer is sufficient to satisfy the needs of
// the input stream.
//
// This component is intended to be used in conjunction with the
// 'bslx_genericoutstream' "externalization" component.  Each input method of
// 'bslx::GenericInStream' reads either a value or a homogeneous array of
// values of a fundamental type, in a format that was written by the
// corresponding 'bslx::GenericOutStream' method.  In general, the user of
// this component cannot rely on being able to read data that was written by
// any mechanism other than 'bslx::GenericOutStream'.
//
// The supported types and required content are listed in the 'bslx'
// package-level documentation under "Supported Types".
//
// Note that input streams can be *invalidated* explicitly and queried for
// *validity*.  Reading from an initially invalid stream has no effect.
// Attempting to read beyond the end of a stream will automatically invalidate
// the stream.  Whenever an inconsistent value is detected, the stream should
// be invalidated explicitly.
//
///Generic Byte-Format Parser
///--------------------------
// The class 'bslx::GenericInStream' is parameterized by a buffered stream
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
//  assert(eof != sb->sbumpc());
//  assert(eof != sb->sgetc());
//  assert(len == sb->sgetn(s, len));
//..
// Suitable choices for 'STREAMBUF' include any class that implements the
// 'bsl::basic_streambuf' protocol.
//
// The class 'bslx::StreambufInStream' is a 'typedef' for
// 'bslx::GenericInStream<bsl::streambuf>'.
//
///Usage
///-----
// This section illustrates intended use of this component.  The first example
// depicts usage with a 'bsl::stringbuf'.  The second example replaces the
// 'bsl::stringbuf' with a user-defined 'STREAMBUF'.
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
// 'janeSmith1' and a 'bslx::GenericOutStream' 'outStream1':
//..
//  MyPerson                               janeSmith1("Jane", "Smith", 42);
//  bsl::stringbuf                         buffer1;
//  bslx::GenericOutStream<bsl::stringbuf> outStream1(&buffer1, 20131127);
//  const int                              VERSION1 = 1;
//  outStream1.putVersion(VERSION1);
//  janeSmith1.bdexStreamOut(outStream1, VERSION1);
//  assert(outStream1.isValid());
//..
// Next, create a 'MyPerson' 'janeCopy1' initialized to the default value, and
// assert that 'janeCopy1' is different from 'janeSmith1':
//..
//  MyPerson janeCopy1;
//  assert(janeCopy1 != janeSmith1);
//..
// Then, create a 'bslx::GenericInStream' 'inStream1' initialized with the
// buffer from the 'bslx::GenericOutStream' object 'outStream1' and
// unexternalize this data into 'janeCopy1':
//..
//  bslx::GenericInStream<bsl::stringbuf> inStream1(&buffer1);
//  int                                   version1;
//  inStream1.getVersion(version1);
//  janeCopy1.bdexStreamIn(inStream1, version1);
//  assert(inStream1.isValid());
//..
// Finally, 'assert' the obtained values are as expected and display the
// results to 'bsl::stdout':
//..
//  assert(version1  == VERSION1);
//  assert(janeCopy1 == janeSmith1);
//
//  if (janeCopy1 == janeSmith1) {
//      bsl::cout << "Successfully serialized and de-serialized Jane Smith:"
//                << "\n\tFirstName: " << janeCopy1.firstName()
//                << "\n\tLastName : " << janeCopy1.lastName()
//                << "\n\tAge      : " << janeCopy1.age() << bsl::endl;
//  }
//  else {
//      bsl::cout << "Serialization unsuccessful.  'janeCopy1' holds:"
//                << "\n\tFirstName: " << janeCopy1.firstName()
//                << "\n\tLastName : " << janeCopy1.lastName()
//                << "\n\tAge      : " << janeCopy1.age() << bsl::endl;
//  }
//..
//
///Example 2: Sample 'STREAMBUF' Implementation
/// - - - - - - - - - - - - - - - - - - - - - -
// For this example, we will implement 'MyStreamBuf', a minimal 'STREAMBUF' to
// to be used with 'bslx::GenericInStream' and 'bslx::GenericOutStream'.  The
// implementation will consist of only what is required of the type.  For
// comparison, we will reuse 'MyPerson' and repeat part of {Example 1} to
// demonstrate how to use 'bslx::GenericInStream'.
//
// First, we implement 'MyStreamBuf' (which, for brevity, simply uses the
// default allocator):
//..
//  class MyStreamBuf {
//      // This class implements a very basic stream buffer suitable for use in
//      // 'bslx::GenericOutStream' and 'bslx::GenericInStream'.
//
//      // DATA
//      bsl::deque<char> d_buffer;  // the input and output buffer
//
//    private:
//      // NOT IMPLEMENTED
//      MyStreamBuf(const MyStreamBuf&);
//      MyStreamBuf& operator=(const MyStreamBuf&);
//
//    public:
//      // TYPES
//      struct traits_type {
//          static int eof() { return -1; }
//      };
//
//      // CREATORS
//      MyStreamBuf();
//          // Create an empty stream buffer.
//
//      ~MyStreamBuf();
//          // Destroy this stream buffer.
//
//      // MANIPULATORS
//      int pubsync();
//          // Return 0.
//
//      int sbumpc();
//          // Read the next character in this buffer.  Return the value of the
//          // character on success, and 'traits_type::eof()' otherwise.
//
//      int sgetc();
//          // Peek at the next character in this buffer.  Return the value of
//          // the character on success, and 'traits_type::eof()' otherwise.
//
//      bsl::streamsize sgetn(char *s, bsl::streamsize length);
//          // Load the specified 'length' characters into the specified
//          // address 's', and return the number of characters read.
//
//      int sputc(char c);
//          // Write the specified character 'c' to this buffer.  Return 'c' on
//          // success, and 'traits_type::eof()' otherwise.
//
//      bsl::streamsize sputn(const char *s, bsl::streamsize length);
//          // Write the specified 'length' characters at the specified address
//          // 's' to this buffer, and return the number of characters written.
//  };
//
//  // ========================================================================
//  //                  INLINE FUNCTION DEFINITIONS
//  // ========================================================================
//
//  // CREATORS
//  MyStreamBuf::MyStreamBuf()
//  : d_buffer()
//  {
//  }
//
//  MyStreamBuf::~MyStreamBuf()
//  {
//  }
//
//  // MANIPULATORS
//  int MyStreamBuf::pubsync()
//  {
//      // In this implementation, there is nothing to be done except return
//      // success.
//
//      return 0;
//  }
//
//  int MyStreamBuf::sbumpc()
//  {
//      if (!d_buffer.empty()) {
//          const int rv = static_cast<int>(d_buffer.front());
//          d_buffer.pop_front();
//          return rv;                                                // RETURN
//      }
//      return traits_type::eof();
//  }
//
//  int MyStreamBuf::sgetc()
//  {
//      if (!d_buffer.empty()) {
//          return static_cast<int>(d_buffer.front());                // RETURN
//      }
//      return traits_type::eof();
//  }
//
//  bsl::streamsize MyStreamBuf::sgetn(char *s, bsl::streamsize length)
//  {
//      for (bsl::streamsize i = 0; i < length; ++i) {
//          if (d_buffer.empty()) {
//              return i;                                             // RETURN
//          }
//          s[i] = d_buffer.front();
//          d_buffer.pop_front();
//      }
//      return length;
//  }
//
//  int MyStreamBuf::sputc(char c)
//  {
//      d_buffer.push_back(c);
//      return static_cast<int>(c);
//  }
//
//  bsl::streamsize MyStreamBuf::sputn(const char      *s,
//                                     bsl::streamsize  length)
//  {
//      for (bsl::streamsize i = 0; i < length; ++i) {
//          d_buffer.push_back(s[i]);
//      }
//      return length;
//  }
//..
// Then, we create a 'MyPerson' 'janeSmith2' and a 'bslx::GenericOutStream'
// 'outStream2':
//..
//  MyPerson                               janeSmith2("Jane", "Smith", 42);
//  MyStreamBuf                            buffer2;
//  bslx::GenericOutStream<MyStreamBuf>    outStream2(&buffer2, 20131127);
//  const int                              VERSION2 = 1;
//  outStream2.putVersion(VERSION2);
//  janeSmith2.bdexStreamOut(outStream2, VERSION2);
//  assert(outStream2.isValid());
//..
// Next, create a 'MyPerson' 'janeCopy2' initialized to the default value, and
// assert that 'janeCopy2' is different from 'janeSmith2':
//..
//  MyPerson janeCopy2;
//  assert(janeCopy2 != janeSmith2);
//..
// Then, create a 'bslx::GenericInStream' 'inStream2' initialized with the
// buffer from the 'bslx::GenericOutStream' object 'outStream2' and
// unexternalize this data into 'janeCopy2':
//..
//  bslx::GenericInStream<MyStreamBuf>    inStream2(&buffer2);
//  int                                   version2;
//  inStream2.getVersion(version2);
//  janeCopy2.bdexStreamIn(inStream2, version2);
//  assert(inStream2.isValid());
//..
// Finally, 'assert' the obtained values are as expected:
//..
//  assert(version2  == VERSION2);
//  assert(janeCopy2 == janeSmith2);
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLX_INSTREAMFUNCTIONS
#include <bslx_instreamfunctions.h>
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

                        // =====================
                        // class GenericInStream
                        // =====================

template <class STREAMBUF>
class GenericInStream {
    // This class provides input methods to unexternalize values, and C-style
    // arrays of values, of the fundamental integral and floating-point types,
    // as well as 'bsl::string' values, using a byte format documented in the
    // 'bslx_byteoutstream' component.  In particular, each 'get' method of
    // this class is guaranteed to read stream data written by the
    // corresponding 'put' method of 'bslx::GenericOutStream'.  Note that
    // attempting to read beyond the end of a stream will automatically
    // invalidate the stream.  See the 'bslx' package-level documentation for
    // the definition of the BDEX 'InStream' protocol.

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
    STREAMBUF *d_streamBuf;  // held stream to read from

    bool       d_validFlag;  // stream validity flag; 'true' if stream is in
                             // valid state, 'false' otherwise

    // NOT IMPLEMENTED
    GenericInStream(const GenericInStream&);
    GenericInStream& operator=(const GenericInStream&);

  private:
    // PRIVATE MANIPULATORS
    void validate();
        // Put this output stream into a valid state.  This function has no
        // effect if this stream is already valid.

  public:
    // CREATORS
    GenericInStream(STREAMBUF *streamBuf);
        // Create an input byte stream that reads its input from the specified
        // 'streamBuf'.

    ~GenericInStream();
        // Destroy this object.

    // MANIPULATORS
    GenericInStream& getLength(int& length);
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

    GenericInStream& getVersion(int& version);
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

                      // *** scalar integer values ***

    GenericInStream& getInt64(bsls::Types::Int64& variable);
        // Assign to the specified 'variable' the eight-byte, two's complement
        // integer (in host byte order) comprised of the eight bytes of this
        // stream at the current cursor location (in network byte order),
        // update the cursor location, and return a reference to this stream.
        // If this stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value, this
        // stream is marked invalid and the value of 'variable' is undefined.
        // Note that the value will be sign-extended.

    GenericInStream& getUint64(bsls::Types::Uint64& variable);
        // Assign to the specified 'variable' the eight-byte, two's complement
        // unsigned integer (in host byte order) comprised of the eight bytes
        // of this stream at the current cursor location (in network byte
        // order), update the cursor location, and return a reference to this
        // stream.  If this stream is initially invalid, this operation has no
        // effect.  If this function otherwise fails to extract a valid value,
        // this stream is marked invalid and the value of 'variable' is
        // undefined.  Note that the value will be zero-extended.

    GenericInStream& getInt56(bsls::Types::Int64& variable);
        // Assign to the specified 'variable' the seven-byte, two's complement
        // integer (in host byte order) comprised of the seven bytes of this
        // stream at the current cursor location (in network byte order),
        // update the cursor location, and return a reference to this stream.
        // If this stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value, this
        // stream is marked invalid and the value of 'variable' is undefined.
        // Note that the value will be sign-extended.

    GenericInStream& getUint56(bsls::Types::Uint64& variable);
        // Assign to the specified 'variable' the seven-byte, two's complement
        // unsigned integer (in host byte order) comprised of the seven bytes
        // of this stream at the current cursor location (in network byte
        // order), update the cursor location, and return a reference to this
        // stream.  If this stream is initially invalid, this operation has no
        // effect.  If this function otherwise fails to extract a valid value,
        // this stream is marked invalid and the value of 'variable' is
        // undefined.  Note that the value will be zero-extended.

    GenericInStream& getInt48(bsls::Types::Int64& variable);
        // Assign to the specified 'variable' the six-byte, two's complement
        // integer (in host byte order) comprised of the six bytes of this
        // stream at the current cursor location (in network byte order),
        // update the cursor location, and return a reference to this stream.
        // If this stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value, this
        // stream is marked invalid and the value of 'variable' is undefined.
        // Note that the value will be sign-extended.

    GenericInStream& getUint48(bsls::Types::Uint64& variable);
        // Assign to the specified 'variable' the six-byte, two's complement
        // unsigned integer (in host byte order) comprised of the six bytes of
        // this stream at the current cursor location (in network byte order),
        // update the cursor location, and return a reference to this stream.
        // If this stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value, this
        // stream is marked invalid and the value of 'variable' is undefined.
        // Note that the value will be zero-extended.

    GenericInStream& getInt40(bsls::Types::Int64& variable);
        // Assign to the specified 'variable' the five-byte, two's complement
        // integer (in host byte order) comprised of the five bytes of this
        // stream at the current cursor location (in network byte order),
        // update the cursor location, and return a reference to this stream.
        // If this stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value, this
        // stream is marked invalid and the value of 'variable' is undefined.
        // Note that the value will be sign-extended.

    GenericInStream& getUint40(bsls::Types::Uint64& variable);
        // Assign to the specified 'variable' the five-byte, two's complement
        // unsigned integer (in host byte order) comprised of the five bytes of
        // this stream at the current cursor location (in network byte order),
        // update the cursor location, and return a reference to this stream.
        // If this stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value, this
        // stream is marked invalid and the value of 'variable' is undefined.
        // Note that the value will be zero-extended.

    GenericInStream& getInt32(int& variable);
        // Assign to the specified 'variable' the four-byte, two's complement
        // integer (in host byte order) comprised of the four bytes of this
        // stream at the current cursor location (in network byte order),
        // update the cursor location, and return a reference to this stream.
        // If this stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value, this
        // stream is marked invalid and the value of 'variable' is undefined.
        // Note that the value will be sign-extended.

    GenericInStream& getUint32(unsigned int& variable);
        // Assign to the specified 'variable' the four-byte, two's complement
        // unsigned integer (in host byte order) comprised of the four bytes of
        // this stream at the current cursor location (in network byte order),
        // update the cursor location, and return a reference to this stream.
        // If this stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value, this
        // stream is marked invalid and the value of 'variable' is undefined.
        // Note that the value will be zero-extended.

    GenericInStream& getInt24(int& variable);
        // Assign to the specified 'variable' the three-byte, two's complement
        // integer (in host byte order) comprised of the three bytes of this
        // stream at the current cursor location (in network byte order),
        // update the cursor location, and return a reference to this stream.
        // If this stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value, this
        // stream is marked invalid and the value of 'variable' is undefined.
        // Note that the value will be sign-extended.

    GenericInStream& getUint24(unsigned int& variable);
        // Assign to the specified 'variable' the three-byte, two's complement
        // unsigned integer (in host byte order) comprised of the three bytes
        // of this stream at the current cursor location (in network byte
        // order), update the cursor location, and return a reference to this
        // stream.  If this stream is initially invalid, this operation has no
        // effect.  If this function otherwise fails to extract a valid value,
        // this stream is marked invalid and the value of 'variable' is
        // undefined.  Note that the value will be zero-extended.

    GenericInStream& getInt16(short& variable);
        // Assign to the specified 'variable' the two-byte, two's complement
        // integer (in host byte order) comprised of the two bytes of this
        // stream at the current cursor location (in network byte order),
        // update the cursor location, and return a reference to this stream.
        // If this stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value, this
        // stream is marked invalid and the value of 'variable' is undefined.
        // Note that the value will be sign-extended.

    GenericInStream& getUint16(unsigned short& variable);
        // Assign to the specified 'variable' the two-byte, two's complement
        // unsigned integer (in host byte order) comprised of the two bytes of
        // this stream at the current cursor location (in network byte order),
        // update the cursor location, and return a reference to this stream.
        // If this stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value, this
        // stream is marked invalid and the value of 'variable' is undefined.
        // Note that the value will be zero-extended.

    GenericInStream& getInt8(char&        variable);
    GenericInStream& getInt8(signed char& variable);
        // Assign to the specified 'variable' the one-byte, two's complement
        // integer comprised of the one byte of this stream at the current
        // cursor location, update the cursor location, and return a reference
        // to this stream.  If this stream is initially invalid, this operation
        // has no effect.  If this function otherwise fails to extract a valid
        // value, this stream is marked invalid and the value of 'variable' is
        // undefined.  Note that the value will be sign-extended.

    GenericInStream& getUint8(char&          variable);
    GenericInStream& getUint8(unsigned char& variable);
        // Assign to the specified 'variable' the one-byte, two's complement
        // unsigned integer comprised of the one byte of this stream at the
        // current cursor location, update the cursor location, and return a
        // reference to this stream.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.  Note that the value will be
        // zero-extended.

                      // *** scalar floating-point values ***

    GenericInStream& getFloat64(double& variable);
        // Assign to the specified 'variable' the eight-byte IEEE
        // double-precision floating-point number (in host byte order)
        // comprised of the eight bytes of this stream at the current cursor
        // location (in network byte order), update the cursor location, and
        // return a reference to this stream.  If this stream is initially
        // invalid, this operation has no effect.  If this function otherwise
        // fails to extract a valid value, this stream is marked invalid and
        // the value of 'variable' is undefined.

    GenericInStream& getFloat32(float& variable);
        // Assign to the specified 'variable' the four-byte IEEE
        // single-precision floating-point number (in host byte order)
        // comprised of the four bytes of this stream at the current cursor
        // location (in network byte order), update the cursor location, and
        // return a reference to this stream.  If this stream is initially
        // invalid, this operation has no effect.  If this function otherwise
        // fails to extract a valid value, this stream is marked invalid and
        // the value of 'variable' is undefined.

                      // *** string values ***

    GenericInStream& getString(bsl::string& variable);
        // Assign to the specified 'variable' the string comprised of the
        // length of the string (see 'getLength') and the string data (see
        // 'getUint8'), update the cursor location, and return a reference to
        // this stream.  If this stream is initially invalid, this operation
        // has no effect.  If this function otherwise fails to extract a valid
        // value, this stream is marked invalid and the value of 'variable' is
        // undefined.

                      // *** arrays of integer values ***

    GenericInStream& getArrayInt64(bsls::Types::Int64 *variables,
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

    GenericInStream& getArrayUint64(bsls::Types::Uint64 *variables,
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

    GenericInStream& getArrayInt56(bsls::Types::Int64 *variables,
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

    GenericInStream& getArrayUint56(bsls::Types::Uint64 *variables,
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

    GenericInStream& getArrayInt48(bsls::Types::Int64 *variables,
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

    GenericInStream& getArrayUint48(bsls::Types::Uint64 *variables,
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

    GenericInStream& getArrayInt40(bsls::Types::Int64 *variables,
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

    GenericInStream& getArrayUint40(bsls::Types::Uint64 *variables,
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

    GenericInStream& getArrayInt32(int *variables, int numVariables);
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

    GenericInStream& getArrayUint32(unsigned int *variables, int numVariables);
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

    GenericInStream& getArrayInt24(int *variables, int numVariables);
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

    GenericInStream& getArrayUint24(unsigned int *variables, int numVariables);
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

    GenericInStream& getArrayInt16(short *variables, int numVariables);
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

    GenericInStream& getArrayUint16(unsigned short *variables,
                                    int             numVariables);
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

    GenericInStream& getArrayInt8(char *variables, int numVariables);
    GenericInStream& getArrayInt8(signed char *variables, int numVariables);
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

    GenericInStream& getArrayUint8(char *variables, int numVariables);
    GenericInStream& getArrayUint8(unsigned char *variables, int numVariables);
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

    GenericInStream& getArrayFloat64(double *variables, int numVariables);
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

    GenericInStream& getArrayFloat32(float *variables, int numVariables);
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
        // An invalid stream is a stream in which insufficient or invalid data
        // was detected during an extraction operation.  Note that an empty
        // stream will be valid unless an extraction attempt or explicit
        // invalidation causes it to be otherwise.

    bool isValid() const;
        // Return 'true' if this stream is valid, and 'false' otherwise.  An
        // invalid stream is a stream in which insufficient or invalid data was
        // detected during an extraction operation.  Note that an empty stream
        // will be valid unless an extraction attempt or explicit invalidation
        // causes it to be otherwise.
};

// FREE OPERATORS
template <class STREAMBUF, class TYPE>
GenericInStream<STREAMBUF>&
                   operator>>(GenericInStream<STREAMBUF>& stream, TYPE& value);
    // Read the specified 'value' from the specified input 'stream' following
    // the requirements of the BDEX protocol (see the 'bslx' package-level
    // documentation), and return a reference to 'stream'.  The behavior is
    // undefined unless 'TYPE' is BDEX-compliant.

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                        // ---------------------
                        // class GenericInStream
                        // ---------------------

// PRIVATE MANIPULATORS
template <class STREAMBUF>
inline
void GenericInStream<STREAMBUF>::validate()
{
    d_validFlag = true;
}

// CREATORS
template <class STREAMBUF>
inline
GenericInStream<STREAMBUF>::GenericInStream(STREAMBUF *streamBuf)
: d_streamBuf(streamBuf)
, d_validFlag(true)
{
    BSLS_ASSERT_SAFE(streamBuf);
}

template <class STREAMBUF>
inline
GenericInStream<STREAMBUF>::~GenericInStream()
{
}

// MANIPULATORS
template <class STREAMBUF>
inline
GenericInStream<STREAMBUF>& GenericInStream<STREAMBUF>::getLength(int& length)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    invalidate();

    const int current = d_streamBuf->sgetc();
    if (STREAMBUF::traits_type::eof() != current) {
        validate();
        if (127 < current) {
            // If 'length > 127', 'length' is stored as 4 bytes with top bit
            // set.

            getInt32(length);
            length &= 0x7fffffff;  // Clear top bit.
        }
        else {
            // If 'length <= 127', 'length' is stored as one byte.

            char tmp;
            getInt8(tmp);
            length = tmp;
        }
    }

    return *this;
}

template <class STREAMBUF>
inline
GenericInStream<STREAMBUF>&
GenericInStream<STREAMBUF>::getVersion(int& version)
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

template <class STREAMBUF>
inline
void GenericInStream<STREAMBUF>::invalidate()
{
    d_validFlag = false;
}

                      // *** scalar integer values ***

template <class STREAMBUF>
inline
GenericInStream<STREAMBUF>&
GenericInStream<STREAMBUF>::getInt64(bsls::Types::Int64& variable)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    invalidate();

    if (sizeof variable > k_SIZEOF_INT64) {
        const int current = d_streamBuf->sgetc();
        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                   STREAMBUF::traits_type::eof() == current)) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
            return *this;                                             // RETURN
        }
        variable = 0x80 & current ? -1 : 0;  // sign extend
    }

#ifdef BSLS_PLATFORM_IS_LITTLE_ENDIAN
    char *bytes = reinterpret_cast<char *>(&variable);
    char  rawBytes[k_SIZEOF_INT64];
    if (k_SIZEOF_INT64 == d_streamBuf->sgetn(rawBytes, k_SIZEOF_INT64)) {
        validate();
        bytes[7] = rawBytes[0];
        bytes[6] = rawBytes[1];
        bytes[5] = rawBytes[2];
        bytes[4] = rawBytes[3];
        bytes[3] = rawBytes[4];
        bytes[2] = rawBytes[5];
        bytes[1] = rawBytes[6];
        bytes[0] = rawBytes[7];
    }
#else
    char *bytes =
        reinterpret_cast<char *>(&variable) + sizeof variable - k_SIZEOF_INT64;
    if (k_SIZEOF_INT64 == d_streamBuf->sgetn(bytes, k_SIZEOF_INT64)) {
        validate();
    }
#endif

    return *this;
}

template <class STREAMBUF>
inline
GenericInStream<STREAMBUF>&
GenericInStream<STREAMBUF>::getUint64(bsls::Types::Uint64& variable)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    invalidate();

    if (sizeof variable > k_SIZEOF_INT64) {
        variable = 0;  // zero-extend
    }

#ifdef BSLS_PLATFORM_IS_LITTLE_ENDIAN
    char *bytes = reinterpret_cast<char *>(&variable);
    char  rawBytes[k_SIZEOF_INT64];
    if (k_SIZEOF_INT64 == d_streamBuf->sgetn(rawBytes, k_SIZEOF_INT64)) {
        validate();
        bytes[7] = rawBytes[0];
        bytes[6] = rawBytes[1];
        bytes[5] = rawBytes[2];
        bytes[4] = rawBytes[3];
        bytes[3] = rawBytes[4];
        bytes[2] = rawBytes[5];
        bytes[1] = rawBytes[6];
        bytes[0] = rawBytes[7];
    }
#else
    char *bytes =
        reinterpret_cast<char *>(&variable) + sizeof variable - k_SIZEOF_INT64;
    if (k_SIZEOF_INT64 == d_streamBuf->sgetn(bytes, k_SIZEOF_INT64)) {
        validate();
    }
#endif

    return *this;
}

template <class STREAMBUF>
inline
GenericInStream<STREAMBUF>&
GenericInStream<STREAMBUF>::getInt56(bsls::Types::Int64& variable)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    invalidate();

    const int current = d_streamBuf->sgetc();
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                   STREAMBUF::traits_type::eof() == current)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }
    variable = 0x80 & current ? -1 : 0;  // sign extend

#ifdef BSLS_PLATFORM_IS_LITTLE_ENDIAN
    char *bytes = reinterpret_cast<char *>(&variable);
    char  rawBytes[k_SIZEOF_INT56];
    if (k_SIZEOF_INT56 == d_streamBuf->sgetn(rawBytes, k_SIZEOF_INT56)) {
        validate();
        bytes[6] = rawBytes[0];
        bytes[5] = rawBytes[1];
        bytes[4] = rawBytes[2];
        bytes[3] = rawBytes[3];
        bytes[2] = rawBytes[4];
        bytes[1] = rawBytes[5];
        bytes[0] = rawBytes[6];
    }
#else
    char *bytes =
        reinterpret_cast<char *>(&variable) + sizeof variable - k_SIZEOF_INT56;
    if (k_SIZEOF_INT56 == d_streamBuf->sgetn(bytes, k_SIZEOF_INT56)) {
        validate();
    }
#endif

    return *this;
}

template <class STREAMBUF>
inline
GenericInStream<STREAMBUF>&
GenericInStream<STREAMBUF>::getUint56(bsls::Types::Uint64& variable)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    invalidate();

    variable = 0;  // zero-extend

#ifdef BSLS_PLATFORM_IS_LITTLE_ENDIAN
    char *bytes = reinterpret_cast<char *>(&variable);
    char  rawBytes[k_SIZEOF_INT56];
    if (k_SIZEOF_INT56 == d_streamBuf->sgetn(rawBytes, k_SIZEOF_INT56)) {
        validate();
        bytes[6] = rawBytes[0];
        bytes[5] = rawBytes[1];
        bytes[4] = rawBytes[2];
        bytes[3] = rawBytes[3];
        bytes[2] = rawBytes[4];
        bytes[1] = rawBytes[5];
        bytes[0] = rawBytes[6];
    }
#else
    char *bytes =
        reinterpret_cast<char *>(&variable) + sizeof variable - k_SIZEOF_INT56;
    if (k_SIZEOF_INT56 == d_streamBuf->sgetn(bytes, k_SIZEOF_INT56)) {
        validate();
    }
#endif

    return *this;
}

template <class STREAMBUF>
inline
GenericInStream<STREAMBUF>&
GenericInStream<STREAMBUF>::getInt48(bsls::Types::Int64& variable)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    invalidate();

    const int current = d_streamBuf->sgetc();
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                   STREAMBUF::traits_type::eof() == current)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }
    variable = 0x80 & current ? -1 : 0;  // sign extend

#ifdef BSLS_PLATFORM_IS_LITTLE_ENDIAN
    char *bytes = reinterpret_cast<char *>(&variable);
    char  rawBytes[k_SIZEOF_INT48];
    if (k_SIZEOF_INT48 == d_streamBuf->sgetn(rawBytes, k_SIZEOF_INT48)) {
        validate();
        bytes[5] = rawBytes[0];
        bytes[4] = rawBytes[1];
        bytes[3] = rawBytes[2];
        bytes[2] = rawBytes[3];
        bytes[1] = rawBytes[4];
        bytes[0] = rawBytes[5];
    }
#else
    char *bytes =
        reinterpret_cast<char *>(&variable) + sizeof variable - k_SIZEOF_INT48;
    if (k_SIZEOF_INT48 == d_streamBuf->sgetn(bytes, k_SIZEOF_INT48)) {
        validate();
    }
#endif

    return *this;
}

template <class STREAMBUF>
inline
GenericInStream<STREAMBUF>&
GenericInStream<STREAMBUF>::getUint48(bsls::Types::Uint64& variable)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    invalidate();

    variable = 0;  // zero-extend

#ifdef BSLS_PLATFORM_IS_LITTLE_ENDIAN
    char *bytes = reinterpret_cast<char *>(&variable);
    char  rawBytes[k_SIZEOF_INT48];
    if (k_SIZEOF_INT48 == d_streamBuf->sgetn(rawBytes, k_SIZEOF_INT48)) {
        validate();
        bytes[5] = rawBytes[0];
        bytes[4] = rawBytes[1];
        bytes[3] = rawBytes[2];
        bytes[2] = rawBytes[3];
        bytes[1] = rawBytes[4];
        bytes[0] = rawBytes[5];
    }
#else
    char *bytes =
        reinterpret_cast<char *>(&variable) + sizeof variable - k_SIZEOF_INT48;
    if (k_SIZEOF_INT48 == d_streamBuf->sgetn(bytes, k_SIZEOF_INT48)) {
        validate();
    }
#endif

    return *this;
}

template <class STREAMBUF>
inline
GenericInStream<STREAMBUF>&
GenericInStream<STREAMBUF>::getInt40(bsls::Types::Int64& variable)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    invalidate();

    const int current = d_streamBuf->sgetc();
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                   STREAMBUF::traits_type::eof() == current)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }
    variable = 0x80 & current ? -1 : 0;  // sign extend

#ifdef BSLS_PLATFORM_IS_LITTLE_ENDIAN
    char *bytes = reinterpret_cast<char *>(&variable);
    char  rawBytes[k_SIZEOF_INT40];
    if (k_SIZEOF_INT40 == d_streamBuf->sgetn(rawBytes, k_SIZEOF_INT40)) {
        validate();
        bytes[4] = rawBytes[0];
        bytes[3] = rawBytes[1];
        bytes[2] = rawBytes[2];
        bytes[1] = rawBytes[3];
        bytes[0] = rawBytes[4];
    }
#else
    char *bytes =
        reinterpret_cast<char *>(&variable) + sizeof variable - k_SIZEOF_INT40;
    if (k_SIZEOF_INT40 == d_streamBuf->sgetn(bytes, k_SIZEOF_INT40)) {
        validate();
    }
#endif

    return *this;
}

template <class STREAMBUF>
inline
GenericInStream<STREAMBUF>&
GenericInStream<STREAMBUF>::getUint40(bsls::Types::Uint64& variable)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    invalidate();

    variable = 0;  // zero-extend

#ifdef BSLS_PLATFORM_IS_LITTLE_ENDIAN
    char *bytes = reinterpret_cast<char *>(&variable);
    char  rawBytes[k_SIZEOF_INT40];
    if (k_SIZEOF_INT40 == d_streamBuf->sgetn(rawBytes, k_SIZEOF_INT40)) {
        validate();
        bytes[4] = rawBytes[0];
        bytes[3] = rawBytes[1];
        bytes[2] = rawBytes[2];
        bytes[1] = rawBytes[3];
        bytes[0] = rawBytes[4];
    }
#else
    char *bytes =
        reinterpret_cast<char *>(&variable) + sizeof variable - k_SIZEOF_INT40;
    if (k_SIZEOF_INT40 == d_streamBuf->sgetn(bytes, k_SIZEOF_INT40)) {
        validate();
    }
#endif

    return *this;
}

template <class STREAMBUF>
inline
GenericInStream<STREAMBUF>&
GenericInStream<STREAMBUF>::getInt32(int& variable)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    invalidate();

    if (sizeof variable > k_SIZEOF_INT32) {
        const int current = d_streamBuf->sgetc();
        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                   STREAMBUF::traits_type::eof() == current)) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
            return *this;                                             // RETURN
        }
        variable = 0x80 & current ? -1 : 0;  // sign extend
    }

#ifdef BSLS_PLATFORM_IS_LITTLE_ENDIAN
    char *bytes = reinterpret_cast<char *>(&variable);
    char  rawBytes[k_SIZEOF_INT32];
    if (k_SIZEOF_INT32 == d_streamBuf->sgetn(rawBytes, k_SIZEOF_INT32)) {
        validate();
        bytes[3] = rawBytes[0];
        bytes[2] = rawBytes[1];
        bytes[1] = rawBytes[2];
        bytes[0] = rawBytes[3];
    }
#else
    char *bytes =
        reinterpret_cast<char *>(&variable) + sizeof variable - k_SIZEOF_INT32;
    if (k_SIZEOF_INT32 == d_streamBuf->sgetn(bytes, k_SIZEOF_INT32)) {
        validate();
    }
#endif

    return *this;
}

template <class STREAMBUF>
inline
GenericInStream<STREAMBUF>&
GenericInStream<STREAMBUF>::getUint32(unsigned int& variable)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    invalidate();

    if (sizeof variable > k_SIZEOF_INT32) {
        variable = 0;  // zero-extend
    }

#ifdef BSLS_PLATFORM_IS_LITTLE_ENDIAN
    char *bytes = reinterpret_cast<char *>(&variable);
    char  rawBytes[k_SIZEOF_INT32];
    if (k_SIZEOF_INT32 == d_streamBuf->sgetn(rawBytes, k_SIZEOF_INT32)) {
        validate();
        bytes[3] = rawBytes[0];
        bytes[2] = rawBytes[1];
        bytes[1] = rawBytes[2];
        bytes[0] = rawBytes[3];
    }
#else
    char *bytes =
        reinterpret_cast<char *>(&variable) + sizeof variable - k_SIZEOF_INT32;
    if (k_SIZEOF_INT32 == d_streamBuf->sgetn(bytes, k_SIZEOF_INT32)) {
        validate();
    }
#endif

    return *this;
}

template <class STREAMBUF>
inline
GenericInStream<STREAMBUF>&
GenericInStream<STREAMBUF>::getInt24(int& variable)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    invalidate();

    const int current = d_streamBuf->sgetc();
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                   STREAMBUF::traits_type::eof() == current)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }
    variable = 0x80 & current ? -1 : 0;  // sign extend

#ifdef BSLS_PLATFORM_IS_LITTLE_ENDIAN
    char *bytes = reinterpret_cast<char *>(&variable);
    char  rawBytes[k_SIZEOF_INT24];
    if (k_SIZEOF_INT24 == d_streamBuf->sgetn(rawBytes, k_SIZEOF_INT24)) {
        validate();
        bytes[2] = rawBytes[0];
        bytes[1] = rawBytes[1];
        bytes[0] = rawBytes[2];
    }
#else
    char *bytes =
        reinterpret_cast<char *>(&variable) + sizeof variable - k_SIZEOF_INT24;
    if (k_SIZEOF_INT24 == d_streamBuf->sgetn(bytes, k_SIZEOF_INT24)) {
        validate();
    }
#endif

    return *this;
}

template <class STREAMBUF>
inline
GenericInStream<STREAMBUF>&
GenericInStream<STREAMBUF>::getUint24(unsigned int& variable)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    invalidate();

    variable = 0;  // zero-extend

#ifdef BSLS_PLATFORM_IS_LITTLE_ENDIAN
    char *bytes = reinterpret_cast<char *>(&variable);
    char  rawBytes[k_SIZEOF_INT24];
    if (k_SIZEOF_INT24 == d_streamBuf->sgetn(rawBytes, k_SIZEOF_INT24)) {
        validate();
        bytes[2] = rawBytes[0];
        bytes[1] = rawBytes[1];
        bytes[0] = rawBytes[2];
    }
#else
    char *bytes =
        reinterpret_cast<char *>(&variable) + sizeof variable - k_SIZEOF_INT24;
    if (k_SIZEOF_INT24 == d_streamBuf->sgetn(bytes, k_SIZEOF_INT24)) {
        validate();
    }
#endif

    return *this;
}

template <class STREAMBUF>
inline
GenericInStream<STREAMBUF>&
GenericInStream<STREAMBUF>::getInt16(short& variable)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    invalidate();

    if (sizeof variable > k_SIZEOF_INT16) {
        const int current = d_streamBuf->sgetc();
        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                   STREAMBUF::traits_type::eof() == current)) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
            return *this;                                             // RETURN
        }
        variable = 0x80 & current ? -1 : 0;  // sign extend
    }

#ifdef BSLS_PLATFORM_IS_LITTLE_ENDIAN
    char *bytes = reinterpret_cast<char *>(&variable);
    char  rawBytes[k_SIZEOF_INT16];
    if (k_SIZEOF_INT16 == d_streamBuf->sgetn(rawBytes, k_SIZEOF_INT16)) {
        validate();
        bytes[1] = rawBytes[0];
        bytes[0] = rawBytes[1];
    }
#else
    char *bytes =
        reinterpret_cast<char *>(&variable) + sizeof variable - k_SIZEOF_INT16;
    if (k_SIZEOF_INT16 == d_streamBuf->sgetn(bytes, k_SIZEOF_INT16)) {
        validate();
    }
#endif

    return *this;
}

template <class STREAMBUF>
inline
GenericInStream<STREAMBUF>&
GenericInStream<STREAMBUF>::getUint16(unsigned short& variable)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    invalidate();

    if (sizeof variable > k_SIZEOF_INT16) {
        variable = 0;  // zero-extend
    }

#ifdef BSLS_PLATFORM_IS_LITTLE_ENDIAN
    char *bytes = reinterpret_cast<char *>(&variable);
    char  rawBytes[k_SIZEOF_INT16];
    if (k_SIZEOF_INT16 == d_streamBuf->sgetn(rawBytes, k_SIZEOF_INT16)) {
        validate();
        bytes[1] = rawBytes[0];
        bytes[0] = rawBytes[1];
    }
#else
    char *bytes =
        reinterpret_cast<char *>(&variable) + sizeof variable - k_SIZEOF_INT16;
    if (k_SIZEOF_INT16 == d_streamBuf->sgetn(bytes, k_SIZEOF_INT16)) {
        validate();
    }
#endif

    return *this;
}

template <class STREAMBUF>
inline
GenericInStream<STREAMBUF>&
GenericInStream<STREAMBUF>::getInt8(char& variable)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    invalidate();

    const int current = d_streamBuf->sbumpc();
    if (STREAMBUF::traits_type::eof() != current) {
        validate();
        variable = static_cast<char>(current);
    }

    return *this;
}

template <class STREAMBUF>
inline
GenericInStream<STREAMBUF>&
GenericInStream<STREAMBUF>::getInt8(signed char& variable)
{
    return getInt8(reinterpret_cast<char&>(variable));
}

template <class STREAMBUF>
inline
GenericInStream<STREAMBUF>&
GenericInStream<STREAMBUF>::getUint8(char& variable)
{
    return getInt8(variable);
}

template <class STREAMBUF>
inline
GenericInStream<STREAMBUF>&
GenericInStream<STREAMBUF>::getUint8(unsigned char& variable)
{
    return getInt8(reinterpret_cast<char&>(variable));
}

                      // *** scalar floating-point values ***

template <class STREAMBUF>
inline
GenericInStream<STREAMBUF>&
GenericInStream<STREAMBUF>::getFloat64(double& variable)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    invalidate();

    if (sizeof variable > k_SIZEOF_FLOAT64) {
        variable = 0;  // zero-fill mantissa
    }

#ifdef BSLS_PLATFORM_IS_LITTLE_ENDIAN
    char *bytes = reinterpret_cast<char *>(&variable);
    char  rawBytes[k_SIZEOF_FLOAT64];
    if (k_SIZEOF_FLOAT64 == d_streamBuf->sgetn(rawBytes, k_SIZEOF_FLOAT64)) {
        validate();
        bytes[sizeof variable - 1] = rawBytes[0];
        bytes[sizeof variable - 2] = rawBytes[1];
        bytes[sizeof variable - 3] = rawBytes[2];
        bytes[sizeof variable - 4] = rawBytes[3];
        bytes[sizeof variable - 5] = rawBytes[4];
        bytes[sizeof variable - 6] = rawBytes[5];
        bytes[sizeof variable - 7] = rawBytes[6];
        bytes[sizeof variable - 8] = rawBytes[7];
    }
#else
    char *bytes = reinterpret_cast<char *>(&variable);
    if (k_SIZEOF_FLOAT64 == d_streamBuf->sgetn(bytes, k_SIZEOF_FLOAT64)) {
        validate();
    }
#endif

    return *this;
}

template <class STREAMBUF>
inline
GenericInStream<STREAMBUF>&
GenericInStream<STREAMBUF>::getFloat32(float& variable)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    invalidate();

    if (sizeof variable > k_SIZEOF_FLOAT32) {
        variable = 0;  // zero-fill mantissa
    }

#ifdef BSLS_PLATFORM_IS_LITTLE_ENDIAN
    char *bytes = reinterpret_cast<char *>(&variable);
    char  rawBytes[k_SIZEOF_FLOAT32];
    if (k_SIZEOF_FLOAT32 == d_streamBuf->sgetn(rawBytes, k_SIZEOF_FLOAT32)) {
        validate();
        bytes[sizeof variable - 1] = rawBytes[0];
        bytes[sizeof variable - 2] = rawBytes[1];
        bytes[sizeof variable - 3] = rawBytes[2];
        bytes[sizeof variable - 4] = rawBytes[3];
    }
#else
    char *bytes = reinterpret_cast<char *>(&variable);
    if (k_SIZEOF_FLOAT32 == d_streamBuf->sgetn(bytes, k_SIZEOF_FLOAT32)) {
        validate();
    }
#endif

    return *this;
}

                      // *** string values ***

template <class STREAMBUF>
GenericInStream<STREAMBUF>&
GenericInStream<STREAMBUF>::getString(bsl::string& variable)
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

template <class STREAMBUF>
GenericInStream<STREAMBUF>&
GenericInStream<STREAMBUF>::getArrayInt64(bsls::Types::Int64 *variables,
                                          int                 numVariables)
{
    BSLS_ASSERT(variables);
    BSLS_ASSERT(0 <= numVariables);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(   !isValid()
                                              || 0 == numVariables)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const bsls::Types::Int64 *end = variables + numVariables;
    for (; variables != end; ++variables) {
        getInt64(*variables);
    }

    return *this;
}

template <class STREAMBUF>
GenericInStream<STREAMBUF>&
GenericInStream<STREAMBUF>::getArrayUint64(bsls::Types::Uint64 *variables,
                                           int                  numVariables)
{
    BSLS_ASSERT(variables);
    BSLS_ASSERT(0 <= numVariables);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(   !isValid()
                                              || 0 == numVariables)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const bsls::Types::Uint64 *end = variables + numVariables;
    for (; variables != end; ++variables) {
        getUint64(*variables);
    }

    return *this;
}

template <class STREAMBUF>
GenericInStream<STREAMBUF>&
GenericInStream<STREAMBUF>::getArrayInt56(bsls::Types::Int64 *variables,
                                          int                 numVariables)
{
    BSLS_ASSERT(variables);
    BSLS_ASSERT(0 <= numVariables);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(   !isValid()
                                              || 0 == numVariables)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const bsls::Types::Int64 *end = variables + numVariables;
    for (; variables != end; ++variables) {
        getInt56(*variables);
    }

    return *this;
}

template <class STREAMBUF>
GenericInStream<STREAMBUF>&
GenericInStream<STREAMBUF>::getArrayUint56(bsls::Types::Uint64 *variables,
                                           int                  numVariables)
{
    BSLS_ASSERT(variables);
    BSLS_ASSERT(0 <= numVariables);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(   !isValid()
                                              || 0 == numVariables)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const bsls::Types::Uint64 *end = variables + numVariables;
    for (; variables != end; ++variables) {
        getUint56(*variables);
    }

    return *this;
}

template <class STREAMBUF>
GenericInStream<STREAMBUF>&
GenericInStream<STREAMBUF>::getArrayInt48(bsls::Types::Int64 *variables,
                                          int                 numVariables)
{
    BSLS_ASSERT(variables);
    BSLS_ASSERT(0 <= numVariables);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(   !isValid()
                                              || 0 == numVariables)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const bsls::Types::Int64 *end = variables + numVariables;
    for (; variables != end; ++variables) {
        getInt48(*variables);
    }

    return *this;
}

template <class STREAMBUF>
GenericInStream<STREAMBUF>&
GenericInStream<STREAMBUF>::getArrayUint48(bsls::Types::Uint64 *variables,
                                           int                  numVariables)
{
    BSLS_ASSERT(variables);
    BSLS_ASSERT(0 <= numVariables);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(   !isValid()
                                              || 0 == numVariables)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const bsls::Types::Uint64 *end = variables + numVariables;
    for (; variables != end; ++variables) {
        getUint48(*variables);
    }

    return *this;
}

template <class STREAMBUF>
GenericInStream<STREAMBUF>&
GenericInStream<STREAMBUF>::getArrayInt40(bsls::Types::Int64 *variables,
                                          int                 numVariables)
{
    BSLS_ASSERT(variables);
    BSLS_ASSERT(0 <= numVariables);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(   !isValid()
                                              || 0 == numVariables)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const bsls::Types::Int64 *end = variables + numVariables;
    for (; variables != end; ++variables) {
        getInt40(*variables);
    }

    return *this;
}

template <class STREAMBUF>
GenericInStream<STREAMBUF>&
GenericInStream<STREAMBUF>::getArrayUint40(bsls::Types::Uint64 *variables,
                                           int                  numVariables)
{
    BSLS_ASSERT(variables);
    BSLS_ASSERT(0 <= numVariables);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(   !isValid()
                                              || 0 == numVariables)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const bsls::Types::Uint64 *end = variables + numVariables;
    for (; variables != end; ++variables) {
        getUint40(*variables);
    }

    return *this;
}

template <class STREAMBUF>
GenericInStream<STREAMBUF>&
GenericInStream<STREAMBUF>::getArrayInt32(int *variables, int numVariables)
{
    BSLS_ASSERT(variables);
    BSLS_ASSERT(0 <= numVariables);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(   !isValid()
                                              || 0 == numVariables)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const int *end = variables + numVariables;
    for (; variables != end; ++variables) {
        getInt32(*variables);
    }

    return *this;
}

template <class STREAMBUF>
GenericInStream<STREAMBUF>&
GenericInStream<STREAMBUF>::getArrayUint32(unsigned int *variables,
                                           int           numVariables)
{
    BSLS_ASSERT(variables);
    BSLS_ASSERT(0 <= numVariables);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(   !isValid()
                                              || 0 == numVariables)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const unsigned int *end = variables + numVariables;
    for (; variables != end; ++variables) {
        getUint32(*variables);
    }

    return *this;
}

template <class STREAMBUF>
GenericInStream<STREAMBUF>&
GenericInStream<STREAMBUF>::getArrayInt24(int *variables, int numVariables)
{
    BSLS_ASSERT(variables);
    BSLS_ASSERT(0 <= numVariables);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(   !isValid()
                                              || 0 == numVariables)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const int *end = variables + numVariables;
    for (; variables != end; ++variables) {
        getInt24(*variables);
    }

    return *this;
}

template <class STREAMBUF>
GenericInStream<STREAMBUF>&
GenericInStream<STREAMBUF>::getArrayUint24(unsigned int *variables,
                                           int           numVariables)
{
    BSLS_ASSERT(variables);
    BSLS_ASSERT(0 <= numVariables);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(   !isValid()
                                              || 0 == numVariables)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const unsigned int *end = variables + numVariables;
    for (; variables != end; ++variables) {
        getUint24(*variables);
    }

    return *this;
}

template <class STREAMBUF>
GenericInStream<STREAMBUF>&
GenericInStream<STREAMBUF>::getArrayInt16(short *variables,
                                          int    numVariables)
{
    BSLS_ASSERT(variables);
    BSLS_ASSERT(0 <= numVariables);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(   !isValid()
                                              || 0 == numVariables)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const short *end = variables + numVariables;
    for (; variables != end; ++variables) {
        getInt16(*variables);
    }

    return *this;
}

template <class STREAMBUF>
GenericInStream<STREAMBUF>&
GenericInStream<STREAMBUF>::getArrayUint16(unsigned short *variables,
                                           int             numVariables)
{
    BSLS_ASSERT(variables);
    BSLS_ASSERT(0 <= numVariables);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(   !isValid()
                                              || 0 == numVariables)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const unsigned short *end = variables + numVariables;
    for (; variables != end; ++variables) {
        getUint16(*variables);
    }

    return *this;
}

template <class STREAMBUF>
GenericInStream<STREAMBUF>&
GenericInStream<STREAMBUF>::getArrayInt8(char *variables,
                                         int   numVariables)
{
    BSLS_ASSERT(variables);
    BSLS_ASSERT(0 <= numVariables);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(   !isValid()
                                              || 0 == numVariables)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const char *end = variables + numVariables;
    for (; variables != end; ++variables) {
        getInt8(*variables);
    }

    return *this;
}

template <class STREAMBUF>
inline
GenericInStream<STREAMBUF>&
GenericInStream<STREAMBUF>::getArrayInt8(signed char *variables,
                                         int          numVariables)
{
    BSLS_ASSERT_SAFE(variables);
    BSLS_ASSERT_SAFE(0 <= numVariables);

    return getArrayInt8(reinterpret_cast<char *>(variables), numVariables);
}

template <class STREAMBUF>
inline
GenericInStream<STREAMBUF>&
GenericInStream<STREAMBUF>::getArrayUint8(char *variables,
                                          int   numVariables)
{
    BSLS_ASSERT_SAFE(variables);
    BSLS_ASSERT_SAFE(0 <= numVariables);

    return getArrayInt8(variables, numVariables);
}

template <class STREAMBUF>
inline
GenericInStream<STREAMBUF>&
GenericInStream<STREAMBUF>::getArrayUint8(unsigned char *variables,
                                          int            numVariables)
{
    BSLS_ASSERT_SAFE(variables);
    BSLS_ASSERT_SAFE(0 <= numVariables);

    return getArrayInt8(reinterpret_cast<char *>(variables), numVariables);
}

                      // *** arrays of floating-point values ***

template <class STREAMBUF>
GenericInStream<STREAMBUF>&
GenericInStream<STREAMBUF>::getArrayFloat64(double *variables,
                                            int     numVariables)
{
    BSLS_ASSERT(variables);
    BSLS_ASSERT(0 <= numVariables);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(   !isValid()
                                              || 0 == numVariables)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const double *end = variables + numVariables;
    for (; variables != end; ++variables) {
        getFloat64(*variables);
    }

    return *this;
}

template <class STREAMBUF>
GenericInStream<STREAMBUF>&
GenericInStream<STREAMBUF>::getArrayFloat32(float *variables,
                                            int    numVariables)
{
    BSLS_ASSERT(variables);
    BSLS_ASSERT(0 <= numVariables);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(   !isValid()
                                              || 0 == numVariables)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const float *end = variables + numVariables;
    for (; variables != end; ++variables) {
        getFloat32(*variables);
    }

    return *this;
}

// ACCESSORS
template <class STREAMBUF>
inline
GenericInStream<STREAMBUF>::operator const void *() const
{
    return isValid() ? this : 0;
}

template <class STREAMBUF>
inline
bool GenericInStream<STREAMBUF>::isValid() const
{
    return d_validFlag;
}

template <class STREAMBUF, class TYPE>
inline
GenericInStream<STREAMBUF>&
                operator>>(GenericInStream<STREAMBUF>& stream, TYPE& value)
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
