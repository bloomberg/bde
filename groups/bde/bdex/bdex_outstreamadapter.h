// bdex_outstreamadapter.h                                            -*-C++-*-
#ifndef INCLUDED_BDEX_OUTSTREAMADAPTER
#define INCLUDED_BDEX_OUTSTREAMADAPTER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide parameterized adapter for streaming out polymorphic types.
//
//@CLASSES:
//  bdex_outstreamadapter: adapter for streaming out of polymorphic types
//
//@AUTHOR: Rohan Bhindwale (rbhindwa)
//
//@SEE_ALSO: bdex_outstream, bdex_instreamadapter
//
//@DESCRIPTION: This component contains a parameterized adapter class,
// 'bdex_OutStreamAdapter', that provides a mechanism for externalizing of
// polymorphic classes using the 'bdex' protocol.  (Refer to the 'bdex' package
// level doc for detailed information on 'bdex' streaming).
//
// User-defined classes supporting 'bdex' streaming must contain 'bdexStreamIn'
// and 'bdexStreamOut' methods that are parameterized on the 'STREAM' type.
// Additionally, if the user-defined type is a protocol class, these functions
// need to be virtual so that derived classes can implement appropriate
// streaming functionality.  That is a problem as the C++ language disallows
// parameterized virtual functions.  To solve this problem, authors of
// polymorphic classes who want the concrete derived types to be 'bdex'
// streamable must use the 'bdex_OutStreamAdapter' template class defined in
// this component (and the 'bdex_InStreamAdapter' template class defined in
// 'bdex_instreamadapter'), and in particular must define the 'virtual'
// 'streamIn' and 'streamOut' methods as shown in the Usage Example below.
// Clients of the polymorphic types can then stream values of these types
// exactly as if they were simple concrete types supporting 'bdex'.
//
// Refer to the protocol diagram below for an introduction to this class.
//..
//                                           ,-----------------------.
//      (STREAM template     .............. | template <class STREAM> |
//       argument)           :   :   :      |  bdex_OutStreamAdapter  |
//                           :   :   :       `-----------------------'
//         ,------------------.  :   :                    |
//        ( bdex_ByteOutStream ) :   :                    |
//         `------------------'  :   :                    |
//                               :   :                    V
//          ,---------------------.  :             ,--------------.
//         ( bdex_ByteOutStreamRaw ) :            ( bdex_OutStream )
//          `---------------------'  :             `--------------'
//                                   :                       Protocol class
//                 ,------------------.                      providing
//                ( bdex_ByteOutStream )                     externalization
//                 `------------------'                      interface.
//                           .
//                           .
//                           .
//                      Output streams
//                      implementing the
//                      'bdex_OutStream'
//                      (documentation)
//                      protocol
//..
// The 'bdex_OutStreamAdapter' class is a concrete implementation of the
// 'bdex_OutStream' protocol parameterized on the 'STREAM' type.  The
// adapter class can be instantiated with any stream class implementing the
// 'bdex_OutStream' protocol.  An output adapter is created by supplying an
// instance of the parameterized 'STREAM' type at construction.  The adapter
// then acts as a wrapper around the held stream object and forwards all 'put'
// data requests to it.  Additionally, because 'bdex_OutStreamAdapter' is a
// concrete implementation of the 'bdex_OutStream' protocol, it allows run
// time polymorphism based on the  'bdex_OutStream' protocol class.
//
// Every protocol class that supports 'bdex' streaming should implement the
// parameterized 'bdexStreamIn' and 'bdexStreamOut' functions *and* also
// define virtual 'streamOut' and 'streamIn' functions.  The signature for
// the 'streamOut' and 'streamIn' functions should be as follows:
//..
//  virtual bdex_OutStream& streamOut(bdex_OutStream& stream,
//                                    int             version) const = 0;
//      // Write this value to the specified output 'stream' using the
//      // specified 'version' format and return a reference to the
//      // modifiable 'stream'.  If 'version' is not supported, 'stream'
//      // is unmodified.  Note that 'version' is not written to 'stream'.
//      // See the 'bdex' package-level documentation for more information
//      // on 'bdex' streaming of polymorphic types.
//
//  virtual bdex_InStream& streamIn(bdex_InStream& stream,
//                                  int            version) = 0;
//      // Assign to this object the value read from the specified input
//      // 'stream' using the specified 'version' format and return a
//      // reference to the modifiable 'stream'.  If 'stream' is initially
//      // invalid, this operation has no effect.  If 'stream' becomes
//      // invalid during this operation, this object is valid, but its
//      // value is undefined.  If 'version' is not supported, 'stream' is
//      // marked invalid and this object is unaltered.  Note that no
//      // version is read from 'stream'.  See the 'bdex' package-level
//      // documentation for more information on 'bdex' streaming of
//      // polymorphic types.
//..
// The implementation of the 'bdexStreamOut' and 'bdexStreamIn' functions in
// the protocol class will be as follows:
//..
//  STREAM& protocolClass::bdexStreamOut(STREAM& stream, int version) const
//  {
//      bdex_OutStreamAdapter<STREAM> outAdapter(&stream);
//      streamOut(outAdapter, version);
//  }
//
//  STREAM& protocolClass::bdexStreamIn(STREAM& stream, int version) const
//  {
//      bdex_InStreamAdapter<STREAM> inAdapter(&stream);
//      streamIn(inAdapter, version);
//  }
//..
//
///Usage
///-----
// The 'bdex_OutStreamAdapter' output stream interface provided in this
// component is most commonly used to implement the stream-out functionality
// for a polymorphic type.  For example, consider the following 'myShape'
// protocol class:
//..
//  // myshape.h
//  #include <bdex_instreamadapter.h>
//  #include <bdex_outstreamadapter.h>
//
//  class bdex_OutStream;
//  class bdex_InStream;
//
//  class myShape {
//      // This 'class' acts as a protocol class for displaying various shapes.
//
//    public:
//      // CREATORS
//      virtual ~myShape();
//          // Destroy this shape.
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
//          // value-semantic types and containers.
//
//      virtual bdex_InStream& streamIn(bdex_InStream& stream,
//                                      int            version) = 0;
//          // Assign to this object the value read from the specified input
//          // 'stream' using the specified 'version' format and return a
//          // reference to the modifiable 'stream'.  If 'stream' is initially
//          // invalid, this operation has no effect.  If 'stream' becomes
//          // invalid during this operation, this object is valid, but its
//          // value is undefined.  If 'version' is not supported, 'stream' is
//          // marked invalid and this object is unaltered.  Note that no
//          // version is read from 'stream'.  See the 'bdex' package-level
//          // documentation for more information on 'bdex' streaming of
//          // polymorphic types.
//
//      // ACCESSORS
//      virtual void draw() const = 0;
//          // Draw this shape.
//
//      virtual int maxSupportedBdexVersion() const = 0;
//          // Return the max supported 'bdex' version.
//
//      template <class STREAM>
//      STREAM& bdexStreamOut(STREAM& stream, int version) const;
//          // Write this value to the specified output 'stream' using the
//          // specified 'version' format and return a reference to the
//          // modifiable 'stream'.  If 'version' is not supported, 'stream'
//          // is unmodified.  Note that 'version' is not written to 'stream'.
//          // See the 'bdex' package-level documentation for more information
//          // on 'bdex' streaming of value-semantic types and containers.
//
//      virtual bdex_OutStream& streamOut(bdex_OutStream& stream,
//                                        int             version) const = 0;
//          // Write this value to the specified output 'stream' using the
//          // specified 'version' format and return a reference to the
//          // modifiable 'stream'.  If 'version' is not supported, 'stream'
//          // is unmodified.  Note that 'version' is not written to 'stream'.
//          // See the 'bdex' package-level documentation for more information
//          // on 'bdex' streaming of polymorphic types.
//  };
//
//..
// Note that the 'myShape' class contains the 'streamIn' and 'streamOut'
// virtual functions in addition to the parameterized 'bdexStreamIn' and
// 'bdexStreamOut' functions.  The implementation of the parameterized 'bdex'
// functions is provided below.
//..
//  // INLINE FUNCTION DEFINITIONS
//
//  // CREATORS
//  inline
//  myShape::~myShape()
//  {
//  }
//
//  // MANIPULATORS
//  template <class STREAM>
//  STREAM& myShape::bdexStreamIn(STREAM& stream, int version)
//  {
//      bdex_InStreamAdapter<STREAM> inAdapter(&stream);
//      streamIn(inAdapter, version);
//      return stream;
//  }
//
//  // ACCESSORS
//  template <class STREAM>
//  STREAM& myShape::bdexStreamOut(STREAM& stream, int version) const
//  {
//      bdex_OutStreamAdapter<STREAM> outAdapter(&stream);
//      streamOut(outAdapter, version);
//      return stream;
//  }
//..
// Now consider a concrete implementation of the above protocol class, namely
// the 'myCircle' class.  Provided below is the interface of that class.
//..
//  // mycircle.h
//
//  # include <myshape.h>
//
//  class bdex_OutStream;
//  class bdex_InStream;
//
//  class myCircle : public myShape{
//      // This 'class' provides functions to draw a circle.
//
//      int d_radius;    // radius of this circle
//      int d_x;         // x-coordinate of center
//      int d_y;         // y-coordinate of center
//
//    public:
//      // CREATORS
//      myCircle();
//          // Create a default circle object of zero radius with its center at
//          // (0,0).
//
//      myCircle(int x, int y, int radius);
//          // Create a circle centered at the specified 'x' and 'y' values and
//          // having the specified 'radius'.
//
//      virtual ~myCircle();
//          // Destroy this circle.
//
//      // MANIPULATORS
//      virtual bdex_InStream& streamIn(bdex_InStream& stream, int version);
//          // Assign to this object the value read from the specified input
//          // 'stream' using the specified 'version' format and return a
//          // reference to the modifiable 'stream'.  If 'stream' is initially
//          // invalid, this operation has no effect.  If 'stream' becomes
//          // invalid during this operation, this object is valid, but its
//          // value is undefined.  If 'version' is not supported, 'stream' is
//          // marked invalid and this object is unaltered.  Note that no
//          // version is read from 'stream'.  See the 'bdex' package-level
//          // documentation for more information on 'bdex' streaming of
//          // polymorphic types.
//
//      // ACCESSORS
//      virtual int maxSupportedBdexVersion() const;
//          // Return the max supported 'bdex' version.
//
//      virtual void draw() const;
//          // Draw this shape.
//
//      virtual bdex_OutStream& streamOut(bdex_OutStream& stream,
//                                        int             version) const;
//          // Write this value to the specified output 'stream' using the
//          // specified 'version' format and return a reference to the
//          // modifiable 'stream'.  If 'version' is not supported, 'stream'
//          // is unmodified.  Note that 'version' is not written to 'stream'.
//          // See the 'bdex' package-level documentation for more information
//          // on 'bdex' streaming of polymorphic types.
//
//      int radius() const;
//          // Return the radius of this circle.
//
//      int x() const;
//          // Return the x-coordinate of the center of this circle.
//
//      int y() const;
//          // Return the y-coordinate of the center of this circle.
//  };
//
// // INLINE FUNCTION DEFINITIONS
//
// // CREATORS
// inline
// myCircle::myCircle()
// : d_radius(0)
// , d_x(0)
// , d_y(0)
// {
// }
//
// inline
// myCircle::myCircle(int radius, int x, int y)
// : d_radius(radius)
// , d_x(x)
// , d_y(y)
// {
// }
//
// inline
// myCircle::~myCircle()
// {
// }
//
// inline
// int myCircle::radius() const
// {
//     return d_radius;
// }
//
// inline
// int myCircle::x() const
// {
//     return d_x;
// }
//
// inline
// int myCircle::y() const
// {
//     return d_y;
// }
//..
// Again, note that the 'myCircle' class implements the 'streamIn' and
// 'streamOut' virtual functions derived from it's base class.  This will hold
// true for all classes derived from a 'bdex' streaming compliant protocol
// class.
//
// Now for the implementation of the 'myCircle' class.
//..
//  // mycircle.cpp
//
//  #include <mycircle.h>
//  #include <bdex_instream.h>
//  #include <bdex_outstream.h>
//
//  // MANIPULATORS
//  bdex_InStream& myCircle::streamIn(bdex_InStream& stream, int version)
//  {
//      if (stream) {
//          switch (version) { // switch on the schema version
//            case 1: {
//              bdex_InStreamFunctions::streamIn(stream, d_radius, 0);
//              bdex_InStreamFunctions::streamIn(stream, d_x, 0);
//              bdex_InStreamFunctions::streamIn(stream, d_y, 0);
//            } break;
//            default: {
//              stream.invalidate();            // bad stream data
//            }
//          }
//      }
//      return stream;                                          // RETURN
//  }
//
//  // ACCESSORS
//  int myCircle::maxSupportedBdexVersion() const
//  {
//      return 1;
//  }
//
//  void myCircle::draw() const
//  {
//      // Code for drawing the circle
//  }
//
//  bdex_OutStream& myCircle::streamOut(bdex_OutStream& stream,
//                                      int             version) const
//  {
//      if (!stream) {
//          stream.invalidate();
//          return stream;
//      }
//
//      switch (version) { // switch on the schema version
//        case 1: {
//          bdex_OutStreamFunctions::streamOut(stream, d_radius, 0);
//          bdex_OutStreamFunctions::streamOut(stream, d_x, 0);
//          bdex_OutStreamFunctions::streamOut(stream, d_y, 0);
//        } break;
//        default: {
//          stream.invalidate();            // bad stream data
//        }
//      }
//
//      return stream;
//  }
//..
// Now we are ready to stream out our polymorphic 'myShape' class.
// Refer below to an application that does that.
//..
//  // myapplication.cpp
//
//  #include <mycircle.h>
//  #include <myshape.h>
//  #include <bdex_byteoutstream.h>
//  #include <bdex_byteinstream.h>
//
//  int main() {
//..
// First we create two circles.  'circle1' is created with a pre-decided radius
// and center coordinates.  'circle2' is created with default radius and
// center coordinates.
//..
//     myCircle circle1(3, 6, 10);
//     myCircle circle2;
//
//     assert(circle1.radius() != circle2.radius());
//     assert(circle1.x()      != circle2.x());
//     assert(circle1.y()      != circle2.y());
//..
// Then we bind two 'myShape' references to the two circle objects.
//..
//     myShape& shape1 = circle1;
//     myShape& shape2 = circle2;
//..
// Next, we create an output stream 'os' to write data, and call the
// 'bdexStreamOut' function for the 'shape1' reference.  This will result in
// the streaming out of the 'circle1' object.
//..
//     bdex_ByteOutStream os;
//     const int VERSION = 1;
//     os.putVersion(VERSION);
//     bdex_OutStreamFunctions::streamOut(os, shape1, VERSION);
//..
// Then we create an input stream 'is' and pass it the output stream buffer.
// We then stream in from 'is' into 'shape2'.  Once that is done the default
// constructed 'circle2' object will have the same radius and center
// coordinates as 'circle1'.
//..
//     bdex_ByteInStream  is(os.data(), os.length());
//     int version;
//     is.getVersion(version);
//     bdex_InStreamFunctions::streamIn(is, shape2, version);
//
//     assert(circle1.radius() == circle2.radius());
//     assert(circle1.x()      == circle2.x());
//     assert(circle1.y()      == circle2.y());
//  }
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEX_OUTSTREAM
#include <bdex_outstream.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {

                        // ===========================
                        // class bdex_OutStreamAdapter
                        // ===========================

template <class STREAM>
class bdex_OutStreamAdapter : public bdex_OutStream {
    // This 'class' provides a mechanism to externalize polymorphic types using
    // the 'bdex' protocol.  It efficiently forwards all the externalization
    // requests to the stream it was constructed with.
    //
    // Note that this class is decidedly *not* value-semantic, and instead has
    // *reference* *semantics*.  There is no conventional notion of assignment,
    // copy construction or equality.

    STREAM *d_stream_p;  // pointer to stream (held, not owned)

  public:

    // CREATORS
    bdex_OutStreamAdapter(STREAM *stream);
        // Create an output stream adapter.

    virtual ~bdex_OutStreamAdapter();
        // Destroy this output stream adapter.

    // MANIPULATORS
    virtual void invalidate();
        // Put this output stream in an invalid state.  This function has no
        // effect if this stream is already invalid.  Note that this function
        // should be called whenever a write operation to this stream fails.

    virtual bdex_OutStream& putLength(int length);
        // Format the specified non-negative 'length' to this output stream and
        // return a reference to this modifiable stream.
        //
        // If 'length' is less than 128, then write the least significant byte
        // of 'length'.  Otherwise, write the least significant four bytes of
        // 'length' in network byte order, but with the most significant output
        // bit set to 1.

    virtual bdex_OutStream& putVersion(int version);
        // Format the specified non-negative 'version' to this output stream
        // and return a reference to this modifiable stream.
        //
        // Write the least significant byte of 'version'.

                        // *** scalar integer values ***

    virtual bdex_OutStream& putInt64(bsls_PlatformUtil::Int64 value);
        // Format the least significant 64 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.
        //
        // Write the least significant eight bytes of 'value' in network byte
        // order.

    virtual bdex_OutStream& putUint64(bsls_PlatformUtil::Int64 value);
        // Format the least significant 64 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.
        //
        // Write the least significant eight bytes of 'value' in network byte
        // order.

    virtual bdex_OutStream& putInt56(bsls_PlatformUtil::Int64 value);
        // Format the least significant 56 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.
        //
        // Write the least significant seven bytes of 'value' in network byte
        // order.

    virtual bdex_OutStream& putUint56(bsls_PlatformUtil::Int64 value);
        // Format the least significant 56 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.
        //
        // Write the least significant seven bytes of 'value' in network byte
        // order.

    virtual bdex_OutStream& putInt48(bsls_PlatformUtil::Int64 value);
        // Format the least significant 48 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.
        //
        // Write the least significant six bytes of 'value' in network byte
        // order.

    virtual bdex_OutStream& putUint48(bsls_PlatformUtil::Int64 value);
        // Format the least significant 48 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.
        //
        // Write the least significant six bytes of 'value' in network byte
        // order.

    virtual bdex_OutStream& putInt40(bsls_PlatformUtil::Int64 value);
        // Format the least significant 40 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.
        //
        // Write the least significant five bytes of 'value' in network byte
        // order.

    virtual bdex_OutStream& putUint40(bsls_PlatformUtil::Int64 value);
        // Format the least significant 40 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.
        //
        // Write the least significant five bytes of 'value' in network byte
        // order.

    virtual bdex_OutStream& putInt32(int value);
        // Format the least significant 32 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.
        //
        // Write the least significant four bytes of 'value' in network byte
        // order.

    virtual bdex_OutStream& putUint32(unsigned int value);
        // Format the least significant 32 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.
        //
        // Write the least significant four bytes of 'value' in network byte
        // order.

    virtual bdex_OutStream& putInt24(int value);
        // Format the least significant 24 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.
        //
        // Write the least significant three bytes of 'value' in network byte
        // order.

    virtual bdex_OutStream& putUint24(int value);
        // Format the least significant 24 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.
        //
        // Write the least significant three bytes of 'value' in network byte
        // order.

    virtual bdex_OutStream& putInt16(int value);
        // Format the least significant 16 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.
        //
        // Write the least significant two bytes of 'value' in network byte
        // order.

    virtual bdex_OutStream& putUint16(int value);
        // Format the least significant 16 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.
        //
        // Write the least significant two bytes of 'value' in network byte
        // order.

    virtual bdex_OutStream& putInt8(int value);
        // Format the least significant 8 bits of the specified 'value' to this
        // output stream and return a reference to this modifiable stream.
        //
        // Write the least significant byte of 'value'.

    virtual bdex_OutStream& putUint8(int value);
        // Format the least significant 8 bits of the specified 'value' to this
        // output stream and return a reference to this modifiable stream.
        //
        // Write the least significant byte of 'value'.

                        // *** scalar floating-point values ***

    virtual bdex_OutStream& putFloat64(double value);
        // Format the most significant 64 bits in the specified 'value' to this
        // output stream and return a reference to this modifiable stream.
        // Note that, for non-IEEE-conforming platforms, the "most significant
        // 64 bits" of 'value' might not be contiguous, and that this operation
        // may be lossy.
        //
        // Format 'value' as an IEEE double-precision floating point number
        // and write those eight bytes in network byte order.

    virtual bdex_OutStream& putFloat32(float value);
        // Format the most significant 32 bits in the specified 'value' to this
        // output stream and return a reference to this modifiable stream.
        // Note that, for non-IEEE-conforming platforms, the "most significant
        // 32 bits" of 'value' might not be contiguous, and that this operation
        // may be lossy.
        //
        // Format 'value' as an IEEE single-precision floating point number
        // and write those four bytes in network byte order.

                        // *** string values ***

    virtual bdex_OutStream& putString(const bsl::string& value);
        // Format the specified 'value' to this output stream and return a
        // reference to this modifiable stream.
        //
        // Write the length of the specified 'value' using the 'putLength'
        // format, then write each character of 'value' as a one-byte integer.

                        // *** arrays of integer values ***

    virtual bdex_OutStream&
    putArrayInt64(const bsls_PlatformUtil::Int64 *array, int length);
        // Format as an atomic sequence the least significant 64 bits of each
        // of the specified 'length' leading entries in the specified 'array'
        // to this output stream and return a reference to this modifiable
        // stream.  The behavior is undefined unless 0 <= length.
        //
        // For each element of 'array', write the least significant eight bytes
        // in network byte order.

    virtual bdex_OutStream&
    putArrayUint64(const bsls_PlatformUtil::Uint64 *array,
                   int                              length);
        // Format as an atomic sequence the least significant 64 bits of each
        // of the specified 'length' leading entries in the specified 'array'
        // to this output stream and return a reference to this modifiable
        // stream.  The behavior is undefined unless 0 <= length.
        //
        // For each element of 'array', write the least significant eight bytes
        // in network byte order.

    virtual bdex_OutStream&
    putArrayInt56(const bsls_PlatformUtil::Int64 *array,
                  int                             length);
        // Format as an atomic sequence the least significant 56 bits of each
        // of the specified 'length' leading entries in the specified 'array'
        // to this output stream and return a reference to this modifiable
        // stream.  The behavior is undefined unless 0 <= length.
        //
        // For each element of 'array', write the least significant seven bytes
        // in network byte order.

    virtual bdex_OutStream&
    putArrayUint56(const bsls_PlatformUtil::Uint64 *array,
                   int                              length);
        // Format as an atomic sequence the least significant 56 bits of each
        // of the specified 'length' leading entries in the specified 'array'
        // to this output stream and return a reference to this modifiable
        // stream.  The behavior is undefined unless 0 <= length.
        //
        // For each element of 'array', write the least significant seven bytes
        // in network byte order.

    virtual bdex_OutStream&
    putArrayInt48(const bsls_PlatformUtil::Int64 *array,
                  int                             length);
        // Format as an atomic sequence the least significant 48 bits of each
        // of the specified 'length' leading entries in the specified 'array'
        // to this output stream and return a reference to this modifiable
        // stream.  The behavior is undefined unless 0 <= length.
        //
        // For each element of 'array', write the least significant six bytes
        // in network byte order.

    virtual bdex_OutStream&
    putArrayUint48(const bsls_PlatformUtil::Uint64 *array,
                   int                              length);
        // Format as an atomic sequence the least significant 48 bits of each
        // of the specified 'length' leading entries in the specified 'array'
        // to this output stream and return a reference to this modifiable
        // stream.  The behavior is undefined unless 0 <= length.
        //
        // For each element of 'array', write the least significant six bytes
        // in network byte order.

    virtual bdex_OutStream&
    putArrayInt40(const bsls_PlatformUtil::Int64 *array,
                  int                             length);
        // Format as an atomic sequence the least significant 40 bits of each
        // of the specified 'length' leading entries in the specified 'array'
        // to this output stream and return a reference to this modifiable
        // stream.  The behavior is undefined unless 0 <= length.
        //
        // For each element of 'array', write the least significant five bytes
        // in network byte order.

    virtual bdex_OutStream&
    putArrayUint40(const bsls_PlatformUtil::Uint64 *array,
                   int                              length);
        // Format as an atomic sequence the least significant 40 bits of each
        // of the specified 'length' leading entries in the specified 'array'
        // to this output stream and return a reference to this modifiable
        // stream.  The behavior is undefined unless 0 <= length.
        //
        // For each element of 'array', write the least significant five bytes
        // in network byte order.

    virtual bdex_OutStream& putArrayInt32(const int *array, int length);
        // Format as an atomic sequence the least significant 32 bits of each
        // of the specified 'length' leading entries in the specified 'array'
        // to this output stream and return a reference to this modifiable
        // stream.  The behavior is undefined unless 0 <= length.
        //
        // For each element of 'array', write the least significant four bytes
        // in network byte order.

    virtual bdex_OutStream&
    putArrayUint32(const unsigned int *array, int length);
        // Format as an atomic sequence the least significant 32 bits of each
        // of the specified 'length' leading entries in the specified 'array'
        // to this output stream and return a reference to this modifiable
        // stream.  The behavior is undefined unless 0 <= length.
        //
        // For each element of 'array', write the least significant four bytes
        // in network byte order.

    virtual bdex_OutStream& putArrayInt24(const int *array, int length);
        // Format as an atomic sequence the least significant 24 bits of each
        // of the specified 'length' leading entries in the specified 'array'
        // to this output stream and return a reference to this modifiable
        // stream.  The behavior is undefined unless 0 <= length.
        //
        // For each element of 'array', write the least significant three bytes
        // in network byte order.

    virtual bdex_OutStream& putArrayUint24(const unsigned int *array,
                                           int                 length);
        // Format as an atomic sequence the least significant 24 bits of each
        // of the specified 'length' leading entries in the specified 'array'
        // to this output stream and return a reference to this modifiable
        // stream.  The behavior is undefined unless 0 <= length.
        //
        // For each element of 'array', write the least significant three bytes
        // in network byte order.

    virtual bdex_OutStream& putArrayInt16(const short *array, int length);
        // Format as an atomic sequence the least significant 16 bits of each
        // of the specified 'length' leading entries in the specified 'array'
        // to this output stream and return a reference to this modifiable
        // stream.  The behavior is undefined unless 0 <= length.
        //
        // For each element of 'array', write the least significant two bytes
        // in network byte order.

    virtual bdex_OutStream& putArrayUint16(const unsigned short *array,
                                           int                   length);
        // Format as an atomic sequence the least significant 16 bits of each
        // of the specified 'length' leading entries in the specified 'array'
        // to this output stream and return a reference to this modifiable
        // stream.  The behavior is undefined unless 0 <= length.
        //
        // For each element of 'array', write the least significant two bytes
        // in network byte order.

    virtual bdex_OutStream& putArrayInt8(const char *array, int length);
    virtual bdex_OutStream& putArrayInt8(const signed char *array, int length);
        // Format as an atomic sequence the least significant 8 bits of each of
        // the specified 'length' leading entries in the specified 'array' to
        // this output stream and return a reference to this modifiable stream.
        // The behavior is undefined unless 0 <= length.
        //
        // Write each byte of 'array' in order.

    virtual bdex_OutStream& putArrayUint8(const char *array, int length);
    virtual bdex_OutStream&
    putArrayUint8(const unsigned char *array, int length);
        // Format as an atomic sequence the least significant 8 bits of each of
        // the specified 'length' leading entries in the specified 'array' to
        // this output stream and return a reference to this modifiable stream.
        // The behavior is undefined unless 0 <= length.
        //
        // Write each byte of 'array' in order.

                        // *** arrays of floating-point values ***

    virtual bdex_OutStream& putArrayFloat64(const double *array, int length);
        // Format as an atomic sequence the most significant 64 bits in each of
        // the specified 'length' leading entries in the specified 'array' to
        // this output stream and return a reference to this modifiable stream.
        // The behavior is undefined unless 0 <= length.  Note that, for
        // non-IEEE-conforming platforms, the "most significant 64 bits" might
        // not be contiguous, and that this operation may be lossy.
        //
        // For each element of 'array', format the value as an IEEE
        // double-precision floating point number and write those eight bytes
        // in network byte order.

    virtual bdex_OutStream& putArrayFloat32(const float *array, int length);
        // Format as an atomic sequence the most significant 32 bits in each of
        // the specified 'length' leading entries in the specified 'array' to
        // this output stream and return a reference to this modifiable stream.
        // The behavior is undefined unless 0 <= length.  Note that, for
        // non-IEEE-conforming platforms, the "most significant 32 bits" might
        // not be contiguous, and that this operation may be lossy.
        //
        // For each element of 'array', format the value as an IEEE
        // single-precision floating point number and write those four bytes in
        // network byte order.

    virtual void removeAll();
        // Remove all content in this stream.

    virtual void reserveCapacity(int newCapacity);
        // Set the internal buffer size of this stream to be at least the
        // specified 'newCapacity'.

    // ACCESSORS
    virtual operator const void *() const;
        // Return a non-zero value if this stream is valid, and 0 otherwise.
        // An invalid stream is a stream for which an output operation was
        // detected to have failed.

    virtual const char *data() const;
        // Return the address of the contiguous, non-modifiable internal memory
        // buffer of this stream.  The address will remain valid as long as
        // this stream is not destroyed or modified.  The behavior of accessing
        // elements outside the range [ data() .. data() + (length() - 1) ] is
        // undefined.

    virtual int length() const;
        // Return the number of bytes in this stream.
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

// CREATORS

template <class STREAM>
inline
bdex_OutStreamAdapter<STREAM>::bdex_OutStreamAdapter(STREAM *stream)
: d_stream_p(stream)
{
    BSLS_ASSERT_SAFE(stream);
}

template <class STREAM>
inline
bdex_OutStreamAdapter<STREAM>::~bdex_OutStreamAdapter()
{
}

// MANIPULATORS

template <class STREAM>
inline
void bdex_OutStreamAdapter<STREAM>::invalidate()
{
    d_stream_p->invalidate();
}

template <class STREAM>
inline
bdex_OutStream& bdex_OutStreamAdapter<STREAM>::putLength(int value)
{
    BSLS_ASSERT_SAFE(0 <= value);

    d_stream_p->putLength(value);
    return *this;
}

template <class STREAM>
inline
bdex_OutStream& bdex_OutStreamAdapter<STREAM>::putVersion(int value)
{
    d_stream_p->putVersion(value);
    return *this;
}

template <class STREAM>
inline
bdex_OutStream&
bdex_OutStreamAdapter<STREAM>::putInt64(bsls_PlatformUtil::Int64 value)
{
    d_stream_p->putInt64(value);
    return *this;
}

template <class STREAM>
inline
bdex_OutStream&
bdex_OutStreamAdapter<STREAM>::putUint64(bsls_PlatformUtil::Int64 value)
{
    d_stream_p->putUint64(value);
    return *this;
}

template <class STREAM>
inline
bdex_OutStream&
bdex_OutStreamAdapter<STREAM>::putInt56(bsls_PlatformUtil::Int64 value)
{
    d_stream_p->putInt56(value);
    return *this;
}

template <class STREAM>
inline
bdex_OutStream&
bdex_OutStreamAdapter<STREAM>::putUint56(bsls_PlatformUtil::Int64 value)
{
    d_stream_p->putUint56(value);
    return *this;
}

template <class STREAM>
inline
bdex_OutStream&
bdex_OutStreamAdapter<STREAM>::putInt48(bsls_PlatformUtil::Int64 value)
{
    d_stream_p->putInt48(value);
    return *this;
}

template <class STREAM>
inline
bdex_OutStream&
bdex_OutStreamAdapter<STREAM>::putUint48(bsls_PlatformUtil::Int64 value)
{
    d_stream_p->putUint48(value);
    return *this;
}

template <class STREAM>
inline
bdex_OutStream&
bdex_OutStreamAdapter<STREAM>::putInt40(bsls_PlatformUtil::Int64 value)
{
    d_stream_p->putInt40(value);
    return *this;
}

template <class STREAM>
inline
bdex_OutStream&
bdex_OutStreamAdapter<STREAM>::putUint40(bsls_PlatformUtil::Int64 value)
{
    d_stream_p->putUint40(value);
    return *this;
}

template <class STREAM>
inline
bdex_OutStream&
bdex_OutStreamAdapter<STREAM>::putInt32(int value)
{
    d_stream_p->putInt32(value);
    return *this;
}

template <class STREAM>
inline
bdex_OutStream&
bdex_OutStreamAdapter<STREAM>::putUint32(unsigned int value)
{
    d_stream_p->putUint32(value);
    return *this;
}

template <class STREAM>
inline
bdex_OutStream&
bdex_OutStreamAdapter<STREAM>::putInt24(int value)
{
    d_stream_p->putInt24(value);
    return *this;
}

template <class STREAM>
inline
bdex_OutStream&
bdex_OutStreamAdapter<STREAM>::putUint24(int value)
{
    d_stream_p->putUint24(value);
    return *this;
}

template <class STREAM>
inline
bdex_OutStream&
bdex_OutStreamAdapter<STREAM>::putInt16(int value)
{
    d_stream_p->putInt16(value);
    return *this;
}

template <class STREAM>
inline
bdex_OutStream&
bdex_OutStreamAdapter<STREAM>::putUint16(int value)
{
    d_stream_p->putUint16(value);
    return *this;
}

template <class STREAM>
inline
bdex_OutStream&
bdex_OutStreamAdapter<STREAM>::putInt8(int value)
{
    d_stream_p->putInt8(value);
    return *this;
}

template <class STREAM>
inline
bdex_OutStream&
bdex_OutStreamAdapter<STREAM>::putUint8(int value)
{
    d_stream_p->putUint8(value);
    return *this;
}

template <class STREAM>
inline
bdex_OutStream&
bdex_OutStreamAdapter<STREAM>::putFloat64(double value)
{
    d_stream_p->putFloat64(value);
    return *this;
}

template <class STREAM>
inline
bdex_OutStream&
bdex_OutStreamAdapter<STREAM>::putFloat32(float value)
{
    d_stream_p->putFloat32(value);
    return *this;
}

template <class STREAM>
inline
bdex_OutStream& bdex_OutStreamAdapter<STREAM>::putString(
                                                        const bsl::string& str)
{
    d_stream_p->putString(str);
    return *this;
}

template <class STREAM>
inline
bdex_OutStream&
bdex_OutStreamAdapter<STREAM>::putArrayInt64(
                                        const bsls_PlatformUtil::Int64 *array,
                                        int                             length)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= length);

    d_stream_p->putArrayInt64(array, length);
    return *this;
}

template <class STREAM>
inline
bdex_OutStream&
bdex_OutStreamAdapter<STREAM>::putArrayUint64(
                                       const bsls_PlatformUtil::Uint64 *array,
                                       int                              length)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= length);

    d_stream_p->putArrayUint64(array, length);
    return *this;
}

template <class STREAM>
inline
bdex_OutStream&
bdex_OutStreamAdapter<STREAM>::putArrayInt56(
                                        const bsls_PlatformUtil::Int64 *array,
                                        int                             length)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= length);

    d_stream_p->putArrayInt56(array, length);
    return *this;
}

template <class STREAM>
inline
bdex_OutStream&
bdex_OutStreamAdapter<STREAM>::putArrayUint56(
                                       const bsls_PlatformUtil::Uint64 *array,
                                       int                              length)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= length);

    d_stream_p->putArrayUint56(array, length);
    return *this;
}

template <class STREAM>
inline
bdex_OutStream&
bdex_OutStreamAdapter<STREAM>::putArrayInt48(
                                        const bsls_PlatformUtil::Int64 *array,
                                        int                             length)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= length);

    d_stream_p->putArrayInt48(array, length);
    return *this;
}

template <class STREAM>
inline
bdex_OutStream&
bdex_OutStreamAdapter<STREAM>::putArrayUint48(
                                       const bsls_PlatformUtil::Uint64 *array,
                                       int                              length)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= length);

    d_stream_p->putArrayUint48(array, length);
    return *this;
}

template <class STREAM>
inline
bdex_OutStream&
bdex_OutStreamAdapter<STREAM>::putArrayInt40(
                                        const bsls_PlatformUtil::Int64 *array,
                                        int                             length)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= length);

    d_stream_p->putArrayInt40(array, length);
    return *this;
}

template <class STREAM>
inline
bdex_OutStream&
bdex_OutStreamAdapter<STREAM>::putArrayUint40(
                                       const bsls_PlatformUtil::Uint64 *array,
                                       int                              length)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= length);

    d_stream_p->putArrayUint40(array, length);
    return *this;
}

template <class STREAM>
inline
bdex_OutStream&
bdex_OutStreamAdapter<STREAM>::putArrayInt32(const int *array, int length)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= length);

    d_stream_p->putArrayInt32(array, length);
    return *this;
}

template <class STREAM>
inline
bdex_OutStream&
bdex_OutStreamAdapter<STREAM>::putArrayUint32(const unsigned int *array,
                                              int                 length)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= length);

    d_stream_p->putArrayUint32(array, length);
    return *this;
}

template <class STREAM>
inline
bdex_OutStream&
bdex_OutStreamAdapter<STREAM>::putArrayInt24(const int *array, int length)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= length);

    d_stream_p->putArrayInt24(array, length);
    return *this;
}

template <class STREAM>
inline
bdex_OutStream&
bdex_OutStreamAdapter<STREAM>::putArrayUint24(const unsigned int *array,
                                              int                 length)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= length);

    d_stream_p->putArrayUint24(array, length);
    return *this;
}

template <class STREAM>
inline
bdex_OutStream&
bdex_OutStreamAdapter<STREAM>::putArrayInt16(const short *array, int length)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= length);

    d_stream_p->putArrayInt16(array, length);
    return *this;
}

template <class STREAM>
inline
bdex_OutStream&
bdex_OutStreamAdapter<STREAM>::putArrayUint16(const unsigned short *array,
                                              int                   length)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= length);

    d_stream_p->putArrayUint16(array, length);
    return *this;
}

template <class STREAM>
inline
bdex_OutStream&
bdex_OutStreamAdapter<STREAM>::putArrayInt8(const char *array, int length)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= length);

    d_stream_p->putArrayInt8(array, length);
    return *this;
}

template <class STREAM>
inline
bdex_OutStream&
bdex_OutStreamAdapter<STREAM>::putArrayInt8(const signed char *array,
                                            int                length)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= length);

    d_stream_p->putArrayInt8(array, length);
    return *this;
}

template <class STREAM>
inline
bdex_OutStream&
bdex_OutStreamAdapter<STREAM>::putArrayUint8(const char *array, int length)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= length);

    d_stream_p->putArrayUint8(array, length);
    return *this;
}

template <class STREAM>
inline
bdex_OutStream&
bdex_OutStreamAdapter<STREAM>::putArrayUint8(const unsigned char *array,
                                             int                  length)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= length);

    d_stream_p->putArrayUint8(array, length);
    return *this;
}

template <class STREAM>
inline
bdex_OutStream&
bdex_OutStreamAdapter<STREAM>::putArrayFloat64(const double *array, int length)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= length);

    d_stream_p->putArrayFloat64(array, length);
    return *this;
}

template <class STREAM>
inline
bdex_OutStream&
bdex_OutStreamAdapter<STREAM>::putArrayFloat32(const float *array, int length)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= length);

    d_stream_p->putArrayFloat32(array, length);
    return *this;
}

template <class STREAM>
inline
void bdex_OutStreamAdapter<STREAM>::removeAll()
{
    d_stream_p->removeAll();
}

template <class STREAM>
inline
void bdex_OutStreamAdapter<STREAM>::reserveCapacity(int newCapacity)
{
    BSLS_ASSERT_SAFE(0 <= newCapacity);

    d_stream_p->reserveCapacity(newCapacity);
}

// ACCESSORS

template <class STREAM>
inline
bdex_OutStreamAdapter<STREAM>::operator const void *() const
{
    return (const void *) *d_stream_p;
}

template <class STREAM>
inline
const char *bdex_OutStreamAdapter<STREAM>::data() const
{
    return d_stream_p->data();
}

template <class STREAM>
inline
int bdex_OutStreamAdapter<STREAM>::length() const
{
    return d_stream_p->length();
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
