// bdlxxxx_byteinstream.h                                             -*-C++-*-
#ifndef INCLUDED_BDLXXXX_BYTEINSTREAM
#define INCLUDED_BDLXXXX_BYTEINSTREAM

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Enable (un)externalization of fundamental types.
//
//@CLASSES:
//   bdlxxxx::ByteInStream: byte-array-based input stream for fundamental types
//
//@AUTHOR: Shao-wei Hung
//
//@SEE_ALSO: bdlxxxx_byteoutstream, bdlxxxx_byteoutstreamraw, bdlxxxx_byteinstreamraw
//
//@DESCRIPTION: This component conforms to the 'bdlxxxx_instream' (documentation
// only) "protocol", implementing a byte-array-based data stream object that
// provides platform-independent input methods ("unexternalization") on values,
// and arrays of values, of fundamental types, and on 'bsl::string'.
//
// This component is intended to be used in conjunction with the
// 'bdlxxxx_byteoutstream' externalization component.  Each input method of
// 'bdlxxxx::ByteInStream' reads either a value or a homogeneous array of values of
// a fundamental type, in a format that was written by the corresponding
// 'bdlxxxx::ByteOutStream' method.  'bdlxxxx::ByteInStream' can also read buffers
// written by 'bdlxxxx::ByteOutStreamRaw'; see the 'bdlxxxx_byteoutstreamraw'
// component documentation for more information on "raw" stream
// interoperability.  In general, the user of this component cannot rely on
// being able to read data that was written by any mechanism other than
// 'bdlxxxx::ByteOutStream' or 'bdlxxxx::ByteOutStreamRaw'.
//
// The supported types and required content are listed in the table below.  All
// of the fundamental types in the table may be input as scalar values or as
// homogeneous arrays.  'bsl::string' is input as a logical scalar string.
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
//      bsl::string       BDE STLport implementation of the STL string class
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
///EXTERNALIZATION AND VALUE SEMANTICS
///-----------------------------------
// The concept of "externalization" in this component is intentionally
// specialized to support streaming the *values* of entities atomically.  In
// particular, streaming an array of a specific length is considered to be an
// atomic operation.  It is therefore an error, for example, to stream out an
// array of 'int' of length three and then stream in those three 'int' values
// as three scalar 'int' variables.  Similarly, it is an error to stream out an
// array of length L and stream back in an array of length other than L.
//
///Overloaded Streaming Operators
///------------------------------
// This component, consistent with the 'bdex' package "design contract",
// declares an overloaded 'operator>>' templatized by an 'OBJECT' that is
// assumed to itself comply with the 'bdex' design contract.  The free operator
// has the following declaration.
//..
//  template <typename OBJECT>
//  inline
//  bdlxxxx::ByteInStream& operator>>(bdlxxxx::ByteInStream& stream, OBJECT& object);
//..
// This operator is implemented for 'OBJECT' consistent with the 'bdex'
// contract and specialized for each of the fundamental and string types listed
// in the table above.
//
///Usage
///-----
// Suppose we wish to implement a (deliberately simple) 'my_Person' class (in
// the currently installed 'my_person' component) as a value-semantic object
// that supports 'bdex' externalization.  In addition to whatever data and
// methods that we choose to put into our design, we must supply three methods
// having specific names and signatures in order to comply with the 'bdex'
// "protocol": a class method 'maxSupportedBdexVersion' and two object
// methods, an accessor (i.e., a 'const' method) 'bdexStreamOut', and a
// manipulator (i.e., a non-'const' method) 'bdlxxxx::StreamIn'.  This example
// shows how to implement those three methods for the simple "person"
// component.  Note that, so long as the above three methods are provided,
// there is no need for the author of the value-semantic component to declare
// or implement overloaded 'operator<<' and 'operator>>'; these operators are
// provided by each 'bdex' stream as appropriate, templatized by an assumed
// 'bdex' compliant value-semantic 'OBJECT' type.
//
// In this example we will not worry overly about "good design" of the person
// component, and we will declare but not implement illustrative methods and
// free operators, except for the three required 'bdex' methods, which are
// implemented in full.  In particular, we will not make explicit use of
// 'bdema' allocators; a more complete design would do so.
//..
// // my_person.h
//
// class my_Person {
//     bsl::string d_firstName;
//     bsl::string d_lastName;
//     int         d_age;
//
//     friend bool operator==(const my_Person&, const my_Person&);
//
//   public:
//     // CLASS METHODS
//     static int maxSupportedBdexVersion(void) { return 1; }
//         // Return the most current 'bdex' streaming version number supported
//         // by this class.  (See the 'bdex' package-level documentation for
//         // more information on 'bdex' streaming of container types.)
//
//     // CREATORS
//     my_Person();
//         // Create a default person.
//
//     my_Person(const char *firstName, const char *lastName, int age);
//         // Create a person having the specified 'firstName', 'lastName', and
//         // 'age'.
//
//     my_Person(const my_Person& original);
//         // Create a person having value of the specified 'original' person.
//
//     ~my_Person();
//         // Destroy this object.
//
//     // MANIPULATORS
//     my_Person& operator=(const my_Person& rhs);
//         // Assign to this person the value of the specified 'rhs' person.
//
//     template <class STREAM>
//     STREAM& bdexStreamIn(STREAM& stream, int version);
//         // Assign to this object the value read from the specified input
//         // 'stream' using the specified 'version' format and return a
//         // reference to the modifiable 'stream'.  If 'stream' is initially
//         // invalid, this operation has no effect.  If 'stream' becomes
//         // invalid during this operation, this object is valid, but its
//         // value is undefined.  If the specified 'version' is not supported,
//         // 'stream' is marked invalid, but this object is unaltered.  Note
//         // that no version is read from 'stream'.  (See the 'bdex'
//         // package-level documentation for more information on 'bdex'
//         // streaming of container types.)
//
//     // Other manipulators omitted.
//
//     // ACCESSORS
//     const bsl::string& firstName() const;
//         // Return the first name of this person.
//
//     const bsl::string& lastName() const;
//         // Return the last name of this person.
//
//     int age() const;
//         // Return the age of this person.
//
//     template <class STREAM>
//     STREAM& bdexStreamOut(STREAM& stream, int version) const;
//         // Write this value to the specified output 'stream' and return a
//         // reference to the modifiable 'stream'.  Optionally specify an
//         // explicit 'version' format; by default, the maximum supported
//         // version is written to 'stream' and used as the format.  If
//         // 'version' is specified, that format is used but *not* written to
//         // 'stream'.  If 'version' is not supported, 'stream' is left
//         // unmodified.  (See the 'bdex' package-level documentation for more
//         // information on 'bdex' streaming of container types).
//
//     // Other accessors omitted.
//
// };
//
// // FREE OPERATORS
// bool operator==(const my_Person& lhs, const my_Person& rhs);
//     // Return 'true' if the specified 'lhs' and 'rhs' person objects have
//     // the same value and 'false' otherwise.  Two person objects have the
//     // same value if they have the same first name, last name, and age.
//
// bool operator!=(const my_Person& lhs, const my_Person& rhs);
//     // Return 'true' if the specified 'lhs' and 'rhs' person objects do not
//     // have the same value and 'false' otherwise.  Two person objects differ
//     // in value if they differ in first name, last name, or age.
//
// bsl::ostream& operator<<(bsl::ostream& stream, const my_Person& person);
//     // Write the specified 'date' value to the specified output 'stream' in
//     // some reasonable format.
//
//                          // INLINE FUNCTION DEFINITIONS
// template <class STREAM>
// inline
// STREAM& my_Person::bdexStreamIn(STREAM& stream, int version)
// {
//     if (stream) {
//         switch (version) {    // switch on the 'bdex' version
//           case 1: {
//               stream.getString(d_firstName);
//               if (!stream) {
//                   d_firstName = "stream error";  // *might* be corrupted;
//                                                  //  value for testing
//                   return stream;
//               }
//               stream.getString(d_lastName);
//               if (!stream) {
//                   d_lastName = "stream error";  // *might* be corrupted;
//                                                 //  value for testing
//                   return stream;
//               }
//               stream.getInt32(d_age);
//               if (!stream) {
//                   d_age = 1;      // *might* be corrupted; value for testing
//                   return stream;
//               }
//           } break;
//           default: {
//             stream.invalidate();
//           }
//         }
//     }
//     return stream;
// }
//
// // ACCESSORS
// template <class STREAM>
// inline
// STREAM& my_Person::bdexStreamOut(STREAM& stream, int version) const
// {
//     switch (version) {
//       case 1: {
//           stream.putString(d_firstName);
//           stream.putString(d_lastName);
//           stream.putInt32(d_age);
//       } break;
//     }
//     return stream;
// }
//..
// We can now exercise the new 'my_person' value-semantic component by
// externalizing and reconstituting an object.  First create a 'my_Person'
// 'JaneSmith' and a 'bdlxxxx::ByteOutStream' 'outStream'.
//..
// // my_testapp.m.cpp
//
// using namespace std;
//
// int main(int argc, char *argv[])
// {
//     my_Person JaneSmith("Jane", "Smith", 42);
//     bdlxxxx::ByteOutStream outStream;
//     outStream << JaneSmith;
//..
// Next create a 'my_Person' 'janeCopy' initialized to the default value, and
// assert that 'janeCopy' is different from 'janeSmith'.
//..
//     my_Person janeCopy;                       assert(janeCopy != JaneSmith);
//..
// Now create a 'bdlxxxx::ByteInStream' 'inStream' initialized with the buffer from
// the 'bdlxxxx::ByteOutStream' object 'outStream'.
//..
//     bdlxxxx::ByteInStream inStream(outStream.data(), outStream.length());
//     inStream >> janeCopy;                     assert(janeCopy == JaneSmith);
//
//     // Verify the results on 'stdout'.
//     if (janeCopy == JaneSmith) {
//         cout << "Successfully serialized and de-serialized Jane Smith:"
//              << "\n\tFirstName: " << janeCopy.firstName()
//              << "\n\tLastName : " << janeCopy.lastName()
//              << "\n\tAge      : " << janeCopy.age() << endl;
//     }
//     else {
//         cout << "Serialization unsuccessful.  janeCopy holds:"
//              << "\n\tFirstName: " << janeCopy.firstName()
//              << "\n\tLastName : " << janeCopy.lastName()
//              << "\n\tAge      : " << janeCopy.age() << endl;
//     }
//     return 0;
// }
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLXXXX_BYTEINSTREAMFORMATTER
#include <bdlxxxx_byteinstreamformatter.h>
#endif

#ifndef INCLUDED_BDLXXXX_INSTREAMMETHODS
#include <bdlxxxx_instreammethods.h>
#endif

#ifndef INCLUDED_BDLSB_FIXEDMEMINSTREAMBUF
#include <bdlsb_fixedmeminstreambuf.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_CSTRING
#include <bsl_cstring.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif



namespace BloombergLP {

namespace bdlxxxx {
                        // =======================
                        // class ByteInStream
                        // =======================

class ByteInStream {
    // This class provides input methods to unexternalize values, and C-style
    // arrays of values, of the fundamental integral and floating-point types,
    // as well as 'bsl::string' values, using a byte format documented in the
    // 'bdlxxxx_byteoutstream' component.  In particular, each 'get' method of
    // this class is guaranteed to read stream data written by the
    // corresponding 'put' method of 'ByteOutStream'.  Note that
    // attempting to read beyond the end of a stream will automatically
    // invalidate the stream.  The class supports validity testing by providing
    // the method 'operator const void *()'.

    // PRIVATE TYPES
    enum {
        // CPU-independent size of data types in wire format.  (Might not be
        // the same as size of data types in memory.)
        SIZEOF_INT64   = 8,
        SIZEOF_INT56   = 7,
        SIZEOF_INT48   = 6,
        SIZEOF_INT40   = 5,
        SIZEOF_INT32   = 4,
        SIZEOF_INT24   = 3,
        SIZEOF_INT16   = 2,
        SIZEOF_INT8    = 1,
        SIZEOF_FLOAT64 = 8,
        SIZEOF_FLOAT32 = 4
    };

    // IMPORTANT: d_allocator_p, d_buffer_p, d_streambuf and d_formatter
    // must all be initialized in the order declared below.  Don't
    // re-arrange these declarations!
    // DATA
    bslma::Allocator           *d_allocator_p;  // holds memory allocator
    int                         d_length;       // length of 'd_buffer_p'
    char                       *d_buffer_p;     // bytes to be unexternalized
    bdlsb::FixedMemInStreamBuf   d_streambuf;
    ByteInStreamFormatter  d_formatter;

    // FRIENDS
    friend bsl::ostream& operator<<(bsl::ostream&            stream,
                                    const ByteInStream& object);

    // NOT DEFINED
    ByteInStream(const ByteInStream&);
    ByteInStream& operator=(const ByteInStream&);

    bool operator==(const ByteInStream&) const;
    bool operator!=(const ByteInStream&) const;

  private:
    // PRIVATE CLASS METHODS
    static char *cloneBuffer(const char       *buffer,
                             int               numBytes,
                             bslma::Allocator *basicAllocator);
        // Allocate 'numBytes' from 'basicAllocator' and copy the contents of
        // 'buffer' into the newly allocated memory.  Return the new memory.

  public:
    // CREATORS
    ByteInStream(bslma::Allocator *basicAllocator = 0);
        // Create an empty input byte stream.  Optionally specify the
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    ByteInStream(const char       *buffer,
                      int               numBytes,
                      bslma::Allocator *basicAllocator = 0);
        // Create an input byte stream containing the specified initial
        // 'numBytes' from the specified 'buffer'.  Optionally specify the
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  The behavior is
        // undefined unless 0 <= numBytes, 'numBytes' is less than or equal to
        // the length of 'buffer', and 'buffer' contains data written in a
        // format consistent with the 'bdlxxxx_byteoutstream' component.

    ~ByteInStream();
        // Destroy this object.

    // MANIPULATORS
    ByteInStream& getLength(int& length);
        // Consume a length value from this input stream, place that value in
        // the specified 'length', and return a reference to this modifiable
        // stream.  If this stream is initially invalid, this operation has no
        // effect.  If this function otherwise fails to extract a valid value,
        // this stream is marked invalid and the value of 'length' is
        // undefined.
        //
        // Consume one byte.  If the most significant bit (MSB) is set to 0,
        // then set 'length' to that one-byte integer value; otherwise, set
        // that MSB to zero, consume three more bytes, and interpret the four
        // bytes in network byte order to provide the value for 'length'.

    ByteInStream& getVersion(int& version);
        // Consume a version value from this input stream, place that value in
        // the specified 'version', and return a reference to this modifiable
        // stream.  If this stream is initially invalid, this operation has no
        // effect.  If this function otherwise fails to extract a valid value,
        // this stream is marked invalid and the value of 'version' is
        // undefined.
        //
        // Consume one byte interpreted as an unsigned integer to provide the
        // value for 'version'.

// TBD #ifndef BDE_OMIT_DEPRECATED

    template <class TYPE>
    ByteInStream& get(TYPE& variable);
        // Consume a 'TYPE' value from this input stream, place that value in
        // the specified 'variable', and return a reference to this stream.  If
        // this stream is initially invalid, this operation has no effect.  If
        // this function, otherwise, fails to extract a valid value, this
        // stream is marked invalid and the value of 'variable' is undefined.
        // If the version read from this stream is not supported, this stream
        // is marked invalid and 'variable' is unaltered.  Note that the
        // version is read from this stream only if 'TYPE' supports a valid
        // version.  If 'bdexStreamIn' is specialized for  'TYPE' then that
        // function is called.  If not,
        // 'bdex_InStreamFunctions::streamInVersionAndObject' is called.

// TBD #endif  // BDE_OMIT_DEPRECATED

                        // *** scalar integer values ***

    ByteInStream& getInt64(bsls::Types::Int64& variable);
        // Consume a 64-bit signed integer value from this input stream, place
        // that value in the specified 'variable', and return a reference to
        // this modifiable stream.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.
        //
        // Consume eight bytes interpreted as a 64-bit signed integer in
        // network byte order to provide the value for 'variable'.  Note that
        // the bit pattern is sign extended on platforms where 'variable' is
        // more than 64 bits.

    ByteInStream& getUint64(bsls::Types::Uint64& variable);
        // Consume a 64-bit unsigned integer value from this input stream,
        // place that value in the specified 'variable', and return a reference
        // to this modifiable stream.  If this stream is initially invalid,
        // this operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.
        //
        // Consume eight bytes interpreted as a 64-bit unsigned integer in
        // network byte order to provide the value for 'variable'.  Note that
        // the bit pattern is zero extended on platforms where 'variable' is
        // more than 64 bits.

    ByteInStream& getInt56(bsls::Types::Int64& variable);
        // Consume a 56-bit signed integer value from this input stream, place
        // that value in the specified 'variable', and return a reference to
        // this modifiable stream.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.
        //
        // Consume seven bytes interpreted as a sign extended 64-bit signed
        // integer in network byte order to provide the value for 'variable'.

    ByteInStream& getUint56(bsls::Types::Uint64& variable);
        // Consume a 56-bit unsigned integer value from this input stream,
        // place that value in the specified 'variable', and return a reference
        // to this modifiable stream.  If this stream is initially invalid,
        // this operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.
        //
        // Consume seven bytes interpreted as a zero extended 64-bit unsigned
        // integer in network byte order to provide the value for 'variable'.

    ByteInStream& getInt48(bsls::Types::Int64& variable);
        // Consume a 48-bit signed integer value from this input stream, place
        // that value in the specified 'variable', and return a reference to
        // this modifiable stream.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.
        //
        // Consume six bytes interpreted as a sign extended 64-bit signed
        // integer in network byte order to provide the value for 'variable'.

    ByteInStream& getUint48(bsls::Types::Uint64& variable);
        // Consume a 48-bit unsigned integer value from this input stream,
        // place that value in the specified 'variable', and return a reference
        // to this modifiable stream.  If this stream is initially invalid,
        // this operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.
        //
        // Consume six bytes interpreted as a zero extended 64-bit unsigned
        // integer in network byte order to provide the value for 'variable'.

    ByteInStream& getInt40(bsls::Types::Int64& variable);
        // Consume a 40-bit signed integer value from this input stream, place
        // that value in the specified 'variable', and return a reference to
        // this modifiable stream.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.
        //
        // Consume five bytes interpreted as a sign extended 64-bit signed
        // integer in network byte order to provide the value for 'variable'.

    ByteInStream& getUint40(bsls::Types::Uint64& variable);
        // Consume a 40-bit unsigned integer value from this input stream,
        // place that value in the specified 'variable', and return a reference
        // to this modifiable stream.  If this stream is initially invalid,
        // this operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.
        //
        // Consume five bytes interpreted as a zero extended 64-bit unsigned
        // integer in network byte order to provide the value for 'variable'.

    ByteInStream& getInt32(int& variable);
        // Consume a 32-bit signed integer value from this input stream, place
        // that value in the specified 'variable', and return a reference to
        // this modifiable stream.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.
        //
        // Consume four bytes interpreted as a signed 32-bit integer in network
        // byte order to provide the value for 'variable'.  Note that the bit
        // pattern is sign extended on platforms where 'variable' is more than
        // 32 bits.

    ByteInStream& getUint32(unsigned int& variable);
        // Consume a 32-bit unsigned integer value from this input stream,
        // place that value in the specified 'variable', and return a reference
        // to this modifiable stream.  If this stream is initially invalid,
        // this operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.
        //
        // Consume four bytes interpreted as a 32-bit unsigned integer in
        // network byte order to provide the value for 'variable'.  Note that
        // the bit pattern is zero extended on platforms where 'variable' is
        // more than 32 bits.

    ByteInStream& getInt24(int& variable);
        // Consume a 24-bit signed integer value from this input stream, place
        // that value in the specified 'variable', and return a reference to
        // this modifiable stream.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.
        //
        // Consume three bytes interpreted as a sign extended 32-bit signed
        // integer in network byte order to provide the value for 'variable'.

    ByteInStream& getUint24(unsigned int& variable);
        // Consume a 24-bit unsigned integer value from this input stream,
        // place that value in the specified 'variable', and return a reference
        // to this modifiable stream.  If this stream is initially invalid,
        // this operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.
        //
        // Consume three bytes interpreted as a zero extended 32-bit unsigned
        // integer in network byte order to provide the value for 'variable'.

    ByteInStream& getInt16(short& variable);
        // Consume a 16-bit signed integer value from this input stream, place
        // that value in the specified 'variable', and return a reference to
        // this modifiable stream.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.
        //
        // Consume two bytes interpreted as a 16-bit signed integer in network
        // byte order to provide the value for 'variable'.  Note that the bit
        // pattern is sign extended on platforms where 'variable' is more than
        // 16 bits.

    ByteInStream& getUint16(unsigned short& variable);
        // Consume a 16-bit unsigned integer value from this input stream,
        // place that value in the specified 'variable', and return a reference
        // to this modifiable stream.  If this stream is initially invalid,
        // this operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.
        //
        // Consume two bytes interpreted as a 16-bit unsigned integer in
        // network byte order to provide the value for 'variable'.  Note that
        // the bit pattern is zero extended on platforms where 'variable' is
        // more than 16 bits.

    ByteInStream& getInt8(char& variable);
    ByteInStream& getInt8(signed char& variable);
        // Consume an 8-bit signed integer value from this input stream, place
        // that value in the specified 'variable', and return a reference to
        // this modifiable stream.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        //
        // Consume one byte interpreted as an 8-bit signed integer to provide
        // the value for 'variable'.

    ByteInStream& getUint8(char& variable);
    ByteInStream& getUint8(unsigned char& variable);
        // Consume an 8-bit unsigned integer value from this input stream,
        // place that value in the specified 'variable', and return a reference
        // to this modifiable stream.  If this stream is initially invalid,
        // this operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.
        //
        // Consume one byte interpreted as an 8-bit unsigned integer to provide
        // the value for 'variable'.

                        // *** scalar floating-point values ***

    ByteInStream& getFloat64(double& variable);
        // Consume a 64-bit floating-point value from this input stream, place
        // that value in the specified 'variable', and return a reference to
        // this modifiable stream.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.  Note that, for non-conforming
        // platforms, this operation may be lossy.
        //
        // Consume eight bytes interpreted as an IEEE double-precision value in
        // network byte order to provide the value for 'variable'.

    ByteInStream& getFloat32(float& variable);
        // Consume a 32-bit floating-point value from this input stream, place
        // that value in the specified 'variable', and return a reference to
        // this modifiable stream.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.  Note that, for non-conforming
        // platforms, this operation may be lossy.
        //
        // Consume four bytes interpreted as an IEEE single-precision value in
        // network byte order to provide the value for 'variable'.

                        // *** string values ***

    ByteInStream& getString(bsl::string& stringValue);
        // Consume a string from this input stream, place that value in the
        // specified 'stringValue', and return a reference to this modifiable
        // stream.  If this stream is initially invalid, this operation has no
        // effect.  If this function otherwise fails to extract a valid value,
        // this stream is marked invalid and the value of 'stringValue' is
        // undefined.  The string must be prefaced by a non-negative integer
        // indicating the number of characters composing the string.  The
        // behavior is undefined if the length indicator is not greater than 0.
        //
        // First extract a length value.  Consume one byte and, if the most
        // significant bit (MSB) is set to 0, then that one-byte integer value
        // is the length value; otherwise, set that MSB to zero, consume three
        // more bytes, and interpret the four bytes in network byte order to
        // provide the length value.  Then, for each character of 'stringValue'
        // (of the extracted length) consume one byte providing the value for
        // that character.

                        // *** arrays of integer values ***

    ByteInStream& getArrayInt64(bsls::Types::Int64 *array, int length);
        // Consume the 64-bit signed integer array of the specified 'length'
        // from this input stream, place that value in the specified 'array',
        // and return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless 0 <= 'length'.
        //
        // For each of the 'length' elements of 'array', consume eight bytes
        // interpreted as a 64-bit signed integer in network byte order to
        // provide the value for that element.  Note that the bit pattern is
        // sign extended on platforms where 'bsls::Types::Int64' is more
        // than 64 bits.

    ByteInStream& getArrayUint64(bsls::Types::Uint64 *array, int length);
        // Consume the 64-bit unsigned integer array of the specified 'length'
        // from this input stream, place that value in the specified 'array',
        // and return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless 0 <= 'length'.
        //
        // For each of the 'length' elements of 'array', consume eight bytes
        // interpreted as a 64-bit unsigned integer in network byte order to
        // provide the value for that element.  Note that the bit pattern is
        // zero extended on platforms where 'bsls::Types::Uint64' is more
        // than 64 bits.

    ByteInStream& getArrayInt56(bsls::Types::Int64 *array, int length);
        // Consume the 56-bit signed integer array of the specified 'length'
        // from this input stream, place that value in the specified 'array',
        // and return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless 0 <= 'length'.
        //
        // For each of the 'length' elements of 'array', consume seven bytes
        // interpreted as a sign extended 64-bit signed integer in network byte
        // order to provide the value for that element.

    ByteInStream& getArrayUint56(bsls::Types::Uint64 *array, int length);
        // Consume the 56-bit unsigned integer array of the specified 'length'
        // from this input stream, place that value in the specified 'array',
        // and return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless 0 <= 'length'.
        //
        // For each of the 'length' elements of 'array', consume seven bytes
        // interpreted as a zero extended 64-bit unsigned integer in network
        // byte order to provide the value for that element.

    ByteInStream& getArrayInt48(bsls::Types::Int64 *array, int length);
        // Consume the 48-bit signed integer array of the specified 'length'
        // from this input stream, place that value in the specified 'array',
        // and return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless 0 <= 'length'.
        //
        // For each of the 'length' elements of 'array', consume six bytes
        // interpreted as a sign extended 64-bit signed integer in network byte
        // order to provide the value for that element.

    ByteInStream& getArrayUint48(bsls::Types::Uint64 *array, int length);
        // Consume the 48-bit unsigned integer array of the specified 'length'
        // from this input stream, place that value in the specified 'array',
        // and return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless 0 <= 'length'.
        //
        // For each of the 'length' elements of 'array', consume six bytes
        // interpreted as a zero extended 64-bit unsigned integer in network
        // byte order to provide the value for that element.

    ByteInStream& getArrayInt40(bsls::Types::Int64 *array, int length);
        // Consume the 40-bit signed integer array of the specified 'length'
        // from this input stream, place that value in the specified 'array',
        // and return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless 0 <= 'length'.
        //
        // For each of the 'length' elements of 'array', consume five bytes
        // interpreted as a sign extended 64-bit signed integer in network byte
        // order to provide the value for that element.

    ByteInStream& getArrayUint40(bsls::Types::Uint64 *array, int length);
        // Consume the 40-bit unsigned integer array of the specified 'length'
        // from this input stream, place that value in the specified 'array',
        // and return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless 0 <= 'length'.
        //
        // For each of the 'length' elements of 'array', consume five bytes
        // interpreted as a zero extended 64-bit unsigned integer in network
        // byte order to provide the value for that element.

    ByteInStream& getArrayInt32(int *array, int  length);
        // Consume the 32-bit signed integer array of the specified 'length'
        // from this input stream, place that value in the specified 'array',
        // and return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless 0 <= length.
        //
        // For each of the 'length' elements of 'array', consume four bytes
        // interpreted as a 32-bit signed integer in network byte order to
        // provide the value for that element.  Note that the bit pattern is
        // sign extended on platforms where 'int' is more than 32 bits.

    ByteInStream& getArrayUint32(unsigned int *array, int length);
        // Consume the 32-bit unsigned integer array of the specified 'length'
        // from this input stream, place that value in the specified 'array',
        // and return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless 0 <= length.
        //
        // For each of the 'length' elements of 'array', consume four bytes
        // interpreted as a 32-bit unsigned integer in network byte order to
        // provide the value for that element.  Note that the bit pattern is
        // zero extended on platforms where 'unsigned' 'int' is more than 32
        // bits.

    ByteInStream& getArrayInt24(int *array, int length);
        // Consume the 24-bit signed integer array of the specified 'length'
        // from this input stream, place that value in the specified 'array',
        // and return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless 0 <= 'length'.
        //
        // For each of the 'length' elements of 'array', consume three bytes
        // interpreted as a sign extended 32-bit signed integer in network byte
        // order to provide the value for that element.

    ByteInStream& getArrayUint24(unsigned int *array, int length);
        // Consume the 24-bit unsigned integer array of the specified 'length'
        // from this input stream, place that value in the specified 'array',
        // and return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless 0 <= 'length'.
        //
        // For each of the 'length' elements of 'array', consume three bytes
        // interpreted as a zero extended 32-bit unsigned integer in network
        // byte order to provide the value for that element.

    ByteInStream& getArrayInt16(short *array, int length);
        // Consume the 16-bit signed integer array of the specified 'length'
        // from this input stream, place that value in the specified 'array',
        // and return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless 0 <= length.
        //
        // For each of the 'length' elements of 'array', consume two bytes
        // interpreted as a 16-bit signed integer in network byte order to
        // provide the value for that element.  Note that the bit pattern is
        // sign extended on platforms where 'short' is more than 16 bits.

    ByteInStream& getArrayUint16(unsigned short *array, int length);
        // Consume the 16-bit unsigned integer array of the specified 'length'
        // from this input stream, place that value in the specified 'array',
        // and return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless 0 <= length.
        //
        // For each of the 'length' elements of 'array', consume two bytes
        // interpreted as a 16-bit unsigned integer in network byte order to
        // provide the value for that element.  Note that the bit pattern is
        // zero extended on platforms where 'unsigned' 'short' is more than 16
        // bits.

    ByteInStream& getArrayInt8(char *array, int length);
    ByteInStream& getArrayInt8(signed char *array, int length);
        // Consume the 8-bit signed integer array of the specified 'length'
        // from this input stream, place that value in the specified 'array',
        // and return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless 0 <= length.
        //
        // For each of the 'length' elements of 'array', consume one byte
        // interpreted as an 8-bit signed integer to provide the value for that
        // element.

    ByteInStream& getArrayUint8(char *array, int length);
    ByteInStream& getArrayUint8(unsigned char *array, int length);
        // Consume the 8-bit unsigned integer array of the specified 'length'
        // from this input stream, place that value in the specified 'array',
        // and return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless 0 <= length.
        //
        // For each of the 'length' elements of 'array', consume one byte
        // interpreted as an 8-bit unsigned integer to provide the value for
        // that element.

                        // *** arrays of floating-point values ***

    ByteInStream& getArrayFloat64(double *array, int count);
        //
        // Consume the IEEE double-precision (8-byte) floating-point array of
        // the specified 'length' from this input stream, place that value in
        // the specified 'array', and return a reference to this modifiable
        // stream.  If this stream is initially invalid, this operation has no
        // effect.  If this function otherwise fails to extract a valid array
        // of the required 'length', this stream is marked invalid and the
        // value of 'array' is undefined.  The behavior is undefined unless
        // 0 <= length.  Note that for non-conforming platforms, this operation
        // may be lossy.
        //
        // For each of the 'length' elements of 'array', consume eight bytes
        // interpreted as an IEEE double-precision value in network byte order
        // to provide the value for that element.

    ByteInStream& getArrayFloat32(float *array, int length);
        //
        // Consume the IEEE single-precision (4-byte) floating-point array of
        // the specified 'length' from this input stream, place that value in
        // the specified 'array', and return a reference to this modifiable
        // stream.  If this stream is initially invalid, this operation has no
        // effect.  If this function otherwise fails to extract a valid array
        // of the required 'length', this stream is marked invalid and the
        // value of 'array' is undefined.  The behavior is undefined unless
        // 0 <= length.  Note that for non-conforming platforms, this operation
        // may be lossy.
        //
        // For each of the 'length' elements of 'array', consume four bytes
        // interpreted as an IEEE single-precision value in network byte order
        // to provide the value for that element.

    void invalidate();
        // Put this input stream in an invalid state.  This function has no
        // effect if this stream is already invalid.  Note that this function
        // should be called whenever a value extracted from this stream is
        // determined to be invalid, inconsistent, or otherwise incorrect.

    // ACCESSORS
    operator const void *() const;
        // Return a non-zero value if this stream is valid, and 0 otherwise.
        // An invalid stream is a stream in which insufficient or invalid data
        // was detected during an extraction operation.  Note that an empty
        // stream will be valid unless an extraction attempt or explicit
        // invalidation causes it to be otherwise.

    const char *data() const;
        // Return the address of the contiguous, non-modifiable internal memory
        // buffer of this stream.  The address will remain valid as long as
        // this array is not destroyed or modified (i.e., the current capacity
        // is not exceeded).  The behavior of accessing elements outside the
        // range '[ data() - data() + (length() - 1) ]' is undefined.

    bool isEmpty() const;
        // Return 'true' if this stream is empty, and 'false' otherwise.  The
        // behavior is undefined unless this stream is valid (i.e., would
        // convert to a non-zero value via the 'operator const void *()'
        // member).  Note that this function enables higher-level components
        // to verify that, after successfully reading all expected data, no
        // data remains.

    int length() const;
        // Return the total number of bytes stored in this stream.

    int cursor() const;
        // Return the index of the next byte to be extracted from this stream.
        // The behavior is undefined unless this stream is valid.
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&            stream,
                         const ByteInStream& object);
    // Write the specified 'object' to the specified output 'stream' in some
    // reasonable (multi-line) format, and return a reference to 'stream'.

// TBD #ifndef BDE_OMIT_DEPRECATED

template <class OBJECT>
inline
ByteInStream& operator>>(ByteInStream& stream, OBJECT& object);
    // Read from the specified 'stream', using the 'getVersion' method of this
    // component, a 'bdex' version and assign to the specified 'bdex'-compliant
    // 'object' the value read from 'stream' using 'OBJECT's 'bdex' version
    // format as read from 'stream'.  Return a reference to the modifiable
    // 'stream'.  If 'stream' is initially invalid, this operation has no
    // effect.  If 'stream' becomes invalid during this operation, 'object' is
    // valid, but its value is undefined.  If the read in version is not
    // supported by 'object', 'stream' is marked invalid, but 'object' is
    // unaltered.  The behavior is undefined unless the data in 'stream' was
    // written by a 'ByteOutStream' object.  (See the 'bdex' package-level
    // documentation for more information on 'bdex' streaming of container
    // types.)  Note that specializations for the supported fundamental
    // (scalar) types and for 'bsl::string' are also provided, in which case
    // this operator calls the appropriate 'get' method from this component.

// TBD #endif  // BDE_OMIT_DEPRECATED

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

// PRIVATE CLASS METHODS
inline
char *ByteInStream::cloneBuffer(const char       *buffer,
                                     int               numBytes,
                                     bslma::Allocator *basicAllocator)
{
    BSLS_ASSERT_SAFE(0 <= numBytes);
    BSLS_ASSERT_SAFE(basicAllocator);

    char *clone = (char *)basicAllocator->allocate(numBytes);
    bsl::memcpy(clone, buffer, numBytes);
    return clone;
}

// CREATORS
inline
ByteInStream::ByteInStream(bslma::Allocator *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_length(0)
, d_buffer_p(0)
, d_streambuf(d_buffer_p, 0)
, d_formatter(&d_streambuf)
{
    // IMPORTANT: d_allocator_p, d_buffer_p, d_streambuf and d_formatter
    // must be all be initialized in the order shown here.  Don't
    // re-arrange their declarations in the class definition.
}

inline
ByteInStream::ByteInStream(const char       *buffer,
                                     int               numBytes,
                                     bslma::Allocator *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_length(numBytes)
, d_buffer_p(cloneBuffer(buffer, numBytes, d_allocator_p))
, d_streambuf(d_buffer_p, numBytes)
, d_formatter(&d_streambuf)
{
    // IMPORTANT: d_allocator_p, d_buffer_p, d_streambuf and d_formatter
    // must be all be initialized in the order shown here.  Don't
    // re-arrange their declarations in the class definition.

    BSLS_ASSERT_SAFE(0 <= numBytes);
}

inline
ByteInStream::~ByteInStream()
{
    if (d_buffer_p) {
        // Must disconnect 'd_buffer_p' from stream before deallocating.
        d_streambuf.pubsetbuf((char *)0, 0);
        d_allocator_p->deallocate(d_buffer_p);
    }
}

// MANIPULATORS
inline
void ByteInStream::invalidate()
{
    d_formatter.invalidate();
}

inline
ByteInStream& ByteInStream::getLength(int& variable)
{
    d_formatter.getLength(variable);
    return *this;
}

inline
ByteInStream& ByteInStream::getString(bsl::string& stringValue)
{
    d_formatter.getString(stringValue);
    return *this;
}

inline
ByteInStream& ByteInStream::getVersion(int& variable)
{
    d_formatter.getVersion(variable);
    return *this;
}

inline
ByteInStream&
ByteInStream::getInt64(bsls::Types::Int64& variable)
{
    d_formatter.getInt64(variable);
    return *this;
}

inline
ByteInStream&
ByteInStream::getUint64(bsls::Types::Uint64& variable)
{
    d_formatter.getUint64(variable);
    return *this;
}

inline
ByteInStream&
ByteInStream::getInt56(bsls::Types::Int64& variable)
{
    d_formatter.getInt56(variable);
    return *this;
}

inline
ByteInStream&
ByteInStream::getUint56(bsls::Types::Uint64& variable)
{
    d_formatter.getUint56(variable);
    return *this;
}

inline
ByteInStream&
ByteInStream::getInt48(bsls::Types::Int64& variable)
{
    d_formatter.getInt48(variable);
    return *this;
}

inline
ByteInStream&
ByteInStream::getUint48(bsls::Types::Uint64& variable)
{
    d_formatter.getUint48(variable);
    return *this;
}

inline
ByteInStream&
ByteInStream::getInt40(bsls::Types::Int64& variable)
{
    d_formatter.getInt40(variable);
    return *this;
}

inline
ByteInStream&
ByteInStream::getUint40(bsls::Types::Uint64& variable)
{
    d_formatter.getUint40(variable);
    return *this;
}

inline
ByteInStream& ByteInStream::getInt32(int& variable)
{
    d_formatter.getInt32(variable);
    return *this;
}

inline
ByteInStream& ByteInStream::getUint32(unsigned int& variable)
{
    d_formatter.getUint32(variable);
    return *this;
}

inline
ByteInStream& ByteInStream::getInt24(int& variable)
{
    d_formatter.getInt24(variable);
    return *this;
}

inline
ByteInStream& ByteInStream::getUint24(unsigned int& variable)
{
    d_formatter.getUint24(variable);
    return *this;
}

inline
ByteInStream& ByteInStream::getInt16(short& variable)
{
    d_formatter.getInt16(variable);
    return *this;
}

inline
ByteInStream& ByteInStream::getUint16(unsigned short& variable)
{
    d_formatter.getUint16(variable);
    return *this;
}

inline
ByteInStream& ByteInStream::getInt8(char& variable)
{
    d_formatter.getInt8(variable);
    return *this;
}

inline
ByteInStream& ByteInStream::getInt8(signed char& variable)
{
    d_formatter.getInt8(variable);
    return *this;
}

inline
ByteInStream& ByteInStream::getUint8(char& variable)
{
    d_formatter.getUint8(variable);
    return *this;
}

inline
ByteInStream& ByteInStream::getUint8(unsigned char& variable)
{
    d_formatter.getUint8(variable);
    return *this;
}

inline
ByteInStream& ByteInStream::getFloat64(double& variable)
{
    d_formatter.getFloat64(variable);
    return *this;
}

inline
ByteInStream& ByteInStream::getFloat32(float& variable)
{
    d_formatter.getFloat32(variable);
    return *this;
}

inline
ByteInStream&
ByteInStream::getArrayInt64(bsls::Types::Int64 *array, int numValues)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= numValues);

    d_formatter.getArrayInt64(array, numValues);
    return *this;
}

inline
ByteInStream&
ByteInStream::getArrayUint64(bsls::Types::Uint64 *array, int numValues)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= numValues);

    d_formatter.getArrayUint64(array, numValues);
    return *this;
}

inline
ByteInStream&
ByteInStream::getArrayInt56(bsls::Types::Int64 *array, int numValues)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= numValues);

    d_formatter.getArrayInt56(array, numValues);
    return *this;
}

inline
ByteInStream&
ByteInStream::getArrayUint56(bsls::Types::Uint64 *array, int numValues)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= numValues);

    d_formatter.getArrayUint56(array, numValues);
    return *this;
}

inline
ByteInStream&
ByteInStream::getArrayInt48(bsls::Types::Int64 *array, int numValues)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= numValues);

    d_formatter.getArrayInt48(array, numValues);
    return *this;
}

inline
ByteInStream&
ByteInStream::getArrayUint48(bsls::Types::Uint64 *array, int numValues)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= numValues);

    d_formatter.getArrayUint48(array, numValues);
    return *this;
}

inline
ByteInStream&
ByteInStream::getArrayInt40(bsls::Types::Int64 *array, int numValues)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= numValues);

    d_formatter.getArrayInt40(array, numValues);
    return *this;
}

inline
ByteInStream&
ByteInStream::getArrayUint40(bsls::Types::Uint64 *array, int numValues)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= numValues);

    d_formatter.getArrayUint40(array, numValues);
    return *this;
}

inline
ByteInStream&
ByteInStream::getArrayInt32(int *array, int numValues)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= numValues);

    d_formatter.getArrayInt32(array, numValues);
    return *this;
}

inline
ByteInStream&
ByteInStream::getArrayUint32(unsigned int *array, int numValues)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= numValues);

    d_formatter.getArrayUint32(array, numValues);
    return *this;
}

inline
ByteInStream&
ByteInStream::getArrayInt24(int *array, int numValues)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= numValues);

    d_formatter.getArrayInt24(array, numValues);
    return *this;
}

inline
ByteInStream&
ByteInStream::getArrayUint24(unsigned int *array, int numValues)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= numValues);

    d_formatter.getArrayUint24(array, numValues);
    return *this;
}

inline
ByteInStream&
ByteInStream::getArrayInt16(short *array, int numValues)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= numValues);

    d_formatter.getArrayInt16(array, numValues);
    return *this;
}

inline
ByteInStream&
ByteInStream::getArrayUint16(unsigned short *array, int numValues)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= numValues);

    d_formatter.getArrayUint16(array, numValues);
    return *this;
}

inline
ByteInStream&
ByteInStream::getArrayInt8(char *array, int numValues)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= numValues);

    d_formatter.getArrayInt8(array, numValues);
    return *this;
}

inline
ByteInStream&
ByteInStream::getArrayInt8(signed char *array, int numValues)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= numValues);

    d_formatter.getArrayInt8(array, numValues);
    return *this;
}

inline
ByteInStream&
ByteInStream::getArrayUint8(char *array, int numValues)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= numValues);

    d_formatter.getArrayUint8(array, numValues);
    return *this;
}

inline
ByteInStream&
ByteInStream::getArrayUint8(unsigned char *array, int numValues)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= numValues);

    d_formatter.getArrayUint8(array, numValues);
    return *this;
}

inline
ByteInStream&
ByteInStream::getArrayFloat64(double *array, int numValues)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= numValues);

    d_formatter.getArrayFloat64(array, numValues);
    return *this;
}

inline
ByteInStream& ByteInStream::getArrayFloat32(float *array,
                                                      int    numValues)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= numValues);

    d_formatter.getArrayFloat32(array, numValues);
    return *this;
}

// TBD #ifndef BDE_OMIT_DEPRECATED

template <class TYPE>
inline
ByteInStream& ByteInStream::get(TYPE& variable)
{
    return InStreamMethodsUtil::streamInVersionAndObject(*this, variable);
}
}  // close package namespace

// TBD #endif  // BDE_OMIT_DEPRECATED

// ACCESSORS
inline
bdlxxxx::ByteInStream::operator const void *() const
{
    return d_formatter ? this : 0;
}

namespace bdlxxxx {
inline
const char *ByteInStream::data() const
{
    return d_buffer_p;
}

inline
bool ByteInStream::isEmpty() const
{
    return 0 == d_streambuf.length();
}

inline
int ByteInStream::length() const
{
    return d_length;
}

inline
int ByteInStream::cursor() const
{
    return d_length - static_cast<int>(d_streambuf.length());
}
}  // close package namespace

// FREE OPERATORS

// TBD #ifndef BDE_OMIT_DEPRECATED

template <class OBJECT>
inline
bdlxxxx::ByteInStream& bdlxxxx::operator>>(ByteInStream& stream, OBJECT& object)
{
    return stream.get(object);
}

// TBD #endif  // BDE_OMIT_DEPRECATED

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
