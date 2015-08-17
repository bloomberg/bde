// bdlxxxx_byteoutstreamformatter.h                                   -*-C++-*-
#ifndef INCLUDED_BDLXXXX_BYTEOUTSTREAMFORMATTER
#define INCLUDED_BDLXXXX_BYTEOUTSTREAMFORMATTER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Format fundamental types for streaming to a 'streambuf'.
//
//@CLASSES:
//   bdlxxxx::ByteOutStreamFormatter: write fundamentals to a 'streambuf'
//
//@AUTHOR: Lea Fester (lfester), Herve Bronnimann (hbronnimann)
//
//@SEE_ALSO: bdlxxxx_byteinstreamformatter
//
//@DESCRIPTION: This component conforms to the 'bdlxxxx_outstream' (documentation
// only) "protocol", implementing a byte-array-based output stream class that
// provides platform-independent output methods ("externalization") on values,
// and arrays of values, of fundamental types, and on 'bsl::string'.
//
// This component is intended to be used in conjunction with the
// 'bdlxxxx_byteinstreamformatter' "unexternalization" component.  Each output
// method of 'bdlxxxx::ByteOutStreamFormatter' writes a value or a homogeneous
// array of values to an internally held stream buffer.  The values are
// formatted to be readable by the corresponding 'bdlxxxx::ByteInStreamFormatter'
// method.  In general, the user cannot rely on any other mechanism to read
// data written by 'bdlxxxx::ByteOutStreamFormatter' unless that mechanism
// explicitly states its ability to do so.
//
// The supported types and required content are listed in the table below.  All
// of the fundamental types in the table may be output as scalar values or as
// homogeneous arrays.  'bsl::string' is output as a logical scalar string.
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
//      bsl::string       BDE STL implementation of the STL string class
//..
// This component also supports compact streaming of integer types.  In
// particular, 64-bit integers can be streamed as 40, 48, 56, or 64-bit values,
// and 32-bit integers can be streamed as 24 or 32-bit values, at the user's
// discretion.  In all cases, the least significant bytes of the fundamental
// integer type are written to the stream.  Note that, for signed types, this
// truncation may not preserve the sign of the streamed value; it is the user's
// responsibility to choose output methods appropriate to the data.
//
// Note that the values are stored in 'Big Endian' (i.e., network byte order)
// format.
//
// Note that output streams can be *invalidated* explicitly and queried for
// *validity*.  Writing to an initially invalid stream has no effect.  Whenever
// an output operation fails, the stream should be invalidated explicitly.
//
///Generic formatters
///------------------
// Another pair of classes, 'bdlxxxx::GenericByteInStream' and
// 'bdlxxxx::GenericByteOutStream', are parameterized by a 'StreamBuf' type, which
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
// array of length *L* and stream back in an array of length other than *L*.
//
///Usage
///-----
// The 'bdlxxxx::ByteOutStreamFormatter' implementation of the 'bdlxxxx::OutStream'
// protocol can be used to externalize values in a platform-neutral way.
// Writing out fundamental C++ types and 'bsl::string' requires no additional
// work on the part of the client; the client can simply use the stream
// directly.  The following code serializes a few representative values using a
// 'bdlxxxx::ByteOutStreamFormatter', and then writes the contents of the 'bdex'
// stream's buffer to 'stdout'.
//..
// int main(int argc, char **argv)
// {
//
//     // Specify the buffer to write to
//     char buffer[4096];
//     bdlsb::FixedMemOutStreamBuf sb(buffer, sizeof buffer);
//
//     // Create a stream and write out some values.
//     bdlxxxx::ByteOutStreamFormatter outStream(&sb);
//     bdex_OutStreamFunctions::streamOut(outStream, 1, 0);
//     bdex_OutStreamFunctions::streamOut(outStream, 2, 0);
//     bdex_OutStreamFunctions::streamOut(outStream, 'c', 0);
//     bdex_OutStreamFunctions::streamOut(outStream, bsl::string("hello"), 0);
//
//     // Verify the results on 'stdout'.
//     const int length = 15;
//     for(int i = 0; i < length; ++i) {
//         if(bsl::isalnum(static_cast<unsigned char>(buffer[i]))) {
//             cout << "nextByte (char): " << buffer[i] << endl;
//         }
//         else {
//             cout << "nextByte (int): " << (int)buffer[i] << endl;
//         }
//     }
//     return 0;
// }
//..
// Executing the above code results in the following output:
//..
// nextByte (int): 0
// nextByte (int): 0
// nextByte (int): 0
// nextByte (int): 1
// nextByte (int): 0
// nextByte (int): 0
// nextByte (int): 0
// nextByte (int): 2
// nextByte (char): c
// nextByte (int): 5
// nextByte (char): h
// nextByte (char): e
// nextByte (char): l
// nextByte (char): l
// nextByte (char): o
//..
// Note the numeric value 5 prepended to the string literal "hello".  This is
// because 'const char *' types are picked up by the template specialization
// for type 'bsl::string', which outputs the string length before emitting the
// characters.
//
// See the 'bdlxxxx_byteinstream' component usage example for a more practical
// example of using 'bdex' streams with a value-semantic type.

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLXXXX_GENERICBYTEOUTSTREAM
#include <bdlxxxx_genericbyteoutstream.h>
#endif

#ifndef INCLUDED_BSL_STREAMBUF
#include <bsl_streambuf.h>
#endif


namespace BloombergLP {

namespace bdlxxxx {
                        // =================================
                        // class ByteOutStreamFormatter
                        // =================================

typedef GenericByteOutStream<bsl::streambuf> ByteOutStreamFormatter;
    // This class provides output methods to externalize fundamental types in
    // byte representation and stores the accumulated result in network byte
    // order format.

// FREE OPERATORS
ByteOutStreamFormatter&
  operator<<(ByteOutStreamFormatter& stream,
             ByteOutStreamFormatter& (*pf)(ByteOutStreamFormatter&));
}  // close package namespace
    // Provide support for all manipulator functions on this stream (e.g.,
    // 'bdexFlush').

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

// FREE OPERATORS
inline
bdlxxxx::ByteOutStreamFormatter&
  bdlxxxx::operator<<(ByteOutStreamFormatter& stream,
             ByteOutStreamFormatter& (*pf)(ByteOutStreamFormatter&))
{
    return (*pf)(stream);
}

}  // close enterprise namespace



#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
