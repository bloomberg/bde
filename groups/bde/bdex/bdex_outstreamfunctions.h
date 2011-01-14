// bdex_outstreamfunctions.h                                          -*-C++-*-
#ifndef INCLUDED_BDEX_OUTSTREAMFUNCTIONS
#define INCLUDED_BDEX_OUTSTREAMFUNCTIONS

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Enable uniform externalization of user and fundamental types.
//
//@CLASSES:
//   bdex_OutStreamFunctions: namespace for uniform 'bdex' type externalization
//
//@AUTHOR: Rohan Bhindwale (rbhindwa)
//
//@SEE_ALSO: bdex_instreamfunctions, bdex_versionfunctions
//
//@DESCRIPTION: This component provides a namespace, 'bdex_OutStreamFunctions',
// that enables uniform support for 'bdex' externalization across all 'bdex'
// compliant user-defined types, including template types and containers, as
// well as those fundamental types (and 'bsl::string' and 'bsl::vector') for
// which the 'bdex' protocol provides direct support.
//
// The namespace 'bdex_OutStreamFunctions' allows clients to externalize
// objects in a uniform, type independent manner.  It contains the 'streamOut'
// function that externalizes objects of all 'bdex' compliant types.  This
// function externalizes the specified 'object' in the specified 'version'.
// The 'streamOut' function is specialized for fundamental types, 'enum' types,
// 'bsl::string' and 'bsl::vector'.  Note that version numbers are not output
// while externalizing these types.
//
// Enum types are streamed out as 32 bit 'int's.  Users can override this
// default behavior by providing specializations of the 'streamOut' function
// for their 'enum' type.  The general form of this specialization will be:
//..
//  namespace bdex_OutStreamFunctions {
//
//    template <class STREAM>
//    STREAM& streamOut(STREAM& stream, const my_Enum& value, int version)
//    {
//        // Code to stream out objects of my_Enum type
//
//        return stream;
//    }
//..
// For value-semantic types following the 'bdex' protocol, 'streamOut' calls
// the 'bdexStreamOut' function for that type.
//
///Component Design, Anticipated Usage, and the 'bdex' Contract
///------------------------------------------------------------
// 'bdex_outstreamfunctions' is an integral part of the 'bdex' externalization
// contract.  The 'bdex' contract is at least in part "collaborative", which is
// to say that each developer of a given *kind* of component (e.g., a stream or
// a value-semantic container) must comply with the relevant portions of the
// contract to ensure that the "system as a whole" works for everybody.
// 'bdex_outstreamfunctions' plays several related but different roles in
// helping various developers to produce 'bdex'-compliant components.  In this
// section we briefly highlight how and why 'bdex_outstreamfunctions' is
// helpful (or required) for these different developers.  By discussing
// different aspects of usage, we hope also to convey the general design goals
// of this component, and, to a certain extent, the overall 'bdex' contract.
// See the 'bdex' package-level documentation for a full specification of the
// 'bdex' contract.
//
///Implementing 'bdex' Streaming in Value-Semantic Template Classes
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The author of a non-template value-semantic type has full knowledge of the
// details of the "value" of that type, and may choose to use the appropriate
// output stream 'put' methods directly when implementing the required
// 'bdexStreamOut' method for that type.  However, if one or more aspects of
// the value are of template parameterized type, then the author cannot in
// general know how to stream the value using the 'put' methods.  For example,
// if a type has as its value one 'int' data member:
//..
//    int d_value;
//..
// then the implementation of the 'bdexStreamOut' method can contain:
//..
//    stream.putInt32(d_value);
//..
// However, if the data member is of parameterized 'TYPE':
//..
//    TYPE d_value;
//..
// then the implementation of the 'bdexStreamOut' method must rely on the
// 'bdex_OutStreamFunctions' implementation to write the value:
//..
//    bdex_OutStreamFunctions::streamOut(stream, d_value, 1);
//..
// This call will resolve to the correct sequence of 'put' calls no matter
// whether 'TYPE' is a fundamental type, a 'bdex'-compliant 'enum', or a proper
// 'bdex'-compliant class.  In the latter two cases, the explicit specification
// of the version format (in this case, 1) guarantees the stable operation of
// this method whether or not 'TYPE' chooses to change its version format.
//
///Factored "Output" Methods
///- - - - - - - - - - - - -
// One obvious design choice for 'bdex_outstreamfunctions' is that it supports
// the *output* portion of the 'bdex' "contract" only.  This factoring is by no
// means central to the 'bdex' contract as a whole, but allows authors of
// special-purpose (i.e., non-value-semantic) components to implement a subset
// of the 'bdex' facilities without causing a compile-time error when their
// clients attempt to use that 'bdex' functionality through the template
// mechanism of this component.
//
///Usage
///-----
// In this example we illustrate the primary intended use of the templatized
// methods of this component, as well as a few trivial invocations just to show
// the syntax clearly.  To accomplish this, we exhibit three separate example
// "components": an out-stream, a value-semantic point object, and an 'enum'.
// In all cases, the component designs are very simple, with much of the
// implied functionality omitted, in order to focus attention on the key
// aspects of the functionality of *this* component.
//
// First, consider an 'enum' 'my_color' that enumerates a set of colors.
//..
//  enum Color {
//      RED            =  0,
//      GREEN          =  1,
//      BLUE           =  2
//  };
//..
// Next, we consider a very special-purpose point that has as a data member its
// color.  Such a point provides an excellent opportunity for factoring, but
// since we are interested in highlighting 'bdex' streaming of various types,
// we will present a simple and unfactored design here.  In a real-world
// problem, the 'my_point' component would be implemented differently.
//
// Note that the 'my_Point' class in this example represents its coordinates as
// 'short' integer values; this is done to make the 'bdex' stream output byte
// pattern somewhat easier for the reader of this example to recognize when the
// output buffer is printed.
//..
//  // my_point.h
//
//  class my_Point {
//      // This class provides a geometric point having integer coordinates and
//      // an enumerated color property.
//
//      short d_x;      // x coordinate
//      short d_y;      // y coordinate
//      Color d_color;  // enumerated color property
//
//    public:
//    // CLASS METHODS
//    static int maxSupportedBdexVersion();
//        // Return the most current 'bdex' streaming version number supported
//        // by this class.
//
//    // CREATORS
//    my_Point();
//        // Create a valid default point.
//
//    my_Point(int x, int y, Color color);
//        // Create a point having the specified 'x' and 'y' coordinates
//        // and the specified 'color'.
//
//    ~my_Point();
//        // Destroy this point.
//
//    // MANIPULATORS
//    // ...
//
//    // ACCESSORS
//    int x() const;
//        // Return the x coordinate of this point.
//
//    int y() const;
//        // Return the y coordinate of this point.
//
//    Color color() const;
//        // Return the enumerated color of this point.
//
//    template <class STREAM>
//    STREAM& bdexStreamOut(STREAM& stream, int version) const;
//        // Write this value to the specified output 'stream' using the
//        // specified 'version' format and return a reference to the
//        // modifiable 'stream'.  ...
//  };
//
//  // FREE OPERATORS
//  inline
//  int operator==(const my_Point& lhs, const my_Point& rhs);
//
//..
// Representative (inline) implementations of these methods are shown below.
//..
//  //                  INLINE FUNCTION DEFINITIONS
//
//  // CLASS METHODS
//  inline
//  int my_Point::maxSupportedBdexVersion()
//  {
//      return 1;
//  }
//
//  // CREATORS
//  inline
//  my_Point::my_Point(int x, int y, Color color)
//  : d_x(x)
//  , d_y(y)
//  , d_color(color)
//  {
//  }
//
//  inline
//  my_Point::~my_Point()
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
//  int my_Point::x() const
//  {
//      return d_x;
//  }
//
//  // ...
//
//  template <class STREAM>
//  inline
//  STREAM& my_Point::bdexStreamOut(STREAM& stream, int version) const
//  {
//      switch (version) {
//        case 1: {
//          stream.putInt16(d_x);           // write the x coordinate
//          stream.putInt16(d_y);           // write the y coordinate
//          stream.putInt8((char) d_color); // write the color enum as one byte
//        } break;
//      }
//      return stream;
//  }
//
//  // FREE OPERATORS
//  inline
//  int operator==(const my_Point& lhs, const my_Point& rhs)
//  {
//      return lhs.x()     == rhs.x()
//          && lhs.y()     == rhs.y()
//          && lhs.color() == rhs.color();
//  }
//..
// Finally, we will implement an extremely simple output stream that supports
// the 'bdex' documentation-only protocol.  For simplicity, we will use a
// fixed-size buffer (usually a bad idea in any event, and more so here since
// the implementation knows the buffer size, but makes no effort to prevent
// overwriting that buffer), and will only show a few methods needed for this
// example.  Among the key design features *ignored* in this implementation is
// the byte order, or "endianness" of the host platform.  See other 'bdex'
// stream components for examples of properly-designed streams.
//..
//  // my_outstream.h
//  // ...
//
//  class my_OutStream {
//      // This class implements a limited-size fixed-buffer output stream that
//      // conforms to the 'bdex' protocol for output streams.  This class is
//      // suitable for demonstration purposes only.
//
//      char d_buffer[1000]; // externalized values stored as contiguous bytes
//      int  d_length;       // length of 'd_buffer' (bytes)
//
//    public:
//      // CREATORS
//      my_OutStream();
//          // Create an empty output stream of limited, fixed capacity.  Note
//          // that this object is suitable for demonstration purposes only.
//
//      ~my_OutStream();
//         // Destroy this output byte stream.
//
//      // MANIPULATORS
//      my_OutStream& putVersion(int version);
//          // Format the specified non-negative 'version' to this output
//          // stream and return a reference to this modifiable stream.
//
//      my_OutStream& putInt32(int value);
//          // Format the least significant 32 bits of the specified 'value' to
//          // this output stream and return a reference to this modifiable
//          // stream.
//
//      my_OutStream& putInt16(int value);
//          // Format the least significant 16 bits of the specified 'value' to
//          // this output stream and return a reference to this modifiable
//          // stream.
//
//      my_OutStream& putInt8(int value);
//          // Format the least significant 8 bits of the specified 'value' to
//          // this output stream and return a reference to this modifiable
//          // stream.
//
//      my_OutStream& putFloat64(double value);
//          // Format the most significant 64 bits in the specified 'value' to
//          // this output stream and return a reference to this modifiable
//          // stream.  ...
//
//      void removeAll();
//          // Remove all content in this stream.
//
//      // ACCESSORS
//      const char *data() const;
//          // Return the address of the contiguous, non-modifiable internal
//          // memory buffer of this stream.
//
//      int length() const;
//          // Return the number of bytes in this stream.
//  };
//
//  // FREE OPERATORS
//  inline
//  bsl::ostream& operator<<(bsl::ostream&       stream,
//                           const my_OutStream& object);
//      // Write the specified 'object' to the specified output 'stream' in
//      // some reasonable (multi-line) format, and return a reference to
//      // 'stream'.
//..
// The relevant (inline) implementations are as follows.
//..
//  //                  INLINE FUNCTION DEFINITIONS
//
//  // CREATORS
//  inline
//  my_OutStream::my_OutStream()
//  : d_length(0)
//  {
//  }
//
//  inline
//  my_OutStream::~my_OutStream()
//  {
//  }
//
//  // MANIPULATORS
//  inline
//  my_OutStream& my_OutStream::putVersion(int value)
//  {
//      unsigned char temp = value;
//      memcpy(d_buffer + d_length, &temp, 1);
//      ++d_length;
//      return *this;
//   }
//
//  inline
//  my_OutStream& my_OutStream::putInt32(int value)
//  {
//      memcpy(d_buffer + d_length, (const char *)&value, 4);
//      d_length += 4;
//      return *this;
//  }
//
//  inline
//  my_OutStream& my_OutStream::putInt16(int value)
//  {
//      unsigned short temp = value;
//      memcpy(d_buffer + d_length, (const char *)&temp, 2);
//      d_length += 2;
//      return *this;
//  }
//
//  inline
//  my_OutStream& my_OutStream::putInt8(int value)
//  {
//      unsigned char temp = value;
//      memcpy(d_buffer + d_length, &temp, 1);
//      d_length += 1;  // Imp Note: using syntax parallel to other 'putInt'
//      return *this;
//  }
//
//  inline
//  my_OutStream& my_OutStream::putFloat64(double value)
//  {
//      const char *tempPtr = (const char *)&value;
//      memcpy(d_buffer + d_length, &tempPtr, 8);
//      d_length += 8;
//      return *this;
//  }
//
//  inline
//  void my_OutStream::removeAll()
//  {
//      d_length = 0;
//  }
//
//  // ACCESSORS
//  inline
//  const char *my_OutStream::data() const
//  {
//      return (const char *) d_buffer;
//  }
//
//  inline
//  int my_OutStream::length() const
//  {
//      return d_length;
//  }
//..
// We can now write a small 'main' test program that will use the above 'enum',
// point class, and output stream to illustrate 'bdex_OutStreamFunctions'
// functionality.  This test program does not attempt to do anything more
// useful than writing known values to a stream and confirming that the
// expected byte pattern was in fact written.
//..
// // my_testapp.m.cpp
//
// using namespace std;
//
// int main(int argc, char **argv)
// {
//     int             i     = 168496141;       // byte pattern 0a 0b 0c 0d
//     Color           color = BLUE;            // byte pattern 02
//     my_Point        p(0, -1, color);         // byte pattern 00 00 ff ff 02
//
//     my_OutStream    out;                assert(0 == out.length());
//
//     bdex_OutStreamFunctions::streamOut(out, i, 1);
//                                         assert(4 == out.length());
//     out.removeAll();                    assert(0 == out.length());
//     bdex_OutStreamFunctions::streamOut(out, i, 0);
//                                         assert(4 == out.length());
//
//     out.removeAll();                    assert(0 == out.length());
//     bdex_OutStreamFunctions::streamOut(out, color, 1);
//                                         assert(4 == out.length());
//     out.removeAll();                    assert(0 == out.length());
//     bdex_OutStreamFunctions::streamOut(out, color, 0);
//                                         assert(4 == out.length());
//
//     out.removeAll();                    assert(0 == out.length());
//     bdex_OutStreamFunctions::streamOut(out, p, 1);
//                                         assert(5 == out.length());
//     out.removeAll();                    assert(0 == out.length());
//     bdex_OutStreamFunctions::streamOut(out, p, 1);
//                                         assert(5 == out.length());
// }
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEX_VERSIONFUNCTIONS
#include <bdex_versionfunctions.h>
#endif

#ifndef INCLUDED_BSLMF_ISENUM
#include <bslmf_isenum.h>
#endif

#ifndef INCLUDED_BSLMF_IF
#include <bslmf_if.h>
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

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef INCLUDED_BSL_CLIMITS
#include <bsl_climits.h>
#endif


namespace BloombergLP {

struct bdex_OutStreamFunctions_IsEnumType {
    // This 'struct' is a dummy struct used to perform function overload
    // resolution for types that are enum types.

    // This struct contains no interface or implementation by design.
};

struct bdex_OutStreamFunctions_IsNotEnumType {
    // This 'struct' is a dummy struct used to perform function overload
    // resolution for types that are *not* enum types.

    // This struct contains no interface or implementation by design.
};

                       // =================================
                       // namespace bdex_OutStreamFunctions
                       // =================================

namespace bdex_OutStreamFunctions {
    // This namespace allows externalization of various types in a type
    // independent manner all 'bdex'-compliant types.  The externalization
    // functions are specialized for fundamental types, enum types,
    // 'bsl::string' and 'bsl::vector'.  A compilation error will occur if
    // 'TYPE' does not support these two methods with appropriate signatures.
    // Additionally, the functions are specialized for fundamental types, enum
    // types, 'bsl::string' and 'bsl::vector'.

    // CLASS METHODS
    template <typename STREAM, typename TYPE>
    inline
    STREAM& streamOut_Imp(STREAM&                                   stream,
                          const TYPE&                               value,
                          int,
                          const bdex_OutStreamFunctions_IsEnumType&)
        // Write the specified 'value' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.  Note that this
        // function is called only for enum types and that a version is not
        // written to 'stream'.
    {
        // Assert that the enum value is between INT_MIN and INT_MAX.
        BSLS_ASSERT_SAFE(
           bsls_PlatformUtil::Int64(value) >= bsls_PlatformUtil::Int64(INT_MIN)
           &&
           bsls_PlatformUtil::Int64(value) <= bsls_PlatformUtil::Int64(INT_MAX)
        );

        // stream the enum value as a 32-bit 'int'
        int intValue = (int)value;
        return stream.putInt32(intValue);
    }

    template <typename STREAM, typename TYPE>
    inline
    STREAM& streamOut_Imp(STREAM&                                      stream,
                          const TYPE&                                  value,
                          int                                          version,
                          const bdex_OutStreamFunctions_IsNotEnumType&)
        // Write the specified 'value' to the specified output 'stream' using
        // the specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  The behavior is undefined unless 'TYPE' supports a
        // 'bdex'-compliant method having the same name and 'STREAM' is a
        // 'bdex'-compliant output stream.  Note that 'version' is not written
        // to 'stream'.
    {
        // A compilation error indicating the next line of code implies the
        // class of 'TYPE' does not support the 'bdexStreamOut' method.

        return value.bdexStreamOut(stream, version);
    }

    template <typename STREAM, typename TYPE>
    inline
    STREAM& streamOut(STREAM& stream, const TYPE& value, int version)
        // Write the specified 'value' to the specified output 'stream' using
        // the specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written.
    {
        // Select the function to call depending on whether the TYPE is an enum
        // type or not.  Note the use of the dummy structs
        // bdex_OutStreamFunctions_IsEnumType and
        // bdex_OutStreamFunctions_IsNotEnumType to choose the correct
        // function.

        typedef typename bslmf_If<bslmf_IsEnum<TYPE>::VALUE,
                                  bdex_OutStreamFunctions_IsEnumType,
                                  bdex_OutStreamFunctions_IsNotEnumType>::Type
                                                                     dummyType;

        return streamOut_Imp(stream, value, version, dummyType());
    }

                       // ------------------------------
                       // template class specializations
                       // ------------------------------

    template <typename STREAM>
    inline
    STREAM& streamOut(STREAM& stream, const bool& value, int)
        // Write the specified 'value' to the specified output 'stream' using
        // the appropriate 'put' method of 'stream'.  No version is used.  The
        // behavior is undefined unless 'STREAM' is a 'bdex'-compliant output
        // stream.
    {
        return stream.putInt8(static_cast<char>(value));
    }

    // This specialization implements 'streamOut' for 'char'.

    template <typename STREAM>
    inline
    STREAM& streamOut(STREAM& stream, const char& value, int)
        // Write the specified 'value' to the specified output 'stream' using
        // the appropriate 'put' method of 'stream'.  No version is used.  The
        // behavior is undefined unless 'STREAM' is a 'bdex'-compliant output
        // stream.
    {
        return stream.putInt8(value);
    }

    // This specialization implements 'streamOut' for 'signed' 'char'.

    template <typename STREAM>
    inline
    STREAM& streamOut(STREAM& stream, const signed char& value, int)
        // Write the specified 'value' to the specified output 'stream' using
        // the appropriate 'put' method of 'stream'.  No version is used.  The
        // behavior is undefined unless 'STREAM' is a 'bdex'-compliant output
        // stream.
    {
        return stream.putInt8(value);
    }

    // This specialization implements 'streamOut' for 'unsigned' 'char'.

    template <typename STREAM>
    inline
    STREAM& streamOut(STREAM& stream, const unsigned char& value, int)
        // Write the specified 'value' to the specified output 'stream' using
        // the appropriate 'put' method of 'stream'.  No version is used.  The
        // behavior is undefined unless 'STREAM' is a 'bdex'-compliant output
        // stream.
    {
        return stream.putUint8(value);
    }

    // This specialization implements 'streamOut' for 'short'.

    template <typename STREAM>
    inline
    STREAM& streamOut(STREAM& stream, const short& value, int)
        // Write the specified 'value' to the specified output 'stream' using
        // the appropriate 'put' method of 'stream'.  No version is used.  The
        // behavior is undefined unless 'STREAM' is a 'bdex'-compliant output
        // stream.
    {
        return stream.putInt16(value);
    }

    // This specialization implements 'streamOut' for 'unsigned' 'short'.

    template <typename STREAM>
    inline
    STREAM& streamOut(STREAM& stream, const unsigned short& value, int)
        // Write the specified 'value' to the specified output 'stream' using
        // the appropriate 'put' method of 'stream'.  No version is used.  The
        // behavior is undefined unless 'STREAM' is a 'bdex'-compliant output
        // stream.
    {
        return stream.putUint16(value);
    }

    // This specialization implements 'streamOut' for 'int'.

    template <typename STREAM>
    inline
    STREAM& streamOut(STREAM& stream, const int& value, int)
        // Write the specified 'value' to the specified output 'stream' using
        // the appropriate 'put' method of 'stream'.  No version is used.  The
        // behavior is undefined unless 'STREAM' is a 'bdex'-compliant output
        // stream.
    {
        return stream.putInt32(value);
    }

    // This specialization implements 'streamOut' for 'unsigned' 'int'.

    template <typename STREAM>
    inline
    STREAM& streamOut(STREAM& stream, const unsigned int& value, int)
        // Write the specified 'value' to the specified output 'stream' using
        // the appropriate 'put' method of 'stream'.  No version is used.  The
        // behavior is undefined unless 'STREAM' is a 'bdex'-compliant output
        // stream.
    {
        return stream.putUint32(value);
    }

    // This specialization implements 'streamOut' for 'long'.

    template <typename STREAM>
    inline
    STREAM& streamOut(STREAM& stream, const long& value, int)
        // Write the specified 'value' to the specified output 'stream' using
        // the appropriate 'put' method of 'stream'.  No version is used.  The
        // behavior is undefined unless 'STREAM' is a 'bdex'-compliant output
        // stream.  Note that 'value' is truncated to 4 bytes (if necessary)
        // before being written.
    {
        return stream.putInt32(static_cast<int>(value));
    }

    // This specialization implements 'streamOut' for 'unsigned long'.

    template <typename STREAM>
    inline
    STREAM& streamOut(STREAM& stream, const unsigned long& value, int)
        // Write the specified 'value' to the specified output 'stream' using
        // the appropriate 'put' method of 'stream'.  No version is used.  The
        // behavior is undefined unless 'STREAM' is a 'bdex'-compliant output
        // stream.  Note that 'value' is truncated to 4 bytes (if necessary)
        // before being written.
    {
        return stream.putUint32(static_cast<unsigned int>(value));
    }

    // This specialization implements 'streamOut' for
    // 'bsls_PlatformUtil::Int64'.

    template <typename STREAM>
    inline
    STREAM& streamOut(STREAM&                         stream,
                      const bsls_PlatformUtil::Int64& value,
                      int)
        // Write the specified 'value' to the specified output 'stream' using
        // the appropriate 'put' method of 'stream'.  No version is used.  The
        // behavior is undefined unless 'STREAM' is a 'bdex'-compliant output
        // stream.
    {
        return stream.putInt64(value);
    }

    // This specialization implements 'streamOut' for
    // 'bsls_PlatformUtil::Uint64'.

    template <typename STREAM>
    inline
    STREAM& streamOut(STREAM&                          stream,
                      const bsls_PlatformUtil::Uint64& value,
                      int)
        // Write the specified 'value' to the specified output 'stream' using
        // the appropriate 'put' method of 'stream'.  No version is used.  The
        // behavior is undefined unless 'STREAM' is a 'bdex'-compliant output
        // stream.
    {
        return stream.putUint64(value);
    }

    // This specialization implements 'streamOut' and 'maxSupportedBdexVersion'
    // for 'bsl::string'.

    template <typename STREAM>
    inline
    STREAM& streamOut(STREAM& stream, const bsl::string& value, int)
        // Write the specified 'value' to the specified output 'stream' using
        // the appropriate 'put' method of 'stream'.  No version is used.  The
        // behavior is undefined unless 'STREAM' is a 'bdex'-compliant output
        // stream.
    {
        return stream.putString(value);
    }

    // This specialization implements 'streamOut' for 'float'.

    template <typename STREAM>
    inline
    STREAM& streamOut(STREAM& stream, const float& value, int)
        // Write the specified 'value' to the specified output 'stream' using
        // the appropriate 'put' method of 'stream'.  No version is used.  The
        // behavior is undefined unless 'STREAM' is a 'bdex'-compliant output
        // stream.
    {
        return stream.putFloat32(value);
    }

    // This specialization implements 'streamOut' for 'double'.

    template <typename STREAM>
    inline
    STREAM& streamOut(STREAM& stream, const double& value, int)
        // Write the specified 'value' to the specified output 'stream' using
        // the appropriate 'put' method of 'stream'.  No version is used.  The
        // behavior is undefined unless 'STREAM' is a 'bdex'-compliant output
        // stream.
    {
        return stream.putFloat64(value);
    }

    // This specialization implements 'streamOut' for 'bsl::vector<char,
    // ALLOC>'.

    template <typename STREAM, typename ALLOC>
    inline
    STREAM& streamOut(STREAM&                         stream,
                      const bsl::vector<char, ALLOC>& value,
                      int)
        // Write the specified 'value' to the specified output 'stream' using
        // the appropriate 'put' method of 'stream'.  No version is used.  The
        // behavior is undefined unless 'STREAM' is a 'bdex'-compliant output
        // stream.
    {
        const int length = (int)value.size();
        stream.putLength(length);

        return 0 < length ? stream.putArrayInt8(&value[0], length) : stream;
    }

    // This specialization implements 'streamOut' for 'bsl::vector<short,
    // ALLOC>'.

    template <typename STREAM, typename ALLOC>
    inline
    STREAM& streamOut(STREAM&                          stream,
                      const bsl::vector<short, ALLOC>& value,
                      int)
        // Write the specified 'value' to the specified output 'stream' using
        // the appropriate 'put' method of 'stream'.  No version is used.  The
        // behavior is undefined unless 'STREAM' is a 'bdex'-compliant output
        // stream.
    {
        const int length = (int)value.size();
        stream.putLength(length);

        return 0 < length ? stream.putArrayInt16(&value[0], length) : stream;
    }

    // This specialization implements 'streamOut' for 'bsl::vector<int,
    // ALLOC>'.

    template <typename STREAM, typename ALLOC>
    inline
    STREAM& streamOut(STREAM&                        stream,
                      const bsl::vector<int, ALLOC>& value,
                      int)
        // Write the specified 'value' to the specified output 'stream' using
        // the appropriate 'put' method of 'stream'.  No version is used.  The
        // behavior is undefined unless 'STREAM' is a 'bdex'-compliant output
        // stream.
    {
        const int length = (int)value.size();
        stream.putLength(length);

        return 0 < length ? stream.putArrayInt32(&value[0], length) : stream;
    }

    // This specialization implements 'streamOut' for
    // 'bsl::vector<bsls_PlatformUtil::Int64, ALLOC>'.

    template <typename STREAM, typename ALLOC>
    inline
    STREAM& streamOut(
                   STREAM&                                             stream,
                   const bsl::vector<bsls_PlatformUtil::Int64, ALLOC>& value,
                   int)
        // Write the specified 'value' to the specified output 'stream' using
        // the appropriate 'put' method of 'stream'.  No version is used.  The
        // behavior is undefined unless 'STREAM' is a 'bdex'-compliant output
        // stream.
    {
        const int length = (int)value.size();
        stream.putLength(length);

        return 0 < length ? stream.putArrayInt64(&value[0], length) : stream;
    }

    // This specialization implements 'streamOut' for 'bsl::vector<float,
    // ALLOC>'.

    template <typename STREAM, typename ALLOC>
    inline
    STREAM& streamOut(STREAM&                          stream,
                      const bsl::vector<float, ALLOC>& value,
                      int)
        // Write the specified 'value' to the specified output 'stream' using
        // the appropriate 'put' method of 'stream'.  No version is used.  The
        // behavior is undefined unless 'STREAM' is a 'bdex'-compliant output
        // stream.
    {
        const int length = (int)value.size();
        stream.putLength(length);

        return 0 < length ? stream.putArrayFloat32(&value[0], length) : stream;
    }

    // This specialization implements 'streamOut' for 'bsl::vector<double,
    // ALLOC>'.

    template <typename STREAM, typename ALLOC>
    inline
    STREAM& streamOut(STREAM&                           stream,
                      const bsl::vector<double, ALLOC>& value,
                      int)
        // Write the specified 'value' to the specified output 'stream' using
        // the appropriate 'put' method of 'stream'.  No version is used.  The
        // behavior is undefined unless 'STREAM' is a 'bdex'-compliant output
        // stream.
    {
        const int length = (int)value.size();
        stream.putLength(length);

        return 0 < length ? stream.putArrayFloat64(&value[0], length) : stream;
    }

    // This specialization implements 'streamOut' for 'bsl::vector<TYPE,
    // ALLOC>'.

    template <typename STREAM, typename TYPE, typename ALLOC>
    STREAM& streamOut(STREAM&                         stream,
                      const bsl::vector<TYPE, ALLOC>& value,
                      int                             version)
        // Write the specified 'value' to the specified output 'stream' using
        // the appropriate 'put' method of 'stream' that is passed the
        // specified 'version'.  The behavior is undefined unless 'STREAM' is a
        // 'bdex'-compliant output stream.
    {
        typedef typename bsl::vector<TYPE, ALLOC>::const_iterator Iterator;

        const int length = (int)value.size();
        stream.putLength(length);

        for (Iterator it = value.begin(); it != value.end(); ++it) {
            streamOut(stream, *it, version);
        }

        return stream;
    }

// TBD #if !defined(BSL_LEGACY) || 1 == BSL_LEGACY

    template <typename STREAM, typename TYPE>
    inline
    STREAM& streamOutVersionAndObject(STREAM& stream, const TYPE& value)
        // Write the specified 'value' to the specified output 'stream'
        // and return a reference to the modifiable 'stream'.  Note that the
        // version number is output only if the version number corresponding to
        // the specified 'TYPE' is positive.
    {
        int version = bdex_VersionFunctions::maxSupportedVersion(value);

        if (version > 0) {
            stream.putVersion(version);
        }

        return streamOut(stream, value, version);
    }

// TBD #endif

}  // close namespace bdex_OutStreamFunctions

                // =========================
                // I/O manipulator functions
                // =========================

template <typename STREAM>
inline
STREAM& bdexFlush(STREAM& stream)
{
    stream.flush();
    return stream;
}

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
