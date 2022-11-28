// bslx_instreamfunctions.t.cpp                                       -*-C++-*-

#include <bslx_instreamfunctions.h>

#include <bslma_sequentialallocator.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <bsl_climits.h>
#include <bsl_cstdlib.h>
#include <bsl_deque.h>
#include <bsl_iostream.h>
#include <bsl_vector.h>

#ifdef BSLS_PLATFORM_CMP_CLANG
#pragma clang diagnostic ignored "-Wunknown-warning-option"
#endif // BSLS_PLATFORM_CMP_CLANG

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
// [ 2] bdexStreamIn(STREAM& stream, TYPE& value);
// [ 1] bdexStreamIn(STREAM& stream, TYPE& value, int version);
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

// ============================================================================
//              Classes, functions, etc., needed for Usage Example
// ----------------------------------------------------------------------------
// In this example we illustrate the primary intended use of the parameterized
// methods of this component, as well as a few trivial invocations just to show
// the syntax clearly.  To accomplish this, we exhibit three separate example
// "components": an 'enum', a value-semantic point object, and an input stream.
// In all cases, the component designs are very simple, with much of the
// implied functionality omitted, in order to focus attention on the key
// aspects of the functionality of *this* component.
//
// First, consider an 'enum' 'Color' that enumerates a set of colors.
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
// 'short' integer values; this is done to make the BDEX stream input byte
// pattern somewhat easier for the reader of this example to recognize when the
// input buffer is printed.
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
        // ...

        // CREATORS
        MyPoint();
            // Create a default point.

        MyPoint(short x, short y, Color color);
            // Create a point having the specified 'x' and 'y' coordinates
            // and the specified 'color'.

        ~MyPoint();
            // Destroy this point.

        // MANIPULATORS
        // ...

        // ACCESSORS
        short x() const;
            // Return the x coordinate of this point.

        short y() const;
            // Return the y coordinate of this point.

        Color color() const;
            // Return the enumerated color of this point.

        template <class STREAM>
        STREAM& bdexStreamIn(STREAM& stream, int version);
            // Assign to this object the value read from the specified input
            // 'stream' using the specified 'version' format, and return a
            // reference to 'stream'.  If 'stream' is initially invalid, this
            // operation has no effect.  If 'version' is not supported, this
            // object is unaltered and 'stream' is invalidated, but otherwise
            // unmodified.  If 'version' is supported but 'stream' becomes
            // invalid during this operation, this object has an undefined, but
            // valid, state.  Note that no version is read from 'stream'.  See
            // the 'bslx' package-level documentation for more information on
            // BDEX streaming of value-semantic types and containers.
    };

    // FREE OPERATORS
    inline
    bool operator==(const MyPoint& lhs, const MyPoint& rhs);
        // Return 'true' if the specified 'lhs' and 'rhs' points have the same
        // value, and 'false' otherwise.  Two points have the same value if
        // they have the same x and y coordinates and the same color.
//..
// Representative (inline) implementations of these methods are shown below.
//..
// ============================================================================
//                          INLINE FUNCTION DEFINITIONS
// ============================================================================

// CREATORS
inline
MyPoint::MyPoint()
{
}

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
Color MyPoint::color() const
{
    return d_color;
}

inline
short MyPoint::x() const
{
    return d_x;
}

inline
short MyPoint::y() const
{
    return d_y;
}

// ...

template <class STREAM>
STREAM& MyPoint::bdexStreamIn(STREAM& stream, int version)
{
    switch (version) {
      case 1: {
        stream.getInt16(d_x);           // input the x coordinate
        stream.getInt16(d_y);           // input the y coordinate
        char color;
        stream.getInt8(color);          // input the color enum as one byte
        d_color = static_cast<Color>(color);
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
    return lhs.x()     == rhs.x() &&
           lhs.y()     == rhs.y() &&
           lhs.color() == rhs.color();
}
//..
// Then, we will implement an extremely simple input stream that supports the
// BDEX documentation-only protocol.  For simplicity, we will use an externally
// managed buffer, and will only show a few methods needed for this example.
//..
// myinstream.h
// ...

class MyInStream {
    // This class implements a limited-size fixed-buffer input stream that
    // partially conforms to the BDEX protocol for input streams.  This class
    // is suitable for demonstration purposes only.

    const char *d_buffer;  // input buffer, held but not owned
    int         d_length;  // length of 'd_buffer' (bytes)
    int         d_cursor;  // cursor (index into 'd_buffer')

  public:
    // CREATORS
    MyInStream(const char *buffer, int length);
        // Create an input stream using the specified 'buffer' having the
        // specified 'length' (in bytes).

    ~MyInStream();
        // Destroy this input byte stream.

    // MANIPULATORS
    MyInStream& getVersion(int& version);
        // Consume a version value from this input stream, store that value in
        // the specified 'version', and return a reference to this stream.  ...

    MyInStream& getInt32(int& value);
        // Consume a 32-bit signed integer value from this input stream, store
        // that value in the specified 'value', and return a reference to this
        // stream.  ...

    MyInStream& getInt16(short& value);
        // Consume a 16-bit signed integer value from this input stream, store
        // that value in the specified 'value', and return a reference to this
        // stream.  ...

    MyInStream& getInt8(char& value);
        // Consume an 8-bit signed integer value from this input stream, store
        // that value in the specified 'value', and return a reference to this
        // stream.  ...

    void invalidate();
        // Put this input stream in an invalid state.  ...

    // ACCESSORS
    operator const void *() const;
        // Return a non-zero value if this stream is valid, and 0 otherwise.
        // An invalid stream is a stream in which insufficient or invalid data
        // was detected during an extraction operation.  Note that an empty
        // stream will be valid unless an extraction attempt or explicit
        // invalidation causes it to be otherwise.

    int cursor() const;
        // Return the index of the next byte to be extracted from this stream.

    bool isEmpty() const;
        // Return 'true' if this stream is empty, and 'false' otherwise.
        // Note that this function enables higher-level types to verify
        // that, after successfully reading all expected data, no data
        // remains.

    int length() const;
        // Return the total number of bytes stored in this stream.
};

//..
// The relevant (inline) implementations are as follows.
//..
// ============================================================================
//                          INLINE FUNCTION DEFINITIONS
// ============================================================================

// CREATORS
inline
MyInStream::MyInStream(const char *buffer, int length)
: d_buffer(buffer)
, d_length(length)
, d_cursor(0)
{
}

inline
MyInStream::~MyInStream()
{
}

// MANIPULATORS

inline
MyInStream& MyInStream::getVersion(int& value)
{
    value = static_cast<unsigned char>(d_buffer[d_cursor++]);
    return *this;
}

inline
MyInStream& MyInStream::getInt32(int& value)
{
    const unsigned char *buffer =
                             reinterpret_cast<const unsigned char *>(d_buffer);
    value = static_cast<int>((buffer[d_cursor    ] << 24U) |
                             (buffer[d_cursor + 1] << 16U) |
                             (buffer[d_cursor + 2] <<  8U) |
                             (buffer[d_cursor + 3]       ));
    d_cursor += 4;
    return *this;
}

inline
MyInStream& MyInStream::getInt16(short& value)
{
    const unsigned char *buffer =
                             reinterpret_cast<const unsigned char *>(d_buffer);
    value = static_cast<short>((buffer[d_cursor    ] <<  8) |
                               (buffer[d_cursor + 1]      ));
    d_cursor += 2;
    return *this;
 }

inline
MyInStream& MyInStream::getInt8(char& value)
{
    value = d_buffer[d_cursor];
    d_cursor += 1;
    return *this;
}

inline
void MyInStream::invalidate()
{
    d_buffer = 0;
}

// ACCESSORS
inline
MyInStream::operator const void *() const
{
    return d_cursor <= d_length ? d_buffer : 0;
}

inline
int MyInStream::cursor() const
{
    return d_cursor;
}

inline
bool MyInStream::isEmpty() const
{
    return d_cursor >= d_length;
}

inline
int MyInStream::length() const
{
    return d_length;
}
//..

// ============================================================================
//                      GLOBAL TEST CLASSES
// ----------------------------------------------------------------------------

class MyTestInStream {
    // Test class used to test streaming.

    bsl::deque<int> d_fun;
    int             d_lastLength;
    bool            d_error;

    MyTestInStream& getArrayHelper(int num, int type) {
        if (!d_fun.size() || d_lastLength < num) {
            d_error = true;
            return *this;                                             // RETURN
        }

        if (d_fun.front() == k_ARRAY + type) {
            d_fun.pop_front();
            return *this;                                             // RETURN
        }

        if (static_cast<int>(d_fun.size()) < num) {
            d_error = true;
            return *this;                                             // RETURN
        }

        for (int i = 0; i < num; ++i) {
            if (d_fun.front() != type) {
                d_error = true;
                return *this;                                         // RETURN
            }
            d_fun.pop_front();
        }

        return *this;
    }

  public:
    enum {
      k_CHAR     =     10,
      k_INT      =     20,
      k_FLOAT    =     30,
      k_STRING   =     40,
      k_UNSIGNED =    100,
      k_SIGNED   =    200,
      k_ARRAY    =   1000,
      k_VERSION  =  10000,
      k_LENGTH   = 100000
    };

    // CREATORS
    MyTestInStream()
      : d_fun()
      , d_lastLength(-1)
      , d_error(false)
    {
    }

    ~MyTestInStream() { }

    void putType(int value) {  d_fun.push_back(value);  }
    template <typename T>
    void putType(const typename bsl::vector<T>& v)
    {
        for (typename bsl::vector<T>::const_iterator i = v.begin();
             i != v.end();
             ++i) {
            d_fun.push_back(*i);
        }
    }
    void putVersion(int version) {  d_fun.push_back(k_VERSION + version);  }
    void putLength(int length) {  d_fun.push_back(k_LENGTH + length);  }

    MyTestInStream& getLength(int& length) {
        if (!d_fun.size() || d_fun.front() < k_LENGTH) d_error = true;
        else {
            length = d_fun.front() - k_LENGTH;
            d_fun.pop_front();
            d_lastLength = length;
        }
        return *this;
    }

    MyTestInStream& getVersion(int& version) {
        if (!d_fun.size() || d_fun.front() < k_VERSION ||
            d_fun.front() >= k_LENGTH) {
            d_error = true;
        }
        else {
            version = d_fun.front() - k_VERSION;
            d_fun.pop_front();
        }
        return *this;
    }

    typedef bsls::Types::Uint64 Uint64;

    MyTestInStream& getInt64(Int64& /* value */) {
        if (!d_fun.size() || d_fun.front() != k_INT + 8) d_error = true;
        else d_fun.pop_front();
        return *this;
    }

    MyTestInStream& getUint64(Uint64& /* value */) {
        if (!d_fun.size() || d_fun.front() != k_UNSIGNED + k_INT + 8)
            d_error = true;
        else d_fun.pop_front();
        return *this;
    }

    MyTestInStream& getInt32(int& /* value */) {
        if (!d_fun.size() || d_fun.front() != k_INT + 4) d_error = true;
        else d_fun.pop_front();
        return *this;
    }

    MyTestInStream& getUint32(unsigned int& /* value */) {
        if (!d_fun.size() || d_fun.front() != k_UNSIGNED + k_INT + 4)
            d_error = true;
        else d_fun.pop_front();
        return *this;
    }

    MyTestInStream& getInt16(short& /* value */) {
        if (!d_fun.size() || d_fun.front() != k_INT + 2) d_error = true;
        else d_fun.pop_front();
        return *this;
    }

    MyTestInStream& getUint16(unsigned short& /* value */) {
        if (!d_fun.size() || d_fun.front() != k_UNSIGNED + k_INT + 2)
            d_error = true;
        else d_fun.pop_front();
        return *this;
    }

    MyTestInStream& getInt8(char& /* value */) {
        if (!d_fun.size() || d_fun.front() != k_INT + 1) d_error = true;
        else d_fun.pop_front();
        return *this;
    }

    MyTestInStream& getInt8(signed char& /* value */) {
        if (!d_fun.size() || d_fun.front() != k_SIGNED + k_INT + 1)
            d_error = true;
        else d_fun.pop_front();
        return *this;
    }

    MyTestInStream& getUint8(unsigned char& /* value */) {
        if (!d_fun.size() || d_fun.front() != k_UNSIGNED + k_INT + 1)
            d_error = true;
        else d_fun.pop_front();
        return *this;
    }

    MyTestInStream& getFloat64(double& /* value */) {
        if (!d_fun.size() || d_fun.front() != k_FLOAT + 8) d_error = true;
        else d_fun.pop_front();
        return *this;
    }

    MyTestInStream& getFloat32(float& /* value */) {
        if (!d_fun.size() || d_fun.front() != k_FLOAT + 4) d_error = true;
        else d_fun.pop_front();
        return *this;
    }

    MyTestInStream& getString(bsl::string& /* value */) {
        if (!d_fun.size() || d_fun.front() != k_STRING) d_error = true;
        else d_fun.pop_front();
        return *this;
    }

    MyTestInStream& getArrayInt64(Int64 * /* value */, int num) {
        return getArrayHelper(num, k_INT + 8);
    }

    MyTestInStream& getArrayUint64(Uint64 * /* value */, int num) {
        return getArrayHelper(num, k_UNSIGNED + k_INT + 8);
    }

    MyTestInStream& getArrayInt32(int * /* value */, int num) {
        return getArrayHelper(num, k_INT + 4);
    }

    MyTestInStream& getArrayUint32(unsigned int * /* value */, int num) {
        return getArrayHelper(num, k_UNSIGNED + k_INT + 4);
    }

    MyTestInStream& getArrayInt16(short * /* value */, int num) {
        return getArrayHelper(num, k_INT + 2);
    }

    MyTestInStream& getArrayUint16(unsigned short * /* value */, int num) {
        return getArrayHelper(num, k_UNSIGNED + k_INT + 2);
    }

    MyTestInStream& getArrayInt8(char * /* value */, int num) {
        return getArrayHelper(num, k_INT + 1);
    }

    MyTestInStream& getArrayInt8(signed char * /* value */, int num) {
        return getArrayHelper(num, k_SIGNED + k_INT + 1);
    }

    MyTestInStream& getArrayUint8(unsigned char * /* value */, int num) {
        return getArrayHelper(num, k_UNSIGNED + k_INT + 1);
    }

    MyTestInStream& getArrayFloat64(double * /* value */, int num) {
        return getArrayHelper(num, k_FLOAT + 8);
    }

    MyTestInStream& getArrayFloat32(float * /* value */, int num) {
        return getArrayHelper(num, k_FLOAT + 4);
    }

    // MANIPULATORS
    void invalidate() {}
    void removeAll() {}
    void reserveCapacity(int /* newCapacity */) {}
    void clear() { d_fun.clear(); d_lastLength = -1; d_error = false; }

    // ACCESSORS
    operator const void *() const { return this; }
    const char *data() const { return 0; }
    bool error() const { return d_error || false == d_fun.empty(); }
    int length() const { return 0; }
};

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
STREAM& bdexStreamIn(STREAM&                stream,
                     MySpecializedTestEnum& variable,
                     int                 /* version */)
{
    using bslx::InStreamFunctions::bdexStreamIn;

    float x = 0;
    bdexStreamIn(stream, x, 1);
    variable = static_cast<MySpecializedTestEnum>(static_cast<int>(x));
    return stream;
}

namespace ThirdParty {

enum MyThirdPartyTestEnum {
    e_I = -1,
    e_J = 0,
    e_K = 1
};

template <class STREAM>
STREAM& bdexStreamIn(STREAM&               stream,
                     MyThirdPartyTestEnum& variable,
                     int                /* version */)
{
    using bslx::InStreamFunctions::bdexStreamIn;

    char x = 0;
    bdexStreamIn(stream, x, 1);
    variable = static_cast<MyThirdPartyTestEnum>(x);
    return stream;
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

    // MANIPULATORS
    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
};

int MyTestClass::maxSupportedBdexVersion(int versionSelector) {
    if (versionSelector >= 20131201) return 2;                        // RETURN
    return 1;
}

template <class STREAM>
STREAM& MyTestClass::bdexStreamIn(STREAM& stream, int version)
{
    if (2 == version) {
      int x = 0;
      double y = 0;
      InStreamFunctions::bdexStreamIn(stream, x, 1);
      InStreamFunctions::bdexStreamIn(stream, y, 1);
    }
    else {
      int x = 0;
      float y = 0;
      InStreamFunctions::bdexStreamIn(stream, x, 1);
      InStreamFunctions::bdexStreamIn(stream, y, 1);
    }
    return stream;
}

// ============================================================================
//                                  MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 the usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

// Finally, use the above 'enum', point class, and input stream to illustrate
// 'bslx::InStreamFunctions' functionality.  This test code does not attempt to
// do anything more useful than reading values from a stream whose buffer was
// written "by hand" and confirming that the expected values were read
// correctly from the known byte pattern in the buffer.
//..
    using bslx::InStreamFunctions::bdexStreamIn;

    {
        const int  EXP       = 0x0A0B0C0D;
        const char buffer[4] = { 0xA, 0xB, 0xC, 0xD };  // 'int' (no version)
        int        i         = 0;

        MyInStream in1(buffer, 4);  // use the one buffer
        bdexStreamIn(in1, i, 1);
        ASSERT(in1);  ASSERT(EXP == i);

        i = 0;                      // reset 'i'
        MyInStream in2(buffer, 4);  // re-use 'buffer (no version)
        bdexStreamIn(in2, i, 0);
        ASSERT(in2);  ASSERT(EXP == i);
    }

    {
        const MyPoint EXP(0, -1, BLUE);
        const char buffer1[5] = { 0, 0, -1, -1, 2 };     // 'MyPoint' (no ver)
        const char buffer2[6] = { 1, 0, 0, -1, -1, 2 };  // version, 'MyPoint'
        MyPoint p1, p2;  // two default points

        MyInStream in1(buffer1, 5);  // 'buffer1' has no version byte
        bdexStreamIn(in1, p1, 1);
        ASSERT(in1);  ASSERT(EXP == p1);

        MyInStream in2(buffer2, 6);  // 'buffer2' *has* a version
        int version;
        in2.getVersion(version);
        ASSERT(1 == version);
        bdexStreamIn(in2, p2, version);
        ASSERT(in2);  ASSERT(EXP == p2);
    }
//..
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'bdexStreamIn(stream, value)'
        //   Ensure the 'bdexStreamIn' methods forward to the correct stream
        //   methods and unexternalize the version where required.
        //
        // Concerns:
        //: 1 The correct methods on 'stream' are invoked.
        //:
        //: 2 Testing covers all 'stream' methods.
        //:
        //: 3 Non-directly supported vectors are unexternalized correctly.
        //:
        //: 4 The version is forwarded correctly.
        //:
        //: 5 The version is unexternalized only where appropriate.
        //:
        //: 6 Vectors correctly unexternalize the version.
        //:
        //: 7 Vectors over 16M elements are allocated in 2 passes.
        //
        // Plan:
        //: 1 Create a test stream object that will track invoked methods.
        //:
        //: 2 Create a test object which unexternalizes differently for
        //:   different versions.
        //:
        //: 3 Unexternalize a set of objects which cover all directly supported
        //:   BDEX types and verify correct method forwarding.  (C-1..2)
        //:
        //: 4 Unexternalize vectors of a test object and verify correct method
        //:   forwarding.  (C-3)
        //:
        //: 5 Unexternalize a test object which unexternalizes differently for
        //:   different versions and vectors of this type with different
        //:   supplied versions; verify correct method forwarding.  (C-4)
        //:
        //: 6 Verify the version is correctly unexternalized in all tests.
        //:   (C-5)
        //:
        //: 7 Verify that very long (>16M bytes) vectors are allocated in 2
        //:   passes, handling missing data before or after the cutoff
        //:   correctly. (C-7)
        //
        // Testing:
        //   bdexStreamIn(STREAM& stream, TYPE& value)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'bdexStreamIn(stream, value)'" << endl
                          << "=====================================" << endl;

        if (verbose) cout << "\nTesting 'bdexStreamIn'" << endl;
        {
            using namespace InStreamFunctions;

            MyTestInStream  stream;
            MyTestInStream *pRV;

            // bool
            if (veryVerbose) cout << "\t... bool" << endl;

            bool                                       XA;
            bsl::vector<bool>                          XVA;
            bsl::vector<bsl::vector<bool> >            XVVA;

            stream.clear();
            stream.putType(MyTestInStream::k_INT + 1);
            pRV = &bdexStreamIn(stream, XA);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(0);
            pRV = &bdexStreamIn(stream, XVA);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(0 == XVA.size());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_INT + 1);
            stream.putType(MyTestInStream::k_INT + 1);
            stream.putType(MyTestInStream::k_INT + 1);
            pRV = &bdexStreamIn(stream, XVA);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(3 == XVA.size());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(2);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_INT + 1);
            stream.putType(MyTestInStream::k_INT + 1);
            stream.putType(MyTestInStream::k_INT + 1);
            stream.putLength(5);
            // short array, expect failure
            stream.putType(MyTestInStream::k_INT + 1);
            pRV = &bdexStreamIn(stream, XVVA);
            ASSERT(pRV == &stream);
            ASSERT(true == stream.error());
            ASSERT(2 == XVVA.size());
            ASSERT(3 == XVVA[0].size());
            ASSERT(5 == XVVA[1].size());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(2);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_INT + 1);
            stream.putType(MyTestInStream::k_INT + 1);
            stream.putType(MyTestInStream::k_INT + 1);
            stream.putLength(5);
            stream.putType(MyTestInStream::k_INT + 1);
            stream.putType(MyTestInStream::k_INT + 1);
            stream.putType(MyTestInStream::k_INT + 1);
            stream.putType(MyTestInStream::k_INT + 1);
            stream.putType(MyTestInStream::k_INT + 1);
            pRV = &bdexStreamIn(stream, XVVA);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(2 == XVVA.size());
            ASSERT(3 == XVVA[0].size());
            ASSERT(5 == XVVA[1].size());

            // char
            if (veryVerbose) cout << "\t... char" << endl;

            char                                       XB;
            bsl::vector<char>                          XVB;
            bsl::vector<bsl::vector<char> >            XVVB;

            stream.clear();
            stream.putType(MyTestInStream::k_INT + 1);
            pRV = &bdexStreamIn(stream, XB);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(0);
            pRV = &bdexStreamIn(stream, XVB);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(0 == XVB.size());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_INT
                           + 1);
            pRV = &bdexStreamIn(stream, XVB);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(3 == XVB.size());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(2);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_INT
                           + 1);
            stream.putLength(5);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_INT
                           + 1);
            pRV = &bdexStreamIn(stream, XVVB);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(2 == XVVB.size());
            ASSERT(3 == XVVB[0].size());
            ASSERT(5 == XVVB[1].size());

            // signed char
            if (veryVerbose) cout << "\t... signed char" << endl;

            signed char                                XC;
            bsl::vector<signed char>                   XVC;
            bsl::vector<bsl::vector<signed char> >     XVVC;

            stream.clear();
            stream.putType(MyTestInStream::k_SIGNED
                           + MyTestInStream::k_INT
                           + 1);
            pRV = &bdexStreamIn(stream, XC);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(0);
            pRV = &bdexStreamIn(stream, XVC);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(0 == XVC.size());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_SIGNED
                           + MyTestInStream::k_INT
                           + 1);
            pRV = &bdexStreamIn(stream, XVC);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(3 == XVC.size());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(2);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_SIGNED
                           + MyTestInStream::k_INT
                           + 1);
            stream.putLength(5);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_SIGNED
                           + MyTestInStream::k_INT
                           + 1);
            pRV = &bdexStreamIn(stream, XVVC);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(2 == XVVC.size());
            ASSERT(3 == XVVC[0].size());
            ASSERT(5 == XVVC[1].size());

            // unsigned char
            if (veryVerbose) cout << "\t... unsigned char" << endl;

            unsigned char                              XD;
            bsl::vector<unsigned char>                 XVD;
            bsl::vector<bsl::vector<unsigned char> >   XVVD;

            stream.clear();
            stream.putType(MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 1);
            pRV = &bdexStreamIn(stream, XD);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(0);
            pRV = &bdexStreamIn(stream, XVD);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(0 == XVD.size());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 1);
            pRV = &bdexStreamIn(stream, XVD);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(3 == XVD.size());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(2);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 1);
            stream.putLength(5);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 1);
            pRV = &bdexStreamIn(stream, XVVD);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(2 == XVVD.size());
            ASSERT(3 == XVVD[0].size());
            ASSERT(5 == XVVD[1].size());

            // short
            if (veryVerbose) cout << "\t... short" << endl;

            short                                      XE;
            bsl::vector<short>                         XVE;
            bsl::vector<bsl::vector<short> >           XVVE;

            stream.clear();
            stream.putType(MyTestInStream::k_INT + 2);
            pRV = &bdexStreamIn(stream, XE);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(0);
            pRV = &bdexStreamIn(stream, XVE);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(0 == XVE.size());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_INT
                           + 2);
            pRV = &bdexStreamIn(stream, XVE);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(3 == XVE.size());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(2);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_INT
                           + 2);
            stream.putLength(5);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_INT
                           + 2);
            pRV = &bdexStreamIn(stream, XVVE);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(2 == XVVE.size());
            ASSERT(3 == XVVE[0].size());
            ASSERT(5 == XVVE[1].size());

            // unsigned short
            if (veryVerbose) cout << "\t... unsigned short" << endl;

            unsigned short                             XF;
            bsl::vector<unsigned short>                XVF;
            bsl::vector<bsl::vector<unsigned short> >  XVVF;

            stream.clear();
            stream.putType(MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 2);
            pRV = &bdexStreamIn(stream, XF);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(0 == XVF.size());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(0);
            pRV = &bdexStreamIn(stream, XVF);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(0 == XVF.size());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 2);
            pRV = &bdexStreamIn(stream, XVF);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(3 == XVF.size());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(2);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 2);
            stream.putLength(5);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 2);
            pRV = &bdexStreamIn(stream, XVVF);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(2 == XVVF.size());
            ASSERT(3 == XVVF[0].size());
            ASSERT(5 == XVVF[1].size());

            // int
            if (veryVerbose) cout << "\t... int" << endl;

            int                                        XG;
            bsl::vector<int>                           XVG;
            bsl::vector<bsl::vector<int> >             XVVG;

            stream.clear();
            stream.putType(MyTestInStream::k_INT + 4);
            pRV = &bdexStreamIn(stream, XG);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(0);
            pRV = &bdexStreamIn(stream, XVG);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(0 == XVG.size());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_INT
                           + 4);
            pRV = &bdexStreamIn(stream, XVG);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(3 == XVG.size());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(2);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_INT
                           + 4);
            stream.putLength(5);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_INT
                           + 4);
            pRV = &bdexStreamIn(stream, XVVG);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(2 == XVVG.size());
            ASSERT(3 == XVVG[0].size());
            ASSERT(5 == XVVG[1].size());

            // unsigned int
            if (veryVerbose) cout << "\t... unsigned int" << endl;

            unsigned int                               XH;
            bsl::vector<unsigned int>                  XVH;
            bsl::vector<bsl::vector<unsigned int> >    XVVH;

            stream.clear();
            stream.putType(MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 4);
            pRV = &bdexStreamIn(stream, XH);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(0);
            pRV = &bdexStreamIn(stream, XVH);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(0 == XVH.size());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 4);
            pRV = &bdexStreamIn(stream, XVH);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(3 == XVH.size());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(2);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 4);
            stream.putLength(5);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 4);
            pRV = &bdexStreamIn(stream, XVVH);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(2 == XVVH.size());
            ASSERT(3 == XVVH[0].size());
            ASSERT(5 == XVVH[1].size());

            // long
            if (veryVerbose) cout << "\t... long" << endl;

            long                                       XI;
            bsl::vector<long>                          XVI;
            bsl::vector<bsl::vector<long> >            XVVI;

            stream.clear();
            stream.putType(MyTestInStream::k_INT + 4);
            pRV = &bdexStreamIn(stream, XI);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(0);
            pRV = &bdexStreamIn(stream, XVI);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(0 == XVI.size());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_INT + 4);
            pRV = &bdexStreamIn(stream, XVI);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(3 == XVI.size());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(2);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putLength(5);
            // short stream, expect failure
            stream.putType(MyTestInStream::k_INT + 4);
            pRV = &bdexStreamIn(stream, XVVI);
            ASSERT(pRV == &stream);
            ASSERT(true == stream.error());
            ASSERT(2 == XVVI.size());
            ASSERT(3 == XVVI[0].size());
            ASSERT(5 == XVVI[1].size());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(2);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putLength(5);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_INT + 4);
            pRV = &bdexStreamIn(stream, XVVI);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(2 == XVVI.size());
            ASSERT(3 == XVVI[0].size());
            ASSERT(5 == XVVI[1].size());

            // unsigned long
            if (veryVerbose) cout << "\t... unsigned long" << endl;

            unsigned long                              XJ;
            bsl::vector<unsigned long>                 XVJ;
            bsl::vector<bsl::vector<unsigned long> >   XVVJ;

            stream.clear();
            stream.putType(MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 4);
            pRV = &bdexStreamIn(stream, XJ);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(0);
            pRV = &bdexStreamIn(stream, XVJ);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(0 == XVJ.size());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 4);
            stream.putType(MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 4);
            stream.putType(MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 4);
            pRV = &bdexStreamIn(stream, XVJ);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(3 == XVJ.size());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(2);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 4);
            stream.putType(MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 4);
            stream.putType(MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 4);
            stream.putLength(5);
            // short stream, expect failure
            stream.putType(MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 4);
            pRV = &bdexStreamIn(stream, XVVJ);
            ASSERT(pRV == &stream);
            ASSERT(true == stream.error());
            ASSERT(2 == XVVJ.size());
            ASSERT(3 == XVVJ[0].size());
            ASSERT(5 == XVVJ[1].size());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(2);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 4);
            stream.putType(MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 4);
            stream.putType(MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 4);
            stream.putLength(5);
            stream.putType(MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 4);
            stream.putType(MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 4);
            stream.putType(MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 4);
            stream.putType(MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 4);
            stream.putType(MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 4);
            pRV = &bdexStreamIn(stream, XVVJ);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(2 == XVVJ.size());
            ASSERT(3 == XVVJ[0].size());
            ASSERT(5 == XVVJ[1].size());

            // Int64
            if (veryVerbose) cout << "\t... Int64" << endl;

            Int64                                      XK;
            bsl::vector<Int64>                         XVK;
            bsl::vector<bsl::vector<Int64> >           XVVK;

            stream.clear();
            stream.putType(MyTestInStream::k_INT + 8);
            pRV = &bdexStreamIn(stream, XK);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(0);
            pRV = &bdexStreamIn(stream, XVK);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(0 == XVK.size());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_INT
                           + 8);
            pRV = &bdexStreamIn(stream, XVK);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(3 == XVK.size());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(2);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_INT
                           + 8);
            stream.putLength(5);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_INT
                           + 8);
            pRV = &bdexStreamIn(stream, XVVK);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(2 == XVVK.size());
            ASSERT(3 == XVVK[0].size());
            ASSERT(5 == XVVK[1].size());

            // Uint64
            if (veryVerbose) cout << "\t... Uint64" << endl;

            Uint64                                     XL;
            bsl::vector<Uint64>                        XVL;
            bsl::vector<bsl::vector<Uint64> >          XVVL;

            stream.clear();
            stream.putType(MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 8);
            pRV = &bdexStreamIn(stream, XL);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(0);
            pRV = &bdexStreamIn(stream, XVL);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(0 == XVL.size());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 8);
            pRV = &bdexStreamIn(stream, XVL);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(3 == XVL.size());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(2);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 8);
            stream.putLength(5);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 8);
            pRV = &bdexStreamIn(stream, XVVL);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(2 == XVVL.size());
            ASSERT(3 == XVVL[0].size());
            ASSERT(5 == XVVL[1].size());

            // float
            if (veryVerbose) cout << "\t... float" << endl;

            float                                      XM;
            bsl::vector<float>                         XVM;
            bsl::vector<bsl::vector<float> >           XVVM;

            stream.clear();
            stream.putType(MyTestInStream::k_FLOAT + 4);
            pRV = &bdexStreamIn(stream, XM);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(0);
            pRV = &bdexStreamIn(stream, XVM);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(0 == XVM.size());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_FLOAT
                           + 4);
            pRV = &bdexStreamIn(stream, XVM);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(3 == XVM.size());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(2);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_FLOAT
                           + 4);
            stream.putLength(5);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_FLOAT
                           + 4);
            pRV = &bdexStreamIn(stream, XVVM);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(2 == XVVM.size());
            ASSERT(3 == XVVM[0].size());
            ASSERT(5 == XVVM[1].size());

            // double
            if (veryVerbose) cout << "\t... double" << endl;

            double                                     XN;
            bsl::vector<double>                        XVN;
            bsl::vector<bsl::vector<double> >          XVVN;

            stream.clear();
            stream.putType(MyTestInStream::k_FLOAT + 8);
            pRV = &bdexStreamIn(stream, XN);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(0);
            pRV = &bdexStreamIn(stream, XVN);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(0 == XVN.size());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_FLOAT
                           + 8);
            pRV = &bdexStreamIn(stream, XVN);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(3 == XVN.size());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(2);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_FLOAT
                           + 8);
            stream.putLength(5);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_FLOAT
                           + 8);
            pRV = &bdexStreamIn(stream, XVVN);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(2 == XVVN.size());
            ASSERT(3 == XVVN[0].size());
            ASSERT(5 == XVVN[1].size());

            // string
            if (veryVerbose) cout << "\t... string" << endl;

            bsl::string                                XO;
            bsl::vector<bsl::string>                   XVO;
            bsl::vector<bsl::vector<bsl::string> >     XVVO;

            stream.clear();
            stream.putType(MyTestInStream::k_STRING);
            pRV = &bdexStreamIn(stream, XO);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(0);
            pRV = &bdexStreamIn(stream, XVO);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(0 == XVO.size());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_STRING);
            stream.putType(MyTestInStream::k_STRING);
            stream.putType(MyTestInStream::k_STRING);
            pRV = &bdexStreamIn(stream, XVO);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(3 == XVO.size());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(2);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_STRING);
            stream.putType(MyTestInStream::k_STRING);
            stream.putType(MyTestInStream::k_STRING);
            stream.putLength(1);
            stream.putType(MyTestInStream::k_STRING);
            pRV = &bdexStreamIn(stream, XVVO);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(2 == XVVO.size());
            ASSERT(3 == XVVO[0].size());
            ASSERT(1 == XVVO[1].size());

            // MyTestEnum
            if (veryVerbose) cout << "\t... MyTestEnum" << endl;

            MyTestEnum                                 XP;
            bsl::vector<MyTestEnum>                    XVP;
            bsl::vector<bsl::vector<MyTestEnum> >      XVVP;

            stream.clear();
            stream.putType(MyTestInStream::k_INT + 4);
            pRV = &bdexStreamIn(stream, XP);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(0);
            pRV = &bdexStreamIn(stream, XVP);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(0 == XVP.size());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_INT + 4);
            pRV = &bdexStreamIn(stream, XVP);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(3 == XVP.size());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(2);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putLength(5);
            // short stream, expect failure
            stream.putType(MyTestInStream::k_INT + 4);
            pRV = &bdexStreamIn(stream, XVVP);
            ASSERT(pRV == &stream);
            ASSERT(true == stream.error());
            ASSERT(2 == XVVP.size());
            ASSERT(3 == XVVP[0].size());
            ASSERT(5 == XVVP[1].size());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(2);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putLength(5);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_INT + 4);
            pRV = &bdexStreamIn(stream, XVVP);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(2 == XVVP.size());
            ASSERT(3 == XVVP[0].size());
            ASSERT(5 == XVVP[1].size());

            // MySpecializedTestEnum
            if (veryVerbose) cout << "\t... MySpecializedTestEnum" << endl;

            MySpecializedTestEnum                            XPS;
            bsl::vector<MySpecializedTestEnum>               XVPS;
            bsl::vector<bsl::vector<MySpecializedTestEnum> > XVVPS;

            stream.clear();
            stream.putType(MyTestInStream::k_FLOAT + 4);
            pRV = &bdexStreamIn(stream, XPS);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(0);
            pRV = &bdexStreamIn(stream, XVPS);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(0 == XVPS.size());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_FLOAT + 4);
            stream.putType(MyTestInStream::k_FLOAT + 4);
            stream.putType(MyTestInStream::k_FLOAT + 4);
            pRV = &bdexStreamIn(stream, XVPS);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(3 == XVPS.size());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(2);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_FLOAT + 4);
            stream.putType(MyTestInStream::k_FLOAT + 4);
            stream.putType(MyTestInStream::k_FLOAT + 4);
            stream.putLength(5);
            stream.putType(MyTestInStream::k_FLOAT + 4);
            stream.putType(MyTestInStream::k_FLOAT + 4);
            stream.putType(MyTestInStream::k_FLOAT + 4);
            stream.putType(MyTestInStream::k_FLOAT + 4);
            stream.putType(MyTestInStream::k_FLOAT + 4);
            pRV = &bdexStreamIn(stream, XVVPS);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(2 == XVVPS.size());
            ASSERT(3 == XVVPS[0].size());
            ASSERT(5 == XVVPS[1].size());

            // MyThirdPartyTestEnum
            if (veryVerbose) cout << "\t... MyThirdPartyTestEnum" << endl;

            ThirdParty::MyThirdPartyTestEnum                            XPTP;
            bsl::vector<ThirdParty::MyThirdPartyTestEnum>               XVPTP;
            bsl::vector<bsl::vector<ThirdParty::MyThirdPartyTestEnum> > XVVPTP;

            stream.clear();
            stream.putType(MyTestInStream::k_INT + 1);
            pRV = &bdexStreamIn(stream, XPTP);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(0);
            pRV = &bdexStreamIn(stream, XVPTP);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(0 == XVPTP.size());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_INT + 1);
            stream.putType(MyTestInStream::k_INT + 1);
            stream.putType(MyTestInStream::k_INT + 1);
            pRV = &bdexStreamIn(stream, XVPTP);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(3 == XVPTP.size());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(2);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_INT + 1);
            stream.putType(MyTestInStream::k_INT + 1);
            stream.putType(MyTestInStream::k_INT + 1);
            stream.putLength(5);
            stream.putType(MyTestInStream::k_INT + 1);
            stream.putType(MyTestInStream::k_INT + 1);
            stream.putType(MyTestInStream::k_INT + 1);
            stream.putType(MyTestInStream::k_INT + 1);
            stream.putType(MyTestInStream::k_INT + 1);
            pRV = &bdexStreamIn(stream, XVVPTP);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(2 == XVVPTP.size());
            ASSERT(3 == XVVPTP[0].size());
            ASSERT(5 == XVVPTP[1].size());

            // MyTestClass version 1
            if (veryVerbose) cout << "\t... MyTestClass version 1" << endl;

            MyTestClass                                XQ;
            bsl::vector<MyTestClass>                   XVQ;
            bsl::vector<bsl::vector<MyTestClass> >     XVVQ;

            stream.clear();
            stream.putVersion(1);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_FLOAT + 4);
            pRV = &bdexStreamIn(stream, XQ);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(0);
            pRV = &bdexStreamIn(stream, XVQ);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(0 == XVQ.size());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_FLOAT + 4);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_FLOAT + 4);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_FLOAT + 4);
            pRV = &bdexStreamIn(stream, XVQ);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(3 == XVQ.size());

            stream.clear();
            stream.putVersion(1);
            stream.putLength(2);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_FLOAT + 4);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_FLOAT + 4);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_FLOAT + 4);
            stream.putLength(1);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_FLOAT + 4);
            pRV = &bdexStreamIn(stream, XVVQ);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(2 == XVVQ.size());
            ASSERT(3 == XVVQ[0].size());
            ASSERT(1 == XVVQ[1].size());

            // MyTestClass version 2
            if (veryVerbose) cout << "\t... MyTestClass version 2" << endl;

            stream.clear();
            stream.putVersion(2);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_FLOAT + 8);
            pRV = &bdexStreamIn(stream, XQ);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putVersion(2);
            stream.putLength(0);
            pRV = &bdexStreamIn(stream, XVQ);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(0 == XVQ.size());

            stream.clear();
            stream.putVersion(2);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_FLOAT + 8);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_FLOAT + 8);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_FLOAT + 8);
            pRV = &bdexStreamIn(stream, XVQ);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(3 == XVQ.size());

            stream.clear();
            stream.putVersion(2);
            stream.putLength(2);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_FLOAT + 8);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_FLOAT + 8);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_FLOAT + 8);
            stream.putLength(1);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_FLOAT + 8);
            pRV = &bdexStreamIn(stream, XVVQ);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
            ASSERT(2 == XVVQ.size());
            ASSERT(3 == XVVQ[0].size());
            ASSERT(1 == XVVQ[1].size());

            // Overlong (near/over 16M bytes) vector handling (C-7)
            if (veryVerbose) {
                cout << "\t... Overlong (near/over 16M bytes) vector "
                        "handling (C-7)"
                     << endl;
            }
            {
                const int k_VALUE_ALLOCATION_THRESHOLD =
                    16 * 1024 * 1024 / sizeof(short);
                const int k_VECTOR_ALLOCATION_THRESHOLD =
                    16 * 1024 * 1024 / sizeof(bsl::vector<short>);


#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic ignored "-Wlarger-than="
#endif
                static char
                    buffer[k_VECTOR_ALLOCATION_THRESHOLD *
                               sizeof(bsl::vector<bsl::vector<short> >) +
                           2 * k_VALUE_ALLOCATION_THRESHOLD *
                               sizeof(bsl::vector<short>)];

                // vector: empty
                if (veryVerbose)
                    cout << "\t\t ... vector: empty" << endl;
                {
                    bslma::SequentialAllocator alloc(buffer, sizeof(buffer));
                    bsl::vector<short>         XVA(&alloc);

                    XVA.resize(10);
                    ASSERTV(XVA.size(), 10 == XVA.size());
                    stream.clear();
                    stream.putVersion(0);
                    stream.putLength(0);
                    // expect success after resize to 0
                    pRV = &bdexStreamIn(stream, XVA);
                    ASSERT(pRV == &stream);
                    ASSERT(false == stream.error());
                    ASSERTV(XVA.size(), 0 == XVA.size());
                }

                // vector<vector>: empty
                if (veryVerbose) {
                    cout << "\t\t ... vector<vector>: empty" << endl;
                }
                {
                    bslma::SequentialAllocator alloc(buffer, sizeof(buffer));
                    bsl::vector<bsl::vector<short> > XVVA(&alloc);

                    XVVA.resize(10);
                    ASSERTV(XVVA.size(), 10 == XVVA.size());
                    stream.clear();
                    stream.putVersion(0);
                    stream.putLength(0);
                    // expect success after resize to 0
                    pRV = &bdexStreamIn(stream, XVVA);
                    ASSERT(pRV == &stream);
                    ASSERT(false == stream.error());
                    ASSERTV(XVVA.size(), 0 == XVVA.size());
                }

                // vector: size below threshold followed by missing data
                if (veryVerbose) {
                    cout << "\t\t ... vector: size below threshold followed "
                            "by missing data"
                         << endl;
                }
                {
                    bslma::SequentialAllocator alloc(buffer, sizeof(buffer));
                    bsl::vector<short>         XVA(&alloc);

                    XVA.resize(10);
                    ASSERT(10 == XVA.size());
                    stream.clear();
                    stream.putVersion(0);
                    stream.putLength(2);
                    // expect failure after resize - missing data
                    pRV = &bdexStreamIn(stream, XVA);
                    ASSERT(pRV == &stream);
                    ASSERT(true == stream.error());
                    ASSERTV(XVA.size(), 2, 2 == XVA.size());
                }

                // vector<vector>: size below threshold followed by missing
                // data
                if (veryVerbose) {
                    cout << "\t\t ... vector<vector>: size below threshold "
                            "followed by missing data"
                         << endl;
                }
                {
                    bslma::SequentialAllocator alloc(buffer, sizeof(buffer));
                    bsl::vector<bsl::vector<short> > XVVA(&alloc);

                    XVVA.resize(10);
                    ASSERT(10 == XVVA.size());
                    stream.clear();
                    stream.putVersion(0);
                    stream.putLength(2);
                    // expect failure after resize - missing data
                    pRV = &bdexStreamIn(stream, XVVA);
                    ASSERT(pRV == &stream);
                    ASSERT(true == stream.error());
                    ASSERTV(XVVA.size(), 2, 2 == XVVA.size());
                }

                // vector: size below threshold followed by correct data
                if (veryVerbose) {
                    cout << "\t\t ... vector: size below threshold followed "
                            "by correct data"
                         << endl;
                }

                {
                    bslma::SequentialAllocator alloc(buffer, sizeof(buffer));
                    bsl::vector<bool>          XVA(&alloc);

                    XVA.resize(10);
                    ASSERT(10 == XVA.size());
                    stream.clear();
                    stream.putVersion(0);
                    stream.putLength(2);
                    for (int i = 0; i < 2; ++i) {
                        stream.putType(MyTestInStream::k_INT + 1);
                    }
                    pRV = &bdexStreamIn(stream, XVA);
                    ASSERT(pRV == &stream);
                    ASSERT(false == stream.error());
                    ASSERTV(XVA.size(), 2 == XVA.size());
                }

                {
                    bslma::SequentialAllocator alloc(buffer, sizeof(buffer));
                    bsl::vector<short>         XVA(&alloc);

                    XVA.resize(10);
                    ASSERT(10 == XVA.size());
                    stream.clear();
                    stream.putVersion(0);
                    stream.putLength(2);
                    for (int i = 0; i < 2; ++i) {
                        stream.putType(MyTestInStream::k_INT + 2);
                    }
                    pRV = &bdexStreamIn(stream, XVA);
                    ASSERT(pRV == &stream);
                    ASSERT(false == stream.error());
                    ASSERTV(XVA.size(), 2 == XVA.size());
                }

                // vector<vector>: size below threshold followed by correct
                // data
                if (veryVerbose) {
                    cout << "\t\t ... vector<vector>: size below threshold "
                            "followed by correct data"
                         << endl;
                }
                {
                    bslma::SequentialAllocator alloc(buffer, sizeof(buffer));
                    bsl::vector<bsl::vector<short> > XVVA(&alloc);

                    XVVA.resize(10);
                    ASSERT(10 == XVVA.size());
                    stream.clear();
                    stream.putVersion(0);
                    stream.putLength(2);
                    for (int i = 0; i < 2; ++i) {
                        stream.putLength(1);
                        stream.putType(MyTestInStream::k_INT + 2);
                    }
                    pRV = &bdexStreamIn(stream, XVVA);
                    ASSERT(pRV == &stream);
                    ASSERT(false == stream.error());
                    ASSERTV(XVVA.size(), 2 == XVVA.size());
                    ASSERTV(XVVA[0].size(), 1 == XVVA[0].size());
                    ASSERTV(XVVA[1].size(), 1 == XVVA[1].size());
                }

                // vector: size AT threshold, followed by missing data
                if (veryVerbose) {
                    cout << "\t\t ... vector: size AT threshold, followed by "
                            "missing data"
                         << endl;
                }
                {
                    bslma::SequentialAllocator alloc(buffer, sizeof(buffer));
                    bsl::vector<short>         XVA(&alloc);

                    XVA.resize(10);
                    ASSERT(10 == XVA.size());
                    stream.clear();
                    stream.putVersion(0);
                    stream.putLength(k_VALUE_ALLOCATION_THRESHOLD);
                    // expect failure after resize - missing data
                    for (int i = 0; i < 2; ++i) {
                        stream.putType(MyTestInStream::k_INT + 2);
                    }
                    pRV = &bdexStreamIn(stream, XVA);
                    ASSERT(pRV == &stream);
                    ASSERT(true == stream.error());
                    ASSERTV(XVA.size(),
                            k_VALUE_ALLOCATION_THRESHOLD,
                            k_VALUE_ALLOCATION_THRESHOLD == XVA.size());
                }

                // vector<vector>: size AT threshold, followed by missing data
                if (veryVerbose) {
                    cout << "\t\t ... vector<vector>: size AT threshold, "
                            "followed by missing data"
                         << endl;
                }
                {
                    bslma::SequentialAllocator alloc(buffer, sizeof(buffer));
                    bsl::vector<bsl::vector<short> > XVVA(&alloc);

                    XVVA.resize(10);
                    ASSERT(10 == XVVA.size());
                    stream.clear();
                    stream.putVersion(0);
                    stream.putLength(k_VECTOR_ALLOCATION_THRESHOLD);
                    // expect failure after resize - missing data
                    for (int i = 0; i < 2; ++i) {
                        stream.putLength(0);
                    }
                    pRV = &bdexStreamIn(stream, XVVA);
                    ASSERT(pRV == &stream);
                    ASSERT(true == stream.error());
                    ASSERTV(XVVA.size(),
                            k_VECTOR_ALLOCATION_THRESHOLD,
                            k_VECTOR_ALLOCATION_THRESHOLD == XVVA.size());
                }

                // vector: size AT threshold, followed by correct data
                if (veryVerbose) {
                    cout << "\t\t ... vector: size AT threshold, followed by "
                            "correct data"
                         << endl;
                }
                {
                    bslma::SequentialAllocator alloc(buffer, sizeof(buffer));
                    bsl::vector<short>         XVA(&alloc);

                    XVA.resize(10);
                    ASSERT(10 == XVA.size());
                    stream.clear();
                    stream.putVersion(0);
                    stream.putLength(k_VALUE_ALLOCATION_THRESHOLD);
                    for (int i = 0; i < k_VALUE_ALLOCATION_THRESHOLD; ++i) {
                        stream.putType(MyTestInStream::k_INT + 2);
                    }
                    pRV = &bdexStreamIn(stream, XVA);
                    ASSERT(pRV == &stream);
                    ASSERT(false == stream.error());
                    ASSERTV(XVA.size(),
                            k_VALUE_ALLOCATION_THRESHOLD,
                            k_VALUE_ALLOCATION_THRESHOLD == XVA.size());
                }

                // vector<vector>: size AT threshold, followed by correct data
                if (veryVerbose) {
                    cout << "\t\t ... vector<vector>: size AT threshold, "
                            "followed by correct data"
                         << endl;
                }
                {
                    bslma::SequentialAllocator alloc(buffer, sizeof(buffer));
                    bsl::vector<bsl::vector<short> > XVVA(&alloc);

                    XVVA.resize(10);
                    ASSERT(10 == XVVA.size());
                    stream.clear();
                    stream.putVersion(0);
                    stream.putLength(k_VECTOR_ALLOCATION_THRESHOLD);
                    for (int i = 0; i < k_VECTOR_ALLOCATION_THRESHOLD; ++i) {
                        stream.putLength(1);
                        stream.putType(MyTestInStream::k_INT + 2);
                    }
                    pRV = &bdexStreamIn(stream, XVVA);
                    ASSERT(pRV == &stream);
                    ASSERT(false == stream.error());
                    ASSERTV(XVVA.size(),
                            k_VECTOR_ALLOCATION_THRESHOLD,
                            k_VECTOR_ALLOCATION_THRESHOLD == XVVA.size());
                    for (int i = 0; i < k_VECTOR_ALLOCATION_THRESHOLD; ++i) {
                        ASSERTV(i, XVVA[i].size(), 1 == XVVA[i].size());
                    }
                }

                // vector: size above threshold, followed by missing data AFTER
                // threshold
                if (veryVerbose) {
                    cout << "\t\t ... vector: size above threshold, followed "
                            "by missing data AFTER threshold"
                         << endl;
                }
                {
                    bslma::SequentialAllocator alloc(buffer, sizeof(buffer));
                    bsl::vector<short>         XVA(&alloc);

                    XVA.resize(10);
                    ASSERT(10 == XVA.size());
                    stream.clear();
                    stream.putVersion(0);
                    stream.putLength(k_VALUE_ALLOCATION_THRESHOLD + 1);
                    // expect failure after 2nd resize - missing data
                    for (int i = 0; i < k_VALUE_ALLOCATION_THRESHOLD; ++i) {
                        stream.putType(MyTestInStream::k_INT + 2);
                    }
                    pRV = &bdexStreamIn(stream, XVA);
                    ASSERT(pRV == &stream);
                    ASSERT(true == stream.error());
                    ASSERTV(XVA.size(),
                            k_VALUE_ALLOCATION_THRESHOLD + 1,
                            k_VALUE_ALLOCATION_THRESHOLD + 1 == XVA.size());
                }

                // vector<vector>: size above threshold, followed by missing
                // data AFTER threshold
                if (veryVerbose) {
                    cout << "\t\t ... vector<vector>: size above threshold, "
                            "followed by missing data AFTER threshold"
                         << endl;
                }
                {
                    bslma::SequentialAllocator alloc(buffer, sizeof(buffer));
                    bsl::vector<bsl::vector<short> > XVVA(&alloc);

                    XVVA.resize(10);
                    ASSERT(10 == XVVA.size());
                    stream.clear();
                    stream.putVersion(0);
                    stream.putLength(k_VECTOR_ALLOCATION_THRESHOLD + 1);
                    // expect failure after 2nd resize - missing data
                    for (int i = 0; i < k_VECTOR_ALLOCATION_THRESHOLD; ++i) {
                        stream.putLength(0);
                    }
                    pRV = &bdexStreamIn(stream, XVVA);
                    ASSERT(pRV == &stream);
                    ASSERT(true == stream.error());
                    ASSERTV(XVVA.size(),
                            k_VECTOR_ALLOCATION_THRESHOLD + 1,
                            k_VECTOR_ALLOCATION_THRESHOLD + 1 == XVVA.size());
                }

                // vector: size above threshold, followed by correct data
                if (veryVerbose) {
                    cout << "\t\t ... vector: size above threshold, followed "
                            "by correct data"
                         << endl;
                }
                {
                    bslma::SequentialAllocator alloc(buffer, sizeof(buffer));
                    bsl::vector<short>         XVA(&alloc);

                    XVA.resize(10);
                    ASSERT(10 == XVA.size());
                    stream.clear();
                    stream.putVersion(0);
                    stream.putLength(k_VALUE_ALLOCATION_THRESHOLD + 1);
                    for (int i = 0; i < k_VALUE_ALLOCATION_THRESHOLD + 1;
                         ++i) {
                        stream.putType(MyTestInStream::k_INT + 2);
                    }
                    pRV = &bdexStreamIn(stream, XVA);
                    ASSERT(pRV == &stream);
                    ASSERT(false == stream.error());
                    ASSERTV(XVA.size(),
                            k_VALUE_ALLOCATION_THRESHOLD + 1,
                            k_VALUE_ALLOCATION_THRESHOLD + 1 == XVA.size());
                }

                // vector<vector>: size above threshold, followed by correct
                // data
                if (veryVerbose) {
                    cout << "\t\t ... vector<vector>: size above threshold, "
                            "followed by correct data"
                         << endl;
                }
                {
                    bslma::SequentialAllocator alloc(buffer, sizeof(buffer));
                    bsl::vector<bsl::vector<short> > XVVA(&alloc);

                    XVVA.resize(10);
                    ASSERT(10 == XVVA.size());
                    stream.clear();
                    stream.putVersion(0);
                    stream.putLength(k_VECTOR_ALLOCATION_THRESHOLD + 1);
                    for (int i = 0; i < k_VECTOR_ALLOCATION_THRESHOLD + 1;
                         ++i) {
                        stream.putLength(0);
                    }
                    pRV = &bdexStreamIn(stream, XVVA);
                    ASSERT(pRV == &stream);
                    ASSERT(false == stream.error());
                    ASSERTV(XVVA.size(),
                            k_VECTOR_ALLOCATION_THRESHOLD + 1,
                            k_VECTOR_ALLOCATION_THRESHOLD + 1 == XVVA.size());
                    ASSERTV(XVVA[0].size(), 0 == XVVA[0].size());
                    ASSERTV(XVVA[k_VECTOR_ALLOCATION_THRESHOLD].size(),
                            0 == XVVA[k_VECTOR_ALLOCATION_THRESHOLD].size());
                }
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'bdexStreamIn(stream, value, version)'
        //   Ensure the 'bdexStreamIn' methods forward to the correct stream
        //   methods.
        //
        // Concerns:
        //: 1 The correct methods on 'stream' are invoked.
        //:
        //: 2 Testing covers all 'stream' methods.
        //:
        //: 3 Non-directly supported vectors are unexternalized correctly.
        //:
        //: 4 The version is forwarded correctly.
        //
        // Plan:
        //: 1 Create a test stream object that will track invoked methods.
        //:
        //: 2 Create a test object which unexternalizes differently for
        //:   different versions.
        //:
        //: 3 Unexternalize a set of objects which cover all directly supported
        //:   BDEX types and verify correct method forwarding.  (C-1..2)
        //:
        //: 4 Unexternalize vectors of a test object and verify correct method
        //:   forwarding.  (C-3)
        //:
        //: 5 Unexternalize a test object which unexternalizes differently for
        //:   different versions and vectors of this type with different
        //:   supplied versions; verify correct method forwarding.  (C-4)
        //
        // Testing:
        //   bdexStreamIn(STREAM& stream, TYPE& value, int version)
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "TESTING 'bdexStreamIn(stream, value, version)'" << endl
                 << "==============================================" << endl;

        if (verbose) cout << "\nTesting 'bdexStreamIn'" << endl;
        {
            using namespace InStreamFunctions;

            MyTestInStream  stream;
            MyTestInStream *pRV;

            // bool

            bool                                       XA;
            bsl::vector<bool>                          XVA;
            bsl::vector<bsl::vector<bool> >            XVVA;

            stream.clear();
            stream.putType(MyTestInStream::k_INT + 1);
            pRV = &bdexStreamIn(stream, XA, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(0);
            pRV = &bdexStreamIn(stream, XVA, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(3);
            stream.putType(MyTestInStream::k_INT + 1);
            stream.putType(MyTestInStream::k_INT + 1);
            stream.putType(MyTestInStream::k_INT + 1);
            pRV = &bdexStreamIn(stream, XVA, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(2);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_INT + 1);
            stream.putType(MyTestInStream::k_INT + 1);
            stream.putType(MyTestInStream::k_INT + 1);
            stream.putLength(1);
            stream.putType(MyTestInStream::k_INT + 1);
            pRV = &bdexStreamIn(stream, XVVA, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            // char

            char                                       XB;
            bsl::vector<char>                          XVB;
            bsl::vector<bsl::vector<char> >            XVVB;

            stream.clear();
            stream.putType(MyTestInStream::k_INT + 1);
            pRV = &bdexStreamIn(stream, XB, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(0);
            pRV = &bdexStreamIn(stream, XVB, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(3);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_INT
                           + 1);
            pRV = &bdexStreamIn(stream, XVB, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(2);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_INT
                           + 1);
            stream.putLength(5);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_INT
                           + 1);
            pRV = &bdexStreamIn(stream, XVVB, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            // signed char

            signed char                                XC;
            bsl::vector<signed char>                   XVC;
            bsl::vector<bsl::vector<signed char> >     XVVC;

            stream.clear();
            stream.putType(MyTestInStream::k_SIGNED
                           + MyTestInStream::k_INT
                           + 1);
            pRV = &bdexStreamIn(stream, XC, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(0);
            pRV = &bdexStreamIn(stream, XVC, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(3);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_SIGNED
                           + MyTestInStream::k_INT
                           + 1);
            pRV = &bdexStreamIn(stream, XVC, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(2);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_SIGNED
                           + MyTestInStream::k_INT
                           + 1);
            stream.putLength(5);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_SIGNED
                           + MyTestInStream::k_INT
                           + 1);
            pRV = &bdexStreamIn(stream, XVVC, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            // unsigned char

            unsigned char                              XD;
            bsl::vector<unsigned char>                 XVD;
            bsl::vector<bsl::vector<unsigned char> >   XVVD;

            stream.clear();
            stream.putType(MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 1);
            pRV = &bdexStreamIn(stream, XD, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(0);
            pRV = &bdexStreamIn(stream, XVD, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(3);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 1);
            pRV = &bdexStreamIn(stream, XVD, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(2);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 1);
            stream.putLength(5);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 1);
            pRV = &bdexStreamIn(stream, XVVD, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            // short

            short                                      XE;
            bsl::vector<short>                         XVE;
            bsl::vector<bsl::vector<short> >           XVVE;

            stream.clear();
            stream.putType(MyTestInStream::k_INT + 2);
            pRV = &bdexStreamIn(stream, XE, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(0);
            pRV = &bdexStreamIn(stream, XVE, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(3);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_INT
                           + 2);
            pRV = &bdexStreamIn(stream, XVE, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(2);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_INT
                           + 2);
            stream.putLength(5);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_INT
                           + 2);
            pRV = &bdexStreamIn(stream, XVVE, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            // unsigned short

            unsigned short                             XF;
            bsl::vector<unsigned short>                XVF;
            bsl::vector<bsl::vector<unsigned short> >  XVVF;

            stream.clear();
            stream.putType(MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 2);
            pRV = &bdexStreamIn(stream, XF, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(0);
            pRV = &bdexStreamIn(stream, XVF, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(3);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 2);
            pRV = &bdexStreamIn(stream, XVF, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(2);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 2);
            stream.putLength(5);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 2);
            pRV = &bdexStreamIn(stream, XVVF, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            // int

            int                                        XG;
            bsl::vector<int>                           XVG;
            bsl::vector<bsl::vector<int> >             XVVG;

            stream.clear();
            stream.putType(MyTestInStream::k_INT + 4);
            pRV = &bdexStreamIn(stream, XG, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(0);
            pRV = &bdexStreamIn(stream, XVG, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(3);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_INT
                           + 4);
            pRV = &bdexStreamIn(stream, XVG, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(2);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_INT
                           + 4);
            stream.putLength(5);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_INT
                           + 4);
            pRV = &bdexStreamIn(stream, XVVG, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            // unsigned int

            unsigned int                               XH;
            bsl::vector<unsigned int>                  XVH;
            bsl::vector<bsl::vector<unsigned int> >    XVVH;

            stream.clear();
            stream.putType(MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 4);
            pRV = &bdexStreamIn(stream, XH, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(0);
            pRV = &bdexStreamIn(stream, XVH, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(3);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 4);
            pRV = &bdexStreamIn(stream, XVH, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(2);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 4);
            stream.putLength(5);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 4);
            pRV = &bdexStreamIn(stream, XVVH, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            // long

            long                                       XI;
            bsl::vector<long>                          XVI;
            bsl::vector<bsl::vector<long> >            XVVI;

            stream.clear();
            stream.putType(MyTestInStream::k_INT + 4);
            pRV = &bdexStreamIn(stream, XI, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(0);
            pRV = &bdexStreamIn(stream, XVI, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(3);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_INT + 4);
            pRV = &bdexStreamIn(stream, XVI, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(2);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putLength(1);
            stream.putType(MyTestInStream::k_INT + 4);
            pRV = &bdexStreamIn(stream, XVVI, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            // unsigned long

            unsigned long                              XJ;
            bsl::vector<unsigned long>                 XVJ;
            bsl::vector<bsl::vector<unsigned long> >   XVVJ;

            stream.clear();
            stream.putType(MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 4);
            pRV = &bdexStreamIn(stream, XJ, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(0);
            pRV = &bdexStreamIn(stream, XVJ, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(3);
            stream.putType(MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 4);
            stream.putType(MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 4);
            stream.putType(MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 4);
            pRV = &bdexStreamIn(stream, XVJ, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(2);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 4);
            stream.putType(MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 4);
            stream.putType(MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 4);
            stream.putLength(1);
            stream.putType(MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 4);
            pRV = &bdexStreamIn(stream, XVVJ, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            // Int64

            Int64                                      XK;
            bsl::vector<Int64>                         XVK;
            bsl::vector<bsl::vector<Int64> >           XVVK;

            stream.clear();
            stream.putType(MyTestInStream::k_INT + 8);
            pRV = &bdexStreamIn(stream, XK, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(0);
            pRV = &bdexStreamIn(stream, XVK, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(3);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_INT
                           + 8);
            pRV = &bdexStreamIn(stream, XVK, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(2);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_INT
                           + 8);
            stream.putLength(5);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_INT
                           + 8);
            pRV = &bdexStreamIn(stream, XVVK, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            // Uint64

            Uint64                                     XL;
            bsl::vector<Uint64>                        XVL;
            bsl::vector<bsl::vector<Uint64> >          XVVL;

            stream.clear();
            stream.putType(MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 8);
            pRV = &bdexStreamIn(stream, XL, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(0);
            pRV = &bdexStreamIn(stream, XVL, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(3);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 8);
            pRV = &bdexStreamIn(stream, XVL, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(2);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 8);
            stream.putLength(5);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_UNSIGNED
                           + MyTestInStream::k_INT
                           + 8);
            pRV = &bdexStreamIn(stream, XVVL, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            // float

            float                                      XM;
            bsl::vector<float>                         XVM;
            bsl::vector<bsl::vector<float> >           XVVM;

            stream.clear();
            stream.putType(MyTestInStream::k_FLOAT + 4);
            pRV = &bdexStreamIn(stream, XM, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(0);
            pRV = &bdexStreamIn(stream, XVM, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(3);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_FLOAT
                           + 4);
            pRV = &bdexStreamIn(stream, XVM, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(2);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_FLOAT
                           + 4);
            stream.putLength(5);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_FLOAT
                           + 4);
            pRV = &bdexStreamIn(stream, XVVM, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            // double

            double                                     XN;
            bsl::vector<double>                        XVN;
            bsl::vector<bsl::vector<double> >          XVVN;

            stream.clear();
            stream.putType(MyTestInStream::k_FLOAT + 8);
            pRV = &bdexStreamIn(stream, XN, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(0);
            pRV = &bdexStreamIn(stream, XVN, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(3);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_FLOAT
                           + 8);
            pRV = &bdexStreamIn(stream, XVN, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(2);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_FLOAT
                           + 8);
            stream.putLength(5);
            stream.putType(MyTestInStream::k_ARRAY
                           + MyTestInStream::k_FLOAT
                           + 8);
            pRV = &bdexStreamIn(stream, XVVN, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            // string

            bsl::string                                XO;
            bsl::vector<bsl::string>                   XVO;
            bsl::vector<bsl::vector<bsl::string> >     XVVO;

            stream.clear();
            stream.putType(MyTestInStream::k_STRING);
            pRV = &bdexStreamIn(stream, XO, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(0);
            pRV = &bdexStreamIn(stream, XVO, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(3);
            stream.putType(MyTestInStream::k_STRING);
            stream.putType(MyTestInStream::k_STRING);
            stream.putType(MyTestInStream::k_STRING);
            pRV = &bdexStreamIn(stream, XVO, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(2);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_STRING);
            stream.putType(MyTestInStream::k_STRING);
            stream.putType(MyTestInStream::k_STRING);
            stream.putLength(1);
            stream.putType(MyTestInStream::k_STRING);
            pRV = &bdexStreamIn(stream, XVVO, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            // MyTestEnum

            MyTestEnum                                 XP;
            bsl::vector<MyTestEnum>                    XVP;
            bsl::vector<bsl::vector<MyTestEnum> >      XVVP;

            stream.clear();
            stream.putType(MyTestInStream::k_INT + 4);
            pRV = &bdexStreamIn(stream, XP, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(0);
            pRV = &bdexStreamIn(stream, XVP, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(3);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_INT + 4);
            pRV = &bdexStreamIn(stream, XVP, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(2);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putLength(1);
            stream.putType(MyTestInStream::k_INT + 4);
            pRV = &bdexStreamIn(stream, XVVP, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            // MySpecializedTestEnum

            MySpecializedTestEnum                            XPS;
            bsl::vector<MySpecializedTestEnum>               XVPS;
            bsl::vector<bsl::vector<MySpecializedTestEnum> > XVVPS;

            stream.clear();
            stream.putType(MyTestInStream::k_FLOAT + 4);
            pRV = &bdexStreamIn(stream, XPS, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(0);
            pRV = &bdexStreamIn(stream, XVPS, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(3);
            stream.putType(MyTestInStream::k_FLOAT + 4);
            stream.putType(MyTestInStream::k_FLOAT + 4);
            stream.putType(MyTestInStream::k_FLOAT + 4);
            pRV = &bdexStreamIn(stream, XVPS, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(2);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_FLOAT + 4);
            stream.putType(MyTestInStream::k_FLOAT + 4);
            stream.putType(MyTestInStream::k_FLOAT + 4);
            stream.putLength(1);
            stream.putType(MyTestInStream::k_FLOAT + 4);
            pRV = &bdexStreamIn(stream, XVVPS, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            // MyThirdPartyTestEnum

            ThirdParty::MyThirdPartyTestEnum                            XPTP;
            bsl::vector<ThirdParty::MyThirdPartyTestEnum>               XVPTP;
            bsl::vector<bsl::vector<ThirdParty::MyThirdPartyTestEnum> > XVVPTP;

            stream.clear();
            stream.putType(MyTestInStream::k_INT + 1);
            pRV = &bdexStreamIn(stream, XPTP, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(0);
            pRV = &bdexStreamIn(stream, XVPTP, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(3);
            stream.putType(MyTestInStream::k_INT + 1);
            stream.putType(MyTestInStream::k_INT + 1);
            stream.putType(MyTestInStream::k_INT + 1);
            pRV = &bdexStreamIn(stream, XVPTP, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(2);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_INT + 1);
            stream.putType(MyTestInStream::k_INT + 1);
            stream.putType(MyTestInStream::k_INT + 1);
            stream.putLength(1);
            stream.putType(MyTestInStream::k_INT + 1);
            pRV = &bdexStreamIn(stream, XVVPTP, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            // MyTestClass version 1

            MyTestClass                                XQ;
            bsl::vector<MyTestClass>                   XVQ;
            bsl::vector<bsl::vector<MyTestClass> >     XVVQ;

            stream.clear();
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_FLOAT + 4);
            pRV = &bdexStreamIn(stream, XQ, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(0);
            pRV = &bdexStreamIn(stream, XVQ, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(3);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_FLOAT + 4);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_FLOAT + 4);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_FLOAT + 4);
            pRV = &bdexStreamIn(stream, XVQ, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(2);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_FLOAT + 4);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_FLOAT + 4);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_FLOAT + 4);
            stream.putLength(1);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_FLOAT + 4);
            pRV = &bdexStreamIn(stream, XVVQ, 1);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            // MyTestClass version 2

            stream.clear();
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_FLOAT + 8);
            pRV = &bdexStreamIn(stream, XQ, 2);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(0);
            pRV = &bdexStreamIn(stream, XVQ, 2);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(3);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_FLOAT + 8);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_FLOAT + 8);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_FLOAT + 8);
            pRV = &bdexStreamIn(stream, XVQ, 2);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());

            stream.clear();
            stream.putLength(2);
            stream.putLength(3);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_FLOAT + 8);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_FLOAT + 8);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_FLOAT + 8);
            stream.putLength(1);
            stream.putType(MyTestInStream::k_INT + 4);
            stream.putType(MyTestInStream::k_FLOAT + 8);
            pRV = &bdexStreamIn(stream, XVVQ, 2);
            ASSERT(pRV == &stream);
            ASSERT(false == stream.error());
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
