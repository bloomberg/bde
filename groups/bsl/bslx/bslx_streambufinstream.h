// bslx_streambufinstream.h                                           -*-C++-*-
#ifndef INCLUDED_BSLX_STREAMBUFINSTREAM
#define INCLUDED_BSLX_STREAMBUFINSTREAM

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Unexternalization of fundamental types from a 'bsl::streambuf'.
//
//@CLASSES:
//  bslx::StreambufInStream: 'bsl::streambuf' input stream for fundamentals
//
//@SEE_ALSO: bslx_streambufoutstream, bslx_genericinstream
//
//@DESCRIPTION: This component implements a 'bsl::streambuf' input stream
// class, 'bslx::StreambufInStream', that provides platform-independent input
// methods ("unexternalization") on values, and arrays of values, of
// fundamental types, and on 'bsl::string'.
//
// The 'bslx::StreambufInStream' type reads from a user-supplied
// 'bsl::streambuf' directly, with no data copying or assumption of ownership.
// The user must therefore make sure that the lifetime and visibility of the
// buffer is sufficient to satisfy the needs of the input stream.
//
// This component is intended to be used in conjunction with the
// 'bslx_streambufoutstream' "externalization" component.  Each input method of
// 'bslx::StreambufInStream' reads either a value or a homogeneous array of
// values of a fundamental type, in a format that was written by the
// corresponding 'bslx::StreambufOutStream' method.  In general, the user of
// this component cannot rely on being able to read data that was written by
// any mechanism other than 'bslx::StreambufOutStream'.
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
// 'janeSmith' and a 'bslx::StreambufOutStream' 'outStream':
//..
//  MyPerson                 janeSmith("Jane", "Smith", 42);
//  bsl::stringbuf           buffer;
//  bslx::StreambufOutStream outStream(&buffer, 20131127);
//  const int                VERSION = 1;
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
// Then, create a 'bslx::StreambufInStream' 'inStream' initialized with the
// buffer from the 'bslx::StreambufOutStream' object 'outStream' and
// unexternalize this data into 'janeCopy':
//..
//  bslx::StreambufInStream inStream(&buffer);
//  int                     version;
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

#ifndef INCLUDED_BSLX_GENERICINSTREAM
#include <bslx_genericinstream.h>
#endif

#ifndef INCLUDED_BSL_STREAMBUF
#include <bsl_streambuf.h>
#endif

namespace BloombergLP {
namespace bslx {

                       // =======================
                       // class StreambufInStream
                       // =======================

typedef GenericInStream<bsl::streambuf> StreambufInStream;
    // This class facilitates the unexternalization of values (and C-style
    // arrays of values) of the fundamental integral and floating-point types
    // in a data-independent, platform-neutral representation.  It is currently
    // a 'typedef' for 'bslx::GenericInStream<bsl::streambuf>'.

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
