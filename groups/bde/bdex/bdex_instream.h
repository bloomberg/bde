// bdex_instream.h              -*-C++-*-
#ifndef INCLUDED_BDEX_INSTREAM
#define INCLUDED_BDEX_INSTREAM

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Enable (un)externalization of fundamental types.
//
//@CLASSES:
//   bdex_InStream: protocol class for (un)externalizing fundamental types
//
//@AUTHOR: John Lakos (jlakos)
//
//@SEE_ALSO: bdex_outstream
//
//@DESCRIPTION: This component provides a protocol for a data stream object
// that implements the input methods ("unexternalization") of a
// platform-independent externalization package.  Each input method reads
// either a value or a homogeneous array of values of a fundamental type that
// has been formatted by a cooperating externalization component implementing
// the 'bdex_outstream' protocol.  The supported types and required content are
// as follows (note that 'Int64' and 'Uint64' are 'typedef' names for the
// signed and unsigned 64-bit integer types, respectively, on the host
// platform):
//..
//      C++ TYPE          REQUIRED CONTENT OF ANY PLATFORM-NEUTRAL FORMAT
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
// This interface also supports compact streaming in of integer types.  In
// particular, 64-bit values can be streamed in from 40, 48, 56, or 64-bit
// values, and 32-bit values can be streamed in from 24 and 32-bit values
// (consistent with what has been written to the stream, of course).  Note
// that, for signed types, the sign is preserved for all streamed-in values.
//
// Note that (unlike output streams) input streams can be *invalidated*
// explicitly and queried for *validity* and *emptiness* via this interface.
// Reading from an initially invalid stream has no effect.  Attempting to read
// beyond the end of a stream will automatically invalidate the stream.
// Whenever an inconsistent value is detected, the stream should be invalidated
// explicitly.
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
///Usage Example 1
///---------------
// The 'bdex_InStream' input stream interface provided in this component is
// most commonly used to implement the stream-in functionality for a
// user-defined type.  For example, consider the following 'MyBox' class:
//..
//  // mybox.h
//
//  class MyBox {
//      MyPoint d_originCorner;
//      MyPoint d_oppositeCorner;
//
//    public:
//      // CLASS METHODS
//     static int maxSupportedBdexVersion();
//         // Return the most current 'bdex' streaming version number
//         // supported by this class.  (See the package-group-level
//         // documentation for more information on 'bdex' streaming of
//         // container types.)
//
//      // CREATORS
//      MyBox();                                    // [ default ctor needed ]
//          // Create a zero-perimeter box at the origin (0, 0).
//
//      // MANIPULATORS
//      template <class STREAM>
//      STREAM& bdexStreamIn(STREAM& stream, int version);
//          // Assign to this object the value read from the specified input
//          // 'stream' using the specified 'version' format and return a
//          // reference to the modifiable 'stream'.  If 'stream' is initially
//          // invalid, this operation has no effect.  If 'stream' becomes
//          // invalid during this operation, this object is valid, but its
//          // value is undefined.  If 'version' is not supported, 'stream' is
//          // marked invalid and this object is unaltered.  Note that no
//          // version is read from 'stream'.  See the 'bdex' package-level
//          // documentation for more information on 'bdex' streaming of
//          // value-semantic types and containers.  Assign to this object the
//          // value read from the specified 'stream'.
//
//      // ACCESSORS
//      template <class STREAM>
//      STREAM& bdexStreamOut(STREAM& stream, int version) const;
//          // Write this value to the specified output 'stream' using the
//          // specified 'version' format and return a reference to the
//          // modifiable 'stream'.  If 'version' is not supported, 'stream' is
//          // unmodified.  Note that 'version' is not written to 'stream'.
//          // See the 'bdex' package-level documentation for more information
//          // on 'bdex' streaming of value-semantic types and containers.
//  };
//..
// The following 'mybox.cpp' file excerpt illustrates an implementation of the
// stream-in and corresponding stream-out functionality.  Note that the
// higher-level 'MyBox' component must assume the existence of 'bdex' streaming
// functionality in the 'MyPoint' component on which 'MyBox' depends.
//..
//  // mybox.cpp
//  #include <mybox.h>
//  #include <mypoint.h>
//
//  // CLASS METHODS
//  static int MyBox::maxSupportedBdexVersion()
//  {
//      return 1;
//  }
//
//  // MANIPULATORS
//  template <class STREAM>
//  STREAM& MyBox::bdexStreamIn(STREAM& stream, int version)
//  {
//      if (stream) {
//          switch (version) {// switch on the schema version (starting with 1)
//            case 1: {
//              bdex_InStreamFunctions::streamIn(stream, d_originCorner, 1);
//              if (!stream) {
//                  return stream;                                    // RETURN
//              }
//
//              bdex_InStreamFunctions::streamIn(stream, d_oppositeCorner, 1);
//              if (!stream) {
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
//  template <class STREAM>
//  STREAM& MyBox::bdexStreamOut(STREAM& stream, int version) const
//  {
//      if (stream) {
//          switch (version) {// switch on the schema version (starting with 1)
//            case 1: {
//              bdex_OutStreamFunctions::streamOut(stream, d_originCorner, 1);
//              bdex_OutStreamFunctions::streamOut(stream,
//                                                 d_oppositeCorner,
//                                                 1);
//            } break;
//            default: {
//              stream.invalidate();
//            }
//          }
//      }
//      return stream;
//  }
//..
// Note that when streaming in (unexternalizing) the value of an object, it is
// usually wise to gather sufficient data in advance in order to guard against
// leaving the object in an inconsistent state due to a corrupted stream.  If
// bad data is detected (e.g., a bad version number), the input stream should
// be invalidated explicitly.
//
///Usage Example 2
///---------------
// Frameworks for streaming the values of objects may have expectations of what
// is being transmitted.  It may, for example, be considered an error to have
// data remaining in a stream after the expected data has been removed.  For
// that reason, the 'isEmpty' function is provided to enable a messaging-layer
// component to verify, after reading all expected data, that the stream is not
// only valid, but also empty:
//..
//  int extractExpectedObjectsFromStream(bdex_InStream& inputStream)
//  {
//      enum { CORRUPTED_STREAM = -1, SUCCESS = 0, UNEXPECTED_EXTRA_DATA = 1 };
//
//      // Read all expected input from the input stream.
//
//      return !inputStream                // "if (!inputStream)
//             ? CORRUPTED_STREAM          //    return CORRUPTED_STREAM;
//             : !inputStream.isEmpty()    //  else if (!inputStream.isEmpty())
//               ? UNEXPECTED_EXTRA_DATA   //    return UNEXPECTED_EXTRA_DATA;
//               : SUCCESS;                //  else return SUCCESS;"
//  }
//..
// Note that the behavior of the 'isEmpty' method is explicitly undefined if
// the stream is not valid.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEX_INSTREAMMETHODS
#include <bdex_instreammethods.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {

                        // ===================
                        // class bdex_InStream
                        // ===================

class bdex_InStream {
    // This class defines a protocol (i.e., a pure interface) for an input data
    // stream object that enables values, and C-style arrays of values, of the
    // fundamental integral and floating-point types, as well as 'bsl::string'
    // values, to be unexternalized from some platform-neutral representation.
    //
    // This protocol assumes that short integers are at least 16 bits, integers
    // and floats are at least 32 bits, and 'bsls_PlatformUtil' "64-bit"
    // integers and doubles are at least 64 bits, but makes no other
    // internal-format or stream-format assumptions.  Note that concrete
    // streams following this protocol are free to publish specific formats
    // that must then be strictly implemented on all platforms.

  public:
    // CREATORS
    virtual ~bdex_InStream();
        // Destroy this input stream.

    // MANIPULATORS
    virtual bdex_InStream& getLength(int& length) = 0;
        // Consume a length value from this input stream, place that value in
        // the specified 'length', and return a reference to this modifiable
        // stream.  If this stream is initially invalid, this operation has no
        // effect.  If this function otherwise fails to extract a valid value,
        // this stream is marked invalid and the value of 'length' is
        // undefined.

    virtual bdex_InStream& getVersion(int& version) = 0;
        // Consume a version value from this input stream, place that value in
        // the specified 'version', and return a reference to this modifiable
        // stream.  If this stream is initially invalid, this operation has no
        // effect.  If this function otherwise fails to extract a valid value,
        // this stream is marked invalid and the value of 'version' is
        // undefined.

                        // *** scalar integer values ***

    virtual bdex_InStream& getInt64(bsls_PlatformUtil::Int64& variable) = 0;
        // Consume a 64-bit signed integer value from this input stream, place
        // that value in the specified 'variable', and return a reference to
        // this modifiable stream.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.

    virtual bdex_InStream& getUint64(bsls_PlatformUtil::Uint64& variable) = 0;
        // Consume a 64-bit unsigned integer value from this input stream,
        // place that value in the specified 'variable', and return a reference
        // to this modifiable stream.  If this stream is initially invalid,
        // this operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.

    virtual bdex_InStream& getInt56(bsls_PlatformUtil::Int64& variable) = 0;
        // Consume a 56-bit signed integer value from this input stream, place
        // that value in the specified 'variable', and return a reference to
        // this modifiable stream.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.

    virtual bdex_InStream& getUint56(bsls_PlatformUtil::Uint64& variable) = 0;
        // Consume a 56-bit unsigned integer value from this input stream,
        // place that value in the specified 'variable', and return a reference
        // to this modifiable stream.  If this stream is initially invalid,
        // this operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.

    virtual bdex_InStream& getInt48(bsls_PlatformUtil::Int64& variable) = 0;
        // Consume a 48-bit signed integer value from this input stream, place
        // that value in the specified 'variable', and return a reference to
        // this modifiable stream.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.

    virtual bdex_InStream& getUint48(bsls_PlatformUtil::Uint64& variable) = 0;
        // Consume a 48-bit unsigned integer value from this input stream,
        // place that value in the specified 'variable', and return a reference
        // to this modifiable stream.  If this stream is initially invalid,
        // this operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.

    virtual bdex_InStream& getInt40(bsls_PlatformUtil::Int64& variable) = 0;
        // Consume a 40-bit signed integer value from this input stream, place
        // that value in the specified 'variable', and return a reference to
        // this modifiable stream.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.

    virtual bdex_InStream& getUint40(bsls_PlatformUtil::Uint64& variable) = 0;
        // Consume a 40-bit unsigned integer value from this input stream,
        // place that value in the specified 'variable', and return a reference
        // to this modifiable stream.  If this stream is initially invalid,
        // this operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.

    virtual bdex_InStream& getInt32(int& variable) = 0;
        // Consume a 32-bit signed integer value from this input stream, place
        // that value in the specified 'variable', and return a reference to
        // this modifiable stream.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.

    virtual bdex_InStream& getUint32(unsigned int& variable) = 0;
        // Consume a 32-bit unsigned integer value from this input stream,
        // place that value in the specified 'variable', and return a reference
        // to this modifiable stream.  If this stream is initially invalid,
        // this operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.

    virtual bdex_InStream& getInt24(int& variable) = 0;
        // Consume a 24-bit signed integer value from this input stream, place
        // that value in the specified 'variable', and return a reference to
        // this modifiable stream.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.

    virtual bdex_InStream& getUint24(unsigned int& variable) = 0;
        // Consume a 24-bit unsigned integer value from this input stream,
        // place that value in the specified 'variable', and return a reference
        // to this modifiable stream.  If this stream is initially invalid,
        // this operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.

    virtual bdex_InStream& getInt16(short& variable) = 0;
        // Consume a 16-bit signed integer value from this input stream, place
        // that value in the specified 'variable', and return a reference to
        // this modifiable stream.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.

    virtual bdex_InStream& getUint16(unsigned short& variable) = 0;
        // Consume a 16-bit unsigned integer value from this input stream,
        // place that value in the specified 'variable', and return a reference
        // to this modifiable stream.  If this stream is initially invalid,
        // this operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.

    virtual bdex_InStream& getInt8(char& variable) = 0;
    virtual bdex_InStream& getInt8(signed char& variable) = 0;
        // Consume an 8-bit signed integer value from this input stream, place
        // that value in the specified 'variable', and return a reference to
        // this modifiable stream.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.

    virtual bdex_InStream& getUint8(char& variable) = 0;
    virtual bdex_InStream& getUint8(unsigned char& variable) = 0;
        // Consume an 8-bit unsigned integer value from this input stream,
        // place that value in the specified 'variable', and return a reference
        // to this modifiable stream.  If this stream is initially invalid,
        // this operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.

                        // *** scalar floating-point values ***

    virtual bdex_InStream& getFloat64(double& variable) = 0;
        // Consume a 64-bit floating-point value from this input stream, place
        // that value in the specified 'variable', and return a reference to
        // this modifiable stream.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.  Note that, for non-conforming
        // platforms, this operation may be lossy.

    virtual bdex_InStream& getFloat32(float& variable) = 0;
        // Consume a 32-bit floating-point value from this input stream, place
        // that value in the specified 'variable', and return a reference to
        // this modifiable stream.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.  Note that, for non-conforming
        // platforms, this operation may be lossy.

                        // *** string values ***

    virtual bdex_InStream& getString(bsl::string& value) = 0;
        // Consume a string from this input stream, place that value in the
        // specified 'value', and return a reference to this modifiable stream.
        // If this stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value, this
        // stream is marked invalid and the value of 'variable' is undefined.

                        // *** arrays of integer values ***

    virtual bdex_InStream&
    getArrayInt64(bsls_PlatformUtil::Int64 *array, int length) = 0;
        // Consume a 64-bit signed integer array of the specified 'length' from
        // this input stream, place that value in the specified 'array', and
        // return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless 0 <= length.

    virtual
    bdex_InStream& getArrayUint64(bsls_PlatformUtil::Uint64 *array,
                                  int                        length) = 0;
        // Consume a 64-bit unsigned integer array of the specified 'length'
        // from this input stream, place that value in the specified 'array',
        // and return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless 0 <= length.

    virtual bdex_InStream&
    getArrayInt56(bsls_PlatformUtil::Int64 *array, int length) = 0;
        // Consume a 56-bit signed integer value from this input stream, place
        // that value in the specified 'variable', and return a reference to
        // this modifiable stream.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.

    virtual bdex_InStream& getArrayUint56(bsls_PlatformUtil::Uint64 *array,
                                          int                        length)=0;
        // Consume a 56-bit unsigned integer value from this input stream,
        // place that value in the specified 'variable', and return a reference
        // to this modifiable stream.  If this stream is initially invalid,
        // this operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.

    virtual bdex_InStream&
    getArrayInt48(bsls_PlatformUtil::Int64 *array, int length) = 0;
        // Consume a 48-bit signed integer value from this input stream, place
        // that value in the specified 'variable', and return a reference to
        // this modifiable stream.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.

    virtual bdex_InStream& getArrayUint48(bsls_PlatformUtil::Uint64 *array,
                                          int                        length)=0;
        // Consume a 48-bit unsigned integer value from this input stream,
        // place that value in the specified 'variable', and return a reference
        // to this modifiable stream.  If this stream is initially invalid,
        // this operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.

    virtual bdex_InStream&
    getArrayInt40(bsls_PlatformUtil::Int64 *array, int length) = 0;
        // Consume a 40-bit signed integer value from this input stream, place
        // that value in the specified 'variable', and return a reference to
        // this modifiable stream.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.

    virtual bdex_InStream& getArrayUint40(bsls_PlatformUtil::Uint64 *array,
                                          int                        length)=0;
        // Consume a 40-bit unsigned integer value from this input stream,
        // place that value in the specified 'variable', and return a reference
        // to this modifiable stream.  If this stream is initially invalid,
        // this operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.

    virtual bdex_InStream& getArrayInt32(int *array, int length) = 0;
        // Consume a 32-bit signed integer array of the specified 'length' from
        // this input stream, place that value in the specified 'array', and
        // return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless 0 <= length.

    virtual bdex_InStream& getArrayUint32(unsigned int *array, int length) = 0;
        // Consume a 32-bit unsigned integer array of the specified 'length'
        // from this input stream, place that value in the specified 'array',
        // and return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless 0 <= length.

    virtual bdex_InStream& getArrayInt24(int *array, int length) = 0;
        // Consume a 24-bit signed integer value from this input stream, place
        // that value in the specified 'variable', and return a reference to
        // this modifiable stream.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.

    virtual bdex_InStream& getArrayUint24(unsigned int *array, int length) = 0;
        // Consume a 24-bit unsigned integer value from this input stream,
        // place that value in the specified 'variable', and return a reference
        // to this modifiable stream.  If this stream is initially invalid,
        // this operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.

    virtual bdex_InStream& getArrayInt16(short *array, int length) = 0;
        // Consume a 16-bit signed integer array of the specified 'length' from
        // this input stream, place that value in the specified 'array', and
        // return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless 0 <= length.

    virtual bdex_InStream& getArrayUint16(unsigned short *array,
                                          int             length) = 0;
        // Consume a 16-bit unsigned integer array of the specified 'length'
        // from this input stream, place that value in the specified 'array',
        // and return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless 0 <= length.

    virtual bdex_InStream& getArrayInt8(char *array, int length) = 0;
    virtual bdex_InStream& getArrayInt8(signed char *array, int length) = 0;
        // Consume an 8-bit signed integer array of the specified 'length' from
        // this input stream, place that value in the specified 'array', and
        // return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless 0 <= length.

    virtual bdex_InStream& getArrayUint8(char *array, int length) = 0;
    virtual bdex_InStream& getArrayUint8(unsigned char *array, int length) = 0;
        // Consume an 8-bit unsigned integer array of the specified 'length'
        // from this input stream, place that value in the specified 'array',
        // and return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless 0 <= length.

                        // *** arrays of floating-point values ***

    virtual bdex_InStream& getArrayFloat64(double *array, int count) = 0;
        // Consume a 64-bit floating-point array of the specified 'length' from
        // this input stream, place that value in the specified 'array', and
        // return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless 0 <= length.  Note that for
        // non-conforming platforms, this operation may be lossy.

    virtual bdex_InStream& getArrayFloat32(float *array, int length) = 0;
        // Consume a 32-bit floating-point array of the specified 'length' from
        // this input stream, place that value in the specified 'array', and
        // return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless 0 <= length.  Note that for
        // non-conforming platforms, this operation may be lossy.

    virtual void invalidate() = 0;
        // Put this input stream in an invalid state.  This function has no
        // effect if this stream is already invalid.  Note that this function
        // should be called whenever a value extracted from this stream is
        // determined to be invalid, inconsistent, or otherwise incorrect.

    // ACCESSORS
    virtual operator const void *() const = 0;
        // Return a non-zero value if this stream is valid, and 0 otherwise.
        // An invalid stream denotes a stream in which insufficient or invalid
        // data was detected during an extraction operation.  Note that an
        // empty stream will be valid unless an extraction attempt or explicit
        // invalidation causes it to be otherwise.

    virtual int isEmpty() const = 0;
        // Return 1 if this stream is empty, and 0 otherwise.  The behavior is
        // undefined unless this stream is valid (i.e., would convert to a
        // non-zero value via the 'operator const void *()' member).  Note that
        // this function enables higher-level components to verify that, after
        // successfully reading all expected data, no data remains.

    virtual int length() const = 0;
        // Return the total number of bytes stored in this stream.

    virtual int cursor() const = 0;
        // Return the index of the next byte to be extracted from this stream.
        // The behavior is undefined unless this stream is valid.
};

// FREE OPERATORS
// TBD #ifndef BDE_OMIT_DEPRECATED

template <typename OBJECT>
inline
bdex_InStream& operator>>(bdex_InStream& stream, OBJECT& object);
    // Read from the specified 'stream', using the 'getVersion' method of this
    // component, a 'bdex' version and assign to the specified 'bdex'-compliant
    // 'object' the value read from 'stream' using 'OBJECT's 'bdex' version
    // format as read from 'stream'.  Return a reference to the modifiable
    // 'stream'.  If 'stream' is initially invalid, this operation has no
    // effect.  If 'stream' becomes invalid during this operation, 'object' is
    // valid, but its value is undefined.  If the read in version is not
    // supported by 'object', 'stream' is marked invalid, but 'object' is
    // unaltered.  The behavior is undefined unless the data in 'stream' was
    // written by a 'bdex_ByteOutStream' object.  (See the 'bdex' package-level
    // documentation for more information on 'bdex' streaming of container
    // types.)  Note that specializations for the supported fundamental
    // (scalar) types and for 'bsl::string' are also provided, in which case
    // this operator calls the appropriate 'get' method from this component.

// TBD #endif  // BDE_OMIT_DEPRECATED

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

// FREE OPERATORS

// TBD #ifndef BDE_OMIT_DEPRECATED

template <typename OBJECT>
inline
bdex_InStream& operator>>(bdex_InStream& stream, OBJECT& object)
{
    return bdex_InStreamMethodsUtil::streamInVersionAndObject(stream, object);
}

// TBD #endif  // BDE_OMIT_DEPRECATED

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
