// bslx_streambufoutstream.h                                          -*-C++-*-
#ifndef INCLUDED_BSLX_STREAMBUFOUTSTREAM
#define INCLUDED_BSLX_STREAMBUFOUTSTREAM

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Externalization of fundamental types to a 'bsl::streambuf'.
//
//@CLASSES:
//  bslx::StreambufOutStream: 'bsl::streambuf' output for fundamentals
//
//@SEE_ALSO: bslx_streambufinstream, bslx_genericoutstream
//
//@DESCRIPTION: This component implements a 'bsl::streambuf' output stream
// class, 'bslx::StreambufOutStream', that provides platform-independent
// output methods ("externalization") on values, and arrays of values, of
// fundamental types, and on 'bsl::string'.
//
// This component is intended to be used in conjunction with the
// 'bslx_streambufinstream' "unexternalization" component.  Each output method
// of 'bslx::StreambufOutStream' writes a value or a homogeneous array of
// values to a 'bsl::streambuf'.  The values are formatted to be readable by
// the corresponding 'bslx::StreambufInStream' method.  In general, the user
// cannot rely on any other mechanism to read data written by
// 'bslx::StreambufOutStream' unless that mechanism explicitly states its
// ability to do so.
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
// A 'bslx::StreambufOutStream' can be used to externalize values in a
// platform-neutral way.  Writing out fundamental C++ types and 'bsl::string'
// requires no additional work on the part of the client; the client can simply
// use the stream directly.  The following code serializes a few representative
// values using a 'bslx::StreambufOutStream', compares the contents of this
// stream to the expected value, and then writes the contents of this stream's
// buffer to 'stdout'.
//
// First, we create a 'bslx::StreambufOutStream' with an arbitrary value for
// its 'versionSelector' and externalize some values:
//..
//  bsl::stringbuf               buffer;
//  bslx::StreambufOutStream outStream(&buffer, 20131127);
//  outStream.putInt32(1);
//  outStream.putInt32(2);
//  outStream.putInt8('c');
//  outStream.putString(bsl::string("hello"));
//..
// Then, we compare the contents of the stream to the expected value:
//..
//  bsl::string  theChars = buffer.str();
//  ASSERT(15 == theChars.size());
//  ASSERT( 0 == bsl::memcmp(theChars.data(),
//                           "\x00\x00\x00\x01\x00\x00\x00\x02""c\x05""hello",
//                           15));
//..
// Finally, we print the stream's contents to 'bsl::cout'.
//..
//  for (bsl::size_t i = 0; i < theChars.size(); ++i) {
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
// See the 'bslx_streambufinstream' component usage example for a more
// practical example of using 'bslx' streams.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLX_GENERICOUTSTREAM
#include <bslx_genericoutstream.h>
#endif

#ifndef INCLUDED_BSL_STREAMBUF
#include <bsl_streambuf.h>
#endif

namespace BloombergLP {
namespace bslx {

                       // ========================
                       // class StreambufOutStream
                       // ========================

typedef GenericOutStream<bsl::streambuf> StreambufOutStream;
    // This class facilitates the externalization of values (and C-style arrays
    // of values) of the fundamental integral and floating-point types in a
    // data-independent, platform-neutral representation.  It is currently a
    // 'typedef' for 'bslx::GenericOutStream<bsl::streambuf>'.

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
