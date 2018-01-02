// bslx_instreamfunctions.h                                           -*-C++-*-
#ifndef INCLUDED_BSLX_INSTREAMFUNCTIONS
#define INCLUDED_BSLX_INSTREAMFUNCTIONS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Facilitate uniform unexternalization of user and fundamental types.
//
//@CLASSES:
//  bslx::InStreamFunctions: namespace for BDEX unexternalization functions
//
//@SEE_ALSO: bslx_outstreamfunctions, bslx_versionfunctions
//
//@DESCRIPTION: This component provides a namespace, 'bslx::InStreamFunctions',
// that facilitates uniform support for BDEX unexternalization across all
// BDEX-compliant user-defined types, including template types and containers,
// as well as those fundamental types (and 'bsl::string' and 'bsl::vector') for
// which the BDEX protocol provides direct support.
//
// The namespace 'bslx::InStreamFunctions' facilitates client unexternalization
// of objects in a uniform, type-independent manner.  It contains the
// 'bdexStreamIn' function that unexternalizes objects of all BDEX-compliant
// types.  This function unexternalizes the specified 'object' in the specified
// 'version' or the 'version' read from the input stream as required by the
// BDEX protocol.  The 'bdexStreamIn' function is overloaded for fundamental
// types, enumeration types, 'bsl::string', and 'bsl::vector'.  Note that,
// excluding 'bsl::vector', version information is never read from the stream
// while unexternalizing these types.
//
// By default, objects of enumeration type are streamed in as 32-bit 'int'
// values.  Users can override this behavior by providing overloads of the
// 'InStreamFunctions::bdexStreamIn' function in the enumeration's namespace
// for their enumeration types.  The general form of this overload is:
//..
//  template <class STREAM>
//  STREAM& bdexStreamIn(STREAM& stream, MyEnum& variable, int version)
//  {
//      using bslx::InStreamFunctions::bdexStreamIn;
//
//      // Code to stream in objects of 'MyEnum' type.
//
//      return stream;
//  }
//..
// For value-semantic types that support the BDEX protocol, the free function
// 'bdexStreamIn' calls the 'bdexStreamIn' member function for that type.
//
///Component Design, Anticipated Usage, and the BDEX Contract
///----------------------------------------------------------
// 'bslx_instreamfunctions' is an integral part of the BDEX unexternalization
// contract.  The BDEX contract is at least in part "collaborative", which is
// to say that each developer of a given *kind* of component (e.g., a stream or
// a value-semantic container) must comply with the relevant portions of the
// contract to ensure that the "system as a whole" works for everybody.
// 'bslx_instreamfunctions' plays several related but different roles in
// helping various developers to produce BDEX-compliant components.  In this
// section we briefly highlight how and why 'bslx_instreamfunctions' is helpful
// (or required) for these different developers.  By discussing different
// aspects of usage, we convey the general design goals of this component, and,
// to a certain extent, the overall BDEX contract.  See the 'bslx'
// package-level documentation for a full specification of the BDEX contract.
//
///Implementing BDEX Streaming in Value-Semantic Template Classes
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The author of a non-template value-semantic type has full knowledge of the
// details of the "value" of that type, and may choose to use the appropriate
// input stream 'get' methods directly when implementing the required
// 'bdexStreamIn' method for that type.  However, if one or more aspects of the
// value are of template parameter type, then the author cannot in general know
// how to stream the value using the 'get' methods.  For example, if a type has
// as its value one 'int' data member:
//..
//  int d_value;
//..
// then the implementation of the 'bdexStreamIn' method can contain:
//..
//  stream.getInt32(d_value);
//..
// However, if the data member is of (template parameter) 'TYPE':
//..
//  TYPE d_value;
//..
// then the implementation of the 'bdexStreamIn' method must rely on the
// 'bslx::InStreamFunctions' implementation to input the value:
//..
//  using bslx::InStreamFunctions::bdexStreamIn;
//  bdexStreamIn(stream, d_value, 1);
//..
// This call will resolve to the correct sequence of 'get' calls no matter
// whether 'TYPE' is a fundamental type, a BDEX-compliant 'enum', or a proper
// BDEX-compliant class.  In the latter two cases, the explicit specification
// of the version format (in this case, 1) guarantees the stable operation of
// this method whether or not 'TYPE' is provided additional version formats.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using 'bslx::InStreamFunctions' to Unexternalize Data
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example we illustrate the primary intended use of the parameterized
// methods of this component, as well as a few trivial invocations just to show
// the syntax clearly.  To accomplish this, we exhibit three separate example
// "components": an 'enum', a value-semantic point object, and an input stream.
// In all cases, the component designs are very simple, with much of the
// implied functionality omitted, in order to focus attention on the key
// aspects of the functionality of *this* component.
//
// First, consider an 'enum' 'Color' that enumerates a set of colors:
//..
//  enum Color {
//      RED   = 0,
//      GREEN = 1,
//      BLUE  = 2
//  };
//..
// Next, we consider a very special-purpose point that has as a data member its
// color.  Such a point provides an excellent opportunity for factoring, but
// since we are interested in highlighting BDEX streaming of various types, we
// will present a simple and unfactored design here.  In a real-world problem,
// the 'mypoint' component would be implemented differently.
//
// Note that the 'MyPoint' class in this example represents its coordinates as
// 'short' integer values; this is done to make the BDEX stream input byte
// pattern somewhat easier for the reader of this example to recognize when the
// input buffer is printed.
//..
//  // mypoint.h
//
//  class MyPoint {
//      // This class provides a geometric point having integer coordinates and
//      // an enumerated color property.
//
//      short d_x;      // x coordinate
//      short d_y;      // y coordinate
//      Color d_color;  // enumerated color property
//
//    public:
//      // CLASS METHODS
//      // ...
//
//      // CREATORS
//      MyPoint();
//          // Create a default point.
//
//      MyPoint(short x, short y, Color color);
//          // Create a point having the specified 'x' and 'y' coordinates
//          // and the specified 'color'.
//
//      ~MyPoint();
//          // Destroy this point.
//
//      // MANIPULATORS
//      // ...
//
//      // ACCESSORS
//      short x() const;
//          // Return the x coordinate of this point.
//
//      short y() const;
//          // Return the y coordinate of this point.
//
//      Color color() const;
//          // Return the enumerated color of this point.
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
//  };
//
//  // FREE OPERATORS
//  inline
//  bool operator==(const MyPoint& lhs, const MyPoint& rhs);
//      // Return 'true' if the specified 'lhs' and 'rhs' points have the same
//      // value, and 'false' otherwise.  Two points have the same value if
//      // they have the same x and y coordinates and the same color.
//..
// Representative (inline) implementations of these methods are shown below:
//..
//  // ========================================================================
//  //                      INLINE FUNCTION DEFINITIONS
//  // ========================================================================
//
//  // CREATORS
//  inline
//  MyPoint::MyPoint()
//  {
//  }
//
//  inline
//  MyPoint::MyPoint(short x, short y, Color color)
//  : d_x(x)
//  , d_y(y)
//  , d_color(color)
//  {
//  }
//
//  inline
//  MyPoint::~MyPoint()
//  {
//  }
//
//  // ...
//
//  // MANIPULATORS
//  // ...
//
//  // ACCESSORS
//  inline
//  Color MyPoint::color() const
//  {
//      return d_color;
//  }
//
//  inline
//  short MyPoint::x() const
//  {
//      return d_x;
//  }
//
//  inline
//  short MyPoint::y() const
//  {
//      return d_y;
//  }
//  // ...
//
//  template <class STREAM>
//  STREAM& MyPoint::bdexStreamIn(STREAM& stream, int version)
//  {
//      switch (version) {
//        case 1: {
//          stream.getInt16(d_x);           // input the x coordinate
//          stream.getInt16(d_y);           // input the y coordinate
//          char color;
//          stream.getInt8(color);          // input the color enum as one byte
//          d_color = static_cast<Color>(color);
//        } break;
//        default: {
//          stream.invalidate();
//        } break;
//      }
//      return stream;
//  }
//
//  // FREE OPERATORS
//  inline
//  bool operator==(const MyPoint& lhs, const MyPoint& rhs)
//  {
//      return lhs.x()     == rhs.x()
//          && lhs.y()     == rhs.y()
//          && lhs.color() == rhs.color();
//  }
//..
// Then, we will implement an extremely simple input stream that supports the
// BDEX documentation-only protocol.  For simplicity, we will use an externally
// managed buffer, and will only show a few methods needed for this example.
//..
//  // myinstream.h
//  // ...
//
// class MyInStream {
//     // This class implements a limited-size fixed-buffer input stream that
//     // partially conforms to the BDEX protocol for input streams.  This
//     // class is suitable for demonstration purposes only.
//
//     const char *d_buffer;  // input buffer, held but not owned
//     int         d_length;  // length of 'd_buffer' (bytes)
//     int         d_cursor;  // cursor (index into 'd_buffer')
//
//   public:
//     // CREATORS
//     MyInStream(const char *buffer, int length);
//         // Create an input stream using the specified 'buffer' having the
//         // specified 'length' (in bytes).
//
//     ~MyInStream();
//         // Destroy this input byte stream.
//
//     // MANIPULATORS
//     MyInStream& getVersion(int& version);
//         // Consume a version value from this input stream, store that value
//         // in the specified 'version', and return a reference to this
//         // stream.  ...
//
//     MyInStream& getInt32(int& value);
//         // Consume a 32-bit signed integer value from this input stream,
//         // store that value in the specified 'value', and return a reference
//         // to this stream.  ...
//
//     MyInStream& getInt16(short& value);
//         // Consume a 16-bit signed integer value from this input stream,
//         // store that value in the specified 'value', and return a reference
//         // to this stream.  ...
//
//     MyInStream& getInt8(char& value);
//         // Consume an 8-bit signed integer value from this input stream,
//         // store that value in the specified 'value', and return a reference
//         // to this stream.  ...
//
//     void invalidate();
//         // Put this input stream in an invalid state.  ...
//
//     // ACCESSORS
//     operator const void *() const;
//         // Return a non-zero value if this stream is valid, and 0
//         // otherwise.  An invalid stream is a stream in which insufficient
//         // or invalid data was detected during an extraction operation.
//         // Note that an empty stream will be valid unless an extraction
//         // attempt or explicit invalidation causes it to be otherwise.
//
//     int cursor() const;
//         // Return the index of the next byte to be extracted from this
//         // stream.
//
//     bool isEmpty() const;
//          // Return 'true' if this stream is empty, and 'false' otherwise.
//          // Note that this function enables higher-level types to verify
//          // that, after successfully reading all expected data, no data
//          // remains.
//
//     int length() const;
//         // Return the total number of bytes stored in this stream.
// };
//
//..
// The relevant (inline) implementations are as follows.
//..
//  // ========================================================================
//  //                      INLINE FUNCTION DEFINITIONS
//  // ========================================================================
//
//  // CREATORS
//  inline
//  MyInStream::MyInStream(const char *buffer, int length)
//  : d_buffer(buffer)
//  , d_length(length)
//  , d_cursor(0)
//  {
//  }
//
//  inline
//  MyInStream::~MyInStream()
//  {
//  }
//
//  // MANIPULATORS
//
//  inline
//  MyInStream& MyInStream::getVersion(int& value)
//  {
//      value = static_cast<unsigned char>(d_buffer[d_cursor++]);
//      return *this;
//  }
//
//  inline
//  MyInStream& MyInStream::getInt32(int& value)
//  {
//      const unsigned char *buffer =
//                           reinterpret_cast<const unsigned char *>(d_buffer);
//      value = static_cast<int>((buffer[d_cursor    ] << 24U) |
//                               (buffer[d_cursor + 1] << 16U) |
//                               (buffer[d_cursor + 2] <<  8U) |
//                               (buffer[d_cursor + 3]       ));
//      d_cursor += 4;
//      return *this;
//  }
//
//  inline
//  MyInStream& MyInStream::getInt16(short& value)
//  {
//      const unsigned char *buffer =
//                           reinterpret_cast<const unsigned char *>(d_buffer);
//      value = static_cast<short>((buffer[d_cursor    ] <<  8) |
//                                 (buffer[d_cursor + 1]      ));
//      d_cursor += 2;
//      return *this;
//   }
//
//  inline
//  MyInStream& MyInStream::getInt8(char& value)
//  {
//      value = d_buffer[d_cursor];
//      d_cursor += 1;
//      return *this;
//  }
//
//  inline
//  void MyInStream::invalidate()
//  {
//      d_buffer = 0;
//  }
//
//  // ACCESSORS
//  inline
//  MyInStream::operator const void *() const
//  {
//      return d_cursor <= d_length ? d_buffer : 0;
//  }
//
//  inline
//  int MyInStream::cursor() const
//  {
//      return d_cursor;
//  }
//
//  inline
//  bool MyInStream::isEmpty() const
//  {
//      return d_cursor >= d_length;
//  }
//
//  inline
//  int MyInStream::length() const
//  {
//      return d_length;
//  }
//..
// Finally, use the above 'enum', point class, and input stream to illustrate
// 'bslx::InStreamFunctions' functionality.  This test code does not attempt to
// do anything more useful than reading values from a stream whose buffer was
// written "by hand" and confirming that the expected values were read
// correctly from the known byte pattern in the buffer.
//..
//  using bslx::InStreamFunctions::bdexStreamIn;
//
//  {
//      const int  EXP       = 0x0A0B0C0D;
//      const char buffer[4] = { 0xA, 0xB, 0xC, 0xD };  // 'int' (no version)
//      int        i         = 0;
//
//      MyInStream in1(buffer, 4);  // use the one buffer
//      bdexStreamIn(in1, i, 1);
//      assert(in1);  assert(EXP == i);
//
//      i = 0;                      // reset 'i'
//      MyInStream in2(buffer, 4);  // re-use 'buffer (no version)
//      bdexStreamIn(in2, i, 0);
//      assert(in2);  assert(EXP == i);
//  }
//
//  {
//      const MyPoint EXP(0, -1, BLUE);
//      const char buffer1[5] = { 0, 0, -1, -1, 2 };     // 'MyPoint' (no ver)
//      const char buffer2[6] = { 1, 0, 0, -1, -1, 2 };  // version, 'MyPoint'
//      MyPoint p1, p2;  // two default points
//
//      MyInStream in1(buffer1, 5);  // 'buffer1' has no version byte
//      bdexStreamIn(in1, p1, 1);
//      assert(in1);  assert(EXP == p1);
//
//      MyInStream in2(buffer2, 6);  // 'buffer2' *has* a version
//      int version;
//      in2.getVersion(version);
//      assert(1 == version);
//      bdexStreamIn(in2, p2, version);
//      assert(in2);  assert(EXP == p2);
//  }
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLX_VERSIONFUNCTIONS
#include <bslx_versionfunctions.h>
#endif

#ifndef INCLUDED_BSLMF_IF
#include <bslmf_if.h>
#endif

#ifndef INCLUDED_BSLMF_ISENUM
#include <bslmf_isenum.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {
namespace bslx {

                         // ===========================
                         // namespace InStreamFunctions
                         // ===========================

namespace InStreamFunctions {
    // This namespace facilitates unexternalization of all BDEX-compliant types
    // in a type-independent manner.  The unexternalization functions are
    // overloaded for fundamental types, enumeration types, 'bsl::string', and
    // 'bsl::vector'.  A compilation error will occur if the (template
    // parameter) 'TYPE' of a non-overloaded method of
    // 'bslx::InStreamFunctions' does not support 'bdexStreamIn' (with the
    // appropriate signature).

                         // =================
                         // struct IsEnumType
                         // =================

    struct IsEnumType {
        // This 'struct', together with 'IsNotEnumType' (below), is used to
        // distinguish enumeration types from other types in function overload
        // resolution.  This 'struct' contains no interface or implementation
        // by design, and is meant for internal use only.
    };

                         // ====================
                         // struct IsNotEnumType
                         // ====================

    struct IsNotEnumType {
        // This 'struct', together with 'IsEnumType' (above), is used to
        // distinguish enumeration types from other types in function overload
        // resolution.  This 'struct' contains no interface or implementation
        // by design, and is meant for internal use only.
    };

    // PRIVATE CLASS METHODS
    template <class STREAM, class TYPE>
    STREAM& bdexStreamInImp(STREAM&           stream,
                            TYPE&             variable,
                            int               version,
                            const IsEnumType&);
        // Assign to the specified 'variable' the 'TYPE' value read from the
        // specified input 'stream', and return a reference to 'stream'.  The
        // specified 'version' is ignored.  If 'stream' is initially invalid,
        // this operation has no effect.  If 'version' is not supported by
        // 'TYPE', 'variable' is unaltered and 'stream' is invalidated, but
        // otherwise unmodified.  If 'version' is supported but 'stream'
        // becomes invalid during this operation, 'variable' has an undefined,
        // but valid, state.  Note that this function is called only for
        // enumeration types and that this function is for internal use only.
        // See the 'bslx' package-level documentation for more information on
        // BDEX streaming of value-semantic types and containers.

    template <class STREAM, class TYPE>
    STREAM& bdexStreamInImp(STREAM&              stream,
                            TYPE&                variable,
                            int                  version,
                            const IsNotEnumType&);
        // Assign to the specified 'variable' the 'TYPE' value read from the
        // specified input 'stream' using the specified 'version' format, and
        // return a reference to 'stream'.  If 'stream' is initially invalid,
        // this operation has no effect.  If 'version' is not supported by
        // 'TYPE', 'variable' is unaltered and 'stream' is invalidated, but
        // otherwise unmodified.  If 'version' is supported but 'stream'
        // becomes invalid during this operation, 'variable' has an undefined,
        // but valid, state.  Note that this function is for internal use only.
        // See the 'bslx' package-level documentation for more information on
        // BDEX streaming of value-semantic types and containers.

    // CLASS METHODS
    template <class STREAM, class TYPE>
    STREAM& bdexStreamIn(STREAM& stream, TYPE& variable);
        // Assign to the specified 'variable' the 'TYPE' value read from the
        // specified input 'stream', and return a reference to 'stream'.  If
        // 'stream' is initially invalid, this operation has no effect.  If
        // needed, first read the version information from the 'stream' and if
        // this version is not supported by 'TYPE', 'stream' is invalidated,
        // but otherwise unmodified.  If 'stream' becomes invalid during this
        // operation, 'variable' has an undefined, but valid, state.  Note that
        // the version is only needed when the (template parameter) 'TYPE' is a
        // 'bsl::vector' or a user-defined type.  See the 'bslx' package-level
        // documentation for more information on BDEX streaming of
        // value-semantic types and containers.

    template <class STREAM, class TYPE>
    STREAM& bdexStreamIn(STREAM& stream, TYPE& variable, int version);
        // Assign to the specified 'variable' the 'TYPE' value read from the
        // specified input 'stream' using the specified 'version' format, and
        // return a reference to 'stream'.  If 'stream' is initially invalid,
        // this operation has no effect.  If 'version' is not supported by
        // 'TYPE', 'variable' is unaltered and 'stream' is invalidated, but
        // otherwise unmodified.  If 'version' is supported but 'stream'
        // becomes invalid during this operation, 'variable' has an undefined,
        // but valid, state.  See the 'bslx' package-level documentation for
        // more information on BDEX streaming of value-semantic types and
        // containers.

                       /* overloads */

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, bool& variable, int version = 0);
        // Assign to the specified 'variable' the 'bool' value read from the
        // specified input 'stream', and return a reference to 'stream'.  The
        // optionally specified 'version' is ignored.  If 'stream' is initially
        // invalid, this operation has no effect.  If 'stream' becomes invalid
        // during this operation, 'variable' has an undefined, but valid,
        // state.  See the 'bslx' package-level documentation for more
        // information on BDEX streaming of value-semantic types and
        // containers.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, char& variable, int version = 0);
        // Assign to the specified 'variable' the 'char' value read from the
        // specified input 'stream', and return a reference to 'stream'.  The
        // optionally specified 'version' is ignored.  If 'stream' is initially
        // invalid, this operation has no effect.  If 'stream' becomes invalid
        // during this operation, 'variable' has an undefined, but valid,
        // state.  See the 'bslx' package-level documentation for more
        // information on BDEX streaming of value-semantic types and
        // containers.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM&      stream,
                         signed char& variable,
                         int          version = 0);
        // Assign to the specified 'variable' the 'signed char' value read from
        // the specified input 'stream', and return a reference to 'stream'.
        // The optionally specified 'version' is ignored.  If 'stream' is
        // initially invalid, this operation has no effect.  If 'stream'
        // becomes invalid during this operation, 'variable' has an undefined,
        // but valid, state.  See the 'bslx' package-level documentation for
        // more information on BDEX streaming of value-semantic types and
        // containers.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM&        stream,
                         unsigned char& variable,
                         int            version = 0);
        // Assign to the specified 'variable' the 'unsigned char' value read
        // from the specified input 'stream', and return a reference to
        // 'stream'.  The optionally specified 'version' is ignored.  If
        // 'stream' is initially invalid, this operation has no effect.  If
        // 'stream' becomes invalid during this operation, 'variable' has an
        // undefined, but valid, state.  See the 'bslx' package-level
        // documentation for more information on BDEX streaming of
        // value-semantic types and containers.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, short& variable, int version = 0);
        // Assign to the specified 'variable' the 'short' value read from the
        // specified input 'stream', and return a reference to 'stream'.  The
        // optionally specified 'version' is ignored.  If 'stream' is initially
        // invalid, this operation has no effect.  If 'stream' becomes invalid
        // during this operation, 'variable' has an undefined, but valid,
        // state.  See the 'bslx' package-level documentation for more
        // information on BDEX streaming of value-semantic types and
        // containers.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM&         stream,
                         unsigned short& variable,
                         int             version = 0);
        // Assign to the specified 'variable' the 'unsigned short' value read
        // from the specified input 'stream', and return a reference to
        // 'stream'.  The optionally specified 'version' is ignored.  If
        // 'stream' is initially invalid, this operation has no effect.  If
        // 'stream' becomes invalid during this operation, 'variable' has an
        // undefined, but valid, state.  See the 'bslx' package-level
        // documentation for more information on BDEX streaming of
        // value-semantic types and containers.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int& variable, int version = 0);
        // Assign to the specified 'variable' the 'int' value read from the
        // specified input 'stream', and return a reference to 'stream'.  The
        // optionally specified 'version' is ignored.  If 'stream' is initially
        // invalid, this operation has no effect.  If 'stream' becomes invalid
        // during this operation, 'variable' has an undefined, but valid,
        // state.  See the 'bslx' package-level documentation for more
        // information on BDEX streaming of value-semantic types and
        // containers.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM&       stream,
                         unsigned int& variable,
                         int           version = 0);
        // Assign to the specified 'variable' the 'unsigned int' value read
        // from the specified input 'stream', and return a reference to
        // 'stream'.  The optionally specified 'version' is ignored.  If
        // 'stream' is initially invalid, this operation has no effect.  If
        // 'stream' becomes invalid during this operation, 'variable' has an
        // undefined, but valid, state.  See the 'bslx' package-level
        // documentation for more information on BDEX streaming of
        // value-semantic types and containers.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, long& variable, int version = 0);
        // Assign to the specified 'variable' the 32-bit 'int' value read from
        // the specified input 'stream', and return a reference to 'stream'.
        // The optionally specified 'version' is ignored.  If 'stream' is
        // initially invalid, this operation has no effect.  If 'stream'
        // becomes invalid during this operation, 'variable' has an undefined,
        // but valid, state.  See the 'bslx' package-level documentation for
        // more information on BDEX streaming of value-semantic types and
        // containers.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM&        stream,
                         unsigned long& variable,
                         int            version = 0);
        // Assign to the specified 'variable' the 32-bit 'unsigned int' value
        // read from the specified input 'stream', and return a reference to
        // 'stream'.  The optionally specified 'version' is ignored.  If
        // 'stream' is initially invalid, this operation has no effect.  If
        // 'stream' becomes invalid during this operation, 'variable' has an
        // undefined, but valid, state.  See the 'bslx' package-level
        // documentation for more information on BDEX streaming of
        // value-semantic types and containers.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM&             stream,
                         bsls::Types::Int64& variable,
                         int                 version = 0);
        // Assign to the specified 'variable' the 'bsls::Types::Int64' value
        // read from the specified input 'stream', and return a reference to
        // 'stream'.  The optionally specified 'version' is ignored.  If
        // 'stream' is initially invalid, this operation has no effect.  If
        // 'stream' becomes invalid during this operation, 'variable' has an
        // undefined, but valid, state.  See the 'bslx' package-level
        // documentation for more information on BDEX streaming of
        // value-semantic types and containers.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM&              stream,
                         bsls::Types::Uint64& variable,
                         int                  version = 0);
        // Assign to the specified 'variable' the 'bsls::Types::Uint64' value
        // read from the specified input 'stream', and return a reference to
        // 'stream'.  The optionally specified 'version' is ignored.  If
        // 'stream' is initially invalid, this operation has no effect.  If
        // 'stream' becomes invalid during this operation, 'variable' has an
        // undefined, but valid, state.  See the 'bslx' package-level
        // documentation for more information on BDEX streaming of
        // value-semantic types and containers.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, float& variable, int version = 0);
        // Assign to the specified 'variable' the 'float' value read from the
        // specified input 'stream', and return a reference to 'stream'.  The
        // optionally specified 'version' is ignored.  If 'stream' is initially
        // invalid, this operation has no effect.  If 'stream' becomes invalid
        // during this operation, 'variable' has an undefined, but valid,
        // state.  See the 'bslx' package-level documentation for more
        // information on BDEX streaming of value-semantic types and
        // containers.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, double& variable, int version = 0);
        // Assign to the specified 'variable' the 'double' value read from the
        // specified input 'stream', and return a reference to 'stream'.  The
        // optionally specified 'version' is ignored.  If 'stream' is initially
        // invalid, this operation has no effect.  If 'stream' becomes invalid
        // during this operation, 'variable' has an undefined, but valid,
        // state.  See the 'bslx' package-level documentation for more
        // information on BDEX streaming of value-semantic types and
        // containers.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM&      stream,
                         bsl::string& variable,
                         int          version = 0);
        // Assign to the specified 'variable' the 'bsl::string' value read from
        // the specified input 'stream', and return a reference to 'stream'.
        // The optionally specified 'version' is ignored.  If 'stream' is
        // initially invalid, this operation has no effect.  If 'stream'
        // becomes invalid during this operation, 'variable' has an undefined,
        // but valid, state.  See the 'bslx' package-level documentation for
        // more information on BDEX streaming of value-semantic types and
        // containers.

    template <class STREAM, class ALLOC>
    STREAM& bdexStreamIn(STREAM&                   stream,
                         bsl::vector<char, ALLOC>& variable,
                         int                       version);
        // Assign to the specified 'variable' the 'bsl::vector<char, ALLOC>'
        // value read from the specified input 'stream', and return a reference
        // to 'stream'.  The specified 'version' is ignored.  If 'stream' is
        // initially invalid, this operation has no effect.  If 'stream'
        // becomes invalid during this operation, 'variable' has an undefined,
        // but valid, state.  See the 'bslx' package-level documentation for
        // more information on BDEX streaming of value-semantic types and
        // containers.

    template <class STREAM, class ALLOC>
    STREAM& bdexStreamIn(STREAM&                          stream,
                         bsl::vector<signed char, ALLOC>& variable,
                         int                              version);
        // Assign to the specified 'variable' the
        // 'bsl::vector<signed char, ALLOC>' value read from the specified
        // input 'stream', and return a reference to 'stream'.  The specified
        // 'version' is ignored.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, 'variable' has an undefined, but valid, state.  See the
        // 'bslx' package-level documentation for more information on BDEX
        // streaming of value-semantic types and containers.

    template <class STREAM, class ALLOC>
    STREAM& bdexStreamIn(STREAM&                            stream,
                         bsl::vector<unsigned char, ALLOC>& variable,
                         int                                version);
        // Assign to the specified 'variable' the
        // 'bsl::vector<unsigned char, ALLOC>' value read from the specified
        // input 'stream', and return a reference to 'stream'.  The specified
        // 'version' is ignored.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, 'variable' has an undefined, but valid, state.  See the
        // 'bslx' package-level documentation for more information on BDEX
        // streaming of value-semantic types and containers.

    template <class STREAM, class ALLOC>
    STREAM& bdexStreamIn(STREAM&                    stream,
                         bsl::vector<short, ALLOC>& variable,
                         int                        version);
        // Assign to the specified 'variable' the 'bsl::vector<short, ALLOC>'
        // value read from the specified input 'stream', and return a reference
        // to 'stream'.  The specified 'version' is ignored.  If 'stream' is
        // initially invalid, this operation has no effect.  If 'stream'
        // becomes invalid during this operation, 'variable' has an undefined,
        // but valid, state.  See the 'bslx' package-level documentation for
        // more information on BDEX streaming of value-semantic types and
        // containers.

    template <class STREAM, class ALLOC>
    STREAM& bdexStreamIn(STREAM&                             stream,
                         bsl::vector<unsigned short, ALLOC>& variable,
                         int                                 version);
        // Assign to the specified 'variable' the
        // 'bsl::vector<unsigned short, ALLOC>' value read from the specified
        // input 'stream', and return a reference to 'stream'.  The specified
        // 'version' is ignored.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, 'variable' has an undefined, but valid, state.  See the
        // 'bslx' package-level documentation for more information on BDEX
        // streaming of value-semantic types and containers.

    template <class STREAM, class ALLOC>
    STREAM& bdexStreamIn(STREAM&                  stream,
                         bsl::vector<int, ALLOC>& variable,
                         int                      version);
        // Assign to the specified 'variable' the 'bsl::vector<int, ALLOC>'
        // value read from the specified input 'stream', and return a reference
        // to 'stream'.  The specified 'version' is ignored.  If 'stream' is
        // initially invalid, this operation has no effect.  If 'stream'
        // becomes invalid during this operation, 'variable' has an undefined,
        // but valid, state.  See the 'bslx' package-level documentation for
        // more information on BDEX streaming of value-semantic types and
        // containers.

    template <class STREAM, class ALLOC>
    STREAM& bdexStreamIn(STREAM&                           stream,
                         bsl::vector<unsigned int, ALLOC>& variable,
                         int                               version);
        // Assign to the specified 'variable' the
        // 'bsl::vector<unsigned int, ALLOC>' value read from the specified
        // input 'stream', and return a reference to 'stream'.  The specified
        // 'version' is ignored.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, 'variable' has an undefined, but valid, state.  See the
        // 'bslx' package-level documentation for more information on BDEX
        // streaming of value-semantic types and containers.

    template <class STREAM, class ALLOC>
    STREAM& bdexStreamIn(STREAM&                                 stream,
                         bsl::vector<bsls::Types::Int64, ALLOC>& variable,
                         int                                     version);
        // Assign to the specified 'variable' the
        // 'bsl::vector<bsls::Types::Int64, ALLOC>' value read from the
        // specified input 'stream', and return a reference to 'stream'.  The
        // specified 'version' is ignored.  If 'stream' is initially invalid,
        // this operation has no effect.  If 'stream' becomes invalid during
        // this operation, 'variable' has an undefined, but valid, state.  See
        // the 'bslx' package-level documentation for more information on BDEX
        // streaming of value-semantic types and containers.

    template <class STREAM, class ALLOC>
    STREAM& bdexStreamIn(STREAM&                                  stream,
                         bsl::vector<bsls::Types::Uint64, ALLOC>& variable,
                         int                                      version);
        // Assign to the specified 'variable' the
        // 'bsl::vector<bsls::Types::Uint64, ALLOC>' value read from the
        // specified input 'stream', and return a reference to 'stream'.  The
        // specified 'version' is ignored.  If 'stream' is initially invalid,
        // this operation has no effect.  If 'stream' becomes invalid during
        // this operation, 'variable' has an undefined, but valid, state.  See
        // the 'bslx' package-level documentation for more information on BDEX
        // streaming of value-semantic types and containers.

    template <class STREAM, class ALLOC>
    STREAM& bdexStreamIn(STREAM&                    stream,
                         bsl::vector<float, ALLOC>& variable,
                         int                        version);
        // Assign to the specified 'variable' the 'bsl::vector<float, ALLOC>'
        // value read from the specified input 'stream', and return a reference
        // to 'stream'.  The specified 'version' is ignored.  If 'stream' is
        // initially invalid, this operation has no effect.  If 'stream'
        // becomes invalid during this operation, 'variable' has an undefined,
        // but valid, state.  See the 'bslx' package-level documentation for
        // more information on BDEX streaming of value-semantic types and
        // containers.

    template <class STREAM, class ALLOC>
    STREAM& bdexStreamIn(STREAM&                     stream,
                         bsl::vector<double, ALLOC>& variable,
                         int                         version);
        // Assign to the specified 'variable' the 'bsl::vector<double, ALLOC>'
        // value read from the specified input 'stream', and return a reference
        // to 'stream'.  The specified 'version' is ignored.  If 'stream' is
        // initially invalid, this operation has no effect.  If 'stream'
        // becomes invalid during this operation, 'variable' has an undefined,
        // but valid, state.  See the 'bslx' package-level documentation for
        // more information on BDEX streaming of value-semantic types and
        // containers.

    template <class STREAM, class TYPE, class ALLOC>
    STREAM& bdexStreamIn(STREAM&                   stream,
                         bsl::vector<TYPE, ALLOC>& variable);
        // Assign to the specified 'variable' the 'bsl::vector<TYPE, ALLOC>'
        // value read from the specified input 'stream', and return a reference
        // to 'stream'.  If 'stream' is initially invalid, this operation has
        // no effect.  First read the version information from the 'stream' and
        // if this version is not supported by 'TYPE' and the vector is not
        // empty, 'stream' is invalidated, but otherwise unmodified.  If
        // 'stream' becomes invalid during this operation, 'variable' has an
        // undefined, but valid, state.  See the 'bslx' package-level
        // documentation for more information on BDEX streaming of
        // value-semantic types and containers.

    template <class STREAM, class TYPE, class ALLOC>
    STREAM& bdexStreamIn(STREAM&                   stream,
                         bsl::vector<TYPE, ALLOC>& variable,
                         int                       version);
        // Assign to the specified 'variable' the 'bsl::vector<TYPE, ALLOC>'
        // value read from the specified input 'stream' using the specified
        // 'version' format, and return a reference to 'stream'.  If 'stream'
        // is initially invalid, this operation has no effect.  If 'version' is
        // not supported by 'TYPE' and the vector is not empty, 'stream' is
        // invalidated, but otherwise unmodified.  If 'stream' becomes invalid
        // during this operation, 'variable' has an undefined, but valid,
        // state.  See the 'bslx' package-level documentation for more
        // information on BDEX streaming of value-semantic types and
        // containers.

}  // close namespace InStreamFunctions

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                         // ---------------------------
                         // namespace InStreamFunctions
                         // ---------------------------

template <class STREAM, class TYPE>
inline
STREAM& InStreamFunctions::bdexStreamInImp(STREAM&           stream,
                                           TYPE&             variable,
                                           int            /* version */,
                                           const IsEnumType&)
{
    int enumVariable = 0;
    stream.getInt32(enumVariable);

    if (stream) {
        variable = static_cast<TYPE>(enumVariable);
    }
    return stream;
}

template <class STREAM, class TYPE>
inline
STREAM& InStreamFunctions::bdexStreamInImp(STREAM&              stream,
                                           TYPE&                variable,
                                           int                  version,
                                           const IsNotEnumType&)
{
    // A compilation error indicating the next line of code implies the class
    // of 'TYPE' does not support the 'bdexStreamIn' method.

    return variable.bdexStreamIn(stream, version);
}

template <class STREAM, class TYPE>
inline
STREAM& InStreamFunctions::bdexStreamIn(STREAM& stream, TYPE& variable)
{
    using VersionFunctions::maxSupportedBdexVersion;

    // Determine if the 'TYPE' requires a version to be externalized using an
    // arbitrary value for 'versionSelector'.

    int version = maxSupportedBdexVersion(&variable, 0);
    if (VersionFunctions::k_NO_VERSION != version) {
        stream.getVersion(version);

        if (!stream) {
            return stream;                                            // RETURN
        }
    }

    return bdexStreamIn(stream, variable, version);
}

template <class STREAM, class TYPE>
inline
STREAM& InStreamFunctions::bdexStreamIn(STREAM& stream,
                                        TYPE&   variable,
                                        int     version)
{
    typedef typename bslmf::If<bslmf::IsEnum<TYPE>::value,
                               IsEnumType,
                               IsNotEnumType>::Type dummyType;
    return bdexStreamInImp(stream, variable, version, dummyType());
}

template <class STREAM>
inline
STREAM& InStreamFunctions::bdexStreamIn(STREAM& stream,
                                        bool&   variable,
                                        int  /* version */)
{
    char temp = 0;

    stream.getInt8(temp);
    variable = static_cast<bool>(temp);

    return stream;
}

template <class STREAM>
inline
STREAM& InStreamFunctions::bdexStreamIn(STREAM& stream,
                                        char&   variable,
                                        int  /* version */)
{
    return stream.getInt8(variable);
}

template <class STREAM>
inline
STREAM& InStreamFunctions::bdexStreamIn(STREAM&      stream,
                                        signed char& variable,
                                        int       /* version */)
{
    return stream.getInt8(variable);
}

template <class STREAM>
inline
STREAM& InStreamFunctions::bdexStreamIn(STREAM&        stream,
                                        unsigned char& variable,
                                        int         /* version */)
{
    return stream.getUint8(variable);
}

template <class STREAM>
inline
STREAM& InStreamFunctions::bdexStreamIn(STREAM& stream,
                                        short&  variable,
                                        int  /* version */)
{
    return stream.getInt16(variable);
}

template <class STREAM>
inline
STREAM& InStreamFunctions::bdexStreamIn(STREAM&         stream,
                                        unsigned short& variable,
                                        int          /* version */)
{
    return stream.getUint16(variable);
}

template <class STREAM>
inline
STREAM& InStreamFunctions::bdexStreamIn(STREAM& stream,
                                        int&    variable,
                                        int  /* version */)
{
    return stream.getInt32(variable);
}

template <class STREAM>
inline
STREAM& InStreamFunctions::bdexStreamIn(STREAM&       stream,
                                        unsigned int& variable,
                                        int        /* version */)
{
    return stream.getUint32(variable);
}

template <class STREAM>
inline
STREAM& InStreamFunctions::bdexStreamIn(STREAM& stream,
                                        long&   variable,
                                        int  /* version */)
{
    int temp = 0;  // 'long' and 'int' may not be the same size.
    stream.getInt32(temp);
    variable = temp;
    return stream;
}

template <class STREAM>
inline
STREAM& InStreamFunctions::bdexStreamIn(STREAM&        stream,
                                        unsigned long& variable,
                                        int         /* version */)
{
    unsigned int temp = 0;  // 'unsigned long' and 'unsigned int' may not be
                            // the same size.
    stream.getUint32(temp);
    variable = temp;
    return stream;
}

template <class STREAM>
inline
STREAM& InStreamFunctions::bdexStreamIn(STREAM&             stream,
                                        bsls::Types::Int64& variable,
                                        int              /* version */)
{
    return stream.getInt64(variable);
}

template <class STREAM>
inline
STREAM& InStreamFunctions::bdexStreamIn(STREAM&              stream,
                                        bsls::Types::Uint64& variable,
                                        int               /* version */)
{
    return stream.getUint64(variable);
}

template <class STREAM>
inline
STREAM& InStreamFunctions::bdexStreamIn(STREAM& stream,
                                        float&  variable,
                                        int  /* version */)
{
    return stream.getFloat32(variable);
}

template <class STREAM>
inline
STREAM& InStreamFunctions::bdexStreamIn(STREAM& stream,
                                        double& variable,
                                        int  /* version */)
{
    return stream.getFloat64(variable);
}

template <class STREAM>
inline
STREAM& InStreamFunctions::bdexStreamIn(STREAM&      stream,
                                        bsl::string& variable,
                                        int       /* version */)
{
    return stream.getString(variable);
}

template <class STREAM, class ALLOC>
inline
STREAM& InStreamFunctions::bdexStreamIn(STREAM&                   stream,
                                        bsl::vector<char, ALLOC>& variable,
                                        int                    /* version */)
{
    int length = 0;
    stream.getLength(length);

    if (!stream) {
        return stream;                                                // RETURN
    }

    variable.resize(length);

    return 0 < length ? stream.getArrayInt8(&variable[0], length) : stream;
}

template <class STREAM, class ALLOC>
inline
STREAM& InStreamFunctions::bdexStreamIn(
                                   STREAM&                          stream,
                                   bsl::vector<signed char, ALLOC>& variable,
                                   int                           /* version */)
{
    int length = 0;
    stream.getLength(length);

    if (!stream) {
        return stream;                                                // RETURN
    }

    variable.resize(length);

    return 0 < length ? stream.getArrayInt8(&variable[0], length) : stream;
}

template <class STREAM, class ALLOC>
inline
STREAM& InStreamFunctions::bdexStreamIn(
                                 STREAM&                            stream,
                                 bsl::vector<unsigned char, ALLOC>& variable,
                                 int                             /* version */)
{
    int length = 0;
    stream.getLength(length);

    if (!stream) {
        return stream;                                                // RETURN
    }

    variable.resize(length);

    return 0 < length ? stream.getArrayUint8(&variable[0], length) : stream;
}

template <class STREAM, class ALLOC>
inline
STREAM& InStreamFunctions::bdexStreamIn(STREAM&                    stream,
                                        bsl::vector<short, ALLOC>& variable,
                                        int                     /* version */)
{
    int length = 0;
    stream.getLength(length);

    if (!stream) {
        return stream;                                                // RETURN
    }

    variable.resize(length);

    return 0 < length ? stream.getArrayInt16(&variable[0], length) : stream;
}

template <class STREAM, class ALLOC>
inline
STREAM& InStreamFunctions::bdexStreamIn(
                                STREAM&                             stream,
                                bsl::vector<unsigned short, ALLOC>& variable,
                                int                              /* version */)
{
    int length = 0;
    stream.getLength(length);

    if (!stream) {
        return stream;                                                // RETURN
    }

    variable.resize(length);

    return 0 < length ? stream.getArrayUint16(&variable[0], length) : stream;
}

template <class STREAM, class ALLOC>
inline
STREAM& InStreamFunctions::bdexStreamIn(STREAM&                  stream,
                                        bsl::vector<int, ALLOC>& variable,
                                        int                   /* version */)
{
    int length = 0;
    stream.getLength(length);

    if (!stream) {
        return stream;                                                // RETURN
    }

    variable.resize(length);

    return 0 < length ? stream.getArrayInt32(&variable[0], length) : stream;
}

template <class STREAM, class ALLOC>
inline
STREAM& InStreamFunctions::bdexStreamIn(
                                  STREAM&                           stream,
                                  bsl::vector<unsigned int, ALLOC>& variable,
                                  int                            /* version */)
{
    int length = 0;
    stream.getLength(length);

    if (!stream) {
        return stream;                                                // RETURN
    }

    variable.resize(length);

    return 0 < length ? stream.getArrayUint32(&variable[0], length) : stream;
}

template <class STREAM, class ALLOC>
inline
STREAM& InStreamFunctions::bdexStreamIn(
                            STREAM&                                 stream,
                            bsl::vector<bsls::Types::Int64, ALLOC>& variable,
                            int                                  /* version */)
{
    int length = 0;
    stream.getLength(length);

    if (!stream) {
        return stream;                                                // RETURN
    }

    variable.resize(length);

    return 0 < length ? stream.getArrayInt64(&variable[0], length) : stream;
}

template <class STREAM, class ALLOC>
inline
STREAM& InStreamFunctions::bdexStreamIn(
                           STREAM&                                  stream,
                           bsl::vector<bsls::Types::Uint64, ALLOC>& variable,
                           int                                   /* version */)
{
    int length = 0;
    stream.getLength(length);

    if (!stream) {
        return stream;                                                // RETURN
    }

    variable.resize(length);

    return 0 < length ? stream.getArrayUint64(&variable[0], length) : stream;
}

template <class STREAM, class ALLOC>
inline
STREAM& InStreamFunctions::bdexStreamIn(STREAM&                    stream,
                                        bsl::vector<float, ALLOC>& variable,
                                        int                     /* version */)
{
    int length = 0;
    stream.getLength(length);

    if (!stream) {
        return stream;                                                // RETURN
    }

    variable.resize(length);

    return 0 < length ? stream.getArrayFloat32(&variable[0], length) : stream;
}

template <class STREAM, class ALLOC>
inline
STREAM& InStreamFunctions::bdexStreamIn(STREAM&                     stream,
                                        bsl::vector<double, ALLOC>& variable,
                                        int                      /* version */)
{
    int length = 0;
    stream.getLength(length);

    if (!stream) {
        return stream;                                                // RETURN
    }

    variable.resize(length);

    return 0 < length ? stream.getArrayFloat64(&variable[0], length) : stream;
}

template <class STREAM, class TYPE, class ALLOC>
inline
STREAM& InStreamFunctions::bdexStreamIn(STREAM&                   stream,
                                        bsl::vector<TYPE, ALLOC>& variable)
{
    int version = 0;
    stream.getVersion(version);

    if (!stream) {
        return stream;                                                // RETURN
    }

    return bdexStreamIn(stream, variable, version);
}

template <class STREAM, class TYPE, class ALLOC>
STREAM& InStreamFunctions::bdexStreamIn(STREAM&                   stream,
                                        bsl::vector<TYPE, ALLOC>& variable,
                                        int                       version)
{
    typedef typename bsl::vector<TYPE, ALLOC>::iterator Iterator;

    int length = 0;
    stream.getLength(length);

    if (!stream) {
        return stream;                                                // RETURN
    }

    variable.resize(length);

    for (Iterator it = variable.begin(); it != variable.end(); ++it) {

        bdexStreamIn(stream, *it, version);

        if (!stream) {
            return stream;                                            // RETURN
        }
    }

    return stream;
}

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
