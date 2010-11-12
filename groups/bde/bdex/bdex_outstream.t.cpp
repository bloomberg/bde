// bdex_outstream.t.cpp         -*-C++-*-

#include <bdex_outstream.h>

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

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
        << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

//==========================================================================
//                              TEST PLAN
//--------------------------------------------------------------------------
//                              OVERVIEW
// We are testing a pure protocol class as well as a set of overloaded
// operators.  We need to verify that (1) a concrete derived class compiles
// and links, and (2) that each overloaded output operator correctly forwards
// the call to the appropriate method.
//--------------------------------------------------------------------------
// CREATORS
// [ 1] virtual ~bdex_OutStream();
//
// MANIPULATORS
// [ 1] virtual void    invalidate() = 0;
// [ 1] virtual stream& putLength(int length) = 0;
// [ 1] virtual stream& putVersion(int version) = 0;
// [ 1] virtual stream& putInt64(Int64 value) = 0;
// [ 1] virtual stream& putUint64(Int64 value) = 0;
// [ 1] virtual stream& putInt56(Int64 value) = 0;
// [ 1] virtual stream& putUint56(Int64 value) = 0;
// [ 1] virtual stream& putInt48(Int64 value) = 0;
// [ 1] virtual stream& putUint48(Int64 value) = 0;
// [ 1] virtual stream& putInt40(Int64 value) = 0;
// [ 1] virtual stream& putUint40(Int64 value) = 0;
// [ 1] virtual stream& putInt32(int value) = 0;
// [ 1] virtual stream& putUint32(int value) = 0;
// [ 1] virtual stream& putInt24(int value) = 0;
// [ 1] virtual stream& putUint24(int value) = 0;
// [ 1] virtual stream& putInt16(int value) = 0;
// [ 1] virtual stream& putUint16(int value) = 0;
// [ 1] virtual stream& putInt8(int value) = 0;
// [ 1] virtual stream& putUint8(int value) = 0;
// [ 1] virtual stream& putFloat64(double value) = 0;
// [ 1] virtual stream& putFloat32(float value) = 0;
// [ 1] virtual stream& putString(const bsl::string& value) = 0;
// [ 1] virtual stream& putArrayInt64(const Int64 *ary, int cnt) = 0;
// [ 1] virtual stream& putArrayUint64(const Uint64 *ary, int cnt) = 0;
// [ 1] virtual stream& putArrayInt56(const Int64 *ary, int cnt) = 0;
// [ 1] virtual stream& putArrayUint56(const Uint64 *ary, int cnt) = 0;
// [ 1] virtual stream& putArrayInt48(const Int64 *ary, int cnt) = 0;
// [ 1] virtual stream& putArrayUint48(const Uint64 *ary, int cnt) = 0;
// [ 1] virtual stream& putArrayInt40(const Int64 *ary, int cnt) = 0;
// [ 1] virtual stream& putArrayUint40(const Uint64 *ary, int cnt) = 0;
// [ 1] virtual stream& putArrayInt32(const int *ary, int cnt) = 0;
// [ 1] virtual stream& putArrayUint32(*ary, int cnt) = 0;
// [ 1] virtual stream& putArrayInt24(const int *ary, int cnt) = 0;
// [ 1] virtual stream& putArrayUint24(*ary, int cnt) = 0;
// [ 1] virtual stream& putArrayInt16(const short *ary, int cnt) = 0;
// [ 1] virtual stream& putArrayUint16(*ary, int cnt) = 0;
// [ 1] virtual stream& putArrayInt8(const char *ary, int cnt) = 0;
// [ 1] virtual stream& putArrayInt8(*ary, int cnt) = 0;
// [ 1] virtual stream& putArrayUint8(const char *ary, int cnt) = 0;
// [ 1] virtual stream& putArrayUint8(*ary, int cnt) = 0;
// [ 1] virtual stream& putArrayFloat64(*ary, int cnt) = 0;
// [ 1] virtual stream& putArrayFloat32(const float *ary, int cnt) = 0;
// [ 1] virtual void    removeAll() = 0;
// [ 1] virtual void    reserveCapacity(int newCapacity) = 0;
//
// ACCESSORS
// [ 1] virtual operator const void *() const = 0;
// [ 1] virtual const char          *data() const = 0;
// [ 1] virtual int                  length() const = 0;
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
        // Create a point at the origin (0, 0).  [ default ctor needed ]

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

//==========================================================================
//                      CONCRETE DERIVED TYPE
//--------------------------------------------------------------------------

class MyOutStream : public bdex_OutStream {
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
                //     output as signed and unsigned char values, respectively.
                //   - Miscellaneous manipulator functions, when code is a two
                //     digit number with first digit 9.
                //   - Miscellaneous accessor functions, when code is a two
                //     digit number with first digit 8.
  public:
    MyOutStream() { }
    ~MyOutStream() { }

    typedef bsls_PlatformUtil::Int64 Int64;

    bdex_OutStream& putLength(int length)   { d_fun =  10; return *this; }
    bdex_OutStream& putVersion(int version) { d_fun =  11; return *this; }

    bdex_OutStream& putInt64(Int64 value)   { d_fun = -64; return *this; }
    bdex_OutStream& putUint64(Int64 value)  { d_fun = +64; return *this; }
    bdex_OutStream& putInt56(Int64 value)   { d_fun = -56; return *this; }
    bdex_OutStream& putUint56(Int64 value)  { d_fun = +56; return *this; }
    bdex_OutStream& putInt48(Int64 value)   { d_fun = -48; return *this; }
    bdex_OutStream& putUint48(Int64 value)  { d_fun = +48; return *this; }
    bdex_OutStream& putInt40(Int64 value)   { d_fun = -40; return *this; }
    bdex_OutStream& putUint40(Int64 value)  { d_fun = +40; return *this; }
    bdex_OutStream& putInt32(int value)     { d_fun = -32; return *this; }
    bdex_OutStream& putUint32(unsigned int value)
                                            { d_fun = +32; return *this; }
    bdex_OutStream& putInt24(int value)     { d_fun = -24; return *this; }
    bdex_OutStream& putUint24(int value)    { d_fun = +24; return *this; }
    bdex_OutStream& putInt16(int value)     { d_fun = -16; return *this; }
    bdex_OutStream& putUint16(int value)    { d_fun = +16; return *this; }
    bdex_OutStream& putInt8(int value)      { d_fun =  -8; return *this; }
    bdex_OutStream& putUint8(int value)     { d_fun =  +8; return *this; }
    bdex_OutStream& putFloat64(double value){ d_fun = 164; return *this; }
    bdex_OutStream& putFloat32(float value) { d_fun = 132; return *this; }

    typedef bsls_PlatformUtil::Uint64 Uint64;

    bdex_OutStream& putArrayInt64(const Int64 *array, int count)
                                            { d_fun = -1064; return *this; }
    bdex_OutStream& putArrayUint64(const Uint64 *array, int count)
                                            { d_fun = +1064; return *this; }
    bdex_OutStream& putArrayInt56(const Int64 *array, int count)
                                            { d_fun = -1056; return *this; }
    bdex_OutStream& putArrayUint56(const Uint64 *array, int count)
                                            { d_fun = +1056; return *this; }
    bdex_OutStream& putArrayInt48(const Int64 *array, int count)
                                            { d_fun = -1048; return *this; }
    bdex_OutStream& putArrayUint48(const Uint64 *array, int count)
                                            { d_fun = +1048; return *this; }
    bdex_OutStream& putArrayInt40(const Int64 *array, int count)
                                            { d_fun = -1040; return *this; }
    bdex_OutStream& putArrayUint40(const Uint64 *array, int count)
                                            { d_fun = +1040; return *this; }
    bdex_OutStream& putArrayInt32(const int *array, int count)
                                            { d_fun = -1032; return *this; }
    bdex_OutStream& putArrayUint32(const unsigned int *array, int count)
                                            { d_fun = +1032; return *this; }
    bdex_OutStream& putArrayInt24(const int *array, int count)
                                            { d_fun = -1024; return *this; }
    bdex_OutStream& putArrayUint24(const unsigned int *array, int count)
                                            { d_fun = +1024; return *this; }
    bdex_OutStream& putArrayInt16(const short *array, int count)
                                            { d_fun = -1016; return *this; }
    bdex_OutStream& putArrayUint16(const unsigned short *array, int count)
                                            { d_fun = +1016; return *this; }
    bdex_OutStream& putArrayInt8(const signed char *array, int count)
                                            { d_fun = -1008; return *this; }
    bdex_OutStream& putArrayUint8(const unsigned char *array, int count)
                                            { d_fun = +1008; return *this; }
    bdex_OutStream& putArrayInt8(const char *array, int count)
                                            { d_fun = -1001; return *this; }
    bdex_OutStream& putArrayUint8(const char *array, int count)
                                            { d_fun = +1001; return *this; }
    bdex_OutStream& putArrayFloat64(const double *array, int count)
                                            { d_fun = +1164; return *this; }
    bdex_OutStream& putArrayFloat32(const float *array, int count)
                                            { d_fun = +1132; return *this; }

    bdex_OutStream& putString(const bsl::string& value)
                                            { d_fun = +10000; return *this; }

    // MANIPULATORS
    void            invalidate()            { d_fun =  99; }
    void            removeAll()             { d_fun =  98; }
    void            reserveCapacity(int newCapacity)
                                            { d_fun =  97; }
    // ACCESSORS
    operator const void *() const           { d_fun =  89; return this; }
    const char     *data() const            { d_fun =  88; return 0; }
    int             length() const          { d_fun =  87; return 0; }

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

        MyOutStream myS;
        bdex_OutStream& s = myS;

        MyPoint point;
        const int VERSION = MyPoint::maxSupportedBdexVersion();

        bdex_OutStreamFunctions::streamOut(s, point, VERSION);
      } break;
      case 2: {
        // -----------------------------------------------------------------
        // OPERATOR TEST:
        //   We want to make sure that the correct underlying method is
        //   called based on the type of the bdex stream functions.
        //
        // Testing:
        //   bdex_OutStreamFunctions::streamOut(...);
        // -----------------------------------------------------------------

        if (verbose) cout << endl << "OPERATOR TEST" << endl
                                  << "=============" << endl;
        MyOutStream myS;
        bdex_OutStream& s = myS;

        if (verbose) cout << "\nTesting scalar output operators." << endl;

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

        ASSERT(&myS == &(bdex_OutStreamFunctions::streamOut(s, i64, 0)));
        ASSERT(-64 == myS.fun());

        ASSERT(&myS == &(bdex_OutStreamFunctions::streamOut(s, ui64, 0)));
        ASSERT(+64 == myS.fun());

        ASSERT(&myS == &(bdex_OutStreamFunctions::streamOut(s, i, 0)));
        ASSERT(-32 == myS.fun());

        ASSERT(&myS == &(bdex_OutStreamFunctions::streamOut(s, ui, 0)));
        ASSERT(+32 == myS.fun());

        ASSERT(&myS == &(bdex_OutStreamFunctions::streamOut(s, si, 0)));
        ASSERT(-16 == myS.fun());

        ASSERT(&myS == &(bdex_OutStreamFunctions::streamOut(s, usi, 0)));
        ASSERT(+16 == myS.fun());

        // Note that a character is always output as a signed 8-bit integer.
        ASSERT(&myS == &(bdex_OutStreamFunctions::streamOut(s, c, 0)));
        if (veryVerbose) cout <<
            "*** function code for char = " << myS.fun() << " ***" << endl;
                                                ASSERT( -8 == myS.fun());

        ASSERT(&myS == &(bdex_OutStreamFunctions::streamOut(s, sc, 0)));
        ASSERT( -8 == myS.fun());

        ASSERT(&myS == &(bdex_OutStreamFunctions::streamOut(s, uc, 0)));
        ASSERT( +8 == myS.fun());

        ASSERT(&myS == &(bdex_OutStreamFunctions::streamOut(s, d, 0)));
        ASSERT(164 == myS.fun());

        ASSERT(&myS == &(bdex_OutStreamFunctions::streamOut(s, f, 0)));
        ASSERT(132 == myS.fun());

        ASSERT(&myS == &(bdex_OutStreamFunctions::streamOut(s, str, 0)));
        ASSERT(10000 == myS.fun());

      } break;
      case 1: {
        // -----------------------------------------------------------------
        // PROTOCOL TEST:
        //   All we need to do is make sure that a subclass of the
        //   'bdex_OutStream' class compiles and links when all virtual
        //   functions are defined.
        //
        // Plan:
        //   Construct an object of a class derived from 'bdex_OutStream'.
        //   Upcast a reference to the object to the base class
        //   'bdex_OutStream'.  Using the base class reference invoke various
        //   methods of the base class.  Verify that the correct
        //   implementations of the methods are called.
        //
        // Testing:
        //
        //   virtual ~bdex_OutStream();
        //   virtual void    invalidate() = 0;
        //   virtual stream& putLength(int length) = 0;
        //   virtual stream& putVersion(int version) = 0;
        //   virtual stream& putInt64(Int64 value) = 0;
        //   virtual stream& putUint64(Int64 value) = 0;
        //   virtual stream& putInt56(Int64 value) = 0;
        //   virtual stream& putUint56(Int64 value) = 0;
        //   virtual stream& putInt48(Int64 value) = 0;
        //   virtual stream& putUint48(Int64 value) = 0;
        //   virtual stream& putInt40(Int64 value) = 0;
        //   virtual stream& putUint40(Int64 value) = 0;
        //   virtual stream& putInt32(int value) = 0;
        //   virtual stream& putUint32(int value) = 0;
        //   virtual stream& putInt24(int value) = 0;
        //   virtual stream& putUint24(int value) = 0;
        //   virtual stream& putInt16(int value) = 0;
        //   virtual stream& putUint16(int value) = 0;
        //   virtual stream& putInt8(int value) = 0;
        //   virtual stream& putUint8(int value) = 0;
        //   virtual stream& putFloat64(double value) = 0;
        //   virtual stream& putFloat32(float value) = 0;
        //   virtual stream& putString(const bsl::string& value) = 0;
        //   virtual stream& putArrayInt64(const Int64 *ary, int cnt) = 0;
        //   virtual stream& putArrayUint64(const Uint64 *ary, int cnt) = 0;
        //   virtual stream& putArrayInt56(const Int64 *ary, int cnt) = 0;
        //   virtual stream& putArrayUint56(const Uint64 *ary, int cnt) = 0;
        //   virtual stream& putArrayInt48(const Int64 *ary, int cnt) = 0;
        //   virtual stream& putArrayUint48(const Uint64 *ary, int cnt) = 0;
        //   virtual stream& putArrayInt40(const Int64 *ary, int cnt) = 0;
        //   virtual stream& putArrayUint40(const Uint64 *ary, int cnt) = 0;
        //   virtual stream& putArrayInt32(const int *ary, int cnt) = 0;
        //   virtual stream& putArrayUint32(*ary, int cnt) = 0;
        //   virtual stream& putArrayInt24(const int *ary, int cnt) = 0;
        //   virtual stream& putArrayUint24(*ary, int cnt) = 0;
        //   virtual stream& putArrayInt16(const short *ary, int cnt) = 0;
        //   virtual stream& putArrayUint16(*ary, int cnt) = 0;
        //   virtual stream& putArrayInt8(const char *ary, int cnt) = 0;
        //   virtual stream& putArrayInt8(*ary, int cnt) = 0;
        //   virtual stream& putArrayUint8(const char *ary, int cnt) = 0;
        //   virtual stream& putArrayUint8(*ary, int cnt) = 0;
        //   virtual stream& putArrayFloat64(*ary, int cnt) = 0;
        //   virtual stream& putArrayFloat32(const float *ary, int cnt) = 0;
        //   virtual void    removeAll() = 0;
        //   virtual void    reserveCapacity(int newCapacity) = 0;
        //
        //   virtual operator const void *() const = 0;
        //   virtual const char          *data() const = 0;
        //   virtual int                  length() const = 0;
        //
        //   PROTOCOL TEST - Make sure derived class compiles and links.
        // -----------------------------------------------------------------

        if (verbose) cout << endl << "PROTOCOL TEST" << endl
                                  << "=============" << endl;
        MyOutStream myS;
        bdex_OutStream& s = myS; const bdex_OutStream& S = myS;

        if (verbose) cout << "\nTesting length and version output functions."
                          << endl;

        ASSERT(&myS == &s.putLength(1));         ASSERT(10 == myS.fun());
        ASSERT(&myS == &s.putVersion(1));        ASSERT(11 == myS.fun());

        if (verbose) cout << "\nTesting scalar output functions." << endl;

        ASSERT(&myS == &s.putInt64(1));         ASSERT(-64 == myS.fun());
        ASSERT(&myS == &s.putUint64(1));        ASSERT(+64 == myS.fun());
        ASSERT(&myS == &s.putInt56(1));         ASSERT(-56 == myS.fun());
        ASSERT(&myS == &s.putUint56(1));        ASSERT(+56 == myS.fun());
        ASSERT(&myS == &s.putInt48(1));         ASSERT(-48 == myS.fun());
        ASSERT(&myS == &s.putUint48(1));        ASSERT(+48 == myS.fun());
        ASSERT(&myS == &s.putInt40(1));         ASSERT(-40 == myS.fun());
        ASSERT(&myS == &s.putUint40(1));        ASSERT(+40 == myS.fun());
        ASSERT(&myS == &s.putInt32(1));         ASSERT(-32 == myS.fun());
        ASSERT(&myS == &s.putUint32(1));        ASSERT(+32 == myS.fun());
        ASSERT(&myS == &s.putInt24(1));         ASSERT(-24 == myS.fun());
        ASSERT(&myS == &s.putUint24(1));        ASSERT(+24 == myS.fun());
        ASSERT(&myS == &s.putInt16(1));         ASSERT(-16 == myS.fun());
        ASSERT(&myS == &s.putUint16(1));        ASSERT(+16 == myS.fun());
        ASSERT(&myS == &s.putInt8(1));          ASSERT( -8 == myS.fun());
        ASSERT(&myS == &s.putUint8(1));         ASSERT( +8 == myS.fun());
        ASSERT(&myS == &s.putFloat64(1));       ASSERT(164 == myS.fun());
        ASSERT(&myS == &s.putFloat32(1));       ASSERT(132 == myS.fun());

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

        ASSERT(&myS == &s.putArrayInt64( ai64, 0)); ASSERT(-1064 == myS.fun());
        ASSERT(&myS == &s.putArrayUint64(aui64,0)); ASSERT(+1064 == myS.fun());
        ASSERT(&myS == &s.putArrayInt56( ai64, 0)); ASSERT(-1056 == myS.fun());
        ASSERT(&myS == &s.putArrayUint56(aui64,0)); ASSERT(+1056 == myS.fun());
        ASSERT(&myS == &s.putArrayInt48( ai64, 0)); ASSERT(-1048 == myS.fun());
        ASSERT(&myS == &s.putArrayUint48(aui64,0)); ASSERT(+1048 == myS.fun());
        ASSERT(&myS == &s.putArrayInt40( ai64, 0)); ASSERT(-1040 == myS.fun());
        ASSERT(&myS == &s.putArrayUint40(aui64,0)); ASSERT(+1040 == myS.fun());
        ASSERT(&myS == &s.putArrayInt32( ai,   0)); ASSERT(-1032 == myS.fun());
        ASSERT(&myS == &s.putArrayUint32(aui,  0)); ASSERT(+1032 == myS.fun());
        ASSERT(&myS == &s.putArrayInt24( ai,   0)); ASSERT(-1024 == myS.fun());
        ASSERT(&myS == &s.putArrayUint24(aui,  0)); ASSERT(+1024 == myS.fun());
        ASSERT(&myS == &s.putArrayInt16( as,   0)); ASSERT(-1016 == myS.fun());
        ASSERT(&myS == &s.putArrayUint16(aus,  0)); ASSERT(+1016 == myS.fun());
        ASSERT(&myS == &s.putArrayInt8(  ac,   0)); ASSERT(-1001 == myS.fun());
        ASSERT(&myS == &s.putArrayInt8(  asc,  0)); ASSERT(-1008 == myS.fun());
        ASSERT(&myS == &s.putArrayUint8( ac,   0)); ASSERT(+1001 == myS.fun());
        ASSERT(&myS == &s.putArrayUint8( auc,  0)); ASSERT(+1008 == myS.fun());
        ASSERT(&myS == &s.putArrayFloat64(ad,  0)); ASSERT( 1164 == myS.fun());
        ASSERT(&myS == &s.putArrayFloat32(af,  0)); ASSERT( 1132 == myS.fun());

        if (verbose) cout << "\nTesting string output function." << endl;

        bsl::string str;
        ASSERT(&myS == &s.putString(str)); ASSERT(10000 == myS.fun());

        if (verbose) cout << "\nTesting manipulator functions." << endl;

        s.removeAll();          ASSERT(98 == myS.fun());
        s.reserveCapacity(0);   ASSERT(97 == myS.fun());
        s.invalidate();         ASSERT(99 == myS.fun());

        if (verbose) cout << "\nTesting accessor functions." << endl;

        ASSERT(&myS == (const void *)S);  ASSERT(89 == myS.fun());
        ASSERT(0    ==  S.data());        ASSERT(88 == myS.fun());
        ASSERT(0    ==  S.length());      ASSERT(87 == myS.fun());

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
