// bdex_instream.t.cpp          -*-C++-*-

#include <bdex_instream.h>

#include <bdex_instreamfunctions.h>
#include <bdex_outstreamfunctions.h>

#include <bsl_cstdlib.h>     // bsl::atoi()
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script


//==========================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------

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
//--------------------------------------------------------------------------

//==========================================================================
//                              TEST PLAN
//--------------------------------------------------------------------------
//                              OVERVIEW
// We are testing a pure protocol class as well as a set of overloaded
// operators.  We need to verify that (1) a concrete derived class compiles
// and links, and (2) that each overloaded input operator correctly forwards
// the call to the appropriate method.
//--------------------------------------------------------------------------
// [ 1] virtual ~bdex_InStream();
// [ 1] virtual stream& getLength(int& variable) = 0;
// [ 1] virtual stream& getVersion(int& variable) = 0;
// [ 1] virtual stream& getInt64(Int64& variable) = 0;
// [ 1] virtual stream& getUint64(Uint64& variable) = 0;
// [ 1] virtual stream& getInt56(Int64& variable) = 0;
// [ 1] virtual stream& getUint56(Uint64& variable) = 0;
// [ 1] virtual stream& getInt48(Int64& variable) = 0;
// [ 1] virtual stream& getUint48(Uint64& variable) = 0;
// [ 1] virtual stream& getInt40(Int64& variable) = 0;
// [ 1] virtual stream& getUint40(Uint64& variable) = 0;
// [ 1] virtual stream& getInt32(int& variable) = 0;
// [ 1] virtual stream& getUint32(unsigned int& variable) = 0;
// [ 1] virtual stream& getInt24(int& variable) = 0;
// [ 1] virtual stream& getUint24(unsigned int& variable) = 0;
// [ 1] virtual stream& getInt16(short& variable) = 0;
// [ 1] virtual stream& getUint16(unsigned short& variable) = 0;
// [ 1] virtual stream& getInt8(char& variable) = 0;
// [ 1] virtual stream& getInt8(signed char& variable) = 0;
// [ 1] virtual stream& getUint8(char& variable) = 0;
// [ 1] virtual stream& getUint8(unsigned char& variable) = 0;
// [ 1] virtual stream& getFloat64(double& variable) = 0;
// [ 1] virtual stream& getFloat32(float& value) = 0;
// [ 1] virtual stream& getString(bsl::string& value) = 0;
// [ 1] virtual stream& getArrayInt64(Int64 *a, int cnt) = 0;
// [ 1] virtual stream& getArrayUint64(Uint64 *a, int cnt) = 0;
// [ 1] virtual stream& getArrayInt56(Int64 *a, int cnt) = 0;
// [ 1] virtual stream& getArrayUint56(Uint64 *a, int cnt) = 0;
// [ 1] virtual stream& getArrayInt48(Int64 *a, int cnt) = 0;
// [ 1] virtual stream& getArrayUint48(Uint64 *a, int cnt) = 0;
// [ 1] virtual stream& getArrayInt40(Int64 *a, int cnt) = 0;
// [ 1] virtual stream& getArrayUint40(Uint64 *a, int cnt) = 0;
// [ 1] virtual stream& getArrayInt32(int *a, int cnt) = 0;
// [ 1] virtual stream& getArrayUint32(unsigned int *a, int cnt) = 0;
// [ 1] virtual stream& getArrayInt24(int *a, int cnt) = 0;
// [ 1] virtual stream& getArrayUint24(unsigned int *a, int cnt) = 0;
// [ 1] virtual stream& getArrayInt16(short *a, int cnt) = 0;
// [ 1] virtual stream& getArrayUint16(unsigned short *a, int cnt) = 0;
// [ 1] virtual stream& getArrayInt8(char *a, int cnt) = 0;
// [ 1] virtual stream& getArrayInt8(signed char *a, int cnt) = 0;
// [ 1] virtual stream& getArrayUint8(char *a, int cnt) = 0;
// [ 1] virtual stream& getArrayUint8(unsigned char *a, int cnt) = 0;
// [ 1] virtual stream& getArrayFloat64(double *a, int cnt) = 0;
// [ 1] virtual stream& getArrayFloat32(float *a, int cnt) = 0;
// [ 1] virtual void invalidate() = 0;
// [ 1] virtual operator const void *() const = 0;
// [ 1] virtual int isEmpty() const = 0;
// [ 1] virtual int length() const = 0;
// [ 1] virtual int cursor() const = 0;
//--------------------------------------------------------------------------
// [ 1] PROTOCOL TEST - Make sure derived class compiles and links.
// [ 2] BDEX TEST - Make sure bdex functions call correct functions.
// [ 3] USAGE TEST - Make sure usage example compiles and works as advertized.
//==========================================================================

//==========================================================================
//                      USAGE EXAMPLE
//--------------------------------------------------------------------------

// mypoint.h

class MyPoint {
    int d_xCoord;
    int d_yCoord;

  public:
     // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number
        // supported by this class.  (See the package-group-level
        // documentation for more information on 'bdex' streaming of
        // container types.)

    // CREATORS
    MyPoint();
        // Create a point at the origin (0, 0). [ default ctor needed ]

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
         // value-semantic types and containers.  Assign to this object the
         // value read from the specified 'stream'.

     // ACCESSORS
     template <class STREAM>
     STREAM& bdexStreamOut(STREAM& stream, int version) const;
         // Write this value to the specified output 'stream' using the
         // specified 'version' format and return a reference to the
         // modifiable 'stream'.  If 'version' is not supported, 'stream' is
         // unmodified.  Note that 'version' is not written to 'stream'.
         // See the 'bdex' package-level documentation for more information
         // on 'bdex' streaming of value-semantic types and containers.
};

// mypoint.cpp

// CLASS METHODS
int MyPoint::maxSupportedBdexVersion()
{
    return 1;
}

// CREATORS
MyPoint::MyPoint()
: d_xCoord(0)
, d_yCoord(0)
{
}

// MANIPULATORS
template <class STREAM>
STREAM& MyPoint::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {// switch on the schema version (starting with 1)
          case 1: {
            bdex_InStreamFunctions::streamIn(stream, d_xCoord, 0);
            if (!stream) {
                return stream;                                    // RETURN
            }

            bdex_InStreamFunctions::streamIn(stream, d_yCoord, 0);
            if (!stream) {
                return stream;                                    // RETURN
            }
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

// ACCESSORS
template <class STREAM>
STREAM& MyPoint::bdexStreamOut(STREAM& stream, int version) const
{
    if (stream) {
        switch (version) {// switch on the schema version (starting with 1)
          case 1: {
            bdex_OutStreamFunctions::streamOut(stream, d_xCoord, 0);
            bdex_OutStreamFunctions::streamOut(stream, d_yCoord, 0);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

//--------------------------------------------------------------------------
// mybox.h

class MyBox {

    MyPoint d_originCorner;
    MyPoint d_oppositeCorner;

  public:
     // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number
        // supported by this class.  (See the package-group-level
        // documentation for more information on 'bdex' streaming of
        // container types.)

     // CREATORS
     MyBox();                                    // [ default ctor needed ]
         // Create a zero-perimeter box at the origin (0, 0).

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
         // value-semantic types and containers.  Assign to this object the
         // value read from the specified 'stream'.

     // ACCESSORS
     template <class STREAM>
     STREAM& bdexStreamOut(STREAM& stream, int version) const;
         // Write this value to the specified output 'stream' using the
         // specified 'version' format and return a reference to the
         // modifiable 'stream'.  If 'version' is not supported, 'stream' is
         // unmodified.  Note that 'version' is not written to 'stream'.
         // See the 'bdex' package-level documentation for more information
         // on 'bdex' streaming of value-semantic types and containers.
};

// mybox.cpp
//  #include <mybox.h>
//  #include <mypoint.h>

// CLASS METHODS
int MyBox::maxSupportedBdexVersion()
{
    return 1;
}


// Make MyBox::MyBox defined so test case will compile.
MyBox::MyBox() {}

// MANIPULATORS
template <class STREAM>
STREAM& MyBox::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {// switch on the schema version (starting with 1)
          case 1: {
            bdex_InStreamFunctions::streamIn(stream, d_originCorner, 1);
            if (!stream) {
                return stream;                                    // RETURN
            }

            bdex_InStreamFunctions::streamIn(stream, d_oppositeCorner, 1);
            if (!stream) {
                return stream;                                    // RETURN
            }
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

// ACCESSORS
template <class STREAM>
STREAM& MyBox::bdexStreamOut(STREAM& stream, int version) const
{
    if (stream) {
        switch (version) {// switch on the schema version (starting with 1)
          case 1: {
            bdex_OutStreamFunctions::streamOut(stream, d_originCorner, 1);
            bdex_OutStreamFunctions::streamOut(stream,
                                               d_oppositeCorner,
                                               1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

//--------------------------------------------------------------------------
// second usage example
int extractExpectedObjectsFromStream(bdex_InStream& inputStream)
{
    enum { CORRUPTED_STREAM = -1, SUCCESS = 0, UNEXPECTED_EXTRA_DATA = 1 };

    // Read all expected input from the input stream.

    return !inputStream
           ? CORRUPTED_STREAM
           : !inputStream.isEmpty()
             ? UNEXPECTED_EXTRA_DATA
             : SUCCESS;
}

//==========================================================================
//                      CONCRETE DERIVED TYPE
//--------------------------------------------------------------------------
class MyInStream : public bdex_InStream {
  // Test class used to verify protocol.

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

    bdex_InStream& getLength(int&)              { d_fun =  10; return *this; }
    bdex_InStream& getVersion(int&)             { d_fun =  11; return *this; }
    bdex_InStream& getInt64(Int64&)             { d_fun = -64; return *this; }
    bdex_InStream& getUint64(Uint64&)           { d_fun = +64; return *this; }
    bdex_InStream& getInt56(Int64&)             { d_fun = -56; return *this; }
    bdex_InStream& getUint56(Uint64&)           { d_fun = +56; return *this; }
    bdex_InStream& getInt48(Int64&)             { d_fun = -48; return *this; }
    bdex_InStream& getUint48(Uint64&)           { d_fun = +48; return *this; }
    bdex_InStream& getInt40(Int64&)             { d_fun = -40; return *this; }
    bdex_InStream& getUint40(Uint64&)           { d_fun = +40; return *this; }
    bdex_InStream& getInt32(int&)               { d_fun = -32; return *this; }
    bdex_InStream& getUint32(unsigned int&)     { d_fun = +32; return *this; }
    bdex_InStream& getInt24(int&)               { d_fun = -24; return *this; }
    bdex_InStream& getUint24(unsigned int&)     { d_fun = +24; return *this; }
    bdex_InStream& getInt16(short&)             { d_fun = -16; return *this; }
    bdex_InStream& getUint16(unsigned short&)   { d_fun = +16; return *this; }
    bdex_InStream& getInt8(signed char&)        { d_fun =  -8; return *this; }

    // Initialize argument to quell purify: UMR: Uninitialized memory read
    bdex_InStream& getInt8(char& c)    { c = 'z'; d_fun =  -1; return *this; }

    bdex_InStream& getUint8(char&)              { d_fun =  +1; return *this; }
    bdex_InStream& getUint8(unsigned char&)     { d_fun =  +8; return *this; }
    bdex_InStream& getFloat64(double&)          { d_fun = 164; return *this; }
    bdex_InStream& getFloat32(float&)           { d_fun = 132; return *this; }


    bdex_InStream& getArrayInt64(Int64 *array, int count)
                                            { d_fun = -1064; return *this; }
    bdex_InStream& getArrayUint64(Uint64 *array, int count)
                                            { d_fun = +1064; return *this; }
    bdex_InStream& getArrayInt56(Int64 *array, int count)
                                            { d_fun = -1056; return *this; }
    bdex_InStream& getArrayUint56(Uint64 *array, int count)
                                            { d_fun = +1056; return *this; }
    bdex_InStream& getArrayInt48(Int64 *array, int count)
                                            { d_fun = -1048; return *this; }
    bdex_InStream& getArrayUint48(Uint64 *array, int count)
                                            { d_fun = +1048; return *this; }
    bdex_InStream& getArrayInt40(Int64 *array, int count)
                                            { d_fun = -1040; return *this; }
    bdex_InStream& getArrayUint40(Uint64 *array, int count)
                                            { d_fun = +1040; return *this; }
    bdex_InStream& getArrayInt32(int *array, int count)
                                            { d_fun = -1032; return *this; }
    bdex_InStream& getArrayUint32(unsigned int *array, int count)
                                            { d_fun = +1032; return *this; }
    bdex_InStream& getArrayInt24(int *array, int count)
                                            { d_fun = -1024; return *this; }
    bdex_InStream& getArrayUint24(unsigned int *array, int count)
                                            { d_fun = +1024; return *this; }
    bdex_InStream& getArrayInt16(short *array, int count)
                                            { d_fun = -1016; return *this; }
    bdex_InStream& getArrayUint16(unsigned short *array, int count)
                                            { d_fun = +1016; return *this; }
    bdex_InStream& getArrayInt8(signed char *array, int count)
                                            { d_fun = -1008; return *this; }
    bdex_InStream& getArrayUint8(unsigned char *array, int count)
                                            { d_fun = +1008; return *this; }
    bdex_InStream& getArrayInt8(char *array, int count)
                                            { d_fun = -1001; return *this; }
    bdex_InStream& getArrayUint8(char *array, int count)
                                            { d_fun = +1001; return *this; }
    bdex_InStream& getArrayFloat64(double *array, int count)
                                            { d_fun = +1164; return *this; }
    bdex_InStream& getArrayFloat32(float *array, int count)
                                            { d_fun = +1132; return *this; }
    bdex_InStream& getString(bsl::string& value)
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
//                      MAIN PROGRAM
//--------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 3: {
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

        if (verbose) cout << endl << "USAGE TEST" << endl
                                  << "==========" << endl;

        MyInStream myS;
        bdex_InStream& s = myS;

        MyPoint point;
        const int VERSION = 1;

        bdex_InStreamFunctions::streamIn(s, point, VERSION);

        MyBox box;

        bdex_InStreamFunctions::streamIn(s, box, VERSION);

        int status = extractExpectedObjectsFromStream(s);

      } break;
      case 2: {
        // -----------------------------------------------------------------
        // OPERATOR TEST:
        //   We want to make sure that the correct underlying method is
        //   called based on the type of the bdex functions.
        //
        // Testing:
        //   bdex_InStreamFunctions::streamIn(...);
        // -----------------------------------------------------------------

        if (verbose) cout << endl << "OPERATOR TEST" << endl
                                  << "=============" << endl;
        MyInStream myS;
        bdex_InStream& s = myS;

        if (verbose) cout << "\nTesting scalar input operators." << endl;

        bsls_PlatformUtil::Int64 i64 = 0;
        bsls_PlatformUtil::Uint64 ui64 = 0;
        int i = 0;
        unsigned int ui = 0;
        short int si = 0;
        unsigned short int usi = 0;
        char c = 0;
        signed char sc = 0;
        unsigned char uc = 0;
        double d = 0;
        float f = 0;
        bsl::string str;

        ASSERT(&myS == &(bdex_InStreamFunctions::streamIn(s, i64, 0)));
        ASSERT(-64 == myS.fun());
        ASSERT(&myS == &(bdex_InStreamFunctions::streamIn(s, ui64, 0)));
        ASSERT(+64 == myS.fun());

        ASSERT(&myS == &(bdex_InStreamFunctions::streamIn(s, i, 0)));
        ASSERT(-32 == myS.fun());
        ASSERT(&myS == &(bdex_InStreamFunctions::streamIn(s, ui, 0)));
        ASSERT(+32 == myS.fun());

        ASSERT(&myS == &(bdex_InStreamFunctions::streamIn(s, si, 0)));
        ASSERT(-16 == myS.fun());
        ASSERT(&myS == &(bdex_InStreamFunctions::streamIn(s, usi, 0)));
        ASSERT(+16 == myS.fun());

        // Note that a character is always read as a signed 8-bit integer.
        ASSERT(&myS == &(bdex_InStreamFunctions::streamIn(s, c, 0)));
        if (veryVerbose) cout <<
            "*** function code for char = " << myS.fun() << " ***" << endl;
                                                ASSERT( -1 == myS.fun());

        // Note also that there are two flavors of variable ('char' and
        // 'signed' 'char') whose overloaded input operators are able to
        // correctly read the format of the same output function putInt8().

        ASSERT(&myS == &(bdex_InStreamFunctions::streamIn(s, sc, 0)));
        ASSERT( -8 == myS.fun());
        ASSERT(&myS == &(bdex_InStreamFunctions::streamIn(s, uc, 0)));
        ASSERT( +8 == myS.fun());

        ASSERT(&myS == &(bdex_InStreamFunctions::streamIn(s, d, 0)));
        ASSERT(164 == myS.fun());
        ASSERT(&myS == &(bdex_InStreamFunctions::streamIn(s, f, 0)));
        ASSERT(132 == myS.fun());

        ASSERT(&myS == &(bdex_InStreamFunctions::streamIn(s, str, 0)));
        ASSERT(10000 == myS.fun());

      } break;
      case 1: {
        // -----------------------------------------------------------------
        // PROTOCOL TEST:
        //   All we need to do is make sure that a subclass of the
        //   'bdex_InStream' class compiles and links when all virtual
        //   functions are defined.
        //
        // Plan:
        //   Construct an object of a class derived from 'bdex_InStream'.
        //   Upcast a reference to the object to the base class
        //   'bdex_InStream'.  Using the base class reference invoke various
        //   methods of the base class.  Verify that the correct
        //   implementations of the methods are called.
        //
        // Testing:
        //   virtual ~bdex_InStream();
        //   virtual stream& getLength(int& variable) = 0;
        //   virtual stream& getVersion(int& variable) = 0;
        //   virtual stream& getInt64(Int64& variable) = 0;
        //   virtual stream& getUint64(Uint64& variable) = 0;
        //   virtual stream& getInt56(Int64& variable) = 0;
        //   virtual stream& getUint56(Uint64& variable) = 0;
        //   virtual stream& getInt48(Int64& variable) = 0;
        //   virtual stream& getUint48(Uint64& variable) = 0;
        //   virtual stream& getInt40(Int64& variable) = 0;
        //   virtual stream& getUint40(Uint64& variable) = 0;
        //   virtual stream& getInt32(int& variable) = 0;
        //   virtual stream& getUint32(unsigned int& variable) = 0;
        //   virtual stream& getInt24(int& variable) = 0;
        //   virtual stream& getUint24(unsigned int& variable) = 0;
        //   virtual stream& getInt16(short& variable) = 0;
        //   virtual stream& getUint16(unsigned short& variable) = 0;
        //   virtual stream& getInt8(char& variable) = 0;
        //   virtual stream& getInt8(signed char& variable) = 0;
        //   virtual stream& getUint8(char& variable) = 0;
        //   virtual stream& getUint8(unsigned char& variable) = 0;
        //   virtual stream& getFloat64(double& variable) = 0;
        //   virtual stream& getFloat32(float& value) = 0;
        //   virtual stream& getString(bsl::string& value) = 0;
        //   virtual stream& getArrayInt64(Int64 *a, int cnt) = 0;
        //   virtual stream& getArrayUint64(Uint64 *a, int cnt) = 0;
        //   virtual stream& getArrayInt56(Int64 *a, int cnt) = 0;
        //   virtual stream& getArrayUint56(Uint64 *a, int cnt) = 0;
        //   virtual stream& getArrayInt48(Int64 *a, int cnt) = 0;
        //   virtual stream& getArrayUint48(Uint64 *a, int cnt) = 0;
        //   virtual stream& getArrayInt40(Int64 *a, int cnt) = 0;
        //   virtual stream& getArrayUint40(Uint64 *a, int cnt) = 0;
        //   virtual stream& getArrayInt32(int *a, int cnt) = 0;
        //   virtual stream& getArrayUint32(unsigned int *a, int cnt) = 0;
        //   virtual stream& getArrayInt24(int *a, int cnt) = 0;
        //   virtual stream& getArrayUint24(unsigned int *a, int cnt) = 0;
        //   virtual stream& getArrayInt16(short *a, int cnt) = 0;
        //   virtual stream& getArrayUint16(unsigned short *a, int cnt) = 0;
        //   virtual stream& getArrayInt8(char *a, int cnt) = 0;
        //   virtual stream& getArrayInt8(signed char *a, int cnt) = 0;
        //   virtual stream& getArrayUint8(char *a, int cnt) = 0;
        //   virtual stream& getArrayUint8(unsigned char *a, int cnt) = 0;
        //   virtual stream& getArrayFloat64(double *a, int cnt) = 0;
        //   virtual stream& getArrayFloat32(float *a, int cnt) = 0;
        //
        //   virtual void invalidate() = 0;
        //
        //   virtual operator const void *() const = 0;
        //   virtual int isEmpty() const = 0;
        //   virtual int length() const = 0;
        //   virtual int cursor() const = 0;
        //
        //   PROTOCOL TEST - Make sure derived class compiles and links.
        // -----------------------------------------------------------------

        if (verbose) cout << endl << "PROTOCOL TEST" << endl
                                  << "=============" << endl;
        MyInStream myS;
        bdex_InStream& s = myS; const bdex_InStream& S = s;

        if (verbose) cout << "\nTesting length and version input functions."
                          << endl;
        {
            int length = 0;
            int version = 0;

            ASSERT(&myS == &s.getLength(length));     ASSERT(10 == myS.fun());
            ASSERT(&myS == &s.getVersion(version));   ASSERT(11 == myS.fun());
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

            ASSERT(&myS == &s.getInt64(i64));   ASSERT(-64 == myS.fun());
            ASSERT(&myS == &s.getUint64(ui64)); ASSERT(+64 == myS.fun());
            ASSERT(&myS == &s.getInt56(i64));   ASSERT(-56 == myS.fun());
            ASSERT(&myS == &s.getUint56(ui64)); ASSERT(+56 == myS.fun());
            ASSERT(&myS == &s.getInt48(i64));   ASSERT(-48 == myS.fun());
            ASSERT(&myS == &s.getUint48(ui64)); ASSERT(+48 == myS.fun());
            ASSERT(&myS == &s.getInt40(i64));   ASSERT(-40 == myS.fun());
            ASSERT(&myS == &s.getUint40(ui64)); ASSERT(+40 == myS.fun());
            ASSERT(&myS == &s.getInt32(i));     ASSERT(-32 == myS.fun());
            ASSERT(&myS == &s.getUint32(ui));   ASSERT(+32 == myS.fun());
            ASSERT(&myS == &s.getInt24(i));     ASSERT(-24 == myS.fun());
            ASSERT(&myS == &s.getUint24(ui));   ASSERT(+24 == myS.fun());
            ASSERT(&myS == &s.getInt16(si));    ASSERT(-16 == myS.fun());
            ASSERT(&myS == &s.getUint16(usi));  ASSERT(+16 == myS.fun());
            ASSERT(&myS == &s.getInt8(sc));     ASSERT( -8 == myS.fun());
            ASSERT(&myS == &s.getInt8(c));      ASSERT( -1 == myS.fun());
            ASSERT(&myS == &s.getUint8(c));     ASSERT( +1 == myS.fun());
            ASSERT(&myS == &s.getUint8(uc));    ASSERT( +8 == myS.fun());
            ASSERT(&myS == &s.getFloat64(d));   ASSERT(164 == myS.fun());
            ASSERT(&myS == &s.getFloat32(f));   ASSERT(132 == myS.fun());
            ASSERT(&myS == &s.getString(str));  ASSERT(10000 == myS.fun());
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

        ASSERT(&myS == &s.getArrayInt64( ai64, 0)); ASSERT(-1064 == myS.fun());
        ASSERT(&myS == &s.getArrayUint64(aui64,0)); ASSERT(+1064 == myS.fun());
        ASSERT(&myS == &s.getArrayInt56( ai64, 0)); ASSERT(-1056 == myS.fun());
        ASSERT(&myS == &s.getArrayUint56(aui64,0)); ASSERT(+1056 == myS.fun());
        ASSERT(&myS == &s.getArrayInt48( ai64, 0)); ASSERT(-1048 == myS.fun());
        ASSERT(&myS == &s.getArrayUint48(aui64,0)); ASSERT(+1048 == myS.fun());
        ASSERT(&myS == &s.getArrayInt40( ai64, 0)); ASSERT(-1040 == myS.fun());
        ASSERT(&myS == &s.getArrayUint40(aui64,0)); ASSERT(+1040 == myS.fun());
        ASSERT(&myS == &s.getArrayInt32( ai,   0)); ASSERT(-1032 == myS.fun());
        ASSERT(&myS == &s.getArrayUint32(aui,  0)); ASSERT(+1032 == myS.fun());
        ASSERT(&myS == &s.getArrayInt24( ai,   0)); ASSERT(-1024 == myS.fun());
        ASSERT(&myS == &s.getArrayUint24(aui,  0)); ASSERT(+1024 == myS.fun());
        ASSERT(&myS == &s.getArrayInt16( as,   0)); ASSERT(-1016 == myS.fun());
        ASSERT(&myS == &s.getArrayUint16(aus,  0)); ASSERT(+1016 == myS.fun());
        ASSERT(&myS == &s.getArrayInt8(  ac,   0)); ASSERT(-1001 == myS.fun());
        ASSERT(&myS == &s.getArrayInt8(  asc,  0)); ASSERT(-1008 == myS.fun());
        ASSERT(&myS == &s.getArrayUint8( ac,   0)); ASSERT(+1001 == myS.fun());
        ASSERT(&myS == &s.getArrayUint8( auc,  0)); ASSERT(+1008 == myS.fun());
        ASSERT(&myS == &s.getArrayFloat64(ad,  0)); ASSERT( 1164 == myS.fun());
        ASSERT(&myS == &s.getArrayFloat32(af,  0)); ASSERT( 1132 == myS.fun());

        if (verbose) cout << "\nTesting other manipulator methods." << endl;
        s.invalidate();                             ASSERT( 2001 == myS.fun());

        if (verbose) cout << "\nTesting accessor methods." << endl;
        ASSERT(&myS == (const void *)S);            ASSERT( 3001 == myS.fun());
        ASSERT(   0 == S.isEmpty());                ASSERT( 3002 == myS.fun());
        ASSERT(   0 == S.length());                 ASSERT( 3003 == myS.fun());
        ASSERT(   0 == S.cursor());                 ASSERT( 3004 == myS.fun());

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
