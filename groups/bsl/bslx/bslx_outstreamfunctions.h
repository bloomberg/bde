// bslx_outstreamfunctions.h                                          -*-C++-*-
#ifndef INCLUDED_BSLX_OUTSTREAMFUNCTIONS
#define INCLUDED_BSLX_OUTSTREAMFUNCTIONS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Facilitate uniform externalization of user and fundamental types.
//
//@CLASSES:
//  bslx::OutStreamFunctions: namespace for BDEX externalization functions
//
//@SEE_ALSO: bslx_instreamfunctions, bslx_versionfunctions
//
//@DESCRIPTION: This component provides a namespace,
// 'bslx::OutStreamFunctions', that facilitates uniform support for BDEX
// externalization across all BDEX-compliant user-defined types, including
// template types and containers, as well as those fundamental types (and
// 'bsl::string' and 'bsl::vector') for which the BDEX protocol provides direct
// support.
//
// The namespace 'bslx::OutStreamFunctions' facilitates client externalization
// of objects in a uniform, type-independent manner.  It contains the
// 'bdexStreamOut' function that externalizes objects of all BDEX-compliant
// types.  This function externalizes the specified 'object' in the specified
// 'version'.  The 'bdexStreamOut' function is overloaded for fundamental
// types, enumeration types, 'bsl::string', and 'bsl::vector'.  Note that,
// excluding 'bsl::vector', version information is never written to the stream
// while externalizing these types.
//
// By default, objects of enumeration type are streamed out as 32-bit 'int'
// values.  Users can override this behavior by providing overloads of the
// 'OutStreamFunctions::bdexStreamOut' function in the enumeration's namespace
// for their enumeration types.  The general form of this overload is:
//..
//  template <class STREAM>
//  STREAM& bdexStreamOut(STREAM& stream, const MyEnum& value, int version)
//  {
//      using bslx::OutStreamFunctions::bdexStreamOut;
//
//      // Code to stream out objects of 'MyEnum' type.
//
//      return stream;
//  }
//..
// For value-semantic types that support the BDEX protocol, the free function
// 'bdexStreamOut' calls the 'bdexStreamOut' member function for that type.
//
///Component Design, Anticipated Usage, and the BDEX Contract
///----------------------------------------------------------
// 'bslx_outstreamfunctions' is an integral part of the BDEX externalization
// contract.  The BDEX contract is at least in part "collaborative", which is
// to say that each developer of a given *kind* of component (e.g., a stream or
// a value-semantic container) must comply with the relevant portions of the
// contract to ensure that the "system as a whole" works for everybody.
// 'bslx_outstreamfunctions' plays several related but different roles in
// helping various developers to produce BDEX-compliant components.  In this
// section we briefly highlight how and why 'bslx_outstreamfunctions' is
// helpful (or required) for these different developers.  By discussing
// different aspects of usage, we convey the general design goals of this
// component, and, to a certain extent, the overall BDEX contract.  See the
// 'bslx' package-level documentation for a full specification of the BDEX
// contract.
//
///Implementing BDEX Streaming in Value-Semantic Template Classes
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The author of a non-template value-semantic type has full knowledge of the
// details of the "value" of that type, and may choose to use the appropriate
// output stream 'put' methods directly when implementing the required
// 'bdexStreamOut' method for that type.  However, if one or more aspects of
// the value are of template parameter type, then the author cannot in general
// know how to stream the value using the 'put' methods.  For example, if a
// type has as its value one 'int' data member:
//..
//  int d_value;
//..
// then the implementation of the 'bdexStreamOut' method can contain:
//..
//  stream.putInt32(d_value);
//..
// However, if the data member is of (template parameter) 'TYPE':
//..
//  TYPE d_value;
//..
// then the implementation of the 'bdexStreamOut' method must rely on the
// 'bslx::OutStreamFunctions' implementation to output the value:
//..
//  using bslx::OutStreamFunctions::bdexStreamOut;
//  bdexStreamOut(stream, d_value, 1);
//..
// This call will resolve to the correct sequence of 'put' calls no matter
// whether 'TYPE' is a fundamental type, a BDEX-compliant 'enum', or a proper
// BDEX-compliant class.  In the latter two cases, the explicit specification
// of the version format (in this case, 1) guarantees the stable operation of
// this method whether or not 'TYPE' is provided additional version formats.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using 'bslx::OutStreamFunctions' to Externalize Data
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example we illustrate the primary intended use of the parameterized
// methods of this component, as well as a few trivial invocations just to show
// the syntax clearly.  To accomplish this, we exhibit two separate example
// "components": a value-semantic point object, and an 'enum'.  In all cases,
// the component designs are very simple, with much of the implied
// functionality omitted, in order to focus attention on the key aspects of the
// functionality of *this* component.
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
// 'short' integer values; this is done to make the BDEX stream output byte
// pattern somewhat easier for the reader of this example to recognize when the
// output buffer is printed.
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
//      static int maxSupportedBdexVersion(int versionSelector);
//          // Return the maximum valid BDEX format version, as indicated by
//          // the specified 'versionSelector', to be passed to the
//          // 'bdexStreamOut' method.  Note that it is highly recommended that
//          // versionSelector' be formatted as "YYYYMMDD", a date
//          // representation.  Also note that 'versionSelector' should be a
//          // *compile*-time-chosen value that selects a format version
//          // supported by both externalizer and unexternalizer.  See the
//          // 'bslx' package-level documentation for more information on BDEX
//          // streaming of value-semantic types and containers.
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
//      int x() const;
//          // Return the x coordinate of this point.
//
//      int y() const;
//          // Return the y coordinate of this point.
//
//      Color color() const;
//          // Return the enumerated color of this point.
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
//  // CLASS METHODS
//  inline
//  int MyPoint::maxSupportedBdexVersion(int versionSelector)
//  {
//      if (versionSelector >= 20131201) {
//          return 2;
//      }
//      return 1;
//  }
//
//  // CREATORS
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
//  int MyPoint::x() const
//  {
//      return d_x;
//  }
//
//  // ...
//
//  template <class STREAM>
//  STREAM& MyPoint::bdexStreamOut(STREAM& stream, int version) const
//  {
//      switch (version) {
//        case 1: {
//          stream.putInt16(d_x);          // output the x coordinate
//          stream.putInt16(d_y);          // output the y coordinate
//          stream.putInt8(static_cast<char>(d_color));
//                                         // output the color enum as one byte
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
// Then, we will implement an extremely simple output stream that supports the
// BDEX documentation-only protocol.  For simplicity, we will use a fixed-size
// buffer (usually a bad idea in any event, and more so here since the
// implementation knows the buffer size, but makes no effort to prevent
// overwriting that buffer), and will only show a few methods needed for this
// example.  See other 'bslx' stream components for examples of
// properly-designed BDEX streams.
//..
//  // myoutstream.h
//  // ...
//
//  class MyOutStream {
//      // This class implements a limited-size fixed-buffer output stream that
//      // partially conforms to the BDEX protocol for output streams.  This
//      // class is suitable for demonstration purposes only.
//
//      char d_buffer[1000]; // externalized values stored as contiguous bytes
//
//      int  d_length;       // length of data in 'd_buffer' (in bytes)
//
//      bool d_validFlag;    // stream validity flag; 'true' if stream is in
//                           // valid state, 'false' otherwise
//
//    public:
//      // CREATORS
//      MyOutStream();
//          // Create an empty output stream of limited, fixed capacity.  Note
//          // that this object is suitable for demonstration purposes only.
//
//      ~MyOutStream();
//         // Destroy this output stream.
//
//      // MANIPULATORS
//      void invalidate();
//          // Put this input stream in an invalid state.  This function has no
//          // effect if this stream is already invalid.  Note that this
//          // function should be called whenever a value extracted from this
//          // stream is determined to be invalid, inconsistent, or otherwise
//          // incorrect.
//
//      MyOutStream& putVersion(int version);
//          // Write to this stream the one-byte, two's complement integer
//          // comprised of the least-significant one byte of the specified
//          // 'version', and return a reference to this stream.
//
//      MyOutStream& putInt32(int value);
//          // Write to this stream the four-byte, two's complement integer (in
//          // network byte order) comprised of the least-significant four
//          // bytes of the specified 'value' (in host byte order), and return
//          // a reference to this stream.
//
//      MyOutStream& putInt16(int value);
//          // Write to this stream the two-byte, two's complement integer
//          // (in network byte order) comprised of the least-significant two
//          // bytes of the specified 'value' (in host byte order), and return
//          // a reference to this stream.
//
//      MyOutStream& putInt8(int value);
//          // Write to this stream the one-byte, two's complement integer
//          // comprised of the least-significant one byte of the specified
//          // 'value', and return a reference to this stream.
//
//      void removeAll();
//          // Remove all content in this stream.
//
//      // ACCESSORS
//      const char *data() const;
//          // Return the address of the contiguous, non-modifiable internal
//          // memory buffer of this stream.  The address will remain valid as
//          // long as this stream is not destroyed or modified.  The behavior
//          // of accessing elements outside the range
//          // '[ data() .. data() + (length() - 1) ]' is undefined.
//
//      int length() const;
//          // Return the number of bytes in this stream.
//  };
//
//  // FREE OPERATORS
//  inline
//  bsl::ostream& operator<<(bsl::ostream&      stream,
//                           const MyOutStream& object);
//      // Write the specified 'object' to the specified output 'stream' in
//      // some reasonable (multi-line) format, and return a reference to
//      // 'stream'.
//..
// The relevant (inline) implementations are as follows.
//..
//  // ========================================================================
//  //                      INLINE FUNCTION DEFINITIONS
//  // ========================================================================
//
//  // CREATORS
//  inline
//  MyOutStream::MyOutStream()
//  : d_length(0)
//  , d_validFlag(true)
//  {
//  }
//
//  inline
//  MyOutStream::~MyOutStream()
//  {
//  }
//
//  // MANIPULATORS
//  inline
//  void MyOutStream::invalidate()
//  {
//      d_validFlag = false;
//  }
//
//  inline
//  MyOutStream& MyOutStream::putVersion(int value)
//  {
//      d_buffer[d_length] = static_cast<char>(value);
//      ++d_length;
//      return *this;
//  }
//
//  inline
//  MyOutStream& MyOutStream::putInt32(int value)
//  {
//      d_buffer[d_length + 0] = static_cast<char>((value >> 24) & 0xff);
//      d_buffer[d_length + 1] = static_cast<char>((value >> 16) & 0xff);
//      d_buffer[d_length + 2] = static_cast<char>((value >>  8) & 0xff);
//      d_buffer[d_length + 3] = static_cast<char>((value >>  0) & 0xff);
//      d_length += 4;
//      return *this;
//  }
//
//  inline
//  MyOutStream& MyOutStream::putInt16(int value)
//  {
//      d_buffer[d_length + 0] = static_cast<char>((value >> 8) & 0xff);
//      d_buffer[d_length + 1] = static_cast<char>((value >> 0) & 0xff);
//      d_length += 2;
//      return *this;
//  }
//
//  inline
//  MyOutStream& MyOutStream::putInt8(int value)
//  {
//      d_buffer[d_length] = static_cast<char>(value);
//      d_length += 1;
//      return *this;
//  }
//
//  inline
//  void MyOutStream::removeAll()
//  {
//      d_length = 0;
//  }
//
//  // ACCESSORS
//  inline
//  const char *MyOutStream::data() const
//  {
//      return static_cast<const char *>(d_buffer);
//  }
//
//  inline
//  int MyOutStream::length() const
//  {
//      return d_length;
//  }
//..
// Finally, use the above 'enum', point class, and output stream to illustrate
// 'bslx::OutStreamFunctions' functionality.  This test code does not attempt
// to do anything more useful than writing known values to a stream and
// confirming that the expected byte pattern was in fact written.
//..
//  int             i     = 168496141;  // byte pattern 0a 0b 0c 0d
//  Color           color = BLUE;       // byte pattern 02
//  MyPoint         p(0, -1, color);    // byte pattern 00 00 ff ff 02
//
//  using bslx::OutStreamFunctions::bdexStreamOut;
//
//  MyOutStream out;
//  assert(0 == out.length());
//
//  bdexStreamOut(out, i, 1);
//  assert(4 == out.length());
//  assert(0 == bsl::memcmp(out.data(), "\x0a\x0b\x0c\x0d", out.length()));
//
//  out.removeAll();
//  assert(0 == out.length());
//
//  bdexStreamOut(out, i, 0);
//  assert(4 == out.length());
//  assert(0 == bsl::memcmp(out.data(), "\x0a\x0b\x0c\x0d", out.length()));
//
//  out.removeAll();
//  assert(0 == out.length());
//
//  bdexStreamOut(out, color, 1);
//  assert(4 == out.length());
//  assert(0 == bsl::memcmp(out.data(), "\x00\x00\x00\x02", out.length()));
//
//  out.removeAll();
//  assert(0 == out.length());
//
//  bdexStreamOut(out, color, 0);
//  assert(4 == out.length());
//  assert(0 == bsl::memcmp(out.data(), "\x00\x00\x00\x02", out.length()));
//
//  out.removeAll();
//  assert(0 == out.length());
//
//  bdexStreamOut(out, p, 1);
//  assert(5 == out.length());
//  assert(0 == bsl::memcmp(out.data(), "\x00\x00\xff\xff\x02", out.length()));
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLX_VERSIONFUNCTIONS
#include <bslx_versionfunctions.h>
#endif

#ifndef INCLUDED_BSLMF_ASSERT
#include <bslmf_assert.h>
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

#ifndef INCLUDED_BSL_CSTDINT
#include <bsl_cstdint.h>
#endif

namespace BloombergLP {
namespace bslx {

                         // ============================
                         // namespace OutStreamFunctions
                         // ============================

namespace OutStreamFunctions {
    // This namespace facilitates externalization of all BDEX-compliant types
    // in a type-independent manner.  The externalization functions are
    // overloaded for fundamental types, enumeration types, 'bsl::string', and
    // 'bsl::vector'.  A compilation error will occur if the (template
    // parameter) 'TYPE' of a non-overloaded method of
    // 'bslx::OutStreamFunctions' does not support 'bdexStreamOut' and
    // 'maxSupportedBdexVersion' (with the appropriate signatures).

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
    STREAM& bdexStreamOutImp(STREAM&           stream,
                             const TYPE&       value,
                             int               version,
                             const IsEnumType&);
        // Write the specified 'value' to the specified output 'stream' as a
        // 32-bit 'int', and return a reference to 'stream'.  The specified
        // 'version' is ignored.  If 'stream' is initially invalid, this
        // operation has no effect.  Note that this function is called only for
        // enumeration types and that if 'value' is outside the range of an
        // 'int32_t' the externalization will be lossy.  Also note that this
        // function is for internal use only.  See the 'bslx' package-level
        // documentation for more information on BDEX streaming of
        // value-semantic types and containers.

    template <class STREAM, class TYPE>
    STREAM& bdexStreamOutImp(STREAM&              stream,
                             const TYPE&          value,
                             int                  version,
                             const IsNotEnumType&);
        // Write the specified 'value', using the specified 'version' format,
        // to the specified output 'stream', and return a reference to
        // 'stream'.  If 'stream' is initially invalid, this operation has no
        // effect.  If 'version' is not supported by 'TYPE', 'stream' is
        // invalidated, but otherwise unmodified.  Note that 'version' is not
        // written to 'stream'.  Also note that this function is for internal
        // use only.  See the 'bslx' package-level documentation for more
        // information on BDEX streaming of value-semantic types and
        // containers.

    // CLASS METHODS
    template <class STREAM, class TYPE>
    STREAM& bdexStreamOut(STREAM& stream, const TYPE& value);
        // Write the specified 'value' to the specified output 'stream', and
        // return a reference to 'stream'.  If 'stream' is initially invalid,
        // this operation has no effect.  If needed, first write the computed
        // version information to the 'stream' and if this version is not
        // supported by 'TYPE', 'stream' is invalidated.  Note that the version
        // is only needed when the (template parameter) 'TYPE' is a
        // 'bsl::vector' or a user-defined type.  See the 'bslx' package-level
        // documentation for more information on BDEX streaming of
        // value-semantic types and containers.

    template <class STREAM, class TYPE>
    STREAM& bdexStreamOut(STREAM& stream, const TYPE& value, int version);
        // Write the specified 'value', using the specified 'version' format,
        // to the specified output 'stream', and return a reference to
        // 'stream'.  If 'stream' is initially invalid, this operation has no
        // effect.  If 'version' is not supported by 'TYPE', 'stream' is
        // invalidated, but otherwise unmodified.  Note that 'version' is not
        // written to 'stream'.  See the 'bslx' package-level documentation for
        // more information on BDEX streaming of value-semantic types and
        // containers.

                       /* overloads */

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, const bool& value, int version = 0);
        // Write the specified 'bool' 'value' to the specified output 'stream',
        // and return a reference to 'stream'.  The optionally specified
        // 'version' is ignored.  If 'stream' is initially invalid, this
        // operation has no effect.  See the 'bslx' package-level documentation
        // for more information on BDEX streaming of value-semantic types and
        // containers.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, const char& value, int version = 0);
        // Write the specified 'char' 'value' to the specified output 'stream',
        // and return a reference to 'stream'.  The optionally specified
        // 'version' is ignored.  If 'stream' is initially invalid, this
        // operation has no effect.  See the 'bslx' package-level documentation
        // for more information on BDEX streaming of value-semantic types and
        // containers.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM&            stream,
                          const signed char& value,
                          int                version = 0);
        // Write the specified 'signed char' 'value' to the specified output
        // 'stream', and return a reference to 'stream'.  The optionally
        // specified 'version' is ignored.  If 'stream' is initially invalid,
        // this operation has no effect.  See the 'bslx' package-level
        // documentation for more information on BDEX streaming of
        // value-semantic types and containers.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM&              stream,
                          const unsigned char& value,
                          int                  version = 0);
        // Write the specified 'unsigned char' 'value' to the specified output
        // 'stream', and return a reference to 'stream'.  The optionally
        // specified 'version' is ignored.  If 'stream' is initially invalid,
        // this operation has no effect.  See the 'bslx' package-level
        // documentation for more information on BDEX streaming of
        // value-semantic types and containers.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM&      stream,
                          const short& value,
                          int          version = 0);
        // Write the specified 'short' 'value' to the specified output
        // 'stream', and return a reference to 'stream'.  The optionally
        // specified 'version' is ignored.  If 'stream' is initially invalid,
        // this operation has no effect.  See the 'bslx' package-level
        // documentation for more information on BDEX streaming of
        // value-semantic types and containers.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM&               stream,
                          const unsigned short& value,
                          int                   version = 0);
        // Write the specified 'unsigned short' 'value' to the specified output
        // 'stream', and return a reference to 'stream'.  The optionally
        // specified 'version' is ignored.  If 'stream' is initially invalid,
        // this operation has no effect.  See the 'bslx' package-level
        // documentation for more information on BDEX streaming of
        // value-semantic types and containers.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, const int& value, int version = 0);
        // Write the specified 'int' 'value' to the specified output 'stream',
        // and return a reference to 'stream'.  The optionally specified
        // 'version' is ignored.  If 'stream' is initially invalid, this
        // operation has no effect.  See the 'bslx' package-level
        // documentation for more information on BDEX streaming of
        // value-semantic types and containers.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM&             stream,
                          const unsigned int& value,
                          int                 version = 0);
        // Write the specified 'unsigned int' 'value' to the specified output
        // 'stream', and return a reference to 'stream'.  The optionally
        // specified 'version' is ignored.  If 'stream' is initially invalid,
        // this operation has no effect.  See the 'bslx' package-level
        // documentation for more information on BDEX streaming of
        // value-semantic types and containers.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, const long& value, int version = 0);
        // Write the specified 'long' 'value' to the specified output 'stream'
        // as a 32-bit 'int', and return a reference to 'stream'.  The
        // optionally specified 'version' is ignored.  If 'stream' is initially
        // invalid, this operation has no effect.  Note that for platforms
        // where 'long' is not equivalent to 'int32_t', this operation may be
        // lossy.  See the 'bslx' package-level documentation for more
        // information on BDEX streaming of value-semantic types and
        // containers.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM&              stream,
                          const unsigned long& value,
                          int                  version = 0);
        // Write the specified 'unsigned long' 'value' to the specified output
        // 'stream' as a 32-bit 'unsigned int', and return a reference to
        // 'stream'.  The optionally specified 'version' is ignored.  If
        // 'stream' is initially invalid, this operation has no effect.  Note
        // that for platforms where 'unsigned long' is not equivalent to
        // 'uint32_t', this operation may be lossy.  See the 'bslx'
        // package-level documentation for more information on BDEX streaming
        // of value-semantic types and containers.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM&                   stream,
                          const bsls::Types::Int64& value,
                          int                       version = 0);
        // Write the specified 'bsls::Types::Int64' 'value' to the specified
        // output 'stream', and return a reference to 'stream'.  The optionally
        // specified 'version' is ignored.  If 'stream' is initially invalid,
        // this operation has no effect.  See the 'bslx' package-level
        // documentation for more information on BDEX streaming of
        // value-semantic types and containers.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM&                    stream,
                          const bsls::Types::Uint64& value,
                          int                        version = 0);
        // Write the specified 'bsls::Types::Uint64' 'value' to the specified
        // output 'stream', and return a reference to 'stream'.  The optionally
        // specified 'version' is ignored.  If 'stream' is initially invalid,
        // this operation has no effect.  See the 'bslx' package-level
        // documentation for more information on BDEX streaming of
        // value-semantic types and containers.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, const float& value, int version = 0);
        // Write the specified 'float' 'value' to the specified output
        // 'stream', and return a reference to 'stream'.  The optionally
        // specified 'version' is ignored.  If 'stream' is initially invalid,
        // this operation has no effect.  See the 'bslx' package-level
        // documentation for more information on BDEX streaming of
        // value-semantic types and containers.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM&       stream,
                          const double& value,
                          int           version = 0);
        // Write the specified 'double' 'value' to the specified output
        // 'stream', and return a reference to 'stream'.  The optionally
        // specified 'version' is ignored.  If 'stream' is initially invalid,
        // this operation has no effect.  See the 'bslx' package-level
        // documentation for more information on BDEX streaming of
        // value-semantic types and containers.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM&            stream,
                          const bsl::string& value,
                          int                version = 0);
        // Write the specified 'bsl::string' 'value' to the specified output
        // 'stream', and return a reference to 'stream'.  The optionally
        // specified 'version' is ignored.  If 'stream' is initially invalid,
        // this operation has no effect.  See the 'bslx' package-level
        // documentation for more information on BDEX streaming of
        // value-semantic types and containers.

    template <class STREAM, class ALLOC>
    STREAM& bdexStreamOut(STREAM&                         stream,
                          const bsl::vector<char, ALLOC>& value,
                          int                             version);
        // Write the specified 'bsl::vector<char, ALLOC>' 'value' to the
        // specified output 'stream', and return a reference to 'stream'.  The
        // specified 'version' is ignored.  If 'stream' is initially invalid,
        // this operation has no effect.  See the 'bslx' package-level
        // documentation for more information on BDEX streaming of
        // value-semantic types and containers.

    template <class STREAM, class ALLOC>
    STREAM& bdexStreamOut(STREAM&                                stream,
                          const bsl::vector<signed char, ALLOC>& value,
                          int                                    version);
        // Write the specified 'bsl::vector<signed char, ALLOC>' 'value' to the
        // specified output 'stream', and return a reference to 'stream'.  The
        // specified 'version' is ignored.  If 'stream' is initially invalid,
        // this operation has no effect.  See the 'bslx' package-level
        // documentation for more information on BDEX streaming of
        // value-semantic types and containers.

    template <class STREAM, class ALLOC>
    STREAM& bdexStreamOut(STREAM&                                  stream,
                          const bsl::vector<unsigned char, ALLOC>& value,
                          int                                      version);
        // Write the specified 'bsl::vector<unsigned char, ALLOC>' 'value' to
        // the specified output 'stream', and return a reference to 'stream'.
        // The specified 'version' is ignored.  If 'stream' is initially
        // invalid, this operation has no effect.  See the 'bslx' package-level
        // documentation for more information on BDEX streaming of
        // value-semantic types and containers.

    template <class STREAM, class ALLOC>
    STREAM& bdexStreamOut(STREAM&                          stream,
                          const bsl::vector<short, ALLOC>& value,
                          int                              version);
        // Write the specified 'bsl::vector<short, ALLOC>' 'value' to the
        // specified output 'stream', and return a reference to 'stream'.  The
        // specified 'version' is ignored.  If 'stream' is initially invalid,
        // this operation has no effect.  See the 'bslx' package-level
        // documentation for more information on BDEX streaming of
        // value-semantic types and containers.

    template <class STREAM, class ALLOC>
    STREAM& bdexStreamOut(STREAM&                                   stream,
                          const bsl::vector<unsigned short, ALLOC>& value,
                          int                                       version);
        // Write the specified 'bsl::vector<unsigned short, ALLOC>' 'value' to
        // the specified output 'stream', and return a reference to 'stream'.
        // The specified 'version' is ignored.  If 'stream' is initially
        // invalid, this operation has no effect.  See the 'bslx' package-level
        // documentation for more information on BDEX streaming of
        // value-semantic types and containers.

    template <class STREAM, class ALLOC>
    STREAM& bdexStreamOut(STREAM&                        stream,
                          const bsl::vector<int, ALLOC>& value,
                          int                            version);
        // Write the specified 'bsl::vector<int, ALLOC>' 'value' to the
        // specified output 'stream', and return a reference to 'stream'.  The
        // specified 'version' is ignored.  If 'stream' is initially invalid,
        // this operation has no effect.  See the 'bslx' package-level
        // documentation for more information on BDEX streaming of
        // value-semantic types and containers.

    template <class STREAM, class ALLOC>
    STREAM& bdexStreamOut(STREAM&                                 stream,
                          const bsl::vector<unsigned int, ALLOC>& value,
                          int                                     version);
        // Write the specified 'bsl::vector<unsigned int, ALLOC>' 'value' to
        // the specified output 'stream', and return a reference to 'stream'.
        // The specified 'version' is ignored.  If 'stream' is initially
        // invalid, this operation has no effect.  See the 'bslx' package-level
        // documentation for more information on BDEX streaming of
        // value-semantic types and containers.

    template <class STREAM, class ALLOC>
    STREAM& bdexStreamOut(
                        STREAM&                                       stream,
                        const bsl::vector<bsls::Types::Int64, ALLOC>& value,
                        int                                           version);
        // Write the specified 'bsl::vector<bsls::Types::Int64, ALLOC>' 'value'
        // to the specified output 'stream', and return a reference to
        // 'stream'.  The specified 'version' is ignored.  If 'stream' is
        // initially invalid, this operation has no effect.  See the 'bslx'
        // package-level documentation for more information on BDEX streaming
        // of value-semantic types and containers.

    template <class STREAM, class ALLOC>
    STREAM& bdexStreamOut(
                       STREAM&                                        stream,
                       const bsl::vector<bsls::Types::Uint64, ALLOC>& value,
                       int                                            version);
        // Write the specified 'bsl::vector<bsls::Types::Uint64, ALLOC>'
        // 'value' to the specified output 'stream', and return a reference to
        // 'stream'.  The specified 'version' is ignored.  If 'stream' is
        // initially invalid, this operation has no effect.  See the 'bslx'
        // package-level documentation for more information on BDEX streaming
        // of value-semantic types and containers.

    template <class STREAM, class ALLOC>
    STREAM& bdexStreamOut(STREAM&                          stream,
                          const bsl::vector<float, ALLOC>& value,
                          int                              version);
        // Write the specified 'bsl::vector<float, ALLOC>' 'value' to the
        // specified output 'stream', and return a reference to 'stream'.  The
        // specified 'version' is ignored.  If 'stream' is initially invalid,
        // this operation has no effect.  See the 'bslx' package-level
        // documentation for more information on BDEX streaming of
        // value-semantic types and containers.

    template <class STREAM, class ALLOC>
    STREAM& bdexStreamOut(STREAM&                           stream,
                          const bsl::vector<double, ALLOC>& value,
                          int                               version);
        // Write the specified 'bsl::vector<double, ALLOC>' 'value' to the
        // specified output 'stream', and return a reference to 'stream'.  The
        // specified 'version' is ignored.  If 'stream' is initially invalid,
        // this operation has no effect.  See the 'bslx' package-level
        // documentation for more information on BDEX streaming of
        // value-semantic types and containers.

    template <class STREAM, class TYPE, class ALLOC>
    STREAM& bdexStreamOut(STREAM&                         stream,
                          const bsl::vector<TYPE, ALLOC>& value);
        // Write the specified 'bsl::vector<TYPE, ALLOC>' 'value' to the
        // specified output 'stream', and return a reference to 'stream'.  If
        // 'stream' is initially invalid, this operation has no effect.  If
        // needed, first write the computed version information to the 'stream'
        // and if this version is not supported by 'TYPE' and the vector is not
        // empty, 'stream' is invalidated.  Note that the version is only
        // needed when the (template parameter) 'TYPE' is a 'bsl::vector' or a
        // user-defined type.  See the 'bslx' package-level documentation for
        // more information on BDEX streaming of value-semantic types and
        // containers.

    template <class STREAM, class TYPE, class ALLOC>
    STREAM& bdexStreamOut(STREAM&                         stream,
                          const bsl::vector<TYPE, ALLOC>& value,
                          int                             version);
        // Write the specified 'bsl::vector<TYPE, ALLOC>' 'value', using the
        // specified 'version' format, to the specified output 'stream', and
        // return a reference to 'stream'.  If 'stream' is initially invalid,
        // this operation has no effect.  If 'version' is not supported by
        // 'TYPE' and the vector is not empty, 'stream' is invalidated, but
        // otherwise unmodified.  Note that the specified 'TYPE' might not
        // require a 'version' to be serialized and that 'version' is not
        // written to 'stream'.  See the 'bslx' package-level documentation for
        // more information on BDEX streaming of value-semantic types and
        // containers.

}  // close namespace OutStreamFunctions

// ============================================================================
//                          INLINE FUNCTION DEFINITIONS
// ============================================================================

                         // ----------------------------
                         // namespace OutStreamFunctions
                         // ----------------------------

template <class STREAM, class TYPE>
inline
STREAM& OutStreamFunctions::bdexStreamOutImp(STREAM&           stream,
                                             const TYPE&       value,
                                             int            /* version */,
                                             const IsEnumType&)
{
    // A compilation error indicating the next line of code implies the 'TYPE'
    // cannot be represented as a 32-bit 'int' and an overload for the
    // 'OutStreamFunctions::bdexStreamOut' function, in the enumeration's
    // namespace, should be provided.

    BSLMF_ASSERT(sizeof(TYPE) <= sizeof(bsl::int32_t));

    // Stream the 'enum' value as a 32-bit 'int'.
    return stream.putInt32(static_cast<int>(value));
}

template <class STREAM, class TYPE>
inline
STREAM& OutStreamFunctions::bdexStreamOutImp(STREAM&              stream,
                                             const TYPE&          value,
                                             int                  version,
                                             const IsNotEnumType&)
{
    // A compilation error indicating the next line of code implies the class
    // of 'TYPE' does not support the 'bdexStreamOut' method.

    return value.bdexStreamOut(stream, version);
}

template <class STREAM, class TYPE>
inline
STREAM& OutStreamFunctions::bdexStreamOut(STREAM& stream, const TYPE& value)
{
    using VersionFunctions::maxSupportedBdexVersion;

    const int version = maxSupportedBdexVersion(&value,
                                                stream.bdexVersionSelector());
    if (VersionFunctions::k_NO_VERSION != version) {
        stream.putVersion(version);
    }

    return bdexStreamOut(stream, value, version);
}

template <class STREAM, class TYPE>
inline
STREAM& OutStreamFunctions::bdexStreamOut(STREAM&     stream,
                                          const TYPE& value,
                                          int         version)
{
    typedef typename bslmf::If<bslmf::IsEnum<TYPE>::value,
                               IsEnumType,
                               IsNotEnumType>::Type dummyType;
    return bdexStreamOutImp(stream, value, version, dummyType());
}

template <class STREAM>
inline
STREAM& OutStreamFunctions::bdexStreamOut(STREAM&     stream,
                                          const bool& value,
                                          int      /* version */)
{
    return stream.putInt8(static_cast<char>(value));
}

template <class STREAM>
inline
STREAM& OutStreamFunctions::bdexStreamOut(STREAM&     stream,
                                          const char& value,
                                          int      /* version */)
{
    return stream.putInt8(value);
}

template <class STREAM>
inline
STREAM& OutStreamFunctions::bdexStreamOut(STREAM&            stream,
                                          const signed char& value,
                                          int             /* version */)
{
    return stream.putInt8(value);
}

template <class STREAM>
inline
STREAM& OutStreamFunctions::bdexStreamOut(STREAM&              stream,
                                          const unsigned char& value,
                                          int               /* version */)
{
    return stream.putUint8(value);
}

template <class STREAM>
inline
STREAM& OutStreamFunctions::bdexStreamOut(STREAM&      stream,
                                          const short& value,
                                          int       /* version */)
{
    return stream.putInt16(value);
}

template <class STREAM>
inline
STREAM& OutStreamFunctions::bdexStreamOut(STREAM&               stream,
                                          const unsigned short& value,
                                          int                /* version */)
{
    return stream.putUint16(value);
}

template <class STREAM>
inline
STREAM& OutStreamFunctions::bdexStreamOut(STREAM&    stream,
                                          const int& value,
                                          int     /* version */)
{
    return stream.putInt32(value);
}

template <class STREAM>
inline
STREAM& OutStreamFunctions::bdexStreamOut(STREAM&             stream,
                                          const unsigned int& value,
                                          int              /* version */)
{
    return stream.putUint32(value);
}

template <class STREAM>
inline
STREAM& OutStreamFunctions::bdexStreamOut(STREAM&     stream,
                                          const long& value,
                                          int      /* version */)
{
    return stream.putInt32(static_cast<int>(value));
}

template <class STREAM>
inline
STREAM& OutStreamFunctions::bdexStreamOut(STREAM&              stream,
                                          const unsigned long& value,
                                          int               /* version */)
{
    return stream.putUint32(static_cast<unsigned int>(value));
}

template <class STREAM>
inline
STREAM& OutStreamFunctions::bdexStreamOut(STREAM&                   stream,
                                          const bsls::Types::Int64& value,
                                          int                    /* version */)
{
    return stream.putInt64(value);
}

template <class STREAM>
inline
STREAM& OutStreamFunctions::bdexStreamOut(
                                         STREAM&                    stream,
                                         const bsls::Types::Uint64& value,
                                         int                     /* version */)
{
    return stream.putUint64(value);
}

template <class STREAM>
inline
STREAM& OutStreamFunctions::bdexStreamOut(STREAM&      stream,
                                          const float& value,
                                          int       /* version */)
{
    return stream.putFloat32(value);
}

template <class STREAM>
inline
STREAM& OutStreamFunctions::bdexStreamOut(STREAM&       stream,
                                          const double& value,
                                          int        /* version */)
{
    return stream.putFloat64(value);
}

template <class STREAM>
inline
STREAM& OutStreamFunctions::bdexStreamOut(STREAM&            stream,
                                          const bsl::string& value,
                                          int             /* version */)
{
    return stream.putString(value);
}

template <class STREAM, class ALLOC>
inline
STREAM& OutStreamFunctions::bdexStreamOut(
                                    STREAM&                         stream,
                                    const bsl::vector<char, ALLOC>& value,
                                    int                          /* version */)
{
    const int length = static_cast<int>(value.size());
    stream.putLength(length);

    return 0 < length ? stream.putArrayInt8(&value[0], length) : stream;
}

template <class STREAM, class ALLOC>
inline
STREAM& OutStreamFunctions::bdexStreamOut(
                             STREAM&                                stream,
                             const bsl::vector<signed char, ALLOC>& value,
                             int                                 /* version */)
{
    const int length = static_cast<int>(value.size());
    stream.putLength(length);

    return 0 < length ? stream.putArrayInt8(&value[0], length) : stream;
}

template <class STREAM, class ALLOC>
inline
STREAM& OutStreamFunctions::bdexStreamOut(
                           STREAM&                                  stream,
                           const bsl::vector<unsigned char, ALLOC>& value,
                           int                                   /* version */)
{
    const int length = static_cast<int>(value.size());
    stream.putLength(length);

    return 0 < length ? stream.putArrayUint8(&value[0], length) : stream;
}

template <class STREAM, class ALLOC>
inline
STREAM& OutStreamFunctions::bdexStreamOut(
                                   STREAM&                          stream,
                                   const bsl::vector<short, ALLOC>& value,
                                   int                           /* version */)
{
    const int length = static_cast<int>(value.size());
    stream.putLength(length);

    return 0 < length ? stream.putArrayInt16(&value[0], length) : stream;
}

template <class STREAM, class ALLOC>
inline
STREAM& OutStreamFunctions::bdexStreamOut(
                          STREAM&                                   stream,
                          const bsl::vector<unsigned short, ALLOC>& value,
                          int                                    /* version */)
{
    const int length = static_cast<int>(value.size());
    stream.putLength(length);

    return 0 < length ? stream.putArrayUint16(&value[0], length) : stream;
}

template <class STREAM, class ALLOC>
inline
STREAM& OutStreamFunctions::bdexStreamOut(
                                     STREAM&                        stream,
                                     const bsl::vector<int, ALLOC>& value,
                                     int                         /* version */)
{
    const int length = static_cast<int>(value.size());
    stream.putLength(length);

    return 0 < length ? stream.putArrayInt32(&value[0], length) : stream;
}

template <class STREAM, class ALLOC>
inline
STREAM& OutStreamFunctions::bdexStreamOut(
                            STREAM&                                 stream,
                            const bsl::vector<unsigned int, ALLOC>& value,
                            int                                  /* version */)
{
    const int length = static_cast<int>(value.size());
    stream.putLength(length);

    return 0 < length ? stream.putArrayUint32(&value[0], length) : stream;
}

template <class STREAM, class ALLOC>
inline
STREAM& OutStreamFunctions::bdexStreamOut(
                      STREAM&                                       stream,
                      const bsl::vector<bsls::Types::Int64, ALLOC>& value,
                      int                                        /* version */)
{
    const int length = static_cast<int>(value.size());
    stream.putLength(length);

    return 0 < length ? stream.putArrayInt64(&value[0], length) : stream;
}

template <class STREAM, class ALLOC>
inline
STREAM& OutStreamFunctions::bdexStreamOut(
                     STREAM&                                        stream,
                     const bsl::vector<bsls::Types::Uint64, ALLOC>& value,
                     int                                         /* version */)
{
    const int length = static_cast<int>(value.size());
    stream.putLength(length);

    return 0 < length ? stream.putArrayUint64(&value[0], length) : stream;
}

template <class STREAM, class ALLOC>
inline
STREAM& OutStreamFunctions::bdexStreamOut(
                                   STREAM&                          stream,
                                   const bsl::vector<float, ALLOC>& value,
                                   int                           /* version */)
{
    const int length = static_cast<int>(value.size());
    stream.putLength(length);

    return 0 < length ? stream.putArrayFloat32(&value[0], length) : stream;
}

template <class STREAM, class ALLOC>
inline
STREAM& OutStreamFunctions::bdexStreamOut(
                                  STREAM&                           stream,
                                  const bsl::vector<double, ALLOC>& value,
                                  int                            /* version */)
{
    const int length = static_cast<int>(value.size());
    stream.putLength(length);

    return 0 < length ? stream.putArrayFloat64(&value[0], length) : stream;
}

template <class STREAM, class TYPE, class ALLOC>
inline
STREAM& OutStreamFunctions::bdexStreamOut(
                                        STREAM&                         stream,
                                        const bsl::vector<TYPE, ALLOC>& value)
{
    using VersionFunctions::maxSupportedBdexVersion;

    const int version = maxSupportedBdexVersion(&value,
                                                stream.bdexVersionSelector());

    stream.putVersion(version);
    return bdexStreamOut(stream, value, version);
}

template <class STREAM, class TYPE, class ALLOC>
STREAM& OutStreamFunctions::bdexStreamOut(
                                       STREAM&                         stream,
                                       const bsl::vector<TYPE, ALLOC>& value,
                                       int                             version)
{
    typedef typename bsl::vector<TYPE, ALLOC>::const_iterator Iterator;

    const int length = static_cast<int>(value.size());
    stream.putLength(length);

    for (Iterator it = value.begin(); it != value.end(); ++it) {
        bdexStreamOut(stream, *it, version);
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
