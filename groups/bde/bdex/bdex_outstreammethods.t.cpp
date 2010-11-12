// bdex_outstreammethods.t.cpp         -*-C++-*-

#include <bdex_outstreammethods.h>

#include <bsls_platformutil.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script


//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This component provides a template class that has two static member
// functions, each of which invokes a method of its template parameter.  In
// addition, the component provides a number of specializations of this class
// for fundamental types and 'bsl::string'.  The testing requirements are
// fairly straightforward, and somewhat similar to the testing strategy for an
// abstract protocol.  It is sufficient to provide a test type and a test
// stream, each of which responds in a simple, observable manner when its
// various methods are called.
//-----------------------------------------------------------------------------
// template <typename TYPE> class bdex_OutStreamMethods
//      int maxSupportedBdexVersion()
//      template <typename STREAM>
//      STREAM& bdexStreamOut(STREAM& stream, const TYPE& value, int version)
//
// class bdex_OutStreamMethods<bsls_PlatformUtil::Int64>
//      int maxSupportedBdexVersion()
//      template <typename STREAM>
//      STREAM& bdexStreamOut(STREAM& stream, const Int64& value, int version)
//
// class bdex_OutStreamMethods<bsls_PlatformUtil::Uint64>
//      int maxSupportedBdexVersion()
//      template <typename STREAM>
//      STREAM& bdexStreamOut(STREAM& stream, const Uint64& value, int version)
//
// class bdex_OutStreamMethods<int>
//      int maxSupportedBdexVersion()
//      template <typename STREAM>
//      STREAM& bdexStreamOut(STREAM& stream, const int& value, int version)
//
// class bdex_OutStreamMethods<unsigned int>
//      int maxSupportedBdexVersion()
//      template <typename STREAM>
//      STREAM& bdexStreamOut(STREAM&, const unsigned int&, int)
//
// class bdex_OutStreamMethods<short>
//      int maxSupportedBdexVersion()
//      template <typename STREAM>
//      STREAM& bdexStreamOut(STREAM& stream, const short& value, int version)
//
// class bdex_OutStreamMethods<unsigned short>
//      int maxSupportedBdexVersion()
//      template <typename STREAM>
//      STREAM& bdexStreamOut(STREAM&, const unsigned short&, int version)
//
// class bdex_OutStreamMethods<bsl::string>
//      int maxSupportedBdexVersion()
//      template <typename STREAM>
//      STREAM& bdexStreamOut(STREAM&, const bsl::string&, int)
//
// class bdex_OutStreamMethods<char>
//      int maxSupportedBdexVersion()
//      template <typename STREAM>
//      STREAM& bdexStreamOut(STREAM& stream, const char& value, int version)
//
// class bdex_OutStreamMethods<signed char>
//      int maxSupportedBdexVersion()
//      template <typename STREAM>
//      STREAM& bdexStreamOut(STREAM&, const signed char&, int)
//
// class bdex_OutStreamMethods<unsigned char>
//      int maxSupportedBdexVersion()
//      template <typename STREAM>
//      STREAM& bdexStreamOut(STREAM&, const unsigned char&, int)
//
// class bdex_OutStreamMethods<double>
//      int maxSupportedBdexVersion()
//      template <typename STREAM>
//      STREAM& bdexStreamOut(STREAM& stream, const double& value, int version)
//
// class bdex_OutStreamMethods<float>
//      int maxSupportedBdexVersion()
//      template <typename STREAM>
//      STREAM& bdexStreamOut(STREAM& stream, const float& value, int version)
//-----------------------------------------------------------------------------
// [ 1] Usage Example
//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;
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
#define T_()  cout << "\t" << flush;          // Print a tab (w/o newline)

//=============================================================================
//                   GLOBAL CONSTANTS/TYPEDEFS FOR TESTING
//=============================================================================

//=============================================================================
//              Classes, functions, etc., for testing
//-----------------------------------------------------------------------------

namespace BloombergLP {

enum Shape {
  CIRCLE,
  RECTANGLE,
  SQUARE
};

const int shapeVersion = 999;

template <>
class bdex_OutStreamMethods<Shape> {
  public:
    static int maxSupportedBdexVersion() {
        return shapeVersion;
    }

    template <typename STREAM>
    static STREAM& bdexStreamOut(STREAM&      stream,
                                 const Shape& value,
                                 int          version)
    {
        stream.putVersion(shapeVersion);
        return stream;
    }
};

}


//=============================================================================
//              Classes, functions, etc., needed for Usage Example
//-----------------------------------------------------------------------------
// In this example we illustrate the primary intended use of the templatized
// methods of this component, as well as a few trivial invocations just to
// show the syntax clearly.  To accomplish this, we exhibit three
// separate example "components": an out-stream, a value-semantic point object,
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

struct my_Color {
    // This 'struct' provides a namespace for enumerating set of colors
    // appropriate for objects in the 'my' package.
//
    // TYPES
    enum Color {
        RED            =  0,
        GREEN          =  1,
        BLUE           =  2
    };
//
    enum {
        NUM_COLORS = BLUE + 1      // Number of colors
    };
//
  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number
        // supported by this class.
//
    static const char *toAscii(my_Color::Color value);
        // Return the string representation exactly matching the enumerator
        // name corresponding to the specified enumerator 'value'.
//
    template <class STREAM>
    static STREAM& bdexStreamIn(STREAM&          stream,
                                my_Color::Color& value,
                                int              version);
        // Assign to the specified 'value' the value read from the
        // specified input 'stream' using the specified 'version' format
        // and return a reference to the modifiable 'stream'.  ...
//
    static bsl::ostream& streamOut(bsl::ostream&   stream,
                                   my_Color::Color rhs);
        // Write the enumerator corresponding to the specified 'rhs' value
        // to the specified 'stream'.
//
    template <class STREAM>
    static STREAM& bdexStreamOut(STREAM&         stream,
                                 my_Color::Color value,
                                 int             version);
        // Write the specified 'value' to the specified output 'stream'
        // using the specified 'version' format and return a reference to
        // the modifiable 'stream'. ...
};
//
// FREE OPERATORS
inline
bsl::ostream& operator<<(bsl::ostream& stream, my_Color::Color rhs);
    // Write the enumerator corresponding to the specified 'rhs' value
    // to the specified 'stream'.
//..
// The (inline) implementations of selected functions are as follows.
//..
//                    INLINE FUNCTION DEFINITIONS
//
// CLASS METHODS
inline
int my_Color::maxSupportedBdexVersion()
{
    return 1;
}
//

inline
bsl::ostream& my_Color::streamOut(bsl::ostream&   stream,
                                  my_Color::Color rhs)
{
    stream << toAscii(rhs);
    return stream;
}

template <class STREAM>
inline
STREAM& my_Color::bdexStreamOut(STREAM&         stream,
                                my_Color::Color value,
                                int             version)
{
    switch (version) {
      case 1: {
        stream.putInt8((char) value);  // Write the value as a single byte.
      } break;
    }
    return stream;
}

namespace BloombergLP {

template <>
class bdex_OutStreamMethods<my_Color::Color> {
  public:
    static int maxSupportedBdexVersion() {
        return my_Color::maxSupportedBdexVersion();
    }

    template <typename STREAM>
    static STREAM& bdexStreamOut(STREAM&                stream,
                                 const my_Color::Color& value,
                                 int                    version)
    {
        return my_Color::bdexStreamOut(stream, value, version);
    }
};

}

//..
//
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
//  // ...
//  #ifndef INCLUDED_MY_COLOR
//  #include <my_color.h>
//  #endif
//
class my_Point {
    // This class provides a geometric point having integer coordinates and
    // an enumerated color property.
//
    short           d_x;      // x coordinate
    short           d_y;      // y coordinate
    my_Color::Color d_color;  // enumerated color property
//
  public:
  // CLASS METHODS
  static int maxSupportedBdexVersion();
      // Return the most current 'bdex' streaming version number supported
      // by this class.
//
  // CREATORS
  my_Point(int x, int y, my_Color::Color color);
      // Construct a point having the specified 'x' and 'y' coordinates
      // and the specified 'color'.
//
  ~my_Point();
      // Destroy this point.
//
  // MANIPULATORS
  // ...
//
  // ACCESSORS
  int x();
      // Return the x coordinate of this point.
//
  int y();
      // Return the y coordinate of this point.
//
  my_Color::Color color();
      // Return the enumerated color of this point.
//
  template <class STREAM>
  STREAM& bdexStreamOut(STREAM& stream, int version) const;
      // Write this value to the specified output 'stream' using the
      // specified 'version' format and return a reference to the
      // modifiable 'stream'.  If 'version' is not supported, 'stream' is
      // unmodified.  Note that 'version' is not written to 'stream'.
      // See the 'bdex' package-level documentation for more information
      // on 'bdex' streaming of value-semantic types and containers.
//
};
//..
// The (inline) implementations of these methods is as follows.
//..
//                INLINE FUNCTION DEFINITIONS
//
// CLASS METHODS
inline
int my_Point::maxSupportedBdexVersion()
{
    return 1;
}
//
// CREATORS
my_Point::my_Point(int x, int y, my_Color::Color color)
: d_x(x)
, d_y(y)
, d_color(color)
{ }
//
my_Point::~my_Point()
{ }
//
// MANIPULATORS
// ...
//
// ACCESSORS
int my_Point::x()
{
    return d_x;
}
//
int my_Point::y()
{
    return d_y;
}
//
my_Color::Color my_Point::color()
{
    return d_color;
}
//
template <class STREAM>
STREAM& my_Point::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        stream.putInt16(d_x);            // write the x coordinate
        stream.putInt16(d_y);            // write the y coordinate
        stream.putInt8((char) d_color);  // write the color enum as one byte
      } break;
    }
    return stream;
}
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
// my_outstream.h
// ...
//
class my_OutStream {
    // This class implements a limited-size fixed-buffer output stream that
    // conforms to the 'bdex' protocol for output streams.  This class is
    // suitable for demonstration purposes only.
//
    char d_buffer[1000]; // externalized values stored as contiguous bytes
    int  d_length;       // length of 'd_buffer' (bytes)
//
  public:
    // CREATORS
    my_OutStream();
        // Create an empty output stream of limited, fixed capacity.  Note
        // that this object is suitable for demonstration purposes only.
//
    ~my_OutStream();
       // Destroy this output byte stream.
//
    // MANIPULATORS
    my_OutStream& putLength(int length);
        // Format the specified non-negative 'length' to this output
        // stream and return a reference to this modifiable stream.
//
    my_OutStream& putVersion(int version);
        // Format the specified non-negative 'version' to this output
        // stream and return a reference to this modifiable stream.
//
    my_OutStream& putInt32(int value);
        // Format the least significant 32 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable
        // stream.
//
    my_OutStream& putInt16(int value);
        // Format the least significant 16 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable
        // stream.
//
    my_OutStream& putInt8(int value);
        // Format the least significant 8 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable
        // stream.
//
    my_OutStream& putFloat64(double value);
        // Format the most significant 64 bits in the specified 'value' to
        // this output stream and return a reference to this modifiable
        // stream.  ...

    my_OutStream& putArrayFloat64(const double *value, int length)
    {
        const int numBytes = length * sizeof(*value);
        bsl::memcpy(d_buffer + d_length, value, numBytes);
        d_length += numBytes;
        return *this;
    }

    my_OutStream& putArrayFloat32(const float *value, int length)
    {
        const int numBytes = length * sizeof(*value);
        bsl::memcpy(d_buffer + d_length, value, numBytes);
        d_length += numBytes;
        return *this;
    }

    my_OutStream& putArrayInt64(const bsls_PlatformUtil::Int64 *value,
                                int                             length)
    {
        const int numBytes = length * sizeof(*value);
        bsl::memcpy(d_buffer + d_length, value, numBytes);
        d_length += numBytes;
        return *this;
    }

    my_OutStream& putArrayInt32(const int *value, int length)
    {
        const int numBytes = length * sizeof(*value);
        bsl::memcpy(d_buffer + d_length, value, numBytes);
        d_length += numBytes;
        return *this;
    }

    my_OutStream& putArrayInt16(const short *value, int length)
    {
        const int numBytes = length * sizeof(*value);
        bsl::memcpy(d_buffer + d_length, value, numBytes);
        d_length += numBytes;
        return *this;
    }

    my_OutStream& putArrayInt8(const char *value, int length)
    {
        const int numBytes = length * sizeof(*value);
        bsl::memcpy(d_buffer + d_length, value, numBytes);
        d_length += numBytes;
        return *this;
    }
//
    void removeAll();
        // Remove all content in this stream.

    // ACCESSORS
    inline const char *data() const;
        // Return the address of the contiguous, non-modifiable internal
        // memory buffer of this stream.
//
    inline int length() const;
        // Return the number of bytes in this stream.
};
//
//   FREE OPERATORS
inline
bsl::ostream& operator<<(bsl::ostream&       stream,
                         const my_OutStream& object);
    // Write the specified 'object' to the specified output 'stream' in
    // some reasonable (multi-line) format, and return a reference to
    // 'stream'.
//
template <typename OBJECT>
inline
my_OutStream& operator<<(my_OutStream& stream, const OBJECT& object);
    // Write to the specified output 'stream', using the 'putVersion'
    // method of this component, the most current version number supported
    // by 'OBJECT' and format the value of the specified 'object' to
    // 'stream' using the 'bdex' compliant 'OBJECT::bdexStreamOut' method
    // in the 'OBJECT::maxSupportedBdexVersion' format.  Return a reference
    // to the modifiable 'stream'.  Note that specializations for the
    // supported fundamental (scalar) types and for 'bsl::string' are also
    // provided, in which case this operator calls the appropriate 'put'
    // method from this component.
//..
// The relevant (inline) implementations are as follows.
//..
//                INLINE FUNCTION DEFINITIONS

// CREATORS
inline
my_OutStream::my_OutStream()
: d_length(0)
{
}
//
inline
my_OutStream::~my_OutStream()
{
}
//
// MANIPULATORS
//
inline
my_OutStream& my_OutStream::putLength(int length)
{
    return putInt8(length);
 }
inline
my_OutStream& my_OutStream::putVersion(int value)
{
    unsigned char temp = value;
    memcpy(d_buffer + d_length, &temp, 1);
    ++d_length;
    return *this;
 }
//
inline
my_OutStream& my_OutStream::putInt32(int value)
{
    memcpy(d_buffer + d_length, (const char *) &value, 4);
    d_length += 4;
    return *this;
}
//
inline
my_OutStream& my_OutStream::putInt16(int value)
{
    unsigned short temp = value;
    memcpy(d_buffer + d_length, (const char *) &temp, 2);
    d_length += 2;
    return *this;
}

inline
my_OutStream& my_OutStream::putInt8(int value)
{
    unsigned char temp = value;
    memcpy(d_buffer + d_length, &temp, 1);
    d_length += 1;  // Imp Note: using syntax parallel to other 'putInt'
    return *this;
}
//
inline
my_OutStream& my_OutStream::putFloat64(double value)
{
    const char *tempPtr = (const char *) &value;
    memcpy(d_buffer + d_length, &tempPtr, 8);
    d_length += 8;
    return *this;
}

inline
void my_OutStream::removeAll()
{
    d_length = 0;
}
//
//
// ACCESSORS
inline
const char *my_OutStream::data() const
{
    return (const char *) d_buffer;
}

inline
int my_OutStream::length() const
{
    return d_length;
}

// FREE OPERATORS
inline
bsl::ostream& operator<<(bsl::ostream&       stream,
                         const my_OutStream& object)
{
    const char *hex = "0123456789abcdef";
    const char *data   = object.data();
    int         length = object.length();

    stream << "[ ";
    for (int i = 0; i < length; ++i) {
        stream << hex[(unsigned char) data[i] >> 4]
               << hex[(unsigned char) data[i] & 0xf]
               << ' ';
    }
    stream << ']';

    return stream;
}

template <typename OBJECT>
inline
my_OutStream& operator<<(my_OutStream& stream, const OBJECT& object)
{
    const int version =
                  bdex_OutStreamMethods<OBJECT>::maxSupportedBdexVersion();
    stream.putVersion(version);
    return bdex_OutStreamMethods<OBJECT>::bdexStreamOut(stream,
                                                        object,
                                                        version);
}

                        // *** integer values ***

template <>
inline
my_OutStream& operator<<(my_OutStream& stream, const int& value)
{
    return stream.putInt32(value);
}

template <>
inline
my_OutStream& operator<<(my_OutStream& stream, const short& value)
{
    return stream.putInt16(value);
}

template <>
inline
my_OutStream& operator<<(my_OutStream& stream, const char& value)
{
    return stream.putInt8(value);
}

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
//       case 8: {
//         // -----------------------------------------------------------------
//         // TESTING float VECTOR STREAM
//         //
//         // Plan:
//         //
//         // Testing:
//         //   bdex_OutStreamMethods<bsl::vector<float, ALLOC> >
//         // -----------------------------------------------------------------

//         const double DATA[] = {  // any arbitrary data array
//             7.45,
//             5.24,
//             432.43544,
//             8765.32,
//             543.0,
//         };
//         const int NUM_DATA = sizeof DATA / sizeof *DATA;

//         for (int size = 0; size <= NUM_DATA; ++size) {
//             bsl::vector<double> vect;  // source vector

//             for (int i = 0; i < size; ++i) {
//                 vect.push_back(DATA[i]);
//             }

//             my_OutStream out1, out2;

//             // no version
//             bdex_OutStreamMethods<bsl::vector<double> >::bdexStreamOut(out1,
//                                                                        vect,
//                                                                        0);

//             // with version
//             out2 << vect;

//             char buffer1[128];  // expected buffer for out1
//             char buffer2[128];  // expected buffer for out2

//             // buffer1 will contain length + elements
//             buffer1[0] = size;
//             bsl::memcpy(buffer1 + 1, DATA, size*sizeof(*DATA));
//             const int buffer1Len = 1 + (size*sizeof(*DATA));

//             // buffer2 will contain version + length + elements
//             buffer2[0] = 0;
//             buffer2[1] = size;
//             bsl::memcpy(buffer2 + 2, DATA, size*sizeof(*DATA));
//             const int buffer2Len = 2 + (size*sizeof(*DATA));

//             LOOP3_ASSERT(size, out1.length(),   buffer1Len,
//                                out1.length() == buffer1Len);
//             LOOP_ASSERT(size,
//                         0 == bsl::memcmp(buffer1,
//                                          out1.data(),
//                                          out1.length()));

//             LOOP3_ASSERT(size, out2.length(),   buffer2Len,
//                                out2.length() == buffer2Len);
//             LOOP_ASSERT(size,
//                         0 == bsl::memcmp(buffer2,
//                                          out2.data(),
//                                          out2.length()));
//         }
//       } break;
//       case 7: {
//         // -----------------------------------------------------------------
//         // TESTING float VECTOR STREAM
//         //
//         // Plan:
//         //
//         // Testing:
//         //   bdex_OutStreamMethods<bsl::vector<float, ALLOC> >
//         // -----------------------------------------------------------------

//         const float DATA[] = {  // any arbitrary data array
//             7.45f,
//             5.24f,
//             432.43544f,
//             8765.32f,
//             543.0f,
//         };
//         const int NUM_DATA = sizeof DATA / sizeof *DATA;

//         for (int size = 0; size <= NUM_DATA; ++size) {
//             bsl::vector<float> vect;  // source vector

//             for (int i = 0; i < size; ++i) {
//                 vect.push_back(DATA[i]);
//             }

//             my_OutStream out1, out2;

//             // no version
//             bdex_OutStreamMethods<bsl::vector<float> >::bdexStreamOut(out1,
//                                                                       vect,
//                                                                       0);

//             // with version
//             out2 << vect;

//             char buffer1[128];  // expected buffer for out1
//             char buffer2[128];  // expected buffer for out2

//             // buffer1 will contain length + elements
//             buffer1[0] = size;
//             bsl::memcpy(buffer1 + 1, DATA, size*sizeof(*DATA));
//             const int buffer1Len = 1 + (size*sizeof(*DATA));

//             // buffer2 will contain version + length + elements
//             buffer2[0] = 0;
//             buffer2[1] = size;
//             bsl::memcpy(buffer2 + 2, DATA, size*sizeof(*DATA));
//             const int buffer2Len = 2 + (size*sizeof(*DATA));

//             LOOP3_ASSERT(size, out1.length(),   buffer1Len,
//                                out1.length() == buffer1Len);
//             LOOP_ASSERT(size,
//                         0 == bsl::memcmp(buffer1,
//                                          out1.data(),
//                                          out1.length()));

//             LOOP3_ASSERT(size, out2.length(),   buffer2Len,
//                                out2.length() == buffer2Len);
//             LOOP_ASSERT(size,
//                         0 == bsl::memcmp(buffer2,
//                                          out2.data(),
//                                          out2.length()));
//         }
//       } break;
//       case 6: {
//         // -----------------------------------------------------------------
//         // TESTING bsls_PlatformUtil::Int64 VECTOR STREAM
//         //
//         // Plan:
//         //
//         // Testing:
//         //   bdex_OutStreamMethods<bsl::vector<Int64, ALLOC> >
//         // -----------------------------------------------------------------

//         const bsls_PlatformUtil::Int64 DATA[] = {
//             // any arbitrary data array
//             7,
//             5,
//             432,
//             8765,
//             543,
//         };
//         const int NUM_DATA = sizeof DATA / sizeof *DATA;

//         for (int size = 0; size <= NUM_DATA; ++size) {
//             bsl::vector<bsls_PlatformUtil::Int64> vect;  // source vector

//             for (int i = 0; i < size; ++i) {
//                 vect.push_back(DATA[i]);
//             }

//             my_OutStream out1, out2;

//             // no version
//             bdex_OutStreamMethods<bsl::vector<bsls_PlatformUtil::Int64> >::
//                                                bdexStreamOut(out1, vect, 0);

//             // with version
//             out2 << vect;

//             char buffer1[128];  // expected buffer for out1
//             char buffer2[128];  // expected buffer for out2

//             // buffer1 will contain length + elements
//             buffer1[0] = size;
//             bsl::memcpy(buffer1 + 1, DATA, size*sizeof(*DATA));
//             const int buffer1Len = 1 + (size*sizeof(*DATA));

//             // buffer2 will contain version + length + elements
//             buffer2[0] = 0;
//             buffer2[1] = size;
//             bsl::memcpy(buffer2 + 2, DATA, size*sizeof(*DATA));
//             const int buffer2Len = 2 + (size*sizeof(*DATA));

//             LOOP3_ASSERT(size, out1.length(),   buffer1Len,
//                                out1.length() == buffer1Len);
//             LOOP_ASSERT(size,
//                         0 == bsl::memcmp(buffer1,
//                                          out1.data(),
//                                          out1.length()));

//             LOOP3_ASSERT(size, out2.length(),   buffer2Len,
//                                out2.length() == buffer2Len);
//             LOOP_ASSERT(size,
//                         0 == bsl::memcmp(buffer2,
//                                          out2.data(),
//                                          out2.length()));
//         }
//       } break;
//       case 5: {
//         // -----------------------------------------------------------------
//         // TESTING int VECTOR STREAM
//         //
//         // Plan:
//         //
//         // Testing:
//         //   bdex_OutStreamMethods<bsl::vector<int, ALLOC> >
//         // -----------------------------------------------------------------

//         const int DATA[] = {  // any arbitrary data array
//             7,
//             5,
//             432,
//             8765,
//             543,
//         };
//         const int NUM_DATA = sizeof DATA / sizeof *DATA;

//         for (int size = 0; size <= NUM_DATA; ++size) {
//             bsl::vector<int> vect;  // source vector

//             for (int i = 0; i < size; ++i) {
//                 vect.push_back(DATA[i]);
//             }

//             my_OutStream out1, out2;

//             // no version
//             bdex_OutStreamMethods<bsl::vector<int> >::bdexStreamOut(out1,
//                                                                     vect,
//                                                                     0);

//             // with version
//             out2 << vect;

//             char buffer1[128];  // expected buffer for out1
//             char buffer2[128];  // expected buffer for out2

//             // buffer1 will contain length + elements
//             buffer1[0] = size;
//             bsl::memcpy(buffer1 + 1, DATA, size*sizeof(*DATA));
//             const int buffer1Len = 1 + (size*sizeof(*DATA));

//             // buffer2 will contain version + length + elements
//             buffer2[0] = 0;
//             buffer2[1] = size;
//             bsl::memcpy(buffer2 + 2, DATA, size*sizeof(*DATA));
//             const int buffer2Len = 2 + (size*sizeof(*DATA));

//             LOOP3_ASSERT(size, out1.length(),   buffer1Len,
//                                out1.length() == buffer1Len);
//             LOOP_ASSERT(size,
//                         0 == bsl::memcmp(buffer1,
//                                          out1.data(),
//                                          out1.length()));

//             LOOP3_ASSERT(size, out2.length(),   buffer2Len,
//                                out2.length() == buffer2Len);
//             LOOP_ASSERT(size,
//                         0 == bsl::memcmp(buffer2,
//                                          out2.data(),
//                                          out2.length()));
//         }
//       } break;
//       case 4: {
//         // -----------------------------------------------------------------
//         // TESTING short VECTOR STREAM
//         //
//         // Plan:
//         //
//         // Testing:
//         //   bdex_OutStreamMethods<bsl::vector<short, ALLOC> >
//         // -----------------------------------------------------------------

//         const short DATA[] = {  // any arbitrary data array
//             7,
//             5,
//             432,
//             8765,
//             543,
//         };
//         const int NUM_DATA = sizeof DATA / sizeof *DATA;

//         for (int size = 0; size <= NUM_DATA; ++size) {
//             bsl::vector<short> vect;  // source vector

//             for (int i = 0; i < size; ++i) {
//                 vect.push_back(DATA[i]);
//             }

//             my_OutStream out1, out2;

//             // no version
//             bdex_OutStreamMethods<bsl::vector<short> >:: bdexStreamOut(out1,
//                                                                        vect,
//                                                                        0);
//             // with version
//             out2 << vect;

//             char buffer1[128];  // expected buffer for out1
//             char buffer2[128];  // expected buffer for out2

//             // buffer1 will contain length + elements
//             buffer1[0] = size;
//             bsl::memcpy(buffer1 + 1, DATA, size*sizeof(*DATA));
//             const int buffer1Len = 1 + (size*sizeof(*DATA));

//             // buffer2 will contain version + length + elements
//             buffer2[0] = 0;
//             buffer2[1] = size;
//             bsl::memcpy(buffer2 + 2, DATA, size*sizeof(*DATA));
//             const int buffer2Len = 2 + (size*sizeof(*DATA));

//             LOOP3_ASSERT(size, out1.length(),   buffer1Len,
//                                out1.length() == buffer1Len);
//             LOOP_ASSERT(size,
//                         0 == bsl::memcmp(buffer1,
//                                          out1.data(),
//                                          out1.length()));

//             LOOP3_ASSERT(size, out2.length(),   buffer2Len,
//                                out2.length() == buffer2Len);
//             LOOP_ASSERT(size,
//                         0 == bsl::memcmp(buffer2,
//                                          out2.data(),
//                                          out2.length()));
//         }
//       } break;
//       case 3: {
//         // -----------------------------------------------------------------
//         // TESTING char VECTOR STREAM
//         //
//         // Plan:
//         //
//         // Testing:
//         //   bdex_OutStreamMethods<bsl::vector<char, ALLOC> >
//         // -----------------------------------------------------------------

//         const char DATA[] = {  // any arbitrary data array
//             'd',
//             'a',
//             't',
//             'a',
//             '!',
//         };
//         const int NUM_DATA = sizeof DATA / sizeof *DATA;

//         for (int size = 0; size <= NUM_DATA; ++size) {
//             bsl::vector<char> vect;  // source vector

//             for (int i = 0; i < size; ++i) {
//                 vect.push_back(DATA[i]);
//             }

//             my_OutStream out1, out2;

//             // no version
//             bdex_OutStreamMethods<bsl::vector<char> >::bdexStreamOut(out1,
//                                                                      vect,
//                                                                      0);

//             // with version
//             out2 << vect;

//             char buffer1[128];  // expected buffer for out1
//             char buffer2[128];  // expected buffer for out2

//             // buffer1 will contain length + elements
//             buffer1[0] = size;
//             for (int i = 0; i < size; ++i) {
//                 buffer1[1 + i] = DATA[i];
//             }
//             const int buffer1Len = 1 + size;

//             // buffer2 will contain version + length + elements
//             buffer2[0] = 0;
//             buffer2[1] = size;
//             for (int i = 0; i < size; ++i) {
//                 buffer2[2 + i] = DATA[i];
//             }
//             const int buffer2Len = 2 + size;

//             LOOP3_ASSERT(size, out1.length(),   buffer1Len,
//                                out1.length() == buffer1Len);
//             LOOP_ASSERT(size,
//                         0 == bsl::memcmp(buffer1,
//                                          out1.data(),
//                                          out1.length()));

//             LOOP3_ASSERT(size, out2.length(),   buffer2Len,
//                                out2.length() == buffer2Len);
//             LOOP_ASSERT(size,
//                         0 == bsl::memcmp(buffer2,
//                                          out2.data(),
//                                          out2.length()));
//         }
//       } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING bdex_OutStreamMethodsUtil
        //
        // Plan:
        //
        // Testing:
        //   bdex_OutStreamMethods<bsl::vector<TYPE, ALLOC> >
        // --------------------------------------------------------------------

        Shape shape = SQUARE;
        my_OutStream out;

        bdex_OutStreamMethodsUtil::streamOutVersionAndObject(out, shape);

      } break;

      case 2: {
        // --------------------------------------------------------------------
        // TESTING GENERIC VECTOR STREAM
        //
        // Plan:
        //
        // Testing:
        //   bdex_OutStreamMethods<bsl::vector<TYPE, ALLOC> >
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
            bsl::vector<my_Color::Color> vect;  // source vector

            for (int i = 0; i < size; ++i) {
                vect.push_back(DATA[i]);
            }

            my_OutStream out1, out2;

            // no version
            bdex_OutStreamMethods<bsl::vector<my_Color::Color> >::
                                                  bdexStreamOut(out1, vect, 1);

            // with version
            out2 << vect;

            char buffer1[128];  // expected buffer for out1
            char buffer2[128];  // expected buffer for out2

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

            LOOP3_ASSERT(size, out1.length(),   buffer1Len,
                               out1.length() == buffer1Len);
            LOOP_ASSERT(size,
                        0 == bsl::memcmp(buffer1, out1.data(), out1.length()));

            LOOP3_ASSERT(size, out2.length(),   buffer2Len,
                               out2.length() == buffer2Len);
            LOOP_ASSERT(size,
                        0 == bsl::memcmp(buffer2, out2.data(), out2.length()));
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

// // my_testapp.m.cpp
//
// using namespace bsl;
//
// int main(int argc, char **argv)
   {
       ASSERT(1 == bdex_OutStreamMethods<my_Point>::maxSupportedBdexVersion());
       ASSERT(-1 == bdex_OutStreamMethods<int>::maxSupportedBdexVersion());
       ASSERT(-1 == bdex_OutStreamMethods<double>::maxSupportedBdexVersion());

       int             i = 168496141;           // byte pattern 0a 0b 0c 0d
       my_Color::Color color = my_Color::BLUE;  // byte pattern 02
       my_Point        p(0, -1, color);         // byte pattern 00 00 ff ff 02

       my_OutStream    out;                ASSERT( 0 == out.length());

       bdex_OutStreamMethods<int>::bdexStreamOut(out, i, 1);
                                           ASSERT( 4 == out.length());
       if (verbose) cout << out << endl;
       out.removeAll();                    ASSERT( 0 == out.length());
       out << i;                           ASSERT( 4 == out.length());
       if (verbose) cout << out << endl << endl;

       out.removeAll();                    ASSERT( 0 == out.length());
       bdex_OutStreamMethods<my_Color::Color>::bdexStreamOut(out, color, 1);
                                           ASSERT( 1 == out.length());
       if (verbose) cout << out << endl;
       out.removeAll();                    ASSERT( 0 == out.length());
       out << color;                       ASSERT( 2 == out.length());
       if (verbose) cout << out << endl << endl;

       out.removeAll();                    ASSERT( 0 == out.length());
       bdex_OutStreamMethods<my_Point>::bdexStreamOut(out, p, 1);
                                           ASSERT( 5 == out.length());
       if (verbose) cout << out << endl;
       out.removeAll();                    ASSERT( 0 == out.length());
       out << p;                           ASSERT( 6 == out.length());
       if (verbose) cout << out << endl << endl;

       if (verbose) cout << "Calling 'enum' static methods from object"
                         << endl;

//     return;
    }

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
