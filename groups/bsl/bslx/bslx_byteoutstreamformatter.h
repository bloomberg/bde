// bslx_byteoutstreamformatter.h                                      -*-C++-*-
#ifndef INCLUDED_BSLX_BYTEOUTSTREAMFORMATTER
#define INCLUDED_BSLX_BYTEOUTSTREAMFORMATTER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Externalization of fundamental types to a 'bsl::streambuf'.
//
//@CLASSES:
//  bslx::ByteOutStreamFormatter: 'bsl::streambuf' output for fundamentals
//
//@SEE_ALSO: bslx_byteinstreamformatter, bslx_genericbyteoutstream
//
//@DESCRIPTION: This component implements a 'bsl::streambuf' output stream
// class, 'bslx::ByteOutStreamFormatter', that provides platform-independent
// output methods ("externalization") on values, and arrays of values, of
// fundamental types, and on 'bsl::string'.
//
// This component is intended to be used in conjunction with the
// 'bslx_byteinstreamformatter' "unexternalization" component.  Each output
// method of 'bslx::ByteOutStreamFormatter' writes a value or a homogeneous
// array of values to a 'bsl::streambuf'.  The values are formatted to be
// readable by the corresponding 'bslx::ByteInStreamFormatter' method.  In
// general, the user cannot rely on any other mechanism to read data written by
// 'bslx::ByteOutStreamFormatter' unless that mechanism explicitly states its
// ability to do so.
//
// The supported types and required content are listed in the table below.  All
// of the fundamental types in the table may be output as scalar values or as
// homogeneous arrays.  'bsl::string' is output as an 'int' representing the
// string's length and a homogeneous 'char' array for the string's data.  Note
// that 'Int64' and 'Uint64' denote 'bsls::Types::Int64' and
// 'bsls::Types::Uint64', which in turn are 'typedef' names for the signed and
// unsigned 64-bit integer types, respectively, on the host platform.
//..
//      C++ TYPE          REQUIRED CONTENT OF ANY PLATFORM-NEUTRAL FORMAT
//      --------          -----------------------------------------------
//      Int64             least significant 64 bits (signed)
//      Uint64            least significant 64 bits (unsigned)
//      int               least significant 32 bits (signed)
//      unsigned int      least significant 32 bits (unsigned)
//      short             least significant 16 bits (signed)
//      unsigned short    least significant 16 bits (unsigned)
//      char              least significant  8 bits (platform-dependent)
//      signed char       least significant  8 bits (signed)
//      unsigned char     least significant  8 bits (unsigned)
//      double            IEEE standard 8-byte floating-point value
//      float             IEEE standard 4-byte floating-point value
//
//      bsl::string       BDE implementation of the STL string class
//..
// This component also supports compact streaming of integer types.  In
// particular, 64-bit integers can be streamed as 40-, 48-, 56-, or 64-bit
// values, and 32-bit integers can be streamed as 24- or 32-bit values, at the
// user's discretion.  In all cases, the least significant bytes of the
// fundamental integer type are written to the stream.  Note that, for signed
// types, this truncation may not preserve the sign of the streamed value; it
// is the user's responsibility to choose output methods appropriate to the
// data.
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
///- - - - - - - - - - - - - - - -
// A 'bslx::ByteOutStreamFormatter' can be used to externalize values in a
// platform-neutral way.  Writing out fundamental C++ types and 'bsl::string'
// requires no additional work on the part of the client; the client can simply
// use the stream directly.  The following code serializes a few representative
// values using a 'bslx::ByteOutStreamFormatter', compares the contents of this
// stream to the expected value, and then writes the contents of this stream's
// buffer to 'stdout'.
//
// First, we create a 'bslx::ByteOutStreamFormatter' with an arbitrary value
// for its 'versionSelector' and externalize some values:
//..
//  bsl::stringbuf               buffer;
//  bslx::ByteOutStreamFormatter outStream(&buffer, 20131127);
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
// See the 'bslx_byteinstreamformatter' component usage example for a more
// practical example of using 'bslx' streams.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLX_GENERICBYTEOUTSTREAM
#include <bslx_genericbyteoutstream.h>
#endif

#ifndef INCLUDED_BSL_STREAMBUF
#include <bsl_streambuf.h>
#endif

namespace BloombergLP {
namespace bslx {

                     // ============================
                     // class ByteOutStreamFormatter
                     // ============================

typedef GenericByteOutStream<bsl::streambuf> ByteOutStreamFormatter;
    // This class provides output methods to externalize fundamental types in
    // byte representation and stores the accumulated result in network byte
    // order format.

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2014 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
