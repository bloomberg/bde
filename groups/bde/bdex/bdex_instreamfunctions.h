// bdex_instreamfunctions.h                                           -*-C++-*-
#ifndef INCLUDED_BDEX_INSTREAMFUNCTIONS
#define INCLUDED_BDEX_INSTREAMFUNCTIONS

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Enable uniform (un)externalization of user and fundamental types.
//
//@CLASSES:
//  bdex_InStreamFunctions: namespace for uniform 'bdex' type unexternalization
//
//@AUTHOR: Rohan Bhindwale (rbhindwa)
//
//@SEE_ALSO: bdex_outstreamfunctions, bdex_versionfunctions
//
//@DESCRIPTION: This component provides a namespace, 'bdex_InStreamFunctions',
// that enables uniform support for 'bdex' unexternalization across all 'bdex'-
// compliant user-defined types, including template types and containers, as
// well as those fundamental types (and 'bsl::string' and 'bsl::vector') for
// which the 'bdex' protocol provides direct support.
//
// The namespace 'bdex_InStreamFunctions' allows clients to unexternalize
// objects in a uniform, type independent manner.  It contains the 'streamIn'
// function that unexternalizes objects of all 'bdex'-compliant types.  This
// function unexternalizes the specified 'object' in the 'version' read from
// the input stream.  The 'streamOut' function is specialized for fundamental
// types, 'enum' types, 'bsl::string' and 'bsl::vector' does not input the
// version while unexternalizing these types.
//
// Enum types are streamed in as 32 bit 'int's.  Users can override this
// default behavior by providing specializations of the 'streamIn' function for
// their 'enum' type.  The general form of this specialization will be:
//..
//  namespace bdex_InStreamFunctions {
//
//    template <class STREAM>
//    STREAM& streamIn(STREAM& stream, my_Enum& value, int version)
//    {
//        // Code to stream in objects of my_Enum type
//
//        return stream;
//    }
//  }
//..
// For value-semantic types following the 'bdex' protocol, 'streamIn' calls the
// 'bdexStreamIn' function for that type.
//
///Component Design, Anticipated Usage, and the 'bdex' Contract
///------------------------------------------------------------
// 'bdex_instreamfunctions' is an integral part of the 'bdex' unexternalization
// contract.  The 'bdex' contract is at least in part "collaborative", which is
// to say that each developer of a given *kind* of component (e.g., a stream or
// a value-semantic container) must comply with the relevant portions of the
// contract to ensure that the "system as a whole" works for everybody.
// 'bdex_instreamfunctions' plays several related but different roles in
// helping various developers to produce 'bdex'-compliant components.  In this
// section we briefly highlight how and why 'bdex_instreamfunctions' is helpful
// (or required) for these different developers.  By discussing different
// aspects of usage, we hope also to convey the general design goals of this
// component, and, to a certain extent, the overall 'bdex' contract.  See the
// 'bdex' package-level documentation for a full specification of the 'bdex'
// contract.
//
///Implementing 'bdex' Streaming in Value-Semantic Template Classes
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The author of a non-template value-semantic type has full knowledge of the
// details of the "value" of that type, and may choose to use the appropriate
// input stream 'put' methods directly when implementing the required
// 'bdexStreamIn' method for that type.  However, if one or more aspects of the
// value are of template parameterized type, then the author cannot in general
// know how to stream the value using the 'put' methods.  For example, if a
// type has as its value one 'int' data member:
//..
//    int d_value;
//..
// then the implementation of the 'bdexStreamIn' method can contain:
//..
//    stream.putInt32(d_value);
//..
// However, if the data member is of parameterized 'TYPE':
//..
//    TYPE d_value;
//..
// then the implementation of the 'bdexStreamIn' method must rely on the
// 'bdex_InStreamFunctions' implementation to write the value:
//..
//    bdex_InStreamFunctions::streamIn(stream, d_value, 1);
//..
// This call will resolve to the correct sequence of 'put' calls no matter
// whether 'TYPE' is a fundamental type, a 'bdex'-compliant 'enum', or a proper
// 'bdex'-compliant class.  In the latter two cases, the explicit specification
// of the version format (in this case, 1) guarantees the stable operation of
// this method whether or not 'TYPE' chooses to change its version format.
//
///Factored "Input" Methods
///- - - - - - - - - - - - -
// One obvious design choice for 'bdex_instreamfunctions' is that it supports
// the *input* portion of the 'bdex' "contract" only.  This factoring is by no
// means central to the 'bdex' contract as a whole, but allows authors of
// special-purpose (i.e., non-value-semantic) components to implement a subset
// of the 'bdex' facilities within causing a compile-time error when their
// clients attempt to use that 'bdex' functionality through the template
// mechanism of this component.
//
///Usage
///-----
// In this example we illustrate the primary intended use of the templatized
// methods of this component, as well as a few trivial invocations just to show
// the syntax clearly.  To accomplish this, we exhibit three separate example
// "components": an in-stream, a value-semantic point object, and an 'enum'.
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
// 'short' integer values; this is done to make the 'bdex' stream input byte
// pattern somewhat easier for the reader of this example to recognize when the
// input buffer is printed.
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
//    STREAM& bdexStreamIn(STREAM& stream, int version);
//        // Write this value to the specified input 'stream' using the
//        // specified 'version' format and return a reference to the
//        // modifiable 'stream'.  ...
//  };
//
//  // FREE OPERATORS
//  inline
//  int operator==(const my_Point& lhs, const my_Point& rhs);
//      // Return 1 if the specified 'lhs' and 'rhs' points have the same value
//      // and 0 otherwise.  Two points have the same value if they have the
//      // same x and y coordinates and the same color.
//
//..
// Representative (inline) implementations of these methods are shown below.
//..
//  //                  INLINE FUNCTION DEFINITIONS
//  //
//  // CLASS METHODS
//  inline
//  int my_Point::maxSupportedBdexVersion()
//  {
//      return 1;
//  }
//
//  // CREATORS
//  inline
//  my_Point::my_Point()
//  {
//  }
//
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
//  Color my_Point::color() const
//  {
//      return d_color;
//  }
//  inline
//  int my_Point::x() const
//  {
//      return d_x;
//  }
//  inline
//  int my_Point::y() const
//  {
//      return d_y;
//  }
//  // ...
//
//  template <class STREAM>
//  inline
//  STREAM& my_Point::bdexStreamIn(STREAM& stream, int version)
//  {
//      switch (version) {
//        case 1: {
//          stream.getInt16(d_x);           // write the x coordinate
//          stream.getInt16(d_y);           // write the y coordinate
//          char color;
//          stream.getInt8(color);          // write the color enum as one byte
//          d_color = (Color) color;
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
// Finally, we will implement an extremely simple input stream that supports
// the 'bdex' documentation-only protocol.  For simplicity we will use an
// externally managed buffer, and will only show a few methods needed for this
// example.
//..
//  // my_instream.h
//  // ...
//
//  class my_InStream {
//      // This class implements a limited-size fixed-buffer input stream that
//      // conforms to the 'bdex' protocol for input streams.  This class is
//      // suitable for demonstration purposes only.
//
//      const char *d_buffer;   // input buffer, held but not owned
//      int         d_length;   // length of 'd_buffer' (bytes)
//      int         d_cursor;   // cursor (index into 'd_buffer'
//
//    public:
//      // CREATORS
//      my_InStream(const char *buffer, int length);
//          // Create an input stream using the specified 'buffer' of the
//          // specified 'length'.
//
//      ~my_InStream();
//         // Destroy this input byte stream.
//
//      // MANIPULATORS
//      my_InStream& getVersion(int& version);
//          // Consume a version value from this input stream, place that
//          // value in the specified 'version', and return a reference to
//          // this modifiable stream.  ...
//
//      my_InStream& getInt32(int& value);
//          // Consume a 32-bit signed integer value from this input stream,
//          // place that value in the specified 'variable', and return a
//          // reference to this modifiable stream.  ...
//
//      my_InStream& getInt16(short& value);
//          // Consume a 16-bit signed integer value from this input stream,
//          // place that value in the specified 'variable', and return a
//          // reference to this modifiable stream.  ...
//
//      my_InStream& getInt8(char& value);
//          // Consume an 8-bit signed integer value from this input stream,
//          // place that value in the specified 'variable', and return a
//          // reference to this modifiable stream.  ...
//
//      void invalidate();
//          // Put this input stream in an invalid state.  ...
//
//      // ACCESSORS
//      operator const void *() const;
//          // Return a non-zero value if this stream is valid, and 0
//          // otherwise.  An invalid stream is a stream in which insufficient
//          // or invalid data was detected during an extraction operation.
//          // Note that an empty stream will be valid unless an extraction
//          // attempt or explicit invalidation causes it to be otherwise.
//
//      int cursor() const;
//          // Return the index of the next byte to be extracted from this
//          // stream.  The behavior is undefined unless this stream is valid.
//
//      bool isEmpty() const;
//          // Return 'true' if this stream is empty, and 'false' otherwise.
//          // The behavior is undefined unless this stream is valid (i.e.,
//          // would convert to a non-zero value via the
//          // 'operator const void *()' member).  Note that this function
//          // enables higher-level components to verify that, after
//          // successfully reading all expected data, no data remains.
//
//      int length() const;
//          // Return the total number of bytes stored in this stream.
//  };
//..
// The relevant (inline) implementations are as follows.
//..
//  // CREATORS
//  inline
//  my_InStream::my_InStream(const char *buffer, int length)
//  : d_buffer(buffer)
//  , d_length(length)
//  , d_cursor(0)
//  {
//  }
//
//  inline
//  my_InStream::~my_InStream()
//  {
//  }
//
//  // MANIPULATORS
//
//  inline
//  my_InStream& my_InStream::getVersion(int& value)
//  {
//      value = (unsigned char) d_buffer[d_cursor++];
//      return *this;
//  }
//
//  inline
//  my_InStream& my_InStream::getInt32(int& value)
//  {
//      const unsigned char *buffer = (const unsigned char *) d_buffer;
//      value = static_cast<int>((buffer[d_cursor    ] << 24U) +
//                               (buffer[d_cursor + 1] << 16U) +
//                               (buffer[d_cursor + 2] <<  8U) +
//                               (buffer[d_cursor + 3]       ));
//      d_cursor += 4;
//      return *this;
//  }
//
//  inline
//  my_InStream& my_InStream::getInt16(short& value)
//  {
//      const unsigned char *buffer = (const unsigned char *) d_buffer;
//      value = static_cast<short>((buffer[d_cursor + 0] <<  8) +
//                                 (buffer[d_cursor + 1]      ));
//      d_cursor += 2;
//      return *this;
//  }
//
//  inline
//  my_InStream& my_InStream::getInt8(char& value)
//  {
//      value = d_buffer[d_cursor];
//      d_cursor += 1;  // Imp Note: using syntax parallel to other 'getInt'
//      return *this;
//  }
//
//  inline
//  void my_InStream::invalidate()
//  {
//      d_buffer = 0;
//  }
//
//  // ACCESSORS
//  inline
//  my_InStream::operator const void *() const
//  {
//      return d_cursor <= d_length ? d_buffer : 0;
//  }
//
//  inline
//  int my_InStream::cursor() const
//  {
//      return d_cursor;
//  }
//
//  inline
//  bool my_InStream::isEmpty() const
//  {
//      return d_cursor >= d_length;
//  }
//
//  inline
//  int my_InStream::length() const
//  {
//      return d_length;
//  }
//..
// We can now write a small 'main' test program that will use the above 'enum',
// point class, and input stream to illustrate 'bdex_InStreamFunctions'
// functionality.  This test program does not attempt to do anything more
// useful than reading values from a stream whose buffer was written "by hand"
// and confirming that the expected values were read correctly from the known
// byte pattern in the buffer.
//
// For each of a sequence of types ('int', 'Color', and 'my_Point'), we will
// define a reference value 'EXP', and one or more input buffers (without and
// possibly with an initial "version" byte) containing the byte pattern for
// 'EXP' (as interpreted by 'my_InStream').  We will then declare one or more
// variables of that type initialized to a value distinct from 'EXP', create
// two 'my_InStream' objects from the appropriate buffers, and stream in a
// value from each stream using either the 'bdex_InStreamFunctions' method
// explicitly (which does not consume a version byte).  We verify success with
// 'assert' statements.  The astute reader may observe that this "application"
// is structured rather like a test driver.  As mentioned above, our intent is
// to focus on the use of *this* component, and not on the *use* of components
// that are clients of this component.  Typical application programmers will
// rarely, if ever, need to use this component directly.
//..
//  // my_testapp.m.cpp
//
//  int main(int argc, char *argv[])
//  {
//      {
//          const int  EXP       = 0x0A0B0C0D;
//          const char buffer[4] = { 0xA, 0xB, 0xC, 0xD };  // int (no version)
//          int        i         = 0;
//
//          my_InStream in1(buffer, 4);  // use the one buffer
//          bdex_InStreamFunctions::streamIn(in1, i, 1);
//                                           assert(in1);  assert(EXP == i);
//
//          i = 0;                       // reset 'i'
//          my_InStream in2(buffer, 4);  // re-use 'buffer (no version)
//          bdex_InStreamFunctions::streamIn(in2, i, 0);
//                                           assert(in2);  assert(EXP == i);
//      }
//
//      {
//          const my_Point EXP(0, -1, BLUE);
//          const char buffer1[5] = { 0, 0, -1, -1, 2 };    //my_Point (no ver)
//          const char buffer2[6] = { 1, 0, 0, -1, -1, 2 }; //version, my_Point
//          my_Point p1, p2;  // two default points
//
//          my_InStream in1(buffer1, 5);  // 'buffer1' has no version byte
//          bdex_InStreamFunctions::streamIn(in1, p1, 1);
//                                           assert(in1);  assert(EXP == p1);
//
//          my_InStream in2(buffer2, 6);  // 'buffer2' *has* a version byte
//          int version;
//          in2.getVersion(version);         assert(1 == version);
//          bdex_InStreamFunctions::streamIn(in2, p2, version);
//                                           assert(in2);  assert(EXP == p2);
//      }
//
//      return 0;
//  }
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEX_VERSIONFUNCTIONS
#include <bdex_versionfunctions.h>
#endif

#ifndef INCLUDED_BSLMF_IF
#include <bslmf_if.h>
#endif

#ifndef INCLUDED_BSLMF_ISENUM
#include <bslmf_isenum.h>
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


namespace BloombergLP {

struct bdex_InStreamFunctions_IsEnumType {
    // This 'struct' is a dummy struct used to perform function overload
    // resolution for types that are 'enum' types.

    // This struct contains no interface or implementation by design.
};

struct bdex_InStreamFunctions_IsNotEnumType {
    // This 'struct' is a dummy struct used to perform function overload
    // resolution for types that are *not* 'enum' types.

    // This struct contains no interface or implementation by design.
};

                       // ================================
                       // namespace bdex_InStreamFunctions
                       // ================================

namespace bdex_InStreamFunctions {
    // This template class allows uniform function calls of the 'bdexStreamIn'
    // method for all 'bdex'-compliant types.  A compilation error will occur
    // if 'TYPE' does not support this method with an appropriate signature.
    // Additionally, the functions are specialized for fundamental types, enum
    // types, 'bsl::string' and 'bsl::vector'.

    // CLASS METHODS
    template <typename STREAM, typename TYPE>
    inline
    STREAM& streamIn_Imp(STREAM&                                  stream,
                         TYPE&                                    object,
                         int                                      ,
                         const bdex_InStreamFunctions_IsEnumType&)
        // Assign to the specified 'object' the value read from the specified
        // input 'stream', and return a reference to the modifiable 'stream'.
        // If 'stream' is initially invalid, this operation has no effect.  If
        // 'stream' becomes invalid during this operation, 'object' is valid,
        // but its value is undefined.  Note that this function is called for
        // 'enum' types and that no version is read from 'stream'.
    {
        if (stream) {
            int enumValue = 0;
            stream.getInt32(enumValue);

            if (stream) {
                object = (TYPE) enumValue;
            }
        }
        return stream;
    }

    template <typename STREAM, typename TYPE>
    inline
    STREAM& streamIn_Imp(STREAM&                                     stream,
                         TYPE&                                       object,
                         int                                         version,
                         const bdex_InStreamFunctions_IsNotEnumType&)
        // Assign to the specified 'object' the value read from the specified
        // input 'stream' using the specified 'version' format and return a
        // reference to the modifiable 'stream'.  If 'stream' is initially
        // invalid, this operation has no effect.  If 'stream' becomes invalid
        // during this operation, 'object' is valid, but its value is
        // undefined.  If 'version' is not supported, 'stream' is marked
        // invalid and 'object' is unaltered.  The behavior is undefined unless
        // 'TYPE' supports a 'bdex'-compliant 'streamIn' method and 'STREAM' is
        // a 'bdex'-compliant input stream.  Note that no version is read from
        // 'stream'.
    {
        // A compilation error indicating the next line of code implies the
        // class of 'TYPE' does not support the 'bdexStreamIn' method.

        return object.bdexStreamIn(stream, version);
    }

    template <typename STREAM, typename TYPE>
    inline
    STREAM& streamIn(STREAM& stream, TYPE& object, int version)
        // Assign to the specified 'object' the value read from the specified
        // input 'stream' using the specified 'version' format and return a
        // reference to the modifiable 'stream'.  If 'stream' is initially
        // invalid, this operation has no effect.  If 'stream' becomes invalid
        // during this operation, 'object' is valid, but its value is
        // undefined.  If 'version' is not supported, 'stream' is marked
        // invalid and 'object' is unaltered.  Note that no version is read
        // from 'stream'.
    {
        // Select the function to call depending on whether the TYPE is an enum
        // type or not.  Note the use of the dummy structs
        // bdex_InStreamFunctions_IsEnumType and
        // bdex_InStreamFunctions_IsNotEnumType to choose the correct function.

        typedef typename bslmf_If<bslmf_IsEnum<TYPE>::VALUE,
                                  bdex_InStreamFunctions_IsEnumType,
                                  bdex_InStreamFunctions_IsNotEnumType>::Type
                                                                     dummyType;

        return streamIn_Imp(stream, object, version, dummyType());
    }

                       // ------------------------------
                       // template class specializations
                       // ------------------------------

    // This specialization implements 'streamIn' for 'bool'.

    template <typename STREAM>
    inline
    STREAM& streamIn(STREAM& stream, bool& variable, int)
        // Assign to the specified 'variable' the value read from the specified
        // input 'stream' using the appropriate 'get' method of 'stream', and
        // return a reference to the modifiable 'stream'.  If 'stream' is
        // initially invalid, this operation has no effect.  If 'stream'
        // becomes invalid during this operation, the value of 'variable' is
        // undefined.  Note that no version is read from 'stream'.
    {
        char temp = 0;

        stream.getInt8(temp);
        variable = static_cast<bool>(temp);

        return stream;
    }

    // This specialization implements 'streamIn' for 'char'.

    template <typename STREAM>
    inline
    STREAM& streamIn(STREAM& stream, char& variable, int)
        // Assign to the specified 'variable' the value read from the specified
        // input 'stream' using the appropriate 'get' method of 'stream', and
        // return a reference to the modifiable 'stream'.  If 'stream' is
        // initially invalid, this operation has no effect.  If 'stream'
        // becomes invalid during this operation, the value of 'variable' is
        // undefined.  Note that no version is read from 'stream'.
    {
        return stream.getInt8(variable);
    }

    // This specialization implements 'streamIn' for 'signed' 'char'.

    template <typename STREAM>
    inline
    STREAM& streamIn(STREAM& stream, signed char& variable, int)
        // Assign to the specified 'variable' the value read from the specified
        // input 'stream' using the appropriate 'get' method of 'stream', and
        // return a reference to the modifiable 'stream'.  If 'stream' is
        // initially invalid, this operation has no effect.  If 'stream'
        // becomes invalid during this operation, the value of 'variable' is
        // undefined.  Note that no version is read from 'stream'.
    {
        return stream.getInt8(variable);
    }

    // This specialization implements 'streamIn' for 'unsigned' 'char'.

    template <typename STREAM>
    inline
    STREAM& streamIn(STREAM& stream, unsigned char& variable, int)
        // Assign to the specified 'variable' the value read from the specified
        // input 'stream' using the appropriate 'get' method of 'stream', and
        // return a reference to the modifiable 'stream'.  If 'stream' is
        // initially invalid, this operation has no effect.  If 'stream'
        // becomes invalid during this operation, the value of 'variable' is
        // undefined.  Note that no version is read from 'stream'.
    {
        return stream.getUint8(variable);
    }

    // This specialization implements 'streamIn' for 'short'.

    template <typename STREAM>
    inline
    STREAM& streamIn(STREAM& stream, short& variable, int)
        // Assign to the specified 'variable' the value read from the specified
        // input 'stream' using the appropriate 'get' method of 'stream', and
        // return a reference to the modifiable 'stream'.  If 'stream' is
        // initially invalid, this operation has no effect.  If 'stream'
        // becomes invalid during this operation, the value of 'variable' is
        // undefined.  Note that no version is read from 'stream'.
    {
        return stream.getInt16(variable);
    }

    // This specialization implements 'streamIn' for 'unsigned' 'short'.

    template <typename STREAM>
    inline
    STREAM& streamIn(STREAM& stream, unsigned short& variable, int)
        // Assign to the specified 'variable' the value read from the specified
        // input 'stream' using the appropriate 'get' method of 'stream', and
        // return a reference to the modifiable 'stream'.  If 'stream' is
        // initially invalid, this operation has no effect.  If 'stream'
        // becomes invalid during this operation, the value of 'variable' is
        // undefined.  Note that no version is read from 'stream'.
    {
        return stream.getUint16(variable);
    }

    // This specialization implements 'streamIn' for 'int'.

    template <typename STREAM>
    inline
    STREAM& streamIn(STREAM& stream, int& variable, int)
        // Assign to the specified 'variable' the value read from the specified
        // input 'stream' using the appropriate 'get' method of 'stream', and
        // return a reference to the modifiable 'stream'.  If 'stream' is
        // initially invalid, this operation has no effect.  If 'stream'
        // becomes invalid during this operation, the value of 'variable' is
        // undefined.  Note that no version is read from 'stream'.
    {
        return stream.getInt32(variable);
    }

    // This specialization implements 'streamIn' for 'unsigned' 'int'.

    template <typename STREAM>
    inline
    STREAM& streamIn(STREAM& stream, unsigned int& variable, int)
        // Assign to the specified 'variable' the value read from the specified
        // input 'stream' using the appropriate 'get' method of 'stream', and
        // return a reference to the modifiable 'stream'.  If 'stream' is
        // initially invalid, this operation has no effect.  If 'stream'
        // becomes invalid during this operation, the value of 'variable' is
        // undefined.  Note that no version is read from 'stream'.
    {
        return stream.getUint32(variable);
    }

    // This specialization implements 'streamIn' for 'long'.

    template <typename STREAM>
    inline
    STREAM& streamIn(STREAM& stream, long& variable, int)
        // Assign to the specified 'variable' the value read from the specified
        // input 'stream' using the appropriate 'get' method of 'stream', and
        // return a reference to the modifiable 'stream'.  If 'stream' is
        // initially invalid, this operation has no effect.  If 'stream'
        // becomes invalid during this operation, the value of 'variable' is
        // undefined.  Note that no version is read from 'stream'.  Also note
        // that only 4 bytes are read from 'stream', even on platforms on which
        // 'sizeof(long)' is larger.
    {
        int temp = 0;  // 'long' and 'int' may not be the same size.
        stream.getInt32(temp);
        variable = temp;
        return stream;
    }

    // This specialization implements 'streamIn' for 'unsigned long'.

    template <typename STREAM>
    inline
    STREAM& streamIn(STREAM& stream, unsigned long& variable, int)
        // Assign to the specified 'variable' the value read from the specified
        // input 'stream' using the appropriate 'get' method of 'stream', and
        // return a reference to the modifiable 'stream'.  If 'stream' is
        // initially invalid, this operation has no effect.  If 'stream'
        // becomes invalid during this operation, the value of 'variable' is
        // undefined.  Note that no version is read from 'stream'.  Also note
        // that only 4 bytes are read from 'stream', even on platforms on which
        // 'sizeof(unsigned long)' is larger.
    {
        unsigned int temp = 0;  // 'unsigned long' and 'unsigned int' may not
                                // be the same size.
        stream.getUint32(temp);
        variable = temp;
        return stream;
    }

    // This specialization implements 'streamIn' for
    // 'bsls_PlatformUtil::Int64'.

    template <typename STREAM>
    inline
    STREAM& streamIn(STREAM&                   stream,
                     bsls_PlatformUtil::Int64& variable,
                     int)
        // Assign to the specified 'variable' the value read from the specified
        // input 'stream' using the appropriate 'get' method of 'stream', and
        // return a reference to the modifiable 'stream'.  If 'stream' is
        // initially invalid, this operation has no effect.  If 'stream'
        // becomes invalid during this operation, the value of 'variable' is
        // undefined.  Note that no version is read from 'stream'.
    {
        return stream.getInt64(variable);
    }

    // This specialization implements 'streamIn' for
    // 'bsls_PlatformUtil::Uint64'.

    template <typename STREAM>
    inline
    STREAM& streamIn(STREAM&                    stream,
                     bsls_PlatformUtil::Uint64& variable,
                     int)
        // Assign to the specified 'variable' the value read from the specified
        // input 'stream' using the appropriate 'get' method of 'stream', and
        // return a reference to the modifiable 'stream'.  If 'stream' is
        // initially invalid, this operation has no effect.  If 'stream'
        // becomes invalid during this operation, the value of 'variable' is
        // undefined.  Note that no version is read from 'stream'.
    {
        return stream.getUint64(variable);
    }

    // This specialization implements 'streamIn' for 'float'.

    template <typename STREAM>
    inline
    STREAM& streamIn(STREAM& stream, float& variable, int)
        // Assign to the specified 'variable' the value read from the specified
        // input 'stream' using the appropriate 'get' method of 'stream', and
        // return a reference to the modifiable 'stream'.  If 'stream' is
        // initially invalid, this operation has no effect.  If 'stream'
        // becomes invalid during this operation, the value of 'variable' is
        // undefined.  Note that no version is read from 'stream'.
    {
        return stream.getFloat32(variable);
    }

    // This specialization implements 'streamIn' for 'double'.

    template <typename STREAM>
    inline
    STREAM& streamIn(STREAM& stream, double& variable, int)
        // Assign to the specified 'variable' the value read from the specified
        // input 'stream' using the appropriate 'get' method of 'stream', and
        // return a reference to the modifiable 'stream'.  If 'stream' is
        // initially invalid, this operation has no effect.  If 'stream'
        // becomes invalid during this operation, the value of 'variable' is
        // undefined.  Note that no version is read from 'stream'.
    {
        return stream.getFloat64(variable);
    }

    // This specialization implements 'streamIn' for 'bsl::string'.

    template <typename STREAM>
    inline
    STREAM& streamIn(STREAM& stream, bsl::string& variable, int)
        // Assign to the specified 'variable' the value read from the specified
        // input 'stream' using the appropriate 'get' method of 'stream', and
        // return a reference to the modifiable 'stream'.  If 'stream' is
        // initially invalid, this operation has no effect.  If 'stream'
        // becomes invalid during this operation, the value of 'variable' is
        // undefined.  Note that no version is read from 'stream'.
    {
        return stream.getString(variable);
    }

    // This specialization implements 'streamIn' for 'bsl::vector<char,
    // ALLOC>'.

    template <typename STREAM, typename ALLOC>
    inline
    STREAM& streamIn(STREAM&                   stream,
                     bsl::vector<char, ALLOC>& variable,
                     int)
        // Assign to the specified 'variable' the value read from the specified
        // input 'stream' using the appropriate 'get' method of 'stream', and
        // return a reference to the modifiable 'stream'.  If 'stream' is
        // initially invalid, this operation has no effect.  If 'stream'
        // becomes invalid during this operation, the value of 'variable' is
        // undefined.  Note that no version is read from 'stream'.
    {
        int length = 0;
        stream.getLength(length);

        if (!stream) {
            return stream;
        }

        variable.resize(length);

        return (0 < length) ? stream.getArrayInt8(&variable[0], length)
                            : stream;
    }

    // This specialization implements 'streamIn' for 'bsl::vector<short,
    // ALLOC>'.

    template <typename STREAM, typename ALLOC>
    inline
    STREAM& streamIn(STREAM&                    stream,
                     bsl::vector<short, ALLOC>& variable,
                     int)
        // Assign to the specified 'variable' the value read from the specified
        // input 'stream' using the appropriate 'get' method of 'stream', and
        // return a reference to the modifiable 'stream'.  If 'stream' is
        // initially invalid, this operation has no effect.  If 'stream'
        // becomes invalid during this operation, the value of 'variable' is
        // undefined.  Note that no version is read from 'stream'.
    {
        int length = 0;
        stream.getLength(length);

        if (!stream) {
            return stream;
        }

        variable.resize(length);

        return (0 < length) ? stream.getArrayInt16(&variable[0], length)
                            : stream;
    }

    // This specialization implements 'streamIn' for 'bsl::vector<int, ALLOC>'.

    template <typename STREAM, typename ALLOC>
    inline
    STREAM& streamIn(STREAM&                  stream,
                     bsl::vector<int, ALLOC>& variable,
                     int)
        // Assign to the specified 'variable' the value read from the specified
        // input 'stream' using the appropriate 'get' method of 'stream', and
        // return a reference to the modifiable 'stream'.  If 'stream' is
        // initially invalid, this operation has no effect.  If 'stream'
        // becomes invalid during this operation, the value of 'variable' is
        // undefined.  Note that no version is read from 'stream'.
    {
        int length = 0;
        stream.getLength(length);

        if (!stream) {
            return stream;
        }

        variable.resize(length);

        return (0 < length) ? stream.getArrayInt32(&variable[0], length)
                            : stream;
    }

    // This specialization implements 'streamIn' for
    // 'bsl::vector<bsls_PlatformUtil::Int64, ALLOC>'.

    template <typename STREAM, typename ALLOC>
    inline
    STREAM& streamIn(STREAM&                                       stream,
                     bsl::vector<bsls_PlatformUtil::Int64, ALLOC>& variable,
                     int)
        // Assign to the specified 'variable' the value read from the specified
        // input 'stream' using the appropriate 'get' method of 'stream', and
        // return a reference to the modifiable 'stream'.  If 'stream' is
        // initially invalid, this operation has no effect.  If 'stream'
        // becomes invalid during this operation, the value of 'variable' is
        // undefined.  Note that no version is read from 'stream'.
    {
        int length = 0;
        stream.getLength(length);

        if (!stream) {
            return stream;
        }

        variable.resize(length);

        return (0 < length) ? stream.getArrayInt64(&variable[0], length)
                            : stream;
    }

    // This specialization implements 'streamIn' for 'bsl::vector<float,
    // ALLOC>'.

    template <typename STREAM, typename ALLOC>
    inline
    STREAM& streamIn(STREAM&                    stream,
                     bsl::vector<float, ALLOC>& variable,
                     int)
        // Assign to the specified 'variable' the value read from the specified
        // input 'stream' using the appropriate 'get' method of 'stream', and
        // return a reference to the modifiable 'stream'.  If 'stream' is
        // initially invalid, this operation has no effect.  If 'stream'
        // becomes invalid during this operation, the value of 'variable' is
        // undefined.  Note that no version is read from 'stream'.
    {
        int length = 0;
        stream.getLength(length);

        if (!stream) {
            return stream;
        }

        variable.resize(length);

        return (0 < length) ? stream.getArrayFloat32(&variable[0], length)
                            : stream;
    }

    // This specialization implements 'streamIn' for 'bsl::vector<double,
    // ALLOC>'.

    template <typename STREAM, typename ALLOC>
    inline
    STREAM& streamIn(STREAM&                     stream,
                     bsl::vector<double, ALLOC>& variable,
                     int)
        // Assign to the specified 'variable' the value read from the specified
        // input 'stream' using the appropriate 'get' method of 'stream', and
        // return a reference to the modifiable 'stream'.  If 'stream' is
        // initially invalid, this operation has no effect.  If 'stream'
        // becomes invalid during this operation, the value of 'variable' is
        // undefined.  Note that no version is read from 'stream'.
    {
        int length = 0;
        stream.getLength(length);

        if (!stream) {
            return stream;
        }

        variable.resize(length);

        return (0 < length) ? stream.getArrayFloat64(&variable[0], length)
                            : stream;
    }

    // This specialization implements 'streamIn' for 'bsl::vector<TYPE,
    // ALLOC>'.

    template <typename STREAM, typename TYPE, typename ALLOC>
    STREAM& streamIn(STREAM&                   stream,
                     bsl::vector<TYPE, ALLOC>& variable,
                     int                       version)
        // Assign to the specified 'variable' the value read from the specified
        // input 'stream' using the appropriate 'get' method of 'stream' that
        // is passed the specified 'version', and return a reference to the
        // modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, the value of 'variable' is undefined.  Note that no
        // version is read from 'stream'.
    {
        typedef typename bsl::vector<TYPE, ALLOC>::iterator Iterator;

        int length = 0;
        stream.getLength(length);

        if (!stream) {
            return stream;
        }

        variable.resize(length);

        for (Iterator it = variable.begin(); it != variable.end(); ++it) {

            streamIn(stream, *it, version);

            if (!stream) {
                return stream;
            }
        }

        return stream;
    }

// TBD #if !defined(BSL_LEGACY) || 1 == BSL_LEGACY

    template <typename STREAM, typename TYPE>
    inline
    STREAM& streamInVersionAndObject(STREAM& stream, TYPE& object)
        // Assign to the specified 'object' the value read from the specified
        // input 'stream' and return a reference to the modifiable 'stream'.
        // If 'stream' is initially invalid, this operation has no effect.  If
        // 'stream' becomes invalid during this operation, 'object' is valid,
        // but its value is undefined.  If 'version' is not supported, 'stream'
        // is marked invalid and 'object' is unaltered.  Note that version is
        // read from 'stream' only if 'TYPE' supports a valid version.
    {
        int hasVersion = bdex_VersionFunctions::maxSupportedVersion(object);
        int version = 0;

        if (hasVersion > 0) {
            if (stream) {
                stream.getVersion(version);

                if (!stream) {
                    return stream;
                }
            }
        }

        return streamIn(stream, object, version);
    }

// TBD #endif

}  // close namespace bdex_InStreamFunctions

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
