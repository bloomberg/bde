// bslx_outstreamfunctions.t.cpp                                      -*-C++-*-

#include <bslx_outstreamfunctions.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <bsl_climits.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using namespace bsl;
using namespace bslx;

// ============================================================================
//                              TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// This component provides two functions, each of which invokes a method of its
// template parameter.  In addition, the component provides a number of
// overloads of these methods for fundamental types, 'bsl::vector', and
// 'bsl::string'.  A test type and a test stream are provided, each of which
// responds in a simple, observable manner when its various methods are called.
// The testing requirements are fairly straightforward and the provided test
// type and test stream are used to verify the behavior of the two methods.
// ----------------------------------------------------------------------------
// [ 2] bdexStreamOut(STREAM& stream, const TYPE& value);
// [ 1] bdexStreamOut(STREAM& stream, const TYPE& value, int version);
// ----------------------------------------------------------------------------
// [ 3] USAGE EXAMPLE
// ----------------------------------------------------------------------------

// ============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

// ============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                   GLOBAL CONSTANTS/TYPEDEFS FOR TESTING
// ============================================================================

typedef bsls::Types::Int64  Int64;
typedef bsls::Types::Uint64 Uint64;

const int VERSION_SELECTOR = 20131201;
const int OTHER_VERSION_SELECTOR = 20131101;

// ============================================================================
//                      GLOBAL TEST CLASSES
// ----------------------------------------------------------------------------

enum MyTestEnum {
    e_A = INT_MIN,
    e_B = -1,
    e_C = 0,
    e_D = 1,
    e_E = INT_MAX
};

enum MySpecializedTestEnum {
    e_F = -1,
    e_G = 0,
    e_H = 1
};

template <class STREAM>
STREAM& bdexStreamOut(STREAM&                      stream,
                      const MySpecializedTestEnum& value,
                      int                       /* version */)
{
    using bslx::OutStreamFunctions::bdexStreamOut;

    return bdexStreamOut(stream, static_cast<float>(value), 1);
}

namespace ThirdParty {

enum MyThirdPartyTestEnum {
    e_I = -1,
    e_J = 0,
    e_K = 1
};

template <class STREAM>
STREAM& bdexStreamOut(STREAM&                      stream,
                      const MyThirdPartyTestEnum&  value,
                      int                       /* version */)
{
    using bslx::OutStreamFunctions::bdexStreamOut;

    return bdexStreamOut(stream, static_cast<char>(value), 1);
}

}  // close namespace ThirdParty

class MyTestClass {
    // This test class is used for testing the streaming functionality.

  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion(int versionSelector);

    // CREATORS
    MyTestClass() { }
    ~MyTestClass() { }

    // ACCESSORS
    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
};

int MyTestClass::maxSupportedBdexVersion(int versionSelector) {
    if (versionSelector >= VERSION_SELECTOR) return 2;                // RETURN
    return 1;
}

template <class STREAM>
STREAM& MyTestClass::bdexStreamOut(STREAM& stream, int version) const
{
    if (2 == version) {
        int x = 17;
        double y = 3.0;
        OutStreamFunctions::bdexStreamOut(stream, x, 1);
        OutStreamFunctions::bdexStreamOut(stream, y, 1);
    }
    else if (1 == version) {
        int x = 17;
        float y = 3.0;
        OutStreamFunctions::bdexStreamOut(stream, x, 1);
        OutStreamFunctions::bdexStreamOut(stream, y, 1);
    }
    return stream;
}

class MyTestOutStream {
    // Test class used to test streaming.

    int              d_versionSelector;
    bsl::vector<int> d_fun;
    int              d_lastVersion;

  public:
    enum {
      k_CHAR     =   10,
      k_INT      =   20,
      k_FLOAT    =   30,
      k_STRING   =   40,
      k_LENGTH   =   50,
      k_VERSION  =   60,
      k_UNSIGNED =  100,
      k_ARRAY    = 1000,

      k_UINT     = k_UNSIGNED + k_INT
    };

    // CREATORS
    MyTestOutStream(int versionSelector)
      : d_versionSelector(versionSelector)
      , d_fun()
      , d_lastVersion(-2)
    {
    }

    ~MyTestOutStream() { }

    MyTestOutStream& putLength(int /* length */)
                                   { d_fun.push_back(k_LENGTH); return *this; }
    MyTestOutStream& putVersion(int version)
        { d_fun.push_back(k_VERSION); d_lastVersion = version;  return *this; }

    MyTestOutStream& putInt64(Int64 /* value */)
                                  { d_fun.push_back(k_INT + 8); return *this; }
    MyTestOutStream& putUint64(Int64 /* value */)
                     { d_fun.push_back(k_UNSIGNED + k_INT + 8); return *this; }
    MyTestOutStream& putInt56(Int64 /* value */)
                                  { d_fun.push_back(k_INT + 7); return *this; }
    MyTestOutStream& putUint56(Int64 /* value */)
                     { d_fun.push_back(k_UNSIGNED + k_INT + 7); return *this; }
    MyTestOutStream& putInt48(Int64 /* value */)
                                  { d_fun.push_back(k_INT + 6); return *this; }
    MyTestOutStream& putUint48(Int64 /* value */)
                     { d_fun.push_back(k_UNSIGNED + k_INT + 6); return *this; }
    MyTestOutStream& putInt40(Int64 /* value */)
                                  { d_fun.push_back(k_INT + 5); return *this; }
    MyTestOutStream& putUint40(Int64 /* value */)
                     { d_fun.push_back(k_UNSIGNED + k_INT + 5); return *this; }
    MyTestOutStream& putInt32(int /* value */)
                                  { d_fun.push_back(k_INT + 4); return *this; }
    MyTestOutStream& putUint32(unsigned int /* value */)
                     { d_fun.push_back(k_UNSIGNED + k_INT + 4); return *this; }
    MyTestOutStream& putInt24(int /* value */)
                                  { d_fun.push_back(k_INT + 3); return *this; }
    MyTestOutStream& putUint24(int /* value */)
                     { d_fun.push_back(k_UNSIGNED + k_INT + 3); return *this; }
    MyTestOutStream& putInt16(int /* value */)
                                  { d_fun.push_back(k_INT + 2); return *this; }
    MyTestOutStream& putUint16(int /* value */)
                     { d_fun.push_back(k_UNSIGNED + k_INT + 2); return *this; }
    MyTestOutStream& putInt8(int /* value */)
                                  { d_fun.push_back(k_INT + 1); return *this; }
    MyTestOutStream& putUint8(int /* value */)
                     { d_fun.push_back(k_UNSIGNED + k_INT + 1); return *this; }
    MyTestOutStream& putFloat64(double /* value */)
                                { d_fun.push_back(k_FLOAT + 8); return *this; }
    MyTestOutStream& putFloat32(float /* value */)
                                { d_fun.push_back(k_FLOAT + 4); return *this; }

    typedef bsls::Types::Uint64 Uint64;

    MyTestOutStream& putArrayInt64(const Int64 * /* array */, int /* count */)
                        { d_fun.push_back(k_ARRAY + k_INT + 8); return *this; }
    MyTestOutStream& putArrayUint64(const Uint64 * /* array */,
                                    int            /* count */)
           { d_fun.push_back(k_ARRAY + k_UNSIGNED + k_INT + 8); return *this; }
    MyTestOutStream& putArrayInt56(const Int64 * /* array */, int /* count */)
                        { d_fun.push_back(k_ARRAY + k_INT + 7); return *this; }
    MyTestOutStream& putArrayUint56(const Uint64 * /* array */,
                                    int            /* count */)
           { d_fun.push_back(k_ARRAY + k_UNSIGNED + k_INT + 7); return *this; }
    MyTestOutStream& putArrayInt48(const Int64 * /* array */, int /* count */)
                        { d_fun.push_back(k_ARRAY + k_INT + 6); return *this; }
    MyTestOutStream& putArrayUint48(const Uint64 * /* array */,
                                    int            /* count */)
           { d_fun.push_back(k_ARRAY + k_UNSIGNED + k_INT + 6); return *this; }
    MyTestOutStream& putArrayInt40(const Int64 * /* array */, int /* count */)
                        { d_fun.push_back(k_ARRAY + k_INT + 5); return *this; }
    MyTestOutStream& putArrayUint40(const Uint64 * /* array */,
                                    int            /* count */)
           { d_fun.push_back(k_ARRAY + k_UNSIGNED + k_INT + 5); return *this; }
    MyTestOutStream& putArrayInt32(const int * /* array */, int /* count */)
                        { d_fun.push_back(k_ARRAY + k_INT + 4); return *this; }
    MyTestOutStream& putArrayUint32(const unsigned int * /* array */,
                                    int                 /* count */)
           { d_fun.push_back(k_ARRAY + k_UNSIGNED + k_INT + 4); return *this; }
    MyTestOutStream& putArrayInt24(const int * /* array */, int /* count */)
                        { d_fun.push_back(k_ARRAY + k_INT + 3); return *this; }
    MyTestOutStream& putArrayUint24(const unsigned int * /* array */,
                                    int                 /* count */)
           { d_fun.push_back(k_ARRAY + k_UNSIGNED + k_INT + 3); return *this; }
    MyTestOutStream& putArrayInt16(const short * /* array */, int /* count */)
                        { d_fun.push_back(k_ARRAY + k_INT + 2); return *this; }
    MyTestOutStream& putArrayUint16(const unsigned short * /* array */,
                                    int                   /* count */)
           { d_fun.push_back(k_ARRAY + k_UNSIGNED + k_INT + 2); return *this; }
    MyTestOutStream& putArrayInt8(const signed char * /* array */,
                                  int                /* count */)
                        { d_fun.push_back(k_ARRAY + k_INT + 1); return *this; }
    MyTestOutStream& putArrayUint8(const unsigned char * /* array */,
                                   int                  /* count */)
           { d_fun.push_back(k_ARRAY + k_UNSIGNED + k_INT + 1); return *this; }
    MyTestOutStream& putArrayInt8(const char * /* array */, int /* count */)
                        { d_fun.push_back(k_ARRAY + k_INT + 1); return *this; }
    MyTestOutStream& putArrayUint8(const char * /* array */, int /* count */)
           { d_fun.push_back(k_ARRAY + k_UNSIGNED + k_INT + 1); return *this; }
    MyTestOutStream& putArrayFloat64(const double * /* array */,
                                     int           /* count */)
                      { d_fun.push_back(k_ARRAY + k_FLOAT + 8); return *this; }
    MyTestOutStream& putArrayFloat32(const float * /* array */,
                                     int           /* count */)
                      { d_fun.push_back(k_ARRAY + k_FLOAT + 4); return *this; }

    MyTestOutStream& putString(const bsl::string& /* value */)
                                   { d_fun.push_back(k_STRING); return *this; }

    // MANIPULATORS
    void invalidate() {}
    void removeAll() {}
    void reserveCapacity(int /* newCapacity */) {}
    void clear() { d_fun.clear();  d_lastVersion = -2; }

    // ACCESSORS
    operator const void *() const { return this; }
    const char *data() const { return 0; }
    int bdexVersionSelector() const { return d_versionSelector; }
    int length() const { return 0; }
    int size() const { return static_cast<int>(d_fun.size()); }
    int operator[](int index) const
                                 { return index < size() ? d_fun[index] : -1; }
    int lastVersion() const { return d_lastVersion; }
};

template <class TYPE>
struct TestWithVersion {
    // This class is a utility for verifying the results of
    // 'bdexStreamOut(stream, value, version)' applied to a (template
    // parameter) type 'TYPE', 'bsl::vector<TYPE>', and
    // 'bsl::vector<bsl::vector<TYPE> >'.

    static void test(const int               line,
                     int                     version,
                     const std::vector<int>& expectedFunctionIndicator,
                     bool                    usesArrayMethod = true)
    {
        using bslx::OutStreamFunctions::bdexStreamOut;

        MyTestOutStream stream(VERSION_SELECTOR);

        TYPE mValue;  const TYPE& value = mValue;
        bsl::vector<TYPE> mV;
        const bsl::vector<TYPE>& V = mV;
        bsl::vector<bsl::vector<TYPE> > mVV;
        const bsl::vector<bsl::vector<TYPE> >& VV = mVV;

        stream.clear();
        LOOP_ASSERT(line, &stream == &bdexStreamOut(stream, value, version));
        LOOP_ASSERT(line, expectedFunctionIndicator.size() == stream.size());
        for (int i = 0; i < expectedFunctionIndicator.size(); ++i) {
            LOOP_ASSERT(line, expectedFunctionIndicator[i] == stream[i]);
        }

        stream.clear();
        LOOP_ASSERT(line, &stream == &bdexStreamOut(stream, V, version));
        LOOP_ASSERT(line, 1 == stream.size());
        LOOP_ASSERT(line, MyTestOutStream::k_LENGTH == stream[0]);

        stream.clear();
        LOOP_ASSERT(line, &stream == &bdexStreamOut(stream, VV, version));
        LOOP_ASSERT(line, 1 == stream.size());
        LOOP_ASSERT(line, MyTestOutStream::k_LENGTH == stream[0]);


        mVV.push_back(V);

        stream.clear();
        LOOP_ASSERT(line, &stream == &bdexStreamOut(stream, VV, version));
        LOOP_ASSERT(line, 2 == stream.size());
        LOOP_ASSERT(line, MyTestOutStream::k_LENGTH == stream[0]);
        LOOP_ASSERT(line, MyTestOutStream::k_LENGTH == stream[1]);

        mV.push_back(value);

        stream.clear();
        LOOP_ASSERT(line, &stream == &bdexStreamOut(stream, V, version));
        if (usesArrayMethod) {
            LOOP_ASSERT(line, 1 == expectedFunctionIndicator.size());
            LOOP_ASSERT(line, 2 == stream.size());
            LOOP_ASSERT(line, MyTestOutStream::k_LENGTH == stream[0]);
            LOOP_ASSERT(line,
                        MyTestOutStream::k_ARRAY
                                  + expectedFunctionIndicator[0] == stream[1]);
        }
        else {
            LOOP_ASSERT(line,
                        expectedFunctionIndicator.size() + 1 == stream.size());
            LOOP_ASSERT(line, MyTestOutStream::k_LENGTH == stream[0]);
            for (int i = 0; i < expectedFunctionIndicator.size(); ++i) {
                LOOP_ASSERT(line,
                            expectedFunctionIndicator[i] == stream[i + 1]);
            }
        }

        mVV.push_back(V);

        stream.clear();
        LOOP_ASSERT(line, &stream == &bdexStreamOut(stream, VV, version));
        if (usesArrayMethod) {
            LOOP_ASSERT(line, 1 == expectedFunctionIndicator.size());
            LOOP_ASSERT(line, 4 == stream.size());
            LOOP_ASSERT(line, MyTestOutStream::k_LENGTH == stream[0]);
            LOOP_ASSERT(line, MyTestOutStream::k_LENGTH == stream[1]);
            LOOP_ASSERT(line, MyTestOutStream::k_LENGTH == stream[2]);
            LOOP_ASSERT(line,
                        MyTestOutStream::k_ARRAY
                                  + expectedFunctionIndicator[0] == stream[3]);
        }
        else {
            LOOP_ASSERT(line,
                        expectedFunctionIndicator.size() + 3 == stream.size());
            LOOP_ASSERT(line, MyTestOutStream::k_LENGTH == stream[0]);
            LOOP_ASSERT(line, MyTestOutStream::k_LENGTH == stream[1]);
            LOOP_ASSERT(line, MyTestOutStream::k_LENGTH == stream[2]);
            for (int i = 0; i < expectedFunctionIndicator.size(); ++i) {
                LOOP_ASSERT(line,
                            expectedFunctionIndicator[i] == stream[i + 3]);
            }
        }
    }
};

template <class TYPE>
struct TestWithoutVersion {
    // This class is a utility for verifying the results of
    // 'bdexStreamOut(stream, value)' applied to a (template parameter) type
    // 'TYPE', 'bsl::vector<TYPE>', and 'bsl::vector<bsl::vector<TYPE> >'.

    static void test(const int               line,
                     MyTestOutStream&        stream,
                     int                     expectedVersion,
                     const std::vector<int>& expectedFunctionIndicator,
                     bool                    usesArrayMethod = true,
                     bool                    externalizesVersion = false)
    {
        using bslx::OutStreamFunctions::bdexStreamOut;

        TYPE mValue;  const TYPE& value = mValue;
        bsl::vector<TYPE> mV;
        const bsl::vector<TYPE>& V = mV;
        bsl::vector<bsl::vector<TYPE> > mVV;
        const bsl::vector<bsl::vector<TYPE> >& VV = mVV;

        stream.clear();
        LOOP_ASSERT(line, &stream == &bdexStreamOut(stream, value));
        if (externalizesVersion) {
            LOOP_ASSERT(line,
                        expectedFunctionIndicator.size() + 1 == stream.size());
            LOOP_ASSERT(line, MyTestOutStream::k_VERSION == stream[0]);
            LOOP_ASSERT(line, expectedVersion == stream.lastVersion());
            for (int i = 0; i < expectedFunctionIndicator.size(); ++i) {
                LOOP_ASSERT(line,
                            expectedFunctionIndicator[i] == stream[i + 1]);
            }
        }
        else {
            LOOP_ASSERT(line,
                        expectedFunctionIndicator.size() == stream.size());
            for (int i = 0; i < expectedFunctionIndicator.size(); ++i) {
                LOOP_ASSERT(line, expectedFunctionIndicator[i] == stream[i]);
            }
        }

        stream.clear();
        LOOP_ASSERT(line, &stream == &bdexStreamOut(stream, V));
        LOOP_ASSERT(line, 2 == stream.size());
        LOOP_ASSERT(line, MyTestOutStream::k_VERSION == stream[0]);
        LOOP_ASSERT(line, expectedVersion == stream.lastVersion());
        LOOP_ASSERT(line, MyTestOutStream::k_LENGTH == stream[1]);

        stream.clear();
        LOOP_ASSERT(line, &stream == &bdexStreamOut(stream, VV));
        LOOP_ASSERT(line, 2 == stream.size());
        LOOP_ASSERT(line, MyTestOutStream::k_VERSION == stream[0]);
        LOOP_ASSERT(line, expectedVersion == stream.lastVersion());
        LOOP_ASSERT(line, MyTestOutStream::k_LENGTH == stream[1]);


        mVV.push_back(V);

        stream.clear();
        LOOP_ASSERT(line, &stream == &bdexStreamOut(stream, VV));
        LOOP_ASSERT(line, 3 == stream.size());
        LOOP_ASSERT(line, MyTestOutStream::k_VERSION == stream[0]);
        LOOP_ASSERT(line, expectedVersion == stream.lastVersion());
        LOOP_ASSERT(line, MyTestOutStream::k_LENGTH == stream[1]);
        LOOP_ASSERT(line, MyTestOutStream::k_LENGTH == stream[2]);

        mV.push_back(value);

        stream.clear();
        LOOP_ASSERT(line, &stream == &bdexStreamOut(stream, V));
        if (usesArrayMethod) {
            LOOP_ASSERT(line, 1 == expectedFunctionIndicator.size());
            LOOP_ASSERT(line, 3 == stream.size());
            LOOP_ASSERT(line, MyTestOutStream::k_VERSION == stream[0]);
            LOOP_ASSERT(line, expectedVersion == stream.lastVersion());
            LOOP_ASSERT(line, MyTestOutStream::k_LENGTH == stream[1]);
            LOOP_ASSERT(line,
                        MyTestOutStream::k_ARRAY
                                  + expectedFunctionIndicator[0] == stream[2]);
        }
        else {
            LOOP_ASSERT(line,
                        expectedFunctionIndicator.size() + 2 == stream.size());
            LOOP_ASSERT(line, MyTestOutStream::k_VERSION == stream[0]);
            LOOP_ASSERT(line, expectedVersion == stream.lastVersion());
            LOOP_ASSERT(line, MyTestOutStream::k_LENGTH == stream[1]);
            for (int i = 0; i < expectedFunctionIndicator.size(); ++i) {
                LOOP_ASSERT(line,
                            expectedFunctionIndicator[i] == stream[i + 2]);
            }
        }

        mVV.push_back(V);

        stream.clear();
        LOOP_ASSERT(line, &stream == &bdexStreamOut(stream, VV));
        if (usesArrayMethod) {
            LOOP_ASSERT(line, 1 == expectedFunctionIndicator.size());
            LOOP_ASSERT(line, 5 == stream.size());
            LOOP_ASSERT(line, MyTestOutStream::k_VERSION == stream[0]);
            LOOP_ASSERT(line, expectedVersion == stream.lastVersion());
            LOOP_ASSERT(line, MyTestOutStream::k_LENGTH == stream[1]);
            LOOP_ASSERT(line, MyTestOutStream::k_LENGTH == stream[2]);
            LOOP_ASSERT(line, MyTestOutStream::k_LENGTH == stream[3]);
            LOOP_ASSERT(line,
                        MyTestOutStream::k_ARRAY
                                  + expectedFunctionIndicator[0] == stream[4]);
        }
        else {
            LOOP_ASSERT(line,
                        expectedFunctionIndicator.size() + 4 == stream.size());
            LOOP_ASSERT(line, MyTestOutStream::k_VERSION == stream[0]);
            LOOP_ASSERT(line, expectedVersion == stream.lastVersion());
            LOOP_ASSERT(line, MyTestOutStream::k_LENGTH == stream[1]);
            LOOP_ASSERT(line, MyTestOutStream::k_LENGTH == stream[2]);
            LOOP_ASSERT(line, MyTestOutStream::k_LENGTH == stream[3]);
            for (int i = 0; i < expectedFunctionIndicator.size(); ++i) {
                LOOP_ASSERT(line,
                            expectedFunctionIndicator[i] == stream[i + 4]);
            }
        }
    }
};

// ============================================================================
//                                 USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using 'bslx::OutStreamFunctions' to Externalize Data
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example we illustrate the primary intended use of the parameterized
// methods of this component, as well as a few trivial invocations just to show
// the syntax clearly.  To accomplish this, we exhibit two separate example
// "components": a value-semantic point object, and an 'enum'.  In all cases,
// the component designs are very simple, with much of the implied
// functionality omitted, in order to focus attention on the key aspects of the
// functionality of *this* component.
//
// First, consider an 'enum' 'Color' that enumerates a set of colors:
//..
    enum Color {
        RED   = 0,
        GREEN = 1,
        BLUE  = 2
    };
//..
// Next, we consider a very special-purpose point that has as a data member its
// color.  Such a point provides an excellent opportunity for factoring, but
// since we are interested in highlighting BDEX streaming of various types, we
// will present a simple and unfactored design here.  In a real-world problem,
// the 'mypoint' component would be implemented differently.
//
// Note that the 'MyPoint' class in this example represents its coordinates as
// 'short' integer values; this is done to make the BDEX stream output byte
// pattern somewhat easier for the reader of this example to recognize when the
// output buffer is printed.
//..
    // mypoint.h

    class MyPoint {
        // This class provides a geometric point having integer coordinates and
        // an enumerated color property.

        short d_x;      // x coordinate
        short d_y;      // y coordinate
        Color d_color;  // enumerated color property

      public:
        // CLASS METHODS
        static int maxSupportedBdexVersion(int versionSelector);
            // Return the maximum valid BDEX format version, as indicated by
            // the specified 'versionSelector', to be passed to the
            // 'bdexStreamOut' method.  Note that it is highly recommended that
            // versionSelector' be formatted as "YYYYMMDD", a date
            // representation.  Also note that 'versionSelector' should be a
            // *compile*-time-chosen value that selects a format version
            // supported by both externalizer and unexternalizer.  See the
            // 'bslx' package-level documentation for more information on BDEX
            // streaming of value-semantic types and containers.

        // CREATORS
        MyPoint();
            // Create a default point.

        MyPoint(short x, short y, Color color);
            // Create a point having the specified 'x' and 'y' coordinates and
            // the specified 'color'.

        ~MyPoint();
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
        STREAM& bdexStreamOut(STREAM& stream, int version) const;
            // Write the value of this object, using the specified 'version'
            // format, to the specified output 'stream', and return a reference
            // to 'stream'.  If 'stream' is initially invalid, this operation
            // has no effect.  If 'version' is not supported, 'stream' is
            // invalidated, but otherwise unmodified.  Note that 'version' is
            // not written to 'stream'.  See the 'bslx' package-level
            // documentation for more information on BDEX streaming of
            // value-semantic types and containers.
    };

    // FREE OPERATORS
    inline
    bool operator==(const MyPoint& lhs, const MyPoint& rhs);
        // Return 'true' if the specified 'lhs' and 'rhs' points have the same
        // value, and 'false' otherwise.  Two points have the same value if
        // they have the same x and y coordinates and the same color.
//..
// Representative (inline) implementations of these methods are shown below:
//..
    // ========================================================================
    //                      INLINE FUNCTION DEFINITIONS
    // ========================================================================

    // CLASS METHODS
    inline
    int MyPoint::maxSupportedBdexVersion(int versionSelector)
    {
        if (versionSelector >= 20131201) {
            return 2;                                                 // RETURN
        }
        return 1;
    }

    // CREATORS
    inline
    MyPoint::MyPoint(short x, short y, Color color)
    : d_x(x)
    , d_y(y)
    , d_color(color)
    {
    }

    inline
    MyPoint::~MyPoint()
    {
    }

    // ...

    // MANIPULATORS
    // ...

    // ACCESSORS
    inline
    int MyPoint::x() const
    {
        return d_x;
    }

    // ...

    template <class STREAM>
    STREAM& MyPoint::bdexStreamOut(STREAM& stream, int version) const
    {
        switch (version) {
          case 1: {
            stream.putInt16(d_x);          // output the x coordinate
            stream.putInt16(d_y);          // output the y coordinate
            stream.putInt8(static_cast<char>(d_color));
                                           // output the color enum as one byte
          } break;
          default: {
            stream.invalidate();
          } break;
        }
        return stream;
    }

    // FREE OPERATORS
    inline
    bool operator==(const MyPoint& lhs, const MyPoint& rhs)
    {
        return lhs.x()     == rhs.x()
            && lhs.y()     == rhs.y()
            && lhs.color() == rhs.color();
    }
//..
// Then, we will implement an extremely simple output stream that supports the
// BDEX documentation-only protocol.  For simplicity, we will use a fixed-size
// buffer (usually a bad idea in any event, and more so here since the
// implementation knows the buffer size, but makes no effort to prevent
// overwriting that buffer), and will only show a few methods needed for this
// example.  See other 'bslx' stream components for examples of
// properly-designed BDEX streams.
//..
    // myoutstream.h
    // ...

    class MyOutStream {
        // This class implements a limited-size fixed-buffer output stream that
        // partially conforms to the BDEX protocol for output streams.  This
        // class is suitable for demonstration purposes only.

        char d_buffer[1000]; // externalized values stored as contiguous bytes

        int  d_length;       // length of data in 'd_buffer' (in bytes)

        bool d_validFlag;    // stream validity flag; 'true' if stream is in
                             // valid state, 'false' otherwise

      public:
        // CREATORS
        MyOutStream();
            // Create an empty output stream of limited, fixed capacity.  Note
            // that this object is suitable for demonstration purposes only.

        ~MyOutStream();
           // Destroy this output stream.

        // MANIPULATORS
        void invalidate();
            // Put this input stream in an invalid state.  This function has no
            // effect if this stream is already invalid.  Note that this
            // function should be called whenever a value extracted from this
            // stream is determined to be invalid, inconsistent, or otherwise
            // incorrect.

        MyOutStream& putVersion(int version);
            // Write to this stream the one-byte, two's complement integer
            // comprised of the least-significant one byte of the specified
            // 'version', and return a reference to this stream.

        MyOutStream& putInt32(int value);
            // Write to this stream the four-byte, two's complement integer (in
            // network byte order) comprised of the least-significant four
            // bytes of the specified 'value' (in host byte order), and return
            // a reference to this stream.

        MyOutStream& putInt16(int value);
            // Write to this stream the two-byte, two's complement integer (in
            // network byte order) comprised of the least-significant two bytes
            // of the specified 'value' (in host byte order), and return a
            // reference to this stream.

        MyOutStream& putInt8(int value);
            // Write to this stream the one-byte, two's complement integer
            // comprised of the least-significant one byte of the specified
            // 'value', and return a reference to this stream.

        void removeAll();
            // Remove all content in this stream.

        // ACCESSORS
        const char *data() const;
            // Return the address of the contiguous, non-modifiable internal
            // memory buffer of this stream.  The address will remain valid as
            // long as this stream is not destroyed or modified.  The behavior
            // of accessing elements outside the range
            // '[ data() .. data() + (length() - 1) ]' is undefined.

        int length() const;
            // Return the number of bytes in this stream.
    };

    // FREE OPERATORS
    inline
    bsl::ostream& operator<<(bsl::ostream&      stream,
                             const MyOutStream& object);
        // Write the specified 'object' to the specified output 'stream' in
        // some reasonable (multi-line) format, and return a reference to
        // 'stream'.
//..
// The relevant (inline) implementations are as follows.
//..
    // ========================================================================
    //                      INLINE FUNCTION DEFINITIONS
    // ========================================================================

    // CREATORS
    inline
    MyOutStream::MyOutStream()
    : d_length(0)
    , d_validFlag(true)
    {
    }

    inline
    MyOutStream::~MyOutStream()
    {
    }

    // MANIPULATORS
    inline
    void MyOutStream::invalidate()
    {
        d_validFlag = false;
    }

    inline
    MyOutStream& MyOutStream::putVersion(int value)
    {
        d_buffer[d_length] = static_cast<char>(value);
        ++d_length;
        return *this;
    }

    inline
    MyOutStream& MyOutStream::putInt32(int value)
    {
        d_buffer[d_length + 0] = static_cast<char>((value >> 24) & 0xff);
        d_buffer[d_length + 1] = static_cast<char>((value >> 16) & 0xff);
        d_buffer[d_length + 2] = static_cast<char>((value >>  8) & 0xff);
        d_buffer[d_length + 3] = static_cast<char>((value >>  0) & 0xff);
        d_length += 4;
        return *this;
    }

    inline
    MyOutStream& MyOutStream::putInt16(int value)
    {
        d_buffer[d_length + 0] = static_cast<char>((value >> 8) & 0xff);
        d_buffer[d_length + 1] = static_cast<char>((value >> 0) & 0xff);
        d_length += 2;
        return *this;
    }

    inline
    MyOutStream& MyOutStream::putInt8(int value)
    {
        d_buffer[d_length] = static_cast<char>(value);
        d_length += 1;
        return *this;
    }

    inline
    void MyOutStream::removeAll()
    {
        d_length = 0;
    }

    // ACCESSORS
    inline
    const char *MyOutStream::data() const
    {
        return static_cast<const char *>(d_buffer);
    }

    inline
    int MyOutStream::length() const
    {
        return d_length;
    }
//..

// ============================================================================
//                                 MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file must
        //:   compile, link, and run as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, replace
        //:   leading comment characters with spaces, replace 'assert' with
        //:   'ASSERT', and insert 'if (veryVerbose)' before all output
        //:   operations.  (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing Usage Examples" << endl
                                  << "======================" << endl;

// Finally, use the above 'enum', point class, and output stream to illustrate
// 'bslx::OutStreamFunctions' functionality.  This test code does not attempt
// to do anything more useful than writing known values to a stream and
// confirming that the expected byte pattern was in fact written.
//..
    int             i     = 168496141;  // byte pattern 0a 0b 0c 0d
    Color           color = BLUE;       // byte pattern 02
    MyPoint         p(0, -1, color);    // byte pattern 00 00 ff ff 02

    using bslx::OutStreamFunctions::bdexStreamOut;

    MyOutStream out;
    ASSERT(0 == out.length());

    bdexStreamOut(out, i, 1);
    ASSERT(4 == out.length());
    ASSERT(0 == bsl::memcmp(out.data(), "\x0a\x0b\x0c\x0d", out.length()));

    out.removeAll();
    ASSERT(0 == out.length());

    bdexStreamOut(out, i, 0);
    ASSERT(4 == out.length());
    ASSERT(0 == bsl::memcmp(out.data(), "\x0a\x0b\x0c\x0d", out.length()));

    out.removeAll();
    ASSERT(0 == out.length());

    bdexStreamOut(out, color, 1);
    ASSERT(4 == out.length());
    ASSERT(0 == bsl::memcmp(out.data(), "\x00\x00\x00\x02", out.length()));

    out.removeAll();
    ASSERT(0 == out.length());

    bdexStreamOut(out, color, 0);
    ASSERT(4 == out.length());
    ASSERT(0 == bsl::memcmp(out.data(), "\x00\x00\x00\x02", out.length()));

    out.removeAll();
    ASSERT(0 == out.length());

    bdexStreamOut(out, p, 1);
    ASSERT(5 == out.length());
    ASSERT(0 == bsl::memcmp(out.data(), "\x00\x00\xff\xff\x02", out.length()));
//..

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'bdexStreamOut(stream, value)'
        //   Ensure the 'bdexStreamOut' methods forward to the correct stream
        //   methods and externalize the version where required.
        //
        // Concerns:
        //: 1 The correct methods on 'stream' are invoked.
        //:
        //: 2 Testing covers all 'stream' methods.
        //:
        //: 3 Non-directly supported vectors are externalized correctly.
        //:
        //: 4 The version is forwarded correctly.
        //:
        //: 5 The version is externalized only where appropriate.
        //:
        //: 6 Vectors correctly externalize the version.
        //:
        //: 7 The version is indirectly obtained from the stream where
        //:    appropriate.
        //
        // Plan:
        //: 1 Create a test stream object that will track invoked methods.
        //:
        //: 2 Create a test object which externalizes differently for different
        //:   versions.
        //:
        //: 3 Externalize a set of objects which cover all directly supported
        //:   BDEX types and verify correct method forwarding.  (C-1..2)
        //:
        //: 4 Externalize vectors of a test object and verify correct method
        //:   forwarding.  (C-3)
        //:
        //: 5 Externalize a test object which externalizes differently for
        //:   different versions and vectors of this type with different
        //:   supplied versions; verify correct method forwarding.  (C-4)
        //:
        //: 6 Verify version is correctly externalized in all tests.  (C-5)
        //:
        //: 7 Repeat tests with different 'versionSelector' in the stream
        //:   constructor and verify results.  (C-7)
        //
        // Testing:
        //   bdexStreamOut(STREAM& stream, const TYPE& value)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'bdexStreamOut(stream, value)'" << endl
                          << "======================================" << endl;

        {  // first choice of versionSelector
            MyTestOutStream stream(OTHER_VERSION_SELECTOR);

            std::vector<int> exp;

            exp.clear();
            exp.push_back(MyTestOutStream::k_INT + 1);
            TestWithoutVersion<bool                 >::test(L_,
                                                            stream,
                                                            1,
                                                            exp,
                                                            false);

            exp.clear();
            exp.push_back(MyTestOutStream::k_INT + 1);
            TestWithoutVersion<char                 >::test(L_,
                                                            stream,
                                                            1,
                                                            exp);

            exp.clear();
            exp.push_back(MyTestOutStream::k_INT + 1);
            TestWithoutVersion<signed char          >::test(L_,
                                                            stream,
                                                            1,
                                                            exp);

            exp.clear();
            exp.push_back(MyTestOutStream::k_UINT + 1);
            TestWithoutVersion<unsigned char        >::test(L_,
                                                            stream,
                                                            1,
                                                            exp);

            exp.clear();
            exp.push_back(MyTestOutStream::k_INT + 2);
            TestWithoutVersion<short                >::test(L_,
                                                            stream,
                                                            1,
                                                            exp);

            exp.clear();
            exp.push_back(MyTestOutStream::k_UINT + 2);
            TestWithoutVersion<unsigned short       >::test(L_,
                                                            stream,
                                                            1,
                                                            exp);

            exp.clear();
            exp.push_back(MyTestOutStream::k_INT + 4);
            TestWithoutVersion<int                  >::test(L_,
                                                            stream,
                                                            1,
                                                            exp);

            exp.clear();
            exp.push_back(MyTestOutStream::k_UINT + 4);
            TestWithoutVersion<unsigned int         >::test(L_,
                                                            stream,
                                                            1,
                                                            exp);

            exp.clear();
            exp.push_back(MyTestOutStream::k_INT + 4);
            TestWithoutVersion<long                 >::test(L_,
                                                            stream,
                                                            1,
                                                            exp,
                                                            false);

            exp.clear();
            exp.push_back(MyTestOutStream::k_UINT + 4);
            TestWithoutVersion<unsigned long        >::test(L_,
                                                            stream,
                                                            1,
                                                            exp,
                                                            false);

            exp.clear();
            exp.push_back(MyTestOutStream::k_INT + 8);
            TestWithoutVersion<Int64                >::test(L_,
                                                            stream,
                                                            1,
                                                            exp);

            exp.clear();
            exp.push_back(MyTestOutStream::k_UINT + 8);
            TestWithoutVersion<Uint64               >::test(L_,
                                                            stream,
                                                            1,
                                                            exp);

            exp.clear();
            exp.push_back(MyTestOutStream::k_FLOAT + 8);
            TestWithoutVersion<double               >::test(L_,
                                                            stream,
                                                            1,
                                                            exp);

            exp.clear();
            exp.push_back(MyTestOutStream::k_FLOAT + 4);
            TestWithoutVersion<float                >::test(L_,
                                                            stream,
                                                            1,
                                                            exp);

            exp.clear();
            exp.push_back(MyTestOutStream::k_STRING);
            TestWithoutVersion<bsl::string          >::test(L_,
                                                            stream,
                                                            1,
                                                            exp,
                                                            false);

            exp.clear();
            exp.push_back(MyTestOutStream::k_INT + 4);
            TestWithoutVersion<MyTestEnum           >::test(L_,
                                                            stream,
                                                            1,
                                                            exp,
                                                            false);

            exp.clear();
            exp.push_back(MyTestOutStream::k_FLOAT + 4);
            TestWithoutVersion<MySpecializedTestEnum>::test(L_,
                                                            stream,
                                                            1,
                                                            exp,
                                                            false);

            exp.clear();
            exp.push_back(MyTestOutStream::k_INT + 1);
            TestWithoutVersion<ThirdParty::MyThirdPartyTestEnum>::test(L_,
                                                                       stream,
                                                                       1,
                                                                       exp,
                                                                       false);

            exp.clear();
            exp.push_back(MyTestOutStream::k_INT + 4);
            exp.push_back(MyTestOutStream::k_FLOAT + 4);
            TestWithoutVersion<MyTestClass          >::test(L_,
                                                            stream,
                                                            1,
                                                            exp,
                                                            false,
                                                            true);
        }
        {  // second choice of versionSelector
            using namespace OutStreamFunctions;

            MyTestOutStream stream(VERSION_SELECTOR);

            std::vector<int> exp;

            exp.clear();
            exp.push_back(MyTestOutStream::k_INT + 1);
            TestWithoutVersion<bool                 >::test(L_,
                                                            stream,
                                                            1,
                                                            exp,
                                                            false);

            exp.clear();
            exp.push_back(MyTestOutStream::k_INT + 1);
            TestWithoutVersion<char                 >::test(L_,
                                                            stream,
                                                            1,
                                                            exp);

            exp.clear();
            exp.push_back(MyTestOutStream::k_INT + 1);
            TestWithoutVersion<signed char          >::test(L_,
                                                            stream,
                                                            1,
                                                            exp);

            exp.clear();
            exp.push_back(MyTestOutStream::k_UINT + 1);
            TestWithoutVersion<unsigned char        >::test(L_,
                                                            stream,
                                                            1,
                                                            exp);

            exp.clear();
            exp.push_back(MyTestOutStream::k_INT + 2);
            TestWithoutVersion<short                >::test(L_,
                                                            stream,
                                                            1,
                                                            exp);

            exp.clear();
            exp.push_back(MyTestOutStream::k_UINT + 2);
            TestWithoutVersion<unsigned short       >::test(L_,
                                                            stream,
                                                            1,
                                                            exp);

            exp.clear();
            exp.push_back(MyTestOutStream::k_INT + 4);
            TestWithoutVersion<int                  >::test(L_,
                                                            stream,
                                                            1,
                                                            exp);

            exp.clear();
            exp.push_back(MyTestOutStream::k_UINT + 4);
            TestWithoutVersion<unsigned int         >::test(L_,
                                                            stream,
                                                            1,
                                                            exp);

            exp.clear();
            exp.push_back(MyTestOutStream::k_INT + 4);
            TestWithoutVersion<long                 >::test(L_,
                                                            stream,
                                                            1,
                                                            exp,
                                                            false);

            exp.clear();
            exp.push_back(MyTestOutStream::k_UINT + 4);
            TestWithoutVersion<unsigned long        >::test(L_,
                                                            stream,
                                                            1,
                                                            exp,
                                                            false);

            exp.clear();
            exp.push_back(MyTestOutStream::k_INT + 8);
            TestWithoutVersion<Int64                >::test(L_,
                                                            stream,
                                                            1,
                                                            exp);

            exp.clear();
            exp.push_back(MyTestOutStream::k_UINT + 8);
            TestWithoutVersion<Uint64               >::test(L_,
                                                            stream,
                                                            1,
                                                            exp);

            exp.clear();
            exp.push_back(MyTestOutStream::k_FLOAT + 8);
            TestWithoutVersion<double               >::test(L_,
                                                            stream,
                                                            1,
                                                            exp);

            exp.clear();
            exp.push_back(MyTestOutStream::k_FLOAT + 4);
            TestWithoutVersion<float                >::test(L_,
                                                            stream,
                                                            1,
                                                            exp);

            exp.clear();
            exp.push_back(MyTestOutStream::k_STRING);
            TestWithoutVersion<bsl::string          >::test(L_,
                                                            stream,
                                                            1,
                                                            exp,
                                                            false);

            exp.clear();
            exp.push_back(MyTestOutStream::k_INT + 4);
            TestWithoutVersion<MyTestEnum           >::test(L_,
                                                            stream,
                                                            1,
                                                            exp,
                                                            false);

            exp.clear();
            exp.push_back(MyTestOutStream::k_FLOAT + 4);
            TestWithoutVersion<MySpecializedTestEnum>::test(L_,
                                                            stream,
                                                            1,
                                                            exp,
                                                            false);

            exp.clear();
            exp.push_back(MyTestOutStream::k_INT + 1);
            TestWithoutVersion<ThirdParty::MyThirdPartyTestEnum>::test(L_,
                                                                       stream,
                                                                       1,
                                                                       exp,
                                                                       false);

            exp.clear();
            exp.push_back(MyTestOutStream::k_INT + 4);
            exp.push_back(MyTestOutStream::k_FLOAT + 8);
            TestWithoutVersion<MyTestClass          >::test(L_,
                                                            stream,
                                                            2,
                                                            exp,
                                                            false,
                                                            true);
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'bdexStreamOut(stream, value, version)'
        //   Ensure the 'bdexStreamOut' methods forward to the correct stream
        //   methods.
        //
        // Concerns:
        //: 1 The correct methods on 'stream' are invoked.
        //:
        //: 2 Testing covers all 'stream' methods.
        //:
        //: 3 Non-directly supported vectors are externalized correctly.
        //:
        //: 4 The version is forwarded correctly.
        //
        // Plan:
        //: 1 Create a test stream object that will track invoked methods.
        //:
        //: 2 Create a test object which externalizes differently for different
        //:   versions.
        //:
        //: 3 Externalize a set of objects which cover all directly supported
        //:   BDEX types and verify correct method forwarding.  (C-1..2)
        //:
        //: 4 Externalize vectors of a test object and verify correct method
        //:   forwarding.  (C-3)
        //:
        //: 5 Externalize a test object which externalizes differently for
        //:   different versions and vectors of this type with different
        //:   supplied versions; verify correct method forwarding.  (C-4)
        //
        // Testing:
        //   bdexStreamOut(STREAM& stream, const TYPE& value, int version)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'bdexStreamOut(stream, value, version)'"
                          << endl
                          << "==============================================="
                          << endl;

        {
            std::vector<int> exp;

            exp.clear();
            exp.push_back(MyTestOutStream::k_INT + 1);
            TestWithVersion<bool                 >::test(L_, 1, exp, false);

            exp.clear();
            exp.push_back(MyTestOutStream::k_INT + 1);
            TestWithVersion<char                 >::test(L_, 1, exp);

            exp.clear();
            exp.push_back(MyTestOutStream::k_INT + 1);
            TestWithVersion<signed char          >::test(L_, 1, exp);

            exp.clear();
            exp.push_back(MyTestOutStream::k_UINT + 1);
            TestWithVersion<unsigned char        >::test(L_, 1, exp);

            exp.clear();
            exp.push_back(MyTestOutStream::k_INT + 2);
            TestWithVersion<short                >::test(L_, 1, exp);

            exp.clear();
            exp.push_back(MyTestOutStream::k_UINT + 2);
            TestWithVersion<unsigned short       >::test(L_, 1, exp);

            exp.clear();
            exp.push_back(MyTestOutStream::k_INT + 4);
            TestWithVersion<int                  >::test(L_, 1, exp);

            exp.clear();
            exp.push_back(MyTestOutStream::k_UINT + 4);
            TestWithVersion<unsigned int         >::test(L_, 1, exp);

            exp.clear();
            exp.push_back(MyTestOutStream::k_INT + 4);
            TestWithVersion<long                 >::test(L_, 1, exp, false);

            exp.clear();
            exp.push_back(MyTestOutStream::k_UINT + 4);
            TestWithVersion<unsigned long        >::test(L_, 1, exp, false);

            exp.clear();
            exp.push_back(MyTestOutStream::k_INT + 8);
            TestWithVersion<Int64                >::test(L_, 1, exp);

            exp.clear();
            exp.push_back(MyTestOutStream::k_UINT + 8);
            TestWithVersion<Uint64               >::test(L_, 1, exp);

            exp.clear();
            exp.push_back(MyTestOutStream::k_FLOAT + 8);
            TestWithVersion<double               >::test(L_, 1, exp);

            exp.clear();
            exp.push_back(MyTestOutStream::k_FLOAT + 4);
            TestWithVersion<float                >::test(L_, 1, exp);

            exp.clear();
            exp.push_back(MyTestOutStream::k_STRING);
            TestWithVersion<bsl::string          >::test(L_, 1, exp, false);

            exp.clear();
            exp.push_back(MyTestOutStream::k_INT + 4);
            TestWithVersion<MyTestEnum           >::test(L_, 1, exp, false);

            exp.clear();
            exp.push_back(MyTestOutStream::k_FLOAT + 4);
            TestWithVersion<MySpecializedTestEnum>::test(L_, 1, exp, false);

            exp.clear();
            exp.push_back(MyTestOutStream::k_INT + 1);
            TestWithVersion<ThirdParty::MyThirdPartyTestEnum>::test(L_,
                                                                    1,
                                                                    exp,
                                                                    false);

            exp.clear();
            exp.push_back(MyTestOutStream::k_INT + 4);
            exp.push_back(MyTestOutStream::k_FLOAT + 4);
            TestWithVersion<MyTestClass          >::test(L_, 1, exp, false);

            exp.clear();
            exp.push_back(MyTestOutStream::k_INT + 4);
            exp.push_back(MyTestOutStream::k_FLOAT + 8);
            TestWithVersion<MyTestClass          >::test(L_, 2, exp, false);
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

// ----------------------------------------------------------------------------
// Copyright 2014 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
