// bdex_byteinstreamformatter.h                                       -*-C++-*-
#ifndef INCLUDED_BDEX_BYTEINSTREAMFORMATTER
#define INCLUDED_BDEX_BYTEINSTREAMFORMATTER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Parse fundamental types from provided stream.
//
//@CLASSES:
//  bdex_ByteInStreamFormatter: read fundamental types from a 'streambuf'
//
//@AUTHOR: Lea Fester (lfester), Herve Bronnimann (hbronnimann)
//
//@DESCRIPTION: This component conforms to the 'bdex_instream' (documentation
// only) "protocol", implementing a byte-array-based data stream object that
// provides platform-independent input methods ("unexternalization") on values,
// and arrays of values, of fundamental types, and on 'bsl::string'.
//
// This component is intended to be used in conjunction with the
// 'bdex_byteoutstreamformatter' externalization component.  Each input
// method of 'bdex_ByteInStreamFormatter' reads either a value or a
// homogeneous array of values of a fundamental type, in a format that was
// written by the corresponding 'bdex_ByteOutStreamFormatter' method.  In
// general, the user of this component cannot rely on being able to read
// data that was written by any mechanism other than
// 'bdex_ByteOutStreamFormatter'.
//
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
///Generic formatters
///------------------
// Another pair of classes, 'bdex_GenericByteInStream' and
// 'bdex_GenericByteOutStream', are parameterized by a 'StreamBuf' type, which
// allow one to avoid initialization and virtual function overhead caused by
// the use of the 'bsl::basic_streambuf' protocol.  Consider using those
// instead when you have a concrete input or output class, not derived from
// 'bsl::streambuf', that you can take advantage of.
//
///Externalization and value semantics
///-----------------------------------
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
// a class method 'maxSupportedBdexVersion' and two instance methods, an
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
//          // Create a person having the specified 'firstname', 'lastName',
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
//  bool operator==(const my_Person& lhs, const my_Person& rhs);
//      // Return 'true' if the specified 'lhs' and 'rhs' person objects have
//      // the same value and 'false' otherwise.  Two person objects have the
//      // same value if they have the same first name, last name, and age.
//
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
//  inline
//  STREAM& my_Person::bdexStreamIn(STREAM& stream, int version)
//  {
//      if (stream) {
//          switch (version) {    // switch on the 'bdex' version
//            case 1: {
//                stream.getString(d_firstName);
//                if (!stream) {
//                    d_firstName = "stream error";  // *might* be corrupted;
//                                                   //  value for testing
//                    return stream;
//                }
//                stream.getString(d_lastName);
//                if (!stream) {
//                    d_lastName = "stream error";  // *might* be corrupted;
//                                                  //  value for testing
//                    return stream;
//                }
//                stream.getInt32(d_age);
//                if (!stream) {
//                    d_age = 1;      // *might* be corrupted; value for
//                                    // testing
//                    return stream;
//                }
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
//  inline
//  STREAM& my_Person::bdexStreamOut(STREAM& stream, int version) const
//  {
//      switch (version) {
//        case 1: {
//            stream.putString(d_firstName);
//            stream.putString(d_lastName);
//            stream.putInt32(d_age);
//        } break;
//      }
//      return stream;
//  }
//..
// We can now exercise the new 'my_person' value-semantic component by
// externalizing and reconstituting an object.  First create a 'my_Person'
// 'JaneSmith' and a 'bdex_ByteOutStream' 'outStream'.
//..
//  // my_testapp.m.cpp
//
//  using namespace std;
//
//  int main(int argc, char *argv[])
//  {
//      my_Person JaneSmith("Jane", "Smith", 42);
//      bdex_ByteOutStreamFormatter outStream;
//      const int VERSION = 1;
//      outStream.putVersion(VERSION);
//      bdex_OutStreamFunctions::streamOut(outStream, JaneSmith, VERSION);
//..
// Next create a 'my_Person' 'janeCopy' initialized to the default value, and
// assert that 'janeCopy' is different from 'janeSmith'.
//..
//      my_Person janeCopy;                      assert(janeCopy != JaneSmith);
//..
// Now create a 'bdex_ByteInStreamFormatter' 'inStream' initialized with the
// buffer from the 'bdex_ByteOutStreamFormatter' object 'outStream'.
//..
//      bdex_ByteInStreamFormatter inStream(outStream.data(),
//                                          outStream.length());
//      int version;
//      inStream.getVersion(version);
//      bdex_InStreamFunctions::streamIn(inStream, janeCopy, version);
//                                               assert(janeCopy == JaneSmith);
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
//  }
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEX_GENERICBYTEINSTREAM
#include <bdex_genericbyteinstream.h>
#endif

#ifndef INCLUDED_BSL_STREAMBUF
#include <bsl_streambuf.h>
#endif


namespace BloombergLP {

                        // =================================
                        // class bdex_ByteInStreamFormatter
                        // =================================

typedef bdex_GenericByteInStream<bsl::streambuf> bdex_ByteInStreamFormatter;
    // This class facilitates the unexternalization of values (and C-style
    // arrays of values) of the fundamental integral and floating-point types
    // in a data-independent, platform-neutral representation.  It is currently
    // a typedef for 'bdex_GenericByteInStream<bsl::streambuf>'.

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

} // close namespace BloombergLP



#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
