// bdex_byteinstreamraw.h                                             -*-C++-*-
#ifndef INCLUDED_BDEX_BYTEINSTREAMRAW
#define INCLUDED_BDEX_BYTEINSTREAMRAW

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Enable (un)externalization of fundamental types.
//
//@CLASSES:
//   bdex_ByteInStreamRaw: byte-array-based input stream for fundamental types
//
//@AUTHOR: Shao-wei Hung
//
//@SEE_ALSO: bdex_byteoutstreamraw, bdex_byteoutstream
//
//@DESCRIPTION: This component conforms to the 'bdex_instream' (documentation
// only) "protocol", implementing a byte-array-based data stream object that
// provides platform-independent input methods ("unexternalization") on values,
// and arrays of values, of fundamental types, and on 'bsl::string'.
//
///Usage Warning
///-------------
// This "raw" component reads from a user-supplied buffer directly, with no
// data copying or assumption of ownership.  The user must therefore make sure
// that the lifetime and visibility of the buffer is sufficient to satisfy the
// needs of the raw input stream.
//
///Interoperability of Raw Streams
///-------------------------------
// Although the general design idea within 'bdex' is that in-streams and
// out-streams must be used in "matched pairs" (e.g., the output buffer of
// 'bdex_TestOutStream' can be read successfully only by 'bdex_TestInStream'),
// the "raw" streams ('bdex_ByteOutStreamRaw' and 'bdex_ByteInStreamRaw') and
// the "production" streams ('bdex_ByteOutStream' and 'bdex_ByteInStream') all
// use the same byte format and are designed to interoperate.
//
// The raw streams have in common that they do not manage or own the buffers
// that they use, but rather just copy pointers.  The ramifications of this
// design choice are rather different for input and output streams (see the
// specific Usage Warning section above), however, and therefore the user may
// wish to "mix and match" streams, depending upon that user's specific needs
// and risk tolerances.
//
// Specifically, this component may be used in conjunction with either the
// 'bdex_byteoutstreamraw' or the 'bdex_byteoutstream' externalization
// components, at the user's choice.  Each input method of
// 'bdex_ByteInStreamRaw' reads a value or a homogeneous array of values from a
// user-supplied buffer.  As long as the data was written by the corresponding
// output method of one of the above-mentioned output streams, the input method
// will read its consumed data successfully.  In general, the user cannot rely
// on any other mechanism to read data written by 'bdex_ByteOutStreamRaw'
// unless that mechanism explicitly states its ability to do so.
//
///Supported Types
///---------------
// The supported types and required content are listed in the table below.
// All of the fundamental types in the table may be input as scalar values or
// as homogeneous arrays.  'bsl::string' is input as a logical scalar string.
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
///- - - - - - - - - - - - - - - - - -
// The concept of "externalization" in this component is intentionally
// specialized to support streaming the *values* of entities atomically.  In
// particular, streaming an array of a specific length is considered to be an
// atomic operation.  It is therefore an error, for example, to stream out an
// array of 'int' of length three and then stream in those three 'int' values
// as three scalar 'int' variables.  Similarly, it is an error to stream out an
// array of length L and stream back in an array of length other than L.
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
// a non-'const' method) 'bdexStreamIn'.  This example shows how to implement
// those three methods for the simple "person" component.
//
// In this example we will not worry overly about "good design" of the person
// component, and we will declare but not implement illustrative methods and
// free operators, except for the three required 'bdex' methods, which are
// implemented in full.  In particular, we will not make explicit use of
// 'bdema' allocators; a more complete design would do so.  Also, we will use
// the two "raw" 'bdex' streams since that is the purpose of this example.  The
// use of the (dangerous) "raw" streams is not well-motivated by the scenario
// that we present, but we note that the performance is improved, since the
// output buffer is not dynamically managed by the output stream.  In general,
// the user of the raw output stream must make *very* sure that the supplied
// buffer is adequate for any possible usage that may occur at runtime.
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
// inline
// bool operator==(const my_Person& lhs, const my_Person& rhs);
//     // Return 'true' if the specified 'lhs' and 'rhs' person objects have
//     // the same value and 'false' otherwise.  Two person objects have the
//     // same value if they have the same first name, last name, and age.
//
// inline
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
// 'JaneSmith' and a 'bdex_ByteOutStreamRaw' 'outStream'.
//..
// // my_testapp.m.cpp
//
// using namespace std;
//
// int main(int argc, char *argv[])
// {
//     my_Person JaneSmith("Jane", "Smith", 42);
//     const int             BUFFER_SIZE = 10000;
//     char                  buffer[BUFFER_SIZE];
//     bdex_ByteOutStreamRaw outStream(buffer);
//     const int             VERSION = 1;
//     outStream.putVersion(VERSION);
//     bdex_OutStreamFunctions::streamOut(outStream, JaneSmith, VERSION);
//..
// Next create a 'my_Person' 'janeCopy' initialized to the default value, and
// assert that 'janeCopy' is different from 'janeSmith'.
//..
//     my_Person janeCopy;                       assert(janeCopy != JaneSmith);
//..
// Now create a 'bdex_ByteInStreamRaw' 'inStream' initialized with the
// user-supplied 'buffer' that was provided to the 'bdex_ByteOutStreamRaw'
// object 'outStream'.  Note that 'bdex_ByteOutStreamRaw' has 'data' and
// 'length' accessors, and so may be used exactly as 'bdex_ByteOutStream', but
// here we choose to illustrate the "raw" nature of the component by supplying
// the buffer and its length explicitly from our own knowledge.  In a more
// realistic example, we'd know the actual length of the buffer by some means
// (e.g., from the transport mechanism that brought the buffer into our
// process).  Our use of 'BUFFER_SIZE' here is to call attention to the fact
// that the stream is "raw", and not to suggest elegant usage.
//..
//     bdex_ByteInStreamRaw inStream(buffer, BUFFER_SIZE);
//     int version;
//     inStream.getVersion(version);
//     bdex_InStreamFunctions::streamIn(inStream, janeCopy, version);
//                                               assert(janeCopy == JaneSmith);
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

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEX_GENERICBYTEINSTREAM
#include <bdex_genericbyteinstream.h>
#endif

#ifndef INCLUDED_BDEX_INSTREAMMETHODS
#include <bdex_instreammethods.h>
#endif

#ifndef INCLUDED_BDESB_FIXEDMEMINPUT
#include <bdesb_fixedmeminput.h>
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

                        // ==========================
                        // class bdex_ByteInStreamRaw
                        // ==========================

class bdex_ByteInStreamRaw {
    // This class provides input methods to unexternalize values (and C-style
    // arrays of values) of fundamental types from their byte representations.
    // Note that attempting to read beyond the end of a stream will
    // automatically invalidate the stream.  The class supports validity
    // testing by providing the method 'operator const void *()'.

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

    // IMPORTANT: d_streambuf and d_formatter must be initialized in the
    // order declared below.  Don't re-arrange these declarations!
    // DATA
    int                                           d_size;
    bdesb_FixedMemInput                           d_streambuf; // externalized
                                                               // values
    bdex_GenericByteInStream<bdesb_FixedMemInput> d_formatter; // formatter

    friend bsl::ostream& operator<<(bsl::ostream&               stream,
                                    const bdex_ByteInStreamRaw& object);

    // NOT DEFINED
    bdex_ByteInStreamRaw(const bdex_ByteInStreamRaw&);
    bdex_ByteInStreamRaw& operator=(const bdex_ByteInStreamRaw&);

    bool operator==(const bdex_ByteInStreamRaw&) const;
    bool operator!=(const bdex_ByteInStreamRaw&) const;

  public:
    // CREATORS
    bdex_ByteInStreamRaw(const char *buffer, int numBytes);
        // Create an input byte stream containing the specified initial
        // 'numBytes' from the specified 'buffer'.  The behavior is undefined
        // unless 0 <= 'numBytes' and 'numBytes' is less than or equal
        // to the length of 'buffer'.

    ~bdex_ByteInStreamRaw();
        // Destroy this stream.

    // MANIPULATORS
    void reset(const char *buffer, int numBytes);
        // Reset this stream to extract from the specified 'buffer' containing
        // the specified 'numBytes'.

    void setCursor(int index);
        // Reposition the stream's cursor to the specified 'index'.

    bdex_ByteInStreamRaw& getLength(int& variable);
        // Consume either an 8-bit unsigned integer or a 32-bit signed integer
        // value representing a length (see the package group level
        // documentation) from this input stream, place it in the specified
        // 'variable', and return a reference to this modifiable stream
        // dependent on the most significant bit of the next byte in the
        // stream; if it is 0 consume an 8-bit unsigned integer, otherwise a
        // 32-bit signed integer after setting this bit to zero.  If this
        // stream is initially invalid, this operation has no effect.  If this
        // function otherwise fails to extract a valid value, this stream is
        // marked invalid and the value of 'variable' is undefined.

    bdex_ByteInStreamRaw& getString(bsl::string& str);
        // Consume a string from this input stream, place that value in the
        // specified 'str', and return a reference to this modifiable stream.
        // If this stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value, this
        // stream is marked invalid and the value of 'variable' is undefined.
        // The string must be prefaced by a non-negative integer indicating
        // the number of characters composing the string.  Behavior is
        // undefined if the length indicator is not greater than 0.

    bdex_ByteInStreamRaw& getVersion(int& variable);
        // Consume an 8-bit unsigned integer value representing a version (see
        // the package group level documentation) from this input stream, place
        // it in the specified 'variable', and return a reference to this
        // modifiable stream.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.

// TBD #if !defined(BSL_LEGACY) || 1 == BSL_LEGACY

    template <typename TYPE>
    bdex_ByteInStreamRaw& get(TYPE& variable);
        // TBD

// TBD #endif

                        // *** scalar integer values ***

    bdex_ByteInStreamRaw& getInt64(bsls_PlatformUtil::Int64& variable);
        // Consume a 64-bit signed integer value from this input stream, place
        // that value in the specified 'variable', and return a reference to
        // this modifiable stream.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.

    bdex_ByteInStreamRaw& getUint64(bsls_PlatformUtil::Uint64& variable);
        // Consume a 64-bit unsigned integer value from this input stream,
        // place that value in the specified 'variable', and return a reference
        // to this modifiable stream.  If this stream is initially invalid,
        // this operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.

    bdex_ByteInStreamRaw& getInt56(bsls_PlatformUtil::Int64& variable);
        // Consume a 56-bit signed integer value from this input stream, place
        // that value in the specified 'variable', and return a reference to
        // this modifiable stream.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.
        //
        // Note, that the value and the sign of the consumed integer value is
        // preserved.

    bdex_ByteInStreamRaw& getUint56(bsls_PlatformUtil::Uint64& variable);
        // Consume a 56-bit unsigned integer value from this input stream,
        // place that value in the specified 'variable', and return a reference
        // to this modifiable stream.  If this stream is initially invalid,
        // this operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.
        //
        // Note, that the value of the consumed integer value is preserved.

    bdex_ByteInStreamRaw& getInt48(bsls_PlatformUtil::Int64& variable);
        // Consume a 48-bit signed integer value from this input stream, place
        // that value in the specified 'variable', and return a reference to
        // this modifiable stream.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.
        //
        // Note, that the value and the sign of the consumed integer value is
        // preserved.

    bdex_ByteInStreamRaw& getUint48(bsls_PlatformUtil::Uint64& variable);
        // Consume a 48-bit unsigned integer value from this input stream,
        // place that value in the specified 'variable', and return a reference
        // to this modifiable stream.  If this stream is initially invalid,
        // this operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.
        //
        // Note, that the value of the consumed integer value is preserved.

    bdex_ByteInStreamRaw& getInt40(bsls_PlatformUtil::Int64& variable);
        // Consume a 40-bit signed integer value from this input stream, place
        // that value in the specified 'variable', and return a reference to
        // this modifiable stream.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.
        //
        // Note, that the value and the sign of the consumed integer value is
        // preserved.

    bdex_ByteInStreamRaw& getUint40(bsls_PlatformUtil::Uint64& variable);
        // Consume a 40-bit unsigned integer value from this input stream,
        // place that value in the specified 'variable', and return a reference
        // to this modifiable stream.  If this stream is initially invalid,
        // this operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.
        //
        // Note, that the value of the consumed integer value is preserved.

    bdex_ByteInStreamRaw& getInt32(int& variable);
        // Consume a 32-bit signed integer value from this input stream, place
        // that value in the specified 'variable', and return a reference to
        // this modifiable stream.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.

    bdex_ByteInStreamRaw& getUint32(unsigned int& variable);
        // Consume a 32-bit unsigned integer value from this input stream,
        // place that value in the specified 'variable', and return a reference
        // to this modifiable stream.  If this stream is initially invalid,
        // this operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.

    bdex_ByteInStreamRaw& getInt24(int& variable);
        // Consume a 24-bit signed integer value from this input stream, place
        // that value in the specified 'variable', and return a reference to
        // this modifiable stream.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.
        //
        // Note, that the value and the sign of the consumed integer value is
        // preserved.

    bdex_ByteInStreamRaw& getUint24(unsigned int& variable);
        // Consume a 24-bit unsigned integer value from this input stream,
        // place that value in the specified 'variable', and return a reference
        // to this modifiable stream.  If this stream is initially invalid,
        // this operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.
        //
        // Note, that the value of the consumed integer value is preserved.

    bdex_ByteInStreamRaw& getInt16(short& variable);
        // Consume a 16-bit signed integer value from this input stream, place
        // that value in the specified 'variable', and return a reference to
        // this modifiable stream.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.

    bdex_ByteInStreamRaw& getUint16(unsigned short& variable);
        // Consume a 16-bit unsigned integer value from this input stream,
        // place that value in the specified 'variable', and return a reference
        // to this modifiable stream.  If this stream is initially invalid,
        // this operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.

    bdex_ByteInStreamRaw& getInt8(char& variable);
    bdex_ByteInStreamRaw& getInt8(signed char& variable);
        // Consume an 8-bit signed integer value from this input stream, place
        // that value in the specified 'variable', and return a reference to
        // this modifiable stream.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the
        // value of 'variable' is undefined.

    bdex_ByteInStreamRaw& getUint8(char& variable);
    bdex_ByteInStreamRaw& getUint8(unsigned char& variable);
        // Consume an 8-bit unsigned integer value from this input stream,
        // place that value in the specified 'variable', and return a reference
        // to this modifiable stream.  If this stream is initially invalid,
        // this operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.

                        // *** scalar floating-point values ***

    bdex_ByteInStreamRaw& getFloat64(double& variable);
        // Consume an IEEE double-precision (8-byte) floating-point value from
        // this input stream, place that value in the specified 'variable', and
        // return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid value, this stream is marked
        // invalid and the value of 'variable' is undefined.  Note that for
        // non-conforming platforms, this operation may be lossy.

    bdex_ByteInStreamRaw& getFloat32(float& variable);
        // Consume an IEEE single-precision (4-byte) floating-point value from
        // this input stream, place that value in the specified 'variable', and
        // return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid value, this stream is marked
        // invalid and the value of 'variable' is undefined.  Note that for
        // non-conforming platforms, this operation may be lossy.

                        // *** arrays of integer values ***

    bdex_ByteInStreamRaw& getArrayInt64(bsls_PlatformUtil::Int64 *array,
                                        int                       length);
        // Consume a 64-bit signed integer array of the specified 'length'
        // from this input stream, place that value in the specified 'array',
        // and return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless '0 <= length'.

    bdex_ByteInStreamRaw& getArrayUint64(bsls_PlatformUtil::Uint64 *array,
                                         int                        length);
        // Consume a 64-bit unsigned integer array of the specified 'length'
        // from this input stream, place that value in the specified 'array',
        // and return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless '0 <= length'.

    bdex_ByteInStreamRaw& getArrayInt56(bsls_PlatformUtil::Int64 *array,
                                        int                       length);
        // Consume a 56-bit signed integer array of the specified 'length'
        // from this input stream, place that value in the specified 'array',
        // and return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless '0 <= length'.

    bdex_ByteInStreamRaw& getArrayUint56(bsls_PlatformUtil::Uint64 *array,
                                         int                        length);
        // Consume a 56-bit unsigned integer array of the specified 'length'
        // from this input stream, place that value in the specified 'array',
        // and return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless '0 <= length'.

    bdex_ByteInStreamRaw& getArrayInt48(bsls_PlatformUtil::Int64 *array,
                                        int                       length);
        // Consume a 48-bit signed integer array of the specified 'length'
        // from this input stream, place that value in the specified 'array',
        // and return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless '0 <= length'.

    bdex_ByteInStreamRaw& getArrayUint48(bsls_PlatformUtil::Uint64 *array,
                                         int                        length);
        // Consume a 48-bit unsigned integer array of the specified 'length'
        // from this input stream, place that value in the specified 'array',
        // and return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless '0 <= length'.

    bdex_ByteInStreamRaw& getArrayInt40(bsls_PlatformUtil::Int64 *array,
                                        int                       length);
        // Consume a 40-bit signed integer array of the specified 'length'
        // from this input stream, place that value in the specified 'array',
        // and return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless '0 <= length'.

    bdex_ByteInStreamRaw& getArrayUint40(bsls_PlatformUtil::Uint64 *array,
                                         int                        length);
        // Consume a 40-bit unsigned integer array of the specified 'length'
        // from this input stream, place that value in the specified 'array',
        // and return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless '0 <= length'.

    bdex_ByteInStreamRaw& getArrayInt32(int *array, int length);
        // Consume a 32-bit signed integer array of the specified 'length'
        // from this input stream, place that value in the specified 'array',
        // and return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless '0 <= length'.

    bdex_ByteInStreamRaw& getArrayUint32(unsigned int *array, int length);
        // Consume a 32-bit unsigned integer array of the specified 'length'
        // from this input stream, place that value in the specified 'array',
        // and return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless '0 <= length'.

    bdex_ByteInStreamRaw& getArrayInt24(int *array, int length);
        // Consume a 24-bit signed integer array of the specified 'length'
        // from this input stream, place that value in the specified 'array',
        // and return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless '0 <= length'.

    bdex_ByteInStreamRaw& getArrayUint24(unsigned int *array, int length);
        // Consume a 24-bit unsigned integer array of the specified 'length'
        // from this input stream, place that value in the specified 'array',
        // and return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless '0 <= length'.

    bdex_ByteInStreamRaw& getArrayInt16(short *array, int length);
        // Consume a 16-bit signed integer array of the specified 'length'
        // from this input stream, place that value in the specified 'array',
        // and return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless '0 <= length'.

    bdex_ByteInStreamRaw& getArrayUint16(unsigned short *array, int length);
        // Consume a 16-bit unsigned integer array of the specified 'length'
        // from this input stream, place that value in the specified 'array',
        // and return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless '0 <= length'.

    bdex_ByteInStreamRaw& getArrayInt8(char *array, int length);
    bdex_ByteInStreamRaw& getArrayInt8(signed char *array, int length);
        // Consume an 8-bit signed integer array of the specified 'length'
        // from this input stream, place that value in the specified 'array',
        // and return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless '0 <= length'.

    bdex_ByteInStreamRaw& getArrayUint8(char *array, int length);
    bdex_ByteInStreamRaw& getArrayUint8(unsigned char *array, int length);
        // Consume an 8-bit unsigned integer array of the specified 'length'
        // from this input stream, place that value in the specified 'array',
        // and return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless '0 <= length'.

                        // *** arrays of floating-point values ***

    bdex_ByteInStreamRaw& getArrayFloat64(double *array, int count);
        // Consume the IEEE double-precision (8-byte) floating-point array of
        // the specified 'length' from this input stream, place that value in
        // the specified 'array', and return a reference to this modifiable
        // stream.  If this stream is initially invalid, this operation has no
        // effect.  If this function otherwise fails to extract a valid array
        // of the required 'length', this stream is marked invalid and the
        // value of 'array' is undefined.  The behavior is undefined unless
        // '0 <= length'.  Note that for non-conforming platforms, this
        // operation may be lossy.

    bdex_ByteInStreamRaw& getArrayFloat32(float *array, int length);
        // Consume the IEEE single-precision (4-byte) floating-point array of
        // the specified 'length' from this input stream, place that value in
        // the specified 'array', and return a reference to this modifiable
        // stream.  If this stream is initially invalid, this operation has no
        // effect.  If this function otherwise fails to extract a valid array
        // of the required 'length', this stream is marked invalid and the
        // value of 'array' is undefined.  The behavior is undefined unless
        // '0 <= length'.  Note that for non-conforming platforms, this
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

    const char *data() const;
        // Return the address of the contiguous, non-modifiable internal memory
        // buffer of this stream.  The address will remain valid as long as
        // this array is not destroyed or modified (i.e., the current capacity
        // is not exceeded).  The behavior of accessing elements outside the
        // range '[ data() .. data() + (length() - 1) ]' is undefined.

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
bsl::ostream& operator<<(bsl::ostream&               stream,
                         const bdex_ByteInStreamRaw& object);
    // Write the specified 'object' to the specified output 'stream' in some
    // reasonable (multi-line) format, and return a reference to 'stream'

// TBD #if !defined(BSL_LEGACY) || 1 == BSL_LEGACY

template <typename OBJECT>
inline
bdex_ByteInStreamRaw& operator>>(bdex_ByteInStreamRaw& stream, OBJECT& object);
    // Read from the specified 'stream' a 'bdex' version and assign to the
    // specified 'bdex'-compliant 'object' the value read from 'stream' using
    // 'OBJECT's 'bdex' version format as read from 'stream'.  Return a
    // reference to the modifiable 'stream'.  If 'stream' is initially invalid,
    // this operation has no effect.  If 'stream' becomes invalid during this
    // operation, 'object' is valid, but its value is undefined.  If the read
    // in version is not supported by 'object', 'stream' is marked invalid,
    // but 'object' is unaltered.  The behavior is undefined unless the data in
    // 'stream' was written by a 'bdex_ByteOutStream' object.  (See the 'bdex'
    // package-level documentation for more information on 'bdex' streaming of
    // container types.)  Note that specializations for the supported
    // fundamental (scalar) types and for 'bsl::string' are also provided, in
    // which case this operator calls the appropriate 'get' method from this
    // component.

// TBD #endif

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

// CREATORS
inline
bdex_ByteInStreamRaw::bdex_ByteInStreamRaw(const char *buffer, int numBytes)
: d_size(numBytes)
, d_streambuf(buffer, numBytes)
, d_formatter(&d_streambuf)
{
    // IMPORTANT: d_streambuf and d_formatter must be initialized in the
    // order shown here.  Don't re-arrange their declarations in the class
    // definition.

    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(0 <= numBytes);
}

inline
bdex_ByteInStreamRaw::~bdex_ByteInStreamRaw()
{
}

// MANIPULATORS
inline
void bdex_ByteInStreamRaw::invalidate()
{
    d_formatter.invalidate();
}

inline
void bdex_ByteInStreamRaw::reset(const char *buffer, int numBytes)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(0 <= numBytes);

    d_streambuf.pubsetbuf(buffer, numBytes);
    d_size = numBytes;
}

inline
void bdex_ByteInStreamRaw::setCursor(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);

    d_streambuf.pubseekpos(index);
}

inline
bdex_ByteInStreamRaw& bdex_ByteInStreamRaw::getLength(int& variable)
{
    d_formatter.getLength(variable);
    return *this;
}

inline
bdex_ByteInStreamRaw& bdex_ByteInStreamRaw::getString(bsl::string& str)
{
    d_formatter.getString(str);
    return *this;
}

inline
bdex_ByteInStreamRaw& bdex_ByteInStreamRaw::getVersion(int& variable)
{
    d_formatter.getVersion(variable);
    return *this;
}

inline
bdex_ByteInStreamRaw&
bdex_ByteInStreamRaw::getInt64(bsls_PlatformUtil::Int64& variable)
{
    d_formatter.getInt64(variable);
    return *this;
}

inline
bdex_ByteInStreamRaw&
bdex_ByteInStreamRaw::getUint64(bsls_PlatformUtil::Uint64& variable)
{
    d_formatter.getUint64(variable);
    return *this;
}

inline
bdex_ByteInStreamRaw&
bdex_ByteInStreamRaw::getInt56(bsls_PlatformUtil::Int64& variable)
{
    d_formatter.getInt56(variable);
    return *this;
}

inline
bdex_ByteInStreamRaw&
bdex_ByteInStreamRaw::getUint56(bsls_PlatformUtil::Uint64& variable)
{
    d_formatter.getUint56(variable);
    return *this;
}

inline
bdex_ByteInStreamRaw&
bdex_ByteInStreamRaw::getInt48(bsls_PlatformUtil::Int64& variable)
{
    d_formatter.getInt48(variable);
    return *this;
}

inline
bdex_ByteInStreamRaw&
bdex_ByteInStreamRaw::getUint48(bsls_PlatformUtil::Uint64& variable)
{
    d_formatter.getUint48(variable);
    return *this;
}

inline
bdex_ByteInStreamRaw&
bdex_ByteInStreamRaw::getInt40(bsls_PlatformUtil::Int64& variable)
{
    d_formatter.getInt40(variable);
    return *this;
}

inline
bdex_ByteInStreamRaw&
bdex_ByteInStreamRaw::getUint40(bsls_PlatformUtil::Uint64& variable)
{
    d_formatter.getUint40(variable);
    return *this;
}

inline
bdex_ByteInStreamRaw& bdex_ByteInStreamRaw::getInt32(int& variable)
{
    d_formatter.getInt32(variable);
    return *this;
}

inline
bdex_ByteInStreamRaw& bdex_ByteInStreamRaw::getUint32(unsigned int& variable)
{
    d_formatter.getUint32(variable);
    return *this;
}

inline
bdex_ByteInStreamRaw& bdex_ByteInStreamRaw::getInt24(int& variable)
{
    d_formatter.getInt24(variable);
    return *this;
}

inline
bdex_ByteInStreamRaw& bdex_ByteInStreamRaw::getUint24(unsigned int& variable)
{
    d_formatter.getUint24(variable);
    return *this;
}

inline
bdex_ByteInStreamRaw& bdex_ByteInStreamRaw::getInt16(short& variable)
{
    d_formatter.getInt16(variable);
    return *this;
}

inline
bdex_ByteInStreamRaw& bdex_ByteInStreamRaw::getUint16(unsigned short& variable)
{
    d_formatter.getUint16(variable);
    return *this;
}

inline
bdex_ByteInStreamRaw& bdex_ByteInStreamRaw::getInt8(char& variable)
{
    d_formatter.getInt8(variable);
    return *this;
}

inline
bdex_ByteInStreamRaw& bdex_ByteInStreamRaw::getInt8(signed char& variable)
{
    d_formatter.getInt8(variable);
    return *this;
}

inline
bdex_ByteInStreamRaw& bdex_ByteInStreamRaw::getUint8(char& variable)
{
    d_formatter.getUint8(variable);
    return *this;
}

inline
bdex_ByteInStreamRaw& bdex_ByteInStreamRaw::getUint8(unsigned char& variable)
{
    d_formatter.getUint8(variable);
    return *this;
}

inline
bdex_ByteInStreamRaw& bdex_ByteInStreamRaw::getFloat64(double& variable)
{
    d_formatter.getFloat64(variable);
    return *this;
}

inline
bdex_ByteInStreamRaw& bdex_ByteInStreamRaw::getFloat32(float& variable)
{
    d_formatter.getFloat32(variable);
    return *this;
}

inline
bdex_ByteInStreamRaw&
bdex_ByteInStreamRaw::getArrayInt64(bsls_PlatformUtil::Int64 *array,
                                    int                       numValues)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= numValues);

    d_formatter.getArrayInt64(array, numValues);
    return *this;
}

inline
bdex_ByteInStreamRaw&
bdex_ByteInStreamRaw::getArrayUint64(bsls_PlatformUtil::Uint64 *array,
                                     int                        numValues)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= numValues);

    d_formatter.getArrayUint64(array, numValues);
    return *this;
}

inline
bdex_ByteInStreamRaw&
bdex_ByteInStreamRaw::getArrayInt56(bsls_PlatformUtil::Int64 *array,
                                    int                       numValues)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= numValues);

    d_formatter.getArrayInt56(array, numValues);
    return *this;
}

inline
bdex_ByteInStreamRaw&
bdex_ByteInStreamRaw::getArrayUint56(bsls_PlatformUtil::Uint64 *array,
                                     int                        numValues)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= numValues);

    d_formatter.getArrayUint56(array, numValues);
    return *this;
}

inline
bdex_ByteInStreamRaw&
bdex_ByteInStreamRaw::getArrayInt48(bsls_PlatformUtil::Int64 *array,
                                    int                       numValues)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= numValues);

    d_formatter.getArrayInt48(array, numValues);
    return *this;
}

inline
bdex_ByteInStreamRaw&
bdex_ByteInStreamRaw::getArrayUint48(bsls_PlatformUtil::Uint64 *array,
                                     int                        numValues)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= numValues);

    d_formatter.getArrayUint48(array, numValues);
    return *this;
}

inline
bdex_ByteInStreamRaw&
bdex_ByteInStreamRaw::getArrayInt40(bsls_PlatformUtil::Int64 *array,
                                    int                       numValues)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= numValues);

    d_formatter.getArrayInt40(array, numValues);
    return *this;
}

inline
bdex_ByteInStreamRaw&
bdex_ByteInStreamRaw::getArrayUint40(bsls_PlatformUtil::Uint64 *array,
                                     int                        numValues)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= numValues);

    d_formatter.getArrayUint40(array, numValues);
    return *this;
}

inline
bdex_ByteInStreamRaw&
bdex_ByteInStreamRaw::getArrayInt32(int *array, int numValues)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= numValues);

    d_formatter.getArrayInt32(array, numValues);
    return *this;
}

inline
bdex_ByteInStreamRaw&
bdex_ByteInStreamRaw::getArrayUint32(unsigned int *array, int numValues)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= numValues);

    d_formatter.getArrayUint32(array, numValues);
    return *this;
}

inline
bdex_ByteInStreamRaw&
bdex_ByteInStreamRaw::getArrayInt24(int *array, int numValues)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= numValues);

    d_formatter.getArrayInt24(array, numValues);
    return *this;
}

inline
bdex_ByteInStreamRaw&
bdex_ByteInStreamRaw::getArrayUint24(unsigned int *array, int numValues)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= numValues);

    d_formatter.getArrayUint24(array, numValues);
    return *this;
}

inline
bdex_ByteInStreamRaw&
bdex_ByteInStreamRaw::getArrayInt16(short *array, int numValues)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= numValues);

    d_formatter.getArrayInt16(array, numValues);
    return *this;
}

inline
bdex_ByteInStreamRaw&
bdex_ByteInStreamRaw::getArrayUint16(unsigned short *array, int numValues)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= numValues);

    d_formatter.getArrayUint16(array, numValues);
    return *this;
}

inline
bdex_ByteInStreamRaw&
bdex_ByteInStreamRaw::getArrayInt8(char *array, int numValues)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= numValues);

    d_formatter.getArrayInt8(array, numValues);
    return *this;
}

inline
bdex_ByteInStreamRaw&
bdex_ByteInStreamRaw::getArrayInt8(signed char *array, int numValues)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= numValues);

    d_formatter.getArrayInt8(array, numValues);
    return *this;
}

inline
bdex_ByteInStreamRaw&
bdex_ByteInStreamRaw::getArrayUint8(char *array, int numValues)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= numValues);

    d_formatter.getArrayUint8(array, numValues);
    return *this;
}

inline
bdex_ByteInStreamRaw&
bdex_ByteInStreamRaw::getArrayUint8(unsigned char *array, int numValues)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= numValues);

    d_formatter.getArrayUint8(array, numValues);
    return *this;
}

inline
bdex_ByteInStreamRaw&
bdex_ByteInStreamRaw::getArrayFloat64(double *array, int numValues)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= numValues);

    d_formatter.getArrayFloat64(array, numValues);
    return *this;
}

inline
bdex_ByteInStreamRaw& bdex_ByteInStreamRaw::getArrayFloat32(float *array,
                                                            int    numValues)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= numValues);

    d_formatter.getArrayFloat32(array, numValues);
    return *this;
}

// TBD #if !defined(BSL_LEGACY) || 1 == BSL_LEGACY

template <typename TYPE>
inline
bdex_ByteInStreamRaw& bdex_ByteInStreamRaw::get(TYPE& variable)
{
    return bdex_InStreamMethodsUtil::streamInVersionAndObject(*this, variable);
}

// TBD #endif

// ACCESSORS
inline
bdex_ByteInStreamRaw::operator const void *() const
{
    return d_formatter ? this : 0;
}

inline
const char *bdex_ByteInStreamRaw::data() const
{
    return d_streambuf.data();
}

inline
bool bdex_ByteInStreamRaw::isEmpty() const
{
    return 0 == d_streambuf.length();
}

inline
int bdex_ByteInStreamRaw::length() const
{
    return d_size;
}

inline
int bdex_ByteInStreamRaw::cursor() const
{
    return d_size - static_cast<int>(d_streambuf.length());
}

// FREE OPERATORS

// TBD #if !defined(BSL_LEGACY) || 1 == BSL_LEGACY

template <typename OBJECT>
inline
bdex_ByteInStreamRaw& operator>>(bdex_ByteInStreamRaw& stream, OBJECT& object)
{
    return stream.get(object);
}

// TBD #endif

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
