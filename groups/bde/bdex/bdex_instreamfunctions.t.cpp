// bdex_instreamfunctions.t.cpp          -*-C++-*-

#include <bdex_instreamfunctions.h>

#include <bsls_platform.h>           // for testing only
#include <bsls_platformutil.h>

#include <bsl_cstdlib.h>     // atoi()
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
// template <typename TYPE> class bdex_InStreamFunctions
//      template <typename STREAM>
//      STREAM& bdexStreamIn(STREAM& stream, TYPE& object, int version)
//
//  class bdex_InStreamFunctions<bsls_PlatformUtil::Int64>
//      template <typename STREAM>
//      STREAM& bdexStreamIn(STREAM& stream, Int64& variable, int version)
//
//  class bdex_InStreamFunctions<bsls_PlatformUtil::Uint64>
//      template <typename STREAM>
//      STREAM& bdexStreamIn(STREAM& stream, Uint64& variable, int version)
//
//  class bdex_InStreamFunctions<int>
//      template <typename STREAM>
//      STREAM& bdexStreamIn(STREAM& stream, int& variable, int version)
//
//  class bdex_InStreamFunctions<unsigned int>
//      template <typename STREAM>
//      STREAM& bdexStreamIn(STREAM&, unsigned int&, int)
//
//  class bdex_InStreamFunctions<short>
//      template <typename STREAM>
//      STREAM& bdexStreamIn(STREAM& stream, short& variable, int version)
//
//  class bdex_InStreamFunctions<unsigned short>
//      template <typename STREAM>
//      STREAM& bdexStreamIn(STREAM&, unsigned short&, int)
//
//  class bdex_InStreamFunctions<bsl::string>
//      template <typename STREAM>
//      STREAM& bdexStreamIn(STREAM&, bsl::string&, int)
//
//  class bdex_InStreamFunctions<char>
//      template <typename STREAM>
//      STREAM& bdexStreamIn(STREAM& stream, char& variable, int version)
//
//  class bdex_InStreamFunctions<signed char>
//      template <typename STREAM>
//      STREAM& bdexStreamIn(STREAM&, signed char&, int)
//
//  class bdex_InStreamFunctions<unsigned char>
//      template <typename STREAM>
//
//  class bdex_InStreamFunctions<double>
//      template <typename STREAM>
//      STREAM& bdexStreamIn(STREAM& stream, double& variable, int version)
//
//  class bdex_InStreamFunctions<float>
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

typedef bsls_PlatformUtil::Int64  Int64;
typedef bsls_PlatformUtil::Uint64 Uint64;

//=============================================================================
//              Classes, functions, etc., needed for Usage Example
//-----------------------------------------------------------------------------
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
 enum Color {
     RED            =  0,
     GREEN          =  1,
     BLUE           =  2
 };
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
 // my_point.h

 class my_Point {
     // This class provides a geometric point having integer coordinates and
     // an enumerated color property.

     short d_x;      // x coordinate
     short d_y;      // y coordinate
     Color d_color;  // enumerated color property

   public:
   // CLASS METHODS
   static int maxSupportedBdexVersion();
       // Return the most current 'bdex' streaming version number supported
       // by this class.

   // CREATORS
   my_Point();
       // Create a valid default point.

   my_Point(int x, int y, Color color);
       // Create a point having the specified 'x' and 'y' coordinates
       // and the specified 'color'.

   ~my_Point();
       // Destroy this point.

   // MANIPULATORS
   // ...

   // ACCESSORS
   int x() const;
       // Return the x coordinate of this point.

   int y() const;
       // Return the y coordinate of this point.

   Color color() const;
       // Return the enumerated color of this point.

   template <class STREAM>
   STREAM& bdexStreamIn(STREAM& stream, int version);
       // Write this value to the specified input 'stream' using the
       // specified 'version' format and return a reference to the
       // modifiable 'stream'.  ...
 };

 // FREE OPERATORS
 inline bool operator==(const my_Point& lhs, const my_Point& rhs);
     // Return 'true' if the specified 'lhs' and 'rhs' points have the same
     // value and 'false' otherwise.  Two points have the same value if they
     // have the same x and y coordinates and the same color.

//..
// Representative (inline) implementations of these methods are shown below.
//..
 //                  INLINE FUNCTION DEFINITIONS

 // CLASS METHODS
 inline
 int my_Point::maxSupportedBdexVersion()
 {
     return 1;
 }

 // CREATORS
 inline
 my_Point::my_Point()
 {
 }

 inline
 my_Point::my_Point(int x, int y, Color color)
 : d_x(x)
 , d_y(y)
 , d_color(color)
 {
 }

 inline
 my_Point::~my_Point()
 {
 }

 // ...

 // MANIPULATORS
 // ...

 // ACCESSORS
 inline
 Color my_Point::color() const
 {
     return d_color;
 }

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

 // ...

 template <class STREAM>
 inline
 STREAM& my_Point::bdexStreamIn(STREAM& stream, int version)
 {
     switch (version) {
       case 1: {
         stream.getInt16(d_x);           // write the x coordinate
         stream.getInt16(d_y);           // write the y coordinate
         char color;
         stream.getInt8(color);          // write the color enum as one byte
         d_color = (Color) color;
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

     bool isEmpty() const;
         // Return 'true' if this stream is empty, and 'false' otherwise.
         // The behavior is undefined unless this stream is valid (i.e.,
         // would convert to a non-zero value via the
         // 'operator const void *()' member).  Note that this function
         // enables higher-level components to verify that, after
         // successfully reading all expected data, no data remains.

     int length() const;
         // Return the total number of bytes stored in this stream.
 };

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
 bool my_InStream::isEmpty() const
 {
     return d_cursor >= d_length;
 }

 inline
 int my_InStream::length() const
 {
     return d_length;
 }

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


//=============================================================================
//                      GLOBAL TEST CLASSES
//-----------------------------------------------------------------------------


class my_TestInStream {
    // Test class used to test streaming.

    mutable int d_fun;  // holds code describing function:
                //   - Negative implies signed, positive unsigned (e.g., -32
                //     is a signed 32-bit integer, and +16 is an unsigned 16
                //     bit integer).
                //   - The 3rd digit is flag for floating point (e.g., 164 is a
                //     double, and 132 is a float).
                //   - Over 1000 means an arrayed function (e.g., 1164 is an
                //     array of doubles and -1008 is an array of signed char.
                //     Note that -1001 and 1001 correspond to arrays of char
                //     output as signed and unsigned char values, respectively.
                //   - Misc manipulator functions, when code is a two digit
                //     number with first digit 9.
                //   - Misc accessor functions, when code is a two digit
                //     number with first digit 8.
    bool        d_versionFlag; // Flag indicating if the 'putVersion' function
                               // was called.

  public:
    // CREATORS
    my_TestInStream()
      : d_fun(0)
      , d_versionFlag(false)
    {
    }

    ~my_TestInStream() { }

    my_TestInStream& getLength(int& length)
    {
        d_fun =  10;
        length = 1;
        return *this;
    }

    my_TestInStream& getVersion(int version) { d_versionFlag = true;
                                               return *this; }


    typedef bsls_PlatformUtil::Uint64 Uint64;

    my_TestInStream& getInt64(Int64 &value)   { d_fun = -64; return *this; }
    my_TestInStream& getUint64(Uint64 &value) { d_fun = +64; return *this; }
    my_TestInStream& getInt56(Int64 &value)   { d_fun = -56; return *this; }
    my_TestInStream& getUint56(Int64 &value)  { d_fun = +56; return *this; }
    my_TestInStream& getInt48(Int64 &value)   { d_fun = -48; return *this; }
    my_TestInStream& getUint48(Int64 &value)  { d_fun = +48; return *this; }
    my_TestInStream& getInt40(Int64 &value)   { d_fun = -40; return *this; }
    my_TestInStream& getUint40(Int64 &value)  { d_fun = +40; return *this; }
    my_TestInStream& getInt32(int &value)     { d_fun = -32; return *this; }
    my_TestInStream& getUint32(unsigned int &value)
                                              { d_fun = +32; return *this; }
    my_TestInStream& getInt24(int &value)     { d_fun = -24; return *this; }
    my_TestInStream& getUint24(int &value)    { d_fun = +24; return *this; }
    my_TestInStream& getInt16(short &value)   { d_fun = -16; return *this; }
    my_TestInStream& getUint16(unsigned short &value)
                                              { d_fun = +16; return *this; }
    my_TestInStream& getInt8(char &value)     { d_fun =  -8; return *this; }
    my_TestInStream& getInt8(signed char &value)
                                              { d_fun =  -8; return *this; }
    my_TestInStream& getUint8(unsigned char &value)
                                              { d_fun =  +8; return *this; }
    my_TestInStream& getFloat64(double &value){ d_fun = 164; return *this; }
    my_TestInStream& getFloat32(float &value) { d_fun = 132; return *this; }

    my_TestInStream& getArrayInt64(Int64 *array, int count)
                                            { d_fun = -1064; return *this; }
    my_TestInStream& getArrayUint64(Uint64 *array, int count)
                                            { d_fun = +1064; return *this; }
    my_TestInStream& getArrayInt56(Int64 *array, int count)
                                            { d_fun = -1056; return *this; }
    my_TestInStream& getArrayUint56(Uint64 *array, int count)
                                            { d_fun = +1056; return *this; }
    my_TestInStream& getArrayInt48(Int64 *array, int count)
                                            { d_fun = -1048; return *this; }
    my_TestInStream& getArrayUint48(Uint64 *array, int count)
                                            { d_fun = +1048; return *this; }
    my_TestInStream& getArrayInt40(Int64 *array, int count)
                                            { d_fun = -1040; return *this; }
    my_TestInStream& getArrayUint40(Uint64 *array, int count)
                                            { d_fun = +1040; return *this; }
    my_TestInStream& getArrayInt32(int *array, int count)
                                            { d_fun = -1032; return *this; }
    my_TestInStream& getArrayUint32(unsigned int *array, int count)
                                            { d_fun = +1032; return *this; }
    my_TestInStream& getArrayInt24(int *array, int count)
                                            { d_fun = -1024; return *this; }
    my_TestInStream& getArrayUint24(unsigned int *array, int count)
                                            { d_fun = +1024; return *this; }
    my_TestInStream& getArrayInt16(short *array, int count)
                                            { d_fun = -1016; return *this; }
    my_TestInStream& getArrayUint16(unsigned short *array, int count)
                                            { d_fun = +1016; return *this; }
    my_TestInStream& getArrayInt8(signed char *array, int count)
                                            { d_fun = -1008; return *this; }
    my_TestInStream& getArrayUint8(unsigned char *array, int count)
                                            { d_fun = +1008; return *this; }
    my_TestInStream& getArrayInt8(char *array, int count)
                                            { d_fun = -1001; return *this; }
    my_TestInStream& getArrayUint8(char *array, int count)
                                            { d_fun = +1001; return *this; }
    my_TestInStream& getArrayFloat64(double *array, int count)
                                            { d_fun = +1164; return *this; }
    my_TestInStream& getArrayFloat32(float *array, int count)
                                            { d_fun = +1132; return *this; }

    my_TestInStream& getString(bsl::string& value)
                                            { d_fun = +10000; return *this; }

    // MANIPULATORS
    void            invalidate()            { d_fun =  99; }
    void            removeAll()             { d_fun =  98; }
    void            reserveCapacity(int newCapacity)
                                            { d_fun =  97; }
    void            resetVersionFlag()      { d_versionFlag = false; }

    // ACCESSORS
    operator const void *() const           { return this; }
    const char     *data() const            { d_fun =  88; return 0; }
    int             length() const          { d_fun =  87; return 0; }

    int  fun() const { return d_fun; }
    bool versionFlag() const { return d_versionFlag; }
        // Return descriptive code for the function called.
};

enum my_TestEnum {
    EnumA = INT_MIN,
    EnumB = -1,
    EnumC = 0,
    EnumD = 1,
    EnumE = INT_MAX
};

class my_TestClass {
    // This test class is used for testing the streaming functionality.

  public:

    enum {
        VERSION = 999
    };

    // CREATORS
    my_TestClass() { }
    ~my_TestClass() { }

    // MANIPULATORS
    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);

    // ACCESSORS
    int maxSupportedBdexVersion() const { return VERSION; }
};

template <class STREAM>
STREAM& my_TestClass::bdexStreamIn(STREAM& stream, int version)
{
    stream.removeAll();
    return stream;
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
      case 2: {
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

 // my_testapp.m.cpp

//  int main(int argc, char *argv[])
//  {
     {
         const int  EXP       = 0x0A0B0C0D;
         const char buffer[4] = { 0xA, 0xB, 0xC, 0xD };  // int (no version)
         int        i         = 0;

         my_InStream in1(buffer, 4);  // use the one buffer
         bdex_InStreamFunctions::streamIn(in1, i, 1);
                                          ASSERT(in1);  ASSERT(EXP == i);

         i = 0;                       // reset 'i'
         my_InStream in2(buffer, 4);  // re-use 'buffer (no version)
         bdex_InStreamFunctions::streamIn(in2, i, 0);
                                          ASSERT(in2);  ASSERT(EXP == i);
     }

     {
         const my_Point EXP(0, -1, BLUE);
         const char buffer1[5] = { 0, 0, -1, -1, 2 };    //my_Point (no ver)
         const char buffer2[6] = { 1, 0, 0, -1, -1, 2 }; //version, my_Point
         my_Point p1, p2;  // two default points

         my_InStream in1(buffer1, 5);  // 'buffer1' has no version byte
         bdex_InStreamFunctions::streamIn(in1, p1, 1);
                                          ASSERT(in1);  ASSERT(EXP == p1);

         my_InStream in2(buffer2, 6);  // 'buffer2' *has* a version byte
         int version;
         in2.getVersion(version);         ASSERT(1 == version);
         bdex_InStreamFunctions::streamIn(in2, p2, version);
                                          ASSERT(in2);  ASSERT(EXP == p2);
     }
//
//      return 0;
//  }
//..

      } break;
#if 0
      case 8: {
        // --------------------------------------------------------------------
        // TESTING double VECTOR STREAM
        //
        // Plan:
        //
        // Testing:
        //   bdex_InStreamFunctions<bsl::vector<double, ALLOC> >
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
            bdex_InStreamFunctions::streamIn(in1, p1, 0);

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
        //   bdex_InStreamFunctions<bsl::vector<float, ALLOC> >
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
            bdex_InStreamFunctions::streamIn(in1, p1, 0);

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
        //   bdex_InStreamFunctions<bsl::vector<Int64, ALLOC> >
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
            bdex_InStreamFunctions::streamIn(in1, p1, 0);

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
        //   bdex_InStreamFunctions<bsl::vector<int, ALLOC> >
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
            bdex_InStreamFunctions::streamIn(in1, p1, 0);
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
        //   bdex_InStreamFunctions<bsl::vector<short, ALLOC> >
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
            bdex_InStreamFunctions::streamIn(in1, p1, 0);

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
        //   bdex_InStreamFunctions<bsl::vector<char, ALLOC> >
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
            bdex_InStreamFunctions::streamIn(in1, p1, 0);
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
        //   bdex_InStreamFunctions<bsl::vector<TYPE, ALLOC> >
        // --------------------------------------------------------------------

        const my_Color::Color DATA[] = {  // any arbitrary data array
            my_Color::BLUE,
            my_Color::RED,
            my_Color::GREEN,
            my_Color::BLUE,
            my_Color::RED,
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
            buffer2[0] = 1;
            buffer2[1] = size;
            for (int i = 0; i < size; ++i) {
                buffer2[2 + i] = DATA[i];
            }
            const int buffer2Len = 2 + size;

            bsl::vector<my_Color::Color> p1, p2;  // two default vectors

            my_InStream in1(buffer1, buffer1Len);  // without version
            ASSERT(in1);
            bdex_InStreamFunctions::streamIn(in1, p1, 1);

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
#endif
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Create 'bdex_ByteOutStream' objects using default and copy
        //   constructors.  Exercise these objects using some "put" methods,
        //   basic accessors, equality operators, and the assignment operator.
        //   Display object values frequently in verbose mode.
        //
        // Plan:
        //  Test the 'maxSupportedVersion', 'streamOut' and
        //  'streamOutVersionAndObject' functions for all fundamental types,
        //  enums and value-semantic objects.
        //
        // Testing:
        //   This "test" *exercises* basic functionality, but *tests* nothing.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        bool                        X0;

        char                        XA;
        signed char                 XB;
        unsigned char               XC;

        short                       XD;
        unsigned short              XE;

        int                         XF;
        unsigned int                XG;

        long                        XFA;
        unsigned long               XFB;

        Int64                       XH;
        Uint64                      XI;

        float                       XJ;
        double                      XK;

        bsl::string                 XL;
        my_TestClass                XM;

        bsl::vector<char>           XN;
        bsl::vector<unsigned char>  XNU;
        bsl::vector<signed char>    XNS;

        bsl::vector<my_TestClass>   XO;

        bsl::vector<short>          XQ;
        bsl::vector<unsigned short> XQU;

        bsl::vector<int>            XR;
        bsl::vector<unsigned int>   XRU;

        bsl::vector<Int64>          XS;
        bsl::vector<Uint64>         XSU;

        bsl::vector<float>          XT;
        bsl::vector<double>         XU;
        bsl::vector<bsl::string>    XV;

        my_TestEnum                 XZ;
        bsl::vector<my_TestEnum>    XXA;

        bsl::vector<bsl::vector<char> >           XNA;
        bsl::vector<bsl::vector<unsigned char> >  XNAU;
        bsl::vector<bsl::vector<signed char> >    XNAS;

        bsl::vector<bsl::vector<short> >          XQA;
        bsl::vector<bsl::vector<unsigned short> > XQAU;

        bsl::vector<bsl::vector<int> >            XRA;
        bsl::vector<bsl::vector<unsigned int> >   XRAU;

        if (verbose) cout << "\nTesting 'streamIn'" << endl;
        {
            using namespace bdex_InStreamFunctions;

            my_TestInStream stream;

            streamIn(stream, X0, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -8 == stream.fun());

            streamIn(stream, XA, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -8 == stream.fun());

            streamIn(stream, XB, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -8 == stream.fun());

            streamIn(stream, XC, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), +8 == stream.fun());

            streamIn(stream, XD, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -16 == stream.fun());

            streamIn(stream, XE, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), +16 == stream.fun());

            streamIn(stream, XF, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -32 == stream.fun());

            streamIn(stream, XG, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), +32 == stream.fun());

            streamIn(stream, XFA, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -32 == stream.fun());

            streamIn(stream, XFB, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), 32 == stream.fun());

            streamIn(stream, XH, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -64 == stream.fun());

            streamIn(stream, XI, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), +64 == stream.fun());

            streamIn(stream, XJ, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), 132 == stream.fun());

            streamIn(stream, XK, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), 164 == stream.fun());

            streamIn(stream, XL, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), 10000 == stream.fun());

            streamIn(stream, XM, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), 98 == stream.fun());

            streamIn(stream, XN, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -1001 == stream.fun());

            streamIn(stream, XNU, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), 8 == stream.fun());

            streamIn(stream, XNS, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -8 == stream.fun());

            streamIn(stream, XO, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), 98 == stream.fun());

            streamIn(stream, XQ, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -1016 == stream.fun());

            streamIn(stream, XQU, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), 16 == stream.fun());

            streamIn(stream, XR, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -1032 == stream.fun());

            streamIn(stream, XRU, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), 32 == stream.fun());

            streamIn(stream, XS, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -1064 == stream.fun());

            streamIn(stream, XSU, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), 64 == stream.fun());

            streamIn(stream, XT, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), +1132 == stream.fun());

            streamIn(stream, XU, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), +1164 == stream.fun());

            streamIn(stream, XV, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), 10000 == stream.fun());

            streamIn(stream, XZ, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -32 == stream.fun());

            streamIn(stream, XXA, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -32 == stream.fun());

            streamIn(stream, XNA, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -1001 == stream.fun());

            streamIn(stream, XNAU, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), 8 == stream.fun());

            streamIn(stream, XNAS, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -8 == stream.fun());

            streamIn(stream, XQA, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -1016 == stream.fun());

            streamIn(stream, XQAU, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), 16 == stream.fun());

            streamIn(stream, XRA, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -1032 == stream.fun());

            streamIn(stream, XRAU, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), 32 == stream.fun());
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
