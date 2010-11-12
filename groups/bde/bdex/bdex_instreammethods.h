// bdex_instreammethods.h              -*-C++-*-
#ifndef INCLUDED_BDEX_INSTREAMMETHODS
#define INCLUDED_BDEX_INSTREAMMETHODS

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Enable uniform (un)externalization of user and fundamental types.
//
//@CLASSES:
//   bdex_InStreamMethods: templates for uniform 'bdex' type unexternalization
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@SEE_ALSO: bdex_instreamfunctions, bdex_outstreammethods
//
//@DEPRECATED: This component should not be used for new code.  Use (and
// specialize the functions in 'bdex_instreamfunctions', instead.
//
//@DESCRIPTION: This component provides a template class that enables uniform
// support for 'bdex' unexternalization across all 'bdex'-compliant
// user-defined types, including template types and containers, as well as
// those fundamental types (and 'bsl::string') for which the 'bdex' protocol
// provides direct support.
//
// The template class 'bdex_InStreamMethods' allows clients to call the
// 'bdexStreamIn' instance method for all objects of a 'bdex'-compliant 'TYPE'
// that support that method.  This component also provides specializations of
// 'bdex_InStreamMethods' for the 'bdex'-supported fundamental types and
// 'bsl::string', which allows authors of value-semantic template types to
// implement 'bdex' streaming uniformly without knowledge of the template type.
// Finally, the 'bdex' contract requires authors of 'bdex'-compliant entities
// that cannot satisfy the template class implementation (e.g., canonical BDE
// 'enum' types such as 'bdet_DayOfWeek::Day') to provide the appropriate
// specialization of 'bdex_InStreamMethods' for that entity.
//
///Component Design, Anticipated Usage, and the 'bdex' Contract
///------------------------------------------------------------
// 'bdex_instreammethods' is an integral part of the 'bdex' externalization
// contract.  The 'bdex' contract is at least in part "collaborative", which is
// to say that each developer of a given *kind* of component (e.g., a stream or
// a value-semantic container) must comply with the relevant portions of the
// contract to ensure that the "system as a whole" works for everybody.
// 'bdex_instreammethods' plays several related but different roles in helping
// various developers to produce 'bdex'-compliant components.  In this section
// we briefly highlight how and why 'bdex_instreammethods' is helpful (or
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
//    class bdex_InStreamMethods<bdet_DayOfWeek::Day>
//..
// The primary motivation for this requirement is that the template class
//..
//     template <class TYPE>
//     class bdex_InStreamMethods
//..
// as defined in this component expects 'TYPE' to support the instance method
// 'bdexStreamIn', and therefore cannot accept a named 'enum' to instantiate a
// class from the template.  The author of the named-'enum' specialization must
// provide an appropriate implementation within the component defining the
// 'enum'.  Note that the requirement to provide a specialization applies as
// well to any entity for which the template class of this component cannot be
// instantiated.
//
///Implementing 'operator>>' in Input Streams and Formatters
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The 'bdex' contract requires each input stream to provide an overloaded
// 'operator>>' that will read any 'bdex'-compliant object from the stream.
// The method of the template class 'bdex_InStreamMethods' provide an effective
// implementation for the required operator.  The standard 'bdex' production
// streams are implemented using components known as "formatters" (see, e.g.,
// 'bdex_ByteInStreamFormatter'), but this factoring is not central to the
// contract.  Consider a simple 'my_instream' component (see the Usage Example
// below for more details).  Such a stream could implement a templatized
// 'operator>>' as follows.
//..
//  template <typename OBJECT>
//  inline
//  my_InStream& operator>>(my_InStream& stream, const OBJECT& object)
//  {
//      if (stream) {
//          int version;
//          stream.getVersion(version);
//          if (!stream) {
//              return stream;
//          }
//          bdex_InStreamMethods<OBJECT>::bdexStreamIn(stream,
//                                                     object
//                                                     version);
//      }
//      return stream;
//  }
//..
// Note that most streams will choose to specialize the above template function
// for the 'bdex'-supported fundamental types in order to suppress the version
// from being read by the stream.  For example, the specialization for 'int'
// might be:
//..
//  template <>
//  inline
//  bdex_ByteInStream& operator>>(bdex_ByteInStream& stream,
//                                int&               variable)
//  {
//      return stream.getInt32(variable);
//  }
//..
// where 'getInt32' is the input stream method for a 32-bit integer value.
// Note that cooperating In- and OutStreams must coordinate these
// specializations as well.
//
///Implementing 'bdex' Streaming in Value-Semantic Template Classes
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The author of a non-template value-semantic type has full knowledge of the
// details of the "value" of that type, and may choose to use the appropriate
// input stream 'get' methods directly when implementing the required
// 'bdexStreamIn' method for that type.  However, if one or more aspects of the
// value are of template parameterized type, then the author cannot in
// general know how to stream the value using the 'get' methods.  For example,
// if a type has as its value one 'int' data member:
//..
//    int d_value;
//..
// then the implementation of the 'bdexStreamIn' method can contain:
//..
//    stream.getInt32(d_value);
//..
// However, if the data member is of parameterized 'TYPE':
//..
//    TYPE d_value;
//..
// then the implementation of the 'bdexStreamIn' method must rely on the
// 'bdex_InStreamMethods' implementation to read the value:
//..
//    bdex_InStreamMethods<TYPE>::bdexStreamIn(stream, d_value, 1);
//..
// This call will resolve to the correct sequence of 'get' calls no matter
// whether 'TYPE' is a fundamental type, a 'bdex'-compliant 'enum', or a proper
// 'bdex'-compliant class.  In the latter two cases, the explicit specification
// of the version format (in this case, 1) guarantees the stable operation of
// this method whether or not 'TYPE' chooses to change its version format.
//
///Factored "Input" Methods
/// - - - - - - - - - - - -
// One obvious design choice for 'bdex_instreammethods' is that it supports the
// *input* portion of the 'bdex' "contract" only.  This factoring is by no
// means central to the 'bdex' contract as a whole, but allows authors of
// special-purpose (i.e., non-value-semantic) components to implement a subset
// of the 'bdex' facilities without causing a compile-time error when their
// clients attempt to use that 'bdex' functionality through the template
// mechanism of this component.
//
///Usage
///-----
// In this example we illustrate the primary intended uses of the templatized
// methods of this component, as well as a few trivial invocations just to
// show the syntax clearly.  To accomplish this, we exhibit three
// separate example "components": an in-stream, a value-semantic point object,
// and a value-semantic 'enum' component.  In all cases, the component designs
// are very simple, with much of the implied functionality omitted, in order to
// focus attention on the key aspects of the functionality of *this* component.
//
// First, consider an 'enum' component 'my_color' that enumerates a set of
// colors.  Such a component may be used to constrain the "color" property of
// some higher-level object that within a component that depends on
// 'my_color'.  Within BDE, an 'enum' component has a canonical
// form (see, e.g., 'bdet_dayofweek').  We show the full interface
// just to illustrate an 'enum' component, but implement only the functionality
// needed for this example.
//..
//  // my_color.h
//  // ...
//
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
//      static bsl::ostream& streamOut(bsl::ostream&   stream,
//                                     my_Color::Color rhs);
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
//  // CLASS METHODS
//  inline
//  int my_Color::maxSupportedBdexVersion()
//  {
//      return 1;
//  }
//
//  template <class STREAM>
//  inline
//  STREAM& my_Color::bdexStreamIn(STREAM&          stream,
//                                 my_Color::Color& value,
//                                 int              version)
//  {
//      switch(version) {
//        case 1: {
//          char newValue;
//          stream.getInt8(newValue);
//          if (stream) {
//              if (0 <= newValue && newValue < my_Color::NUM_COLORS) {
//                  value = my_Color::Color(newValue);
//              }
//              else {
//                  stream.invalidate(); // Bad value in stream.
//              }
//          }
//        } break;
//        default: {
//          stream.invalidate();         // Unrecognized version number.
//        }
//      }
//      return stream;
//  }
//
//  inline
//  bsl::ostream& my_Color::streamOut(bsl::ostream&   stream,
//                                    my_Color::Color rhs)
//  {
//      stream << toAscii(rhs);
//      return stream;
//  }
//..
// The global 'BloombergLP::bdex_InStreamMethods' class must be specialized
// for enumeration types, as shown below.
//
//..
//  namespace BloombergLP {
//
//  template <>
//  class bdex_InStreamMethods<my_Color::Color> {
//    public:
//      template <typename STREAM>
//      static STREAM& bdexStreamIn(STREAM&          stream,
//                                  my_Color::Color& value,
//                                  int              version)
//      {
//          return my_Color::bdexStreamIn(stream, value, version);
//      }
//  };
//
//  } // Close namespace BloombergLP
//..
// Next, we consider a very special-purpose point that has as a data member its
// color.  Such a point provides an excellent opportunity for factoring, but
// since we are interested in highlighting 'bdex' streaming of various types,
// we will present a simple and unfactored design here.  In a real-world
// problem, the 'my_point' component would be implemented differently.
//
// Note that the 'my_Point' class in this example represents its coordinates as
// 'short' integer values; this is done to make the 'bdex' stream output byte
// pattern somewhat easier for the reader of this example to recognize when
// the output buffer is printed.
//..
//  // my_point.h
//  // ...
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
//    template <class STREAM>
//    STREAM& bdexStreamIn(STREAM& stream, int version);
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
//        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
//        // unmodified.  Note that 'version' is not written to 'stream'.
//        // See the 'bdex' package-level documentation for more information
//        // on 'bdex' streaming of value-semantic types and containers.
//
//  };
//  // FREE OPERATORS
//  inline
//  int operator==(const my_Point& lhs, const my_Point& rhs);
//      // Return 1 if the specified 'lhs' and 'rhs' points have the same
//      // value, and 0 otherwise.  Two points have the same value if they
//      // have the same x and y coordinates and the same color.
//..
// The (inline) implementations of these methods are as follows.
//..
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
//  : d_x(0)
//  , d_y(0)
//  , d_color(my_Color::RED)
//  { }
//
//  inline
//  my_Point::my_Point(int x, int y, my_Color::Color color)
//  : d_x(x)
//  , d_y(y)
//  , d_color(color)
//  { }
//
//  inline
//  my_Point::~my_Point()
//  { }
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
//  inline
//  int my_Point::y() const
//  {
//      return d_y;
//  }
//
//  inline
//  my_Color::Color my_Point::color() const
//  {
//      return d_color;
//  }
//
//  template <class STREAM>
//  inline
//  STREAM& my_Point::bdexStreamIn(STREAM& stream, int version)
//  {
//      switch (version) {
//        case 1: {
//          stream.getInt16(d_x);            // read the x coordinate
//          stream.getInt16(d_y);            // read the y coordinate
//          char temp;
//          stream.getInt8(temp);  // read the color enum
//          if (0 <= temp && my_Color::NUM_COLORS > temp) {
//              d_color = (my_Color::Color) temp;
//          }
//        } break;
//      }
//      return stream;
//  }
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
//      return lhs.x()     == rhs.x() &&
//             lhs.y()     == rhs.y() &&
//             lhs.color() == rhs.color();
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
//      const char *d_buffer;  // input buffer, held but not owned
//      int         d_length;  // length of 'd_buffer' (bytes)
//      int         d_cursor;  // cursor (index into 'd_buffer'
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
//      int isEmpty() const;
//          // Return 1 if this stream is empty, and 0 otherwise.  The
//          // behavior is undefined unless this stream is valid (i.e., would
//          // convert to a non-zero value via the 'operator const void *()'
//          // member).  Note that this function enables higher-level
//          // components to verify that, after successfully reading all
//          // expected data, no data remains.
//
//      int length() const;
//          // Return the total number of bytes stored in this stream.
//  };
//
//  //   FREE OPERATORS
//  template <typename OBJECT>
//  inline
//  my_InStream& operator<<(my_InStream& stream, const OBJECT& object);
//      // Write to the specified input 'stream', using the 'putVersion'
//      // method of this component, the most current version number supported
//      // by 'OBJECT' and format the value of the specified 'object' to
//      // 'stream' using the 'bdex' compliant 'OBJECT::bdexStreamIn' method
//      // in the 'OBJECT::maxSupportedBdexVersion' format.  Return a reference
//      // to the modifiable 'stream'.  Note that specializations for the
//      // supported fundamental (scalar) types and for 'bsl::string' are also
//      // provided, in which case this operator calls the appropriate 'put'
//      // method from this component.
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
//  int my_InStream::isEmpty() const
//  {
//      return d_cursor >= d_length;
//  }
//
//  inline
//  int my_InStream::length() const
//  {
//      return d_length;
//  }
//
//  // FREE OPERATORS
//  template <typename OBJECT>
//  inline
//  my_InStream& operator>>(my_InStream& stream, OBJECT& object)
//  {
//      if (stream) {
//          int version;
//          stream.getVersion(version);
//          if (!stream) {
//              return stream;
//          }
//          bdex_InStreamMethods<OBJECT>::bdexStreamIn(stream, object,
//                                                     version);
//      }
//      return stream;
//  }
//
//                          // *** integer values ***
//
//  template <>
//  inline
//  my_InStream& operator>>(my_InStream& stream, int& value)
//  {
//      return stream.getInt32(value);
//  }
//
//  template <>
//  inline
//  my_InStream& operator>>(my_InStream& stream, short& value)
//  {
//      return stream.getInt16(value);
//  }
//
//  template <>
//  inline
//  my_InStream& operator>>(my_InStream& stream, char& value)
//  {
//      return stream.getInt8(value);
//  }
//..
// We can now write a small 'main' test program that will use the above 'enum',
// point class, and input stream to illustrate 'bdex_InStreamMethods'
// functionality.  This test program does not attempt to do anything more
// useful than reading values from a stream whose buffer was written "by hand"
// and confirming that the expected values were read correctly from the known
// byte pattern in the buffer.
//
// For each of a sequence of types ('int', 'my_Color::Color', and 'my_Point'),
// we will define a reference value 'EXP', and one or more input buffers
// (without and possibly with an initial "version" byte) containing the byte
// pattern for 'EXP' (as interpreted by 'my_InStream').  We will then declare
// one or more variables of that type initialized to a value distinct from
// 'EXP', create two 'my_InStream' objects from the appropriate buffers, and
// stream in a value from each stream using either the 'bdex_InStreamMethods'
// method explicitly (which does not consume a version byte), or 'operator>>'
// as defined in 'my_inputstream.h' (which does consume and use a version byte
// except for fundamental types).  We verify success with 'assert' statements.
// The astute reader may observe that this "application" is structured rather
// like a test driver.  As mentioned above, out intent is to focus on the use
// of *this* component, and not on the *use* of components that are clients of
// this component.  Typical application programmers will rarely, if ever, need
// to use this component directly.
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
//          bdex_InStreamMethods<int>::bdexStreamIn(in1, i, 1);
//                                           assert(in1);  assert(EXP == i);
//
//          i = 0;                       // reset 'i'
//          my_InStream in2(buffer, 4);  // re-use 'buffer (no version)
//          in2 >> i;                        assert(in2);  assert(EXP == i);
//      }
//
//      {
//          my_Color::Color EXP   = my_Color::BLUE;
//          const char buffer1[1] = { 2 };    // my_Color::BLUE (no version)
//          const char buffer2[2] = { 1, 2 }; // version, my_Color::BLUE
//          my_Color::Color color = my_Color::RED;
//
//          my_InStream in1(buffer1, 1);  // 'buffer1' has no version byte
//          bdex_InStreamMethods<my_Color::Color>::bdexStreamIn(in1, color, 1);
//                                           assert(in1); assert(EXP == color);
//
//          color = my_Color::RED;        // reset 'color'
//          my_InStream in2(buffer2, 2);  // 'buffer2' *has* a version byte
//          in2 >> color;                    assert(in2); assert(EXP == color);
//      }
//
//      {
//          const my_Point EXP(0, -1, my_Color::BLUE);
//          const char buffer1[5] = { 0, 0, -1, -1, 2 };    //my_Point (no ver)
//          const char buffer2[6] = { 1, 0, 0, -1, -1, 2 }; //version, my_Point
//          my_Point p1, p2;  // two default points
//
//          my_InStream in1(buffer1, 5);  // 'buffer1' has no version byte
//          bdex_InStreamMethods<my_Point>::bdexStreamIn(in1, p1, 1);
//                                           assert(in1);  assert(EXP == p1);
//
//          my_InStream in2(buffer2, 6);  // 'buffer2' *has* a version byte
//          in2 >> p2;                       assert(in2);  assert(EXP == p2);
//      }
//
//      return 0;
//  }
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEX_INSTREAMFUNCTIONS
#include <bdex_instreamfunctions.h>
#endif

#ifndef INCLUDED_BSLMF_ISCONVERTIBLE
#include <bslmf_isconvertible.h>
#endif

namespace BloombergLP {

class bdex_GenericInStreamMethods {
    // This is a dummy class used to find out if the 'bdex_InStreamMethods'
    // class has been specialized or not.  It contains no interface or
    // implementation by design.
};

                       // ==========================
                       // class bdex_InStreamMethods
                       // ==========================

template <typename TYPE>
class bdex_InStreamMethods : public bdex_GenericInStreamMethods {
    // DEPRECATED: New code should not use or specialize this class.  Use and
    // specialize the functions in 'bdex_InStreamFunctions' instead.
    // This template class allows uniform function calls of the 'bdexStreamIn'
    // method for all 'bdex'-compliant types.  A compilation error will occur
    // if 'TYPE' does not support this method with an appropriate signature.

  public:
    template <typename STREAM>
    static STREAM& bdexStreamIn(STREAM& stream, TYPE& object, int version)
        // Assign to the specified 'object' the value read from the specified
        // input 'stream' using the specified 'version' format and return a
        // reference to the modifiable 'stream'.  If 'stream' is initially
        // invalid, this operation has no effect.  If 'stream' becomes invalid
        // during this operation, 'object' is valid, but its value is
        // undefined.  If 'version' is not supported, 'stream' is marked
        // invalid and 'object' is unaltered.  The behavior is undefined unless
        // 'TYPE' supports a 'bdex'-compliant 'bdexStreamIn' method and
        // 'STREAM' is a 'bdex'-compliant input stream.  Note that no version
        // is read from 'stream'.
    {
        return bdex_InStreamFunctions::streamIn(stream, object, version);
    }
};

struct bdex_InStreamMethodsUtil {
    // This 'struct' was introduced to solve backward compatibility problems
    // while streaming in objects.  Its function should not be called
    // directly by clients.

  public:
    template <typename STREAM, typename TYPE>
    inline
    static STREAM& streamInVersionAndObject(STREAM& stream, TYPE& object) {
        // Assign to the specified 'object' the value read from the specified
        // input 'stream' and return a reference to the modifiable 'stream'.
        // If 'stream' is initially invalid, this operation has no effect.  If
        // 'stream' becomes invalid during this operation, 'object' is valid,
        // but its value is undefined.  If 'version' is not supported, 'stream'
        // is marked invalid and 'object' is unaltered.  Note that version is
        // read from 'stream' only if 'TYPE' supports a valid version.  If the
        // user has provided specialization for 'bdexStreamIn' then that
        // function is called.  If not, the 'bdex_InStreamFunctions'
        // 'streamInVersionAndObject' function is called.

        typedef bdex_InStreamMethods<TYPE> methodsClass;

        // If the instantiated 'bdex_InStreamMethods<TYPE>' class has been
        // specialized by the user then it will not derive from
        // 'bdex_GenericInStreamMethods', which the unspecialized
        // 'bdex_InStreamMethods<TYPE>' does.  This way we can find out if
        // the object should be streamed in using the client specializations
        // or using the 'bdex_InStreamFunctions' functions.

        const int isNotSpecialized =
                    bslmf_IsConvertible<methodsClass *,
                                        bdex_GenericInStreamMethods *>::VALUE;

        if (isNotSpecialized) {
            return bdex_InStreamFunctions::streamInVersionAndObject(stream,
                                                                    object);
        }
        else {
            if (stream) {
                int version;
                stream.getVersion(version);
                if (stream) {
                    methodsClass::bdexStreamIn(stream, object, version);
                }
            }
            return stream;
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
