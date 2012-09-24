// bdex_outstream.h             -*-C++-*-
#ifndef INCLUDED_BDEX_OUTSTREAM
#define INCLUDED_BDEX_OUTSTREAM

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Enable externalization of fundamental types.
//
//@CLASSES:
//   bdex_OutStream: protocol class for externalizing fundamental types
//
//@AUTHOR: John Lakos (jlakos)
//
//@SEE_ALSO: bdex_instream
//
//@DESCRIPTION: This component provides a protocol for a data stream object
// that implements output methods ("externalization") on values (and arrays of
// values) of fundamental types.  Each output method writes either a value or a
// homogeneous array of values of a fundamental type, formatted to cooperate
// with an "unexternalization" component implementing the 'bdex_instream'
// protocol.  The supported types and required content are as follows (note
// that 'Int64' and 'Uint64' are 'typedef' names for the signed and unsigned
// 64-bit integer types, respectively, on the host platform):
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
// This interface also supports compact streaming of integer types.  In
// particular, 64-bit integers can be streamed as 40, 48, 56, or 64-bit values,
// and 32-bit integers can be streamed as 24 or 32-bit values, at the user's
// discretion.  In all cases, the least significant bytes of the fundamental
// integer type are written to the stream.  Note that, for signed types, this
// truncation may not preserve the sign of the streamed value; it is the user's
// responsibility to choose output methods appropriate to the data.
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
///Usage
///-----
// The 'bdex_OutStream' output stream interface provided in this component is
// most commonly used to implement the stream-out functionality for a
// user-defined type.  For example, consider the following 'MyPoint' class:
//..
//  // mypoint.h
//
//  class MyPoint {
//      int d_xCoord;
//      int d_yCoord;
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
//      MyPoint();                                   // [ default ctor needed ]
//          // Create a point at the origin (0, 0).
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
// The following 'mypoint.cpp' file excerpt illustrates an implementation of
// the stream-out and corresponding stream-in functionality.  Note that
// higher-level components that use this 'MyPoint' class may assume the
// existence of 'bdex' streaming functionality in 'MyPoint', and therefore may
// implement their streaming functionality in terms of 'MyPoint' streaming
// functionality.
//..
//  // mypoint.cpp
//  #include <mypoint.h>
//
//  // CLASS METHODS
//  static int MyPoint::maxSupportedBdexVersion()
//  {
//      return 1;
//  }
//
//  // MANIPULATORS
//  template <class STREAM>
//  STREAM& MyPoint::bdexStreamIn(STREAM& stream, int version)
//  {
//      if (stream) {
//          switch (version) {// switch on the schema version (starting with 1)
//            case 1: {
//              bdex_InStreamFunctions::streamIn(stream, d_xCoord, 0);
//              if (!stream) {
//                  return stream;                                    // RETURN
//              }
//
//              bdex_InStreamFunctions::streamIn(stream, d_yCoord, 0);
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
//  STREAM& MyPoint::bdexStreamOut(STREAM& stream, int version) const
//  {
//      if (stream) {
//          switch (version) {// switch on the schema version (starting with 1)
//            case 1: {
//              bdex_OutStreamFunctions::streamOut(stream, d_xCoord, 0);
//              bdex_OutStreamFunctions::streamOut(stream, d_yCoord, 0);
//            } break;
//            default: {
//              stream.invalidate();
//            }
//          }
//      }
//      return stream;
//  }
//..
// Note that when streaming in the externalized value of an object, it is
// usually wise to gather sufficient data in advance in order to guard against
// leaving the object in an inconsistent state due to a corrupted stream.  If
// bad data is detected (e.g., a bad version number), the input stream should
// be invalidated explicitly.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEX_OUTSTREAMMETHODS
#include <bdex_outstreammethods.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {

                        // ====================
                        // class bdex_OutStream
                        // ====================

class bdex_OutStream {
    // This class defines a protocol (i.e., a pure interface) for an output
    // data stream object that enables values, and C-style arrays of values, of
    // the fundamental integral and floating-point types, as well as
    // 'bsl::string' values, to be externalized in some platform-neutral
    // representation.
    //
    // This protocol assumes that short integers are at least 16 bits, integers
    // and floats are at least 32 bits, and 'bsls_PlatformUtil' "64-bit"
    // integers and doubles are at least 64 bits, but makes no other
    // internal-format or stream-format assumptions.  Note that concrete
    // streams following this protocol are free to publish specific formats
    // that must then be strictly implemented on all platforms.

  public:
    // CREATORS
    virtual ~bdex_OutStream();
        // Destroy this output stream.

    // MANIPULATORS
    virtual void invalidate() = 0;
        // Put this output stream in an invalid state.  This function has no
        // effect if this stream is already invalid.  Note that this function
        // should be called whenever a write operation to this stream fails.

    virtual bdex_OutStream& putLength(int length) = 0;
        // Format the specified non-negative 'length' to this output stream and
        // return a reference to this modifiable stream.

    virtual bdex_OutStream& putVersion(int version) = 0;
        // Format the specified non-negative 'version' to this output stream
        // and return a reference to this modifiable stream.

                        // *** scalar integer values ***

    virtual bdex_OutStream& putInt64(bsls_PlatformUtil::Int64 value) = 0;
        // Format the least significant 64 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.

    virtual bdex_OutStream& putUint64(bsls_PlatformUtil::Int64 value) = 0;
        // Format the least significant 64 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.

    virtual bdex_OutStream& putInt56(bsls_PlatformUtil::Int64 value) = 0;
        // Format the least significant 56 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.

    virtual bdex_OutStream& putUint56(bsls_PlatformUtil::Int64 value) = 0;
        // Format the least significant 56 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.

    virtual bdex_OutStream& putInt48(bsls_PlatformUtil::Int64 value) = 0;
        // Format the least significant 48 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.

    virtual bdex_OutStream& putUint48(bsls_PlatformUtil::Int64 value) = 0;
        // Format the least significant 48 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.

    virtual bdex_OutStream& putInt40(bsls_PlatformUtil::Int64 value) = 0;
        // Format the least significant 40 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.

    virtual bdex_OutStream& putUint40(bsls_PlatformUtil::Int64 value) = 0;
        // Format the least significant 40 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.

    virtual bdex_OutStream& putInt32(int value) = 0;
        // Format the least significant 32 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.

    virtual bdex_OutStream& putUint32(unsigned int value) = 0;
        // Format the least significant 32 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.

    virtual bdex_OutStream& putInt24(int value) = 0;
        // Format the least significant 24 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.

    virtual bdex_OutStream& putUint24(int value) = 0;
        // Format the least significant 24 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.

    virtual bdex_OutStream& putInt16(int value) = 0;
        // Format the least significant 16 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.

    virtual bdex_OutStream& putUint16(int value) = 0;
        // Format the least significant 16 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.

    virtual bdex_OutStream& putInt8(int value) = 0;
        // Format the least significant 8 bits of the specified 'value' to this
        // output stream and return a reference to this modifiable stream.

    virtual bdex_OutStream& putUint8(int value) = 0;
        // Format the least significant 8 bits of the specified 'value' to this
        // output stream and return a reference to this modifiable stream.

                        // *** scalar floating-point values ***

    virtual bdex_OutStream& putFloat64(double value) = 0;
        // Format the most significant 64 bits in the specified 'value' to this
        // output stream and return a reference to this modifiable stream.
        // Note that, for non-IEEE-conforming platforms, the "most significant
        // 64 bits" of 'value' might not be contiguous, and that this operation
        // may be lossy.

    virtual bdex_OutStream& putFloat32(float value) = 0;
        // Format the most significant 32 bits in the specified 'value' to this
        // output stream and return a reference to this modifiable stream.
        // Note that, for non-IEEE-conforming platforms, the "most significant
        // 32 bits" of 'value' might not be contiguous, and that this operation
        // may be lossy.

                        // *** string values ***

    virtual bdex_OutStream& putString(const bsl::string& value) = 0;
        // Format the specified 'value' to this output stream and return a
        // reference to this modifiable stream.

                        // *** arrays of integer values ***

    virtual bdex_OutStream&
    putArrayInt64(const bsls_PlatformUtil::Int64 *array, int count) = 0;
        // Format as an atomic sequence the least significant 64 bits of each
        // of the specified 'count' leading entries in the specified 'array' to
        // this output stream and return a reference to this modifiable stream.
        // The behavior is undefined unless 0 <= count.

    virtual bdex_OutStream&
    putArrayUint64(const bsls_PlatformUtil::Uint64 *array, int count) = 0;
        // Format as an atomic sequence the least significant 64 bits of each
        // of the specified 'count' leading entries in the specified 'array' to
        // this output stream and return a reference to this modifiable stream.
        // The behavior is undefined unless 0 <= count.

    virtual bdex_OutStream&
    putArrayInt56(const bsls_PlatformUtil::Int64 *array, int count) = 0;
        // Format as an atomic sequence the least significant 56 bits of each
        // of the specified 'count' leading entries in the specified 'array' to
        // this output stream and return a reference to this modifiable stream.
        // The behavior is undefined unless 0 <= count.

    virtual bdex_OutStream&
    putArrayUint56(const bsls_PlatformUtil::Uint64 *array, int count) = 0;
        // Format as an atomic sequence the least significant 56 bits of each
        // of the specified 'count' leading entries in the specified 'array' to
        // this output stream and return a reference to this modifiable stream.
        // The behavior is undefined unless 0 <= count.

    virtual bdex_OutStream&
    putArrayInt48(const bsls_PlatformUtil::Int64 *array, int count) = 0;
        // Format as an atomic sequence the least significant 48 bits of each
        // of the specified 'count' leading entries in the specified 'array' to
        // this output stream and return a reference to this modifiable stream.
        // The behavior is undefined unless 0 <= count.

    virtual bdex_OutStream&
    putArrayUint48(const bsls_PlatformUtil::Uint64 *array, int count) = 0;
        // Format as an atomic sequence the least significant 48 bits of each
        // of the specified 'count' leading entries in the specified 'array' to
        // this output stream and return a reference to this modifiable stream.
        // The behavior is undefined unless 0 <= count.

    virtual bdex_OutStream&
    putArrayInt40(const bsls_PlatformUtil::Int64 *array, int count) = 0;
        // Format as an atomic sequence the least significant 40 bits of each
        // of the specified 'count' leading entries in the specified 'array' to
        // this output stream and return a reference to this modifiable stream.
        // The behavior is undefined unless 0 <= count.

    virtual bdex_OutStream&
    putArrayUint40(const bsls_PlatformUtil::Uint64 *array, int count) = 0;
        // Format as an atomic sequence the least significant 40 bits of each
        // of the specified 'count' leading entries in the specified 'array' to
        // this output stream and return a reference to this modifiable stream.
        // The behavior is undefined unless 0 <= count.

    virtual bdex_OutStream& putArrayInt32(const int *array, int count) = 0;
        // Format as an atomic sequence the least significant 32 bits of each
        // of the specified 'count' leading entries in the specified 'array' to
        // this output stream and return a reference to this modifiable stream.
        // The behavior is undefined unless 0 <= count.

    virtual
    bdex_OutStream& putArrayUint32(const unsigned int *array, int count) = 0;
        // Format as an atomic sequence the least significant 32 bits of each
        // of the specified 'count' leading entries in the specified 'array' to
        // this output stream and return a reference to this modifiable stream.
        // The behavior is undefined unless 0 <= count.

    virtual bdex_OutStream& putArrayInt24(const int *array, int count) = 0;
        // Format as an atomic sequence the least significant 24 bits of each
        // of the specified 'count' leading entries in the specified 'array' to
        // this output stream and return a reference to this modifiable stream.
        // The behavior is undefined unless 0 <= count.

    virtual
    bdex_OutStream& putArrayUint24(const unsigned int *array, int count) = 0;
        // Format as an atomic sequence the least significant 24 bits of each
        // of the specified 'count' leading entries in the specified 'array' to
        // this output stream and return a reference to this modifiable stream.
        // The behavior is undefined unless 0 <= count.

    virtual bdex_OutStream& putArrayInt16(const short *array, int count) = 0;
        // Format as an atomic sequence the least significant 16 bits of each
        // of the specified 'count' leading entries in the specified 'array' to
        // this output stream and return a reference to this modifiable stream.
        // The behavior is undefined unless 0 <= count.

    virtual
    bdex_OutStream& putArrayUint16(const unsigned short *array, int count) = 0;
        // Format as an atomic sequence the least significant 16 bits of each
        // of the specified 'count' leading entries in the specified 'array' to
        // this output stream and return a reference to this modifiable stream.
        // The behavior is undefined unless 0 <= count.

    virtual bdex_OutStream& putArrayInt8(const char *array, int count) = 0;
    virtual bdex_OutStream& putArrayInt8(const signed char *array,
                                         int                count) = 0;
        // Format as an atomic sequence the least significant 8 bits of each of
        // the specified 'count' leading entries in the specified 'array' to
        // this output stream and return a reference to this modifiable stream.
        // The behavior is undefined unless 0 <= count.

    virtual bdex_OutStream& putArrayUint8(const char *array, int count) = 0;
    virtual bdex_OutStream& putArrayUint8(const unsigned char *array,
                                          int                  count) = 0;
        // Format as an atomic sequence the least significant 8 bits of each of
        // the specified 'count' leading entries in the specified 'array' to
        // this output stream and return a reference to this modifiable stream.
        // The behavior is undefined unless 0 <= count.

                        // *** arrayed floating-point values ***

    virtual bdex_OutStream& putArrayFloat64(const double *array,int count) = 0;
        // Format as an atomic sequence the most significant 64 bits in each of
        // the specified 'count' leading entries in the specified 'array' to
        // this output stream and return a reference to this modifiable stream.
        // The behavior is undefined unless 0 <= count.  Note that, for
        // non-IEEE-conforming platforms, the "most significant 64 bits" might
        // not be contiguous, and that this operation may be lossy.

    virtual bdex_OutStream& putArrayFloat32(const float *array, int count) = 0;
        // Format as an atomic sequence the most significant 32 bits in each of
        // the specified 'count' leading entries in the specified 'array' to
        // this output stream and return a reference to this modifiable stream.
        // The behavior is undefined unless 0 <= count.  Note that, for
        // non-IEEE-conforming platforms, the "most significant 4 bytes" might
        // not be contiguous, and that this operation may be lossy.

    virtual void removeAll() = 0;
        // Remove all content in this stream.

    virtual void reserveCapacity(int newCapacity) = 0;
        // Set the internal buffer size of this stream to be at least the
        // specified 'newCapacity'.

    // ACCESSORS
    virtual operator const void *() const = 0;
        // Return a non-zero value if this stream is valid, and 0 otherwise.
        // An invalid stream is a stream for which an output operation was
        // detected to have failed.

    virtual const char *data() const = 0;
        // Return the address of the contiguous, non-modifiable internal memory
        // buffer of this stream.  The address will remain valid as long as
        // this stream is not destroyed or modified.  The behavior of accessing
        // elements outside the range [ data() .. data() + (length() - 1) ] is
        // undefined.

    virtual int length() const = 0;
        // Return the number of bytes in this stream.
};

// FREE OPERATORS
// TBD #ifndef BDE_OMIT_DEPRECATED

template <class OBJECT>
inline
bdex_OutStream& operator<<(bdex_OutStream& stream, const OBJECT& object);
    // Write to the specified output 'stream', using the 'putVersion' method of
    // this component, the most current version number supported by 'OBJECT'
    // and format the value of the specified 'object' to 'stream' using the
    // 'bdex' compliant 'OBJECT::bdexStreamOut' method in the
    // 'OBJECT::maxSupportedBdexVersion' format.  Return a reference to the
    // modifiable 'stream'.  Note that specializations for the supported
    // fundamental (scalar) types and for 'bsl::string' are also provided, in
    // which case this operator calls the appropriate 'put' method from this
    // component.

// TBD #endif  // BDE_OMIT_DEPRECATED

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

// FREE OPERATORS
// TBD #ifndef BDE_OMIT_DEPRECATED

template <class OBJECT>
inline
bdex_OutStream& operator<<(bdex_OutStream& stream, const OBJECT& object)
{
    return bdex_OutStreamMethodsUtil::streamOutVersionAndObject(stream,
                                                                object);
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
