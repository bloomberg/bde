// bdex_instreamadapter.h          -*-C++-*-
#ifndef INCLUDED_BDEX_INSTREAMADAPTER
#define INCLUDED_BDEX_INSTREAMADAPTER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide parameterized adapter for streaming in polymorphic types.
//
//@CLASSES:
// bdex_instreamadapter: adapter facilitating streaming in of polymorphic types
//
//@AUTHOR: Rohan Bhindwale (rbhindwa)
//
//@SEE_ALSO: bdex_instream, bdex_outstreamadapter
//
//@DESCRIPTION: This component contains a parameterized adapter class,
// 'bdex_InStreamAdapter', that provides a mechanism for unexternalizing
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
// streamable must use the 'bdex_InStreamAdapter' template class defined in
// this component (and the 'bdex_OutStreamAdapter' template class defined in
// 'bdex_outstreamadapter'), and in particular must define the 'virtual'
// 'streamIn' and 'streamOut' methods as shown in the Usage Example below.
// Clients of the polymorphic types can then stream values of these types
// exactly as if they were simple concrete types supporting 'bdex'.
//
// Refer to the protocol diagram below for an introduction to this class.
//..
//                                           ,-----------------------.
//      (STREAM template     .............. / template <class STREAM> `
//       argument)           :   :   :      \  bdex_InStreamAdapter   /
//                           :   :   :       `-----------------------'
//          ,-----------------.  :   :                    |
//         ( bdex_ByteInStream ) :   :                    |
//          `-----------------'  :   :                    |
//                               :   :                    V
//           ,--------------------.  :             ,-------------.
//          ( bdex_ByteInStreamRaw ) :            ( bdex_InStream )
//           `--------------------'  :             `-------------'
//                                   :                       Protocol class
//                  ,-----------------.                      providing
//                 ( bdex_ByteInStream )                     externalization
//                  `-----------------'                      interface.
//                           .
//                           .
//                           .
//                      Input streams
//                      implementing the
//                      'bdex_InStream'
//                      (documentation)
//                      protocol
//..
// The 'bdex_InStreamAdapter' class is a concrete implementation of the
// 'bdex_InStream' protocol parameterized on a 'STREAM' type.  The
// adapter class can be instantiated with any stream class implementing the
// 'bdex_InStream' protocol.  An input adapter is created by supplying an
// instance of the parameterized 'STREAM' type at construction.  The adapter
// then acts as a wrapper around the held stream object and forwards all 'get'
// data requests to it.  Additionally, because 'bdex_InStreamAdapter' is a
// concrete implementation of the 'bdex_InStream' protocol, it allows run
// time polymorphism based on the  'bdex_InStream' protocol class.
//
// Every protocol class that supports 'bdex' streaming should implement the
// parameterized 'bdexStreamIn', and 'bdexStreamOut' functions *and* also
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
///Usage
///-----
// The 'bdex_InStreamAdapter' input stream interface provided in this component
// is most commonly used to implement the stream-in functionality for a
// polymorphic type.  For example, consider the following 'myShape' protocol
// class:
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
//      int d_radius;  // radius of this circle
//      int d_x;       // x-coordinate of center
//      int d_y;       // y-coordinate of center
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
//              bdex_InStreamFunctions::streamIn(stream, d_radius, 1);
//              bdex_InStreamFunctions::streamIn(stream, d_x, 1);
//              bdex_InStreamFunctions::streamIn(stream, d_y, 1);
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
//          bdex_OutStreamFunctions::streamOut(stream, d_radius, 1);
//          bdex_OutStreamFunctions::streamOut(stream, d_x, 1);
//          bdex_OutStreamFunctions::streamOut(stream, d_y, 1);
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

#ifndef INCLUDED_BDEX_INSTREAM
#include <bdex_instream.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif


namespace BloombergLP {

                        // ==========================
                        // class bdex_InStreamAdapter
                        // ==========================

template <class STREAM>
class bdex_InStreamAdapter : public bdex_InStream {
    // This 'class' provides a mechanism to unexternalize polymorphic types
    // using the 'bdex' protocol.  It efficiently forwards all the
    // unexternalization requests to the stream it was constructed with.
    //
    // Note that this class is decidedly *not* value-semantic, and instead has
    // *reference* *semantics*.  There is no conventional notion of assignment,
    // copy construction or equality.

    // DATA
    STREAM *d_stream_p;  // stream to read from (held, not owned)

  public:
    // CREATORS
    bdex_InStreamAdapter(STREAM *stream);
        // Create an input stream adapter.

    virtual ~bdex_InStreamAdapter();
        // Destroy this object.

    // MANIPULATORS
    virtual bdex_InStream& getLength(int& length);
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

    virtual bdex_InStream& getVersion(int& version);
        // Consume a version value from this input stream, place that value in
        // the specified 'version', and return a reference to this modifiable
        // stream.  If this stream is initially invalid, this operation has no
        // effect.  If this function otherwise fails to extract a valid value,
        // this stream is marked invalid and the value of 'version' is
        // undefined.
        //
        // Consume one byte interpreted as an unsigned integer to provide the
        // value for 'version'.

                        // *** scalar integer values ***

    virtual bdex_InStream& getInt64(bsls_PlatformUtil::Int64& variable);
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

    virtual bdex_InStream& getUint64(bsls_PlatformUtil::Uint64& variable);
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

    virtual bdex_InStream& getInt56(bsls_PlatformUtil::Int64& variable);
        // Consume a 56-bit signed integer value from this input stream, place
        // that value in the specified 'variable', and return a reference to
        // this modifiable stream.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.
        //
        // Consume seven bytes interpreted as a sign extended 64-bit signed
        // integer in network byte order to provide the value for 'variable'.

    virtual bdex_InStream& getUint56(bsls_PlatformUtil::Uint64& variable);
        // Consume a 56-bit unsigned integer value from this input stream,
        // place that value in the specified 'variable', and return a reference
        // to this modifiable stream.  If this stream is initially invalid,
        // this operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.
        //
        // Consume seven bytes interpreted as a zero extended 64-bit unsigned
        // integer in network byte order to provide the value for 'variable'.

    virtual bdex_InStream& getInt48(bsls_PlatformUtil::Int64& variable);
        // Consume a 48-bit signed integer value from this input stream, place
        // that value in the specified 'variable', and return a reference to
        // this modifiable stream.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.
        //
        // Consume six bytes interpreted as a sign extended 64-bit signed
        // integer in network byte order to provide the value for 'variable'.

    virtual bdex_InStream& getUint48(bsls_PlatformUtil::Uint64& variable);
        // Consume a 48-bit unsigned integer value from this input stream,
        // place that value in the specified 'variable', and return a reference
        // to this modifiable stream.  If this stream is initially invalid,
        // this operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.
        //
        // Consume six bytes interpreted as a zero extended 64-bit unsigned
        // integer in network byte order to provide the value for 'variable'.

    virtual bdex_InStream& getInt40(bsls_PlatformUtil::Int64& variable);
        // Consume a 40-bit signed integer value from this input stream, place
        // that value in the specified 'variable', and return a reference to
        // this modifiable stream.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.
        //
        // Consume five bytes interpreted as a sign extended 64-bit signed
        // integer in network byte order to provide the value for 'variable'.

    virtual bdex_InStream& getUint40(bsls_PlatformUtil::Uint64& variable);
        // Consume a 40-bit unsigned integer value from this input stream,
        // place that value in the specified 'variable', and return a reference
        // to this modifiable stream.  If this stream is initially invalid,
        // this operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.
        //
        // Consume five bytes interpreted as a zero extended 64-bit unsigned
        // integer in network byte order to provide the value for 'variable'.

    virtual bdex_InStream& getInt32(int& variable);
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

    virtual bdex_InStream& getUint32(unsigned int& variable);
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

    virtual bdex_InStream& getInt24(int& variable);
        // Consume a 24-bit signed integer value from this input stream, place
        // that value in the specified 'variable', and return a reference to
        // this modifiable stream.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.
        //
        // Consume three bytes interpreted as a sign extended 32-bit signed
        // integer in network byte order to provide the value for 'variable'.

    virtual bdex_InStream& getUint24(unsigned int& variable);
        // Consume a 24-bit unsigned integer value from this input stream,
        // place that value in the specified 'variable', and return a reference
        // to this modifiable stream.  If this stream is initially invalid,
        // this operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'variable' is undefined.
        //
        // Consume three bytes interpreted as a zero extended 32-bit unsigned
        // integer in network byte order to provide the value for 'variable'.

    virtual bdex_InStream& getInt16(short& variable);
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

    virtual bdex_InStream& getUint16(unsigned short& variable);
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

    virtual bdex_InStream& getInt8(char& variable);
    virtual bdex_InStream& getInt8(signed char& variable);
        // Consume an 8-bit signed integer value from this input stream, place
        // that value in the specified 'variable', and return a reference to
        // this modifiable stream.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        //
        // Consume one byte interpreted as an 8-bit signed integer to provide
        // the value for 'variable'.

    virtual bdex_InStream& getUint8(char& variable);
    virtual bdex_InStream& getUint8(unsigned char& variable);
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

    virtual bdex_InStream& getFloat64(double& variable);
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

    virtual bdex_InStream& getFloat32(float& variable);
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

    virtual bdex_InStream& getString(bsl::string& variable);
        // Consume a string from this input stream, place that value in the
        // specified 'variable', and return a reference to this modifiable
        // stream.  If this stream is initially invalid, this operation has no
        // effect.  If this function otherwise fails to extract a valid value,
        // this stream is marked invalid and the value of 'variable' is
        // undefined.  The string must be prefaced by a non-negative integer
        // indicating the number of characters composing the string.  The
        // behavior is undefined if the length indicator is not greater than 0.
        //
        // First extract a length value.  Consume one byte and, if the most
        // significant bit (MSB) is set to 0, then that one-byte integer value
        // is the length value; otherwise, set that MSB to zero, consume three
        // more bytes, and interpret the four bytes in network byte order to
        // provide the length value.  Then, for each character of 'variable'
        // (of the extracted length) consume one byte providing the value for
        // that character.

                        // *** arrays of integer values ***

    virtual bdex_InStream& getArrayInt64(bsls_PlatformUtil::Int64 *array,
                                         int                       length);
        // Consume the 64-bit signed integer array of the specified 'length'
        // from this input stream, place that value in the specified 'array',
        // and return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless '0 <= length'.
        //
        // For each of the 'length' elements of 'array', consume eight bytes
        // interpreted as a 64-bit signed integer in network byte order to
        // provide the value for that element.  Note that the bit pattern is
        // sign extended on platforms where 'bsls_PlatformUtil::Int64' is more
        // than 64 bits.

    virtual bdex_InStream& getArrayUint64(bsls_PlatformUtil::Uint64 *array,
                                          int                        length);
        // Consume the 64-bit unsigned integer array of the specified 'length'
        // from this input stream, place that value in the specified 'array',
        // and return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless '0 <= length'.
        //
        // For each of the 'length' elements of 'array', consume eight bytes
        // interpreted as a 64-bit unsigned integer in network byte order to
        // provide the value for that element.  Note that the bit pattern is
        // zero extended on platforms where 'bsls_PlatformUtil::Uint64' is more
        // than 64 bits.

    virtual bdex_InStream& getArrayInt56(bsls_PlatformUtil::Int64 *array,
                                         int                       length);
        // Consume the 56-bit signed integer array of the specified 'length'
        // from this input stream, place that value in the specified 'array',
        // and return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless '0 <= length'.
        //
        // For each of the 'length' elements of 'array', consume seven bytes
        // interpreted as a sign extended 64-bit signed integer in network byte
        // order to provide the value for that element.

    virtual bdex_InStream& getArrayUint56(bsls_PlatformUtil::Uint64 *array,
                                          int                        length);
        // Consume the 56-bit unsigned integer array of the specified 'length'
        // from this input stream, place that value in the specified 'array',
        // and return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless '0 <= length'.
        //
        // For each of the 'length' elements of 'array', consume seven bytes
        // interpreted as a zero extended 64-bit unsigned integer in network
        // byte order to provide the value for that element.

    virtual bdex_InStream& getArrayInt48(bsls_PlatformUtil::Int64 *array,
                                         int                       length);
        // Consume the 48-bit signed integer array of the specified 'length'
        // from this input stream, place that value in the specified 'array',
        // and return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless '0 <= length'.
        //
        // For each of the 'length' elements of 'array', consume six bytes
        // interpreted as a sign extended 64-bit signed integer in network byte
        // order to provide the value for that element.

    virtual bdex_InStream& getArrayUint48(bsls_PlatformUtil::Uint64 *array,
                                          int                        length);
        // Consume the 48-bit unsigned integer array of the specified 'length'
        // from this input stream, place that value in the specified 'array',
        // and return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless '0 <= length'.
        //
        // For each of the 'length' elements of 'array', consume six bytes
        // interpreted as a zero extended 64-bit unsigned integer in network
        // byte order to provide the value for that element.

    virtual bdex_InStream& getArrayInt40(bsls_PlatformUtil::Int64 *array,
                                         int                       length);
        // Consume the 40-bit signed integer array of the specified 'length'
        // from this input stream, place that value in the specified 'array',
        // and return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless '0 <= length'.
        //
        // For each of the 'length' elements of 'array', consume five bytes
        // interpreted as a sign extended 64-bit signed integer in network byte
        // order to provide the value for that element.

    virtual bdex_InStream& getArrayUint40(bsls_PlatformUtil::Uint64 *array,
                                          int                        length);
        // Consume the 40-bit unsigned integer array of the specified 'length'
        // from this input stream, place that value in the specified 'array',
        // and return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless '0 <= length'.
        //
        // For each of the 'length' elements of 'array', consume five bytes
        // interpreted as a zero extended 64-bit unsigned integer in network
        // byte order to provide the value for that element.

    virtual bdex_InStream& getArrayInt32(int *array, int  length);
        // Consume the 32-bit signed integer array of the specified 'length'
        // from this input stream, place that value in the specified 'array',
        // and return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless '0 <= length'.
        //
        // For each of the 'length' elements of 'array', consume four bytes
        // interpreted as a 32-bit signed integer in network byte order to
        // provide the value for that element.  Note that the bit pattern is
        // sign extended on platforms where 'int' is more than 32 bits.

    virtual bdex_InStream& getArrayUint32(unsigned int *array, int length);
        // Consume the 32-bit unsigned integer array of the specified 'length'
        // from this input stream, place that value in the specified 'array',
        // and return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless '0 <= length'.
        //
        // For each of the 'length' elements of 'array', consume four bytes
        // interpreted as a 32-bit unsigned integer in network byte order to
        // provide the value for that element.  Note that the bit pattern is
        // zero extended on platforms where 'unsigned' 'int' is more than 32
        // bits.

    virtual bdex_InStream& getArrayInt24(int *array, int length);
        // Consume the 24-bit signed integer array of the specified 'length'
        // from this input stream, place that value in the specified 'array',
        // and return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless '0 <= length'.
        //
        // For each of the 'length' elements of 'array', consume three bytes
        // interpreted as a sign extended 32-bit signed integer in network byte
        // order to provide the value for that element.

    virtual bdex_InStream& getArrayUint24(unsigned int *array, int length);
        // Consume the 24-bit unsigned integer array of the specified 'length'
        // from this input stream, place that value in the specified 'array',
        // and return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless '0 <= length'.
        //
        // For each of the 'length' elements of 'array', consume three bytes
        // interpreted as a zero extended 32-bit unsigned integer in network
        // byte order to provide the value for that element.

    virtual bdex_InStream& getArrayInt16(short *array, int length);
        // Consume the 16-bit signed integer array of the specified 'length'
        // from this input stream, place that value in the specified 'array',
        // and return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless '0 <= length'.
        //
        // For each of the 'length' elements of 'array', consume two bytes
        // interpreted as a 16-bit signed integer in network byte order to
        // provide the value for that element.  Note that the bit pattern is
        // sign extended on platforms where 'short' is more than 16 bits.

    virtual bdex_InStream& getArrayUint16(unsigned short *array, int length);
        // Consume the 16-bit unsigned integer array of the specified 'length'
        // from this input stream, place that value in the specified 'array',
        // and return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless '0 <= length'.
        //
        // For each of the 'length' elements of 'array', consume two bytes
        // interpreted as a 16-bit unsigned integer in network byte order to
        // provide the value for that element.  Note that the bit pattern is
        // zero extended on platforms where 'unsigned' 'short' is more than 16
        // bits.

    virtual bdex_InStream& getArrayInt8(char *array, int length);
    virtual bdex_InStream& getArrayInt8(signed char *array, int length);
        // Consume the 8-bit signed integer array of the specified 'length'
        // from this input stream, place that value in the specified 'array',
        // and return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless '0 <= length'.
        //
        // For each of the 'length' elements of 'array', consume one byte
        // interpreted as an 8-bit signed integer to provide the value for that
        // element.

    virtual bdex_InStream& getArrayUint8(char *array, int length);
    virtual bdex_InStream& getArrayUint8(unsigned char *array, int length);
        // Consume the 8-bit unsigned integer array of the specified 'length'
        // from this input stream, place that value in the specified 'array',
        // and return a reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid array of the required 'length',
        // this stream is marked invalid and the value of 'array' is undefined.
        // The behavior is undefined unless '0 <= length'.
        //
        // For each of the 'length' elements of 'array', consume one byte
        // interpreted as an 8-bit unsigned integer to provide the value for
        // that element.

                        // *** arrays of floating-point values ***

    virtual bdex_InStream& getArrayFloat64(double *array, int length);
        // Consume the IEEE double-precision (8-byte) floating-point array of
        // the specified 'length' from this input stream, place that value in
        // the specified 'array', and return a reference to this modifiable
        // stream.  If this stream is initially invalid, this operation has no
        // effect.  If this function otherwise fails to extract a valid array
        // of the required 'length', this stream is marked invalid and the
        // value of 'array' is undefined.  The behavior is undefined unless
        // '0 <= length'.  Note that for non-conforming platforms, this
        // operation may be lossy.
        //
        // For each of the 'length' elements of 'array', consume eight bytes
        // interpreted as an IEEE double-precision value in network byte order
        // to provide the value for that element.

    virtual bdex_InStream& getArrayFloat32(float *array, int length);
        // Consume the IEEE single-precision (4-byte) floating-point array of
        // the specified 'length' from this input stream, place that value in
        // the specified 'array', and return a reference to this modifiable
        // stream.  If this stream is initially invalid, this operation has no
        // effect.  If this function otherwise fails to extract a valid array
        // of the required 'length', this stream is marked invalid and the
        // value of 'array' is undefined.  The behavior is undefined unless
        // '0 <= length'.  Note that for non-conforming platforms, this
        // operation may be lossy.
        //
        // For each of the 'length' elements of 'array', consume four bytes
        // interpreted as an IEEE single-precision value in network byte order
        // to provide the value for that element.

    virtual void invalidate();
        // Put this input stream in an invalid state.  This function has no
        // effect if this stream is already invalid.  Note that this function
        // should be called whenever a value extracted from this stream is
        // determined to be invalid, inconsistent, or otherwise incorrect.

    // ACCESSORS
    virtual operator const void *() const;
        // Return a non-zero value if this stream is valid, and 0 otherwise.
        // An invalid stream is a stream in which insufficient or invalid data
        // was detected during an extraction operation.  Note that an empty
        // stream will be valid unless an extraction attempt or explicit
        // invalidation causes it to be otherwise.

    virtual int isEmpty() const;
        // Return 1 if this stream is empty, and 0 otherwise.  The behavior is
        // undefined unless this stream is valid (i.e., would convert to a
        // non-zero value via the 'operator const void *() const' accessor).
        // Note that this function enables higher-level components to verify
        // that, after successfully reading all expected data, no data remains.

    virtual int length() const;
        // Return the total number of bytes stored in this stream.

    virtual int cursor() const;
        // Return the index of the next byte to be extracted from this stream.
        // The behavior is undefined unless this stream is valid.
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        // --------------------------
                        // class bdex_InStreamAdapter
                        // --------------------------

// CREATORS
template <class STREAM>
inline
bdex_InStreamAdapter<STREAM>::bdex_InStreamAdapter(STREAM *stream)
{
    d_stream_p = stream;
}

template <class STREAM>
inline
bdex_InStreamAdapter<STREAM>::~bdex_InStreamAdapter()
{
}

// MANIPULATORS
template <class STREAM>
bdex_InStream& bdex_InStreamAdapter<STREAM>::getLength(int& variable)
{
    d_stream_p->getLength(variable);
    return *this;
}

template <class STREAM>
bdex_InStream& bdex_InStreamAdapter<STREAM>::getVersion(int& variable)
{
    d_stream_p->getVersion(variable);
    return *this;
}

template <class STREAM>
bdex_InStream&
bdex_InStreamAdapter<STREAM>::getInt64(bsls_PlatformUtil::Int64& variable)
{
    d_stream_p->getInt64(variable);
    return *this;
}

template <class STREAM>
bdex_InStream&
bdex_InStreamAdapter<STREAM>::getUint64(bsls_PlatformUtil::Uint64& variable)
{
    d_stream_p->getUint64(variable);
    return *this;
}

template <class STREAM>
bdex_InStream&
bdex_InStreamAdapter<STREAM>::getInt56(bsls_PlatformUtil::Int64& variable)
{
    d_stream_p->getInt56(variable);
    return *this;
}

template <class STREAM>
bdex_InStream&
bdex_InStreamAdapter<STREAM>::getUint56(bsls_PlatformUtil::Uint64& variable)
{
    d_stream_p->getUint56(variable);
    return *this;
}

template <class STREAM>
bdex_InStream&
bdex_InStreamAdapter<STREAM>::getInt48(bsls_PlatformUtil::Int64& variable)
{
    d_stream_p->getInt48(variable);
    return *this;
}

template <class STREAM>
bdex_InStream&
bdex_InStreamAdapter<STREAM>::getUint48(bsls_PlatformUtil::Uint64& variable)
{
    d_stream_p->getUint48(variable);
    return *this;
}

template <class STREAM>
bdex_InStream&
bdex_InStreamAdapter<STREAM>::getInt40(bsls_PlatformUtil::Int64& variable)
{
    d_stream_p->getInt40(variable);
    return *this;
}

template <class STREAM>
bdex_InStream&
bdex_InStreamAdapter<STREAM>::getUint40(bsls_PlatformUtil::Uint64& variable)
{
    d_stream_p->getUint40(variable);
    return *this;
}

template <class STREAM>
bdex_InStream& bdex_InStreamAdapter<STREAM>::getInt32(int& variable)
{
    d_stream_p->getInt32(variable);
    return *this;
}

template <class STREAM>
bdex_InStream& bdex_InStreamAdapter<STREAM>::getUint32(unsigned int& variable)
{
    d_stream_p->getUint32(variable);
    return *this;
}

template <class STREAM>
bdex_InStream& bdex_InStreamAdapter<STREAM>::getInt24(int& variable)
{
    d_stream_p->getInt24(variable);
    return *this;
}

template <class STREAM>
bdex_InStream& bdex_InStreamAdapter<STREAM>::getUint24(unsigned int& variable)
{
    d_stream_p->getUint24(variable);
    return *this;
}

template <class STREAM>
bdex_InStream& bdex_InStreamAdapter<STREAM>::getInt16(short& variable)
{
    d_stream_p->getInt16(variable);
    return *this;
}

template <class STREAM>
bdex_InStream& bdex_InStreamAdapter<STREAM>::getUint16(
                                                      unsigned short& variable)
{
    d_stream_p->getUint16(variable);
    return *this;
}

template <class STREAM>
bdex_InStream& bdex_InStreamAdapter<STREAM>::getInt8(char& variable)
{
    d_stream_p->getInt8(variable);
    return *this;
}

template <class STREAM>
bdex_InStream& bdex_InStreamAdapter<STREAM>::getInt8(signed char& variable)
{
    d_stream_p->getInt8(variable);
    return *this;
}

template <class STREAM>
bdex_InStream& bdex_InStreamAdapter<STREAM>::getUint8(char& variable)
{
    d_stream_p->getUint8(variable);
    return *this;
}

template <class STREAM>
bdex_InStream& bdex_InStreamAdapter<STREAM>::getUint8(unsigned char& variable)
{
    d_stream_p->getUint8(variable);
    return *this;
}

template <class STREAM>
bdex_InStream& bdex_InStreamAdapter<STREAM>::getFloat64(double& variable)
{
    d_stream_p->getFloat64(variable);
    return *this;
}

template <class STREAM>
bdex_InStream& bdex_InStreamAdapter<STREAM>::getFloat32(float& variable)
{
    d_stream_p->getFloat32(variable);
    return *this;
}

template <class STREAM>
bdex_InStream& bdex_InStreamAdapter<STREAM>::getString(bsl::string& variable)
{
    d_stream_p->getString(variable);
    return *this;
}

template <class STREAM>
bdex_InStream&
bdex_InStreamAdapter<STREAM>::getArrayInt64(bsls_PlatformUtil::Int64 *array,
                                            int                       length)
{
    d_stream_p->getArrayInt64(array, length);
    return *this;
}

template <class STREAM>
bdex_InStream&
bdex_InStreamAdapter<STREAM>::getArrayUint64(bsls_PlatformUtil::Uint64 *array,
                                             int                        length)
{
    d_stream_p->getArrayUint64(array, length);
    return *this;
}

template <class STREAM>
bdex_InStream&
bdex_InStreamAdapter<STREAM>::getArrayInt56(bsls_PlatformUtil::Int64 *array,
                                            int                       length)
{
    d_stream_p->getArrayInt56(array, length);
    return *this;
}

template <class STREAM>
bdex_InStream&
bdex_InStreamAdapter<STREAM>::getArrayUint56(bsls_PlatformUtil::Uint64 *array,
                                             int                        length)
{
    d_stream_p->getArrayUint56(array, length);
    return *this;
}

template <class STREAM>
bdex_InStream&
bdex_InStreamAdapter<STREAM>::getArrayInt48(bsls_PlatformUtil::Int64 *array,
                                            int                       length)
{
    d_stream_p->getArrayInt48(array, length);
    return *this;
}

template <class STREAM>
bdex_InStream&
bdex_InStreamAdapter<STREAM>::getArrayUint48(bsls_PlatformUtil::Uint64 *array,
                                             int                        length)
{
    d_stream_p->getArrayUint48(array, length);
    return *this;
}

template <class STREAM>
bdex_InStream&
bdex_InStreamAdapter<STREAM>::getArrayInt40(bsls_PlatformUtil::Int64 *array,
                                            int                       length)
{
    d_stream_p->getArrayInt40(array, length);
    return *this;
}

template <class STREAM>
bdex_InStream&
bdex_InStreamAdapter<STREAM>::getArrayUint40(bsls_PlatformUtil::Uint64 *array,
                                             int                        length)
{
    d_stream_p->getArrayUint40(array, length);
    return *this;
}

template <class STREAM>
bdex_InStream&
bdex_InStreamAdapter<STREAM>::getArrayInt32(int *array, int length)
{
    d_stream_p->getArrayInt32(array, length);
    return *this;
}

template <class STREAM>
bdex_InStream&
bdex_InStreamAdapter<STREAM>::getArrayUint32(unsigned int *array, int length)
{
    d_stream_p->getArrayUint32(array, length);
    return *this;
}

template <class STREAM>
bdex_InStream&
bdex_InStreamAdapter<STREAM>::getArrayInt24(int *array, int length)
{
    d_stream_p->getArrayInt24(array, length);
    return *this;
}

template <class STREAM>
bdex_InStream&
bdex_InStreamAdapter<STREAM>::getArrayUint24(unsigned int *array, int length)
{
    d_stream_p->getArrayUint24(array, length);
    return *this;
}

template <class STREAM>
bdex_InStream&
bdex_InStreamAdapter<STREAM>::getArrayInt16(short *array, int length)
{
    d_stream_p->getArrayInt16(array, length);
    return *this;
}

template <class STREAM>
bdex_InStream&
bdex_InStreamAdapter<STREAM>::getArrayUint16(unsigned short *array, int length)
{
    d_stream_p->getArrayUint16(array, length);
    return *this;
}

template <class STREAM>
bdex_InStream&
bdex_InStreamAdapter<STREAM>::getArrayInt8(char *array, int length)
{
    d_stream_p->getArrayInt8(array, length);
    return *this;
}

template <class STREAM>
bdex_InStream&
bdex_InStreamAdapter<STREAM>::getArrayInt8(signed char *array, int length)
{
    d_stream_p->getArrayInt8(array, length);
    return *this;
}

template <class STREAM>
bdex_InStream&
bdex_InStreamAdapter<STREAM>::getArrayUint8(char *array, int length)
{
    d_stream_p->getArrayUint8(array, length);
    return *this;
}

template <class STREAM>
bdex_InStream&
bdex_InStreamAdapter<STREAM>::getArrayUint8(unsigned char *array, int length)
{
    d_stream_p->getArrayUint8(array, length);
    return *this;
}

template <class STREAM>
bdex_InStream&
bdex_InStreamAdapter<STREAM>::getArrayFloat64(double *array, int length)
{
    d_stream_p->getArrayFloat64(array, length);
    return *this;
}

template <class STREAM>
bdex_InStream& bdex_InStreamAdapter<STREAM>::getArrayFloat32(float *array,
                                                             int    length)
{
    d_stream_p->getArrayFloat32(array, length);
    return *this;
}

template <class STREAM>
void bdex_InStreamAdapter<STREAM>::invalidate()
{
    d_stream_p->invalidate();
}

// ACCESSORS
template <class STREAM>
bdex_InStreamAdapter<STREAM>::operator const void *() const
{
    return (const void *) (*d_stream_p);
}

template <class STREAM>
int bdex_InStreamAdapter<STREAM>::isEmpty() const
{
    return d_stream_p->isEmpty();
}

template <class STREAM>
int bdex_InStreamAdapter<STREAM>::length() const
{
    return d_stream_p->length();
}

template <class STREAM>
int bdex_InStreamAdapter<STREAM>::cursor() const
{
    return d_stream_p->cursor();
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
