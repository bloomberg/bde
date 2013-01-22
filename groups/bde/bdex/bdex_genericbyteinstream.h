// bdex_genericbyteinstream.h                                         -*-C++-*-
#ifndef INCLUDED_BDEX_GENERICBYTEINSTREAM
#define INCLUDED_BDEX_GENERICBYTEINSTREAM

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Parse fundamental types from provided memory.
//
//@CLASSES:
//   bdex_GenericByteInStream: read from a parameterized stream buffer
//
//@AUTHOR: Herve Bronnimann (hbronnimann)
//
//@SEE_ALSO:  bdex_byteoutstreamformatter, bdex_genericbyteoutstream
//
//@DESCRIPTION: This component conforms to the 'bdex_instream' (documentation
// only) "protocol", implementing a byte-array-based data stream object that
// provides platform-independent input methods ("unexternalization") on values,
// and arrays of values, of fundamental types, and on 'bsl::string'.
//
// This component is intended to be used in conjunction with the
// 'bdex_genericbyteoutstream' externalization component.  Each input method of
// 'bdex_GenericByteInStream' reads either a value or a homogeneous array of
// values of a fundamental type, in a format that was written by the
// corresponding 'bdex_GenericByteOutStream' method.  In general, the user of
// this component cannot rely on being able to read data that was written by
// any mechanism other than 'bdex_GenericByteOutStream'.  Note that the
// 'bdex_ByteOutStream' is a kind of 'bdex_GenericByteOutStream', so it is
// perfectly acceptable to write with a 'bdex_ByteOutStream' values that will
// be read with a 'bdex_GenericByteInStream', and vice versa.
//
// The supported types and required content are listed in the table below.
// All of the fundamental types in the table may be input as scalar values or
// as homogeneous arrays.  'bsl::string' is input as a logical scalar string.
// Note that 'Int64' and 'Uint64' denote 'bsls_Types::Int64' and
// 'bsls_Types::Uint64', which in turn are 'typedef' names for the signed and
// unsigned 64-bit integer types, respectively, on the host platform.
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
// This component also supports compact streaming in of integer types.  In
// particular, 64-bit values can be streamed in from 40, 48, 56, or 64-bit
// values, and 32-bit values can be streamed in from 24 and 32-bit values
// (consistent with what has been written to the stream, of course).  Note
// that, for signed types, the sign is preserved for all streamed-in values.
//
// Note that input streams can be *invalidated* explicitly and queried for
// *validity* and *emptiness*.  Reading from an initially invalid stream has no
// effect.  Attempting to read beyond the end of a stream will automatically
// invalidate the stream.  Whenever an inconsistent value is detected, the
// stream should be invalidated explicitly.
//
///Generic Byte-Format Parser
///--------------------------
// The class 'bdex_GenericByteInStream' is parameterized by a
// buffered stream class 'StreamBuf' which, given the declarations:
//..
//  char c;
//  int  len;
//  const char *s;
//  StreamBuf  *sb;
//..
// makes the following expressions syntactically valid:
//..
//  sb->sbumpc();
//  sb->sgetc(c);
//  sb->sgetn(s, len);
//  sb->pubsync();
//..
// Suitable choices for 'StreamBuf' include any class that implements the
// 'bsl::basic_streambuf' protocol, but also 'bdesb_FixedMemInput' which does
// not inherit from 'bsl::streambuf' and which, therefore, does not have the
// initialization and virtual function overhead of 'bsl::streambuf'.
//
// The class 'bdex_ByteInStreamFormatter' is not parameterized and is resolved
// to be the same type as 'bdex_GenericByteInStream<bsl::streambuf>'.
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
// Suppose we wish to implement a (deliberately simple) 'my_Person' class (in
// an appropriate 'my_person' component) as a value-semantic object that
// supports 'bdex' externalization.  In addition to whatever data and methods
// that we choose to put into our design, we must supply three methods having
// specific names and signatures in order to comply with the 'bdex' "protocol":
// a class method 'maxSupportedBdexVersion' and two object methods, an
// accessor (i.e., a 'const' method) 'bdexStreamOut', and a manipulator (i.e.,
// a non-'const' method) 'bdex_StreamIn'.  This example shows how to implement
// those three methods for the simple "person" component.
//
// In this example we will not worry overly about "good design" of the person
// component, and we will declare but not implement illustrative methods and
// free operators, except for the three required 'bdex' methods, which are
// implemented in full.  In particular, we will not make explicit use of
// 'bdema' allocators; a more complete design would do so.
//..
//  // my_person.h
//
//  class my_Person {
//      bsl::string d_firstName;
//      bsl::string d_lastName;
//      int         d_age;
//
//      friend bool operator==(const my_Person&, const my_Person&);
//
//    public:
//      // CLASS METHODS
//      static int maxSupportedBdexVersion(void) { return 1; }
//          // Return the most current 'bdex' streaming version number
//          // supported by this class.  (See the 'bdex' package-level
//          // documentation for more information on 'bdex' streaming of
//          // container types.)
//
//      // CREATORS
//      my_Person();
//          // Create a default person.
//
//      my_Person(const char *firstName, const char *lastName, int age);
//          // Create a person having the specified 'firstName', 'lastName',
//          // and 'age'.
//
//      my_Person(const my_Person& original);
//          // Create a person having value of the specified 'original' person.
//
//      ~my_Person();
//          // Destroy this object.
//
//      // MANIPULATORS
//      my_Person& operator=(const my_Person& rhs);
//          // Assign to this person the value of the specified 'rhs' person.
//
//      template <class STREAM>
//      STREAM& bdexStreamIn(STREAM& stream, int version);
//          // Assign to this object the value read from the specified input
//          // 'stream' using the specified 'version' format and return a
//          // reference to the modifiable 'stream'.  If 'stream' is initially
//          // invalid, this operation has no effect.  If 'stream' becomes
//          // invalid during this operation, this object is valid, but its
//          // value is undefined.  If the specified 'version' is not
//          // supported, 'stream' is marked invalid, but this object is
//          // unaltered.  Note that no version is read from 'stream'.  (See
//          // the 'bdex' package-level documentation for more information on
//          // 'bdex' streaming of container types.)
//
//      // Other manipulators omitted.
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
//          // Write this value to the specified output 'stream' and return a
//          // reference to the modifiable 'stream'.  Optionally specify an
//          // explicit 'version' format; by default, the maximum supported
//          // version is written to 'stream' and used as the format.  If
//          // 'version' is specified, that format is used but *not* written to
//          // 'stream'.  If 'version' is not supported, 'stream' is left
//          // unmodified.  (See the 'bdex' package-level documentation for
//          // more information on 'bdex' streaming of container types).
//
//      // Other accessors omitted.
//
//  };
//
//  // FREE OPERATORS
//  inline
//  bool operator==(const my_Person& lhs, const my_Person& rhs);
//      // Return 'true' if the specified 'lhs' and 'rhs' person objects have
//      // the same value and 'false' otherwise.  Two person objects have the
//      // same value if they have the same first name, last name, and age.
//
//  inline
//  bool operator!=(const my_Person& lhs, const my_Person& rhs);
//      // Return 'true' if the specified 'lhs' and 'rhs' person objects do not
//      // have the same value and 'false' otherwise.  Two person objects
//      // differ in value if they differ in first name, last name, or age.
//
//  bsl::ostream& operator<<(bsl::ostream& stream, const my_Person& person);
//      // Write the specified 'date' value to the specified output 'stream' in
//      // some reasonable format.
//
//                           // INLINE FUNCTION DEFINITIONS
//  template <class STREAM>
//  STREAM& my_Person::bdexStreamIn(STREAM& stream, int version)
//  {
//      if (stream) {
//          switch (version) {    // switch on the 'bdex' version
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
//                  d_age = 1;     // *might* be corrupted; value for testing
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
//  STREAM& my_Person::bdexStreamOut(STREAM& stream, int version) const
//  {
//      switch (version) {
//        case 1: {
//          stream.putString(d_firstName);
//          stream.putString(d_lastName);
//          stream.putInt32(d_age);
//        } break;
//      }
//      return stream;
//  }
//..
//
// We can now exercise the new 'my_person' value-semantic component by
// externalizing and reconstituting an object.  First create a 'my_Person'
// 'JaneSmith' and a 'bdex_GenericByteOutStream' 'outStream'.
//..
//  // my_testapp.m.cpp
//
//  using namespace BloombergLP;
//  using namespace bsl;
//
//  int main(int argc, char *argv[])
//  {
//      my_Person JaneSmith("Jane", "Smith", 42);
//      char buffer[200];
//      bdesb_FixedMemOutStreamBuf osb(buffer, 200);
//      bdex_GenericByteOutStream<bdesb_FixedMemOutStreamBuf>
//                                                             outStream(&osb);
//      const int VERSION = 1;
//      outStream.putVersion(VERSION);
//      bdex_OutStreamFunctions::streamOut(outStream, JaneSmith, VERSION);
//..
// Next create a 'my_Person' 'janeCopy' initialized to the default value, and
// assert that 'janeCopy' is different from 'janeSmith'.
//..
//      my_Person janeCopy;                      assert(janeCopy != JaneSmith);
//..
// Now create a 'bdex_GenericByteInStream' object 'inStream' initialized with
// the buffer from the 'bdex_GenericByteOutStream' object 'outStream'.  To
// demonstrate the generic aspect, we will use 'bdesb_FixedMemInput' and
// 'bdesb_FixedMemOutput' objects for implementing the underlying streaming
// to the memory buffers.
//..
//      bdesb_FixedMemInput sb(buffer, osb.length());
//      bdex_GenericByteInStream<bdesb_FixedMemInput> inStream(&sb);
//      int version;
//      inStream.getVersion(version);
//      bdex_InStreamFunctions::streamIn(inStream, janeCopy, version);
//      assert(janeCopy == JaneSmith);
//
//      // Verify the results on 'stdout'.
//      if (janeCopy == JaneSmith) {
//          cout << "Successfully serialized and de-serialized Jane Smith:"
//               << "\n\tFirstName: " << janeCopy.firstName()
//               << "\n\tLastName : " << janeCopy.lastName()
//               << "\n\tAge      : " << janeCopy.age() << endl;
//      }
//      else {
//          cout << "Serialization unsuccessful.  janeCopy holds:"
//               << "\n\tFirstName: " << janeCopy.firstName()
//               << "\n\tLastName : " << janeCopy.lastName()
//               << "\n\tAge      : " << janeCopy.age() << endl;
//      }
//      return 0;
// }
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEX_INSTREAMMETHODS
#include <bdex_instreammethods.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_BYTEORDER
#include <bsls_byteorder.h>
#endif

#ifndef INCLUDED_BSLS_PERFORMANCEHINT
#include <bsls_performancehint.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>  // @DEPRECATED
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_STREAMBUF
#include <bsl_streambuf.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {

                        // ==============================
                        // class bdex_GenericByteInStream
                        // ==============================

template <class StreamBuf>
class bdex_GenericByteInStream {
    // This class, parameterized by a stream buffer, facilitates the
    // unexternalization of values (and C-style arrays of values) of the
    // fundamental integral and floating-point types in a data-independent,
    // platform-neutral representation.  The 'StreamBuf' parameter needs to
    // support 'sgetc', 'sgetn' and 'sbumpc' methods.  Note that
    // 'bsl::streambuf' is suitable for use as the 'StreamBuf' parameter type,
    // in which case use of this parameterized class may be replaced by the
    // non-parameterized 'bdex_ByteOutStreamFormatter' class.

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

    typedef bsl::char_traits<char> traits;
    typedef traits::int_type       int_type;

    // DATA
    StreamBuf *d_streamBuf;  // held, not owned
    bool       d_valid;      // indicates stream validity

    // NOT DEFINED
    bdex_GenericByteInStream(const bdex_GenericByteInStream&);
    bdex_GenericByteInStream& operator=(const bdex_GenericByteInStream&);

    bool operator==(const bdex_GenericByteInStream&) const;
    bool operator!=(const bdex_GenericByteInStream&) const;

    template<class OTHER_BUF>
    bool operator==(const bdex_GenericByteInStream<OTHER_BUF>&) const;
    template<class OTHER_BUF>
    bool operator!=(const bdex_GenericByteInStream<OTHER_BUF>&) const;

  public:
    // CREATORS
    bdex_GenericByteInStream(StreamBuf *streamBuffer);
        // Create an input byte stream that reads bytes obtained from the
        // specified 'streamBuffer' and formatted by a corresponding
        // 'bdex_ByteOutStreamFormatter' object.

    //! ~bdex_GenericByteInStream();
        // Destroy this object.  Note that this method's definition is compiler
        // generated.

    // MANIPULATORS
    bdex_GenericByteInStream& getLength(int& length);
        // Consume either an 8-bit unsigned integer or a 32-bit signed integer
        // value representing a length from this input stream, placing it in
        // the specified 'length', and return a reference to this modifiable
        // stream.  If this stream is initially invalid, this operation has no
        // effect.  If this function otherwise fails to extract a valid value,
        // this stream is marked invalid and the value of 'variable' is
        // undefined.  Choice of 8- versus 32-bit extraction depends on the
        // most significant bit of the next byte to be read from the stream; if
        // it is 0 consume an 8-bit unsigned integer, otherwise consume a
        // 32-bit signed integer after setting this bit to zero.

    bdex_GenericByteInStream& getString(bsl::string& str);
        // Consume a string from this input stream, place that value in the
        // specified 'str', and return a reference to this modifiable stream.
        // If this stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value, this
        // stream is marked invalid and the value of 'str' is undefined.
        // The string must be prefaced by a non-negative integer indicating
        // the number of characters composing the string.

    bdex_GenericByteInStream& getVersion(int& variable);
        // Consume an 8-bit unsigned integer value representing a version from
        // this input stream, place it in the specified 'variable', and return
        // a reference to this modifiable stream.  If this stream is initially
        // invalid, this operation has no effect.  If this function otherwise
        // fails to extract a valid value, this stream is marked invalid and
        // the value of 'variable' is undefined.

     ///Getting Scalar Integral Values
     /// - - - - - - - - - - - - - - -
     bdex_GenericByteInStream& getInt64(bsls_Types::Int64& variable);
        // Read eight bytes from this input stream as a 64-bit, two's
        // complement integer (in network byte order) and store that value
        // in the specified 'variable' (in host byte order).  Note that the
        // value will be sign-extended if 'variable' holds more than 64 bits
        // on this platform.

     bdex_GenericByteInStream& getUint64(bsls_Types::Uint64& variable);
        // Read eight bytes from this input stream as a 64-bit, two's
        // complement integer (in network byte order) and store that value
        // in the specified 'variable' (in host byte order).  Note that the
        // value will be zero-extended if 'variable' holds more than 64 bits
        // on this platform.

     bdex_GenericByteInStream& getInt56(bsls_Types::Int64& variable);
        // Read seven bytes from this input stream as a 56-bit, two's
        // complement integer (in network byte order) and store that value
        // in the specified 'variable' (in host byte order).  Note that the
        // value will be sign-extended to the size of the 'variable'
        // on this platform.

     bdex_GenericByteInStream& getUint56(bsls_Types::Uint64& variable);
        // Read seven bytes from this input stream as a 56-bit, two's
        // complement integer (in network byte order) and store that value
        // in the specified 'variable' (in host byte order).  Note that the
        // value will be zero-extended to the size of the 'variable'
        // on this platform.

     bdex_GenericByteInStream& getInt48(bsls_Types::Int64& variable);
        // Read six bytes from this input stream as a 48-bit, two's
        // complement integer (in network byte order) and store that value
        // in the specified 'variable' (in host byte order).  Note that the
        // value will be sign-extended to the size of the 'variable'
        // on this platform.

     bdex_GenericByteInStream& getUint48(bsls_Types::Uint64& variable);
        // Read six bytes from this input stream as a 48-bit, two's
        // complement integer (in network byte order) and store that value
        // in the specified 'variable' (in host byte order).  Note that the
        // value will be zero-extended to the size of the 'variable'
        // on this platform.

     bdex_GenericByteInStream& getInt40(bsls_Types::Int64& variable);
        // Read five bytes from this input stream as a 40-bit, two's
        // complement integer (in network byte order) and store that value
        // in the specified 'variable' (in host byte order).  Note that the
        // value will be sign-extended to the size of the 'variable'
        // on this platform.

     bdex_GenericByteInStream& getUint40(bsls_Types::Uint64& variable);
        // Read five bytes from this input stream as a 40-bit, two's
        // complement integer (in network byte order) and store that value
        // in the specified 'variable' (in host byte order).  Note that the
        // value will be zero-extended to the size of the 'variable'
        // on this platform.

     bdex_GenericByteInStream& getInt32(int& variable);
        // Read four bytes from this input stream as a 32-bit, two's
        // complement integer (in network byte order) and store that value
        // in the specified 'variable' (in host byte order).  Note that the
        // value will be sign-extended if 'variable' holds more than 32 bits
        // on this platform.

     bdex_GenericByteInStream& getUint32(unsigned int& variable);
        // Read four bytes from this input stream as a 32-bit, two's
        // complement integer (in network byte order) and store that value
        // in the specified 'variable' (in host byte order).  Note that the
        // value will be zero-extended if 'variable' holds more than 32 bits
        // on this platform

     bdex_GenericByteInStream& getInt24(int& variable);
        // Read three bytes from this input stream as a 24-bit, two's
        // complement integer (in network byte order) and store that value
        // in the specified 'variable' (in host byte order).  Note that the
        // value will be sign-extended to the size of the 'variable'
        // on this platform.

     bdex_GenericByteInStream& getUint24(unsigned int& variable);
        // Read three bytes from this input stream as a 24-bit, two's
        // complement integer (in network byte order) and store that value
        // in the specified 'variable' (in host byte order).  Note that the
        // value will be zero-extended to the size of the 'variable'
        // on this platform.

     bdex_GenericByteInStream& getInt16(short& variable);
        // Read two bytes from this input stream as a 16-bit, two's
        // complement integer (in network byte order) and store that value
        // in the specified 'variable' (in host byte order).  Note that the
        // value will be sign-extended if 'variable' holds more than 16 bits
        // on this platform

     bdex_GenericByteInStream& getUint16(unsigned short& variable);
        // Read two bytes from this input stream as a 16-bit, two's
        // complement integer (in network byte order) and store that value
        // in the specified 'variable' (in host byte order).  Note that the
        // value will be zero-extended if 'variable' holds more than 16 bits
        // on this platform.

     bdex_GenericByteInStream& getInt8(char& variable);
     bdex_GenericByteInStream& getInt8(signed char& variable);
        // Read one byte from this input stream as an 8-bit, two's
        // complement integer and store that value in the specified 'variable'.

    bdex_GenericByteInStream& getUint8(char& variable);
    bdex_GenericByteInStream& getUint8(unsigned char& variable);
        // Consume an 8-bit unsigned integer value from this input stream,
        // place that value in the specified 'variable', and return a reference
        // to this modifiable stream.  If this stream is initially invalid,
        // this operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.

     ///Getting Scalar Floating-Point Values
     /// - - - - - - - - - - - - - - - - - -
     bdex_GenericByteInStream& getFloat64(double& variable);
        // Read eight bytes from this input stream as a standard IEEE
        // double-precision floating-point number (in network byte order) and
        // store that value in the specified 'variable' (in host byte order).
        // Note that for non-conforming platforms, this operation may be lossy.

     bdex_GenericByteInStream& getFloat32(float& variable);
        // Read four bytes from this input stream as a standard IEEE
        // single-precision floating-point number (in network byte order) and
        // store that value in the specified 'variable' (in host byte order).
        // Note that for non-conforming platforms, this operation may be lossy.

     ///Getting Arrays of Integral Values
     ///- - - - - - - - - - - - - - - - -
     bdex_GenericByteInStream&
     getArrayInt64(bsls_Types::Int64 *array, int count);
        // Read the specified 'count' eight-byte, two's complement integers (in
        // network byte order) from this input stream and store these
        // values sequentially in the the specified 'array' (in host byte
        // order).  The behavior is undefined unless '0 <= count'.  Note that
        // each element value will be sign-extended if '*array' holds more than
        // 64 bits on this platform.

     bdex_GenericByteInStream&
     getArrayUint64(bsls_Types::Uint64 *array, int count);
        // Read the specified 'count' eight-byte, two's complement integers (in
        // network byte order) from this input stream and store these
        // values sequentially in the the specified 'array' (in host byte
        // order).  The behavior is undefined unless '0 <= count'.  Note that
        // each element value will be zero-extended if '*array' holds more than
        // 64 bits on this platform.

     bdex_GenericByteInStream&
     getArrayInt56(bsls_Types::Int64 *array, int count);
        // Read the specified 'count' seven-byte, two's complement integers (in
        // network byte order) from this input stream and store these
        // values sequentially in the the specified 'array' (in host byte
        // order).  The behavior is undefined unless '0 <= count'.  Note that
        // each element value will be sign-extended to the size of the array
        // element on this platform.

     bdex_GenericByteInStream&
     getArrayUint56(bsls_Types::Uint64 *array, int count);
        // Read the specified 'count' seven-byte, two's complement integers (in
        // network byte order) from this input stream and store these
        // values sequentially in the the specified 'array' (in host byte
        // order).  The behavior is undefined unless '0 <= count'.  Note that
        // each element value will be zero-extended to the size of the array
        // element on this platform.

     bdex_GenericByteInStream&
     getArrayInt48(bsls_Types::Int64 *array, int count);
        // Read the specified 'count' six-byte, two's complement integers (in
        // network byte order) from this input stream and store these
        // values sequentially in the the specified 'array' (in host byte
        // order).  The behavior is undefined unless '0 <= count'.  Note that
        // each element value will be sign-extended to the size of the array
        // element on this platform.

     bdex_GenericByteInStream&
     getArrayUint48(bsls_Types::Uint64 *array, int count);
        // Read the specified 'count' six-byte, two's complement integers (in
        // network byte order) from this input stream and store these
        // values sequentially in the the specified 'array' (in host byte
        // order).  The behavior is undefined unless '0 <= count'.  Note that
        // each element value will be zero-extended to the size of the array
        // element on this platform.

     bdex_GenericByteInStream&
     getArrayInt40(bsls_Types::Int64 *array, int count);
        // Read the specified 'count' five-byte, two's complement integers (in
        // network byte order) from this input stream and store these
        // values sequentially in the the specified 'array' (in host byte
        // order).  The behavior is undefined unless '0 <= count'.  Note that
        // each element value will be sign-extended to the size of the array
        // element on this platform.

     bdex_GenericByteInStream&
     getArrayUint40(bsls_Types::Uint64 *array, int count);
        // Read the specified 'count' five-byte, two's complement integers (in
        // network byte order) from this input stream and store these
        // values sequentially in the the specified 'array' (in host byte
        // order).  The behavior is undefined unless '0 <= count'.  Note that
        // each element value will be zero-extended to the size of the array
        // element on this platform.

     bdex_GenericByteInStream& getArrayInt32(int *array, int count);
        // Read the specified 'count' four-byte, two's complement integers (in
        // network byte order) from this input stream and store these
        // values sequentially in the the specified 'array' (in host byte
        // order).  The behavior is undefined unless '0 <= count'.  Note that
        // each element value will be zero-extended to the size of the array
        // element on this platform.

     bdex_GenericByteInStream& getArrayUint32(unsigned int *array, int count);
        // Read the specified 'count' four-byte, two's complement integers (in
        // network byte order) from this input stream and store these
        // values sequentially in the the specified 'array' (in host byte
        // order).  The behavior is undefined unless '0 <= count'.  Note that
        // each element value will be zero-extended to the size of the array
        // element on this platform.

     bdex_GenericByteInStream& getArrayInt24(int *array, int count);
        // Read the specified 'count' three-byte, two's complement integers (in
        // network byte order) from this input stream and store these
        // values sequentially in the the specified 'array' (in host byte
        // order).  The behavior is undefined unless '0 <= count'.  Note that
        // each element value will be zero-extended to the size of the array
        // element on this platform.

     bdex_GenericByteInStream& getArrayUint24(unsigned int *array, int count);
        // Read the specified 'count' three-byte, two's complement integers (in
        // network byte order) from this input stream and store these
        // values sequentially in the the specified 'array' (in host byte
        // order).  The behavior is undefined unless '0 <= count'.  Note that
        // each element value will be zero-extended to the size of the array
        // element on this platform.

     bdex_GenericByteInStream& getArrayInt16(short *array, int count);
        // Read the specified 'count' two-byte, two's complement integers (in
        // network byte order) from this input stream and store these
        // values sequentially in the the specified 'array' (in host byte
        // order).  The behavior is undefined unless '0 <= count'.  Note that
        // each element value will be zero-extended to the size of the array
        // element on this platform.

     bdex_GenericByteInStream& getArrayUint16(unsigned short *array,
                                              int             count);
        // Read the specified 'count' two-byte, two's complement integers (in
        // network byte order) from this input stream and store these
        // values sequentially in the the specified 'array' (in host byte
        // order).  The behavior is undefined unless '0 <= count'.  Note that
        // each element value will be zero-extended to the size of the array
        // element on this platform.

     bdex_GenericByteInStream& getArrayInt8(char *array, int count);
     bdex_GenericByteInStream& getArrayInt8(signed char *array, int count);
        // Read the specified 'count' one-byte, two's complement integers from
        // this input stream and store these values sequentially in the
        // specified 'array'.  The behavior is undefined unless '0 <= count'.

    bdex_GenericByteInStream& getArrayUint8(char *array, int count);
    bdex_GenericByteInStream& getArrayUint8(unsigned char *array, int count);
        // Consume the 8-bit unsigned integer array of the specified 'count'
        // from this input stream, place that value in the specified 'array',
        // and return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'count',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless '0 <= count'.

     ///Getting Arrays of Floating-Point Values
     ///- - - - - - - - - - - - - - - - - - - -
     bdex_GenericByteInStream& getArrayFloat64(double *array, int count);
        // Read the specified 'count' eight-byte, IEEE double-precision
        // floating-point numbers (in network byte order) from the specified
        // 'buffer' and store these values sequentially in the specified
        // 'array' (in host byte order).  The behavior is undefined unless
        // '0 <= count'.  Note that for non-conforming platforms, this
        // operation may be lossy.

     bdex_GenericByteInStream& getArrayFloat32(float *array, int count);
        // Read the specified 'count' four-byte, IEEE double-precision
        // floating-point numbers (in network byte order) from the specified
        // 'buffer' and store these values sequentially in the specified
        // 'array' (in host byte order).  The behavior is undefined unless
        // '0 <= count'.  Note that for non-conforming platforms, this
        // operation may be lossy.

    void invalidate();
        // Put this input stream in an invalid state.  This function has no
        // effect if this stream is already invalid.  Note that this function
        // should be called whenever a value extracted from this stream is
        // determined to be invalid, inconsistent, or otherwise incorrect.

    // ACCESSORS
    operator const void *() const;
        // Return non-zero if this stream is valid, and 0 otherwise.  An
        // invalid stream denotes a stream in which insufficient or invalid
        // data was detected during an extraction operation.  Note that an
        // empty stream will be valid unless an extraction attempt or explicit
        // invalidation causes it to be otherwise.
};

// FREE OPERATORS
// TBD #ifndef BDE_OMIT_DEPRECATED

template <class StreamBuf, typename VALUE>
inline
bdex_GenericByteInStream<StreamBuf>& operator>>(
                                   bdex_GenericByteInStream<StreamBuf>& stream,
                                   VALUE&                               value);
    // Read from the specified 'stream' a 'bdex' version and assign to the
    // specified 'bdex'-compliant 'value' the value read from 'stream' using
    // 'VALUE's 'bdex' version format as read from 'stream'.  Return a
    // reference to the modifiable 'stream'.  If 'stream' is initially invalid,
    // this operation has no effect.  If 'stream' becomes invalid during this
    // operation, 'value' is valid, but its value is undefined.  If the read
    // in version is not supported by 'value', 'stream' is marked invalid,
    // but 'value' is unaltered.  The behavior is undefined unless the data in
    // 'stream' was written by a 'bdex_ByteOutStream' object.  (See the 'bdex'
    // package-level documentation for more information on 'bdex' streaming of
    // container types.)  Note that specializations for the supported
    // fundamental (scalar) types and for 'bsl::string' are also provided, in
    // which case this operator calls the appropriate 'get' method from this
    // component.

// TBD #endif  // BDE_OMIT_DEPRECATED

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        // ------------------------------
                        // class bdex_GenericByteInStream
                        // ------------------------------

// CREATORS
template <class StreamBuf>
inline
bdex_GenericByteInStream<StreamBuf>::bdex_GenericByteInStream(
                                                       StreamBuf *streamBuffer)
: d_streamBuf(streamBuffer)
, d_valid(true)
{
   BSLS_ASSERT_SAFE(streamBuffer);
}

// MANIPULATORS
template <class StreamBuf>
inline
bdex_GenericByteInStream<StreamBuf>&
bdex_GenericByteInStream<StreamBuf>::getUint8(unsigned char& variable)
{
    getInt8((char &)variable);
    return *this;
}

template <class StreamBuf>
inline
bdex_GenericByteInStream<StreamBuf>&
bdex_GenericByteInStream<StreamBuf>::getVersion(int& variable)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!d_valid)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    unsigned char tmp;
    getUint8(tmp);
    variable = tmp;

    return *this;
}

template <class StreamBuf>
bdex_GenericByteInStream<StreamBuf>&
bdex_GenericByteInStream<StreamBuf>::getString(bsl::string& str)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!d_valid)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    int length;
    getLength(length);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!d_valid)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    // 'length' could be corrupt or intentionally invalid, so we limit the
    // initial 'resize' to something that can accommodate the preponderance of
    // strings that will arise in practice.  The remaining portion of a string
    // longer than 16M is read in via a second pass.

    enum { BDEX_INITIAL_ALLOCATION_SIZE = 16 * 1024 * 1024 };

    const int initialLength = length < BDEX_INITIAL_ALLOCATION_SIZE
                              ? length
                              : BDEX_INITIAL_ALLOCATION_SIZE;

    str.resize(initialLength);

    if (0 == length) {
        return *this;                                                 // RETURN
    }

    getArrayUint8(&str.front(), initialLength);
    if (d_valid && length > initialLength) {
        str.resize(length);
        getArrayUint8(&str[initialLength], length - initialLength);
    }

    return *this;
}

template <class StreamBuf>
inline
bdex_GenericByteInStream<StreamBuf>&
bdex_GenericByteInStream<StreamBuf>::getLength(int& length)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!d_valid)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    if (127 < d_streamBuf->sgetc()) {
        // If 'length > 127', 'length' is stored as 4 bytes with top-bit set.
        getInt32(length);
        length ^= (1 << 31);  // Clear top bit.
    }
    else {
        // If 'length <= 127', 'length' is stored as one byte.
        unsigned char tmp;
        getUint8(tmp);
        length = tmp;
    }

    return *this;
}

///Getting Scalar Integral Values
/// - - - - - - - - - - - - - - -

template <class StreamBuf>
inline
bdex_GenericByteInStream<StreamBuf>&
bdex_GenericByteInStream<StreamBuf>::getInt64(bsls_Types::Int64& variable)
{
    typedef union {
        bsls_Types::Int64 d_variable;
        char              d_bytes[sizeof(bsls_Types::Int64)];
    }& T;

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!d_valid)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(d_streamBuf->sgetn(
                                          &(T(variable).d_bytes[0]), 8) < 8)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        d_valid = false;
    }
#ifdef BSLS_PLATFORM_IS_LITTLE_ENDIAN
    else {
        T(variable).d_variable =
                         BSLS_BYTEORDER_BE_U64_TO_HOST(T(variable).d_variable);
    }
#endif

    return *this;
}

template <class StreamBuf>
inline
bdex_GenericByteInStream<StreamBuf>&
bdex_GenericByteInStream<StreamBuf>::getUint64(bsls_Types::Uint64& variable)
{
    return getInt64((bsls_Types::Int64&)variable);
}

template <class StreamBuf>
#if BSLS_PLATFORM_IS_BIG_ENDIAN
inline
#endif
bdex_GenericByteInStream<StreamBuf>&
bdex_GenericByteInStream<StreamBuf>::getInt56(bsls_Types::Int64& variable)
{
    typedef union {
        bsls_Types::Int64 d_variable;
        char              d_bytes[sizeof(bsls_Types::Int64)];
    }& T;

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!d_valid)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    variable = 0x80 & d_streamBuf->sgetc() ? -1 : 0;  // sign extend

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    T(variable).d_bytes[6] = static_cast<char>(d_streamBuf->sbumpc());
    T(variable).d_bytes[5] = static_cast<char>(d_streamBuf->sbumpc());
    T(variable).d_bytes[4] = static_cast<char>(d_streamBuf->sbumpc());
    T(variable).d_bytes[3] = static_cast<char>(d_streamBuf->sbumpc());
    T(variable).d_bytes[2] = static_cast<char>(d_streamBuf->sbumpc());
    T(variable).d_bytes[1] = static_cast<char>(d_streamBuf->sbumpc());

    const int_type result = d_streamBuf->sbumpc();
    if (traits::eof() == result) {
        d_valid = false;
    }
    T(variable).d_bytes[0] = static_cast<char>(result);
#else
    if (d_streamBuf->sgetn(
                         &(T(variable).d_bytes[sizeof variable - 7]), 7) < 7) {
        d_valid = false;
    }
#endif

    return *this;
}

template <class StreamBuf>
#if BSLS_PLATFORM_IS_BIG_ENDIAN
inline
#endif
bdex_GenericByteInStream<StreamBuf>&
bdex_GenericByteInStream<StreamBuf>::getUint56(bsls_Types::Uint64& variable)
{
    typedef union {
        bsls_Types::Uint64 d_variable;
        char               d_bytes[sizeof(bsls_Types::Uint64)];
    }& T;

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!d_valid)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    variable = 0;  // zero extend

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    T(variable).d_bytes[6] = static_cast<char>(d_streamBuf->sbumpc());
    T(variable).d_bytes[5] = static_cast<char>(d_streamBuf->sbumpc());
    T(variable).d_bytes[4] = static_cast<char>(d_streamBuf->sbumpc());
    T(variable).d_bytes[3] = static_cast<char>(d_streamBuf->sbumpc());
    T(variable).d_bytes[2] = static_cast<char>(d_streamBuf->sbumpc());
    T(variable).d_bytes[1] = static_cast<char>(d_streamBuf->sbumpc());

    const int_type result = d_streamBuf->sbumpc();
    if (traits::eof() == result) {
        d_valid = false;
    }
    T(variable).d_bytes[0] = static_cast<char>(result);
#else
    if (d_streamBuf->sgetn(
                         &(T(variable).d_bytes[sizeof variable - 7]), 7) < 7) {
        d_valid = false;
    }
#endif

    return *this;
}

template <class StreamBuf>
#if BSLS_PLATFORM_IS_BIG_ENDIAN
inline
#endif
bdex_GenericByteInStream<StreamBuf>&
bdex_GenericByteInStream<StreamBuf>::getInt48(bsls_Types::Int64& variable)
{
    typedef union {
        bsls_Types::Int64 d_variable;
        char              d_bytes[sizeof(bsls_Types::Int64)];
    }& T;

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!d_valid)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    variable = 0x80 & d_streamBuf->sgetc() ? -1 : 0;  // sign extend

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    T(variable).d_bytes[5] = static_cast<char>(d_streamBuf->sbumpc());
    T(variable).d_bytes[4] = static_cast<char>(d_streamBuf->sbumpc());
    T(variable).d_bytes[3] = static_cast<char>(d_streamBuf->sbumpc());
    T(variable).d_bytes[2] = static_cast<char>(d_streamBuf->sbumpc());
    T(variable).d_bytes[1] = static_cast<char>(d_streamBuf->sbumpc());

    const int_type result = (int_type)d_streamBuf->sbumpc();
    if (traits::eof() == result) {
        d_valid = false;
    }
    T(variable).d_bytes[0] = static_cast<char>(result);
#else
    if (d_streamBuf->sgetn(
                         &(T(variable).d_bytes[sizeof variable - 6]), 6) < 6) {
        d_valid = false;
    }
#endif

    return *this;
}

template <class StreamBuf>
#if BSLS_PLATFORM_IS_BIG_ENDIAN
inline
#endif
bdex_GenericByteInStream<StreamBuf>&
bdex_GenericByteInStream<StreamBuf>::getUint48(bsls_Types::Uint64& variable)
{
    typedef union {
        bsls_Types::Uint64 d_variable;
        char               d_bytes[sizeof(bsls_Types::Uint64)];
    }& T;

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!d_valid)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    variable = 0;  // zero extend

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    T(variable).d_bytes[5] = static_cast<char>(d_streamBuf->sbumpc());
    T(variable).d_bytes[4] = static_cast<char>(d_streamBuf->sbumpc());
    T(variable).d_bytes[3] = static_cast<char>(d_streamBuf->sbumpc());
    T(variable).d_bytes[2] = static_cast<char>(d_streamBuf->sbumpc());
    T(variable).d_bytes[1] = static_cast<char>(d_streamBuf->sbumpc());

    const int_type result = d_streamBuf->sbumpc();
    if (traits::eof() == result) {
        d_valid = false;
    }
    T(variable).d_bytes[0] = static_cast<char>(result);
#else
    if (d_streamBuf->sgetn(
                         &(T(variable).d_bytes[sizeof variable - 6]), 6) < 6) {
        d_valid = false;
    }
#endif

    return *this;
}

template <class StreamBuf>
#if BSLS_PLATFORM_IS_BIG_ENDIAN
inline
#endif
bdex_GenericByteInStream<StreamBuf>&
bdex_GenericByteInStream<StreamBuf>::getInt40(bsls_Types::Int64& variable)
{
    typedef union {
        bsls_Types::Int64 d_variable;
        char              d_bytes[sizeof(bsls_Types::Int64)];
    }& T;

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!d_valid)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    variable = 0x80 & d_streamBuf->sgetc() ? -1 : 0;  // sign extend

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    T(variable).d_bytes[4] = static_cast<char>(d_streamBuf->sbumpc());
    T(variable).d_bytes[3] = static_cast<char>(d_streamBuf->sbumpc());
    T(variable).d_bytes[2] = static_cast<char>(d_streamBuf->sbumpc());
    T(variable).d_bytes[1] = static_cast<char>(d_streamBuf->sbumpc());

    const int_type result = d_streamBuf->sbumpc();
    if (traits::eof() == result) {
        d_valid = false;
    }
    T(variable).d_bytes[0] = static_cast<char>(result);
#else
    if (d_streamBuf->sgetn(
                         &(T(variable).d_bytes[sizeof variable - 5]), 5) < 5) {
        d_valid = false;
    }
#endif

    return *this;
}

template <class StreamBuf>
#if BSLS_PLATFORM_IS_BIG_ENDIAN
inline
#endif
bdex_GenericByteInStream<StreamBuf>&
bdex_GenericByteInStream<StreamBuf>::getUint40(bsls_Types::Uint64& variable)
{
    typedef union {
        bsls_Types::Uint64 d_variable;
        char               d_bytes[sizeof(bsls_Types::Uint64)];
    }& T;

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!d_valid)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    variable = 0;  // zero extend

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    T(variable).d_bytes[4] = static_cast<char>(d_streamBuf->sbumpc());
    T(variable).d_bytes[3] = static_cast<char>(d_streamBuf->sbumpc());
    T(variable).d_bytes[2] = static_cast<char>(d_streamBuf->sbumpc());
    T(variable).d_bytes[1] = static_cast<char>(d_streamBuf->sbumpc());

    const int_type result = d_streamBuf->sbumpc();
    if (traits::eof() == result) {
        d_valid = false;
    }
    T(variable).d_bytes[0] = static_cast<char>(result);
#else
    if (d_streamBuf->sgetn(
                         &(T(variable).d_bytes[sizeof variable - 5]), 5) < 5) {
        d_valid = false;
    }
#endif

    return *this;
}

template <class StreamBuf>
inline
bdex_GenericByteInStream<StreamBuf>&
bdex_GenericByteInStream<StreamBuf>::getInt32(int& variable)
{
    typedef union {
        int  d_variable;
        char d_bytes[sizeof(int)];
    }& T;

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!d_valid)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(d_streamBuf->sgetn(
                                          &(T(variable).d_bytes[0]), 4) < 4)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        d_valid = false;
    }
#ifdef BSLS_PLATFORM_IS_LITTLE_ENDIAN
    else {
        T(variable).d_variable =
                         BSLS_BYTEORDER_BE_U32_TO_HOST(T(variable).d_variable);
    }
#endif

    return *this;
}

template <class StreamBuf>
inline
bdex_GenericByteInStream<StreamBuf>&
bdex_GenericByteInStream<StreamBuf>::getUint32(unsigned int& variable)
{
    typedef union {
        int  d_variable;
        char d_bytes[sizeof(int)];
    }& T;

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!d_valid)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(d_streamBuf->sgetn(
                                          &(T(variable).d_bytes[0]), 4) < 4)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        d_valid = false;
    }
#ifdef BSLS_PLATFORM_IS_LITTLE_ENDIAN
    else {
        T(variable).d_variable =
                         BSLS_BYTEORDER_BE_U32_TO_HOST(T(variable).d_variable);
    }
#endif

    return *this;
}

template <class StreamBuf>
#if BSLS_PLATFORM_IS_BIG_ENDIAN
inline
#endif
bdex_GenericByteInStream<StreamBuf>&
bdex_GenericByteInStream<StreamBuf>::getInt24(int& variable)
{
    typedef union {
        int  d_variable;
        char d_bytes[sizeof(int)];
    }& T;

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!d_valid)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    variable = 0x80 & d_streamBuf->sgetc() ? -1 : 0;  // sign extend

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    T(variable).d_bytes[2] = static_cast<char>(d_streamBuf->sbumpc());
    T(variable).d_bytes[1] = static_cast<char>(d_streamBuf->sbumpc());

    const int_type result = d_streamBuf->sbumpc();
    if (traits::eof() == result) {
        d_valid = false;
    }
    T(variable).d_bytes[0] = static_cast<char>(result);
#else
    if (d_streamBuf->sgetn(
                         &(T(variable).d_bytes[sizeof variable - 3]), 3) < 3) {
        d_valid = false;
    }
#endif

    return *this;
}

template <class StreamBuf>
#if BSLS_PLATFORM_IS_BIG_ENDIAN
inline
#endif
bdex_GenericByteInStream<StreamBuf>&
bdex_GenericByteInStream<StreamBuf>::getUint24(unsigned int& variable)
{
    typedef union {
        int  d_variable;
        char d_bytes[sizeof(int)];
    }& T;

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!d_valid)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    variable = 0;  // zero extend

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    T(variable).d_bytes[2] = static_cast<char>(d_streamBuf->sbumpc());
    T(variable).d_bytes[1] = static_cast<char>(d_streamBuf->sbumpc());

    const int_type result = d_streamBuf->sbumpc();
    if (traits::eof() == result) {
        d_valid = false;
    }
    T(variable).d_bytes[0] = static_cast<char>(result);
#else
    if (d_streamBuf->sgetn(
                         &(T(variable).d_bytes[sizeof variable - 3]), 3) < 3) {
        d_valid = false;
    }
#endif

    return *this;
}

template <class StreamBuf>
inline
bdex_GenericByteInStream<StreamBuf>&
bdex_GenericByteInStream<StreamBuf>::getInt16(short& variable)
{
    typedef union {
        short d_variable;
        char  d_bytes[sizeof(short)];
    }& T;

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!d_valid)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(d_streamBuf->sgetn(
                                          &(T(variable).d_bytes[0]), 2) < 2)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        d_valid = false;
    }
#ifdef BSLS_PLATFORM_IS_LITTLE_ENDIAN
    else {
        T(variable).d_variable =
                         BSLS_BYTEORDER_BE_U16_TO_HOST(T(variable).d_variable);
    }
#endif

    return *this;
}

template <class StreamBuf>
inline
bdex_GenericByteInStream<StreamBuf>&
bdex_GenericByteInStream<StreamBuf>::getUint16(unsigned short& variable)
{
    typedef union {
        short d_variable;
        char  d_bytes[sizeof(short)];
    }& T;

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!d_valid)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(d_streamBuf->sgetn(
                                          &(T(variable).d_bytes[0]), 2) < 2)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        d_valid = false;
    }
#ifdef BSLS_PLATFORM_IS_LITTLE_ENDIAN
    else {
        T(variable).d_variable =
                         BSLS_BYTEORDER_BE_U16_TO_HOST(T(variable).d_variable);
    }
#endif

    return *this;
}

template <class StreamBuf>
inline
bdex_GenericByteInStream<StreamBuf>&
bdex_GenericByteInStream<StreamBuf>::getInt8(char& variable)
{
    typedef union {
        short d_variable;
        char  d_bytes[sizeof(short)];
    }& T;

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!d_valid)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const int_type result = d_streamBuf->sbumpc();
    if (traits::eof() == result) {
        d_valid = false;
    }

    T(variable).d_bytes[0] = static_cast<char>(result);

    return *this;
}

template <class StreamBuf>
inline
bdex_GenericByteInStream<StreamBuf>&
bdex_GenericByteInStream<StreamBuf>::getInt8(signed char& variable)
{
    getInt8((char &)variable);
    return *this;
}

template <class StreamBuf>
inline
bdex_GenericByteInStream<StreamBuf>&
bdex_GenericByteInStream<StreamBuf>::getUint8(char& variable)
{
    getInt8(variable);
    return *this;
}

///Getting Scalar Floating-Point Values
/// - - - - - - - - - - - - - - - - - -

template <class StreamBuf>
inline
bdex_GenericByteInStream<StreamBuf>&
bdex_GenericByteInStream<StreamBuf>::getFloat64(double& variable)
{
    typedef union {
        double             d_variable;
#ifdef BSLS_PLATFORM_IS_LITTLE_ENDIAN
        bsls_Types::Uint64 d_longlong;
#endif
        char               d_bytes[sizeof(double)];
    }& T;

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!d_valid)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(d_streamBuf->sgetn(
                                          &(T(variable).d_bytes[0]), 8) < 8)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        d_valid = false;
    }
#ifdef BSLS_PLATFORM_IS_LITTLE_ENDIAN
    else {
        T(variable).d_longlong =
                         BSLS_BYTEORDER_BE_U64_TO_HOST(T(variable).d_longlong);
    }
#endif

    return *this;
}

template <class StreamBuf>
inline
bdex_GenericByteInStream<StreamBuf>&
bdex_GenericByteInStream<StreamBuf>::getFloat32(float& variable)
{
    typedef union {
        float d_variable;
#ifdef BSLS_PLATFORM_IS_LITTLE_ENDIAN
        int   d_int;
#endif
        char  d_bytes[sizeof(float)];
    }& T;

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!d_valid)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(d_streamBuf->sgetn(
                                          &(T(variable).d_bytes[0]), 4) < 4)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        d_valid = false;
    }
#ifdef BSLS_PLATFORM_IS_LITTLE_ENDIAN
    else {
        T(variable).d_int = BSLS_BYTEORDER_BE_U32_TO_HOST(T(variable).d_int);
    }
#endif

    return *this;
}

///Getting Arrays of Integral Values
///- - - - - - - - - - - - - - - - -

template <class StreamBuf>
#if BSLS_PLATFORM_IS_BIG_ENDIAN
inline
#endif
bdex_GenericByteInStream<StreamBuf>&
bdex_GenericByteInStream<StreamBuf>::getArrayInt64(bsls_Types::Int64 *array,
                                                   int                count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!d_valid)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    const bsls_Types::Int64 *endArray = array + count;
    for (; array < endArray; ++array) {
        getInt64(*array);
    }
#else
    const int numBytes = static_cast<int>(sizeof(bsls_Types::Int64) * count);
    if (d_streamBuf->sgetn((char *)array, numBytes) < numBytes) {
        d_valid = false;
    }
#endif

    return *this;
}

template <class StreamBuf>
#if BSLS_PLATFORM_IS_BIG_ENDIAN
inline
#endif
bdex_GenericByteInStream<StreamBuf>&
bdex_GenericByteInStream<StreamBuf>::getArrayUint64(bsls_Types::Uint64 *array,
                                                    int                 count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!d_valid)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    const bsls_Types::Uint64 *endArray = array + count;
    for (; array < endArray; ++array) {
        getUint64(*array);
    }
#else
    const int numBytes = static_cast<int>(sizeof(bsls_Types::Int64) * count);
    if (d_streamBuf->sgetn((char *)array, numBytes) < numBytes) {
        d_valid = false;
    }
#endif

    return *this;
}

template <class StreamBuf>
bdex_GenericByteInStream<StreamBuf>&
bdex_GenericByteInStream<StreamBuf>::getArrayInt56(bsls_Types::Int64 *array,
                                                   int                count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!d_valid)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const bsls_Types::Int64 *endArray = array + count;
    for (; array < endArray; ++array) {
        getInt56(*array);
    }

    return *this;
}

template <class StreamBuf>
bdex_GenericByteInStream<StreamBuf>&
bdex_GenericByteInStream<StreamBuf>::getArrayUint56(bsls_Types::Uint64 *array,
                                                    int                 count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!d_valid)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const bsls_Types::Uint64 *endArray = array + count;
    for (; array < endArray; ++array) {
        getUint56(*array);
    }

    return *this;
}

template <class StreamBuf>
bdex_GenericByteInStream<StreamBuf>&
bdex_GenericByteInStream<StreamBuf>::getArrayInt48(bsls_Types::Int64 *array,
                                                   int                count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!d_valid)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const bsls_Types::Int64 *endArray = array + count;
    for (; array < endArray; ++array) {
        getInt48(*array);
    }

    return *this;
}

template <class StreamBuf>
bdex_GenericByteInStream<StreamBuf>&
bdex_GenericByteInStream<StreamBuf>::getArrayUint48(bsls_Types::Uint64 *array,
                                                    int                 count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!d_valid)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const bsls_Types::Uint64 *endArray = array + count;
    for (; array < endArray; ++array) {
        getUint48(*array);
    }

    return *this;
}

template <class StreamBuf>
bdex_GenericByteInStream<StreamBuf>&
bdex_GenericByteInStream<StreamBuf>::getArrayInt40(bsls_Types::Int64 *array,
                                                   int                count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!d_valid)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const bsls_Types::Int64 *endArray = array + count;
    for (; array < endArray; ++array) {
        getInt40(*array);
    }

    return *this;
}

template <class StreamBuf>
bdex_GenericByteInStream<StreamBuf>&
bdex_GenericByteInStream<StreamBuf>::getArrayUint40(bsls_Types::Uint64 *array,
                                                    int                 count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!d_valid)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const bsls_Types::Uint64 *endArray = array + count;
    for (; array < endArray; ++array) {
        getUint40(*array);
    }

    return *this;
}

template <class StreamBuf>
#if BSLS_PLATFORM_IS_BIG_ENDIAN
inline
#endif
bdex_GenericByteInStream<StreamBuf>&
bdex_GenericByteInStream<StreamBuf>::getArrayInt32(int *array, int count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!d_valid)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    const int *endArray = array + count;
    for (; array < endArray; ++array) {
        getInt32(*array);
    }
#else
    const int numBytes = (int)(sizeof(int) * count);
    if (d_streamBuf->sgetn((char *)array, numBytes) < numBytes) {
        d_valid = false;
    }
#endif

    return *this;
}

template <class StreamBuf>
#if BSLS_PLATFORM_IS_BIG_ENDIAN
inline
#endif
bdex_GenericByteInStream<StreamBuf>&
bdex_GenericByteInStream<StreamBuf>::getArrayUint32(unsigned int *array,
                                                    int           count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!d_valid)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    const unsigned int *endArray = array + count;
    for (; array < endArray; ++array) {
        getUint32(*array);
    }
#else
    const int numBytes = static_cast<int>(sizeof(int) * count);
    if (d_streamBuf->sgetn((char *)array, numBytes) < numBytes) {
        d_valid = false;
    }
#endif

    return *this;
}

template <class StreamBuf>
bdex_GenericByteInStream<StreamBuf>&
bdex_GenericByteInStream<StreamBuf>::getArrayInt24(int *array, int count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!d_valid)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const int *endArray = array + count;
    for (; array < endArray; ++array) {
        getInt24(*array);
    }

    return *this;
}

template <class StreamBuf>
bdex_GenericByteInStream<StreamBuf>&
bdex_GenericByteInStream<StreamBuf>::getArrayUint24(unsigned int *array,
                                                    int           count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!d_valid)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

    const unsigned int *endArray = array + count;
    for (; array < endArray; ++array) {
        getUint24(*array);
    }

    return *this;
}

template <class StreamBuf>
#if BSLS_PLATFORM_IS_BIG_ENDIAN
inline
#endif
bdex_GenericByteInStream<StreamBuf>&
bdex_GenericByteInStream<StreamBuf>::getArrayInt16(short *array, int count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!d_valid)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    const short *endArray = array + count;
    for (; array < endArray; ++array) {
        getInt16(*array);
    }
#else
    const int numBytes = static_cast<int>(sizeof(short) * count);
    if (d_streamBuf->sgetn((char *)array, numBytes) < numBytes) {
        d_valid = false;
    }
#endif

    return *this;
}

template <class StreamBuf>
#if BSLS_PLATFORM_IS_BIG_ENDIAN
inline
#endif
bdex_GenericByteInStream<StreamBuf>&
bdex_GenericByteInStream<StreamBuf>::getArrayUint16(unsigned short *array,
                                                    int             count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!d_valid)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    const unsigned short *endArray = array + count;
    for (; array < endArray; ++array) {
        getUint16(*array);
    }
#else
    const int numBytes = static_cast<int>(sizeof(short) * count);
    if (d_streamBuf->sgetn((char *)array, numBytes) < numBytes) {
        d_valid = false;
    }
#endif

    return *this;
}

template <class StreamBuf>
inline
bdex_GenericByteInStream<StreamBuf>&
bdex_GenericByteInStream<StreamBuf>::getArrayInt8(char *array, int count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    if (d_valid && d_streamBuf->sgetn((char *)array, count) < count) {
        d_valid = false;
    }

    return *this;
}

template <class StreamBuf>
inline
bdex_GenericByteInStream<StreamBuf>&
bdex_GenericByteInStream<StreamBuf>::getArrayInt8(signed char *array,
                                                  int          count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    getArrayInt8((char *)array, count);
    return *this;
}

template <class StreamBuf>
inline
bdex_GenericByteInStream<StreamBuf>&
bdex_GenericByteInStream<StreamBuf>::getArrayUint8(char *array, int count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    getArrayInt8(array, count);
    return *this;
}

template <class StreamBuf>
inline
bdex_GenericByteInStream<StreamBuf>&
bdex_GenericByteInStream<StreamBuf>::getArrayUint8(unsigned char *array,
                                                   int            count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    getArrayInt8((char *)array, count);
    return *this;
}

///Getting Arrays of Floating-Point Values
///- - - - - - - - - - - - - - - - - - - -

template <class StreamBuf>
#if BSLS_PLATFORM_IS_BIG_ENDIAN
inline
#endif
bdex_GenericByteInStream<StreamBuf>&
bdex_GenericByteInStream<StreamBuf>::getArrayFloat64(double *array, int count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!d_valid)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    const double *endArray = array + count;
    for (; array < endArray; ++array) {
        getFloat64(*array);
    }
#else
    const int numBytes = static_cast<int>(sizeof(double) * count);
    if (d_streamBuf->sgetn((char *)array, numBytes) < numBytes) {
        d_valid = false;
    }
#endif

    return *this;
}

template <class StreamBuf>
#if BSLS_PLATFORM_IS_BIG_ENDIAN
inline
#endif
bdex_GenericByteInStream<StreamBuf>&
bdex_GenericByteInStream<StreamBuf>::getArrayFloat32(float *array, int count)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!d_valid)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;                                                 // RETURN
    }

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    const float *endArray = array + count;
    for (; array < endArray; ++array) {
        getFloat32(*array);
    }
#else
    const int numBytes = static_cast<int>(sizeof(float) * count);
    if (d_streamBuf->sgetn((char *)array, numBytes) < numBytes) {
        d_valid = false;
    }
#endif

    return *this;
}

template <class StreamBuf>
inline
void bdex_GenericByteInStream<StreamBuf>::invalidate()
{
    d_valid = false;
}

// ACCESSORS
template <class StreamBuf>
inline
bdex_GenericByteInStream<StreamBuf>::operator const void *() const
{
    return d_valid ? this : 0;
}

// FREE OPERATORS
// TBD #ifndef BDE_OMIT_DEPRECATED

template <class StreamBuf, typename OBJECT>
inline
bdex_GenericByteInStream<StreamBuf>&
        operator>>(bdex_GenericByteInStream<StreamBuf>& stream, OBJECT& object)
{
    return bdex_InStreamMethodsUtil::streamInVersionAndObject(stream, object);
}

// TBD #endif  // BDE_OMIT_DEPRECATED

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
