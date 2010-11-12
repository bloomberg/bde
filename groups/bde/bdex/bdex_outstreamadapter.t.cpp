// bdex_outstreamadapter.t.cpp             -*-C++-*-

#include <bdex_outstreamadapter.h>

#include <bdex_byteinstream.h>         // for testing only
#include <bdex_byteoutstream.h>        // for testing only

#include <bdex_outstreamfunctions.h>   // for testing only
#include <bdex_instreamfunctions.h>    // for testing only

#include <bsls_platformutil.h>         // for testing only

#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
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

//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
        << J << "\n"; aSsErT(1, #X, __LINE__); } }


//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              OVERVIEW
// TBD DOC
//-----------------------------------------------------------------------------

// CREATORS
// [ 2] virtual bdex_OutStreamAdapter<STREAM>(STREAM *stream);
// [ 2] virtual ~bdex_OutStreamAdapter();
//
// MANIPULATORS
// [ 3] virtual void    invalidate();
// [ 3] virtual stream& putLength(int length);
// [ 3] virtual stream& putVersion(int version);
//
// [ 3] virtual stream& putInt64(Int64 value);
// [ 3] virtual stream& putUint64(Int64 value);
// [ 3] virtual stream& putInt56(Int64 value);
// [ 3] virtual stream& putUint56(Int64 value);
// [ 3] virtual stream& putInt48(Int64 value);
// [ 3] virtual stream& putUint48(Int64 value);
// [ 3] virtual stream& putInt40(Int64 value);
// [ 3] virtual stream& putUint40(Int64 value);
// [ 3] virtual stream& putInt32(int value);
// [ 3] virtual stream& putUint32(int value);
// [ 3] virtual stream& putInt24(int value);
// [ 3] virtual stream& putUint24(int value);
// [ 3] virtual stream& putInt16(int value);
// [ 3] virtual stream& putUint16(int value);
// [ 3] virtual stream& putInt8(int value);
// [ 3] virtual stream& putUint8(int value);
// [ 3] virtual stream& putFloat64(double value);
// [ 3] virtual stream& putFloat32(float value);
// [ 3] virtual stream& putString(const bsl::string& value);
// [ 3] virtual stream& putArrayInt64(const Int64 *a, int count);
// [ 3] virtual stream& putArrayUint64(const Uint64 *a, int count);
// [ 3] virtual stream& putArrayInt56(const Int64 *a, int count);
// [ 3] virtual stream& putArrayUint56(const Uint64 *a, int count);
// [ 3] virtual stream& putArrayInt48(const Int64 *a, int count);
// [ 3] virtual stream& putArrayUint48(const Uint64 *a, int count);
// [ 3] virtual stream& putArrayInt40(const Int64 *a, int count);
// [ 3] virtual stream& putArrayUint40(const Uint64 *a, int count);
// [ 3] virtual stream& putArrayInt32(const int *a, int count);
// [ 3] virtual stream& putArrayUint32(const unsigned int *a, int count);
// [ 3] virtual stream& putArrayInt24(const int *a, int count);
// [ 3] virtual stream& putArrayUint24(const unsigned int *a, int count);
// [ 3] virtual stream& putArrayInt16(const short *a, int count);
// [ 3] virtual stream& putArrayUint16(const unsigned short *a, int count);
// [ 3] virtual stream& putArrayInt8(const char *a, int count);
// [ 3] virtual stream& putArrayInt8(const signed char *a, int count);
// [ 3] virtual stream& putArrayUint8(const char *a, int count);
// [ 3] virtual stream& putArrayUint8(const unsigned char *a, int count);
// [ 3] virtual stream& putArrayFloat64(const double *a,int count);
// [ 3] virtual stream& putArrayFloat32(const float *a, int count);
// [ 3] virtual void    removeAll() = 0;
// [ 3] virtual void    reserveCapacity(int newCapacity) = 0;
//
// ACCESSORS
// [ 3] virtual operator const void *() const = 0;
// [ 3] virtual const char          *data() const = 0;
// [ 3] virtual int                  length() const = 0;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 4] USAGE EXAMPLE
//=============================================================================
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number



//==========================================================================
//                       GLOBAL CLASS FOR TESTING
//--------------------------------------------------------------------------

class MyOutStream {
    // Test class used to test the template instantiation.

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
                //   - Miscellaneous manipulator functions, when code is a two
                //     digit number with first digit 9.
                //   - Miscellaneous accessor functions, when code is a two
                //     digit number with first digit 8.
  public:
    MyOutStream() { }
    ~MyOutStream() { }

    typedef bsls_PlatformUtil::Int64 Int64;

    MyOutStream& putLength(int length)   { d_fun =  10; return *this; }
    MyOutStream& putVersion(int version) { d_fun =  11; return *this; }

    MyOutStream& putInt64(Int64 value)   { d_fun = -64; return *this; }
    MyOutStream& putUint64(Int64 value)  { d_fun = +64; return *this; }
    MyOutStream& putInt56(Int64 value)   { d_fun = -56; return *this; }
    MyOutStream& putUint56(Int64 value)  { d_fun = +56; return *this; }
    MyOutStream& putInt48(Int64 value)   { d_fun = -48; return *this; }
    MyOutStream& putUint48(Int64 value)  { d_fun = +48; return *this; }
    MyOutStream& putInt40(Int64 value)   { d_fun = -40; return *this; }
    MyOutStream& putUint40(Int64 value)  { d_fun = +40; return *this; }
    MyOutStream& putInt32(int value)     { d_fun = -32; return *this; }
    MyOutStream& putUint32(unsigned int value)
                                            { d_fun = +32; return *this; }
    MyOutStream& putInt24(int value)     { d_fun = -24; return *this; }
    MyOutStream& putUint24(int value)    { d_fun = +24; return *this; }
    MyOutStream& putInt16(int value)     { d_fun = -16; return *this; }
    MyOutStream& putUint16(int value)    { d_fun = +16; return *this; }
    MyOutStream& putInt8(int value)      { d_fun =  -8; return *this; }
    MyOutStream& putUint8(int value)     { d_fun =  +8; return *this; }
    MyOutStream& putFloat64(double value){ d_fun = 164; return *this; }
    MyOutStream& putFloat32(float value) { d_fun = 132; return *this; }

    typedef bsls_PlatformUtil::Uint64 Uint64;

    MyOutStream& putArrayInt64(const Int64 *array, int count)
                                            { d_fun = -1064; return *this; }
    MyOutStream& putArrayUint64(const Uint64 *array, int count)
                                            { d_fun = +1064; return *this; }
    MyOutStream& putArrayInt56(const Int64 *array, int count)
                                            { d_fun = -1056; return *this; }
    MyOutStream& putArrayUint56(const Uint64 *array, int count)
                                            { d_fun = +1056; return *this; }
    MyOutStream& putArrayInt48(const Int64 *array, int count)
                                            { d_fun = -1048; return *this; }
    MyOutStream& putArrayUint48(const Uint64 *array, int count)
                                            { d_fun = +1048; return *this; }
    MyOutStream& putArrayInt40(const Int64 *array, int count)
                                            { d_fun = -1040; return *this; }
    MyOutStream& putArrayUint40(const Uint64 *array, int count)
                                            { d_fun = +1040; return *this; }
    MyOutStream& putArrayInt32(const int *array, int count)
                                            { d_fun = -1032; return *this; }
    MyOutStream& putArrayUint32(const unsigned int *array, int count)
                                            { d_fun = +1032; return *this; }
    MyOutStream& putArrayInt24(const int *array, int count)
                                            { d_fun = -1024; return *this; }
    MyOutStream& putArrayUint24(const unsigned int *array, int count)
                                            { d_fun = +1024; return *this; }
    MyOutStream& putArrayInt16(const short *array, int count)
                                            { d_fun = -1016; return *this; }
    MyOutStream& putArrayUint16(const unsigned short *array, int count)
                                            { d_fun = +1016; return *this; }
    MyOutStream& putArrayInt8(const signed char *array, int count)
                                            { d_fun = -1008; return *this; }
    MyOutStream& putArrayUint8(const unsigned char *array, int count)
                                            { d_fun = +1008; return *this; }
    MyOutStream& putArrayInt8(const char *array, int count)
                                            { d_fun = -1001; return *this; }
    MyOutStream& putArrayUint8(const char *array, int count)
                                            { d_fun = +1001; return *this; }
    MyOutStream& putArrayFloat64(const double *array, int count)
                                            { d_fun = +1164; return *this; }
    MyOutStream& putArrayFloat32(const float *array, int count)
                                            { d_fun = +1132; return *this; }

    MyOutStream& putString(const bsl::string& value)
                                            { d_fun = +10000; return *this; }

    // MANIPULATORS
    void         invalidate()               { d_fun =  99; }
    void         removeAll()                { d_fun =  98; }
    void         reserveCapacity(int newCapacity)
                                            { d_fun =  97; }
    // ACCESSORS
    operator const void *() const           { d_fun =  89; return this; }
    const char          *data() const       { d_fun =  88; return 0; }
    int                  length() const     { d_fun =  87; return 0; }

    int fun() const { return d_fun; }
        // Return descriptive code for the function called.
};

//==========================================================================
//                       USAGE EXAMPLE HELPER CLASS
//--------------------------------------------------------------------------

class bdex_InStream_Test {
    // Test in stream protocol class.

  public:

    // CREATORS
    virtual ~bdex_InStream_Test() { }

    // MANIPULATORS
    virtual bdex_InStream_Test& getInt(int& value) = 0;
    virtual void invalidate() = 0;

    // ACCESSORS
    virtual operator const void *() const = 0;
};

template <class STREAM>
class bdex_InStream_TestAdapter : public bdex_InStream_Test {
    // Test in stream adapter class used for testing streaming in.

    STREAM *d_stream_p;  // underlying stream

  public:

    // CREATORS
    bdex_InStream_TestAdapter(STREAM *stream) : d_stream_p(stream) { }
    virtual ~bdex_InStream_TestAdapter() { }

    // MANIPULATORS
    virtual bdex_InStream_Test& getInt(int& value)
    {
        d_stream_p->getInt32(value);
        return *this;
    }

    virtual void invalidate() { }

    // ACCESSORS
    virtual operator const void *() const { return d_stream_p; }
};

//==========================================================================
//                       USAGE EXAMPLE
//--------------------------------------------------------------------------

// The 'bdex_OutStreamAdapter' output stream interface provided in this
// component is most commonly used to implement the stream-out functionality
// for a polymorphic type.  For example, consider the following 'myShape'
// protocol class:
//..
//  // myshape.h
//  #include <bdex_instreamadapter.h>
//  #include <bdex_outstreamadapter.h>
//
//  class bdex_OutStream;
//  class bdex_InStream_Test;

 class myShape {
     // This 'class' acts as a protocol class for displaying various shapes.

   public:
     // CREATORS
     virtual ~myShape();
         // Destroy this shape.

     // MANIPULATORS
     template <class STREAM>
     STREAM& bdexStreamIn(STREAM& stream, int version);
         // Assign to this object the value read from the specified input
         // 'stream' using the specified 'version' format and return a
         // reference to the modifiable 'stream'.  If 'stream' is initially
         // invalid, this operation has no effect.  If 'stream' becomes
         // invalid during this operation, this object is valid, but its
         // value is undefined.  If 'version' is not supported, 'stream' is
         // marked invalid and this object is unaltered.  Note that no
         // version is read from 'stream'.  See the 'bdex' package-level
         // documentation for more information on 'bdex' streaming of
         // value-semantic types and containers.

     virtual bdex_InStream_Test& streamIn(bdex_InStream_Test& stream,
                                          int                 version) = 0;
         // Assign to this object the value read from the specified input
         // 'stream' using the specified 'version' format and return a
         // reference to the modifiable 'stream'.  If 'stream' is initially
         // invalid, this operation has no effect.  If 'stream' becomes
         // invalid during this operation, this object is valid, but its
         // value is undefined.  If 'version' is not supported, 'stream' is
         // marked invalid and this object is unaltered.  Note that no
         // version is read from 'stream'.  See the 'bdex' package-level
         // documentation for more information on 'bdex' streaming of
         // polymorphic types.

     // ACCESSORS
     virtual void draw() const = 0;
         // Draw this shape.

     virtual int maxSupportedBdexVersion() const = 0;
         // Return the max supported 'bdex' version.

     template <class STREAM>
     STREAM& bdexStreamOut(STREAM& stream, int version) const;
         // Write this value to the specified output 'stream' using the
         // specified 'version' format and return a reference to the
         // modifiable 'stream'.  If 'version' is not supported, 'stream'
         // is unmodified.  Note that 'version' is not written to 'stream'.
         // See the 'bdex' package-level documentation for more information
         // on 'bdex' streaming of value-semantic types and containers.

     virtual bdex_OutStream& streamOut(bdex_OutStream& stream,
                                       int             version) const = 0;
         // Write this value to the specified output 'stream' using the
         // specified 'version' format and return a reference to the
         // modifiable 'stream'.  If 'version' is not supported, 'stream'
         // is unmodified.  Note that 'version' is not written to 'stream'.
         // See the 'bdex' package-level documentation for more information
         // on 'bdex' streaming of polymorphic types.
 };

//..
// Note that the 'myShape' class contains the 'streamIn' and 'streamOut'
// virtual functions in addition to the parameterized 'bdexStreamIn' and
// 'bdexStreamOut' functions.  The implementation of the parameterized 'bdex'
// functions is provided below.
//..
 // INLINE FUNCTION DEFINITIONS

 // CREATORS
 inline
 myShape::~myShape()
 {
 }

 // MANIPULATORS
 template <class STREAM>
 STREAM& myShape::bdexStreamIn(STREAM& stream, int version)
 {
     bdex_InStream_TestAdapter<STREAM> inAdapter(&stream);
     streamIn(inAdapter, version);
     return stream;
 }

 // ACCESSORS
 template <class STREAM>
 STREAM& myShape::bdexStreamOut(STREAM& stream, int version) const
 {
     bdex_OutStreamAdapter<STREAM> outAdapter(&stream);
     streamOut(outAdapter, version);
     return stream;
 }
//..
// Now consider a concrete implementation of the above protocol class, namely
// the 'myCircle' class.  Provided below is the interface of that class.
//..
 // mycircle.h

//  # include <myshape.h>

//  class bdex_OutStream;
//  class bdex_InStream_Test;

 class myCircle : public myShape{
     // This 'class' provides functions to draw a circle.

     int d_radius;    // radius of this circle
     int d_x;         // x-coordinate of center
     int d_y;         // y-coordinate of center

   public:
     // CREATORS
     myCircle();
         // Create a default circle object of zero radius with its center at
         // (0,0).

     myCircle(int x, int y, int radius);
         // Create a circle centered at the specified 'x' and 'y' values and
         // having the specified 'radius'.

     virtual ~myCircle();
         // Destroy this circle.

     // MANIPULATORS
     virtual bdex_InStream_Test& streamIn(bdex_InStream_Test& stream,
                                          int version);
         // Assign to this object the value read from the specified input
         // 'stream' using the specified 'version' format and return a
         // reference to the modifiable 'stream'.  If 'stream' is initially
         // invalid, this operation has no effect.  If 'stream' becomes
         // invalid during this operation, this object is valid, but its
         // value is undefined.  If 'version' is not supported, 'stream' is
         // marked invalid and this object is unaltered.  Note that no
         // version is read from 'stream'.  See the 'bdex' package-level
         // documentation for more information on 'bdex' streaming of
         // polymorphic types.

     // ACCESSORS
     virtual int maxSupportedBdexVersion() const;
         // Return the max supported 'bdex' version.

     virtual void draw() const;
         // Draw this shape.

     virtual bdex_OutStream& streamOut(bdex_OutStream& stream,
                                       int             version) const;
         // Write this value to the specified output 'stream' using the
         // specified 'version' format and return a reference to the
         // modifiable 'stream'.  If 'version' is not supported, 'stream'
         // is unmodified.  Note that 'version' is not written to 'stream'.
         // See the 'bdex' package-level documentation for more information
         // on 'bdex' streaming of polymorphic types.

     int radius() const;
         // Return the radius of this circle.

     int x() const;
         // Return the x-coordinate of the center of this circle.

     int y() const;
         // Return the y-coordinate of the center of this circle.
 };

// INLINE FUNCTION DEFINITIONS

// CREATORS
inline
myCircle::myCircle()
: d_radius(0)
, d_x(0)
, d_y(0)
{
}

inline
myCircle::myCircle(int radius, int x, int y)
: d_radius(radius)
, d_x(x)
, d_y(y)
{
}

inline
myCircle::~myCircle()
{
}

inline
int myCircle::radius() const
{
    return d_radius;
}

inline
int myCircle::x() const
{
    return d_x;
}

inline
int myCircle::y() const
{
    return d_y;
}

//..
// Again, note that the 'myCircle' class implements the 'streamIn' and
// 'streamOut' virtual functions derived from it's base class.  This will hold
// true for all classes derived from a 'bdex' streaming compliant protocol
// class.  Note that the 'myCircle' class could optionally specify the
// parameterized 'bdexStreamOut' and 'bdexStreamIn' functions like other
// value-semantic types.
//
// Now for the implementation of the 'myCircle' class.
//..
//  // mycircle.cpp
//
//  #include <mycircle.h>
//  #include <bdex_instream.h>
//  #include <bdex_outstream.h>
//
 // MANIPULATORS
 bdex_InStream_Test& myCircle::streamIn(bdex_InStream_Test& stream,
                                        int version)
 {
     if (stream) {
         switch (version) { // switch on the schema version
           case 1: {
             stream.getInt(d_radius);
             stream.getInt(d_x);
             stream.getInt(d_y);
           } break;
           default: {
             stream.invalidate();            // bad stream data
           }
         }
     }
     return stream;                                          // RETURN
 }

 // ACCESSORS
 int myCircle::maxSupportedBdexVersion() const
 {
     return 1;
 }

 void myCircle::draw() const
 {
     // Code for drawing the circle
 }

 bdex_OutStream& myCircle::streamOut(bdex_OutStream& stream,
                                     int             version) const
 {
     if (!stream) {
         stream.invalidate();
         return stream;
     }

     switch (version) { // switch on the schema version
       case 1: {
         bdex_OutStreamFunctions::streamOut(stream, d_radius, 0);
         bdex_OutStreamFunctions::streamOut(stream, d_x, 0);
         bdex_OutStreamFunctions::streamOut(stream, d_y, 0);
       } break;
       default: {
         stream.invalidate();            // bad stream data
       }
     }

     return stream;
 }
//..

//=============================================================================
//                      MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 4: {
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

//..
// Now we are ready to stream out our polymorphic 'myShape' class.
// Refer below to an application that does that.
//..
 // myapplication.cpp

//  int main() {
//..
// First we create two circles.  'circle1' is created with a pre-decided radius
// and center coordinates.  'circle2' is created with default radius and
// center coordinates.
//..
    myCircle circle1(3, 6, 10);
    myCircle circle2;

    ASSERT(circle1.radius() != circle2.radius());
    ASSERT(circle1.x()      != circle2.x());
    ASSERT(circle1.y()      != circle2.y());
//..
// Then we create two 'myShape' references that refer to the two circle
// objects.
//..
    myShape& shape1 = circle1;
    myShape& shape2 = circle2;
//..
// Next, we create an output stream 'os' to write data, and call the
// 'bdexStreamOut' function for the 'shape1' reference.  This will result in
// the streaming out of the 'circle1' object.
//..
    bdex_ByteOutStream os;
    const int VERSION = 1;
    os.putVersion(VERSION);
    bdex_OutStreamFunctions::streamOut(os, shape1, VERSION);
//..
// Then we create an input stream 'is' and pass it the output stream buffer.
// We then stream in from 'is' into 'shape2'.  Once that is done the default
// constructed 'circle2' object will have the same radius and center
// coordinates as 'circle1'.
//..
    bdex_ByteInStream  is(os.data(), os.length());
    int version;
    is.getVersion(version);
    bdex_InStreamFunctions::streamIn(is, shape2, version);

    ASSERT(circle1.radius() == circle2.radius());
    ASSERT(circle1.x()      == circle2.x());
    ASSERT(circle1.y()      == circle2.y());
// }
//..

      } break;
      case 3: {
        // --------------------------------
        // Testing forwarding of functions.
        // --------------------------------

        if (verbose) cout << "\nFORWARDING TEST" << endl
                          << "\n===============" << endl;

        MyOutStream myS;
        bdex_OutStreamAdapter<MyOutStream> s(&myS);
        const bdex_OutStreamAdapter<MyOutStream>& S = s;

        if (verbose) cout << "\nTesting length and version output functions."
                          << endl;

        ASSERT(&S == &s.putLength(1));         ASSERT(10 == myS.fun());
        ASSERT(&S == &s.putVersion(1));        ASSERT(11 == myS.fun());

        if (verbose) cout << "\nTesting scalar output functions." << endl;

        ASSERT(&S == &s.putInt64(1));         ASSERT(-64 == myS.fun());
        ASSERT(&S == &s.putUint64(1));        ASSERT(+64 == myS.fun());
        ASSERT(&S == &s.putInt56(1));         ASSERT(-56 == myS.fun());
        ASSERT(&S == &s.putUint56(1));        ASSERT(+56 == myS.fun());
        ASSERT(&S == &s.putInt48(1));         ASSERT(-48 == myS.fun());
        ASSERT(&S == &s.putUint48(1));        ASSERT(+48 == myS.fun());
        ASSERT(&S == &s.putInt40(1));         ASSERT(-40 == myS.fun());
        ASSERT(&S == &s.putUint40(1));        ASSERT(+40 == myS.fun());
        ASSERT(&S == &s.putInt32(1));         ASSERT(-32 == myS.fun());
        ASSERT(&S == &s.putUint32(1));        ASSERT(+32 == myS.fun());
        ASSERT(&S == &s.putInt24(1));         ASSERT(-24 == myS.fun());
        ASSERT(&S == &s.putUint24(1));        ASSERT(+24 == myS.fun());
        ASSERT(&S == &s.putInt16(1));         ASSERT(-16 == myS.fun());
        ASSERT(&S == &s.putUint16(1));        ASSERT(+16 == myS.fun());
        ASSERT(&S == &s.putInt8(1));          ASSERT( -8 == myS.fun());
        ASSERT(&S == &s.putUint8(1));         ASSERT( +8 == myS.fun());
        ASSERT(&S == &s.putFloat64(1));       ASSERT(164 == myS.fun());
        ASSERT(&S == &s.putFloat32(1));       ASSERT(132 == myS.fun());

        if (verbose) cout << "\nTesting arrayed output functions." << endl;

        bsls_PlatformUtil::Int64 ai64[1];
        bsls_PlatformUtil::Uint64 aui64[1];
        int ai[1];
        unsigned int aui[1];
        short as[1];
        unsigned short aus[1];
        char ac[1];
        signed char asc[1];
        unsigned char auc[1];
        double ad[1];
        float af[1];

        ASSERT(&S == &s.putArrayInt64( ai64, 0)); ASSERT(-1064 == myS.fun());
        ASSERT(&S == &s.putArrayUint64(aui64,0)); ASSERT(+1064 == myS.fun());
        ASSERT(&S == &s.putArrayInt56( ai64, 0)); ASSERT(-1056 == myS.fun());
        ASSERT(&S == &s.putArrayUint56(aui64,0)); ASSERT(+1056 == myS.fun());
        ASSERT(&S == &s.putArrayInt48( ai64, 0)); ASSERT(-1048 == myS.fun());
        ASSERT(&S == &s.putArrayUint48(aui64,0)); ASSERT(+1048 == myS.fun());
        ASSERT(&S == &s.putArrayInt40( ai64, 0)); ASSERT(-1040 == myS.fun());
        ASSERT(&S == &s.putArrayUint40(aui64,0)); ASSERT(+1040 == myS.fun());
        ASSERT(&S == &s.putArrayInt32( ai,   0)); ASSERT(-1032 == myS.fun());
        ASSERT(&S == &s.putArrayUint32(aui,  0)); ASSERT(+1032 == myS.fun());
        ASSERT(&S == &s.putArrayInt24( ai,   0)); ASSERT(-1024 == myS.fun());
        ASSERT(&S == &s.putArrayUint24(aui,  0)); ASSERT(+1024 == myS.fun());
        ASSERT(&S == &s.putArrayInt16( as,   0)); ASSERT(-1016 == myS.fun());
        ASSERT(&S == &s.putArrayUint16(aus,  0)); ASSERT(+1016 == myS.fun());
        ASSERT(&S == &s.putArrayInt8(  ac,   0)); ASSERT(-1001 == myS.fun());
        ASSERT(&S == &s.putArrayInt8(  asc,  0)); ASSERT(-1008 == myS.fun());
        ASSERT(&S == &s.putArrayUint8( ac,   0)); ASSERT(+1001 == myS.fun());
        ASSERT(&S == &s.putArrayUint8( auc,  0)); ASSERT(+1008 == myS.fun());
        ASSERT(&S == &s.putArrayFloat64(ad,  0)); ASSERT( 1164 == myS.fun());
        ASSERT(&S == &s.putArrayFloat32(af,  0)); ASSERT( 1132 == myS.fun());

        if (verbose) cout << "\nTesting string output function." << endl;

        bsl::string str;
        ASSERT(&S == &s.putString(str)); ASSERT(10000 == myS.fun());

        if (verbose) cout << "\nTesting manipulator functions." << endl;

        s.removeAll();          ASSERT(98 == myS.fun());
        s.reserveCapacity(0);   ASSERT(97 == myS.fun());
        s.invalidate();         ASSERT(99 == myS.fun());

        if (verbose) cout << "\nTesting accessor functions." << endl;

        ASSERT(&myS == (const void *)S);  ASSERT(89 == myS.fun());
        ASSERT(0    ==  S.data());        ASSERT(88 == myS.fun());
        ASSERT(0    ==  S.length());      ASSERT(87 == myS.fun());
      } break;
      case 2: {
        // ----------------
        // TESTING CREATORS
        // ----------------

        if (verbose) cout << endl << "\nTESTING CREATORS" << endl
                                  << "\n================" << endl;
        {
            MyOutStream myS;

            bdex_OutStreamAdapter<MyOutStream> s(&myS);
            const bdex_OutStreamAdapter<MyOutStream>& S = s;
        }

      } break;

      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Create 'bdex_OutStreamAdapter' object templated on the
        //   'bdex_ByteOutStream' class.  This ensures that the template gets
        //   instantiated correctly.
        //
        // Testing:
        //   This "test" *exercises* basic functionality, but *tests* nothing.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        bdex_ByteOutStream bs;
        bdex_OutStreamAdapter<bdex_ByteOutStream> s(&bs);

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
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
