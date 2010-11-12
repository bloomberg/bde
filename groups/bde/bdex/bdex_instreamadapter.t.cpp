// bdex_instreamadapter.t.cpp              -*-C++-*-

#include <bdex_instreamadapter.h>

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

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
        << J << "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// TBD DOC
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] virtual bdex_InStreamAdapter<STREAM>(STREAM *stream);
// [ 2] virtual ~bdex_InStreamAdapter();
//
// MANIPULATORS
// [ 3] virtual stream& getLength(int& length);
// [ 3] virtual stream& getVersion(int& version);
//
// [ 3] virtual stream& getInt64(Int64& value);
// [ 3] virtual stream& getUint64(Int64& value);
// [ 3] virtual stream& getInt56(Int64& value);
// [ 3] virtual stream& getUint56(Int64& value);
// [ 3] virtual stream& getInt48(Int64& value);
// [ 3] virtual stream& getUint48(Int64& value);
// [ 3] virtual stream& getInt40(Int64& value);
// [ 3] virtual stream& getUint40(Int64& value);
// [ 3] virtual stream& getInt32(int& value);
// [ 3] virtual stream& getUint32(int& value);
// [ 3] virtual stream& getInt24(int& value);
// [ 3] virtual stream& getUint24(int& value);
// [ 3] virtual stream& getInt16(int& value);
// [ 3] virtual stream& getUint16(int& value);
// [ 3] virtual stream& getInt8(int& value);
// [ 3] virtual stream& getUint8(int& value);
// [ 3] virtual stream& getFloat64(double& value);
// [ 3] virtual stream& getFloat32(float& value);
// [ 3] virtual stream& getString(bsl::string& value);
// [ 3] virtual stream& getArrayInt64(Int64 *a, int count);
// [ 3] virtual stream& getArrayUint64(Uint64 *a, int count);
// [ 3] virtual stream& getArrayInt56(Int64 *a, int count);
// [ 3] virtual stream& getArrayUint56(Uint64 *a, int count);
// [ 3] virtual stream& getArrayInt48(Int64 *a, int count);
// [ 3] virtual stream& getArrayUint48(Uint64 *a, int count);
// [ 3] virtual stream& getArrayInt40(Int64 *a, int count);
// [ 3] virtual stream& getArrayUint40(Uint64 *a, int count);
// [ 3] virtual stream& getArrayInt32(int *a, int count);
// [ 3] virtual stream& getArrayUint32(unsigned int *a, int count);
// [ 3] virtual stream& getArrayInt24(int *a, int count);
// [ 3] virtual stream& getArrayUint24(unsigned int *a, int count);
// [ 3] virtual stream& getArrayInt16(short *a, int count);
// [ 3] virtual stream& getArrayUint16(unsigned short *a, int count);
// [ 3] virtual stream& getArrayInt8(char *a, int count);
// [ 3] virtual stream& getArrayInt8(signed char *a, int count);
// [ 3] virtual stream& getArrayUint8(char *a, int count);
// [ 3] virtual stream& getArrayUint8(unsigned char *a, int count);
// [ 3] virtual stream& getArrayFloat64(double *a,int count);
// [ 3] virtual stream& getArrayFloat32(float *a, int count);
// [ 3] virtual void invalidate();
//
// [ 3] virtual operator const void *() const;
// [ 3] virtual int isEmpty() const;
// [ 3] virtual int length() const;
// [ 3] virtual int cursor() const;
//
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 4] USAGE EXAMPLE
//=============================================================================

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number

//==========================================================================
//                      GLOBAL CLASS FOR TESTING
//--------------------------------------------------------------------------

class MyInStream {
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
                //     read as signed and unsigned char values, respectively.
                //   - Over 2000 means other manipulators.
                //   - Over 3000 means accessors.

  public:
    MyInStream() { }
    ~MyInStream() { }

    typedef bsls_PlatformUtil::Int64 Int64;
    typedef bsls_PlatformUtil::Uint64 Uint64;

    MyInStream& getLength(int&)              { d_fun =  10; return *this; }
    MyInStream& getVersion(int&)             { d_fun =  11; return *this; }
    MyInStream& getInt64(Int64&)             { d_fun = -64; return *this; }
    MyInStream& getUint64(Uint64&)           { d_fun = +64; return *this; }
    MyInStream& getInt56(Int64&)             { d_fun = -56; return *this; }
    MyInStream& getUint56(Uint64&)           { d_fun = +56; return *this; }
    MyInStream& getInt48(Int64&)             { d_fun = -48; return *this; }
    MyInStream& getUint48(Uint64&)           { d_fun = +48; return *this; }
    MyInStream& getInt40(Int64&)             { d_fun = -40; return *this; }
    MyInStream& getUint40(Uint64&)           { d_fun = +40; return *this; }
    MyInStream& getInt32(int&)               { d_fun = -32; return *this; }
    MyInStream& getUint32(unsigned int&)     { d_fun = +32; return *this; }
    MyInStream& getInt24(int&)               { d_fun = -24; return *this; }
    MyInStream& getUint24(unsigned int&)     { d_fun = +24; return *this; }
    MyInStream& getInt16(short&)             { d_fun = -16; return *this; }
    MyInStream& getUint16(unsigned short&)   { d_fun = +16; return *this; }
    MyInStream& getInt8(signed char&)        { d_fun =  -8; return *this; }
    MyInStream& getInt8(char&)               { d_fun =  -1; return *this; }
    MyInStream& getUint8(char&)              { d_fun =  +1; return *this; }
    MyInStream& getUint8(unsigned char&)     { d_fun =  +8; return *this; }
    MyInStream& getFloat64(double&)          { d_fun = 164; return *this; }
    MyInStream& getFloat32(float&)           { d_fun = 132; return *this; }

    MyInStream& getArrayInt64(Int64 *array, int count)
                                            { d_fun = -1064; return *this; }
    MyInStream& getArrayUint64(Uint64 *array, int count)
                                            { d_fun = +1064; return *this; }
    MyInStream& getArrayInt56(Int64 *array, int count)
                                            { d_fun = -1056; return *this; }
    MyInStream& getArrayUint56(Uint64 *array, int count)
                                            { d_fun = +1056; return *this; }
    MyInStream& getArrayInt48(Int64 *array, int count)
                                            { d_fun = -1048; return *this; }
    MyInStream& getArrayUint48(Uint64 *array, int count)
                                            { d_fun = +1048; return *this; }
    MyInStream& getArrayInt40(Int64 *array, int count)
                                            { d_fun = -1040; return *this; }
    MyInStream& getArrayUint40(Uint64 *array, int count)
                                            { d_fun = +1040; return *this; }
    MyInStream& getArrayInt32(int *array, int count)
                                            { d_fun = -1032; return *this; }
    MyInStream& getArrayUint32(unsigned int *array, int count)
                                            { d_fun = +1032; return *this; }
    MyInStream& getArrayInt24(int *array, int count)
                                            { d_fun = -1024; return *this; }
    MyInStream& getArrayUint24(unsigned int *array, int count)
                                            { d_fun = +1024; return *this; }
    MyInStream& getArrayInt16(short *array, int count)
                                            { d_fun = -1016; return *this; }
    MyInStream& getArrayUint16(unsigned short *array, int count)
                                            { d_fun = +1016; return *this; }
    MyInStream& getArrayInt8(signed char *array, int count)
                                            { d_fun = -1008; return *this; }
    MyInStream& getArrayUint8(unsigned char *array, int count)
                                            { d_fun = +1008; return *this; }
    MyInStream& getArrayInt8(char *array, int count)
                                            { d_fun = -1001; return *this; }
    MyInStream& getArrayUint8(char *array, int count)
                                            { d_fun = +1001; return *this; }
    MyInStream& getArrayFloat64(double *array, int count)
                                            { d_fun = +1164; return *this; }
    MyInStream& getArrayFloat32(float *array, int count)
                                            { d_fun = +1132; return *this; }
    MyInStream& getString(bsl::string& value)
                                            { d_fun = +10000; return *this; }

    void invalidate()                       { d_fun = 2001; }

    operator const void *() const           { d_fun = 3001; return this;}
    int isEmpty() const                     { d_fun = 3002; return 0; }
    int length() const                      { d_fun = 3003; return 0; }
    int cursor() const                      { d_fun = 3004; return 0; }

    int fun() const { return d_fun; }
        // Return descriptive code for the function called.
};

//==========================================================================
//                       USAGE EXAMPLE HELPER CLASS
//--------------------------------------------------------------------------

class bdex_OutStream_Test {
    // Test out stream protocol class.

  public:

    // CREATORS
    virtual ~bdex_OutStream_Test() { }

    // MANIPULATORS
    virtual bdex_OutStream_Test& putInt(int value) = 0;
    virtual void invalidate() = 0;

    // ACCESSORS
    virtual operator const void *() const = 0;
};

template <class STREAM>
class bdex_OutStream_TestAdapter : public bdex_OutStream_Test {
    // Test out stream adapter class used for streaming out.

    STREAM *d_stream_p;  // underlying stream

  public:

    // CREATORS
    bdex_OutStream_TestAdapter(STREAM *stream) : d_stream_p(stream) { }
    virtual ~bdex_OutStream_TestAdapter() { }

    // MANIPULATORS
    virtual bdex_OutStream_Test& putInt(int value)
    {
        d_stream_p->putInt32(value);
        return *this;
    }

    virtual void invalidate() { }

    // ACCESSORS
    virtual operator const void *() const { return d_stream_p; }
};

//==========================================================================
//                       USAGE EXAMPLE
//--------------------------------------------------------------------------

// The 'bdex_InStreamAdapter' input stream interface provided in this
// component is most commonly used to implement the stream-in functionality
// for a polymorphic type.  For example, consider the following 'myShape'
// protocol class:
//..
//  // myshape.h
//  #include <bdex_instreamadapter.h>
//  #include <bdex_outstreamadapter.h>
//
//  class bdex_OutStream_Test;
//  class bdex_InStream;

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

     virtual bdex_InStream& streamIn(bdex_InStream& stream,
                                     int            version) = 0;
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

     virtual bdex_OutStream_Test& streamOut(bdex_OutStream_Test& stream,
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
     bdex_InStreamAdapter<STREAM> inAdapter(&stream);
     streamIn(inAdapter, version);
     return stream;
 }

 // ACCESSORS
 template <class STREAM>
 STREAM& myShape::bdexStreamOut(STREAM& stream, int version) const
 {
     bdex_OutStream_TestAdapter<STREAM> outAdapter(&stream);
     streamOut(outAdapter, version);
     return stream;
 }

//..
// Now consider a concrete implementation of the above protocol class, namely
// the 'myCircle' class.  Provided below is the interface of that class.
//..
 // mycircle.h

//  # include <myshape.h>

//  class bdex_OutStream_Test;
//  class bdex_InStream;

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
     virtual bdex_InStream& streamIn(bdex_InStream& stream, int version);
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

     virtual bdex_OutStream_Test& streamOut(bdex_OutStream_Test& stream,
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
 bdex_InStream& myCircle::streamIn(bdex_InStream& stream, int version)
 {
     if (stream) {
         switch (version) { // switch on the schema version
           case 1: {
             bdex_InStreamFunctions::streamIn(stream, d_radius, 1);
             bdex_InStreamFunctions::streamIn(stream, d_x, 1);
             bdex_InStreamFunctions::streamIn(stream, d_y, 1);
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

 bdex_OutStream_Test& myCircle::streamOut(bdex_OutStream_Test& stream,
                                          int                  version) const
 {
     if (!stream) {
         stream.invalidate();
         return stream;
     }

     switch (version) { // switch on the schema version
       case 1: {
         stream.putInt(d_radius);
         stream.putInt(d_x);
         stream.putInt(d_y);
       } break;
       default: {
         stream.invalidate();            // bad stream data
       }
     }

     return stream;
 }
//..


//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

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

    if (verbose) { P(os.length()); P(os.data()); }
    const char *data = os.data();
    for (int i = 0; i < os.length(); ++i) {
        if (verbose) { P((int)data[i]); }
    }
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

    LOOP2_ASSERT(circle1.radius(), circle2.radius(),
                 circle1.radius() == circle2.radius());
    LOOP2_ASSERT(circle1.x(), circle2.x(), circle1.x() == circle2.x());
    LOOP2_ASSERT(circle1.y(), circle2.y(), circle1.y() == circle2.y());
// }
//..

      } break;
      case 3: {
        // --------------------------------
        // Testing forwarding of functions.
        // --------------------------------

        if (verbose) cout << "\nFORWARDING TEST" << endl
                          << "\n===============" << endl;
        MyInStream myS;
        bdex_InStreamAdapter<MyInStream> s(&myS);
        const bdex_InStreamAdapter<MyInStream>& S = s;

        if (verbose) cout << "\nTesting length and version input functions."
                          << endl;
        {
            int length = 0;
            int version = 0;

            ASSERT(&S == &s.getLength(length));     ASSERT(10 == myS.fun());
            ASSERT(&S == &s.getVersion(version));   ASSERT(11 == myS.fun());
        }

        if (verbose) cout << "\nTesting scalar input functions." << endl;
        {
            bsls_PlatformUtil::Int64  i64 = 0;
            bsls_PlatformUtil::Uint64 ui64 = 0;
            int                       i = 0;
            unsigned int              ui = 0;
            short int                 si = 0;
            unsigned short int        usi = 0;
            char                      c = 0;
            signed char               sc = 0;
            unsigned char             uc = 0;
            double                    d = 0;
            float                     f = 0;
            bsl::string               str;

            ASSERT(&S == &s.getInt64(i64));   ASSERT(-64 == myS.fun());
            ASSERT(&S == &s.getUint64(ui64)); ASSERT(+64 == myS.fun());
            ASSERT(&S == &s.getInt56(i64));   ASSERT(-56 == myS.fun());
            ASSERT(&S == &s.getUint56(ui64)); ASSERT(+56 == myS.fun());
            ASSERT(&S == &s.getInt48(i64));   ASSERT(-48 == myS.fun());
            ASSERT(&S == &s.getUint48(ui64)); ASSERT(+48 == myS.fun());
            ASSERT(&S == &s.getInt40(i64));   ASSERT(-40 == myS.fun());
            ASSERT(&S == &s.getUint40(ui64)); ASSERT(+40 == myS.fun());
            ASSERT(&S == &s.getInt32(i));     ASSERT(-32 == myS.fun());
            ASSERT(&S == &s.getUint32(ui));   ASSERT(+32 == myS.fun());
            ASSERT(&S == &s.getInt24(i));     ASSERT(-24 == myS.fun());
            ASSERT(&S == &s.getUint24(ui));   ASSERT(+24 == myS.fun());
            ASSERT(&S == &s.getInt16(si));    ASSERT(-16 == myS.fun());
            ASSERT(&S == &s.getUint16(usi));  ASSERT(+16 == myS.fun());
            ASSERT(&S == &s.getInt8(sc));     ASSERT( -8 == myS.fun());
            ASSERT(&S == &s.getInt8(c));      ASSERT( -1 == myS.fun());
            ASSERT(&S == &s.getUint8(c));     ASSERT( +1 == myS.fun());
            ASSERT(&S == &s.getUint8(uc));    ASSERT( +8 == myS.fun());
            ASSERT(&S == &s.getFloat64(d));   ASSERT(164 == myS.fun());
            ASSERT(&S == &s.getFloat32(f));   ASSERT(132 == myS.fun());
            ASSERT(&S == &s.getString(str));  ASSERT(10000 == myS.fun());
        }

        if (verbose) cout << "\nTesting arrayed input functions." << endl;

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

        ASSERT(&S == &s.getArrayInt64( ai64, 0)); ASSERT(-1064 == myS.fun());
        ASSERT(&S == &s.getArrayUint64(aui64,0)); ASSERT(+1064 == myS.fun());
        ASSERT(&S == &s.getArrayInt56( ai64, 0)); ASSERT(-1056 == myS.fun());
        ASSERT(&S == &s.getArrayUint56(aui64,0)); ASSERT(+1056 == myS.fun());
        ASSERT(&S == &s.getArrayInt48( ai64, 0)); ASSERT(-1048 == myS.fun());
        ASSERT(&S == &s.getArrayUint48(aui64,0)); ASSERT(+1048 == myS.fun());
        ASSERT(&S == &s.getArrayInt40( ai64, 0)); ASSERT(-1040 == myS.fun());
        ASSERT(&S == &s.getArrayUint40(aui64,0)); ASSERT(+1040 == myS.fun());
        ASSERT(&S == &s.getArrayInt32( ai,   0)); ASSERT(-1032 == myS.fun());
        ASSERT(&S == &s.getArrayUint32(aui,  0)); ASSERT(+1032 == myS.fun());
        ASSERT(&S == &s.getArrayInt24( ai,   0)); ASSERT(-1024 == myS.fun());
        ASSERT(&S == &s.getArrayUint24(aui,  0)); ASSERT(+1024 == myS.fun());
        ASSERT(&S == &s.getArrayInt16( as,   0)); ASSERT(-1016 == myS.fun());
        ASSERT(&S == &s.getArrayUint16(aus,  0)); ASSERT(+1016 == myS.fun());
        ASSERT(&S == &s.getArrayInt8(  ac,   0)); ASSERT(-1001 == myS.fun());
        ASSERT(&S == &s.getArrayInt8(  asc,  0)); ASSERT(-1008 == myS.fun());
        ASSERT(&S == &s.getArrayUint8( ac,   0)); ASSERT(+1001 == myS.fun());
        ASSERT(&S == &s.getArrayUint8( auc,  0)); ASSERT(+1008 == myS.fun());
        ASSERT(&S == &s.getArrayFloat64(ad,  0)); ASSERT( 1164 == myS.fun());
        ASSERT(&S == &s.getArrayFloat32(af,  0)); ASSERT( 1132 == myS.fun());

        if (verbose) cout << "\nTesting other manipulator methods." << endl;
        s.invalidate();                           ASSERT( 2001 == myS.fun());

        if (verbose) cout << "\nTesting accessor methods." << endl;
        ASSERT(&myS == (const void *)S);          ASSERT( 3001 == myS.fun());
        ASSERT(0 == S.isEmpty());                 ASSERT( 3002 == myS.fun());
        ASSERT(0 == S.length());                  ASSERT( 3003 == myS.fun());
        ASSERT(0 == S.cursor());                  ASSERT( 3004 == myS.fun());

      } break;

      case 2: {
        // ----------------
        // TESTING CREATORS
        // ----------------

        if (verbose) cout << endl << "\nTESTING CREATORS" << endl
                                  << "\n================" << endl;
        {
            MyInStream myS;

            bdex_InStreamAdapter<MyInStream> s(&myS);
            const bdex_InStreamAdapter<MyInStream>& S = s;
        }

      } break;

      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Create 'bdex_instreamadapter' objects containing various data.
        //   Exercise these objects using appropriate input methods and primary
        //   manipulators, then verify the state of the resulting objects using
        //   basic accessors.
        //
        // Testing:
        //   This "test" exercises basic functionality, but tests nothing.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        bdex_ByteInStream bs;
        bdex_InStreamAdapter<bdex_ByteInStream> s(&bs);

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
