// bdex_outstreammethods.h                                            -*-C++-*-
#ifndef INCLUDED_BDEX_OUTSTREAMMETHODS
#define INCLUDED_BDEX_OUTSTREAMMETHODS

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Enable uniform externalization of user and fundamental types.
//
//@CLASSES:
//   bdex_OutStreamMethods: templates for uniform 'bdex' type externalization
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@SEE_ALSO: 'bdex_instreammethods'
//
//@DEPRECATED: This component should not be used for new code.  Use (and
// specialize the functions in 'bdex_outstreamfunctions', instead.
//
//@DESCRIPTION: This component provides a template class that enables uniform
// support for 'bdex' externalization across all 'bdex'-compliant user-defined
// types, including template types and containers, as well as those fundamental
// types (and 'bsl::string') for which the 'bdex' protocol provides direct
// support.
//
// The template class 'bdex_OutStreamMethods' allows clients to call the
// 'bdexStreamOut' instance method and the 'maxSupportedBdexVersion' class
// method for all objects of a 'bdex'-compliant 'TYPE' that support those
// methods.  This component also provides specializations of
// 'bdex_OutStreamMethods' for the 'bdex'-supported fundamental types and
// 'bsl::string', which allows authors of value-semantic template types to
// implement 'bdex' streaming uniformly without knowledge of the template type.
// Finally, the 'bdex' contract requires authors of 'bdex'-compliant entities
// that cannot satisfy the template class implementation (e.g., canonical BDE
// 'enum' types such as 'bdet_DayOfWeek::Day') to provide the appropriate
// specialization of 'bdex_OutStreamMethods' for that entity.
//
///Component Design, Anticipated Usage, and the 'bdex' Contract
///------------------------------------------------------------
// 'bdex_outstreammethods' is an integral part of the 'bdex' externalization
// contract.  The 'bdex' contract is at least in part "collaborative", which is
// to say that each developer of a given *kind* of component (e.g., a stream or
// a value-semantic container) must comply with the relevant portions of the
// contract to ensure that the "system as a whole" works for everybody.
// 'bdex_outstreammethods' plays several related but different roles in helping
// various developers to produce 'bdex'-compliant components.  In this section
// we briefly highlight how and why 'bdex_outstreammethods' is helpful (or
// required) for these different developers.  By discussing different aspects
// of usage, we hope also to convey the general design goals of this component,
// and, to a certain extent, the overall 'bdex' contract.  See the 'bdex'
// package-level documentation for a full specification of the 'bdex' contract.
//
///Required Usage for 'enum' Components
///- - - - - - - - - - - - - - - - - -
// In order to be 'bdex'-compliant, any component defining a named 'enum' type
// (e.g., 'bdet_DayOfWeek::Day') must also define a corresponding
// specialization of the form:
//..
//    template <>
//    class bdex_OutStreamMethods<bdet_DayOfWeek::Day>
//..
// The primary motivation for this requirement is that the template class
//..
//     template <class TYPE>
//     class bdex_OutStreamMethods
//..
// as defined in this component expects 'TYPE' to support the instance method
// 'bdexStreamOut', and therefore cannot accept a named 'enum' to instantiate a
// class from the template.  The author of the named-'enum' specialization must
// provide an appropriate implementation within the component defining the
// 'enum'.  Note that the requirement to provide a specialization applies as
// well to any entity for which the template class of this component cannot be
// instantiated.
//
///Implementing 'operator<<' in Output Streams and Formatters
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The 'bdex' contract requires each output stream to provide an overloaded
// 'operator<<' that will write any 'bdex'-compliant object to the stream.  The
// methods of the template class 'bdex_OutStreamMethods' provide an effective
// implementation for the required operator.  The standard 'bdex' production
// streams are implemented using components known as "formatters" (see, e.g.,
// 'bdex_ByteOutStreamFormatter'), but this factoring is not central to the
// contract.  Consider a simple 'my_outstream' component (see the Usage Example
// below for more details).  Such a stream could implement a templatized
// 'operator<<' as follows.
//..
//  template <typename OBJECT>
//  inline
//  my_OutStream& operator<<(my_OutStream& stream, const OBJECT& object)
//  {
//      const int version =
//                    bdex_OutStreamMethods<OBJECT>::maxSupportedBdexVersion();
//      stream.putVersion(version);
//      return bdex_OutStreamMethods<OBJECT>::bdexStreamOut(stream,
//                                                          object,
//                                                          version);
//  }
//..
// Note that most streams will choose to specialize the above template function
// for the 'bdex'-supported fundamental types in order to suppress the version
// from being written to the stream.  For example, the specialization for 'int'
// might be:
//..
//  template <>
//  inline
//  my_OutStream& operator<<(my_OutStream& stream, const int& value)
//  {
//      return stream.putInt32(value);
//  }
//..
// where 'putInt32' is the output stream method for a 32-bit integer value.
// Note that cooperating In- and OutStreams must coordinate these
// specializations as well.
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
// 'bdex_OutStreamMethods' implementation to write the value:
//..
//    bdex_OutStreamMethods<TYPE>::bdexStreamOut(stream, d_value, 1);
//..
// This call will resolve to the correct sequence of 'put' calls no matter
// whether 'TYPE' is a fundamental type, a 'bdex'-compliant 'enum', or a proper
// 'bdex'-compliant class.  In the latter two cases, the explicit specification
// of the version format (in this case, 1) guarantees the stable operation of
// this method whether or not 'TYPE' chooses to change its version format.
//
///Factored "Output" Methods
///- - - - - - - - - - - - -
// One obvious design choice for 'bdex_outstreammethods' is that it supports
// the *output* portion of the 'bdex' "contract" only.  This factoring is by no
// means central to the 'bdex' contract as a whole, but allows authors of
// special-purpose (i.e., non-value-semantic) components to implement a subset
// of the 'bdex' facilities without causing a compile-time error when their
// clients attempt to use that 'bdex' functionality through the template
// mechanism of this component.
//
///Usage Example
///-------------
// In this example we illustrate the primary intended use of the templatized
// methods of this component, as well as a few trivial invocations just to show
// the syntax clearly.  To accomplish this, we exhibit three separate example
// "components": an out-stream, a value-semantic point object, and a
// value-semantic 'enum' component.  In all cases, the component designs are
// very simple, with much of the implied functionality omitted, in order to
// focus attention on the key aspects of the functionality of *this* component.
//
// First, consider an 'enum' component 'my_color' that enumerates a set of
// colors.  Such a component may be used to constrain the "color" property of
// some higher-level object.  Within BDE, an 'enum' component has a canonical
// form (see, e.g., 'bdet_dayofweek').  We show the full interface just to
// illustrate an 'enum' component, but implement only the functionality needed
// for this example.
//..
//  // my_color.h
//  // ...
//  struct my_Color {
//      // This 'struct' provides a namespace for enumerating set of colors
//      // appropriate for objects in the 'my' package.
//
//      // TYPES
//      enum Color {
//          RED            =  0,
//          GREEN          =  1,
//          BLUE           =  2
//      };
//
//      enum {
//          NUM_COLORS = BLUE + 1      // Number of colors
//      };
//
//    public:
//      // CLASS METHODS
//      static int maxSupportedBdexVersion();
//          // Return the most current 'bdex' streaming version number
//          // supported by this class.
//
//      static const char *toAscii(my_Color::Color value);
//          // Return the string representation exactly matching the enumerator
//          // name corresponding to the specified enumerator 'value'.
//
//      template <class STREAM>
//      static STREAM& bdexStreamIn(STREAM&          stream,
//                                  my_Color::Color& value,
//                                  int              version);
//          // Assign to the specified 'value' the value read from the
//          // specified input 'stream' using the specified 'version' format
//          // and return a reference to the modifiable 'stream'.  ...
//
//    static bsl::ostream& streamOut(bsl::ostream&   stream,
//                                   my_Color::Color rhs);
//          // Write the enumerator corresponding to the specified 'rhs' value
//          // to the specified 'stream'.
//
//      template <class STREAM>
//      static STREAM& bdexStreamOut(STREAM&         stream,
//                                   my_Color::Color value,
//                                   int             version);
//          // Write the specified 'value' to the specified output 'stream'
//          // using the specified 'version' format and return a reference to
//          // the modifiable 'stream'. ...
//  };
//
//  // FREE OPERATORS
//  inline
//  bsl::ostream& operator<<(bsl::ostream& stream, my_Color::Color rhs);
//      // Write the enumerator corresponding to the specified 'rhs' value
//      // to the specified 'stream'.
//..
// The (inline) implementations of selected functions are as follows.
//..
//  //                    INLINE FUNCTION DEFINITIONS
//
//  // CLASS METHODS
//  inline
//  int my_Color::maxSupportedBdexVersion()
//  {
//      return 1;
//  }
//
//  // ...
//
//  template <class STREAM>
//  inline
//  STREAM& my_Color::bdexStreamOut(STREAM&         stream,
//                                  my_Color::Color value,
//                                  int             version)
//  {
//      switch (version) {
//        case 1: {
//          stream.putInt8((char) value);  // Write the value as a single byte.
//        } break;
//      }
//      return stream;
//  }
//..
// To comply with the 'bdex' contract, we must also provide a specialization of
// the 'bdex_OutStreamMethods' template class.  Note that the class method
// 'bdexStreamOut' invokes the *class* *method* 'my_Color::bdexStreamOut' of
// the 'my_Color' 'struct'.
//..
//  template <>
//  class bdex_OutStreamMethods<my_Color::Color> {
//    public:
//      static int maxSupportedBdexVersion() {
//          return my_Color::maxSupportedBdexVersion();
//      }
//
//      template <typename STREAM>
//      static STREAM& bdexStreamOut(STREAM&                stream,
//                                   const my_Color::Color& value,
//                                   int                    version)
//      {
//          return my_Color::bdexStreamOut(stream, value, version);
//      }
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
//  // ...
//  #ifndef INCLUDED_MY_COLOR
//  #include <my_color.h>
//  #endif
//
//  class my_Point {
//      // This class provides a geometric point having integer coordinates and
//      // an enumerated color property.
//
//      short           d_x;      // x coordinate
//      short           d_y;      // y coordinate
//      my_Color::Color d_color;  // enumerated color property
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
//    my_Point(int x, int y, my_Color::Color color);
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
//    my_Color::Color color() const;
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
//  my_Point::my_Point(int x, int y, my_Color::Color color)
//  : d_x(x)
//  , d_y(y)
//  , d_color(color)
//  { }
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
//  STREAM& my_Point::bdexStreamOut(STREAM& stream, int version) const;
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
//      return lhs.x()     == rhs.x() &&
//             lhs.y()     == rhs.y() &&
//             lhs.color() == rhs.color();
//  }
//..
// Finally, we will implement an extremely simple output stream that supports
// the 'bdex' documentation-only protocol.  For simplicity we will use a
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
//
//  template <typename OBJECT>
//  inline
//  my_OutStream& operator<<(my_OutStream& stream, const OBJECT& object);
//      // Write to the specified output 'stream', using the 'putVersion'
//      // method of this component, the most current version number supported
//      // by 'OBJECT' and format the value of the specified 'object' to
//      // 'stream' using the 'bdex' compliant 'OBJECT::bdexStreamOut' method
//      // in the 'OBJECT::maxSupportedBdexVersion' format.  Return a reference
//      // to the modifiable 'stream'.  Note that specializations for the
//      // supported fundamental (scalar) types and for 'bsl::string' are also
//      // provided, in which case this operator calls the appropriate 'put'
//      // method from this component.
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
//      memcpy(d_buffer + d_length, (const char *) &value, 4);
//      d_length += 4;
//      return *this;
//  }
//
//  inline
//  my_OutStream& my_OutStream::putInt16(int value)
//  {
//      unsigned short temp = value;
//      memcpy(d_buffer + d_length, (const char *) &temp, 2);
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
//      const char *tempPtr = (const char *) &value;
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
//
//  // FREE OPERATORS
//  template <typename OBJECT>
//  inline
//  my_OutStream& operator<<(my_OutStream& stream, const OBJECT& object)
//  {
//      const int version =
//                    bdex_OutStreamMethods<OBJECT>::maxSupportedBdexVersion();
//      stream.putVersion(version);
//      return bdex_OutStreamMethods<OBJECT>::bdexStreamOut(stream,
//                                                          object,
//                                                          version);
//  }
//                          // *** integer values ***
//  template <>
//  inline
//  my_OutStream& operator<<(my_OutStream& stream, const int& value)
//  {
//      return stream.putInt32(value);
//  }
//
//  template <>
//  inline
//  my_OutStream& operator<<(my_OutStream& stream, const short& value)
//  {
//      return stream.putInt16(value);
//  }
//
//  template <>
//  inline
//  my_OutStream& operator<<(my_OutStream& stream, const char& value)
//  {
//      return stream.putInt8(value);
//  }
//..
// We can now write a small 'main' test program that will use the above 'enum',
// point class, and output stream to illustrate 'bdex_OutStreamMethods'
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
//     assert(1 == bdex_OutStreamMethods<my_Point>::maxSupportedBdexVersion());
//     assert(0 == bdex_OutStreamMethods<int>::maxSupportedBdexVersion());
//     assert(0 == bdex_OutStreamMethods<double>::maxSupportedBdexVersion());
//
//     int             i = 168496141;           // byte pattern 0a 0b 0c 0d
//     my_Color::Color color = my_Color::BLUE;  // byte pattern 02
//     my_Point        p(0, -1, color);         // byte pattern 00 00 ff ff 02
//
//     my_OutStream    out;                assert(0 == out.length());
//
//     bdex_OutStreamMethods<int>::bdexStreamOut(out, i, 1);
//                                         assert(4 == out.length());
//     cout << out << endl;
//     out.removeAll();                    assert(0 == out.length());
//     out << i;                           assert(4 == out.length());
//     cout << out << endl << endl;
//
//     out.removeAll();                    assert(0 == out.length());
//     bdex_OutStreamMethods<my_Color::Color>::bdexStreamOut(out, color, 1);
//                                         assert(1 == out.length());
//     cout << out << endl;
//     out.removeAll();                    assert(0 == out.length());
//     out << color;                       assert(2 == out.length());
//     cout << out << endl << endl;
//
//     out.removeAll();                    assert(0 == out.length());
//     bdex_OutStreamMethods<my_Point>::bdexStreamOut(out, p, 1);
//                                         assert(5 == out.length());
//     cout << out << endl;
//     out.removeAll();                    assert(0 == out.length());
//     out << p;                           assert(6 == out.length());
//     cout << out << endl << endl;
//
//     return;
// }
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEX_OUTSTREAMFUNCTIONS
#include <bdex_outstreamfunctions.h>
#endif

#ifndef INCLUDED_BDEX_VERSIONFUNCTIONS
#include <bdex_versionfunctions.h>
#endif

#ifndef INCLUDED_BSLMF_ISCONVERTIBLE
#include <bslmf_isconvertible.h>
#endif

namespace BloombergLP {

class bdex_GenericOutStreamMethods {
    // This is a dummy class used to find out if 'bdex_OutStreamMethods' class
    // has been specialized or not.

    // Contains no interface or implementation by design
};
                       // ===========================
                       // class bdex_OutStreamMethods
                       // ===========================

template <typename TYPE>
class bdex_OutStreamMethods : public bdex_GenericOutStreamMethods {
    // DEPRECATED: New code should not use or specialize this class.  Use and
    // specialize the functions in 'bdex_OutStreamFunctions' instead.
    // This template class allows uniform function calls of the 'bdexStreamOut'
    // method and the 'maxSupportedBdexVersion' method for all 'bdex'-compliant
    // types.  A compilation error will occur if 'TYPE' does not support these
    // two methods with appropriate signatures.

  public:
    static int maxSupportedBdexVersion()
        // Return the maximum 'bdex' version supported by 'TYPE' (at compile
        // time).  The behavior is undefined unless 'TYPE' supports a
        // 'bdex'-compliant method having the same name.
    {
        // Forward to new 'bdex_OutStreamFunctions' component.
        // TBD! HACK: Must create an object pointer on which to call
        // 'bdex_OutStreamFunctions::maxSupportedBdexVersion()'.  Since all
        // types currently have static 'maxSupportedBdexVersion' version
        // functions, it is OK for this pointer to be null (for now).
        const TYPE *p = 0;
        return bdex_VersionFunctions::maxSupportedVersion(*p);
    }

    template <typename STREAM>
    static
    STREAM& bdexStreamOut(STREAM& stream, const TYPE& value, int version)
        // Write the specified 'value' to the specified output 'stream' using
        // the specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  The behavior is undefined unless 'TYPE' supports a
        // 'bdex'-compliant method having the same name and 'STREAM' is a
        // 'bdex'-compliant output stream.  Note that 'version' is not written
        // to 'stream'.
    {
        // Forward to new 'bdex_OutStreamFunctions' component.
        return bdex_OutStreamFunctions::streamOut(stream, value, version);
    }

};

struct bdex_OutStreamMethodsUtil {
    // This 'struct' was introduced to solve backward compatibility problems
    // while streaming out objects.  Its function should not be called
    // directly by clients.

  public:
    template <typename STREAM, typename TYPE>
    inline
    static STREAM& streamOutVersionAndObject(STREAM&     stream,
                                             const TYPE& object) {
        // Write the specified 'value' to the specified output 'stream',
        // and return a reference to 'stream'.  If 'maxSupportedBdexVersion'
        // and 'bdexStreamOut' are specialized for 'TYPE' then those functions
        // are called.  If not,
        // 'bdex_OutStreamFunctions::streamOutVersionAndObject' is called.

        typedef bdex_OutStreamMethods<TYPE> methodsClass;

        // If the instantiated 'bdex_OutStreamMethods<TYPE>' class has been
        // specialized by the user then it will not derive from
        // 'bdex_GenericOutStreamMethods', which the unspecialized
        // 'bdex_OutStreamMethods<TYPE>' does.  This way we can find out if
        // the object should be streamed out using the client specializations
        // or using the 'bdex_OutStreamFunctions' functions.

        const int isNotSpecialized =
                    bslmf_IsConvertible<methodsClass *,
                                        bdex_GenericOutStreamMethods *>::VALUE;

        if (isNotSpecialized) {
            return bdex_OutStreamFunctions::streamOutVersionAndObject(stream,
                                                                      object);
        }
        else {
            const int version = methodsClass::maxSupportedBdexVersion();
            stream.putVersion(version);
            return methodsClass::bdexStreamOut(stream,
                                               object,
                                               version);
        }
    }

};

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
