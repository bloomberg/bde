// bdex_instreammethods.t.cpp          -*-C++-*-

#include <bdex_instreammethods.h>

#include <bsls_platform.h>           // for testing only
#include <bsls_platformutil.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::endl;
using bsl::atoi;
using bsl::flush;
using bsl::cerr;

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This component provides a template class that has one static member
// function, which invokes a method of its template parameter.  In
// addition, the component provides a number of specializations of this class
// for fundamental types and 'bsl::string'.  The testing requirements are
// fairly straightforward, and somewhat similar to the testing strategy for an
// abstract protocol.  It is sufficient to provide a test type and a test
// stream, each of which responds in a simple, observable manner when its
// various methods are called.
//-----------------------------------------------------------------------------
// template <typename TYPE> class bdex_InStreamMethods
//      template <typename STREAM>
//      STREAM& bdexStreamIn(STREAM& stream, TYPE& object, int version)
//
//  class bdex_InStreamMethods<bsls_PlatformUtil::Int64>
//      template <typename STREAM>
//      STREAM& bdexStreamIn(STREAM& stream, Int64& variable, int version)
//
//  class bdex_InStreamMethods<bsls_PlatformUtil::Uint64>
//      template <typename STREAM>
//      STREAM& bdexStreamIn(STREAM& stream, Uint64& variable, int version)
//
//  class bdex_InStreamMethods<int>
//      template <typename STREAM>
//      STREAM& bdexStreamIn(STREAM& stream, int& variable, int version)
//
//  class bdex_InStreamMethods<unsigned int>
//      template <typename STREAM>
//      STREAM& bdexStreamIn(STREAM&, unsigned int&, int)
//
//  class bdex_InStreamMethods<short>
//      template <typename STREAM>
//      STREAM& bdexStreamIn(STREAM& stream, short& variable, int version)
//
//  class bdex_InStreamMethods<unsigned short>
//      template <typename STREAM>
//      STREAM& bdexStreamIn(STREAM&, unsigned short&, int)
//
//  class bdex_InStreamMethods<bsl::string>
//      template <typename STREAM>
//      STREAM& bdexStreamIn(STREAM&, bsl::string&, int)
//
//  class bdex_InStreamMethods<char>
//      template <typename STREAM>
//      STREAM& bdexStreamIn(STREAM& stream, char& variable, int version)
//
//  class bdex_InStreamMethods<signed char>
//      template <typename STREAM>
//      STREAM& bdexStreamIn(STREAM&, signed char&, int)
//
//  class bdex_InStreamMethods<unsigned char>
//      template <typename STREAM>
//
//  class bdex_InStreamMethods<double>
//      template <typename STREAM>
//      STREAM& bdexStreamIn(STREAM& stream, double& variable, int version)
//
//  class bdex_InStreamMethods<float>
//      template <typename STREAM>
//      STREAM& bdexStreamIn(STREAM& stream, float& variable, int version)
//-----------------------------------------------------------------------------
// [ 1] Usage Example

//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;
static int verbose = 0;
static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\t" << #K << ": " << K << "\n";       \
                  aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define PS(X) cout << #X " = \n" << (X) << endl; // Print identifier and value.
#define T_  cout << "\t" << flush;            // Print a tab (w/o newline)

//=============================================================================
//                   GLOBAL CONSTANTS/TYPEDEFS FOR TESTING
//=============================================================================

//=============================================================================
//              Classes, functions, etc., needed for Usage Example
//-----------------------------------------------------------------------------
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
    // my_color.h
    // ...

    struct my_Color {
        // This 'struct' provides a namespace for enumerating set of colors
        // appropriate for objects in the 'my' package.

        // TYPES
        enum Color {
            RED            =  0,
            GREEN          =  1,
            BLUE           =  2
        };

        enum {
            NUM_COLORS = BLUE + 1      // Number of colors
        };

      public:
        // CLASS METHODS
        static int maxSupportedBdexVersion();
            // Return the most current 'bdex' streaming version number
            // supported by this class.

        static const char *toAscii(my_Color::Color value);
            // Return the string representation exactly matching the enumerator
            // name corresponding to the specified enumerator 'value'.

        template <class STREAM>
        static STREAM& bdexStreamIn(STREAM&          stream,
                                    my_Color::Color& value,
                                    int              version);
            // Assign to the specified 'value' the value read from the
            // specified input 'stream' using the specified 'version' format
            // and return a reference to the modifiable 'stream'.  ...

        static bsl::ostream& streamOut(bsl::ostream&   stream,
                                       my_Color::Color rhs);
            // Write the enumerator corresponding to the specified 'rhs' value
            // to the specified 'stream'.

        template <class STREAM>
        static STREAM& bdexStreamOut(STREAM&         stream,
                                     my_Color::Color value,
                                     int             version);
            // Write the specified 'value' to the specified output 'stream'
            // using the specified 'version' format and return a reference to
            // the modifiable 'stream'. ...
    };

    // FREE OPERATORS
    inline
    bsl::ostream& operator<<(bsl::ostream& stream, my_Color::Color rhs)
    {
        return my_Color::streamOut(stream, rhs);
    }
        // Write the enumerator corresponding to the specified 'rhs' value
        // to the specified 'stream'.
//..
// The (inline) implementations of selected functions are as follows.
//..
    // CLASS METHODS
    inline
    int my_Color::maxSupportedBdexVersion()
    {
        return 1;
    }

    template <class STREAM>
    inline
    STREAM& my_Color::bdexStreamIn(STREAM&          stream,
                                   my_Color::Color& value,
                                   int              version)
    {
        switch(version) {
          case 1: {
            char newValue;
            stream.getInt8(newValue);
            if (stream) {
                if (0 <= newValue && newValue < my_Color::NUM_COLORS) {
                    value = my_Color::Color(newValue);
                }
                else {
                    stream.invalidate(); // Bad value in stream.
                }
            }
          } break;
          default: {
            stream.invalidate();         // Unrecognized version number.
          }
        }
        return stream;
    }

    inline
    const char *my_Color::toAscii(my_Color::Color value)
    {
        switch (value) {
            case RED:   return "RED";
            case GREEN: return "GREEN";
            case BLUE:  return "BLUE";
            default:    return "(* UNKNOWN *)";
        }
    }

    inline
    bsl::ostream& my_Color::streamOut(bsl::ostream&   stream,
                                      my_Color::Color rhs)
    {
        stream << toAscii(rhs);
        return stream;
    }
//..
// The global 'BloombergLP::bdex_InStreamMethods' class must be specialized
// for enumeration types, as shown below.
//
    namespace BloombergLP {

    template <>
    class bdex_InStreamMethods<my_Color::Color> {
      public:
        template <typename STREAM>
        static STREAM& bdexStreamIn(STREAM&          stream,
                                    my_Color::Color& value,
                                    int              version)
        {
            return my_Color::bdexStreamIn(stream, value, version);
        }
    };

    } // Close namespace BloombergLP
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
    // my_point.h
    // ...

    class my_Point {
        // This class provides a geometric point having integer coordinates and
        // an enumerated color property.

        short           d_x;      // x coordinate
        short           d_y;      // y coordinate
        my_Color::Color d_color;  // enumerated color property

      public:
      // CLASS METHODS
      static int maxSupportedBdexVersion();
          // Return the most current 'bdex' streaming version number supported
          // by this class.

      // CREATORS
      my_Point();
          // Create a valid default point.

      my_Point(int x, int y, my_Color::Color color);
          // Create a point having the specified 'x' and 'y' coordinates
          // and the specified 'color'.

      ~my_Point();
          // Destroy this point.

      // MANIPULATORS
      // ...
      template <class STREAM>
      STREAM& bdexStreamIn(STREAM& stream, int version);

      // ACCESSORS
      int x() const;
          // Return the x coordinate of this point.

      int y() const;
          // Return the y coordinate of this point.

      my_Color::Color color() const;
          // Return the enumerated color of this point.

      template <class STREAM>
      STREAM& bdexStreamOut(STREAM& stream, int version) const;
          // Write this value to the specified output 'stream' using the
          // specified 'version' format and return a reference to the
          // modifiable 'stream'.  If 'version' is not supported, 'stream' is
          // unmodified.  Note that 'version' is not written to 'stream'.
          // See the 'bdex' package-level documentation for more information
          // on 'bdex' streaming of value-semantic types and containers.

    };
    // FREE OPERATORS
    inline bool operator==(const my_Point& lhs, const my_Point& rhs);
        // Return 'true' if the specified 'lhs' and 'rhs' points have the same
        // value, and 'false' otherwise.  Two points have the same value if
        // they have the same x and y coordinates and the same color.
//..
// The (inline) implementations of these methods are as follows.
//..
    // CLASS METHODS
    inline
    int my_Point::maxSupportedBdexVersion()
    {
        return 1;
    }

    // CREATORS
    inline
    my_Point::my_Point()
    : d_x(0)
    , d_y(0)
    , d_color(my_Color::RED)
    { }

    inline
    my_Point::my_Point(int x, int y, my_Color::Color color)
    : d_x(x)
    , d_y(y)
    , d_color(color)
    { }

    inline
    my_Point::~my_Point()
    { }

    // MANIPULATORS
    // ...

    // ACCESSORS
    inline
    int my_Point::x() const
    {
        return d_x;
    }

    inline
    int my_Point::y() const
    {
        return d_y;
    }

    inline
    my_Color::Color my_Point::color() const
    {
        return d_color;
    }

    template <class STREAM>
    inline
    STREAM& my_Point::bdexStreamIn(STREAM& stream, int version)
    {
        switch (version) {
          case 1: {
            stream.getInt16(d_x);            // read the x coordinate
            stream.getInt16(d_y);            // read the y coordinate
            char temp;
            stream.getInt8(temp);  // read the color enum
            if (0 <= temp && my_Color::NUM_COLORS > temp) {
                d_color = (my_Color::Color) temp;
            }
          } break;
        }
        return stream;
    }

    template <class STREAM>
    inline
    STREAM& my_Point::bdexStreamOut(STREAM& stream, int version) const
    {
        switch (version) {
          case 1: {
            stream.putInt16(d_x);           // write the x coordinate
            stream.putInt16(d_y);           // write the y coordinate
            stream.putInt8((char) d_color); // write the color enum as one byte
          } break;
        }
        return stream;
    }

    // FREE OPERATORS
    inline bool operator==(const my_Point& lhs, const my_Point& rhs)
    {
        return lhs.x()     == rhs.x() &&
               lhs.y()     == rhs.y() &&
               lhs.color() == rhs.color();
    }
//..
// Finally, we will implement an extremely simple input stream that supports
// the 'bdex' documentation-only protocol.  For simplicity we will use an
// externally managed buffer, and will only show a few methods needed for this
// example.
//..
    // my_instream.h
    // ...

    class my_InStream {
        // This class implements a limited-size fixed-buffer input stream that
        // conforms to the 'bdex' protocol for input streams.  This class is
        // suitable for demonstration purposes only.

        const char *d_buffer;   // input buffer, held but not owned
        int         d_length;   // length of 'd_buffer' (bytes)
        int         d_cursor;   // cursor (index into 'd_buffer'

      public:
        // CREATORS
        my_InStream(const char *buffer, int length);
            // Create an input stream using the specified 'buffer' of the
            // specified 'length'.

        ~my_InStream();
           // Destroy this input byte stream.

        // MANIPULATORS
        my_InStream& getLength(int& length);
            // Consume a length value from this input stream, place that
            // value in the specified 'length', and return a reference to
            // this modifiable stream.  ...

        my_InStream& getVersion(int& version);
            // Consume a version value from this input stream, place that
            // value in the specified 'version', and return a reference to
            // this modifiable stream.  ...

        my_InStream& getInt32(int& value);
            // Consume a 32-bit signed integer value from this input stream,
            // place that value in the specified 'variable', and return a
            // reference to this modifiable stream.  ...

        my_InStream& getInt16(short& value);
            // Consume a 16-bit signed integer value from this input stream,
            // place that value in the specified 'variable', and return a
            // reference to this modifiable stream.  ...

        my_InStream& getInt8(char& value);
            // Consume an 8-bit signed integer value from this input stream,
            // place that value in the specified 'variable', and return a
            // reference to this modifiable stream.  ...

        my_InStream& getArrayFloat64(double *array,
                                     int     length)
        {
            int numBytes = length*sizeof(*array);
            bsl::memcpy(array, d_buffer + d_cursor, numBytes);
            d_cursor += numBytes;
            return *this;
        }

        my_InStream& getArrayFloat32(float *array,
                                     int    length)
        {
            int numBytes = length*sizeof(*array);
            bsl::memcpy(array, d_buffer + d_cursor, numBytes);
            d_cursor += numBytes;
            return *this;
        }

        my_InStream& getArrayInt64(bsls_PlatformUtil::Int64 *array,
                                   int                       length)
        {
            int numBytes = length*sizeof(*array);
            bsl::memcpy(array, d_buffer + d_cursor, numBytes);
            d_cursor += numBytes;
            return *this;
        }

        my_InStream& getArrayInt32(int* value, int length)
        {
            for (int i = 0; i < length; ++i) {
                getInt32(value[i]);
            }
            return *this;
        }

        my_InStream& getArrayInt16(short* value, int length)
        {
            for (int i = 0; i < length; ++i) {
                getInt16(value[i]);
            }
            return *this;
        }

        my_InStream& getArrayInt8(char* value, int length)
        {
            bsl::memcpy(value, d_buffer + d_cursor, length);
            d_cursor += length;
            return *this;
        }

        void invalidate();
            // Put this input stream in an invalid state.  ...

        // ACCESSORS
        operator const void *() const;
            // Return a non-zero value if this stream is valid, and 0
            // otherwise.  An invalid stream is a stream in which insufficient
            // or invalid data was detected during an extraction operation.
            // Note that an empty stream will be valid unless an extraction
            // attempt or explicit invalidation causes it to be otherwise.

        int cursor() const;
            // Return the index of the next byte to be extracted from this
            // stream.  The behavior is undefined unless this stream is valid.

        int isEmpty() const;
            // Return 1 if this stream is empty, and 0 otherwise.  The
            // behavior is undefined unless this stream is valid (i.e., would
            // convert to a non-zero value via the 'operator const void *()'
            // member).  Note that this function enables higher-level
            // components to verify that, after successfully reading all
            // expected data, no data remains.

        int length() const;
            // Return the total number of bytes stored in this stream.
    };

    //   FREE OPERATORS
    template <typename OBJECT>
    inline
    my_InStream& operator<<(my_InStream& stream, const OBJECT& object);
        // Write to the specified input 'stream', using the 'putVersion'
        // method of this component, the most current version number supported
        // by 'OBJECT' and format the value of the specified 'object' to
        // 'stream' using the 'bdex' compliant 'OBJECT::bdexStreamIn' method
        // in the 'OBJECT::maxSupportedBdexVersion' format.  Return a reference
        // to the modifiable 'stream'.  Note that specializations for the
        // supported fundamental (scalar) types and for 'bsl::string' are also
        // provided, in which case this operator calls the appropriate 'put'
        // method from this component.
//..
// The relevant (inline) implementations are as follows.
//..
    // CREATORS
    inline
    my_InStream::my_InStream(const char *buffer, int length)
    : d_buffer(buffer)
    , d_length(length)
    , d_cursor(0)
    {
    }

    inline
    my_InStream::~my_InStream()
    {
    }

    // MANIPULATORS

    inline
    my_InStream& my_InStream::getLength(int& length)
    {
        char temp;
        my_InStream& ret = getInt8(temp);
        length = temp;
        return ret;
    }

    inline
    my_InStream& my_InStream::getVersion(int& value)
    {
        value = (unsigned char) d_buffer[d_cursor++];
        return *this;
    }

    inline
    my_InStream& my_InStream::getInt32(int& value)
    {
        const unsigned char *buffer = (const unsigned char *) d_buffer;
        value = static_cast<int>((buffer[d_cursor    ] << 24U) +
                                 (buffer[d_cursor + 1] << 16U) +
                                 (buffer[d_cursor + 2] <<  8U) +
                                 (buffer[d_cursor + 3]       ));
        d_cursor += 4;
        return *this;
    }

    inline
    my_InStream& my_InStream::getInt16(short& value)
    {
        const unsigned char *buffer = (const unsigned char *) d_buffer;
        value = static_cast<short>((buffer[d_cursor + 0] <<  8) +
                                   (buffer[d_cursor + 1]      ));
        d_cursor += 2;
        return *this;
    }

    inline
    my_InStream& my_InStream::getInt8(char& value)
    {
        value = d_buffer[d_cursor];
        d_cursor += 1;  // Imp Note: using syntax parallel to other 'getInt'
        return *this;
    }

    inline
    void my_InStream::invalidate()
    {
        d_buffer = 0;
    }

    // ACCESSORS
    inline
    my_InStream::operator const void *() const
    {
        return d_cursor <= d_length ? d_buffer : 0;
    }

    inline
    int my_InStream::cursor() const
    {
        return d_cursor;
    }

    inline
    int my_InStream::isEmpty() const
    {
        return d_cursor >= d_length;
    }

    inline
    int my_InStream::length() const
    {
        return d_length;
    }

    // FREE OPERATORS
    template <typename OBJECT>
    inline
    my_InStream& operator>>(my_InStream& stream, OBJECT& object)
    {
        if (stream) {
            int version;
            stream.getVersion(version);
            if (!stream) {
                return stream;
            }
            bdex_InStreamMethods<OBJECT>::bdexStreamIn(stream, object,
                                                       version);
        }
        return stream;
    }

                            // *** integer values ***

    template <>
    inline
    my_InStream& operator>>(my_InStream& stream, int& value)
    {
        return stream.getInt32(value);
    }

    template <>
    inline
    my_InStream& operator>>(my_InStream& stream, short& value)
    {
        return stream.getInt16(value);
    }

    template <>
    inline
    my_InStream& operator>>(my_InStream& stream, char& value)
    {
        return stream.getInt8(value);
    }
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
    // my_testapp.m.cpp

    int usageExample(int argc, char *argv[])
    {
        {
            const int  EXP       = 0x0A0B0C0D;
            const char buffer[4] = { 0xA, 0xB, 0xC, 0xD };  // int (no version)
            int        i         = 0;

            my_InStream in1(buffer, 4);  // use the one buffer
            bdex_InStreamMethods<int>::bdexStreamIn(in1, i, 1);
                                             ASSERT(in1);  ASSERT(EXP == i);
            if (verbose) { T_ P(EXP); T_ P(i); }

            i = 0;                       // reset 'i'
            my_InStream in2(buffer, 4);  // re-use 'buffer (no version)
            in2 >> i;                        ASSERT(in2);  ASSERT(EXP == i);
            if (verbose) { T_ P(EXP); T_ P(i); }
        }

        {
            my_Color::Color EXP   = my_Color::BLUE;
            const char buffer1[1] = { 2 };    // my_Color::BLUE (no version)
            const char buffer2[2] = { 1, 2 }; // version, my_Color::BLUE
            my_Color::Color color = my_Color::RED;

            my_InStream in1(buffer1, 1);  // 'buffer1' has no version byte
            bdex_InStreamMethods<my_Color::Color>::bdexStreamIn(in1, color, 1);
                                             ASSERT(in1); ASSERT(EXP == color);
            if (verbose) { T_ P((int)EXP); T_ P((int)color); }

            color = my_Color::RED;        // reset 'color'
            my_InStream in2(buffer2, 2);  // 'buffer2' *has* a version byte
            in2 >> color;                    ASSERT(in2); ASSERT(EXP == color);
            if (verbose) { T_ P((int)EXP); T_ P((int)color); }
        }

        {
            const my_Point EXP(0, -1, my_Color::BLUE);
            const char buffer1[5] = { 0, 0, -1, -1, 2 };    //my_Point (no ver)
            const char buffer2[6] = { 1, 0, 0, -1, -1, 2 }; //version, my_Point
            my_Point p1, p2;  // two default points

            my_InStream in1(buffer1, 5);  // 'buffer1' has no version byte
            bdex_InStreamMethods<my_Point>::bdexStreamIn(in1, p1, 1);
                                             ASSERT(in1);  ASSERT(EXP == p1);

            my_InStream in2(buffer2, 6);  // 'buffer2' *has* a version byte
            in2 >> p2;                       ASSERT(in2);  ASSERT(EXP == p2);
        }

        return 0;
    }

//=============================================================================
//                                  MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    // int veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 8: {
        // --------------------------------------------------------------------
        // TESTING double VECTOR STREAM
        //
        // Plan:
        //
        // Testing:
        //   bdex_InStreamMethods<bsl::vector<double, ALLOC> >
        // --------------------------------------------------------------------

        const double DATA[] = {  // any arbitrary data array
            7.45,
            5.24,
            432.43544,
            8765.32,
            543.0,
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int size = 0; size <= NUM_DATA; ++size) {
            char buffer1[128];
            char buffer2[128];

            // buffer1 will contain length + elements
            buffer1[0] = size;
            for (int i = 0, j = 0; i < size; ++i, j += sizeof(*DATA)) {
                bsl::memcpy(1 + buffer1 + j, &DATA[i], sizeof(*DATA));
            }
            const int buffer1Len = 1 + (size*sizeof(*DATA));

            // buffer2 will contain version + length + elements
            buffer2[0] = 0;
            buffer2[1] = size;
            for (int i = 0, j = 0; i < size; ++i, j += sizeof(*DATA)) {
                bsl::memcpy(2 + buffer2 + j, &DATA[i], sizeof(*DATA));
            }
            const int buffer2Len = 2 + (size*sizeof(*DATA));

            bsl::vector<double> p1, p2;  // two default vectors

            my_InStream in1(buffer1, buffer1Len);  // without version
            ASSERT(in1);
            bdex_InStreamMethods<bsl::vector<double> >::bdexStreamIn(
                                                                   in1, p1, 0);
            ASSERT(in1);
            LOOP2_ASSERT(size, p1.size(), size == p1.size());
            for (int i = 0; i < size; ++i) {
                LOOP3_ASSERT(size, i, p1[i], DATA[i] == p1[i]);
            }

            my_InStream in2(buffer2, buffer2Len);  // with version
            ASSERT(in2);
            in2 >> p2;
            ASSERT(in2);
            LOOP2_ASSERT(size, p2.size(), size == p2.size());
            for (int i = 0; i < size; ++i) {
                LOOP3_ASSERT(size, i, p2[i], DATA[i] == p2[i]);
            }
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING float VECTOR STREAM
        //
        // Plan:
        //
        // Testing:
        //   bdex_InStreamMethods<bsl::vector<float, ALLOC> >
        // --------------------------------------------------------------------

        const float DATA[] = {  // any arbitrary data array
            7.45f,
            5.24f,
            432.43544f,
            8765.32f,
            543.0f,
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int size = 0; size <= NUM_DATA; ++size) {
            char buffer1[128];
            char buffer2[128];

            // buffer1 will contain length + elements
            buffer1[0] = size;
            for (int i = 0, j = 0; i < size; ++i, j += sizeof(*DATA)) {
                bsl::memcpy(1 + buffer1 + j, &DATA[i], sizeof(*DATA));
            }
            const int buffer1Len = 1 + (size*sizeof(*DATA));

            // buffer2 will contain version + length + elements
            buffer2[0] = 0;
            buffer2[1] = size;
            for (int i = 0, j = 0; i < size; ++i, j += sizeof(*DATA)) {
                bsl::memcpy(2 + buffer2 + j, &DATA[i], sizeof(*DATA));
            }
            const int buffer2Len = 2 + (size*sizeof(*DATA));

            bsl::vector<float> p1, p2;  // two default vectors

            my_InStream in1(buffer1, buffer1Len);  // without version
            ASSERT(in1);
            bdex_InStreamMethods<bsl::vector<float> >::bdexStreamIn(
                                                                   in1, p1, 0);
            ASSERT(in1);
            LOOP2_ASSERT(size, p1.size(), size == p1.size());
            for (int i = 0; i < size; ++i) {
                LOOP3_ASSERT(size, i, p1[i], DATA[i] == p1[i]);
            }

            my_InStream in2(buffer2, buffer2Len);  // with version
            ASSERT(in2);
            in2 >> p2;
            ASSERT(in2);
            LOOP2_ASSERT(size, p2.size(), size == p2.size());
            for (int i = 0; i < size; ++i) {
                LOOP3_ASSERT(size, i, p2[i], DATA[i] == p2[i]);
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING bsls_PlatformUtil::Int64 VECTOR STREAM
        //
        // Plan:
        //
        // Testing:
        //   bdex_InStreamMethods<bsl::vector<Int64, ALLOC> >
        // --------------------------------------------------------------------

        const bsls_PlatformUtil::Int64 DATA[] = {  // any arbitrary data array
            7,
            5,
            432,
            8765,
            543,
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int size = 0; size <= NUM_DATA; ++size) {
            char buffer1[128];
            char buffer2[128];

            // buffer1 will contain length + elements
            buffer1[0] = size;
            for (int i = 0, j = 0; i < size; ++i, j += sizeof(*DATA)) {
                bsl::memcpy(1 + buffer1 + j, &DATA[i], sizeof(*DATA));
            }
            const int buffer1Len = 1 + (size*sizeof(*DATA));

            // buffer2 will contain version + length + elements
            buffer2[0] = 0;
            buffer2[1] = size;
            for (int i = 0, j = 0; i < size; ++i, j += sizeof(*DATA)) {
                bsl::memcpy(2 + buffer2 + j, &DATA[i], sizeof(*DATA));
            }
            const int buffer2Len = 2 + (size*sizeof(*DATA));

            bsl::vector<bsls_PlatformUtil::Int64> p1, p2;  // two default
                                                           // vectors

            my_InStream in1(buffer1, buffer1Len);  // without version
            ASSERT(in1);
            bdex_InStreamMethods<bsl::vector<bsls_PlatformUtil::Int64> >::
                                                      bdexStreamIn(in1, p1, 0);
            ASSERT(in1);
            LOOP2_ASSERT(size, p1.size(), size == p1.size());
            for (int i = 0; i < size; ++i) {
                LOOP3_ASSERT(size, i, p1[i], DATA[i] == p1[i]);
            }

            my_InStream in2(buffer2, buffer2Len);  // with version
            ASSERT(in2);
            in2 >> p2;
            ASSERT(in2);
            LOOP2_ASSERT(size, p2.size(), size == p2.size());
            for (int i = 0; i < size; ++i) {
                LOOP3_ASSERT(size, i, p2[i], DATA[i] == p2[i]);
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING int VECTOR STREAM
        //
        // Plan:
        //
        // Testing:
        //   bdex_InStreamMethods<bsl::vector<int, ALLOC> >
        // --------------------------------------------------------------------

        const int DATA[] = {  // any arbitrary data array
            7,
            5,
            432,
            8765,
            543,
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int size = 0; size <= NUM_DATA; ++size) {
            char buffer1[128];
            char buffer2[128];

            // buffer1 will contain length + elements
            buffer1[0] = size;
            for (int i = 0, j = 0; i < size; ++i, j += 4) {
                buffer1[1 + j    ] = (DATA[i] >> 24) & 0xFF;
                buffer1[1 + j + 1] = (DATA[i] >> 16) & 0xFF;
                buffer1[1 + j + 2] = (DATA[i] >>  8) & 0xFF;
                buffer1[1 + j + 3] = (DATA[i]      ) & 0xFF;
            }
            const int buffer1Len = 1 + (size*4);

            // buffer2 will contain version + length + elements
            buffer2[0] = 0;
            buffer2[1] = size;
            for (int i = 0, j = 0; i < size; ++i, j += 4) {
                buffer2[2 + j    ] = (DATA[i] >> 24) & 0xFF;
                buffer2[2 + j + 1] = (DATA[i] >> 16) & 0xFF;
                buffer2[2 + j + 2] = (DATA[i] >>  8) & 0xFF;
                buffer2[2 + j + 3] = (DATA[i]      ) & 0xFF;
            }
            const int buffer2Len = 2 + (size*4);

            bsl::vector<int> p1, p2;  // two default vectors

            my_InStream in1(buffer1, buffer1Len);  // without version
            ASSERT(in1);
            bdex_InStreamMethods<bsl::vector<int> >::bdexStreamIn(in1, p1, 0);
            ASSERT(in1);
            LOOP2_ASSERT(size, p1.size(), size == p1.size());
            for (int i = 0; i < size; ++i) {
                LOOP3_ASSERT(size, i, p1[i], DATA[i] == p1[i]);
            }

            my_InStream in2(buffer2, buffer2Len);  // with version
            ASSERT(in2);
            in2 >> p2;
            ASSERT(in2);
            LOOP2_ASSERT(size, p2.size(), size == p2.size());
            for (int i = 0; i < size; ++i) {
                LOOP3_ASSERT(size, i, p2[i], DATA[i] == p2[i]);
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING short VECTOR STREAM
        //
        // Plan:
        //
        // Testing:
        //   bdex_InStreamMethods<bsl::vector<short, ALLOC> >
        // --------------------------------------------------------------------

        const short DATA[] = {  // any arbitrary data array
            7,
            5,
            432,
            8765,
            543,
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int size = 0; size <= NUM_DATA; ++size) {
            char buffer1[128];
            char buffer2[128];

            // buffer1 will contain length + elements
            buffer1[0] = size;
            for (int i = 0, j = 0; i < size; ++i, j += 2) {
                buffer1[1 + j    ] = (DATA[i] >> 8) & 0xFF;
                buffer1[1 + j + 1] = (DATA[i]     ) & 0xFF;
            }
            const int buffer1Len = 1 + (size*2);

            // buffer2 will contain version + length + elements
            buffer2[0] = 0;
            buffer2[1] = size;
            for (int i = 0, j = 0; i < size; ++i, j += 2) {
                buffer2[2 + j    ] = (DATA[i] >> 8) & 0xFF;
                buffer2[2 + j + 1] = (DATA[i]     ) & 0xFF;
            }
            const int buffer2Len = 2 + (size*2);

            bsl::vector<short> p1, p2;  // two default vectors

            my_InStream in1(buffer1, buffer1Len);  // without version
            ASSERT(in1);
            bdex_InStreamMethods<bsl::vector<short> >::bdexStreamIn(
                                                                   in1, p1, 0);
            ASSERT(in1);
            LOOP2_ASSERT(size, p1.size(), size == p1.size());
            for (int i = 0; i < size; ++i) {
                LOOP3_ASSERT(size, i, p1[i], DATA[i] == p1[i]);
            }

            my_InStream in2(buffer2, buffer2Len);  // with version
            ASSERT(in2);
            in2 >> p2;
            ASSERT(in2);
            LOOP2_ASSERT(size, p2.size(), size == p2.size());
            for (int i = 0; i < size; ++i) {
                LOOP3_ASSERT(size, i, p2[i], DATA[i] == p2[i]);
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING char VECTOR STREAM
        //
        // Plan:
        //
        // Testing:
        //   bdex_InStreamMethods<bsl::vector<char, ALLOC> >
        // --------------------------------------------------------------------

        const char DATA[] = {  // any arbitrary data array
            'd',
            'a',
            't',
            'a',
            '!',
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int size = 0; size <= NUM_DATA; ++size) {
            char buffer1[128];
            char buffer2[128];

            // buffer1 will contain length + elements
            buffer1[0] = size;
            for (int i = 0; i < size; ++i) {
                buffer1[1 + i] = DATA[i];
            }
            const int buffer1Len = 1 + size;

            // buffer2 will contain version + length + elements
            buffer2[0] = 0;
            buffer2[1] = size;
            for (int i = 0; i < size; ++i) {
                buffer2[2 + i] = DATA[i];
            }
            const int buffer2Len = 2 + size;

            bsl::vector<char> p1, p2;  // two default vectors

            my_InStream in1(buffer1, buffer1Len);  // without version
            ASSERT(in1);
            bdex_InStreamMethods<bsl::vector<char> >::bdexStreamIn(in1, p1, 0);
            ASSERT(in1);
            LOOP2_ASSERT(size, p1.size(), size == p1.size());
            for (int i = 0; i < size; ++i) {
                LOOP3_ASSERT(size, i, p1[i], DATA[i] == p1[i]);
            }

            my_InStream in2(buffer2, buffer2Len);  // with version
            ASSERT(in2);
            in2 >> p2;
            ASSERT(in2);
            LOOP2_ASSERT(size, p2.size(), size == p2.size());
            for (int i = 0; i < size; ++i) {
                LOOP3_ASSERT(size, i, p2[i], DATA[i] == p2[i]);
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING GENERIC VECTOR STREAM
        //
        // Plan:
        //
        // Testing:
        //   bdex_InStreamMethods<bsl::vector<TYPE, ALLOC> >
        // --------------------------------------------------------------------

        const my_Color::Color DATA[] = {  // any arbitrary data array
            my_Color::BLUE,
            my_Color::RED,
            my_Color::GREEN,
            my_Color::BLUE,
            my_Color::RED,
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

#if 0 // TBD
        for (int size = 0; size <= NUM_DATA; ++size) {
            char buffer1[128];
            char buffer2[128];

            // buffer1 will contain length + elements
            buffer1[0] = size;
            for (int i = 0; i < size; ++i) {
                buffer1[1 + i] = DATA[i];
            }
            const int buffer1Len = 1 + size;

            // buffer2 will contain version + length + elements
            buffer2[0] = 1;
            buffer2[1] = size;
            for (int i = 0; i < size; ++i) {
                buffer2[2 + i] = DATA[i];
            }
            const int buffer2Len = 2 + size;

            bsl::vector<my_Color::Color> p1, p2;  // two default vectors

            my_InStream in1(buffer1, buffer1Len);  // without version
            ASSERT(in1);
            bdex_InStreamMethods<bsl::vector<my_Color::Color> >::bdexStreamIn(
                                                                   in1, p1, 1);
            ASSERT(in1);
            LOOP2_ASSERT(size, p1.size(), size == p1.size());
            for (int i = 0; i < size; ++i) {
                LOOP3_ASSERT(size, i, p1[i], DATA[i] == p1[i]);
            }

            my_InStream in2(buffer2, buffer2Len);  // with version
            ASSERT(in2);
            in2 >> p2;
            ASSERT(in2);
            LOOP2_ASSERT(size, p2.size(), size == p2.size());
            for (int i = 0; i < size; ++i) {
                LOOP3_ASSERT(size, i, p2[i], DATA[i] == p2[i]);
            }
        }
#endif // TBD
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage Example"
                          << "\n=====================" << endl;

        usageExample(argc, argv);

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
